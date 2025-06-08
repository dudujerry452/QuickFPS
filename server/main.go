// server.go
package main

import (
	"bufio"
	"log"
	"net"
	"os"
	"sync"
	"time"

	"google.golang.org/protobuf/proto"

	pb "qfserver/pb/proto"
	"qfserver/wldlib"
)

// --- 常量定义 ---
const (
	// 设置服务器的逻辑帧率 (Ticks per second)
	serverFPS = 20
	// 命令队列的缓冲区大小
	commandQueueSize = 128
	// 客户端超时时间
	clientTimeout = 1 * time.Minute
)

// ServerState 结构体，增加了 lastActivity 字段
type ServerState struct {
	mu sync.Mutex
	// clients map 存储客户端信息
	clients map[string]struct {
		playerID     uint32
		addr         net.Addr
		lastActivity time.Time // 新增：记录客户端最后活跃时间
	}
	world *wldlib.World
}

// --- 全局变量 ---
var state = ServerState{
	clients: make(map[string]struct {
		playerID     uint32
		addr         net.Addr
		lastActivity time.Time
	}),
}

// Command 是一个函数类型，用于我们的命令队列
type Command func()

func main() {
	// --- 初始化世界 ---
	log.Println("正在创建游戏世界...")
	state.world = wldlib.NewWorld()
	if state.world == nil {
		log.Fatalf("创建游戏世界失败！请检查C++库和依赖项。")
	}
	defer state.world.Destroy()
	log.Println("游戏世界创建成功。")

	// --- 读取网络参数 ---
	log.Println("正在从ser_config读取IP地址和端口...")
	ip := "0.0.0.0"
	port := "1077"
	ipFile, err := os.Open("ser_config")
	if err != nil {
		log.Println("找不到ser_config文件. 使用默认配置。")
	} else {
		defer ipFile.Close()
		scanner := bufio.NewScanner(ipFile)
		if scanner.Scan() {
			ip = scanner.Text()
		}
		if scanner.Scan() {
			port = scanner.Text()
		}
	}
	log.Printf("IP地址：%s，端口：%s", ip, port)

	// --- 初始化网络 ---
	addr, err := net.ResolveUDPAddr("udp", ip+":"+port)
	if err != nil {
		log.Fatalf("无法解析地址: %v", err)
	}
	listener, err := net.ListenUDP("udp", addr)
	if err != nil {
		log.Fatalf("无法监听UDP端口: %v", err)
	}
	defer listener.Close()

	// 创建命令队列 Channel
	commandChan := make(chan Command, commandQueueSize)

	// 启动核心游戏循环 Goroutine
	go gameLoop(listener, commandChan)

	log.Printf("Go游戏服务器已启动，正在监听 %s", addr.String())

	// --- 网络监听循环 (只负责接收和分发任务) ---
	buffer := make([]byte, 2048)
	for {
		n, clientAddr, err := listener.ReadFromUDP(buffer)
		if err != nil {
			log.Printf("读取UDP数据时出错: %v", err)
			continue
		}

		dataCopy := make([]byte, n)
		copy(dataCopy, buffer[:n])
		go handlePacket(listener, clientAddr, dataCopy, commandChan)
	}
}

// gameLoop 是服务器的心跳，在独立的goroutine中运行
func gameLoop(conn *net.UDPConn, commandChan <-chan Command) {
	tickInterval := time.Second / serverFPS
	ticker := time.NewTicker(tickInterval)
	defer ticker.Stop()

	for range ticker.C {
		// 1. 处理所有待处理的命令 (例如: 添加玩家)
	processCmds:
		for {
			select {
			case cmd := <-commandChan:
				cmd()
			default:
				break processCmds // channel空了，跳出循环
			}
		}

		// 2. 【新增】检查并移除超时的客户端
		checkInactiveClients()

		// 3. 更新C++游戏世界

		state.world.Update()
		state.world.Update()
		state.world.Update()

		// 4. 导出世界状态
		worldStateBytes := state.world.ExportWorldData()
		if worldStateBytes == nil || len(worldStateBytes) == 0 {
			continue // 如果没有状态，就进入下一帧
		}

		// 5. 将世界状态发送给所有客户端
		broadcastState(conn, worldStateBytes)
	}
}

// checkInactiveClients 遍历所有客户端，移除那些长时间未活动的
func checkInactiveClients() {
	state.mu.Lock()
	
	// 因为不能在遍历map时删除元素，所以先收集需要删除的客户端信息
	var clientsToRemove []struct{
		key string
		id uint32
	}

	now := time.Now()
	for key, client := range state.clients {
		if now.Sub(client.lastActivity) > clientTimeout {
			clientsToRemove = append(clientsToRemove, struct{key string; id uint32}{key: key, id: client.playerID})
		}
	}
	
	// 执行删除操作
	for _, c := range clientsToRemove {
		delete(state.clients, c.key)
		log.Printf("客户端 %s (PlayerID: %d) 因超时被移除。", c.key, c.id)
		// 在解锁后从C++世界中移除，以保持world操作在主循环中
	}
	
	state.mu.Unlock()

	// 在主循环的同步上下文中安全地从C++世界移除实体
	for _, c := range clientsToRemove {
		state.world.RemovePlayer(c.id)
	}
}


// broadcastState 将序列化好的世界状态广播给所有已连接的客户端
func broadcastState(conn *net.UDPConn, stateBytes []byte) {
	state.mu.Lock()
	if len(state.clients) == 0 {
		state.mu.Unlock()
		return
	}

	// 创建地址副本以在解锁后使用，避免长时间持有锁
	clientsCopy := make([]net.Addr, 0, len(state.clients))
	for _, c := range state.clients {
		clientsCopy = append(clientsCopy, c.addr)
	}
	state.mu.Unlock()

	for _, clientAddr := range clientsCopy {
		udpAddr, ok := clientAddr.(*net.UDPAddr)
		if !ok {
			continue // 理论上不应该发生，因为我们存入的就是 *net.UDPAddr
		}
		_, err := conn.WriteToUDP(stateBytes, udpAddr)
		if err != nil {
			// 如果需要，可以取消此日志的注释，但在高并发下可能会产生大量日志
			// log.Printf("向 %s 广播世界状态失败: %v", clientAddr.String(), err)
		}
	}
}

// handlePacket 解析数据包并分发到相应的处理器
func handlePacket(conn *net.UDPConn, addr *net.UDPAddr, data []byte, commandChan chan<- Command) {
	// 【新增】在处理任何消息前，先尝试更新已知客户端的活跃时间
	clientKey := addr.String()
	state.mu.Lock()
	if clientData, ok := state.clients[clientKey]; ok {
		clientData.lastActivity = time.Now()
		state.clients[clientKey] = clientData
	}
	state.mu.Unlock()

	var gameMsg pb.GameMessage
	if err := proto.Unmarshal(data, &gameMsg); err != nil {
		log.Printf("反序列化来自 %s 的消息失败: %v", addr.String(), err)
		return
	}

	switch payload := gameMsg.Payload.(type) {
	case *pb.GameMessage_Chello:
		handleClientHello(conn, addr, payload.Chello, commandChan)

	case *pb.GameMessage_InputState:
		handlePlayerInput(payload.InputState)

	default:
		log.Printf("从 %s 收到未知或非预期的消息类型", addr.String())
	}
}

// handlePlayerInput 将 Protobuf 输入转换为 CGO 结构并推送到世界
func handlePlayerInput(input *pb.PInputState) {
	goInput := wldlib.InputState{
		PlayerID:       input.PlayerId,
		SpacePressed:   uint8(input.SpacePressed),
		MouseDeltaX:    input.MouseDelta.X,
		MouseDeltaY:    input.MouseDelta.Y,
		SequenceNumber: input.SequenceNumber,
	}
	for i := 0; i < 4 && i < len(input.WasdPressed); i++ {
		goInput.WasdPressed[i] = uint8(input.WasdPressed[i])
	}
	state.world.PushInput(&goInput)
}

// handleClientHello 创建一个"添加玩家"命令并提交到队列
func handleClientHello(conn *net.UDPConn, addr *net.UDPAddr, chello *pb.ClientHello, commandChan chan<- Command) {
	log.Printf("收到来自 %s 的 ClientHello，客户端代码: %d", addr.String(), chello.Code)

	addPlayerCmd := func() {
		state.mu.Lock()
		defer state.mu.Unlock()

		if _, exists := state.clients[addr.String()]; exists {
			log.Printf("地址 %s 的玩家已存在，忽略新的 ClientHello 请求。", addr.String())
			return
		}

		newPlayerID := uint32(state.world.AddPlayer())
		if newPlayerID == 0 {
			log.Printf("在C++世界中为 %s 创建玩家失败。", addr.String())
			return
		}

		// 在Go服务器中记录新客户端，并设置初始活跃时间
		state.clients[addr.String()] = struct {
			playerID     uint32
			addr         net.Addr
			lastActivity time.Time
		}{
			playerID:     newPlayerID,
			addr:         addr,
			lastActivity: time.Now(),
		}

		state.world.SetPlayerPosition(newPlayerID, 0, 5, 0)
		log.Printf("命令已执行：为 %s 创建玩家实体，ID: %d", addr.String(), newPlayerID)

		// 立即回复 ServerHello 完成握手
		shello := &pb.ServerHello{
			Code:     chello.Code,
			Playerid: newPlayerID,
		}
		responseMsg := &pb.GameMessage{
			Payload: &pb.GameMessage_Shello{Shello: shello},
		}
		responseBytes, err := proto.Marshal(responseMsg)
		if err != nil {
			log.Printf("序列化 ServerHello 失败: %v", err)
			return
		}
		_, err = conn.WriteToUDP(responseBytes, addr)
		if err != nil {
			log.Printf("向 %s 发送 ServerHello 失败: %v", addr.String(), err)
		} else {
			log.Printf("已向 %s 发送 ServerHello，分配的玩家ID: %d", addr.String(), newPlayerID)
		}
	}

	commandChan <- addPlayerCmd
}
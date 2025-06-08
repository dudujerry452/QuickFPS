// server.go
package main

// import (
// 	"fmt"
// 	"net"
// 	"qfserver/wldlib"
// )

// // --- 全局变量 ---
// type ServerState struct {
// 	// clients map 现在存储 net.Addr 接口，更通用
// 	clients map[string]struct {
// 		playerID uint32
// 		addr     net.Addr
// 	}
// 	world *wldlib.World
// }

// var state = ServerState{
// 	clients: make(map[string]struct {
// 		playerID uint32
// 		addr     net.Addr
// 	}),
// }

// func main() {
// 	state.world = wldlib.NewWorld()
// 	defer state.world.Destroy()
// 	state.world.AddPlayer()
// 	fmt.Println(state.world.GetWorldNumber())
// }

import (
	"log"
	"net"
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
)

// --- ServerState 结构体保持不变 ---
type ServerState struct {
	mu sync.Mutex
	// clients map 现在存储 net.Addr 接口，更通用
	clients map[string]struct {
		playerID uint32
		addr     net.Addr
	}
	world *wldlib.World
}

// --- 全局变量 ---
var state = ServerState{
	clients: make(map[string]struct {
		playerID uint32
		addr     net.Addr
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

	// --- 初始化网络 ---
	addr, err := net.ResolveUDPAddr("udp", "0.0.0.0:1077")
	if err != nil {
		log.Fatalf("无法解析地址: %v", err)
	}
	listener, err := net.ListenUDP("udp", addr)
	if err != nil {
		log.Fatalf("无法监听UDP端口: %v", err)
	}
	defer listener.Close()

	// --- 【新增】创建命令队列 Channel ---
	commandChan := make(chan Command, commandQueueSize)

	// --- 【新增】启动核心游戏循环 Goroutine ---
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

		// 为避免数据竞争，拷贝数据并异步处理
		dataCopy := make([]byte, n)
		copy(dataCopy, buffer[:n])
		// 将命令队列的channel传递给数据包处理器
		go handlePacket(listener, clientAddr, dataCopy, commandChan)
	}
}

// // gameLoop 是服务器的心跳，在独立的goroutine中运行
// func gameLoop(conn *net.UDPConn, commandChan <-chan Command) {
// 	// 根据设定的FPS计算每帧的时间间隔
// 	tickInterval := time.Second / serverFPS
// 	ticker := time.NewTicker(tickInterval)
// 	defer ticker.Stop()

// 	for range ticker.C {
// 		// 1. 【同步】处理所有待处理的命令
// 	processCmds:
// 		for {
// 			select {
// 			case cmd := <-commandChan:
// 				cmd() // 执行命令 (例如: 添加玩家)
// 			default:
// 				break processCmds // channel空了，跳出循环
// 			}
// 		}

// 		// 2. 【同步】更新C++游戏世界
// 		state.world.Update()

// 		// 3. 【异步】导出世界状态
// 		worldStateBytes := state.world.ExportWorldData()
// 		if worldStateBytes == nil {
// 			continue // 如果没有状态，就进入下一帧
// 		}

// 		// 4. 【广播】将世界状态发送给所有客户端
// 		broadcastState(conn, worldStateBytes)
// 	}
// }

// server.go

// gameLoop 是服务器的心跳，在独立的goroutine中运行
func gameLoop(conn *net.UDPConn, commandChan <-chan Command) {
	// 根据设定的FPS计算每帧的时间间隔
	tickInterval := time.Second / serverFPS
	ticker := time.NewTicker(tickInterval)
	defer ticker.Stop()

	var frameCount uint64 = 0 // 添加一个帧计数器，方便追踪

	for range ticker.C {
		frameCount++
		//log.Printf("--- Game Loop Tick #%d ---", frameCount)

		// 1. 【同步】处理所有待处理的命令
		//log.Printf("[%d] 正在处理命令队列...", frameCount)
		processedCmds := 0
	processCmds:
		for {
			select {
			case cmd := <-commandChan:
				cmd() // 执行命令 (例如: 添加玩家)
				processedCmds++
			default:
				break processCmds // channel空了，跳出循环
			}
		}
		if processedCmds > 0 {
			//log.Printf("[%d] 处理了 %d 个命令", frameCount, processedCmds)
		}

		// 2. 【同步】更新C++游戏世界
		//log.Printf("[%d] 正在调用 state.world.Update()...", frameCount)
		// updateStartTime := time.Now()
		state.world.Update()
		state.world.Update()
		state.world.Update()
		// updateDuration := time.Since(updateStartTime)
		//log.Printf("[%d] state.world.Update() 完成, 耗时: %s", frameCount, updateDuration)
		// 如果 updateDuration 特别长 (比如超过 tickInterval)，说明这里可能存在问题。

		// 3. 【异步】导出世界状态
		//log.Printf("[%d] 正在调用 state.world.ExportWorldData()...", frameCount)
		worldStateBytes := state.world.ExportWorldData()

		// 【关键诊断】检查导出的数据
		if worldStateBytes == nil {
			continue // 直接进入下一次循环
		}

		if len(worldStateBytes) == 0 {
			//log.Printf("[%d] ExportWorldData 返回空切片(len 0), 本次 tick 跳过广播。", frameCount)
			continue
		}

		//log.Printf("[%d] ExportWorldData 成功, 获得 %d 字节的状态数据。准备广播...", frameCount, len(worldStateBytes))

		// 4. 【广播】将世界状态发送给所有客户端
		broadcastState(conn, worldStateBytes)
		//log.Printf("[%d] broadcastState 已调用。", frameCount)
	}
}

// broadcastState 函数中日志的修改
func broadcastState(conn *net.UDPConn, stateBytes []byte) {
	// var gameMsg1 pb.GameMessage
	// if err := proto.Unmarshal(stateBytes, &gameMsg1); err != nil {
	// 	log.Printf("广播时反序列化从CGO导出的世界状态失败: %v", err)
	// 	return
	// }

	// gameMsg := &pb.GameMessage{
	// }
	// msgBytes, err := proto.Marshal(gameMsg)
	// if err != nil {
	// 	log.Printf("广播时序列化世界状态失败: %v", err)
	// 	return
	// }

	state.mu.Lock()
	// 【关键诊断】检查广播时客户端列表是否为空
	if len(state.clients) == 0 {
		state.mu.Unlock()
		log.Println("广播时发现客户端列表为空，取消广播。")
		return
	}

	clientsCopy := make([]net.Addr, 0, len(state.clients))
	for _, c := range state.clients {
		// 这是你之前添加的日志，非常好
		// log.Printf("准备向 %s (key: %s) 广播世界状态", c.addr.String(), clientKey)
		clientsCopy = append(clientsCopy, c.addr)
	}
	state.mu.Unlock()

	// ... (后面的发送循环不变) ...
	for _, clientAddr := range clientsCopy {
		// 【修复】进行类型断言，将 net.Addr 转换为 *net.UDPAddr
		udpAddr, ok := clientAddr.(*net.UDPAddr)
		if !ok {
			//log.Printf("无法将 net.Addr %s 转换为 *net.UDPAddr，跳过广播", clientAddr.String())
			continue
		}
		_, err := conn.WriteToUDP(stateBytes, udpAddr) // 使用转换后的 udpAddr
		if err != nil {
			// log.Printf("向 %s 广播世界状态失败: %v", clientAddr.String(), err)
		}
	}
}

// broadcastState 将序列化好的世界状态广播给所有已连接的客户端
// func broadcastState(conn *net.UDPConn, stateBytes []byte) {
// 	var entityStateBatch pb.EntityStateBatch
// 	if err := proto.Unmarshal(stateBytes, &entityStateBatch); err != nil {
// 		log.Printf("广播时反序列化从CGO导出的世界状态失败: %v", err)
// 		return
// 	}

// 	gameMsg := &pb.GameMessage{
// 		Payload: &pb.GameMessage_EntityStateBatch{
// 			EntityStateBatch: &entityStateBatch,
// 		},
// 	}
// 	msgBytes, err := proto.Marshal(gameMsg)
// 	if err != nil {
// 		log.Printf("广播时序列化世界状态失败: %v", err)
// 		return
// 	}

// 	state.mu.Lock()
// 	clientsCopy := make([]net.Addr, 0, len(state.clients))
// 	for _, c := range state.clients {
// 		log.Printf("向 %s 广播世界状态", c.addr.String())
// 		clientsCopy = append(clientsCopy, c.addr)
// 	}
// 	state.mu.Unlock()

// 	for _, clientAddr := range clientsCopy {
// 		// 【修复】进行类型断言，将 net.Addr 转换为 *net.UDPAddr
// 		udpAddr, ok := clientAddr.(*net.UDPAddr)
// 		if !ok {
// 			log.Printf("无法将 net.Addr %s 转换为 *net.UDPAddr，跳过广播", clientAddr.String())
// 			continue
// 		}
// 		_, err := conn.WriteToUDP(msgBytes, udpAddr) // 使用转换后的 udpAddr
// 		if err != nil {
// 			log.Printf("向 %s 广播世界状态失败: %v", clientAddr.String(), err)
// 		}
// 	}
// }

// handlePacket 现在需要 commandChan 来提交需要同步执行的命令
func handlePacket(conn *net.UDPConn, addr *net.UDPAddr, data []byte, commandChan chan<- Command) {
	var gameMsg pb.GameMessage
	if err := proto.Unmarshal(data, &gameMsg); err != nil {
		log.Printf("反序列化来自 %s 的消息失败: %v", addr.String(), err)
		return
	}

	switch payload := gameMsg.Payload.(type) {
	case *pb.GameMessage_Chello:
		// 握手逻辑被简化，它现在只提交一个“添加玩家”的命令
		handleClientHello(conn, addr, payload.Chello, commandChan)

	case *pb.GameMessage_InputState:
		// 【新增】处理玩家输入，这是异步的，直接调用
		handlePlayerInput(payload.InputState)

	default:
		// 当玩家断开连接时，我们也需要一个机制来处理
		// 暂时先记录未知消息
		log.Printf("从 %s 收到未知或非预期的消息类型", addr.String())
	}
}

// handlePlayerInput 将 Protobuf 输入转换为 CGO 结构并推送到世界
func handlePlayerInput(input *pb.PInputState) {
	// log.Printf("收到来自玩家 %d 的输入: %+v", input.PlayerId, input)

	goInput := wldlib.InputState{
		PlayerID:       input.PlayerId,
		SpacePressed:   uint8(input.SpacePressed),
		MouseDeltaX:    input.MouseDelta.X,
		MouseDeltaY:    input.MouseDelta.Y,
		SequenceNumber: input.SequenceNumber,
	}

	// WASD数组转换
	for i := 0; i < 4 && i < len(input.WasdPressed); i++ {
		goInput.WasdPressed[i] = uint8(input.WasdPressed[i])
	}

	// 【修复】调用 wldlib 中新创建的封装方法
	state.world.PushInput(&goInput)
}

// handleClientHello 被重构，不再直接修改世界
func handleClientHello(conn *net.UDPConn, addr *net.UDPAddr, chello *pb.ClientHello, commandChan chan<- Command) {
	log.Printf("收到来自 %s 的 ClientHello，客户端代码: %d", addr.String(), chello.Code)

	// 【核心变更】创建一个“命令”函数闭包
	// 这个函数捕获了所有需要的信息（比如 addr）
	// 它将在游戏主循环的goroutine中被安全地执行
	addPlayerCmd := func() {
		state.mu.Lock()
		defer state.mu.Unlock()

		// 检查玩家是否已经存在 (例如，客户端重发了Hello)
		if _, exists := state.clients[addr.String()]; exists {
			log.Printf("地址 %s 的玩家已存在，忽略新的 ClientHello 请求。", addr.String())
			return
		}

		// 在C++世界中创建玩家，并获取ID
		newPlayerID := uint32(state.world.AddPlayer())
		if newPlayerID == 0 {
			log.Printf("在C++世界中为 %s 创建玩家失败。", addr.String())
			return
		}

		// 在Go服务器中记录这个新客户端
		state.clients[addr.String()] = struct {
			playerID uint32
			addr     net.Addr
		}{
			playerID: newPlayerID,
			addr:     addr,
		}

		// (可选) 设置初始位置
		state.world.SetPlayerPosition(newPlayerID, 0, 5, 0)

		log.Printf("命令已执行：为 %s 创建玩家实体，ID: %d", addr.String(), newPlayerID)

		// --- 注意：初始世界状态不再在此处发送 ---
		// 客户端将通过第一次常规的游戏循环广播接收到它的状态
		// 我们仍然需要立即发送 ServerHello 来完成握手

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

	// 将命令发送到命令队列，让游戏主循环去处理
	commandChan <- addPlayerCmd
}

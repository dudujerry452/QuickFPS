package main

import (
	"fmt"
	"net"
	// "qfserver/wldlib"
)

func main() {

	// use net to listen from incoming udp packets, on port 1077
	fmt.Println("Starting server...")
	server, err := net.ListenUDP("udp", &net.UDPAddr{
		Port: 1077,
		IP:   net.ParseIP("127.0.0.1"),
	})
	if err != nil {
		fmt.Printf("Error starting server: %v\n", err)
		return
	}
	defer server.Close()
	fmt.Println("Server started on port 1077.")
	for {
		buffer := make([]byte, 1024)
		n, addr, err := server.ReadFromUDP(buffer)
		if err != nil {
			fmt.Printf("Error reading from UDP: %v\n", err)
			continue
		}
		fmt.Printf("Received %d bytes from %s: %s\n", n, addr, string(buffer[:n]))

		// Echo the message back to the client
		_, err = server.WriteToUDP(buffer[:n], addr)
		if err != nil {
			fmt.Printf("Error writing to UDP: %v\n", err)
			continue
		}
		fmt.Printf("Echoed back %d bytes to %s\n", n, addr)
	}

}

/*
func main() {

	fmt.Println("Creating world...")
	world := wldlib.NewWorld()
	if world == nil {
		fmt.Println("Failed to create world")
		return
	}
	defer world.Destroy()
	fmt.Println("World created.")

	playerID := world.AddPlayer()
	fmt.Printf("Added player with ID: %d\n", playerID)

	if playerID != 0 {
		world.SetPlayerPosition(playerID, 1.0, 2.0, 3.0)
		fmt.Println("Set player position.")
	}

	fmt.Println("Running world update for a few steps...")
	for i := 0; i < 5; i++ {
		world.Update()
		fmt.Printf("World updated (step %d)\n", i+1)
		// 在实际应用中，Raylib 的 Draw 调用会在这里
		// time.Sleep(16 * time.Millisecond) // 模拟帧率
	}

	if playerID != 0 {
		world.RemovePlayer(playerID)
		fmt.Printf("Removed player with ID: %d\n", playerID)
	}

	fmt.Println("Test finished.")
}
*/

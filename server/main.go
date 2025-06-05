package main

import (
	"fmt"
	"qfserver/wldlib" // 替换为你的模块名/wldlib
	"runtime"
)

func main() {
	runtime.LockOSThread() // Raylib/OpenGL 通常需要这个

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

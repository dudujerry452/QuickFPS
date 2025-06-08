package wldlib

/*
#cgo CFLAGS: -I.

// macOS LDFLAGS (无 Raylib):
#cgo darwin LDFLAGS: -L.  -lwldcapi -lGameLogic  -lSerialize -lspdlogd -lc++


// Linux LDFLAGS (无 Raylib, 保留，以备跨平台):
// #cgo linux LDFLAGS: -L. -lGameLogic -lwldcapi -lspdlogd -lstdc++ -lm -lpthread -ldl


// Windows (MinGW) LDFLAGS (无 Raylib, 保留，以备跨平台):
// #cgo windows LDFLAGS: -L. -lGameLogic -lwldcapi -lspdlogd -pthread


#include "c_api.h"
#include <stdlib.h>
*/
import "C"
import (
	"unsafe"
)

type World struct {
	handle C.WorldHandle
}

func NewWorld() *World {
	h := C.CreateWorld()
	if h == nil {
		return nil
	}
	return &World{handle: h}
}

func (w *World) Destroy() {
	if w.handle != nil {
		C.DestroyWorld(w.handle)
		w.handle = nil
	}
}

func (w *World) Update() {
	if w.handle != nil {
		C.WorldUpdate(w.handle)
	}
}

func (w *World) AddPlayer() C.uint32_t {
	if w.handle == nil {
		return 0 // Or handle error
	}
	return C.uint32_t(C.AddPlayer(w.handle))
}

func (w *World) AddPlayerByID(pl uint32) bool {
	if w.handle == nil {
		return false
	}
	return C.AddPlayerByID(w.handle, C.uint32_t(pl)) != 0
}

func (w *World) GetWorldNumber() uint32 {
	if w.handle == nil {
		return 0
	}
	return uint32(C.GetEntityNumber(w.handle))
}

func (w *World) ExportWorldData() []byte {
	if w.handle == nil {
		return nil
	}
	// int GetAllEntitiesState(WorldHandle world, uint8_t** out_data, uint32_t* out_size);
	var data *C.uint8_t
	var size C.uint32_t
	if C.GetAllEntitiesState(w.handle, &data, &size) != 0 {
		return C.GoBytes(unsafe.Pointer(data), C.int(size))
	}
	return nil
}

func (w *World) RemovePlayer(playerID uint32) {
	if w.handle != nil {
		C.RemovePlayer(w.handle, C.uint32_t(playerID))
	}
}

func (w *World) SetPlayerPosition(playerID uint32, x, y, z float32) {
	if w.handle != nil {
		C.SetPlayerPosition(w.handle, C.uint32_t(playerID), C.float(x), C.float(y), C.float(z))
	}
}

type InputState struct {
	PlayerID       uint32
	WasdPressed    [4]uint8 // 使用固定大小的数组，与C定义匹配
	SpacePressed   uint8
	MouseDeltaX    float32
	MouseDeltaY    float32
	SequenceNumber uint32
}

// --- 【新增】创建 PushInput 方法来封装 CGO 调用 ---
// 这个方法是 *World 类型的一部分，修复了 `undefined method` 错误。
func (w *World) PushInput(input *InputState) {
	if w.handle == nil || input == nil {
		return
	}

	// 1. 将Go的InputState结构体转换为C的CInputState结构体
	cInput := C.CInputState{
		player_id:       C.uint32_t(input.PlayerID),
		space_pressed:   C.uchar(input.SpacePressed),
		mouseDeltax:     C.float(input.MouseDeltaX),
		mouseDeltay:     C.float(input.MouseDeltaY),
		sequence_number: C.uint32_t(input.SequenceNumber),
	}

	// 2. 拷贝wasd_pressed数组
	for i := 0; i < 4; i++ {
		cInput.wasd_pressed[i] = C.uchar(input.WasdPressed[i])
	}

	// 3. 调用底层的C函数
	C.PushInputToWorld(w.handle, &cInput)
}

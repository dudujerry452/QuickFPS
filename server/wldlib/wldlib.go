package wldlib

/*
#cgo CFLAGS: -I.

// macOS LDFLAGS:
#cgo darwin LDFLAGS: -L. -lwldcapi -lspdlogd -lc++ -lraylib -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL
// 如果 raylib 和 wldcapi 依赖 spdlog 且 spdlog 未编译进去, 你可能还需要:
// #cgo darwin LDFLAGS: -L. -lwldcapi -L/path/to/your/spdlog_build/lib -lspdlog -lraylib -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL

// Linux LDFLAGS (保留，以备跨平台):
// #cgo linux LDFLAGS: -L. -lwldcapi -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -L/path/to/spdlog/lib -lspdlog

// Windows (MinGW) LDFLAGS (保留，以备跨平台):
// #cgo windows LDFLAGS: -L. -lwldcapi -lraylib -lopengl32 -lgdi32 -lwinmm -pthread -L/path/to/spdlog/lib -lspdlog

#include "c_api.h"
#include <stdlib.h>
*/
import "C"

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

func (w *World) AddPlayer() uint32 {
	if w.handle == nil {
		return 0 // Or handle error
	}
	return uint32(C.AddPlayer(w.handle))
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

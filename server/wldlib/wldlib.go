package wldlib

/*
#cgo CFLAGS: -I.

// macOS LDFLAGS (无 Raylib):
#cgo darwin LDFLAGS: -L.  -lwldcapi -lGameLogic  -lspdlogd -lc++


// Linux LDFLAGS (无 Raylib, 保留，以备跨平台):
// #cgo linux LDFLAGS: -L. -lGameLogic -lwldcapi -lspdlogd -lstdc++ -lm -lpthread -ldl


// Windows (MinGW) LDFLAGS (无 Raylib, 保留，以备跨平台):
// #cgo windows LDFLAGS: -L. -lGameLogic -lwldcapi -lspdlogd -pthread


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

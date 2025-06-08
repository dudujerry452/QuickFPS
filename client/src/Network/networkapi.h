#ifndef _NETWORKAPI_H_
#define _NETWORKAPI_H_


#include <stdint.h>
#include <vector>
#include <functional>
#include <string>
#include <optional>
#include <chrono>

class Network; 
using MessageHandler = std::function<void(const uint8_t*, size_t)>;  // 回调

typedef Network* NetworkHandle;

NetworkHandle NcreateNetworkHandle();
void NdestroyNetworkHandle(NetworkHandle handle);

void Nstart(NetworkHandle handle, std::string serverAddress, uint16_t port);
void Nsend(NetworkHandle handle, char* message);
void NsendBytes(NetworkHandle handle, std::vector<uint8_t> data); 

void Nstop(NetworkHandle handle);

void Nset_handler(NetworkHandle handle, MessageHandler handler);

std::optional<std::vector<uint8_t>> Nreceive_blocking(NetworkHandle handle, std::chrono::milliseconds timeout);

// protocol implementation ---------------------------

class World;
uint32_t PtryHandshake(NetworkHandle handle, World*& world);
void HandleNetworkMessage(const uint8_t* data, size_t size); 

#endif
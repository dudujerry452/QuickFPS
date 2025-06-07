#include "networkapi.h"

#include "Network.h"
#include <cstdio>

NetworkHandle NcreateNetworkHandle() {
    return new Network();
}

void NdestroyNetworkHandle(NetworkHandle handle) {
    if (handle) {
        delete static_cast<Network*>(handle);
    }
}

void Nstart(NetworkHandle handle, char* serverAddress, uint16_t port) {
    if (handle) {
        static_cast<Network*>(handle)->start(serverAddress, port);
    }
}

void Nsend(NetworkHandle handle, char* message) {
    if (handle && message) {
        static_cast<Network*>(handle)->send<std::string>(std::string(message));
    }
}

void NsendBytes(NetworkHandle handle, std::vector<uint8_t> data) {
    if (handle && !data.empty()) {
        static_cast<Network*>(handle)->send<std::vector<uint8_t>>(data); 
    }
}

void Nstop(NetworkHandle handle) {
    if (handle) {
        static_cast<Network*>(handle)->stop();
    }
}
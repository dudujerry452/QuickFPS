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

void Nstart(NetworkHandle handle, std::string serverAddress, uint16_t port) {
    if (handle) {
        try {
            static_cast<Network*>(handle)->start(serverAddress, port);
        }
        catch(const std::exception& e) {
            throw e;
        }
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

void Nset_handler(NetworkHandle handle, MessageHandler handler) {
    if (handle) {
        static_cast<Network*>(handle)->setMessageHandler(handler);
    }
}

std::optional<std::vector<uint8_t>> Nreceive_blocking(NetworkHandle handle, std::chrono::milliseconds timeout) {
    if (handle) {
        return static_cast<Network*>(handle)->receive_blocking(timeout);
    }
    return std::nullopt;
}
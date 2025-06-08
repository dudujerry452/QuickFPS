#include "networkapi.h"
#include <random> 

#include "../Game/World.h"
#include "../Serialize/Serialize.h"

#include "spdlog/spdlog.h"

uint32_t client_code; 
uint32_t  PtryHandshake(NetworkHandle handle, World* world) {
    client_code = rand();

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<uint32_t> distrib {
        std::numeric_limits<uint32_t>::min(),
        std::numeric_limits<uint32_t>::max()
    };
    client_code = distrib(gen);
    spdlog::info("client code: {}", client_code);

    util::ClientHello clh = {client_code};
    
    auto serialized = serialization::serialize(clh);
    if (!serialized) {
        spdlog::error("Failed to serialize ClientHello");
        return 1;
    }
    NsendBytes(handle, *serialized);
    auto recv = Nreceive_blocking(handle, std::chrono::milliseconds(1000));
    if(!recv) {
        spdlog::error("Failed to receive ClientHello");
        return 1;
    }

    auto deserialized = serialization::deserialize(*recv);
    if (!deserialized) {
        spdlog::error("Failed to deserialize ServerHello");
        return 1;
    }

    if (std::holds_alternative<util::ServerHello>(deserialized.value())) {
        auto& shello = std::get<util::ServerHello>(deserialized.value());
        spdlog::info("Server code: {}", shello.code);
        spdlog::info("Server playerid: {}", shello.playerid);
        if(shello.code != client_code) {
            spdlog::error("Server code does not match client code");
            return 1;
        }
        // handshake success
        if(world) {
            delete world;
        }
        world = new World(); 
        world->InitLocalPlayer(shello.playerid);
        world->Attach();

        return 0;
    }

    return 1;
}

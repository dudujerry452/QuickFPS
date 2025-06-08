#include "networkapi.h"
#include <random> 

#include "../Game/World.h"
#include "../Serialize/Serialize.h"

#include "spdlog/spdlog.h"

extern World* g_world; 

uint32_t client_code; 
uint32_t  PtryHandshake(NetworkHandle handle, World*& world) {
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
        spdlog::error("Failed to receive ServerHello");
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

        // ------apply first world state  -----


        // auto vrecv = Nreceive_blocking(handle, std::chrono::milliseconds(1000));
        // if(!vrecv) {
        //     spdlog::error("Failed to receive ServerFirstState");
        //     return 1;
        // }
        // auto vdeserialized = serialization::deserialize(*recv);
        // if (!vdeserialized) {
        //     spdlog::error("Failed to deserialize ServerFirstState");
        //     return 1;
        // }
        // if (!std::holds_alternative<std::vector<util::EntityState> >(vdeserialized.value())) {
        //     spdlog::error("Receive not ServerFirstState");
        //     return 1;
        // }

        // auto& states = std::get<std::vector<util::EntityState> >(vdeserialized.value());
        // world->PrepareState(states);
        // --------apply end ---------

        return 0;
    }

    return 1;
}

// -------------------------------------------------------------------
// 【核心实现】 独立的网络消息处理函数
// -------------------------------------------------------------------
void HandleNetworkMessage(const uint8_t* data, size_t size) {
    // 检查全局 world 实例是否存在
    if (!g_world) {
        spdlog::warn("HandleNetworkMessage called but g_world is null. Ignoring message.");
        return;
    }

    // 1. 将 C 风格数组转换为 std::vector，以便反序列化
    std::vector<uint8_t> dataVec(data, data + size);

    // 2. 反序列化
    auto deserializedOpt = serialization::deserialize(dataVec);
    if (!deserializedOpt) {
        // 在高频更新中，这个日志可能会刷屏，可以考虑设为 trace 级别
        spdlog::warn("Failed to deserialize a message of size {}", size);
        return;
    }

    // 输出variant类型
    // spdlog::debug("Message type: {}", std::visit(overloaded {
    //     [](const std::vector<util::EntityState>&) { return "std::vector<util::EntityState>"; },
    //     [](const util::InputState&) { return "util::InputState"; },
    //     [](const util::ClientHello&) { return "util::ClientHello"; },
    //     [](const util::ServerHello&) { return "util::ServerHello"; }
    // }, deserializedOpt.value()));

    // 3. 创建一个 visitor 来处理不同的消息类型
    auto visitor = overloaded {
        // --- 处理世界状态广播 ---
        [](const std::vector<util::EntityState>& batch) {
            // 直接使用全局 g_world 指针
            g_world->PrepareState(batch);
        },
        // --- 其他消息类型的处理 (用于调试和健壮性) ---
        [](const util::InputState& state) {
            spdlog::warn("Received an unexpected InputState message from server.");
        },
        [](const util::ClientHello& hello) {
            spdlog::warn("Received an unexpected ClientHello message from server.");
        },
        [](const util::ServerHello& hello) {
            // 握手后的 ServerHello 消息通常可以忽略
            spdlog::info("Received a redundant ServerHello message (playerid: {}). Ignored.", hello.playerid);
        }
    };

    // 4. 使用 std::visit 来调用匹配的 lambda
    std::visit(visitor, deserializedOpt.value());
}
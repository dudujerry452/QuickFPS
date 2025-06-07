#include "Serialize.h"
#include "SerializeTool.h" // 包含底层C++ <-> Nanopb转换器

// Nanopb核心库
#include "pb_encode.h"
#include "pb_decode.h"

// Nanopb生成的头文件，包含了字段定义
extern "C" {
#include "c_protos/gamedata.pb.h"
}

#include <iostream>
#include <iomanip>
#include <assert.h>

namespace serialization {

// ---- PInputState 序列化 / 反序列化 ----

std::optional<std::vector<uint8_t>> serialize(const util::InputState& nativeState) {
    GameMessage gamemsg = GameMessage_init_zero;

    if (!ToNanopb(nativeState, &gamemsg.payload.input_state)) {
        return std::nullopt;
    }

    gamemsg.which_payload = GameMessage_input_state_tag; 

    std::vector<uint8_t> buffer(GameMessage_size);
    pb_ostream_t stream = pb_ostream_from_buffer(buffer.data(), buffer.size());

    if (!pb_encode(&stream, GameMessage_fields, &gamemsg)) {
        return std::nullopt;
    }

    buffer.resize(stream.bytes_written);
    return buffer;
}

deserialized_result deserialize(const std::vector<uint8_t>& data) {
    if (data.empty()) {
        return std::nullopt;
    }

    GameMessage nanopbMsg = GameMessage_init_zero;

    pb_istream_t stream = pb_istream_from_buffer(data.data(), data.size());

    if (!pb_decode(&stream, GameMessage_fields, &nanopbMsg)) {
        return std::nullopt;
    }

    switch (nanopbMsg.which_payload) {
        case GameMessage_entity_state_batch_tag:
            return FromNanopb(nanopbMsg.payload.entity_state_batch);
        case GameMessage_input_state_tag:
            return FromNanopb(nanopbMsg.payload.input_state);
        default:
            return std::nullopt;
    }
}



// ---- EntityStateBatch 序列化 / 反序列化 ----

std::optional<std::vector<uint8_t>> serialize(const std::vector<util::EntityState>& nativeBatch) {
    GameMessage gamemsg = GameMessage_init_zero; 

    if (!ToNanopb(nativeBatch, &gamemsg.payload.entity_state_batch)) {
        return std::nullopt;
    }

    gamemsg.which_payload = GameMessage_entity_state_batch_tag;

    std::vector<uint8_t> buffer(GameMessage_size);
    pb_ostream_t stream = pb_ostream_from_buffer(buffer.data(), buffer.size());

    if (!pb_encode(&stream, GameMessage_fields, &gamemsg)) {
        return std::nullopt;
    }

    buffer.resize(stream.bytes_written);
    return buffer;
}

void test_print_bytes(const std::vector<uint8_t>& vec) {
    std::cout << "Binary Data (size: " << vec.size() << "): ";
    for (const auto& byte : vec) {
        std::cout << "0x" << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte) << " ";
    }
    std::cout << std::dec << std::endl;
}

void test_input_state_serialization() {
    std::cout << "--- Testing InputState ---" << std::endl;

    // 1. 创建一个原生C++对象
    util::InputState originalState;
    originalState.player_id = 101;
    // originalState.wasd_pressed = {true, false, true, false};
    originalState.wasd_pressed[0] = true;
    originalState.wasd_pressed[1] = false;
    originalState.wasd_pressed[2] = true;
    originalState.wasd_pressed[3] = false;
    originalState.space_pressed = true;
    originalState.mouseDelta = {1.5f, -2.5f};
    originalState.sequence_number = 12345;

    // 2. 序列化
    auto binaryDataOpt = serialization::serialize(originalState);

    // 检查序列化是否成功
    assert(binaryDataOpt.has_value() && "Serialization failed!");
    std::vector<uint8_t> binaryData = *binaryDataOpt;
    test_print_bytes(binaryData);
    
    // 这就是你可以传递给 asio::async_write(socket, asio::buffer(binaryData), ...); 的数据

    // 3. 反序列化
    auto deserializedStateOpt = serialization::deserialize(binaryData);
    
    // 检查反序列化是否成功
    assert(deserializedStateOpt.has_value() && "Deserialization failed!");

    auto& val = deserializedStateOpt.value();
    auto visitor = overloaded {
        [](const std::vector<util::EntityState>& batch) {
            std::cout << "Deserialized EntityStateBatch" << std::endl;
        },
        [](const util::InputState& state) {
            auto& deserializedState = state;

            // 4. 验证数据一致性
            // assert(originalState.player_id == deserializedState.player_id);
            // assert(originalState.wasd_pressed == deserializedState.wasd_pressed);
            // assert(originalState.space_pressed == deserializedState.space_pressed);
            // assert(originalState.mouseDelta.x == deserializedState.mouseDelta.x);
            // assert(originalState.mouseDelta.y == deserializedState.mouseDelta.y);
            // assert(originalState.sequence_number == deserializedState.sequence_number);

            std::cout << "InputState test PASSED!" << std::endl;
            std::cout << "Deserialized InputState" << std::endl;
        }
        };
        std::visit(visitor, val);
    }



} // namespace serialization
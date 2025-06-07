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
    PInputState nanopbState = PInputState_init_zero;

    if (!ToNanopb(nativeState, &nanopbState)) {
        return std::nullopt;
    }

    std::vector<uint8_t> buffer(PInputState_size);
    pb_ostream_t stream = pb_ostream_from_buffer(buffer.data(), buffer.size());

    if (!pb_encode(&stream, PInputState_fields, &nanopbState)) {
        return std::nullopt;
    }

    buffer.resize(stream.bytes_written);
    return buffer;
}

// 函数签名已更新为 C++17 兼容的版本
std::optional<util::InputState> deserializeInputState(const std::vector<uint8_t>& data) {
    if (data.empty()) {
        return std::nullopt;
    }

    PInputState nanopbState = PInputState_init_zero;
    
    pb_istream_t stream = pb_istream_from_buffer(data.data(), data.size());

    if (!pb_decode(&stream, PInputState_fields, &nanopbState)) {
        return std::nullopt;
    }

    return FromNanopb(nanopbState);
}


// ---- EntityStateBatch 序列化 / 反序列化 ----

std::optional<std::vector<uint8_t>> serialize(const std::vector<util::EntityState>& nativeBatch) {
    EntityStateBatch nanopbBatch = EntityStateBatch_init_zero;

    if (!ToNanopb(nativeBatch, &nanopbBatch)) {
        return std::nullopt;
    }

    std::vector<uint8_t> buffer(EntityStateBatch_size);
    pb_ostream_t stream = pb_ostream_from_buffer(buffer.data(), buffer.size());

    if (!pb_encode(&stream, EntityStateBatch_fields, &nanopbBatch)) {
        return std::nullopt;
    }

    buffer.resize(stream.bytes_written);
    return buffer;
}

// 函数签名已更新为 C++17 兼容的版本
std::optional<std::vector<util::EntityState>> deserializeEntityStateBatch(const std::vector<uint8_t>& data) {
    if (data.empty()) {
        return std::nullopt;
    }

    EntityStateBatch nanopbBatch = EntityStateBatch_init_zero;

    pb_istream_t stream = pb_istream_from_buffer(data.data(), data.size());

    if (!pb_decode(&stream, EntityStateBatch_fields, &nanopbBatch)) {
        return std::nullopt;
    }

    return FromNanopb(nanopbBatch);
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
    auto deserializedStateOpt = serialization::deserializeInputState(binaryData);
    
    // 检查反序列化是否成功
    assert(deserializedStateOpt.has_value() && "Deserialization failed!");
    util::InputState deserializedState = *deserializedStateOpt;

    // 4. 验证数据一致性
    assert(originalState.player_id == deserializedState.player_id);
    assert(originalState.wasd_pressed == deserializedState.wasd_pressed);
    assert(originalState.space_pressed == deserializedState.space_pressed);
    assert(originalState.mouseDelta.x == deserializedState.mouseDelta.x);
    assert(originalState.mouseDelta.y == deserializedState.mouseDelta.y);
    assert(originalState.sequence_number == deserializedState.sequence_number);

    std::cout << "InputState test PASSED!" << std::endl;
}

void test_entity_state_batch_serialization() {
    std::cout << "\n--- Testing EntityStateBatch ---" << std::endl;

    // 1. 创建原生对象
    std::vector<util::EntityState> originalBatch;
    util::EntityState entity1, entity2;
    entity1.id = 1;
    entity1.position = {1.0f, 2.0f, 3.0f};
    entity1.health = 100;
    entity2.id = 2;
    entity2.position = {4.0f, 5.0f, 6.0f};
    entity2.health = 90;
    originalBatch.push_back(entity1);
    originalBatch.push_back(entity2);

    // 2. 序列化
    auto binaryDataOpt = serialization::serialize(originalBatch);
    assert(binaryDataOpt.has_value() && "Batch serialization failed!");
    std::vector<uint8_t> binaryData = *binaryDataOpt;
    test_print_bytes(binaryData);

    // 3. 反序列化
    auto deserializedBatchOpt = serialization::deserializeEntityStateBatch(binaryData);
    assert(deserializedBatchOpt.has_value() && "Batch deserialization failed!");
    std::vector<util::EntityState> deserializedBatch = *deserializedBatchOpt;

    // 4. 验证
    assert(originalBatch.size() == deserializedBatch.size());
    assert(originalBatch[0].id == deserializedBatch[0].id);
    assert(originalBatch[0].position.x == deserializedBatch[0].position.x);
    assert(originalBatch[1].id == deserializedBatch[1].id);
    assert(originalBatch[1].position.z == deserializedBatch[1].position.z);
    
    std::cout << "EntityStateBatch test PASSED!" << std::endl;
}


} // namespace serialization
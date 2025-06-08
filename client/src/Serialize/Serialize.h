#ifndef _SERIALIZE_H_
#define _SERIALIZE_H_

#include "../Util/Util.h" // 包含您的原生数据结构定义
#include <vector>
#include <cstdint>
#include <optional> // C++17 特性, 保持不变

namespace serialization {

/**
 * @brief 将 util::InputState 对象序列化为二进制字节向量。
 */
std::optional<std::vector<uint8_t>> serialize(const util::InputState& nativeState);

/**
 * @brief 将实体状态批处理（std::vector<util::EntityState>）序列化为二进制字节向量。
 */
std::optional<std::vector<uint8_t>> serialize(const std::vector<util::EntityState>& nativeBatch);

/**
 * @brief 将 util::ClientHello 对象序列化为二进制字节向量。
 */
std::optional<std::vector<uint8_t>> serialize(const util::ClientHello& nativeHello);

/**
 * @brief 将 util::ServerHello 对象序列化为二进制字节向量。
 */
std::optional<std::vector<uint8_t>> serialize(const util::ServerHello& nativeHello);

using deserialized_result = std::optional<std::variant<
    util::InputState, 
    std::vector<util::EntityState>,
    util::ClientHello, 
    util::ServerHello
    > >;

deserialized_result deserialize(const std::vector<uint8_t>& data);

void test_input_state_serialization();
} // namespace serialization



#endif
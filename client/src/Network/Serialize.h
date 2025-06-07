#ifndef _SERIALIZE_H_
#define _SERIALIZE_H_

#include "../Util/Util.h" // 包含您的原生数据结构定义
#include <vector>
#include <cstdint>
#include <optional> // C++17 特性, 保持不变

namespace serialization {

/**
 * @brief 将 util::InputState 对象序列化为二进制字节向量。
 * @param nativeState 要序列化的原生 C++ InputState 对象。
 * @return std::optional<std::vector<uint8_t>> 包含序列化后二进制数据的向量。如果序列化失败，则返回 std::nullopt。
 */
std::optional<std::vector<uint8_t>> serialize(const util::InputState& nativeState);

/**
 * @brief 从二进制数据反序列化为 util::InputState 对象。
 * @param data 包含二进制数据的向量的常量引用。
 * @return std::optional<util::InputState> 成功则返回反序列化后的原生对象，如果数据无效或解析失败，则返回 std::nullopt。
 */
std::optional<util::InputState> deserializeInputState(const std::vector<uint8_t>& data);


/**
 * @brief 将实体状态批处理（std::vector<util::EntityState>）序列化为二进制字节向量。
 * @param nativeBatch 要序列化的原生 C++ EntityState 批处理。
 * @return std::optional<std::vector<uint8_t>> 包含序列化后二进制数据的向量。如果批处理大小超过协议限制或序列化失败，则返回 std::nullopt。
 */
std::optional<std::vector<uint8_t>> serialize(const std::vector<util::EntityState>& nativeBatch);

/**
 * @brief 从二进制数据反序列化为实体状态批处理。
 * @param data 包含二进制数据的向量的常量引用。
 * @return std::optional<std::vector<util::EntityState>> 成功则返回反序列化后的原生批处理，如果数据无效或解析失败，则返回 std::nullopt。
 */
std::optional<std::vector<util::EntityState>> deserializeEntityStateBatch(const std::vector<uint8_t>& data);

void test_entity_state_batch_serialization();
} // namespace serialization



#endif
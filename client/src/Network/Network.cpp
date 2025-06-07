#include "Network.h"
#include "spdlog/spdlog.h"

#include "pb_encode.h"
#include "pb_decode.h"

#include "./c_protos/sensor.pb.h"

int xxxtest() {
    printf("--- Nanopb Sensor Example ---\n\n");

    // --- 1. 编码 (创建并序列化一个SensorBatch消息) ---
    printf("--- Encoding ---\n");

    // 创建一个消息对象。由于Nanopb使用静态分配，
    // 我们直接在栈上声明一个结构体变量。
    // _init_zero宏会将所有字段初始化为0/false/空。
    SensorBatch batch_to_encode = SensorBatch_init_zero;

    // 准备一个缓冲区来存放序列化后的数据
    uint8_t buffer[256];
    
    // 填充数据
    batch_to_encode.readings_count = 2; // 我们要发送两个读数

    // 第一个读数
    strcpy(batch_to_encode.readings[0].sensor_id, "temp_living_room");
    batch_to_encode.readings[0].timestamp = 1678886400;
    batch_to_encode.readings[0].value = 23.5f;
    batch_to_encode.readings[0].status = SensorReading_Status_OK;

    // 第二个读数
    strcpy(batch_to_encode.readings[1].sensor_id, "humidity_basement");
    batch_to_encode.readings[1].timestamp = 1678886410;
    batch_to_encode.readings[1].value = 85.1f;
    batch_to_encode.readings[1].status = SensorReading_Status_WARNING;
    
    // 创建一个输出流，关联到我们的缓冲区
    pb_ostream_t ostream = pb_ostream_from_buffer(buffer, sizeof(buffer));

    // 调用编码函数
    // SensorBatch_fields 是由代码生成器创建的，描述了消息的结构
    bool status = pb_encode(&ostream, SensorBatch_fields, &batch_to_encode);

    if (!status) {
        printf("Encoding failed: %s\n", PB_GET_ERROR(&ostream));
        return 1;
    }

    size_t message_length = ostream.bytes_written;
    printf("Encoding successful! Message size: %zu bytes\n", message_length);


    // --- 2. 解码 (反序列化数据到另一个结构体中) ---
    printf("\n--- Decoding ---\n");

    // 创建一个用于接收解码数据的空消息对象
    SensorBatch batch_to_decode = SensorBatch_init_zero;

    // 创建一个输入流，从包含序列化数据的缓冲区读取
    pb_istream_t istream = pb_istream_from_buffer(buffer, message_length);

    // 调用解码函数
    status = pb_decode(&istream, SensorBatch_fields, &batch_to_decode);

    if (!status) {
        printf("Decoding failed: %s\n", PB_GET_ERROR(&istream));
        return 1;
    }
    
    printf("Decoding successful!\n");
    printf("Number of readings in batch: %d\n", batch_to_decode.readings_count);

    // 打印解码后的数据进行验证
    for (int i = 0; i < batch_to_decode.readings_count; i++) {
        printf("\n  Reading #%d:\n", i + 1);
        printf("    Sensor ID: %s\n", batch_to_decode.readings[i].sensor_id);
        printf("    Timestamp: %llu\n", (unsigned long long)batch_to_decode.readings[i].timestamp);
        printf("    Value: %.2f\n", batch_to_decode.readings[i].value);
        
        const char* status_str = "UNKNOWN";
        switch (batch_to_decode.readings[i].status) {
            case SensorReading_Status_OK:      status_str = "OK"; break;
            case SensorReading_Status_WARNING: status_str = "WARNING"; break;
            case SensorReading_Status_ERROR:   status_str = "ERROR"; break;
            default: break;
        }
        printf("    Status: %s\n", status_str);
    }
    return 0;
}

void Network::start(const std::string& serverAddress, uint16_t port) {
    spdlog::info("Starting network with server {}:{}", serverAddress, port);
    asio::ip::udp::resolver resolver(m_ioContext);
    try {
        m_endpoint = *resolver.resolve(asio::ip::udp::v4(), serverAddress, std::to_string(port)).begin();
        m_socket.open(m_endpoint.protocol());

        m_socket.connect(m_endpoint); // not connect like tcp 
        spdlog::info("Network started, connected to {}:{}", serverAddress, port);
    }    
    catch (const std::exception& e) {
        spdlog::error("Failed to start network: {}", e.what());
        return;
    }

    m_running = true;
    m_networkThread = std::thread([this]() {
        try {
            m_ioContext.run();
        } catch (const std::exception& e) {
            spdlog::error("Network thread error: {}", e.what());
            m_running = false;
        }
    });
}

void Network::send(std::string message) {
    auto data = std::make_shared<std::string>(std::move(message));
    if (m_socket.is_open()) {
        asio::post(m_ioContext, [this, data]() {
            m_socket.async_send(
                asio::buffer(*data), 
                [this, data](const asio::error_code& error, std::size_t) {
                    if(error) {
                        spdlog::error("Failed to send message: {}", error.message());
                    } else {
                        spdlog::info("Message sent successfully: {}", *data);
                    }
                }
            );
        });
    } else {
        spdlog::error("Socket is not open, cannot send message.");
    }
}

void Network::stop() {
    if(!m_running) {
        return;
    }
    m_running = false;
    spdlog::info("Stopping network thread...");
    asio::post(m_ioContext, [this]() {
        if(m_socket.is_open()) {
            asio::error_code ec;
            m_socket.shutdown(asio::ip::udp::socket::shutdown_both, ec);
            if(ec) {
                spdlog::error("Failed to shutdown socket: {}", ec.message());
            } else {
                spdlog::info("Socket shutdown successfully.");  
            }
        }
    });
    m_workGuard.reset(); 
    m_ioContext.stop();

    if(m_networkThread.joinable()) {
        m_networkThread.join();
        spdlog::info("Network thread stopped.");
    } else {
        spdlog::warn("Network thread was not joinable.");
    }


}
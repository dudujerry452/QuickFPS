#include "Network.h"
#include "spdlog/spdlog.h"

#include "pb_encode.h"
#include "pb_decode.h"
#include <stdexcept>

// int xxxtest() {
//     printf("--- Nanopb Sensor Example ---\n\n");

//     // --- 1. 编码 (创建并序列化一个SensorBatch消息) ---
//     printf("--- Encoding ---\n");

//     // 创建一个消息对象。由于Nanopb使用静态分配，
//     // 我们直接在栈上声明一个结构体变量。
//     // _init_zero宏会将所有字段初始化为0/false/空。
//     SensorBatch batch_to_encode = SensorBatch_init_zero;

//     // 准备一个缓冲区来存放序列化后的数据
//     uint8_t buffer[256];
    
//     // 填充数据
//     batch_to_encode.readings_count = 2; // 我们要发送两个读数

//     // 第一个读数
//     strcpy(batch_to_encode.readings[0].sensor_id, "temp_living_room");
//     batch_to_encode.readings[0].timestamp = 1678886400;
//     batch_to_encode.readings[0].value = 23.5f;
//     batch_to_encode.readings[0].status = SensorReading_Status_OK;

//     // 第二个读数
//     strcpy(batch_to_encode.readings[1].sensor_id, "humidity_basement");
//     batch_to_encode.readings[1].timestamp = 1678886410;
//     batch_to_encode.readings[1].value = 85.1f;
//     batch_to_encode.readings[1].status = SensorReading_Status_WARNING;
    
//     // 创建一个输出流，关联到我们的缓冲区
//     pb_ostream_t ostream = pb_ostream_from_buffer(buffer, sizeof(buffer));

//     // 调用编码函数
//     // SensorBatch_fields 是由代码生成器创建的，描述了消息的结构
//     bool status = pb_encode(&ostream, SensorBatch_fields, &batch_to_encode);

//     if (!status) {
//         printf("Encoding failed: %s\n", PB_GET_ERROR(&ostream));
//         return 1;
//     }

//     size_t message_length = ostream.bytes_written;
//     printf("Encoding successful! Message size: %zu bytes\n", message_length);


//     // --- 2. 解码 (反序列化数据到另一个结构体中) ---
//     printf("\n--- Decoding ---\n");

//     // 创建一个用于接收解码数据的空消息对象
//     SensorBatch batch_to_decode = SensorBatch_init_zero;

//     // 创建一个输入流，从包含序列化数据的缓冲区读取
//     pb_istream_t istream = pb_istream_from_buffer(buffer, message_length);

//     // 调用解码函数
//     status = pb_decode(&istream, SensorBatch_fields, &batch_to_decode);

//     if (!status) {
//         printf("Decoding failed: %s\n", PB_GET_ERROR(&istream));
//         return 1;
//     }
    
//     printf("Decoding successful!\n");
//     printf("Number of readings in batch: %d\n", batch_to_decode.readings_count);

//     // 打印解码后的数据进行验证
//     for (int i = 0; i < batch_to_decode.readings_count; i++) {
//         printf("\n  Reading #%d:\n", i + 1);
//         printf("    Sensor ID: %s\n", batch_to_decode.readings[i].sensor_id);
//         printf("    Timestamp: %llu\n", (unsigned long long)batch_to_decode.readings[i].timestamp);
//         printf("    Value: %.2f\n", batch_to_decode.readings[i].value);
        
//         const char* status_str = "UNKNOWN";
//         switch (batch_to_decode.readings[i].status) {
//             case SensorReading_Status_OK:      status_str = "OK"; break;
//             case SensorReading_Status_WARNING: status_str = "WARNING"; break;
//             case SensorReading_Status_ERROR:   status_str = "ERROR"; break;
//             default: break;
//         }
//         printf("    Status: %s\n", status_str);
//     }
//     return 0;
// }

void Network::start(const std::string& serverAddress, uint16_t port) {
    spdlog::info("Starting network with server {}:{}", serverAddress, port);
    spdlog::debug("1");
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
    spdlog::debug("2");

     m_running = true;
    spdlog::info("Starting network with server {}:{}", serverAddress, port);
    
    spdlog::debug("3");
    // 获取 future，以便我们可以等待
    auto readyFuture = m_readyPromise.get_future();

    spdlog::debug("4");
    m_networkThread = std::thread([this]() {
        try {
            // 在开始事件循环前，先提交一个接收任务
            do_receive(); 
    spdlog::debug("5");
            
            // 通知主线程，我们已经准备好了
            m_readyPromise.set_value(); // <--- 新增
    spdlog::debug("6");

            // 现在才开始阻塞运行事件循环
            m_ioContext.run();
        } catch (const std::exception& e) {
            // 如果启动失败，也需要通知主线程
             if (!m_promise_is_set.load()) { // <--- 使用原子 load()
                 try { 
                     m_readyPromise.set_exception(std::current_exception()); 
                 } catch(...) {
                     // set_exception 自身也可能抛出异常 (如果 promise 已被满足),
                     // 但我们的原子标志可以防止这种情况。这个 catch 只是为了绝对安全。
                 }
            }
            spdlog::error("Network thread error: {}", e.what());
            m_running = false;
        }
    });

    spdlog::debug("7");
     try {
        spdlog::debug("try Get future");
        readyFuture.get(); // 这个调用可能会抛出异常
        spdlog::debug("Network thread is ready. Proceeding.");
    } catch (const std::exception& e) {
        spdlog::error("Network thread failed to start: {}", e.what());
        // 既然网络线程启动失败，我们需要确保它被清理
        if (m_networkThread.joinable()) {
            m_networkThread.join();
        }
        throw std::runtime_error("launch failed.");
        return; 
    }
    spdlog::debug("8");

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
                m_socket.cancel(ec);
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

std::optional<std::vector<uint8_t>> Network::receive_blocking(std::chrono::milliseconds timeout) {

    if (!m_running) {
        spdlog::warn("Cannot block on receive, network is not running.");
        return std::nullopt;
    }

    std::promise<std::vector<uint8_t>> promise;
    auto future = promise.get_future();

    // 注册 promise，以便 do_receive 可以找到它
    {
        std::lock_guard<std::mutex> lock(m_blockingRecvMutex);
        // 为了简单起见，我们假设同一时间只有一个线程会调用此函数。
        // 如果需要支持多个线程同时调用，需要一个 promise 队列。
        if (m_blockingPromise.has_value()) {
            spdlog::error("Another thread is already waiting for a blocking receive.");
            // 立即抛出异常或返回空，取决于你的错误处理策略
            throw std::runtime_error("Concurrent blocking receive is not supported.");
        }
        m_blockingPromise = std::move(promise);
    }

    spdlog::trace("Blocking receive waiting for message...");

    // 等待 future
    try {
        if (future.wait_for(timeout) == std::future_status::ready) {
            spdlog::trace("Blocking receive got message.");
            return future.get(); // 获取数据
        } else {
            spdlog::trace("Blocking receive timed out.");
            // 超时后，必须移除我们之前设置的 promise，否则它会一直存在
            // 并且会捕获下一条本应异步处理的消息。
            std::lock_guard<std::mutex> lock(m_blockingRecvMutex);
            m_blockingPromise.reset();
            return std::nullopt; // 返回空表示超时
        }
    } catch (const std::exception& e) {
        // 如果 promise 被设置了异常 (例如在 stop() 中), 这里会捕获到
        spdlog::warn("Blocking receive cancelled: {}", e.what());
        return std::nullopt;
    }
}

void Network::do_receive() {
    m_socket.async_receive(
        asio::buffer(m_recvBuffer),
        [this](const asio::error_code& ec, std::size_t bytes_transferred) {
            if (!ec) {
                bool message_handled_by_blocker = false;

                // 检查是否有阻塞调用在等待
                {
                    std::lock_guard<std::mutex> lock(m_blockingRecvMutex);
                    if (m_blockingPromise.has_value()) {
                        // 创建数据的副本
                        std::vector<uint8_t> data_copy(m_recvBuffer.data(), m_recvBuffer.data() + bytes_transferred);
                        
                        // 满足 promise 并将其移除
                        m_blockingPromise->set_value(std::move(data_copy));
                        m_blockingPromise.reset();
                        
                        message_handled_by_blocker = true;
                        spdlog::trace("Message delivered to blocking receiver.");
                    }
                }

                // 如果没有被阻塞调用处理，则使用常规的异步回调
                if (!message_handled_by_blocker) {
                    
                    // --- 【关键修改】安全地读取和调用 handler ---
                    
                    MessageHandler handler_copy; // 1. 创建一个 handler 的本地拷贝

                    { // 2. 创建一个独立的作用域来限制锁的生命周期
                        std::lock_guard<std::mutex> lock(m_handlerMutex);
                        handler_copy = m_onMessageRecv; // 3. 在锁的保护下，将成员变量拷贝到本地变量
                    } // 4. lock_guard 在这里析构，互斥锁被释放

                    if (handler_copy) { // 5. 在锁之外，安全地使用本地拷贝
                        try {
                            // 调用回调函数
                            handler_copy(m_recvBuffer.data(), bytes_transferred);
                        } catch (const std::exception& e) {
                            spdlog::error("Exception in message handler: {}", e.what());
                        }
                    } else {
                        // 即使 handler 为空，也只是一个日志，没有危害
                        spdlog::trace("Received message but no async handler was set.");
                    }
                }

                // 再次启动下一次接收
                if (m_running) {
                    do_receive();
                }

            } else {
                if (ec == asio::error::operation_aborted) {
                    spdlog::info("Receive operation cancelled (network stopping).");
                } else if(m_running) { // 只有在网络应该运行时才报告错误
                    spdlog::error("Receive error: {}", ec.message());
                }
            }
        });
}
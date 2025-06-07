#ifndef _NETWORK_H_
#define _NETWORK_H_

#include "asio.hpp"
#include <atomic> 
#include <cstdio>

#include "spdlog/spdlog.h"



/* 
Network: A class handle client side network information exchange. 
Model: ASynchronous IO with asio library.
This class is responsible for sending and receiving data to/from the server.
*/
class Network {

    public: 
    using MessageHandler = std::function<void(const uint8_t*, size_t)>;  // 回调
    Network(): 
        m_workGuard(asio::make_work_guard(m_ioContext)), 
        m_socket(m_ioContext),
        m_running(false)
    {}   
    ~Network() {
        stop(); 
    }

    void start(const std::string& serverAddress, uint16_t port);
    void stop(); 

    void setMessageHandler(MessageHandler handler) {
        m_onMessageRecv = std::move(handler);
    }

    private: 

    void do_receive();

    // baisc 
    asio::io_context m_ioContext; // IO context for asio
    asio::executor_work_guard<asio::io_context::executor_type> m_workGuard; // Keeps io_context running
    asio::ip::udp::socket m_socket; // TCP socket for network communication
    asio::ip::udp::endpoint m_endpoint; // Endpoint for the server connection

    // thread
    std::thread m_networkThread;
    std::atomic<bool> m_running; // Flag to control the network thread 

    // udp data
    MessageHandler m_onMessageRecv; 
    std::array<uint8_t, 1500> m_recvBuffer; 
    

    public: 










    public: 
    template<class T>
    void send(T message) { // 沉降语义
        auto data = std::make_shared<T>(std::move(message));
        if (m_socket.is_open()) {
            asio::post(m_ioContext, [this, data]() {
                m_socket.async_send(
                    asio::buffer(*data), 
                    [this, data](const asio::error_code& error, std::size_t) {
                        if(error) {
                            spdlog::error("Failed to send message: {}", error.message());
                        } else {
                            spdlog::info("Message sent successfully");
                        }
                    }
                );
            });
        } else {
            spdlog::error("Socket is not open, cannot send message.");
        }
    }
};


#endif 
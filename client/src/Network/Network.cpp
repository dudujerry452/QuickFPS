#include "Network.h"
#include "spdlog/spdlog.h"

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
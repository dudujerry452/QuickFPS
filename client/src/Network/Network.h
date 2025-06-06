#ifndef _NETWORK_H_
#define _NETWORK_H_

#include "asio.hpp"
#include <atomic> 
#include <cstdio>

/* 
Network: A class handle client side network information exchange. 
Model: ASynchronous IO with asio library.
This class is responsible for sending and receiving data to/from the server.
*/
class Network {

    public: 
    Network(): 
        m_workGuard(asio::make_work_guard(m_ioContext)), 
        m_socket(m_ioContext),
        m_running(false)
    {}   
    ~Network() {
        stop(); 
    }

    void start(const std::string& serverAddress, uint16_t port);
    void send(std::string message);
    void stop(); 




    private: 

    asio::io_context m_ioContext; // IO context for asio
    asio::executor_work_guard<asio::io_context::executor_type> m_workGuard; // Keeps io_context running
    asio::ip::udp::socket m_socket; // TCP socket for network communication
    asio::ip::udp::endpoint m_endpoint; // Endpoint for the server connection

    std::thread m_networkThread;
    std::atomic<bool> m_running; // Flag to control the network thread 

};


#endif 
//
// Created by franciscolopez on 09/08/2023.
//

#ifndef SOCKET_LIB_TCPSOCKET_H
#define SOCKET_LIB_TCPSOCKET_H
#define INFINITERETRIES -1

#include "socket/Socket.h"
#include <ctime>
#include <string>
#ifdef _WIN32
#include <WinSock2.h>
#endif
class TCPSocket : public Socket {
public:
    enum mode { SERVER,
                CLIENT };

protected:
    std::string remoteIp;
    int localPort{};
    int remotePort{};
    TCPSocket() = default;
    TCPSocket(const std::string &ip, int localPort, int remotePort) : remoteIp(ip), localPort(localPort), remotePort(remotePort) {}
    TCPSocket(const std::string &ip, int localPort, int remotePort, mode mode, unsigned maxRetries, const unsigned time) : remoteIp(ip), localPort(localPort), remotePort(remotePort), actualMode(mode), maxRetries(maxRetries), retryTimeout{static_cast<long>(time), 0} {}


    mode actualMode = mode::CLIENT;
    unsigned maxRetries = 5;
    timeval retryTimeout = timeval{1, 0};
    timeval timeoutReading = timeval{1,0};
    bool socketopen = false;
};


#endif//SOCKET_LIB_TCPSOCKET_H

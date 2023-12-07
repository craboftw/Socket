//
// Created by franciscolopez on 16/08/2023.
//

#include "factory/FactorySocket.h"
#include "socket/UDP/UDPSocket.h"
#include "socket/Serial/SerialSocket.h"
#include "socket/TCP/TCPSocket.h"
#include <memory>


std::unique_ptr<Socket> FactorySocket::createUDPSocket(std::string ip, int localPort, int remotePort) {
    return std::make_unique<UDPSocket>(ip,localPort,remotePort);
}

 std::unique_ptr<Socket> FactorySocket::createSerialSocket(std::string portName, int baudRate, int dataBits, int stopBits, int parity)
{
    return std::make_unique<SerialSocket>(portName,baudRate,dataBits,stopBits,parity);
}
/*
std::unique_ptr<Socket> FactorySocket::createTCPSocket(const std::string &ip, int localPort, int remotePort, TCPSocket::mode mode, unsigned int maxRetries, unsigned int retryTimeout)
{
    return std::make_unique<TCPSocket>(ip,localPort,remotePort,mode,maxRetries,retryTimeout);
}
*/
#ifndef SOCKET_LIB_FACTORYSOCKET_H
#define SOCKET_LIB_FACTORYSOCKET_H


#include "socket/Socket.h"
#include <memory>

class FactorySocket {
public:
    /**
@brief Abstract factory class for creating sockets.
This class provides an abstract factory for creating sockets. Concrete
implementations of this class should provide methods for creating UDP sockets,
TCP sockets, and serial sockets. / class FactorySocket { public: /*
@brief Creates a UDP socket.
@param ip The IP address of the remote host.
@param localPort The local port.
@param remotePort The remote port.
@return A unique pointer to the socket. */
    static std::unique_ptr<Socket> createUDPSocket(std::string ip, int localPort, int remotePort);
    /**

@brief Creates a TCP socket.
@param ip The IP address of the remote host.
@param localPort The local port.
@param remotePort The remote port.
@param mode The mode of the socket. This can be either `TCPSocket::mode::client` or `TCPSocket::mode::server`.
@param maxRetries The maximum number of retries.
@param retryTimeout The retry timeout in milliseconds.
@return A unique pointer to the socket. */
   // static std::unique_ptr<Socket> createTCPSocket(std::string ip, int localPort, int remotePort, TCPSocket::mode mode, unsigned int maxRetries, unsigned int retryTimeout) ;
    /**

@brief Creates a serial socket.
@param portName The name of the serial port.
@param baudRate The baud rate.
@param dataBits The number of data bits.
@param stopBits The number of stop bits.
@param parity The parity.
@return A unique pointer to the socket. */
    static std::unique_ptr<Socket> createSerialSocket(std::string portName, int baudRate, int dataBits, int stopBits, int parity) ;
};

#endif //SOCKET_LIB_FACTORYSOCKET_H
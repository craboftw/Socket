/**
* @file LinuxTCPSocket.h
* @brief Contains the LinuxTCPSocket class declaration.
* @author Francisco López Guerrero < francisco.lopez@gtd.eu >

*/

#ifndef SOCKET_LIB_LINUXTCPSOCKET_H
#define SOCKET_LIB_LINUXTCPSOCKET_H

#include "socket/TCP/TCPSocket.h"
#include <netinet/in.h>
#include <string>
#include <vector>

/**
* @class LinuxTCPSocket
* @brief Represents a TCP socket implementation for Linux.
*
* This class inherits from TCPSocket and provides specific functionality
* for TCP communication on the Linux platform.
*/
class LinuxTCPSocket : public TCPSocket {
public:
   /**
    * @brief Default constructor.
    */
   LinuxTCPSocket() = default;

   /**
    * @brief Parameterized constructor for client mode.
    *
    * Creates a LinuxTCPSocket object in client mode with specified connection parameters.
    *
    * @param ip The IP address to connect to.
    * @param localPort The local port to bind.
    * @param remotePort The remote port to connect to.
    */
   LinuxTCPSocket(const std::string& ip, int localPort, int remotePort)
       : TCPSocket(ip, localPort, remotePort) {}

   /**
    * @brief Parameterized constructor for client mode with additional settings.
    *
    * Creates a LinuxTCPSocket object with specified connection parameters
    * and additional settings.
    *
    * @param ip The IP address to connect to.
    * @param localPort The local port to bind.
    * @param remotePort The remote port to connect to.
    * @param mode The communication mode.
    * @param maxRetries The maximum number of connection retries.
    * @param retryTimeout The timeout between connection retries.
    */
   LinuxTCPSocket(const std::string& ip, int localPort, int remotePort, mode mode, unsigned maxRetries, unsigned retryTimeout)
       : TCPSocket(ip, localPort, remotePort, mode, maxRetries, retryTimeout) {}

   /**
    * @brief Destructor.
    */
   ~LinuxTCPSocket() override;

   /**
    * @brief Read data from the TCP socket.
    * @return The deserialized object read from the TCP socket.
    */
   Serializable read() override;

   /**
    * @brief Write data to the TCP socket.
    * @param serializableObj The object to be serialized and written to the TCP socket.
    */
   void write(Serializable serializableObj) override;

   /**
    * @brief Open the TCP socket for communication.
    */
   void open() override;

   /**
    * @brief Close the TCP socket.
    */
   void close() override;

private:
   int tcpSocket;          ///< File descriptor for the TCP socket.
   sockaddr_in localAddr;  ///< Local address structure.
   sockaddr_in remoteAddr; ///< Remote address structure.
   int serverSocket = -1;       ///< Server socket file descriptor.
   int clientSocket = -1;      ///< Client socket file descriptor.

   /**
    * @brief Reconnect to the server.
    */
   void reconnect();

   /**
    * @brief Start listening for incoming connections.
    */
   void startListening();

   /**
    * @brief Connect to the server with retries.
    * @return True if the connection was successful, false otherwise.
    */
   bool connectWithRetries();

   /**
    * @brief Open a thread for socket operations.
    */
   void openthread();
   bool isConnected();
};

#endif // SOCKET_LIB_LINUXTCPSOCKET_H

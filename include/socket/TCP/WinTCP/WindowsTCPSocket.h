#ifdef _WIN32

/**
* @file WindowsTCPSocket.h
* @brief Contains the WindowsTCPSocket class declaration.
* @author Francisco López Guerrero < francisco.lopez@gtd.eu >

*/

#ifndef SOCKET_LIB_WINDOWSTCPSOCKET_H
#define SOCKET_LIB_WINDOWSTCPSOCKET_H

#include <winsock2.h>
#include <windows.h>
#include "../TCPSocket.h"

/**
* @class WindowsTCPSocket
* @brief Represents a TCP socket implementation for Windows.
*
* This class inherits from TCPSocket and provides specific functionality
* for TCP communication on the Windows platform.
*/
class WindowsTCPSocket : public TCPSocket {
public:
   /**
    * @brief Default constructor.
    */
   WindowsTCPSocket();

   /**
    * @brief Parameterized constructor for client mode.
    *
    * Creates a WindowsTCPSocket object in client mode with specified connection parameters.
    *
    * @param ip The IP address to connect to.
    * @param localPort The local port to bind.
    * @param remotePort The remote port to connect to.
    */
   WindowsTCPSocket(const std::string &ip, int localPort, int remotePort);

   /**
    * @brief Parameterized constructor for client mode with additional settings.
    *
    * Creates a WindowsTCPSocket object in client mode with specified connection parameters
    * and additional settings.
    *
    * @param ip The IP address to connect to.
    * @param localPort The local port to bind.
    * @param remotePort The remote port to connect to.
    * @param mode The communication mode.
    * @param maxRetries The maximum number of connection retries.
    * @param retryTimeout The timeout between connection retries.
    */
   WindowsTCPSocket(const std::string &ip, int localPort, int remotePort, mode mode, unsigned maxRetries, unsigned retryTimeout);

   /**
    * @brief Destructor.
    */
   ~WindowsTCPSocket();

   /**
    * @brief Open the TCP socket for communication.
    */
   void open() override;

   /**
    * @brief Close the TCP socket.
    */
   void close() override;

   /**
    * @brief Write data to the TCP socket.
    * @param serializableObj The object to be serialized and written to the TCP socket.
    */
   void write(Serializable serializableObj) override;

   /**
    * @brief Read data from the TCP socket.
    * @return The deserialized object read from the TCP socket.
    */
   Serializable read() override;

private:
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
    * @brief Reconnect to the server.
    */
   void reconnect();

   SOCKET tcpSocket; ///< Socket handler.
   std::string localIp; ///< Local IP address.
   SOCKET serverSocket; ///< Server socket handler.
   SOCKET clientSocket; ///< Client socket handler.

   /**
    * @brief Open a thread for socket operations.
    */
   void openthread();
};

#endif // SOCKET_LIB_WINDOWSTCPSOCKET_H

#endif 
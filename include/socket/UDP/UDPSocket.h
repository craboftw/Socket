#ifndef SOCKET_LIB_UDPSOCKET_H
#define SOCKET_LIB_UDPSOCKET_H

#include <mutex>
#include <string>

#include "socket/Socket.h"

#ifdef _WIN32
#include <winsock2.h>
#else
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#endif

class UDPSocket : public Socket {
 private:
  std::string ip;
  int localPort{};
  int remotePort{};
#ifdef _WIN32
  WSADATA wsaData;
  SOCKET udpSocket;
#else
  int udpSocket;
#endif
  struct sockaddr_in localAddr {};
  struct sockaddr_in remoteAddr {};
  std::mutex socketMutex;

 public:
  UDPSocket();
  UDPSocket(const std::string& ip, int localPort, int remotePort);
  ~UDPSocket();

  void open() override;
  void close() override;
  void write(Serializable serializableObj) override;
  Serializable read() override;
  
};

#endif  // SOCKET_LIB_UDPSOCKET_H

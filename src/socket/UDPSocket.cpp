#include "socket/UDP/UDPSocket.h"

#include <chrono>
#include <iomanip>
#include <sstream>

#include "spdlog/sinks/stdout_color_sinks-inl.h"
#include "spdlog/spdlog.h"

#ifdef _WIN32
#include <windows.h>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#define SOCKET int
#define SOCKET_ERROR -1
#define INVALID_SOCKET (SOCKET)(~0)

#endif

UDPSocket::UDPSocket() : UDPSocket("", 0, 0) {}

UDPSocket::UDPSocket(const std::string &ip, int localPort, int remotePort)
    : ip(ip), localPort(localPort), remotePort(remotePort) {

#ifdef _WIN32
  if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
    throw std::runtime_error("WSAStartup falló");
  }
  udpSocket = INVALID_SOCKET;
#else
  udpSocket = -1;
#endif
  // initialize logger
}

UDPSocket::~UDPSocket() {
  close();

#ifdef _WIN32
  WSACleanup();
#endif
}

void UDPSocket::open() {
  std::lock_guard<std::mutex> lock(socketMutex);
  udpSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

  if (udpSocket == INVALID_SOCKET) {
    throw std::runtime_error("Socket creation failed; UDPSocket::open()");
  }

  localAddr.sin_family = AF_INET;
  localAddr.sin_addr.s_addr = INADDR_ANY;
  localAddr.sin_port = htons(localPort);

  remoteAddr.sin_family = AF_INET;
  remoteAddr.sin_addr.s_addr = inet_addr(ip.c_str());
  remoteAddr.sin_port = htons(remotePort);

  if (bind(udpSocket, (struct sockaddr *)&localAddr, sizeof(localAddr)) ==
      SOCKET_ERROR) {
    throw std::runtime_error("Binding failed; UDPSocket::open()");
  }
  spdlog::info("Socket opened");
}

void UDPSocket::close() {
  std::lock_guard<std::mutex> lock(socketMutex);
#ifdef _WIN32
  if (udpSocket != INVALID_SOCKET) {
    shutdown(udpSocket, SD_BOTH);
    closesocket(udpSocket);
    udpSocket = INVALID_SOCKET;
  }
#else
  if (udpSocket != -1) {
    ::close(udpSocket);
    udpSocket = -1;
  }
#endif
  spdlog::info("Socket closed");
}

void UDPSocket::write(Serializable serializableObj) {
  std::lock_guard<std::mutex> lock(socketMutex);
  std::vector<uint8_t> serializedData =
      serializableObj.operator const std::vector<uint8_t>();
  spdlog::debug("port:{0} sending data to {1}:{2}", localPort, ip, remotePort);
  int bytesSent =
      sendto(udpSocket, reinterpret_cast<const char *>(serializedData.data()),
             serializedData.size(), 0, (struct sockaddr *)&remoteAddr,
             sizeof(remoteAddr));
#ifdef _WIN32
  if (bytesSent == SOCKET_ERROR) {
#else
  if (bytesSent == -1) {
#endif
    spdlog::error("Error sending data");
    return;
  }

  if (bytesSent != static_cast<int>(serializedData.size())) {
    spdlog::error("Mismatch in sent data size");
    return;
  }

  spdlog::debug("Data sent to {0}:{1}", ip, remotePort);
  std::stringstream stream;
  stream << std::hex << std::setfill('0');

  for (uint8_t &byte : serializedData) {
    stream << std::setw(2) << static_cast<int>(byte);
    stream << " ";
  }
  spdlog::info("Data sent: " + stream.str());
}

Serializable UDPSocket::read() {
  std::lock_guard<std::mutex> lock(socketMutex);
  auto now = std::chrono::system_clock::now();
  do {
    try {
      spdlog::debug("port:{0} waiting for data from {1}:{2}", localPort, ip,
                    remotePort);
      std::vector<unsigned char> buffer(1024);
      int bytesRead = 0;

      fd_set readSet;
      FD_ZERO(&readSet);
      FD_SET(udpSocket, &readSet);

      struct timeval timeout;
      timeout.tv_sec = 1;
      timeout.tv_usec = 0;

      int ready = select(udpSocket + 1, &readSet, nullptr, nullptr, &timeout);

#ifdef _WIN32
      if (ready == SOCKET_ERROR) {
#else
      if (ready == -1) {
#endif
        throw std::runtime_error(
            "Error receiving data; connection may have been closed; "
            "UDPSocket::read()");
      }

      if (ready > 0) {
        bytesRead = recv(udpSocket, reinterpret_cast<char *>(buffer.data()),
                         buffer.size(), 0);

#ifdef _WIN32
        if (bytesRead == SOCKET_ERROR) {
#else
        if (bytesRead < 0) {
#endif
          throw std::runtime_error(
              "Error receiving data; connection may have been closed; "
              "UDPSocket::read()");
        }
      }

      if (bytesRead <= 0) {
        spdlog::debug("No data received from {0}:{1}", ip, remotePort);
        return Serializable();
      }

      buffer.resize(bytesRead);
      Serializable receivedData(buffer);
      notify(receivedData);
      spdlog::debug("Data received {0} from {1}", ip, remotePort);
      std::stringstream stream;
      stream << std::hex << std::setfill('0');
      auto serializedData = receivedData.operator const std::vector<uint8_t>();
      for (uint8_t &byte : serializedData) {
        stream << std::setw(2) << static_cast<int>(byte);
        // add space between bytes
        stream << " ";
      }
      spdlog::info("Data received: " + stream.str());
      return receivedData;
    } catch (std::exception &e) {
      spdlog::debug("Excepción capturada: {}", e.what());
    }
  } while (true);
}

#ifdef _WIN32
#include "socket/TCP/WinTCP/WindowsTCPSocket.h"
#include "spdlog/spdlog.h"
#include <chrono>
#include <future>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <thread>
#include <sstream>

WindowsTCPSocket::WindowsTCPSocket()
    : tcpSocket(INVALID_SOCKET), TCPSocket() {
}

WindowsTCPSocket::WindowsTCPSocket(const std::string &ip, int localPort, int remotePort)
    : tcpSocket(INVALID_SOCKET), TCPSocket(ip, localPort, remotePort) {
}

WindowsTCPSocket::WindowsTCPSocket(const std::string &ip, int localPort, int remotePort, mode mode, unsigned maxRetries, unsigned retryTimeout)
    : tcpSocket(INVALID_SOCKET), TCPSocket(ip, localPort, remotePort, mode, maxRetries, retryTimeout) {
}

WindowsTCPSocket::~WindowsTCPSocket() {
    close();
}

bool WindowsTCPSocket::connectWithRetries() {
    unsigned retry = 1;
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    // Connect to remote server using remoteIp and remotePort
    sockaddr_in clientAddress{};
    clientAddress.sin_family = AF_INET;
    clientAddress.sin_port = htons(remotePort);
    clientAddress.sin_addr.s_addr = inet_addr(remoteIp.c_str());
    spdlog::info("Connecting to server: {0}", remoteIp);
    while (retry <= maxRetries || maxRetries == INFINITERETRIES) {
        if (clientSocket == INVALID_SOCKET) {
            spdlog::error("Error creating socket: {0}; WindowsTCPSocket::connectWithRetries()", WSAGetLastError());
            return false;
        }

        if (connect(clientSocket, (sockaddr *) &clientAddress, sizeof(clientAddress)) != SOCKET_ERROR) {
            spdlog::info("Connected to server: {0}", remoteIp);
            return true;
        }

        spdlog::warn("Error connecting to server: {0}; WindowsTCPSocket::connectWithRetries()\nRetry number: {1}", WSAGetLastError(), retry);

        ++retry;
    }

    setsockopt(clientSocket, SOL_SOCKET, SO_RCVTIMEO,(char *)&retryTimeout,sizeof(struct timeval));


    return false;// Todos los intentos fallaron
}

void WindowsTCPSocket::openthread() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        spdlog::error("Error initializing socket library; WindowsTCPSocket::open()", nullptr);
        return;
    }

    switch (actualMode) {
        case mode::SERVER:
            spdlog::info("Server mode initialized");
            startListening();
            break;

        case mode::CLIENT:
            if (!connectWithRetries()) {
                spdlog::error("Error connecting. No mode initialized for TCP socket; WindowsTCPSocket::open()", nullptr);
                WSACleanup();
                return;
            }
            break;

        default:
            spdlog::error("Error connecting. No mode initialized for TCP socket; WindowsTCPSocket::open()", nullptr);
    }
    socketopen = true;
}

void WindowsTCPSocket::open() {
    std::thread t1(&WindowsTCPSocket::openthread, this);
    if(actualMode != mode::SERVER)
        t1.join();
    else
        t1.detach();
}


void WindowsTCPSocket::startListening() {
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        spdlog::error("Error creating socket: {0}; WindowsTCPSocket::startListening()", WSAGetLastError());
        WSACleanup();
        return;
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(localPort);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    sockaddr_in clientAddr{};
    clientAddr.sin_family = AF_INET;
    clientAddr.sin_port = htons(remotePort);
    clientAddr.sin_addr.s_addr = inet_addr(remoteIp.c_str());
    int clientAddrSize = sizeof(clientAddr);

    while (bind(serverSocket, (sockaddr *) &serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        spdlog::error("Error binding socket: {0}; WindowsTCPSocket::startListening()", WSAGetLastError());
    }

    listen(serverSocket, SOMAXCONN);

    spdlog::info("Waiting for connections...");

    clientSocket = INVALID_SOCKET;

    setsockopt(clientSocket, SOL_SOCKET, SO_RCVTIMEO,(char *)&retryTimeout,sizeof(struct timeval));


    clientSocket = accept(serverSocket, (sockaddr *) &clientAddr, &clientAddrSize);

    while (clientSocket == INVALID_SOCKET) {
        spdlog::error("Error accepting connection: {0}; WindowsTCPSocket::startListening()", WSAGetLastError());
        clientSocket = accept(serverSocket, (sockaddr *) &clientAddr, &clientAddrSize);
    }
    spdlog::info("Client connected: {0}", inet_ntoa(clientAddr.sin_addr));

}

void WindowsTCPSocket::close() {
    spdlog::info("Closing socket {0}:{1}", remoteIp, remotePort);
    closesocket(clientSocket);
    closesocket(serverSocket);
    socketopen = false;
}

void WindowsTCPSocket::write(Serializable serializableObj) {
    auto now = std::chrono::system_clock::now();
    unsigned retry = 1;
    do {
        auto elapsedMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - now).count();
        if (elapsedMilliseconds > (retryTimeout.tv_sec * 1000 + retryTimeout.tv_usec / 1000)) { // Convertir segundos y microsegundos a milisegundos
            spdlog::error("Timeout while sending data. Stop sending data; WindowsTCPSocket::write()", nullptr);
            return;
        }
        spdlog::debug("Sending data to {0}:{1}, retry number: {2}", remoteIp, remotePort, retry++);
        try {
            //start chrono
            if (clientSocket == INVALID_SOCKET) {
                throw std::runtime_error("Socket is not open; WindowsTCPSocket::write()");
            }

            //Serialize object and send it
            std::vector<uint8_t> serializedData = serializableObj.operator const std::vector<uint8_t>();
            int bytesSent = send(clientSocket, reinterpret_cast<const char *>(serializedData.data()), static_cast<int>(serializedData.size()), 0);

            if (bytesSent == SOCKET_ERROR) {//Error sending data
                throw std::runtime_error("Error sending data; WindowsTCPSocket::write()");
            }
            spdlog::info("Data sent to {0}:{1}", remoteIp, remotePort);
            if(spdlog::get_level() == spdlog::level::debug) {
                //create a string of the the data and print it
                std::stringstream stream{};
                for (auto &byte: serializedData) {
                    stream << std::setfill('0') << std::setw(2) << std::hex << byte;
                }
                spdlog::debug("Data sent: " + stream.str());
                
            }
            return;
        } catch (const std::exception &e) {
            spdlog::error("Exception caught: {0}", e.what());
            //reconnect();
        }
    } while (true);
}

Serializable WindowsTCPSocket::read() {
    auto now = std::chrono::system_clock::now();
    unsigned retry = 1;
    do {
        auto elapsedMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - now).count();
        if (elapsedMilliseconds > (retryTimeout.tv_sec * 1000 + retryTimeout.tv_usec / 1000)) {
            spdlog::error("Timeout while receiving data. Stop receiving data; WindowsTCPSocket::read()", nullptr);
            return Serializable(); // Return an empty Serializable object
        }

        spdlog::debug("Waiting for data from {0}:{1}, retry number: {2}", remoteIp, remotePort, retry++);
        try {
            if (clientSocket == INVALID_SOCKET) {
                throw std::runtime_error("Socket is not open; WindowsTCPSocket::read()");
            }

            fd_set readSet;
            FD_ZERO(&readSet);
            FD_SET(clientSocket, &readSet);

            timeval timeout = retryTimeout;


            int selectResult = select(clientSocket + 1, &readSet, nullptr, nullptr, &timeout);
            if (selectResult == -1) {
                spdlog::error("Error in select(); WindowsTCPSocket::read()"); 
                throw std::runtime_error("Error in select(); LinuxTCPSocket::read()");
            } else if (selectResult == 0) {
                // Timeout occurred
                spdlog::error("Timeout while waiting for data; LinuxTCPSocket::read()", nullptr);
                return Serializable{};// Return empty Serializable object
            }

            // Receive data into a buffer
            const int bufferSize = 1024; // You can adjust the buffer size
            std::vector<uint8_t> receiveBuffer(bufferSize);
            int bytesRead = recv(clientSocket, reinterpret_cast<char *>(receiveBuffer.data()), bufferSize, 0);

            if (bytesRead == SOCKET_ERROR || bytesRead == 0) {
                throw std::runtime_error("Error receiving data; WindowsTCPSocket::read()");
            }

            // Resize the buffer to match the number of received bytes
            receiveBuffer.resize(bytesRead);

            // Create and return a Serializable object with the received data
            notify(Serializable(receiveBuffer));
            /* spdlog::info("Data received from {0}:{1}", remoteIp, remotePort);
            if (spdlog::get_level() == spdlog::level::debug) {
                //create a string of the the data and print it
                std::stringstream stream;

                for (auto &byte: receiveBuffer) {
                    stream << std::setfill('0') << std::setw(2) << std::hex << byte;
                }
                spdlog::debug("Data received: " + stream.str());
            }
            */
            return Serializable(receiveBuffer);
        } catch (const std::exception &e) {
            spdlog::error("Exception caught: {0}", e.what());
            //reconnect();
        }
    } while (true);
}



void WindowsTCPSocket::reconnect() {
    if (!socketopen) {
        return;
    } else {
        close();
        open();
    }
}

#endif


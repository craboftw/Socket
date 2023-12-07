#include "LinuxTCPSocket.h"
#include <arpa/inet.h>
#include <chrono>
#include <csignal>
#include <spdlog/spdlog.h>
#include <thread>
#include <future>
#include <iostream>
#include <stdexcept>
#include <unistd.h>

LinuxTCPSocket::~LinuxTCPSocket() {
    close();
}

bool LinuxTCPSocket::connectWithRetries() {
    unsigned retry = 1;
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (clientSocket == -1) {
        spdlog::error("Error creating socket: {0}; LinuxTCPSocket::connectWithRetries()", strerror(errno));
        return false;
    }

    sockaddr_in clientAddress{};
    clientAddress.sin_family = AF_INET;
    clientAddress.sin_port = htons(remotePort);
    clientAddress.sin_addr.s_addr = inet_addr(remoteIp.c_str());
    spdlog::info("Connecting to server: {0}", remoteIp);

    while (retry <= maxRetries || maxRetries == INFINITERETRIES) {
        if (connect(clientSocket, reinterpret_cast<sockaddr*>(&clientAddress), sizeof(clientAddress)) != -1) {
            spdlog::info("Connected to server: {0}", remoteIp);
            return true;
        }

        spdlog::warn("Error connecting to server: {0}; LinuxTCPSocket::connectWithRetries()\nRetry number: {1}", strerror(errno), retry);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        ++retry;
    }

    struct timeval tv = retryTimeout;
    setsockopt(clientSocket, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<const char*>(&tv), sizeof(struct timeval));

    return false; // All attempts failed
}


void LinuxTCPSocket::openthread() {
    switch (actualMode) {
        case mode::SERVER:
            spdlog::info("Server mode initialized");
            startListening();
            break;

        case mode::CLIENT:
            if (!connectWithRetries()) {
                spdlog::error("Error connecting. No mode initialized for TCP socket; LinuxTCPSocket::openthread()", nullptr);
            }
            break;
        default:
            spdlog::error("Error connecting. No mode initialized for TCP socket; LinuxTCPSocket::openthread()", nullptr);
    }
    socketopen = true;
}

void LinuxTCPSocket::open() {
    std::thread t1(&LinuxTCPSocket::openthread, this);
    if (actualMode != mode::SERVER) {
        t1.join();
    } else {
        t1.detach();
    }
}

void LinuxTCPSocket::close() {
    if (clientSocket != -1) {
        ::shutdown(clientSocket, SHUT_RDWR);
        ::close(clientSocket);
        clientSocket = -1;
    }
    if (serverSocket != -1) {
        ::shutdown(serverSocket, SHUT_RDWR);
        ::close(serverSocket);
        serverSocket = -1;
    }
    socketopen = false;
}

void LinuxTCPSocket::startListening() {
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        spdlog::error("Error creating socket: {0}; LinuxTCPSocket::startListening()", strerror(errno));
        return;
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(localPort);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) == -1) {
        spdlog::error("Error binding socket: {0}; LinuxTCPSocket::startListening()", strerror(errno));
        return;
    }

    listen(serverSocket, SOMAXCONN);

    spdlog::info("Waiting for connections...");

    int clientAddrSize = sizeof(sockaddr_in);
    clientSocket = accept(serverSocket, nullptr, reinterpret_cast<socklen_t*>(&clientAddrSize));

    if (clientSocket == -1) {
        spdlog::error("Error accepting connection: {0}; LinuxTCPSocket::startListening()", strerror(errno));
        return;
    }
    sockaddr_in clientAddr{};
    socklen_t clientAddrLen = sizeof(clientAddr);
    getpeername(clientSocket, reinterpret_cast<sockaddr*>(&clientAddr), &clientAddrLen);
    spdlog::info("Client connected: {0}", inet_ntoa(clientAddr.sin_addr));
}

void LinuxTCPSocket::write( Serializable serializableObj) {
    if (!isConnected()) {
        spdlog::error("Socket not connected; LinuxTCPSocket::write()");
        return;
    }
    auto now = std::chrono::system_clock::now();
    unsigned retry = 1;

    while (true) {
        auto elapsedMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - now).count();
        if (elapsedMilliseconds > (retryTimeout.tv_sec * 1000 + retryTimeout.tv_usec / 1000)) {
            spdlog::error("Timeout while sending data. Stop sending data; LinuxTCPSocket::write()", nullptr);
            return;
        }

        spdlog::debug("Sending data to {0}:{1}, retry number: {2}", remoteIp, remotePort, retry);

        try {
            if (clientSocket == -1) {
                throw std::runtime_error("Socket is not open; LinuxTCPSocket::write()");
            }

            const std::vector<uint8_t>& serializedData = serializableObj.operator const std::vector<uint8_t>();
            int bytesSent = send(clientSocket, reinterpret_cast<const char*>(serializedData.data()), static_cast<int>(serializedData.size()), 0);

            if (bytesSent == -1) {
                throw std::runtime_error("Error sending data; LinuxTCPSocket::write()");
            }
            return;
        } catch (const std::exception& e) {
            spdlog::error("Exception caught: {0}", e.what());
            std::this_thread::sleep_for(std::chrono::seconds(1));
            reconnect();
        }
        ++retry;
    }
}

Serializable LinuxTCPSocket::read() {
    auto now = std::chrono::system_clock::now();
    unsigned retry = 1;

    while (true) {
        auto elapsedMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - now).count();
        if (elapsedMilliseconds > (retryTimeout.tv_sec * 1000 + retryTimeout.tv_usec / 1000)) {
            spdlog::error("Timeout while receiving data. Stop receiving data; LinuxTCPSocket::read()", nullptr);
            return Serializable(); // Return an empty Serializable object
        }

        spdlog::debug("Receiving data from {0}:{1}, retry number: {2}", remoteIp, remotePort, retry);

        try {
            if (clientSocket == -1) {
                throw std::runtime_error("Socket is not open; LinuxTCPSocket::read()");
            }

            fd_set readSet;
            FD_ZERO(&readSet);
            FD_SET(clientSocket, &readSet);

            timeval timeout = retryTimeout;

            int selectResult = select(clientSocket + 1, &readSet, nullptr, nullptr, &timeout);
            if (selectResult == -1) {
                spdlog::error("Error in select(); LinuxTCPSocket::read()", nullptr);
                throw std::runtime_error("Error in select(); LinuxTCPSocket::read()");
            } else if (selectResult == 0) {
                // Timeout occurred
                spdlog::error("Timeout while waiting for data; LinuxTCPSocket::read()", nullptr);
                return Serializable{}; // Return empty Serializable object
            }

            const int bufferSize = 1024;
            std::vector<uint8_t> receiveBuffer(bufferSize);
            int bytesRead = recv(clientSocket, reinterpret_cast<char*>(receiveBuffer.data()), bufferSize, 0);

            if (bytesRead == -1 || bytesRead == 0) {
                throw std::runtime_error("Error receiving data; LinuxTCPSocket::read()");
            }

            receiveBuffer.resize(bytesRead);
            notify(Serializable(receiveBuffer));
            return Serializable(receiveBuffer);
        } catch (const std::exception& e) {
            spdlog::error("Exception caught: {0}", e.what());
            //reconnect();
        }
        ++retry;
    }
}

void LinuxTCPSocket::reconnect() {
    unsigned retry = 1;
    while(retry <= maxRetries || maxRetries == INFINITERETRIES) {
        if (!socketopen) {
            return;
        } else {
            close();
            try {
                open();
                return; // Successful reconnection, exit method
            } catch (const std::exception& e) {
                spdlog::warn("Reconnection attempt {0} failed: {1}", retry, e.what());
                std::this_thread::sleep_for(std::chrono::seconds(1));
                ++retry;
            }
        }
    }

    if(retry > maxRetries && maxRetries != INFINITERETRIES) {
        throw std::runtime_error("Max number of reconnection attempts reached.");
    }
}

bool LinuxTCPSocket::isConnected() {
    int error = 0;
    socklen_t len = sizeof(error);
    int retval = getsockopt(clientSocket, SOL_SOCKET, SO_ERROR, &error, &len);

    return (retval == 0 && error == 0);
}

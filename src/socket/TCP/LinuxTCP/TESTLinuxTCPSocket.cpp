#include "LinuxTCPSocket.h"
#include "socket/TestSocket.h"
#include "spdlog/spdlog.h"
#include <gtest/gtest.h>
#include <thread>

TestSocket::TestSocket() {

 }

int TestSocket::getRandomPort() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(10000, 60000);
    return distrib(gen);
}

void TestSocket::SetUp() {
    int port1 = getRandomPort();
    int port2 = getRandomPort();
    while(port1 == port2) { // Asegurar que no sean iguales
        port2 = getRandomPort();
    }
    
    socket1 = new LinuxTCPSocket("127.0.0.1", port1, port2, TCPSocket::mode::SERVER, INFINITERETRIES, 10);
    socket2 = new LinuxTCPSocket("127.0.0.1", port2, port1, TCPSocket::mode::CLIENT, INFINITERETRIES, 10);
    
    socket1->open();
    socket2->open();
    
    //set spdlog level to debug
    spdlog::set_level(spdlog::level::debug);
}

void TestSocket::TearDown() {
    socket1->close();
    socket2->close();
    delete socket1;
    delete socket2;
    socket1 = nullptr;
    socket2 = nullptr;
    
    std::this_thread::sleep_for(std::chrono::milliseconds(500)); // Esperando medio segundo
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

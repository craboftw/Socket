#ifdef _WIN32
#include "socket/TestSocket.h"
#include "WindowsTCPSocket.h"
#include <gtest/gtest.h>
#include <spdlog/spdlog.h>
#include <thread>

TestSocket::TestSocket() {
    spdlog::set_level(spdlog::level::debug);
    socket1 = new WindowsTCPSocket("127.0.0.1", 5002, 5003, TCPSocket::mode::SERVER, INFINITERETRIES, 3);
    socket2 = new WindowsTCPSocket("127.0.0.1", 5003, 5002, TCPSocket::mode::CLIENT, INFINITERETRIES, 3);
    socket1->open();
    socket2->open();
}

void TestSocket::SetUp() {

}

void TestSocket::TearDown() {
    //delete
    delete socket1;
    delete socket2;
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

#endif
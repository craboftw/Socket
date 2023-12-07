/**
 * @file SerialSocket.h
 * @brief Contains the SerialSocket class declaration.
 * @date 2023-08-14
 */

#ifndef SOCKET_LIB_SERIALSOCKET_H
#define SOCKET_LIB_SERIALSOCKET_H

#ifdef _WIN32
#include <WinSock2.h>
#include <windows.h>
#else
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#endif

#include <utility>

#include "socket/Socket.h"

/**
 * @class SerialSocket
 * @brief Represents a socket for serial communication.
 *
 * This class inherits from the Socket class and provides functionalities
 * for serial communication over a specified serial port.
 */
class SerialSocket : public Socket {
 public:
  /**
   * @brief Default constructor.
   *
   * Creates a SerialSocket object with default parameters.
   */
  SerialSocket() = default;

  /*
   * @brief Parameterized constructor.
   *
   * Creates a SerialSocket object with the specified serial port name.
   * baud rate is set 115200, data bits is set 8, stop bits is set 1, parity is
   * set 0.
   * @param portName The name of the serial port to communicate with.
   */
  SerialSocket(const std::string &portName);

  /**
   * @brief Parameterized constructor.
   *
   * Creates a SerialSocket object with the specified communication parameters.
   *
   * @param portName The name of the serial port to communicate with.
   * @param baudRate The baud rate for serial communication.
   * @param dataBits The number of data bits per frame.
   * @param stopBits The number of stop bits per frame.
   * @param parity The parity setting for error checking.
   */
  SerialSocket(std::string portName, int baudRate, int dataBits, int stopBits,
               int parity);
  void open() override;
  void close() override;
  void write(Serializable serializableObj) override;
  Serializable read() override;

  ~SerialSocket();

 protected:
  std::string portName;  ///< The name of the serial port.
  int baudRate;          ///< The baud rate for serial communication.
  int dataBits;          ///< The number of data bits per frame.
  int stopBits;          ///< The number of stop bits per frame.
  int parity;            ///< The parity setting for error checking.
#ifdef _WIN32
  HANDLE hSerial;  ///< Handle to the serial port.
  COMSTAT status;  ///< Serial port communication status.
  DWORD m_errors;  ///< Error status during communication.
#else
  int serialPort{};  ///< File descriptor for the serial port.
#endif
  std::mutex mtx;  ///< Mutex for thread safety.   #else
};

#endif  // SOCKET_LIB_SERIALSOCKET_H

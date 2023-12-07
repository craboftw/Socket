#include "socket/Serial/SerialSocket.h"

#include <iomanip>
#include <sstream>

#include "spdlog/spdlog.h"
// to use the serial port in linux

SerialSocket::SerialSocket(std::string portName, int baudRate, int dataBits,
                           int stopBits, int parity)
    : portName(portName),
      baudRate(baudRate),
      dataBits(dataBits),
      stopBits(stopBits),
      parity(parity) {}

SerialSocket::~SerialSocket() {
  close();
  spdlog::shutdown();
}

void SerialSocket::open() {
  std::lock_guard<std::mutex> lock(mtx);
  spdlog::info("Opening serial port...");
#ifdef _WIN32
  // Try to connect to the given port through CreateFile
  hSerial = CreateFile(portName.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL,
                       OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

  if (hSerial == INVALID_HANDLE_VALUE) {
    if (GetLastError() == ERROR_FILE_NOT_FOUND) {
      spdlog::error("Serial port does not exist; SerialSocket::open()",
                    nullptr);
    } else {
      spdlog::error("Error opening serial port; SerialSocket::open()", nullptr);
    }
    return;
  }

  // Set parameters of the serial port
  DCB dcbSerialParams{};
  dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
  if (!GetCommState(hSerial, &dcbSerialParams)) {
    spdlog::error("Error getting serial port state; SerialSocket::open()",
                  nullptr);
    return;
  }

  switch (baudRate) {
    case 110:
      dcbSerialParams.BaudRate = CBR_110;
      break;
    case 300:
      dcbSerialParams.BaudRate = CBR_300;
      break;
    case 600:
      dcbSerialParams.BaudRate = CBR_600;
      break;
    case 1200:
      dcbSerialParams.BaudRate = CBR_1200;
      break;
    case 2400:
      dcbSerialParams.BaudRate = CBR_2400;
      break;
    case 4800:
      dcbSerialParams.BaudRate = CBR_4800;
      break;
    case 9600:
      dcbSerialParams.BaudRate = CBR_9600;
      break;
    case 14400:
      dcbSerialParams.BaudRate = CBR_14400;
      break;
    case 19200:
      dcbSerialParams.BaudRate = CBR_19200;
      break;
    case 38400:
      dcbSerialParams.BaudRate = CBR_38400;
      break;
    case 56000:
      dcbSerialParams.BaudRate = CBR_56000;
      break;
    case 57600:
      dcbSerialParams.BaudRate = CBR_57600;
      break;
    case 115200:
      dcbSerialParams.BaudRate = CBR_115200;
      break;
    case 128000:
      dcbSerialParams.BaudRate = CBR_128000;
      break;
    case 256000:
      dcbSerialParams.BaudRate = CBR_256000;
      break;
    default:
      spdlog::error("Invalid baud rate; SerialSocket::open()", nullptr);
      break;
  }

  switch (dataBits) {
    case 5:
      dcbSerialParams.ByteSize = 5;
      break;
    case 6:
      dcbSerialParams.ByteSize = 6;
      break;
    case 7:
      dcbSerialParams.ByteSize = 7;
      break;
    case 8:
      dcbSerialParams.ByteSize = 8;
      break;
    case 16:
      dcbSerialParams.ByteSize = 16;
      break;
    case 17:  // Si deseas tener una opción para 16X (por ejemplo)
      dcbSerialParams.ByteSize = DATABITS_16X;
      break;
    default:
      spdlog::error("Invalid data bits; SerialSocket::open()", nullptr);
      break;
  }

  switch (stopBits) {
    case 1:
      dcbSerialParams.StopBits = ONESTOPBIT;
      break;
    case 2:
      dcbSerialParams.StopBits = ONE5STOPBITS;
      break;
    case 3:
      dcbSerialParams.StopBits = TWOSTOPBITS;
    default:
      spdlog::error("Invalid stop bits; SerialSocket::open()", nullptr);
      break;
  }

  switch (parity) {
    case 0:
      dcbSerialParams.Parity = NOPARITY;
      break;
    case 1:
      dcbSerialParams.Parity = ODDPARITY;
      break;
    case 2:
      dcbSerialParams.Parity = EVENPARITY;
      break;
    case 3:
      dcbSerialParams.Parity = MARKPARITY;
      break;
    case 4:
      dcbSerialParams.Parity = SPACEPARITY;
      break;
    default:
      spdlog::error("Invalid parity; SerialSocket::open()", nullptr);
      break;
  }
  dcbSerialParams.fOutxDsrFlow = false;
  dcbSerialParams.fOutxCtsFlow = false;

  if (!SetCommState(hSerial, &dcbSerialParams)) {
    spdlog::error("Error setting serial port state; SerialSocket::open() {}",
                  GetLastError());
    return;
  }

  // Set timeouts
  COMMTIMEOUTS timeouts{};
  timeouts.ReadIntervalTimeout = 50;
  timeouts.ReadTotalTimeoutConstant = 50;
  timeouts.ReadTotalTimeoutMultiplier = 10;
  timeouts.WriteTotalTimeoutConstant = 50;
  timeouts.WriteTotalTimeoutMultiplier = 10;

  if (!SetCommTimeouts(hSerial, &timeouts)) {
    spdlog::error("Error setting serial port timeouts; SerialSocket::open()",
                  nullptr);
    return;
  }
#else
  serialPort = ::open(portName.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);
  if (serialPort == -1) {
    spdlog::error("Error opening serial port; LinuxSerialSocket::open()",
                  nullptr);
    throw std::runtime_error(
        "Error opening serial port; LinuxSerialSocket::open()");
  }

  // Configure serial port
  termios serialPortConfig{};
  if (tcgetattr(serialPort, &serialPortConfig) != 0) {
    spdlog::error(
        "Error getting serial port attributes; LinuxSerialSocket::open()",
        nullptr);
    throw std::runtime_error(
        "Error getting serial port attributes; LinuxSerialSocket::open()");
  }

  // Set baud rate
  speed_t baudRateConstant;

  switch (baudRate) {
    case 50:
      baudRateConstant = B50;
      break;
    case 75:
      baudRateConstant = B75;
      break;
    case 110:
      baudRateConstant = B110;
      break;
    case 134:
      baudRateConstant = B134;
      break;
    case 150:
      baudRateConstant = B150;
      break;
    case 200:
      baudRateConstant = B200;
      break;
    case 300:
      baudRateConstant = B300;
      break;
    case 600:
      baudRateConstant = B600;
      break;
    case 1200:
      baudRateConstant = B1200;
      break;
    case 1800:
      baudRateConstant = B1800;
      break;
    case 2400:
      baudRateConstant = B2400;
      break;
    case 4800:
      baudRateConstant = B4800;
      break;
    case 9600:
      baudRateConstant = B9600;
      break;
    case 19200:
      baudRateConstant = B19200;
      break;
    case 38400:
      baudRateConstant = B38400;
      break;
    case 57600:
      baudRateConstant = B57600;
      break;
    case 115200:
      baudRateConstant = B115200;
      break;
    case 230400:
      baudRateConstant = B230400;
      break;
    case 460800:
      baudRateConstant = B460800;
      break;
    case 500000:
      baudRateConstant = B500000;
      break;
    case 576000:
      baudRateConstant = B576000;
      break;
    case 921600:
      baudRateConstant = B921600;
      break;
    case 1000000:
      baudRateConstant = B1000000;
      break;
    case 1152000:
      baudRateConstant = B1152000;
      break;
    case 1500000:
      baudRateConstant = B1500000;
      break;
    case 2000000:
      baudRateConstant = B2000000;
      break;
    case 2500000:
      baudRateConstant = B2500000;
      break;
    case 3000000:
      baudRateConstant = B3000000;
      break;
    case 3500000:
      baudRateConstant = B3500000;
      break;
    case 4000000:
      baudRateConstant = B4000000;
      break;
    default:
      spdlog::error("Invalid baud rate; LinuxSerialSocket::open()", nullptr);
      throw std::runtime_error("Invalid baud rate; LinuxSerialSocket::open()");
  }

  if (cfsetispeed(&serialPortConfig, baudRateConstant) != 0 ||
      cfsetospeed(&serialPortConfig, baudRateConstant) != 0) {
    spdlog::error("Error setting baud rate; LinuxSerialSocket::open()",
                  nullptr);
    throw std::runtime_error(
        "Error setting baud rate; LinuxSerialSocket::open()");
  }
  // Set data bits
  serialPortConfig.c_cflag &= ~CSIZE;
  switch (dataBits) {
    case 5:
      serialPortConfig.c_cflag |= CS5;
      break;
    case 6:
      serialPortConfig.c_cflag |= CS6;
      break;
    case 7:
      serialPortConfig.c_cflag |= CS7;
      break;
    case 8:
      serialPortConfig.c_cflag |= CS8;
      break;
    default:
      spdlog::error("Invalid data bits; LinuxSerialSocket::open()", nullptr);
      throw std::runtime_error("Invalid data bits; LinuxSerialSocket::open()");
  }

  // Set parity
  switch (parity) {
    case 0:
      serialPortConfig.c_cflag &= ~PARENB;
      break;
    case 1:
      serialPortConfig.c_cflag |= PARENB;
      serialPortConfig.c_cflag &= ~PARODD;
      break;
    case 2:
      serialPortConfig.c_cflag |= PARENB;
      serialPortConfig.c_cflag |= PARODD;
      break;
    default:
      spdlog::error("Invalid parity; LinuxSerialSocket::open()", nullptr);
      throw std::runtime_error("Invalid parity; LinuxSerialSocket::open()");
  }

  // Set stop bits
  switch (stopBits) {
    case 1:
      serialPortConfig.c_cflag &= ~CSTOPB;
      break;
    case 2:
      serialPortConfig.c_cflag |= CSTOPB;
      break;
    default:
      spdlog::error("Invalid stop bits; LinuxSerialSocket::open()", nullptr);
      throw std::runtime_error("Invalid stop bits; LinuxSerialSocket::open()");
  }

  // Set flow control
  serialPortConfig.c_cflag &= ~CRTSCTS;
  serialPortConfig.c_iflag &= ~(IXON | IXOFF | IXANY);

  // Set raw input
  serialPortConfig.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);

  // Set raw output
  serialPortConfig.c_oflag &= ~OPOST;

  // Set read timeouts
  serialPortConfig.c_cc[VMIN] = 0;
  serialPortConfig.c_cc[VTIME] = 0;

  // Apply configuration
  if (tcsetattr(serialPort, TCSANOW, &serialPortConfig) != 0) {
    spdlog::error(
        "Error applying serial port configuration; LinuxSerialSocket::open()",
        nullptr);
    throw std::runtime_error(
        "Error applying serial port configuration; LinuxSerialSocket::open()");
  }
  // Flush serial port
  tcflush(serialPort, TCIOFLUSH);
#endif
  spdlog::info("Serial port opened successfully");
}

void SerialSocket::close() {
  std::lock_guard<std::mutex> lock(mtx);
  spdlog::info("Closing serial port");
#ifdef _WIN32
  if (hSerial != INVALID_HANDLE_VALUE) {
    // To close a serial port, simply close the file handle
    CloseHandle(hSerial);
    hSerial = INVALID_HANDLE_VALUE;
  }
#else
  if (serialPort != -1) {
    ::close(serialPort);
    serialPort = -1;
  }
#endif
  spdlog::info("Serial port closed");
}

void SerialSocket::write(Serializable serializable) {
  std::lock_guard<std::mutex> lock(mtx);
  std::vector<uint8_t> data = static_cast<std::vector<uint8_t>>(serializable);

#ifdef _WIN32
  if (hSerial == INVALID_HANDLE_VALUE) {
    spdlog::error("Serial port is not open; SerialSocket::write()", nullptr);
    return;
  }

  DWORD bytesWritten;
  if (!WriteFile(hSerial, data.data(), data.size(), &bytesWritten, NULL)) {
    spdlog::error("Error writing to serial port: {0}", GetLastError());
    return;
  }
  spdlog::info("Data sent to {0}", portName);
#else

  ssize_t bytesSent = ::write(serialPort, data.data(), data.size());
  if (bytesSent != static_cast<ssize_t>(data.size())) {
    spdlog::error("Error sending data; LinuxSerialSocket::write()", nullptr);
    throw std::runtime_error("Error sending data; LinuxSerialSocket::write()");
  }
#endif

  if (spdlog::get_level() == spdlog::level::debug) {
    // create a string of the the data and print it
    std::stringstream stream;

    for (auto &byte : data) {
      stream << std::setfill('0') << std::setw(2) << std::hex << byte;
    }
    spdlog::debug("Data sent: " + stream.str());
  }
}

Serializable SerialSocket::read() {
  std::lock_guard<std::mutex> lock(mtx);
#ifdef _WIN32

  if (hSerial == INVALID_HANDLE_VALUE) {
    spdlog::error("Serial port is not open; SerialSocket::read()", nullptr);
    return {};
  }

  DWORD bytesRead{};
  DWORD toRead = 0;
  const DWORD chunkSize = 1024;  // Read data in 8-byte chunks

  spdlog::info("Waiting for data from {0}", portName);
  ClearCommError(hSerial, &m_errors, &status);

  if (this->status.cbInQue > 0) {
    if (this->status.cbInQue > chunkSize) {
      toRead = chunkSize;
    } else {
      toRead = this->status.cbInQue;
    }
  }

  if (toRead == 0) {
    spdlog::info("No data to read from {0}", portName);
    return {};  // Nothing to read
  }

  std::vector<uint8_t> buffer(toRead);

  if (!ReadFile(hSerial, buffer.data(), toRead, &bytesRead, NULL)) {
    spdlog::error("Error reading from serial port: {0}", GetLastError());
    return {};
  }
  spdlog::info("Read {0} bytes from {1}", bytesRead, portName);
  if (spdlog::get_level() == spdlog::level::debug) {
    // create a string of the the data and print it
    std::stringstream stream;

    for (auto &byte : buffer) {
      stream << std::setfill('0') << std::setw(2) << std::hex << byte;
    }
    spdlog::debug("Data received: " + stream.str());
  }
#else
  std::vector<uint8_t> data;
  data.resize(1024);
  ssize_t bytesRead = ::read(serialPort, data.data(), data.size());
  if (bytesRead == -1) {
    spdlog::error("Error reading data; LinuxSerialSocket::read()", nullptr);
    throw std::runtime_error("Error reading data; LinuxSerialSocket::read()");
  }
  // Flush serial port
  tcflush(serialPort, TCIOFLUSH);

  data.resize(bytesRead);
  spdlog::info("Data received from serial port {0} ,{1} bytes received.",
               serialPort, bytesRead);
  if (spdlog::get_level() == spdlog::level::debug) {
    // create a string of the the data and print it
    std::stringstream stream;

    for (auto &byte : data) {
      stream << std::setfill('0') << std::setw(2) << std::hex << byte;
    }
    spdlog::debug("Data received: " + stream.str());
  }
  Serializable buffer(data);

#endif
  notify(Serializable(buffer));
  return Serializable(buffer);
}

SerialSocket::SerialSocket(const std::string &portName)
    : SerialSocket(portName, 115200, 8, 1, 0) {}

/**
 * @file Socket.h
 * @brief Contains the Socket interface.
 * @author Francisco López Guerrero < francisco.lopez@gtd.eu >
 */

#ifndef SOCKET_LIB_SOCKET_H
#define SOCKET_LIB_SOCKET_H

#include <spdlog/common.h>
#include <spdlog/spdlog.h>

#include "observer/EventListener.h"
#include "serializable/Serializable.h"

/**
 * @class Socket
 * @brief Abstract base class for socket communication.
 *
 * This class defines the basic interface for socket communication operations.
 */
class Socket : public EventListener {
 public:
  /**
   * @brief Default constructor.
   */
  Socket() = default;

  /**
   * @brief Virtual destructor.
   */
  virtual ~Socket() = default;

  /**
   * @brief Reads data from the socket.
   * @return The deserialized object read from the socket.
   */
  virtual Serializable read() = 0;

  /**
   * @brief Writes data to the socket.
   * @param serializableObj The object to be serialized and written to the
   * socket.
   */
  virtual void write(Serializable serializableObj) = 0;

  /**
   * @brief Opens the socket for communication.
   *
   * The specific implementation will establish the necessary connection.
   * The socket is not ready for communication until this method is called.
   */
  virtual void open() = 0;

  /**
   * @brief Closes the socket.
   *
   * The specific implementation will close the established connection.
   * The socket is not ready for communication after this method is called.
   * The destructor will call this method if it has not been called before.
   */
  virtual void close() = 0;

  inline void setLevelspdlog(spdlog::level::level_enum level) {
    spdlog::set_level(level);
  }

 protected:
  // logger
  std::shared_ptr<spdlog::logger> logger;
};

#endif  // SOCKET_LIB_SOCKET_H

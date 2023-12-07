#include <iostream>
#include <thread>  // Para std::this_thread::sleep_for

#include "factory/FactorySocket.h"
#include "socket/Socket.h"
#include "socket/TCP/TCPSocket.h"

static std::unique_ptr<Socket> socketComm;

void chatRecibir() {

  Serializable mensaje;
  while (true) {
    mensaje = socketComm->read();
    if (mensaje.size() > 0)
      std::cout << "Recibido: " << mensaje << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
  }
}

void chatEscribir() {
  std::string mensaje;
  Serializable mensajeSerialized;
  while (true) {
    std::getline(std::cin, mensaje);
    std::cout << "Enviando: " << mensajeSerialized << std::endl;
    if (mensaje == "exit") {
      break;
    }
    if (mensaje == "clear") {
      system("cls");
      // continue is used to skip the rest of the loop and start again
      continue;
    }
    // if the message is only a jump line, it will be ignored
    if (mensaje == "\n") {
      continue;
    }
    std::vector<uint8_t> mensajeVector(mensaje.begin(), mensaje.end());
    mensajeSerialized = Serializable(mensajeVector);
    socketComm->write(mensajeSerialized);
    mensaje = "";
  }
}

int main() {
  std::cout << "Elija el tipo de socketComm que desea crear:\n";
  std::cout << "1. Serial\n";
  std::cout << "2. TCP\n";
  std::cout << "3. UDP\n";
  std::cout << "4. Salir\n";
  std::string argumento = "";
  int argumento1 = 0;
  int argumento2 = 0;
  int argumento3 = 0;
  int argumento4 = 0;
  int opcion;
  TCPSocket::mode modo;
  std::cin >> opcion;
  while (opcion < 1 || opcion > 4 and opcion != 2) {
    std::cin >> opcion;
  }

  switch (opcion) {
    case 1:
      std::cout << "Ingrese el puerto local del socket\n";
      std::cin >> argumento;
// Argumento sera COM10, pero socket recibe \\.\COM10. Argumento es un string0
#ifdef _WIN32
      argumento = "\\\\.\\" + argumento;
#endif
      std::cout << "Ingrese la velocidad del socket\n";
      std::cin >> argumento1;
      std::cout << "Ingrese los bits de datos del socket\n";
      std::cin >> argumento2;
      std::cout << "Ingrese los bits de parada del socket. 1 = 1, 2 = 2, 3 = "
                   "1.5\n";
      std::cin >> argumento3;
      std::cout << "Ingrese el bit de paridad del socket. 0 = none, 1 = odd, 2 "
                   "= even\n 3 = mark, 4 = space\n";
      std::cin >> argumento4;
      socketComm = FactorySocket::createSerialSocket(argumento, argumento1, argumento2, argumento3, argumento4);
      break;
    case 2:
      // Wipmode
      std::cout << "Ingrese la ip local del socket\n";
      std::cout << "Ingrese el puerto local del socket\n";
      std::cin >> argumento1;
      std::cout << "Ingrese la ip remota del socket\n";
      std::cin >> argumento;
      std::cout << "Ingrese el puerto remoto del socket\n";
      std::cin >> argumento2;
      std::cout << "Ingrese el modo del socket\n";
      std::cout << "1. Cliente\n2. Servidor\n";
      std::cin >> argumento3;
      if (argumento3 == 1) {
        modo = TCPSocket::mode::CLIENT;
      } else {
        modo = TCPSocket::mode::SERVER;
      }
      // socketComm =
      // FactorySocket::createTCPSocket(argumento,argumento1,argumento2,modo,INFINITERETRIES,100);
      break;
    case 3:
      std::cout << "Ingrese el puerto local del socket\n";
      std::cin >> argumento1;
      std::cout << "Ingrese la ip remota del socket\n";
      std::cin.ignore();
      std::getline(std::cin, argumento);
      std::cout << "Ingrese el puerto remoto del socket\n";
      std::cin >> argumento2;
      socketComm =
          FactorySocket::createUDPSocket(argumento, argumento1, argumento2);
      break;
  }
  socketComm->open();
  socketComm->setLevelspdlog(spdlog::level::off);
  std::thread hilo1(chatRecibir);
  hilo1.detach();
  chatEscribir();
  socketComm->close();
  return 0;
}

### Socket
This module is in charge of the communication between sockets. It is divided in three parts: UDP, TCP and Serial. 
```mermaid
classDiagram 
Socket <|-- UDPSocket
Socket <|-- TCPSocket
Socket <|-- SerialSocket
EventListner <|-- Socket
EventListner o-- Suscriber
%% FactorySocket creates Socket and text it
FactorySocket --  Socket

class FactorySocket{
    +createSocketUDP(localPort, remotePort, remoteIp)
    +createSocketTCP(localPort, remotePort, remoteIp,TCPMode)
    +createSocketSerial(port, baudrate, bytesize, parity, stopbits)
}

class Socket{
    open();
    close();
    write(Serializable);
    read();
}

class Serializable {
    +Serializable()
    +Serializable(serializedData: std::vector<uint8_t>)
    +operator const std::vector<uint8_t>() const
    +~Serializable()
    +size(): int
    +empty(): bool
    #serializedData: std::vector<uint8_t>
    +setVector(serializedData: std::vector<uint8_t>)
    +friend std::ostream &operator<<(os: std::ostream, serializable: Serializable)
    +explicit operator std::string() const
}

class Suscriber{
    void update(Serializable updateData)
}

class EventListner{
    void addSuscriber(Suscriber suscriber)
    void addSuscriber(Suscriber suscriber, string topic)
    void removeSuscriber(Suscriber suscriber)
    void removeSuscriber(Suscriber suscriber, string topic)
    void removeTopic(string topic)
    void notify(Serializable updateData)
}

```
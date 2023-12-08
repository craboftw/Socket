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
    +Serializable(serializedData: vector<bytes>)
    +operator const std::vector<bytes>() const
    +~Serializable()
    +size()
    +empty()
    #serializedData: std::vector<bytes>
    +setVector(serializedData: vector<bytes>)
    +explicit operator string() const
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

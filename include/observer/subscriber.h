/**
* @file Subscriber.h
* @brief Contains the Subscriber class declaration.
* @date 2023-08-16
*/

#ifndef SOCKET_LIB_SUBSCRIBER_H
#define SOCKET_LIB_SUBSCRIBER_H

#include "serializable/Serializable.h"

/**
* @class Subscriber
* @brief Represents an abstract base class for subscribers.
*
* This class defines an interface for subscribers that can receive updates.
*/
class Subscriber {
public:
   /**
    * @brief Virtual function for receiving updates.
    * @param updateData The update data in a Serializable format.
    */
   virtual void update(Serializable updateData) = 0;
};

#endif // SOCKET_LIB_SUBSCRIBER_H

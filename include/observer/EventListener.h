/**
* @file EventListener.h
* @brief Contains the EventListener class declaration.
* @date 2023-08-16
*/

#ifndef SOCKET_LIB_EVENTLISTENER_H
#define SOCKET_LIB_EVENTLISTENER_H

#include "serializable/Serializable.h"
#include "subscriber.h"

#include <memory>
#include <vector>
#include <algorithm>

/**
* @class EventListener
* @brief Represents an event listener that manages subscribers and notifies them of events.
*/
class EventListener {
private:
   std::vector<std::shared_ptr<Subscriber>> subscribers; ///< Vector of subscribers.

public:
   /**
    * @brief Add a subscriber to the event listener.
    * @param subscriber The subscriber to be added.
    */
   void addSubscriber(std::shared_ptr<Subscriber> subscriber) ;

   /**
    * @brief Remove a subscriber from the event listener.
    * @param subscriber The subscriber to be removed.
    */
   void removeSubscriber(std::shared_ptr<Subscriber> subscriber) ;

   /**
    * @brief Notify all subscribers of an event.
    * @param event The event to be notified.
    */
   void notify(Serializable event) ;
};

#endif // SOCKET_LIB_EVENTLISTENER_H

#include "observer/EventListener.h"
#include <algorithm>

void EventListener::addSubscriber(std::shared_ptr<Subscriber> subscriber) {
    subscribers.push_back(subscriber);
}

void EventListener::removeSubscriber(std::shared_ptr<Subscriber> subscriber) {
    auto it = std::find(subscribers.begin(), subscribers.end(), subscriber);
    if (it != subscribers.end()) {
        subscribers.erase(it);
    }
}

void EventListener::notify(Serializable event) {
    for (auto &subscriber : subscribers) {
        subscriber->update(event);
    }
}

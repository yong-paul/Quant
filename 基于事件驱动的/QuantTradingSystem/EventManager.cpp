#include "EventManager.h"
#include "Event.h"

void EventManager::registerHandler(std::shared_ptr<EventHandler> handler) {
    handlers.push_back(handler);
}

void EventManager::unregisterHandler(std::shared_ptr<EventHandler> handler) {
    handlers.erase(std::remove(handlers.begin(), handlers.end(), handler), handlers.end());
}

void EventManager::processEvents() {
    std::lock_guard<std::mutex> lock(eventsMutex);
    for (auto& event : events) {
        for (auto& handler : handlers) {
            handler->handleEvent(event);
        }
    }
    events.clear();
}

void EventManager::addEvent(std::shared_ptr<Event> event) {
    std::lock_guard<std::mutex> lock(eventsMutex);
    events.push_back(event);
}
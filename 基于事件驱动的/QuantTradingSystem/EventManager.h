#pragma once

#include <vector>
#include <memory>
#include "Handlers/EventHandler.h"
#include "Events/Event.h"
#include <mutex>

class EventManager {
public:
    // 注册事件处理器
    void registerHandler(std::shared_ptr<EventHandler> handler);
    
    // 注销事件处理器
    void unregisterHandler(std::shared_ptr<EventHandler> handler);
    
    // 处理所有事件
    void processEvents();
    
    // 添加事件
    void addEvent(std::shared_ptr<Event> event);
    
private:
    std::vector<std::shared_ptr<EventHandler>> handlers;
    std::vector<std::shared_ptr<Event>> events;
    std::mutex eventsMutex;
};
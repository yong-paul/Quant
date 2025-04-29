#pragma once

#include <memory>

class Event;

class EventHandler {
public:
    explicit EventHandler(const std::string& name) : handlerName(name) {}
    virtual ~EventHandler() = default;
    
    // 处理事件
    virtual void handleEvent(const std::shared_ptr<Event>& event) = 0;
    
private:
    std::string handlerName;
};
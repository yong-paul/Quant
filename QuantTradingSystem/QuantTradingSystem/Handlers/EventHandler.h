#pragma once
#include <memory>
#include <string>
#include "../Events/AllEvents.h"

// 事件处理器基类
class EventHandler {
public:
    EventHandler(const std::string& name) : handlerName_(name) {}
    virtual ~EventHandler() = default;
    
    // 处理事件
    virtual void handleEvent(const std::shared_ptr<Event>& event) = 0;
    
    // 获取处理器名称
    std::string getName() const { return handlerName_; }
    
    // 重载相等运算符，用于查找和删除处理器
    bool operator==(const EventHandler& other) const {
        return handlerName_ == other.handlerName_;
    }
    
    bool operator==(const std::shared_ptr<EventHandler>& other) const {
        return other && handlerName_ == other->handlerName_;
    }

protected:
    std::string handlerName_;
};
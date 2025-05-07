#pragma once
#include <memory>
#include "../Events/Event.h"

// 事件处理器基类
class EventHandler {
public:
    virtual ~EventHandler() = default;
    
    // 处理事件
    virtual void handleEvent(const std::shared_ptr<Event>& event) = 0;
};
#pragma once
#include "EventHandler.h"
#include "Event.h"
#include <memory>

// 风险控制类
class RiskManager : public EventHandler {
public:
    void handleEvent(const std::shared_ptr<Event>& event) override {
        switch (event->getType()) {
            case EventType::ORDER:
                checkOrderRisk(event);
                break;
            case EventType::TRADE:
                updatePosition(event);
                break;
            default:
                break;
        }
    }

private:
    // 检查订单风险
    void checkOrderRisk(const std::shared_ptr<Event>& event);
    
    // 更新持仓
    void updatePosition(const std::shared_ptr<Event>& event);
    
    // 当前持仓
    double currentPosition = 0.0;
};
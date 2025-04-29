#pragma once
#include "EventHandler.h"
#include "Event.h"
#include <memory>

// 策略引擎类
class StrategyEngine : public EventHandler {
public:
    void handleEvent(const std::shared_ptr<Event>& event) override {
        switch (event->getType()) {
            case EventType::MARKET_DATA:
                processMarketData(event);
                break;
            case EventType::RISK:
                processRiskEvent(event);
                break;
            default:
                break;
        }
    }

private:
    // 处理行情数据
    void processMarketData(const std::shared_ptr<Event>& event);
    
    // 处理风控事件
    void processRiskEvent(const std::shared_ptr<Event>& event);
};
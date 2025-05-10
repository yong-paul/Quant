#pragma once
#include "EventHandler.h"
#include "../Events/AllEvents.h"
#include "../Trade/TradeService.h"
#include <memory>
#include <string>
#include <chrono>
#include <ctime>

#ifdef _WIN32
#define SAFE_CTIME(result, time, size) ctime_s(result, size, time)
#else
#define SAFE_CTIME(result, time, size) ctime_r(time, result)
#endif

// 信号处理器
class SignalHandler : public EventHandler {
public:
    SignalHandler(std::shared_ptr<EventManager> eventManager, 
                 std::shared_ptr<TradeService> tradingService)
        : EventHandler("SignalHandler"), 
          eventManager_(eventManager),
          tradingService_(tradingService) {}
    
    ~SignalHandler() override = default;
    
    // 处理事件
    void handleEvent(const std::shared_ptr<Event>& event) override {
        if (!event) return;
        
        // 处理策略信号事件
        if (event->getType() == EventType::STRATEGY_SIGNAL) {
            auto signalEvent = std::dynamic_pointer_cast<StrategySignalEvent>(event);
            if (signalEvent) {
                processSignal(signalEvent);
            }
        }
    }
    
private:
    // 处理信号
    void processSignal(const std::shared_ptr<StrategySignalEvent>& signal) {
        if (!tradingService_) {
            return;
        }
        
        // 检查交易服务是否就绪
        if (!tradingService_->IsConnected() || !tradingService_->IsLoggedIn()) {
            // 记录错误：交易服务未就绪
            return;
        }
        
        // 处理信号并下单
        if (!tradingService_->ProcessSignal(signal)) {
            // 处理失败，可以生成风控事件或系统事件
            const auto& data = signal->getData();
            
            // 创建系统事件
            SystemEventData sysData;
            sysData.type = SystemEventType::ERROR;
            sysData.source = "SignalHandler";
            sysData.message = "Failed to process signal for strategy: " + data.strategyId;
            
            auto now = std::chrono::system_clock::now();
            auto now_time_t = std::chrono::system_clock::to_time_t(now);
            char timeStr[26];
            SAFE_CTIME(timeStr, &now_time_t, sizeof(timeStr));
            sysData.eventTime = timeStr;
            
            auto sysEvent = std::make_shared<SystemEvent>(sysData);
            eventManager_->addEvent(sysEvent);
        }
    }
    
private:
    std::shared_ptr<EventManager> eventManager_;
    std::shared_ptr<TradeService> tradingService_;
}; 
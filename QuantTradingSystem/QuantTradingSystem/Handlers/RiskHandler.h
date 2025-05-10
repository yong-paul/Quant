#pragma once
#include "EventHandler.h"
#include "../Events/AllEvents.h"
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <map>
#include <mutex>
#include <chrono>
#include <ctime>

#ifdef _WIN32
#define SAFE_CTIME(result, time, size) ctime_s(result, size, time)
#else
#define SAFE_CTIME(result, time, size) ctime_r(time, result)
#endif

// 风控规则基类
class RiskRule {
public:
    RiskRule(const std::string& name) : name_(name) {}
    virtual ~RiskRule() = default;
    
    // 检查风控规则
    virtual bool check(const std::shared_ptr<Event>& event) = 0;
    
    // 获取风控规则名称
    std::string getName() const { return name_; }
    
protected:
    std::string name_;
};

// 订单频率风控规则
class OrderFrequencyRule : public RiskRule {
public:
    OrderFrequencyRule(int maxOrdersPerSecond, int timeWindowSeconds)
        : RiskRule("OrderFrequencyRule"), 
          maxOrdersPerSecond_(maxOrdersPerSecond),
          timeWindowSeconds_(timeWindowSeconds) {}
    
    bool check(const std::shared_ptr<Event>& event) override {
        if (event->getType() != EventType::ORDER) {
            return true; // 非订单事件，直接通过
        }
        
        auto orderEvent = std::dynamic_pointer_cast<OrderEvent>(event);
        if (!orderEvent) {
            return true;
        }
        
        const auto& data = orderEvent->getData();
        
        // 获取当前时间
        auto now = std::chrono::system_clock::now();
        
        // 清理过期记录
        clearExpiredRecords(now);
        
        // 记录本次订单
        std::lock_guard<std::mutex> lock(mutex_);
        orders_[data.strategyId].push_back(now);
        
        // 检查订单频率
        if (orders_[data.strategyId].size() > static_cast<size_t>(maxOrdersPerSecond_ * timeWindowSeconds_)) {
            return false; // 超过限制，拒绝订单
        }
        
        return true;
    }
    
private:
    void clearExpiredRecords(const std::chrono::system_clock::time_point& now) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto threshold = now - std::chrono::seconds(timeWindowSeconds_);
        
        for (auto& pair : orders_) {
            auto& orderTimes = pair.second;
            orderTimes.erase(
                std::remove_if(
                    orderTimes.begin(), 
                    orderTimes.end(),
                    [&threshold](const std::chrono::system_clock::time_point& t) { 
                        return t < threshold; 
                    }
                ),
                orderTimes.end()
            );
        }
    }
    
    int maxOrdersPerSecond_;
    int timeWindowSeconds_;
    std::unordered_map<std::string, std::vector<std::chrono::system_clock::time_point>> orders_;
    std::mutex mutex_;
};

// 持仓限制风控规则
class PositionLimitRule : public RiskRule {
public:
    PositionLimitRule(int maxPositionPerSymbol, int maxTotalPosition)
        : RiskRule("PositionLimitRule"), 
          maxPositionPerSymbol_(maxPositionPerSymbol),
          maxTotalPosition_(maxTotalPosition) {}
    
    bool check(const std::shared_ptr<Event>& event) override {
        if (event->getType() != EventType::ORDER) {
            return true; // 非订单事件，直接通过
        }
        
        auto orderEvent = std::dynamic_pointer_cast<OrderEvent>(event);
        if (!orderEvent) {
            return true;
        }
        
        const auto& data = orderEvent->getData();
        
        // 只检查开仓订单
        if (data.status != OrderStatus::SUBMITTED) {
            return true;
        }
        
        // 检查单个合约持仓限制
        {
            std::lock_guard<std::mutex> lock(mutex_);
            auto& pos = positions_[data.symbol];
            if (pos + data.volume > maxPositionPerSymbol_) {
                return false;
            }
        }
        
        // 检查总持仓限制
        {
            std::lock_guard<std::mutex> lock(mutex_);
            int totalPos = 0;
            for (const auto& pair : positions_) {
                totalPos += pair.second;
            }
            
            if (totalPos + data.volume > maxTotalPosition_) {
                return false;
            }
        }
        
        return true;
    }
    
    // 更新持仓数据
    void updatePosition(const std::string& symbol, int position) {
        std::lock_guard<std::mutex> lock(mutex_);
        positions_[symbol] = position;
    }
    
private:
    int maxPositionPerSymbol_;
    int maxTotalPosition_;
    std::unordered_map<std::string, int> positions_;
    std::mutex mutex_;
};

// 亏损限制风控规则
class LossLimitRule : public RiskRule {
public:
    LossLimitRule(double maxLossPerTrade, double maxDailyLoss)
        : RiskRule("LossLimitRule"), 
          maxLossPerTrade_(maxLossPerTrade),
          maxDailyLoss_(maxDailyLoss),
          dailyLoss_(0.0) {}
    
    bool check(const std::shared_ptr<Event>& event) override {
        if (event->getType() != EventType::TRADE) {
            return true; // 非成交事件，直接通过
        }
        
        auto tradeEvent = std::dynamic_pointer_cast<TradeEvent>(event);
        if (!tradeEvent) {
            return true;
        }
        
        const auto& data = tradeEvent->getData();
        
        // 计算本次交易盈亏
        // 这里需要更多的成交和持仓信息才能准确计算
        
        return true;
    }
    
    // 重置每日亏损
    void resetDailyLoss() {
        std::lock_guard<std::mutex> lock(mutex_);
        dailyLoss_ = 0.0;
    }
    
    // 更新亏损数据
    void updateLoss(double loss) {
        std::lock_guard<std::mutex> lock(mutex_);
        dailyLoss_ += loss;
    }
    
private:
    double maxLossPerTrade_;
    double maxDailyLoss_;
    double dailyLoss_;
    std::mutex mutex_;
};

// 风控管理器
class RiskManager : public EventHandler {
public:
    RiskManager(std::shared_ptr<EventManager> eventManager)
        : EventHandler("RiskManager"), eventManager_(eventManager) {}
    
    ~RiskManager() override = default;
    
    // 添加风控规则
    void addRule(std::shared_ptr<RiskRule> rule) {
        if (!rule) return;
        
        std::lock_guard<std::mutex> lock(mutex_);
        rules_.push_back(rule);
        
        // 记录添加规则的时间
        auto now = std::chrono::system_clock::now();
        auto now_time_t = std::chrono::system_clock::to_time_t(now);
        char timeStr[26];
        SAFE_CTIME(timeStr, &now_time_t, sizeof(timeStr));
        lastRuleUpdateTime_ = timeStr;
    }
    
    // 移除风控规则
    bool removeRule(const std::string& ruleName) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = std::find_if(
            rules_.begin(), 
            rules_.end(),
            [&ruleName](const std::shared_ptr<RiskRule>& rule) { 
                return rule->getName() == ruleName; 
            }
        );
        
        if (it != rules_.end()) {
            rules_.erase(it);
            return true;
        }
        
        return false;
    }
    
    // 处理事件
    void handleEvent(const std::shared_ptr<Event>& event) override {
        if (!event) return;
        
        // 检查所有风控规则
        bool passed = true;
        std::string failedRule;
        
        std::vector<std::shared_ptr<RiskRule>> rulesCopy;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            rulesCopy = rules_;
        }
        
        for (auto& rule : rulesCopy) {
            if (!rule->check(event)) {
                passed = false;
                failedRule = rule->getName();
                break;
            }
        }
        
        // 如果未通过风控检查，生成风控事件
        if (!passed && event->getType() == EventType::ORDER) {
            auto orderEvent = std::dynamic_pointer_cast<OrderEvent>(event);
            if (orderEvent) {
                const auto& data = orderEvent->getData();
                
                RiskData riskData;
                riskData.level = RiskLevel::WARNING;
                riskData.type = RiskType::ORDER_FREQUENCY_LIMIT; // 这里应该根据具体规则类型设置
                riskData.strategyId = data.strategyId;
                riskData.symbol = data.symbol;
                riskData.message = "Risk check failed: " + failedRule;
                
				auto now = std::chrono::system_clock::now();
				auto now_time_t = std::chrono::system_clock::to_time_t(now);
				char timeStr[26];
				SAFE_CTIME(timeStr, &now_time_t, sizeof(timeStr));
                riskData.triggerTime = timeStr;
                
                auto riskEvent = std::make_shared<RiskEvent>(riskData);
                eventManager_->addEvent(riskEvent);
                
                // 将订单修改为被拒绝状态
                OrderData rejectedOrder = data;
                rejectedOrder.status = OrderStatus::REJECTED;
                
                auto rejectedOrderEvent = std::make_shared<OrderEvent>(rejectedOrder);
                eventManager_->addEvent(rejectedOrderEvent);
            }
        }
    }
    
private:
    std::shared_ptr<EventManager> eventManager_;
    std::vector<std::shared_ptr<RiskRule>> rules_;
    std::mutex mutex_;
    std::string lastRuleUpdateTime_;
}; 
#pragma once
#include "EventHandler.h"
#include "../Events/AllEvents.h"
#include <memory>
#include <string>
#include <unordered_map>
#include <mutex>
#include <atomic>
#include "MarketDataField.h"

// 策略状态
enum class StrategyStatus {
    STOPPED,     // 停止
    RUNNING,     // 运行中
    PAUSED       // 暂停
};

// 策略参数基类
class StrategyParam {
public:
    virtual ~StrategyParam() = default;
};

// 策略接口基类
class Strategy {
public:
    Strategy(const std::string& id, const std::string& name) 
        : id_(id), name_(name), status_(StrategyStatus::STOPPED) {}
    
    virtual ~Strategy() = default;
    
    // 获取策略ID
    std::string getId() const { return id_; }
    
    // 获取策略名称
    std::string getName() const { return name_; }
    
    // 获取策略状态
    StrategyStatus getStatus() const { return status_; }
    
    // 初始化策略
    virtual bool init(std::shared_ptr<StrategyParam> param) = 0;
    
    // 启动策略
    virtual bool start() {
        status_ = StrategyStatus::RUNNING;
        return true;
    }
    
    // 停止策略
    virtual bool stop() {
        status_ = StrategyStatus::STOPPED;
        return true;
    }
    
    // 暂停策略
    virtual bool pause() {
        if (status_ == StrategyStatus::RUNNING) {
            status_ = StrategyStatus::PAUSED;
            return true;
        }
        return false;
    }
    
    // 恢复策略
    virtual bool resume() {
        if (status_ == StrategyStatus::PAUSED) {
            status_ = StrategyStatus::RUNNING;
            return true;
        }
        return false;
    }
    
    // 处理行情数据
    virtual void onMarketData(const MarketDataField& data) = 0;
    
    // 处理订单状态
    virtual void onOrder(const OrderData& data) = 0;
    
    // 处理成交回报
    virtual void onTrade(const TradeData& data) = 0;
    
protected:
    std::string id_;
    std::string name_;
    std::atomic<StrategyStatus> status_;
};

// 策略管理器
class StrategyManager : public EventHandler {
public:
    StrategyManager(std::shared_ptr<EventManager> eventManager)
        : EventHandler("StrategyManager"), eventManager_(eventManager) {}
    
    ~StrategyManager() override = default;
    
    // 注册策略
    bool registerStrategy(std::shared_ptr<Strategy> strategy) {
        if (!strategy) return false;
        
        std::lock_guard<std::mutex> lock(mutex_);
        auto id = strategy->getId();
        if (strategies_.find(id) != strategies_.end()) {
            return false; // 策略已存在
        }
        
        strategies_[id] = strategy;
        return true;
    }
    
    // 注销策略
    bool unregisterStrategy(const std::string& strategyId) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = strategies_.find(strategyId);
        if (it == strategies_.end()) {
            return false; // 策略不存在
        }
        
        // 确保策略已停止
        it->second->stop();
        strategies_.erase(it);
        return true;
    }
    
    // 启动策略
    bool startStrategy(const std::string& strategyId) {
        std::shared_ptr<Strategy> strategy;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            auto it = strategies_.find(strategyId);
            if (it == strategies_.end()) {
                return false; // 策略不存在
            }
            strategy = it->second;
        }
        
        return strategy->start();
    }
    
    // 停止策略
    bool stopStrategy(const std::string& strategyId) {
        std::shared_ptr<Strategy> strategy;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            auto it = strategies_.find(strategyId);
            if (it == strategies_.end()) {
                return false; // 策略不存在
            }
            strategy = it->second;
        }
        
        return strategy->stop();
    }
    
    // 暂停策略
    bool pauseStrategy(const std::string& strategyId) {
        std::shared_ptr<Strategy> strategy;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            auto it = strategies_.find(strategyId);
            if (it == strategies_.end()) {
                return false; // 策略不存在
            }
            strategy = it->second;
        }
        
        return strategy->pause();
    }
    
    // 恢复策略
    bool resumeStrategy(const std::string& strategyId) {
        std::shared_ptr<Strategy> strategy;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            auto it = strategies_.find(strategyId);
            if (it == strategies_.end()) {
                return false; // 策略不存在
            }
            strategy = it->second;
        }
        
        return strategy->resume();
    }
    
    // 处理事件
    void handleEvent(const std::shared_ptr<Event>& event) override {
        if (!event) return;
        
        switch (event->getType()) {
            case EventType::MARKET_DATA: {
                auto mdEvent = std::dynamic_pointer_cast<MarketDataEvent>(event);
                if (mdEvent) {
                    onMarketData(mdEvent->getData());
                }
                break;
            }
            case EventType::ORDER: {
                auto orderEvent = std::dynamic_pointer_cast<OrderEvent>(event);
                if (orderEvent) {
                    onOrder(orderEvent->getData());
                }
                break;
            }
            case EventType::TRADE: {
                auto tradeEvent = std::dynamic_pointer_cast<TradeEvent>(event);
                if (tradeEvent) {
                    onTrade(tradeEvent->getData());
                }
                break;
            }
            default:
                break;
        }
    }
    
private:
    // 处理行情数据
    void onMarketData(const MarketDataField& data) {
        std::vector<std::shared_ptr<Strategy>> activeStrategies;
        
        {
            std::lock_guard<std::mutex> lock(mutex_);
            for (auto& pair : strategies_) {
                if (pair.second->getStatus() == StrategyStatus::RUNNING) {
                    activeStrategies.push_back(pair.second);
                }
            }
        }
        
        // 向所有运行中的策略传递行情数据
        for (auto& strategy : activeStrategies) {
            try {
                strategy->onMarketData(data);
            } catch (const std::exception& e) {
                // 记录异常信息
            }
        }
    }
    
    // 处理订单状态
    void onOrder(const OrderData& data) {
        // 找到对应的策略
        std::shared_ptr<Strategy> strategy;
        
        {
            std::lock_guard<std::mutex> lock(mutex_);
            auto it = strategies_.find(data.strategyId);
            if (it != strategies_.end() && it->second->getStatus() != StrategyStatus::STOPPED) {
                strategy = it->second;
            }
        }
        
        if (strategy) {
            try {
                strategy->onOrder(data);
            } catch (const std::exception& e) {
                // 记录异常信息
            }
        }
    }
    
    // 处理成交回报
    void onTrade(const TradeData& data) {
        // 找到对应的策略
        std::shared_ptr<Strategy> strategy;
        
        {
            std::lock_guard<std::mutex> lock(mutex_);
            auto it = strategies_.find(data.strategyId);
            if (it != strategies_.end() && it->second->getStatus() != StrategyStatus::STOPPED) {
                strategy = it->second;
            }
        }
        
        if (strategy) {
            try {
                strategy->onTrade(data);
            } catch (const std::exception& e) {
                // 记录异常信息
            }
        }
    }
    
private:
    std::shared_ptr<EventManager> eventManager_;
    std::unordered_map<std::string, std::shared_ptr<Strategy>> strategies_;
    std::mutex mutex_;
}; 
#pragma once
#include "EventHandler.h"
#include "../Events/AllEvents.h"
#include "../MarketData/MarketDataField.h"
#include <memory>
#include <string>
#include <unordered_map>
#include <mutex>

// 行情数据处理器基类
class MarketDataHandler : public EventHandler {
public:
    MarketDataHandler(const std::string& name) 
        : EventHandler(name) {}
        
    ~MarketDataHandler() override = default;

    // 处理事件
    void handleEvent(const std::shared_ptr<Event>& event) override {
        if (!event) return;
        
        if (event->getType() == EventType::MARKET_DATA) {
            auto mdEvent = std::dynamic_pointer_cast<MarketDataEvent>(event);
            if (mdEvent) {
                onMarketData(mdEvent);
            }
        }
    }
    
    // 获取特定合约的最新行情数据
    virtual MarketDataField getLatestMarketData(const std::string& symbol) const = 0;
    
    // 检查合约是否存在缓存的行情数据
    virtual bool hasMarketData(const std::string& symbol) const = 0;
    
protected:
    // 行情数据事件处理
    virtual void onMarketData(const std::shared_ptr<MarketDataEvent>& event) = 0;
};

// 行情数据缓存处理器
class MarketDataCache : public MarketDataHandler {
public:
    MarketDataCache() 
        : MarketDataHandler("MarketDataCache") {}
        
    ~MarketDataCache() override = default;
    
    // 获取特定合约的最新行情数据
    MarketDataField getLatestMarketData(const std::string& symbol) const override {
        std::lock_guard<std::mutex> lock(cacheMutex_);
        auto it = marketDataCache_.find(symbol);
        if (it != marketDataCache_.end()) {
            return it->second;
        }
        
        // 返回空的行情数据
        return MarketDataField{};
    }
    
    // 检查合约是否存在缓存的行情数据
    bool hasMarketData(const std::string& symbol) const override {
        std::lock_guard<std::mutex> lock(cacheMutex_);
        return marketDataCache_.find(symbol) != marketDataCache_.end();
    }
    
protected:
    // 行情数据事件处理
    void onMarketData(const std::shared_ptr<MarketDataEvent>& event) override {
        const MarketDataField& data = event->getData();
        
        // 更新缓存
        std::lock_guard<std::mutex> lock(cacheMutex_);
        marketDataCache_[data.symbol] = data;
    }
    
private:
    // 行情数据缓存
    std::unordered_map<std::string, MarketDataField> marketDataCache_;
    
    // 缓存锁
    mutable std::mutex cacheMutex_;
}; 
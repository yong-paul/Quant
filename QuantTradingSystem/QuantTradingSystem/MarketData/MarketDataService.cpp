#include "MarketDataService.h"
#include "../EventManager.h"
#include <chrono>
#include <ctime>
#include <iostream>
#include <algorithm>

MarketDataService::MarketDataService(std::shared_ptr<EventManager> eventManager)
    : eventManager_(eventManager), running_(false) {
}

MarketDataService::~MarketDataService() {
    stop();
}

bool MarketDataService::init(const std::string& provider, const std::string& config) {
    // TODO: 使用工厂模式创建具体的行情数据源实例
    std::cerr << "Market data provider not implemented: " << provider << std::endl;
    return false;
}

bool MarketDataService::start() {
    if (!quoteApi_) {
        std::cerr << "Market data API not initialized" << std::endl;
        return false;
    }
    
    if (running_) {
        return true; // 已经在运行
    }
    
    // 连接服务器
    if (!quoteApi_->Connect()) {
        std::cerr << "Failed to connect to market data server" << std::endl;
        return false;
    }
    
    // 重新订阅之前的合约
    std::vector<std::string> symbols;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        symbols.assign(subscribedSymbols_.begin(), subscribedSymbols_.end());
    }
    
    if (!symbols.empty()) {
        if (!quoteApi_->Subscribe(symbols)) {
            std::cerr << "Failed to resubscribe symbols" << std::endl;
            // 不返回失败，继续运行
        }
    }
    
    running_ = true;
    return true;
}

void MarketDataService::stop() {
    if (!running_) {
        return;
    }
    
    if (quoteApi_) {
        quoteApi_->Disconnect();
        quoteApi_->Release();
    }
    
    running_ = false;
}

bool MarketDataService::subscribeSymbols(const std::vector<std::string>& symbols) {
    if (symbols.empty()) {
        return true;
    }
    
    if (!quoteApi_ || !isConnected()) {
        std::cerr << "Cannot subscribe: market data API not initialized or not connected" << std::endl;
        return false;
    }
    
    // 过滤出尚未订阅的合约
    std::vector<std::string> newSymbols;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        for (const auto& symbol : symbols) {
            if (subscribedSymbols_.find(symbol) == subscribedSymbols_.end()) {
                newSymbols.push_back(symbol);
            }
        }
    }
    
    if (newSymbols.empty()) {
        return true; // 所有合约已订阅
    }
    
    // 订阅新合约
    if (!quoteApi_->Subscribe(newSymbols)) {
        std::cerr << "Failed to subscribe symbols" << std::endl;
        return false;
    }
    
    // 添加到已订阅集合
    {
        std::lock_guard<std::mutex> lock(mutex_);
        for (const auto& symbol : newSymbols) {
            subscribedSymbols_.insert(symbol);
        }
    }
    
    return true;
}

bool MarketDataService::unsubscribeSymbols(const std::vector<std::string>& symbols) {
    if (symbols.empty()) {
        return true;
    }
    
    if (!quoteApi_ || !isConnected()) {
        std::cerr << "Cannot unsubscribe: market data API not initialized or not connected" << std::endl;
        return false;
    }
    
    // 过滤出已订阅的合约
    std::vector<std::string> existingSymbols;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        for (const auto& symbol : symbols) {
            if (subscribedSymbols_.find(symbol) != subscribedSymbols_.end()) {
                existingSymbols.push_back(symbol);
            }
        }
    }
    
    if (existingSymbols.empty()) {
        return true; // 所有合约未订阅
    }
    
    // 取消订阅
    if (!quoteApi_->Unsubscribe(existingSymbols)) {
        std::cerr << "Failed to unsubscribe symbols" << std::endl;
        return false;
    }
    
    // 从已订阅集合中移除
    {
        std::lock_guard<std::mutex> lock(mutex_);
        for (const auto& symbol : existingSymbols) {
            subscribedSymbols_.erase(symbol);
        }
    }
    
    return true;
}

std::vector<std::string> MarketDataService::getSubscribedSymbols() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return std::vector<std::string>(subscribedSymbols_.begin(), subscribedSymbols_.end());
}

bool MarketDataService::isConnected() const {
    return quoteApi_ && quoteApi_->IsConnected();
}

void MarketDataService::onMarketData(const MarketDataField& field) {
    if (!running_ || !eventManager_) {
        return;
    }
    
    // 转换为事件并发送
    auto event = convertToEvent(field);
    if (event) {
        eventManager_->addEvent(event);
    }
}

std::shared_ptr<MarketDataEvent> MarketDataService::convertToEvent(const MarketDataField& field) {
    // 创建市场数据事件
    auto event = std::make_shared<MarketDataEvent>(field);
    return event;
} 
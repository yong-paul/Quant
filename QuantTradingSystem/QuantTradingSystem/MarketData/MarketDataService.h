#pragma once

#include <memory>
#include <string>
#include <vector>
#include <set>
#include <mutex>
#include "../Events/MarketDataEvent.h"
#include "IMarketDataFeed.h"

class EventManager;

class MarketDataService {
public:
    // 构造函数
    explicit MarketDataService(std::shared_ptr<EventManager> eventManager);
    
    // 析构函数
    ~MarketDataService();
    
    // 初始化服务
    bool init(const std::string& provider, const std::string& config);
    
    // 启动和停止服务
    bool start();
    void stop();
    
    // 订阅和取消订阅合约
    bool subscribeSymbols(const std::vector<std::string>& symbols);
    bool unsubscribeSymbols(const std::vector<std::string>& symbols);
    
    // 获取已订阅的合约列表
    std::vector<std::string> getSubscribedSymbols() const;
    
    // 获取连接状态
    bool isConnected() const;
    
    // 获取当前使用的数据源名称
    std::string getProviderName() const;
    
private:
    // 行情回调处理
    void onMarketData(const MarketDataField& field);
    
    // 将行情数据转换为事件
    std::shared_ptr<MarketDataEvent> convertToEvent(const MarketDataField& field);
    
private:
    // 事件管理器
    std::shared_ptr<EventManager> eventManager_;
    
    // 行情数据源
    std::shared_ptr<IMarketDataFeed> quoteApi_;
    
    // 已订阅的合约集合
    std::set<std::string> subscribedSymbols_;
    
    // 互斥锁
    mutable std::mutex mutex_;
    
    // 服务状态
    bool running_;
}; 
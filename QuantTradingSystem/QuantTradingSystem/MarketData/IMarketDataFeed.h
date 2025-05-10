#pragma once
#include <string>
#include <vector>
#include <functional>
#include "MarketDataField.h"
#include <memory>
//// 行情数据结构
//struct MarketData {
//    std::string symbol;           // 合约代码
//    double lastPrice;            // 最新价
//    double openPrice;            // 开盘价
//    double highPrice;            // 最高价
//    double lowPrice;             // 最低价
//    double closePrice;           // 收盘价
//    double volume;               // 成交量
//    double turnover;            // 成交额
//    double bidPrice[5];         // 买价深度
//    double bidVolume[5];        // 买量深度
//    double askPrice[5];         // 卖价深度
//    double askVolume[5];        // 卖量深度
//    std::string updateTime;      // 更新时间
//    std::string tradingDay;      // 交易日
//};
using MarketDataCallback = std::function<void(const MarketDataField&)>;
// 市场数据源接口
class IMarketDataFeed {
public:
    virtual ~IMarketDataFeed() = default;

    // 初始化接口
    virtual bool Init(const std::string& config) = 0;
    
    // 连接和断开
    virtual bool Connect() = 0;
    virtual void Disconnect() = 0;
    virtual bool IsConnected() const = 0;
    
    // 订阅和取消订阅
    virtual bool Subscribe(const std::vector<std::string>& symbols) = 0;
    virtual bool Unsubscribe(const std::vector<std::string>& symbols) = 0;
    
    // 释放资源
    virtual void Release() = 0;
    
    // 设置回调
    virtual void SetMarketDataCallback(MarketDataCallback) = 0;

private:
    MarketDataCallback marketDataCallback_;
};

// 市场数据源工厂
class MarketDataFeedFactory {
public:
    static std::shared_ptr<IMarketDataFeed> createMarketDataFeed(const std::string& provider);
}; 
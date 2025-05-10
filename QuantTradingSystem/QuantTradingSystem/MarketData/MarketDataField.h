#pragma once

#include <string>
#include <array>

struct MarketDataField {
    std::string symbol;           // 合约代码
    std::string exchange;         // 交易所代码
    std::string tradingDay;       // 交易日
    std::string updateTime;       // 更新时间
    int updateMillisec;          // 更新毫秒
    
    double lastPrice;            // 最新价
    double openPrice;            // 开盘价
    double highPrice;            // 最高价
    double lowPrice;             // 最低价
    double closePrice;           // 收盘价
    double preClosePrice;        // 昨收价
    double upperLimit;           // 涨停价
    double lowerLimit;           // 跌停价
    
    std::array<double, 5> bidPrice;   // 申买价
    std::array<int, 5> bidVolume;     // 申买量
    std::array<double, 5> askPrice;   // 申卖价
    std::array<int, 5> askVolume;     // 申卖量
    
    int volume;                 // 成交量
    double turnover;           // 成交额
    double openInterest;       // 持仓量
    double preOpenInterest;    // 昨持仓量
    
    MarketDataField() : 
        lastPrice(0.0), openPrice(0.0), highPrice(0.0), lowPrice(0.0),
        closePrice(0.0), preClosePrice(0.0), upperLimit(0.0), lowerLimit(0.0),
        volume(0), turnover(0.0), openInterest(0.0), preOpenInterest(0.0),
        updateMillisec(0)
    {
        bidPrice.fill(0.0);
        bidVolume.fill(0);
        askPrice.fill(0.0);
        askVolume.fill(0);
    }
}; 
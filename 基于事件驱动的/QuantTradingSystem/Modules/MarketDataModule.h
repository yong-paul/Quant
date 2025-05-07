#pragma once
#include <string>

class MarketDataModule {
public:
    // 处理新市场数据
    void onNewMarketData(const std::string& symbol, double price, int volume);
};
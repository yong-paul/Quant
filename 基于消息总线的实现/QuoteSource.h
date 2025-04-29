#pragma once
#include "Message.h"

// 行情源接口
class IQuoteSource {
public:
    virtual ~IQuoteSource() = default;
    virtual MarketDataMessage getMarketData(const std::string& symbol) = 0;
    virtual void connect() = 0;
    virtual void disconnect() = 0;
};
#pragma once

#include <string>
#include <vector>
#include <functional>

struct MarketData {
    std::string symbol;
    double price;
    int volume;
    long timestamp;
};

using DataCallback = std::function<void(const MarketData&)>;

class IDataFeed {
public:
    virtual ~IDataFeed() = default;
    virtual void subscribe(const std::vector<std::string>& symbols) = 0;
    virtual void unsubscribe(const std::vector<std::string>& symbols) = 0;
    virtual void registerCallback(DataCallback callback) = 0;
    virtual void start() = 0;
    virtual void stop() = 0;
};
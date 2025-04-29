#pragma once

#include <vector>
#include <string>
#include "DataFeed.h"

struct Signal {
    std::string symbol;
    double price;
    int volume;
    char direction; // 'B' for buy, 'S' for sell
};

using SignalCallback = std::function<void(const Signal&)>;

class IStrategyEngine {
public:
    virtual ~IStrategyEngine() = default;
    virtual void registerDataCallback(DataCallback callback) = 0;
    virtual void registerSignalCallback(SignalCallback callback) = 0;
    virtual void addStrategy(const std::string& strategyName) = 0;
    virtual void removeStrategy(const std::string& strategyName) = 0;
    virtual void start() = 0;
    virtual void stop() = 0;
};
#pragma once

#include <vector>
#include <string>
#include "RiskManager.h"

struct ExecutionReport {
    std::string symbol;
    double price;
    int volume;
    char direction;
    std::string orderId;
    std::string status;
};

using ExecutionCallback = std::function<void(const ExecutionReport&)>;

class ITradeExecutor {
public:
    virtual ~ITradeExecutor() = default;
    virtual void registerRiskCheckCallback(RiskCheckCallback callback) = 0;
    virtual void registerExecutionCallback(ExecutionCallback callback) = 0;
    virtual void connect(const std::string& configPath) = 0;
    virtual void disconnect() = 0;
    virtual void execute(const Signal& signal) = 0;
};
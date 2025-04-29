#pragma once

#include <vector>
#include <string>
#include "StrategyEngine.h"

struct RiskCheckResult {
    bool approved;
    std::string reason;
};

using RiskCheckCallback = std::function<RiskCheckResult(const Signal&)>;

class IRiskManager {
public:
    virtual ~IRiskManager() = default;
    virtual void registerSignalCallback(SignalCallback callback) = 0;
    virtual void addRiskRule(const std::string& ruleName) = 0;
    virtual void removeRiskRule(const std::string& ruleName) = 0;
    virtual void setRiskCheckCallback(RiskCheckCallback callback) = 0;
    virtual void start() = 0;
    virtual void stop() = 0;
};
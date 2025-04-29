#include "RiskManager.h"
#include <iostream>

class RiskManager : public IRiskManager {
private:
    SignalCallback m_signalCallback;
    RiskCheckCallback m_riskCheckCallback;
    bool m_running = false;

public:
    void registerSignalCallback(SignalCallback callback) override {
        m_signalCallback = callback;
    }

    void addRiskRule(const std::string& ruleName) override {
        std::cout << "Added risk rule: " << ruleName << std::endl;
    }

    void removeRiskRule(const std::string& ruleName) override {
        std::cout << "Removed risk rule: " << ruleName << std::endl;
    }

    void setRiskCheckCallback(RiskCheckCallback callback) override {
        m_riskCheckCallback = callback;
    }

    void start() override {
        m_running = true;
        std::cout << "Risk manager started" << std::endl;
    }

    void stop() override {
        m_running = false;
        std::cout << "Risk manager stopped" << std::endl;
    }
};
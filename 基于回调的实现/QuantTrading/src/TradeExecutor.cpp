#include "TradeExecutor.h"
#include <iostream>

class TradeExecutor : public ITradeExecutor {
private:
    RiskCheckCallback m_riskCheckCallback;
    ExecutionCallback m_executionCallback;
    bool m_connected = false;

public:
    void registerRiskCheckCallback(RiskCheckCallback callback) override {
        m_riskCheckCallback = callback;
    }

    void registerExecutionCallback(ExecutionCallback callback) override {
        m_executionCallback = callback;
    }

    void connect(const std::string& configPath) override {
        m_connected = true;
        std::cout << "Connected to trading system with config: " << configPath << std::endl;
    }

    void disconnect() override {
        m_connected = false;
        std::cout << "Disconnected from trading system" << std::endl;
    }

    void execute(const Signal& signal) override {
        if (m_connected) {
            ExecutionReport report;
            report.symbol = signal.symbol;
            report.price = signal.price;
            report.volume = signal.volume;
            report.direction = signal.direction;
            report.orderId = "ORDER_" + std::to_string(rand());
            report.status = "FILLED";
            
            if (m_executionCallback) {
                m_executionCallback(report);
            }
        }
    }
};
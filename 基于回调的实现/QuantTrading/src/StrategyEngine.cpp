#include "StrategyEngine.h"
#include <iostream>

class StrategyEngine : public IStrategyEngine {
private:
    DataCallback m_dataCallback;
    SignalCallback m_signalCallback;
    bool m_running = false;

public:
    void registerDataCallback(DataCallback callback) override {
        m_dataCallback = callback;
    }

    void registerSignalCallback(SignalCallback callback) override {
        m_signalCallback = callback;
    }

    void addStrategy(const std::string& strategyName) override {
        std::cout << "Added strategy: " << strategyName << std::endl;
    }

    void removeStrategy(const std::string& strategyName) override {
        std::cout << "Removed strategy: " << strategyName << std::endl;
    }

    void start() override {
        m_running = true;
        std::cout << "Strategy engine started" << std::endl;
    }

    void stop() override {
        m_running = false;
        std::cout << "Strategy engine stopped" << std::endl;
    }
};
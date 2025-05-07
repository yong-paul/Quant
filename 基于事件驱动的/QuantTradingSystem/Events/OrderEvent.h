#pragma once
#include "Event.h"
#include <string>

// 订单事件
class OrderEvent : public Event {
public:
    OrderEvent(const std::string& symbol, double price, int quantity, bool isBuy)
        : Event(EventType::ORDER), symbol_(symbol), price_(price), quantity_(quantity), isBuy_(isBuy) {}
    
    std::string getSymbol() const { return symbol_; }
    double getPrice() const { return price_; }
    int getQuantity() const { return quantity_; }
    bool isBuy() const { return isBuy_; }
    
    std::string toString() const override {
        return "OrderEvent: " + symbol_ + " " + (isBuy_ ? "BUY" : "SELL") + 
               " Price: " + std::to_string(price_) + " Quantity: " + std::to_string(quantity_);
    }
    
private:
    std::string symbol_;
    double price_;
    int quantity_;
    bool isBuy_;
};
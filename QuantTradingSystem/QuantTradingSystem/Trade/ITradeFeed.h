#pragma once
#include <string>
#include <memory>
#include <functional>
#include <vector>
#include "TradeDataStruct.h"

// 回调函数类型定义
using OrderCallback = std::function<void(const trade::OrderData&)>;
using TradeCallback = std::function<void(const trade::TradeData&)>;
using PositionCallback = std::function<void(const trade::PositionData&)>;
using AccountCallback = std::function<void(const trade::AccountData&)>;

// 交易接口基类
class ITradeFeed {
public:
    virtual ~ITradeFeed() = default;

    // 初始化接口
    virtual bool Init(const std::string& config) = 0;
    
    // 连接和断开
    virtual bool Connect() = 0;
    virtual void Disconnect() = 0;
    virtual bool IsConnected() const = 0;
    
    // 登录和登出
    virtual bool Login(const std::string& userId, const std::string& password) = 0;
    virtual bool Logout() = 0;
    virtual bool IsLoggedIn() const = 0;
    
    // 交易相关操作
    virtual std::string PlaceOrder(const trade::OrderData& orderData) = 0;
    virtual bool CancelOrder(const std::string& orderId) = 0;
    
    // 查询接口
    virtual std::vector<trade::OrderData> QueryPendingOrders() = 0;
    virtual trade::OrderData QueryOrder(const std::string& orderId) = 0;
    virtual std::vector<trade::PositionData> QueryPositions() = 0;
    virtual trade::AccountData QueryAccount() = 0;
    
    // 资源释放
    virtual void Release() = 0;
    
    // 设置回调
    virtual void SetOrderCallback(OrderCallback callback) = 0;
    virtual void SetTradeCallback(TradeCallback callback) = 0;
    virtual void SetPositionCallback(PositionCallback callback) = 0;
    virtual void SetAccountCallback(AccountCallback callback) = 0;
};

// 交易接口工厂类
class TradeFeedFactory {
public:
    static std::shared_ptr<ITradeFeed> CreateTradeFeed(const std::string& provider);
}; 
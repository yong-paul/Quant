#pragma once
#include "ITradeFeed.h"
#include <string>
#include <memory>
#include <mutex>
#include <atomic>
#include <vector>
#include <unordered_map>

// 前置声明CTP的交易API类
namespace CTP {
    class CThostFtdcTraderApi;
    class CThostFtdcTraderSpi;
    struct CThostFtdcOrderField;
    struct CThostFtdcTradeField;
    struct CThostFtdcInvestorPositionField;
    struct CThostFtdcTradingAccountField;
    struct CThostFtdcRspInfoField;
    struct CThostFtdcRspUserLoginField;
}

// CTP交易接口实现
class CTPTradeFeed : public ITradeFeed {
public:
    CTPTradeFeed();
    ~CTPTradeFeed();

    // 初始化接口
    bool Init(const std::string& config) override;
    
    // 释放资源
    void Release() override;
    
    // 连接和断开
    bool Connect() override;
    void Disconnect() override;
    bool IsConnected() const override;
    
    // 登录和登出
    bool Login(const std::string& userId, const std::string& password) override;
    bool Logout() override;
    bool IsLoggedIn() const override;
    
    // 交易相关操作
    std::string PlaceOrder(const trade::OrderData& orderData) override;
    bool CancelOrder(const std::string& orderId) override;
    
    // 查询接口
    std::vector<trade::OrderData> QueryPendingOrders() override;
    trade::OrderData QueryOrder(const std::string& orderId) override;
    std::vector<trade::PositionData> QueryPositions() override;
    trade::AccountData QueryAccount() override;
    
    // 设置回调
    void SetOrderCallback(OrderCallback callback) override;
    void SetTradeCallback(TradeCallback callback) override;
    void SetPositionCallback(PositionCallback callback) override;
    void SetAccountCallback(AccountCallback callback) override;
    
private:
    // CTP API相关
    CTP::CThostFtdcTraderApi* traderApi_;
    CTP::CThostFtdcTraderSpi* traderSpi_;
    
    // 回调函数
    OrderCallback orderCallback_;
    TradeCallback tradeCallback_;
    PositionCallback positionCallback_;
    AccountCallback accountCallback_;
    
    // 连接参数
    std::string frontAddress_;
    std::string brokerId_;
    std::string userId_;
    std::string password_;
    std::string appId_;
    std::string authCode_;
    
    // 状态标志
    std::atomic<bool> connected_;
    std::atomic<bool> loggedIn_;
    
    // 会话参数
    int frontId_;
    int sessionId_;
    int orderRef_;
    
    // 订单映射，用于管理订单
    std::unordered_map<std::string, trade::OrderData> orders_;
    
    // 互斥锁
    mutable std::mutex apiMutex_;
}; 
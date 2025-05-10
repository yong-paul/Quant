#pragma once

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <atomic>

#include "ITradeFeed.h"
#include "../Events/AllEvents.h"
#include "../EventManager.h"

class TradeService {
public:
    // 构造函数
    explicit TradeService(std::shared_ptr<EventManager> eventManager);
    
    // 析构函数
    ~TradeService();
    
    // 初始化服务
    bool Init(const std::string& provider, const std::string& config);
    
    // 启动和停止服务
    bool Start();
    void Stop();
    
    // 登录和登出
    bool Login(const std::string& userId, const std::string& password);
    bool Logout();
    
    // 下单处理（处理信号事件）
    bool ProcessSignal(const std::shared_ptr<StrategySignalEvent>& signal);
    
    // 手动下单
    std::string PlaceOrder(const trade::OrderData& orderData);
    
    // 撤单
    bool CancelOrder(const std::string& orderId);
    
    // 更新所有持仓和账户
    bool RefreshData();
    
    // 查询接口
    std::vector<trade::OrderData> QueryPendingOrders() const;
    trade::OrderData QueryOrder(const std::string& orderId) const;
    std::vector<trade::PositionData> QueryPositions() const;
    trade::AccountData QueryAccount() const;
    
    // 是否已连接
    bool IsConnected() const;
    
    // 是否已登录
    bool IsLoggedIn() const;
    
    // 获取当前使用的交易接口名称
    std::string GetProviderName() const;
    
private:
    // 回调处理函数
    void OnOrder(const trade::OrderData& data);
    void OnTrade(const trade::TradeData& data);
    void OnPosition(const trade::PositionData& data);
    void OnAccount(const trade::AccountData& data);
    
    // 创建订单数据
    trade::OrderData CreateOrderFromSignal(const StrategySignalData& signalData);
    
    // 将交易数据转换为事件
    std::shared_ptr<OrderEvent> ConvertToOrderEvent(const trade::OrderData& data);
    std::shared_ptr<TradeEvent> ConvertToTradeEvent(const trade::TradeData& data);
    
private:
    // 事件管理器
    std::shared_ptr<EventManager> eventManager_;
    
    // 交易接口
    std::shared_ptr<ITradeFeed> tradeFeed_;
    
    // 交易接口提供商名称
    std::string providerName_;
    
    // 持仓缓存
    std::unordered_map<std::string, trade::PositionData> positions_;
    
    // 账户数据缓存
    trade::AccountData accountData_;
    
    // 互斥锁
    mutable std::mutex mutex_;
    
    // 服务状态
    std::atomic<bool> running_;
}; 
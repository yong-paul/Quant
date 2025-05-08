#pragma once

#include <string>
#include <memory>
class IQuoteDataApi
{
public:
	virtual ~IQuoteDataApi() {}

public:
	/*
	 *	初始化解析模块
	 *	@config	模块配置
	 *	返回值	是否初始化成功
	 */
	virtual bool Init(const std::string config) { return false; }

	/*
	 *	释放解析模块
	 *	用于退出时
	 */
	virtual void Release() {}

	/*
	 *	开始连接服务器
	 *	@返回值	连接命令是否发送成功
	 */
	virtual bool Connect() { return false; }

	/*
	 *	断开连接
	 *	@返回值	命令是否发送成功
	 */
	virtual bool Disconnect() { return false; }

	/*
	 *	是否已连接
	 *	@返回值	是否已连接
	 */
	virtual bool IsConnected() { return false; }

	///*
	// *	订阅合约列表
	// */
	//virtual void subscribe(const & setCodes) {}

	///*
	// *	退订合约列表
	// */
	//virtual void unsubscribe(const CodeSet& setCodes) {}
};


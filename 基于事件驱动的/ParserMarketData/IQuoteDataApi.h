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
	virtual bool init(const std::string config) { return false; }

	/*
	 *	释放解析模块
	 *	用于退出时
	 */
	virtual void release() {}

	/*
	 *	开始连接服务器
	 *	@返回值	连接命令是否发送成功
	 */
	virtual bool connect() { return false; }

	/*
	 *	断开连接
	 *	@返回值	命令是否发送成功
	 */
	virtual bool disconnect() { return false; }

	/*
	 *	是否已连接
	 *	@返回值	是否已连接
	 */
	virtual bool isConnected() { return false; }

	///*
	// *	订阅合约列表
	// */
	//virtual void subscribe(const & setCodes) {}

	///*
	// *	退订合约列表
	// */
	//virtual void unsubscribe(const CodeSet& setCodes) {}
};


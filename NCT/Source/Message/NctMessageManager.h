/**************************************************************************
**  Copyright (C) 20018-2019 北京捷安军工科技有限公司, All Rights Reserved
**
**  文件名：  NctMessageManager.h
**  创建者：  闫相伟
**  创建日期：2018年4月26日
**  详细信息： 队列管理器
**
**
***************************************************************************/
#pragma once
#include "Utility/NctModule.h"
#include "Message/NctMessageQueue.h"
#include "Threading/NctThreadDefines.h"
#include "NctMsgBuffer.h"

namespace Nct
{

	class MessageManager : public Module<MessageManager>
	{
	public:

		/**
		* 将接收到的状态信息入队
		* @param[in] inMsg
		* @param[in] size
		*/
		void pushInStateMsg(const char * inMsg,const std::uint32_t size);

		
		void  popInStateMsg(MsgBuffer & inMsg);

		bool tryPopInStateMsg(MsgBuffer & inMsg);

		bool timePopInStateMsg(MsgBuffer & inMsg, std::uint32_t time);

		/**
		* 将要发出去的状态信息入队 
		* @param[in] outMsg
		*/
		void pushOutStateMsg(const char * inMsg, const std::uint32_t size);


		void popOutStateMsg(MsgBuffer & inMsg);


		bool tryPopOutStateMsg(MsgBuffer & inMsg);


		bool timePopOutStateMsg(MsgBuffer & inMsg, std::uint32_t time);
	

		/**
		* 设置接收到的实时信息
		* @param[in] outMsg
		*/
		void setInRealMsg(const char * inMsg, const std::uint32_t size);


		void getInRealMsg(MsgBuffer & inMsg);

		/**
		* 设置准备发出实时信息
		* @param[in] inMsg
		*/
		void setOutRealMsg(const char * inMsg, const std::uint32_t size);

		void getOutRealMsg(MsgBuffer & inMsg);

	private:
		virtual void onStartUp();


		virtual void onShutDown();
		

	private:
		// 用于存储接收到的状态消息的队列
		MessageQueue<std::shared_ptr<MsgBuffer>> mInStateMsgQueue;
		// 用于存储发送出去的状态消息的队列 
		MessageQueue<std::shared_ptr<MsgBuffer>> mOutStateMsgQueue;

		NctMutex mInRealMsgMutex;
		NctMutex mOutRealMsgMutex;
		// 存储实时接收到的消息
		std::shared_ptr<MsgBuffer>               mInRealMsg;
		// 用于储存实时发出的消息
		std::shared_ptr<MsgBuffer>               mOutRealMsg;
	};

}
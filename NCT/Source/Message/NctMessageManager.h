/**************************************************************************
**  Copyright (C) 20018-2019 �����ݰ������Ƽ����޹�˾, All Rights Reserved
**
**  �ļ�����  NctMessageManager.h
**  �����ߣ�  ����ΰ
**  �������ڣ�2018��4��26��
**  ��ϸ��Ϣ�� ���й�����
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
		* �����յ���״̬��Ϣ���
		* @param[in] inMsg
		* @param[in] size
		*/
		void pushInStateMsg(const char * inMsg,const std::uint32_t size);

		
		void  popInStateMsg(MsgBuffer & inMsg);

		bool tryPopInStateMsg(MsgBuffer & inMsg);

		bool timePopInStateMsg(MsgBuffer & inMsg, std::uint32_t time);

		/**
		* ��Ҫ����ȥ��״̬��Ϣ��� 
		* @param[in] outMsg
		*/
		void pushOutStateMsg(const char * inMsg, const std::uint32_t size);


		void popOutStateMsg(MsgBuffer & inMsg);


		bool tryPopOutStateMsg(MsgBuffer & inMsg);


		bool timePopOutStateMsg(MsgBuffer & inMsg, std::uint32_t time);
	

		/**
		* ���ý��յ���ʵʱ��Ϣ
		* @param[in] outMsg
		*/
		void setInRealMsg(const char * inMsg, const std::uint32_t size);


		void getInRealMsg(MsgBuffer & inMsg);

		/**
		* ����׼������ʵʱ��Ϣ
		* @param[in] inMsg
		*/
		void setOutRealMsg(const char * inMsg, const std::uint32_t size);

		void getOutRealMsg(MsgBuffer & inMsg);

	private:
		virtual void onStartUp();


		virtual void onShutDown();
		

	private:
		// ���ڴ洢���յ���״̬��Ϣ�Ķ���
		MessageQueue<std::shared_ptr<MsgBuffer>> mInStateMsgQueue;
		// ���ڴ洢���ͳ�ȥ��״̬��Ϣ�Ķ��� 
		MessageQueue<std::shared_ptr<MsgBuffer>> mOutStateMsgQueue;

		NctMutex mInRealMsgMutex;
		NctMutex mOutRealMsgMutex;
		// �洢ʵʱ���յ�����Ϣ
		std::shared_ptr<MsgBuffer>               mInRealMsg;
		// ���ڴ���ʵʱ��������Ϣ
		std::shared_ptr<MsgBuffer>               mOutRealMsg;
	};

}
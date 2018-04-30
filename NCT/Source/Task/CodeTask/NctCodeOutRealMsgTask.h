
/**************************************************************************
**  Copyright (C) 2002-2018 �����ݰ���ı�����Ƽ����޹�˾, All Rights Reserved
**
**  �ļ�����  NctDecodeTaskBase.h
**  �����ߣ�  ����ΰ
**  �������ڣ�2018��4��27��
**  ��ϸ��Ϣ���������������
**
**
***************************************************************************/


#pragma once

#include "Prerequisites/NctPrerequisitesUtil.h"
#include "Task/NctPersistentTaskBase.h"
#include "Message/NctMsgBuffer.h"

namespace Nct
{

	class DecodeMsgTask : public PersistentTaskBase
	{
	public:
		DecodeMsgTask();

		virtual ~ DecodeMsgTask();
	public:
		virtual bool getMsg() = 0;

		virtual void  decodeMsg() = 0;

	private:
		void run();

	protected:		
		std::shared_ptr<MsgBuffer> mMsgBuf;		
	};

}
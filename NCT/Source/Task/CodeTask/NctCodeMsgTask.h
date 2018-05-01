
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

	class CodeMsgTask : public PersistentTaskBase
	{
	public:
		CodeMsgTask();

		virtual ~CodeMsgTask();
	public:
		virtual void setMsg() = 0;

		virtual bool  codeMsg() = 0;

		void run() override;
	protected:		
		std::shared_ptr<MsgBuffer> mMsgBuf;		
	};

}
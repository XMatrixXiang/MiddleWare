
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
#include "Task/CodeTask/NctCodeMsgTask.h"
#include "Message/NctMsgBuffer.h"

namespace Nct
{

	class CodeOutRealMsgTask : public CodeMsgTask
	{
	public:
		void setMsg() override;		

		bool  codeMsg() override;
	};

}
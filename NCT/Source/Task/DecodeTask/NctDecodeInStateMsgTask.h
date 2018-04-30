
/**************************************************************************
**  Copyright (C) 2002-2018 �����ݰ���ı�����Ƽ����޹�˾, All Rights Reserved
**
**  �ļ�����  NctDecodeTaskBase.h
**  �����ߣ�  ����ΰ
**  �������ڣ�2018��4��27��
**  ��ϸ��Ϣ������ʵʱ�յ���״̬��Ϣ
**
**
***************************************************************************/


#pragma once

#include "Prerequisites/NctPrerequisitesUtil.h"
#include "Task/DecodeTask/NctDecodeMsgTask.h"

namespace Nct
{

	class DecodeInStateMsgTask : public DecodeMsgTask
	{
	public:
		virtual bool getMsg() override;

		virtual void decodeMsg() override;

	};

}
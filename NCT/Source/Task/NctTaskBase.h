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
#include "Threading/NctTaskScheduler.h"


namespace Nct
{

	class TaskBase
	{
	public:
		TaskBase() : mTerminate(false) {}
	public:
		virtual std::function<void(void)> getRunTaskFunction() = 0;
		virtual void terminate();
		virtual void reserveTask(std::shared_ptr<Task> task) { mTask = task; }
	protected:
		bool mTerminate;
		std::shared_ptr<Task> mTask;
	};
}
/**************************************************************************
**  Copyright (C) 20018-2019 �����ݰ������Ƽ����޹�˾, All Rights Reserved
**
**  �ļ�����  NctStackAlloc.h
**  �����ߣ�  ����ΰ
**  �������ڣ�2018��4��26��
**  ��ϸ��Ϣ��
**
**
***************************************************************************/

#pragma once

#include "Prerequisites/NctPrerequisitesUtil.h"

namespace Nct
{
	
	class ProfilerAlloc
	{};

	
	template<>
	class MemoryAllocator<ProfilerAlloc> : public MemoryAllocatorBase
	{
	public:
		
		static void* allocate(size_t bytes)
		{
			return malloc(bytes);
		}

		
		static void free(void* ptr)
		{
			::free(ptr);
		}
	};
}
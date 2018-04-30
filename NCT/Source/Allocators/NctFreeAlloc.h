/**************************************************************************
**  Copyright (C) 2002-2018 �����ݰ���ı�����Ƽ����޹�˾, All Rights Reserved
**
**  �ļ�����  NctFreeAlloc.h
**  �����ߣ�  ����ΰ
**  �������ڣ�2018��4��27��
**  ��ϸ��Ϣ���ڴ��ͷ�
**
**
***************************************************************************/
#pragma once

#include "Prerequisites/NctPrerequisitesUtil.h"

namespace Nct
{

	class FreeAlloc
	{
	public:		
		std::uint8_t * alloc(std::uint32_t amount)
		{
			return (std::uint8_t*)malloc(amount);
		}
		
		void free(void* data)
		{
			::free(data);
		}
		
		void clear()
		{

		}
	};
}

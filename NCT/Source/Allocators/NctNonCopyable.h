/**************************************************************************
**  Copyright (C) 20018-2019 �����ݰ������Ƽ����޹�˾, All Rights Reserved
**
**  �ļ�����  NctNonCopyable.h
**  �����ߣ�  ����ΰ
**  �������ڣ�2018��4��26��
**  ��ϸ��Ϣ��
**
**
***************************************************************************/

#pragma once

namespace Nct
{

	class INonCopyable
	{
	protected:
		INonCopyable() = default;
		~INonCopyable() = default;

	private:
		INonCopyable(const INonCopyable&) = delete;
		INonCopyable& operator=(const INonCopyable&) = delete;

	};	
}

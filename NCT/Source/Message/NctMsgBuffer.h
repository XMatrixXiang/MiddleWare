/**************************************************************************
**  Copyright (C) 2002-2018 �����ݰ���ı�����Ƽ����޹�˾, All Rights Reserved
**
**  �ļ�����  NctMsgBuffer.h
**  �����ߣ�  ����ΰ
**  �������ڣ�2018��4��27��
**  ��ϸ��Ϣ����Ϣ�ڴ��
**
**
***************************************************************************/

#pragma once

#include "Prerequisites/NctPrerequisitesUtil.h"
#include "Allocators/NctStaticAlloc.h"

namespace Nct
{

	class MsgBuffer
	{		
	public:
		MsgBuffer() :mMemSize(0), mData(nullptr) , mDataSize(0){}
		~MsgBuffer()
		{
			if (mData != nullptr)
			{
				mAlloc.free(mData);
				mAlloc.clear();
				mData = nullptr;
				mMemSize = 0;
				mDataSize = 0;
			}

		}
	public:
		std::uint32_t getCurBufferSize() { return mMemSize; }
		std::uint8_t * getCurBufferPtr() { return mData; }
		std::uint32_t getDataSize() { return mDataSize; }
		void setRawData(const char * buffer, const std::uint32_t bufferSize)
		{
			if (bufferSize == 0)
			{
				std::memset(getCurBufferPtr(),'\0', getDataSize());
				mDataSize = 0;
				return;
			}
			if (getCurBufferSize() < bufferSize)
			{
				resizeBufferMemorySize(bufferSize);
			}
			std::memcpy(getCurBufferPtr(),buffer,bufferSize);
			mDataSize = bufferSize;
		}	
		void resizeBufferMemorySize(const std::uint32_t bufferSize)
		{ 
			if (bufferSize == 0) return;
			if (mData != nullptr)
			{
				mAlloc.free(mData);
			}
			mData = mAlloc.alloc(bufferSize); 
			mMemSize = bufferSize;
		}
	private:
		StaticAlloc<> mAlloc;
		std::uint32_t mMemSize;
		std::uint32_t mDataSize;
		std::uint8_t * mData;
	};
}
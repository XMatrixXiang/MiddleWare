/**************************************************************************
**  Copyright (C) 20018-2019 北京捷安军工科技有限公司, All Rights Reserved
**
**  文件名：  NctMessageQueue.h
**  创建者：  闫相伟
**  创建日期：2018年4月26日
**  详细信息：线性队列
**
**
***************************************************************************/

#pragma once
#include "Prerequisites/NctPrerequisitesUtil.h"
#include "Threading/NctThreadDefines.h"

namespace Nct
{

	template <class T>
	class MessageQueue
	{
	public:

		typedef T value_type;
		typedef size_t size_type;

		MessageQueue();
		~MessageQueue();

		void clear();

		void push(const T & value);

		value_type pop();
		value_type tryPop(bool & is_empty);
		value_type timedPop(bool & is_empty, unsigned long ms);

	private:

		MessageQueue(const MessageQueue &) = delete;
		MessageQueue & operator = (const MessageQueue &) = delete;

		NctMutex            mMutex;
		NctSignal           mNotEmpty;
		std::deque<T>       mQueue;
	};


	template <class T>
	MessageQueue<T>::MessageQueue(){}

	template <class T>
	MessageQueue<T>::~MessageQueue() {}


	template <class T>
	void MessageQueue<T>::clear()
	{
		NctLock lock(mMutex);
		mQueue.clear();
	}

	template <typename T>
	void MessageQueue<T>::push(const T & value)
	{
		{
			NctLock lock(mMutex);
			mQueue.push_back(value);
		}
		mNotEmpty.notify_one();
	}



	template <typename T>
	typename MessageQueue<T>::value_type MessageQueue<T>::pop()
	{
		NctLock lock(mMutex);

		while (mQueue.empty())
		{
			mNotEmpty.wait(lock);
		}

		const value_type value = mQueue.front();
		mQueue.pop_front();

		return value;
	}

	template <typename T>
	typename MessageQueue<T>::value_type MessageQueue<T>::tryPop(bool & is_empty)
	{
		NctLock lock(mMutex);
		is_empty = mQueue.empty();
		if (is_empty)
		{
			return value_type();
		}
		const value_type value = mQueue.front();
		mQueue.pop_front();
		return value;
	}

	template <typename T>
	typename  MessageQueue<T>::value_type MessageQueue<T>::timedPop(bool & is_empty, const unsigned long ms)
	{
		NctLock lock(mMutex);
		if (mQueue.empty())
		{
			mNotEmpty.wait_for(lock, std::chrono::seconds(ms));
		}
		is_empty = mQueue.empty();
		if (is_empty)
		{
			return value_type();
		}
		const value_type value = mQueue.front();
		mQueue.pop_front();
		return value;
	}
}


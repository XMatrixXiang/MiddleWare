/**************************************************************************
**  Copyright (C) 20018-2019 北京捷安军工科技有限公司, All Rights Reserved
**
**  文件名：  NctBoundedMessageQueue.h
**  创建者：  闫相伟
**  创建日期：2018年4月26日
**  详细信息：圆形队列
**
**
***************************************************************************/
#pragma once;

#include "Threading/NctThreadDefines.h"
#include "Prerequisites/NctPrerequisitesUtil.h"

namespace Nct
{
	/** 消息队列 */
	template <typename T>
	class BoundedMessageQueue
	{
	public:

		typedef T value_type;
		typedef size_t size_type;
		
		/**
		 * @param[in] capacity 队列容量
		 */
		explicit BoundedMessageQueue(size_type capacity);
		~BoundedMessageQueue();

		void clear();

		/**
		* @param[in] destructor 使用特定的析构器来析构当前所有的元素，并且触发not full 信号
		*/
		template <typename Destructor>
		void flush(const Destructor destructor);

		/**
		* 如果不满足入队条件，则一直等待，直到条件满足
		* @param[in] value 将value入队
		*/
		void push(const value_type & value);

		/**
		* 如果不满足入队条件，直接返回false,否则返回true
		* @param[in] value 将value入队
		*/
		bool tryPush(const value_type & value);

		/**
		* 在指定的ms时间内进行入队尝试，如果无法如归则返回false，否则返回true
		* @param[in] value 将value入队
		* @param[in] ms    将等待时间入队
		*/
		bool timedPush(const value_type & value, unsigned long ms);

		/** 出队*/
		value_type pop();

		/**
		* 尝试出队
		* @param[out] is_empty  是否可以成功出队	
		*/
		value_type tryPop(bool & is_empty);

		/**
		* 在指定的时间内尝试出队
		* @param[in] ms  限定时间
		* @param[out] is_empty  是否可以成功出队
		*/
		value_type timedPop(bool & is_empty, unsigned long ms);

	private:

		BoundedMessageQueue(const BoundedMessageQueue &) = delete;
		BoundedMessageQueue & operator = (const BoundedMessageQueue &) = delete;

		typedef std::vector<T> Buffer;

		bool isFull() const;
		bool isEmpty() const;

		void unsafePush(const value_type & value);
		value_type unsafePop();

		Buffer        mBuffer;
		// 当前有效消息存储链开始索引
		size_type     mBegin;
		// 当前有效消息存储链结束索引
		size_type     mEnd;
		// 当前消息量
		size_type     mSize;
		// 互斥锁
		Mutex         mMutex;
		// 不空信号
		Signal        mNotEmpty;
		// 不满信号
		Signal        mNotFull;
	};


	template <typename T>
	BoundedMessageQueue<T>::BoundedMessageQueue(const size_type capacity) :
		mBuffer(capacity),
		mBegin(0),
		mEnd(0),
		mSize(0)
	{

	}



	template <typename T>
	BoundedMessageQueue<T>::~BoundedMessageQueue()
	{

	}



	template <typename T>
	void BoundedMessageQueue<T>::clear()
	{
		{
			Lock lock(m_mutex);

			mBuffer.clear();
			mBegin = 0;
			mEnd = 0;
			mSize = 0;
		}

		mNotFull.broadcast();
	}



	template <typename T>
	template <typename Destructor>
	void BoundedMessageQueue<T>::flush(const Destructor destructor)
	{
		{
			Lock lock(m_mutex);

			while (!isEmpty())
			{
				value_type value = unsafePop();
				destructor(value);
			}

			mBegin = 0;
			mEnd = 0;
			mSize = 0;
		}

		mNotFull.broadcast();
	}



	template <typename T>
	void BoundedMessageQueue<T>::push(const value_type & value)
	{
		{
			Lock lock(m_mutex);

			while (isFull())
				m_not_full.wait(&m_mutex);

			unsafePush(value);
		}
		m_not_empty.signal();
	}



	template <typename T>
	bool BoundedMessageQueue<T>::tryPush(const value_type & value)
	{
		{
			Lock lock(m_mutex);

			if (isFull())
				return false;

			unsafePush(value);
		}

		m_not_empty.signal();
		return true;
	}

	template <typename T>
	bool BoundedMessageQueue<T>::timedPush(const value_type & value, const unsigned long ms)
	{
		{
			Lock lock(m_mutex);

			if (isFull())
				m_not_full.wait(&m_mutex, std::chrono::seconds(ms));

			if (isFull())
				return false;

			unsafePush(value);
		}

		m_not_empty.signal();

		return true;
	}



	template <typename T>
	typename BoundedMessageQueue<T>::value_type BoundedMessageQueue<T>::pop()
	{
		value_type value;

		{
			Lock lock(m_mutex);

			while (isEmpty())
				m_not_empty.wait(&m_mutex);

			value = unsafePop();
		}

		m_not_full.signal();

		return value;
	}



	template <class T>
	typename BoundedMessageQueue<T>::value_type BoundedMessageQueue<T>::tryPop(bool & is_empty)
	{
		value_type value;

		{
			Lock lock(m_mutex);

			is_empty = isEmpty();

			if (is_empty)
				return value_type();

			value = unsafePop();
		}

		m_not_full.signal();

		return value;
	}



	template <class T>
	typename BoundedMessageQueue<T>::value_type BoundedMessageQueue<T>::timedPop(bool & is_empty, const unsigned long ms)
	{
		value_type value;

		{
			Lock lock(m_mutex);

			if (isEmpty())
				m_not_empty.wait(&m_mutex, ms);

			is_empty = isEmpty();

			if (is_empty)
				return value_type();

			value = unsafePop();
		}

		m_not_full.signal();

		return value;
	}



	template <typename T>
	inline bool BoundedMessageQueue<T>::isFull() const
	{
		return m_size == m_buffer.size();
	}

	template <typename T>
	inline bool BoundedMessageQueue<T>::isEmpty() const
	{
		return m_size == 0;
	}


	template <typename T>
	inline void BoundedMessageQueue<T>::unsafePush(const value_type & value)
	{
		assert(!isFull());

		m_buffer[m_end++] = value;

		if (m_end == m_buffer.size())
			m_end = 0;

		++m_size;
	}

	template <typename T>
	inline  BoundedMessageQueue<T>::value_type BoundedMessageQueue<T>::unsafePop()
	{
		assert(!isEmpty());

		const size_t pos = m_begin;

		++m_begin;
		--m_size;

		if (m_begin == m_buffer.size())
			m_begin = 0;

		return m_buffer[pos];
	}

}


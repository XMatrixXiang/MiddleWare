/**************************************************************************
**  Copyright (C) 20018-2019 �����ݰ������Ƽ����޹�˾, All Rights Reserved
**
**  �ļ�����  NctBoundedMessageQueue.h
**  �����ߣ�  ����ΰ
**  �������ڣ�2018��4��26��
**  ��ϸ��Ϣ��Բ�ζ���
**
**
***************************************************************************/
#pragma once;

#include "Threading/NctThreadDefines.h"
#include "Prerequisites/NctPrerequisitesUtil.h"

namespace Nct
{
	/** ��Ϣ���� */
	template <typename T>
	class BoundedMessageQueue
	{
	public:

		typedef T value_type;
		typedef size_t size_type;
		
		/**
		 * @param[in] capacity ��������
		 */
		explicit BoundedMessageQueue(size_type capacity);
		~BoundedMessageQueue();

		void clear();

		/**
		* @param[in] destructor ʹ���ض�����������������ǰ���е�Ԫ�أ����Ҵ���not full �ź�
		*/
		template <typename Destructor>
		void flush(const Destructor destructor);

		/**
		* ��������������������һֱ�ȴ���ֱ����������
		* @param[in] value ��value���
		*/
		void push(const value_type & value);

		/**
		* ������������������ֱ�ӷ���false,���򷵻�true
		* @param[in] value ��value���
		*/
		bool tryPush(const value_type & value);

		/**
		* ��ָ����msʱ���ڽ�����ӳ��ԣ�����޷�����򷵻�false�����򷵻�true
		* @param[in] value ��value���
		* @param[in] ms    ���ȴ�ʱ�����
		*/
		bool timedPush(const value_type & value, unsigned long ms);

		/** ����*/
		value_type pop();

		/**
		* ���Գ���
		* @param[out] is_empty  �Ƿ���Գɹ�����	
		*/
		value_type tryPop(bool & is_empty);

		/**
		* ��ָ����ʱ���ڳ��Գ���
		* @param[in] ms  �޶�ʱ��
		* @param[out] is_empty  �Ƿ���Գɹ�����
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
		// ��ǰ��Ч��Ϣ�洢����ʼ����
		size_type     mBegin;
		// ��ǰ��Ч��Ϣ�洢����������
		size_type     mEnd;
		// ��ǰ��Ϣ��
		size_type     mSize;
		// ������
		Mutex         mMutex;
		// �����ź�
		Signal        mNotEmpty;
		// �����ź�
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



#pragma once

#include <atomic>

namespace Nct
{	
	class SpinLock
	{
	public:
		SpinLock()
		{
			mLock.clear(std::memory_order_relaxed);
		}		
		void lock()
		{
			while(mLock.test_and_set(std::memory_order_acquire))
			{ }
		}		
		void unlock()
		{
			mLock.clear(std::memory_order_release);
		}

	private:
		std::atomic_flag mLock;
	};
	
	class ScopedSpinLock
	{
	public:
		ScopedSpinLock(SpinLock& spinLock)
			:mSpinLock(spinLock)
		{
			mSpinLock.lock();
		}

		~ScopedSpinLock()
		{
			mSpinLock.unlock();
		}

	private:
		SpinLock& mSpinLock;
	};	
}
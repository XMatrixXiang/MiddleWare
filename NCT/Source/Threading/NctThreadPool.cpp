


#include "Threading/NctThreadPool.h"
#include "Threading/NctThreadDefines.h"
#include "Error/NctException.h"
#include "windows.h"


#pragma warning(disable: 4509)


namespace Nct
{
	HThread::HThread(ThreadPool* pool, UINT32 threadId)
		:mThreadId(threadId), mPool(pool)
	{ }

	void HThread::blockUntilComplete()
	{
		PooledThread* parentThread = nullptr;

		{
			NctLock lock(mPool->mMutex);

			for (auto& thread : mPool->mThreads)
			{
				if (thread->getId() == mThreadId)
				{
					parentThread = thread;
					break;
				}
			}
		}

		if (parentThread != nullptr)
		{
			NctLock lock(parentThread->mMutex);

			if (parentThread->mId == mThreadId)
			{
				while (!parentThread->mIdle)
					parentThread->mWorkerEndedCond.wait(lock);
			}
		}
	}

	void PooledThread::initialize()
	{
		mThread = new NctThread(std::bind(&PooledThread::run, this));

		NctLock lock(mMutex);

		while(!mThreadStarted)
			mStartedCond.wait(lock);
	}

	void PooledThread::start(std::function<void()> workerMethod, UINT32 id)
	{
		{
			NctLock lock(mMutex);

			mWorkerMethod = workerMethod;
			mIdle = false;
			mIdleTime = std::time(nullptr);
			mThreadReady = true;
			mId = id;
		}

		mReadyCond.notify_one();
	}

	void PooledThread::run()
	{
		onThreadStarted(mName);

		{
			NctLock lock(mMutex);
			mThreadStarted = true;
		}

		mStartedCond.notify_one();

		while(true)
		{
			std::function<void()> worker = nullptr;

			{
				{
					NctLock lock(mMutex);

					while (!mThreadReady)
						mReadyCond.wait(lock);

					worker = mWorkerMethod;
				}

				if (worker == nullptr)
				{
					onThreadEnded(mName);
					return;
				}
			}


			worker();

			{
				NctLock lock(mMutex);

				mIdle = true;
				mIdleTime = std::time(nullptr);
				mThreadReady = false;
				mWorkerMethod = nullptr; // Make sure to clear as it could have bound shared pointers and similar

				mWorkerEndedCond.notify_one();
			}
		}
	}

	void PooledThread::destroy()
	{
		blockUntilComplete();

		{
			NctLock lock(mMutex);
			mWorkerMethod = nullptr;
			mThreadReady = true;
		}

		mReadyCond.notify_one();
		mThread->join();
		delete mThread;
	}

	void PooledThread::blockUntilComplete()
	{
		NctLock lock(mMutex);

		while (!mIdle)
			mWorkerEndedCond.wait(lock);
	}

	bool PooledThread::isIdle()
	{
		NctLock lock(mMutex);

		return mIdle;
	}

	time_t PooledThread::idleTime()
	{
		NctLock lock(mMutex);

		return (time(nullptr) - mIdleTime);
	}

	void PooledThread::setName(const std::string& name)
	{
		mName = name;
	}

	UINT32 PooledThread::getId() const
	{
		NctLock lock(mMutex);

		return mId;
	}

	ThreadPool::ThreadPool(std::uint32_t threadCapacity, std::uint32_t maxCapacity, std::uint32_t idleTimeout)
		:mDefaultCapacity(threadCapacity), mMaxCapacity(maxCapacity), mIdleTimeout(idleTimeout)
	{

	}

	ThreadPool::~ThreadPool()
	{
		stopAll();
	}

	HThread ThreadPool::run(const std::string& name, std::function<void()> workerMethod)
	{
		PooledThread* thread = getThread(name);
		thread->start(workerMethod, mUniqueId++);

		return HThread(this, thread->getId());
	}

	void ThreadPool::stopAll()
	{
		NctLock lock(mMutex);
		for(auto& thread : mThreads)
		{
			destroyThread(thread);
		}

		mThreads.clear();
	}

	void ThreadPool::clearUnused()
	{
		NctLock lock(mMutex);
		mAge = 0;

		if(mThreads.size() <= mDefaultCapacity)
			return;

		std::vector<PooledThread*> idleThreads;
		std::vector<PooledThread*> expiredThreads;
		std::vector<PooledThread*> activeThreads;

		idleThreads.reserve(mThreads.size());
		expiredThreads.reserve(mThreads.size());
		activeThreads.reserve(mThreads.size());

		for(auto& thread : mThreads)
		{
			if(thread->isIdle())
			{
				if(thread->idleTime() >= mIdleTimeout)
					expiredThreads.push_back(thread);
				else
					idleThreads.push_back(thread);
			}
			else
				activeThreads.push_back(thread);
		}

		idleThreads.insert(idleThreads.end(), expiredThreads.begin(), expiredThreads.end());
		std::uint32_t limit = min((std::uint32_t)idleThreads.size(), mDefaultCapacity);

		std::uint32_t i = 0;
		mThreads.clear();

		for(auto& thread : idleThreads)
		{
			if (i < limit)
			{
				mThreads.push_back(thread);
				i++;
			}
			else
				destroyThread(thread);
		}

		mThreads.insert(mThreads.end(), activeThreads.begin(), activeThreads.end());
	}

	void ThreadPool::destroyThread(PooledThread* thread)
	{
		thread->destroy();
		delete thread;
	}

	PooledThread* ThreadPool::getThread(const std::string& name)
	{
		UINT32 age = 0;
		{
			NctLock lock(mMutex);
			age = ++mAge;
		}

		if(age == 32)
			clearUnused();

		PooledThread* newThread = nullptr;
		NctLock lock(mMutex);

		for(auto& thread : mThreads)
		{
			if(thread->isIdle())
			{
				thread->setName(name);
				return thread;
			}
		}

		if(newThread == nullptr)
		{
			if(mThreads.size() >= mMaxCapacity)
				NCT_EXCEPT(InvalidStateException, "Unable to create a new thread in the pool because maximum capacity has been reached.");

			newThread = createThread(name);
			mThreads.push_back(newThread);
		}

		return newThread;
	}

	UINT32 ThreadPool::getNumAvailable() const
	{
		UINT32 numAvailable = 0;

		NctLock lock(mMutex);
		for(auto& thread : mThreads)
		{
			if(thread->isIdle())
				numAvailable++;
		}

		return numAvailable;
	}

	UINT32 ThreadPool::getNumActive() const
	{
		UINT32 numActive = 0;

		NctLock lock(mMutex);
		for(auto& thread : mThreads)
		{
			if(!thread->isIdle())
				numActive++;
		}

		return numActive;
	}

	UINT32 ThreadPool::getNumAllocated() const
	{
		NctLock lock(mMutex);

		return (UINT32)mThreads.size();
	}
}

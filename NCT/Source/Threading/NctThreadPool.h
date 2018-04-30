
#pragma once

#include "Prerequisites/NctPrerequisitesUtil.h"
#include "Utility/NctModule.h"
#include "Threading/NctThreadDefines.h"

namespace Nct
{

	class ThreadPool;
	
	class NCT_EXPORT HThread
	{
	public:
		HThread() = default;;
		HThread(ThreadPool* pool, std::uint32_t threadId);
		
		void blockUntilComplete();

	private:
		std::uint32_t mThreadId = 0;
		ThreadPool* mPool = nullptr;
	};

	class  PooledThread
	{
	public:
		PooledThread(const std::string& name)
			:mName(name)
		{ }

		virtual ~PooledThread() = default;
	
		void initialize();

		void start(std::function<void()> workerMethod, std::uint32_t id);

	
		void destroy();
		
		bool isIdle();
		
		time_t idleTime();
		
		void setName(const std::string& name);
		
		std::uint32_t getId() const;
		
		void blockUntilComplete();
		
		virtual void onThreadStarted(const std::string& name) = 0;
		
		virtual void onThreadEnded(const std::string& name) = 0;

	protected:
		friend class HThread;
		
		void run();

	protected:
		std::function<void()> mWorkerMethod;
		std::string mName;
		std::uint32_t mId = 0;
		bool mIdle = true;
		bool mThreadStarted = false;
		bool mThreadReady = false;

		time_t mIdleTime = 0;

		NctThread* mThread;
		mutable NctMutex mMutex;
		NctSignal mStartedCond;
		NctSignal mReadyCond;
		NctSignal mWorkerEndedCond;
	};


	template<class ThreadPolicy>
	class TPooledThread : public PooledThread
	{
	public:
		using PooledThread::PooledThread;
		
		void onThreadStarted(const std::string& name) override
		{
			ThreadPolicy::onThreadStarted(name);
		}		
		void onThreadEnded(const std::string& name) override
		{
			ThreadPolicy::onThreadEnded(name);
		}
	};

	class  ThreadPool : public Module<ThreadPool>
	{
	public:
	
		ThreadPool(std::uint32_t threadCapacity, std::uint32_t maxCapacity = 16, std::uint32_t idleTimeout = 60);
		virtual ~ThreadPool();

		
		HThread run(const std::string& name, std::function<void()> workerMethod);

		void stopAll();
		
		void clearUnused();
		
		std::uint32_t getNumAvailable() const;
		
		std::uint32_t getNumActive() const;


		std::uint32_t getNumAllocated() const;

	protected:
		friend class HThread;

		std::vector<PooledThread*> mThreads;
		
		virtual PooledThread* createThread(const std::string & name) = 0;
		
		void destroyThread(PooledThread* thread);

		PooledThread* getThread(const std::string & name);

		std::uint32_t mDefaultCapacity;
		std::uint32_t mMaxCapacity;
		std::uint32_t mIdleTimeout;
		std::uint32_t mAge = 0;

		std::atomic_uint mUniqueId;
		mutable NctMutex mMutex;
	};

	class ThreadNoPolicy
	{
	public:
		static void onThreadStarted(const std::string& name) { }
		static void onThreadEnded(const std::string& name) { }
	};

	template<class ThreadPolicy = ThreadNoPolicy>
	class TThreadPool : public ThreadPool
	{
	public:
		TThreadPool(std::uint32_t threadCapacity, std::uint32_t maxCapacity = 16, std::uint32_t idleTimeout = 60)
			:ThreadPool(threadCapacity, maxCapacity, idleTimeout)
		{

		}

	protected:		
		PooledThread* createThread(const std::string& name) override
		{
			PooledThread* newThread = bs_new<TPooledThread<ThreadPolicy>>(name);
			newThread->initialize();

			return newThread;
		}
	};
}

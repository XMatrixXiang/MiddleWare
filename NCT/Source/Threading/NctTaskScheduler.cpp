#include "Threading/NctTaskScheduler.h"
#include "Threading/NctThreadPool.h"

namespace Nct
{
	Task::Task(const PrivatelyConstruct& dummy, const std::string& name, std::function<void()> taskWorker,
		TaskPriority priority, std::shared_ptr<Task> dependency)
		: mName(name), mPriority(priority), mTaskWorker(std::move(taskWorker)), mTaskDependency(std::move(dependency))
	{

	}

	std::shared_ptr<Task> Task::create(const std::string& name, std::function<void()> taskWorker, TaskPriority priority,
		std::shared_ptr<Task> dependency)
	{
		return std::make_shared<Task>(PrivatelyConstruct(), name, std::move(taskWorker), priority, std::move(dependency));
	}

	bool Task::isComplete() const
	{
		return mState == 2;
	}

	bool Task::isCanceled() const
	{
		return mState == 3;
	}

	void Task::wait()
	{
		if(mParent != nullptr)
			mParent->waitUntilComplete(this);
	}

	void Task::cancel()
	{
		mState = 3;
	}

	TaskScheduler::TaskScheduler()
		:mTaskQueue(&TaskScheduler::taskCompare)
	{
		mMaxActiveTasks = NCT_THREAD_HARDWARE_CONCURRENCY;

		mTaskSchedulerThread = ThreadPool::instance().run("TaskScheduler", std::bind(&TaskScheduler::runMain, this));
	}

	TaskScheduler::~TaskScheduler()
	{		
		{
			NctLock activeTaskLock(mReadyMutex);

			while (mActiveTasks.size() > 0)
			{
				std::shared_ptr<Task> task = mActiveTasks[0];
				activeTaskLock.unlock();

				task->wait();
				activeTaskLock.lock();
			}
		}
		
		{
			NctLock lock(mReadyMutex);

			mShutdown = true;
		}

		mTaskReadyCond.notify_one();

		mTaskSchedulerThread.blockUntilComplete();
	}

	void TaskScheduler::addTask(std::shared_ptr<Task> task)
	{
		NctLock lock(mReadyMutex);

		assert(task->mState != 1 && "Task is already executing, it cannot be executed again until it finishes.");

		task->mParent = this;
		task->mTaskId = mNextTaskId++;
		task->mState.store(0); 

		mCheckTasks = true;
		mTaskQueue.insert(std::move(task));
		
		mTaskReadyCond.notify_one();
	}

	void TaskScheduler::addWorker()
	{
		NctLock lock(mReadyMutex);

		mMaxActiveTasks++;
		
		mTaskReadyCond.notify_one();
	}

	void TaskScheduler::removeWorker()
	{
		NctLock lock(mReadyMutex);

		if(mMaxActiveTasks > 0)
			mMaxActiveTasks--;
	}

	void TaskScheduler::runMain()
	{
		while(true)
		{
			NctLock lock(mReadyMutex);

			while((!mCheckTasks || (std::uint32_t)mActiveTasks.size() >= mMaxActiveTasks) && !mShutdown)
				mTaskReadyCond.wait(lock);

			mCheckTasks = false;

			if(mShutdown)
				break;

			for(auto iter = mTaskQueue.begin(); iter != mTaskQueue.end();)
			{
				if ((std::uint32_t)mActiveTasks.size() >= mMaxActiveTasks)
					break;	

				std::shared_ptr<Task> curTask = *iter;

				if(curTask->isCanceled())
				{
					iter = mTaskQueue.erase(iter);
					continue;
				}

				if(curTask->mTaskDependency != nullptr && !curTask->mTaskDependency->isComplete())
				{
					++iter;
					continue;
				}

				iter = mTaskQueue.erase(iter);

				curTask->mState.store(1);
				mActiveTasks.push_back(curTask);

				ThreadPool::instance().run(curTask->mName, std::bind(&TaskScheduler::runTask, this, curTask));
			}
		}
	}

	void TaskScheduler::runTask(std::shared_ptr<Task> task)
	{
		task->mTaskWorker();

		{
			NctLock lock(mReadyMutex);

			auto findIter = std::find(mActiveTasks.begin(), mActiveTasks.end(), task);
			if (findIter != mActiveTasks.end())
				mActiveTasks.erase(findIter);
		}

		{
			NctLock lock(mCompleteMutex);
			task->mState.store(2);

			mTaskCompleteCond.notify_all();
		}
		
		{
			NctLock lock(mReadyMutex);

			mCheckTasks = true;
			mTaskReadyCond.notify_one();
		}
	}

	void TaskScheduler::waitUntilComplete(const Task* task)
	{
		if(task->isCanceled())
			return;

		{
			NctLock lock(mCompleteMutex);

			while(!task->isComplete())
			{
				addWorker();
				mTaskCompleteCond.wait(lock);
				removeWorker();
			}
		}
	}

	bool TaskScheduler::taskCompare(const std::shared_ptr<Task>& lhs, const std::shared_ptr<Task>& rhs)
	{		
		if(lhs->mPriority > rhs->mPriority)
			return true;
		
		return lhs->mTaskId < rhs->mTaskId;
	}
}

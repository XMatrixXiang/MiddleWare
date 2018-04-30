
#pragma once

#include "Prerequisites/NctPrerequisitesUtil.h"
#include "Threading/NctThreadDefines.h"
#include "Utility/NctModule.h"
#include "Threading/NctThreadPool.h"

namespace Nct
{

	class TaskScheduler;
	
	enum class TaskPriority
	{
		VeryLow = 98,
		Low = 99,
		Normal = 100,
		High = 101,
		VeryHigh = 102
	};


	class  Task
	{
		struct PrivatelyConstruct {};

	public:
		Task(const PrivatelyConstruct& dummy, const std::string& name, std::function<void()> taskWorker,
			TaskPriority priority, std::shared_ptr<Task> dependency);


		static std::shared_ptr<Task> create(const std::string& name, std::function<void()> taskWorker, 
			TaskPriority priority = TaskPriority::Normal, std::shared_ptr<Task> dependency = nullptr);
		
		bool isComplete() const;
		
		bool isCanceled() const;

		void wait();
		
		void cancel();

	private:
		friend class TaskScheduler;

		std::string mName;
		TaskPriority mPriority;
		std::uint32_t mTaskId = 0;
		std::function<void()> mTaskWorker;
		std::shared_ptr<Task> mTaskDependency;
		std::atomic<std::uint32_t> mState{0}; /**< 0 - Inactive, 1 - In progress, 2 - Completed, 3 - Canceled */

		TaskScheduler* mParent = nullptr;
	};

	
	class  TaskScheduler : public Module<TaskScheduler>
	{
	public:
		TaskScheduler();
		~TaskScheduler();
		
		void addTask(std::shared_ptr<Task> task);
		
		void addWorker();
		
		void removeWorker();
		
		std::uint32_t getNumWorkers() const { return mMaxActiveTasks; }
	protected:
		friend class Task;
		
		void runMain();
		
		void runTask(std::shared_ptr<Task> task);
		
		void waitUntilComplete(const Task* task);
		
		static bool taskCompare(const std::shared_ptr<Task>& lhs, const std::shared_ptr<Task>& rhs);

		HThread mTaskSchedulerThread;
		std::set<std::shared_ptr<Task>, std::function<bool(const std::shared_ptr<Task>&, const std::shared_ptr<Task>&)>> mTaskQueue;
		std::vector<std::shared_ptr<Task>> mActiveTasks;
		std::uint32_t mMaxActiveTasks = 0;
		std::uint32_t mNextTaskId = 0;
		bool mShutdown = false;
		bool mCheckTasks = false;

		NctMutex mReadyMutex;
		NctMutex mCompleteMutex;
		NctSignal mTaskReadyCond;
		NctSignal mTaskCompleteCond;
	};	
}


#include "Threading/NctAsyncOp.h"
#include "Threading/NctThreadDefines.h"

namespace Nct
{
	bool AsyncOp::hasCompleted() const 
	{ 
		return mData->mIsCompleted.load(std::memory_order_acquire);
	}

	void AsyncOp::_completeOperation(Any returnValue) 
	{ 
		mData->mReturnValue = returnValue; 
		mData->mIsCompleted.store(true, std::memory_order_release);

		if (mSyncData != nullptr)
			mSyncData->mCondition.notify_all();
	}

	void AsyncOp::_completeOperation() 
	{ 
		mData->mIsCompleted.store(true, std::memory_order_release);

		if (mSyncData != nullptr)
			mSyncData->mCondition.notify_all();
	}

	void AsyncOp::blockUntilComplete() const
	{
		if (mSyncData == nullptr)
		{
			//"No sync data is available. Cannot block until AsyncOp is complete."
			return;
		}

		NctLock lock(mSyncData->mMutex);
		while (!hasCompleted())
			mSyncData->mCondition.wait(lock);
	}
}
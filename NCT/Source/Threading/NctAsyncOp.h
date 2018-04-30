
#pragma once

#include "Prerequisites/NctPrerequisitesUtil.h"
#include "Utility/NctAny.h"
#include "Threading/NctThreadDefines.h"

namespace Nct
{	
	class  AsyncOpSyncData
	{
	public:
		NctMutex mMutex;
		NctSignal mCondition;
	};

	
	struct  AsyncOpEmpty {};

	
	class  AsyncOp
	{
	private:
		struct AsyncOpData
		{
			AsyncOpData() = default;

			Any mReturnValue;
			volatile std::atomic<bool> mIsCompleted{false};
		};

	public:
		AsyncOp()
			:mData(std::make_shared<AsyncOpData>())
		{ }

		AsyncOp(AsyncOpEmpty empty)
		{ }

		AsyncOp(const std::shared_ptr<AsyncOpSyncData>& syncData)
			:mData(std::make_shared<AsyncOpData>()), mSyncData(syncData)
		{ }

		AsyncOp(AsyncOpEmpty empty, const std::shared_ptr<AsyncOpSyncData>& syncData)
			:mSyncData(syncData)
		{ }
		
		bool hasCompleted() const;
		
		void blockUntilComplete() const;
		
		template <typename T>
		T getReturnValue() const 
		{ 		
			return any_cast<T>(mData->mReturnValue);
		}
		
		Any getGenericReturnValue() const { return mData->mReturnValue; }

	public: 
		void _completeOperation(Any returnValue);
		
		void _completeOperation();
		
	private:
		std::shared_ptr<AsyncOpData> mData;
		std::shared_ptr<AsyncOpSyncData> mSyncData;
	};	
}

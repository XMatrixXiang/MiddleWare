
#include "NctTaskBase.h"

namespace Nct
{
	void TaskBase::terminate()
	{
		mTerminate = true;
		if (mTask == nullptr) return;
		mTask->wait();
	}
}
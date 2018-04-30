#include "NctDecodeMsgTask.h"
#include "Message/NctMessageManager.h"


namespace Nct
{
	DecodeMsgTask::DecodeMsgTask()
	{
		mMsgBuf = std::make_shared<MsgBuffer>();
	}
	DecodeMsgTask::~DecodeMsgTask()
	{
		mMsgBuf = nullptr;
	}
	void DecodeMsgTask::run()
	{
		do
		{
			NCT_THREAD_SLEEP(mThreadSleep);
			if (mTerminate == true) break;			
			bool ret = getMsg();
			if (ret == false) continue;
			decodeMsg();
		} while (mTerminate == false);		
	}


}
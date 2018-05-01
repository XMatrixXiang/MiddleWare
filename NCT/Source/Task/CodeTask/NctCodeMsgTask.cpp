#include "NctCodeMsgTask.h"
#include "Message/NctMessageManager.h"


namespace Nct
{
	CodeMsgTask::CodeMsgTask()
	{
		mMsgBuf = std::make_shared<MsgBuffer>();
	}
	CodeMsgTask::~CodeMsgTask()
	{
		mMsgBuf = nullptr;
	}
	void CodeMsgTask::run()
	{
		do
		{
			NCT_THREAD_SLEEP(mThreadSleep);
			if (mTerminate == true) break;			
			bool ret = codeMsg();
			if (ret == false) continue;
			setMsg();
		} while (mTerminate == false);		
	}


}
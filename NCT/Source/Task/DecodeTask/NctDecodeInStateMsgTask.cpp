
#include "Task/DecodeTask/NctDecodeInStateMsgTask.h"
#include "Message/NctMessageManager.h"


namespace Nct
{
	bool DecodeInStateMsgTask::getMsg()
	{
		return MessageManager::instance().timePopInStateMsg(*mMsgBuf, 10);
	}
	void DecodeInStateMsgTask::decodeMsg()
	{
	}
}
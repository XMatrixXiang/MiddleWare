#include "NctCodeOutRealMsgTask.h"
#include "Message/NctMessageManager.h"


namespace Nct
{	

    void CodeOutRealMsgTask::setMsg()
	{		
		MessageManager::instance().setOutRealMsg(reinterpret_cast<const char *>(mMsgBuf->getCurBufferPtr()),mMsgBuf->getDataSize());
	}

	bool CodeOutRealMsgTask::codeMsg()
	{

	}
}
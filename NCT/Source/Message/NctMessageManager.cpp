#include "NctMessageManager.h"


namespace Nct
{
	void MessageManager::pushInStateMsg(const char * inMsg, const std::uint32_t size)
	{
		std::shared_ptr<MsgBuffer> tempBuf = std::make_shared<MsgBuffer>();
		tempBuf->setRawData(inMsg, size);
		mInStateMsgQueue.push(tempBuf);
	}
	
	void MessageManager::popInStateMsg(MsgBuffer & inMsg)
	{
		std::shared_ptr<MsgBuffer> tempBuf = mInStateMsgQueue.pop();
		inMsg.setRawData(reinterpret_cast<char*>(tempBuf->getCurBufferPtr()), tempBuf->getDataSize());
	}
	bool MessageManager::tryPopInStateMsg(MsgBuffer & inMsg)
	{
		bool ret = false;
		std::shared_ptr<MsgBuffer> tempBuf = mInStateMsgQueue.tryPop(ret);
		if (ret == true)
		{
			inMsg.setRawData(reinterpret_cast<char*>(tempBuf->getCurBufferPtr()), tempBuf->getDataSize());
		}
		else
		{
			inMsg.setRawData(nullptr, 0);
		}
		return ret;
	}
	bool MessageManager::timePopInStateMsg(MsgBuffer & inMsg, std::uint32_t time)
	{
		bool ret = false;
		std::shared_ptr<MsgBuffer> tempBuf = mInStateMsgQueue.timedPop(ret,time);
		if (ret == true)
		{
			inMsg.setRawData(reinterpret_cast<char*>(tempBuf->getCurBufferPtr()), tempBuf->getDataSize());
		}
		else
		{
			inMsg.setRawData(nullptr, 0);
		}
		return ret;
	}
	void MessageManager::pushOutStateMsg(const char * inMsg, const std::uint32_t size)
	{
		std::shared_ptr<MsgBuffer> tempBuf = std::make_shared<MsgBuffer>();
		tempBuf->setRawData(inMsg, size);
		mOutStateMsgQueue.push(tempBuf);
		
	}
	void  MessageManager::popOutStateMsg(MsgBuffer & inMsg)
	{
		std::shared_ptr<MsgBuffer> tempBuf = mOutStateMsgQueue.pop();
		inMsg.setRawData(reinterpret_cast<char*>(tempBuf->getCurBufferPtr()), tempBuf->getDataSize());
	}
	bool MessageManager::tryPopOutStateMsg(MsgBuffer & inMsg)
	{
		bool ret = false;
		std::shared_ptr<MsgBuffer> tempBuf = mOutStateMsgQueue.tryPop(ret);
		if (ret == true)
		{
			inMsg.setRawData(reinterpret_cast<char*>(tempBuf->getCurBufferPtr()), tempBuf->getDataSize());
		}
		else
		{
			inMsg.setRawData(nullptr, 0);
		}
		return ret;
	}
	bool MessageManager::timePopOutStateMsg(MsgBuffer & inMsg, std::uint32_t time)
	{
		bool ret = false;
		std::shared_ptr<MsgBuffer> tempBuf = mOutStateMsgQueue.tryPop(ret);
		if (ret == true)
		{
			inMsg.setRawData(reinterpret_cast<char*>(tempBuf->getCurBufferPtr()), tempBuf->getDataSize());
		}
		else
		{
			inMsg.setRawData(nullptr, 0);
		}
		return ret;
	}
	void MessageManager::setInRealMsg(const char * inMsg,const std::uint32_t size)
	{
		NctLock lock(mInRealMsgMutex);
		mInRealMsg->setRawData(inMsg,size);
	}

	void MessageManager::getInRealMsg(MsgBuffer & inMsg)
	{
		NctLock lock(mInRealMsgMutex);
		inMsg.setRawData(reinterpret_cast<char *>(mInRealMsg->getCurBufferPtr()),mInRealMsg->getDataSize());
		mInRealMsg->setRawData(nullptr, 0);
	}

	void MessageManager::setOutRealMsg(const char * inMsg,const std::uint32_t size)
	{
		NctLock lock(mOutRealMsgMutex);
		mOutRealMsg->setRawData(inMsg, size);
	}

	void MessageManager::getOutRealMsg(MsgBuffer & inMsg)
	{
		NctLock lock(mOutRealMsgMutex);
		inMsg.setRawData(reinterpret_cast<char *>(mOutRealMsg->getCurBufferPtr()), mOutRealMsg->getDataSize());
		mOutRealMsg->setRawData(nullptr, 0);
	}

	void MessageManager::onStartUp()
	{
		mInStateMsgQueue.clear();
		mOutStateMsgQueue.clear();
		mInRealMsg = std::make_shared<MsgBuffer>();
		mOutRealMsg = std::make_shared<MsgBuffer>();
	}
	void MessageManager::onShutDown()
	{
		mInStateMsgQueue.clear();
		mOutStateMsgQueue.clear();
		mInRealMsg = nullptr;
		mOutRealMsg = nullptr;
	}
}
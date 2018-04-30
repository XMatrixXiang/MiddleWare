/**************************************************************************
**  Copyright (C) 20018-2019 北京捷安军工科技有限公司, All Rights Reserved
**
**  文件名：  NctThreadDefines.h
**  创建者：  闫相伟
**  创建日期：2018年4月26日
**  详细信息：C++线程库类型封装
**
**
***************************************************************************/
#pragma once

#define BS_AUTO_MUTEX_NAME mutex

#include <thread>
#include <chrono>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include "Threading/NctSpinLock.h"


#define NCT_THREAD_HARDWARE_CONCURRENCY std::thread::hardware_concurrency()

#define NCT_THREAD_CURRENT_ID std::this_thread::get_id()

#define NCT_THREAD_SLEEP(ms) std::this_thread::sleep_for(std::chrono::milliseconds(ms));

#define NCT_THREADLOCAL __declspec(thread)


using NctMutex = std::mutex;

using NctRecursiveMutex = std::recursive_mutex;

using NctSignal = std::condition_variable;

using NctThread = std::thread;

using NctThreadId = std::thread::id;

using NctLock = std::unique_lock<NctMutex>;

using NctRecursiveLock = std::unique_lock<NctRecursiveMutex>;


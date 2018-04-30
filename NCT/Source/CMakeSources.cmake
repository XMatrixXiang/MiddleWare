set(NCT_ALLOCATORS_INC
	"Allocators/NctStaticAlloc.h"
	"Allocators/NctFreeAlloc.h"
	"Allocators/NctFrameAlloc.h"
	"Allocators/NctGroupAlloc.h"
	"Allocators/NctMemAllocProfiler.h"
	"Allocators/NctMemoryAllocator.h"
	"Allocators/NctPoolAlloc.h"
	"Allocators/NctStackAlloc.h"
	"Allocators/NctNonCopyable.h"
)

set(NCT_ALLOCATORS_SRC	
	"Allocators/NctFrameAlloc.cpp"
	"Allocators/NctMemoryAllocator.cpp"
	"Allocators/NctStackAlloc.cpp"
)

set(NCT_MESSAGE_INC
 "Message/NctMessageQueue.h"
 "Message/NctBoundedMessageQueue.h"
 "Message/NctMessageManager.h"
 "Message/NctMsgBuffer.h"
)

set(NET_MESSAGE_SRC  
	"Message/NctMessageManager.cpp"
)

set(NCT_THREADING_INC
	"Threading/NctAsyncOp.h"
	"Threading/NctSpinLock.h"
	"Threading/NctTaskScheduler.h"
	"Threading/NctThreadDefines.h"
	"Threading/NctThreadPool.h"
)
set(NCT_THREADING_SRC
	"Threading/NctAsyncOp.cpp"
	"Threading/NctTaskScheduler.cpp"
	"Threading/NctThreadPool.cpp"
)
set(NCT_PREREQUISTIES_INC
	"Prerequisites/NctPrerequisitesUtil.h"
)
set(NCT_UTILITY_INC
	"Utility/NctAny.h"
	"Utility/NctModule.h"
)

set(NCT_ERROR_INC
	"Error/NctException.h"
)

set(NCT_TASK_INC
	"Task/NctTaskBase.h"	
	"Task/NctPersistentTaskBase.h"	
)
set(NCT_TASK_SRC
	"Task/NctTaskBase.cpp"	
	"Task/NctPersistentTaskBase.cpp"	
)

set(NCT_TASK_DECODE_INC
	"Task/DecodeTask/NctDecodeMsgTask.h"
	"Task/DecodeTask/NctDecodeInStateMsgTask.h"
	"Task/DecodeTask/NctDecodeInRealMsgTask.h"
)
set(NCT_TASK_DECODE_SRC
	"Task/DecodeTask/NctDecodeMsgTask.cpp"
	"Task/DecodeTask/NctDecodeInStateMsgTask.cpp"
	"Task/DecodeTask/NctDecodeInRealMsgTask.cpp"
)

set(NCT_TASK_CODE_INC
	"Task/CodeTask/NctCodeMsgTask.h"
	"Task/CodeTask/NctCodeOutRealMsgTask.h"
	"Task/CodeTask/NctCodeOutStateMsgTask.h"
)
set(NCT_TASK_CODE_SRC
	"Task/CodeTask/NctCodeMsgTask.cpp"
	"Task/CodeTask/NctCodeOutRealMsgTask.cpp"
	"Task/CodeTask/NctCodeOutStateMsgTask.cpp"
)


source_group("Allocators" FILES ${NCT_ALLOCATORS_INC} ${NCT_ALLOCATORS_SRC})
source_group("Message" FILES ${NET_MESSAGE_SRC} ${NCT_MESSAGE_INC})
source_group("Threading" FILES ${NCT_THREADING_SRC} ${NCT_THREADING_INC})
source_group("Prerequisites" FILES ${NCT_PREREQUISTIES_INC})
source_group("Utility" FILES ${NCT_UTILITY_INC})
source_group("Error" FILES ${NCT_ERROR_INC})
source_group("Task" FILES ${NCT_TASK_INC} ${NCT_TASK_SRC})
source_group("Task\\Decode" FILES ${NCT_TASK_DECODE_INC} ${NCT_TASK_DECODE_SRC})
source_group("Task\\Code" FILES ${NCT_TASK_CODE_INC} ${NCT_TASK_CODE_SRC})

set(NET_CHANNEL_TRANITION_SRC
	${NET_MESSAGE_SRC}
	${NCT_MESSAGE_INC}
	${NCT_THREADING_SRC}
	${NCT_THREADING_INC}
	${NCT_PREREQUISTIES_INC}
	${NCT_UTILITY_INC}
	${NCT_ERROR_INC}
	${NCT_TASK_INC}
	${NCT_TASK_SRC}
	${NCT_ALLOCATORS_INC}
	${NCT_ALLOCATORS_SRC}
	${NCT_TASK_DECODE_INC}
	${NCT_TASK_DECODE_SRC}
	${NCT_TASK_CODE_INC}
	${NCT_TASK_CODE_SRC}
)
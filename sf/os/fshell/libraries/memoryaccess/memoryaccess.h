// memoryaccess.h
// 
// Copyright (c) 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#ifndef __MemoryAccess_H__
#define __MemoryAccess_H__

#ifndef __KERNEL_MODE__
#include <e32std.h>
#endif
#include <u32std.h>

_LIT(KMemoryAccessName,"memoryAccess-fshell");

typedef TBuf8<KMaxFullName> TFullName8;
typedef TBuf8<KMaxFileName> TFileName8;

//size of buffer used for returning queues
const TInt KQueBufferLength = 1024; 

//Buffer used to pass pointer arrays, of pointers to kernel side objects
class TPointerArrayBuffer
	{
public:
	TUint8* iBuffer[KQueBufferLength];  //buffer holding up to KQueBufferLength items of the array or queue
	TUint iStartFrom; //the point in the array to start from
	TUint iCount; //the total number of entries written into iBuffer.  If KMaxTInt, there are more available
	};

//
//Structures used for communication with driver
//

class RProcess;

class TObjectInfoByIndexParams
	{
public:
	TObjectType iObjectType;
	TUint iObjectIndex;
	};
typedef TPckgBuf<TObjectInfoByIndexParams> TObjectInfoByIndexParamsBuf;

class TObjectInfoByPtrParams
	{
public:
	TObjectType iObjectType;
	TUint8* iObjectPtr;	
	};
typedef TPckgBuf<TObjectInfoByPtrParams> TObjectInfoByPtrParamsBuf;

class TObjectInfoByNameParams
	{
public:
	TObjectType iObjectType;
	TBuf8<KMaxFullName> iObjectName;
	};
typedef TPckgBuf<TObjectInfoByNameParams> TObjectInfoByNameParamsBuf;

class TObjectInfoByHandleParams
	{
public:
	TObjectType iObjectType;
	TInt iThreadId;	
	TInt iObjectHandle;	
	};
typedef TPckgBuf<TObjectInfoByHandleParams> TObjectInfoByHandleParamsBuf;
	
class TThreadMemoryAccessParams
	{
public:
	TUint iId;
	const TUint8* iAddr;
	TInt iSize;
	};
typedef TPckgBuf<TThreadMemoryAccessParams> TThreadMemoryAccessParamsBuf;

class TObjectKillParams
	{
public:
	TObjectType iObjectType;
	TUint iObjectId;
	TUint8* iObjectPtr;	
	TExitType iType;
	TInt iReason;
	TFullName8 iCategory;
	};
typedef TPckgBuf<TObjectKillParams> TObjectKillParamsBuf;

class TGetObjectAddressesParams
	{
public:
	TObjectType iObjectType;
	TFullName8 iOwningProcess;
	};
typedef TPckgBuf<TGetObjectAddressesParams> TGetObjectAddressesParamsBuf;

class TGetChunkAddressesParams
	{
public:
	TUint iControllingProcessId;
	};
typedef TPckgBuf<TGetChunkAddressesParams> TGetChunkAddressesParamsBuf;

class TObjectKernelInfo
	{
public:
	TUint8* iAddressOfKernelObject;
	TFullName8 iName;
	TFullName8 iFullName;
	TInt iAccessCount;
	TInt iUniqueID;
	TUint iProtection;
	TUint8* iAddressOfKernelOwner;
	};
typedef TPckgBuf<TObjectKernelInfo> TObjectKernelInfoBuf;

class TThreadKernelInfo : public TObjectKernelInfo
	{
public:
	TUint iThreadId;
	TUint8* iAddressOfOwningProcess;
	TInt iThreadPriority;
	TInt iUserStackSize;
	TUint32 iFlags;
	TLinAddr iSupervisorStack;
	TInt iSupervisorStackSize;
	TLinAddr iUserStackLimit;
	TInt iNThreadSuspendCount;
	inline TLinAddr UserStackBase() const { return iUserStackLimit + iUserStackSize; }
	};
typedef TPckgBuf<TThreadKernelInfo> TThreadKernelInfoBuf;

const TInt KNumEnvironmentSlots = 16;

class TProcessKernelInfo : public TObjectKernelInfo
	{ 
public:
	TUint iProcessId;
	TInt iPriority;
	TUint8* iAddressOfOwningProcess;
	TUint iCreatorId;
	TUint iSecurityZone;
	TUidType iUids;
	TInt iAttributes;
	TUint8* iAddressOfDataBssStackChunk;
	TPointerArrayBuffer iThreadQ;
	SSecurityInfo iProcessSecurityInfo;
	SSecurityInfo iProcessCreatorSecurityInfo;
	TBuf8<256> iCommandLine;
	TUint32 iFlags;
	TInt iEnvironmentData[KNumEnvironmentSlots];
	TUint iFirstThreadId;
	};
typedef TPckgBuf<TProcessKernelInfo> TProcessKernelInfoBuf;	 

class TChunkKernelInfo : public TObjectKernelInfo
	{ 
public:
	TInt iSize;
	TInt iMaxSize;
	TUint8* iBase;
	TInt iBottom;
	TInt iTop;
	TInt iAttributes;
	TInt iStartPos;
	TUint iControllingOwnerProcessId;
	TUint iRestrictions;
	TUint iMapAttr;
	TUint iChunkType;
	TUint8* iAddressOfOwningProcess;
	};
typedef TPckgBuf<TChunkKernelInfo> TChunkKernelInfoBuf;	 

class TLibraryKernelInfo : public TObjectKernelInfo
	{
public:
	TInt iMapCount;
	TUint8 iState;
	TUint8* iAddressOfCodeSeg;
	};
typedef TPckgBuf<TLibraryKernelInfo> TLibraryKernelInfoBuf;	 

class TSemaphoreKernelInfo : public TObjectKernelInfo
	{ 
public:
	TInt iCount;
	TUint8 iResetting;
	TPointerArrayBuffer iSuspendedQ;
	};
typedef TPckgBuf<TSemaphoreKernelInfo> TSemaphoreKernelInfoBuf;	 

class TMutexKernelInfo : public TObjectKernelInfo
	{
public:
	TInt iHoldCount;
	TInt iWaitCount;
	TUint8 iResetting;
	TUint8 iOrder;
	TPointerArrayBuffer iSuspendedQ;
	TPointerArrayBuffer iPendingQ;
	};
typedef TPckgBuf<TMutexKernelInfo> TMutexKernelInfoBuf;	 

class TTimerKernelInfo : public TObjectKernelInfo
	{
public:
	TUint8 iState;
	TUint8 iType;
	TLinAddr iClientStatus;
	};
typedef TPckgBuf<TTimerKernelInfo> TTimerKernelInfoBuf;	 

class TServerKernelInfo : public TObjectKernelInfo
	{
public:
	TUint8*	iAddressOfOwningThread;
	TUint8 iSessionType;
	TPointerArrayBuffer iSessionQ;
	TUint iOwningThreadId;
	};
typedef TPckgBuf<TServerKernelInfo> TServerKernelInfoBuf;	 

class TSessionKernelInfo : public TObjectKernelInfo
	{ 
public:
	TUint8*	iServer;
	TAny* 	iSessionPtr;
	TUint16	iTotalAccessCount;
	TUint8 iSessionType;
	TUint8 iSvrSessionType;
	TInt iMsgCount;
	TInt iMsgLimit;
	};
typedef TPckgBuf<TSessionKernelInfo> TSessionKernelInfoBuf;	 

class TLogicalDeviceKernelInfo : public TObjectKernelInfo
	{
public:
	TVersion iVersion;
	TUint iParseMask;
	TUint iUnitsMask;
	TInt iOpenChannels;
	};
typedef TPckgBuf<TLogicalDeviceKernelInfo> TLogicalDeviceKernelInfoBuf;	 

class TPhysicalDeviceKernelInfo : public TObjectKernelInfo
	{ 
public:
	TVersion iVersion;
	TUint iUnitsMask;
	TUint8* iAddressOfCodeSeg;
	};
typedef TPckgBuf<TPhysicalDeviceKernelInfo> TPhysicalDeviceKernelInfoBuf;	 

class TLogicalChannelKernelInfo : public TObjectKernelInfo
	{
public:
	};
typedef TPckgBuf<TLogicalChannelKernelInfo> TLogicalChannelKernelInfoBuf;	 

class TChangeNotifierKernelInfo : public TObjectKernelInfo
	{
public:
	TUint iChanges;
	TUint8* iAddressOfThread;
	};
typedef TPckgBuf<TChangeNotifierKernelInfo> TChangeNotifierKernelInfoBuf;	 

class TUndertakerKernelInfo : public TObjectKernelInfo
	{
public:
	TUint8* iOwningThread;
	};
typedef TPckgBuf<TUndertakerKernelInfo> TUndertakerKernelInfoBuf;	 

class TMsgQueueKernelInfo : public TObjectKernelInfo
	{
public:
	TUint iMaxMsgLength;
	};
typedef TPckgBuf<TMsgQueueKernelInfo> TMsgQueueKernelInfoBuf;	 

class TPropertyRefKernelInfo : public TObjectKernelInfo
	{
public:
	};
typedef TPckgBuf<TPropertyRefKernelInfo> TPropertyRefKernelInfoBuf;	 

class TCondVarKernelInfo : public TObjectKernelInfo
	{
public:
	TUint8 iResetting;
	TUint8* iAddressOfMutex;
	TInt iWaitCount;
	TPointerArrayBuffer iSuspendedQ;
	};
typedef TPckgBuf<TCondVarKernelInfo> TCondVarKernelInfoBuf;	 

class TCodeSegKernelInfoBase
	{
public:
	TUint32 iRunAddress;
	TInt iSize;
	TFileName8 iFileName;
	};

// We don't really need a base class, it's just for BC reasons 
class TCodeSegKernelInfo : public TCodeSegKernelInfoBase
	{
public:
	TUint8* iAddressOfKernelObject;
	TFullName8 iName;
	TInt iAccessCount;
	TInt iDepCount;
	TBool iXip;
	};
typedef TPckgBuf<TCodeSegKernelInfo> TCodeSegKernelInfoBuf;	 


class TProp
	{
public:
	TUid iCategory;
	TUint iKey;
	TInt iActualSize; // Return value, only used when getting a descriptor property
	TBool iDefine;
	};

class TPropNotifyResult
	{
public:
	TUint iCategory;
	TUint iKey;
	TInt iMissedChanges;
	TInt iError;
	};

class TRamZoneInfo
	{
public:
	TUint iFreePages;
	TUint iUnknownPages;
	TUint iFixedPages;
	TUint iMovablePages;
	TUint iDiscardablePages;
	};

class TProcessProperties
	{
public:
	TProcessProperties() : iSid(0xFFFFFFFFu), iVid(0xFFFFFFFFu), /*iHeapMin(0), iHeapMax(0), iStackSize(0),*/ iProcessPriority(0)
		{
		iCapsToAdd.SetEmpty();
		iCapsToRemove.SetEmpty();
		}

	TCapabilitySet iCapsToAdd;
	TCapabilitySet iCapsToRemove;
	TUint iSid;
	TUint iVid;
	//TUint iHeapMin;
	//TUint iHeapMax;
	//TUint iStackSize;
	TInt iProcessPriority;
	};

// class RMemoryAccess
//  User side communication interface to Memory Access driver.
//  Provides access to arbitrary kernel objects, memory, and some basic object manipulation.
//  For use in debugging tools only!  Do not include in a release ROM.
class RMemoryAccess : public RBusLogicalChannel
	{
public:
	enum TControl
		{
		EControlGetThreadMem,
		EControlGetAllocatorAddress,
		EControlResetTimer,
		EControlGetContainerCount,
		EControlAcquireContainerMutex,
		EControlReleaseContainerMutex,
		EControlGetObjectType,
		EControlGetObjectInfo,
		EControlGetObjectInfoByPtr,
		EControlGetObjectInfoByHandle,
		EControlAcquireCodeSegMutex,
		EControlReleaseCodeSegMutex,
		EControlGetNextCodeSegInfo,
		EControlObjectDie,
		EControlGetObjectInfoByName,
		EControlGetObjectAddresses,
		EControlGetChunkAddresses,
		EControlGetCurrentAllocatorAddress,
		EControlFindPtrInCodeSegments,
		EControlGetHandleOwners,
		EControlForceCrash,
		EControlGetPropertyInt,
		EControlGetPropertyDesc,
		EControlSetPropertyInt,
		EControlSetPropertyDesc,
		EControlDeleteProperty,
		EControlSetThreadPriority,
		EControlNotifyThreadCreation,
		EControlCancelNotifyThreadCreation,
		EControlSetPriorityOverride,
		EControlSetCriticalFlags,
		EControlOpenThread,
		EControlGetThreadHandles,
		EControlGetProcessHandles,
		EControlInPlaceThreadRename,
		ERemoved1, //EControlGetBinaryPropertyKernelLocation
		EControlInPlaceObjectRename,
		EControlInPlaceSetProcessFileName,
		ERemoved2, //EControlGetAllProperties,
		ERemoved3, //EControlSetupMassPropertyNotify,
		ERemoved4, //EControlMassPropertyNotifyStart,
		ERemoved5, //EControlMassPropertyNotifyStop,
		EControlEnableHeapTracing,
		EControlDefragRam,
		EControlEmptyRamZone,
		EControlGetRamZoneInfo,
		EControlSetProcessProperties,
		EControlWriteShadowMemory,
		EControlFreeShadowMemory,
		EControlSetZombieDebugMode,
		EControlWriteMem,
		EControlSetTextTraceMode,
		EControlGetRegisters,
		EControlGetZombies,
		EControlGetZombieDebugMode,
		EControlReleaseZombie,
		EControlSuspendThread,
		EControlNotifyBreakpoint,
		EControlSetBreakpoint,
		EControlSetBreakpointEnabled,
		EControlClearBreakpoint,
		EControlContinueFromBreakpoint,
		EControlGetBreakpoints,
		EControlCancelNotifyBreakpoint,
		EControlRegisterPersistantBreakpoint,
		EControlSetSymbolicBreakpoint,
		EControlSetDebugPort,
		EControlReboot,
		EControlGetCreatorThread,
		EControlPropertyNotify,
		EControlPropertyNotifyCancel,
		EControlSubscribeToProperty,
		EControlAcquireCodeSegMutexAndFilterCodesegsForProcess,
		ENumRequests,  // Add new commands above this line
        };
public:
	static TInt LoadDriver();
	static void CloseDriver();

public:
	//Open the driver
	TInt Open();
	//Deprecated.
	TInt ResetTimer();
	//Read memory from the context of a thread
	TInt GetThreadMem(const TThreadMemoryAccessParamsBuf& aParams, TDes8& aBuf);
	//Get the address of the allocator of a thread
	TInt GetAllocatorAddress(TUint aThreadId, TUint8*& aAddressOfKernelObject);
	TInt GetCurrentAllocatorAddress(TUint aThreadId, TUint8*& aAddressOfKernelObject);
	TInt FindAddressInCodeSegments(TFullName8& aDllName, TAny* aPtr);
	TInt GetHandleOwners(TAny* aKernelObjectPtr, TDes8& aOwnersBuf);
	TInt GetThreadHandles(TUint aThreadId, TDes8& aHandlesBuf);
	TInt GetProcessHandles(TUint aProcessId, TDes8& aHandlesBuf);
	void ForceCrash();
	void Reboot(TInt aReason);

	//Get the number of objects in a specified kernel container
	TInt GetContainerCount (const TObjectType aObjectType, TUint& aCount);
	//Wait on the mutex of a specified kernel container
	TInt AcquireContainerMutex (const TObjectType aObjectType);
	//Signal the mutex of a specified kernel container
	TInt ReleaseContainerMutex (const TObjectType aObjectType);
	//Get the type of a DObject* object in the kernel
	TInt GetObjectType (TUint8* aKernelObjectAddr, TObjectType& aObjectType);
	//Get info about a kernel object, referenced by position in its container.  Call AcquireContainerMutex before calling.
	TInt GetObjectInfo (TObjectType aObjectType, TUint aIndexInKernelContainer, TDes8& aObjectInfoBuf);
	//Get info about a kernel object specified as a DObject*
	TInt GetObjectInfo (TObjectType aObjectType, TUint8* aAddressOfKernelObject, TDes8& aObjectInfoBuf);
	//Get info about a kernel object specified by name.
	TInt GetObjectInfo (TObjectType aObjectType, const TDesC& aObjectName, TDes8& aObjectInfoBuf);
	//Get info about a kernel object specified as a user side handle	
	TInt GetObjectInfoByHandle (TObjectType aObjectType, TInt aThreadId, TInt aObjectHandle, TDes8& aObjectInfoBuf);
	//Wait on the kernel's code seg mutex
	TInt AcquireCodeSegMutex();
	TInt AcquireCodeSegMutexAndFilterCodesegsForProcess(TUint aProcessId);
	//Signal the kernel's code seg mutex
	TInt ReleaseCodeSegMutex();
	//Get info about the next code seg.  Call repeatedly to iterate all code segs.  Call AcquireCodeSegMutex before calling.
	TInt GetNextCodeSegInfo(TDes8& aCodeSegInfoBuf);
	//Kill an object.  aObyType should be a thread or process.  Either an objectId (user side handle) _OR_ objectPtr (kernel side DObject*) should be provided.
	TInt ObjectDie(TObjectType aObjType, TUint aObjectId, TUint8* aObjectPtr, TExitType aType, TInt aReason, const TDesC& aCategory);
	//Pack the addresses of all the kernel objects of the specified type belonging to the specified process into the supplied buffer.
	//Returns KErrOverflow if the buffer isn't big enough to hold all the addresses. aOwningProcess may contain '?' and '*' wild characters.
	TInt GetObjectAddresses(TObjectType aObjType, const TDesC& aOwningProcess, TDes8& aAddressBuffer);
	//Pack the addresses of all the chunks with a the specified controlling process id into the supplied buffer.
	//Returns KErrOverflow if the buffer isn't big enough to hold all the addresses. Note, this interface allows global chunks (which have
	//a NULL owner pointers, hence can't be found using RMemoryAccess::GetObjectAddresses) that were created by a particular process to be found.
	TInt GetChunkAddresses(TUint aControllingProcessId, TDes8& aAddressBuffer);

	TInt GetProperty(TUid aCategory, TUint aKey, TInt& aValue);
	TInt GetProperty(TUid aCategory, TUint aKey, TDes8& aValue, TInt& aActualSize);
	TInt SetProperty(TUid aCategory, TUint aKey, TInt aValue, TBool aDefineIfNotSet=EFalse);
	TInt SetProperty(TUid aCategory, TUint aKey, const TDesC8& aValue, TBool aDefineIfNotSet=EFalse);
	TInt DeleteProperty(TUid aCategory, TUint aKey);
	TInt SubscribeToProperty(TUid aCategory, TUint aKey, TBool aOutputToBtrace); // If aOutputToBtrace is false, use NotifyPropertyChange to get details

	void NotifyPropertyChange(TPropNotifyResult& aResult, TRequestStatus& aStatus);
	void CancelPropertyChange();

	TInt RThreadForceOpen(RThread& aThread, TUint aId);
	TInt SetThreadPriority(RThread& aThread, TInt aPriority);
	void NotifyThreadCreation(TRequestStatus& aStatus);
	void CancelNotifyThreadCreation();
	TInt SetPriorityOverride(TInt aPriority, const TDesC8& aMatchString); // any thread that is created that matches the wildcard string aMatchString is given thread priority aPriority
	TInt SetThreadCriticalFlags(RThread& aThread, TUint aFlags);
	TLinAddr InPlaceThreadRename(RThread& aThread, const TDesC8& aNewName);
	TInt InPlaceObjectRename(TObjectType aObjectType, TUint8* aAddressOfKernelObject, const TDesC8& aNewName);
	TInt InPlaceSetProcessFileName(RProcess& aProcess, const TDesC8& aNewName);
	TInt SetProcessProperties(RProcess& aProcess, const TProcessProperties& aProperties);
	TInt WriteShadowMemory(TLinAddr aAddress, const TDesC8& aNewContents);
	TInt FreeShadowMemory(TLinAddr aAddress, TInt aLen);
	TInt SetDebugPort(TInt aPort);
	TUint GetThreadCreatorId(TUint aThreadId);

	TInt EnableHeapTracing(TUint aThreadId, TBool aEnable);
	TInt DefragRam(TInt aPriority);
	TInt EmptyRamZone(TUint aZone, TInt aPriority);
	TInt GetRamZoneInfo(TUint aZone, TDes8& aInfoPkg);

	// Debugger APIs
	TInt GetZombieDebugMode();
	TInt SetZombieDebugMode(TInt aMode);
	class TZombieInfo
		{
	public:
		TUint iThreadId;
		enum TFlags { ESuspended = 1, EBreakpoint = 2 };
		TUint iFlags;
		};
	TInt GetZombies(TDes8& aResultBuf); // buffer full of TZombieInfos
	TInt WriteMem(TUint aThreadId, const TDesC8& aMem, TAny* aAddr);
	TInt SetTextTraceMode(TUint& aMode, TUint aMask);
	TInt GetRegisters(RThread& aThread, TBool aUserMode, TDes8& aRegBuf, TUint32& aValid);
	TInt ReleaseZombie(RThread& aThread);
	TInt SuspendThread(RThread& aThread); // To resume, call ReleaseZombie

	class TBreakpointNotification
		{
	public:
		TUint iThreadId;
		TInt iBreakpointId;
		TLinAddr iAddress;
		};

	class TPredicate
		{
	public:
		enum { KNumSlots = 4 };
		enum TOp
			{
			ENothing = 0,
			EEq, ENe, ELt, ELe, EGt, EGe, ESignedEq, ESignedNe, ESignedLt, ESignedLe, ESignedGt, ESignedGe,
			};
		TPredicate() : iOp(0) {}
		TBool HasConditions() const { return iOp != 0; }

#ifdef __KERNEL_MODE__
		TBool SatisfiesConditions(TUint32* aRegisterSet) const;
	private:
		static TBool Test(TOp aOperation, TUint32 aCurrentValue, TUint32 aStoredValue);
#else
		TInt AddCondition(TOp aOperation, TInt aRegNum, TUint32 aValue);
		void Description(TDes& aBuf);
#endif

	private:
		TUint32 iOp; // 8 bits for each, of which top 4 is reg that the relevant iVals is to be compared against, bottom 4 is the comparison operation (one of TOp)
		TUint32 iVals[KNumSlots];
		};

	void NotifyBreakpoint(TPckg<TBreakpointNotification>& aPkg, TRequestStatus& aStatus);
	void CancelNotifyBreakpoint();
	//TInt SetBreakpoint(TLinAddr aAddress); // Global, all threads (except caller and the memaccess DFC thread). Returns breakpoint ID or an error code
	TInt SetBreakpoint(RThread& aThread, TLinAddr aAddress, TPredicate* aCondition = NULL); // Returns breakpoint ID or an error code. Breakpoint ID may be OR'd with TBreakpointInfo::EHardware if a hardware breakpoint was sucessfully set
	TInt SetSymbolicBreakpoint(RThread& aThread, const TDesC8& aCodeseg, TUint aOffset, TPredicate* aCondition = NULL);
	TInt RegisterPersistantBreakpoint(TLinAddr aAddress);
	TInt SetBreakpointEnabled(TInt aBreakpointId, TBool aEnabled);
	TInt ClearBreakpoint(TInt aBreakpointId); // Removes it completely. Doesn't resume anything waiting on it though
	TInt ContinueFromBreakpoint(RThread& aThread); // aThread must be waiting on a breakpoint otherwise KErrNotReady

	class TBreakpointInfo
		{
	public:
		TUint iThreadId;
		TInt iBreakpointId;
		TLinAddr iAddress;
		enum TFlags
			{
			EPending = 1,
			EEnabled = 2, // Enabled by user, that is. It could still be pending and thus not actually going to case a break at this point in time
			EHardware = 0x40000000,
			};
		TUint32 iFlags;
		TPredicate iCondition;
		};
	TInt GetBreakpoints(TDes8& aBuf); // buffer full of TBreakpointInfos
	};

#ifndef __KERNEL_MODE__
// Inline implementations of user side interface

inline TInt RMemoryAccess::LoadDriver()
	{	return User::LoadLogicalDevice(KMemoryAccessName);	}
inline void RMemoryAccess::CloseDriver()
	{	User::FreeLogicalDevice(KMemoryAccessName);	}
inline TInt RMemoryAccess::Open()
	{	return DoCreate(KMemoryAccessName,TVersion(1,0,0),KNullUnit,NULL,NULL); 	}
inline TInt RMemoryAccess::GetThreadMem(const TThreadMemoryAccessParamsBuf& aParams, TDes8& aBuf)
	{	return DoControl(EControlGetThreadMem, (TAny*)&aParams, (TAny*)&aBuf);	}
inline TInt RMemoryAccess::GetAllocatorAddress(TUint aThreadId, TUint8*& aAddressOfKernelObject)
	{	return DoControl(EControlGetAllocatorAddress, (TAny*)aThreadId, &aAddressOfKernelObject);	}
// This returns the current allocator (User::Allocator) as opposed to the one created in SThreadCreateInfo
inline TInt RMemoryAccess::GetCurrentAllocatorAddress(TUint aThreadId, TUint8*& aAddressOfKernelObject)
	{	return DoControl(EControlGetCurrentAllocatorAddress, (TAny*)aThreadId, &aAddressOfKernelObject);	}
inline TInt RMemoryAccess::FindAddressInCodeSegments(TFullName8& aDllName, TAny* aPtr)
	{	return DoControl(EControlFindPtrInCodeSegments, (TAny*)&aDllName, aPtr);	}
inline TInt RMemoryAccess::GetHandleOwners(TAny* aHandle, TDes8& aOwnersBuf)
	{	return DoControl(EControlGetHandleOwners, (TAny*)aHandle, (TAny*)&aOwnersBuf);	}
inline TInt RMemoryAccess::GetThreadHandles(TUint aThreadId, TDes8& aHandlesBuf)
	{	return DoControl(EControlGetThreadHandles, (TAny*)aThreadId, &aHandlesBuf);	}
inline TInt RMemoryAccess::GetProcessHandles(TUint aProcessId, TDes8& aHandlesBuf)
	{	return DoControl(EControlGetProcessHandles, (TAny*)aProcessId, &aHandlesBuf);	}
inline void RMemoryAccess::ForceCrash()
	{	DoControl(EControlForceCrash);	}
inline void RMemoryAccess::Reboot(TInt aReason)
	{	DoControl(EControlReboot, (TAny*)aReason); }
inline TInt RMemoryAccess::ResetTimer()
	{	return DoControl(EControlResetTimer, NULL, NULL);	}
inline TInt RMemoryAccess::GetContainerCount (const TObjectType aObjectType, TUint& aCount)
	{	return DoControl(EControlGetContainerCount, (TAny*)aObjectType, (TAny*)&aCount);	}
inline TInt RMemoryAccess::AcquireContainerMutex (const TObjectType aObjectType)	{	return DoControl(EControlAcquireContainerMutex, (TAny*)aObjectType, NULL);		}
inline TInt RMemoryAccess::ReleaseContainerMutex (const TObjectType aObjectType)
	{	return DoControl(EControlReleaseContainerMutex, (TAny*)aObjectType, NULL);		}
inline TInt RMemoryAccess::GetObjectType (TUint8* aKernelObjectAddr, TObjectType& aObjectType)
	{	return DoControl(EControlGetObjectType, (TAny*)aKernelObjectAddr, (TAny*)&aObjectType);	}
inline TInt RMemoryAccess::GetObjectInfo (TObjectType aObjectType, TUint aIndexInKernelContainer, TDes8& aObjectInfoBuf)
	{	
	TObjectInfoByIndexParamsBuf params;
	params().iObjectType=aObjectType;
	params().iObjectIndex=aIndexInKernelContainer;
	return DoControl(EControlGetObjectInfo, (TAny*)&params, (TAny*)&aObjectInfoBuf);	
	}
inline TInt RMemoryAccess::GetObjectInfo (TObjectType aObjectType, TUint8* aAddressOfKernelObject, TDes8& aObjectInfoBuf)
	{
	TObjectInfoByPtrParamsBuf params;
	params().iObjectType=aObjectType;
	params().iObjectPtr=aAddressOfKernelObject;	
	return DoControl(EControlGetObjectInfoByPtr, (TAny*)&params, (TAny*)&aObjectInfoBuf);	
	}
inline TInt RMemoryAccess::GetObjectInfo (TObjectType aObjectType, const TDesC& aObjectName, TDes8& aObjectInfoBuf)
	{
	TObjectInfoByNameParamsBuf params;
	params().iObjectType=aObjectType;
	params().iObjectName.Copy(aObjectName);	
	return DoControl(EControlGetObjectInfoByName, (TAny*)&params, (TAny*)&aObjectInfoBuf);	
	}
inline TInt RMemoryAccess::GetObjectInfoByHandle (TObjectType aObjectType, TInt aThreadId, TInt aObjectHandle, TDes8& aObjectInfoBuf)
	{
	TObjectInfoByHandleParamsBuf params;
	params().iObjectType=aObjectType;
	params().iThreadId=aThreadId;	
	params().iObjectHandle=aObjectHandle;	
	return DoControl(EControlGetObjectInfoByHandle, (TAny*)&params, (TAny*)&aObjectInfoBuf);	
	}
inline TInt RMemoryAccess::AcquireCodeSegMutex()
	{	return DoControl(EControlAcquireCodeSegMutex, NULL, NULL);		}
inline TInt RMemoryAccess::AcquireCodeSegMutexAndFilterCodesegsForProcess(TUint aProcessId)
	{	return DoControl(EControlAcquireCodeSegMutexAndFilterCodesegsForProcess, (TAny*)aProcessId);	}
inline TInt RMemoryAccess::ReleaseCodeSegMutex()
	{	return DoControl(EControlReleaseCodeSegMutex, NULL, NULL);		}
inline TInt RMemoryAccess::GetNextCodeSegInfo(TDes8& aCodeSegInfoBuf)
	{	return DoControl(EControlGetNextCodeSegInfo, (TAny*)&aCodeSegInfoBuf, NULL);	}
inline TInt RMemoryAccess::ObjectDie(TObjectType aObjType, TUint aObjectId, TUint8* aObjectPtr, TExitType aType, TInt aReason, const TDesC& aCategory)
	{
	TObjectKillParamsBuf params;
	params().iObjectType=aObjType;
	params().iObjectId=aObjectId;
	params().iObjectPtr=aObjectPtr;	
	params().iType=aType;
	params().iReason=aReason;
	params().iCategory.Copy(aCategory);
	return DoControl(EControlObjectDie, (TAny*)&params, NULL);	
	}
inline TInt RMemoryAccess::GetObjectAddresses(TObjectType aObjType, const TDesC& aOwningProcess, TDes8& aAddressBuffer)
	{
	TGetObjectAddressesParamsBuf params;
	params().iObjectType=aObjType;
	params().iOwningProcess.Copy(aOwningProcess);
	return DoControl(EControlGetObjectAddresses, (TAny*)&params, (TAny*)&aAddressBuffer);	
	}
inline TInt RMemoryAccess::GetChunkAddresses(TUint aControllingProcessId, TDes8& aAddressBuffer)
	{
	TGetChunkAddressesParamsBuf params;
	params().iControllingProcessId=aControllingProcessId;
	return DoControl(EControlGetChunkAddresses, (TAny*)&params, (TAny*)&aAddressBuffer);	
	}
	
inline TInt RMemoryAccess::GetProperty(TUid aCategory, TUint aKey, TInt& aValue)
	{
	TProp params;
	params.iCategory = aCategory;
	params.iKey = aKey;
	params.iDefine = EFalse;

	TPckg<TInt> res(aValue);

	return DoControl(EControlGetPropertyInt, (TAny*)&params, (TAny*)&res);
	}

inline TInt RMemoryAccess::GetProperty(TUid aCategory, TUint aKey, TDes8& aValue, TInt& aActualSize)
	{
	TProp params;
	params.iCategory = aCategory;
	params.iKey = aKey;
	params.iDefine = EFalse;

	TInt err = DoControl(EControlGetPropertyDesc, (TAny*)&params, (TAny*)&aValue);
	if (err == KErrOverflow)
		{
		aActualSize = params.iActualSize;
		}
	return err;
	}

inline TInt RMemoryAccess::SetProperty(TUid aCategory, TUint aKey, TInt aValue, TBool aDefineIfNotSet)
	{
	TProp params;
	params.iCategory = aCategory;
	params.iKey = aKey;
	params.iDefine = aDefineIfNotSet;

	return DoControl(EControlSetPropertyInt, (TAny*)&params, (TAny*)aValue);
	}

inline TInt RMemoryAccess::SetProperty(TUid aCategory, TUint aKey, const TDesC8& aValue, TBool aDefineIfNotSet)
	{
	TProp params;
	params.iCategory = aCategory;
	params.iKey = aKey;
	params.iDefine = aDefineIfNotSet;

	return DoControl(EControlSetPropertyDesc, (TAny*)&params, (TAny*)&aValue);
	}

inline TInt RMemoryAccess::DeleteProperty(TUid aCategory, TUint aKey)
	{
	TProp params;
	params.iCategory = aCategory;
	params.iKey = aKey;
	params.iDefine = EFalse;

	return DoControl(EControlDeleteProperty, (TAny*)&params, NULL);
	}

inline void RMemoryAccess::NotifyPropertyChange(TPropNotifyResult& aResult, TRequestStatus& aStatus)
	{
	aStatus = KRequestPending;
	TInt err = DoControl(EControlPropertyNotify, (TAny*)&aResult, (TAny*)&aStatus);
	if (err)
		{
		TRequestStatus* ffs = &aStatus;
		User::RequestComplete(ffs, err);
		}
	}

inline void RMemoryAccess::CancelPropertyChange()
	{
	DoControl(EControlPropertyNotifyCancel, NULL, NULL);
	}

inline TInt RMemoryAccess::SubscribeToProperty(TUid aCategory, TUint aKey, TBool aOutputToBtrace)
	{
	TProp params;
	params.iCategory = aCategory;
	params.iKey = aKey;
	params.iDefine = EFalse;
	return DoControl(EControlSubscribeToProperty, (TAny*)&params, (TAny*)aOutputToBtrace);
	}

inline TInt RMemoryAccess::SetThreadPriority(RThread& aThread, TInt aPriority)
	{
	return DoControl(EControlSetThreadPriority, (TAny*)aThread.Handle(), (TAny*)aPriority);
	}

inline void RMemoryAccess::NotifyThreadCreation(TRequestStatus& aStatus)
	{
	aStatus = KRequestPending;
	TRequestStatus* stat = &aStatus;
	TInt err = DoControl(EControlNotifyThreadCreation, stat, NULL);
	if (err) User::RequestComplete(stat, err);
	}

inline void RMemoryAccess::CancelNotifyThreadCreation()
	{
	DoControl(EControlCancelNotifyThreadCreation);
	}

inline TInt RMemoryAccess::SetPriorityOverride(TInt aPriority, const TDesC8& aMatchString)
	{
	return DoControl(EControlSetPriorityOverride, (TAny*)aPriority, (TAny*)&aMatchString);
	}

inline TInt RMemoryAccess::SetThreadCriticalFlags(RThread& aThread, TUint aFlags)
	{
	return DoControl(EControlSetCriticalFlags, (TAny*)aThread.Handle(), (TAny*)aFlags);
	}

inline TInt RMemoryAccess::RThreadForceOpen(RThread& aThread, TUint aId)
	{
	TInt handle = DoControl(EControlOpenThread, (TAny*)aId, NULL);
	if (handle < 0) return handle; // error
	aThread.SetHandle(handle);
	return KErrNone;
	}

inline TLinAddr RMemoryAccess::InPlaceThreadRename(RThread& aThread, const TDesC8& aNewName)
	{
	return DoControl(EControlInPlaceThreadRename, (TAny*)aThread.Handle(), (TAny*)&aNewName);
	}

inline TInt RMemoryAccess::InPlaceObjectRename(TObjectType aObjectType, TUint8* aAddressOfKernelObject, const TDesC8& aNewName)
	{
	TObjectInfoByPtrParamsBuf params;
	params().iObjectType = aObjectType;
	params().iObjectPtr = aAddressOfKernelObject;
	return DoControl(EControlInPlaceObjectRename, (TAny*)&params, (TAny*)&aNewName);
	}

inline TInt RMemoryAccess::InPlaceSetProcessFileName(RProcess& aProcess, const TDesC8& aNewName)
	{
	return DoControl(EControlInPlaceSetProcessFileName, (TAny*)aProcess.Handle(), (TAny*)&aNewName);
	}

inline TInt RMemoryAccess::EnableHeapTracing(TUint aThreadId, TBool aEnable)
	{
	return DoControl(EControlEnableHeapTracing, (TAny*)aThreadId, (TAny*)aEnable);
	}

inline TInt RMemoryAccess::DefragRam(TInt aPriority)
	{
	return DoControl(EControlDefragRam, (TAny*)aPriority, NULL);
	}

inline TInt RMemoryAccess::EmptyRamZone(TUint aZone, TInt aPriority)
	{
	return DoControl(EControlEmptyRamZone, (TAny*)aPriority, (TAny*)aZone);
	}

inline TInt RMemoryAccess::GetRamZoneInfo(TUint aZone, TDes8& aInfoPkg)
	{
	return DoControl(EControlGetRamZoneInfo, (TAny*)aZone, &aInfoPkg);
	}

inline TInt RMemoryAccess::SetProcessProperties(RProcess& aProcess, const TProcessProperties& aProperties)
	{
	const TPckgC<TProcessProperties> props(aProperties);
	return DoControl(EControlSetProcessProperties, (TAny*)aProcess.Handle(), (TAny*)&props);
	}

inline TInt RMemoryAccess::WriteShadowMemory(TLinAddr aAddress, const TDesC8& aNewContents)
	{
	return DoControl(EControlWriteShadowMemory, (TAny*)aAddress, (TAny*)&aNewContents);
	}

inline TInt RMemoryAccess::FreeShadowMemory(TLinAddr aAddress, TInt aLen)
	{
	return DoControl(EControlFreeShadowMemory, (TAny*)aAddress, (TAny*)aLen);
	}

inline TInt RMemoryAccess::SetZombieDebugMode(TInt aMode)
	{
	return DoControl(EControlSetZombieDebugMode, (TAny*)aMode, NULL);
	}

inline TInt RMemoryAccess::GetZombieDebugMode()
	{
	return DoControl(EControlGetZombieDebugMode, NULL, NULL);
	}

inline TInt RMemoryAccess::WriteMem(TUint aThreadId, const TDesC8& aMem, TAny* aAddr)
	{
	TAny* args[3];
	args[0] = (TAny*)aThreadId;
	args[1] = (TAny*)&aMem;
	args[2] = aAddr;
	return DoControl(EControlWriteMem, &args[0], NULL);
	}

inline TInt RMemoryAccess::SetTextTraceMode(TUint& aMode, TUint aMask)
	{
	return DoControl(EControlSetTextTraceMode, (TAny*)&aMode, (TAny*)aMask);
	}

inline TInt RMemoryAccess::GetRegisters(RThread& aThread, TBool aUserMode, TDes8& aRegBuf, TUint32& aValid)
	{
	TUint32 args[4];
	args[0] = aThread.Handle();
	args[1] = aUserMode;
	args[2] = (TUint32)&aRegBuf;
	args[3] = (TUint32)&aValid;
	return DoControl(EControlGetRegisters, (TAny*)&args[0], NULL);
	}

inline TInt RMemoryAccess::GetZombies(TDes8& aResultBuf)
	{
	return DoControl(EControlGetZombies, (TAny*)&aResultBuf, NULL);
	}

inline TInt RMemoryAccess::ReleaseZombie(RThread& aThread)
	{
	return DoControl(EControlReleaseZombie, (TAny*)aThread.Handle(), NULL);
	}

inline TInt RMemoryAccess::SuspendThread(RThread& aThread)
	{
	return DoControl(EControlSuspendThread, (TAny*)aThread.Handle(), NULL);
	}

inline void RMemoryAccess::NotifyBreakpoint(TPckg<TBreakpointNotification>& aPkg, TRequestStatus& aStatus)
	{
	TRequestStatus* stat = &aStatus;
	TInt err = DoControl(EControlNotifyBreakpoint, &aPkg, stat);
	if (err) User::RequestComplete(stat, err);
	}

inline void RMemoryAccess::CancelNotifyBreakpoint()
	{
	DoControl(EControlCancelNotifyBreakpoint, NULL, NULL);
	}
	
inline TInt RMemoryAccess::SetBreakpoint(RThread& aThread, TLinAddr aAddress, RMemoryAccess::TPredicate* aCondition /*=NULL*/)
	{
	TUint32 args[2] = { aAddress, (TUint32)aCondition };
	return DoControl(EControlSetBreakpoint, (TAny*)aThread.Handle(), (TAny*)&args[0]);
	}

inline TInt RMemoryAccess::SetSymbolicBreakpoint(RThread& aThread, const TDesC8& aCodeseg, TUint aOffset, RMemoryAccess::TPredicate* aCondition /*=NULL*/)
	{
	TUint32 args[3] = { aThread.Handle(), aOffset, (TUint32)aCondition };
	return DoControl(EControlSetSymbolicBreakpoint, (TAny*)&args[0], (TAny*)&aCodeseg);
	}

inline TInt RMemoryAccess::SetBreakpointEnabled(TInt aBreakpointId, TBool aEnabled)
	{
	return DoControl(EControlSetBreakpointEnabled, (TAny*)aBreakpointId, (TAny*)aEnabled);
	}

inline TInt RMemoryAccess::ClearBreakpoint(TInt aBreakpointId)
	{
	return DoControl(EControlClearBreakpoint, (TAny*)aBreakpointId, NULL);
	}

inline TInt RMemoryAccess::ContinueFromBreakpoint(RThread& aThread)
	{
	return DoControl(EControlContinueFromBreakpoint, (TAny*)aThread.Handle());
	}

inline TInt RMemoryAccess::GetBreakpoints(TDes8& aBuf)
	{
	return DoControl(EControlGetBreakpoints, &aBuf);
	}

inline TInt RMemoryAccess::RegisterPersistantBreakpoint(TLinAddr aAddress)
	{
	// aAddress is implicitly in the current thread
	return DoControl(EControlRegisterPersistantBreakpoint, (TAny*)aAddress, NULL);
	}

inline TInt RMemoryAccess::TPredicate::AddCondition(TOp aOperation, TInt aRegNum, TUint32 aValue)
	{
	if (aRegNum < 0 || aRegNum > 15) return KErrArgument;

	TInt freeSlot = 0;
	while (((iOp >> (freeSlot*8)) & 0xF) != ENothing)
		{
		freeSlot++;
		if (freeSlot >= KNumSlots) return KErrOverflow;
		}

	TInt slotShift = freeSlot * 8;
	TUint32 slotMask = 0xFF << slotShift;
	iOp = (iOp & ~slotMask) | (aOperation << slotShift) | (aRegNum << (slotShift+4));
	iVals[freeSlot] = aValue;
	return KErrNone;
	}

inline void RMemoryAccess::TPredicate::Description(TDes& aBuf)
	{
	TInt origLen = aBuf.Length();
	for (TInt slot = 0; slot < KNumSlots; slot++)
		{
		TInt slotShift = slot * 8;
		TUint32 opAndReg = (iOp >> slotShift) & 0xFF;
		TOp op = (TOp)(opAndReg & 0xF);
		if (op == ENothing) break;
		TInt reg = opAndReg >> 4;

		aBuf.AppendFormat(_L("r%d"), reg);
		switch(op)
			{
			case EEq:
			case ESignedEq:
				aBuf.Append(_L("==")); break;
			case ENe:
			case ESignedNe:
				aBuf.Append(_L("!=")); break;
			case ELt:
			case ESignedLt:
				aBuf.Append('<'); break;
			case ELe:
			case ESignedLe:
				aBuf.Append(_L("<=")); break;
			case EGt:
			case ESignedGt:
				aBuf.Append('>'); break;
			case EGe:
			case ESignedGe:
				aBuf.Append(_L(">=")); break;
			default:
				break;
			}
		if (op >= ESignedEq)
			{
			aBuf.AppendFormat(_L("%d,"), (TInt)iVals[slot]);
			}
		else
			{
			aBuf.AppendFormat(_L("%uu,"), iVals[slot]);
			}
		}
	if (aBuf.Length() > origLen) aBuf.SetLength(aBuf.Length() - 1); // Chomp final comma
	}

inline TInt RMemoryAccess::SetDebugPort(TInt aPort)
	{
	return DoControl(EControlSetDebugPort, (TAny*)aPort, NULL);
	}

inline TUint RMemoryAccess::GetThreadCreatorId(TUint aThreadId)
	{
	TUint result = 0;
	/*TInt err =*/ DoControl(EControlGetCreatorThread, (TAny*)aThreadId, &result);
	return result;
	}

#endif //__KERNEL_MODE__

#endif //__MemoryAccess_H__

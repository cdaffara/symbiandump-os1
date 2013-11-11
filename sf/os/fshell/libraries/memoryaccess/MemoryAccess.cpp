// MemoryAccess.cpp
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

//#include <m32std.h>
#include <nkern.h>
#include <kernel.h>
#include <kern_priv.h>

#ifdef __EPOC32__
#include <memmodel/epoc/plat_priv.h> // For DEpocCodeSeg
#endif

#include <fshell/common.mmh>

#ifdef FSHELL_DOBJECTIX_SUPPORT
#include "dobject.h" // To pick up my defn of DObjectIx/DObjectIxNinePointOneHack
#endif

#include <e32cmn.h>
#include "memoryaccess.h"
#include "PropertyAccess.h"
#include "fdebuggerkernel.h"
#include "DynamicDfcSupport.h"

//define this to get log output
/*#define DO_LOGGING

#ifdef DO_LOGGING
	#define DO_LOG(X) X
#else
	#define DO_LOG(X)
#endif */

const TInt KLocalBufferSize=4096;

const TInt KMajorVersionNumber=1;
const TInt KMinorVersionNumber=0;
const TInt KBuildVersionNumber=0;
_LIT(KMemAccessPanicCategory,"MemAccess");
enum TMemAccessPanic
	{
	EMemAccessPanicBadContainerType,
	EMemAccessMutexAlreadyHeld,
	EMemAccessMutexNotHeld
	};

class DMemoryAccess;

// DMemoryAccessFactory
//
NONSHARABLE_CLASS(DMemoryAccessFactory) : public DLogicalDevice
    {
public:
    DMemoryAccessFactory();
	~DMemoryAccessFactory();

	TInt GetEventHandler(DDebuggerEventHandler*& aResult);
	TInt GetDfcQue(TDfcQue*& aResult);
	
private:
    TInt Install();                     //overriding pure virtual
    void GetCaps(TDes8& aDes) const;    //overriding pure virtual
    TInt Create(DLogicalChannelBase*& aChannel);         //overriding pure virtual
	void Lock();
	void Unlock();

private:
	DDebuggerEventHandler* iSharedEventHandler;
	TDynDfcQueWrapper* iDfcQueWrapper;
	NFastMutex iLock;
    };


// DECLARE_STANDARD_LDD
//  Standard Extension Framework export
DECLARE_STANDARD_EXTENSION()
	{
	return KErrNone;
	}

DECLARE_EXTENSION_LDD()
	{
	return new DMemoryAccessFactory();
	}

class DThreadChangeHandler;
class DMsgQueue;

// DMemoryAccess
//
NONSHARABLE_CLASS(DMemoryAccess) : public DLogicalChannel, public MDebuggerEventClient
    {
public:
    DMemoryAccess();
    ~DMemoryAccess();
private:
//Framework
    TInt DoCreate(TInt aUnit,const TDesC* anInfo,const TVersion& aVer);
	void HandleMsg(TMessageBase* aMsg);
    void DoCancel(TInt aReqNo);
    TInt DoRequest(TInt aReqNo,TRequestStatus aStatus,TAny* a1,TAny* a2);
    TInt DoControl(TInt aFunction,TAny *a1,TAny *a2);
//API
    TInt GetThreadMem(TAny* aParams, TAny* aBuf);
    TInt GetAllocatorAddress(TUint aId, TUint8*& aAddr, TBool aGetCurrentAllocator=EFalse); //TOMSCI If false, get the created allocator. If true, get the current one
	TInt GetContainerCount(const TObjectType aObjectType, TUint& aCount);
	TInt AcquireContainerMutex(const TObjectType aObjectType);
	TInt ReleaseContainerMutex(const TObjectType aObjectType);
	TInt GetObjectType(TUint8* aObjectPtr, TObjectType& aType);
	TInt GetObjectInfo(TObjectType aObjectType, TInt aObjectIndexInContainer, TDes8* aKernelInfoBuf);
	TInt GetObjectInfoByPtr(TObjectType aObjectType, TUint8* aObjectPtr, TDes8* aKernelInfoBuf);
	TInt GetObjectInfoByName(TObjectType aObjectType, const TDesC8& aObjectName, TDes8* aKernelInfoBuf);
	TInt GetObjectInfoByHandle(TObjectType aObjectType, TInt aThreadId, TInt aObjectHandle, TDes8* aKernelInfoBuf);
	TInt GetObjectAddresses(TObjectType aObjectType, const TDesC8& aOwningProcess, TDes8* aKernelInfoBuf);
	TInt GetChunkAddresses(TUint aControllingProcessId, TDes8* aKernelInfoBuf);
	TInt AcquireCodeSegMutex();
	TInt AcquireCodeSegMutexAndFilterCodesegsForProcess(TUint aProcessId);
	TInt ReleaseCodeSegMutex();
	TInt GetNextCodeSegInfo(TDes8* aCodeSegInfoBuf);
	TInt ObjectDie(TObjectKillParamsBuf& aObjectKillParamsBuf);
	TInt FindPtrInCodeSegments(TAny* aDllNamePtr, TAny* aPtr);
	TInt GetHandleOwners(TAny* aObj, TAny* aOwnersBuf);
	TInt GetThreadHandles(TInt aThreadId, TAny* aHandlesBuf);
	TInt GetProcessHandles(TInt aProcessId, TAny* aHandlesBuf);
	TInt SetThreadPriority(TInt aThreadHandle, TInt aPriority);
	void NotifyThreadCreation(TRequestStatus* aStatus);
	void CancelNotifyThreadCreation();
	TInt SetPriorityOverride(TInt aPriority, TAny* aMatchString);
	TInt SetCriticalFlags(TInt aThreadHandle, TUint aFlags);
	TInt OpenThread(TUint aTid);
//Helpers
	void GetObjectInfo(DObject* aObject, TObjectKernelInfo& aInfo);
	TInt GetObjectPositionInContainer(TUint8* aObjectPtr, DObjectCon& aContainer, TInt& aPosInContainer);
	TInt GetThreadInfo(DThread* aThread, TDes8* aThreadInfoBuf);
	TInt GetProcessInfo(DProcess* aProcess, TDes8* aProcessInfoBuf);
	TInt GetChunkInfo(DChunk* aChunk, TDes8* aChunkInfoBuf);
	TInt GetLibraryInfo(DLibrary* aLibrary, TDes8* aLibraryInfoBuf);
	TInt GetSemaphoreInfo(DSemaphore* aSemaphore, TDes8* aLibraryInfoBuf);
	TInt GetMutexInfo(DMutex* aMutex, TDes8* aLibraryInfoBuf);
	TInt GetTimerInfo(DTimer* aTimer, TDes8* aTimerInfoBuf);
	TInt GetServerInfo(DServer* aServer, TDes8* aServerInfoBuf);
	TInt GetSessionInfo(DSession* aSession, TDes8* aSessionInfoBuf);
	TInt GetLogicalDeviceInfo(DLogicalDevice* aLogicalDevice, TDes8* aLogicalDeviceInfoBuf);
	TInt GetPhysicalDeviceInfo(DPhysicalDevice* aPhysicalDevice, TDes8* aPhysicalDeviceInfoBuf);
	TInt GetLogicalChannelInfo(DLogicalChannel* aLogicalChannel, TDes8* aLogicalChannelInfoBuf);
	TInt GetChangeNotifierInfo(DChangeNotifier* aChangeNotifier, TDes8* aChangeNotifierInfoBuf);
	TInt GetUndertakerInfo(DUndertaker* aUndertaker, TDes8* aUndertakerInfoBuf);
	TInt GetMsgQueueInfo(DMsgQueue* aMsgQueue, TDes8* aMsgQueueInfoBuf);
//	TInt GetPropertyRefInfo(DPropertyRef* aPropertyRef, TDes8* aPropertyRefInfoBuf);
	TInt GetCondVarInfo(DCondVar* aCondVar, TDes8* aCondVarInfoBuf);
	TInt InPlaceObjectRename(TAny* aParamsBuf, TAny* aNewNamePtr);
	TInt InPlaceSetProcessFileName(TAny* aParamsBuf, TAny* aNewNamePtr);
	TLinAddr DoGetAllocatorAddress(DThread* aThread, TBool aGetCurrent);
	TInt EnableHeapTracing(TUint aThreadId, TBool aEnable);
	TInt DefragRam(TInt aPriority);
	TInt EmptyRamZone(TInt aPriority, TUint aZone);
	TInt GetRamZoneInfo(TUint aZone, TAny* aInfoPkg);
	TInt SetProcessProperties(TInt aProcessHandle, TAny* aProperties);
	TInt WriteShadowMemory(TLinAddr aAddress, TAny* aNewContents);
	TInt FreeShadowMemory(TLinAddr aAddress, TInt aLength);
	TInt DebuggerFn(TInt aFn, TAny* a1, TAny* a2);
	TInt GetRegisters(TAny* a1, TAny* /*a2*/);
	DThread* ThreadFromHandle(TInt aHandle);


private: // From MDebuggerEventClient
	void BreakpointHit(TDes& aPkg);

private:
	DThread* iClient;
	TBool iLocks[ENumObjectTypes];
	enum TCodesegLogStatus
		{
		ENotHeld,
		EHeld,
		EHeldAndTraversing,
		} iCodeSegLock;
	SDblQueLink* iCurrentCodeSegLink;
	DThreadChangeHandler* iEventHandler;
	DPropertyAccess* iPropertyAccess;
	TAny* iClientBreakpointNotifyPkg;
	TRequestStatus* iClientBreakpointNotifyStatus;

	SDblQue iFilteredCodesegQue;
	};


NONSHARABLE_CLASS(DThreadChangeHandler) : public DKernelEventHandler
	{
public:
	DThreadChangeHandler(DThread* aClientThread);
	~DThreadChangeHandler();
	static TUint Event(TKernelEvent aEvent, TAny* a1, TAny* a2, TAny* aPrivateData);
	TUint DoEvent(TKernelEvent aEvent, TAny* a1, TAny* a2);
	void SetStatus(TRequestStatus* aStatus);

private:
	DThread* iThread;
	TRequestStatus* iStatus;
public:
	TBuf8<256> iMatch;
	TInt iPriorityToSet;
	};

// class DMemoryAccessFactory Implementation

//
//
DMemoryAccessFactory::DMemoryAccessFactory()
	{
    iVersion=TVersion(KMajorVersionNumber,KMinorVersionNumber,KBuildVersionNumber);
	}

// DMemoryAccessFactory::Create
//
TInt DMemoryAccessFactory::Create(DLogicalChannelBase*& aChannel)
    {
    aChannel=new DMemoryAccess();
	if (aChannel==NULL)
		return KErrNoMemory;
	else
		return KErrNone;
    }

// DMemoryAccessFactory::Install
//
TInt DMemoryAccessFactory::Install()
    {
    TInt err = SetName(&KMemoryAccessName);
	if (err == KErrNone)
		{
		DDebuggerEventHandler* dontCare;
		err = GetEventHandler(dontCare); // We call this to make sure the event handler is always running, so we can track creator info asap
		}
	return err;
    }

// DMemoryAccessFactory::GetCaps
//
void DMemoryAccessFactory::GetCaps(TDes8& /*aDes*/) const
    {
/*    TCapsMemoryAccessV01 b;
    b.iVersion=TVersion(KMajorVersionNumber,KMinorVersionNumber,KBuildVersionNumber);
    aDes.FillZ(aDes.MaxLength());
    aDes.Copy((TUint8*)&b,Min(aDes.MaxLength(),sizeof(b)));
*/    } 


// class DMemoryAccess Implementation

// Ctor
//
DMemoryAccess::DMemoryAccess()
    : DLogicalChannel()
    {
    //SetBehaviour(-1); //JR
    }


const TInt KMemAccessDfcThreadPriority = 27;

// DMemoryAccess::DoCreate
//  Check capabilities & version & start listening for events
TInt DMemoryAccess::DoCreate(TInt /*aUnit*/,const TDesC* /*anInfo*/,const TVersion& aVer)
    {
	TInt ret=KErrNone;
	
	//Require Power Management to use this driver
	//Not ideal, but better than nothing
	//if(!Kern::CurrentThreadHasCapability(ECapabilityPowerMgmt,__PLATSEC_DIAGNOSTIC_STRING("Checked by MEMORYACCESS.LDD (Memory Access for Task Managers)")))
	//    return KErrPermissionDenied;
    
    if (!Kern::QueryVersionSupported(TVersion(KMajorVersionNumber,KMinorVersionNumber,KBuildVersionNumber),aVer))
    	{
    	return KErrNotSupported;
    	}
    iClient=&Kern::CurrentThread();
	iClient->Open();

	TDfcQue* q = NULL;
	ret = ((DMemoryAccessFactory*)iDevice)->GetDfcQue(q);

	if (ret == KErrNone)
		{
    	SetDfcQ(q);
    	iMsgQ.Receive();		
		}

    return ret;
    }

TInt DMemoryAccessFactory::GetDfcQue(TDfcQue*& aQue)
	{
	Lock();
	if (iDfcQueWrapper)
		{
		aQue = iDfcQueWrapper->Que();
		Unlock();
		return KErrNone;
		}
	Unlock(); // Can't hold a fast mutex around DFC q creation

	TDynDfcQueWrapper* wrapper = NULL;
	_LIT(KDfcThreadName, "MemoryAccess-fshell");
	TInt ret = TDynDfcQueWrapper::Create(wrapper, KMemAccessDfcThreadPriority, KDfcThreadName);
	
	if (ret == KErrNone)
		{
		Lock();
		if (iDfcQueWrapper)
			{
			// Someone beat us to it while we weren't holding the lock
			aQue = iDfcQueWrapper->Que();
			Unlock();
			wrapper->Destroy();
			}
		else
			{
			iDfcQueWrapper = wrapper;
			aQue = iDfcQueWrapper->Que();
			Unlock();
			}
		}
	return ret;
	}


// dtor
//
DMemoryAccess::~DMemoryAccess()
    {
    //Clean up any mutexes that have been left open
	DObjectCon* const * cons = Kern::Containers();
    for (TInt ii=0;ii<ENumObjectTypes;++ii)
    	{
    	if (iLocks[ii])
    		{
			DObjectCon* container = cons[ii];
			container->Signal();
    		}
    	}

	DMutex* const codeSegLock = Kern::CodeSegLock();

    if (iCodeSegLock && codeSegLock)
		{
		Kern::MutexSignal(*codeSegLock);
		}

	if (iClientBreakpointNotifyStatus)
		{
		// Hmm I'm sure there are some threading subtleties here that I'm missing
		DDebuggerEventHandler* handler = NULL;
		TInt err = ((DMemoryAccessFactory*)iDevice)->GetEventHandler(handler);
		if (!err) handler->UnregisterForBreakpointNotification(this);
		Kern::RequestComplete(iClient, iClientBreakpointNotifyStatus, KErrCancel);
		}

	if (iEventHandler)
		{
		NKern::ThreadEnterCS();
		iEventHandler->Close();
		NKern::ThreadLeaveCS();
		}

	delete iPropertyAccess;
	if (iClient)
		{
		iClient->Close(NULL);
		}
	}
   
// DMemoryAccess::HandleMsg
//  Called when user side sends us something
void DMemoryAccess::HandleMsg(TMessageBase* aMsg)
	{
    TThreadMessage& m=*(TThreadMessage*)aMsg;

    // Get message type
    TInt id=m.iValue;

    // Decode the message type and dispatch it to the relevent handler function...
    
    // A logical channel can be closed either explicitly by its user-side client,
    // or implicitly if the client thread dies. In the latter case, the channel
    // is closed in the context of the kernel supervisor thread. 

    if (id==(TInt)ECloseMsg)
        {
        // Channel Close
		// We don't actually have any async requests
        //DoCancel(RMemoryAccess::EAllRequests); //JR
        m.Complete(KErrNone, EFalse);
        return;
        }

    // For all other message types, we check that the message is from the thread
    // that created us.
    if(m.Client()!=iClient)
        {
        Kern::ThreadKill(m.Client(),EExitPanic,ERequestFromWrongThread,KMemAccessPanicCategory);
        m.Complete(KErrNone,ETrue);
        return;
        }
    if (id==KMaxTInt)
        {
        // DoCancel
        DoCancel(m.Int0());
        m.Complete(KErrNone,ETrue);
        return;
        }
    if (id<0)
        {
        // DoRequest
        TRequestStatus* pS=(TRequestStatus*)m.Ptr0();
        TInt r=DoRequest(~id, *pS, m.Ptr1(), m.Ptr2());
        if (r!=KErrNone)
            Kern::RequestComplete(iClient,pS,r);
        m.Complete(KErrNone,ETrue);
        }
    else
        {
        // DoControl
        TInt r=DoControl(id,m.Ptr0(),m.Ptr1());
        m.Complete(r,ETrue);
        }	
	}

// DMemoryAccess::DoRequest
//  Handles async messages, called from HandleMsg
TInt DMemoryAccess::DoRequest(TInt /*aReqNo*/, TRequestStatus /*aStatus*/, TAny* /*a1*/, TAny* /*a2*/)
    {
    return KErrNone;
    }

// DMemoryAccess::DoCancel
//  Cancels async messages, called from HandleMsg
void DMemoryAccess::DoCancel(TInt /*aReqNo*/)
    {
    }

// DMemoryAccess::DoControl
//  Handles sync messages, called from HandleMsg
TInt DMemoryAccess::DoControl(TInt aFunction, TAny* a1, TAny* a2)
    {
	// Kern::Printf("[DMemoryAccess] ::DoControl; Function: %d", aFunction);
    switch (aFunction)
        {
    case RMemoryAccess::EControlGetThreadMem:
        return GetThreadMem(a1, a2);
    case RMemoryAccess::EControlGetAllocatorAddress:
        return GetAllocatorAddress((TUint)a1, *(TUint8**)a2);
    case RMemoryAccess::EControlGetCurrentAllocatorAddress:
        return GetAllocatorAddress((TUint)a1, *(TUint8**)a2, ETrue);
    case RMemoryAccess::EControlFindPtrInCodeSegments:
		return FindPtrInCodeSegments(a1, a2);
    case RMemoryAccess::EControlGetHandleOwners:
		return GetHandleOwners(a1, a2);
    case RMemoryAccess::EControlGetThreadHandles:
		return GetThreadHandles((TUint)a1, a2);
    case RMemoryAccess::EControlGetProcessHandles:
		return GetProcessHandles((TUint)a1, a2);
	case RMemoryAccess::EControlForceCrash:
		Kern::Fault("ForceCrash-QR3", 0);
		return KErrNone;
	case RMemoryAccess::EControlReboot:
		Kern::Restart((TInt)a1);
		return KErrNone;
	case RMemoryAccess::EControlGetPropertyInt:
	case RMemoryAccess::EControlGetPropertyDesc:
	case RMemoryAccess::EControlSetPropertyInt:
	case RMemoryAccess::EControlSetPropertyDesc:
	case RMemoryAccess::EControlDeleteProperty:
	case RMemoryAccess::EControlPropertyNotify:
	case RMemoryAccess::EControlPropertyNotifyCancel:
	case RMemoryAccess::EControlSubscribeToProperty:
		if (!iPropertyAccess)
			{
			iPropertyAccess = new DPropertyAccess(iClient);
			}
		if (!iPropertyAccess) return KErrNoMemory;
		return iPropertyAccess->HandleProperty(aFunction, a1, a2);
	case RMemoryAccess::EControlSetThreadPriority:
		return SetThreadPriority((TInt)a1, (TInt)a2);
	case RMemoryAccess::EControlNotifyThreadCreation:
		NotifyThreadCreation((TRequestStatus*)a1);
		return KErrNone;
	case RMemoryAccess::EControlCancelNotifyThreadCreation:
		CancelNotifyThreadCreation();
		return KErrNone;
	case RMemoryAccess::EControlSetPriorityOverride:
		return SetPriorityOverride((TInt)a1, a2);
	case RMemoryAccess::EControlSetCriticalFlags:
		return SetCriticalFlags((TInt)a1, (TUint)a2);
	case RMemoryAccess::EControlInPlaceThreadRename:
		{
		DThread* thread = ThreadFromHandle((TInt)a1);
		if (!thread) return 0;
		TBuf<64> buf;
		TInt err = Kern::ThreadDesRead(iClient, a2, buf, 0);
		if (err)
			{
			thread->Close(NULL);
			return err;
			}
		thread->iName->Copy(buf);
		TLinAddr addr = (TLinAddr)thread->iName;
		thread->Close(NULL);
		return addr;
		}
	case RMemoryAccess::EControlInPlaceObjectRename:
		return InPlaceObjectRename(a1, a2);
	case RMemoryAccess::EControlInPlaceSetProcessFileName:
		return InPlaceSetProcessFileName(a1, a2);
	case RMemoryAccess::EControlOpenThread:
		return OpenThread((TUint)a1);
    case RMemoryAccess::EControlResetTimer:
        return KErrNone;
	case RMemoryAccess::EControlGetContainerCount:
		return GetContainerCount(*(TObjectType*)&a1, *(TUint*)a2);
	case RMemoryAccess::EControlAcquireContainerMutex:
		return AcquireContainerMutex(*(TObjectType*)&a1);
	case RMemoryAccess::EControlReleaseContainerMutex:
		return ReleaseContainerMutex(*(TObjectType*)&a1);
    case RMemoryAccess::EControlGetObjectType:
    	return GetObjectType((TUint8*)a1, *(TObjectType*)a2);
    case RMemoryAccess::EControlGetObjectInfo:
		{
		TObjectInfoByIndexParamsBuf params;
		TInt err = Kern::ThreadDesRead(iClient,a1,params,0);
		if (err == KErrNone)
			{
			err = GetObjectInfo(params().iObjectType, params().iObjectIndex, (TDes8*)a2);
			}
		return err;
		}
    case RMemoryAccess::EControlGetObjectInfoByPtr:
		{
		TObjectInfoByPtrParamsBuf params;
		TInt err = Kern::ThreadDesRead(iClient,a1,params,0);
		if (err == KErrNone)
			{
			err = GetObjectInfoByPtr(params().iObjectType, params().iObjectPtr, (TDes8*)a2);
			}
		return err;
		}
    case RMemoryAccess::EControlGetObjectInfoByName:
		{
		TObjectInfoByNameParamsBuf params;
		TInt err = Kern::ThreadDesRead(iClient,a1,params,0);
		if (err == KErrNone)
			{
			err = GetObjectInfoByName(params().iObjectType, params().iObjectName, (TDes8*)a2);
			}
		return err;
		}
    case RMemoryAccess::EControlGetObjectInfoByHandle:
    	{
		TObjectInfoByHandleParamsBuf params;
		TInt err = Kern::ThreadDesRead(iClient,a1,params,0);
		if (err == KErrNone)
			{
			err = GetObjectInfoByHandle(params().iObjectType, params().iThreadId, params().iObjectHandle, (TDes8*)a2);
			}
		return err;
		}
    case RMemoryAccess::EControlAcquireCodeSegMutex:
    	return AcquireCodeSegMutex();
    case RMemoryAccess::EControlAcquireCodeSegMutexAndFilterCodesegsForProcess:
		return AcquireCodeSegMutexAndFilterCodesegsForProcess((TUint)a1);
    case RMemoryAccess::EControlReleaseCodeSegMutex:
    	return ReleaseCodeSegMutex();
    case RMemoryAccess::EControlGetNextCodeSegInfo:
    	return GetNextCodeSegInfo((TDes8*)a1);
    case RMemoryAccess::EControlObjectDie:
    	{
		TObjectKillParamsBuf params;
		TInt err = Kern::ThreadDesRead(iClient,a1,params,0);
		if (err == KErrNone)
			{
			err = ObjectDie(params);
			}
		return err;
    	}
    case RMemoryAccess::EControlGetObjectAddresses:
		{
		TGetObjectAddressesParamsBuf params;
		TInt err = Kern::ThreadDesRead(iClient,a1,params,0);
		if (err == KErrNone)
			{
			err = GetObjectAddresses(params().iObjectType,params().iOwningProcess,(TDes8*)a2);
			}
		return err;
		}
    case RMemoryAccess::EControlGetChunkAddresses:
		{
		TGetChunkAddressesParamsBuf params;
		TInt err = Kern::ThreadDesRead(iClient,a1,params,0);
		if (err == KErrNone)
			{
			err = GetChunkAddresses(params().iControllingProcessId,(TDes8*)a2);
			}
		return err;
		}
	case RMemoryAccess::EControlEnableHeapTracing:
		return EnableHeapTracing((TUint)a1, (TBool)a2);
	case RMemoryAccess::EControlDefragRam:
		return DefragRam((TInt)a1);
	case RMemoryAccess::EControlEmptyRamZone:
		return EmptyRamZone((TInt)a1, (TUint)a2);
	case RMemoryAccess::EControlGetRamZoneInfo:
		return GetRamZoneInfo((TUint)a1, a2);
	case RMemoryAccess::EControlSetProcessProperties:
		return SetProcessProperties((TInt)a1, a2);
	case RMemoryAccess::EControlWriteShadowMemory:
		return WriteShadowMemory((TLinAddr)a1, a2);
	case RMemoryAccess::EControlFreeShadowMemory:
		return FreeShadowMemory((TLinAddr)a1, (TInt)a2);
	case RMemoryAccess::EControlGetZombieDebugMode:
	case RMemoryAccess::EControlSetZombieDebugMode:
	case RMemoryAccess::EControlGetZombies:
	case RMemoryAccess::EControlReleaseZombie:
	case RMemoryAccess::EControlSuspendThread:
	case RMemoryAccess::EControlNotifyBreakpoint:
	case RMemoryAccess::EControlSetBreakpoint:
	case RMemoryAccess::EControlSetBreakpointEnabled:
	case RMemoryAccess::EControlClearBreakpoint:
	case RMemoryAccess::EControlContinueFromBreakpoint:
	case RMemoryAccess::EControlGetBreakpoints:
	case RMemoryAccess::EControlCancelNotifyBreakpoint:
	case RMemoryAccess::EControlRegisterPersistantBreakpoint:
	case RMemoryAccess::EControlSetSymbolicBreakpoint:
		return DebuggerFn(aFunction, a1, a2);
	case RMemoryAccess::EControlWriteMem:
		{
		TAny* args[3];
		TInt err = Kern::ThreadRawRead(iClient, a1, &args, 3*sizeof(TAny*));
		if (err) return err;
		TUint targetId = (TUint)args[0];
		TAny* srcAddr = args[1];
		TAny* destAddr = args[2];
		Kern::Containers()[EThread]->Wait();
		DThread* target = Kern::ThreadFromId(targetId);
		if (target) target->Open();
		Kern::Containers()[EThread]->Signal();
		if (!target) return KErrNotFound;
		TInt dataLen = Kern::ThreadGetDesLength(iClient, srcAddr);
		if (dataLen <= 0) err = dataLen;
		TAny* mem = NULL;
		if (!err) mem = Kern::Alloc(dataLen);
		if (!mem) err = KErrNoMemory;
		TPtr8 bah((TUint8*)mem, 0, dataLen);
		if (!err) err = Kern::ThreadDesRead(iClient, srcAddr, bah, dataLen);
		if (!err) err = Kern::ThreadRawWrite(target, destAddr, mem, dataLen);
		Kern::Free(mem);
		target->Close(NULL);
		return err;
		}
#ifdef FSHELL_BTRACE_SUPPORT // SetTextTraceMode went in at the same time as btrace, afaik
	case RMemoryAccess::EControlSetTextTraceMode:
		{
		TUint traceMode = 0;
		TInt err = Kern::ThreadRawRead(iClient, a1, &traceMode, sizeof(TUint));
		if (!err)
			{
			TUint origMode = Kern::SetTextTraceMode(traceMode, (TUint)a2);
			err = Kern::ThreadRawWrite(iClient, a1, &origMode, sizeof(TUint));
			}
		return err;
		}
#endif
	case RMemoryAccess::EControlGetRegisters:
		return GetRegisters(a1, a2);
	case RMemoryAccess::EControlSetDebugPort:
		Kern::SuperPage().iDebugPort = (TInt)a1;
		return KErrNone;
	case RMemoryAccess::EControlGetCreatorThread:
		{
		DDebuggerEventHandler* handler = NULL;
		TInt err = ((DMemoryAccessFactory*)iDevice)->GetEventHandler(handler);
		if (err) return err;
		TUint creator = handler->GetCreatorThread((TUint)a1);
		if (creator == 0)
			{
			err = KErrNotFound;
			}
		else
			{
			err = Kern::ThreadRawWrite(iClient, a2, &creator, sizeof(TUint));
			}
		return err;
		}
    default:
        return KErrNotSupported;
        }
    }
    
// DMemoryAccess::GetThreadMem
//  Gets a buffer of memory from a thread
TInt DMemoryAccess::GetThreadMem(TAny* aParams, TAny* aBuf)
	{
    TThreadMemoryAccessParams params;
	TPckg<TThreadMemoryAccessParams> paramsBuf(params);
	TInt err = Kern::ThreadDesRead(iClient, aParams, paramsBuf, 0);
	if (err) return err;

	// Copy the memory in page-sized chunks, so we don't have to allocate a huge buffer kernel-side
	TInt localBufSize = Min(params.iSize, KLocalBufferSize);
	HBuf8* localBuf = HBuf8::New(localBufSize);
	if (!localBuf) return KErrNoMemory;

	Kern::Containers()[EThread]->Wait();
	DThread* thread = Kern::ThreadFromId(params.iId);
	if (thread) thread->Open();
	Kern::Containers()[EThread]->Signal();
	if (!thread)
		{
		delete localBuf;
		return KErrNotFound;
		}
	
	TInt bytesCopied = 0;
	TUint8* localPtr = (TUint8*)localBuf->Ptr();
	while (!err && bytesCopied < params.iSize)
		{
		TInt toCopy = Min(params.iSize - bytesCopied, localBufSize);
		localBuf->SetLength(toCopy);
		err = Kern::ThreadRawRead(thread, params.iAddr + bytesCopied, localPtr, toCopy);
		if (err) break;
		err = Kern::ThreadDesWrite(iClient, aBuf, *localBuf, bytesCopied, 0, NULL);
		bytesCopied += toCopy;
		}
	delete localBuf;
	thread->Close(NULL);
	return err;
	}

// DMemoryAccess::GetHeapBase
//  Gets the base addess of a thread's heap
TInt DMemoryAccess::GetAllocatorAddress(TUint aId, TUint8*& aAddr, TBool aGetCurrent)
    {
    TInt err = KErrNone;
   
 	//Acquire the threads container mutex
	DObjectCon* const * cons = Kern::Containers();
	DObjectCon* threads = cons[EThread];
	threads->Wait();

	NKern::ThreadEnterCS();
	DThread* thread = Kern::ThreadFromId(aId);
	if (thread) thread->Open();
    NKern::ThreadLeaveCS();

    threads->Signal();

    if (!thread)
        err = KErrNotFound;

	if (!err)
		{
		TLinAddr allocator = DoGetAllocatorAddress(thread, aGetCurrent);
		thread->Close(NULL);
		err = Kern::ThreadRawWrite(iClient, &aAddr, (TUint8*)&allocator, sizeof(TUint8*));
    	}
	return err;
	}
    
TLinAddr DMemoryAccess::DoGetAllocatorAddress(DThread* aThread, TBool aGetCurrent)
	{
	RAllocator* allocator = NULL;

	allocator = aGetCurrent ? aThread->iAllocator : aThread->iCreatedAllocator;
	if (!allocator)
		{
		// Check if it's a kernel thread
		Kern::Containers()[EProcess]->Wait();
		DProcess* kernProc = Kern::ProcessFromId(1);
		Kern::Containers()[EProcess]->Signal();
		if (aThread->iOwningProcess == kernProc)
			{
			// GeorgeN's cunning approach - the kern DProcess is one of the first things allocated to the kern heap and the heap is page aligned, so rounding down the dprocess pointer will give you the RHeapK addresss
			allocator = (RAllocator*)(Kern::RoundToPageSize((TLinAddr)kernProc) - Kern::RoundToPageSize(1));
			}
		}

	return (TLinAddr)allocator;
    }

// DMemoryAccess::GetContainerCount
//  Gets the object count of a kernel container specified by aObjectType
TInt DMemoryAccess::GetContainerCount(const TObjectType aObjectType, TUint& aCount)
	{
	// Kern::Printf("[DMemoryAccess] ::GetContainerCount");
	TInt err=KErrNone;
	if (aObjectType<0 || aObjectType>= ENumObjectTypes)
		{
		Kern::ThreadKill(iClient,EExitPanic,EMemAccessPanicBadContainerType,KMemAccessPanicCategory);
		}
	
	DObjectCon* const * cons = Kern::Containers();
	DObjectCon* container = cons[aObjectType];
	container->Wait();
	
    TUint count=container->Count();
    err = Kern::ThreadRawWrite(iClient, &aCount, (TUint8*)&count, sizeof(TUint));
    
    container->Signal();
	// Kern::Printf("[DMemoryAccess] ::GetContainerCount returning %d; count %d",err,count);
	return err;    	
	}
	
// DMemoryAccess::AcquireContainerMutex
//  Acquire a mutex on the specified kernel container
//  Must be called before attempting to retrieve any info from the container
//  Must only be called once per container
TInt DMemoryAccess::AcquireContainerMutex(const TObjectType aObjectType)
	{
	// Kern::Printf("[DMemoryAccess] ::AcquireContainerMutex, object type %d", aObjectType);
	TInt err=KErrNone;
	if (aObjectType<0 || aObjectType>= ENumObjectTypes)
		{ //Check this is a valid container type
		Kern::ThreadKill(iClient,EExitPanic,EMemAccessPanicBadContainerType,KMemAccessPanicCategory);
		err=KErrArgument;
		}
	else
		{
		//Make sure none of the containers are already locked
		for (TInt ii=0; ii<ENumObjectTypes;ii++)
			{
			if (iLocks[ii])
				{ //Check the lock on this container is not currently held
				Kern::ThreadKill(iClient,EExitPanic,EMemAccessMutexAlreadyHeld,KMemAccessPanicCategory);
				}		
			}
		
		DObjectCon* const * cons = Kern::Containers();
		DObjectCon* container = cons[aObjectType];
		container->Wait();
		
		iLocks[aObjectType]=ETrue;
		}
	// Kern::Printf("[DMemoryAccess] ::AcquireContainerMutex returning %d", err);
	return err;    	
	}
	
// DMemoryAccess::ReleaseContainerMutex
//  Releases a mutex on the specified kernel container
//  Must only be called when the mutex on the container is already held
TInt DMemoryAccess::ReleaseContainerMutex(const TObjectType aObjectType)
	{
	// Kern::Printf("[DMemoryAccess] ::ReleaseContainerMutex, object type %d", aObjectType);
	TInt err=KErrNone;
	if (aObjectType<0 || aObjectType>= ENumObjectTypes)
		{ //Check this is a valid container type
		Kern::ThreadKill(iClient,EExitPanic,EMemAccessPanicBadContainerType,KMemAccessPanicCategory);
		err=KErrArgument;
		}
	if (!iLocks[aObjectType])
		{ //Check the lock on this container is currently held
		Kern::ThreadKill(iClient,EExitPanic,EMemAccessMutexNotHeld,KMemAccessPanicCategory);
		err=KErrGeneral;
		}

	if (err==KErrNone)
		{
		DObjectCon* const * cons = Kern::Containers();
		DObjectCon* container = cons[aObjectType];
		__ASSERT_MUTEX(container->Lock());

	    container->Signal();
		iLocks[aObjectType]=EFalse;
		}
	// Kern::Printf("[DMemoryAccess] ::ReleaseContainerMutex returning %d", err);
	return err;    	
	}

// DMemoryAccess::GetObjectInfo
//  Utility function to extract generic info from a DObject
void DMemoryAccess::GetObjectInfo(DObject* aObject, TObjectKernelInfo& aInfo)
	{
	//Kern::Printf("[DMemoryAccess] ::GetObjectInfo");
	if (aObject)
		{
		aInfo.iAddressOfKernelObject=(TUint8*)aObject;
		aObject->FullName(aInfo.iFullName);
		aObject->Name(aInfo.iName);
		aInfo.iAccessCount=aObject->AccessCount();
		aInfo.iUniqueID=aObject->UniqueID();
		aInfo.iProtection=aObject->Protection();
		aInfo.iAddressOfKernelOwner=(TUint8*)aObject->Owner();			
		}
	//Kern::Printf("[DMemoryAccess] ::GetObjectInfo returning");
	}

// DMemoryAccess::GetObjectType
//  Finds the type of a kernel side DObject pointer by iterating the kernel containers
//  Requires that none of the kernel containers are already locked
//  returns KErrNotFound if the object does not exist in any of the kernel's containers
TInt DMemoryAccess::GetObjectType(TUint8* aObjectPtr, TObjectType& aType)
	{
	// Kern::Printf("[DMemoryAccess] ::GetObjectType");
	TInt err=KErrNotFound;
	//Make sure none of the containers are already locked
	for (TInt ii=0; ii<ENumObjectTypes;ii++)
		{
		if (iLocks[ii])
			{ //Check the lock on this container is not currently held
			Kern::ThreadKill(iClient,EExitPanic,EMemAccessMutexAlreadyHeld,KMemAccessPanicCategory);
			err=KErrGeneral;
			}		
		}

	if (err==KErrNone || err==KErrNotFound)
		{
		// Kern::Printf("[DMemoryAccess] Looking for object 0x%08X",aObjectPtr);
		for (TInt ii=EThread; ii<ENumObjectTypes; ii++)
			{
			TInt posInContainer=KErrNotFound;
			DObjectCon* const * cons = Kern::Containers();
			DObjectCon& container = *cons[ii];
			container.Wait();
			if (GetObjectPositionInContainer(aObjectPtr, container, posInContainer)==KErrNone)
				{
				// Kern::Printf("[DMemoryAccess] ::GetObjectType found type %d",ii);
				err = Kern::ThreadRawWrite(iClient, &aType, (TUint8*)&ii, sizeof(TObjectType));
				ii=ENumObjectTypes; //prevent further loop iterations
				}
			container.Signal();
			}
		}
	// Kern::Printf("[DMemoryAccess] ::GetObjectType returning %d",err);
	return err;
	}

// DMemoryAccess::ObjectPositionInContainer
//  Finds the position in the aContainer of the aObjectPtr kernel object
//  Returns KErrNone if found, KErrNotFound if not
//  Position in container will be returned in aPosInContainer
//  The container mutex corresponding to the specified container must be already locked
TInt DMemoryAccess::GetObjectPositionInContainer(TUint8* aObjectPtr, DObjectCon& aContainer, TInt& aPosInContainer)
	{
	// Kern::Printf("[DMemoryAccess] ::GetObjectPositionInContainer");
	TInt err=KErrNotFound;
	aPosInContainer=KErrNotFound;
	
	for (TInt jj=0; jj<aContainer.Count();jj++)
		{
		DObject* obj = (DObject*)aContainer[jj];
		if ((TUint8*)obj == aObjectPtr)
			{
			aPosInContainer=jj;
			err=KErrNone;
			continue;
			}
		}
	// Kern::Printf("[DMemoryAccess] ::GetObjectPositionInContainer returning %d; position %d",err,aPosInContainer);
	return err;
	}

// DMemoryAccess::GetObjectInfo
//  Provides data about a kernel object
//  aObjectType specifies the type of object
//  aObjectIndexInContainer specifies the index of the required object in the respective kernel container
//  aKernelInfoBuf provides a buffer to populate with info.  Should be a derived class of TObjectKernelInfo corresponding to the object type
//  The appropriate kernel container must already be locked
TInt DMemoryAccess::GetObjectInfo(TObjectType aObjectType, TInt aObjectIndexInContainer, TDes8* aKernelInfoBuf)
	{
	//Kern::Printf("[DMemoryAccess] ::GetObjectInfo");
	TInt err=KErrNone;

	TBool alreadyHeld = iLocks[aObjectType];
	if (!alreadyHeld)
		{
		// Automatically grab the lock
		DObjectCon* const * cons = Kern::Containers();
		DObjectCon& container = *cons[aObjectType];
		container.Wait();
		iLocks[aObjectType] = ETrue;
		}

	if (!iLocks[aObjectType])
		{ //Check the lock on this container is currently held
		Kern::ThreadKill(iClient,EExitPanic,EMemAccessMutexNotHeld,KMemAccessPanicCategory);
		//Kern::Printf("[DMemoryAccess] ::GetObjectInfo: No Lock!");
		err=KErrGeneral;
		}
	else
		{
		DObjectCon* const * cons = Kern::Containers();
		DObjectCon& container = *cons[aObjectType];
		//Kern::Printf("[DMemoryAccess] ::GetObjectInfo: Asserting lock...");
		__ASSERT_MUTEX(container.Lock());
		//Kern::Printf("[DMemoryAccess] ::GetObjectInfo: Lock OK");

		// Bounds check index: because we acquire and release the locks more, we risk the container changing size after the client has got the size
		if (aObjectIndexInContainer >= container.Count())
			{
			err = KErrOverflow;
			goto returnAndReleaseLock; // Not elegant, and in fact the first goto I've used in over 5 years
			}

		DObject* object = (DObject*)container[aObjectIndexInContainer];

		//Get the object info according to its type
		//Kern::Printf("[DMemoryAccess] ::GetObjectInfo: Calling specific function");
		switch (aObjectType)
			{
		case EThread:
			err=GetThreadInfo((DThread*)object, aKernelInfoBuf);
			break;
		case EProcess:
			err=GetProcessInfo((DProcess*)object, aKernelInfoBuf);
			break;
		case EChunk:
			err=GetChunkInfo((DChunk*)object, aKernelInfoBuf);
			break;
		case ELibrary:
			err=GetLibraryInfo((DLibrary*)object, aKernelInfoBuf);
			break;
		case ESemaphore:
			err=GetSemaphoreInfo((DSemaphore*)object, aKernelInfoBuf);
			break;
		case EMutex:
			err=GetMutexInfo((DMutex*)object, aKernelInfoBuf);
			break;
		case ETimer:
			err=GetTimerInfo((DTimer*)object, aKernelInfoBuf);
			break;
		case EServer:
			err=GetServerInfo((DServer*)object, aKernelInfoBuf);
			break;
		case ESession:
			err=GetSessionInfo((DSession*)object, aKernelInfoBuf);
			break;
		case ELogicalDevice:
			err=GetLogicalDeviceInfo((DLogicalDevice*)object, aKernelInfoBuf);
			break;
		case EPhysicalDevice:
			err=GetPhysicalDeviceInfo((DPhysicalDevice*)object, aKernelInfoBuf);
			break;
		case ELogicalChannel:
			err=GetLogicalChannelInfo((DLogicalChannel*)object, aKernelInfoBuf);
			break;
		case EChangeNotifier:
			err=GetChangeNotifierInfo((DChangeNotifier*)object, aKernelInfoBuf);
			break;
		case EUndertaker:
			err=GetUndertakerInfo((DUndertaker*)object, aKernelInfoBuf);
			break;
		case EMsgQueue:	
			err=GetMsgQueueInfo((DMsgQueue*)object, aKernelInfoBuf);
			break;
		case EPropertyRef:
//			err=GetPropertyRefInfo((DPropertyRef*)object, aKernelInfoBuf);
			break;
		case ECondVar:
			err=GetCondVarInfo((DCondVar*)object, aKernelInfoBuf);
			break;
		default:
			err=KErrNotFound;
			}
		}
returnAndReleaseLock:
	if (!alreadyHeld)
		{
		// Release the lock if we grabbed it automatically
		DObjectCon* const * cons = Kern::Containers();
		DObjectCon& container = *cons[aObjectType];
		container.Signal();
		iLocks[aObjectType] = alreadyHeld;
		}
	//Kern::Printf("[DMemoryAccess] ::GetObjectInfo returning %d",err);
	return err;    				
	}
	
// DMemoryAccess::GetObjectInfoByPtr
//  Provides data about a kernel object
//  aObjectType specifies the type of object
//  aObjectPtr specifies address of the required DObject in the kernel
//  aKernelInfoBuf provides a buffer to populate with info.  Should be a derived class of TObjectKernelInfo corresponding to the object type
//  No kernel container locks other than the one selected should be held
TInt DMemoryAccess::GetObjectInfoByPtr(TObjectType aObjectType, TUint8* aObjectPtr, TDes8* aKernelInfoBuf)
	{
	// Kern::Printf("[DMemoryAccess] ::GetObjectInfoByPtr");
	TInt posInContainer=KErrNotFound;
	DObjectCon* const * cons = Kern::Containers();
	DObjectCon& container = *cons[aObjectType];
	container.Wait();
	TBool alreadyHeld = iLocks[aObjectType];
	iLocks[aObjectType] = ETrue;
	
	TInt err=GetObjectPositionInContainer(aObjectPtr,container,posInContainer);

	if (err==KErrNone)
		err=GetObjectInfo(aObjectType, posInContainer, aKernelInfoBuf);
	
	container.Signal();
	iLocks[aObjectType] = alreadyHeld;
	return err;
	}

// DMemoryAccess::GetObjectInfoByName
//  Provides data about a kernel object
//  aObjectType specifies the type of object
//  aObjectName specifies name of the required DObject in the kernel
//  aKernelInfoBuf provides a buffer to populate with info.  Should be a derived class of TObjectKernelInfo corresponding to the object type
//  No kernel container locks other than the one selected should be held
TInt DMemoryAccess::GetObjectInfoByName(TObjectType aObjectType, const TDesC8& aObjectName, TDes8* aKernelInfoBuf)
	{
	DObjectCon* const * cons = Kern::Containers();
	DObjectCon& container = *cons[aObjectType];
	container.Wait();
	TBool alreadyHeld = iLocks[aObjectType];
	iLocks[aObjectType] = ETrue;

	TInt err = KErrNotFound;
	for (TInt jj=0; jj<container.Count();jj++)
		{
		DObject* obj = (DObject*)container[jj];
		TBuf8<KMaxFullName> fullName;
		obj->FullName(fullName);
		if (fullName == aObjectName)
			{
			err=GetObjectInfo(aObjectType, jj, aKernelInfoBuf);
			break;
			}
		}
	
	container.Signal();
	iLocks[aObjectType] = alreadyHeld;
	return err;
	}

// DMemoryAccess::GetObjectInfoByHandle
//  Provides data about a kernel object
//  aObjectType specifies the type of object
//  aThreadHandle specifies the user side handle of the thread in which context to find the required object
//  aObjectHandle specifies the user side handle of the required object
//  aKernelInfoBuf provides a buffer to populate with info.  Should be a derived class of TObjectKernelInfo corresponding to the object type
//  No kernel container locks other than the one selected should be held
TInt DMemoryAccess::GetObjectInfoByHandle(TObjectType aObjectType, TInt aThreadId, TInt aObjectHandle, TDes8* aKernelInfoBuf)
	{
	// Kern::Printf("[DMemoryAccess] ::GetObjectInfoByHandle");
	TInt err=KErrNone;	

	DObjectCon* const * cons = Kern::Containers();
	DObjectCon& container = *cons[EThread];
	container.Wait();
	NKern::ThreadEnterCS();
	DThread* thread = Kern::ThreadFromId(aThreadId);
    NKern::ThreadLeaveCS();
	container.Signal();

	if (!thread)
		{
		err=KErrNotFound;
		}
	else
		{
		DObject* object;

		if (aObjectHandle == (KCurrentThreadHandle&~KHandleNoClose))
			object = thread;
		else if (aObjectHandle == (KCurrentProcessHandle&~KHandleNoClose))
			object = thread->iOwningProcess;
		else
			{
			NKern::ThreadEnterCS();
			NKern::LockSystem();
			object = thread->ObjectFromHandle(aObjectHandle,aObjectType);
			NKern::UnlockSystem();
			NKern::ThreadLeaveCS();
			}

		if (!object)
			{
			err=KErrNotFound;
			}
		else
			{
			err=GetObjectInfoByPtr(aObjectType, (TUint8*)object, aKernelInfoBuf);
			}
		}

	return err;
	}


// DMemoryAccess::AcquireCodeSegMutex
//  Acquires the code segment mutex
TInt DMemoryAccess::AcquireCodeSegMutex()
	{
	// Kern::Printf("[DMemoryAccess] ::AcquireCodeSegMutex");
	TInt err=KErrNone;

	if (iCodeSegLock)
		{ //Check the lock on code segs is not currently held
		Kern::ThreadKill(iClient, EExitPanic, EMemAccessMutexAlreadyHeld, KMemAccessPanicCategory);
		err = KErrAbort;
		}

	DMutex* const codeSegLock = Kern::CodeSegLock();

	if (!err)
		{
		if (codeSegLock)
			{
			Kern::MutexWait(*codeSegLock);
			iCodeSegLock = EHeld;
			}
		else
			{
			err = KErrNotFound;
			}
		}
	// Kern::Printf("[DMemoryAccess] ::AcquireCodeSegMutex returning %d", err);
	return err;    	
	}

TInt DMemoryAccess::AcquireCodeSegMutexAndFilterCodesegsForProcess(TUint aProcessId)
	{
	TInt err = KErrNone;
	NKern::ThreadEnterCS();
	Kern::Containers()[EProcess]->Wait();
	DProcess* process = Kern::ProcessFromId(aProcessId);
	if (process == NULL || process->Open() != KErrNone)
		{
		err = KErrNotFound;
		}
	Kern::Containers()[EProcess]->Signal();

	if (!err)
		{
		err = AcquireCodeSegMutex();
		}
	if (!err)
		{
		iCodeSegLock = EHeldAndTraversing;
		TInt numTraversed = process->TraverseCodeSegs(&iFilteredCodesegQue, NULL, DCodeSeg::EMarkDebug, DProcess::ETraverseFlagAdd);
		err = numTraversed;
		}

	if (process) process->Close(NULL);
	NKern::ThreadLeaveCS();
	return err;
	}

TInt DMemoryAccess::GetObjectAddresses(TObjectType aObjectType, const TDesC8& aOwningProcess, TDes8* aKernelInfoBuf)
	{
	DObjectCon* const * cons = Kern::Containers();
	DObjectCon& container = *cons[aObjectType];
	container.Wait();
	TBool alreadyHeld = iLocks[aObjectType];
	iLocks[aObjectType] = ETrue;

	TInt maxLength = Kern::ThreadGetDesMaxLength(iClient, aKernelInfoBuf);
	TInt err = KErrNone;
	TInt offset = 0;

	for (TInt jj=0;jj<container.Count();jj++)
		{
		if (offset >= maxLength)
			{
			err = KErrOverflow;
			break;
			}

		TBool doAppend(EFalse);
		DObject* obj = (DObject*)container[jj];
		if (aOwningProcess.Length() > 0)
			{
			if (obj->Owner())
				{
				TBuf8<KMaxFullName> fullName;
				obj->Owner()->FullName(fullName);
				if (fullName.MatchF(aOwningProcess) == 0)
					{
					doAppend = ETrue;
					}
				}
			}
		else
			{
			doAppend = ETrue;
			}


		if (doAppend)
			{
			err = Kern::ThreadDesWrite(iClient, aKernelInfoBuf, TPtrC8((TUint8*)&obj, sizeof(TAny*)), offset, KChunkShiftBy0, iClient);
			if (err)
				{
				break;
				}
			else
				{
				offset += sizeof(TAny*);
				}
			}
		}
	
	container.Signal();
	iLocks[aObjectType] = alreadyHeld;
	return err;
	}

TInt DMemoryAccess::GetChunkAddresses(TUint aControllingProcessId, TDes8* aKernelInfoBuf)
	{
	DObjectCon* const * cons = Kern::Containers();
	DObjectCon& container = *cons[EChunk];
	container.Wait();
	TBool alreadyHeld = iLocks[EChunk];
	iLocks[EChunk] = ETrue;

	TInt maxLength = Kern::ThreadGetDesMaxLength(iClient, aKernelInfoBuf);
	const TInt maxPtrs = maxLength / sizeof (TUint32*);
	TInt err = KErrNone;

	if (maxPtrs == 0)
		{
		err = KErrOverflow;
		}
	else
		{
		TInt offset = 0;

		for (TInt jj=0;jj<container.Count();jj++)
			{
			DChunk* chunk = (DChunk*)container[jj];
			if ((aControllingProcessId == KNullProcessId) || (chunk->iControllingOwner == aControllingProcessId))
				{
				err = Kern::ThreadDesWrite(iClient, aKernelInfoBuf, TPtrC8((TUint8*)&chunk, sizeof(TAny*)), offset, KChunkShiftBy0, iClient);
				if (err)
					{
					break;
					}
				else
					{
					offset += sizeof(TAny*);
					if (offset >= maxPtrs)
						{
						err = KErrOverflow;
						break;
						}
					}
				}
			}
		}
	
	container.Signal();
	iLocks[EChunk] = alreadyHeld;
	return err;
	}


// DMemoryAccess::ReleaseCodeSegMutex
//  Releases the code segment mutex
TInt DMemoryAccess::ReleaseCodeSegMutex()
	{
	// Kern::Printf("[DMemoryAccess] ::ReleaseCodeSegMutex");
	if (iCodeSegLock == ENotHeld)
		{
		return KErrNotReady;
		}

	if (iCodeSegLock == EHeldAndTraversing)
		{
		DCodeSeg::EmptyQueue(iFilteredCodesegQue, DCodeSeg::EMarkDebug);
		}

	Kern::MutexSignal(*Kern::CodeSegLock());
	iCodeSegLock = ENotHeld;
	// Kern::Printf("[DMemoryAccess] ::ReleaseCodeSegMutex returning %d", err);
	return KErrNone;
	}


TInt DMemoryAccess::GetNextCodeSegInfo(TDes8* aCodeSegInfoBuf)
	{
	// Kern::Printf("[DMemoryAccess] ::GetNextCodeSegInfo");
	TInt err = KErrNone;
	
	SDblQue* p;
	if (iCodeSegLock == EHeldAndTraversing)
		{
		p = &iFilteredCodesegQue;
		}
	else
		{
		p = Kern::CodeSegList();
		}
	SDblQueLink* anchor=&p->iA;

	if (!iCurrentCodeSegLink)
		{
		iCurrentCodeSegLink = anchor;
		}

	if (iCurrentCodeSegLink->iNext != anchor)
		{
		iCurrentCodeSegLink = iCurrentCodeSegLink->iNext;
		}
	else
		{
		iCurrentCodeSegLink = NULL;
		err = KErrNotFound;
		}
		
	if (!err)
		{
		//TCodeSegKernelInfoBuf* localInfoBuf = new TCodeSegKernelInfoBuf;
		DCodeSeg* currentCodeseg = (iCodeSegLock == EHeldAndTraversing) ? _LOFF(iCurrentCodeSegLink, DCodeSeg, iTempLink) : _LOFF(iCurrentCodeSegLink, DCodeSeg, iLink);
		TPckgBuf<TCodeSegKernelInfo>* localInfoBuf = new TPckgBuf<TCodeSegKernelInfo>;
		if (!localInfoBuf)
			{
			err = KErrNoMemory;
			}
		else
			{	
			//Get the code seg info
			(*localInfoBuf)().iRunAddress = currentCodeseg->iRunAddress;
			(*localInfoBuf)().iSize = currentCodeseg->iSize;
			if (currentCodeseg->iFileName)
				{
				(*localInfoBuf)().iFileName = currentCodeseg->iFileName->Right((*localInfoBuf)().iFileName.MaxLength());
				}
			(*localInfoBuf)().iAccessCount = currentCodeseg->iAccessCount;
			(*localInfoBuf)().iAddressOfKernelObject = (TUint8*)currentCodeseg;
			(*localInfoBuf)().iName = currentCodeseg->iRootName;
			(*localInfoBuf)().iDepCount = currentCodeseg->iDepCount;
#ifdef __EPOC32__
			(*localInfoBuf)().iXip = ((DEpocCodeSeg*)currentCodeseg)->iXIP;
#else
			(*localInfoBuf)().iXip = EFalse;
#endif
			//Copy the local info buffer into the client's address space
		    err = Kern::ThreadDesWrite(iClient, aCodeSegInfoBuf, *localInfoBuf, 0, KTruncateToMaxLength, NULL);
			delete localInfoBuf;
			}
		}

	// Kern::Printf("[DMemoryAccess] ::GetNextCodeSegInfo returning %d", err);
	return err;
	}

TInt DMemoryAccess::ObjectDie(TObjectKillParamsBuf& aObjectKillParamsBuf)
	{
	TInt err=KErrNone;
	DObjectCon* const * cons = Kern::Containers();

	switch(aObjectKillParamsBuf().iObjectType)
		{
	case EThread:
		{
		//Use the address of the kernel object ...
		DThread* thread = (DThread*)aObjectKillParamsBuf().iObjectPtr;
		//... unless the client has supplied an object ID
		if (aObjectKillParamsBuf().iObjectId!=0)
			{
			DObjectCon& container = *cons[EThread];
			container.Wait();
			NKern::ThreadEnterCS();
			thread = Kern::ThreadFromId(aObjectKillParamsBuf().iObjectId);
		    NKern::ThreadLeaveCS();
			container.Signal();
			}
		if (thread==NULL)
			err=KErrNotFound;
		else
			Kern::ThreadKill(thread, aObjectKillParamsBuf().iType, aObjectKillParamsBuf().iReason, aObjectKillParamsBuf().iCategory);
		break;
		}
	case EProcess:
		{
		//Use the address of the kernel object ...
		DProcess* process = (DProcess*)aObjectKillParamsBuf().iObjectPtr;
		//... unless the client has supplied an object ID
		if (aObjectKillParamsBuf().iObjectId!=0)
			{
			DObjectCon& container = *cons[EProcess];
			container.Wait();
			NKern::ThreadEnterCS();
			process = Kern::ProcessFromId(aObjectKillParamsBuf().iObjectId);
		    NKern::ThreadLeaveCS();
			container.Signal();
			}
		if (process==NULL)
			err=KErrNotFound;
		else
			{
			Kern::ThreadKill(process->FirstThread(), aObjectKillParamsBuf().iType, aObjectKillParamsBuf().iReason, aObjectKillParamsBuf().iCategory);
			}
		break;
		}
	default:
		err=KErrNotSupported;
		break;
		}
	return err;
	}
	
	
TInt DMemoryAccess::GetThreadInfo(DThread* aThread, TDes8* aThreadInfoBuf)
	{
	// Kern::Printf("[DMemoryAccess] ::GetThreadInfo");
	TInt err=KErrNone;
	//construct local info buffer to populate
	TThreadKernelInfoBuf* localInfoBuf = new TThreadKernelInfoBuf;	
	if (!localInfoBuf)
		{
		err=KErrNoMemory;
		}
	else
		{
		//Get the standard DObject info
		TThreadKernelInfo& localInfo = (*localInfoBuf)();
		GetObjectInfo(aThread, localInfo);
		//Get the thread specific info
		localInfo.iThreadId=aThread->iId;
		localInfo.iAddressOfOwningProcess=(TUint8*)aThread->iOwningProcess;
		localInfo.iThreadPriority = aThread->iDefaultPriority; // iDefaultPriority is more useful to know than iThreadPriority
		localInfo.iUserStackSize = aThread->iUserStackSize;
		localInfo.iFlags = aThread->iFlags;
		localInfo.iSupervisorStack = (TLinAddr)aThread->iSupervisorStack;
		localInfo.iSupervisorStackSize = aThread->iSupervisorStackSize;
		localInfo.iUserStackLimit = aThread->iUserStackRunAddress;
		localInfo.iNThreadSuspendCount = aThread->iNThread.iSuspendCount;
		//Copy the local info buffer into the client's address space
	    err = Kern::ThreadDesWrite(iClient, aThreadInfoBuf, *localInfoBuf, 0, KTruncateToMaxLength, NULL);	
	    delete localInfoBuf;
		}
	return err;    		
	}

TInt DMemoryAccess::GetProcessInfo(DProcess* aProcess, TDes8* aProcessInfoBuf)
	{
	//Kern::Printf("[DMemoryAccess] ::GetProcessInfo");
	TInt err=KErrNone;

	//construct local info buffer to populate
	TProcessKernelInfoBuf* localInfoBuf = new TProcessKernelInfoBuf;
	if (!localInfoBuf)
		{
		err=KErrNoMemory;
		}
	else
		{
		//Get the standard DObject info
		// Kern::Printf("DMemoryAccess::GetProcessInfo getting object info");
		TProcessKernelInfo& localInfo = (*localInfoBuf)();
		GetObjectInfo (aProcess, localInfo);
		//Get the process specific info
		localInfo.iProcessId = aProcess->iId;
		localInfo.iPriority = aProcess->iPriority;
		localInfo.iAddressOfOwningProcess = (TUint8*)aProcess->iOwningProcess;
		localInfo.iCreatorId = aProcess->iCreatorId;
		localInfo.iSecurityZone = aProcess->iSecurityZone;
		localInfo.iUids = aProcess->iUids;
		localInfo.iAttributes = aProcess->iAttributes;
		localInfo.iFlags = aProcess->iFlags;
		localInfo.iAddressOfDataBssStackChunk = (TUint8*)aProcess->iDataBssStackChunk;
		TInt ii=0;
	    SDblQueLink* pLink=aProcess->iThreadQ.First();
		while ((pLink!=&aProcess->iThreadQ.iA) && (ii<KQueBufferLength))
			 {
			 DThread* pT=_LOFF(pLink,DThread,iProcessLink);
			 localInfo.iThreadQ.iBuffer[ii]=(TUint8*)pT;
			 pLink=pLink->iNext;
			 ii++;
			 }
		localInfo.iThreadQ.iStartFrom=0;
		localInfo.iThreadQ.iCount=ii;
		localInfo.iProcessSecurityInfo=aProcess->iS;
		localInfo.iProcessCreatorSecurityInfo=aProcess->iCreatorInfo;
		HBuf* commandLine = aProcess->iCommandLine;
		if (commandLine)
			{
			localInfo.iCommandLine = commandLine->Left(localInfo.iCommandLine.MaxLength());
			}
		memcpy(localInfo.iEnvironmentData, aProcess->iEnvironmentData, sizeof(TInt)*KNumEnvironmentSlots);
		localInfo.iFirstThreadId = 0;
		if (aProcess->FirstThread()) localInfo.iFirstThreadId = aProcess->FirstThread()->iId;

		//Copy the local info buffer into the client's address space
		// Kern::Printf("DMemoryAccess::GetProcessInfo writing back");
	    err = Kern::ThreadDesWrite(iClient, aProcessInfoBuf, *localInfoBuf, 0, KTruncateToMaxLength, NULL);	
		delete localInfoBuf;
		}
	return err;
	}

	
TInt DMemoryAccess::GetChunkInfo(DChunk* aChunk, TDes8* aChunkInfoBuf)
	{
	// Kern::Printf("[DMemoryAccess] ::GetChunkInfo");
	TInt err=KErrNone;
	//construct local info buffer to populate
	TChunkKernelInfoBuf* localInfoBuf = new TChunkKernelInfoBuf;	
	if (!localInfoBuf)
		{
		err=KErrNoMemory;
		}
	else
		{
		TChunkKernelInfo& localInfo = (*localInfoBuf)();
		//Get the standard DObject info
		GetObjectInfo (aChunk, localInfo);
		//Get the chunk specific info
		localInfo.iSize = aChunk->Size();
		localInfo.iMaxSize = aChunk->MaxSize();
		localInfo.iBase = aChunk->Base();
		localInfo.iBottom = aChunk->Bottom();
		localInfo.iTop = aChunk->Top();
		localInfo.iAttributes = aChunk->iAttributes;
		localInfo.iStartPos = aChunk->iStartPos;
		localInfo.iControllingOwnerProcessId = aChunk->iControllingOwner;
		localInfo.iRestrictions = aChunk->iRestrictions;
		localInfo.iMapAttr = aChunk->iMapAttr;
		localInfo.iChunkType = aChunk->iChunkType;
		localInfo.iAddressOfOwningProcess = (TUint8*)aChunk->iOwningProcess;
#ifdef FSHELL_FLEXIBLEMM_AWARE
		if (localInfo.iBase == 0)
			{
			_LIT8(KKern, "ekern");
			DProcess* proc = aChunk->iOwningProcess;
			if (localInfo.iFullName.Left(KKern().Length()) != KKern && proc && proc->Open() == KErrNone)
				{
				// Probably shouldn't call ChunkUserBase for a non-user-owned chunk
				localInfo.iBase = Kern::ChunkUserBase(aChunk, proc->FirstThread());
				proc->Close(NULL);
				}
			}
#endif
		//Copy the local info buffer into the client's address space
	    err = Kern::ThreadDesWrite(iClient, aChunkInfoBuf, *localInfoBuf, 0, KTruncateToMaxLength, NULL);	
		delete localInfoBuf;
		}
	return err;    		
	}
	
TInt DMemoryAccess::GetLibraryInfo(DLibrary* aLibrary, TDes8* aLibraryInfoBuf)
	{
	// Kern::Printf("[DMemoryAccess] ::GetLibraryInfo");
	TInt err=KErrNone;
	//construct local info buffer to populate
	TLibraryKernelInfoBuf* localInfoBuf = new TLibraryKernelInfoBuf;	
	if (!localInfoBuf)
		{
		err=KErrNoMemory;
		}
	else
		{
		//Get the standard DObject info
		GetObjectInfo (aLibrary, (*localInfoBuf)());
		//Get the library specific info
		(*localInfoBuf)().iMapCount = aLibrary->iMapCount;
		(*localInfoBuf)().iState = aLibrary->iState;
		(*localInfoBuf)().iAddressOfCodeSeg = (TUint8*)aLibrary->iCodeSeg;

		//Copy the local info buffer into the client's address space
	    err = Kern::ThreadDesWrite(iClient, aLibraryInfoBuf, *localInfoBuf, 0, KTruncateToMaxLength, NULL);	
		delete localInfoBuf;
		}
	return err;    		
	}

TInt DMemoryAccess::GetSemaphoreInfo(DSemaphore* aSemaphore, TDes8* aSemaphoreInfoBuf)
	{
	// Kern::Printf("[DMemoryAccess] ::GetSemaphoreInfo");
	TInt err=KErrNone;
	//construct local info buffer to populate
	TSemaphoreKernelInfoBuf* localInfoBuf = new TSemaphoreKernelInfoBuf;	
	if (!localInfoBuf)
		{
		err=KErrNoMemory;
		}
	else
		{
		//Get the standard DObject info
		GetObjectInfo (aSemaphore, (*localInfoBuf)());
		//Get the Semaphore specific info
		(*localInfoBuf)().iCount = aSemaphore->iCount;
		(*localInfoBuf)().iResetting = aSemaphore->iResetting;
		TInt ii=0;
	    SDblQueLink* pLink=aSemaphore->iSuspendedQ.First();
		while ((pLink!=&aSemaphore->iSuspendedQ.iA) && (ii<KQueBufferLength))
			 {
			 DThread* pT=_LOFF(pLink,DThread,iWaitLink);
			 (*localInfoBuf)().iSuspendedQ.iBuffer[ii]=(TUint8*)pT;
			 pLink=pLink->iNext;
			 ii++;
			 }
		(*localInfoBuf)().iSuspendedQ.iStartFrom=0;
		(*localInfoBuf)().iSuspendedQ.iCount=ii;

		//Copy the local info buffer into the client's address space
	    err = Kern::ThreadDesWrite(iClient, aSemaphoreInfoBuf, *localInfoBuf, 0, KTruncateToMaxLength, NULL);	
		delete localInfoBuf;
		}
	return err;    		
	}

TInt DMemoryAccess::GetMutexInfo(DMutex* aMutex, TDes8* aMutexInfoBuf)
	{
	// Kern::Printf("[DMemoryAccess] ::GetMutexInfo");
	TInt err=KErrNone;
	//construct local info buffer to populate
	TMutexKernelInfoBuf* localInfoBuf = new TMutexKernelInfoBuf;	
	if (!localInfoBuf)
		{
		err=KErrNoMemory;
		}
	else
		{
		//Get the standard DObject info
		GetObjectInfo (aMutex, (*localInfoBuf)());
		//Get the Mutex specific info
		(*localInfoBuf)().iHoldCount = aMutex->iHoldCount;
		(*localInfoBuf)().iWaitCount = aMutex->iWaitCount;
		(*localInfoBuf)().iResetting = aMutex->iResetting;
		(*localInfoBuf)().iOrder = aMutex->iOrder;
		TInt ii=0;
	    SDblQueLink* pLink=aMutex->iSuspendedQ.First();
		while ((pLink!=&aMutex->iSuspendedQ.iA) && (ii<KQueBufferLength))
			 {
			 DThread* pT=_LOFF(pLink,DThread,iWaitLink);
			 (*localInfoBuf)().iSuspendedQ.iBuffer[ii]=(TUint8*)pT;
			 pLink=pLink->iNext;
			 ii++;
			 }
		(*localInfoBuf)().iSuspendedQ.iStartFrom=0;
		(*localInfoBuf)().iSuspendedQ.iCount=ii;
		ii=0;
	    pLink=aMutex->iPendingQ.First();
		while ((pLink!=&aMutex->iPendingQ.iA) && (ii<KQueBufferLength))
			 {
			 DThread* pT=_LOFF(pLink,DThread,iWaitLink);
			 (*localInfoBuf)().iPendingQ.iBuffer[ii]=(TUint8*)pT;
			 pLink=pLink->iNext;
			 ii++;
			 }
		(*localInfoBuf)().iPendingQ.iStartFrom=0;
		(*localInfoBuf)().iPendingQ.iCount=ii;

		//Copy the local info buffer into the client's address space
	    err = Kern::ThreadDesWrite(iClient, aMutexInfoBuf, *localInfoBuf, 0, KTruncateToMaxLength, NULL);	
		delete localInfoBuf;
		}
	return err;    		
	}

TInt DMemoryAccess::GetTimerInfo(DTimer* aTimer, TDes8* aTimerInfoBuf)
	{
	// Kern::Printf("[DMemoryAccess] ::GetTimerInfo");
	TInt err=KErrNone;
	//construct local info buffer to populate
	TTimerKernelInfoBuf* localInfoBuf = new TTimerKernelInfoBuf;	
	if (!localInfoBuf)
		{
		err=KErrNoMemory;
		}
	else
		{
		//Get the standard DObject info
		GetObjectInfo (aTimer, (*localInfoBuf)());
		//Get the timer specific info
		(*localInfoBuf)().iState = aTimer->iTimer.iState;
		(*localInfoBuf)().iType = aTimer->iTimer.iType;
#if FSHELL_PLATFORM_SYMTB >= 92 || FSHELL_PLATFORM_FOUNDATION >= 3
		(*localInfoBuf)().iClientStatus = NULL; //TOMSCI TEMP
#else
		(*localInfoBuf)().iClientStatus = (TLinAddr)aTimer->iTimer.iStatus;
#endif

		//Copy the local info buffer into the client's address space
	    err = Kern::ThreadDesWrite(iClient, aTimerInfoBuf, *localInfoBuf, 0, KTruncateToMaxLength, NULL);	
		delete localInfoBuf;
		}
	return err;    		
	}

TInt DMemoryAccess::GetServerInfo(DServer* aServer, TDes8* aServerInfoBuf)
	{
	// Kern::Printf("[DMemoryAccess] ::GetServerInfo");
	TInt err=KErrNone;
	//construct local info buffer to populate
	TServerKernelInfoBuf* localInfoBuf = new TServerKernelInfoBuf;
	if (!localInfoBuf)
		{
		err=KErrNoMemory;
		}
	else
		{
		//Get the standard DObject info
		GetObjectInfo (aServer, (*localInfoBuf)());
		//Get the server specific info
		(*localInfoBuf)().iAddressOfOwningThread = (TUint8*)aServer->iOwningThread;
		(*localInfoBuf)().iOwningThreadId = aServer->iOwningThread->iId;
		(*localInfoBuf)().iSessionType = aServer->iSessionType;
		TInt ii=0;
	    SDblQueLink* pLink=aServer->iSessionQ.First();
		while ((pLink!=&aServer->iSessionQ.iA) && (ii<KQueBufferLength))
			 {
			 DSession* pT=_LOFF(pLink,DSession,iServerLink);
			 (*localInfoBuf)().iSessionQ.iBuffer[ii]=(TUint8*)pT;
			 pLink=pLink->iNext;
			 ii++;
			 }
		(*localInfoBuf)().iSessionQ.iStartFrom=0;
		(*localInfoBuf)().iSessionQ.iCount=ii;

		//Copy the local info buffer into the client's address space
	    err = Kern::ThreadDesWrite(iClient, aServerInfoBuf, *localInfoBuf, 0, KTruncateToMaxLength, NULL);	
		delete localInfoBuf;
		}
	return err;    		
	}

TInt DMemoryAccess::GetSessionInfo(DSession* aSession, TDes8* aSessionInfoBuf)
	{
	// Kern::Printf("[DMemoryAccess] ::GetSessionInfo");
	TInt err=KErrNone;
	//construct local info buffer to populate
	TSessionKernelInfoBuf* localInfoBuf = new TSessionKernelInfoBuf;
	if (!localInfoBuf)
		{
		err=KErrNoMemory;
		}
	else
		{
		//Get the standard DObject info
		GetObjectInfo (aSession, (*localInfoBuf)());
		//Get the session specific info
		(*localInfoBuf)().iServer = (TUint8*)aSession->iServer;
#if defined(FSHELL_PLATFORM_FOUNDATION) || defined(FSHELL_PLATFORM_SYMTB)
		(*localInfoBuf)().iSessionPtr = (TAny*)aSession->iSessionCookie;
#else
		(*localInfoBuf)().iSessionPtr = (TAny*)aSession->iSessionPtr;
#endif
		(*localInfoBuf)().iTotalAccessCount = aSession->iTotalAccessCount;
		(*localInfoBuf)().iSessionType = aSession->iSessionType;
		(*localInfoBuf)().iSvrSessionType = aSession->iSvrSessionType;
		(*localInfoBuf)().iMsgCount = aSession->iMsgCount;
		(*localInfoBuf)().iMsgLimit = aSession->iMsgLimit;

		//Copy the local info buffer into the client's address space
	    err = Kern::ThreadDesWrite(iClient, aSessionInfoBuf, *localInfoBuf, 0, KTruncateToMaxLength, NULL);	
		delete localInfoBuf;
		}
	return err;    		
	}

TInt DMemoryAccess::GetLogicalDeviceInfo(DLogicalDevice* aLogicalDevice, TDes8* aLogicalDeviceInfoBuf)
	{
	// Kern::Printf("[DMemoryAccess] ::GetLogicalDeviceInfo");
	TInt err=KErrNone;
	//construct local info buffer to populate
	TLogicalDeviceKernelInfoBuf* localInfoBuf = new TLogicalDeviceKernelInfoBuf;
	if (!localInfoBuf)
		{
		err=KErrNoMemory;
		}
	else
		{	
		//Get the standard DObject info
		GetObjectInfo (aLogicalDevice, (*localInfoBuf)());
		//Get the logical device specific info
		(*localInfoBuf)().iVersion = aLogicalDevice->iVersion;
		(*localInfoBuf)().iParseMask = aLogicalDevice->iParseMask;
		(*localInfoBuf)().iUnitsMask = aLogicalDevice->iUnitsMask;
		(*localInfoBuf)().iOpenChannels = aLogicalDevice->iOpenChannels;

		//Copy the local info buffer into the client's address space
	    err = Kern::ThreadDesWrite(iClient, aLogicalDeviceInfoBuf, *localInfoBuf, 0, KTruncateToMaxLength, NULL);	
		delete localInfoBuf;
		}
	return err;    		
	}

TInt DMemoryAccess::GetPhysicalDeviceInfo(DPhysicalDevice* aPhysicalDevice, TDes8* aPhysicalDeviceInfoBuf)
	{
	// Kern::Printf("[DMemoryAccess] ::GetPhysicalDeviceInfo");
	TInt err=KErrNone;
	//construct local info buffer to populate
	TPhysicalDeviceKernelInfoBuf* localInfoBuf = new TPhysicalDeviceKernelInfoBuf;
	if (!localInfoBuf)
		{
		err=KErrNoMemory;
		}
	else
		{
		//Get the standard DObject info
		GetObjectInfo (aPhysicalDevice, (*localInfoBuf)());
		//Get the physical device specific info
		(*localInfoBuf)().iVersion = aPhysicalDevice->iVersion;
		(*localInfoBuf)().iUnitsMask = aPhysicalDevice->iUnitsMask;
		(*localInfoBuf)().iAddressOfCodeSeg = (TUint8*)aPhysicalDevice->iCodeSeg;

		//Copy the local info buffer into the client's address space
	    err = Kern::ThreadDesWrite(iClient, aPhysicalDeviceInfoBuf, *localInfoBuf, 0, KTruncateToMaxLength, NULL);	
		delete localInfoBuf;
		}
	return err;    		
	}

TInt DMemoryAccess::GetLogicalChannelInfo(DLogicalChannel* aLogicalChannel, TDes8* aLogicalChannelInfoBuf)
	{
	// Kern::Printf("[DMemoryAccess] ::GetLogicalChannelInfo");
	TInt err=KErrNone;
	//construct local info buffer to populate
	TLogicalChannelKernelInfoBuf* localInfoBuf = new TLogicalChannelKernelInfoBuf;
	if (!localInfoBuf)
		{
		err=KErrNoMemory;
		}
	else
		{	
		//Get the standard DObject info
		GetObjectInfo (aLogicalChannel, (*localInfoBuf)());
		//Get the logical channel info
		
		//Copy the local info buffer into the client's address space
	    err = Kern::ThreadDesWrite(iClient, aLogicalChannelInfoBuf, *localInfoBuf, 0, KTruncateToMaxLength, NULL);	
		delete localInfoBuf;
		}
	return err;    		
	}

TInt DMemoryAccess::GetChangeNotifierInfo(DChangeNotifier* aChangeNotifier, TDes8* aChangeNotifierInfoBuf)
	{
	// Kern::Printf("[DMemoryAccess] ::GetChangeNotifierInfo");
	TInt err=KErrNone;
	//construct local info buffer to populate
	TChangeNotifierKernelInfoBuf* localInfoBuf = new TChangeNotifierKernelInfoBuf;
	if (!localInfoBuf)
		{
		err=KErrNoMemory;
		}
	else
		{
		//Get the standard DObject info
		GetObjectInfo (aChangeNotifier, (*localInfoBuf)());
		//Get the change notifier specific info
		(*localInfoBuf)().iChanges = aChangeNotifier->iChanges;
		(*localInfoBuf)().iAddressOfThread = (TUint8*)aChangeNotifier->iThread;
		
		//Copy the local info buffer into the client's address space
	    err = Kern::ThreadDesWrite(iClient, aChangeNotifierInfoBuf, *localInfoBuf, 0, KTruncateToMaxLength, NULL);	
		delete localInfoBuf;
		}
	return err;    		
	}

TInt DMemoryAccess::GetMsgQueueInfo(DMsgQueue* aMsgQueue, TDes8* aMsgQueueInfoBuf)
	{
	// Kern::Printf("[DMemoryAccess] ::GetUndertakerInfo");
	TInt err=KErrNone;
	//construct local info buffer to populate
	TMsgQueueKernelInfoBuf* localInfoBuf = new TMsgQueueKernelInfoBuf;	
	if (!localInfoBuf)
		{
		err=KErrNoMemory;
		}
	else
		{
		//Get the standard DObject info
		GetObjectInfo((DObject*)aMsgQueue, (*localInfoBuf)());
		//Get the specific info - highly fragile this!
		const TInt KSizeOffset = sizeof(DObject)+(8*4);
		(*localInfoBuf)().iMaxMsgLength = *(TUint16*)((TLinAddr)aMsgQueue + KSizeOffset);
		
		//Copy the local info buffer into the client's address space
	    err = Kern::ThreadDesWrite(iClient, aMsgQueueInfoBuf, *localInfoBuf, 0, KTruncateToMaxLength, NULL);	
		delete localInfoBuf;
		}
	return err;    		
	}

TInt DMemoryAccess::GetUndertakerInfo(DUndertaker* aUndertaker, TDes8* aUndertakerInfoBuf)
	{
	// Kern::Printf("[DMemoryAccess] ::GetUndertakerInfo");
	TInt err=KErrNone;
	//construct local info buffer to populate
	TUndertakerKernelInfoBuf* localInfoBuf = new TUndertakerKernelInfoBuf;	
	if (!localInfoBuf)
		{
		err=KErrNoMemory;
		}
	else
		{
		//Get the standard DObject info
		GetObjectInfo (aUndertaker, (*localInfoBuf)());
		//Get the thread specific info
		(*localInfoBuf)().iOwningThread = (TUint8*)aUndertaker->iOwningThread;
		
		//Copy the local info buffer into the client's address space
	    err = Kern::ThreadDesWrite(iClient, aUndertakerInfoBuf, *localInfoBuf, 0, KTruncateToMaxLength, NULL);	
		delete localInfoBuf;
		}
	return err;    		
	}

TInt DMemoryAccess::GetCondVarInfo(DCondVar* aCondVar, TDes8* aCondVarInfoBuf)
	{
	// Kern::Printf("[DMemoryAccess] ::GetCondVarInfo");
	TInt err=KErrNone;
	//construct local info buffer to populate
	TCondVarKernelInfoBuf* localInfoBuf = new TCondVarKernelInfoBuf;
	if (!localInfoBuf)
		{
		err=KErrNoMemory;
		}
	else
		{	
		//Get the standard DObject info
		GetObjectInfo (aCondVar, (*localInfoBuf)());
		//Get the cond var specific info
		(*localInfoBuf)().iResetting = aCondVar->iResetting;
		(*localInfoBuf)().iAddressOfMutex = (TUint8*)aCondVar->iMutex;
		(*localInfoBuf)().iWaitCount = aCondVar->iWaitCount;
		TInt ii=0;
		SDblQueLink* pLink=aCondVar->iSuspendedQ.First();
		while ((pLink!=&aCondVar->iSuspendedQ.iA) && (ii<KQueBufferLength))
			 {
			 DThread* pT=_LOFF(pLink,DThread,iWaitLink);
			 (*localInfoBuf)().iSuspendedQ.iBuffer[ii]=(TUint8*)pT;
			 pLink=pLink->iNext;
			 ii++;
			 }
		(*localInfoBuf)().iSuspendedQ.iStartFrom=0;
		(*localInfoBuf)().iSuspendedQ.iCount=ii;

		//Copy the local info buffer into the client's address space
		err = Kern::ThreadDesWrite(iClient, aCondVarInfoBuf, *localInfoBuf, 0, KTruncateToMaxLength, NULL);	
		delete localInfoBuf;
		}
	return err;    		
	}

TInt DMemoryAccess::FindPtrInCodeSegments(TAny* aDllNamePtr, TAny* aPtr)
	{
	/*
	// Iterate the code segments
	TInt err = AcquireCodeSegMutex();
	if (err) return err;
	DCodeSeg* currentCodeSeg = NULL;

	//BEGIN copied from GetNextCodeSegInfo
	SDblQue* p = Kern::CodeSegList();
	SDblQueLink* anchor=&p->iA;

	currentCodeSeg = _LOFF(anchor->iNext, DCodeSeg, iLink);

	while (currentCodeSeg)
		{
		if (currentCodeSeg->iLink.iNext != anchor)
			{
			currentCodeSeg = _LOFF(currentCodeSeg->iLink.iNext, DCodeSeg, iLink);
			if ((TUint)aPtr >= currentCodeSeg->iRunAddress && (TUint)aPtr < currentCodeSeg->iRunAddress + currentCodeSeg->iSize)
				{
				// Found it
				break;
				}
			}
		else
			{
			// Not found any code seg matching
			currentCodeSeg = NULL;
			err = KErrNotFound;
			}
		}
	//END copied from GetNextCodeSegInfo
	ReleaseCodeSegMutex();
	// End iterate
	*/

	Kern::MutexWait(*Kern::CodeSegLock());
	DCodeSeg* currentCodeSeg = Kern::CodeSegFromAddress((TLinAddr)aPtr, iClient->iOwningProcess);
	TInt err = KErrNone;
	TInt offset = 0;
	TBuf8<256> dllName;
	if (currentCodeSeg)
		{
		dllName.Copy(*currentCodeSeg->iFileName);
		offset = (TUint)aPtr - currentCodeSeg->iRunAddress; // This won't work on flexible memory model!
		}
	else
		{
		err = KErrNotFound;
		}
	Kern::MutexSignal(*Kern::CodeSegLock());

	if (!err)
		{
		err = Kern::ThreadDesWrite(iClient, aDllNamePtr, dllName, 0, KTruncateToMaxLength, NULL);
		}
	return err ? err : offset;
	}

TInt DMemoryAccess::GetHandleOwners(TAny* aObj, TAny* aOwnersBuf)
	{
#ifdef FSHELL_DOBJECTIX_SUPPORT
	TBuf8<256> buf;
	
	// Code adapted from ExecHandler::HandleInfo
	DObject* pO=(DObject*)aObj;
	TInt r = KErrNone;
	/*
	DThread& t = *iClient;
	//TInt r=K::OpenObjectFromHandle(aHandle,pO);
	//BEGIN this bit copied from K::OpenObjectFromHandle
	TInt r=KErrBadHandle;
	NKern::ThreadEnterCS();
	NKern::LockSystem();
	pO=t.ObjectFromHandle(aHandle);
	if (pO)
		r=pO->Open();
	NKern::UnlockSystem();
	if (r!=KErrNone)
		{
		pO=NULL;
		NKern::ThreadLeaveCS();
		}
	//END
	*/

	if (r==KErrNone)
		{
		//DObjectIx::Wait(); //TOMSCI I can't call this frmo a device driver, why did the code I copied do it but none of the DMemoryAccess stuff that uses containers do it??
		//DProcess* pCurrentProcess=TheCurrentThread->iOwningProcess;
		//hinfo.iNumOpenInThread=TheCurrentThread->iHandles->Count(pO);
		//hinfo.iNumOpenInProcess=pCurrentProcess->iHandles->Count(pO);

		DObjectCon* const * cons=Kern::Containers();
		DObjectCon& threads = *cons[EThread];
		threads.Wait();
		TInt c=threads.Count();
		TInt i=0;
		for (;i<c;i++)
			{
			DThread *pT=(DThread *)threads[i];
			DObjectIx* handles = pT->iHandles;
			if (handles) // maybe a nearly dead one hanging around
				{
				TInt rr=((DObjectIxNinePointTwoHack*)handles)->At(pO);
				if (rr!=KErrNotFound)
					{
					TPckgBuf<TUint> idBuf(pT->iId);
					if (buf.Length() + idBuf.Length() >= buf.MaxLength())
						{
						r = KErrOverflow;
						break;
						}
					else
						{
						buf.Append(idBuf);
						}
					//++hinfo.iNumThreads;
					//if (pT->iOwningProcess==pCurrentProcess)
					//	++hinfo.iNumOpenInProcess;
					}
				}
			}
		threads.Signal();

		DObjectCon& processes = *cons[EProcess];
		processes.Wait();
		c=processes.Count();
		for (i=0;i<c;i++)
			{
			DProcess *pP=(DProcess *)processes[i];
			DObjectIx* handles = pP->iHandles;
			if (handles) // maybe a nearly dead one hanging around
				{
				TInt rr=((DObjectIxNinePointTwoHack*)handles)->At(pO);
				if (rr!=KErrNotFound)
					{
					//++hinfo.iNumProcesses;
					TPckgBuf<TUint> idBuf(pP->iId);
					if (buf.Length() + idBuf.Length() >= buf.MaxLength())
						{
						r = KErrOverflow;
						break;
						}
					else
						{
						buf.Append(idBuf);
						}
					}
				}
			}
		processes.Signal();
		//DObjectIx::Signal();


		//DObjectIx::Signal();
		//pO->Close(NULL);
		//NKern::ThreadLeaveCS();
		}
	
	TInt clientLen = Kern::ThreadGetDesMaxLength(iClient, aOwnersBuf);
	if (clientLen < 0) return clientLen;
	TInt writeErr = Kern::ThreadDesWrite(iClient, aOwnersBuf, buf, 0, KTruncateToMaxLength, NULL);
	if (writeErr) return writeErr;
	return (clientLen < buf.Length()) ? KErrOverflow : r;
#else
	return KErrNotSupported;
#endif
	}

TInt DMemoryAccess::GetThreadHandles(TInt aThreadId, TAny* aHandlesBuf)
	{
#ifdef FSHELL_DOBJECTIX_SUPPORT
	TInt maxLength = Kern::ThreadGetDesMaxLength(iClient, aHandlesBuf);
	TInt err = KErrNone;

	DObjectCon* const * cons = Kern::Containers();
	DObjectCon& container = *cons[EThread];
	container.Wait();
	NKern::ThreadEnterCS();
	DThread* thread = Kern::ThreadFromId(aThreadId);
	//TOMSCI FIXME we don't increment thread's ref count
	NKern::ThreadLeaveCS();
	container.Signal();
	if (thread == NULL) 
		{
		return KErrNotFound;
		}

	// Note, this code is inherently dodgy because it doesn't claim DObjectIx::HandleMutex.
	DObjectIxNinePointTwoHack* handles = (DObjectIxNinePointTwoHack*)thread->iHandles;
	if (handles)
		{
		TInt offset = 0;

		for (TInt i = 0; i < handles->Count(); ++i)
			{
			if (offset >= maxLength)
				{
				err = KErrOverflow;
				break;
				}
			NKern::LockSystem();
			DObject* obj = (*handles)[i];
			NKern::UnlockSystem();
			if (obj != NULL)
				{
				err = Kern::ThreadDesWrite(iClient, aHandlesBuf, TPtrC8((TUint8*)&obj, sizeof(TAny*)), offset, KChunkShiftBy0, iClient);
				if (err)
					{
					break;
					}
				else
					{
					offset += sizeof(TAny*);
					}
				}
			}
		}

	//TOMSCI What is this unlock doing here? TODO FIXME!!!
	NKern::UnlockSystem();
	return err;
#else
	return KErrNotSupported;
#endif
	}

TInt DMemoryAccess::GetProcessHandles(TInt aProcessId, TAny* aHandlesBuf)
	{
#ifdef FSHELL_DOBJECTIX_SUPPORT
	TInt maxLength = Kern::ThreadGetDesMaxLength(iClient, aHandlesBuf);
	TInt err = KErrNone;

	DObjectCon* const * cons = Kern::Containers();
	DObjectCon& container = *cons[EProcess];
	container.Wait();
	NKern::ThreadEnterCS();
	DProcess* process = Kern::ProcessFromId(aProcessId);
	NKern::ThreadLeaveCS();
	container.Signal();
	if (process == NULL) 
		{
		return KErrNotFound;
		}

	// Note, this code is inherently dodgy because it doesn't claim DObjectIx::HandleMutex.
	DObjectIxNinePointTwoHack* handles = (DObjectIxNinePointTwoHack*)process->iHandles;
	if (handles)
		{
		TInt offset = 0;

		for (TInt i = 0; i < handles->Count(); ++i)
			{
			if (offset >= maxLength)
				{
				err = KErrOverflow;
				break;
				}
			NKern::LockSystem();
			DObject* obj = (*handles)[i];
			NKern::UnlockSystem();
			if (obj != NULL)
				{
				err = Kern::ThreadDesWrite(iClient, aHandlesBuf, TPtrC8((TUint8*)&obj, sizeof(TAny*)), offset, KChunkShiftBy0, iClient);
				if (err)
					{
					break;
					}
				else
					{
					offset += sizeof(TAny*);
					}
				}
			}
		}

	return err;
#else
	return KErrNotSupported;
#endif
	}

TInt DMemoryAccess::SetCriticalFlags(TInt aThreadHandle, TUint aFlags)
	{
	NKern::LockSystem();
	DThread* thread = (DThread*)Kern::ObjectFromHandle(iClient, aThreadHandle, EThread);
	if (!thread) 
		{
		NKern::UnlockSystem();
		return KErrNotFound;
		}

	const TUint32 clear =	KThreadFlagSystemPermanent | KThreadFlagSystemCritical |
							KThreadFlagProcessPermanent | KThreadFlagProcessCritical;

	TUint newFlags = (thread->iFlags&~clear) | aFlags;
	thread->iFlags = newFlags;

	NKern::UnlockSystem();
	return KErrNone;
	}

// Enter and exit with system lock held
TInt SetPri(DThread* aThread, TInt aPriority)
	{
	//Kern::SetThreadPriority(aPriority, aThread);

	// Bypass limitation of Kern::SetThreadPriority on specifying relative priorities
	if (aPriority<EThrdPriorityIdle || aPriority==-1 || aPriority==0 || aPriority>=KNumPriorities)
		return KErrArgument;
	aThread->SetThreadPriority(aPriority);
	return KErrNone;
	}

TInt DMemoryAccess::SetThreadPriority(TInt aThreadHandle, TInt aPriority)
	{
	NKern::LockSystem();
	DThread* thread = (DThread*)Kern::ObjectFromHandle(iClient, aThreadHandle, EThread);
	if (!thread) 
		{
		NKern::UnlockSystem();
		return KErrNotFound;
		}

	TInt err = SetPri(thread, aPriority);
	NKern::UnlockSystem();
	return err;
	}

void DMemoryAccess::NotifyThreadCreation(TRequestStatus* aStatus)
	{
	// TODO: Have this migrate to the shared event handler
	TInt err = KErrNone;
	TBool supported = DKernelEventHandler::DebugSupportEnabled();
	if (!supported) err = KErrNotSupported;
	if (!err && !iEventHandler)
		{
		iEventHandler = new DThreadChangeHandler(iClient);
		if (iEventHandler)
			{
			iEventHandler->Add(DKernelEventHandler::EAppend);
			}
		else
			{
			err = KErrNoMemory;
			}
		}

	if (err)
		{
		Kern::RequestComplete(iClient, aStatus, err);
		}
	else
		{
		iEventHandler->SetStatus(aStatus);
		}
	}

void DMemoryAccess::CancelNotifyThreadCreation()
	{
	if (iEventHandler)
		{
		iEventHandler->SetStatus(NULL);
		}
	}

TInt DMemoryAccess::SetPriorityOverride(TInt aPriority, TAny* aMatchString)
	{
	// TODO: Have this migrate to the shared event handler
	// Or... just remove it. It's really odd code that could probably achieve its end goals in a much more sensible way.
	TInt err = KErrNone;
	if (!iEventHandler)
		{
		iEventHandler = new DThreadChangeHandler(iClient);
		if (iEventHandler)
			{
			iEventHandler->Add(DKernelEventHandler::EAppend);
			}
		else
			{
			err = KErrNoMemory;
			}
		}
	
	// TODO: this isn't thread safe, as iEventHandler is already added... meh.
	
	if (!err)
		{
		iEventHandler->iPriorityToSet = aPriority;
		err = Kern::ThreadDesRead(iClient, aMatchString, iEventHandler->iMatch,	0);
		}
	return err;
	}

TInt DMemoryAccess::OpenThread(TUint aThreadId)
	{
	NKern::ThreadEnterCS();
	DObjectCon* threads = Kern::Containers()[EThread];
	threads->Wait();
	DThread* thread = Kern::ThreadFromId(aThreadId);
	if (thread && thread->Open() != KErrNone)
		{
		// Failed to open
		thread = NULL;
		}
	threads->Signal();

	if (thread)
		{
		TInt res = Kern::MakeHandleAndOpen(iClient, thread);
		thread->Close(NULL);
		NKern::ThreadLeaveCS();
		return res;
		}
	else
		{
		NKern::ThreadLeaveCS();
		return KErrNotFound;
		}
	}

DThreadChangeHandler::DThreadChangeHandler(DThread* aClientThread)
	: DKernelEventHandler(&Event, this), iThread(aClientThread)
	{
	}

DThreadChangeHandler::~DThreadChangeHandler()
	{
	}

/*static*/ TUint DThreadChangeHandler::Event(TKernelEvent aEvent, TAny* a1, TAny* a2, TAny* aPrivateData)
	{
	DThreadChangeHandler* self = static_cast<DThreadChangeHandler*>(aPrivateData);
	return self->DoEvent(aEvent, a1, a2);
	}

TUint DThreadChangeHandler::DoEvent(TKernelEvent aEvent, TAny* a1, TAny* /*a2*/)
	{
	if (aEvent == EEventAddThread)
		{
		TRequestStatus* nullstat = NULL;
		TRequestStatus* stat = (TRequestStatus*)NKern::SafeSwap(nullstat, (TAny*&)iStatus);
		DThread* thread = static_cast<DThread*>(a1);
		TBool notify = ETrue;

		if (iPriorityToSet && iMatch.Length())
			{
			TBuf8<256> name;
			thread->AppendFullName(name);
			if (name.MatchF(iMatch) >= 0)
				{
				//Kern::Printf("Matched %O", thread);
				NKern::LockSystem();
				SetPri(thread, iPriorityToSet);
				NKern::UnlockSystem();
				}
			else
				{
				// Not a thread we're interested in
				notify = EFalse;
				}
			}
		if (stat && notify)
			{
			TInt res = Kern::MakeHandleAndOpen(iThread, thread);
			Kern::RequestComplete(iThread, stat, res);
			}
		}
	return ERunNext;
	}

void DThreadChangeHandler::SetStatus(TRequestStatus* aStatus)
	{
	TRequestStatus* oldStat = (TRequestStatus*)NKern::SafeSwap(aStatus, (TAny*&)iStatus);
	if (oldStat)
		{
		Kern::RequestComplete(iThread, oldStat, KErrCancel);
		}
	}

TInt DMemoryAccess::InPlaceObjectRename(TAny* aParamsBuf, TAny* aNewNamePtr)
	{
	TObjectInfoByPtrParams params;
	TPckg<TObjectInfoByPtrParams> paramsPkg(params);
	TInt err = Kern::ThreadDesRead(iClient, aParamsBuf, paramsPkg, 0);
	if (err) return err;

	TKName newName;
	err = Kern::ThreadDesRead(iClient, aNewNamePtr, newName, 0);
	if (err) return err;

	TInt posInContainer = KErrNotFound;
	DObjectCon* const * cons = Kern::Containers();
	DObjectCon& container = *cons[params.iObjectType];
	container.Wait();
	TBool alreadyHeld = iLocks[params.iObjectType];
	iLocks[params.iObjectType] = ETrue;
	
	err = GetObjectPositionInContainer(params.iObjectPtr, container, posInContainer);

	if (err == KErrNone)
		{
		DObject* obj = container[posInContainer];
		obj->iName->Copy(newName);
		}
	container.Signal();
	iLocks[params.iObjectType] = alreadyHeld;
	return err;
	}

TInt DMemoryAccess::InPlaceSetProcessFileName(TAny* a1, TAny* aNewNamePtr)
	{
	NKern::LockSystem();
	DProcess* process = (DProcess*)Kern::ObjectFromHandle(iClient, (TUint)a1, EProcess);
	NKern::UnlockSystem();
	if (!process) 
		{
		return KErrNotFound;
		}
	TBuf<256> filename;
	TInt err = Kern::ThreadDesRead(iClient, aNewNamePtr, filename, 0);
	if (err) return err;
	//thread->iName->Copy(buf);
#ifdef __WINS__
	// Wins does 'optimisations' on the file name meaning an in-place copy will never work
	HBuf* newName = HBuf8::New(filename);
	if (!newName) return KErrNoMemory;
	HBuf* oldName = (HBuf*)NKern::SafeSwap(newName, (TAny*&)process->iCodeSeg->iFileName);
	delete oldName;
#else
	process->iCodeSeg->iFileName->Copy(filename);
#endif
	return KErrNone;
	}

class RHackAllocator : public RAllocator
	{
public:
	using RAllocator::iFlags;
	};

TInt DMemoryAccess::EnableHeapTracing(TUint aThreadId, TBool aEnable)
	{
#ifndef FSHELL_BTRACE_SUPPORT
	// If the kernel doesn't have btrace, the heap won't have instrumentation
	(void)aThreadId;
	(void)aEnable;
	return KErrNotSupported;
#else

	DObjectCon* threads = Kern::Containers()[EThread];
	NKern::ThreadEnterCS();
	threads->Wait();
	DThread* thread = Kern::ThreadFromId(aThreadId);
	if (thread) thread->Open();
    threads->Signal();
    NKern::ThreadLeaveCS();

	if (!thread) return KErrNotFound;
	TLinAddr allocatorAddr = DoGetAllocatorAddress(thread, EFalse);
	if (!allocatorAddr) 
		{
		thread->Close(NULL);
		return KErrNotFound;
		}

	TLinAddr flagsAddr = allocatorAddr + _FOFF(RHackAllocator, iFlags);
	TUint flags = 0;
	TInt err = Kern::ThreadRawRead(thread, (void*)flagsAddr, &flags, sizeof(TUint));
	if (!err)
		{
		if (aEnable) flags |= RAllocator::ETraceAllocs;
		else flags &= ~(RAllocator::ETraceAllocs);
		err = Kern::ThreadRawWrite(thread, (void*)flagsAddr, &flags, sizeof(TUint));
		}
	NKern::ThreadEnterCS();
	thread->Close(NULL);
    NKern::ThreadLeaveCS();
	return err;
#endif
	}

#if defined(FSHELL_RAMDEFRAG_SUPPORT) && !defined(__WINS__) // emulator platform.h doesn't have the ram defrag APIs

TInt DMemoryAccess::DefragRam(TInt aPriority)
	{
	TInt priority = aPriority;
	if (priority == -1) priority = TRamDefragRequest::KInheritPriority; // I know KInheritPriority is actually -1 anyway but better to be correctly typed
	TRamDefragRequest request;
	return request.DefragRam(priority);
	}

TInt DMemoryAccess::EmptyRamZone(TInt aPriority, TUint aZone)
	{
	TInt priority = aPriority;
	if (priority == -1) priority = TRamDefragRequest::KInheritPriority; // I know KInheritPriority is actually -1 anyway but better to be correctly typed
	TRamDefragRequest request;
	return request.EmptyRamZone(aZone, priority);
	}

TInt DMemoryAccess::GetRamZoneInfo(TUint aZone, TAny* aInfoPkg)
	{
	SRamZonePageCount info;
	TInt err = Epoc::GetRamZonePageCount(aZone, info);
	if (err) return err;

	TRamZoneInfo clientInf;
	clientInf.iFreePages = info.iFreePages;
	clientInf.iUnknownPages = info.iUnknownPages;
	clientInf.iFixedPages = info.iFixedPages;
	clientInf.iMovablePages = info.iMovablePages;
	clientInf.iDiscardablePages = info.iDiscardablePages;
	TPckg<TRamZoneInfo> clientPkg(clientInf);
	err = Kern::ThreadDesWrite(iClient, aInfoPkg, clientPkg, 0, KTruncateToMaxLength, NULL);
	return err;
	}

#else

TInt DMemoryAccess::DefragRam(TInt /*aPriority*/)
	{
	return KErrNotSupported;
	}

TInt DMemoryAccess::EmptyRamZone(TInt /*aPriority*/, TUint /*aZone*/)
	{
	return KErrNotSupported;
	}

TInt DMemoryAccess::GetRamZoneInfo(TUint /*aZone*/, TAny* /*aInfoPkg*/)
	{
	return KErrNotSupported;
	}

#endif

TCapabilitySet StoT(const SCapabilitySet& aCaps)
	{
	TCapabilitySet result;
	result.SetEmpty();
	memcpy(&result, &aCaps, sizeof(SCapabilitySet));
	return result;
	}
	
SCapabilitySet TtoS(const TCapabilitySet& aCaps)
	{
	SCapabilitySet result;
	memcpy(&result, &aCaps, sizeof(SCapabilitySet));
	return result;
	}

// Copied from DProcess::ConvertPriority
TInt DProcessConvertPriority(TProcessPriority aPriority)
	{
	TInt p=-1;
	switch(aPriority)
		{
		case EPriorityLow: p=EProcPriorityLow; break;
		case EPriorityBackground: p=EProcPriorityBackground; break;
		case EPriorityForeground: p=EProcPriorityForeground; break;
		case EPriorityHigh: p=EProcPriorityHigh; break;
		case EPriorityWindowServer: p=EProcPrioritySystemServer1; break;
		case EPriorityFileServer: p=EProcPrioritySystemServer2; break;
		case EPriorityRealTimeServer: p=EProcPriorityRealTimeServer; break;
		case EPrioritySupervisor: p=EProcPrioritySystemServer3; break;
		}
	return p;
	}

TInt DMemoryAccess::SetProcessProperties(TInt aProcessHandle, TAny* aProperties)
	{
	TProcessProperties properties;
	TPckg<TProcessProperties> pkg(properties);
	TInt err = Kern::ThreadDesRead(iClient, aProperties, pkg, 0);
	if (err) return err;

	NKern::LockSystem();
	DProcess* proc = (DProcess*)Kern::ObjectFromHandle(iClient, aProcessHandle, EProcess);
	if (!proc) return KErrBadHandle;
	proc->Open();
	NKern::UnlockSystem();

	if (properties.iSid != 0xFFFFFFFFu) proc->iS.iSecureId = properties.iSid;
	if (properties.iVid != 0xFFFFFFFFu) proc->iS.iVendorId = properties.iVid;

	// Why is there no way of converting from an SCapabilitySet to a TCapabilitySet?
	TCapabilitySet caps = StoT(proc->iS.iCaps);
	caps.Union(properties.iCapsToAdd);
	caps.Remove(properties.iCapsToRemove);
	SCapabilitySet newSet = TtoS(caps);
	memcpy(&proc->iS.iCaps, &newSet, sizeof(SCapabilitySet));

	if (properties.iProcessPriority)
		{
		TInt priority = DProcessConvertPriority((TProcessPriority)properties.iProcessPriority);
		if (priority < 0) err = KErrArgument;
		else proc->iPriority = priority; // Note this won't recalculate thread priorities
		}

	proc->Close(NULL);
	return err;
	}

#ifndef FSHELL_COPYTOSHADOWMEMORY_SUPPORT
// Not sure exactly when it was introduced. Somewhere between 9.1 and 9.3 or anywhere that used ARM7
// If CopyToShadowMemory is available it MUST be used, on ARM7 or later, otherwise you'll get an exception
#define NO_COPYTOSHADOWMEMORY
#endif

TInt DMemoryAccess::WriteShadowMemory(TLinAddr aAddress, TAny* aNewContents)
	{
#ifdef __WINS__
	(void)aAddress;
	(void)aNewContents;
	return KErrNotSupported; // Shadowing not supported on emulator
#else
	TBuf8<32> newMem;
	TInt len = Kern::ThreadGetDesLength(iClient, aNewContents);
	if (len < 0) return len;
	if (len > 32) return KErrTooBig; // API for CopyToShadowMemory says max 32 bytes, who are we to argue with hard-coded magic numbers (that aren't actually enforced afaics)

	TInt err = Kern::ThreadDesRead(iClient, aNewContents, newMem, 0);
	if (err) return err;

#ifdef NO_COPYTOSHADOWMEMORY
	// Can't check if the page is already shadowed, just error and go straight to trying to call AllocShadowPage
	err = KErrArgument;
#else
	err = Epoc::CopyToShadowMemory(aAddress, (TLinAddr)newMem.Ptr(), newMem.Size());
#endif
	if (err == KErrArgument || err == KErrNotFound)
		{
		// The documentation *says* it will always be KErrArgument but Flexible memory model actually returns KErrNotFound
		// Page (or pages) hasn't been shadowed yet
		TLinAddr pageForStart = aAddress & ~(Kern::RoundToPageSize(1)-1);
		TLinAddr pageForEnd = (aAddress + newMem.Size()) & ~(Kern::RoundToPageSize(1)-1);
		err = Epoc::AllocShadowPage(pageForStart);
		if ((err == KErrNone || err == KErrAlreadyExists) && pageForEnd != pageForStart)
			{
			// Maybe the area spans a page boundary and that was why the copy failed, so try allocing the end bit too
			err = Epoc::AllocShadowPage(pageForEnd);
			}

		if (err == KErrNone || err == KErrAlreadyExists)
			{
			// Retry the copy
#ifdef NO_COPYTOSHADOWMEMORY
			memcpy((TAny*)aAddress, newMem.Ptr(), newMem.Size());
			err = KErrNone;
#else
			err = Epoc::CopyToShadowMemory(aAddress, (TLinAddr)newMem.Ptr(), newMem.Size());
#endif
			}
		}
	return err;
#endif // __WINS__
	}

TInt DMemoryAccess::FreeShadowMemory(TLinAddr aAddress, TInt aLength)
	{
#ifdef __WINS__
	(void)aAddress;
	(void)aLength;
	return KErrNotSupported; // Shadowing not supported on emulator
#else
	const TInt pageSize = Kern::RoundToPageSize(1);
	TLinAddr end = aAddress + aLength;
	TLinAddr page = aAddress & ~(pageSize -1);
	while (page < end)
		{
		Epoc::FreeShadowPage(page);
		page += pageSize;
		}
	return KErrNone; // There aren't any errors that it's important or sensible to pass to the client
#endif // __WINS__
	}

TInt DMemoryAccessFactory::GetEventHandler(DDebuggerEventHandler*& aResult)
	{
	if (!DKernelEventHandler::DebugSupportEnabled()) return KErrNotSupported;
	
	Lock();
	if (iSharedEventHandler)
		{
		aResult = iSharedEventHandler;
		Unlock();
		return KErrNone;
		}
	Unlock(); // Can't hold fast mutex around allocs

	TDfcQue* q = NULL;
	TInt err = GetDfcQue(q);
	if (err) return err;

	DDebuggerEventHandler* handler = DDebuggerEventHandler::New(q);
	if (!handler) return KErrNoMemory;
	// We must NOT call anything that could fail from this point on - otherwise it is impossible to safely close the DDebuggerEventHandler (looks like a bug in constructing a DKernelEventHandler from DLogicalDevice::Install())
	if (err != KErrNone)
		{
		handler->Close();
		return err;
		}
	Lock();
	if (iSharedEventHandler)
		{
		// Someone beat us to it
		Unlock();
		handler->Close();
		}
	else
		{
		iSharedEventHandler = handler;
		Unlock();
		}
	aResult = iSharedEventHandler;

	return KErrNone;
	}

void DMemoryAccessFactory::Lock()
	{
	NKern::FMWait(&iLock);
	}

void DMemoryAccessFactory::Unlock()
	{
	NKern::FMSignal(&iLock);
	}

TInt DMemoryAccess::DebuggerFn(TInt aFn, TAny* a1, TAny* a2)
	{
	DDebuggerEventHandler* handler = NULL;
	TInt err = ((DMemoryAccessFactory*)iDevice)->GetEventHandler(handler);
	if (err) return err;
	
	switch (aFn)
		{
		case RMemoryAccess::EControlGetZombieDebugMode:
			return handler->GetZombieMode();
		case RMemoryAccess::EControlSetZombieDebugMode:
			{
			TInt newmode = (TInt)a1;
			TInt oldmode = handler->GetZombieMode();
			if (newmode && !oldmode) iDevice->Open(); // Stops us getting unloaded
			else if (!newmode && oldmode) iDevice->Close(NULL);
			err = handler->SetZombieMode(newmode);
			break;
			}
		case RMemoryAccess::EControlGetZombies:
			{
			HBuf* zoms = handler->GetZombieThreadIds();
			if (zoms)
				{
				err = Kern::ThreadDesWrite(iClient, a1, *zoms, 0, KTruncateToMaxLength, NULL);
				if (err == KErrNone || err == KErrOverflow) err = zoms->Length();
				delete zoms;
				}
			else
				{
				err = KErrNoMemory;
				}
			break;
			}
		case RMemoryAccess::EControlReleaseZombie:
			{
			DThread* thread = ThreadFromHandle((TInt)a1);
			if (!thread) return KErrBadHandle;
			err = handler->ReleaseZombie(thread);
			thread->Close(NULL);
			break;
			}
		case RMemoryAccess::EControlSuspendThread:
			{
			DThread* thread = ThreadFromHandle((TInt)a1);
			if (!thread) return KErrBadHandle;
			err = handler->SuspendThread(thread);
			thread->Close(NULL);
			break;
			}
		case RMemoryAccess::EControlNotifyBreakpoint:
			{
			if (iClientBreakpointNotifyStatus) return KErrAlreadyExists;
			iClientBreakpointNotifyPkg = a1;
			iClientBreakpointNotifyStatus = (TRequestStatus*)a2;
			err = handler->RegisterForBreakpointNotification(this);
			if (err)
				{
				iClientBreakpointNotifyPkg = NULL;
				iClientBreakpointNotifyStatus = NULL;
				}
			break;
			}
		case RMemoryAccess::EControlCancelNotifyBreakpoint:
			{
			iClientBreakpointNotifyPkg = NULL;
			if (iClientBreakpointNotifyStatus)
				{
				handler->UnregisterForBreakpointNotification(this);
				Kern::RequestComplete(iClient, iClientBreakpointNotifyStatus, KErrCancel);
				}
			break;
			}
		case RMemoryAccess::EControlSetBreakpoint:
			{
			DThread* thread = ThreadFromHandle((TInt)a1);
			if (!thread) return KErrBadHandle;
			TUint32 args[2];
			RMemoryAccess::TPredicate cond;
			err = Kern::ThreadRawRead(iClient, a2, args, sizeof(args));
			if (!err && args[1]) err = Kern::ThreadRawRead(iClient, (TAny*)args[1], &cond, sizeof(RMemoryAccess::TPredicate));
			if (!err) err = handler->SetBreakpoint(thread, args[0], cond);
			thread->Close(NULL);
			break;
			}
		case RMemoryAccess::EControlSetSymbolicBreakpoint:
			{
			TUint32 args[3];
			err = Kern::ThreadRawRead(iClient, a1, args, sizeof(args));
			if (err) return err;
			DThread* thread = ThreadFromHandle(args[0]);
			if (!thread) return KErrBadHandle;
	
			RMemoryAccess::TPredicate cond;
			if (args[2] != 0) err = Kern::ThreadRawRead(iClient, (TAny*)args[2], &cond, sizeof(RMemoryAccess::TPredicate));
			if (err)
				{
				thread->Close(NULL);
				return err;
				}
			
			TInt nameLen = Kern::ThreadGetDesLength(iClient, a2);
			if (nameLen < 0) err = nameLen;
			HBuf* codeseg = NULL;
			if (!err)
				{
				codeseg = HBuf::New(nameLen);
				if (!codeseg) err = KErrNoMemory;
				}
			if (!err) err = Kern::ThreadDesRead(iClient, a2, *codeseg, 0);
			if (!err) err = handler->SetSymbolicBreakpoint(thread, codeseg, args[1], cond);
			
			if (err < 0) delete codeseg;
			thread->Close(NULL);
			break;
			}
		case RMemoryAccess::EControlSetBreakpointEnabled:
			return handler->SetBreakpointEnabled((TInt)a1, (TBool)a2);
		case RMemoryAccess::EControlClearBreakpoint:
			return handler->ClearBreakpoint((TInt)a1);
		case RMemoryAccess::EControlContinueFromBreakpoint:
			{
			DThread* thread = ThreadFromHandle((TInt)a1);
			if (!thread) return KErrBadHandle;
			err = handler->ContinueFromBreakpoint(thread, 0);
			thread->Close(NULL);
			break;
			}
		case RMemoryAccess::EControlGetBreakpoints:
			{
			HBuf* breakpoints = handler->GetBreakpoints();
			if (breakpoints)
				{
				err = Kern::ThreadDesWrite(iClient, a1, *breakpoints, 0, KTruncateToMaxLength, NULL);
				if (err == KErrNone || err == KErrOverflow) err = breakpoints->Length();
				delete breakpoints;
				}
			else
				{
				err = KErrNoMemory;
				}
			break;
			}
		case RMemoryAccess::EControlRegisterPersistantBreakpoint:
			return handler->RegisterPersistantBreakpoint(iClient, (TLinAddr)a1);
		default:
			err = KErrNotSupported;
		}
	return err;
	}

DThread* DMemoryAccess::ThreadFromHandle(TInt aHandle)
	{
	if (aHandle == KCurrentThreadHandle)
		{
		iClient->Open();
		return iClient;
		}
	else
		{
		NKern::LockSystem();
		DThread* thread = (DThread*)Kern::ObjectFromHandle(iClient, aHandle, EThread);
		if (thread) thread->Open();
		NKern::UnlockSystem();
		return thread;
		}
	}

DMemoryAccessFactory::~DMemoryAccessFactory()
	{
	if (iSharedEventHandler)
		{
		iSharedEventHandler->SetZombieMode(0); // This is needed because its iAccessCount is incremented while threads are blocked in its Event function, therefore Close will otherwise never get to the destructor (Which is what otherwise calls CompleteZombies)
		iSharedEventHandler->Close();
		iSharedEventHandler = NULL;
		}
	if (iDfcQueWrapper)
		{
		iDfcQueWrapper->Destroy();
		}
	}

TInt DMemoryAccess::GetRegisters(TAny* a1, TAny* /*a2*/)
	{
	TUint32 args[4];
	TInt err = Kern::ThreadRawRead(iClient, a1, &args[0], sizeof(args));
	if (err) return err;
	TInt threadHandle = args[0];
	TBool userMode = args[1];
	TAny* regBuf = (TAny*)args[2];
	TAny* validPtr = (TAny*)args[3];

	DThread* thread = ThreadFromHandle(threadHandle);
	if (!thread) return KErrBadHandle;
	TUint32 regs[32];
	TUint32 valid = 0;

#if !defined(__WINS__) && !defined(FSHELL_9_1_SUPPORT) // win32 ekern doesn't even export this API let alone implement it
	if (!thread->iSupervisorStack)
		{
		// Thread has not only died but has got as far as deallocating its supervisor stack - in which case we can't
		// get the registers, and moreover NKern::ThreadGetXXXContext will crash if we try
		thread->Close(NULL);
		return KErrDied;
		}
	if (userMode)
		{
		NKern::ThreadGetUserContext(&thread->iNThread, &regs[0], valid);
		}
	else
		{
		NKern::ThreadGetSystemContext(&thread->iNThread, &regs[0], valid);
		}
#else
	(void)userMode;
#endif

	thread->Close(NULL);
	TPckg<TUint32[32]> regsPkg(regs);
	err = Kern::ThreadDesWrite(iClient, regBuf, regsPkg, 0);
	if (err) return err;
	err = Kern::ThreadRawWrite(iClient, validPtr, &valid, sizeof(TUint32));
	return err;
	}

void DMemoryAccess::BreakpointHit(TDes& aPkg)
	{
	TRequestStatus* stat = (TRequestStatus*)NKern::SafeSwap(NULL, (TAny*&)iClientBreakpointNotifyStatus);
	if (stat)
		{
		// Can't see how we could get here with it being null, but still
		TInt err = Kern::ThreadDesWrite(iClient, iClientBreakpointNotifyPkg, aPkg, 0);
		Kern::RequestComplete(iClient, stat, err);
		iClientBreakpointNotifyPkg = NULL;
		}
	}

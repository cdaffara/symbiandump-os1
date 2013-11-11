// debugRouter.cpp
// 
// Copyright (c) 2007 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#include <nkern.h>
#include <kernel.h>
#include <kern_priv.h>
#include <fshell/common.mmh>

#include <e32cmn.h>
#include "debugRouter.h"
#include <platform.h>

#include "dobject_compat.h"
#include "debugRouter-kext.h"
using namespace CloggerDebugRouter;

//define this to get log output
//#define DO_LOGGING

#ifdef DO_LOGGING
#	define LOG Kern::Printf
#else
#	ifdef __WINS__
		inline void LOG(...) {}
#	else
#		define LOG(args...)
#	endif
#endif

#undef ASSERT
#define ASSERT __NK_ASSERT_DEBUG

EXPORT_C extern const TInt KChunkSize; // See patchables.cpp
EXPORT_C extern const TInt KIsrBufSize; // Likewise
EXPORT_C extern const TBool KEnableEarlyRdebug; // Likewise

////////////////////////////////////////////////


const TInt KMajorVersionNumber=1;
const TInt KMinorVersionNumber=0;
const TInt KBuildVersionNumber=0;
_LIT(KPanicCategory,"CloggerRouter");
const TUid KCloggerUid = { FSHELL_UID_CLOGGERSERVER };

class DCloggerDebugRouter;
class DCloggerDebugRouterLDD;
DCloggerDebugRouter* gRouter = NULL;

// DCloggerDebugRouterFactory
//
NONSHARABLE_CLASS(DCloggerDebugRouterFactory) : public DLogicalDevice
    {
public:
    DCloggerDebugRouterFactory();
private:
    TInt Install();                     //overriding pure virtual
    void GetCaps(TDes8& aDes) const;    //overriding pure virtual
    TInt Create(DLogicalChannelBase*& aChannel);         //overriding pure virtual
    
private:
	DOBJECT_PADDING;
    };
    
const TInt KMaxCrashDumpAreas = 8;

NONSHARABLE_CLASS(DCloggerDebugRouter) : public DBase
	{
public:
	DCloggerDebugRouter();
	~DCloggerDebugRouter();

	static TInt Init();
	static TBool TraceHook(const TDesC8& aText, TTraceSource aTraceSource);
	void SetCrashDumpFunctions(TRegisterFn aRegisterFn, TUnregisterFn aUnregisterFn);
	
	// Functions for the LDD
	TInt OpenChunk();
	void CloseChunk();
	TInt RegisterCrashDumpAreas(const TDesC8& aAreas); // Kernel must be unlocked & system lock not held
	TDfcQue& PrivateDfcQ();
	void SetConsumeLogs(TBool aConsume);
	void SetEnabled(TBool aEnabled);
	void ClientRequestData(TRequestStatus* aStatus);

	// DFC callback functions
	static void HandleInterrupt(TAny* aSelf);
	static void KillThreadDFC(TAny* aSelf);
	static void CompleteClient(TAny* aSelf);

	static TBool StayEnabled();
private:
	TBool DoTraceHook(const TDesC8& aText, TTraceSource aTraceSource);
	TBool WriteOut(const TDesC8& aBuf, TInt aLengthToConsider = 0);
	void DoTrace(const TDesC8& aText, TTraceSource aTraceSource, TUint aTraceThread);
	void DoCompleteClient();
	void NotifyClient();
	TInt Construct();
	void CheckInterruptBuffer();

private:
	DChunk* iChunk;
	TLinAddr iStartAddr;
	TLinAddr iEndAddr;
	TLinAddr iCurrent;
	TLinAddr iFirstFrame; // This isn't necessarily the same as iStartAddr, because if we wrap the first thing in the buffer will be continuation and not a header
	TTraceSource iTempBufTraceSource;

	TDfcQue iPrivateDfcQ;
	TDfc iHandleInterruptDfc;
	TDfc iCompleteClientDfc;
	HBuf8* iInterruptBuf;
	//TBuf8<2048> iTempBuf; // This is used to get the data from the unknown context of the trace handler to the DFC
	// Using iTempBuf means we can minimise the amount of time we have to run with interrupts off, while at the same
	// time allowing logging from ISRs and suchlike.
#ifdef __SMP__
	TSpinLock iInterruptBufLock;
	TSpinLock iBufferLock;
#endif
	TBool iConsumeLogs;
	TUint iOverflows;
	TBool iInTheMiddleOfTracing;
	TBool iClientIsBeingNotified;

	TInt iNumCrashDumpAreas;
	SCrashDumpArea iCrashDumpAreas[KMaxCrashDumpAreas];
	TRegisterFn iRegisterCrashDumpFn;
	TUnregisterFn iUnregisterCrashDumpFn;
	
public: // Accessible by LDD class
	DThread* iClient;
	TLinAddr iClientStart;
	TLinAddr iClientEnd;
	TRequestStatus* iClientStatus;
	TBuf8<sizeof(SCloggerCrashDumpArea) * 8> iClientCrashDumpBuf; // For temporary use
	};

// Functions for testing purposes
void DummyRegisterCrashDump(TAny* aAddr, TUint aSize, SCrashDumpArea& aCrashDumpArea)
	{
	LOG("Registering crashdump 0x%x %d %S", aAddr, aSize, &aCrashDumpArea.iName);
	LOG("First and last words are %x %x", ((TUint*)aAddr)[0], ((TUint*)aAddr)[(aSize/4)-1]);
	}
void DummyUnregisterCrashDump(SCrashDumpArea& aCrashDumpArea)
	{
	LOG("Unregistering crashdump %S", &aCrashDumpArea.iName);
	}
	
// DECLARE_STANDARD_LDD
//  Standard Extension Framework export
DECLARE_STANDARD_EXTENSION()
	{
	//__BREAKPOINT();
	TInt err = DCloggerDebugRouter::Init();
#ifdef __WINS__
	// On WINS there's no way of setting the debug port to indicate we should enable ourselves.
	// Therefore we assume that if someone's gone to the trouble of adding "extension += clogger-debugrouter.ldd" to
	// their epoc.ini, then they want us to be enabled.
	if (!err)
		{
		err = gRouter->OpenChunk();
		}
	if (!err)
		{
		gRouter->SetConsumeLogs(ETrue);
		gRouter->SetEnabled(ETrue);
		}
#endif

	return err;
	}

DECLARE_EXTENSION_LDD()
	{
	//__BREAKPOINT();
	TBool ok = CalculateDObjectSize();
	if (!ok) return NULL;
	return new DCloggerDebugRouterFactory();
	}

// DCloggerDebugRouter
//
NONSHARABLE_CLASS(DCloggerDebugRouterLDD) : public DLogicalChannel
    {
public:
	DCloggerDebugRouterLDD(DCloggerDebugRouter* aRouter);
	~DCloggerDebugRouterLDD();
private:
//Framework
    TInt DoCreate(TInt aUnit,const TDesC* anInfo,const TVersion& aVer);
	void HandleMsg(TMessageBase* aMsg);
    void DoCancel(TInt aReqNo);
    TInt DoRequest(TInt aReqNo,TRequestStatus* aStatus,TAny* a1,TAny* a2);
    TInt DoControl(TInt aFunction,TAny *a1,TAny *a2);

	DCloggerDebugRouter* _iRouter;
	DOBJECT_PADDING;
	};

#define iRouter SAFE_MEMBER(_iRouter)

DCloggerDebugRouterFactory::DCloggerDebugRouterFactory()
	{
	SAFE_MEMBER(iVersion)=TVersion(KMajorVersionNumber,KMinorVersionNumber,KBuildVersionNumber);
	}

TInt DCloggerDebugRouterFactory::Create(DLogicalChannelBase*& aChannel)
    {
	TInt err = DCloggerDebugRouter::Init();
	if (err) return err;

    aChannel = new DCloggerDebugRouterLDD(gRouter);
	if (aChannel==NULL)
		return KErrNoMemory;
	else
		return KErrNone;
    }

TInt DCloggerDebugRouterFactory::Install()
    {
    return(SetName(&KDebugRouterName));
    }

void DCloggerDebugRouterFactory::GetCaps(TDes8& /*aDes*/) const
    {
/*    TCapsMemoryAccessV01 b;
    b.iVersion=TVersion(KMajorVersionNumber,KMinorVersionNumber,KBuildVersionNumber);
    aDes.FillZ(aDes.MaxLength());
    aDes.Copy((TUint8*)&b,Min(aDes.MaxLength(),sizeof(b)));
*/    } 

// class DCloggerDebugRouter Implementation

DCloggerDebugRouterLDD::DCloggerDebugRouterLDD(DCloggerDebugRouter* aRouter)
    : DLogicalChannel()
    {
    //SetBehaviour(-1); //JR
	iRouter = aRouter;
    }

DCloggerDebugRouter::DCloggerDebugRouter()
	: iHandleInterruptDfc(NULL, NULL), iCompleteClientDfc(NULL, NULL)
#ifdef __SMP__
	, iInterruptBufLock(TSpinLock::EOrderGenericIrqLow0), iBufferLock(TSpinLock::EOrderGenericPreLow0)
#endif
	{
	}

const TInt KMemAccessDfcThreadPriority = 27;

// DCloggerDebugRouter::DoCreate
//  Check capabilities & version & start listening for events
TInt DCloggerDebugRouterLDD::DoCreate(TInt /*aUnit*/,const TDesC* /*anInfo*/,const TVersion& aVer)
    {
	TInt ret=KErrNone;
	
	/*
	//Require Power Management and All Files to use this driver
	//Not ideal, but better than nothing
	if(!Kern::CurrentThreadHasCapability(ECapabilityPowerMgmt,__PLATSEC_DIAGNOSTIC_STRING("Checked by MEMORYACCESS.LDD (Memory Access for Task Managers)")))
	    ret= KErrPermissionDenied;
	if(!Kern::CurrentThreadHasCapability(ECapabilityAllFiles,__PLATSEC_DIAGNOSTIC_STRING("Checked by MEMORYACCESS.LDD (Memory Access for Task Managers)")))
	    ret= KErrPermissionDenied;
    */
    if (!Kern::QueryVersionSupported(TVersion(KMajorVersionNumber,KMinorVersionNumber,KBuildVersionNumber),aVer))
    	{
    	return KErrNotSupported;
    	}

	if (iRouter->iClient)
		{
		return KErrAlreadyExists;
		}

    iRouter->iClient=&Kern::CurrentThread();

   	SetDfcQ(&iRouter->PrivateDfcQ());
   	SAFE_MEMBER(iMsgQ).Receive();		

	//Kern::Printf("DCloggerDebugRouter::DoCreate %d",ret);
    return ret;
    }

TDfcQue& DCloggerDebugRouter::PrivateDfcQ()
	{
	return iPrivateDfcQ;
	}

TInt DCloggerDebugRouter::Init()
	{
	//__BREAKPOINT();
	if (gRouter) return KErrNone;
	TInt err;
#ifdef __WINS__
	// WINSCW can't handle DLLs with WSD very well, particularly dynamically loaded ones like LDDs
	RPropertyRef property; // use this to stash the pointer that DLL loading keeps stomping over
	NKern::ThreadEnterCS();
	const TInt EDebugRouterPtr = 1000; // from cliserv.h
	err = property.Open(KCloggerUid, EDebugRouterPtr);
	if (!err)
		{
		TInt& ptr = *reinterpret_cast<TInt*>(&gRouter);
		property.Get(ptr);
		}
	property.Close();
	NKern::ThreadLeaveCS();
	if (gRouter) return KErrNone;
#endif

	DCloggerDebugRouter* self = new DCloggerDebugRouter;
	if (!self) return KErrNoMemory;

	err = self->Construct();
	if (err == KErrNone && StayEnabled())
		{
		// Then enable ourselves
		err = self->OpenChunk();
		if (!err)
			{
			self->SetConsumeLogs(ETrue);
			self->SetEnabled(ETrue);
			}
		}
	if (err)
		{
		delete self;
		return err;
		}

	gRouter = self;
#ifdef __WINS__
	// See comment at top of function
	NKern::ThreadEnterCS();
	err = property.Attach(KCloggerUid, EDebugRouterPtr);
	if (!err)
		{
		_LIT_SECURITY_POLICY_FAIL(KKernOnlyPolicy);
		err = property.Define(RProperty::EInt, KKernOnlyPolicy, KKernOnlyPolicy);
		}
	if (err == KErrNone || err == KErrAlreadyExists)
		{
		TInt& ptr = *reinterpret_cast<TInt*>(&gRouter);
		property.Set(ptr);
		}
	property.Close();
	NKern::ThreadLeaveCS();
#endif
	
	// And finally, set up some dummy functions for debugging
#ifdef __WINS__
	CloggerDebugRouter::SetCrashDumpFunctions(&DummyRegisterCrashDump, &DummyUnregisterCrashDump);
#endif

	return KErrNone;
	}

TInt DCloggerDebugRouter::Construct()
	{
	iInterruptBuf = HBuf8::New(KIsrBufSize);
	if (!iInterruptBuf) return KErrNoMemory;

	_LIT(KDfcThreadName, "CloggerDebugRouter");
	TInt err = Kern::DfcQInit(&iPrivateDfcQ, KMemAccessDfcThreadPriority, &KDfcThreadName);
	if (err) 
		{
		return err;
		}
	iHandleInterruptDfc = TDfc(&HandleInterrupt, this, &iPrivateDfcQ, 1); // Interrupt handler DFC higher priority than completing client
	iCompleteClientDfc = TDfc(&CompleteClient, this, &iPrivateDfcQ, 0);
	return err;
	}

// dtor
//
DCloggerDebugRouter::~DCloggerDebugRouter()
    {
	//__BREAKPOINT();
	TTraceHandler old = Kern::SetTraceHandler(NULL);
	if (old != &TraceHook)
		{
		// Then set it back. What we wanted to do is if (TraceHandler == &TraceHook) { SetTraceHandler(NULL) }
		// But there is no API to get the hook without also setting it.
		Kern::SetTraceHandler(old);
		}
	CloseChunk();

	// Finally queue a DFC to kill our DFC thread
	if (iPrivateDfcQ.iThread)
		{
		TDfc killDfcThread(KillThreadDFC, this, &iPrivateDfcQ, 0); 
		killDfcThread.Enque();
		}
	}

DCloggerDebugRouterLDD::~DCloggerDebugRouterLDD()
    {
	//__BREAKPOINT();
	ASSERT(!iRouter->iClientStatus); // Should have received a close message first
	iRouter->iClient = NULL;
	}

void DCloggerDebugRouter::KillThreadDFC(TAny* /*aSelfP*/)
	{
	Kern::Exit(KErrNone);
	} 

void DCloggerDebugRouter::HandleInterrupt(TAny* aSelf)
	{
	static_cast<DCloggerDebugRouter*>(aSelf)->CheckInterruptBuffer();
	}

void DCloggerDebugRouter::CheckInterruptBuffer()
	{
	TBuf8<256> interruptBufCopy; // Using a stack buffer here means we don't have to lock the kernel just yet
#ifdef __SMP__
	TInt irq = __SPIN_LOCK_IRQSAVE(iInterruptBufLock);
#else
	TInt irq = NKern::DisableAllInterrupts(); // Prevent an interrupt changing it while we're copying it
#endif
	if (iInterruptBuf->Length() > 256)
		{
		// Need to rethink the stack buffer!
		__BREAKPOINT();
		}
	interruptBufCopy.Copy(*iInterruptBuf);
	iInterruptBuf->Zero();
	memclr((TAny*)iInterruptBuf->Ptr(), iInterruptBuf->Length()); // So that you don't see old data in the crashlog
#ifdef __SMP__
	__SPIN_UNLOCK_IRQRESTORE(iInterruptBufLock, irq);
#else
	NKern::RestoreInterrupts(irq);
#endif
	if (interruptBufCopy.Length())
		{
		//__BREAKPOINT();
		DoTrace(interruptBufCopy, EKernelTrace, 0);
		}
	}
   
// DCloggerDebugRouter::HandleMsg
//  Called when user side sends us something
void DCloggerDebugRouterLDD::HandleMsg(TMessageBase* aMsg)
	{
	//Kern::Printf("DCloggerDebugRouter::HandleMsg");
    TThreadMessage& m=*(TThreadMessage*)aMsg;

    // Get message type
    TInt id=m.iValue;

    // Decode the message type and dispatch it to the relevent handler function...
    
    // A logical channel can be closed either explicitly by its user-side client,
    // or implicitly if the client thread dies. In the latter case, the channel
    // is closed in the context of the kernel supervisor thread. 

    if (id==(TInt)ECloseMsg)
        {
        //Kern::Printf("DCloggerDebugRouter::HandleMsg Close");
        // Channel Close
        DoCancel(RCloggerDebugRouter::EAllRequests); //JR
        m.Complete(KErrNone, EFalse);
        return;
        }

    // For all other message types, we check that the message is from the thread
    // that created us.
    if(m.Client()!=iRouter->iClient)
        {
      	//Kern::Printf("DCloggerDebugRouter::HandleMsg About");
        Kern::ThreadKill(m.Client(),EExitPanic,ERequestFromWrongThread,KPanicCategory);
        m.Complete(KErrNone,ETrue);
        return;
        }
    if (id==KMaxTInt)
        {
        //Kern::Printf("DCloggerDebugRouter::HandleMsg Cancel");
        // DoCancel
        DoCancel(m.Int0());
        m.Complete(KErrNone,ETrue);
        return;
        }
    if (id<0)
        {
        // DoRequest
        //Kern::Printf("DCloggerDebugRouter::HandleMsg Do Request");
        TRequestStatus* pS=(TRequestStatus*)m.Ptr0();
        TInt r=DoRequest(~id, pS, m.Ptr1(), m.Ptr2());
        if (r!=KErrNone)
            Kern::RequestComplete(iRouter->iClient,pS,r);
        m.Complete(KErrNone,ETrue);
        }
    else
        {
        // DoControl
        //Kern::Printf("DCloggerDebugRouter::HandleMsg Do Control");
        TInt r=DoControl(id,m.Ptr0(),m.Ptr1());
        m.Complete(r,ETrue);
        }	
	}

// DCloggerDebugRouter::DoRequest
//  Handles async messages, called from HandleMsg
TInt DCloggerDebugRouterLDD::DoRequest(TInt aReqNo, TRequestStatus* aStatus, TAny* /*a1*/, TAny* /*a2*/)
    {
	if (aReqNo == RCloggerDebugRouter::ERequestGetData)
		{
		iRouter->ClientRequestData(aStatus);
		}
    return KErrNone;
    }

void DCloggerDebugRouter::ClientRequestData(TRequestStatus* aStatus)
	{
	iClientStatus = aStatus;
	NKern::Lock();
#ifdef __SMP__
	__SPIN_LOCK(iBufferLock);
	NotifyClient();
	__SPIN_UNLOCK(iBufferLock);
#else
	NotifyClient();
#endif
	NKern::Unlock();
	}



// DCloggerDebugRouter::DoCancel
//  Cancels async messages, called from HandleMsg
void DCloggerDebugRouterLDD::DoCancel(TInt /*aReqNo*/)
    {
	if (iRouter->iClientStatus)
		{
		Kern::RequestComplete(iRouter->iClient, iRouter->iClientStatus, KErrCancel);
		}
    }

void DCloggerDebugRouter::SetEnabled(TBool aEnabled)
	{
	if (aEnabled)
		{
		/*TTraceHandler old =*/ Kern::SetTraceHandler(&TraceHook);
		}
	else
		{
		if (Kern::SuperPage().iDebugPort != KCloggerDebugPort)
			{
			// If we are the debug port, we cannot unset our trace hook, there's nowhere else for the logs to go
			// Don't try and restore any previous trace handler - no guarantee the pointer is still valid or safe to call
			Kern::SetTraceHandler(NULL);
			iCurrent = iStartAddr;
			iFirstFrame = iCurrent;
			}
		// Reset client pointers
		iClientStart = iStartAddr;
		iClientEnd = iClientStart;
		}
	}

void DCloggerDebugRouter::SetConsumeLogs(TBool aConsume)
	{
	if (Kern::SuperPage().iDebugPort == KCloggerDebugPort)
		{
		// Then the LDD has no say in whether we are allowed to consume the logs or not
		// Because there is nowhere else for them to go if we are the debug port, we must consume them
		aConsume = ETrue;
		}
	//iConsumeLogs = EFalse; // So that we can get logging from the kernel even when rdebug redirection is enabled
	iConsumeLogs = aConsume;
	}

// DCloggerDebugRouter::DoControl
//  Handles sync messages, called from HandleMsg
TInt DCloggerDebugRouterLDD::DoControl(TInt aFunction, TAny* a1, TAny* a2)
    {
	//Kern::Printf("[DCloggerDebugRouter] ::DoControl; Function: %d", aFunction);
    switch (aFunction)
        {
	case RCloggerDebugRouter::EControlEnableRouting:
		{
		TBool consume = (TInt)a1 == RCloggerDebugRouter::EEnableRoutingAndConsume;
		TBool enable = (TBool)a1;

		iRouter->SetConsumeLogs(consume);
		iRouter->SetEnabled(enable);
		return KErrNone;
		}
	case RCloggerDebugRouter::EControlOpenChunk:
		{
		TInt res = iRouter->OpenChunk();
		return res;
		}
	case RCloggerDebugRouter::EControlRegisterBuffers:
		{
		TInt err = Kern::ThreadDesRead(iRouter->iClient, a1, iRouter->iClientCrashDumpBuf, 0);
		if (!err)
			{
			err = iRouter->RegisterCrashDumpAreas(iRouter->iClientCrashDumpBuf);
			}
		return err;
		}
	case RCloggerDebugRouter::EControlCreateAndOpenAChunk:
		{
		SCreateChunkParams params;
		TInt res = Kern::ThreadRawRead(iRouter->iClient, a1, &params, sizeof(SCreateChunkParams));
		if (res != KErrNone) return res;
		
		DThread* otherThread = NULL;
		TInt otherThreadHandle = 0;
		if (params.iHandleOfOtherThread)
			{
			NKern::LockSystem();
			otherThread = (DThread*)Kern::ObjectFromHandle(iRouter->iClient, params.iHandleOfOtherThread, EThread);
			if (otherThread) otherThread->Open(); // Make sure it doesn't disappear before we've finished with it
			NKern::UnlockSystem();
			}
		
		DChunk* chunk = NULL;
		TChunkCreateInfo info;
		info.iType         = TChunkCreateInfo::ESharedKernelSingle;
		info.iMaxSize      = params.iMaxSize;
#ifdef __EPOC32__
		info.iMapAttr      = (TInt)EMapAttrFullyBlocking; // Full caching
#endif
		info.iOwnsMemory   = ETrue; // Use memory from system's free pool
		info.iDestroyedDfc = NULL;

		TUint32 chunkMapAttr;
		TLinAddr addr;
		NKern::ThreadEnterCS();
		res = Kern::ChunkCreate(info, chunk, addr, chunkMapAttr);
		if (res != KErrNone)
			{
			goto CloseAndBail;
			}
		if (params.iCommittedSize > 0)
			{
			res = Kern::ChunkCommit(chunk,0,params.iCommittedSize);
			}
		if(res!=KErrNone)
			{
			goto CloseAndBail;
			}

		// Still in CS
		
		if (otherThread)
			{
			// Create handle for other thread
			otherThreadHandle = Kern::MakeHandleAndOpen(otherThread, chunk);
			}
		if (otherThreadHandle < 0)
			{
			res = otherThreadHandle;
			goto CloseAndBail;
			}
		// Create handle for our thread
		res = Kern::MakeHandleAndOpen(iRouter->iClient, chunk);
		// Open our handle last so we don't have to worry about closing it in CloseAndBail if an error occurred later
		goto CloseAndBail;
		// All done.
		
CloseAndBail:
		Kern::ChunkClose(chunk);
		if (res < 0 && otherThreadHandle > 0) Kern::CloseHandle(otherThread, otherThreadHandle);
		NKern::ThreadLeaveCS();
		
		if (res > 0)
			{
			params.iOtherThreadChunkHandle = otherThreadHandle;
			params.iChunkHandle = res;
			res = KErrNone;
			Kern::ThreadRawWrite(iRouter->iClient, a1, &params, sizeof(SCreateChunkParams));
			}
		return res;
		}
	case RCloggerDebugRouter::EControlAdjustChunk:
		{
		TInt handle = (TInt)a1;
		TInt newSize = (TInt)a2;
		NKern::LockSystem();
		DChunk* chunk = (DChunk*)Kern::ObjectFromHandle(iRouter->iClient, handle, EChunk);
		if (chunk) chunk->Open(); // Make sure it doesn't disappear before we've finished with it
		NKern::UnlockSystem();
		TInt err = KErrNotFound;
		if (chunk)
			{
			NKern::ThreadEnterCS();
			TInt oldSize = chunk->Size();
			TInt diff = Kern::RoundToPageSize(newSize - oldSize);
			// Because kernel-created chunks appear to be specified as disconnected, we can't use the simple Adjust
			//err = chunk->Adjust(newSize);
			// even though that's the behaviour we want
			if (diff > 0)
				{
				err = Kern::ChunkCommit(chunk, oldSize, diff);
				if (err == KErrArgument) err = KErrNoMemory; // Generally that's what it means
				}
			else if (diff < 0)
				{
				// It appears that there is no way of decommitting memory from a shared chunk
				// So we will just stay the same size
				//err = chunk->Decommit(newSize, -diff);
				err = KErrNone;
				}
			else
				{
				// no change
				err = KErrNone;
				}
			Kern::ChunkClose(chunk);
			NKern::ThreadLeaveCS();
			}
		return err;
		}
    default:
		(void)a2;
        return KErrNotSupported;
        }
    }


TBool DCloggerDebugRouter::TraceHook(const TDesC8& aText, TTraceSource aTraceSource)
	{
#if defined(DO_LOGGING) // || defined(_DEBUG)
	// This code is here so it's easier to debug problems in my trace hook that cause kern faults (that in turn
	// try to call Kern::Printf...)
	if (aTraceSource == EKernelTrace) return EFalse;
#endif
#ifdef __WINS__
	// This is necessary in case loading the LDD stomped over the global data
	if (!gRouter) return ETrue;
#endif
	return gRouter->DoTraceHook(aText, aTraceSource);
	}

TBool DCloggerDebugRouter::DoTraceHook(const TDesC8& aText, TTraceSource aTraceSource)
	{
	TBool consume = iConsumeLogs;

	TUint traceThread = 0; // 0 means non-thread (ie interrupt or IDFC) that will appear as the generic "Kern::Printf"
	TInt context = NKern::CurrentContext();
	if (context == NKern::EInterrupt)
		{
		// Interrupts copy to a special buffer. No timestamping or prettying up is done at the point of the interrupt
		// It is handled next time someone logs or when the DFC fires, whichever is sooner
#ifdef __SMP__
		TInt irq = __SPIN_LOCK_IRQSAVE(iInterruptBufLock);
		iInterruptBuf->Append(aText);
		__SPIN_UNLOCK_IRQRESTORE(iInterruptBufLock, irq);
#else
		iInterruptBuf->Append(aText);
#endif

		iHandleInterruptDfc.Add();
		return consume;
		}
	
	if (context == NKern::EThread)
		{
		DThread& thread = Kern::CurrentThread();
		traceThread = thread.iId;
		}

	// Before doing our logging, check if there's anything in the interrupt buffer. This means that (barring interrupt logging that happens while we're running the TraceHook function) the logging should still end up sequential
	CheckInterruptBuffer();

	if (aTraceSource == EUserTrace)
		{
		DProcess& userProc = Kern::CurrentProcess();
		if (userProc.iS.iSecureId == (TUint)KCloggerUid.iUid)
			{
			return EFalse; // Debugs from clogger itself should be passed through to the debugport
			}
		}

	DoTrace(aText, aTraceSource, traceThread);
	return consume;
	}

void DCloggerDebugRouter::DoTrace(const TDesC8& aText, TTraceSource aTraceSource, TUint aTraceThread)
	{
	LOG("+DoTrace iStartAddr=%x iEndAddr=%x iCurrent=%x iClientStart=%x iClientEnd=%x", iStartAddr, iEndAddr, iCurrent, iClientStart, iClientEnd);
	SCloggerTraceInfo info;
	info.iTraceType = aTraceSource == EUserTrace ? 'U' : aTraceSource == EPlatSecTrace ? 'P' : 'K';
	info.iReserved = 0;
	info.iLength = aText.Length();
	info.iTickCount = NKern::TickCount();
	info.iThreadId = aTraceThread;
	TPckg<SCloggerTraceInfo> id(info);
	
	NKern::Lock();
#ifdef __SMP__
	__SPIN_LOCK(iBufferLock);
#endif
	if (iInTheMiddleOfTracing)
		{
		// In a recursive loop, probably due to an assertion failure in the below code triggering a kern::printf
		__BREAKPOINT();
		}
	iInTheMiddleOfTracing = ETrue;

	TBool ok = WriteOut(id, id.Length() + aText.Length());
	if (ok) WriteOut(aText);
	if (!ok)
		{
		//consume = EFalse; // Don't consume if the buffer overflowed
		if (iOverflows != KMaxTUint)
			{
			// Don't overflow the overflow counter!
			iOverflows++;
			}
		}
	NotifyClient();
	iInTheMiddleOfTracing = EFalse;
	LOG("-DoTrace iStartAddr=%x iEndAddr=%x iCurrent=%x iClientStart=%x iClientEnd=%x", iStartAddr, iEndAddr, iCurrent, iClientStart, iClientEnd);
#ifdef __SMP__
	__SPIN_UNLOCK(iBufferLock);
#endif

	NKern::Unlock();
	}

TBool DCloggerDebugRouter::WriteOut(const TDesC8& aBuf, TInt aLengthToConsider)
	{
	/*
	In the case we are using the end of the buffer and the client is writing the start (canWrap == TRUE)
	                +-----------+
                    |           |
	iClientStart -> |           |
	                |           |
	                |           |
	iClientEnd   -> |           | ^
	                |           | | This is what needs writing out next
	                |           | | (currently)
	                |           | v
	                |           | <- iCurrent
	                |           |
	                |           |
					+-----------+

	In the case where the client is writing near the end and we have wrapped round to the beginning (canWrap == FALSE)
                    +-----------+
                    |           |
                    |           |
	                |           | <- iCurrent
	                |           |
	                |           |
	                |           |
	                |           |
	                |           |
    iClientStart -> |           | 
	                |           |
	iClientEnd   -> |           |
					+-----------+
	*/

	TUint endSpace, startSpace;
	if (iClient && iClientStart)
		{
		TBool canWrap = iClientStart <= iCurrent; // If the client is still using the end of the buffer, we can't wrap around
		endSpace = (canWrap ? iEndAddr : iClientStart) - iCurrent;
		startSpace = canWrap ? (iClientStart - iStartAddr) : 0;
		}
	else
		{
		// If there's no client yet, we can ignore the client ptrs.
		// TODO if there's no client, we should also do a rolling overwrite of the buffer (ie better to drop the oldest statements than the recent ones
		// TODO however this has its problems because this is not a pure byte stream - it'd mess up the headers if you were to try and save what was in the buffer rather than clearing it when it gets full
		endSpace = iEndAddr - iCurrent;
		startSpace = 0; //iCurrent - iStartAddr;
		}

	ASSERT(endSpace <= iEndAddr - iStartAddr);
	ASSERT(startSpace < iEndAddr - iStartAddr);

	TInt lenToTest = aLengthToConsider;
	if (lenToTest == 0) lenToTest = aBuf.Length();

	if ((TUint)lenToTest > startSpace + endSpace) return EFalse;

	TPtrC8 endFrag = aBuf.Left(endSpace);
	memcpy((TAny*)iCurrent, endFrag.Ptr(), endFrag.Size());
	iCurrent += endFrag.Size();

	if (endFrag.Length() < aBuf.Length())
		{
		LOG("Wrapping round");
		ASSERT(iCurrent == iEndAddr);
		iCurrent = iStartAddr;
		TPtrC8 remainder = aBuf.Mid(endSpace);
		memcpy((TAny*)iStartAddr, remainder.Ptr(), remainder.Size());
		iCurrent += remainder.Size();
		iFirstFrame = iCurrent; // iFirstFrame is now no longer iStartAddr, it's the earliest point in the buffer that a frame starts at
		}
	return ETrue;
	}

void DCloggerDebugRouter::CloseChunk()
	{
	if (iChunk)
		{
		NKern::ThreadEnterCS();
		Kern::ChunkClose(iChunk);
		iChunk = NULL;
		NKern::ThreadLeaveCS();
		}
	}
    
TInt DCloggerDebugRouter::OpenChunk()
	{
	TInt r = KErrNone;
	if (iChunk == NULL)
		{
		TChunkCreateInfo info;
		info.iType         = TChunkCreateInfo::ESharedKernelSingle;
		info.iMaxSize      = KChunkSize;
#ifdef __EPOC32__
		info.iMapAttr      = (TInt)EMapAttrFullyBlocking; // Full caching
#endif
		info.iOwnsMemory   = ETrue; // Use memory from system's free pool
		info.iDestroyedDfc = NULL;

		TUint32 chunkMapAttr;
		TLinAddr addr;
		NKern::ThreadEnterCS();
		if (KErrNone != (r = Kern::ChunkCreate(info, iChunk, addr, chunkMapAttr)))
			{
			NKern::ThreadLeaveCS();
			return r;
			}
		r = Kern::ChunkCommit(iChunk,0,KChunkSize);
		if(r!=KErrNone)
			{
			Kern::ChunkClose(iChunk);
			iChunk = NULL;
			NKern::ThreadLeaveCS();
			return r;
			}
		NKern::ThreadLeaveCS();
		//TLinAddr addr = (TLinAddr)iChunk->Base();
		iStartAddr = addr + sizeof(SDebugChunkHeader); // Bottom few words are reserved for metadata
		iEndAddr = addr + KChunkSize;
		iCurrent = iStartAddr;
		iFirstFrame = iStartAddr;
		RegisterCrashDumpAreas(KNullDesC8); // As now we have our main chunk
		}
	if (iClient)
		{
		NKern::ThreadEnterCS();
		r = Kern::MakeHandleAndOpen(iClient, iChunk); 
		NKern::ThreadLeaveCS();
		/*
		if(r < KErrNone)
			{
			Kern::ChunkClose(iChunk);
			iChunk = NULL;
			NKern::ThreadLeaveCS();
			return r;
			}
		*/
		iClientStart = iFirstFrame;
		iClientEnd = iClientStart;
		}

	return r;
	}

void DCloggerDebugRouter::NotifyClient()
	{
	LOG("NotifyClient");
	if (iCompleteClientDfc.Queued())
		{
		// The DFC that will signal the client has already been fired and the client pointers calculated. Don't get in its way.
		return;
		}
	if (!iClientStatus || !iClient)
		{
		// Client not ready
		return;
		}
	if (iCurrent == iClientEnd)
		{
		// No new data to send
		return;
		}

	LOG("Notifying client enqueuing client DFC");
	
	// The start of the chunk stores the info for the client about which bits of the buffer it needs to read
	TUint realStartAddr = iStartAddr - sizeof(SDebugChunkHeader);
	SDebugChunkHeader* header = (SDebugChunkHeader*)realStartAddr;
	TUint* clientStartPtr = &header->iStartOffset;
	TUint* clientEndPtr = &header->iEndOffset;
	TUint* overflows = &header->iOverflows;

	LOG("Notifying with iClientStart=%x iClientEnd=%x iCurrent=%x", iClientStart, iClientEnd, iCurrent);

	*overflows = iOverflows;
	iOverflows = 0; // Reset this each time we write to the client, it's not cumulative

	TLinAddr end = iCurrent;
	iClientStart = iClientEnd;
	iClientEnd = end;

	*clientStartPtr = iClientStart - realStartAddr; // Offset of the end of the last read from the start of the chunk
	*clientEndPtr = iClientEnd - realStartAddr; // Offset of current ptr from start of chunk

	LOG("Signalling client start %08x end %08x", *clientStartPtr, *clientEndPtr);
	iCompleteClientDfc.DoEnque();
	}

void DCloggerDebugRouter::CompleteClient(TAny* aSelf)
	{
	DCloggerDebugRouter* self = static_cast<DCloggerDebugRouter*>(aSelf);
	self->DoCompleteClient();
	}

void DCloggerDebugRouter::DoCompleteClient()
	{
	// Kern::RequestComplete involves taking the system lock, which will cause debug if NKERN is set
	// So temporarily unset it
	TInt debugMask = 0;
	TInt oldVal = (TInt)NKern::SafeSwap((TAny*)debugMask, (TAny*&)Kern::SuperPage().iDebugMask[0]);
	Kern::RequestComplete(iClient, iClientStatus, KErrNone);
	Kern::SuperPage().iDebugMask[0] = oldVal;
	}

EXPORT_C void CloggerDebugRouter::DebugPortChanged()
	{
	DCloggerDebugRouter* self = gRouter;
	if (self)
		{
		if (Kern::SuperPage().iDebugPort == KCloggerDebugPort)
			{
			// Then enable ourselves
			TInt err = self->OpenChunk();
			if (!err)
				{
				self->SetConsumeLogs(ETrue);
				self->SetEnabled(ETrue);
				}
			}
		else
			{
			// Disable? Or should we just stay running until explicitly disabled by the LDD?
			//self->SetEnabled(EFalse);

			// I think we should stay enabled until the LDD tells us otherwise...
			}
		}
	}

EXPORT_C void CloggerDebugRouter::SetCrashDumpFunctions(TRegisterFn aRegisterFn, TUnregisterFn aUnregisterFn)
	{
	if (gRouter)
		{
		gRouter->SetCrashDumpFunctions(aRegisterFn, aUnregisterFn);
		}
	}

void DCloggerDebugRouter::SetCrashDumpFunctions(TRegisterFn aRegisterFn, TUnregisterFn aUnregisterFn)
	{
	NKern::Lock();
	iRegisterCrashDumpFn = aRegisterFn;
	iUnregisterCrashDumpFn = aUnregisterFn;
	NKern::Unlock();
	RegisterCrashDumpAreas(KNullDesC8);
	}

TInt DCloggerDebugRouter::RegisterCrashDumpAreas(const TDesC8& aAreas)
	{
	if (!iRegisterCrashDumpFn || !iUnregisterCrashDumpFn)
		{
		return KErrNotReady;
		}
	
	LOG("Unregistering current crashdumps");
	// Unregister current ones
	for (TInt i = 0; i < iNumCrashDumpAreas; i++)
		{
		(*iUnregisterCrashDumpFn)(iCrashDumpAreas[i]);
		DChunk* chunk = iCrashDumpAreas[i].iChunk;
		if (chunk)
			{
			LOG("Closing chunk %O", chunk);
			NKern::ThreadEnterCS();
			chunk->Close(NULL);
			NKern::ThreadLeaveCS();
			LOG("Closed chunk %O", chunk);
			}
		}
	iNumCrashDumpAreas = 0;
	
	// Register our ones
	LOG("Registering new crashdumps");
	if (iChunk)
		{
		LOG("Registering rdebugprint buffer");
		_LIT8(KLddBuf, "Clogger RDebug::Print buffer");
		SCrashDumpArea& area = iCrashDumpAreas[iNumCrashDumpAreas++];
		area.iChunk = NULL; // Only specify the chunk for client-side chunks we otherwise don't hold open
		area.iName = KLddBuf;
		(*iRegisterCrashDumpFn)((TAny*)iStartAddr, iEndAddr-iStartAddr, area);
		}
	LOG("Registering ISR buffer");
	_LIT8(KIsrBuf, "Clogger ISR buffer");
	SCrashDumpArea& area = iCrashDumpAreas[iNumCrashDumpAreas++];
	area.iChunk = NULL; // Only specify the chunk for client-side chunks we otherwise don't hold open
	area.iName = KIsrBuf;
	(*iRegisterCrashDumpFn)((TAny*)iInterruptBuf->Ptr(), iInterruptBuf->MaxSize(), area);
	
	// Now do client ones
	const SCloggerCrashDumpArea* clientAreas = (const SCloggerCrashDumpArea*)aAreas.Ptr();
	TInt num = aAreas.Length() / sizeof(SCloggerCrashDumpArea);
	for (TInt i = 0; i < num && iNumCrashDumpAreas < KMaxCrashDumpAreas; i++)
		{
		const SCloggerCrashDumpArea& clientArea = clientAreas[i];
		LOG("Getting chunk for client buffer %d", i);
		NKern::LockSystem();
		DChunk* chunk = (DChunk*)Kern::ObjectFromHandle(iClient, clientArea.iChunkHandle, EChunk);
		NKern::UnlockSystem();
		LOG("Got client chunk %O", chunk);
		if (chunk && chunk->Open() == KErrNone)
			{
			SCrashDumpArea& area = iCrashDumpAreas[iNumCrashDumpAreas++];
			area.iChunk = chunk;
			area.iName = clientArea.iName;
			TAny* ptr = (TAny*)(chunk->Base() + clientArea.iOffset);
			(*iRegisterCrashDumpFn)(ptr, clientArea.iSize, area);
			}
		}
	LOG("Completed registering new crashdump areas");
	return KErrNone;
	}

TBool DCloggerDebugRouter::StayEnabled()
	{
	return KEnableEarlyRdebug || Kern::SuperPage().iDebugPort == KCloggerDebugPort;
	}

// End of File

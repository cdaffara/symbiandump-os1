// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

/**
 @file
 @internalComponent
*/

#include "rsstd.h"
#include <elements/cfmacro.h>
#include <cfmsgs.h>
#include <cfshared.h>
#include <cflog.h>
#include <cfextras.h>
#include "bindmgr.h"


#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ElemRootServerrt, "ElemRootServerrt");
#endif

__CFLOG_STMT(_LIT8(KLogSubSysRS, "RootServer");) // subsystem name

CNotifySuddenDeathContainer::CNotifySuddenDeathContainer(MNotifySuddenDeath* aNotifier):
    iNotifier(aNotifier)
    /** Constructor for Sudden Death notifier container

     @param aNotifier The object to be notified of death
     */
    {
    }

void CNotifySuddenDeathContainer::Dequeue()
	/** Dequeue

	 */
    {
    iLink.Deque();
    }

void CNotifySuddenDeathContainer::Notify(TInt aReason)
	/** Notify of SuddenDeath

	 @param aReason The reason to associate with the Sudden Death event
	 */
    {
    iNotifier->SuddenDeath(aReason);
    }

CModuleUndertaker* CModuleUndertaker::NewL(MNotifySuddenDeath& aNotifier, RThread& aThread)
    /** Creates new CModuleUndertaker object.
     @param aNotifier The object to be notified of death.
     @param aThread The thread to be monitored.
     @return Pointer to the undertaker object.
     */
    {
    CModuleUndertaker* pUndertaker = new (ELeave) CModuleUndertaker(aNotifier, aThread);
    CActiveScheduler::Add(pUndertaker);
    return pUndertaker;
    }

void CModuleUndertaker::Logon()
    /** Logs on to thread.
     Thread logon to monitor for death
     */
    {
	iThread.Logon(iStatus);
    SetActive();
    }

void CModuleUndertaker::DoCancel()
    {
	(void)iThread.LogonCancel(iStatus);
    }

void CModuleUndertaker::RunL()
    /** Calls sudden death function on object registered for notification
     */
    {
    LogThreadDeath();

    TExitType type = iThread.ExitType();
	TInt reason = iThread.ExitReason();

	// Decide if the thread exit was expected or caused by an error.
	if((iThread.ExitType() != EExitKill) || (reason != KErrNone))
		{
		// Looks like a panic or a forced kill/terminate happened to it.
		reason = KErrRSSuddenViolentDeath;
		}
	else
		{
		reason = KErrRSSuddenDeath;
		}
		
	// Call the notifier.
    iNotifier.SuddenDeath(reason);
    }

void CModuleUndertaker::LogThreadDeath()
	{
	#if defined _DEBUG || defined __CFLOG_ACTIVE || defined SYMBIAN_TRACE_ENABLE

		    RThread& deadThread = iThread;

	    TExitType type = deadThread.ExitType();
		TInt reason = deadThread.ExitReason();

	    TName processName = RProcess().Name();
	    TPtrC processDisplayName = processName;
	    TInt pos = processName.Locate(L'[');
	    if(pos > 0)
	    	{
	    	// Strip off the UID and instance number.
	    	processDisplayName.Set(processName.Left(pos));
	    	}
	    TBuf8<KMaxName> processName8;
	    processName8.Copy(processDisplayName);

	    TName threadName = deadThread.Name();
	    TBuf8<KMaxName> threadName8;
	    threadName8.Copy(threadName);

	    TBuf8<KMaxExitCategoryName> cat8;
	    cat8.Copy(deadThread.ExitCategory());
		
		if(type == EExitPending)
			{
			__CFLOG_1(KLogSubSysRS, KLogCode,
				_L8("ERROR: CModuleUndertaker::RunL - the death of thread %S was reported but the exit type indicates that the thread is still running.  The rootserver is probably in an unstable state."),
				&threadName8);
				
			__CF_BREAK_IF_DEBUGGER();
			}
		else
			{
			_LIT8(KExitNormalStr, "(Normal)");
			_LIT8(KExitKillStr, "(Kill)");
			_LIT8(KExitTerminateStr, "(Terminate)");
			_LIT8(KExitPanicStr, "Panic:");

			_LIT8(KKillNormalInfoFormatStr, "%S::%S Thread Died %S - Category: \"%S\", Reason: %d, Thread ID: %x, BTrace ID: 0x%x");
			_LIT8(KKillCancelInfoFormatStr, "%S::%S Thread Died %S - Category: \"%S\", Reason: %d, Thread ID: %x, BTrace ID: 0x%x");
			_LIT8(KKillErrorInfoFormatStr, "ERROR: %S::%S Thread Died %S - Category: \"%S\", Reason: %d, Thread ID: %x, BTrace ID: 0x%x");
			_LIT8(KPanicInfoFormatStr, "ERROR: %S::%S Thread Died %S %S %d - Thread ID: %x, BTrace ID: 0x%x");
			
			TPtrC8 type8;
			TPtrC8 formatStr;

			// Determine the reason why the thread exited and try to log as much information
			// as possible if the exit was unexpected.
			switch(type)
				{
				case EExitKill:
					{
					switch(reason)
						{
						case KErrNone:
							type8.Set(KExitNormalStr);
							formatStr.Set(KKillNormalInfoFormatStr);
						break;
						case KErrCancel:
							type8.Set(KExitKillStr);
							formatStr.Set(KKillCancelInfoFormatStr);
						break;
						default:
							type8.Set(KExitKillStr);
							formatStr.Set(KKillErrorInfoFormatStr);
						break;
						}
					}
				break;
				case EExitTerminate:
					type8.Set(KExitTerminateStr);
					formatStr.Set(KKillErrorInfoFormatStr);
				break;
				case EExitPanic:
					type8.Set(KExitPanicStr);
					formatStr.Set(KPanicInfoFormatStr);
				break;
				default:
					__ASSERT_DEBUG(false, User::Panic(KSpecAssert_ElemRootServerrt, 1)); // should never get here
				break;
				}

			// Decide if the thread exit was expected or caused by an error.
			#ifdef _DEBUG
				if((deadThread.ExitType() != EExitKill) || ((reason != KErrNone) && (reason != KErrCancel)))
					{
					// Log unexpected thread exits to the debug port.
					RDebug::Printf(reinterpret_cast<const char*>(formatStr.Ptr()), &processName8, &threadName8, &type8, &cat8, reason, TUint(deadThread.Id().Id()), deadThread.BTraceId());
					}
			#endif
					
			// Log the thread's death to UTrace if available.
			#ifdef SYMBIAN_TRACE_ENABLE
			    enum
			        {
			        KPrimaryFilter = 194, // server den
			        KMaxLogTextLength = 250
			        };

			    class TLogIgnoreOverflow8 : public TDes8Overflow
			        {
			        public:
			            void Overflow(TDes8& /*aDes*/) { }
			        };

				// Format the log text into a buffer for UTrace.
			    TBuf8<KMaxLogTextLength> buf;
		    	TLogIgnoreOverflow8 overflowHandler;
			    buf.AppendFormat(formatStr, &overflowHandler, &processName8, &threadName8, &type8, &cat8, reason, TUint(deadThread.Id().Id()), deadThread.BTraceId());

				UTracePfAny(KPrimaryFilter, KText, ETrue, EFalse, buf.Length(), buf.Ptr(), buf.Length());
			#endif
			
			// Log the thread's death to CFlog if available.
			__CFLOG_VAR((KLogSubSysRS, KLogCode, formatStr, &processName8, &threadName8, &type8, &cat8, reason, TUint(deadThread.Id().Id()), deadThread.BTraceId()));
			}

	#endif
	}

CModuleUndertaker::CModuleUndertaker(MNotifySuddenDeath& aNotifier, RThread& aThread)
: CActive(EPriorityLow),	// NB! lower priority than that of the session handlers to avoid race between undertaker & load handler
  iNotifier(aNotifier),
  iThread(aThread)
    /** Constructor for ModuleUndertaker
     */
    {
    }

CModuleUndertaker::~CModuleUndertaker()
    /** Destructor for ModuleUndertaker
     */
    {
    Cancel();
    }


EXPORT_C CCommsProviderModule::CCommsProviderModule( RThread& aThread ) 
	: CActive(EPriorityStandard)
	, iState(EIdle)
	, iThread(aThread)
	, iNotifiers(_FOFF(CNotifySuddenDeathContainer, iLink))
// Surplus initialisers left as documentation of intent
//iUndertaker(NULL),
    {}

EXPORT_C CCommsProviderModule::~CCommsProviderModule()
    {
	__CFLOG_1(KLogSubSysRS, KLogCode,
			_L8("CCommsProviderModule::~CCommsProviderModule: %S"), &iName);

    iSend.Close();
    iRecv.Close();

	if(iHeapPtr)
		{
#if defined(SYMBIAN_C32ROOT_API_V3)
#if !defined(ESOCKV3_TEMPORARY_PAIN_RELIEF)
#ifdef _DEBUG
		// If modules sharing a heap are unloaded in anything but LIFO order then
		// they can't easily check for leaks (ie A marks heap, B loads, A unloads but
		// doesn't check heap because it sees that someone else is using it, B unloads
		// and doesn't check heap either. The real owner of the heap is the RootServer,
		// so here we check the count and log leaked cells if any remain after the last
		// user has unloaded.
		// In _DEBUG builds we also execute a debugger breakpoint when cells remain,
		// unless the module has been flagged as "unstable". A stable module sharing a
		// heap with an unstable one might see this breakpoint but in general a leaky
		// module should have a private heap to ensure cleanup.
		RCFSharedHeap& heap = static_cast<RCFSharedHeap&>(*iHeapPtr);
		if(heap.AccessCount() <= 2)	//@todo worry more about why it's two
			{
			if(heap.Count() > 0)
				{
				__CFLOG_VAR( (KLogSubSysRSModule, KLogCode, _L8("~CCommsProviderModule: module '%S' heap(%08x) has %d cells remaining:"), &iName, &heap, heap.Count()) );
				heap.LogAllocatedCells(KLogSubSysRSModule, KLogRSLeakTag);

				// The configuration flags suppressing the panic: modules known to be unstable are not checked. These
				// include all modules that have panicked (if they never had the chance to cleanup it isn't helpful
				// to list "leaks")
				const TUint32 noCheckCondition = TRSStartModuleParamContainer::KCF_UnstableModule;

				// If not waivered by optional flags, call __DEBUGGER()
				if( (iControlFlags & noCheckCondition)==0)
					{
					RProperty pubsub;
					TInt res = pubsub.Attach(iRootServer->iProcessKey, iRootServer->iLeakKey.iUid);	
					//Does not leave before close therefore no need for cleanup stack
					if (res == KErrNone)
						{
						TInt count;
						res =pubsub.Get(count);
						if (res == KErrNone)
							{
							++count;
							res =pubsub.Set(count);
							}
						}
					pubsub.Close();
					if (res != KErrNone)
						{
						__CFLOG_1(KLogSubSysRSModule, KLogRSLeakTag, _L8("Unable to report leaks. Error: %d"), res);
						}
					}
				// As much as anything this log line is here to make it apparent that the breakpoint above was hit
				// See the comment block above for more explanations.
				__CFLOG_VAR( (KLogSubSysRSModule, KLogRSLeakTag, _L8("--- end of leaked cell log")) );
				}
			}
#endif
#endif
#endif
		iHeapPtr->Close();
		}

	if(iRootServer->FindCpm(iName))
		{
		iLink.Deque();                     // Remove from list in Root Server
		__CFLOG_1(KLogSubSysRS, KLogCode,
		_L8("CCommsProviderModule::~CCommsProviderModule: removing %S from RS List"), &iName);
		}

    delete iUndertaker;

	// Create a timer in case some other entity holds an open handle on the
	// thread which prevents the kernel from destroying it.  We timeout
	// after one second.
	TAutoClose<RTimer> timer;
	if(timer.iObj.CreateLocal() == KErrNone)
		{
		// Ensure the thread has been destroyed by using a notification so that its unique name is available for
		// reuse in case the module is reloaded soon afterwards.
		TRequestStatus destructionStatus;
		iThread.NotifyDestruction(destructionStatus);
	    iThread.Close();
    
		enum{ KThreadDestructionTimeout = 1000000 };
  					
		TRequestStatus timerStatus;
		timer.iObj.After(timerStatus, KThreadDestructionTimeout);
	 					
		// Wait for the thread to be destroyed or for the timeout.
		User::WaitForRequest(destructionStatus, timerStatus);
		if(timerStatus.Int() == KRequestPending)
			{
			timer.iObj.Cancel();
			User::WaitForRequest(timerStatus);
			}
		else
			{
			User::CancelMiscNotifier(destructionStatus);
			User::WaitForRequest(destructionStatus);
			
			__CFLOG_2(KLogSubSysRS, KLogCode,
				_L8("CCommsProviderModule::~CCommsProviderModule: %S thread has still not been destroyed after %fs"), &iName, KThreadDestructionTimeout / 1000.0);
			}
		}
	else
		{
		iThread.Close();
		}

	// If a starting thread has been given the pointer then it's too dangerous to delete it here as might crash
	// them - better to risk a small leak
	if(iState != EStarting)
		{
		delete iThreadStartupInfo;
		}
#ifdef SYMBIAN_C32ROOT_API_V3
	delete iIniData;
#endif

	// Remove from CActiveScheduler
    if(IsAdded())
    	{
        Deque();
        }
	// Remove from RootServer list of modules
	if(iLink.iNext != NULL)
		{
		iLink.Deque();
		}
    }

TInt CCommsProviderModule::CancelLoad()
    /** Cancel Load. Marks whether an attempt to cancel load has happened
     *
     * @return	KErrCancel if cancelation of the load module operation was initiated
     *			or KErrRSModuleAlreadyExist if the module's thread has started
     *			running and can no longer be canceled.
     */	
    {
	__CFLOG_1(KLogSubSysRS, KLogCode,
			_L8("CCommsProviderModule - CancelLoad %S"), &iName);

	TInt result = KErrCancel;			

	// Check to see if the load module operation can be canceled or if the thread
	// is already in the process of starting.
	if((iState == EStarting) || (iState == ERunning))
		{
		__CFLOG_1(KLogSubSysRS, KLogCode,
				_L8("CCommsProviderModule - CancelLoad: %S module has already started running and can no longer be canceled"), &iName);

		result = KErrRSModuleAlreadyExist;
		}
	
	return result;
    }

TInt CCommsProviderModule::CancelUnload(TRSUnLoadType aType)
    /** Cancel unload. Marks whether an attempt to cancel an unload has happened
     *
     * @param	aType The type of unload that was in progress
     * @return	KErrCancel if cancelation of the unload module operation was initiated
     *			or KErrRSModuleNotLoaded if the module's thread has started dying and
     *			can no longer be canceled.
     */	    
	{
	#ifdef __CFLOG_ACTIVE
		TPtrC8 unloadType = GetUnloadTypeName(aType);
		__CFLOG_2(KLogSubSysRS, KLogCode,
				_L8("CCommsProviderModule - CancelUnload: %S module has started an %S unload"), &iName, &unloadType);
	#endif
			
	TInt result = KErrCancel;			

	// Check to see if the unload module operation can be canceled or if the thread
	// is already in the process of dying.
	if(aType == EUnGraceful)
		{
		__CFLOG_1(KLogSubSysRS, KLogCode,
				_L8("CCommsProviderModule - CancelUnload: %S module has already started an EUnGraceful unload and can no longer be canceled"), &iName);

		result = KErrRSModuleNotLoaded;
		}
	else
		{
	 	if(iState==EStopping)
			{
			iState=ERunning;
			}
		}
		
	return result;
    }

void CCommsProviderModule::CreateThreadL()
    /** Creates and sets up thread ready for initialisation
     */
    {
    iUndertaker = CModuleUndertaker::NewL(*this, iThread);

    TInt res = iLibrary.Load(iDll);
    __CFLOG_STMT(TBuf8<KMaxFileName> fnam;)
	__CFLOG_STMT(fnam.Copy(iDll);)


    if (res!=KErrNone && res!=KErrAlreadyExists)
		{
		__CFLOG_2(KLogSubSysRS, KLogCode,
			_L8("CModuleBase::CreateThreadL: Unable to load DLL %S: %d."),
			&fnam, res);

        User::Leave(res);
		}

	__CFLOG_1( KLogSubSysRS, KLogCode,
		_L8("CModuleBase::CreateThreadL: Loaded module library %S."),
		&fnam);

    TUidType type = iLibrary.Type();
    if ( ! UidMatch(type[1]) )
		{
		__CFLOG_1(KLogSubSysRS, KLogCode,_L8("CModuleBase - UID mismatch for %S"), &fnam);
        User::Leave(KErrRSInvalidUidType);
		}

	// If ordinal is set to 0, default to 1 (first)
	if(iThreadFunctionOrdinal==0)
		{
		__CFLOG(KLogSubSysRS, KLogCode, _L8("CModuleBase - Using default ordinal: 1"));
		iThreadFunctionOrdinal = 1;
		}
	__CFLOG_STMT(
	else
		{
		__CFLOG_1(KLogSubSysRS, KLogCode,_L8("CModuleBase - Using configured ordinal: %d"), iThreadFunctionOrdinal);
		}
		); // __CFLOG_STMT

    TThreadFunction libEntry=(TThreadFunction)iLibrary.Lookup(iThreadFunctionOrdinal);

    if (libEntry==NULL)
    	{
        User::Leave(KErrBadLibraryEntryPoint);
        }

    RCFChannel::TMsgQueues inputQueue;
	RCFChannel::TMsgQueues outputQueue;

	iSend.GetMsgQueues(inputQueue);
	iRecv.GetMsgQueues(outputQueue);

	iThreadStartupInfo = new(ELeave) CRSTransientModuleInfo(outputQueue, inputQueue, iHeapType, libEntry, iIniData, iPriority, iControlFlags);
#ifdef SYMBIAN_C32ROOT_API_V3
	iIniData = NULL;	// iThreadStartupInfo now has ownership
#endif

	switch(iHeapType)
		{
		case EShareHeap:
			iHeapPtr = iRootServer->FindHeap(iShareHeapWith);
			if(iHeapPtr)
				{
				if(KErrNone!=iHeapPtr->Open())
					{
					__CFLOG(KLogSubSysRS, KLogWarning,
						_L8("CCommsProviderModule::CreateThreadL - Couldn't open shared heap!"));
					User::Leave(KErrRSUnableToOpenHeap); // Found the heap but couldn't open it
					}
				__CFLOG_1(KLogSubSysRS, KLogCode,
					_L8("CCommsProviderModule::CreateThreadL: Shared heap address %x"), iHeapPtr);
				}
			else
				{
				__CFLOG(KLogSubSysRS, KLogWarning,
						_L8("CCommsProviderModule - Couldn't find correct heap!"));
				User::Leave(KErrRSUnableToFindHeap); // Couldn't find the required heap!
				}
			break;
		case ENewHeap:
			iHeapPtr = User::ChunkHeap(NULL,iMinHeapSize,iMaxHeapSize);
			if(!iHeapPtr)
				{
				__CFLOG(KLogSubSysRS, KLogWarning,
						_L8("CCommsProviderModule::CreateThreadL:Unable to create new heap"));
				User::Leave(KErrRSNoNewHeapsAvailable);
				}
			else
				{
				__CFLOG_1(KLogSubSysRS, KLogCode, _L8("CCommsProviderModule::CreateThreadL: New heap address %x"), iHeapPtr);
				}
			break;
		case EDefaultHeap:
			__CFLOG(KLogSubSysRS, KLogCode, _L8("CCommsProviderModule::CreateThreadL: Using Default Heap."));
			break;	// iHeapPtr is left NULL
		default:
			__ASSERT_DEBUG(0, User::Panic(KSpecAssert_ElemRootServerrt, 2));
		}

	TName threadName;
	threadName.Copy(iName);
	res=CreateModuleThread(threadName, CRSTransientModuleInfo::ModuleThreadStartup,
									iStackSize, iHeapPtr, iThreadStartupInfo);
#if defined(SYMBIAN_C32ROOT_API_V3)
#if !defined(ESOCKV3_TEMPORARY_PAIN_RELIEF)
#if defined(C32ROOT_ALLOW_SUBSTITUTE_THREAD_NAMES)
	// Reloading a module that has leaked a named resource will cause an already-exists
	// error because the old thread hasn't been fully cleaned up. It can be helpful to get past
	// this point in a debug build since if the resource in question is named it may later fail
	// in a more specific fashion
	if (res == KErrAlreadyExists)
		{
	// If not waivered by optional flags, call __DEBUGGER()
	if( (iControlFlags & TRSStartModuleParamContainer::KCF_UnstableModule)==0)
		{
		__DEBUGGER();
		}
		TInt suffix = 0;
		RProperty::Get(KUidSystemCategory, iRootServer->iDeathKey.iUid, suffix);
		do
			{
			threadName.Copy(iName);	// expand to Unicode
			_LIT(KSubstituteNameFormat, "_%d");
			threadName.AppendFormat(KSubstituteNameFormat, ++suffix);
			__CFLOG_VAR( (KLogSubSysRS, KLogCode,
				_L8("CCommsProviderModule::CreateThreadL - KErrAlreadyExists; trying with %S"), &threadName) );
			res=CreateModuleThread(threadName, CRSTransientModuleInfo::ModuleThreadStartup,
											iStackSize, iHeapPtr, iThreadStartupInfo);
			} while(res == KErrAlreadyExists);
		}
#endif
#endif
#endif
    if (res!=KErrNone)
		{
		__CFLOG_2(KLogSubSysRS, KLogCode,
		_L8("CCommsProviderModule::CreateThreadL:Unable to create thread %S error %d.."),
		&iName, res);
		User::Leave(res);
		}

	__CFLOG_1(KLogSubSysRS, KLogCode,
			_L8("CCommsProviderModule::CreateThreadL: Created thread for %S."), &iName);

    CActiveScheduler::Add(this);
    }

EXPORT_C void CCommsProviderModule::ConstructL(CRootServer* aRootServer,
					const TRSStartModuleParams& aParams, HBufC8* aIniData)
    /**
     * Contruct module. Sets up module, creating comms channels and thread.
	 @param aRootServer The root server object
	 @param aParams The data needed to load module
	 @param aIniData The initialisation data for the module
     */
    {
    __CFLOG_STMT(TBuf8<KMaxFileName> fnam;)
	__CFLOG_STMT(fnam.Copy(aParams.iParams.iFilename);)
    __CFLOG_4(KLogSubSysRS, KLogCode,
    			_L8("CCommsProviderModule::ConstructL:%S,%S Pri:%d Stack:%d."),
    			&aParams.iParams.iName, &fnam, aParams.iParams.iPriority, aParams.iParams.iStackSize);

    // Copy the essential information
    iName.Copy(aParams.iParams.iName);
    iDll.Copy(aParams.iParams.iFilename);
    iPriority = aParams.iParams.iPriority;
    iRootServer = aRootServer;
    iIniData = aIniData;
    iStackSize = aParams.iParams.iStackSize;
   	iHeapType = aParams.iParams.iHeapType;
    iMinHeapSize = aParams.iParams.iMinHeapSize;
    iMaxHeapSize = aParams.iParams.iMaxHeapSize;
    iShareHeapWith = static_cast<const TCFModuleNameF&>(aParams.iParams.iShareHeapWith);
	iThreadFunctionOrdinal = aParams.iParams.iThreadFunctionOrdinal;
	iIsSticky = aParams.iParams.iIsSticky;
	iIsServer = aParams.iParams.iIsServer;
	iControlFlags = aParams.iParams.iControlFlags;

	if(KErrNone!=iSend.Create(KCFMaxChannelQueues))
		{
		User::Leave(KErrRSUnableToCreateQueues);
		}

	if(KErrNone!=iRecv.Create(KCFMaxChannelQueues))
		{
		User::Leave(KErrRSUnableToCreateQueues);
		}
    }

TInt CCommsProviderModule::Load(TRequestStatus& aStatus)
    /** Initiates the loading of a module. Sets states and resumes thread
	 @param aStatus The status of the active object which will be completed on load
     */
    {
    iState = EStarting;
    iReqStatus = &aStatus;
    aStatus = KRequestPending;

	iThread.Rendezvous(iStatus);
	iThread.Resume();
	SetActive();

	__CFLOG_1(KLogSubSysRS, KLogCode,
			_L8("CCommsProviderModule::Load: Resumed thread %S."), &iName);

    return KErrNone;
    }

TInt CCommsProviderModule::Unload()
    /** Initiates the unloading of a module. Simply sets internal state
     */
    {

	// We now attempt to send shutdowns to zombies - even though they failed to respond to
	// some earlier request they may notice a shutdown, and it's much better that they shutdown
	// politely than that an UnGraceful thread kill happens
    if ( iState != ERunning && iState != EZombie )
    	{
		return KErrRSModuleNotRunning;
		}

	__CFLOG_1(KLogSubSysRS, KLogCode,
			_L8("CCommsProviderModule::UnLoading %S."), &iName);

	iState = EStopping;

	return KErrNone;
    }

TBool CCommsProviderModule::UidMatch(const TUid& aUid) const
   /** Matches the Uid for a CPM
	 @param aUid The Uid to be checked
     */
    {
    return ((aUid.iUid==KUidCommsServerModule)||(aUid.iUid==KUidCommsProviderModule));
    }

RThread CCommsProviderModule::GetThread() const
   /** Return pointer to thread for a CPM
     */
    {
	return iThread;
    }

#ifdef __CFLOG_ACTIVE
const TText8* CCommsProviderModule::GetStateName() const
	/** Returns a pointer to a text string identifying the module's current state
	 */
	{
	switch(iState)
		{
		case EIdle:
			return _S8("Idle");
		case EStarting:
			return _S8("Starting");
		case ERunning:
			return _S8("Running");
		case EStopping:
			return _S8("Stopping");
		case EZombie:
			return _S8("Zombie");
		case EDead:
			return _S8("Dead");
		default:
			return _S8("<Unknown State>");
		}
	}
#endif

#if defined _DEBUG || defined __CFLOG_ACTIVE || defined SYMBIAN_TRACE_ENABLE
const TText8* CCommsProviderModule::GetUnloadTypeName(RootServer::TRSUnLoadType aType)
	/** Returns a pointer to a text string identifying the requested module unload type
	 */
	{
	// Determine the unload type.
	switch(aType)
		{
		case EOptional:
			return _S8("EOptional");
		case EGraceful:
			return _S8("EGraceful");
		case EImmediate:
			return _S8("EImmediate");
		case EUnGraceful:
			return _S8("EUnGraceful");
		default:
			return _S8("<Unknown Module Unload Type>");
		}
	}
#endif

void CCommsProviderModule::Inhume()
	{
	iState = EZombie;
	}


void CCommsProviderModule::RegisterSuddenDeathNotifierL(MNotifySuddenDeath* aNotifier)
    /** Register a sudden death notifier.
     Instantiates a container with the pointer to the notifier and
     puts it in the TblQue list.
     @param aNotifier The notifier to be registered.
     */
    {
    __CFLOG(KLogSubSysRS, KLogCode, _L8("CCommsProviderModule - RegisterSuddenDeathNotifier"));
    CNotifySuddenDeathContainer* pContainer = new (ELeave) CNotifySuddenDeathContainer(aNotifier);
    iNotifiers.AddLast(*pContainer);
    }		//lint !e429	// custody taken of pContainer

void CCommsProviderModule::UnregisterSuddenDeathNotifier(MNotifySuddenDeath* aNotifier)
    /** Unregister a sudden death notifier.
     Searches for container with a pointer to notifier being unregistered, and if found deletes
	 unlinks the container and deletes it. Failure to find such a notifier is not an error as this
	 is typically a clean-up behaviour.
     @param aNotifier The notifier to be unregistered.
     */
	{
    TDblQueIter<CNotifySuddenDeathContainer> notifierIter(iNotifiers);
    CNotifySuddenDeathContainer *pContainer;

    while((pContainer = notifierIter++)!=NULL)
        {
        if(pContainer->Match(aNotifier))
			{
			pContainer->Dequeue();
			delete pContainer;
			}
        }
    }

EXPORT_C void CCommsProviderModule::SuddenDeath(TInt aDeathTypeErr)
    /** Controls events on death of a module
     Notifies Bind Manager of death event, so that it can clean up
	 aReason The reason associated with the death event
	 @param aDeathTypeErr xxx
     */
    {
    #ifdef __CFLOG_ACTIVE
	    TPtrC8 stateName(GetStateName());
	    __CFLOG_2(KLogSubSysRS, KLogCode,
	    			_L8("CCommsProviderModule::SuddenDeath - %S module's thread died while module was in state: %S."), &iName, &stateName);
	#endif

	iExitReason = iThread.ExitReason();
	if(aDeathTypeErr == KErrRSSuddenViolentDeath)
		{
		// Looks like a panic or a kill happened to it. Flag it as unstable to suppress the leak detection, since
		// it never had a chance to clean-up
		SetControlFlags(ControlFlags() | TRSStartModuleParamContainer::KCF_UnstableModule);
		}
    TInt result(KErrNone);
	__CFLOG(KLogSubSysRS, KLogCode,
				_L8("CCommsProviderModule::SuddenDeath - BM::UnbindAlreadyDeadL called"));
	TRAP(result, iRootServer->BindManager()->UnbindAlreadyDeadL(iStatus, iName));
	SetActive();
	if( result != KErrNone )
		{
		TRequestStatus *status = &iStatus;
		User::RequestComplete(status, result);
		}
	if ( iState != EStopping )
		{
		iState = EZombie;
		}
    }


void CCommsProviderModule::NotifyOfDeath(TInt aReason)
    /** Controls notifications on death of a module
	 @param aReason The reason associated with the death event
     */
	{

    TDblQueIter<CNotifySuddenDeathContainer> notifierIter(iNotifiers);
    CNotifySuddenDeathContainer *pContainer;

    while((pContainer = notifierIter++)!=NULL)
        {
        pContainer->Notify(aReason);
        pContainer->Dequeue();
        delete pContainer;
        }
    iNotifiers.Reset();
    }

EXPORT_C void CCommsProviderModule::RunL()
    /** Handles each active object completion event differently
	 depending on the state of the module
     */
    {
    #ifdef __CFLOG_ACTIVE
	    TPtrC8 stateName(GetStateName());
	    __CFLOG_3(KLogSubSysRS, KLogCode,
	    	_L8("CCommsProviderModule::RunL name: %S, state: %S, status: %d"),
	    	&iName, &stateName, iStatus.Int());
	#endif
	
    TBool dieNow = EFalse;
    switch(iState)
        {
        case EZombie:
            dieNow = ETrue;
			iRootServer->SuddenDeath(KErrRSSuddenDeath); // Notify Root Server
			__CFLOG_1(KLogSubSysRS, KLogCode,
				_L8("CCommsProviderModule::ModuleEndedL(%S) called"), &iName);
			TRAPD(res, iRootServer->BindManager()->ModuleEndedL(iName));	// no use checking if this left...module is dead
			iLink.Deque();                     // Remove from list in Root Server
			NotifyOfDeath(KErrRSSuddenDeath);
			break;
        case EIdle:
            break;
        case EStarting:
			delete iThreadStartupInfo;	// clean up startupinfo now, seeing as thread has rendezvoused
			iThreadStartupInfo = NULL;
            if (iStatus.Int()==KErrNone)
                {
				__CFLOG_1(KLogSubSysRS, KLogCode, 
					_L8("CCommsProviderModule::RunL - %S moving from Starting state to Running state"), &iName);
                iState = ERunning;
                iUndertaker->Logon(); // Monitor for thread death
                }
            else
                {
				__CFLOG_2(KLogSubSysRS, KLogCode, 
					_L8("CCommsProviderModule::RunL - %S dying now due to error %d"), &iName, iStatus.Int());
                dieNow = ETrue;
				NotifyOfDeath(KErrRSSuddenDeath);
                }

            User::RequestComplete(iReqStatus, iStatus.Int());	// trigger session handler
			break;
        case ERunning:
            break;
        case EStopping:
			__CFLOG_1(KLogSubSysRS, KLogCode, 
				_L8("CCommsProviderModule::RunL - %S dying now"), &iName);
			dieNow = ETrue;
			__CFLOG_1(KLogSubSysRS, KLogCode,
				_L8("CCommsProviderModule::RunL - ModuleEndedL(%S)"),&iName);
			TRAP(res, iRootServer->BindManager()->ModuleEndedL(iName));
			__ASSERT_DEBUG(res==KErrNone, User::Panic(KSpecAssert_ElemRootServerrt, 3));
			iLink.Deque();                     // Remove from list in Root Server
			iRootServer->SuddenDeath(KErrNone);
			NotifyOfDeath(iExitReason);
			break;
        case EDead:
        	dieNow = ETrue;
        	break;
        default:
        	__CFLOG_1(KLogSubSysRS, KLogCode, _L8("CCommsProviderModule::RunL() unexpected case <%d>!"), iState);
        }

    if (dieNow)
        {
		__CFLOG_1(KLogSubSysRS, KLogCode, 
			_L8("CCommsProviderModule::RunL - %S deleting self"), &iName);
       	if (IsAdded())
       		{
        	Deque();      // Remove from active scheduler
        	}
        iLibrary.Close(); //
        delete this;  // Disappear
        }
    }


CSessionHandler::CSessionHandler(CRootServerSession* aSession, CBindManager* aBindMgr, const RMessage2& aMessage):
								CActive(EPriorityStandard), iRequestCompleted(EFalse),
								iSession(aSession), iBindMgr(aBindMgr), iState(ESHIdle), iMessage(aMessage)
   /** Abstract base class to handle session requests
	 @param aSession The root server session
	 @param aBindMgr Pointer to the bind manager
	 @param aMessage The message which will be completed
     */
	{
	RS_DETLOG((KLogSubSysRS, KLogCode, _L8("CSessionHandler(%X) - msgHdl = %x"), this, iMessage.Handle()));
    CActiveScheduler::Add(this);
	}

CSessionHandler::~CSessionHandler()
    {
// Although sometimes useful this logging statement has to be left suppressed because sessions can be
// deleted by the base CServer2 dtor, ie after CFLOG has been deleted by the RootServer dtor.
//	RS_DETLOG((KLogSubSysRS, KLogCode, _L8("~CSessionHandler(%X)"), this));
    Cancel();
    if (IsAdded())
    	{
        Deque();
        }
    iLink.Deque();
    }

void CSessionHandler::SuddenDeath(TInt aError)
    /** Only called if not over-ridden by derived class, but this
     should never be the case.
	 @param aError The reason associated with the death event
     */
	{
	(void) aError;	// suppress error when not logging
	__CFLOG_VAR((KLogSubSysRS, KLogCode, _L8("CSessionHandler::SuddenDeath(%d)"), aError));
	User::Panic(RThread().Name(), KRSBadState);	// derived class should override this for cases where it can legiarise
	}

CCommsProviderModule* CSessionHandler::GetModulePtr(const TCFModuleNameF& aName) const
    /** Retrieves module pointer
	 @param aName The name to retrieve module pointer for
	 @return Pointer to module
     */
	{
	CRootServer* rootServer = iSession->RootServer();
	return rootServer->FindCpm( aName );
	}

void CSessionHandler::CompleteClientRequest(TInt aRequest)
    /** Complete Request
	 @param aRequest The reason associated with the completion event
     */
    {
	if (!iRequestCompleted)
		{
		RS_DETLOG((KLogSubSysRS, KLogCode, _L8("CSessionHandler(%X)::CompleteClientRequest(hdl %x = %d)"), this, iMessage.Handle(), aRequest));
		iMessage.Complete(aRequest);
		iRequestCompleted = ETrue;
		}
	}

CSessionHandlerLoadCPM::CSessionHandlerLoadCPM(CRootServerSession* aSession, CBindManager* aBindMgr, const RMessage2& aMessage)
										: CSessionHandler(aSession, aBindMgr, aMessage)
    /** Constructor for Derived session handler
	 @param aSession The root server session
	 @param aBindMgr Pointer to Bind Manager
     */
	{
	}

CSessionHandlerLoadCPM::~CSessionHandlerLoadCPM()
    /** Destructor for Derived session handler
     */
	{
	delete iLoadCompletionNotificationWaiter;
	}
	
CSessionHandlerLoadCPM* CSessionHandlerLoadCPM::NewL(CRootServerSession* aSession, CBindManager* aBindMgr, const RMessage2& aMessage)
    /** Create New instance of this session handler
	 @param aSession The root server session
	 @param aBindMgr Pointer to Bind Manager
     */
	{
	CSessionHandlerLoadCPM* pSession = new(ELeave) CSessionHandlerLoadCPM(aSession, aBindMgr, aMessage);
	CleanupStack::PushL(pSession);
	pSession->ConstructL();
	CleanupStack::Pop();
	return pSession;
	}

CSessionHandler::TSHType CSessionHandlerLoadCPM::HandlerType()
    /** Identifies the subclass type
     */
	{
	return ESHTypeLoadCPM;
	}

TInt CSessionHandlerLoadCPM::Start()
    /** Starts the load process
     */
    {
	iState = ESHStarting;
	TInt res = KErrNotFound;
	CCommsProviderModule* modulePtr = GetModulePtr(iName);
	if(modulePtr)
		{
		res = modulePtr->Load(iStatus);
		SetActive();
		}
	return res;
	}


void CSessionHandlerLoadCPM::SetHandler(const TCFModuleNameF& aModule)
   /** Set up this handler
	 @param aModule The name of the module handled
     */
    {
	RS_DETLOG((KLogSubSysRS, KLogCode, _L8("CSessionHandlerLoadCPM - Setting up handler")));
	iName = aModule;
    }

TBool CSessionHandlerLoadCPM::CancelHandler(TBool aCompleteRequest, TBool aWaitForCompletionIfUncancelable)
    /** Cancel Handler attempts to cancel request
	 @param aCompleteRequest Whether to complete request or not
 	 @param aWaitForCompletionIfUncancelable Whether to wait for completion in a nested active scheduler (option used during session cleanup)
 	 @return Returns ETrue if the handler should be deleted by the caller or EFalse if the handler will delete itself
    */
    {
    // Pretend message has already been completed if
    // we are asked not to and it hasn't already
    if ((!aCompleteRequest) && (!iRequestCompleted))
    	{
        iRequestCompleted = ETrue;
        }

	TBool prevIsActive = IsActive();        
	TInt prevStatus = iStatus.Int();

	Cancel();
	
	// If the module's thread is already starting, reactivate the load handler
	// because it is no longer possible to cancel the load operation and we
	// need to let the load handler finish.
	if(iStatus.Int() == KErrRSModuleAlreadyExist)
		{
		// We should not complete the request now if the cancel did not succeed
		// regardless of the value of the aCompleteRequest parameter: the
		// operation is still in progress.
		aCompleteRequest = EFalse;

		// Restore the previous state of our active object before cancellation was
		// attempted.
		iStatus = prevStatus;
		if(prevIsActive)
			{
			SetActive();
			}
		
		// Check to see if we should wait for the uncancelable operation to
		// complete.  This is necessary if the session that initiated the operation
		// is closing or else the handler will be orphaned and the completion signal
		// will go astray.
		if(aWaitForCompletionIfUncancelable)
			{
			__ASSERT_DEBUG(iLoadCompletionNotificationWaiter, User::Panic(KSpecAssert_ElemRootServerrt, 4));
			
			// Start a nested scheduler to process the signals required to complete
			// the operation.  Not the neatest solution, but the least invasive.
			// Otherwise CloseSession would be have to be broken into steps and
			// restructured around an active object.
			// CSessionHandlerLoadCPM::Cancel's semantics are arguably broken anyway
			// since it is not guaranteed to work and doesn't wait for completion of
			// the load module operation if it fails.
			CActiveSchedulerWait* waiter = iLoadCompletionNotificationWaiter;
			waiter->Start();
				
			delete waiter;
			}
		
		return EFalse;
		}

	// Must complete the client AFTER we have canceled all unload operations.
	if (aCompleteRequest)
		{
		CompleteClientRequest(iStatus.Int());
		}
		
	return ETrue;
	}


CSessionHandler* CSessionHandlerLoadCPM::MatchPending(const TCFModuleNameF& aModule)
    /** Match a pending load
	 @param aModule The name of the loading module to be found
     */
    {
	if ( aModule != iName )
		{
		return NULL;
		}
	else
		{
		__CFLOG(KLogSubSysRS, KLogCode, _L8("CSessionHandlerLoadCPM - Found match"));
		return this;
		}
	}
	
void CSessionHandlerLoadCPM::ConstructL()
	{
	// We preallocate this in case we need it since it is used in
	// certain session cleanup situations.
	iLoadCompletionNotificationWaiter = new(ELeave) CActiveSchedulerWait;
	}

void CSessionHandlerLoadCPM::CompleteClientRequest(TInt aRequest)
    /** Complete Request
	 @param aRequest The reason associated with the completion event
     */	
    {
	if (!iRequestCompleted)
		{
		RS_DETLOG((KLogSubSysRS, KLogCode, _L8("CSessionHandlerLoadCPM(%X)::CompleteClientRequest(hdl %x = %d)"), this, iMessage.Handle(), aRequest));
		iMessage.Complete(aRequest);
		iRequestCompleted = ETrue;
		}

	// Complete a caller waiting in a nested scheduler for us to complete.
	// This is necessary to handle the corner case where a load module
	// operation was not cancelled in time and a session needs to wait for
	// the handler to complete before the session is closed.
	if(iLoadCompletionNotificationWaiter && iLoadCompletionNotificationWaiter->IsStarted())
		{
		iLoadCompletionNotificationWaiter->AsyncStop();
		
		// Waiter is now responsible for deletion for safety - we will
		// probably delete ourselves directly after this function call.
		iLoadCompletionNotificationWaiter = NULL;
		}
	}

void CSessionHandlerLoadCPM::RunL()
     /** Handles different states of handler as load progresses
     */
    {
    __CFLOG_1(KLogSubSysRS, KLogCode,
    		_L8("CSessionHandlerLoadCPM::RunL status %d"), iStatus.Int());
	CCommsProviderModule* modulePtr = GetModulePtr(iName);

	switch(iState)
		{
		case ESHStarting:
			// We get here when the module's thread Rendezvous()s
			if (iStatus.Int() == KErrNone)
				{
				iState = ESHLoading;
				if(modulePtr)
					{
					RCFChannel::TMsgQueues inputQueue;
					RCFChannel::TMsgQueues outputQueue;
					modulePtr->GetSendChannel().GetMsgQueues(inputQueue);
					modulePtr->GetRecvChannel().GetMsgQueues(outputQueue);
					__CFLOG_1( KLogSubSysRS, KLogCode,
					_L8("CSessionHandlerLoadCPM called BM::CreateModuleL(%S)" ),&iName);
					TRAPD(res, iBindMgr->CreateModuleL(iStatus, modulePtr->GetName(),
								inputQueue, outputQueue));
					SetActive();
					if (res != KErrNone)
						{
						__CFLOG_2(KLogSubSysRS, KLogCode,
						_L8("CSessionHandlerLoadCPM::Load: Unable to register %S thread with BindMgr - will kill it: %d.."),
																				&iName, res);
						// This is invidious - the thread lives but the bindmgr can't register it
						TRequestStatus* status = &iStatus;
						User::RequestComplete(status, res);
						delete modulePtr;
						}
					}
				}
			else
				{
				// if rendezvous with !KErrNone, just complete, cos object is deleted
				__CFLOG(KLogSubSysRS, KLogCode,_L8("CSessionHandlerLoadCPM::RunL bad rendezvous"));
				CompleteClientRequest(iStatus.Int());
				delete this;
				}
			break;
		case ESHLoading:
			if ( iStatus.Int() == KErrNone )
				{
				__CFLOG_1(KLogSubSysRS, KLogCode,
					_L8("CSessionHandlerLoadCPM::RunL %S loaded sucessfully"),
					&iName);
				CompleteClientRequest( iStatus.Int());
				delete this;
				}
			else
				{
				// load failed so we want to kill our thread!
				__CFLOG_1(KLogSubSysRS, KLogCode,
					_L8("CSessionHandlerLoadCPM::RunL %S load failed"),
					&iName);
				if(modulePtr)
					{
					// the thread has failed discovery so the client must
					// decide whether it wants to kill the module as the rootserver
					// now has a case of the living dead...
					modulePtr->Inhume();
					CompleteClientRequest(KErrRSZombie);
					delete this;
					}
				else
					{
					// there is no valid module representation in the rootserver
					// so this module must have had a sudden death after rendezvous
					// and did not complete loading fully
					CompleteClientRequest(KErrServerTerminated);
					delete this;
					}
				}
			break;
		case ESHCompleting:
		case ESHKilling:
		case ESHIdle:
		case ESHBinding:
		case ESHUnBinding:
		case ESHUnLoading:
		default:
			User::Panic(RThread().Name(), KRSBadState);
		}
    }

void CSessionHandlerLoadCPM::DoCancel()
    /** Attempt to cancel request
     */
    {
    CCommsProviderModule* modulePtr = GetModulePtr(iName);
	__ASSERT_DEBUG(modulePtr!=NULL, User::Panic(RThread().Name(), KRSModuleRefMissing));
	
	TInt cancelLoadResult = KErrRSModuleUnknown;
	
	if (modulePtr)
		{
		cancelLoadResult = modulePtr->CancelLoad();
		}
		
	if((cancelLoadResult == KErrCancel) || (cancelLoadResult == KErrRSModuleUnknown))
		{
		if(iBindMgr->Cancel(iStatus) != KErrNone)
			{
			TRequestStatus* pStatus = &iStatus;
			User::RequestComplete(pStatus, cancelLoadResult);
			}

		// Must complete the client AFTER we have canceled all load operations.
		CompleteClientRequest(KErrCancel);
		}
	else
		{
		TRequestStatus* pStatus = &iStatus;
		User::RequestComplete(pStatus, cancelLoadResult);
		}
	}
	
void CSessionHandlerLoadCPM::SuddenDeath(TInt aError)
    /** Completes message on death of a module
	 @param aError The reason associated with the death event
     */
    {
	__CFLOG(KLogSubSysRS, KLogCode,
		_L8("CSessionHandlerLoadCPM - Notified of SuddenDeath"));
	CompleteClientRequest(aError);
	delete this;

	}

CSessionHandlerUnLoadCPM::CSessionHandlerUnLoadCPM(CRootServerSession* aSession, CBindManager* aBindMgr, const RMessage2& aMessage)
										:CSessionHandler(aSession,aBindMgr,aMessage)
    /** Constructor for Derived session handler
	 @param aSession The root server session
	 @param aBindMgr Pointer to Bind Manager
     */
    {
	}

CSessionHandlerUnLoadCPM::~CSessionHandlerUnLoadCPM()
    /** Destructor for Derived session handler
     */
	{
	delete iLoadCompletionNotificationWaiter;
	}

CSessionHandlerUnLoadCPM* CSessionHandlerUnLoadCPM::NewL(CRootServerSession* aSession, CBindManager* aBindMgr, const RMessage2& aMessage)
    /** Create New instance of this session handler
	 @param aSession The root server session
	 @param aBindMgr Pointer to Bind Manager
     */
    {
	CSessionHandlerUnLoadCPM* pSession = new(ELeave) CSessionHandlerUnLoadCPM(aSession, aBindMgr,aMessage);
	CleanupStack::PushL(pSession);
	pSession->ConstructL();
	CleanupStack::Pop();
	return pSession;
	}

CSessionHandler::TSHType CSessionHandlerUnLoadCPM::HandlerType()
    /** Identifies the subclass type
     */
	{
	return ESHTypeUnLoadCPM;
	}

TInt CSessionHandlerUnLoadCPM::Start()
    /** Starts unload procedure
	 We have 2 types of unload: Graceful and Ungraceful.
	 Each unload works differently, but ultimately ends in the module
	 being removed
     */
    {
    RS_DETLOG((KLogSubSysRS, KLogCode, _L8("CSessionHandlerUnLoadCPM(%X)::Start(%S)"), this, &iName));

	CCommsProviderModule* modulePtr = GetModulePtr(iName);
	// we have already checked it is running, so if we can't find it now...
	__ASSERT_ALWAYS( modulePtr!=NULL, User::Panic(RThread().Name(), KRSModuleRefMissing));

	TInt result;
	switch(modulePtr->GetState())
		{
		case EStarting:
		case EStopping:
			// If it's an optional or graceful request then module needs to be in the running state
			if(iType == EUnGraceful)
				break;
			delete this;
			return KErrRSModuleNotRunning;
		case EDead:
			return KErrRSModuleNotRunning;
		default:
			// Reassure the tools that we're ignoring other values
			break;
		}

	switch(iType)
		{
		case EOptional:
		case EGraceful:
		case EImmediate:
			__CFLOG_1(KLogSubSysRS, KLogCode,
				_L8( "CSessionHandlerUnLoadCPM - BM::UnbindAllAndShutDownL(%S)"),
				&iName );
			// fire off the asynchronous request
			TRAP(result, iBindMgr->UnbindAllAndShutDownL(iStatus, iName, iType));
			SetActive();
			if (result == KErrNone)
				{
				result = modulePtr->Unload();
				// we have already checked module is running so...
				__CFLOG_1(KLogSubSysRS, KLogCode,
					_L8( "CSessionHandlerUnLoadCPM - error %d"), result);
				__ASSERT_DEBUG(result==KErrNone, User::Panic(RThread().Name(), KRSBindMgrError));
				iState = ESHUnLoading;
				}
			else
				{
				__CFLOG_1(KLogSubSysRS, KLogCode,
					_L8("CSessionHandlerUnLoadCPM - BM unable to unload %S gracefully"),
					&iName);
				TRequestStatus* status = &iStatus;
				User::RequestComplete(status, result);
				}
			break;
		case EUnGraceful:
			iState = ESHKilling;
			TRAPD( res, modulePtr->RegisterSuddenDeathNotifierL(this));
			if (res!=KErrNone)
				{
				CompleteClientRequest(res);
				delete this;
				}
			else
				{
				__CFLOG_1(KLogSubSysRS, KLogCode,
					_L8("CSessionHandlerUnLoadCPM - Unloading %S ungracefully"),
					&iName);
				modulePtr->GetThread().Kill(KErrCancel);
#ifdef SYMBIAN_C32ROOT_API_V3
				// Any module whose thread is terminated must be regarded as unstable: no
				// point in checking their cleanup when they had no chance to do any
				modulePtr->SetControlFlags(modulePtr->ControlFlags() | TRSStartModuleParamContainer::KCF_UnstableModule);
#endif
				}
			// this is all, cos we have already set in motion the events which
			// will call our RunL and complete the message and clean up
			break;
		default:
			User::Panic(RThread().Name(), KRSBadState);
		}
	return KErrNone;
	}

void CSessionHandlerUnLoadCPM::SetHandler(const TCFModuleNameF& aModule, TRSUnLoadType aType)
    /**
     @param aModule The name of the module handled
	 @param aType The type of unload - graceful/ungraceful
     */
    {
    #ifdef __CFLOG_ACTIVE
		TPtrC8 unloadType = CCommsProviderModule::GetUnloadTypeName(aType);
		__CFLOG_1(KLogSubSysRS, KLogCode, _L8("CSessionHandlerUnLoadCPM - Setting up handler for an %S unload"), &unloadType);
	#endif
	
	iName = aModule;
	iType = aType;
    }

TBool CSessionHandlerUnLoadCPM::CancelHandler(TBool aCompleteRequest, TBool aWaitForCompletionIfUncancelable)
    /** Cancel handler attempts to cancel request
	 @param aCompleteRequest Whether to complete request or not
 	 @param aWaitForCompletionIfUncancelable Whether to wait for completion in a nested active scheduler (option used during session cleanup)
 	 @return Returns ETrue if the handler should be deleted by the caller or EFalse if the handler will delete itself
     */   
    {
	// Although sometimes useful this logging statement has to be left suppressed because sessions can be
	// deleted by the base CServer2 dtor, ie after CFLOG has been deleted by the RootServer dtor.
	//__CFLOG(KLogSubSysRS, KLogCode, _L8("CSessionHandlerUnLoadCPM::CancelHandler"));

    // Pretend message has already been completed if
    // we are asked not to and it hasn't already
    if ((!aCompleteRequest) && (!iRequestCompleted))
		{
        iRequestCompleted = ETrue;
		}
		
	TBool prevIsActive = IsActive();        
	TInt prevStatus = iStatus.Int();

	// We cannot place this code inside DoCancel because the handler may not be waiting for any
	// operation to complete so DoCancel will not be called.
	TInt cancelUnloadResult = KErrRSModuleUnknown;
    CCommsProviderModule* module = GetModulePtr(iName);
	if (module)
		{
		cancelUnloadResult = module->CancelUnload(iType);
		
		if(cancelUnloadResult == KErrCancel)
			{
			module->UnregisterSuddenDeathNotifier(this);
			}
		}
	
	// If the module unload is still cancelable, cancel any bind operations and
	// complete the client.
	if((cancelUnloadResult == KErrCancel) || (cancelUnloadResult == KErrRSModuleUnknown))
		{
		// Cancel any bind operations.
		Cancel();
		
		// Must complete the client request AFTER canceling any bind operations.
		CompleteClientRequest(KErrCancel);
		}
				
	// Else, if the module's thread is already dying, reactivate the unload handler
	// because it is no longer possible to cancel the unload operation and we need
	// to let the unload handler finish.
	else if(cancelUnloadResult == KErrRSModuleNotLoaded)
		{
		// We should not complete the request now if the cancel did not succeed
		// regardless of the value of the aCompleteRequest parameter: the
		// operation is still in progress.
		aCompleteRequest = EFalse;

		// Restore the previous state of our active object before cancellation was
		// attempted.
		iStatus = prevStatus;
		if(prevIsActive)
			{
			SetActive();
			}
		
		// Check to see if we should wait for the uncancelable operation to
		// complete.  This is necessary if the session that initiated the operation
		// is closing or else the handler will be orphaned and the completion signal
		// will go astray.
		if(aWaitForCompletionIfUncancelable)
			{
			__ASSERT_DEBUG(iLoadCompletionNotificationWaiter, User::Panic(KSpecAssert_ElemRootServerrt, 5));
			
			// Start a nested scheduler to process the signals required to complete
			// the operation.  Not the neatest solution, but the least invasive.
			// Otherwise CloseSession would be have to be broken into steps and
			// restructured around an active object.
			// CSessionHandlerUnLoadCPM::Cancel's semantics are arguably broken anyway
			// since it is not guaranteed to work and doesn't wait for completion of
			// the load module operation if it fails.
			CActiveSchedulerWait* waiter = iLoadCompletionNotificationWaiter;
			waiter->Start();
				
			delete waiter;
			}
			
		return EFalse;
		}
	
	// Must complete the client AFTER we have canceled all unload operations.
	if (aCompleteRequest)
		{
		CompleteClientRequest(iStatus.Int());
		}
		
	return ETrue;
	}


CSessionHandler* CSessionHandlerUnLoadCPM::MatchPending(const TCFModuleNameF& aModule)
    /** Match a pending load
	 @param aModule The name of the loading module to be found
     */
    {
	if (aModule != iName)
		{
		return NULL;
		}
	else
		{
		__CFLOG(KLogSubSysRS, KLogCode,_L8("CSessionHandlerUnLoadCPM - Found match"));
		return this;
		}
	}
	
void CSessionHandlerUnLoadCPM::ConstructL()
	{
	// We preallocate this in case we need it since it is used in
	// certain session cleanup situations.
	iLoadCompletionNotificationWaiter = new(ELeave) CActiveSchedulerWait;
	}

void CSessionHandlerUnLoadCPM::CompleteClientRequest(TInt aRequest)
    /** Complete Request
	 @param aRequest The reason associated with the completion event
     */	
    {
	if (!iRequestCompleted)
		{
		RS_DETLOG((KLogSubSysRS, KLogCode, _L8("CSessionHandlerUnLoadCPM(%X)::CompleteClientRequest(hdl %x = %d)"), this, iMessage.Handle(), aRequest));
		iMessage.Complete(aRequest);
		iRequestCompleted = ETrue;
		}

	// Complete a caller waiting in a nested scheduler for us to complete.
	// This is necessary to handle the corner case where an unload module
	// operation was not cancelled in time and a session needs to wait for
	// the handler to complete before the session is closed.
	if(iLoadCompletionNotificationWaiter && iLoadCompletionNotificationWaiter->IsStarted())
		{
		iLoadCompletionNotificationWaiter->AsyncStop();
		
		// Waiter is now responsible for deletion for safety - we will
		// probably delete ourselves directly after this function call.
		iLoadCompletionNotificationWaiter = NULL;
		}
	}

void CSessionHandlerUnLoadCPM::RunL()
    /** Deal with unloading as progresses through different states
     */
    {
	// first time we come in here will be on a successful
	// bindmgr request.  we only ignore as we will be back in here
	// after triggered from sudden death event
	__CFLOG_1(KLogSubSysRS, KLogCode,
			_L8("CSessionHandlerUnloadCPM::RunL status %d"), iStatus.Int());

   	CCommsProviderModule* module = GetModulePtr(iName);
	switch(iState)
		{
		case ESHUnLoading:
			if(iStatus.Int()==KErrNone)
				{
				iState = ESHKilling;
				// if we get a valid pointer then we are not too late to get a notification
				// of death from module.  If we are too late, then module is already dead
				// so we are happy and can complete message
				if (module)
					{
					TRAPD(res, module->RegisterSuddenDeathNotifierL(this));
					if(res!=KErrNone)
						{
						CompleteClientRequest(res);
						delete this;
						}
					else
						{
						__CFLOG(KLogSubSysRS, KLogCode,
							_L8("CSessionHandlerUnloadCPM::RunL now waiting for Sudden Death"));
						}
					}
				else
					{
					// module already dead
					CompleteClientRequest(iStatus.Int());
					delete this;
					}
				}
			else
				{
				// BM could not unload cleanly so return to client and let them decide
				if (module)
					{
					module->CancelUnload(iType);
					}
				CompleteClientRequest(iStatus.Int());
				delete this;
				}
			break;
		case ESHKilling:
			__CFLOG(KLogSubSysRS, KLogCode,_L8("CSessionHandlerUnloadCPM::RunL Successfully unloaded"));
			CompleteClientRequest(iStatus.Int());
			delete this;
			break;
		case ESHIdle:
			__CFLOG_1(KLogSubSysRS, KLogCode,_L8("CSessionHandlerUnloadCPM::RunL Unbind request failed (%d)"), iStatus.Int());
			CompleteClientRequest(iStatus.Int());
			delete this;
			break;
		default:
			User::Panic(RThread().Name(), KRSBadState);
		}
    }

void CSessionHandlerUnLoadCPM::DoCancel()
    /** Attempt to cancel request
     */
    {
	if(iBindMgr->Cancel(iStatus) != KErrNone)
		{
		TRequestStatus* pStatus = &iStatus;
		User::RequestComplete(pStatus, KErrCancel);
		}
	}

void CSessionHandlerUnLoadCPM::SuddenDeath(TInt aError)
    /** Completes message on death of a module
	 @param aError The reason associated with the death event
     */
    {
	__CFLOG(KLogSubSysRS, KLogCode, _L8("CSessionHandlerUnLoadCPM - Notified of SuddenDeath"));
	if(iType == EUnGraceful)
		{
		aError = KErrNone;	// client requested this sudden death, so make it look less surprising
		}
	CompleteClientRequest(aError);
	delete this;

	}

CSessionHandlerBinding::CSessionHandlerBinding(CRootServerSession* aSession, CBindManager* aBindMgr, const RMessage2& aMessage)
												: CSessionHandler(aSession, aBindMgr,aMessage)
    /** Constructor for Derived session handler
	 @param aSession The root server session
	 @param aBindMgr Pointer to Bind Manager
 	 @param aMessage The message which will be completed
     */
    {
	}

CSessionHandlerBinding* CSessionHandlerBinding::NewL(CRootServerSession* aSession, CBindManager* aBindMgr, const RMessage2& aMessage)
    /** Create New instance of this session handler
	 @param aSession The root server session
	 @param aBindMgr Pointer to Bind Manager
	 @param aMessage The message which will be completed
     */
    {
	CSessionHandlerBinding* pSession = new(ELeave) CSessionHandlerBinding(aSession, aBindMgr,aMessage);
	return pSession;
	}

CSessionHandler::TSHType CSessionHandlerBinding::HandlerType()
    /** Identifies the subclass type
     */
	{
	return ESHTypeBinding;
	}

TInt CSessionHandlerBinding::Start()
    /** Starts Bind request
     */
    {
	iState = ESHBinding;
	__CFLOG(KLogSubSysRS, KLogCode,_L8("CSessionHandlerBinding - Call BM::BindSubModulesL"));
	TRAPD(result, iBindMgr->BindSubmodulesL(iStatus, iBindType, iNameFrom, iNameTo, iForwardQLength, iReverseQLength));
	SetActive();
	if (result != KErrNone)
		{
		__CFLOG_1(KLogSubSysRS, KLogCode,
			_L8( "CSessionHandlerBinding - BM::BindSubmodulesL left with %d"), result);
		// trigger our runL to deal with aftermath of leave
		TRequestStatus* status = &iStatus;
		User::RequestComplete(status, result);
		}
	return KErrNone;
	}

CSessionHandler* CSessionHandlerBinding::MatchPending(const TCFSubModuleAddress& aFrom,
														 const TCFSubModuleAddress& aTo)
    /** Match a pending load
	 @param aFrom The name of one module involved
 	 @param aTo The name of the other module involved
     */
    {

	if ((iNameFrom == aFrom) && (iNameTo == aTo))
		{
		return this;
		}
	else
	    {
		return NULL;
		}
	}

void CSessionHandlerBinding::SetHandler(const TCFSubModuleAddress& aFrom,
				const TCFSubModuleAddress& aTo,	TRSBindType& aBindType,
				TInt aForwardQLength, TInt aReverseQLength)
    /** Set up this handler
	 @param aFrom The name of one module handled
	 @param aTo The name of the other module handled
	 @param aBindType The type of bind
	 @param aForwardQLength Number of slots in queue from->to
	 @param aReverseQLength Number of slots in queue to->from
     */
    {
	iNameFrom = aFrom;
	iNameTo = aTo;
	iBindType = aBindType;
	iForwardQLength = aForwardQLength;
	iReverseQLength = aReverseQLength;
	}

TBool CSessionHandlerBinding::CancelHandler(TBool aCompleteRequest, const TBool /*aWaitForCompletionIfUncancelable*/)
    /** Cancel Handler attempts to cancel request
	 @param aCompleteRequest Whether to complete request or not
 	 @param aWaitForCompletionIfUncancelable Whether to wait for completion in a nested active scheduler (option used during session cleanup)
 	 @return Returns ETrue if the handler should be deleted by the caller or EFalse if the handler will delete itself
     */
    {
    // Pretend message has already been completed if
    // we are asked not to complete it, and it hasn't already
    iState = ESHCompleting;

    if ((!aCompleteRequest) && (!iRequestCompleted))
    	{
        iRequestCompleted = ETrue;
        }

// Although sometimes useful this logging statement has to be left suppressed because sessions can be
// deleted by the base CServer2 dtor, ie after CFLOG has been deleted by the RootServer dtor.
//	__CFLOG(KLogSubSysRS, KLogCode,_L8("CSessionHandlerBinding - Cancel Handler"));

	Cancel();

	return ETrue;
	}

void CSessionHandlerBinding::RunL()
     /** Handles the aftermath of a bind request. Fills TRSBindingInfo struct
     with bind state for each sub-module involved
     */
    {
    __CFLOG_1( KLogSubSysRS, KLogCode,_L8("CSessionHandlerBinding::RunL status %d"),
    	iStatus.Int() );

	// Check completion code
	TInt res=iStatus.Int() ;
	if (res!=KErrNone)
		{
	    CompleteClientRequest(res);
    	delete this;
		return;
    	}

	// Get BindInfo structure from client, we might need to put stuff in it
	// and write it back before completing the client
	TRSBindingInfo bindInfo;
	res = iMessage.Read(0, bindInfo);
	if (res!=KErrNone)
		{
	    CompleteClientRequest(res);
    	delete this;
		return;
    	}

	CBindManager::TBindingInfo info;
	TBool found=EFalse;
	switch(iState)
		{
		case ESHBinding:
			// Do while more bindings and match not found
			res=iBindMgr->EnumerateBindings(iNameFrom, ETrue, info);
			while( (KErrNone==res) && (!found))
				{
				if(EHierarchical==bindInfo.iParams.iType)
					{
					found = ((info.iSubModuleAddr2==iNameTo)&&(info.iSubModuleAddr1==iNameFrom));
					}
				else // if type == ECustom
					{
					found = (((info.iSubModuleAddr2==iNameTo)&&(info.iSubModuleAddr1==iNameFrom)) ||
							 ((info.iSubModuleAddr1==iNameTo)&&(info.iSubModuleAddr2==iNameFrom)) );
					}

				if(found)
					{
					__CFLOG(KLogSubSysRS, KLogCode,
						_L8("CSessionHandlerBinding found a match in bind info"));
					bindInfo.iParams.iState1 = info.iSubModuleState1;
					bindInfo.iParams.iState2 = info.iSubModuleState2;
					}
				else //	if not found
					{
					res=iBindMgr->EnumerateBindings(iNameFrom, EFalse, info);
					}
				}

			// We succeeded bindingm but have no knowledge of the binding? ASSERT
			__ASSERT_DEBUG(found, User::Panic(RThread().Name(), KRSBadState));

			// Write the info struct back to client, complete the RMessage and delete this
			res = iMessage.Write(0, bindInfo);
			if (KErrNone==res)
				{
				res=iStatus.Int();
				}

			CompleteClientRequest(res);
			delete this;
			break;


		case ESHCompleting:
			CompleteClientRequest(iStatus.Int());
			delete this;
			break;
		default:
			User::Panic(RThread().Name(), KRSBadState);
		}
    }

void CSessionHandlerBinding::DoCancel()
    /** Attempt to cancel request
     */
    {
	TInt res = iBindMgr->Cancel(iStatus);
	if (res!=KErrNone)
		{
		TRequestStatus *status = &iStatus;
		User::RequestComplete(status, KErrCancel);
		}
		
	// Must complete the client request AFTER canceling any bind operations.
	CompleteClientRequest(KErrCancel);
	}

CSessionHandlerUnBinding::CSessionHandlerUnBinding(CRootServerSession* aSession, CBindManager* aBindMgr, const RMessage2& aMessage)
												: CSessionHandler(aSession, aBindMgr,aMessage)
    /** Constructor for Derived session handler
	 @param aSession The root server session
	 @param aBindMgr Pointer to Bind Manager
     */
    {
	}

CSessionHandlerUnBinding* CSessionHandlerUnBinding::NewL(CRootServerSession* aSession, CBindManager* aBindMgr, const RMessage2& aMessage)
    /** Create New instance of this session handler
	 @param aSession The root server session
	 @param aBindMgr Pointer to Bind Manager
	 @param aMessage The message which will be completed
     */
    {
	CSessionHandlerUnBinding* pSession = new(ELeave) CSessionHandlerUnBinding(aSession, aBindMgr,aMessage);
	return pSession;
	}

CSessionHandler::TSHType CSessionHandlerUnBinding::HandlerType()
    /** Identifies the subclass type
     */
	{
	return ESHTypeUnBinding;
	}

TInt CSessionHandlerUnBinding::Start()
    /** Starts the unbinding process
     */
    {
	iState = ESHUnBinding;
	__CFLOG(KLogSubSysRS, KLogCode,_L8("SessionHandlerUnBind - BM::UnbindSubmodulesL"));
	TRAPD(result, iBindMgr->UnbindSubmodulesL(iStatus, iNameFrom, iNameTo));
	SetActive();
	if (result != KErrNone)
		{
		// trigger our runL to deal with aftermath of leave
		TRequestStatus* status = &iStatus;
		User::RequestComplete(status, result);
		}
	return KErrNone;
	}

CSessionHandler* CSessionHandlerUnBinding::MatchPending(const TCFSubModuleAddress& aFrom,
												const TCFSubModuleAddress& aTo)
    /** Match a pending load
	 @param aFrom The name of one module involved
 	 @param aTo The name of the other module involved
     */
    {

	if ((iNameFrom == aFrom) && (iNameTo == aTo))
		{
		return this;
		}
	else return NULL;

	}

void CSessionHandlerUnBinding::SetHandler(const TCFSubModuleAddress& aFrom,
					const TCFSubModuleAddress& aTo)
    /** Set up this handler
	 @param aFrom The name of one module handled
	 @param aTo The name of the other module handled
     */
    {
	iNameFrom = aFrom;
	iNameTo = aTo;
	}

TBool CSessionHandlerUnBinding::CancelHandler(TBool aCompleteRequest, const TBool /*aWaitForCompletionIfUncancelable*/)
    /** Cancel Handler attempts to cancel request
	 @param aCompleteRequest Whether to complete request or not
 	 @param aWaitForCompletionIfUncancelable Whether to wait for completion in a nested active scheduler (option used during session cleanup)
 	 @return Returns ETrue if the handler should be deleted by the caller or EFalse if the handler will delete itself
     */
    {
    iState = ESHCompleting;
    // Pretend message has already been completed if
    // we are asked not to and it hasn't already
    if  ((!aCompleteRequest) && (!iRequestCompleted))
    	{
        iRequestCompleted = ETrue;
        }

// Although sometimes useful this logging statement has to be left suppressed because sessions can be
// deleted by the base CServer2 dtor, ie after CFLOG has been deleted by the RootServer dtor.
// 	__CFLOG( KLogSubSysRS, KLogCode,_L8("CSessionHandlerUnBinding - Cancel Handler"));

	Cancel();
	
	return ETrue;
	}

void CSessionHandlerUnBinding::RunL()
    /** Handles aftermath of unbind, by completing the message
     */
    {
    __CFLOG_1(KLogSubSysRS, KLogCode,_L8("CSessionHandlerUnBinding::RunL status %d"),
    	iStatus.Int());
    CompleteClientRequest(iStatus.Int());
    delete this;
    }

void CSessionHandlerUnBinding::DoCancel()
    /** Attempt to cancel request
     */
    {
	CompleteClientRequest(KErrCancel);
	TInt res = iBindMgr->Cancel(iStatus);
	if (res!=KErrNone)
		{
		TRequestStatus *status = &iStatus;
		User::RequestComplete(status, KErrCancel);
		}
	}

CRSTransientModuleInfo::CRSTransientModuleInfo(const RCFChannel::TMsgQueues& aTxQueues,
											   const RCFChannel::TMsgQueues& aRxQueues,
											   TRSHeapType aThreadHeapType,
											   TThreadFunction aLibEntry,
											   HBufC8* aIniDataOnRSHeap,
											   TThreadPriority aPriority,
											   TUint32 aControlFlags)
: iThreadHeapType(aThreadHeapType),
  iLibEntry(aLibEntry),
  iIniDataOnRSHeap(aIniDataOnRSHeap),
  iPriority(aPriority),
  iControlFlags(aControlFlags)
	{
	iCFModuleInfo.iTxQueues=aTxQueues;
	iCFModuleInfo.iRxQueues=aRxQueues;
	}

CRSTransientModuleInfo::~CRSTransientModuleInfo()
	{
	delete iIniDataOnRSHeap;
	}

TInt CRSTransientModuleInfo::ModuleThreadStartup(TAny* aArg)
	{
	CRSTransientModuleInfo* startupInfo = (CRSTransientModuleInfo*) aArg;

	if (startupInfo->iControlFlags & TRSStartModuleParamContainer::KCF_ThreadAsSystem)
		{
		User::SetCritical( User::ESystemCritical);
		}
	else
		{
		User::SetCritical( User::ENotCritical);
		}

    RThread().SetPriority(startupInfo->iPriority);

	// Duplicate the ini data into the new thread's heap.
	// ** NOTE FOR CPM IMPLEMENTORS **
	// It is an explicit responsibility to take ownership of the iIniData and to delete it
	if(startupInfo->iIniDataOnRSHeap)
		{
		startupInfo->iCFModuleInfo.iIniData = startupInfo->iIniDataOnRSHeap->Alloc();
		if(!startupInfo->iCFModuleInfo.iIniData )
			{
			return KErrNoMemory;
			}
		}
	else
		{
		startupInfo->iCFModuleInfo.iIniData = startupInfo->iIniDataOnRSHeap;
		}
	return startupInfo->iLibEntry(&startupInfo->iCFModuleInfo);
	}



// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// ssmpolicysrv.cpp
// 
//

#include <e32debug.h>
#include <e32math.h>
#include <f32file.h>
#include <s32file.h>
#include <ssm/ssmcommandlist.h>

#include "ssmswppolicysrv.h"
#include "ssmswppolicysess.h"
#include "ssmswppolicyconst.h"
#include "ssmdebug.h"
#include "ssmserverpanic.h"

// ------------------- SsmSwpPolicy Server Security Setup ----------------------

const TInt KSsmServerSid = 0x2000D75B; //UID3 of sysstatemgr.exe

const TUint  KRangeCount = 3;
const TInt   KRanges[KRangeCount] =
	{
	0,
	KFirstUsedOpCode,		//range: 1...(EEndOfSsmSwpPolicySrvOpCodes-1) inclusive
	EEndOfSsmSwpPolicySrvOpCodes
	};

/**
 Maps to index 0 in the array KPolicyElements
 */
const TInt KSsmServerSidCheck = 0;

/**
 Specifies the appropriate action for each range in KRanges.
 The nth element of KElementsIndex specifies the appropriate action for the nth range in KRanges.
 */
const TUint8 KElementsIndex[KRangeCount] =
	{
	CPolicyServer::ENotSupported,
	KSsmServerSidCheck,
	CPolicyServer::ENotSupported,
	};

/**
 Array containing the different security checks performed by this server
 */
const CPolicyServer::TPolicyElement KPolicyElements[] =
	{
	{_INIT_SECURITY_POLICY_S0(KSsmServerSid), CPolicyServer::EFailClient} //lint !e778 suppress Constant expression evaluates to 0 in operation '+'
	};

/**
 Setup a security policy which requires all caps to be used by the SsmServer for all requests
 including creating a connection. The caller's SID is matched against SsmServer's
 SID in each ServiceL call
 */
const CPolicyServer::TPolicy KSsmSwpPolicyServerPolicy =
	{
	KSsmServerSidCheck,	// map connection attempts as well to index [0] in KPolicyElements[]
	KRangeCount,
	KRanges,
	KElementsIndex,
	KPolicyElements
	};

/**
CSsmSwpPolicyStepCompletion is an active object used for executing all the steps involved in 
the swp transition. Initial state for the active object is set as EIdle.
*/
CSsmSwpPolicyServer::CSsmSwpPolicyStepCompletion* CSsmSwpPolicyServer::CSsmSwpPolicyStepCompletion::NewL(MSsmSwpPolicy* aSwpPolicy, CSsmSwpPolicyServer& aServer, const TInt aSessionIndex)
	{
	CSsmSwpPolicyStepCompletion* self = new(ELeave)CSsmSwpPolicyStepCompletion(aSwpPolicy, aServer, aSessionIndex);
	return self;
	}

CSsmSwpPolicyServer::CSsmSwpPolicyStepCompletion::CSsmSwpPolicyStepCompletion(MSsmSwpPolicy* aSwpPolicy, CSsmSwpPolicyServer& aServer, const TInt aSessionIndex)
	: CActive(EPriorityStandard), iStep(EIdle), iSwpPolicy(aSwpPolicy), iSessionIndex(aSessionIndex), iServer(aServer)
	{
	CActiveScheduler::Add(this);
	}

CSsmSwpPolicyServer::CSsmSwpPolicyStepCompletion::~CSsmSwpPolicyStepCompletion()
	{
	Cancel();
	}

/**
Calls Initialize of the SwpPolicy. Sets the active object state as EInitialize.
*/
void CSsmSwpPolicyServer::CSsmSwpPolicyStepCompletion::StartInitialize(const RMessagePtr2& aMessagePtr2)
	{
	__ASSERT_DEBUG(!IsActive(), PanicNow(KSsmSwpPolicyServer, ESsmSwpPolicySrvStepError1));
	__ASSERT_DEBUG(iStep == EIdle, PanicNow(KSsmSwpPolicyServer, ESsmSwpPolicySrvStepError1));

	// This message will be completed in RunL().
	iMessagePtr = aMessagePtr2;

	// Initialize is the first step in the process of swp transition.
	// CSsmSwpPolicyFrame does	all the step checking before giving the request to ssmswppolicyserver 
	// so no further checking is required here.
	iStep = EInitialize;
	iSwpPolicy->Initialize(iStatus);
	SetActive();
	DEBUGPRINT2(_L("CSsmSwpPolicyStepCompletion: CSsmSwpPolicyStepCompletion State %d"), iStep);
	}

/**
Calls PrepareCommandList of the SwpPolicy. Sets the active object state as EPrepareCommandList.
*/
void CSsmSwpPolicyServer::CSsmSwpPolicyStepCompletion::StartPrepareCommandList(const TSsmSwp& aSwp, const RMessagePtr2& aMessagePtr2)
	{
	__ASSERT_DEBUG(!IsActive(), PanicNow(KSsmSwpPolicyServer, ESsmSwpPolicySrvStepError2));
	__ASSERT_DEBUG(iStep == EInitialize, PanicNow(KSsmSwpPolicyServer, ESsmSwpPolicySrvStepError2));

	// This message will be completed in RunL().
	iMessagePtr = aMessagePtr2;

	// PrepareCommandList should be called only after initializing the swp policy.  
	// CSsmSwpPolicyFrame does all the step checking before giving the request to ssmswppolicyserver 
	// so no further checking is required here.
	iStep = EPrepareCommandList;
	DEBUGPRINT2(_L("CSsmSwpPolicyStepCompletion: CSsmSwpPolicyStepCompletion State %d"), iStep);
	iSwpPolicy->PrepareCommandList(aSwp, iStatus);
	SetActive();
	}

/**
Calls HandleCleReturnValue of the SwpPolicy.
*/
void CSsmSwpPolicyServer::CSsmSwpPolicyStepCompletion::StartHandleCleReturnValue(const TSsmSwp& aSwp, TInt aError, TInt aSeverity, const RMessagePtr2& aMessagePtr2)
	{
	__ASSERT_DEBUG(!IsActive(), PanicNow(KSsmSwpPolicyServer, ESsmSwpPolicySrvStepError4));
	__ASSERT_DEBUG(iStep == ECallCommandList, PanicNow(KSsmSwpPolicyServer, ESsmSwpPolicySrvStepError4));

	// This message will be completed in RunL().
	iMessagePtr = aMessagePtr2;

	// HandleCleReturnValue should be called only after executing the command list which is returned by swp policy.
	// CSsmSwpPolicyFrame does all the step checking before giving the request to ssmswppolicyserver so no further checking is required here.
	iStep = EHandleCleReturnValue;
	DEBUGPRINT2(_L("CSsmSwpPolicyStepCompletion: CSsmSwpPolicyStepCompletion State %d"), iStep);
	iSwpPolicy->HandleCleReturnValue(aSwp, aError, aSeverity, iStatus);
	SetActive();
	}

void CSsmSwpPolicyServer::CSsmSwpPolicyStepCompletion::RunL()
	{
	DEBUGPRINT2(_L("CSsmSwpPolicyStepCompletion: CSsmSwpPolicyStepCompletion State %d"), iStep);
	// Completes the message
	iMessagePtr.Complete(iStatus.Int());

	// swp transition will be marked as complete only after the execution of HandleCleReturnValue method
	if (iStep == EHandleCleReturnValue)
		{
		// In order to avoid the memory leak we have to delete the session info as this swp transition is cancelled, 
		// and the same session can be used for the next similar swp transition.
		iServer.DeleteSSwpPolicySessionInfo(iSessionIndex);
		}
	}

/**
Cancels any outstanding request, based on the state of the active object.
*/
void CSsmSwpPolicyServer::CSsmSwpPolicyStepCompletion::DoCancel()
	{
	switch(iStep)
		{
		case EInitialize:
			{
			DEBUGPRINT1(_L("CSsmSwpPolicyStepCompletion: Cancelled the Initization of SwpPolicy"));
			iSwpPolicy->InitializeCancel();
			iMessagePtr.Complete(KErrCancel);
			break;
			}
		case EPrepareCommandList:
			{
			DEBUGPRINT1(_L("CSsmSwpPolicyStepCompletion: Cancelled the Preparation of the commandlist"));
			iSwpPolicy->PrepareCommandListCancel();
			iMessagePtr.Complete(KErrCancel);
			break;
			}
		case EHandleCleReturnValue:
			{
			DEBUGPRINT1(_L("CSsmSwpPolicyStepCompletion: Cancelled the HandleCleReturnValue"));
			iSwpPolicy->HandleCleReturnValueCancel();
			iMessagePtr.Complete(KErrCancel);
			break;
			}
		default:
			{
			break;
			}
		};
	}

/** 
Current RunL() implemenation doesn't leave, so RunError will never be called.
*/
TInt CSsmSwpPolicyServer::CSsmSwpPolicyStepCompletion::RunError()
	{
	DEBUGPRINT1(_L("CSsmSwpPolicyStepCompletion: RunError"));
	return KErrNone;
	}

/**
Gets the dll handle to SwpPolicy and initializes the SwpPolicy
*/
void CSsmSwpPolicyServer::CallSetDllHandleAndInitializeL(const RMessage2& aMessage, const TInt aSessionIndex)
	{
	__ASSERT_DEBUG(IN_RANGE(aSessionIndex, iSessionInfoArray.Count()), PanicNow(KSsmSwpPolicyBadIdx, KSsmSwpPolicySrvArrayIndexInvalid));

	RLibrary library;
	// Sets the swp policy library handle.  Library is owned by the CSsmSwpRequestHandler.
	library.SetHandle(aMessage.Int0());

	DEBUGPRINT1(_L("CSsmSwpPolicyServer: SwpPolicy handle is set"));
	iSessionInfoArray[aSessionIndex].iSwpPolicy = reinterpret_cast<MSsmSwpPolicy*>(library.Lookup(1)());

	DEBUGPRINT1(_L("CSsmSwpPolicyServer: Create CSsmSwpPolicyStepCompletion active object"));
	iSessionInfoArray[aSessionIndex].iSsmSwpPolicyStepCompletion = CSsmSwpPolicyStepCompletion::NewL(iSessionInfoArray[aSessionIndex].iSwpPolicy, *this, aSessionIndex);

	// aMessage will be completed in CSsmSwpPolicyStepCompletion's RunL method,
	// when the Initialization of the swp policy is completed.
	iSessionInfoArray[aSessionIndex].iSsmSwpPolicyStepCompletion->StartInitialize(aMessage);
	}

/**
Cancels the Initialization of the swppolicy, if any Initialization request is pending.
*/
void CSsmSwpPolicyServer::CallInitializeCancel(TInt aSessionIndex)
	{
	DEBUGPRINT1(_L("CSsmSwpPolicyServer: CallInitializeCancel"));
	__ASSERT_DEBUG(IN_RANGE(aSessionIndex, iSessionInfoArray.Count()), PanicNow(KSsmSwpPolicyBadIdx, KSsmSwpPolicySrvArrayIndexInvalid));
	__ASSERT_DEBUG(iSessionInfoArray[aSessionIndex].iSsmSwpPolicyStepCompletion != NULL, PanicNow(KSsmSwpPolicyServer, ESsmSwpPolicyServerError1));

	// CSsmSwpPolicyFrame checks whether initialize is called before cancelling the initialization, , so no need to check
	// for valid iSsmSwpPolicyStepCompletion pointer.
	// iSessionInfoArray[aSessionIndex].iMessagePtr will be completed in CSsmSwpPolicyStepCompletion's DoCancel method,
	iSessionInfoArray[aSessionIndex].iSsmSwpPolicyStepCompletion->Cancel();
	
	DEBUGPRINT1(_L("CSsmSwpPolicyServer: Deleted the session info as Initialize is cancelled"));
	// In order to avoid the memory leak we have to delete the session info as this swp transition is cancelled, 
	// and the same session can be used for the next similar swp transition.
	DeleteSSwpPolicySessionInfo(aSessionIndex);
	}

/**
Prepares the commandlist in swppolicy
*/
void CSsmSwpPolicyServer::CallPrepareCommandListL(const RMessage2& aMessage, TInt aSessionIndex)
	{
	DEBUGPRINT1(_L("CSsmSwpPolicyServer: Call SwpPolicy's PrepareCommandList "));
	__ASSERT_DEBUG(IN_RANGE(aSessionIndex, iSessionInfoArray.Count()), PanicNow(KSsmSwpPolicyBadIdx, KSsmSwpPolicySrvArrayIndexInvalid));
	__ASSERT_DEBUG(iSessionInfoArray[aSessionIndex].iSsmSwpPolicyStepCompletion != NULL, PanicNow(KSsmSwpPolicyServer, ESsmSwpPolicyServerError2));

	TSsmSwp ssmSwp(0,0);
	TPckg<TSsmSwp> swpBuf(ssmSwp);
    aMessage.ReadL(0, swpBuf);

	// CSsmSwpPolicyFrame checks whether initialize is called before CallHandleCleReturnValue is called, so no need to check
	// for valid iSsmSwpPolicyStepCompletion pointer.

	// aMessage will be completed in CSsmSwpPolicyStepCompletion's RunL method,
	// when the preparation of the commandlist is completed.
	iSessionInfoArray[aSessionIndex].iSsmSwpPolicyStepCompletion->StartPrepareCommandList(ssmSwp, aMessage);
	}

/**
Cancels the preparation of the commandlist, if any PrepareCommandList request is pending.
*/
void CSsmSwpPolicyServer::CallPrepareCommandListCancel(TInt aSessionIndex)
	{
	DEBUGPRINT1(_L("CSsmSwpPolicyServer: CallPrepareCommandListCancel"));
	__ASSERT_DEBUG(IN_RANGE(aSessionIndex, iSessionInfoArray.Count()), PanicNow(KSsmSwpPolicyBadIdx, KSsmSwpPolicySrvArrayIndexInvalid));
	__ASSERT_DEBUG(iSessionInfoArray[aSessionIndex].iSsmSwpPolicyStepCompletion != NULL, PanicNow(KSsmSwpPolicyServer, ESsmSwpPolicyServerError3));

	// CSsmSwpPolicyFrame checks whether initialize is called before cancelling the initialization, so no need to check
	// for valid iSsmSwpPolicyStepCompletion pointer.
	// iSessionInfoArray[aSessionIndex].iMessagePtr will be completed in CSsmSwpPolicyStepCompletion's DoCancel method,
	iSessionInfoArray[aSessionIndex].iSsmSwpPolicyStepCompletion->Cancel();
	
	DEBUGPRINT1(_L("CSsmSwpPolicyServer: Deleted the session info as PrepareCommandList is cancelled"));
	// In order to avoid the memory leak we have to delete the session info as this swp transition is cancelled, 
	// and the same session can be used for the next similar swp transition.
	DeleteSSwpPolicySessionInfo(aSessionIndex);
	}

/**
Returns the commandlist to the client from swppolicy.
@return A pointer to commandlist.
*/
void CSsmSwpPolicyServer::CallCommandListL(const RMessage2& aMessage, TInt aSessionIndex)
	{
	DEBUGPRINT1(_L("CSsmSwpPolicyServer: CallCommandListL"));
	__ASSERT_DEBUG(IN_RANGE(aSessionIndex, iSessionInfoArray.Count()), PanicNow(KSsmSwpPolicyBadIdx, KSsmSwpPolicySrvArrayIndexInvalid));
	__ASSERT_DEBUG(iSessionInfoArray[aSessionIndex].iSsmSwpPolicyStepCompletion != NULL, PanicNow(KSsmSwpPolicyServer, ESsmSwpPolicyServerError4));
		// CommandList should be called only after preparing the command list, so the active object state should be EPrepareCommandList.
	__ASSERT_DEBUG(iSessionInfoArray[aSessionIndex].iSsmSwpPolicyStepCompletion->CurrentStep() == CSsmSwpPolicyServer::CSsmSwpPolicyStepCompletion::EPrepareCommandList, 
			PanicNow(KSsmSwpPolicyServer, ESsmSwpPolicySrvStepError3));
	
	// CSsmSwpPolicyFrame checks whether initialize is called before CallCommandList is called, so no need to check
	// for valid iSsmSwpPolicyStepCompletion pointer.
	// Sets the current step to CallCommandList
	iSessionInfoArray[aSessionIndex].iSsmSwpPolicyStepCompletion->SetCurrentStep(CSsmSwpPolicyServer::CSsmSwpPolicyStepCompletion::ECallCommandList);
	CSsmCommandList* ssmCmdList = (iSessionInfoArray[aSessionIndex].iSwpPolicy)->CommandList();
	CleanupStack::PushL(ssmCmdList);
	CBufFlat* buf = CBufFlat::NewL(KSsmSwpPolicyStreamBufMaxSize);
	CleanupStack::PushL(buf);
	
	RBufWriteStream writeStream(*buf);
	CleanupClosePushL(writeStream);
	
	DEBUGPRINT1(_L("CSsmSwpPolicyServer: Externalizes the commandlist"));
	ssmCmdList->ExternalizeL(writeStream);

	// Ensure any memory that might already have been allocated  is disposed of. 
	// Transfer the streamed cmd list from the CBuf.
	RBuf8 cmdListBuf;
	cmdListBuf.CreateL(buf->Size());
	CleanupClosePushL(cmdListBuf);	
	buf->Read(0, cmdListBuf);
	
	aMessage.WriteL(0, cmdListBuf);
	
	aMessage.Complete(KErrNone);
	CleanupStack::PopAndDestroy(4, ssmCmdList);	//	buf, writeStream and cmdListBuf
	}

/**
Handles the value returned by Cle.
*/
void CSsmSwpPolicyServer::CallHandleCleReturnValueL(const RMessage2& aMessage, TInt aSessionIndex)
	{
	DEBUGPRINT1(_L("CSsmSwpPolicyServer: CallHandleCleReturnValueL"));
	__ASSERT_DEBUG(IN_RANGE(aSessionIndex, iSessionInfoArray.Count()), PanicNow(KSsmSwpPolicyBadIdx, KSsmSwpPolicySrvArrayIndexInvalid));
	__ASSERT_DEBUG(iSessionInfoArray[aSessionIndex].iSsmSwpPolicyStepCompletion != NULL, PanicNow(KSsmSwpPolicyServer, ESsmSwpPolicyServerError5));

	TSsmSwp ssmSwp(0,0);
	TPckg<TSsmSwp> swpBuf(ssmSwp);
	aMessage.ReadL(0, swpBuf);

	const TInt error = aMessage.Int1();
	const TInt severity = aMessage.Int2();

	// CSsmSwpPolicyFrame checks whether initialize is called before CallHandleCleReturnValue is called, so no need to check
	// for valid iSsmSwpPolicyStepCompletion pointer.

	// aMessage will be completed in CSsmSwpPolicyStepCompletion's RunL method,
	// when the cle's return value is handled by the swppolicy.
	(iSessionInfoArray[aSessionIndex].iSsmSwpPolicyStepCompletion)->StartHandleCleReturnValue(ssmSwp, error, severity, aMessage);
	}

/**
Cancels the HandleCleReturnValue, if any HandleCleReturnValue request is pending.
*/
void CSsmSwpPolicyServer::CallHandleCleReturnValueCancel(TInt aSessionIndex)
	{
	DEBUGPRINT1(_L("CSsmSwpPolicyServer: CallHandleCleReturnValueCancel"));
	__ASSERT_DEBUG(IN_RANGE(aSessionIndex, iSessionInfoArray.Count()), PanicNow(KSsmSwpPolicyBadIdx, KSsmSwpPolicySrvArrayIndexInvalid));
	__ASSERT_DEBUG(iSessionInfoArray[aSessionIndex].iSsmSwpPolicyStepCompletion != NULL, PanicNow(KSsmSwpPolicyServer, ESsmSwpPolicyServerError6));

	// CSsmSwpPolicyFrame checks whether initialize is called before cancelling the initialization, so no need to check
	// for valid iSsmSwpPolicyStepCompletion pointer.
	// iSessionInfoArray[aSessionIndex].iMessagePtr will be completed in CSsmSwpPolicyStepCompletion's DoCancel method.
	iSessionInfoArray[aSessionIndex].iSsmSwpPolicyStepCompletion->Cancel();
	
	DEBUGPRINT1(_L("CSsmSwpPolicyServer: Deleted the session info as HandleCleReturnValue is cancelled"));
	// In order to avoid the memory leak we have to delete the session info as this swp transition is cancelled, 
	// and the same session can be used for the next similar swp transition.
	DeleteSSwpPolicySessionInfo(aSessionIndex);
	}

/**
Iterate through iSessionInfoArray to find an unused array element
If found, use it. Otherwise, Append() a new SSwpPolicySessionInfo.
This function is called during CSession construction.
*/
void CSsmSwpPolicyServer::RegisterSessionL(TInt& aSessionIndex)
	{
	const TInt count = iSessionInfoArray.Count();
	TBool slotFound = EFalse;
	
	for(TInt i = 0; i < count; ++i)
		{
		if(!iSessionInfoArray[i].iInUse)
			{
			DEBUGPRINT1(_L("CSsmSwpPolicyServer: Using the free slot of the sessionarray"));
			iSessionInfoArray[i].iInUse = ETrue;
			__ASSERT_ALWAYS(NULL, iSessionInfoArray[i].iSwpPolicy);
			aSessionIndex = i;
			slotFound = ETrue;
			break;
			}
		}
	
	if(!slotFound)
		{
		DEBUGPRINT1(_L("CSsmSwpPolicyServer: No free slot found, so appending a new member to the sessionarray"));
		SSwpPolicySessionInfo sessionInfo;
		sessionInfo.iInUse = ETrue;
		sessionInfo.iSwpPolicy = NULL;
		sessionInfo.iSsmSwpPolicyStepCompletion = NULL;
		iSessionInfoArray.AppendL(sessionInfo);
		// using count instead of iSessionInfoArray.Count()-1 as it is set before appending the element to the array,
		// and its value is equal to iSessionInfoArray.Count()-1
		aSessionIndex = count;
		}
	DEBUGPRINT2(_L("Registered SsmSwpPolicyCli in slot %d"), aSessionIndex);
	}

/**
This method is used to delete the session information once the swp transition is complete.
This is required as the same session can be used for more than one individual swp transition.
*/
void CSsmSwpPolicyServer::DeleteSSwpPolicySessionInfo(TInt aSessionIndex)
	{
	__ASSERT_DEBUG(IN_RANGE(aSessionIndex, iSessionInfoArray.Count()), PanicNow(KSsmSwpPolicyBadIdx, KSsmSwpPolicySrvArrayIndexInvalid));

	DEBUGPRINT1(_L("CSsmSwpPolicyServer: Deletes the SwpPolicySession information."));
	if(iSessionInfoArray[aSessionIndex].iSsmSwpPolicyStepCompletion)
		{
		delete iSessionInfoArray[aSessionIndex].iSsmSwpPolicyStepCompletion;
		iSessionInfoArray[aSessionIndex].iSsmSwpPolicyStepCompletion = NULL;
		}

	if(iSessionInfoArray[aSessionIndex].iSwpPolicy)
		{
		DEBUGPRINT1(_L("CSsmSwpPolicyServer: Releases the SwpPolicy"));
		(iSessionInfoArray[aSessionIndex].iSwpPolicy)->Release();
		iSessionInfoArray[aSessionIndex].iSwpPolicy = NULL;
		}
	}

/**
Called from CSession destructor.
*/
void CSsmSwpPolicyServer::DeregisterSession(const TInt& aSessionIndex)
	{
	__ASSERT_DEBUG(IN_RANGE(aSessionIndex, iSessionInfoArray.Count()), PanicNow(KSsmSwpPolicyBadIdx, KSsmSwpPolicySrvArrayIndexInvalid));
	
	DeleteSSwpPolicySessionInfo(aSessionIndex);
	iSessionInfoArray[aSessionIndex].iInUse = EFalse;

	// Remove unused elements at the end of the array, so iSessionInfoArray can be (granular) shrunk 
    const TInt count = iSessionInfoArray.Count(); 
    TBool slotRemoved = EFalse; 
    for (TInt i = count-1; i >= 0 ; i--) 
         {
        if (!iSessionInfoArray[i].iInUse) 
        	{
            iSessionInfoArray.Remove(i); 
            slotRemoved = ETrue; 
        	}
        else 
        	{
        	break;
        	}
         } 
	   if (slotRemoved) 
		   { 
		   iSessionInfoArray.GranularCompress(); 
		   } 
	DEBUGPRINT2(_L("De-registered SsmSwpPolicyCli slot %d"), aSessionIndex);
 	}

/**
Used to create a new server-side session.
@return A pointer to the new instance of CSession2.
@leave KErrNotSupported if versions are incompatible.
*/
EXPORT_C CSession2* CSsmSwpPolicyServer::NewSessionL(const TVersion& aVersion, const RMessage2& /*aMessage*/) const
	{
	TVersion version(KSsmSwpPolicySrvVersionMajor, KSsmSwpPolicySrvVersionMinor, KSsmSwpPolicySrvVersionBuild);

	if(!User::QueryVersionSupported(version, aVersion))
		{
		DEBUGPRINT1(_L("CSsmSwpPolicyServer: Server version not supported"));
		User::Leave(KErrNotSupported);
		}

	CSsmSwpPolicyServer& mutatedSelf = const_cast< CSsmSwpPolicyServer& >(*this);
	return CSsmSwpPolicySession::NewL(mutatedSelf); 
	}
	
/**
Used to create a new CSsmSwpPolicyServer
@return A pointer to the CSsmSwpPolicyServer
*/
CSsmSwpPolicyServer* CSsmSwpPolicyServer::NewLC(const TDesC& aServerName)
	{
	CSsmSwpPolicyServer* self = new(ELeave) CSsmSwpPolicyServer;	
	CleanupStack::PushL(self);
	self->ConstructL(aServerName);
	return self;	
	}

/**
Static function used to create and start CSsmSwpPolicyServer
@return KErrAlreadyExists if the server is already running
*/
EXPORT_C TInt CSsmSwpPolicyServer::StartSsmSwpPolicySrv(const TDesC& aServerName)
	{
	TAny* threadParam = reinterpret_cast<TAny*>(const_cast<TDesC*>(&aServerName));

	// Create a Unique name for the thread name
	TName name(aServerName);
	_LIT(KUnderScore, "_");
	name.Append(KUnderScore);
	name.AppendNum(Math::Random(), EHex);

	const TInt KSsmPolicySrvMinHeapSize = 0x2000;	// 8kB
	const TInt KSsmPolicySrvMaxHeapSize = 10 * KSsmPolicySrvMinHeapSize;
	RThread srvThread;
	TInt err = srvThread.Create(name, &CSsmSwpPolicyServer::SsmSwpPolicySrvThreadFn, 
								 KDefaultStackSize, KSsmPolicySrvMinHeapSize, KSsmPolicySrvMaxHeapSize, 
								 threadParam, EOwnerProcess);

	DEBUGPRINT2(_L("CSsmSwpPolicyServer: SsmSwpPolicyServer created with %d") ,err);
	if(KErrNone == err)
		{
		TRequestStatus trs;
	
		srvThread.Rendezvous(trs);
		srvThread.Resume();
		
		User::WaitForRequest(trs);	
		err = trs.Int();
		srvThread.Close();
		}

	return err;
	}

/**
Creates the ssmSwpPolicyServer
*/
TInt CSsmSwpPolicyServer::SsmSwpPolicySrvThreadFn(TAny* aAny)
	{
	const TDesC* serverName = const_cast<const TDesC*>(static_cast<TDesC*>(aAny));
	TInt err = KErrNoMemory;
    CTrapCleanup* cleanup = CTrapCleanup::New();
    if(cleanup)
    	{
	    TRAP(err,
	        {
	        CActiveScheduler* sched = new(ELeave) CActiveScheduler();
	        CleanupStack::PushL(sched);
	        CActiveScheduler::Install(sched);
	
	        CSsmSwpPolicyServer* ssmSwpPolicySrv = CSsmSwpPolicyServer::NewLC(*serverName);
	        RThread::Rendezvous(KErrNone);
	    	// Must not use serverName after rendezvousing with client
	        serverName = NULL;
	        
	        CActiveScheduler::Start();
	
	        CleanupStack::PopAndDestroy(ssmSwpPolicySrv);
	        CleanupStack::PopAndDestroy(sched);
	        });

	    delete cleanup;
    	}
 
    return err;
	}

CSsmSwpPolicyServer::CSsmSwpPolicyServer()
	: CPolicyServer(EPriorityHigh, KSsmSwpPolicyServerPolicy, EUnsharableSessions)
	{
	}

/**
Starts the ssmSwpPolicyServer
*/
void CSsmSwpPolicyServer::ConstructL(const TDesC& aServerName)
	{
	StartL(aServerName);
	// The starting thread is signalled in the thread function.
	}

CSsmSwpPolicyServer::~CSsmSwpPolicyServer()
	{
	iSessionInfoArray.Close();
	}


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
//

#include <ssm/ssmswppolicy.h>
#include <ssm/ssmcommandlist.h>

#include "ssmdebug.h"
#include "ssmserverpanic.h"
#include "ssmswppolicyframe.h"
#include "ssmswptransitionengine.h"
#include "ssmswppolicyframe.h"
#include "clesessionproxy.h"
#include "ssmcommandlistutils.h"


//
//Construction/Destruction
//

/**
Create an engine using the swp value supplied

@param aSwp Associated swp value
@return A new instance of CSsmSwpTransitionEngine
*/
CSsmSwpTransitionEngine* CSsmSwpTransitionEngine::NewL(const TSsmSwp& aSwp)
	{
	CSsmSwpTransitionEngine* self = new (ELeave) CSsmSwpTransitionEngine(aSwp);
	CleanupStack::PushL (self);
	self->ConstructL ();
	CleanupStack::Pop (self);
	return self;
	}

/**
Empty Construct
*/
void CSsmSwpTransitionEngine::ConstructL()
	{
	}

/**
Construct engine

@param aSwp Associated swp value

*/
CSsmSwpTransitionEngine::CSsmSwpTransitionEngine(const TSsmSwp& aSwp) :
	CActive(CActive::EPriorityStandard), iClientStatus(NULL),
			iCurrentSwpValue(aSwp), iNextAction(EIdle), iPerformCommandListValidation(ETrue)
	{
	CActiveScheduler::Add (this);
	}

/**
Destruct engine and destroy owned frame

*/
CSsmSwpTransitionEngine::~CSsmSwpTransitionEngine()
	{
	__ASSERT_DEBUG(KNullHandle == iClientMessage.Handle(), PanicNow(KPanicSysStateMgr,ESwpTransitionEngineError18));
	Cancel();
	delete iPolicyFrame; // owned by me
    delete iCommandList;
    }

/**
Give the engine a client message
If a client message is supplied, it will be completed when the engine starts running
or if an error occurs

@param aMessage Client message

*/
void CSsmSwpTransitionEngine::SetClientMessage(const RMessage2* aMessage)
	{
	if(aMessage)
		{
		iClientMessage = *aMessage;
		}
	}

CSession2* CSsmSwpTransitionEngine::OriginatingSesssion() const
	{
	return iClientMessage.Session();
	}

void CSsmSwpTransitionEngine::PerformCommandListValidation(TBool aSetting)
	{
	iPerformCommandListValidation = aSetting;
	}

/**
Give the engine a cle session

@param aCleSession Object that implements CLE Session proxy interface

*/
void CSsmSwpTransitionEngine::SetCleSessionProxy(MCleSessionProxy* aCleSession)
	{
	__ASSERT_DEBUG(NULL != aCleSession, PanicNow(KPanicSysStateMgr,ESwpTransitionEngineError1));
	iCleSession = aCleSession;
	}


/**
Give the engine a ssmSwpPolicy session

@param aSsmSwpPolicySession Object that has RSsmSwpPolicySession Session
*/
void CSsmSwpTransitionEngine::SetSsmSwpPolicySession(CSsmSwpPolicyCliSession* aSsmSwpPolicySession)
	{
	__ASSERT_DEBUG(NULL != aSsmSwpPolicySession, PanicNow(KPanicSysStateMgr,ESwpTransitionEngineError21));
	iSsmSwpPolicySession = aSsmSwpPolicySession;
	}

/**
Give the engine a policy frame
Note: the engine takes ownership of the frame

@param aPolicyFrame Policy frame

*/
void CSsmSwpTransitionEngine::SetSwpPolicyFrame(CSsmSwpPolicyFrame* aPolicyFrame)
	{
	__ASSERT_DEBUG(NULL != aPolicyFrame, PanicNow(KPanicSysStateMgr,ESwpTransitionEngineError2));
	iPolicyFrame = aPolicyFrame;
	}

#ifdef TEST_SSM_SERVER
/**
Start a transition without completion notification
This method is for testcode only
*/
void CSsmSwpTransitionEngine::Submit()
	{
	__ASSERT_DEBUG(!IsActive(), PanicNow(KPanicSysStateMgr,ESwpTransitionEngineError3));
	__ASSERT_DEBUG(iClientStatus == NULL, PanicNow(KPanicSysStateMgr,ESwpTransitionEngineError5));
	iClientStatus=NULL;
	DoSubmit ();
	}
#endif

/**
Start a transition with completion notification

@param aClientStatus - the client status to complete

*/
void CSsmSwpTransitionEngine::Submit(TRequestStatus& aClientStatus)
	{
	__ASSERT_ALWAYS(!IsActive(), PanicNow(KPanicSysStateMgr,ESwpTransitionEngineError4));
	__ASSERT_ALWAYS(iClientStatus == NULL, PanicNow(KPanicSysStateMgr,ESwpTransitionEngineError5));

	aClientStatus = KRequestPending;
	iClientStatus = &aClientStatus;
	DoSubmit ();
	}

/**
Start transition engine running
*/
void CSsmSwpTransitionEngine::DoSubmit()
	{
	// Submit can only be called on a brand new engine
	__ASSERT_DEBUG(EIdle == iNextAction, PanicNow(KPanicSysStateMgr,ESwpTransitionEngineError6));
	__ASSERT_ALWAYS(NULL != iCleSession, PanicNow(KPanicSysStateMgr,ESwpTransitionEngineError14));
	__ASSERT_ALWAYS(NULL != iPolicyFrame, PanicNow(KPanicSysStateMgr,ESwpTransitionEngineError15));
	__ASSERT_ALWAYS(NULL != iSsmSwpPolicySession, PanicNow(KPanicSysStateMgr,ESwpTransitionEngineError23));

	TInt err = KErrNone;
	// Before this transition engine can be used, we need to connect to (and probably start) the Ssmswppolicysrv.
	// The session remains connected as long as SsmSwpPolicySrv does not disconnect itself (due to a panic for example)
	if (!iSsmSwpPolicySession->IsConnected())
		{
		TRAP(err, iSsmSwpPolicySession->ConnectL());
		DEBUGPRINT2(_L("CSsmSwpTransitionEngine iSsmSwpPolicySession->ConnectL called with err: %d"), err);
		}
	if (err != KErrNone)
		{
		// Call DoRunError() to handle the error.
		DoRunError(err);
		}
	else
		{
		// Sets the SsmSwpPolicySession in the policyframe
		iPolicyFrame->SetSsmSwpPolicySession(iSsmSwpPolicySession);

		iNextAction = iCleSession->IsConnected() ? ESetSwpPolicyDllHandleAndInitialize : EUnConnected;
		Start ();
		}
	}

/**
Start active scheduler running it's state machine
*/
void CSsmSwpTransitionEngine::Start()
	{
	SetActive ();
	TRequestStatus* status = &iStatus;
	User::RequestComplete (status, KErrNone);
	}

/**
Cancel the running of the engine
Cancel any running command lists
Complete the client request if supplied
*/
void CSsmSwpTransitionEngine::DoCancel()
	{
	__ASSERT_DEBUG(iPolicyFrame, PanicNow(KPanicSysStateMgr,ESwpTransitionEngineError7));

	TInt err = KErrCancel;
	//Cancel any request this engine is waiting for
	if( iNextAction == EIdle)
		{
		__ASSERT_DEBUG(iClientStatus==NULL, PanicNow(KPanicSysStateMgr,ESwpTransitionEngineError17));
		return;
		}
	else if( iPolicyFrame && (iNextAction == ETransitionAllowed) )
		{
		err = iPolicyFrame->CallInitializeCancel();
		}
	else if( iPolicyFrame && (iNextAction == EExecuteCommandList) )
		{
		err = iPolicyFrame->CallPrepareCommandListCancel();
		}
	else if( iCleSession && (iNextAction == EWaitForCommandListExecute) )
		{
		iCleSession->ExecuteCommandListCancel();
		}
	else if( iPolicyFrame && (iNextAction == EWaitForHandleCleReturnValue) )
		{
		err = iPolicyFrame->CallHandleCleReturnValueCancel();
		//iCommandList needs to be deleted when cancel has been called before completion of command 
        if (iCommandList)
            {
            delete iCommandList;
            iCommandList = NULL;
            }
		}

	//Then complete the transition-scheduler's TRequestStatus
	if( NULL != iClientStatus )
		{
		User::RequestComplete (iClientStatus, err);
		iClientStatus = NULL;
		}

	//And let the end-user know
	CompleteClientMessage (err);

	iNextAction = EIdle;
	}

/**
The engine runs through a fixed sequence of steps, most of which require an asynchronous reply from
another component in the system

@leave One of the system-wide error codes
*/

void CSsmSwpTransitionEngine::RunL()
	{
	if(iNextAction != EWaitForCommandListExecute)
		{
		//Propagate all problems except CleSrv's return code to RunError
		SSMLOGLEAVEIFERROR(iStatus.Int()); //will leave in release builds as well
		}

	switch (iNextAction)
		{
		case EUnConnected:
			DoConnectCleSessionL();
			iNextAction = ESetSwpPolicyDllHandleAndInitialize;
			break;

		case ESetSwpPolicyDllHandleAndInitialize:
			DoInitialize();
			iNextAction = ETransitionAllowed;
			break;

		case ETransitionAllowed:
			iNextAction = DoTransitionAllowed() ? EPrepareCommandList : EIdle;
			break;

		case EPrepareCommandList:
			DoPrepareCommandList();
			iNextAction = EExecuteCommandList;
			break;

		case EExecuteCommandList:
			DoExecuteCommandList();
			iNextAction = EWaitForCommandListExecute;
			break;

		case EWaitForCommandListExecute:
            if (iCommandList)
                {
                delete iCommandList;
                iCommandList = NULL;
                }
			DoHandleCleReturnValue();
			iNextAction = EWaitForHandleCleReturnValue;
			break;

		case EWaitForHandleCleReturnValue:
			if(NULL != iClientStatus)
				{
				User::RequestComplete(iClientStatus, KErrNone);
				iClientStatus = NULL;
				}
			iNextAction = ETransitionComplete;
			break;

		case EIdle:
		default:
			__ASSERT_DEBUG(EFalse, PanicNow(KPanicSysStateMgr,ESwpTransitionEngineError19));
			break;
		}
	}

TBool CSsmSwpTransitionEngine::InTransition() const
	{
	return (iNextAction > ETransitionAllowed);
	}

/**
 * Returns the current SwP value for this transition engine.
 */
const TSsmSwp& CSsmSwpTransitionEngine::TransitionValue() const
	{
	return iCurrentSwpValue;
	}

/**
 * Returns the MCleSessionProxy currently associated with this engine
 */
MCleSessionProxy* CSsmSwpTransitionEngine::CleSessionProxy()
	{
	return iCleSession;
	}

/**
 Most errors cause a Panic followed by Kernel fault. The rationale behind this is that all
 swp policies placed on the rom by the device manufacturer must be free from errors when
 a device is shipped. There is no case for graceful handling of missing commandlists,
 bad resource files or an unknown swp.
 In case there is low memory condition then we are completing the client request status with KErrNoMemory,
 instead of panicking the client.

 Note: Both the return value from CleSrv (which is forwarded to the Swp Policy DLL) and the error resulting
 	   from a transition not allowed do NOT cause this RunError() to be called.
*/
TInt CSsmSwpTransitionEngine::RunError(TInt aError)
	{
	DoRunError(aError);
	return KErrNone;
	}

void CSsmSwpTransitionEngine::DoRunError(TInt aError)
	{
	if(NULL != iClientStatus && aError == KErrNoMemory)
		{
		User::RequestComplete(iClientStatus, aError);
		iClientStatus = NULL;

		//And let the end-user know
		CompleteClientMessage (aError);
		}
	else
		{
		DEBUGPRINT3(_L("CSsmSwpTransitionEngine RunError called with aError: %d when iNextAction had the value of: %d"), aError, iNextAction);
		PanicNow(KPanicSysStateMgr,ESwpTransitionEngineError20);
		}
	}

/**
 Before this transition engine can be used, we need to connect to (and probably start) the CleSrv.
 The session remains connected as long as Cle does not disconnect itself (due to a panic for example)
 */
void CSsmSwpTransitionEngine::DoConnectCleSessionL()
	{
	iCleSession->ConnectL();
	Start();
	}

/**
Call the policy's Initialize method
Wait asynchronously for completion
*/
void CSsmSwpTransitionEngine::DoInitialize()
	{
	__ASSERT_DEBUG(iPolicyFrame, PanicNow(KPanicSysStateMgr,ESwpTransitionEngineError8));
	//Let the swp policy do whatever it needs to do e.g. initialize its CSsmCommandListResourceReader
	iPolicyFrame->CallInitialize (iStatus);
	SetActive ();
	}

/**
Call the policy's TransitionAllowed method and handle return value appropriately
Continue with sequence if allowed
Complete message and request with correct status if not allowed
*/
TBool CSsmSwpTransitionEngine::DoTransitionAllowed()
	{
	__ASSERT_DEBUG(iPolicyFrame, PanicNow(KPanicSysStateMgr,ESwpTransitionEngineError9));

	MSsmSwpPolicy::TResponse response = iPolicyFrame->CallTransitionAllowed(iCurrentSwpValue, iClientMessage);

	if( MSsmSwpPolicy::EAllowed == response) // continue as normal
		{
		CompleteClientMessage(KErrNone);	// tell the caller we were accepted

		Start();
		return ETrue;
		}
	else // complete with error status
		{
		CompleteClientMessage (KErrNotSupported); // tell the caller we were rejected

		if ( NULL != iClientStatus)
			{
			User::RequestComplete (iClientStatus, KErrNotSupported);
			iClientStatus = NULL;
			}
		return EFalse;
		}
	}

/**
Call the policy's PrepareCommandList method and wait for completion
*/
void CSsmSwpTransitionEngine::DoPrepareCommandList()
	{
	__ASSERT_DEBUG(iPolicyFrame, PanicNow(KPanicSysStateMgr,ESwpTransitionEngineError10));
	iPolicyFrame->CallPrepareCommandList (iCurrentSwpValue, iStatus);
	SetActive ();
	}

/**
Retrieve the prepared command list
Request execution of the command list
Wait asynchronously for execution to complete

@leave KErrNoMemory if the list is NULL
*/
void CSsmSwpTransitionEngine::DoExecuteCommandList()
	{
	__ASSERT_DEBUG(iPolicyFrame, PanicNow(KPanicSysStateMgr,ESwpTransitionEngineError11));
	__ASSERT_DEBUG(iCleSession, PanicNow(KPanicSysStateMgr,ESwpTransitionEngineError13));

	iCommandList = iPolicyFrame->CallCommandList();
	if(iPerformCommandListValidation)
		{
		DoValidation();
		}

	if(iCommandList)
		{
		iCleSession->ExecuteCommandList(*iCommandList, iStatus, iSeverity);
		SetActive ();
		}
	else
		{
		Start();
		}
	}

void CSsmSwpTransitionEngine::DoValidation()
	{
	TBool valid = EFalse;
	if(!iCommandList)
		{
		DEBUGPRINT1(_L ("System-Wide-Property DLL's CommandList() function returned NULL"));
		}
	else
		{
		valid = CSsmCommandListUtils::IsValidSwpList(*iCommandList);
		if(!valid)
			{
			DEBUGPRINT1(_L("System-Wide-Property DLL's CommandList() function returned an invalid CommandList()"));
			}
		}
	__ASSERT_ALWAYS( iCommandList && valid, PanicNow(KPanicSysStateMgr,ESwpTransitionEngineError16));
	}

/**
Pass the request status returned by the CLE to the policy
Wait asynchronously for the policy to complete the handling
*/

void CSsmSwpTransitionEngine::DoHandleCleReturnValue()
	{
	__ASSERT_DEBUG(iPolicyFrame, PanicNow(KPanicSysStateMgr,ESwpTransitionEngineError12));
	const TInt error = iStatus.Int ();

	// In case the server is no longer present
	if(error == KErrServerTerminated)
		{
		// Close cle handle and mark it as disconnected
		iCleSession->Close();
		}

	// Severity must be obtained from CLE somehow. This needs proper implementation.
	TCmdErrorSeverity severity = ECmdIgnoreFailure;
	if(KErrNone != error)
		{
		DEBUGPRINT3(_L("Commandlist execution reported error: %d, with severity %d."), error, severity);
		severity = ECmdCriticalSeverity;
		}

	iPolicyFrame->CallHandleCleReturnValue(iCurrentSwpValue, error, severity, iStatus);
	SetActive ();
	}

/**
Complete the client message if not already done.
This should be done only for the first time.
After that the client message is invalid
*/
void CSsmSwpTransitionEngine::CompleteClientMessage(TInt aError)
	{
	if( iClientMessage.Handle() )
		{
		iClientMessage.Complete (aError);
		}
	}

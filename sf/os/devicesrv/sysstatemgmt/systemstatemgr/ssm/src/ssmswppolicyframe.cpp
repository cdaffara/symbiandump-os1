// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32cmn.h>

#include <ssm/ssmswp.h>
#include "ssmswppolicyframe.h"

#include "ssmdebug.h"
#include "ssmserverpanic.h"
#include "ssmswppolicyproxy.h"


/**
signature of expected DLL factory function
*/
typedef MSsmSwpPolicy*(*TFuncSwpNewL)(void);

/**
Trivial constructor
*/
CSsmSwpPolicyFrame::CSsmSwpPolicyFrame()
	{
	}

/**
Static method to create a new frame and instantiate a policy using the
supplied factory method

@param aNewLFunc - a factory method that creates a new policy
                   (the frame takes ownership)
@return a new frame object that encapsulates a valid policy
*/
CSsmSwpPolicyFrame* CSsmSwpPolicyFrame::NewL(TLibraryFunction aNewLFunc)
	{
	CSsmSwpPolicyFrame* self = new (ELeave) CSsmSwpPolicyFrame();
	CleanupStack::PushL (self);
	self->ConstructL (aNewLFunc);
	CleanupStack::Pop (self);
	return self;
	}

/**
Construct the frame and create the policy

@param aNewLFunc - a factory method that creates a new policy
@return a new frame object that encapsulates a valid policy
*/
void CSsmSwpPolicyFrame::ConstructL(TLibraryFunction aNewLFunc)
	{
	TFuncSwpNewL newL = reinterpret_cast<TFuncSwpNewL>( aNewLFunc );
	iSwpPolicy = newL ();
#ifdef _DEBUG
	if(!iSwpPolicy)
		{
		DEBUGPRINT1(_L("First function in Swp Policy DLL didn't create a new instance"));
		}
#endif
	SSMLOGLEAVEIFNULL(iSwpPolicy);
	}

/**
Destroy the owned policy and release the library (if available).
*/
CSsmSwpPolicyFrame::~CSsmSwpPolicyFrame()
	{
	//Let the derived implementation handle its own destruction
	if(iSwpPolicy)
		{
		iSwpPolicy->Release();
		iSwpPolicy = NULL;
		iLibrary.Close(); // release the dll
		}
	}

/**
Return the swp key that this frame is associated with

@return this frame's swp value

*/
TUint CSsmSwpPolicyFrame::SwpKey() const
	{
	return iSwpKey;
	}

/**
Set the swp key that this frame is associated with

@param a valid swp key that matches the loaded policy

*/
void CSsmSwpPolicyFrame::SetSwpKey(TUint aKey)
	{
	iSwpKey = aKey;
	}  //lint !e1746 Suppress parameter 'aKey' could be made const reference

/**
 Takes ownership of open handle
 */
void CSsmSwpPolicyFrame::SetLibrary(const RLibrary& aLibrary)
	{
	iLibrary = aLibrary;
	}

void CSsmSwpPolicyFrame::SetSsmSwpPolicySession(CSsmSwpPolicyCliSession* aSsmSwpPolicySession)
	{
	iSsmSwpPolicySession = aSsmSwpPolicySession;
	}

/**
Accessor to the policy's asynchronous Initialize method

@param aStatus to inform the caller of completion.
Note: the possible values for the completion status are defined by the policy method

*/
void CSsmSwpPolicyFrame::CallInitialize(TRequestStatus& aStatus)
	{
	__ASSERT_DEBUG(NULL != iSwpPolicy, PanicNow(KPanicSysStateMgr,ESwpPolicyFrameError1));
	__ASSERT_DEBUG(NULL != iSsmSwpPolicySession, PanicNow(KPanicSysStateMgr,ESwpPolicyFrameError18));

	// lazy initialisation
	// if initialization is already called then we are simply completing the request with KErrNone.
	if (!iInitializeCalled)
		{
		iSsmSwpPolicySession->CallSetDllHandleAndInitialize(iLibrary.Handle(), aStatus);
		iInitializeCalled = ETrue;
		iInternalState = EWaitForPrepare;
		}
	else // complete
		{
		DEBUGPRINT1(_L("SwpPolicy is already initialized"));
		TRequestStatus* status = &aStatus;
		User::RequestComplete (status, KErrNone);
		}
	}

/**
Accessor to the policy's synchronous Cancel method

*/
TInt CSsmSwpPolicyFrame::CallInitializeCancel()
	{
	__ASSERT_DEBUG(NULL != iSwpPolicy, PanicNow(KPanicSysStateMgr,ESwpPolicyFrameError15));
	__ASSERT_DEBUG(NULL != iSsmSwpPolicySession, PanicNow(KPanicSysStateMgr,ESwpPolicyFrameError22));

	TInt err = KErrNone;
	if (iInitializeCalled)
		{
		err = iSsmSwpPolicySession->CallInitializeCancel();
		iInternalState = EWaitForPrepare;
		}
	return err;
	}

/**
Accessor to the policy's synchronous TransitionAllowed method

@param aSwp swp value to interrogate
@param aMessage Contains information about the requesting client process.
@return the response to the query
*/
MSsmSwpPolicy::TResponse CSsmSwpPolicyFrame::CallTransitionAllowed(const TSsmSwp& aSwp, const RMessagePtr2& aMessage)
	{
	// CallTransitionAllowed can be called at any time as long as the object has been initialised
	__ASSERT_DEBUG(NULL != iSwpPolicy, PanicNow(KPanicSysStateMgr,ESwpPolicyFrameError3));
	__ASSERT_DEBUG(iInitializeCalled, PanicNow(KPanicSysStateMgr,ESwpPolicyFrameError4));

	MSsmSwpPolicy::TResponse response = MSsmSwpPolicy::ENotAllowed;

	// Since this is a synchronous call we are not using iSsmSwpPolicySession
	// for enquiring about response returned from TransitionAllowed.
	if (iInitializeCalled)
		{
		response = iSwpPolicy->TransitionAllowed(aSwp, aMessage);
		}
	return response;
	}

/**
Accessor to the policy's asynchronous PrepareCommandList method

@param aSwp swp value to interrogate
@param aStatus to inform the caller of completion
Note: the possible values for the completion status are defined by the policy method

*/
void CSsmSwpPolicyFrame::CallPrepareCommandList(const TSsmSwp& aSwp, TRequestStatus& aStatus)
	{
	__ASSERT_DEBUG(EWaitForPrepare == iInternalState || EWaitForCommandListExecution == iInternalState, PanicNow(KPanicSysStateMgr,ESwpPolicyFrameError5));
	__ASSERT_DEBUG(aSwp.Key() == iSwpKey, PanicNow(KPanicSysStateMgr,ESwpPolicyFrameError6));

	__ASSERT_DEBUG(NULL != iSwpPolicy, PanicNow(KPanicSysStateMgr,ESwpPolicyFrameError7));
	__ASSERT_DEBUG(NULL != iSsmSwpPolicySession, PanicNow(KPanicSysStateMgr,ESwpPolicyFrameError19));
	__ASSERT_DEBUG(iInitializeCalled, PanicNow(KPanicSysStateMgr,ESwpPolicyFrameError8));

	// if initialization is not called then we are simply completing the request with KErrNone.
	if (iInitializeCalled)
		{
		iSsmSwpPolicySession->CallPrepareCommandList(aSwp, aStatus);
		iInternalState = EWaitForGet;
		}
	else // complete
		{
		DEBUGPRINT1(_L("SwpPolicy is not initialized"));
		TRequestStatus* status = &aStatus;
		User::RequestComplete (status, KErrNone);
		}

	} //lint !e1746 Suppress parameter 'aSwp' could be made const reference

/**
Accessor to the policy's synchronous Cancel method

*/
TInt CSsmSwpPolicyFrame::CallPrepareCommandListCancel()
	{
	__ASSERT_DEBUG(NULL != iSwpPolicy, PanicNow(KPanicSysStateMgr,ESwpPolicyFrameError16));
	__ASSERT_DEBUG(NULL != iSsmSwpPolicySession, PanicNow(KPanicSysStateMgr,ESwpPolicyFrameError23));

	TInt err = KErrNone;
	// iSsmSwpPolicySession is the session with ssmswppolicycli
	if (iInitializeCalled)
		{
		err = iSsmSwpPolicySession->CallPrepareCommandListCancel();
		iInternalState = EWaitForPrepare;
		}
	return err;
	}

/**
Accessor to the policy's synchronous CommandList method

@return a valid command list for execution or NULL

*/
CSsmCommandList* CSsmSwpPolicyFrame::CallCommandList()
	{
	__ASSERT_DEBUG(EWaitForGet == iInternalState, PanicNow(KPanicSysStateMgr,ESwpPolicyFrameError9));
	__ASSERT_DEBUG(NULL != iSwpPolicy, PanicNow(KPanicSysStateMgr,ESwpPolicyFrameError10));
	__ASSERT_DEBUG(NULL != iSsmSwpPolicySession, PanicNow(KPanicSysStateMgr,ESwpPolicyFrameError20));
	__ASSERT_DEBUG(iInitializeCalled, PanicNow(KPanicSysStateMgr,ESwpPolicyFrameError11));

	CSsmCommandList* list=  NULL;
	if (iInitializeCalled)
		{
		list = iSsmSwpPolicySession->CallCommandList();
		iInternalState = EWaitForCommandListExecution;
		}
	return list;
	}

/**
Accessor to the policy's asynchronous HandleCleReturnValue method

@param TInt the value returned by the CLE
@param aStatus to inform the caller of completion
Note: the possible values for the completion status are defined by the policy method

*/
void CSsmSwpPolicyFrame::CallHandleCleReturnValue(const TSsmSwp& aSwp, TInt aError, TInt aSeverity, TRequestStatus& aStatus)
	{
	__ASSERT_DEBUG(EWaitForCommandListExecution == iInternalState, PanicNow(KPanicSysStateMgr,ESwpPolicyFrameError12));
	__ASSERT_DEBUG(NULL != iSwpPolicy, PanicNow(KPanicSysStateMgr,ESwpPolicyFrameError13));
	__ASSERT_DEBUG(NULL != iSsmSwpPolicySession, PanicNow(KPanicSysStateMgr,ESwpPolicyFrameError21));
	__ASSERT_DEBUG(iInitializeCalled, PanicNow(KPanicSysStateMgr,ESwpPolicyFrameError14));

	// If initialization is not called then we are simply completing the request with KErrNone.
	if (iInitializeCalled)
		{
		iSsmSwpPolicySession->CallHandleCleReturnValue(aSwp, aError, aSeverity, aStatus);
		iInternalState = EWaitForPrepare;
		}
	else // complete
		{
		DEBUGPRINT1(_L("SwpPolicy is not initialized"));
		TRequestStatus* status = &aStatus;
		User::RequestComplete(status, KErrNone);
		}
	}

/**
Accessor to the policy's synchronous Cancel method

*/
TInt CSsmSwpPolicyFrame::CallHandleCleReturnValueCancel()
	{
	__ASSERT_DEBUG(EWaitForPrepare == iInternalState, PanicNow(KPanicSysStateMgr,ESwpPolicyFrameError17));
	__ASSERT_DEBUG(NULL != iSwpPolicy, PanicNow(KPanicSysStateMgr,ESwpPolicyFrameError2));
	__ASSERT_DEBUG(NULL != iSsmSwpPolicySession, PanicNow(KPanicSysStateMgr,ESwpPolicyFrameError24));
	__ASSERT_DEBUG(iInitializeCalled, PanicNow(KPanicSysStateMgr,ESwpPolicyFrameError14));

	TInt err = iSsmSwpPolicySession->CallHandleCleReturnValueCancel();
	iInternalState = EWaitForPrepare;
	return err;
	}

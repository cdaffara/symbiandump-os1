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
// Base state transition classes  for the CLbsSuplPushImpl class
// 
//

#include "lbssystemcontroller.h"

#include "Te_LbsSuplPushImpl.h"
#include "Te_LbsSuplPushBaseTrans.h"
#include "Te_LbsSuplPushStateStep.h"
#include "Te_LbsSuplErrorPattern.h"


/**
Constructor

@param aStep [In] A reference on the test step owning this object
*/
CTe_LbsSuplPushBaseTrans::CTe_LbsSuplPushBaseTrans(CTe_LbsSuplPushStateStep& aStep) :
iStep(aStep)
	{
	//Intentionally left blank	
	}

/**
Receives notification when a SUPL INIT message was sent to the LBS sub-system or failed.
Overrides the pure virtual MLbsSuplPushObserver::OnSuplInitComplete.

The implementation here stops the CActiveSchedulerWait object.

@see MLbsSuplPushObserver
*/
void CTe_LbsSuplPushBaseTrans::OnSuplInitComplete(TLbsSuplPushChannel /*aChannel*/, 
		TLbsSuplPushRequestId /*aReqId*/, TInt /*aError*/, TInt /*aReserved*/)
	{
	if(iStep.ActiveSchedulerWait().IsStarted())
		{
		iStep.ActiveSchedulerWait().AsyncStop();
		}
	}

/**
Constructor

@param aStep [In] A reference on the test step owning this object
*/
CTe_LbsSuplPushBaseSuplInit::CTe_LbsSuplPushBaseSuplInit(CTe_LbsSuplPushStateStep& aStep) :
CTe_LbsSuplPushBaseTrans(aStep)
	{
	//Intentionally left blank
	}	

/**
Runs the various scenarios for this transition. Overrides the pure virtual 
CTe_LbsSuplPushBaseTrans::RunTransL.

@leave If a error happens, it leaves with one of the system error codes.
*/
void CTe_LbsSuplPushBaseSuplInit::RunTransL()
	{	
	TLbsSuplPushRequestId reqId;
	
	//Zero length message - KErrArgument
	CLbsSuplPushImpl* pushImpl = MakeSuplPushImplLC();
	CTe_LbsSuplPushImpl::DisableAsserts();
	if(pushImpl->SuplInit(reqId, KNullDesC8, 0)!=KErrArgument)
		{
		User::Leave(KErrGeneral);
		}
	pushImpl->CheckStateL(State());
	CTe_LbsSuplPushImpl::EnableAsserts();
	CleanupStack::PopAndDestroy(pushImpl);
	
	
	
	//Very large message
	pushImpl = MakeSuplPushImplLC();
	HBufC8* buf = HBufC8::NewLC(4096);
	buf->Des().Fill('a', 4096);
	CTe_LbsSuplPushImpl::DisableAsserts();
	if(pushImpl->SuplInit(reqId, *buf, 0)!=KErrArgument)
		{
		User::Leave(KErrGeneral);
		}
	pushImpl->CheckStateL(State());
	CTe_LbsSuplPushImpl::EnableAsserts();
	CleanupStack::PopAndDestroy(buf);
	CleanupStack::PopAndDestroy(pushImpl);
	
	
	//Error starting lbs - commented as transient startup is not supported currently
	/*
	pushImpl = MakeSuplPushImplLC();
	RTe_LbsSuplErrorPattern::Reset();
	RTe_LbsSuplErrorPattern::AppendNextErrorL(
			Te_LbsSuplErrorId::ERLbsSystemController_RequestSystemStartup,
			1, ELbsStartupFail, EFalse);
	if(pushImpl->SuplInit(reqId, _L8("abcd"), 0)!=KErrNotReady)
		{
		User::Leave(KErrGeneral);
		}
	__ASSERT_ALWAYS(RTe_LbsSuplErrorPattern::ErrorReached()==1, User::Invariant());
	pushImpl->CheckStateL(State());
	RTe_LbsSuplErrorPattern::Reset();
	CleanupStack::PopAndDestroy(pushImpl);
	*/
	
	//Wrong BUSY property
	pushImpl = MakeSuplPushImplLC();
	CTe_LbsSuplPushImpl::DisableAsserts();
	TRAPD(check, pushImpl->CheckStateL(CLbsSuplPushImpl::ECreated));
	if(check==KErrNone)
		{
		//to switch to EWaitingAck state
		if(pushImpl->SuplInit(reqId, _L8("abcd"), 0)!=KErrNone)
			{
			User::Leave(KErrGeneral);
			}
		pushImpl->CheckStateL(CLbsSuplPushImpl::EWaitingAck);
	}
	User::LeaveIfError(RProperty::Set(iStep.PropOwnerSecureId(), iStep.BusyPropKey(), 0));
	if(pushImpl->SuplInit(reqId, _L8("abcd"), 0)!=KErrGeneral)
		{
		User::Leave(KErrGeneral);
		}
	pushImpl->CheckStateL(WrongBusyPropState());
	CleanupStack::PopAndDestroy(pushImpl);
	CTe_LbsSuplPushImpl::EnableAsserts();
	

	//Test RProperty::Get method errors
	pushImpl = MakeSuplPushImplLC();
	TInt count=1;
	do	{
		RTe_LbsSuplErrorPattern::Reset();
		RTe_LbsSuplErrorPattern::AppendNextErrorL(Te_LbsSuplErrorId::ERProperty_Get,
				count, KErrGeneral, EFalse);
			
		CTe_LbsSuplPushImpl::DisableAsserts();
		TInt err = pushImpl->SuplInit(reqId, _L8("abcd"),0);
		CTe_LbsSuplPushImpl::EnableAsserts();
		
		if(RTe_LbsSuplErrorPattern::ErrorReached()==1 && err!=KErrGeneral)
			{
			User::LeaveIfError(err);
			}
		
		pushImpl->CheckStateL(RPropertyGetErrorState());
		count++;
		}
	while(RTe_LbsSuplErrorPattern::ErrorReached()==1);	
	RTe_LbsSuplErrorPattern::Reset();	
	CleanupStack::PopAndDestroy(pushImpl);	
	
	//Test RProperty::Set method errors
	pushImpl = MakeSuplPushImplLC();
	count=1;
	do	{
		RTe_LbsSuplErrorPattern::Reset();
		RTe_LbsSuplErrorPattern::AppendNextErrorL(Te_LbsSuplErrorId::ERProperty_Set,
				count, KErrGeneral, EFalse);
			
		CTe_LbsSuplPushImpl::DisableAsserts();
		TInt err = pushImpl->SuplInit(reqId, _L8("abcd"), 0);
		CTe_LbsSuplPushImpl::EnableAsserts();
		
		if(RTe_LbsSuplErrorPattern::ErrorReached()==1 && err!=KErrGeneral)
			{
			User::LeaveIfError(err);
			}
		
		pushImpl->CheckStateL(RPropertySetErrorState());
		count++;
		}
	while(RTe_LbsSuplErrorPattern::ErrorReached()==1);	
	RTe_LbsSuplErrorPattern::Reset();	
	CleanupStack::PopAndDestroy(pushImpl);	
	}

/**
Constructor

@param aStep [In] A reference on the test step owning this object
*/
CTe_LbsSuplPushBaseOnTimerEvent::CTe_LbsSuplPushBaseOnTimerEvent(CTe_LbsSuplPushStateStep& aStep) :
CTe_LbsSuplPushBaseTrans(aStep)
	{
	}

/**
Runs the various scenarios for this transition. Overrides the pure virtual 
CTe_LbsSuplPushBaseTrans::RunTransL.

@leave If a error happens, it leaves with one of the system error codes.
*/
void CTe_LbsSuplPushBaseOnTimerEvent::RunTransL()
	{
	//Success, a single message in the queue
	CLbsSuplPushImpl* pushImpl = MakeSuplPushImplLC();
	CTe_LbsSuplPushImpl::DisableAsserts();
	iStep.ActiveSchedulerWait().Start();
	pushImpl->CheckStateL(SingleSuccessState());
	CTe_LbsSuplPushImpl::EnableAsserts();
	CleanupStack::PopAndDestroy(pushImpl);
	
	//Success, two messages in the queue
	pushImpl = MakeSuplPushImplLC();
	User::After(1000000);
	TLbsSuplPushRequestId reqId;
	User::LeaveIfError(pushImpl->SuplInit(reqId, _L8("abcd"), 0));
	CTe_LbsSuplPushImpl::DisableAsserts();
	iStep.ActiveSchedulerWait().Start();
	pushImpl->CheckStateL(SeveralSuccessState());
	CTe_LbsSuplPushImpl::EnableAsserts();
	CleanupStack::PopAndDestroy(pushImpl);
	
	RTe_LbsSuplErrorPattern::Reset();
	
	CheckErrorsL(Te_LbsSuplErrorId::ERProperty_Get, KErrGeneral, 1);
	CheckErrorsL(Te_LbsSuplErrorId::ERProperty_Get, KErrGeneral, 2);
	CheckErrorsL(Te_LbsSuplErrorId::ERProperty_Get, KErrGeneral, 5);
	CheckErrorsL(Te_LbsSuplErrorId::ERProperty_Set, KErrGeneral, 1);
	CheckErrorsL(Te_LbsSuplErrorId::ERProperty_Set, KErrGeneral, 2);
	CheckErrorsL(Te_LbsSuplErrorId::ERProperty_Set, KErrGeneral, 5);
	}

/**
Applies different error patterns and checks the result.

@param aFunction [In] The function that fails.
@param aError [In] The error the function fails with.
@param aMessagesInQueue [In] Number of messages in the queue before the transition.

@leave If a error happens, it leaves with one of the system error codes.
*/
void CTe_LbsSuplPushBaseOnTimerEvent::CheckErrorsL(TInt aFunction, TInt aError, 
		TUint aMessagesInQueue)
	{
	
	__ASSERT_ALWAYS(aMessagesInQueue>0, User::Invariant());
	TInt count=1;
	TInt errorReached = 0;
	do	{
		//Reinitialize BUSY prop here to avoid reuse of the old message id, which is incorrect
		//from the CLbsSuplPushRec point of view. It considers it as an old message and ignores it.
		User::LeaveIfError(RProperty::Set(iStep.PropOwnerSecureId(), iStep.BusyPropKey(), KMaxTInt-10));
		CLbsSuplPushImpl* pushImpl = MakeSuplPushImplLC();
		TLbsSuplPushRequestId reqId;
		//Restore the BUSY property to normal
		for(TUint i=1;i<aMessagesInQueue;i++)
			{
			User::After(2000000/aMessagesInQueue);
			User::LeaveIfError(pushImpl->SuplInit(reqId, _L8("abcd"), 0));
			}
		
		RTe_LbsSuplErrorPattern::AppendNextErrorL(aFunction,
				count, aError, EFalse);
			
		CTe_LbsSuplPushImpl::DisableAsserts();
		iStep.ActiveSchedulerWait().Start();
		errorReached = RTe_LbsSuplErrorPattern::ErrorReached();
		RTe_LbsSuplErrorPattern::Reset();
		CTe_LbsSuplPushImpl::EnableAsserts();
		
		if(errorReached==1)
			{
			if(aMessagesInQueue==1)
				{
				pushImpl->CheckStateL(SingleErrorState());
				}
			else
				{
				pushImpl->CheckStateL(SeveralErrorState());
				}
			}
		else
			{
			if(aMessagesInQueue==1)
				{
				pushImpl->CheckStateL(SingleSuccessState());
				}
			else
				{
				pushImpl->CheckStateL(SeveralSuccessState());
				}
			}
		CleanupStack::PopAndDestroy(pushImpl);	
		count++;
		}
	while(errorReached==1);	
	}


/**
Receives notification when a SUPL INIT message was sent to the LBS sub-system or failed.
Overrides the virtual CTe_LbsSuplPushBaseTrans::OnSuplInitComplete.

The implementation here stops the CActiveSchedulerWait object and checks that aError==KErrTimedOut.

@param aError [In] 	KErrNone if successful, KErrTimeout if it was not possible to deliver
	                the request before the timeout period, KErrArgument if the structure 
	                or content of the SUPL INIT message was incorrect. 
	                Any system wide error code otherwise.

@see MLbsSuplPushObserver
@see CTe_LbsSuplPushBaseTrans
*/
void CTe_LbsSuplPushBaseOnTimerEvent::OnSuplInitComplete(TLbsSuplPushRequestId /*aReqId*/, 
		TInt aError, TInt /*aReserved*/)
	{
	if(aError!=KErrTimedOut)
		{
		iStep.SetTestStepResult(EFail);
		}
	if(iStep.ActiveSchedulerWait().IsStarted())
		{
		iStep.ActiveSchedulerWait().AsyncStop();
		}
	}


/**
Constructor

@param aStep [In] A reference on the test step owning this object
*/
CTe_LbsSuplPushBaseRunL::CTe_LbsSuplPushBaseRunL(CTe_LbsSuplPushStateStep& aStep) :
CTe_LbsSuplPushBaseTrans(aStep)
	{
	//Intentionally left blank
	}

/**
Runs the various scenarios for this transition. Overrides the pure virtual 
CTe_LbsSuplPushBaseTrans::RunTransL.

@leave If a error happens, it leaves with one of the system error codes.
*/
void CTe_LbsSuplPushBaseRunL::RunTransL()
	{
	//Error - subscribe attempt fails, a single message in the queue
	CLbsSuplPushImpl* pushImpl = MakeSuplPushImplLC();
	CTe_LbsSuplPushImpl::DisableAsserts();
	pushImpl->Cancel(); //we must cancel it here to avoid a stray signal later
	TRequestStatus* status = &pushImpl->iStatus;
	User::RequestComplete(status, KErrGeneral);
	CTe_LbsSuplPushImpl::SetActive(*pushImpl); 
	iStep.ActiveSchedulerWait().Start();
	pushImpl->CheckStateL(SingleErrorState());
	CTe_LbsSuplPushImpl::EnableAsserts();
	CleanupStack::PopAndDestroy(pushImpl);	
	}






	


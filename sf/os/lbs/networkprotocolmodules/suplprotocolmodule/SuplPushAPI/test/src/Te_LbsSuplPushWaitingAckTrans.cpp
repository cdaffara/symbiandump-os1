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
// State transitions for the CLbsSuplPushImpl class from the EWaitingAck state
// 
//

#include "Te_LbsSuplPushImpl.h"
#include "Te_LbsSuplPushWaitingAckTrans.h"
#include "Te_LbsSuplPushStateStep.h"
#include "Te_LbsSuplErrorPattern.h"

/**
Creates a SuplInit from EWaitingAck state transition object and puts it into the cleanup stack.

@param aStep [In] A reference on the test step owning this object.

@return A pointer on the created transition.
 
@leave If a error happens, it leaves with one of the system error codes.
*/
CTe_LbsSuplPushWaitingAckSuplInit* 
CTe_LbsSuplPushWaitingAckSuplInit::NewLC(CTe_LbsSuplPushStateStep& aStep)
	{
	CTe_LbsSuplPushWaitingAckSuplInit* self = 
			new (ELeave) CTe_LbsSuplPushWaitingAckSuplInit(aStep);
	CleanupStack::PushL(self);
	return self;
	}


/**
Constructor

@param aStep [In] A reference on the test step owning this object
*/
CTe_LbsSuplPushWaitingAckSuplInit::CTe_LbsSuplPushWaitingAckSuplInit(
		CTe_LbsSuplPushStateStep& aStep) : CTe_LbsSuplPushBaseSuplInit(aStep)
	{
	//Intentionally left blank	
	}

/**
Constructs the CSuplPushImpl object in the EWaitingAck state. Places the created object on 
the cleanup stack.
	
@return A pointer on the created CSuplPushImpl object.
	
@leave If a error happens, it leaves with one of the system error codes.

@see CTe_LbsSuplPushBaseTrans::MakeSuplPushImplLC
*/
CLbsSuplPushImpl* CTe_LbsSuplPushWaitingAckSuplInit::MakeSuplPushImplLC()
	{
	return iStep.MakeWaitingAckSuplPushImplLC(*this);
	}

/**
Returns the possible main states the object is after the transition.
	
@return The possible main states the object is after the transition.

@see CTe_LbsSuplPushBaseSuplInit::State
*/
TInt CTe_LbsSuplPushWaitingAckSuplInit::State()
	{
	return CLbsSuplPushImpl::EWaitingAck;
	}

/**
Returns the possible states object switches to after the transition when there is a wrong
BUSY property set.
	
@return The possible states object switches to after the transition when there is a wrong
BUSY property set.

@see CTe_LbsSuplPushBaseSuplInit::WrongBusyPropState
*/
TInt CTe_LbsSuplPushWaitingAckSuplInit::WrongBusyPropState()
	{
	return CLbsSuplPushImpl::EWaitingAck;
	}

/**
Returns the possible states object switches to after the transition when the RProperty::Get
function fails. 
	
@return The possible states object switches to after the transition when the RProperty::Get
function fails.

@see CTe_LbsSuplPushBaseSuplInit::RPropertyGetErrorState
*/
TInt CTe_LbsSuplPushWaitingAckSuplInit::RPropertyGetErrorState()
	{
	return CLbsSuplPushImpl::EWaitingAck;
	}

/**
Returns the possible states object switches to after the transition when the RProperty::Set
function fails. 
	
@return The possible states object switches to after the transition when the RProperty::Set
function fails.

@see CTe_LbsSuplPushBaseSuplInit::RPropertySetErrorState
*/
TInt CTe_LbsSuplPushWaitingAckSuplInit::RPropertySetErrorState()
	{
	return CLbsSuplPushImpl::EWaitingAck;
	}



/**
Creates a OnTimerEvent from EWaitingAck state transition object and puts it into the cleanup stack.

@param aStep [In] A reference on the test step owning this object.

@return A pointer on the created transition.
 
@leave If a error happens, it leaves with one of the system error codes.
*/
CTe_LbsSuplPushWaitingAckOnTimerEvent* 
CTe_LbsSuplPushWaitingAckOnTimerEvent::NewLC(CTe_LbsSuplPushStateStep& aStep)
	{
	CTe_LbsSuplPushWaitingAckOnTimerEvent* self = 
		new (ELeave) CTe_LbsSuplPushWaitingAckOnTimerEvent(aStep);
	CleanupStack::PushL(self);
	return self;
	}

/**
Constructor

@param aStep [In] A reference on the test step owning this object
*/
CTe_LbsSuplPushWaitingAckOnTimerEvent::CTe_LbsSuplPushWaitingAckOnTimerEvent(CTe_LbsSuplPushStateStep& aStep) :
CTe_LbsSuplPushBaseOnTimerEvent(aStep)
	{
	}

/**
Constructs the CSuplPushImpl object in the EWaitingAck state. Places the created object on 
the cleanup stack.
	
@return A pointer on the created CSuplPushImpl object.
	
@leave If a error happens, it leaves with one of the system error codes.

@see CTe_LbsSuplPushBaseTrans::MakeSuplPushImplLC
*/
CLbsSuplPushImpl* CTe_LbsSuplPushWaitingAckOnTimerEvent::MakeSuplPushImplLC()
	{
	return iStep.MakeWaitingAckSuplPushImplLC(*this);
	}

/**
Returns the possible states object switches to after the transition when no error happens and
there is a single message in the queue. 
	
@return The possible states 
*/
TInt CTe_LbsSuplPushWaitingAckOnTimerEvent::SingleSuccessState()
	{
	return CLbsSuplPushImpl::EInitialized;
	}

/**
Returns the possible states object switches to after the transition when no error happens and
there are several messages in the queue. 
	
@return The possible states 
*/
TInt CTe_LbsSuplPushWaitingAckOnTimerEvent::SeveralSuccessState()
	{
	return CLbsSuplPushImpl::EWaitingAck;
	}

/**
Returns the possible states object switches to after the transition when a error happens and
there is a single message in the queue. 
	
@return The possible states 
*/
TInt CTe_LbsSuplPushWaitingAckOnTimerEvent::SingleErrorState()
	{
	return CLbsSuplPushImpl::EInitialized;
	}

/**
Returns the possible states object switches to after the transition when a error happens and
there are several messages in the queue. 
	
@return The possible states 
*/
TInt CTe_LbsSuplPushWaitingAckOnTimerEvent::SeveralErrorState()
	{
	return CLbsSuplPushImpl::EInitialized | CLbsSuplPushImpl::EWaitingAck | CLbsSuplPushImpl::EWaitingBusy;
	}


/**
Creates a RunL from EWaitingAck state transition object and puts it into the cleanup stack.

@param aStep [In] A reference on the test step owning this object.

@return A pointer on the created transition.
 
@leave If a error happens, it leaves with one of the system error codes.
*/
CTe_LbsSuplPushWaitingAckRunL* CTe_LbsSuplPushWaitingAckRunL::NewLC(CTe_LbsSuplPushStateStep& aStep)
	{
	CTe_LbsSuplPushWaitingAckRunL* self = new (ELeave) CTe_LbsSuplPushWaitingAckRunL(aStep);
	CleanupStack::PushL(self);
	return self;
	}


/**
Constructor

@param aStep [In] A reference on the test step owning this object
*/
CTe_LbsSuplPushWaitingAckRunL::CTe_LbsSuplPushWaitingAckRunL(CTe_LbsSuplPushStateStep& aStep) :
CTe_LbsSuplPushBaseRunL(aStep)
	{
	}


/**
Constructs the CSuplPushImpl object in the EWaitingAck state. Places the created object on 
the cleanup stack.
	
@return A pointer on the created CSuplPushImpl object.
	
@leave If a error happens, it leaves with one of the system error codes.

@see CTe_LbsSuplPushBaseTrans::MakeSuplPushImplLC
*/
CLbsSuplPushImpl* CTe_LbsSuplPushWaitingAckRunL::MakeSuplPushImplLC()
	{
	return iStep.MakeWaitingAckSuplPushImplLC(*this);
	}

/**
Returns the possible states object switches to after the transition when no error happens and
there are several messages in the queue. 
	
@return The possible states 
*/
TInt CTe_LbsSuplPushWaitingAckRunL::SingleSuccessState()
	{
	return CLbsSuplPushImpl::EInitialized;
	}

/**
Returns the possible states object switches to after the transition when no error happens and
there are several messages in the queue. 
	
@return The possible states 
*/
TInt CTe_LbsSuplPushWaitingAckRunL::SeveralSuccessState()
	{
	return CLbsSuplPushImpl::EWaitingAck ;
	}

/**
Returns the possible states object switches to after the transition when a error happens and
there is a single message in the queue. 
	
@return The possible states 
*/
TInt CTe_LbsSuplPushWaitingAckRunL::SingleErrorState()
	{
	return CLbsSuplPushImpl::EInitialized | CLbsSuplPushImpl::EWaitingAck;
	}

/**
Returns the possible states object switches to after the transition when a error happens and
there are several messages in the queue. 
	
@return The possible states 
*/
TInt CTe_LbsSuplPushWaitingAckRunL::SeveralErrorState()
	{
	return CLbsSuplPushImpl::EWaitingBusy | CLbsSuplPushImpl::EWaitingAck | CLbsSuplPushImpl::EInitialized;
	}

/**
Receives notification about an incoming SUPL INIT message. Overrides the pure virtual 
MLbsSuplPushRecObserver::OnSuplInit.

@see MLbsSuplPushRecObserver::OnSuplInit	
*/
void CTe_LbsSuplPushWaitingAckRunL::OnSuplInit(TLbsSuplPushChannel /*aChannel*/,
		TLbsSuplPushRequestId /*aReqId*/, TDesC8& /*aMsg*/)
	{
	TRAPD(err, RTe_LbsSuplErrorPattern::AppendNextErrorL(iFunction,
			iCount, iError, EFalse));
	__ASSERT_ALWAYS(err==KErrNone, User::Invariant());
	}


/**
Runs the various scenarios for this transition.

@leave If a error happens, it leaves with one of the system error codes.
*/
void CTe_LbsSuplPushWaitingAckRunL::RunTransL()
	{
	CTe_LbsSuplPushBaseRunL::RunTransL();
	
	//Error - wrong ACK id, a single message in the queue
	CLbsSuplPushImpl* pushImpl = MakeSuplPushImplLC();
	CTe_LbsSuplPushImpl::DisableAsserts();
	User::LeaveIfError(RProperty::Set(iStep.PropOwnerSecureId(), iStep.AckPropKey(), 0));
	iStep.ActiveSchedulerWait().Start();
	pushImpl->CheckStateL(SingleErrorState());
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
Applies different error patterns for the transition and checks the result.

@param aFunction [In] The function that fails.
@param aError [In] The error the function fails with.
@param aMessagesInQueue [In] Number of messages in the queue before the transition.

@leave If a error happens, it leaves with one of the system error codes.
*/
void CTe_LbsSuplPushWaitingAckRunL::CheckErrorsL(TInt aFunction, TInt aError, 
		TUint aMessagesInQueue)
	{
	__ASSERT_ALWAYS(aMessagesInQueue>0 && aMessagesInQueue < 100, User::Invariant());
	
	iFunction = aFunction;
	iError = aError;
	
	iCount=1;
	TInt errorReached = 0;
	CLbsSuplPushRec* suplPushRec = CLbsSuplPushRec::NewL(*this);
	CleanupStack::PushL(suplPushRec);
	
	
	do	{
		//Reinitialize BUSY prop here to avoid reuse of the old message id, which is incorrect
		//from the CLbsSuplPushRec point of view. It considers it as an old message and ignores it.
		User::LeaveIfError(RProperty::Set(iStep.PropOwnerSecureId(), iStep.BusyPropKey(), 
				KMaxTInt-(iCount+100*aMessagesInQueue)));
	
		CLbsSuplPushImpl* pushImpl = MakeSuplPushImplLC();
		TLbsSuplPushRequestId reqId;
		
		for(TUint i=1;i<aMessagesInQueue;i++)
			{
			User::LeaveIfError(pushImpl->SuplInit(reqId, _L8("abcd"), 0));
			}
			
		CTe_LbsSuplPushImpl::DisableAsserts();
		iStep.ActiveSchedulerWait().Start();
		errorReached = RTe_LbsSuplErrorPattern::ErrorReached();
		RTe_LbsSuplErrorPattern::Reset();
		
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
		CTe_LbsSuplPushImpl::EnableAsserts();
		iCount++;
		}
	while(errorReached==1);	
	CleanupStack::PopAndDestroy(suplPushRec);	
	}



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
// State transitions for the CLbsSuplPushImpl class from the EWaitingBusy state
// 
//

#include "Te_LbsSuplPushImpl.h"
#include "Te_LbsSuplPushWaitingBusyTrans.h"
#include "Te_LbsSuplPushStateStep.h"


/**
Creates a SuplInit from EWaitingBusy state transition object and puts it into the cleanup stack.

@param aStep [In] A reference on the test step owning this object.

@return A pointer on the created transition.
 
@leave If a error happens, it leaves with one of the system error codes.
*/
CTe_LbsSuplPushWaitingBusySuplInit* 
CTe_LbsSuplPushWaitingBusySuplInit::NewLC(CTe_LbsSuplPushStateStep& aStep)
	{
	CTe_LbsSuplPushWaitingBusySuplInit* self = 
			new (ELeave) CTe_LbsSuplPushWaitingBusySuplInit(aStep);
	CleanupStack::PushL(self);
	return self;
	}
	
/**
Constructor

@param aStep [In] A reference on the test step owning this object
*/
CTe_LbsSuplPushWaitingBusySuplInit::CTe_LbsSuplPushWaitingBusySuplInit(
		CTe_LbsSuplPushStateStep& aStep) : CTe_LbsSuplPushBaseSuplInit(aStep)
	{
	}

/**
Constructs the CSuplPushImpl object in the EWaitingBusy state. Places the created object on 
the cleanup stack.
	
@return A pointer on the created CSuplPushImpl object.
	
@leave If a error happens, it leaves with one of the system error codes.

@see CTe_LbsSuplPushBaseTrans::MakeSuplPushImplLC
*/
CLbsSuplPushImpl* CTe_LbsSuplPushWaitingBusySuplInit::MakeSuplPushImplLC()
	{
	return iStep.MakeWaitingBusySuplPushImplLC(*this);
	}


/**
Returns the possible main states the object is after the transition.
	
@return The possible main states the object is after the transition.

@see CTe_LbsSuplPushBaseSuplInit::State
*/
TInt CTe_LbsSuplPushWaitingBusySuplInit::State()
	{
	return CLbsSuplPushImpl::EWaitingBusy;
	}

/**
Returns the possible states object switches to after the transition when there is a wrong
BUSY property set.
	
@return The possible states object switches to after the transition when there is a wrong
BUSY property set.

@see CTe_LbsSuplPushBaseSuplInit::WrongBusyPropState
*/
TInt CTe_LbsSuplPushWaitingBusySuplInit::WrongBusyPropState()
	{
	return CLbsSuplPushImpl::EWaitingBusy;
	}

/**
Returns the possible states object switches to after the transition when the RProperty::Get
function fails. 
	
@return The possible states object switches to after the transition when the RProperty::Get
function fails.

@see CTe_LbsSuplPushBaseSuplInit::RPropertyGetErrorState
*/
TInt CTe_LbsSuplPushWaitingBusySuplInit::RPropertyGetErrorState()
	{
	return CLbsSuplPushImpl::EWaitingBusy;
	}

/**
Returns the possible states object switches to after the transition when the RProperty::Set
function fails. 
	
@return The possible states object switches to after the transition when the RProperty::Set
function fails.

@see CTe_LbsSuplPushBaseSuplInit::RPropertySetErrorState
*/
TInt CTe_LbsSuplPushWaitingBusySuplInit::RPropertySetErrorState()
	{
	return CLbsSuplPushImpl::EWaitingBusy;
	}


/**
Creates a RunL from EWaitingBusy state transition object and puts it into the cleanup stack.

@param aStep [In] A reference on the test step owning this object.

@return A pointer on the created transition.
 
@leave If a error happens, it leaves with one of the system error codes.
*/
CTe_LbsSuplPushWaitingBusyRunL* CTe_LbsSuplPushWaitingBusyRunL::NewLC(CTe_LbsSuplPushStateStep& aStep)
	{
	CTe_LbsSuplPushWaitingBusyRunL* self = new (ELeave) CTe_LbsSuplPushWaitingBusyRunL(aStep);
	CleanupStack::PushL(self);
	return self;
	}

/**
Constructor

@param aStep [In] A reference on the test step owning this object
*/
CTe_LbsSuplPushWaitingBusyRunL::CTe_LbsSuplPushWaitingBusyRunL(CTe_LbsSuplPushStateStep& aStep) :
CTe_LbsSuplPushBaseRunL(aStep)
	{
	}

/**
Constructs the CSuplPushImpl object in the EWaitingBusy state. Places the created object on 
the cleanup stack.
	
@return A pointer on the created CSuplPushImpl object.
	
@leave If a error happens, it leaves with one of the system error codes.

@see CTe_LbsSuplPushBaseTrans::MakeSuplPushImplLC
*/
CLbsSuplPushImpl* CTe_LbsSuplPushWaitingBusyRunL::MakeSuplPushImplLC()
	{
	return iStep.MakeWaitingBusySuplPushImplLC(*this);
	}


/**
Returns the possible states object switches to after the transition when no error happens and
there are several messages in the queue. 
	
@return The possible states 
*/
TInt CTe_LbsSuplPushWaitingBusyRunL::SingleSuccessState()
	{
	return CLbsSuplPushImpl::EInitialized;
	}

/**
Returns the possible states object switches to after the transition when no error happens and
there are several messages in the queue. 
	
@return The possible states 
*/
TInt CTe_LbsSuplPushWaitingBusyRunL::SeveralSuccessState()
	{
	return CLbsSuplPushImpl::EWaitingBusy;
	}

/**
Returns the possible states object switches to after the transition when a error happens and
there is a single message in the queue. 
	
@return The possible states 
*/
TInt CTe_LbsSuplPushWaitingBusyRunL::SingleErrorState()
	{
	return CLbsSuplPushImpl::EInitialized | CLbsSuplPushImpl::EWaitingBusy;
	}

/**
Returns the possible states object switches to after the transition when a error happens and
there are several messages in the queue. 
	
@return The possible states 
*/
TInt CTe_LbsSuplPushWaitingBusyRunL::SeveralErrorState()
	{
	return CLbsSuplPushImpl::EInitialized | CLbsSuplPushImpl::EWaitingBusy;
	}

/**
Receives notification about an incoming SUPL INIT message. Overrides the pure virtual 
MLbsSuplPushRecObserver::OnSuplInit.

@see MLbsSuplPushRecObserver::OnSuplInit	
*/
void CTe_LbsSuplPushWaitingBusyRunL::OnSuplInit(TLbsSuplPushChannel /*aChannel*/,
		TLbsSuplPushRequestId /*aReqId*/, TDesC8& /*aMsg*/)
	{
	//Intentionally left blank
	}

/**
Runs the various scenarios for this transition.

@leave If a error happens, it leaves with one of the system error codes.
*/
void CTe_LbsSuplPushWaitingBusyRunL::RunTransL()
	{
	User::LeaveIfError(RProperty::Set(iStep.PropOwnerSecureId(), iStep.BusyPropKey(), 1));
	
	CTe_LbsSuplPushBaseRunL::RunTransL();
	
	//Correct release of BUSY property and delivery it to the receiver
	CLbsSuplPushRec* suplPushRec = CLbsSuplPushRec::NewL(*this);
	CleanupStack::PushL(suplPushRec);
	CLbsSuplPushImpl* pushImpl = MakeSuplPushImplLC();
	
	User::LeaveIfError(RProperty::Set(iStep.PropOwnerSecureId(), iStep.BusyPropKey(), 1));
	iStep.ActiveSchedulerWait().Start();
	pushImpl->CheckStateL(SingleSuccessState());
	
	CleanupStack::PopAndDestroy(pushImpl);	
	CleanupStack::PopAndDestroy(suplPushRec);	
	}




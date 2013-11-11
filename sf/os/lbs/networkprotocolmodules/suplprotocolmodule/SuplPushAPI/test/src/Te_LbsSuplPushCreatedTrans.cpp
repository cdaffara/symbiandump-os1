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
// State transitions for the CLbsSuplPushImpl class from the ECreated state
// 
//

#include "Te_LbsSuplPushImpl.h" 

#include "Te_LbsSuplPushRProperty.h"
#include "Te_LbsSuplPushCreatedTrans.h"

#include "Te_LbsSuplCommon.h"
#include "Te_LbsSuplErrorPattern.h"
#include "lbssystemcontroller.h"
#include "Te_LbsSuplPushStateStep.h"
	

/**
Creates a SuplInit from ECreated state transition object and puts it into the cleanup stack.

@param aStep [In] A reference on the test step owning this object.

@return A pointer on the created transition.
 
@leave If a error happens, it leaves with one of the system error codes.
*/
CTe_LbsSuplPushCreatedSuplInit* CTe_LbsSuplPushCreatedSuplInit::NewLC(
		CTe_LbsSuplPushStateStep& aStep)
	{
	CTe_LbsSuplPushCreatedSuplInit* self = new (ELeave) CTe_LbsSuplPushCreatedSuplInit(aStep);
	CleanupStack::PushL(self);
	return self;
	}
	

/**
Constructor

@param aStep [In] A reference on the test step owning this object
*/
CTe_LbsSuplPushCreatedSuplInit::CTe_LbsSuplPushCreatedSuplInit(CTe_LbsSuplPushStateStep& aStep) :
CTe_LbsSuplPushBaseSuplInit(aStep)
	{
	//Intentionally left blank
	}

/**
Constructs the CSuplPushImpl object in the ECreated state. Places the created object on 
the cleanup stack.
	
@return A pointer on the created CSuplPushImpl object.
	
@leave If a error happens, it leaves with one of the system error codes.

@see CTe_LbsSuplPushBaseTrans::MakeSuplPushImplLC
*/
CLbsSuplPushImpl* CTe_LbsSuplPushCreatedSuplInit::MakeSuplPushImplLC()
	{
	return iStep.MakeCreatedSuplPushImplLC(*this);
	}

/**
Returns the possible main states the object is after the transition.
	
@return The possible main states the object is after the transition.

@see CTe_LbsSuplPushBaseSuplInit::State
*/
TInt CTe_LbsSuplPushCreatedSuplInit::State()
	{
	return CLbsSuplPushImpl::ECreated;
	}

/**
Returns the possible states object switches to after the transition when there is a wrong
BUSY property set.
	
@return The possible states object switches to after the transition when there is a wrong
BUSY property set.

@see CTe_LbsSuplPushBaseSuplInit::WrongBusyPropState
*/
TInt CTe_LbsSuplPushCreatedSuplInit::WrongBusyPropState()
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
TInt CTe_LbsSuplPushCreatedSuplInit::RPropertyGetErrorState()
	{
	return CLbsSuplPushImpl::ECreated | CLbsSuplPushImpl::EInitialized | CLbsSuplPushImpl::EWaitingAck;
	}

/**
Returns the possible states object switches to after the transition when the RProperty::Set
function fails. 
	
@return The possible states object switches to after the transition when the RProperty::Set
function fails.

@see CTe_LbsSuplPushBaseSuplInit::RPropertySetErrorState
*/
TInt CTe_LbsSuplPushCreatedSuplInit::RPropertySetErrorState()
	{
	return CLbsSuplPushImpl::ECreated | CLbsSuplPushImpl::EInitialized | CLbsSuplPushImpl::EWaitingAck;
	}


/**
Creates a forbidden transition object for ECreated state and puts it into the cleanup stack.

@param aStep [In] A reference on the test step owning this object.

@return A pointer on the created transition.
 
@leave If a error happens, it leaves with one of the system error codes.
*/
CTe_LbsSuplPushCreatedForbidden* CTe_LbsSuplPushCreatedForbidden::NewLC(CTe_LbsSuplPushStateStep& aStep)
	{
	CTe_LbsSuplPushCreatedForbidden* self = new (ELeave) CTe_LbsSuplPushCreatedForbidden(aStep);
	CleanupStack::PushL(self);
	return self;
	}

/**
Runs the various scenarios for this transition.

@leave If a error happens, it leaves with one of the system error codes.
*/
void CTe_LbsSuplPushCreatedForbidden::RunTransL()
	{	
	//Call Cancel
	CLbsSuplPushImpl* pushImpl = iStep.MakeCreatedSuplPushImplLC(*this);
	CTe_LbsSuplPushImpl::DisableAsserts();
	TRequestStatus* status = &pushImpl->iStatus;
	User::RequestComplete(status, KErrGeneral);
	CTe_LbsSuplPushImpl::SetActive(*pushImpl); 
	pushImpl->Cancel();
	pushImpl->CheckStateL(CLbsSuplPushImpl::ECreated);
	CTe_LbsSuplPushImpl::EnableAsserts();
	CleanupStack::PopAndDestroy(pushImpl);
	
	}

/**
Constructor

@param aStep [In] A reference on the test step owning this object
*/
CTe_LbsSuplPushCreatedForbidden::CTe_LbsSuplPushCreatedForbidden(CTe_LbsSuplPushStateStep& aStep) :
CTe_LbsSuplPushBaseTrans(aStep)
	{
	}


/**
Constructs the CSuplPushImpl object in the ECreated state. Places the created object on 
the cleanup stack.
	
@return A pointer on the created CSuplPushImpl object.
	
@leave If a error happens, it leaves with one of the system error codes.

@see CTe_LbsSuplPushBaseTrans::MakeSuplPushImplLC
*/

CLbsSuplPushImpl* CTe_LbsSuplPushCreatedForbidden::MakeSuplPushImplLC()
	{
	return iStep.MakeCreatedSuplPushImplLC(*this);
	}

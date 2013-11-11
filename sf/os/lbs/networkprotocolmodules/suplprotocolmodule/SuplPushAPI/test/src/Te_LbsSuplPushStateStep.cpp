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
// These are the state transition unit tests for the CLbsSuplPush, CLbsSuplPushImpl and related classes
// 
//

#include "lbssuplpushprops.h"

#include "Te_LbsSuplPushImpl.h"
#include "Te_LbsSuplPushStateStep.h"
#include "Te_LbsSuplErrorPattern.h"
#include "Te_LbsSuplPushNullTrans.h"
#include "Te_LbsSuplPushWaitingAckTrans.h"
#include "Te_LbsSuplPushWaitingBusyTrans.h"


/**
Constructor. It sets the test step name used later by the TEF.
*/
CTe_LbsSuplPushStateStep::CTe_LbsSuplPushStateStep()
	{
	SetTestStepName(KLbsSuplPushStateStep);
	}

/**
Overrides the pure virtual CTe_LbsSuplPushBaseStep::ReallyDoTestStepL. The test step code is 
placed here.

@leave If the test step fails, it leaves with either the CTestStep::EFail code or one of 
the system error codes.

@see CTe_LbsSuplPushBaseStep::ReallyDoTestStepL
*/
void CTe_LbsSuplPushStateStep::ReallyDoTestStepL()
	{
	CTe_LbsSuplPushImpl::EnableAsserts();
	DoNullTransL();
	DoCreatedTransL();
	DoWaitingAckTransL();
	DoWaitingBusyTransL();
	}

/**
Does the CSuplPushImpl object creation tests.

@leave If the test step fails, it leaves with either the CTestStep::EFail code or one of 
the system error codes.
*/
void CTe_LbsSuplPushStateStep::DoNullTransL()
	{
	CTe_LbsSuplPushNullNewL* trans = CTe_LbsSuplPushNullNewL::NewLC();
	trans->RunTransL();
	CleanupStack::PopAndDestroy(trans);
	}

/**
Does the transition tests from ECreated state for the CSuplPushImpl object.

@leave If the test step fails, it leaves with either the CTestStep::EFail code or one of 
the system error codes.
*/
void CTe_LbsSuplPushStateStep::DoCreatedTransL()
	{
	CTe_LbsSuplPushBaseTrans* trans = CTe_LbsSuplPushCreatedSuplInit::NewLC(*this);
	trans->RunTransL();
	CleanupStack::PopAndDestroy(trans);
	
	trans = CTe_LbsSuplPushCreatedForbidden::NewLC(*this);
	trans->RunTransL();
	CleanupStack::PopAndDestroy(trans);
	
	}

/**
Does the transition tests from EWaitingAck state for the CSuplPushImpl object.

@leave If the test step fails, it leaves with either the CTestStep::EFail code or one of 
the system error codes.
*/
void CTe_LbsSuplPushStateStep::DoWaitingAckTransL()
	{
	CTe_LbsSuplPushBaseTrans* trans = CTe_LbsSuplPushWaitingAckSuplInit::NewLC(*this);
	trans->RunTransL();
	CleanupStack::PopAndDestroy(trans);
	
	trans = CTe_LbsSuplPushWaitingAckOnTimerEvent::NewLC(*this);
	trans->RunTransL();
	CleanupStack::PopAndDestroy(trans);
	
	trans = CTe_LbsSuplPushWaitingAckRunL::NewLC(*this);
	trans->RunTransL();
	CleanupStack::PopAndDestroy(trans);
	}

/**
Does the transition tests from EWaitingBusy state for the CSuplPushImpl object.

@leave If the test step fails, it leaves with either the CTestStep::EFail code or one of 
the system error codes.
*/
void CTe_LbsSuplPushStateStep::DoWaitingBusyTransL()
	{
	CTe_LbsSuplPushBaseTrans* trans = CTe_LbsSuplPushWaitingBusySuplInit::NewLC(*this);
	trans->RunTransL();
	CleanupStack::PopAndDestroy(trans);
		
	trans = CTe_LbsSuplPushWaitingBusyRunL::NewLC(*this);
	trans->RunTransL();
	CleanupStack::PopAndDestroy(trans);
	}

/**
Constructs the CSuplPushImpl object in the ECreated state and puts it into the cleanup stack.

@return A pointer on the created CSuplPushImpl object.

@leave If a error happens, it leaves with one of the system error codes.
*/
CLbsSuplPushImpl* CTe_LbsSuplPushStateStep::MakeCreatedSuplPushImplLC(MLbsSuplPushObserver& aObserver)
	{
	CLbsSuplPushImpl* res = CLbsSuplPushImpl::NewL(iChannel, aObserver);
	CleanupStack::PushL(res);
	return res;
	}

/**
Constructs the CSuplPushImpl object in the EWaitingAck state and puts it into the cleanup stack.

@return A pointer on the created CSuplPushImpl object.

@leave If a error happens, it leaves with one of the system error codes.
*/
CLbsSuplPushImpl* CTe_LbsSuplPushStateStep::MakeWaitingAckSuplPushImplLC(MLbsSuplPushObserver& aObserver)
	{
	TLbsSuplPushRequestId reqId;
	CLbsSuplPushImpl* res = MakeCreatedSuplPushImplLC(aObserver);
	User::LeaveIfError(res->SuplInit(reqId, _L8("abcd"), 0));
	res->CheckStateL(CLbsSuplPushImpl::EWaitingAck);
	return res;
	}

/**
Constructs the CSuplPushImpl object in the EWaitingBusy state and puts it into the cleanup stack.

@return A pointer on the created CSuplPushImpl object.

@leave If a error happens, it leaves with one of the system error codes.
*/
CLbsSuplPushImpl* CTe_LbsSuplPushStateStep::MakeWaitingBusySuplPushImplLC(MLbsSuplPushObserver& aObserver)
	{
	User::LeaveIfError(RProperty::Set(iPropOwnerSecureId, iBusyPropKey, -1));
	TLbsSuplPushRequestId reqId;
	CLbsSuplPushImpl* res = MakeCreatedSuplPushImplLC(aObserver);
	User::LeaveIfError(res->SuplInit(reqId, _L8("abcd"), 0));
	res->CheckStateL(CLbsSuplPushImpl::EWaitingBusy);
	User::LeaveIfError(RProperty::Set(iPropOwnerSecureId, iBusyPropKey, 1));
	return res;
	}

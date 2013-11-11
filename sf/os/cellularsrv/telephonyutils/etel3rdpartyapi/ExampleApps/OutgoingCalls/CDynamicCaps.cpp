// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "CDynamicCaps.h"

/**
Factory constructor.
@param aController Pointer to MExecAsync object
passed to constructor of CISVAPIBase
@param aCallId Identifies the call whose capabilities will be interrogated
@return Instance of CDynamicCaps class
*/
CDynamicCaps* CDynamicCaps::NewL(MExecAsync* aController, CTelephony::TCallId aCallId)
	{
	CDynamicCaps* self = new(ELeave) CDynamicCaps(aController, aCallId);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/**
Destructor.
Cancels outstanding requests
*/
CDynamicCaps::~CDynamicCaps()
	{
	Cancel();
	}

/**
Gets the dynamic call capabilities of the call identified by iCallId
and stores the results in the iCallCapsV1Pckg package.
*/
void CDynamicCaps::StartRequestL()
	{
	_LIT(KDummyAnswerPanic, "CDynamicCaps Get Method");
	__ASSERT_ALWAYS(!IsActive(), User::Panic(KDummyAnswerPanic, 1));
	iRequestNotify = EFalse;
	iTelephony->GetCallDynamicCaps(iCallId, iCallCapsV1Pckg);
	if( iCallCapsV1.iControlCaps & CTelephony::KCapsHold )
		{
		gConsole->Printf(_L("Call can be put on hold\n"));
		ExampleNotify();
		}
	else
		{
		gConsole->Printf(_L("Call cannot be put on hold\n"));
		// TODO Code in CDynamicCaps::StartRequestL() needs to be finished
		//
		// do some thing!
		//
		}
	}

/**
Constructor.
@param aController Pointer to an MExecAsync object passed to
constructor of CISVAPIBase
@param aCallId Identifies the call whose capabilities will be interrogated
*/
CDynamicCaps::CDynamicCaps(MExecAsync* aController, CTelephony::TCallId aCallId)
	: CISVAPIAsync(aController)
	, iCallCapsV1Pckg(iCallCapsV1)
	, iCallId(aCallId)
	{
	iExtensionId = KDynamicCaps;
	}

/**
Second phase constructor.
*/
void CDynamicCaps::ConstructL()
	{
 	CActiveScheduler::Add(this);
	}

/**
TODO Code in CDynamicCaps::RunL() needs to be written, empty method at present
*/
void CDynamicCaps::RunL()
	{

	}

/**
TODO Code in CDynamicCaps::DoCancel() needs to be written, empty method at present
*/
void CDynamicCaps::DoCancel()
	{

	}


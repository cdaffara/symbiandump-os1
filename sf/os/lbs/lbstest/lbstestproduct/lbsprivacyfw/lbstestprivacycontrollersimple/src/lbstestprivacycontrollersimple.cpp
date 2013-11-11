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

/**
 @file lbstestprivacycontrollersimple.cpp
*/
 
#include <e32base.h>
#include <lbs/lbsloccommon.h>

#include "lbstestprivacycontrollersimple.h"

CTestPrivacyControllerSimple* CTestPrivacyControllerSimple::NewL()
	{
	CTestPrivacyControllerSimple* self = new (ELeave) CTestPrivacyControllerSimple;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CTestPrivacyControllerSimple::~CTestPrivacyControllerSimple()
	{
	delete iLbsPrivacyController;
	iLbsPrivacyController = NULL;
	}

CTestPrivacyControllerSimple::CTestPrivacyControllerSimple()
	{
	}

void CTestPrivacyControllerSimple::ConstructL()
	{
	iLbsPrivacyController = CLbsPrivacyController::NewL(*this);
	}

		
void CTestPrivacyControllerSimple::ProcessNetworkLocationRequest(TUint aRequestId, const TLbsExternalRequestInfo& aRequestInfo, const TNotificationType& aNotificationType)
	{
	(void)aRequestInfo;
	
	ASSERT(iLbsPrivacyController != NULL);
	
	switch(aNotificationType)
		{
		case ENotifyAndVerifyLocationAcceptedIfNoReponse:
			{
			iLbsPrivacyController->RespondNetworkLocationRequest(aRequestId, CLbsPrivacyController::ERequestAccepted);
			break;
			}

		case ENotifyAndVerifyLocationRejectedIfNoReponse:
			{
			iLbsPrivacyController->RespondNetworkLocationRequest(aRequestId, CLbsPrivacyController::ERequestRejected);
			break;
			}

		// A response is not required for notifications.
		case ENotifyLocationAccepted:
		case ENotifyLocationRejected:
			{
			break;	
			}

		default:
			//Panic
			User::Invariant();
		}
	}

void CTestPrivacyControllerSimple::ProcessNetworkPositionUpdate(TUint aRequestId, const TPositionInfo& aPosInfo)
	{
	(void) aRequestId;
	(void) aPosInfo;
	User::Invariant();
	//Shouldn't be called
	}

void CTestPrivacyControllerSimple::ProcessRequestComplete(TUint aRequestId, TInt aReason)
	{
	(void)aRequestId;
	(void)aReason;
	//Not needed for simple controller
	}



// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "ATERROR.H"
#include "ATWAIT.H"
#include "NOTIFY.H"

CATErrorHandler* CATErrorHandler::NewL(CPhoneGlobals* aPhoneGlobals,CATWaitForCall* aWaitForCall)
	{
	return new(ELeave) CATErrorHandler(aPhoneGlobals,aWaitForCall);
	}

CATErrorHandler::CATErrorHandler(CPhoneGlobals* aPhoneGlobals,CATWaitForCall* aWaitForCall)
						: iWaitForCall(aWaitForCall), iPhoneGlobals(aPhoneGlobals)
	{}

CATErrorHandler::~CATErrorHandler()
	{}

void CATErrorHandler::CleanUp(TInt aError)
//
//	Completes all notifications with the error. If there were no notifications, the error
//  remains until the next 
	{
	iPhoneGlobals->iPhoneStatus.iMode = RPhone::EModeUnknown;
	iPhoneGlobals->iPhoneStatus.iDataAndFaxFlags = RPhone::KCapsUnknown;
	iPhoneGlobals->iPhoneStatus.iInitStatus = EPhoneNotInitialised;
	iPhoneGlobals->iNotificationStore->CompleteNotificationsWithError(aError);
	iWaitForCall->StartWait();	// re-start the RING expect string
	}

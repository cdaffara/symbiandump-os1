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


#ifndef __CMAINMENU_H__
#define __CMAINMENU_H__

#include <e32base.h>
#include <etel3rdparty.h>


#include "CISVAPIAsync.h"

#include "CBaseMenuAsync.h"

// Active objects
#include "CPhoneId.h"
#include "CFlightModeInfo.h"
#include "CCallBarringStatus.h"
#include "CCallForwardingStatus.h"
#include "CCallWaitingStatus.h"
#include "CIdentityServiceStatus.h"

_LIT(KPhoneIdMsg, "Phone Id:\n");
_LIT(KGetLockInfoMsg, "Lock Info:\n");
_LIT(KCallForwardingStatusMsg, "Call Forwarding Status:\n");
_LIT(KCallBarringStatusMsg, "Call Barring Status:\n");
_LIT(KCallWaitingStatusMsg, "Call Waiting Status:\n");
_LIT(KIdentityServiceStatusMsg, "Identity Service Status:\n");
_LIT(KMenuMsg, "Press E to exit\n");

/**
Provides the required functionality for the application to run correctly. Owns
required objects to make Etel 3rd party calls required by the application.
*/
class CMainMenu : public CBaseMenuAsync
	{
	// Methods
public:
	static CMainMenu* NewLC(CConsoleBase& aConsole);
	~CMainMenu();

	void ExecComplete(TTelISVExampleType aDerivedType);
	void ExecNotify(TTelISVExampleType aDerivedType);

private:
	CMainMenu(CConsoleBase& aConsole);
	void ConstructL();

	void RunL();	// Handles key presses
	void DoCancel();

private:

	// Owned active objects
	/**
	Active object which demonstrates the use of CTelephony::GetPhoneId().
	*/
	CISVAPIAsync* iPhoneId;
	/**
	Active object which demonstrates the use of CTelephony::GetCallForwardingStatus().
	*/
	CISVAPIAsync* iCallForwarding;
	/**
	Active object which demonstrates the use of CTelephony::GetCallBarringStatus().
	*/
	CISVAPIAsync* iCallBarring;
	/**
	Active object which demonstrates the use of CTelephony::GetCallWaitingStatus().
	*/
	CISVAPIAsync* iCallWaiting;
	/**
	Active object which demonstrates the use of CTelephony::GetFlightMode().
	*/
	CISVAPIAsync* iFlightModeInfo;
	/**
	Active object which demonstrates the use of CTelephony::GetIdentityServiceStatus().
	*/
	CISVAPIAsync* iIdentityService;

	};

#endif // __CMAINMENU_H__


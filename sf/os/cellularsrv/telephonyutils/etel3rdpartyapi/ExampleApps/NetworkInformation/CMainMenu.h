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
#include "CCurrentNetworkInfo.h"

_LIT(KPhoneIdMsg, "Phone Id:\n");
_LIT(KNetworkInfoMsg, "Network Info:\n");
_LIT(KNetworkNameMsg, "Network Name:\n");
_LIT(KOperatorNameMsg, "Operator Name:\n");
_LIT(KNetworkRegMsg, "Network Registration Status:\n");
_LIT(KMenuMsg, "Press E to exit\n");

/**
Provides the required functionality for the application to run correctly. Owns
required objects to make Etel3rdParty calls required by the application.
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

	void RunL();	// Handles Key presses
	void DoCancel();

// Data
private:

	// Owned active objects
	/**
	Active object which demonstrates the use of CTelephony::GetPhoneID().
	*/
	CISVAPIAsync* iPhoneId;
	/**
	Active object which demonstrates the use of CTelephony::GetCurrentNetworkInfo().
	*/
	CISVAPIAsync* iNetworkInfo;
	/**
	Active object which demonstrates the use of CTelephony::GetFlightMode().
	*/
	CISVAPIAsync* iFlightModeInfo;

	};

#endif // __CMAINMENU_H__


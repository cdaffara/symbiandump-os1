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
#include "CBatteryInfo.h"
#include "CFlightModeInfo.h"
#include "CSignalInfo.h"
#include "CGetIndicator.h"

_LIT(KPhoneIdMsg, "Phone Id:\n");
_LIT(KGetIndicatorMsg, "Indicator Information:\n");
_LIT(KBatteryInfoMsg, "Battery Info:\n");
_LIT(KSignalStrengthMsg, "Signal Strength Info:\n");
_LIT(KMenuMsg, "Press E to exit\n");

/**
Provides the required functionality for the application to run correctly. Owns
the required objects to make Etel 3rd Party calls required by the application.
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
	Active object which demonstrates the use of CTelephony::GetPhoneId()
	*/
	CISVAPIAsync* iPhoneId;
	/**
	Active object which demonstrates the use of CTelephony::GetBatteryInfo()
	*/
	CISVAPIAsync* iBatteryInfo;
	/**
	Active object which demonstrates the use of CTelephony::GetSignalStrength()
	*/
	CISVAPIAsync* iSignalInfo;
	/**
	Active object which demonstrates the use of CTelephony::GetFlightMode()
	*/
	CISVAPIAsync* iFlightModeInfo;
	/**
	Active object which demonstrates the use of CTelephony::GetIndicator()
	*/
	CISVAPIAsync* iGetIndicator;

	};

#endif // __CMAINMENU_H__


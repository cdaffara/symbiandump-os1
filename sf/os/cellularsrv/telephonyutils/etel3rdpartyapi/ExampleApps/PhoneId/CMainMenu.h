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

#include "CISVAPISync.h"

#include "CBaseMenuSync.h"

// Active objects

#include "CPhoneId.h"
#include "CSubscriberId.h"

_LIT(KPhoneIdMsg, "Phone Id:\n");
_LIT(KSubscriberIdMsg, "Subscriber Id:\n");

/**
Provides the required functionality for the application to run correctly. 
Owns the required objects to make Etel 3rd Party calls required by the application.
*/
class CMainMenu : public CBaseMenuSync
	{

	// Methods
public:
	static CMainMenu* NewLC(CConsoleBase& aConsole);
	~CMainMenu();

private:
	CMainMenu(CConsoleBase& aConsole);
	void ConstructL();
	void DoCancel();

	void RunL();	// Handles key presses
	// Data
private:
	// Owned active objects
	/**
	Active object which demonstrates the use of CTelephony::GetPhoneId().
	*/
	CISVAPISync* iPhoneId;
	/**
	Active object which demonstrates the use of CTelephony::GetSubscriberId().
	*/
	CISVAPISync* iSubscriberId;

	};

#endif // __CMAINMENU_H__


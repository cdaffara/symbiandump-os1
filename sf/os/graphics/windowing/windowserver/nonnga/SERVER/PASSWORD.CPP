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
// Password handling
// 
//

#include <e32std.h>
#include <w32std.h>
#include "server.h"
#include "rootwin.h"
#include "windowgroup.h"
#include "password.h"

TBool CWsPassword::iPasswordModeActive=EFalse;
TPasswordMode CWsPassword::iPasswordMode=EPasswordCancel;
CWsClientWindow *CWsPassword::iPasswordWindow=NULL;
TInt CWsPassword::iPrevPasswordDay=-1;

void CWsPassword::WindowDestroyed(CWsClientWindow *aWindow)
	{
	if (aWindow==iPasswordWindow)
		CancelPasswordWindow();
	}

void CWsPassword::CancelPasswordWindow()
	{
	iPasswordWindow=NULL;
	iPasswordModeActive=EFalse;
	}

void CWsPassword::SetPasswordWindowL(CWsClientWindow *aWindow, TPasswordMode aPasswordMode)
	{
	TBool triggerNow=EFalse;
	if (aPasswordMode==EPasswordAlwaysTriggerNow || aPasswordMode==EPasswordOnceADayTriggerNow)
		{
		triggerNow=ETrue;
		if (aPasswordMode==EPasswordOnceADayTriggerNow)
			aPasswordMode=EPasswordOnceADay;
		else
			aPasswordMode=EPasswordAlways;
		}
	if (aPasswordMode==EPasswordCancel)
		{
		if (aWindow!=iPasswordWindow)
			aWindow->OwnerPanic(EWservPanicPassword);
		CancelPasswordWindow();
		}
	else
		{
		if (iPasswordWindow && iPasswordWindow!=aWindow)
			User::Leave(KErrInUse);
		if (aPasswordMode!=EPasswordAlways && aPasswordMode!=EPasswordOnceADay && aPasswordMode!=EPasswordNone)
			aWindow->OwnerPanic(EWservPanicPassword);
		iPasswordWindow=aWindow;
		}
	iPasswordMode=aPasswordMode;
	if (iPasswordMode==EPasswordOnceADay)
		iPrevPasswordDay=-1;	// Force a password to be displayed on the this switch on
	if (triggerNow)
		SwitchOn();
	}

TInt CWsPassword::Day()
//
// Return the current day taking into account the password midnight offset
//
	{
	TTime now;
	now.HomeTime();
	now-=TTimeIntervalHours(EPasswordMidnightOffset);
	return(now.DateTime().Day());
	}

void CWsPassword::SwitchOn()
	{
	if (!iPasswordModeActive && iPasswordWindow && iPasswordMode!=EPasswordNone)
		{
		if (iPasswordMode==EPasswordOnceADay)
			{
			TInt oldDay=iPrevPasswordDay;
			iPrevPasswordDay=Day();
			if (iPrevPasswordDay==oldDay)
				return;		// No password display needed
			}
		iPasswordModeActive=ETrue;
		iPasswordWindow->WinGroup()->QueueEvent(EEventPassword);
		iPasswordWindow->WinGroup()->SetOrdinalPriority(0,KPasswordWindowGroupPriority);
		iPasswordWindow->SetOrdinalPosition(0);
		iPasswordWindow->SetVisible(ETrue);
//
		iPasswordWindow->RootWindow()->InvalidateWholeScreen();
		iPasswordWindow->RootWindow()->ClearDisplay();
		}
	}

void CWsPassword::PasswordEntered(CWsClient *aClient)
	{
	if (!iPasswordWindow || aClient!=iPasswordWindow->WsOwner())
		aClient->PPanic(EWservPanicPassword);
	iPasswordModeActive=EFalse;
	}

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
#include "CNetworkRegInfo.h"
#include "CDialCall.h"
#include "CSendDTMF.h"
#include "CHangup.h"
#include "CLineStatus.h"
#include "CCallStatus.h"


_LIT(KPhoneIdMsg, "Phone Id:\n");
_LIT(KNetworkRegMsg, "Network Registration Status:\n");
_LIT(KMenuMsg, "*~Auto DTMF Dialler~*\nPress E to exit\n");
_LIT(KHangupMsg, "Press H to Hangup the active Call\n");
_LIT(KDialMsg, "Press D to Dial a Call\n");
_LIT(KDTMFQuestion, "How would you like to send the DTMF string?\n" );
_LIT(KiSingleString, "Press S to Send Predefinied DTMF string\n");
_LIT(KAsChar, "Press Required numbers then # to specify the DTMF string with Keypresses\n");

_LIT(KTheNumber, "01632960000");

/**
Provides the required functionality for the application to run correctly; owns 
active objects required to make ETel3rdParty calls.
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

	// Data
private:
	// Owned active objects
	/**
	Active object which demonstrates the use of CTelephony::GetPhoneId()
	*/
	CISVAPIAsync* iPhoneId;
	/**
	Active object which demonstrates the use of CTelephony::GetFlightMode()
	*/
	CISVAPIAsync* iFlightModeInfo;
	/**
	Active object which demonstrates the use of CTelephony::GetNetworkRegistrationStatus()
	*/
	CISVAPIAsync* iNetworkRegInfo;
	/**
	Active object which demonstrates the use of CTelephony::SendDTMFTones()
	*/
	CISVAPIAsync* iSendDTMF;
	/**
	Active object which demonstrates the use of CTelephony::DialNewCall()
	*/
	CISVAPIAsync* iDialCall;
	/**
	Active object which demonstrates the use of CTelephony::HangUp()
	*/
	CISVAPIAsync* iHangup;
	/**
	Active object which demonstrates the use of CTelephony::GetCallStatus()
	*/
	CISVAPIAsync* iCallStatus;
	/**
	Active object which demonstrates the use of CTelephony::GetLineStatus()
	*/
	CISVAPIAsync* iLineStatus;
	/**
	Call ID of the currently active call.
	*/
	CTelephony::TCallId iCallId;
	/**
	The current single tone the user has specified.
	*/
	TBuf<1> iCurrentTone;
	/**
	A log of all tones sent down the line.
	*/
	TBuf<100> iCurrentToneLog;
	/**
	The number to be sent as a whole DTMF string.
	*/
	TBuf<100> iTheNumber;
	/**
	Has the user selected to enter the DTMF as a single string all
	in one go or as single characters.
	*/
	TBool iSingleString;
	};

#endif // __CMAINMENU_H__

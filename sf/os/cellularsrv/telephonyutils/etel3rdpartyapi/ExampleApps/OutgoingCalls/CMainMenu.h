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
#include "CGetLockInfo.h"
#include "CFlightModeInfo.h"
#include "CNetworkRegInfo.h"
#include "CDialCall.h"
#include "CHold.h"
#include "CResume.h"
#include "CSwap.h"
#include "CHangup.h"
#include "CLineStatus.h"
#include "CCallStatus.h"
#include "CCurrentNetworkName.h"
#include "CCurrentNetworkInfo.h"
#include "COperatorName.h"
#include "CCallWaitingStatus.h"
#include "CAnswerIncomingCall.h"


_LIT(KPhoneIdMsg, "Phone Id:\n");
_LIT(KNetworkInfoMsg, "Network Info:\n");
_LIT(KNetworkNameMsg, "Network Name:\n");
_LIT(KOperatorNameMsg, "Operator Name:\n");
_LIT(KNetworkRegMsg, "Network Registration Status:\n");
_LIT(KCallWaitingStatusMsg, "Call Waiting Status:\n");
_LIT(KMenuMsg, "*~Outgoing Call~*\nPress E to exit\n");
_LIT(KIncomingMsg, "Press I to simulate an incoming call\n");
_LIT(KHoldMsg, "Press H to put current call on Hold\n");
_LIT(KResumeMsg, "Press R to resume the on hold call\n");
_LIT(KSwapMsg, "Press S to swap the active and on hold calls\n");
_LIT(KHangupMsg, "Press Esc Hangup the active Call\n");
_LIT(KDialMsg, "Press D to Dial a Call\n");
_LIT(KAnswerMsg, "Press A to Answer the incoming Call\n");

_LIT(KTheNumber, "01632960000");

/**
Provides the required functionality for the application to run correctly. Owns 
required objects to make Etel 3rd Party calls required by the application.
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

	/**
	Prints the correct menu with the correct options that can be selected.
	*/
	void PrintCorrectMenu(CTelephony::TCallCapsV1 callCapsV1);

	/**
	Retrieves the call capabilities of the currently active call ID.
	*/
	CTelephony::TCallCapsV1 GetCurrentDynamicCaps();

	/**
	Places a request for input to the console. Reads the character input from the
	keypad/keyboard etc.
	*/
	void GetInput();

	// Data
private:

	// Owned active objects
	/**
	Active object which demonstrates the use of CTelephony::GetPhoneId()
	*/
	CISVAPIAsync* iPhoneId;
	/**
	Active object which demonstrates the use of CTelephony::GetCurrentNetworkInfo()
	*/
	CISVAPIAsync* iNetworkInfo;
	/**
	Active object which demonstrates the use of CTelephony::GetCurrentNetworkName()
	*/
	CISVAPIAsync* iNetworkName;
	/**
	Active object which demonstrates the use of CTelephony::GetOperatorName()
	*/
	CISVAPIAsync* iOperatorName;
	/**
	Active object which demonstrates the use of CTelephony::DialNewCall()
	*/
	CISVAPIAsync* iDialCall;
	/**
	Active object which demonstrates the use of CTelephony::Hangup()
	*/
	CISVAPIAsync* iHangup;
	/**
	Active object which demonstrates the use of CTelephony::Hold()
	*/
	CISVAPIAsync* iHold;
	/**
	Active object which demonstrates the use of CTelephony::Swap()
	*/
	CISVAPIAsync* iSwap;
	/**
	Active object which demonstrates the use of CTelephony::Resume()
	*/
	CISVAPIAsync* iResume;
	/**
	Active object which demonstrates the use of CTelephony::GetFlightMode()
	*/
	CISVAPIAsync* iFlightModeInfo;
	/**
	Active object which demonstrates the use of CTelephony::GetNetworkRegistrationStatus()
	*/
	CISVAPIAsync* iNetworkRegInfo;
	/**
	Active object which demonstrates the use of CTelephony::GetLineStatus()
	*/
	CISVAPIAsync* iLineStatus;
	/**
	Active object which demonstrates the use of CTelephony::GetLockInfo()
	*/
	CISVAPIAsync* iGetLockInfo;
	/**
	Active object which demonstrates the use of CTelephony::GetCallWaitingStatus()
	*/
	CISVAPIAsync* iCallWaiting;
	/**
	Active object which demonstrates the use of CTelephony::AnswerIncomingCall()
	*/
	CISVAPIAsync* iAnswerIncomingCall;
	/**
	Active object which demonstrates the use of CTelephony::GetCallStatus()
	*/
	CISVAPIAsync* iCallStatus;
	/**
	Active object which demonstrates the use of CTelephony::GetCallStatus()
	*/
	CISVAPIAsync* iCallStatus2;
	/**
	The call status of active call.
	*/
	CISVAPIAsync* iLastCallStatus;
	/**
	The call Id of the active call.
	*/
	CTelephony::TCallId iCallId;
	/**
	The call Id of another active call.
	*/
	CTelephony::TCallId iCallId2;
	/**
	Is the call waiting supplementary service provisioned.
	*/
	TBool iCallWaitingOn;
	/**
	Specifies if the phone is ringing or not. Used to set how the
	EAnswerIncomingCall iState is handled.
	*/
	TBool iRinging;
	/**
	Specifies that the active call is put on hold and then the ringing call
	should be answered.
	*/
	TBool iThenAnswer;

	};

#endif // __CMAINMENU_H__


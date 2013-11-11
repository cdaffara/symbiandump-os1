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
#include "CIdentityServiceStatus.h"
#include "CCallWaitingStatus.h"

#include "CLineStatus.h"
#include "CCallStatus.h"
#include "CAnswerIncomingCall.h"
#include "CDummyAnswer.h"
#include "CCallInfo.h"
#include "CHangup.h"

_LIT(KPhoneIdMsg, "Phone Id:\n");
_LIT(KCallForwardingStatusMsg, "Call Forwarding Status:\n");
_LIT(KCallBarringStatusMsg, "Call Barring Status:\n");
_LIT(KIdentityServiceStatusMsg, "Identity Service Status:\n");
_LIT(KCallWaitingStatusMsg, "Call Waiting Status:\n");
_LIT(KMenuMsg, "~* Answer Phone *~\nPress E to exit\n");
_LIT(KHangupMsg, "Press H to simulate a remote hangup\n");
_LIT(KAnyMsg, "Press Any key to simulate an incoming Call\n");

/**
Provides the required functionality for the application to run correctly. Owns the
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

	void RunL();	// Handles Key presses
	void DoCancel();

	/**
	Prints message if you have recieved messages. Also alerts if people have
	called and left no message.
	*/
	void CallsCount();

private:
	// Owned active objects
	/**
	Active object which demonstrates the use of CTelephony::GetPhoneId().
	*/
	CISVAPIAsync* iPhoneId;
	/**
	Active object which demonstrates the use of CTelephony::GetIdentityServiceStatus().
	*/
	CISVAPIAsync* iIdentityService;
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
	Active object which demonstrates the use of CTelephony::GetLineStatus().
	*/
	CISVAPIAsync* iLineStatus;
	/**
	Active object which demonstrates the use of CTelephony::GetCallStatus().
	*/
	CISVAPIAsync* iCallStatus;
	/**
	Active object which demonstrates the use of CTelephony::AnswerIncomingCall().
	*/
	CISVAPIAsync* iAnswerIncomingCall;
	/**
	Active object which demonstrates the use of CTelephony::Hangup().
	*/
	CISVAPIAsync* iHangup;
	/**
	Owned CDummyAnswer object that simulates answering a call.
	*/
	CDummyAnswer* iDummyAnswer;
	/**
	Active object which demonstrates the use of CTelephony::GetCallInfo().
	*/
	CISVAPIAsync* iCallInfo;
	/**
	The call ID of the currently active call.
	*/
	CTelephony::TCallId iCallId;
	/**
	Specifies if this is the first run of the application so initialisation
	data is only set once.
	*/
	TInt iCount;
	/**
	Counts how many messages have been received.
	*/
	TInt iMessages;
	/**
	Counts the number of recieved calls regardless of whether they left messages or
	not.
	*/
	TInt iRecievedCalls;
	/**
	Stores whether the call waiting supplementary service is provisioned.
	*/
	TBool iCallWaitingOn;
	};

#endif // __CMAINMENU_H__

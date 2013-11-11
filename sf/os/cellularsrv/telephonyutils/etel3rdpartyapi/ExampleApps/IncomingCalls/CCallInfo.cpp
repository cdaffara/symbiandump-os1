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

#include "CCallInfo.h"

/**
Factory constructor.

@param  aController Pointer to MExecAsync object passed to constructor of 
                    CISVAPIBase
@return             Instance of CCallInfo class
*/
CCallInfo* CCallInfo::NewL(MExecAsync* aController)
	{
	CCallInfo* self = new(ELeave) CCallInfo(aController);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/**
Destructor.
Cancels outstanding requests.
*/
CCallInfo::~CCallInfo()
	{
	Cancel();
	}

/**
Gets the current call's information, stores the details in the iCallInfoV1Pckg
and iRemoteInfoV1Pckg packages and displays the details to the console.
*/
void CCallInfo::DoStartRequestL()
	{
	CTelephony::TCallSelectionV1 callSelectionV1;
	CTelephony::TCallSelectionV1Pckg callSelectionV1Pckg( callSelectionV1 );

	callSelectionV1.iLine = CTelephony::EVoiceLine;
	callSelectionV1.iSelect = CTelephony::EInProgressCall;
	
	// Retrieves information about the call selected by the aCallSelect argument.
	iTelephony->GetCallInfo(callSelectionV1Pckg,
	                        iCallInfoV1Pckg,
	                        iRemoteInfoV1Pckg);

	switch(iCallInfoV1.iStatus)
		{
	case CTelephony::EStatusRinging:
		iConsole->Printf(_L("RING RING RING\n"));
		break;
	case CTelephony::EStatusConnected:
		iConsole->Printf(_L("Call Status: Connected\n"));
		break;
	case CTelephony::EStatusConnecting:
		iConsole->Printf(_L("Call Status: Connecting\n"));
		break;
	case CTelephony::EStatusAnswering:
		iConsole->Printf(_L("Call Status: Answering\n"));
		break;
	case CTelephony::EStatusIdle:
		iConsole->Printf(_L("Call Status: Idle\n"));
		break;
	case CTelephony::EStatusDisconnecting:
		iConsole->Printf(_L("Call Status: Disconnecting\n"));
		break;
	default:
		iConsole->Printf(_L("Call status: Changed\n"));
		break;
		}
	TDateTime time = iCallInfoV1.iStartTime;
	
	// Print the received call information
	iConsole->Printf(_L("Call recieved at %d:%d:%d\n"),
	                     time.Hour(),
	                     time.Minute(),
	                     time.Second());
	iConsole->Printf(_L("on %d.%d.%d\n"),
	                     time.Day(),
	                     time.Month(),
	                     time.Year());

	if (iRemoteInfoV1.iRemoteIdStatus == CTelephony::ERemoteIdentityAvailable)
		{
		iConsole->Printf(iRemoteInfoV1.iRemoteNumber.iTelNumber);
		}
	else
		{
		iConsole->Printf(_L("Private Number Dialling\n"));
		}
	ExampleComplete();
	}

/**
Constructor.

@param aController Pointer to an MExecAsync object passed to constructor of 
                   CISVAPIBase
*/
CCallInfo::CCallInfo(MExecAsync* aController)
	: CISVAPIAsync(aController, KCallInfo),
	  iCallInfoV1Pckg(iCallInfoV1),
	  iRemoteInfoV1Pckg(iRemoteInfoV1)
	{
	// Empty method
	}

/**
Second phase constructor.
*/
void CCallInfo::ConstructL()
	{
	// Empty method
	}

/**
Does nothing.
*/
void CCallInfo::RunL()
	{
 	// Empty method
	}

/**
Does nothing.
*/
void CCallInfo::DoCancel()
	{
	// Empty method
	}

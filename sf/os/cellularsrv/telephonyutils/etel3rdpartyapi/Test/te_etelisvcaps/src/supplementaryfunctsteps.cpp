// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Implmentation of all test objects required to verify capability check on 
// ETel3rdParty supplementary functionality part.
// 
//

/**
 @file 
 @internalTechnology
*/

#include "supplementaryfunctsteps.h"

//
CGetCallForwardingStatusTestStep::CGetCallForwardingStatusTestStep()
	: iCallForwardingPckg(iCallForwarding)
	{
	}
   
TInt CGetCallForwardingStatusTestStep::IssueRequestL(TRequestStatus& aStatus)
	{
	iTelephony->GetCallForwardingStatus(aStatus, 
										CTelephony::ECallForwardingUnconditional,
										iCallForwardingPckg,
										CTelephony::EVoiceService);	
	return KErrNone;
	}

TInt CGetCallForwardingStatusTestStep::CancelRequestL()
	{
	iTelephony->CancelAsync(CTelephony::EGetCallForwardingStatusCancel);
	return KErrNone;
	}

//
CGetCallBarringStatusTestStep::CGetCallBarringStatusTestStep()
	:iCallBarringPckg(iCallBarring)
	{
	}
   
TInt CGetCallBarringStatusTestStep::IssueRequestL(TRequestStatus& aStatus)
	{
	iTelephony->GetCallBarringStatus(aStatus, 
									 CTelephony::EBarAllIncoming,
									 iCallBarringPckg,
									 CTelephony::EVoiceService);	
	return KErrNone;
	}

TInt CGetCallBarringStatusTestStep::CancelRequestL()
	{
	iTelephony->CancelAsync(CTelephony::EGetCallBarringStatusCancel);
	return KErrNone;
	}

//
CGetCallWaitingStatusTestStep::CGetCallWaitingStatusTestStep()
	:iCallWaitingPckg(iCallWaiting)
	{
	}
   
TInt CGetCallWaitingStatusTestStep::IssueRequestL(TRequestStatus& aStatus)
	{
	iTelephony->GetCallWaitingStatus(aStatus, 
									 iCallWaitingPckg,
									 CTelephony::EVoiceService);	
	return KErrNone;
	}

TInt CGetCallWaitingStatusTestStep::CancelRequestL()
	{
	iTelephony->CancelAsync(CTelephony::EGetCallWaitingStatusCancel);
	return KErrNone;
	}

//
CGetIdentityServiceTestStep::CGetIdentityServiceTestStep()
	:iIdentityServicePckg(iIdentityService)
	{
	}
   
TInt CGetIdentityServiceTestStep::IssueRequestL(TRequestStatus& aStatus)
	{
	iTelephony->GetIdentityServiceStatus(aStatus, 
										 CTelephony::EIdServiceCallerPresentation,
										 iIdentityServicePckg);	
	return KErrNone;
	}

TInt CGetIdentityServiceTestStep::CancelRequestL()
	{
	iTelephony->CancelAsync(CTelephony::EGetIdentityServiceStatusCancel);
	return KErrNone;
	}

//

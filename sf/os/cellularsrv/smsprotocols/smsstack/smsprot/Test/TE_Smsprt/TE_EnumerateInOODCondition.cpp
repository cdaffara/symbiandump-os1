// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
*/

#include "Te_SmsTestSteps.h"

CEnumerateInOODCondition::CEnumerateInOODCondition(RSocketServ &aSocketServer)
/** 
	Each test step initialises it's own name
*/
	{
    iSharedSocketServer = &aSocketServer;
    iPartOfMultiStepTestCase = ETrue;
	}

/**
  Enumerates SIM messages.
 */
TVerdict CEnumerateInOODCondition::doTestStepL()
	{
#ifndef _DEBUG
    INFO_PRINTF1(_L("This test can only be run when the SMS Stack is in debug mode."));
#else   
	
	//Open a socket 
	RSocket socket;
	OpenSmsSocketL(*iSharedSocketServer, socket, ESmsAddrRecvAny);
    CleanupClosePushL(socket);
	
	TRequestStatus status;
	TPckgBuf<TUint> sbuf;
	sbuf()=0;

	//Now enumerate messages from store
	socket.Ioctl(KIoctlEnumerateSmsMessages,status,&sbuf, KSolSmsProv);
	User::WaitForRequest(status);
	TESTCHECK(status.Int(), KErrDiskFull, "Checking the Enumeration was unsuccessful due to the disk being full");
	
    CleanupStack::PopAndDestroy(&socket);
#endif		
	return TestStepResult();
	}

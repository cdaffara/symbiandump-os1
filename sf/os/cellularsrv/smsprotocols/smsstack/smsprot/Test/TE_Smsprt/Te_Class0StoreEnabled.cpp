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

CClass0StoreEnabled::CClass0StoreEnabled(RSocketServ &aSocketServer)
/** 
	Each test step initialises it's own name
*/
	{
    iSharedSocketServer = &aSocketServer;
	iPartOfMultiStepTestCase = ETrue;
	}


/**
  Checks wheather the CLASS0 store has in fact been enabled.
 */
TVerdict CClass0StoreEnabled::doTestStepL()
	{	
	TBool isClass0StoreEnabled = EFalse;
	GetBoolFromConfig(ConfigSection(), _L("isClass0StoreEnabled"), isClass0StoreEnabled);
		
	//Open a socket 
	RSocket socket;
	OpenSmsSocketL(*iSharedSocketServer, socket, ESmsAddrRecvAny);
	CleanupClosePushL(socket);

	TRequestStatus status;
	socket.Ioctl(KIoctlSupportOODClass0SmsMessages, status, NULL, KSolSmsProv);
	User::WaitForRequest(status);

	TEST((isClass0StoreEnabled  &&  status == KErrNone)  ||
		 (!isClass0StoreEnabled  &&  status == KErrNotSupported));
	
	CleanupStack::PopAndDestroy(&socket);
	return TestStepResult();
	}

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
//

/**
 @file
*/

#include "Te_SmsTestSteps.h"

CSendSMS::CSendSMS(RSocketServ &aSocketServer)
/** 
	Each test step initialises it's own name
*/
	{
    iSharedSocketServer = &aSocketServer;
	iPartOfMultiStepTestCase = ETrue;
	}

/**
  Send SMS Message
*/
TVerdict CSendSMS::doTestStepL()
	{
#ifndef _DEBUG
    INFO_PRINTF1(_L("This test can only be run when the SMS Stack is in debug mode."));
#else
	//Open the socket for sending messages
	RSocket socket;
	OpenSmsSocketL(*iSharedSocketServer, socket, ESmsAddrSendOnly);
    CleanupClosePushL(socket);
    
	//Create SMS
	CSmsMessage* sms=CreateSMSL();
    CleanupStack::PushL(sms);

	if( sms )
		{
		SendSmsL(sms,socket);				
		}
	else
		{
		INFO_PRINTF1(_L("Error creating SMS") );
		SetTestStepResult(EFail);
		}

    CleanupStack::PopAndDestroy(sms);
	CleanupStack::PopAndDestroy(&socket);
#endif	
	return TestStepResult();
	}

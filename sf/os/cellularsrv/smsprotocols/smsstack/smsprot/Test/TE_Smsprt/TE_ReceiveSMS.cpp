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

CReceiveSMS::CReceiveSMS(RSocketServ &aSocketServer)
/** 
	Each test step initialises it's own name
*/
	{
    iSharedSocketServer = &aSocketServer;
	iPartOfMultiStepTestCase = ETrue;
	}

/**
  Receive SMS Message
*/
TVerdict CReceiveSMS::doTestStepL()
	{	
#ifndef _DEBUG
    INFO_PRINTF1(_L("This test can only be run when the SMS Stack is in debug mode."));
#else   
	//Read from the INI file
	TInt timeout=10;
	GetIntFromConfig(ConfigSection(), _L("timeout"), timeout);
	
	TBool messageExpected;
	GetBoolFromConfig(ConfigSection(), _L("isClientExpectingMessage"), messageExpected);
	
	if( messageExpected && timeout < 40 )
		{
		// Change timeout to be 40 seconds
		timeout = 40;
		}

	INFO_PRINTF1(_L("Receiving SMS message..."));

	//Open the socket for receiving messages
	RSocket socket;
	OpenSmsSocketL(*iSharedSocketServer, socket, ESmsAddrRecvAny);
	CleanupClosePushL(socket);

	//Message must be received in "timeout" else the test will fail	
	INFO_PRINTF2(_L("Wait for message (timeout: %d)..."), timeout);
	TBool messageReceived = TimedWaitForRecvL(socket, timeout*1000000);
	
    if( !messageReceived )
        {
        if( messageExpected )
            {
            ERR_PRINTF1(_L("Message expected - not received!"));  
            TEST(EFalse);
            }
        else
            {
            INFO_PRINTF1(_L("No message expected and not received"));  
            }
        }
    else
        {
        CSmsMessage* smsMessage = RecvSmsL(socket);   //Get the message from the STACK.
        CleanupStack::PushL(smsMessage);
        
        if( !messageExpected )
            {
            ERR_PRINTF1(_L("Message not expected!"));  
            TEST(EFalse);
            }
        else
            {
            //Get the text from the ini file
            TPtrC messageText;
            GetStringFromConfig(ConfigSection(), _L("messageExpected"), messageText);

            if(messageText!=_L(""))
                {
                TestSmsContentsL(smsMessage,messageText);
                }
            }
        CleanupStack::PopAndDestroy(smsMessage);
        }
	CleanupStack::PopAndDestroy(&socket);
#endif
	return TestStepResult();
	}

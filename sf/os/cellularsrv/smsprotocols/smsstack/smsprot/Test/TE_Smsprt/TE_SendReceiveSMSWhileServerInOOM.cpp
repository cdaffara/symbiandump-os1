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
// TE_SendReceiveSMSWhileServerInOOM.cpp.cpp
// 
//

/**
 @file
*/

#include "Te_SmsTestSteps.h"

CSendReceiveSMSWhileServerInOOM::CSendReceiveSMSWhileServerInOOM(RSocketServ &aSocketServer)
/** 
	Each test step initialises it's own name
*/
	{
    iSharedSocketServer = &aSocketServer;
	iPartOfMultiStepTestCase = ETrue;
	}

/**
  Send and Receive a SMS Message while the socket server has been placed in OOM
*/
TVerdict CSendReceiveSMSWhileServerInOOM::doTestStepL()
	{
#ifndef _DEBUG
    INFO_PRINTF1(_L("This test can only be run when the SMS Stack is in debug mode."));
#else	
	//Read from the INI file.  
	TInt timeout=10;
	GetIntFromConfig(ConfigSection(), _L("timeout"), timeout);
	
	//Open the socket for sending messages
	RSocket socket;
	OpenSmsSocketL(*iSharedSocketServer, socket, ESmsAddrSendOnly);
	CleanupClosePushL(socket);
	
	//Create SMS
	CSmsMessage* sms=CreateSMSL();
	CleanupStack::PushL(sms);

	TBool isClientExpectingMessage;
	GetBoolFromConfig(ConfigSection(), _L("isClientExpectingMessage"), isClientExpectingMessage);

	INFO_PRINTF1(_L("Sending SMS...") );
	PrintMessageL(sms);
			
	//Send SMS
	SendSmsL(sms,socket);
	CleanupStack::PopAndDestroy(sms);
	CleanupStack::PopAndDestroy(&socket);
	
	//Open the socket for receiving messages
	RSocket recSocket;
	OpenSmsSocketL(*iSharedSocketServer, recSocket, ESmsAddrRecvAny);
	CleanupClosePushL(recSocket);
	
	//Receive
	TInt allocFailCount=0;
	TInt ret=KErrNoMemory;
	while (ret==KErrNoMemory || ret==KErrEof)
		{
				
		INFO_PRINTF2(_L("Heap checking for allocation %d"),allocFailCount );
		//Message must be received in "timeout" else the test will fail	
		TBool messageReceived = TimedWaitForRecvL(recSocket, timeout*1000000);
		
		//Set the result to "fail" if no message is received in the specified time
		if(messageReceived && isClientExpectingMessage )
			{
			INFO_PRINTF1(_L("Message received!"));
			
			iSharedSocketServer->__DbgMarkHeap();
			iSharedSocketServer->__DbgFailNext(allocFailCount);
			
			CSmsMessage *iSmsMessage=NULL;
			
			TRAP(ret,iSmsMessage=RecvSmsL(recSocket));	//Get the message from the STACK.
			iSharedSocketServer->__DbgMarkEnd(0);			

			//If a message was retrieved successfully.
			if(ret==KErrNone)
				{
				INFO_PRINTF1(_L("Message received!"));		
			
				CleanupStack::PushL(iSmsMessage);
				
				//Get the text from the ini file
				TPtrC messageText;
				GetStringFromConfig(ConfigSection(), _L("messageExpected"), messageText);

				if(messageText!=_L(""))
					{
					TestSmsContentsL(iSmsMessage,messageText);
					}
				CleanupStack::PopAndDestroy(iSmsMessage);
				}
			++allocFailCount;
			}
		}

	CleanupStack::PopAndDestroy(&recSocket);
	iSharedSocketServer->__DbgFailNext(-1); // Reset heap
#endif	
	return TestStepResult();
	}


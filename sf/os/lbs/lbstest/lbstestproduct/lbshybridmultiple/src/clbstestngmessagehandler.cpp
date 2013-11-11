// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Description: Extra test interface to the NetProtocolProxy.  It allows the test
//  to be asynchronous and continue to run other events whilst waiting for a message
//  from the NG. The test is notified when a message arrives (or if it times out)
// 
//

#include "clbstestngmessagehandler.h"


/** 2nd Phase constructor
 */
CLbsTestNgMessageHandler* CLbsTestNgMessageHandler::NewL(MLbsTestNgMessageHandler& aObserver, TInt aPmId)
	{
	CLbsTestNgMessageHandler* self = new (ELeave) CLbsTestNgMessageHandler(aObserver, aPmId);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CLbsTestNgMessageHandler::~CLbsTestNgMessageHandler()
	{
	Cancel();
	iRequestTimeout.Close();
	delete iProxy;
	}

void CLbsTestNgMessageHandler::ConstructL()
	{
	iProxy = CNetProtocolProxy::NewL(iPmId);
	}

CLbsTestNgMessageHandler::CLbsTestNgMessageHandler(MLbsTestNgMessageHandler& aObserver, TInt aPmId)
:CActive(CActive::EPriorityStandard),
iObserver(aObserver),
iPmId(aPmId)
	{
	CActiveScheduler::Add(this);
	}

/* AO RunL method, called when either:
 *  - Message received from the NG
 *  - Timeout occurs (-1000)
 * The message type is returned back to the test.  It then decides whether it should set itself
 *  listening again for another message
 */
void CLbsTestNgMessageHandler::RunL()
	{
	//Callback the test, to let it know a message has been received. The iStatus contains the ID of the message received
	iObserver.OnHandleNGMessage(iPmId, iStatus.Int());
	
	//Check to see whether any requests were made when we were busy completing this request
	if((iRequestTimeout.Count() > 0) && (iStatus.Int() != -1000))
		{
		//Set us off listening again, with the stored timeout value
		WaitForResponseL(iRequestTimeout[0]);
		//This request has been satisfied, remove the timeout value from the array
		iRequestTimeout.Remove(0);
		}
	}

void CLbsTestNgMessageHandler::DoCancel()
	{
	//Cancel waiting for a message
	iProxy->CancelWait();
	}

TInt CLbsTestNgMessageHandler::RunError(TInt /*aError*/)
	{
	return KErrNone;
	}


/* Sets the NetProtocolProxy off listening for a new message from the NG. This function
 *  does not block, but asynchronously waits for a new message to arrive. It can be called
 *  even when already active, it will remember that a request was made and will re-request
 *  as soon as the first request finishes.
 *  NOTE: If this is called when we are already active, the timeout is not started straightway,
 *  but is stored.  As such, the timeout value will be larger than specified.
 *  
 * @param aTimeOut Specifies the time that should be spent waiting for the message to arrive
 */
void CLbsTestNgMessageHandler::WaitForResponseL(TInt aTimeOut)
	{
	if(!IsActive())
		{
		iProxy->WaitForResponseL(aTimeOut, iStatus);
		SetActive();
		}
	else
		{
		//Remember that a sequence requested we listen for a message when we were busy
		iRequestTimeout.Append(aTimeOut);
		}
	}

/* Gives direct access to the NetProtocolProxy which allows the test to directly send messages 
 * and access elements of the message.  It should not be used for waiting for a message (use
 * WaitForResponseL from this class) 
 */
CNetProtocolProxy* CLbsTestNgMessageHandler::ProtocolProxy()
	{
	return iProxy;
	}

/** Returns the PM ID of the PM that this Network proxy is talking to.
 */
TInt CLbsTestNgMessageHandler::GetPmId()
	{
	return iPmId;
	}

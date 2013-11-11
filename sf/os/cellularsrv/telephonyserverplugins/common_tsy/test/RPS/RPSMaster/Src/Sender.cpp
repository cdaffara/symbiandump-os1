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

#include "sender.h"
#include "IpInfoM.h"

#include <e32debug.h>

#include "rpsloggermacros.h"
#include "rpscommon.h"
	
CSender* CSender::NewL()
	{
	CSender* self = new(ELeave) CSender();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CSender::~CSender()	
	{
	iSendSocket.CancelAll();
	iSendSocket.Close();
	
	iSocketServ.Close();
	
	iBuffer.Close();
	iBufferAll.Close();
	}
	
void CSender::ConstructL()
	{
	// Connect to socket server
	User::LeaveIfError(iSocketServ.Connect());
	User::LeaveIfError(iSocketServ.ShareAuto());  // allow other threads to use
	
	
	iSendSocket.Close();
	User::LeaveIfError(iSendSocket.Open(iSocketServ, KAfInet, KSockStream, KProtocolInetTcp));
	
	TRequestStatus status;

	// Request the Socket to connect to the destination address (implicit Connection)
	TInetAddr destAddr;
	destAddr.Input(KRemoteAddr);
	destAddr.SetPort(KRemotePort);
	iSendSocket.Connect(destAddr, status);
	
	RDEBUGPRINTLOGGER1(_L("RPS: Waiting for Send socket to connect to remote destination ..."));
	User::WaitForRequest(status);
	RDEBUGPRINTLOGGER2(_L("RPS: Wait over. Status returned: %d\n"), status.Int());
	
	if(status != KErrNone)
		{
		RDEBUGPRINTLOGGER1(_L("RPS: Socket could not Connect!!"));
		User::Leave(KErrCouldNotConnect);
		}
	else
		{
		RDEBUGPRINTLOGGER1(_L("RPS: Send socket connected to remote destination "));
		}
	}
	
	
// SendRequestL
// Creates the msg from the data elements passed in the descriptor array
// and sends over socket connection.
// Then waits until a response is received from the slave.
// Caller has to pass in empty HBufC8.
TInt CSender::SendRequestL(CRpsMsg& aRpsMsg, HBufC8** aResp)
	{
	
	// Generate request data buf
	HBufC8* msg = NULL;
	aRpsMsg.ExternalizeL(msg);
	CleanupStack::PushL(msg);

	// Send the msg over the socket
	TRequestStatus sendStatus;
	iSendSocket.Send(*msg, 0, sendStatus);
	User::WaitForRequest(sendStatus);
	CleanupStack::PopAndDestroy(msg);
	if(sendStatus != KErrNone)
		{
		RDEBUGPRINTLOGGER2(_L("CSender::SendRequestL Send error [%d]"),sendStatus.Int());
		return sendStatus.Int();
		}

	// Expect a response msg back from slave synchronously.
	// Return an error if resp not received in time-out.
	// Else pass the response msg up to the caller.

	// Expect the msg header+length
	TRequestStatus recvStatus;
	iBufferPreRead.Delete(0, iBufferPreRead.Length());
	iSendSocket.Recv(iBufferPreRead, 0, recvStatus);
	
	// ** TODO: check for error value in recvStatus (-191 returned if slave has crashed) **
	TInt ret = GetRequestOrTimeOut(recvStatus, KRespFromSlaveTimeout);
	if (recvStatus.Int() != KErrNone)
		{
		RDEBUGPRINTLOGGER2(_L("CSender::SendRequestL Recv error [%d]"),recvStatus.Int());
		}
		
	if(ret != KErrNone)
		{
		// We didn't receive a response within timelimit. 
		return ret;
		}
	 
	// Receive the rest of the msg.
	// Get msg length from pre-read
	TUint msgLen(0);
	TLex8 lex(iBufferPreRead.Right(KMaxMsgLenChars));
	lex.Val(msgLen);

	__ASSERT_ALWAYS(msgLen > KRespSocketPreReadSize, User::Panic(KPanicMsgFormat, 303)); // TODO - panic number?
	
	// Now read the rest of msg
	iBuffer.Close();
	iBuffer.CreateL(msgLen - KReqstSocketPreReadSize);
	// Recv operation should complete immediately if msg has been formatted correctly
	iSendSocket.Recv(iBuffer, 0, recvStatus);
	ret = GetRequestOrTimeOut(recvStatus, KRespFromSlaveTimeout);
	if(ret != KErrNone)
		{
		// We didn't receive a response within timelimit. 
		return ret;
		}
	// Check status of Recv operation
	if(recvStatus != KErrNone)
		{
		return recvStatus.Int();
		}
	
	iBufferAll.Close();
	iBufferAll.CreateL(msgLen);		// Give HandleRequestL the entire msg
	iBufferAll += iBufferPreRead;
	iBufferAll += iBuffer;
	
	// For debug ...
	RBuf16 buf;
	buf.CreateL(iBufferAll.Length());
	buf.Copy(iBufferAll);
	buf.Close();
	
	// Passing entire response msg to caller
	// Caller owns memory
	*aResp = iBufferAll.AllocL();
	
	iBufferPreRead.Delete(0, iBufferPreRead.Length());
	iBuffer.Close();
	iBufferAll.Close();

	return KErrNone;
	}

	

/**
 * Acts exactly like User::WaitForRequest, except aTimeOut is a timeout period 
 * to give up waiting after. 
 *
 * @return		If the request times out, returns KErrTimedOut, otherwise
 *				returns KErrNone and aRequestStatus is completed as usual.
 *
 */	
TInt CSender::GetRequestOrTimeOut(	TRequestStatus& aRequestStatus, 
									TTimeIntervalMicroSeconds32 aTimeOut)
	{
	RTimer timer;
	TRequestStatus timerRequest;
	timer.CreateLocal();
	timer.After(timerRequest, aTimeOut);

	// Wait for the requestto complete or until we time out
	User::WaitForRequest(timerRequest, aRequestStatus);

	// If timer is no longer pending => we have timed out
	if (timerRequest != KRequestPending)
		{
		RDEBUGPRINTLOGGER1(_L("RPS: GetRequestOrTimeOut *** Timed out ***"));
		return KErrTimedOut;
		}

	timer.Cancel();

	User::WaitForRequest(timerRequest);

	return KErrNone;
	}

// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// @file CEchoSocket.cpp
// This contains the CEchoSocket class implementation and its saftey timer, CSafety 
// 
//

/**
 @file
*/

#include "CEchoSocket.h"

CEchoSocket::CEchoSocket() : CActive(EPriorityNormal),
							iSockErr(KErrNone),
							iPacketSize(CTS_MultiHomingStep::PACKET_SIZE), 
							iPacketsToSend(CTS_MultiHomingStep::NUM_OF_PACKETS),
							iRecvdGood(0), 
							iProtocol(KProtocolInetUdp), 
							iUDPTolerance(CTS_MultiHomingStep::UDP_TOLERANCE),
							iPtrRecvd(0,0),
							iPtrRecvdThisRead(0,0),
							iPtrWritn(0,0),
							iConsecRecvTimeOuts(0),
							iIsListener(EFalse)
	{
	}

CEchoSocket::~CEchoSocket()
	{
	Cancel();
	delete iWritebuf;
	delete iReadbufThisRead;
	delete iReadbuf;
	delete iSafety;

	}

CEchoSocket* CEchoSocket::NewL()
	{
	
	CEchoSocket* self = new(ELeave) CEchoSocket;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	CEnhancedScheduler::Add(self);
	return self;
	}


void CEchoSocket::ConstructL()
	{
	iSafety = CSafety::NewL(this);

	iWritebuf = HBufC8::NewMaxL(CTS_MultiHomingStep::MAX_PACKET_SIZE);
	iPtrWritn.Set(iWritebuf->Des());
	iReadbuf = HBufC8::NewMaxL(CTS_MultiHomingStep::MAX_PACKET_SIZE);
	iPtrRecvd.Set(iReadbuf->Des());
	iReadbufThisRead = HBufC8::NewMaxL(CTS_MultiHomingStep::MAX_PACKET_SIZE);
	iPtrRecvdThisRead.Set(iReadbufThisRead->Des());
	}

void CEchoSocket::RunL()
	{
	
	switch (iState)
		{
		case EInitialised:
			if(iIsListener)
				{
				iAcceptedConnection.Open( (iTConnection->iSocketServ));
		
				iSocket.Accept(iAcceptedConnection, iStatus);
				iState = EReady;
				SetActive();
				}
			else 
				{
				if (KProtocolInetTcp == iProtocol)
					{
					ConnectTcp();
					iOwnerStep->StepLog(_L("Connecting TCP %S:%i"),&iName, iSockNum);
					}
				else if (KProtocolInetUdp == iProtocol)
					{
					BindUdp();
					iOwnerStep->StepLog(_L("Binding UDP %S:%i"), &iName, iSockNum);
					}
				}
			break;	
			
		case EReady:
			iSafety->Cancel();

			if (iStatus == KErrNone)
				{
				if(iIsListener)
					{
					RecvDataTcpListen();
					}
				else
					{
					SendData();
					}
				}
			else
				{
				iOwnerStep->StepLog(KErrConnectFailed,&iName,iSockNum, iStatus.Int());
				SetComplete();
				}
			break;
			
		case ESentData:
			if (iStatus == KErrNone)
				{
					if(iIsListener)
					{
					RecvDataTcpListen();
					}
				else
					{
					RecvData();
					}
				}
			else
				{
				iOwnerStep->StepLog(KErrSendFailed, &iName, iSockNum, iStatus.Int());
				SetComplete();	// Couldn't send, so bit of a problem...
				}
			break;
		
		case ERecvdData:
			iSafety->Cancel();
			if(iIsListener && iStatus == KErrEof)
				{
				SetComplete();
				}
			else if(iStatus == KErrNone)
				{
				TInt err = KErrNone;
				// Reset our consecutive count
				iConsecRecvTimeOuts = 0;
				// Compare sent and received
				err = iPtrWritn.Compare(iPtrRecvd);
 				if (err == KErrNone || iIsListener)
					{
					iRecvdGood++;
					if (iPacketsToSend>0)
						if(iIsListener)
							SendDataToClient();						
						else 
							SendData();	
					else
						SetComplete();
					}
				else
					{
					// Try reading again, usualy happens because of a timeout
					iOwnerStep->StepLog(KErrPacketsDiff, &iName, iSockNum);
					RecvData();
					}				
				}
			else if (KErrCancel == iStatus.Int()) // We cancelled our receive
				{
				iConsecRecvTimeOuts++;
				iOwnerStep->StepLog(_L("Receive timeout on %S:%i,%d"),&iName, iSockNum,
					iConsecRecvTimeOuts);
				
				if (iPacketsToSend > 0 && iConsecRecvTimeOuts < MAX_CONSEC_TIMEOUT)
					SendData();
				else
					SetComplete();
				}
			else
				{
				TPtrC errorText = CLog::EpocErrorToText(iStatus.Int());
				iOwnerStep->StepLog(KErrRecvFailed, &iName, iSockNum, &errorText);
				SetComplete();
				}
			break;

			
		case EComplete:
			iOwnerStep->iOwnerSuite->iScheduler->DecCount();
			if(0 == iOwnerStep->iOwnerSuite->iScheduler->GetCount())
				CEnhancedScheduler::Stop();
			break;

		default:
			iOwnerStep->StepLog(KTxtWhatHappened);
			break;
		
		}
	// Error handling of iStatus...


	}




void CEchoSocket::DoCancel()
	{
	iSocket.CancelAll();
	// Close is handles in set complete
	//iSocket.Close();		
	}

TInt CEchoSocket::Echo(const TInetAddr& aDest, const TInetAddr& aSrc,
						const TInt& aSize,	const TInt& aPackets, 
						const TUint& aProto, const TInt& aTol,
						TConnDetails* aConn, const TInt& aSockNum, 
						const TBool aIsListener, CTS_MultiHomingStep* aStep)
/**
 * Creates an Echo socket and runs it to completion or error
 * @param aSockServ The socket server to attach to
 * @param aDest The destination IP address
 * @param aSize The packet size to use in bytes
 * @param aPackets The number of packets to send
 * @param aProto The protocol to use
 * @param aTol The UDP data tolerance level
 * @return Error value for initialisation stage
 */
	{
	__ASSERT_ALWAYS(!IsActive(), User::Panic(KTxtCEchoSocket,KErrAlreadyExists));
	__ASSERT_ALWAYS(aSize <= CTS_MultiHomingStep::MAX_PACKET_SIZE, User::Panic(KTxtCEchoSocket,KErrOverflow));
	
	// Remember the config
	iDestAddr = aDest;
	iSrcAddr = aSrc;
	iPacketSize = aSize;
	iPacketsToSend = aPackets;
	iPackets = aPackets;
	iProtocol = aProto;
	iUDPTolerance = aTol;
	iOwnerStep = aStep;
	iTConnection = aConn;
	iSockNum = aSockNum;

	iIsListener = aIsListener;

	TInt err = KErrNone;

	iPtrWritn.SetLength(aSize);
	iPtrRecvd.SetLength(aSize);

	
	if(iTConnection == 0)
		return KErrGeneral;
	// Open the socket for implicit or explicit case
	if(iTConnection->iConnectionType != TConnDetails::implicitConn)
		{
		err = iSocket.Open( (iTConnection->iSocketServ), KAfInet, 
					iProtocol == KProtocolInetTcp ? KSockStream : KSockDatagram, iProtocol,
					(iTConnection->iConnection));
		}
	else
		{
		err = iSocket.Open( (iTConnection->iSocketServ), KAfInet, 
					iProtocol == KProtocolInetTcp ? KSockStream : KSockDatagram, iProtocol);
		}
	if (err != KErrNone)
		return err;			// No point going any further...

	iSocket.Name(iName);
	
	err = iSocket.SetOpt(KSolInetIp, KSoReuseAddr, 1);
	// Ignore this error for unbound sockets
	err = iSocket.Bind(iSrcAddr);
	if(err != KErrNone)
		return err;

	if(iProtocol == KProtocolInetTcp)
		{
		// Disable the nagle algorithm.
		iSocket.SetOpt(KSoTcpNoDelay, KSolInetTcp, 1);
		}
	if(iIsListener)
		{
		// Accept a single connection
		iSocket.Listen(1);
		}

	iState = EInitialised;

	iStatus = KRequestPending;
	SetActive();

	// Increment the echosocket counter and client
	iOwnerStep->iOwnerSuite->iScheduler->IncCount();

	TName addrBuf;
	iDestAddr.Output(addrBuf);
	iOwnerStep->StepLog(KSockDetails,&iName,iSockNum,&addrBuf,iDestAddr.Port(),iPacketsToSend,iPacketSize);

	TRequestStatus* pS = &iStatus;
	User::RequestComplete(pS,KErrNone);

	return KErrNone;
	}



void CEchoSocket::ConnectTcp()
/**
 * Connects a TCP socket to the remote host.
 */
	{
	__ASSERT_ALWAYS(iProtocol==KProtocolInetTcp, User::Panic(KTxtCEchoSocket,KErrArgument));
	iSocket.Connect(iDestAddr,iStatus);
	iSafety->SetSafety();
	iState = EReady;
	SetActive();
	}

void CEchoSocket::BindUdp()
/**
 * Binds a socket to its address
 */
	{
	__ASSERT_ALWAYS(KProtocolInetUdp == iProtocol, User::Panic(KTxtCEchoSocket,KErrArgument));

	iSocket.Connect(iDestAddr,iStatus);
	iSafety->SetSafety();
	iState = EReady;
	SetActive();
	}



void CEchoSocket::SendData()
/**
 * Sends a packet on the socket and updates state information
 */
	{
	// Packet data
	TBuf8<100> data;
	// Write data to socket 
	if (iProtocol == KProtocolInetTcp)
		{
		data.Format(_L8("TCP-packet[%S:%d] MultiHoming"), &iName, iPacketsToSend--);
		}
	else if (iProtocol == KProtocolInetUdp)
		{
		data.Format(_L8("UDP-packet[%S:%d] MultiHoming"), &iName, iPacketsToSend--);
		}
	iPtrWritn.Repeat( data );

	iPtrRecvd.SetLength( iPtrWritn.Length() );

	iSocket.Send(iPtrWritn,0,iStatus);
	
	iState = ESentData;
	SetActive();
	}

void CEchoSocket::SendDataToClient()
	{
	iAcceptedConnection.Send(iPtrRecvd,0,iStatus);
	
	iState = ESentData;
	SetActive();
	}
void CEchoSocket::RecvDataTcpListen()
/** 
 * Receives data on socket
 *
 */
	{
	// Receive data
	iSafety->SetSafety();
	TInt expected = iPtrRecvd.Length();
    iAcceptedConnection.Read(iPtrRecvd, iStatus);
    iOwnerStep->StepLog(_L("Receiving Data TCP Listen %S:%i"),&iName, iSockNum);
	iState = ERecvdData;
	SetActive();
	}



void CEchoSocket::RecvData()
/** 
 * Receives data on socket
 *
 */
	{
	// Receive data
	iSafety->SetSafety();
	TInt expected = iPtrRecvd.Length();
    if(iProtocol == KProtocolInetTcp)
        {
	    iSocket.Read(iPtrRecvd, iStatus);
        }
    else if(iProtocol == KProtocolInetUdp)
        {
	    iSocket.Recv(iPtrRecvd, 0, iStatus);
        }
	if(expected != iPtrRecvd.Length())
		{
	    _LIT(KErrMsg, "%S: Expected to Recv %i octets, actual is %i");
		iOwnerStep->StepLog(KErrMsg, &iName, expected, iPtrRecvd.Length());
		}
	iState = ERecvdData;
	SetActive();
	}

TInt CEchoSocket::LogReport(TBool& aPassed, TInt& aNumSent, TInt& aUdpRate)
/**
 * Allows a CEchoSocket to pass back its outcome
 * @param aPassed Did the test pass?
 * @param aUdpRate Percentage of UDP packets dropped
 * @return The last error state of the socket
 */
	{
	__ASSERT_ALWAYS(iState==EComplete,User::Panic(KTxtCEchoSocket,KErrNotReady));

	aPassed=EFalse;
	aNumSent = iPackets;
	if (KProtocolInetTcp == iProtocol && iRecvdGood == iPackets)
		{
		aPassed=ETrue;
		aUdpRate=0;
		}
	else
		{
		TInt dropped = 100;
		if (iPackets != 0)	// No div/0 errs thanks...
			// nL = nS - nR
			// nL/nS = 1 - nR/nS 
			dropped = 100 - ((100*iRecvdGood) / iPackets);

		if (dropped <= iUDPTolerance)
			aPassed=ETrue;
		aUdpRate= dropped;
		}
	return iSockErr;
	}


void CEchoSocket::SafetyCall()
/**
 * Allows the safety timer access, without becoming a friend class.
 */
	{
	if (iIsListener && (KProtocolInetTcp == iProtocol))
		{
		iAcceptedConnection.CancelAll();
		}
	
	iSocket.CancelAll();
	}
void CEchoSocket::SetComplete()
/**
 * Sets the socket to completion status.	
 */	
	{
	iSafety->Cancel();		
	iSocket.CancelAll();
	iSocket.Close();		
	if(iIsListener && (KProtocolInetTcp == iProtocol))
		{
		iAcceptedConnection.CancelAll();
		iAcceptedConnection.Close();		
		}

	


	iSockErr = iStatus.Int();	// Record the last error for use by caller.
	iStatus = KRequestPending;
	iState = EComplete;
	if(!IsActive() )
		SetActive();
	TRequestStatus* pS = &iStatus;
	User::RequestComplete(pS,KErrNone);
	
	}





CSafety::CSafety() : CActive(EPriorityNormal), iTime(CEchoSocket::SAFETY_TIMEOUT)
	{
	}


CSafety::~CSafety()
	{
	Cancel();
	iSafety.Close();
	}

CSafety* CSafety::NewL(CEchoSocket* aOwner)
	{
	CSafety* self = new(ELeave) CSafety;
	CleanupStack::PushL(self);
	self->ConstructL(aOwner);
	CleanupStack::Pop();
	return self;
	}


void CSafety::ConstructL(CEchoSocket* aOwner)
	{
	myOwner=aOwner;
	iSafety.CreateLocal();
	CEnhancedScheduler::Add(this);
	}

void CSafety::RunL()
	{
	myOwner->SafetyCall();
	}


void CSafety::DoCancel()
	{
	iSafety.Cancel();
	}

void CSafety::SetSafety()
	{
	__ASSERT_ALWAYS(!IsActive(), User::Panic(KTxtCEchoSocket, KErrAlreadyExists));
	iSafety.After(iStatus, iTime);
	SetActive();
	}
CSplitEchoSocket* CSplitEchoSocket::NewL()
	{
	CSplitEchoSocket* self = new (ELeave)CSplitEchoSocket();
	CleanupStack::PushL(self);
	self->ConstructL();
	CEnhancedScheduler::Add(self);
	CleanupStack::Pop(self);
	return self;
	}

CSplitEchoSocket::CSplitEchoSocket() : CEchoSocket()
	{
	}

CSplitEchoSocket::~CSplitEchoSocket()
	{
	}

TInt CSplitEchoSocket::Echo(const TInetAddr& aDest, const TInetAddr& aSrc,
							const TInt& aSize,	const TInt& aPackets, 
							const TUint& aProto, const TInt& aTol,
							TConnDetails* aConn, const TInt& aSockNum, 
							const TBool aIsListener, CTS_MultiHomingStep* aStep)
/**
 * Creates an Echo socket and runs it to completion or error
 * @param aSockServ The socket server to attach to
 * @param aDest The destination IP address
 * @param aSize The packet size to use in bytes
 * @param aPackets The number of packets to send
 * @param aProto The protocol to use
 * @param aTol The UDP data tolerance level
 * @return Error value for initialisation stage
 */
	{
	__ASSERT_ALWAYS(!IsActive(), User::Panic(KTxtCEchoSocket,KErrAlreadyExists));
	__ASSERT_ALWAYS(aSize <= CTS_MultiHomingStep::MAX_PACKET_SIZE, User::Panic(KTxtCEchoSocket,KErrOverflow));
	
	// Remember the config
	iDestAddr = aDest;
	iSrcAddr = aSrc;
	iPacketSize = aSize;
	iPacketsToSend = aPackets;
	iPackets = aPackets;
	iProtocol = aProto;
	iUDPTolerance = aTol;
	iOwnerStep = aStep;
	iTConnection = aConn;
	iSockNum = aSockNum;

	iIsListener = aIsListener;

	TInt err = KErrNone;

	iPtrWritn.SetLength(aSize);
	iPtrRecvd.SetLength(aSize);

	
	if(iTConnection == 0)
		return KErrGeneral;
	// Open the socket for implicit or explicit case
	if(iTConnection->iConnectionType != TConnDetails::implicitConn)
		{
		err = iSocket.Open( (iTConnection->iSocketServ), KAfInet, 
					iProtocol == KProtocolInetTcp ? KSockStream : KSockDatagram, iProtocol,
					(iTConnection->iConnection));
		}
	else
		{
		err = iSocket.Open( (iTConnection->iSocketServ), KAfInet, 
					iProtocol == KProtocolInetTcp ? KSockStream : KSockDatagram, iProtocol);
		}
	if (err != KErrNone)
		return err;			// No point going any further...

	iSocket.Name(iName);
	
	if(iIsListener)	// Don't bind if we are only sending
		{
		err = iSocket.Bind(iSrcAddr);
		iOwnerStep->StepLog(_L("Binding %S:%i"),&iName, iSockNum);
		if(err != KErrNone)
			return err;
		
		// Accept a single connection
		if (iProtocol == KProtocolInetTcp)
			{
			iSocket.Listen(1);
			iOwnerStep->StepLog(_L("Listening %S:%i"),&iName, iSockNum);
			}
		}

	iState = EInitialised;
	iStatus = KRequestPending;
	SetActive();

	// Increment the echosocket counter and client
	iOwnerStep->iOwnerSuite->iScheduler->IncCount();

	TName addrBuf;
	iDestAddr.Output(addrBuf);
	iOwnerStep->StepLog(KSockDetails,&iName,iSockNum,&addrBuf,iDestAddr.Port(),iPacketsToSend,iPacketSize);

	TRequestStatus* pS = &iStatus;
	User::RequestComplete(pS,KErrNone);

	return KErrNone;
	}


void CSplitEchoSocket::RunL()
	{
	switch (iState)
		{
		case EInitialised:
			if (KProtocolInetTcp == iProtocol)
				{
				if(iIsListener)
					{
					iAcceptedConnection.Open( (iTConnection->iSocketServ));
					iSocket.Accept(iAcceptedConnection, iStatus);
					iOwnerStep->StepLog(_L("Waiting for TCP accept on %S:%i"),&iName, iSockNum);
					iSafety->SetSafety();
					iState = EReady;
					SetActive();
					}
				else
					{
					ConnectTcp();
					iOwnerStep->StepLog(_L("Connecting TCP %S:%i"),&iName, iSockNum);
					}
				}
			else if (KProtocolInetUdp == iProtocol)
				{
				if(iIsListener)
					{
					RecvData();
					}
				else
					{
					BindUdp();
					iOwnerStep->StepLog(_L("Binding UDP %S:%i"), &iName, iSockNum);
					}
				}
			break;	
			
		case EReady:
			iSafety->Cancel();
			if (iStatus.Int() == KErrNone)
				{
				if(iIsListener)
					{
					if (KProtocolInetTcp == iProtocol)
						{
						RecvDataTcpListen();
						}
					else
						{
						RecvData();
						}
					}
				else
					{
					SendData();
					}
				}
			else
				{
				iOwnerStep->StepLog(KErrConnectFailed,&iName,iSockNum, iStatus.Int());
				SetComplete();
				}
			break;
			
		case ESentData:
			iSafety->Cancel();
			if (iStatus == KErrNone)
				{
				if(iIsListener)
					{
					if (KProtocolInetTcp == iProtocol)
						{
						RecvDataTcpListen();
						}
					else
						{
						RecvData();
						}
					}
				else
					{
					iRecvdGood++;	// Fake that we've received all we should (sending socket doesn't receive)
					// Blindly send, no receiving
					if (iPacketsToSend>0)
						{
						SendData();
						}
					else
						{
						SetComplete();
						}
					}
				}
			else
				{
				iOwnerStep->StepLog(KErrSendFailed, &iName, iSockNum, iStatus.Int());
				SetComplete();	// Couldn't send, so bit of a problem...
				}
			break;
		
		case ERecvdData:
			iSafety->Cancel();
			if(iIsListener && (iStatus.Int() == KErrEof) )
				{
				iRecvdGood++;
				iStatus = KErrNone;
				SetComplete();
				}
			else if(iStatus.Int() == KErrNone)
				{
				// Reset our consecutive count
				iConsecRecvTimeOuts = 0;
 				if (iIsListener)
					{
					iRecvdGood++;
					
					if (iPacketsToSend > 0)
						{
						if(iIsListener)
							{
							if (KProtocolInetUdp == iProtocol)
								{
								RecvData();
								}
							else
								{
								RecvDataTcpListen();		
								}
							}
						else 
							{
							SendData();	
							}
						}
					else
						{
						SetComplete();
						}
					}
				else
					{
					// Try reading again, usualy happens because of a timeout
					iOwnerStep->StepLog(KErrPacketsDiff, &iName, iSockNum);
					RecvData();
					}				
				}
			else if (KErrCancel == iStatus.Int()) // We cancelled our receive
				{
				iConsecRecvTimeOuts++;
				iOwnerStep->StepLog(_L("Receive timeout on %S:%i,%d"),&iName, iSockNum,
					iConsecRecvTimeOuts);
				
				iOwnerStep->StepLog(_L("%d received packets, %d still to send"),iRecvdGood, iPacketsToSend);
					
				if ( (iPacketsToSend - iUDPTolerance) > 0 && iConsecRecvTimeOuts < MAX_CONSEC_TIMEOUT)
					{
					if (iIsListener)
						{
						if (KProtocolInetUdp == iProtocol)
							{
							RecvData();
							}
						else
							{
							RecvDataTcpListen();		
							}
						}
					else
						{
						if (KProtocolInetTcp == iProtocol)
							SendData();
						}
					}
				else
					{
					SetComplete();
					}
				}
			else
				{
				TPtrC errorText = CLog::EpocErrorToText(iStatus.Int());
				iOwnerStep->StepLog(KErrRecvFailed, &iName, iSockNum, &errorText);
				SetComplete();
				}
			break;

			
		case EComplete:
			iOwnerStep->iOwnerSuite->iScheduler->DecCount();
			if(0 == iOwnerStep->iOwnerSuite->iScheduler->GetCount())
				CEnhancedScheduler::Stop();
			break;

		default:
			iOwnerStep->StepLog(KTxtWhatHappened);
			break;
		
		}
	}

void CSplitEchoSocket::RecvData()
/** 
 * Receives data on socket
 *
 */
	{
	// Receive data
	iSafety->SetSafety();
	TInt expected = iPtrRecvd.Length();
    if(iProtocol == KProtocolInetTcp)
        {
	    iSocket.Read(iPtrRecvd, iStatus);
        }
    else if(iProtocol == KProtocolInetUdp)
        {
        if (iIsListener)
        	{
            //iSocket.RecvFrom(iPtrRecvd, iSrcAddr, 0, iStatus);
            iSocket.Recv(iPtrRecvd, 0, iStatus);
            iOwnerStep->StepLog(_L("Receiving UDP Packet..."));
        	}
        else
        	{
        	iSocket.Recv(iPtrRecvd, 0, iStatus);
        	}
        }
    if (iIsListener)
    	iPacketsToSend--;	// Decrement this
	
    iState = ERecvdData;
	SetActive();
	}


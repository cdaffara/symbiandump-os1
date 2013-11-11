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

//#include <e32des8.h> // For Rbuf
#include "Listener.h"
#include "IcmpMessage.h"
#include "IpInfoS.h"
#include "ReqstParser.h"
#include "RpsSFlogger.h"
//------------------------------------------------------------------------------//
// --- Class CListenerOptions --------------------------------------------------//
//------------------------------------------------------------------------------//


CListenerOptions::CListenerOptions(TUint aAddrFamily, TUint aProtocol, TUint aSockType)
: iProtocol(aProtocol), iSockType(aSockType)
	{
	iBindAddr.SetFamily(aAddrFamily);
	}


TInetAddr CListenerOptions::Addr()
// 
// Return address where to bind
// 
	{
	return iBindAddr;
	}

void CListenerOptions::SetAddress(const TInetAddr& aAddr)
//
// Method to set address we want to bind to
// 
	{
	iBindAddr = aAddr;
	}

TUint CListenerOptions::Family()
// 
// Return Address family
// 
	{
	return iBindAddr.Family();
	}

void CListenerOptions::SetFamily(TUint aFamily)
// 
// Set address family
// 
	{
	iBindAddr.SetFamily(aFamily);
	}

TInt CListenerOptions::Port()
// 
// Get Port where to bind
// 
	{
	return iBindAddr.Port();
	}

void CListenerOptions::SetPort(TInt aPort)
//
// Set port where to bind
// 
	{
	iBindAddr.SetPort(aPort);
	}

TName CListenerOptions::ProgramName()
//
// Return program name to launch
// 
	{
	return iProgramName;
	}

void CListenerOptions::SetProgramName(const TName& aName)
//
// Set program to launch name
// 
	{
	iProgramName = aName;
	}

TName CListenerOptions::ProgramOptions()
// 
// Return command line arguments options
// 
	{
	return iProgramOptions;
	}

void CListenerOptions::SetProgramOptions(const TName& aOptions)
// 
// Set command line arguments options
// 
	{
	iProgramOptions = aOptions;
	}

TUint CListenerOptions::Protocol()
// 
// Return socket protocol
// 
	{
	return iProtocol;
	}

void CListenerOptions::SetProtocol(TUint aProtocol)
//
// Set socket protocol
//
	{
	iProtocol = aProtocol;
	}

TUint CListenerOptions::SockType()
//
// Return socket type
//
	{
	return iSockType;
	}

void CListenerOptions::SetSockType(TUint aSockType)
// 
// Set socket type
// 
	{
	iSockType = aSockType;
	}

TInetAddr CListenerOptions::KnownAdapterAddr()
	{
	return iKnownAdapterAddr;
	}
	
void CListenerOptions::SetKnownAdapterAddr(const TDesC &aAddrBuf)
	{
	iKnownAdapterAddr.Input(aAddrBuf);
	}
		
//------------------------------------------------------------------------------//
// --- Class CListener ---------------------------------------------------------//
//------------------------------------------------------------------------------//


CListener::CListener(CRequestHandler& aRqstHndlr) 
 : CActive(EPriorityStandard), iRqstHndlr(aRqstHndlr)
	{
	iListenerState = EInitial;
	
	CActiveScheduler::Add(this);
	}
	
CListener::~CListener()
	{
	Cancel();  // for CActive
	iListenSocket.CancelAll();
	iListenSocket.Close();
	iSockServ.Close();
	delete iOptions;
	
	iBuffer.Close();
	iBufferAll.Close();
	}

CListener* CListener::NewL(CRequestHandler& aRqstHndlr)
	{
	CListener* self = new(ELeave) CListener(aRqstHndlr);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CListener::ConstructL()
	{
	// Connect to socket server
	User::LeaveIfError(iSockServ.Connect());
	LOGLINE1(_L("Connected to Socket Server"));
		
	// Create Listener options
	iOptions = new(ELeave) CListenerOptions( KAfInet, KProtocolInetTcp, KSockStream);
	iOptions->SetPort(KLocalPort);
	iOptions->SetKnownAdapterAddr(KnownAdaptAddr);
	
	// Register the Listener with ARP
	ArpRegisterL();
	
	TInetAddr bindAddr;
	
	// 1: Open listening socket
	iListenSocket.Close();
	User::LeaveIfError(iListenSocket.Open(iSockServ, iOptions->Family(), iOptions->SockType(), iOptions->Protocol()));
	LOGLINE1(_L("Socket opened"));
	
	// 2: Bind listening socket
	bindAddr = iOptions->Addr();
	User::LeaveIfError(iListenSocket.Bind(bindAddr));
	LOGLINE1(_L("Socket bind ok"));
		
	// 3: Listen
	User::LeaveIfError(iListenSocket.Listen(KQueueListenSize));
	}
	

void CListener::ArpRegisterL()
	{
	/*
	Open a connectionless socket to a known tcp/ip adapter
	and send it an ICMP msg (ping request).
	This has the effect of registring the Listener to the ARP, so that it's address can be resolvd.
	We are not interested in the echo (response to ping).
	*/	
	RSocket tempSocket;
	User::LeaveIfError(tempSocket.Open(iSockServ, KAfInet,KSockDatagram, KProtocolInetIcmp));
	
	TRequestStatus stat;

	// Create the data to send (Ping msg)
	HPingHeader* sendData;
	sendData = HPingHeader::NewL(KDefaultPingSize);
	TInt Id;
	Id=User::TickCount()&KMaxTUint16;
	TUint nrTransmitted(0);
	sendData->FormatSend(Id, nrTransmitted++);

	TInetAddr knownAddr = iOptions->KnownAdapterAddr();
	tempSocket.SendTo(*(sendData->Grab()), knownAddr, 0, stat);
	User::WaitForRequest(stat);
	if(stat == KErrNone)
		{
		LOGLINE2(_L("ArpRegisterL() status: %d"), stat.Int());
		}
	else
		{
		LOGLINE2(_L("WARNING ArpRegisterL() failure!! status: %d"), stat.Int());
		}
	
	delete sendData;
	
	tempSocket.Close();
	}


CListenerOptions* CListener::Options()
	{
	return iOptions;
	}


void CListener::AcceptL()
// Method that grabs the next connect received from a remote party
	{
	__ASSERT_ALWAYS(!IsActive(), User::Panic(KPanicMsgFormat, KListenerNotActive));
	
	// Create a new blank socket for accept
	User::LeaveIfError(iBlankSocket.Open(iSockServ));

	// Accept
	iListenSocket.Accept(iBlankSocket, iStatus);
	iListenerState = EWaitingForConnection;
	SetActive();

	}
	

void CListener::IssueRead()
	{
	iListenerState = EWaitingForData;
	if (!IsActive())
		{
		// Reads just the msg header & length
		iBufferPreRead.Delete(0, iBufferPreRead.Length());
		iBlankSocket.Recv(iBufferPreRead, 0, iStatus);
		SetActive();
		}
	}
	

void CListener::RunL()
	{
	if(iListenerState == EWaitingForConnection)
		{
		// We are here when incoming connection detected and Accepted on Listener.
		if(iStatus != KErrNone)
			{
			// Something went wrong when accepting a remote connection
			// Not much we can do at this point.
			LOGLINE2(_L("Accept returns error! %d"), iStatus.Int());
			LOGLINE1(_L("Leaving....!"))
			DoCancel();
			User::Leave(KErrCommsLineFail);
			}
			
		// iBlankSocket is now ready to communicate with the incoming connection.
		LOGLINE1(_L("Remote connection detected!"));
			
		// Wait for data from remote port
		IssueRead();
		}
	else if(iListenerState == EWaitingForData)
		{
		
		// Check status of Recv operation
		if(iStatus != KErrNone)
			{
			if(iStatus == KErrEof)
				{
				LOGLINE1(_L("Remote party may have closed the connection"));
				}
			// Terminate the active socket
			LOGLINE1(_L("Closing active socket"));
			iBlankSocket.Close();
			
			// Set listening socket to accept new connection
			AcceptL();
			return;
			}

		// Print received data!!!!
		TBuf16<KReqstSocketPreReadSize> Buffer;
		Buffer.Copy(iBufferPreRead);

		// Get msg length from pre-read
		TUint msgLen = CReqstParser::ExtractMsgLenL(iBufferPreRead);
		__ASSERT_ALWAYS(msgLen > KReqstSocketPreReadSize, User::Panic(KPanicMsgFormat, KIncorrectPreReadSize)); 
		
		
		// Now read the rest of msg by creating an Rbuf just large enough according to the msg length
		iBuffer.Close();

		// Create a buffer to recv exact amount of remaining reqst msg
		iBuffer.CreateL(msgLen - KReqstSocketPreReadSize);
		
		// Read remaining msg data.
		// Recv operation should complete immediately if msg has been formatted correctly
		iBlankSocket.Recv(iBuffer, 0, iStatus);
		User::WaitForRequest(iStatus);
		// Check status of Recv operation
		if(iStatus != KErrNone)
			{
			if(iStatus == KErrEof)
				{
				LOGLINE1(_L("Remote party may have closed the connection during 2nd Recv"));
				}
			// Terminate the active socket
			LOGLINE1(_L("Error during 2nd Recv!!"));
			LOGLINE1(_L("Closing active socket"));
			iBlankSocket.Close();
			
			// Set listening socket to accept new connection
			AcceptL();
			return;
			}
		
		// Tell reqst hndlr we have a msg. That thread will parse msg and build the reply which we
		// want to send back to master. The call to ETEL to process the actual reqst
		// will have to be done asynchronously in HandleRequestL().
		iBufferAll.Close();
		iBufferAll.CreateL(msgLen);		// Give HandleRequestL the entire msg!
		iBufferAll += iBufferPreRead;
		iBufferAll += iBuffer;
		const TDesC8& response = iRqstHndlr.HandleRequestL(iBufferAll);
		
		if(response == KNullDesC8)
			{
			LOGLINE1(_L("Response msg is NULL"));
			}
		else
			{
			// Send back the reply msg
			TRequestStatus status;
			//convert to 16 bit
			TBuf16<4*KMaxElementSize> responsebuf16;
			responsebuf16.Copy(response);
			LOGLINE2(_L("Sending data back to sender...%S"), &responsebuf16);
			iBlankSocket.Send(response, 0, status);
			User::WaitForRequest(status);
			// There's a slim chance the remote party has closed the connection since the time a msg 
			// was received
			if(status != KErrNone)
				{
				if(status == KErrEof)
					{
					LOGLINE1(_L("Remote party may have closed the connection before we could send the response"));
					}
				// Terminate the active socket
				LOGLINE1(_L("Closing active socket"));
				iBlankSocket.Close();
				
				// Set listening socket to accept new connection
				AcceptL();
				return;
				}
			
			LOGLINE2(_L("Data sent. Status = %d"), status.Int());
			}
		
		// Wait for data from remote port
		IssueRead();
		}
	}


void CListener::DoCancel()
	{
	// Cancel accept
	iListenSocket.CancelAccept();

	// Terminates
	iBlankSocket.Close();
	}



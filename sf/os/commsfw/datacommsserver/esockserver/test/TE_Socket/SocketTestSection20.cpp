// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This contains ESock Test cases from section 20
// 
//



#include <e32base.h>
#include "SocketTestSection20.h"
#include "ES_DUMMY.H"
#include <es_sock.h>
#include <es_mbuf.h>
#include <es_prot.h>
#include <e32test.h>
#include <comms-infras/nifif.h>
#include <es_mbuf.h>
#include <ss_pman.h>
#include <ss_glob.h>
#include <agenterrors.h>

enum options
	{
	KOOM = 0,
	KEndOOM = 1,
	};
	
enum level
	{
	KHigh = 0,
	KLow = 1,
	};

const TInt KDesBufSize = 50;


// Test step 20.1
const TDesC& CSocketTest20_1::GetTestName()
	{
	_LIT(ret,"Test20.1");
	return ret;
	}

enum TVerdict CSocketTest20_1::InternalDoTestStepL( void )
	{
	// Create socket, connect and then shutdown using 
	TVerdict verdict = EFail;
	Logger().WriteFormat(_L("Test Purpose: Connect Socket, then Shutdown.... KSIConnectData Protocol"));

	// Connect to esock
	Logger().WriteFormat(_L("Attempting to connect to socket server"));
    RSocketServ ss;
	TInt ret = ss.Connect();
	Logger().WriteFormat(_L("Connect returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);
	CleanupClosePushL(ss); 

	// Create and open socket on TCP protocol
	RSocket socket1;
	Logger().WriteFormat(_L("Opening Socket 1"));
	ret = socket1.Open(ss, KAfInet, KSockStream, KProtocolInetTcp);
	TESTL(KErrNone == ret);
	Logger().WriteFormat(_L("Socket 1 Open returned %S"), &EpocErrorToText(ret));	
	CleanupClosePushL(socket1); 

	// Create Ports for Socket and socket to connect to
 	const TInt KSocket2Port = 3881;
 	_LIT(KLoopback, "127.0.0.1");
 	_LIT(KLocal, "0.0.0.0");
 	
 	// Set the local address of Socket1
 	TInetAddr socket1Addr;
 	socket1Addr.SetPort(KSocket2Port);
	socket1Addr.Input(KLocal);
	ret = socket1.Bind(socket1Addr);
 	
 	// Set remote address for socket 
 	TInetAddr socket2Addr;
 	socket2Addr.SetPort(KSocket2Port);
	socket2Addr.Input(KLoopback);
 	
 	// Connect socket1 to remote address
 	TRequestStatus connectSocket1Status;
 	Logger().WriteFormat(_L("Connecting Socket1 "));
 	socket1.Connect(socket2Addr, connectSocket1Status);
 	User::WaitForRequest(connectSocket1Status);
 	Logger().WriteFormat(_L("Socket 1 connect returned %S"), &EpocErrorToText(connectSocket1Status.Int()));
	TESTL(connectSocket1Status == KErrNone);
	 
	// Shutdown the Socket
	Logger().WriteFormat(_L("Shutting down Connection on Socket1"));
	TRequestStatus shutdownStatus;
	_LIT8(desOut, "Some test stuff to send to protocol");
	TBuf8<KDesBufSize> desIn;
	socket1.Shutdown(RSocket::EImmediate,desOut,desIn, shutdownStatus);
	User::WaitForRequest(shutdownStatus);
	Logger().WriteFormat(_L("Socket 1 shutdown returned %S"), &EpocErrorToText(shutdownStatus.Int()));
	if(shutdownStatus == KErrNone)
	 	{
	 	verdict = EPass;
	 	}
	 	
	// Clean up the Server side objects
 	CleanupStack::PopAndDestroy(&socket1);
 	CleanupStack::PopAndDestroy(&ss);
 	SetTestStepResult(verdict);   
	return verdict;
	}
	

// Test step 20.2
const TDesC& CSocketTest20_2::GetTestName()
	{
	_LIT(ret,"Test20.2");
	return ret;
	}

enum TVerdict CSocketTest20_2::InternalDoTestStepL( void )
	{
	// Create socket, connect and then shutdown using 
	TVerdict verdict = EFail;
	Logger().WriteFormat(_L("Test Purpose: Connect Socket, then Shutdown... KSIConnectionless Protocol"));

	// Connect to esock
	Logger().WriteFormat(_L("Attempting to connect to socket server"));
    RSocketServ ss;
	TInt ret = ss.Connect();
	Logger().WriteFormat(_L("Connect returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);
	CleanupClosePushL(ss); 


	// Create and open socket on TCP protocol
	RSocket socket1;
	Logger().WriteFormat(_L("Opening Socket 1"));
	ret = socket1.Open(ss, KAfInet, KSockDatagram, KProtocolInetUdp);
	Logger().WriteFormat(_L("Socket 1 Open returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);	
	CleanupClosePushL(socket1); 

	// Create Ports for Socket and socket to connect to
 	const TInt KSocket2Port = 3881;
 	_LIT(KLoopback, "127.0.0.1");
 	_LIT(KLocal, "0.0.0.0");
 	
 	// Set the local address of Socket1
 	TInetAddr socket1Addr;
 	socket1Addr.SetPort(KSocket2Port);
	socket1Addr.Input(KLocal);
	ret = socket1.Bind(socket1Addr);
 	
 	// Set remote address for socket 
 	TInetAddr socket2Addr;
 	socket2Addr.SetPort(KSocket2Port);
	socket2Addr.Input(KLoopback);
 	
 	// Connect socket1 to remote address
 	TRequestStatus connectSocket1Status;
 	Logger().WriteFormat(_L("Connecting Socket1 "));
 	socket1.Connect(socket2Addr, connectSocket1Status);
 	User::WaitForRequest(connectSocket1Status);
 	Logger().WriteFormat(_L("Socket 1 connect returned %S"), &EpocErrorToText(connectSocket1Status.Int()));
	TESTL(connectSocket1Status == KErrNone);
	 
	// Shutdown the Socket
	Logger().WriteFormat(_L("Shutting down Connection on Socket1"));
	TRequestStatus shutdownStatus;
	_LIT8(desOut, "Some test data to send to protocol");
	TBuf8<KDesBufSize> desIn;
	socket1.Shutdown(RSocket::EImmediate,desOut,desIn, shutdownStatus);
	User::WaitForRequest(shutdownStatus);
	Logger().WriteFormat(_L("Socket 1 shutdown returned %S"), &EpocErrorToText(shutdownStatus.Int()));
	if(shutdownStatus == KErrNone)
	 	{
	 	verdict = EPass;
	 	}
	
	 	
	// Clean up the Server side objects
 	CleanupStack::PopAndDestroy(&socket1);
	CleanupStack::PopAndDestroy(&ss);
 	SetTestStepResult(verdict);   
	return verdict;
	}


// Test step 20.3
const TDesC& CSocketTest20_3::GetTestName()
	{
	_LIT(ret,"Test20.3");
	return ret;
	}

enum TVerdict CSocketTest20_3::InternalDoTestStepL( void )
	{
	// Create socket, shutdown socket then connect 
	TVerdict verdict = EFail;
	Logger().WriteFormat(_L("Test Purpose: Shutdown Socket, then attempt to connect"));

	// Connect to esock
	Logger().WriteFormat(_L("Attempting to connect to socket server"));
    RSocketServ ss;
	TInt ret = ss.Connect();
	Logger().WriteFormat(_L("Connect returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);
	CleanupClosePushL(ss); 


	// Create and open socket on TCP protocol
	RSocket socket1;
	Logger().WriteFormat(_L("Opening Socket 1"));
	ret = socket1.Open(ss, KAfInet, KSockStream, KProtocolInetTcp);
	Logger().WriteFormat(_L("Socket 1 Open returned %S"), &EpocErrorToText(ret));	
	TESTL(KErrNone == ret);
	CleanupClosePushL(socket1); 

	// Create Ports for Socket and socket to connect to
 	const TInt KSocket2Port = 3881;
 	_LIT(KLoopback, "127.0.0.1");
 	_LIT(KLocal, "0.0.0.0");
 	
 	// Set the local address of Socket1
 	TInetAddr socket1Addr;
 	socket1Addr.SetPort(KSocket2Port);
	socket1Addr.Input(KLocal);
	ret = socket1.Bind(socket1Addr);
 	
 	// Set remote address for socket 
 	TInetAddr socket2Addr;
 	socket2Addr.SetPort(KSocket2Port);
	socket2Addr.Input(KLoopback);
 	
  	 	
 	// Shutdown the Socket
	Logger().WriteFormat(_L("Shutting down Connection on Socket1"));
	TRequestStatus shutdownStatus;
	_LIT8(desOut, "Some test stuff to send to protocol");
	TBuf8<KDesBufSize> desIn;
	socket1.Shutdown(RSocket::EImmediate,desOut,desIn, shutdownStatus);
	User::WaitForRequest(shutdownStatus);
	Logger().WriteFormat(_L("Socket 1 shutdown returned %S"), &EpocErrorToText(shutdownStatus.Int()));
 	
 	// Connect socket1 to remote address
 	TRequestStatus connectSocket1Status;
 	Logger().WriteFormat(_L("Connecting Socket1 "));
 	socket1.Connect(socket2Addr, connectSocket1Status);
 	User::WaitForRequest(connectSocket1Status);
 	Logger().WriteFormat(_L("Socket 1 connect returned %S"), &EpocErrorToText(connectSocket1Status.Int()));
	if(connectSocket1Status == KErrBadHandle)
	 	{
	 	verdict = EPass;
	 	}
	 	
	// Clean up the Server side objects
 	CleanupStack::PopAndDestroy(&socket1);
 	CleanupStack::PopAndDestroy(&ss);
 	SetTestStepResult(verdict);   
	return verdict;
	}
	
	
	
// Test step 20.4
const TDesC& CSocketTest20_4::GetTestName()
	{
	_LIT(ret,"Test20.4");
	return ret;
	}

enum TVerdict CSocketTest20_4::InternalDoTestStepL( void )
	{
	// Create socket, connect and then shutdown using 
	TVerdict verdict = EFail;
	Logger().WriteFormat(_L("Test Purpose: Connect Socket, then Get DisconnectData.... KSIConnectionless Protocol"));

	// Connect to esock
	Logger().WriteFormat(_L("Attempting to connect to socket server"));
    RSocketServ ss;
	TInt ret = ss.Connect();
	Logger().WriteFormat(_L("Connect returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);
	CleanupClosePushL(ss); 

	// Create and open socket on UDP protocol
	RSocket socket1;
	Logger().WriteFormat(_L("Opening Socket 1"));
	ret = socket1.Open(ss, KAfInet, KSockDatagram, KProtocolInetUdp);
	Logger().WriteFormat(_L("Socket 1 Open returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);
	CleanupClosePushL(socket1); 

	// Create Ports for Socket and socket to connect to
 	const TInt KSocket2Port = 3881;
 	_LIT(KLoopback, "127.0.0.1");
 	_LIT(KLocal, "0.0.0.0");
 	
 	// Set the local address of Socket1
 	TInetAddr socket1Addr;
 	socket1Addr.SetPort(KSocket2Port);
	socket1Addr.Input(KLocal);
	ret = socket1.Bind(socket1Addr);
 	
 	// Set remote address for socket 
 	TInetAddr socket2Addr;
 	socket2Addr.SetPort(KSocket2Port);
	socket2Addr.Input(KLoopback);
 	
 	// Connect socket1 to remote address
 	TRequestStatus connectSocket1Status;
 	Logger().WriteFormat(_L("Connecting Socket1 "));
 	socket1.Connect(socket2Addr, connectSocket1Status);
 	User::WaitForRequest(connectSocket1Status);
 	Logger().WriteFormat(_L("Socket 1 connect returned %S"), &EpocErrorToText(connectSocket1Status.Int()));
	TESTL(KErrNone == connectSocket1Status.Int());
	 
	TBuf8<KDesBufSize> desIn;
	ret = socket1.GetDisconnectData(desIn);
	if(ret == KErrNotSupported)
		{
		verdict = EPass;
		}
	 	
	// Clean up the Server side objects
 	CleanupStack::PopAndDestroy(&socket1);
 	CleanupStack::PopAndDestroy(&ss);
 	SetTestStepResult(verdict);   
	return verdict;
	}
	

// Test step 20.5
const TDesC& CSocketTest20_5::GetTestName()
	{
	_LIT(ret,"Test20.5");
	return ret;
	}

enum TVerdict CSocketTest20_5::InternalDoTestStepL( void )
	{
	// Create socket, connect, disconnect and then GetDisconnectData 
	TVerdict verdict = EFail;
	Logger().WriteFormat(_L("Test Purpose: Connect Socket, then Get DisconnectData.... KSIConnectData Protocol"));

	// Connect to esock
	Logger().WriteFormat(_L("Attempting to connect to socket server"));
    RSocketServ ss;
	TInt ret = ss.Connect();
	Logger().WriteFormat(_L("Connect returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);
	CleanupClosePushL(ss); 
	
	// Create and open socket on Dummy protocol
	RSocket socket1;
	Logger().WriteFormat(_L("Opening Socket 1"));
	ret = socket1.Open(ss, KDummyAddrFamily, KSockSeqPacket, KDummyThree);
	Logger().WriteFormat(_L("Socket 1 Open returned %S"), &EpocErrorToText(ret));	
	TESTL(KErrNone == ret);
	CleanupClosePushL(socket1); 

	// Create Ports for Socket and socket to connect to
 	const TInt KSocket2Port = 3881;
 	_LIT(KLoopback, "127.0.0.1");
 	_LIT(KLocal, "0.0.0.0");
 	
 	// Set the local address of Socket1
 	TInetAddr socket1Addr;
 	socket1Addr.SetPort(KSocket2Port);
	socket1Addr.Input(KLocal);
	ret = socket1.Bind(socket1Addr);
 	
 	// Set remote address for socket 
 	TInetAddr socket2Addr;
 	socket2Addr.SetPort(KSocket2Port);
	socket2Addr.Input(KLoopback);
 	
 	// Connect socket1 to remote address
 	TRequestStatus connectSocket1Status;
 	Logger().WriteFormat(_L("Connecting Socket1 "));
 	socket1.Connect(socket2Addr, connectSocket1Status);
 	User::WaitForRequest(connectSocket1Status);
 	Logger().WriteFormat(_L("Socket 1 connect returned %S"), &EpocErrorToText(connectSocket1Status.Int()));
	if(connectSocket1Status != KErrNone)
	 	{
	 	return EFail;
	 	}
	 
	// Get disconnect data
	TBuf8<KDesBufSize> desIn;
	ret = socket1.GetDisconnectData(desIn);
	if(ret == KErrNotFound)
		{
		verdict = EPass;
		}
	
	// Clean up the Server side objects
 	CleanupStack::PopAndDestroy(&socket1);	
 	CleanupStack::PopAndDestroy(&ss);
 	SetTestStepResult(verdict);   
	return verdict;
	}
	
	










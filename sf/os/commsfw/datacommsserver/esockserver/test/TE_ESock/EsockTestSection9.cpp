// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This contains ESock Test cases from section 9
// 
//

// EPOC includes
#include <e32base.h>
#include <in_sock.h>

// Test system includes
#include "EsockTestSection9.h"


// Test step 9.1
const TDesC& CEsockTest9_1::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test9.1");
	
	return ret;
	}

CEsockTest9_1::~CEsockTest9_1()
	{
	}

enum TVerdict CEsockTest9_1::easyTestStepPreambleL()
	{
	if (KErrNone != OpenMinSockets(_L("Test_9.1"),KAfInet, KSockStream, KProtocolInetTcp))
		{
		return EFail;
		}
	
	return EPass;
	}

enum TVerdict CEsockTest9_1::easyTestStepL()
	{
	Logger().WriteFormat(_L("TE_ESock: Test 9.1"));
	
	// List Route(s)
	ListRoutes();
	
	return EPass;
	}


// Test step 9.2
const TDesC& CEsockTest9_2::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test9.2");
	
	return ret;
	}

CEsockTest9_2::~CEsockTest9_2()
	{
	}

enum TVerdict CEsockTest9_2::easyTestStepPreambleL()
	{
	if (KErrNone != OpenMinSockets(_L("Test_9.2"),KAfInet, KSockStream, KProtocolInetTcp))
		{
		return EFail;
		}
	
	return EPass;
	}

enum TVerdict CEsockTest9_2::easyTestStepL()
	{
	Logger().WriteFormat(_L("TE_ESock: test 9.2"));
	TESTL(EPass == TestStepResult());
	
	// ipv4
	// List Route(s)
	ListRoutes();

	TInt ret = SetRoute(KSoInetAddRoute, IPADDR(10,158,7,52),IPADDR(255,0,0,0),IPADDR(10,158,7,73), IPADDR(127,0,0,1), 0);
	TESTEL(KErrNone == ret, ret);
	
	// List Route(s)
	ListRoutes();
	
	// Delete Route
	ret = SetRoute(KSoInetDeleteRoute, IPADDR(10,158,7,52),IPADDR(255,0,0,0),IPADDR(10,158,7,73), IPADDR(127,0,0,1), 0);
	TESTEL(KErrNone == ret, ret);
	
	// List Route(s)
	ListRoutes();
	
	return EPass;
	}

// Test step 9.3
const TDesC& CEsockTest9_3::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test9.3");
	
	return ret;
	}

CEsockTest9_3::~CEsockTest9_3()
	{
	}

enum TVerdict CEsockTest9_3::easyTestStepPreambleL()
	{
	if (KErrNone != OpenMinSockets(_L("Test_9.3"),KAfInet, KSockStream, KProtocolInetTcp))
		{
		return EFail;
		}
	
	return EPass;
	}

enum TVerdict CEsockTest9_3::easyTestStepL()
	{
	Logger().WriteFormat(_L("TE_ESock: test 9.3"));
	TESTL(EPass == TestStepResult());
	
	// ipv6
	// List Route(s)
	ListRoutes();
	
	TPckgBuf<TSoInetRouteInfo> opt;
	opt().iType = ERtUser;
	opt().iState = ERtReady;
	opt().iIfAddr.SetFamily(KAFUnspec);
	opt().iMetric = 1;
	(TInetAddr::Cast(opt().iIfAddr)).Input(_L("::1"));
	(TInetAddr::Cast(opt().iGateway)).Input(_L("fe80::dead:beef"));
	(TInetAddr::Cast(opt().iDstAddr)).Input(_L("2001:618:400:61::dead:fee"));
	(TInetAddr::Cast(opt().iNetMask)).Input(_L("ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff"));
	// Add Route
	TInt ret = iEsockSuite->GetSocketHandle(1).SetOpt(KSoInetAddRoute, KSolInetRtCtrl, opt);
	TESTEL(KErrNone == ret, ret);
	
	// List Route(s)
	ListRoutes();
	
	// Delete Route
	ret = iEsockSuite->GetSocketHandle(1).SetOpt(KSoInetDeleteRoute, KSolInetRtCtrl, opt);
	TESTEL(KErrNone == ret, ret);
	
	// List Route(s)
	ListRoutes();
	
	return EPass;
	}

// Test step 9.4
const TDesC& CEsockTest9_4::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test9.4");
	
	return ret;
	}

CEsockTest9_4::~CEsockTest9_4()
	{
	}

enum TVerdict CEsockTest9_4::easyTestStepPreambleL()
	{
	if (KErrNone != OpenMinSockets(_L("Test_9.4"),KAfInet, KSockStream, KProtocolInetTcp))
		{
		return EFail;
		}
	
	return EPass;
	}

enum TVerdict CEsockTest9_4::easyTestStepL()
	{
	Logger().WriteFormat(_L("TE_ESock: test 9.4"));
	TESTL(EPass == TestStepResult());
	
	// ipv4
	// List Route(s)
	ListRoutes();
	
	// Add Route
	TInt ret = SetRoute(KSoInetAddRoute, IPADDR(10,158,7,52),IPADDR(255,0,0,0),IPADDR(10,158,7,73), IPADDR(127,0,0,1), 0);
	TESTEL(KErrNone == ret, ret);
	
	// List Route(s)
	ListRoutes();
	
	// Change Route
	ret = SetRoute(KSoInetChangeRoute, IPADDR(10,158,7,52),IPADDR(255,0,0,0),IPADDR(10,158,7,70), IPADDR(127,0,0,1), 0);
	TESTEL(KErrNotFound == ret, ret);
	
	// List Route(s)
	ListRoutes();
	
	return EPass;
	}

// Test step 9.5
const TDesC& CEsockTest9_5::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test9.5");
	
	return ret;
	}

CEsockTest9_5::~CEsockTest9_5()
	{
	}

enum TVerdict CEsockTest9_5::easyTestStepPreambleL()
	{
	if (KErrNone != OpenMinSockets(_L("Test_9.5"),KAfInet, KSockStream, KProtocolInetTcp))
		{
		return EFail;
		}
	
	return EPass;
	}

enum TVerdict CEsockTest9_5::easyTestStepL()
	{
	Logger().WriteFormat(_L("TE_ESock: test 9.5"));
	TESTL(EPass == TestStepResult());
	
	// ipv6
	// List Route(s)
	ListRoutes();
	
	TPckgBuf<TSoInetRouteInfo> opt;
	opt().iType = ERtUser;
	opt().iState = ERtReady;
	opt().iIfAddr.SetFamily(KAFUnspec);
	opt().iMetric = 1;
	(TInetAddr::Cast(opt().iIfAddr)).Input(_L("::1"));
	(TInetAddr::Cast(opt().iGateway)).Input(_L("fe80::dead:beef"));
	(TInetAddr::Cast(opt().iDstAddr)).Input(_L("2001:618:400:61::dead:fee"));
	(TInetAddr::Cast(opt().iNetMask)).Input(_L("ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff"));
	// Add Route
	TInt ret = iEsockSuite->GetSocketHandle(1).SetOpt(KSoInetAddRoute, KSolInetRtCtrl, opt);
	TESTEL(KErrNone == ret, ret);
	
	// List Route(s)
	ListRoutes();
	
	(TInetAddr::Cast(opt().iIfAddr)).Input(_L("::1"));
	(TInetAddr::Cast(opt().iGateway)).Input(_L("ffff::new:beef"));
	// Change Route
	ret = iEsockSuite->GetSocketHandle(1).SetOpt(KSoInetChangeRoute, KSolInetRtCtrl, opt);
	TESTEL(KErrNotFound == ret, ret);
	
	// List Route(s)
	ListRoutes();
	
	return EPass;
	}


const TInt KTimerPeriod = 10 * 1000000; // Guard timer period, in microseconds.

CEsockTest9_6::CEsockTest9_6()
	{
	iGuardTimer.CreateLocal();	// Set up the timer
	}

// Test step 9.5
const TDesC& CEsockTest9_6::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test9.6");
	
	return ret;
	}

CEsockTest9_6::~CEsockTest9_6()
	{
	iGuardTimer.Close();		// Cleanup the timer
	}

/**
   Create a UDP socket. Perform a zero-length SendTo, then attempt to receive the zero-length datagram.

   @return EPass on success, EFail on failure
*/
TVerdict CEsockTest9_6::easyTestStepL()
	{
	Logger().WriteFormat(_L("TE_ESock: test 9.6"));
	
	TInt cleanupCount = 0;		// Number of items we've put on Cleanup Stack - remember to increment this after every Push operation.
	
	RSocket sendSocket;

	CleanupClosePushL( sendSocket );
	++cleanupCount;

	TSockAddr sendSocketAddress;
	Logger().WriteFormat(_L("Creating Send Socket"));
	if( !CreateUdpSocket( sendSocket, sendSocketAddress ) )
		{
		CleanupStack::PopAndDestroy( cleanupCount );
		return EFail;
		}


	RSocket recvSocket;

	CleanupClosePushL( recvSocket );
	++cleanupCount;

	TSockAddr recvSocketAddress;
	Logger().WriteFormat(_L("Creating Receive Socket"));
	if( !CreateUdpSocket( recvSocket, recvSocketAddress ) )
		{
		CleanupStack::PopAndDestroy( cleanupCount );
		return EFail;
		}



	HBufC8 *udpWriteBuffer = HBufC8::NewLC( 0 ); // Empty buffer for zero-length write
	++cleanupCount;

	HBufC8 *udpReadBuffer  = HBufC8::NewMaxLC( 32 ); // Non-Empty buffer for reads - 32 is an unimportant magic number
	++cleanupCount;

	TPtrC8  ptrWritebuf( udpWriteBuffer->Des() );
	TPtr8   ptrReadbuf( udpReadBuffer->Des() );
	TSockAddr toAddress( recvSocketAddress );

	TBool isOk = PerformSend( sendSocket, ptrWritebuf, toAddress );
	if( isOk )
		{
		TSockAddr fromAddress;
		isOk = PerformRecv( recvSocket, ptrReadbuf, fromAddress );
		if( isOk )
			{
			Logger().WriteFormat(_L("Receieved %d bytes"), udpReadBuffer->Length());
			if( udpReadBuffer->Length() != 0 )
				{
				isOk = EFalse;
				}
			}
		}
	
	CleanupStack::PopAndDestroy( cleanupCount );

	return isOk ? EPass : EFail;
	}

// Work around for timer oddness....
/**
   Wait until either a request completes, or the specified period has elapsed

   @param aRequestStatus  Request to wait on
   @param aPeriod        Timeout in microseconds
*/
void CEsockTest9_6::WaitFor( TRequestStatus& aRequestStatus, TInt aPeriod )
	{
	// Set up the timer
	TRequestStatus timerStatus;
	iGuardTimer.After( timerStatus, aPeriod );
	// Wait for either the supplied event, or the timer, to complete
	User::WaitForRequest( aRequestStatus, timerStatus );
	if( timerStatus == KRequestPending )
		{
		// This is lifted from SockBench to solve some apparently
		// strange behaviour with the timer.
		
		// Since we are still waiting for this timer, cancel it and
		// wait for the request to be flagged.  I believe this is
		// needed since the kernel may have setup a semaphore on the
		// 'TimerStatus' Request Status.  The Cancel() will flag the
		// status change (or maybe the timer will expire anyway) and
		// if we don't clear it, we may crash later when we return
		// to the Active Scheduler (since the TRequestStatus and
		// RTimer won't exist).
		//
		iGuardTimer.Cancel();
		User::WaitForRequest( timerStatus );
		}
	iGuardTimer.Cancel();
	}


/**
   Send data on a socket via SendTo

   @param aSocket  The socket to send the data on
   @param aDesc    The data to send
   @param anAddress The address to send to
   @return ETrue on success, EFalse on failure
*/
TBool CEsockTest9_6::PerformSend( RSocket& aSocket, const TDesC8& aDesc, TSockAddr& anAddress )
	{
	TRequestStatus socketStatus;

	aSocket.SendTo( aDesc, anAddress, 0, socketStatus );
	WaitFor( socketStatus, KTimerPeriod );

	Logger().WriteFormat(_L("UDP Send returns %d"), socketStatus.Int() );

	if( socketStatus == KRequestPending )
		{
		aSocket.CancelSend();
		Logger().WriteFormat(_L("Timeout on UDP Send"));
		User::WaitForRequest( socketStatus );
		return EFalse;
		}

	if( socketStatus != KErrNone )
		{
		Logger().WriteFormat(_L("UDP Send failed with error %d"), socketStatus.Int() );
		return EFalse;
		}

	return ETrue;
	}

/**
   Receive data on a socket via RecvFrom

   @param aSocket The socket to recevie data from
   @param aDesc Descriptor to receive the data into
   @param anAddress On success, contains the address from which data was received
   @return ETrue on success, EFalse on failure
*/
TBool CEsockTest9_6::PerformRecv( RSocket& aSocket, TDes8& aDesc, TSockAddr& anAddress )
	{
	TRequestStatus socketStatus;

	aSocket.RecvFrom( aDesc, anAddress, 0, socketStatus );
	WaitFor( socketStatus, KTimerPeriod );

	Logger().WriteFormat(_L("UDP Recv returns %d"), socketStatus.Int() );

	if( socketStatus == KRequestPending )
		{
		aSocket.CancelRecv();
		Logger().WriteFormat(_L("Timeout on UDP Recv"));
		User::WaitForRequest( socketStatus );
		return EFalse;
		}

	if( socketStatus != KErrNone )
		{
		Logger().WriteFormat(_L("UDP Recv failed with error %d"), socketStatus.Int() );
		return EFalse;
		}

	return ETrue;
	}

/**
   Create a UDP Socket and bind it to an arbitrary address

   @param aSocket The socket
   @param aAddress On return, contains the local socket name
   @return ETrue on success, EFalse otherwise
*/
TBool CEsockTest9_6::CreateUdpSocket( RSocket& aSocket, TSockAddr& aAddress )
	{
	// Open the socket
	TInt nRet = aSocket.Open( iEsockSuite->iSocketServer, KAfInet, KSockDatagram, KProtocolInetUdp );
	if( KErrNone != nRet )
		{
		Logger().WriteFormat(_L("Failed to open socket: return value = <%d>"), nRet );
		return EFalse;
		}

	// Bind to any address.
	//TInetAddr myAddress( KInetAddrAny, KInetPortAny );
	TInetAddr myAddress( KInetAddrLoop, KInetPortAny );
	nRet = aSocket.Bind( myAddress );
	if( KErrNone != nRet )
		{
		Logger().WriteFormat(_L("Failed to bind socket: return value = <%d>"), nRet );
		return EFalse;
		}

	// Retrieve the bound address
	aSocket.LocalName( aAddress );

	// Happy shiny
	return ETrue;
	}



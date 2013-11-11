// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This contains ESock Test cases from section 8
// 
//

// EPOC includes
#include <e32base.h>

#include <es_sock_internal.h>

// Test system includes
#include "SocketTestSection8.h"
#include "ES_DUMMY.H"


// Test step 8.1
const TDesC& CSocketTest8_1::GetTestName()
	{
	_LIT(ret,"Test8.1");
	return ret;
	}

enum TVerdict CSocketTest8_1::InternalDoTestStepL( void )
	{
	TVerdict verdict = EPass;
	
	Logger().WriteFormat(_L("Test Purpose: Ioctl returns not started on null socket"));
	
	// connect to esock
	Logger().WriteFormat(_L("Attempting to connect to socket server"));
    RSocketServ ss;
	TInt ret = OptimalConnect(ss);
	CleanupClosePushL(ss);
	Logger().WriteFormat(_L("Connect returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);
	
	//PG 14/02/2001
	//Added test for defect: TOD-4QLDXY - Ioctl on incorrectly opened socket crashes socket server.
	Logger().WriteFormat(_L("Opening a socket with no protocol"));
	RSocket socketNoProtoDefined;
	ret = socketNoProtoDefined.Open(ss);
	Logger().WriteFormat(_L("Open returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);
	
	// see if it's started
	Logger().WriteFormat(_L("Check Ioctl fails"));
	TRequestStatus stat;
	socketNoProtoDefined.Ioctl(KDummyIoctlCheckStarted, stat);
	User::WaitForRequest(stat);
	Logger().WriteFormat(_L("Ioctl returned with status %S"), &EpocErrorToText(stat.Int()));
	TESTL(KErrNotSupported == stat.Int());
	socketNoProtoDefined.Close();
	
	CleanupStack::Pop(&ss);
	ss.Close();
	
	SetTestStepResult(verdict);
	return verdict;
	}

// Test step 8.2
const TDesC& CSocketTest8_2::GetTestName()
	{
	_LIT(ret,"Test8.2");
	return ret;
	}

enum TVerdict CSocketTest8_2::InternalDoTestStepL( void )
	{
	TVerdict verdict = EPass;
	
	Logger().WriteFormat(_L("Test Purpose: Ioctl observes InternalOptionBit"));
	
	// connect to esock
	Logger().WriteFormat(_L("Attempting to connect to socket server"));
    RSocketServ ss;
	TInt ret = OptimalConnect(ss);
	CleanupClosePushL(ss);
	Logger().WriteFormat(_L("Connect returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);
	
	// get a protocol
	Logger().WriteFormat(_L("Attempting to FindProtocol dummy protocol 1"));
	TProtocolDesc protoInfo;
	ret = ss.FindProtocol(_L("Dummy Protocol 1"), protoInfo);
	Logger().WriteFormat(_L("FindProtocol returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);
	
	// check internal options bit
	RSocket socket;
	Logger().WriteFormat(_L("Opening a new socket"));
	ret = socket.Open(ss, protoInfo.iAddrFamily, protoInfo.iSockType, protoInfo.iProtocol);
	Logger().WriteFormat(_L("Open returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);
	
	Logger().WriteFormat(_L("Trying Ioctl on internal options bit"));
	TRequestStatus stat;
	socket.Ioctl(KInternalIoctlBit, stat);
	User::WaitForRequest(stat);
	Logger().WriteFormat(_L("Ioctl returned with status %S"), &EpocErrorToText(stat.Int()));
	TESTL(KErrAccessDenied == stat.Int());
	
	Logger().WriteFormat(_L("Trying to SetOpt internal options bit"));
	ret = socket.SetOpt(KSocketInternalOptionBit, KSOLSocket);
	Logger().WriteFormat(_L("SetOpt returned %S"), &EpocErrorToText(ret));
	TESTL(KErrAccessDenied == ret);
	
	Logger().WriteFormat(_L("Trying to GetOpt internal options bit"));
	TPckgBuf<TBool> boolean;
	ret = socket.GetOpt(KSocketInternalOptionBit, KSOLSocket, boolean);
	Logger().WriteFormat(_L("GetOpt returned %S"), &EpocErrorToText(ret));
	TESTL(KErrAccessDenied == ret);
	
	Logger().WriteFormat(_L("Testing StartL() with Ioctl KDummyIoctlCheckStarted"));
	socket.Ioctl(KDummyIoctlCheckStarted, stat, NULL, KIoctlDummyLevel);
	User::WaitForRequest(stat);
	Logger().WriteFormat(_L("Ioctl returned with status %S"), &EpocErrorToText(stat.Int()));
	TESTL(KErrNone == stat.Int());
	
	CleanupStack::Pop(&ss);
	ss.Close();
	
	SetTestStepResult(verdict);
	return verdict;
	}

// Test step 8.3
const TDesC& CSocketTest8_3::GetTestName()
	{
	_LIT(ret,"Test8.3");
	return ret;
	}

enum TVerdict CSocketTest8_3::InternalDoTestStepL( void )
	{
	TVerdict verdict = EPass;
	
	Logger().WriteFormat(_L("Test Purpose: Ioctl auto bind and check started"));
	
	// connect to esock
	Logger().WriteFormat(_L("Attempting to connect to socket server"));
    RSocketServ ss;
	TInt ret = OptimalConnect(ss);
	CleanupClosePushL(ss);
	Logger().WriteFormat(_L("Connect returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);
	
	// get a protocol
	Logger().WriteFormat(_L("Attempting to FindProtocol dummy protocol 1"));
	TProtocolDesc protoInfo;
	ret = ss.FindProtocol(_L("Dummy Protocol 1"), protoInfo);
	Logger().WriteFormat(_L("FindProtocol returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);
	
	Logger().WriteFormat(_L("Opening a new socket"));
	RSocket socket;
	ret = socket.Open(ss, protoInfo.iAddrFamily, protoInfo.iSockType, protoInfo.iProtocol);
	Logger().WriteFormat(_L("Open returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);
	
	Logger().WriteFormat(_L("Testing StartL() with Ioctl KDummyIoctlCheckStarted"));
	TRequestStatus stat;
	socket.Ioctl(KDummyIoctlCheckStarted, stat, NULL, KIoctlDummyLevel);
	User::WaitForRequest(stat);
	Logger().WriteFormat(_L("Ioctl returned with status %S"), &EpocErrorToText(stat.Int()));
	TESTL(KErrNone == stat.Int());
	
	Logger().WriteFormat(_L("Testing AutoBind()"));
	TSockAddr addr;
	Logger().WriteFormat(_L("Trying to Connect socket"));
	socket.Connect(addr, stat);
	User::WaitForRequest(stat);
	Logger().WriteFormat(_L("Connect returned with status %S"), &EpocErrorToText(stat.Int()));
	TESTL(KErrNone == stat.Int());
	
	Logger().WriteFormat(_L("Checking bound status with Ioctl"));
	socket.Ioctl(KDummyIoctlCheckBound, stat, NULL, KIoctlDummyLevel);
	User::WaitForRequest(stat);
	Logger().WriteFormat(_L("Ioctl returned with status %S"), &EpocErrorToText(stat.Int()));
	TESTL(KErrNone == stat.Int());
	
	CleanupStack::Pop(&ss);
	ss.Close();
	
	SetTestStepResult(verdict);
	return verdict;
	}

// Test step 8.4
const TDesC& CSocketTest8_4::GetTestName()
	{
	_LIT(ret,"Test8.4");
	return ret;
	}

enum TVerdict CSocketTest8_4::InternalDoTestStepL( void )
	{
	TVerdict verdict = EPass;
	
	Logger().WriteFormat(_L("Test Purpose: Ioctl Get and Set Options"));
	
	// connect to esock
	Logger().WriteFormat(_L("Attempting to connect to socket server"));
    RSocketServ ss;
	TInt ret = OptimalConnect(ss);
	CleanupClosePushL(ss);
	Logger().WriteFormat(_L("Connect returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);
	
	// try SetLocalPort
	Logger().WriteFormat(_L("Opening a socket"));
	RSocket socket;
	ret = socket.Open(ss, _L("Dummy Protocol 1"));
	Logger().WriteFormat(_L("Open returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);
	
	Logger().WriteFormat(_L("Trying SetLocalPort to 666"));
	ret = socket.SetLocalPort(666);
	Logger().WriteFormat(_L("SetLocalPort returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);
	Logger().WriteFormat(_L("Port now set to %d"), socket.LocalPort());
	TESTL(666 == socket.LocalPort());
	
	// try blocking Get/Set
	Logger().WriteFormat(_L("Testing blocking mode Get and Set options"));
	Logger().WriteFormat(_L("Trying to SetOption KSONonBlockingIO"));
	ret = socket.SetOpt(KSONonBlockingIO, KSOLSocket);
	Logger().WriteFormat(_L("SetOption returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);
	
	Logger().WriteFormat(_L("Trying to GetOption KSONonBlockingIO"));
	TPckgBuf<TBool> boolean;
	ret = socket.GetOpt(KSONonBlockingIO, KSOLSocket, boolean);
	Logger().WriteFormat(_L("GetOption returned %S, value %d"), 
		&EpocErrorToText(ret), boolean());
	TESTL(KErrNone == ret);
	TESTL(TRUE == boolean());
	
	Logger().WriteFormat(_L("Trying to GetOption KSOBlockingIO"));
	ret = socket.GetOpt(KSOBlockingIO, KSOLSocket, boolean);
	Logger().WriteFormat(_L("GetOption returned %S, value %d"), 
		&EpocErrorToText(ret), boolean());
	TESTL(KErrNone == ret);
	TESTL(FALSE == boolean());
	
	Logger().WriteFormat(_L("Trying to SetOption KSOBlockingIO"));
	ret = socket.SetOpt(KSOBlockingIO, KSOLSocket);
	Logger().WriteFormat(_L("SetOption returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);
	
	Logger().WriteFormat(_L("Trying to GetOption KSONonBlockingIO"));
	ret = socket.GetOpt(KSONonBlockingIO, KSOLSocket, boolean);
	Logger().WriteFormat(_L("GetOption returned %S, value %d"), 
		&EpocErrorToText(ret), boolean());
	TESTL(KErrNone == ret);
	TESTL(FALSE == boolean());
	
	Logger().WriteFormat(_L("Trying to GetOption KSOBlockingIO"));
	ret = socket.GetOpt(KSOBlockingIO, KSOLSocket, boolean);
	Logger().WriteFormat(_L("GetOption returned %S, value %d"), 
		&EpocErrorToText(ret), boolean());
	TESTL(KErrNone == ret);
	TESTL(TRUE == boolean());
	
	CleanupStack::Pop(&ss);
	ss.Close();
	
	return verdict;
	}

// Test step 8.5
const TDesC& CSocketTest8_5::GetTestName()
	{
	_LIT(ret,"Test8.5");
	return ret;
	}

enum TVerdict CSocketTest8_5::InternalDoTestStepL( void )
	{
	TVerdict verdict = EPass;
	
	Logger().WriteFormat(_L("Test Purpose: Ioctl returns not started on null socket"));
	
	// connect to esock
	Logger().WriteFormat(_L("Attempting to connect to socket server"));
    RSocketServ ss;
	TInt ret = OptimalConnect(ss);
    CleanupClosePushL(ss);
	Logger().WriteFormat(_L("Connect returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);
	
	// Added test for defect: DEF043787 	SAP never gets told to cancel an Ioctl if client calls RSocket::CancelAll().
	_LIT(KSomeProtocol, "Dummy Protocol 1");
	RSocket socket;
	ret = socket.Open(ss, KSomeProtocol);
	CleanupClosePushL(socket);
	Logger().WriteFormat(_L("Open returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);
	
	// Do test IOCTL thats never completes, then cancel it
	Logger().WriteFormat(_L("Do IOCTL/CancelALL."));
	TRequestStatus stat;
	socket.Ioctl(KDummyIoctlIgnore, stat);
	socket.CancelAll();
	User::WaitForRequest(stat);
	Logger().WriteFormat(_L("Ioctl returned with status %S"), &EpocErrorToText(stat.Int()));
	TESTL(KErrCancel == stat.Int());
	
	// Now query for the result of the previous IOCtl.
	socket.Ioctl(KDummyIoctlCancelled, stat);
	User::WaitForRequest(stat);
	if(stat==KErrNotFound)
		{
		Logger().WriteFormat(_L("Ioctl returned with status %S: CancelAll failed to cancel previous IOCTL."), &EpocErrorToText(stat.Int()));
		}
	else
		{
		Logger().WriteFormat(_L("Ioctl returned with status %S"), &EpocErrorToText(stat.Int()));
		}
	TESTL(KErrNone == stat.Int());
	
	CleanupStack::PopAndDestroy(2,&ss); // socket, ss
	SetTestStepResult(verdict);
	return verdict;
	}
	
const TDesC &CSocketTest8_6::GetTestName()
	{
	_LIT(ret,"Test8.6");
	return ret;
	}
	
enum TVerdict CSocketTest8_6::InternalDoTestStepL( void )
	{
	TVerdict verdict = EPass;
	
	Logger().WriteFormat(_L("Test Purpose: Retention of socket options from before Accept to after Accept"));
	
	// connect to esock
	Logger().WriteFormat(_L("Attempting to connect to socket server"));
    RSocketServ ss1;
	TInt ret = OptimalConnect(ss1);
	TESTL(KErrNone == ret);
	CleanupClosePushL(ss1);
	
	Logger().WriteFormat(_L("Opening Socket on pdummy3"));
	RSocket sock1;
	ret = sock1.Open(ss1,_L("Dummy Protocol 3"));
	TESTL(KErrNone == ret);
	
	Logger().WriteFormat(_L("Binding Sock1"));
	TSockAddr addr;
	ret = sock1.Bind(addr);
	TESTL(KErrNone == ret);
	
	Logger().WriteFormat(_L("Listening on Sock1"));
	ret = sock1.Listen(5);
	TESTL(KErrNone == ret);
	
	Logger().WriteFormat(_L("Opening a null Socket"));
	RSocket nullSock;
	nullSock.Open(ss1);
	TESTL(KErrNone == ret);
	
	const TUint KBufferSize = 1024;
	Logger().WriteFormat(_L("Setting a whole bunch of KSOLSocket options on the parent (sock1) socket"));
	ret = sock1.SetOpt(KSOSendBuf, KSOLSocket, KBufferSize);
	TESTL(KErrNone == ret);
	ret = sock1.SetOpt(KSORecvBuf, KSOLSocket, KBufferSize);
	TESTL(KErrNone == ret);
	ret = sock1.SetOpt(KSODebug, KSOLSocket, ETrue);
	TESTL(KErrNone == ret);
	ret = sock1.SetOpt(KSONonBlockingIO, KSOLSocket);
	TESTL(KErrNone == ret);
	_LIT_SECURITY_POLICY_C2(KProcPolicy, ECapabilityNetworkServices, ECapabilityNetworkControl);
	ret = nullSock.SetOpt(KSOEnableTransfer, KSOLSocket, KProcPolicy().Package());
	TESTL(KErrNotReady == ret);
	
	Logger().WriteFormat(_L("Now simulating a remote connection to complete the accept"));
	ret = sock1.SetOpt(KDummyOptionSetConnectComplete, KIoctlDummyLevel);
	TESTL(KErrNone == ret);
	Logger().WriteFormat(_L("Now performing accept"));
	TRequestStatus stat;
	sock1.Accept(nullSock, stat);
	User::WaitForRequest(stat);
	TESTL(KErrNone == stat.Int());
	
	Logger().WriteFormat(_L("Now retrieving all the options set on the null socket with getOpts"));
	TInt retrievedOpt;
	ret = nullSock.GetOpt(KSOSendBuf, KSOLSocket, retrievedOpt);
	TESTL(KErrNone == ret);
	if (retrievedOpt != KBufferSize)
		{
		Logger().WriteFormat(_L("Opt KSOSendBuf did not return the correct value"));
		User::Leave(EFail);
		}	
	ret = nullSock.GetOpt(KSORecvBuf, KSOLSocket, retrievedOpt);
	TESTL(KErrNone == ret);
	if (retrievedOpt != KBufferSize)
		{
		Logger().WriteFormat(_L("Opt KSORecvBuf did not return the correct value"));
		User::Leave(EFail);
		}	
	ret = nullSock.GetOpt(KSODebug, KSOLSocket, retrievedOpt);
	TESTL(KErrNone == ret);
	if (!retrievedOpt) 
		{
		Logger().WriteFormat(_L("Opt KSODebug did not return the correct value"));
		User::Leave(EFail);
		}
	ret = nullSock.GetOpt(KSONonBlockingIO, KSOLSocket, retrievedOpt);
	TESTL(KErrNone == ret);
	if (!retrievedOpt)
		{
		Logger().WriteFormat(_L("Opt KSONonBlockingIO did not return the correct value"));
		User::Leave(EFail);
		}
	
	CleanupStack::Pop(&ss1);
	ss1.Close();
	
	return verdict;
	}

const TDesC &CSocketTest8_7::GetTestName()
	{
	_LIT(ret,"Test8.7");
	return ret;
	}

enum TVerdict CSocketTest8_7::InternalDoTestStepL( void )
	{
	TVerdict verdict = EPass;
	
	Logger().WriteFormat(_L("Test Purpose: getOpt with small client buffer"));
	
	// connect to esock
	Logger().WriteFormat(_L("Attempting to connect to socket server"));
    RSocketServ ss;
	TInt ret = OptimalConnect(ss);
	CleanupClosePushL(ss);
	Logger().WriteFormat(_L("Connect returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);
	
	// try SetLocalPort
	Logger().WriteFormat(_L("Opening a socket"));
	RSocket socket;
	ret = socket.Open(ss, _L("Dummy Protocol 1"));
	Logger().WriteFormat(_L("Open returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);
	
	// try with small buffer - 8 bit descriptor
	Logger().WriteFormat(_L("Trying to GetOption KSOBlockingIO"));
	TBuf8<1> smallBuf8;
	ret = socket.GetOpt(KSOBlockingIO, KSOLSocket, smallBuf8);
	Logger().WriteFormat(_L("GetOption returned %S"),&EpocErrorToText(ret));
	TESTL(KErrOverflow == ret);
		
	// try with correct size descriptor
	Logger().WriteFormat(_L("Trying to GetOption KSOBlockingIO"));
	TPckgBuf<TBool> boolean;
	ret = socket.GetOpt(KSOBlockingIO, KSOLSocket, boolean);
	Logger().WriteFormat(_L("GetOption returned %S, value %d"), 
			&EpocErrorToText(ret), boolean());
	TESTL(KErrNone == ret);
	TESTL(TRUE == boolean());
		
	CleanupStack::Pop(&ss);
	ss.Close();
	
	return verdict;
	}

const TDesC &CSocketTest8_8::GetTestName()
	{
	_LIT(ret,"Test8.8");
	return ret;
	}

enum TVerdict CSocketTest8_8::InternalDoTestStepL( void )
	{
	TVerdict verdict = EPass;

	Logger().WriteFormat(_L("Test Purpose: KSOSelectLastError option update"));
	RSocketServ ss;
	TInt ret=OptimalConnect(ss);
	CleanupClosePushL(ss);
    TESTL(KErrNone == ret);	
    
	Logger().WriteFormat(_L("Open by name"));             
	RSocket sock, sock2;                                                    
	ret=sock.Open(ss,_L("Dummy Protocol 3")); // dummy 3 is reconnectable                         
	TESTL(KErrNone == ret);	

	// get last error - check that option value is updated
  	TInt val;
   	ret = sock.GetOpt(KSOSelectLastError, KSOLSocket, val);
   	Logger().WriteFormat(_L("KSOSelectLastError returned with status %S"), &EpocErrorToText(ret));	
   	TESTL(KErrNone == ret);
   	// return value should match the updated value
   	TESTL (val == ret);

	ret=sock2.Open(ss); // null socket to accept onto                         
	TESTL(KErrNone == ret);	
	
	Logger().WriteFormat(_L("Set test option"));             
	const TInt KExpectedError = -4567; // Something random.
	ret=sock.SetOpt(KDummyOptionSetErrorNextListen,0,KExpectedError);
	TESTL(KErrNone == ret);	
	
	TSockAddr addr;
	ret=sock.Bind(addr);
	TESTL(KErrNone == ret);	
	
	Logger().WriteFormat(_L("Listen"));             
	ret=sock.Listen(0); // No need for a que
	TESTL(KErrNone == ret);	
	
	Logger().WriteFormat(_L("Accept - error"));             
	TRequestStatus rstat;
	sock.Accept(sock2, rstat);
	User::WaitForRequest(rstat);
	TESTL(rstat.Int() == KExpectedError);	
	
	// get last error
   	ret = sock.GetOpt(KSOSelectLastError, KSOLSocket, val);
   	Logger().WriteFormat(_L("KSOSelectLastError returned with status %S"), &EpocErrorToText(ret));	
   	TESTL(KExpectedError == ret);
   	// return value should match the updated value
   	TESTL(val == ret);
	
	// Clears last error
   	ret = sock.GetOpt(KSOSelectLastError, KSOLSocket, val);
   	Logger().WriteFormat(_L("KSOSelectLastError returned with status %S"), &EpocErrorToText(ret));	
   	TESTL(KErrNone == ret);	
   	// return value should match the updated value
   	TESTL(val == ret);
   	
	CleanupStack::Pop(&ss);
	ss.Close();
	
	SetTestStepResult(verdict);
	return verdict;
	}



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
// This contains ESock Test cases from section 14
// 
//

// EPOC includes
#include <e32base.h>
#include <test/es_dummy.h>

// Test system includes
#include "SocketTestSection14.h"


// Test step 14.1
const TDesC& CSocketTest14_1::GetTestName()
	{
	_LIT(ret,"Test14.1");
	return ret;
	}

enum TVerdict CSocketTest14_1::InternalDoTestStepL( void )
	{
	TVerdict verdict = EPass;
	
	Logger().WriteFormat(_L("Testing erroring a listening socket"));
	RSocketServ ss;
	TInt ret=OptimalConnect(ss);
	CleanupClosePushL(ss);
    TESTL(KErrNone == ret);	
    
	Logger().WriteFormat(_L("Open by name"));             
	RSocket sock, sock2;                                                    
	ret=sock.Open(ss,_L("Dummy Protocol 3")); // dummy 3 is reconnectable                         
	TESTL(KErrNone == ret);	
	
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
	
	Logger().WriteFormat(_L("Listen - reconnect"));             
	ret=sock.Listen(1);
	TESTL(KErrNone == ret);	
	
    Logger().WriteFormat(_L("Accept"));             
	sock.Accept(sock2, rstat);
    //emulate en connect complete event
	ret=sock.SetOpt(KDummyOptionSetConnectComplete,0,0);
	TESTL(KErrNone == ret);	
	User::WaitForRequest(rstat);
	TESTL(rstat.Int() == KErrNone);	
	
	CleanupStack::Pop(&ss);
    ss.Close();
	
	SetTestStepResult(verdict);
	return verdict;
	}
	
	// Test step 14.2
const TDesC& CSocketTest14_2::GetTestName()
	{
	_LIT(ret,"Test14.2");
	return ret;
	}

enum TVerdict CSocketTest14_2::InternalDoTestStepL( void )
	{
	TVerdict verdict = EPass;
	TInt ret;
	
	Logger().WriteFormat(_L("Test14.2 Error writing to socket"));
	RSocketServ ss;
	ret = OptimalConnect(ss);
	TESTL(KErrNone == ret);	
	CleanupClosePushL(ss);
    
	Logger().WriteFormat(_L("Openning Dummy Protocol 3"));             
	RSocket sock;                                                  
	ret = sock.Open(ss,_L("Dummy Protocol 3")); // dummy 3 is reconnectable
	TESTL(KErrNone == ret);
	
	TSockAddr addr;
	TRequestStatus stat;
	sock.Connect(addr, stat);
	User::WaitForRequest(stat);
	TESTL(KErrNone == stat.Int());	
	
	const TInt KExpectedError = -4567; // Something random.
	Logger().WriteFormat(_L("Set write error code to %d"), KExpectedError);             
	ret=sock.SetOpt(KDummyOptionSetErrorNextWrite, 0, KExpectedError);
	TESTL(KErrNone == ret);	
		
	_LIT8(KWriteData,"1234\t\n\rcwe8\'\"");
	Logger().WriteFormat(_L("Sending Data - Should fail with %d"), KExpectedError);             
	sock.Send(KWriteData(), 0, stat);
	User::WaitForRequest(stat);
	TESTL(KExpectedError == stat.Int());	
	
	Logger().WriteFormat(_L("Sending Data - Should Succeed"));             
	sock.Send(KWriteData(),0,stat);
	User::WaitForRequest(stat);
	TESTL(KErrNone == stat.Int());	
	
	Logger().WriteFormat(_L("Receiving Data"));
	TBuf8<20> buf;          
	sock.Recv(buf, 0, stat);
	User::WaitForRequest(stat);
	TESTL(KErrNone == stat.Int());	
	
    TESTL(buf == KWriteData);	
	
	CleanupStack::Pop(&ss);
    ss.Close();
	
	SetTestStepResult(verdict);
	return verdict;
	}
	
	// Test step 14.3
const TDesC& CSocketTest14_3::GetTestName()
	{
	_LIT(ret,"Test14.3");
	return ret;
	}

enum TVerdict CSocketTest14_3::InternalDoTestStepL( void )
	{
	TVerdict verdict = EPass;
	TInt ret;
	
	Logger().WriteFormat(_L("Test14.3 Error shutting a socket down"));
	RSocketServ ss;
	ret = OptimalConnect(ss);
	TESTL(KErrNone == ret);	
	CleanupClosePushL(ss);
    
	Logger().WriteFormat(_L("Openning Dummy Protocol 3"));             
	RSocket sock;                                                  
	ret = sock.Open(ss,_L("Dummy Protocol 3")); // dummy 3 is reconnectable
	TESTL(KErrNone == ret);
	
	TSockAddr addr;
	TRequestStatus stat;
	sock.Connect(addr, stat);
	User::WaitForRequest(stat);
	TESTL(KErrNone == stat.Int());	
	
	const TInt KExpectedError = -4567; // Something random.
	Logger().WriteFormat(_L("Set write error code to %d"), KExpectedError);             
	ret=sock.SetOpt(KDummyOptionSetErrorNextShutdown, 0, KExpectedError);
	TESTL(KErrNone == ret);	
	
	Logger().WriteFormat(_L("Shutting the socket down"));             
	sock.Shutdown(RSocket::EImmediate,stat);
	User::WaitForRequest(stat);
	TESTL(KExpectedError == stat.Int());	
	
	CleanupStack::Pop(&ss);
    ss.Close();
	
	SetTestStepResult(verdict);
	return verdict;
	}
	
		// Test step 14.4
const TDesC& CSocketTest14_4::GetTestName()
	{
	_LIT(ret,"Test14.4");
	return ret;
	}

enum TVerdict CSocketTest14_4::InternalDoTestStepL( void )
	{
	TVerdict verdict = EPass;
	TInt ret;
	
	Logger().WriteFormat(_L("Test14.4 Error connecting to socket down"));
	RSocketServ ss;
	ret = OptimalConnect(ss);
	TESTL(KErrNone == ret);	
	CleanupClosePushL(ss);
    
	Logger().WriteFormat(_L("Openning Dummy Protocol 3"));             
	RSocket sock;                                                  
	ret = sock.Open(ss,_L("Dummy Protocol 3")); // dummy 3 is reconnectable
	TESTL(KErrNone == ret);
	
	const TInt KExpectedError = -4567; // Something random.
	Logger().WriteFormat(_L("Set connect error code to %d"), KExpectedError);             
	ret=sock.SetOpt(KDummyOptionSetErrorNextConnect, 0, KExpectedError);
	TESTL(KErrNone == ret);
	
	TSockAddr addr;
	TRequestStatus stat;
	sock.Connect(addr, stat);
	User::WaitForRequest(stat);
	TESTL(KExpectedError == stat.Int());
	
	Logger().WriteFormat(_L("Set write error code to %d"), KExpectedError);             
	ret=sock.SetOpt(KDummyOptionSetErrorNextConnect, 0, KExpectedError);
	TESTL(KErrNone == ret);
	
	Logger().WriteFormat(_L("Performing Connect"));
	sock.Connect(addr, stat);
	User::WaitForRequest(stat);
	TESTL(KExpectedError == stat.Int());	
	
	Logger().WriteFormat(_L("Setting blocked connect"));             
	ret=sock.SetOpt(KDummyOptionSetBlockConnect, 0, TRUE);
	TESTL(KErrNone == ret);
	
	Logger().WriteFormat(_L("Set connect error code to %d"), KExpectedError);             
	ret=sock.SetOpt(KDummyOptionSetErrorNextConnect, 0, KExpectedError);
	TESTL(KErrNone == ret);	
	
	Logger().WriteFormat(_L("Performing Connect"));
	sock.Connect(addr, stat);
	User::WaitForRequest(stat);
	TESTL(KExpectedError == stat.Int());		
	
	Logger().WriteFormat(_L("Setting blocked connect"));             
	ret=sock.SetOpt(KDummyOptionSetBlockConnect, 0, TRUE);
	TESTL(KErrNone == ret);
	
	Logger().WriteFormat(_L("Performing Connect"));
	sock.Connect(addr, stat);
	
	Logger().WriteFormat(_L("Set connect error code to %d"), KExpectedError);             
	ret=sock.SetOpt(KDummyOptionSetErrorNextConnect, 0, KExpectedError);
	TESTL(KErrNone == ret);	
	
	Logger().WriteFormat(_L("Setting blocked connect off"));             
	ret=sock.SetOpt(KDummyOptionSetBlockConnect, 0, FALSE);
	TESTL(KErrNone == ret);
		
	User::WaitForRequest(stat);
	TESTL(KExpectedError == stat.Int());	
	
	
	CleanupStack::Pop(&ss);
    ss.Close();
	
	SetTestStepResult(verdict);
	return verdict;
	}
	
		// Test step 14.5
const TDesC& CSocketTest14_5::GetTestName()
	{
	_LIT(ret,"Test14.5");
	return ret;
	}

enum TVerdict CSocketTest14_5::InternalDoTestStepL( void )
	{
	TVerdict verdict = EPass;
	TInt ret;
	
	Logger().WriteFormat(_L("Test14.5 Two accepts accepting on the same null socket"));
	RSocketServ ss;
	ret = OptimalConnect(ss);
	TESTL(KErrNone == ret);	
	CleanupClosePushL(ss);
    
	Logger().WriteFormat(_L("Openning Dummy Protocol 2"));             
	RSocket sock;                                                  
	ret = sock.Open(ss,_L("Dummy Protocol 2")); 
	TESTL(KErrNone == ret);
	
	TSockAddr addr;
	TRequestStatus stat;
	
	Logger().WriteFormat(_L("Binding sock1"));             
	ret = sock.Bind(addr); 
	TESTL(KErrNone == ret);
	
	Logger().WriteFormat(_L("Listening on sock1"));             
	ret = sock.Listen(5); 
	TESTL(KErrNone == ret);
	
	Logger().WriteFormat(_L("Openning Dummy Protocol 3"));             
	RSocket sock2;                                                  
	ret = sock2.Open(ss,_L("Dummy Protocol 3")); 
	TESTL(KErrNone == ret);
	
	TSockAddr addr2;
		
	Logger().WriteFormat(_L("Binding sock2"));             
	ret = sock2.Bind(addr); 
	TESTL(KErrNone == ret);
	
	Logger().WriteFormat(_L("Listening on sock2"));             
	ret = sock2.Listen(5); 
	TESTL(KErrNone == ret);
	
	Logger().WriteFormat(_L("Null socket"));             
	RSocket nullsock;                                                  
	ret = nullsock.Open(ss); 
	TESTL(KErrNone == ret);
	
	TRequestStatus stat2;
	
	Logger().WriteFormat(_L("Accepting on sock1"));             
	sock.Accept(nullsock,stat); 
	
	Logger().WriteFormat(_L("Accepting on sock2"));             
	sock2.Accept(nullsock,stat2); 
	
	// Wait to be paniced
	User::After( 5000000 );
	
	//Should have panicked by now so lets just fail the test.
	verdict = EFail;
	CleanupStack::Pop(&ss);
    ss.Close();
	
	SetTestStepResult(verdict);
	return verdict;
	}





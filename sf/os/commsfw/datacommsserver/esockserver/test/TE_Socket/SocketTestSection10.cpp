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
// This contains ESock Test cases from section 10
//
//

// EPOC includes
#include <e32base.h>
#include <test/es_dummy.h>

// Test system includes
#include "SocketTestSection10.h"


// Test step 10.1
const TDesC& CSocketTest10_1::GetTestName()
	{
	_LIT(ret,"Test10.1");
	return ret;
	}

enum TVerdict CSocketTest10_1::InternalDoTestStepL( void )
	{
	TVerdict verdict = EPass;
	
	Logger().WriteFormat(_L("Test Purpose: Shutdown with EStopOutput"));
	
	// connect to esock
	Logger().WriteFormat(_L("Attempting to connect to socket server"));
    RSocketServ ss;
	TInt ret = OptimalConnect(ss);
	CleanupClosePushL(ss);
	Logger().WriteFormat(_L("Connect returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);
	
	Logger().WriteFormat(_L("Opening a socket on Dummy protocol 2"));
	RSocket sk;
	ret = sk.Open(ss, _L("Dummy Protocol 2"));
	Logger().WriteFormat(_L("Open returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);
	
	Logger().WriteFormat(_L("Shutting down socket with EStopOutput"));
	TRequestStatus shutStat;
	TRequestStatus rstat;
	sk.Shutdown(RSocket::EStopOutput, shutStat);
	TESTL(KRequestPending == shutStat.Int());
	
	// Check we can still receive
	Logger().WriteFormat(_L("Attempting to receive data"));
	TSockXfrLength len;
	TBuf8<6> buf;
	TBuf<6> buf16;
	sk.Recv(buf, 0, rstat, len);
	User::WaitForRequest(rstat);
	TESTL(KErrNone == rstat.Int());
	buf16.Copy(buf);
	Logger().WriteFormat(_L("Recv status %S, length %d, buf '%S'"), 
		&EpocErrorToText(rstat.Int()), len(), &buf16);
	TESTL(KErrNone == rstat.Int());
	TESTL(_L8("End of") == buf);
	TESTL(6 == len());
	
	// Check writing fails
	Logger().WriteFormat(_L("Attempting to Send data"));
	buf.SetLength(1);
	sk.Send(buf, 0, rstat);
	User::WaitForRequest(rstat);
	Logger().WriteFormat(_L("Send status %S"), &EpocErrorToText(rstat.Int()));
	TESTL(KErrNotReady == rstat.Int());
	
	Logger().WriteFormat(_L("Attempting to receive more data"));
	sk.Recv(buf, 0, rstat, len);
	User::WaitForRequest(rstat);
	buf16.Copy(buf);
	Logger().WriteFormat(_L("Recv status %S, length %d, buf '%S'"), 
		&EpocErrorToText(rstat.Int()), len(), &buf16);
	TESTL(KErrEof == rstat.Int());
	TESTL(_L8(" Data") == buf);
	TESTL(5 == len());
	
	Logger().WriteFormat(_L("Attempting to receive more data"));
	sk.Recv(buf, 0, rstat, len);
	User::WaitForRequest(rstat);
	buf16.Copy(buf);
	Logger().WriteFormat(_L("Recv status %S, length %d, buf '%S'"), 
		&EpocErrorToText(rstat.Int()), len(), &buf16);
	TESTL(KErrEof == rstat.Int());
	TESTL(0 == buf.Length());
	TESTL(0 == len());
	
	Logger().WriteFormat(_L("Closing socket"));
	sk.Close(); // Should cancel shutStat
	User::WaitForRequest(shutStat);
	Logger().WriteFormat(_L("Shutdown status %S"), &EpocErrorToText(shutStat.Int()));
	TESTL(KErrCancel == shutStat.Int());
	
	ss.Close();
	
	CleanupStack::Pop(&ss);
	
	SetTestStepResult(verdict);
	return verdict;
	}

// Test step 10.2
const TDesC& CSocketTest10_2::GetTestName()
	{
	_LIT(ret,"Test10.2");
	return ret;
	}

enum TVerdict CSocketTest10_2::InternalDoTestStepL( void )
	{
	TVerdict verdict = EPass;
	
	Logger().WriteFormat(_L("Test Purpose: Shutdown"));
	
	// connect to esock
	Logger().WriteFormat(_L("Attempting to connect to socket server"));
    RSocketServ ss;
	TInt ret = OptimalConnect(ss);
	CleanupClosePushL(ss);
	Logger().WriteFormat(_L("Connect returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);
	
	Logger().WriteFormat(_L("Opening a socket on Dummy protocol 2"));
	RSocket sk;
	ret = sk.Open(ss, _L("Dummy Protocol 2"));
	Logger().WriteFormat(_L("Open returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);
	
	Logger().WriteFormat(_L("Shutting down socket with EStopInput"));
	TRequestStatus shutStat;
	TRequestStatus rstat;
	sk.Shutdown(RSocket::EStopInput, shutStat);		// Dummy doesn't complete EStopInput
	
	// Check we can still send
	Logger().WriteFormat(_L("Attempting to Send data"));
	TBuf8<6> buf;
	buf.SetLength(1);
	sk.Send(buf, 0, rstat);
	sk.CancelSend();
	User::WaitForRequest(rstat);
	Logger().WriteFormat(_L("Send status %S"), &EpocErrorToText(rstat.Int()));
	TESTL(KErrNotReady == rstat.Int());
	
	// Check receiving fails
	Logger().WriteFormat(_L("Attempting to Receive data"));
	TSockXfrLength len;
	sk.Recv(buf, 0, rstat, len);
	User::WaitForRequest(rstat);
	Logger().WriteFormat(_L("Recv status %S, length %d"), &EpocErrorToText(rstat.Int()), len());
	TESTL(KErrEof == rstat.Int());
	TESTL(0 == len());
	
	Logger().WriteFormat(_L("Closing socket"));
	sk.Close(); // Should cancel shutStat
	User::WaitForRequest(shutStat);
	Logger().WriteFormat(_L("Shutdown status %S"), &EpocErrorToText(shutStat.Int()));
	TESTL(KErrCancel == shutStat.Int());
	
	CleanupStack::Pop(&ss);
	ss.Close();
	
	SetTestStepResult(verdict);
	return verdict;
	}
	
	// Test step 10.2
	const TDesC& CSocketTest10_3::GetTestName()
	{
	_LIT(ret,"Test10.3");
	return ret;
	}
	
	enum TVerdict CSocketTest10_3::InternalDoTestStepL( void )
	{
	TVerdict verdict = EPass;
	
	Logger().WriteFormat(_L("Testing close before completion of accept on new socket"));
	RSocketServ ss;
	TInt ret=OptimalConnect(ss);
	CleanupClosePushL(ss);
    TESTL(KErrNone == ret);	
    
	Logger().WriteFormat(_L("Opening main socket by name"));             
	RSocket sock, sock2;                                                    
	ret=sock.Open(ss,_L("Dummy Protocol 2")); 
	TESTL(KErrNone == ret);	
	
	Logger().WriteFormat(_L("Opening null socket"));  
	ret=sock2.Open(ss); // null socket to accept onto                         
	TESTL(KErrNone == ret);	
	
	TSockAddr addr;
	Logger().WriteFormat(_L("Binding main socket"));
	ret=sock.Bind(addr);
	TESTL(KErrNone == ret);	
	
	Logger().WriteFormat(_L("Performing Listen"));
	Logger().WriteFormat(_L("Listen"));             
	ret=sock.Listen(5);
	TESTL(KErrNone == ret);	
	
	Logger().WriteFormat(_L("Performing Accept"));             
	TRequestStatus rstat;
	sock.Accept(sock2, rstat);
	TESTL(rstat.Int() == KRequestPending);	
	
	User::After(2 * 1000 * 1000);	// make sure accept processing has proceeded before closing the socket, otherwise
									// we risk incurring a BadHandle panic from it (as seen on SMP testing)
	
	Logger().WriteFormat(_L("Closing new socket"));
	sock2.Close();             

	Logger().WriteFormat(_L("Attempting to simulate a connect and complete the accept"));             
	//emulate en connect complete event
	ret=sock.SetOpt(KDummyOptionSetConnectComplete,KIoctlDummyLevel,NULL);
	TESTL(KErrNone == ret);	
	User::WaitForRequest(rstat); //Just a wait to allow something to possibly go wrong
	TESTL(rstat.Int() == KErrCancel);
	
	Logger().WriteFormat(_L("Opening null socket"));  
	ret=sock2.Open(ss); // null socket to accept onto                         
	TESTL(KErrNone == ret);	
	
	Logger().WriteFormat(_L("Performing Accept again"));           
	sock.Accept(sock2, rstat);
	TESTL(rstat.Int() == KRequestPending);	
	
	Logger().WriteFormat(_L("Attempting to simulate a connect and complete the accept"));             
	//emulate en connect complete event
	ret=sock.SetOpt(KDummyOptionSetConnectComplete,KIoctlDummyLevel,NULL);
	TESTL(KErrNone == ret);	
	User::WaitForRequest(rstat); //Just a wait to allow something to possibly go wrong
	TESTL(rstat.Int() == KErrNone);		
	
	CleanupStack::Pop(&ss);
    ss.Close();
	
	SetTestStepResult(verdict);
	return verdict;
	}


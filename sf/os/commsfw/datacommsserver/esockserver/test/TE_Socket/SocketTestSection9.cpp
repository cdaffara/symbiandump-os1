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
// This contains ESock Test cases from section 9
// 
//

// EPOC includes
#include <e32base.h>

#include <test/es_dummy.h>

// Test system includes
#include "SocketTestSection9.h"


// Test step 9.1
const TDesC& CSocketTest9_1::GetTestName()
	{
	_LIT(ret,"Test9.1");
	return ret;
	}

enum TVerdict CSocketTest9_1::InternalDoTestStepL( void )
	{
	TVerdict verdict = EPass;
	
	Logger().WriteFormat(_L("Test Purpose: Partial Read"));
	
	// connect to esock
	Logger().WriteFormat(_L("Attempting to connect to socket server"));
    RSocketServ ss;
	TInt ret = OptimalConnect(ss);
	CleanupClosePushL(ss);
	Logger().WriteFormat(_L("Connect returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);
	
	RSocket sk;
	Logger().WriteFormat(_L("Opening a socket on Dummy protocol 2"));
	ret = sk.Open(ss, _L("Dummy Protocol 2"));
	CleanupClosePushL(sk);
	Logger().WriteFormat(_L("Open returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);
	
	Logger().WriteFormat(_L("Connecting socket"));
	TRequestStatus shutStat;
	TRequestStatus rstat;
	TSockAddr addr;
	sk.Connect(addr, rstat);
	User::WaitForRequest(rstat);
	Logger().WriteFormat(_L("Connect status %S"), &EpocErrorToText(rstat.Int()));
	TESTL(KErrNone == rstat.Int());
	
	// test XSockXfrLength
	Logger().WriteFormat(_L("Trying recv of 6 octets and send of 12"));
	TSockXfrLength len, rlen;
	TBuf8<6> buf;
	TBuf16<6>buf16;
	sk.Recv(buf, 0, shutStat, rlen);
	sk.Send(_L8("Hello HELLO "), 0, rstat, len);
	User::WaitForRequest(rstat);
	Logger().WriteFormat(_L("Send status %S, length %d"), &EpocErrorToText(rstat.Int()), len());
	TESTL(KErrNone == rstat.Int());
	TESTL(12 == len());
	
	User::WaitForRequest(shutStat);
	buf16.Copy(buf);
	Logger().WriteFormat(_L("Recv status %S, length %d, buffer '%S'"), 
		&EpocErrorToText(shutStat.Int()), rlen(), &buf16);
	TESTL(KErrNone == shutStat.Int());
	TESTL(6 == rlen());
	TESTL(_L8("Hello ") == buf);
	
	Logger().WriteFormat(_L("Trying recv of another 6 octets"));
	sk.Recv(buf, 0, rstat, rlen);
	User::WaitForRequest(rstat);
	buf16.Copy(buf);
	Logger().WriteFormat(_L("Recv status %S, length %d, buffer '%S'"), 
		&EpocErrorToText(shutStat.Int()), rlen(), &buf16);
	TESTL(KErrNone == shutStat.Int());
	TESTL(6 == rlen());
	TESTL(_L8("HELLO ") == buf);
	
	CleanupStack::Pop(&sk);
	sk.Close();
	CleanupStack::Pop(&ss);
	ss.Close();
	
	SetTestStepResult(verdict);
	return verdict;
	}

// Test step 9.2
const TDesC& CSocketTest9_2::GetTestName()
	{
	_LIT(ret,"Test9.2");
	return ret;
	}

enum TVerdict CSocketTest9_2::InternalDoTestStepL( void )
	{
	TVerdict verdict = EPass;
	
	Logger().WriteFormat(_L("Test Purpose: RecvFrom not supported on ConnectionOriented socket"));
	
	// connect to esock
	Logger().WriteFormat(_L("Attempting to connect to socket server"));
    RSocketServ ss;
	TInt ret = OptimalConnect(ss);
	CleanupClosePushL(ss);
	Logger().WriteFormat(_L("Connect returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);
	
	RSocket sk;
	Logger().WriteFormat(_L("Opening a socket on Dummy protocol 2"));
	ret = sk.Open(ss, _L("Dummy Protocol 2"));
	CleanupClosePushL(sk);
	Logger().WriteFormat(_L("Open returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);
	
	Logger().WriteFormat(_L("Connecting socket"));
	TRequestStatus rstat;
	TSockAddr addr;
	sk.Connect(addr, rstat);
	User::WaitForRequest(rstat);
	Logger().WriteFormat(_L("Connect status %S"), &EpocErrorToText(rstat.Int()));
	TESTL(KErrNone == rstat.Int());
	
	Logger().WriteFormat(_L("Trying RecvFrom"));
	TSockXfrLength rlen;
	TBuf8<6> buf;
	sk.RecvFrom(buf, addr, 0, rstat, rlen);
	User::WaitForRequest(rstat);
	Logger().WriteFormat(_L("RecvFrom status %S"), &EpocErrorToText(rstat.Int()));
	TESTL(KErrNotSupported == rstat.Int());
	
	CleanupStack::Pop(&sk);
	sk.Close();
	CleanupStack::Pop(&ss);
	ss.Close();
	
	SetTestStepResult(verdict);
	return verdict;
	}

// Test step 9.3
const TDesC& CSocketTest9_3::GetTestName()
	{
	_LIT(ret,"Test9.3");
	return ret;
	}

enum TVerdict CSocketTest9_3::InternalDoTestStepL( void )
	{
	TVerdict verdict = EPass;
	
	Logger().WriteFormat(_L("Test Purpose: Fail Urgent Data on Non-Supporting Socket"));
	
	// connect to esock
	Logger().WriteFormat(_L("Attempting to connect to socket server"));
    RSocketServ ss;
	TInt ret = OptimalConnect(ss);
	CleanupClosePushL(ss);
	Logger().WriteFormat(_L("Connect returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);
	
	Logger().WriteFormat(_L("Finding for dummy protocol 1"));
	TProtocolDesc protoInfo;
	ret = ss.FindProtocol(_L("Dummy Protocol 1"), protoInfo);
	Logger().WriteFormat(_L("FindProtocol returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);
	
	Logger().WriteFormat(_L("Opening a socket"));
	RSocket socket;
	ret = socket.Open(ss, protoInfo.iAddrFamily, protoInfo.iSockType, protoInfo.iProtocol);
	CleanupClosePushL(socket);
	Logger().WriteFormat(_L("Open returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);
	
	Logger().WriteFormat(_L("Trying to write urgent data"));
	TBuf8<10> buf;
	TRequestStatus stat;
	buf.SetLength(5);
	TSockAddr addr;
	socket.SendTo(buf, addr, KSockWriteUrgent, stat);
	User::WaitForRequest(stat);
	Logger().WriteFormat(_L("SendTo status %S"), &EpocErrorToText(stat.Int()));
	TESTL(KErrNotSupported == stat.Int());
	
	CleanupStack::Pop(&socket);
	socket.Close();
	
	CleanupStack::Pop(&ss);
	ss.Close();
	
	SetTestStepResult(verdict);
	return verdict;
	}

// Test step 9.4
const TDesC& CSocketTest9_4::GetTestName()
	{
	_LIT(ret,"Test9.4");
	return ret;
	}

enum TVerdict CSocketTest9_4::InternalDoTestStepL( void )
	{
	TVerdict verdict = EPass;
	
	Logger().WriteFormat(_L("Test Purpose: Trying to SetOpt/GetOpt socket buffer lengths"));
	
	// connect to esock
	Logger().WriteFormat(_L("Attempting to connect to socket server"));
    RSocketServ ss;
	TInt ret = OptimalConnect(ss);
	CleanupClosePushL(ss);
	Logger().WriteFormat(_L("Connect returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);
	
	Logger().WriteFormat(_L("Finding for dummy protocol 1"));
	TProtocolDesc protoInfo;
	ret = ss.FindProtocol(_L("Dummy Protocol 1"), protoInfo);
	Logger().WriteFormat(_L("FindProtocol returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);
	
	Logger().WriteFormat(_L("Opening a socket"));
	RSocket socket;
	ret = socket.Open(ss, protoInfo.iAddrFamily, protoInfo.iSockType, protoInfo.iProtocol);
	CleanupClosePushL(socket);
	Logger().WriteFormat(_L("Open returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);
	
	Logger().WriteFormat(_L("Getting RecvBuf length"));
    TPckgBuf<TUint> lenbuf;
	ret = socket.GetOpt(KSORecvBuf, KSOLSocket, lenbuf);
	Logger().WriteFormat(_L("GetOpt returned %S, length %d"), 
		&EpocErrorToText(ret), lenbuf());
	TESTL(KErrNone == ret);
	
	Logger().WriteFormat(_L("Getting SendBuf length"));
	ret = socket.GetOpt(KSOSendBuf, KSOLSocket, lenbuf);
	Logger().WriteFormat(_L("GetOpt returned %S, length %d"), 
		&EpocErrorToText(ret), lenbuf());
	TESTL(KErrNone == ret);
	
    TUint x = lenbuf()/2;
    lenbuf() = x;
	Logger().WriteFormat(_L("Setting RecvBuf length to %d"), lenbuf());
	ret = socket.SetOpt(KSORecvBuf, KSOLSocket, lenbuf);
	Logger().WriteFormat(_L("SetOpt returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);
	
	Logger().WriteFormat(_L("Getting RecvBuf length"));
	ret = socket.GetOpt(KSORecvBuf, KSOLSocket, lenbuf);
	Logger().WriteFormat(_L("GetOpt returned %S, length %d"), 
		&EpocErrorToText(ret), lenbuf());
	TESTL(KErrNone == ret);
	TESTL(lenbuf() == x);
	
    lenbuf() = x;
	Logger().WriteFormat(_L("Setting SendBuf length to %d"), lenbuf());
	ret = socket.SetOpt(KSOSendBuf, KSOLSocket, lenbuf);
	Logger().WriteFormat(_L("SetOpt returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);
	
	Logger().WriteFormat(_L("Getting SendBuf length"));
	ret = socket.GetOpt(KSOSendBuf, KSOLSocket, lenbuf);
	Logger().WriteFormat(_L("GetOpt returned %S, length %d"), &EpocErrorToText(ret), lenbuf());
	TESTL(KErrNone == ret);
	TESTL(lenbuf() == x);
	
	CleanupStack::Pop(&socket);
	socket.Close();
	
	CleanupStack::Pop(&ss);
	ss.Close();
	
	SetTestStepResult(verdict);
	return verdict;
	}

// Test step 9.5
const TDesC& CSocketTest9_5::GetTestName()
	{
	_LIT(ret,"Test9.5");
	return ret;
	}

enum TVerdict CSocketTest9_5::InternalDoTestStepL( void )
	{
	TVerdict verdict = EPass;
	
	Logger().WriteFormat(_L("Test Purpose: Write to Unconnected Socket"));
	
	// connect to esock
	Logger().WriteFormat(_L("Attempting to connect to socket server"));
    RSocketServ ss;
	TInt ret = OptimalConnect(ss);
	CleanupClosePushL(ss);
	Logger().WriteFormat(_L("Connect returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);
	
	// Write on an unconnected sock to send datagram
	Logger().WriteFormat(_L("Opening socket on dummy protocol 3"));
	TRequestStatus rstat;
	RSocket sock3;                                                    
	ret = sock3.Open(ss,_L("Dummy Protocol 3"));                         
	Logger().WriteFormat(_L("Open returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);
	
	Logger().WriteFormat(_L("Attempting Write"));
	sock3.Write(_L8("Hello HELLO "), rstat);
	User::WaitForRequest(rstat);
	Logger().WriteFormat(_L("Write status %S"), &EpocErrorToText(rstat.Int()));
	TESTL(KErrNotReady == rstat.Int());
	sock3.Close(); 
	
	// try for dummy protocol 2
	Logger().WriteFormat(_L("Opening socket on dummy protocol 2"));
	RSocket sk;
	ret = sk.Open(ss,_L("Dummy Protocol 2"));
	CleanupClosePushL(sk);
	Logger().WriteFormat(_L("Open returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);
	
	Logger().WriteFormat(_L("Attempting Write"));
	sk.Write(_L8("Hello HELLO "), rstat);
	User::WaitForRequest(rstat);
	Logger().WriteFormat(_L("Write status %S"), &EpocErrorToText(rstat.Int()));
	TESTL(KErrNotReady == rstat.Int());
	
	CleanupStack::Pop(&sk);
	sk.Close(); 
	CleanupStack::Pop(&ss);
	ss.Close();
	
	SetTestStepResult(verdict);	
	return verdict;
	}

// Test step 9.6
const TDesC& CSocketTest9_6::GetTestName()
	{
	_LIT(ret,"Test9.6");
	return ret;
	}

enum TVerdict CSocketTest9_6::InternalDoTestStepL( void )
	{
	TVerdict verdict = EPass;

	Logger().WriteFormat(_L("Test Purpose: Trying to set a protocol to have nolimit on datagram size and send over 64K datagram."));

	// connect to esock
	Logger().WriteFormat(_L("Attempting to connect to socket server"));
	RSocketServ ss;
    
	TSessionPref pref;
	pref.iAddrFamily = KDummyAddrFamily;
	pref.iProtocol = KDummyFive;
	TInt ret = ss.Connect(pref);

//	TInt ret = OptimalConnect(ss);
	CleanupClosePushL(ss);
	Logger().WriteFormat(_L("Connect returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);

	Logger().WriteFormat(_L("Finding for dummy protocol 5"));
	TProtocolDesc protoInfo;
	ret = ss.FindProtocol(_L("Dummy Protocol 5"), protoInfo);
	Logger().WriteFormat(_L("FindProtocol returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);

	Logger().WriteFormat(_L("Opening a socket"));
	RSocket socket;
	ret = socket.Open(ss, protoInfo.iAddrFamily, protoInfo.iSockType, protoInfo.iProtocol);
	CleanupClosePushL(socket);
	Logger().WriteFormat(_L("Open returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);

	TPckgBuf<TUint> lenbuf;
	Logger().WriteFormat(_L("Getting SendBuf length"));
	ret = socket.GetOpt(KSOSendBuf, KSOLSocket, lenbuf);
	Logger().WriteFormat(_L("GetOpt returned %S, length %d"), 
		&EpocErrorToText(ret), lenbuf());
	TESTL(KErrNone == ret);

	TUint x(70000);
	lenbuf() = x;
	Logger().WriteFormat(_L("Setting SendBuf length to %d"), lenbuf());
	ret = socket.SetOpt(KSOSendBuf, KSOLSocket, lenbuf);
	Logger().WriteFormat(_L("SetOpt returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);

	Logger().WriteFormat(_L("Getting SendBuf length"));
	ret = socket.GetOpt(KSOSendBuf, KSOLSocket, lenbuf);
	Logger().WriteFormat(_L("GetOpt returned %S, length %d"), &EpocErrorToText(ret), lenbuf());
	TESTL(KErrNone == ret);
	TESTL(lenbuf() == x);

	Logger().WriteFormat(_L("Try sending datagram over protocol allowing unlimited datagram size"));
	TInetAddr      destAddr;
	TRequestStatus status;

	const TInt KBufferLength = 70000;
	HBufC8* hbuf = HBufC8::NewMaxLC(KBufferLength);
	TPtr8 buffer(hbuf->Des());
	buffer.FillZ();

	socket.SendTo(buffer, destAddr, 0, status);
	User::WaitForRequest(status);
	Logger().WriteFormat(_L("...which returned %d\n"), status.Int());
	TESTL(status == KErrNone);

	CleanupStack::PopAndDestroy(hbuf);

	CleanupStack::Pop(&socket);
	socket.Close();

	CleanupStack::Pop(&ss);
	ss.Close();

	SetTestStepResult(verdict);
	return verdict;
	}

// Test step 9.7
const TDesC& CSocketTest9_7::GetTestName()
	{
	_LIT(ret,"Test9.7");
	return ret;
	}

enum TVerdict CSocketTest9_7::InternalDoTestStepL( void )
	{
	TVerdict verdict = EPass;

	Logger().WriteFormat(_L("Test Purpose: Repeat send over 64K datagram using TSockXfrLength."));

	// connect to esock
	Logger().WriteFormat(_L("Attempting to connect to socket server"));
	RSocketServ ss;
    
	// protocol allowing unlimited datagram size
	TSessionPref pref;
	pref.iAddrFamily = KDummyAddrFamily;
	pref.iProtocol = KDummyFive;
	TInt ret = ss.Connect(pref);

	CleanupClosePushL(ss);
	Logger().WriteFormat(_L("Connect returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);

	Logger().WriteFormat(_L("Finding for dummy protocol 5"));
	TProtocolDesc protoInfo;
	ret = ss.FindProtocol(_L("Dummy Protocol 5"), protoInfo);
	Logger().WriteFormat(_L("FindProtocol returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);

	Logger().WriteFormat(_L("Opening a socket"));
	RSocket socket;
	ret = socket.Open(ss, protoInfo.iAddrFamily, protoInfo.iSockType, protoInfo.iProtocol);
	CleanupClosePushL(socket);
	Logger().WriteFormat(_L("Open returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);

	TPckgBuf<TUint> lenbuf;
	Logger().WriteFormat(_L("Getting SendBuf length"));
	ret = socket.GetOpt(KSOSendBuf, KSOLSocket, lenbuf);
	Logger().WriteFormat(_L("GetOpt returned %S, length %d"), 
		&EpocErrorToText(ret), lenbuf());
	TESTL(KErrNone == ret);

	TUint x(70000);
	lenbuf() = x;
	Logger().WriteFormat(_L("Setting SendBuf length to %d"), lenbuf());
	ret = socket.SetOpt(KSOSendBuf, KSOLSocket, lenbuf);
	Logger().WriteFormat(_L("SetOpt returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);

	Logger().WriteFormat(_L("Getting SendBuf length"));
	ret = socket.GetOpt(KSOSendBuf, KSOLSocket, lenbuf);
	Logger().WriteFormat(_L("GetOpt returned %S, length %d"), &EpocErrorToText(ret), lenbuf());
	TESTL(KErrNone == ret);
	TESTL(lenbuf() == x);

	Logger().WriteFormat(_L("Try sending datagram over protocol allowing unlimited datagram size"));
	TInetAddr      destAddr;
	TRequestStatus status;

	const TInt KBufferLength = 70000;
	HBufC8* hbuf = HBufC8::NewMaxLC(KBufferLength);
	TPtr8 buffer(hbuf->Des());
	buffer.FillZ();

	TSockXfrLength length;
	socket.SendTo(buffer, destAddr, 0, status, length);
	User::WaitForRequest(status);
	Logger().WriteFormat(_L("Status of Send() is %S %d"), &EpocErrorToText(status.Int()), length());
	TESTL(status == KErrNone);
	TESTL(KBufferLength == length());

	CleanupStack::PopAndDestroy(hbuf);

	CleanupStack::Pop(&socket);
	socket.Close();

	CleanupStack::Pop(&ss);
	ss.Close();

	SetTestStepResult(verdict);
	return verdict;
	}

// Test step 9.8
const TDesC& CSocketTest9_8::GetTestName()
    {
    _LIT(ret,"Test9.8");
    return ret;
    }

enum TVerdict CSocketTest9_8::InternalDoTestStepL( void )
    {
    TVerdict verdict = EPass;
    
    Logger().WriteFormat(_L("Test Purpose: SendTo using TSockXfrLength"));

    // connect to esock
    Logger().WriteFormat(_L("Attempting to connect to socket server"));
    RSocketServ ss;
    TSessionPref pref;
    pref.iAddrFamily = KDummyAddrFamily;
    pref.iProtocol = KDummyOne;
    TInt ret = ss.Connect(pref);

    CleanupClosePushL(ss);
    Logger().WriteFormat(_L("Connect returned %S"), &EpocErrorToText(ret));
    TESTL(KErrNone == ret);
    
    Logger().WriteFormat(_L("Finding for dummy protocol 1"));
    TProtocolDesc protoInfo;
    ret = ss.FindProtocol(_L("Dummy Protocol 1"), protoInfo);
    Logger().WriteFormat(_L("FindProtocol returned %S"), &EpocErrorToText(ret));
    TESTL(KErrNone == ret);
    
    Logger().WriteFormat(_L("Opening a socket"));
    RSocket socket;
    ret = socket.Open(ss, protoInfo.iAddrFamily, protoInfo.iSockType, protoInfo.iProtocol);
    CleanupClosePushL(socket);
    Logger().WriteFormat(_L("Open returned %S"), &EpocErrorToText(ret));
    TESTL(KErrNone == ret);
 

    // Force the SendTo to 'flow off'
    TBool flowOff = ETrue;
    ret = socket.SetOpt(KDummyOptionSetFlowOffWrite, 0, flowOff);
    Logger().WriteFormat(_L("SetOpt (flow off) returned %S"), &EpocErrorToText(ret));
    TESTL(KErrNone == ret);
    
    // data to send
    TBuf8<10> pNameBuf;
     _LIT( Kmessage, "helloworld" );
    pNameBuf.Copy( Kmessage); 
    
    Logger().WriteFormat(_L("Try sending datagram"));
    TInetAddr destAddr;
    TRequestStatus status;
    // induce the 'flow off' state - should never complete
    TSockXfrLength len;
    socket.SendTo(pNameBuf, destAddr, 0, status, len);
    
    // Now enable the write to complete, by ending the 'flow off'
    flowOff = EFalse;
    ret = socket.SetOpt(KDummyOptionSetFlowOffWrite, 0, flowOff);
    TESTL(KErrNone == ret);

    // check the length returned by the send is OK
    User::WaitForRequest(status);
    Logger().WriteFormat(_L("...SendTo returned %d %d\n"), status.Int(),len());
    TESTL(status == KErrNone);
    TESTL(len() == pNameBuf.Length());
    
    CleanupStack::Pop(&socket);
    socket.Close();
    
    CleanupStack::Pop(&ss);
    ss.Close();
    
    SetTestStepResult(verdict);
    return verdict;
    }

// Test step 9.9
//
// Test ESoRecvOneOrMoreNoLength IPC
//
const TDesC& CSocketTest9_9::GetTestName()
    {
    _LIT(ret,"Test9.9");
    return ret;
    }

enum TVerdict CSocketTest9_9::InternalDoTestStepL( void )
    {
    TVerdict verdict = EPass;
    
    Logger().WriteFormat(_L("Test Purpose: Test RecvOneOrMore (no length) API"));
    
    // connect to esock
    Logger().WriteFormat(_L("Attempting to connect to socket server"));
    RSocketServ ss;
    TInt ret = OptimalConnect(ss);
    CleanupClosePushL(ss);
    Logger().WriteFormat(_L("Connect returned %S"), &EpocErrorToText(ret));
    TESTL(KErrNone == ret);
    
    RSocket sk;
    Logger().WriteFormat(_L("Opening a socket on Dummy protocol 2"));
    ret = sk.Open(ss, _L("Dummy Protocol 2"));
    CleanupClosePushL(sk);
    Logger().WriteFormat(_L("Open returned %S"), &EpocErrorToText(ret));
    TESTL(KErrNone == ret);
    
    Logger().WriteFormat(_L("Connecting socket"));
    TSockAddr addr;
    TRequestStatus rstat;
    sk.Connect(addr, rstat);
    User::WaitForRequest(rstat);
    Logger().WriteFormat(_L("Connect status %S"), &EpocErrorToText(rstat.Int()));
    TESTL(KErrNone == rstat.Int());
    
    Logger().WriteFormat(_L("Trying RecvOneOrMore (no length)"));
    TBuf8<10> buf;
    sk.RecvOneOrMore(buf, 0, rstat);
    TRequestStatus sstat;
    _LIT8(KTestString, "Hello");
    sk.Send(KTestString(), 0, sstat);
    User::WaitForRequest(rstat);
    Logger().WriteFormat(_L("RecvOneOrMore status %S"), &EpocErrorToText(rstat.Int()));
    TESTL(KErrNone == rstat.Int());
    User::WaitForRequest(sstat);
    Logger().WriteFormat(_L("Send status %S"), &EpocErrorToText(sstat.Int()));
    TESTL(KErrNone == sstat.Int());
    TESTL(buf == KTestString());
    
    CleanupStack::Pop(&sk);
    sk.Close();
    CleanupStack::Pop(&ss);
    ss.Close();
    
    SetTestStepResult(verdict);
    return verdict;
    }

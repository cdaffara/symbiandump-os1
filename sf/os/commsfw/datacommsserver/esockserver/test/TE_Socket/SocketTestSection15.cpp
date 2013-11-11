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
// This contains ESock Test cases from section 15
// 
//

#include <e32base.h>
#include "SocketTestSection15.h"
#include <ss_std.h>
#include "ES_DUMMY.H"
#include <elements/sd_std.h>

_LIT8(KSendString, "ABCDEFGHIJKLMNOPQRSTUVWXYZ"); // Sample testdata to send in these tests
const TInt txlength1 = 26; // Length of KSEndString Its useful to be const

_LIT8(KSendString2, "0123456789");                // Sample testdata to send in these tests
const TInt txlength2 = 10; // Length of KSEndString2 Its useful to be const

const TInt segmentlength = 10; // Amount of data to receive at a time. The number choosen must
// allow for three reads of the 26-octet testdata where the last
// read doesnt is less than this. 10 fulfills there requirements.

// Test step 15.0
const TDesC& CSocketTest15_0::GetTestName()
	{
	_LIT(ret,"Test15.0");
	return ret;
	}

enum TVerdict CSocketTest15_0::InternalDoTestStepL( void )
	{
	TVerdict verdict = EPass;
	
	Logger().WriteFormat(_L("Test Purpose: Panic Client using a Bad Descriptor"));
	
	// stop debugger crashing
	TBool oldJit = User::JustInTime();
	User::SetJustInTime(EFalse);
	
	Logger().WriteFormat(_L("Creating thread which uses a bad descriptor"));
	RThread t;
	TSocketThreadArg tArg;
	tArg.iHandle = this;
	tArg.iSem  = NULL;
	tArg.iNumSockets = 0;
	TInt ret = t.Create(_L("BadDescriptor"), BadDescriptorThread, KDefaultStackSize, 
		KDefaultHeapSize, KDefaultHeapSize, &tArg);
	CleanupClosePushL(t);
	Logger().WriteFormat(_L("Create returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);
	
	Logger().WriteFormat(_L("Logging on and resuming thread"));
	TRequestStatus stat;
	t.Logon(stat);
	t.Resume();
	User::WaitForRequest(stat);
	TPtrC catName = t.ExitCategory();
	Logger().WriteFormat(_L("Thread Exit Category '%S', Reason %d, Type %d"), 
		&catName, t.ExitReason(), t.ExitType());
	TESTL(t.ExitCategory() == Den::KWorkerClientPanic);
	TESTL(Den::ECommonBadDescriptorLength == t.ExitReason());  

	TESTL(EExitPanic == t.ExitType());
	CleanupStack::PopAndDestroy(&t);
	
	// reset JIT state
	User::SetJustInTime(oldJit);
	
	SetTestStepResult(verdict);
	return verdict;
	
	}

// Test step 15.1
const TDesC& CSocketTest15_1::GetTestName()
	{
	_LIT(ret,"Test15.1");
	return ret;
	}


enum TVerdict CSocketTest15_1::InternalDoTestStepL( void )
	{
	Logger().WriteFormat(_L("Test Purpose: Datagram Continuation test. The trivial case"));
	Logger().WriteFormat(_L("Will send the 26 letters and retrieve them in portions of 10, 10 and the rest in a 10 octet buffer"));

	// <Test setup>
	
	Logger().WriteFormat(_L("Open socket server"));
	TAutoClose<RSocketServ> server;
	TInt ret=server.iObj.Connect();
	Logger().WriteFormat(_L("Connect returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);
	server.PushL();
	
	TAutoClose<RSocket> socket;
	Logger().WriteFormat(_L("Opening a socket on %S"), &KDummyOneName);
	ret = socket.iObj.Open(server.iObj, KDummyOneName);
	Logger().WriteFormat(_L("Open returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);
	socket.PushL();
	
	Logger().WriteFormat(_L("Connecting socket"));
	TSockAddr addr;
	TRequestStatus status;
	socket.iObj.Connect(addr, status);
	User::WaitForRequest(status);
	Logger().WriteFormat(_L("Connect status %S"), &EpocErrorToText(status.Int()));
	TESTL(KErrNone == status.Int());

	// <Main test body>
	
	// Setup the txdata
	TBuf<txlength1> logstr;
	logstr.Copy(KSendString);
	Logger().WriteFormat(_L("Sending 8bit data: \"%S\", %d octets"), &logstr, txlength1);
	
	// Send the data out
	TSockXfrLength length;
	socket.iObj.Send(KSendString, 0, status, length);
	User::WaitForRequest(status);
	Logger().WriteFormat(_L("Send status is %S"), &EpocErrorToText(status.Int()));
	TESTL(KErrNone == status.Int());
	TESTL(txlength1 == length());
	
	// Receive setup
	TSockXfrLength rxlength;
	TBuf8<segmentlength> rxbuf;
	TBuf<txlength1> expected; // Contain stuff to compare received to
	expected.Copy(KSendString);
	TInt remaining = txlength1;
	TInt recvFlags = 0;
	
	// Receive loop
	while (remaining > 0)
		{
		TInt minlen = Min(segmentlength, remaining);
		// show how much we expect to receive
		Logger().WriteFormat(_L("Recv next %d octets of expected %d remaining"), minlen, remaining);

		// receive up to segmentlength octets
		socket.iObj.Recv(rxbuf, recvFlags, status, rxlength);
		User::WaitForRequest(status);

		// display what we got and what we expected
		logstr.Copy(rxbuf);
		expected.SetLength(minlen);
		Logger().WriteFormat(_L("Recv status %S, expected length remaining %d, length remaining %d, buffer '%S', expected '%S'"), 
			&EpocErrorToText(status.Int()), remaining - minlen, rxlength(), &logstr, &expected);
		TESTL(KErrNone == status.Int());
		TESTL(expected==logstr);
		recvFlags = KSockReadContinuation;

		// update comparison string
		expected.SetLength(remaining);
		remaining -= minlen;
		expected.Delete(0, segmentlength);
		TESTL(remaining==rxlength());
		}
	// <End of life as we know it>
	socket.Pop(); // Autoclose object
	server.Pop(); // Autoclose object
	SetTestStepResult(EPass);
	return EPass;
	}
	
// Test step 15.2
const TDesC& CSocketTest15_2::GetTestName()
	{
	_LIT(ret,"Test15.2");
	return ret;
	}

enum TVerdict CSocketTest15_2::InternalDoTestStepL( void )
	{
	Logger().WriteFormat(_L("Test Purpose: Datagram Continuation test."));
	Logger().WriteFormat(_L("Will send a 26 octet dgram, then send a 10 octet dgram, retrieve the first 10, then continue to retrieve next datagram instead."));

	// <Test setup>
	
	Logger().WriteFormat(_L("Open socket server"));
	TAutoClose<RSocketServ> server;
	TInt ret=server.iObj.Connect();
	Logger().WriteFormat(_L("Connect returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);
	server.PushL();
	
	TAutoClose<RSocket> socket;
	Logger().WriteFormat(_L("Opening a socket on %S"), &KDummyOneName);
	ret = socket.iObj.Open(server.iObj, KDummyOneName);
	Logger().WriteFormat(_L("Open returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);
	socket.PushL();
	
	Logger().WriteFormat(_L("Connecting socket"));
	TSockAddr addr;
	TRequestStatus status;
	socket.iObj.Connect(addr, status);
	User::WaitForRequest(status);
	Logger().WriteFormat(_L("Connect status %S"), &EpocErrorToText(status.Int()));
	TESTL(KErrNone == status.Int());
	
	// <Main test body>
	
	// show what we are going to send in the first string
	TBuf<txlength1> logstr;
	logstr.Copy(KSendString);
	Logger().WriteFormat(_L("Sending 8bit data: \"%S\", %d octets"), &logstr, txlength1);
	
	// send the first string
	TSockXfrLength length;
	socket.iObj.Send(KSendString, 0, status, length);
	User::WaitForRequest(status);
	Logger().WriteFormat(_L("Status of Send() is %S"), &EpocErrorToText(status.Int()));
	TESTL(KErrNone == status.Int());
	TESTL(txlength1 == length());
	
	// show what we are going to send in the second string
	logstr.Copy(KSendString2);
	Logger().WriteFormat(_L("Sending 8bit data: \"%S\", %d octets"), &logstr, txlength2);

	// send the second string
	socket.iObj.Send(KSendString2, 0, status, length);
	User::WaitForRequest(status);
	Logger().WriteFormat(_L("Status of Send() is %S"), &EpocErrorToText(status.Int()));
	TESTL(KErrNone == status.Int());
	TESTL(txlength2 == length());

	// show how much we expect to receive
	TInt remaining = txlength1;
	Logger().WriteFormat(_L("Recv max %d octets of expected %d remaining"), segmentlength, remaining);

	// receive setup
	TSockXfrLength rxlength;
	TBuf8<segmentlength> rxbuf;
	TBuf<txlength1> expected; // Contain stuff to compare received to
	
	// receive the first 10 octets
	socket.iObj.Recv(rxbuf, 0, status, rxlength);
	remaining-=segmentlength;
	User::WaitForRequest(status);
	
	// display what we got and what we expected
	logstr.Copy(rxbuf);
	expected.Copy(KSendString);
	expected.SetLength(segmentlength);
	Logger().WriteFormat(_L("Recv status %S, expected length remaining %d, length remaining %d, buffer '%S', expected '%S'"), 
		&EpocErrorToText(status.Int()), remaining, rxlength(), &logstr, &expected);
	TESTL(KErrNone == status.Int());
	TESTL(remaining==rxlength());
	TESTL(expected==logstr);
	
	Logger().WriteFormat(_L("Ignore rest of datagram and continue to retrieve next instead."));
	
	// show how much we expect to receive
	remaining = txlength2;
	Logger().WriteFormat(_L("Recv max %d octets of expected %d remaining"), segmentlength, remaining);
	
	// receive the first 10 octets of next datagram
	socket.iObj.Recv(rxbuf, 0, status, rxlength);
	remaining-=segmentlength;
	User::WaitForRequest(status);
	
	// display what we got and what we expected
	logstr.Copy(rxbuf);
	expected.Copy(KSendString2);
	Logger().WriteFormat(_L("Recv status %S, expected length remaining %d, length remaining %d, buffer '%S', expected '%S'"), 
		&EpocErrorToText(status.Int()), remaining, rxlength(), &logstr, &expected);
	TESTL(KErrNone == status.Int());
	TESTL(remaining==rxlength());
	TESTL(expected==logstr);
	
	// <End of life as we know it>
	socket.Pop(); // Autoclose object
	server.Pop(); // Autoclose object
	SetTestStepResult(EPass);
	return EPass;
	}

// Test step 15.3
const TDesC& CSocketTest15_3::GetTestName()
	{
	_LIT(ret,"Test15.3");
	return ret;
	}

enum TVerdict CSocketTest15_3::InternalDoTestStepL( void )
	{
	Logger().WriteFormat(_L("Test Purpose: Datagram Continuation test."));
	Logger().WriteFormat(_L("Retrieve async 10 octets from a dgram, send a 26 octet dgram, retrieve async next dgram, wait a bit to make sure it doesnt complete without data, then send 10octets for the second recv."));

	// <Test setup>

	Logger().WriteFormat(_L("Open socket server"));
	TAutoClose<RSocketServ> server;
	TInt ret=server.iObj.Connect();
	Logger().WriteFormat(_L("Connect returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);
	server.PushL();
	
	TAutoClose<RSocket> socket;
	Logger().WriteFormat(_L("Opening a socket on %S"), &KDummyOneName);
	ret = socket.iObj.Open(server.iObj, KDummyOneName);
	Logger().WriteFormat(_L("Open returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);
	socket.PushL();
	
	Logger().WriteFormat(_L("Connecting socket"));
	TSockAddr addr;
	TRequestStatus status;
	socket.iObj.Connect(addr, status);
	User::WaitForRequest(status);
	Logger().WriteFormat(_L("Connect status %S"), &EpocErrorToText(status.Int()));
	TESTL(KErrNone == status.Int());

	// <Main test body>
	
	TSockXfrLength rxlength;
	TInt remaining = txlength1;
	TBuf8<segmentlength> rxbuf;
	TBuf<txlength1> expected; // Contain stuff to compare received to
	
	// receive the first 10 octets (which shouldn't block or complete)
	Logger().WriteFormat(_L("Recv max %d octets of expected %d remaining"), segmentlength, remaining);
	socket.iObj.Recv(rxbuf, 0, status, rxlength);
	TESTL(KRequestPending == status.Int());
	remaining-=segmentlength; 
	
	// show what we are going to send
	TBuf<txlength1> logstr;
	logstr.Copy(KSendString);
	Logger().WriteFormat(_L("Sending 8bit data: \"%S\", %d octets"), &logstr, txlength1);
		
	// send the first string
	TRequestStatus txstatus;
	TSockXfrLength length;
	socket.iObj.Send(KSendString, 0, txstatus, length);
	
	User::WaitForRequest(txstatus); // wait for tx to complete
	Logger().WriteFormat(_L("Status of Send() is %S"), &EpocErrorToText(txstatus.Int()));
	// test that send was a success
	TESTL(KErrNone == txstatus.Int());
	TESTL(txlength1 == length());

	User::WaitForRequest(status);   // wait for rx to complete
	Logger().WriteFormat(_L("Status of Recv() is %S"), &EpocErrorToText(status.Int()));
	// test that the receive was a success
	logstr.Copy(rxbuf);
	expected.Copy(KSendString);
	expected.SetLength(segmentlength);
	Logger().WriteFormat(_L("Recv status %S, expected length remaining %d, length remaining %d, buffer '%S', expected '%S'"), 
		&EpocErrorToText(status.Int()), remaining, rxlength(), &logstr, &expected);
	TESTL(KErrNone == status.Int());
	TESTL(remaining==rxlength());
	TESTL(expected==logstr);
	
	// receive the first 10 octets from the next datagram (which shouldn't block or complete)
	Logger().WriteFormat(_L("Recv %d octets"), txlength2);
	socket.iObj.Recv(rxbuf, 0, status, rxlength);
	TESTL(KRequestPending == status.Int());
	remaining=0;
	
	// show what we are going to send in the second string
	logstr.Copy(KSendString2);
	Logger().WriteFormat(_L("Sending 8bit data: \"%S\", %d octets"), &logstr, txlength2);

	// send the second string
	socket.iObj.Send(KSendString2, 0, txstatus, length);
	
	User::WaitForRequest(txstatus); // wait for tx to complete
	Logger().WriteFormat(_L("Status of Send() is %S"), &EpocErrorToText(txstatus.Int()));
	// test that send was a success
	TESTL(KErrNone == txstatus.Int());
	TESTL(txlength2 == length());

	User::WaitForRequest(status);   // wait for rx to complete
	Logger().WriteFormat(_L("Status of Recv() is %S"), &EpocErrorToText(status.Int()));
	// Test that receive was a success
	logstr.Copy(rxbuf);
	expected.Copy(KSendString2);
	Logger().WriteFormat(_L("Recv status %S, expected length remaining %d, length remaining %d, buffer '%S', expected '%S'"), 
		&EpocErrorToText(status.Int()), remaining, rxlength(), &logstr, &expected);
	TESTL(KErrNone == status.Int());
	TESTL(remaining==rxlength());
	TESTL(expected==logstr);
	
	// <End of life as we know it>
	socket.Pop(); // Autoclose object
	server.Pop(); // Autoclose object
	SetTestStepResult(EPass);
	return EPass;
	}
	
// Test step 15.4
const TDesC& CSocketTest15_4::GetTestName()
	{
	_LIT(ret,"Test15.4");
	return ret;
	}

enum TVerdict CSocketTest15_4::InternalDoTestStepL( void )
	{
	Logger().WriteFormat(_L("Test Purpose: Datagram Continuation test. The trivial case"));
	Logger().WriteFormat(_L("Will send the 26 letters and retrieve them in portions of 10, 10 and the rest in a 10 octet buffer"));

	// <Test setup>

	Logger().WriteFormat(_L("Open socket server"));
	TAutoClose<RSocketServ> server;
	TInt ret=server.iObj.Connect();
	Logger().WriteFormat(_L("Connect returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);
	server.PushL();
	
	TAutoClose<RSocket> socket;
	Logger().WriteFormat(_L("Opening a socket on %S"), &KDummyOneName);
	ret = socket.iObj.Open(server.iObj, KDummyOneName);
	Logger().WriteFormat(_L("Open returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);
	socket.PushL();
	
	Logger().WriteFormat(_L("Connecting socket"));
	TSockAddr addr;
	TRequestStatus status;
	socket.iObj.Connect(addr, status);
	User::WaitForRequest(status);
	Logger().WriteFormat(_L("Connect status %S"), &EpocErrorToText(status.Int()));
	TESTL(KErrNone == status.Int());

	// <Main test body>

	// Setup the txdata
	TBuf<txlength1> logstr;
	logstr.Copy(KSendString);
	Logger().WriteFormat(_L("Sending 8bit data: \"%S\", %d octets"), &logstr, txlength1);
	
	// Send the data out
	TSockXfrLength length;
	socket.iObj.Send(KSendString, 0, status, length);
	User::WaitForRequest(status);
	Logger().WriteFormat(_L("Send status is %S"), &EpocErrorToText(status.Int()));
	TESTL(KErrNone == status.Int());
	TESTL(txlength1 == length());
	
	// Receive setup
	TSockXfrLength rxlength;
	TBuf8<segmentlength> rxbuf;
	TBuf<txlength1> expected; // Contain stuff to compare received to
	expected.Copy(KSendString);
	TInt remaining = txlength1;
	TInt recvFlags = KSockReadPeek;
	
	// Receive loop
	while (remaining > 0)
		{
		TInt minlen = Min(segmentlength, remaining);
		// show how much we expect to peek
		Logger().WriteFormat(_L("PEEK Recv next %d octets of expected %d remaining"), minlen, remaining);

		// receive up to segmentlength octets
		socket.iObj.Recv(rxbuf, recvFlags, status, rxlength);
		User::WaitForRequest(status);

		// display what we got and what we expected
		logstr.Copy(rxbuf);
		expected.SetLength(minlen);
		Logger().WriteFormat(_L("PEEK Recv status %S, expected length remaining %d, length remaining %d, buffer '%S', expected '%S'"), 
			&EpocErrorToText(status.Int()), remaining, rxlength(), &logstr, &expected);
		TESTL(KErrNone == status.Int());
		TESTL(expected==logstr);
		TESTL(remaining==rxlength());
		recvFlags = KSockReadContinuation;

		// show how much we expect to receive
		Logger().WriteFormat(_L("Recv next %d octets of expected %d remaining"), minlen, remaining);

		// receive up to segmentlength octets
		socket.iObj.Recv(rxbuf, recvFlags, status, rxlength);
		User::WaitForRequest(status);

		// display what we got and what we expected
		logstr.Copy(rxbuf);
		expected.SetLength(minlen);
		Logger().WriteFormat(_L("Recv status %S, expected length remaining %d, length remaining %d, buffer '%S', expected '%S'"), 
			&EpocErrorToText(status.Int()), remaining - minlen, rxlength(), &logstr, &expected);
		TESTL(KErrNone == status.Int());
		TESTL(expected==logstr);
		recvFlags = KSockReadContinuation | KSockReadPeek;

		// update comparison string
		expected.SetLength(remaining);
		remaining -= minlen;
		expected.Delete(0, segmentlength);
		TESTL(remaining==rxlength());
		}

	// <End of life as we know it>
	socket.Pop(); // Autoclose object
	server.Pop(); // Autoclose object
	SetTestStepResult(EPass);
	return EPass;
	}
	
// Test step 15.4
const TDesC& CSocketTest15_5::GetTestName()
	{
	_LIT(ret,"Test15.5");
	return ret;
	}

enum TVerdict CSocketTest15_5::InternalDoTestStepL( void )
	{
	_LIT8(KSendData, "ABC"); // Sample testdata to send in this test
	
	Logger().WriteFormat(_L("Test Purpose: Datagram Continuation test."));
	Logger().WriteFormat(_L("Perform multiple reads from each individual packet received (non-connected socket)."));

	// <Test setup>

	Logger().WriteFormat(_L("Open socket server"));
	TAutoClose<RSocketServ> server;
	TInt ret=server.iObj.Connect();
	Logger().WriteFormat(_L("Connect returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);
	server.PushL();
	
	TAutoClose<RSocket> socket;
	Logger().WriteFormat(_L("Opening a socket on %S"), &KDummyOneName);
	ret = socket.iObj.Open(server.iObj, KDummyOneName);
	Logger().WriteFormat(_L("Open returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);
	socket.PushL();
	
	Logger().WriteFormat(_L("Connecting socket"));
	TSockAddr addr;
	TRequestStatus status;
	socket.iObj.Connect(addr, status);
	User::WaitForRequest(status);
	Logger().WriteFormat(_L("Connect status %S"), &EpocErrorToText(status.Int()));
	TESTL(KErrNone == status.Int());

	// <Main test body>
	
	TUint count;
	for (count=0; count<5; count++) // perform the test a few times
		{
		// send the data
		TSockXfrLength txlength;
		socket.iObj.Send(KSendData, 0, status, txlength);
		User::WaitForRequest(status);
		TESTL(KErrNone == status.Int());
	
		TBuf8<1> rxbuf;
		TBuf<1> buf16;
		TSockXfrLength rxLength = 0;
	
		// receive it back (using the KSockReadContinuation option)
		socket.iObj.Recv(rxbuf, 0, status, rxLength);
		User::WaitForRequest(status);
		buf16.Copy(rxbuf);
		Logger().WriteFormat(_L("Receive status is %S, data received is '%S'"), &EpocErrorToText(status.Int()), &buf16);
		TESTL(KErrNone == status.Int());
		TESTL(rxLength() == 2);

		// receive more of it back (using the KSockReadContinuation option)
		socket.iObj.Recv(rxbuf, KSockReadContinuation, status, rxLength);
		User::WaitForRequest(status);
		buf16.Copy(rxbuf);
		Logger().WriteFormat(_L("Receive status is %S, data received is '%S'"), &EpocErrorToText(status.Int()), &buf16);
		TESTL(KErrNone == status.Int());
		TESTL(rxLength() == 1);

		// receive the rest of it back (using the KSockReadContinuation option)
		socket.iObj.Recv(rxbuf, KSockReadContinuation, status, rxLength);
		User::WaitForRequest(status);
		buf16.Copy(rxbuf);
		Logger().WriteFormat(_L("Receive status is %S, data received is '%S'"), &EpocErrorToText(status.Int()), &buf16);
		TESTL(KErrNone == status.Int());
		TESTL(rxLength() == 0);
		}
		
	CleanupStack::PopAndDestroy(2);
			
	return EPass;
	}


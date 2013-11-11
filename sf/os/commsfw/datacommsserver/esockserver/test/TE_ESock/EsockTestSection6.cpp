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
// This contains ESock Test cases from section 6
// 
//

// EPOC includes
#include <e32base.h>
#include <in_sock.h>
#include <es_mbuf.h>
#include <ss_pman.h>

// Test system includes
#include "EsockTestSection6.h"


// Test step 6.1
const TDesC& CEsockTest6_1::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test6.1");
	
	return ret;
	}

CEsockTest6_1::~CEsockTest6_1()
	{
	}

enum TVerdict CEsockTest6_1::easyTestStepPreambleL()
	{
	CloseSockets();

	TInt connections;
	TRAPD(ret, connections = OpenConnectionsL(_L("Test_6.1"), iSockIndex2, iSockIndex3));
	if (KErrNone != ret)
		{
		return EFail;
		}
	if (0 >= connections)
		{
		return EFail;
		}

	return EPass;
	}

enum TVerdict CEsockTest6_1::easyTestStepL()
	{
	TESTL(EPass == TestStepResult());
	
	TRequestStatus stat;
	// shutdown server socket - Normal mode
	iEsockSuite->GetSocketHandle(iSockIndex3).Shutdown(RSocket::ENormal, stat);
	User::WaitForRequest(stat);
	TESTEL(stat==KErrNone, stat.Int());
	
	TBuf8<40> rbuf;
	// read from server socket
	iEsockSuite->GetSocketHandle(iSockIndex3).Read(rbuf, stat);
	User::WaitForRequest(stat);
	TESTEL(stat==KErrBadHandle, stat.Int());
	
	// write on server socket
	iEsockSuite->GetSocketHandle(iSockIndex3).Write(_L8("some data"), stat);
	User::WaitForRequest(stat);
	TESTEL(stat==KErrBadHandle, stat.Int());
	
	// read fronm client socket
	iEsockSuite->GetSocketHandle(iSockIndex2).Read(rbuf, stat);
	User::WaitForRequest(stat);
	TESTEL(stat==KErrEof, stat.Int()); // nothing to read
	
	// write on client socket
	iEsockSuite->GetSocketHandle(iSockIndex2).Write(_L8("some data"), stat);
	User::WaitForRequest(stat);
	TESTEL(stat==KErrNone, stat.Int()); // successful
	
	return EPass;
	}


// Test step 6.2
const TDesC& CEsockTest6_2::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test6.2");
	
	return ret;
	}

CEsockTest6_2::~CEsockTest6_2()
	{
	}

enum TVerdict CEsockTest6_2::easyTestStepPreambleL()
	{
	CloseSockets();

	TInt connections;
	TRAPD(ret, connections = OpenConnectionsL(_L("Test_6.2"), iSockIndex2, iSockIndex3));
	if (KErrNone != ret)
		{
		return EFail;
		}
	if (0 >= connections)
		{
		return EFail;
		}

	return EPass;
	}

enum TVerdict CEsockTest6_2::easyTestStepL()
	{
	TESTL(EPass == TestStepResult());
	
	TRequestStatus stat;
	// shutdown server socket - StopOutput mode
	iEsockSuite->GetSocketHandle(iSockIndex3).Shutdown(RSocket::EStopOutput, stat);
	User::WaitForRequest(stat);
	TESTEL(stat==KErrNone, stat.Int());
	
	// write on server socket
	iEsockSuite->GetSocketHandle(iSockIndex3).Write(_L8("some data"), stat);
	User::WaitForRequest(stat);
	TESTEL(stat==KErrEof, stat.Int());
	
	// read on client
	TBuf8<40> rbuf, wbuf;
	iEsockSuite->GetSocketHandle(iSockIndex2).Read(rbuf, stat);
	User::WaitForRequest(stat);
	TESTEL(stat==KErrEof, stat.Int());
	
	// write on client
	wbuf.SetMax();
	iEsockSuite->GetSocketHandle(iSockIndex2).Write(wbuf, stat);
	User::WaitForRequest(stat);
	TESTEL(stat==KErrNone, stat.Int());
	
	// read on server
	iEsockSuite->GetSocketHandle(iSockIndex3).Read(rbuf, stat);
	User::WaitForRequest(stat);
	TESTEL(stat==KErrNone, stat.Int());
	
	TESTL(rbuf==wbuf);
	
	return EPass;
	}


// Test step 6.3
const TDesC& CEsockTest6_3::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test6.3");
	
	return ret;
	}

CEsockTest6_3::~CEsockTest6_3()
	{
	}

enum TVerdict CEsockTest6_3::easyTestStepPreambleL()
	{
	CloseSockets();

	TInt connections;
	TRAPD(ret, connections = OpenConnectionsL(_L("Test_6.3"), iSockIndex2, iSockIndex3));
	if (KErrNone != ret)
		{
		return EFail;
		}
	if (0 >= connections)
		{
		return EFail;
		}

	return EPass;
	}

enum TVerdict CEsockTest6_3::easyTestStepL()
	{
	TESTL(EPass == TestStepResult());
	
	TRequestStatus stat;
	// shutdown server socket - StopInput mode
	iEsockSuite->GetSocketHandle(iSockIndex3).Shutdown(RSocket::EStopInput, stat);
	User::WaitForRequest(stat);
	TESTEL(stat==KErrNone, stat.Int());
	
	TBuf8<4> rbuf, wbuf;
	// write on server
	wbuf.SetMax();
	iEsockSuite->GetSocketHandle(iSockIndex3).Write(wbuf, stat);
	User::WaitForRequest(stat);
	TESTEL(stat==KErrNone, stat.Int());
	
	// read on client
	iEsockSuite->GetSocketHandle(iSockIndex2).Read(rbuf, stat);
	User::WaitForRequest(stat);
	TESTEL(stat==KErrNone, stat.Int());
	
	TESTL(rbuf==wbuf);
	
	// write on client socket
	iEsockSuite->GetSocketHandle(iSockIndex2).Write(_L8("some data"), stat);
	User::WaitForRequest(stat);
	TESTEL(stat==KErrNone, stat.Int());
	
	// read on server
	iEsockSuite->GetSocketHandle(iSockIndex3).Read(rbuf, stat);
	User::WaitForRequest(stat);
	TESTEL(stat==KErrEof, stat.Int());
	
	return EPass;
	}


// Test step 6.4
const TDesC& CEsockTest6_4::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test6.4");
	
	return ret;
	}

CEsockTest6_4::~CEsockTest6_4()
	{
	}

enum TVerdict CEsockTest6_4::easyTestStepPreambleL()
	{
	CloseSockets();

	TInt connections;
	TRAPD(ret, connections = OpenConnectionsL(_L("Test_6.4"), iSockIndex2, iSockIndex3));
	if (KErrNone != ret)
		{
		return EFail;
		}
	if (0 >= connections)
		{
		return EFail;
		}

	return EPass;
	}

enum TVerdict CEsockTest6_4::easyTestStepL()
	{
	TESTL(EPass == TestStepResult());
	
	TRequestStatus stat;
	// shutdown server socket - Immediate mode
	iEsockSuite->GetSocketHandle(iSockIndex3).Shutdown(RSocket::EImmediate, stat);
	User::WaitForRequest(stat);
	TESTEL(stat==KErrNone, stat.Int());
	
	TBuf8<40> rbuf;
	// read on server socket
	iEsockSuite->GetSocketHandle(iSockIndex3).Read(rbuf, stat);
	User::WaitForRequest(stat);
	TESTEL(stat==KErrBadHandle, stat.Int());
	
	// write on server
	iEsockSuite->GetSocketHandle(iSockIndex3).Write(_L8("some data"), stat);
	User::WaitForRequest(stat);
	TESTEL(stat==KErrBadHandle, stat.Int());
	
	// read on client
	iEsockSuite->GetSocketHandle(iSockIndex2).Read(rbuf, stat);
	User::WaitForRequest(stat);
	TESTEL(stat==KErrDisconnected, stat.Int());
	
	// write on client
	iEsockSuite->GetSocketHandle(iSockIndex2).Write(_L8("more data"), stat);
	User::WaitForRequest(stat);
	TESTEL(stat==KErrDisconnected, stat.Int());
	
	return EPass;
	}


// Test step 6.5
const TDesC& CEsockTest6_5::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test6.5");
	
	return ret;
	}

CEsockTest6_5::~CEsockTest6_5()
	{
	}

enum TVerdict CEsockTest6_5::easyTestStepPreambleL()
	{
	CloseSockets();

	TInt connections;
	TRAPD(ret, connections = OpenConnectionsL(_L("Test_6.5"), iSockIndex2, iSockIndex3));
	if (KErrNone != ret)
		{
		return EFail;
		}
	if (0 >= connections)
		{
		return EFail;
		}

	return EPass;
	}

enum TVerdict CEsockTest6_5::easyTestStepL()
	{
	TESTL(EPass == TestStepResult());
	
	TRequestStatus stat;
	// shutdown client socket - Normal mode
	iEsockSuite->GetSocketHandle(iSockIndex2).Shutdown(RSocket::ENormal, stat);
	User::WaitForRequest(stat);
	TESTEL(stat==KErrNone, stat.Int());
	
	TBuf8<40> rbuf;
	// read on client socket
	iEsockSuite->GetSocketHandle(iSockIndex2).Read(rbuf, stat);
	User::WaitForRequest(stat);
	TESTEL(stat==KErrBadHandle, stat.Int());
	
	// write on client
	iEsockSuite->GetSocketHandle(iSockIndex2).Write(_L8("some data"), stat);
	User::WaitForRequest(stat);
	TESTEL(stat==KErrBadHandle, stat.Int());
	
	// read on server
	iEsockSuite->GetSocketHandle(iSockIndex3).Read(rbuf, stat);
	User::WaitForRequest(stat);
	TESTEL(stat==KErrEof, stat.Int());
	
	// write on server
	iEsockSuite->GetSocketHandle(iSockIndex3).Write(_L8("more data"), stat);
	User::WaitForRequest(stat);
	TESTEL(stat==KErrNone, stat.Int());
	
	return EPass;
	}


// Test step 6.6
const TDesC& CEsockTest6_6::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test6.6");
	
	return ret;
	}

CEsockTest6_6::~CEsockTest6_6()
	{
	}

enum TVerdict CEsockTest6_6::easyTestStepPreambleL()
	{
	CloseSockets();

	TInt connections;
	TRAPD(ret, connections = OpenConnectionsL(_L("Test_6.6"), iSockIndex2, iSockIndex3));
	if (KErrNone != ret)
		{
		return EFail;
		}
	if (0 >= connections)
		{
		return EFail;
		}

	return EPass;
	}

enum TVerdict CEsockTest6_6::easyTestStepL()
	{
	TESTL(EPass == TestStepResult());
	
	TRequestStatus stat;
	// shutdown client socket - StopOutput mode
	iEsockSuite->GetSocketHandle(iSockIndex2).Shutdown(RSocket::EStopOutput, stat);
	User::WaitForRequest(stat);
	TESTEL(stat==KErrNone, stat.Int());
	
	// write on client socket
	iEsockSuite->GetSocketHandle(iSockIndex2).Write(_L8("some data"), stat);
	User::WaitForRequest(stat);
	TESTEL(stat==KErrEof, stat.Int());
	
	TBuf8<40> rbuf, wbuf;
	// read on server
	iEsockSuite->GetSocketHandle(iSockIndex3).Read(rbuf, stat);
	User::WaitForRequest(stat);
	TESTEL(stat==KErrEof, stat.Int());
	
	// write on server
	wbuf.SetMax();
	iEsockSuite->GetSocketHandle(iSockIndex3).Write(wbuf, stat);
	User::WaitForRequest(stat);
	TESTEL(stat==KErrNone, stat.Int());
	
	// read on client
	iEsockSuite->GetSocketHandle(iSockIndex2).Read(rbuf, stat);
	User::WaitForRequest(stat);
	TESTEL(stat==KErrNone, stat.Int());
	
	TESTL(rbuf==wbuf);
	
	return EPass;
	}


// Test step 6.7
const TDesC& CEsockTest6_7::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test6.7");
	
	return ret;
	}

CEsockTest6_7::~CEsockTest6_7()
	{
	}

enum TVerdict CEsockTest6_7::easyTestStepPreambleL()
	{
	CloseSockets();

	TInt connections;
	TRAPD(ret, connections = OpenConnectionsL(_L("Test_6.7"), iSockIndex2, iSockIndex3));
	if (KErrNone != ret)
		{
		return EFail;
		}
	if (0 >= connections)
		{
		return EFail;
		}

	return EPass;
	}

enum TVerdict CEsockTest6_7::easyTestStepL()
	{
	TESTL(EPass == TestStepResult());
	
	TRequestStatus stat;
	// shutdown client socket - StopInput mode
	iEsockSuite->GetSocketHandle(iSockIndex2).Shutdown(RSocket::EStopInput, stat);
	User::WaitForRequest(stat);
	TESTEL(stat==KErrNone, stat.Int());
	
	// For testing - try a short delay 
	User::After(1000000); 
	
	TBuf8<40> rbuf, wbuf;
	// write on client
	wbuf.SetMax();
	iEsockSuite->GetSocketHandle(iSockIndex2).Write(wbuf, stat);
	User::WaitForRequest(stat);
	TESTEL(stat==KErrNone, stat.Int());
	
	// read on server
	iEsockSuite->GetSocketHandle(iSockIndex3).Read(rbuf, stat);
	User::WaitForRequest(stat);
	TESTEL(stat==KErrNone, stat.Int());
	
	// write on server socket
	iEsockSuite->GetSocketHandle(iSockIndex3).Write(_L8("some data"), stat);
	User::WaitForRequest(stat);
	TESTEL(stat==KErrNone, stat.Int());
	
	// read on client
	iEsockSuite->GetSocketHandle(iSockIndex2).Read(rbuf, stat);
	User::WaitForRequest(stat);
	TESTEL(stat==KErrEof, stat.Int()); 
	
	// For testing - try a short delay 
	User::After(1000000); 
	
	return EPass;
	}


// Test step 6.8
const TDesC& CEsockTest6_8::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test6.8");
	
	return ret;
	}

CEsockTest6_8::~CEsockTest6_8()
	{
	}

enum TVerdict CEsockTest6_8::easyTestStepPreambleL()
	{
	CloseSockets();

	TInt connections;
	TRAPD(ret, connections = OpenConnectionsL(_L("Test_6.8"), iSockIndex2, iSockIndex3));
	if (KErrNone != ret)
		{
		return EFail;
		}
	if (0 >= connections)
		{
		return EFail;
		}

	return EPass;
	}

enum TVerdict CEsockTest6_8::easyTestStepL()
	{
	TESTL(EPass == TestStepResult());
	
	TRequestStatus stat;
	// shutdown client socket - Immediate mode
	iEsockSuite->GetSocketHandle(iSockIndex2).Shutdown(RSocket::EImmediate, stat);
	User::WaitForRequest(stat);
	TESTEL(stat==KErrNone, stat.Int());
	
	// read on client socket
	TBuf8<40> rbuf;
	iEsockSuite->GetSocketHandle(iSockIndex2).Read(rbuf, stat);
	User::WaitForRequest(stat);
	TESTEL(stat==KErrBadHandle, stat.Int());
	
	// write on client
	iEsockSuite->GetSocketHandle(iSockIndex2).Write(_L8("some data"), stat);
	User::WaitForRequest(stat);
	TESTEL(stat==KErrBadHandle, stat.Int());
	
	// read on server
	iEsockSuite->GetSocketHandle(iSockIndex3).Read(rbuf, stat);
	User::WaitForRequest(stat);
	TESTEL(stat==KErrDisconnected, stat.Int());
	
	// write on server
	iEsockSuite->GetSocketHandle(iSockIndex3).Write(_L8("more data"), stat);
	User::WaitForRequest(stat);
	TESTEL(stat==KErrDisconnected, stat.Int());
	return EPass;
	}


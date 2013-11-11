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
// This contains ESock Test cases from section 10
// 
//

// EPOC includes
#include <e32base.h>
#include <in_sock.h>

// Test system includes
#include "EsockTestSection10.h"


// Test step 10.1
const TDesC& CEsockTest10_1::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test10.1");
	
	return ret;
	}

CEsockTest10_1::~CEsockTest10_1()
	{
	}

enum TVerdict CEsockTest10_1::easyTestStepPreambleL()
	{
	TInt connections;
	TRAPD(ret, connections = OpenConnectionsL(_L("Test_10.1"), iSockIndex2, iSockIndex3));
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

enum TVerdict CEsockTest10_1::easyTestStepL()
	{
	TESTL(EPass == TestStepResult());
	TPckgBuf<TUint> flags;
	
	// get select state of server socket
	TRequestStatus stat;
	flags() = KSockSelectRead | KSockSelectWrite | KSockSelectExcept;
	iEsockSuite->GetSocketHandle(iSockIndex3).Ioctl(KIOctlSelect, stat, &flags, KSOLSocket);
	User::WaitForRequest(stat);
	TESTL(stat==KErrNone);
	TESTL(flags()==KSockSelectWrite);
	
	// send data from client to server (2 bytes, 1 urgent)
	TInt ret = iEsockSuite->GetSocketHandle(iSockIndex2).SetOpt(KSoTcpNextSendUrgentData, KSolInetTcp, 1);
	TESTEL(KErrNone == ret, ret);
	iEsockSuite->GetSocketHandle(iSockIndex2).Write(_L8("12"), stat);
	User::WaitForRequest(stat);
	TESTEL(stat==KErrNone, stat.Int());
	
	// delay for 1 second
	User::After(1000000);
	
	// get select state of the server socket
	flags() = KSockSelectRead | KSockSelectWrite | KSockSelectExcept;
	iEsockSuite->GetSocketHandle(iSockIndex3).Ioctl(KIOctlSelect, stat, &flags, KSOLSocket);
	User::WaitForRequest(stat);
	TESTL(stat==KErrNone);
	TESTL(flags()==(KSockSelectRead | KSockSelectWrite | KSockSelectExcept));
	
	// read data (server)
	TBuf8<20> rbuf2;
	iEsockSuite->GetSocketHandle(iSockIndex3).Read(rbuf2, stat);
	User::WaitForRequest(stat);
	TESTEL(stat==KErrUrgentData, stat.Int());
	
	// get select state of the server socket
	flags() = KSockSelectRead | KSockSelectWrite | KSockSelectExcept;
	iEsockSuite->GetSocketHandle(iSockIndex3).Ioctl(KIOctlSelect, stat, &flags, KSOLSocket);
	User::WaitForRequest(stat);
	TESTL(stat==KErrNone);
	
	TESTL(flags()==(KSockSelectWrite | KSockSelectExcept));
	
	// read the urgent data byte
	TInt nRet;
	ret = iEsockSuite->GetSocketHandle(iSockIndex3).GetOpt(KSoTcpReadUrgentData, KSolInetTcp, nRet);
	TESTEL(KErrNone == ret, ret);
	TESTL(nRet=='2');
	ret = iEsockSuite->GetSocketHandle(iSockIndex3).GetOpt(KSoTcpReadUrgentData, KSolInetTcp, nRet);
	TESTEL(KErrNotFound == ret, ret);
	
	// get select state of the server socket
	flags() = KSockSelectRead | KSockSelectWrite | KSockSelectExcept;
	iEsockSuite->GetSocketHandle(iSockIndex3).Ioctl(KIOctlSelect, stat, &flags, KSOLSocket);
	User::WaitForRequest(stat);
	TESTL(stat==KErrNone);
	TESTL(flags()==KSockSelectWrite);
	
	return EPass;
	}


// Test step 10.2
const TDesC& CEsockTest10_2::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test10.2");
	
	return ret;
	}

CEsockTest10_2::~CEsockTest10_2()
	{
	}

enum TVerdict CEsockTest10_2::easyTestStepL()
	{
	
	// Ioctl on an unconnected socket
	
	// open a TCP socket
	RSocket sock;
	CleanupClosePushL(sock);
	TInt nRet = sock.Open(iEsockSuite->iSocketServer, KAfInet, KSockStream, KProtocolInetTcp);
	TESTEL(KErrNone == nRet, nRet);
	
	// get select state of the socket
	sock.GetOpt (KSOSelectPoll, KSOLSocket, (nRet = 0));
	
	// socket is blocked for writing, has no data for reading
	// and has no exception condition
	TESTEL(0 == nRet, nRet);
	
	// close the socket
	CleanupStack::PopAndDestroy();
	
	return EPass;
	}


// Test step 10.3
const TDesC& CEsockTest10_3::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test10.3");
	
	return ret;
	}

CEsockTest10_3::~CEsockTest10_3()
	{
	}

enum TVerdict CEsockTest10_3::easyTestStepPreambleL()
	{
	TInt connections;
	TRAPD(ret, connections = OpenConnectionsL(_L("Test_10.3"), iSockIndex2, iSockIndex3));
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

enum TVerdict CEsockTest10_3::easyTestStepL()
	{
	TESTL(EPass == TestStepResult());
	
	// cancel an Ioctl operation
	
	TPckgBuf<TUint> flags;
	flags() = KSockSelectExcept;
	
	// get select state of the server socket
	// do not wait for operation to complete
	TRequestStatus stat;
	iEsockSuite->GetSocketHandle(iSockIndex3).Ioctl(KIOctlSelect, stat, &flags, KSOLSocket);
	TESTEL(KRequestPending == stat.Int(), stat.Int());
	
	// cancel the Ioctl operation
	iEsockSuite->GetSocketHandle(iSockIndex3).CancelIoctl();
	
	// wait for operation to complete
	User::WaitForRequest(stat);
	
	// completion status will be KErrNone or KErrCancel
	TESTEL(KErrCancel == stat.Int(), stat.Int());
	
	// get select state of the server socket
	flags() = KSockSelectRead | KSockSelectWrite | KSockSelectExcept;
	iEsockSuite->GetSocketHandle(iSockIndex3).Ioctl(KIOctlSelect, stat, &flags, KSOLSocket);
	
	User::WaitForRequest(stat);
	TESTEL(KErrNone == stat.Int(), stat.Int());
	
	// socket is not blocked for writing
	TESTL(flags()==KSockSelectWrite);
	
	return EPass;
	}


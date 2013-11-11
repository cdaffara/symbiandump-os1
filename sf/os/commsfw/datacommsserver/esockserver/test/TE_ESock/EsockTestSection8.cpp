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
// This contains ESock Test cases from section 8
// 
//

// EPOC includes
#include <e32base.h>
#include <in_sock.h>

// Test system includes
#include "EsockTestSection8.h"


// Test step 8.1
const TDesC& CEsockTest8_1::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test8.1");
	
	return ret;
	}

CEsockTest8_1::~CEsockTest8_1()
	{
	}

enum TVerdict CEsockTest8_1::easyTestStepPreambleL()
	{
	TInt connections;
	TRAPD(ret, connections = OpenConnectionsL(_L("Test_8.1"), iSockIndex2, iSockIndex3));
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

enum TVerdict CEsockTest8_1::easyTestStepL()
	{
	TESTL(EPass == TestStepResult());
	
	// Urgent data test - 1 byte read out-of-band
	
	// for client enable KSoTcpNextSendUrgentData
	TESTL(iEsockSuite->GetSocketHandle(iSockIndex2).SetOpt(KSoTcpNextSendUrgentData, KSolInetTcp, 1)==KErrNone);
	
	// send a single byte from the client
	TRequestStatus stat;
	iEsockSuite->GetSocketHandle(iSockIndex2).Write(_L8("7"), stat);
	User::WaitForRequest(stat);
	TESTEL(stat==KErrNone, stat.Int());
	
	// wait for 1 second
	User::After(1000000);
	
	// check on server that urgent data is available
	TInt nRet;
	TInt ret = iEsockSuite->GetSocketHandle(iSockIndex3).GetOpt(KSoTcpRcvAtMark, KSolInetTcp, nRet);
	TESTEL(KErrNone == ret, ret);
	TESTEL(nRet == 1, nRet);
	
	// check the urgent data offset (server)
	ret = iEsockSuite->GetSocketHandle(iSockIndex3).GetOpt(KSOUrgentDataOffset, KSOLSocket, nRet);
	TESTEL(KErrNone == ret, ret);
	TESTEL(nRet == 0, nRet);
	
	// check the number of bytes to read (server)
	ret = iEsockSuite->GetSocketHandle(iSockIndex3).GetOpt(KSOReadBytesPending, KSOLSocket, nRet);
	TESTEL(KErrNone == ret, ret);
	TESTEL(nRet == 0, nRet);
	
	// get the last error - check it is KErrUrgentData (server)
	ret = iEsockSuite->GetSocketHandle(iSockIndex3).GetOpt(KSOSelectLastError, KSOLSocket, nRet);
	TESTEL(KErrUrgentData == ret, ret);
	
	// peek for urgent data (server)
	ret = iEsockSuite->GetSocketHandle(iSockIndex3).GetOpt(KSoTcpPeekUrgentData, KSolInetTcp, nRet);
	TESTEL(KErrNone == ret, ret);
	TESTEL(nRet == '7', nRet);
	
	// peek again
	ret = iEsockSuite->GetSocketHandle(iSockIndex3).GetOpt(KSoTcpPeekUrgentData, KSolInetTcp, nRet);
	TESTEL(KErrNone == ret, ret);
	TESTEL(nRet == '7', nRet);
	
	// read the urgent data (server)
	ret = iEsockSuite->GetSocketHandle(iSockIndex3).GetOpt(KSoTcpReadUrgentData, KSolInetTcp, nRet);
	TESTEL(KErrNone == ret, ret);
	TESTEL(nRet == '7', nRet);
	
	// read again
	ret = iEsockSuite->GetSocketHandle(iSockIndex3).GetOpt(KSoTcpReadUrgentData, KSolInetTcp, nRet);
	TESTEL(KErrNotFound == ret, ret);
	
	return EPass;
	}


// Test step 8.2
const TDesC& CEsockTest8_2::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test8.2");
	
	return ret;
	}

CEsockTest8_2::~CEsockTest8_2()
	{
	}

enum TVerdict CEsockTest8_2::easyTestStepPreambleL()
	{
	TInt connections;
	TRAPD(ret, connections = OpenConnectionsL(_L("Test_8.2"), iSockIndex2, iSockIndex3));
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

enum TVerdict CEsockTest8_2::easyTestStepL()
	{
	TESTL(EPass == TestStepResult());
	
	// Urgent data test - 2 bytes, 1 of urgent data read out-of-band
	
	// for client enable KSoTcpNextSendUrgentData
	TInt ret = iEsockSuite->GetSocketHandle(iSockIndex2).SetOpt(KSoTcpNextSendUrgentData, KSolInetTcp, 1);
	TESTEL(KErrNone == ret, ret);
	
	// send 2 bytes of data from the client
	TRequestStatus stat;
	iEsockSuite->GetSocketHandle(iSockIndex2).Write(_L8("89"), stat);
	User::WaitForRequest(stat);
	TESTEL(stat==KErrNone, stat.Int());
	
	// check on server that urgent data is not available
	TInt nRet;
	ret = iEsockSuite->GetSocketHandle(iSockIndex3).GetOpt(KSoTcpRcvAtMark, KSolInetTcp, nRet);
	TESTEL(KErrNone == ret, ret);
	TESTEL(0 == nRet, nRet);
	
	// wait for 1 second
	User::After(1000000);
	
	// check the urgent data offset (server)
	ret = iEsockSuite->GetSocketHandle(iSockIndex3).GetOpt(KSOUrgentDataOffset, KSOLSocket, nRet);
	TESTEL(KErrNone == ret, ret);
	TESTEL(1 == nRet, nRet);
	
	// check the number of bytes to read (server)
	ret = iEsockSuite->GetSocketHandle(iSockIndex3).GetOpt(KSOReadBytesPending, KSOLSocket, nRet);
	TESTEL(KErrNone == ret, ret);
	TESTEL(1 == nRet, nRet);
	
	// read the data - check only a single byte is read
	TBuf8<40> rbuf;
	iEsockSuite->GetSocketHandle(iSockIndex3).Read(rbuf, stat);
	User::WaitForRequest(stat);
	TESTEL(stat==KErrUrgentData, stat.Int());
	TESTL(rbuf==_L8("8"));
	
	// check that urgent data is now available
	ret = iEsockSuite->GetSocketHandle(iSockIndex3).GetOpt(KSoTcpRcvAtMark, KSolInetTcp, nRet);
	TESTEL(KErrNone == ret, ret);
	TESTEL(1 == nRet, nRet);
	
	// check KSoTcpNextSendUrgentData option is disabled (client)
	ret = iEsockSuite->GetSocketHandle(iSockIndex2).GetOpt(KSoTcpNextSendUrgentData, KSolInetTcp, nRet);
	TESTEL(KErrNone == ret, ret);
	TESTEL(0 == nRet, nRet);
	
	// get the last error - check it is KErrUrgentData (server)
	ret = iEsockSuite->GetSocketHandle(iSockIndex3).GetOpt(KSOSelectLastError, KSOLSocket, nRet);
	TESTEL(KErrUrgentData == ret, ret);
	
	// peek for urgent data (server)
	ret = iEsockSuite->GetSocketHandle(iSockIndex3).GetOpt(KSoTcpPeekUrgentData, KSolInetTcp, nRet);
	TESTEL(KErrNone == ret, ret);
	TESTL(nRet=='9');
	
	// read the urgent data (server)
	ret = iEsockSuite->GetSocketHandle(iSockIndex3).GetOpt(KSoTcpReadUrgentData, KSolInetTcp, nRet);
	TESTEL(KErrNone == ret, ret);
	TESTL(nRet=='9');
	
	// read again
	ret = iEsockSuite->GetSocketHandle(iSockIndex3).GetOpt(KSoTcpReadUrgentData, KSolInetTcp, nRet);
	TESTEL(KErrNotFound == ret, ret);
	
	return EPass;
	}


// Test step 8.3
const TDesC& CEsockTest8_3::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test8.3");
	
	return ret;
	}

CEsockTest8_3::~CEsockTest8_3()
	{
	}

enum TVerdict CEsockTest8_3::easyTestStepPreambleL()
	{
	TInt connections;
	TRAPD(ret, connections = OpenConnectionsL(_L("Test_8.3"), iSockIndex2, iSockIndex3));
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

enum TVerdict CEsockTest8_3::easyTestStepL()
	{
	TESTL(EPass == TestStepResult());
	
	// Urgent data test - 14 bytes (4th is urgent) read out-of-band
	
	// for client enable KSoTcpNextSendUrgentData
	TInt ret = iEsockSuite->GetSocketHandle(iSockIndex2).SetOpt(KSoTcpNextSendUrgentData, KSolInetTcp, 1);
	TESTEL(KErrNone == ret, ret);
	
	// send 4 bytes of data from the client
	TRequestStatus stat;
	iEsockSuite->GetSocketHandle(iSockIndex2).Write(_L8("1234"), stat);
	User::WaitForRequest(stat);
	TESTEL(stat==KErrNone, stat.Int());
	
	// send 10 more bytes of data from the client
	iEsockSuite->GetSocketHandle(iSockIndex2).Write(_L8("abcdefghij"), stat);
	
	User::WaitForRequest(stat);
	TESTEL(stat==KErrNone, stat.Int());
	
	// wait for 1 second
	User::After(1000000);
	
	// check on server that urgent data is not available
	TInt nRet;
	ret = iEsockSuite->GetSocketHandle(iSockIndex3).GetOpt(KSoTcpRcvAtMark, KSolInetTcp, nRet);
	TESTEL(KErrNone == ret, ret);
	TESTEL(0 == nRet, nRet);
	
	// check the urgent data offset (server)
	ret = iEsockSuite->GetSocketHandle(iSockIndex3).GetOpt(KSOUrgentDataOffset, KSOLSocket, nRet);
	TESTEL(KErrNone == ret, ret);
	TESTEL(3 == nRet, nRet);
	
	// check the number of bytes to read before urgent data(server)
	ret = iEsockSuite->GetSocketHandle(iSockIndex3).GetOpt(KSOReadBytesPending, KSOLSocket, nRet);
	TESTEL(KErrNone == ret, ret);
	TESTEL(3 == nRet, nRet);
	
	// read the data - check only 3 bytes are read
	TBuf8<10> rbuf;
	iEsockSuite->GetSocketHandle(iSockIndex3).Read(rbuf, stat);
	User::WaitForRequest(stat);
	TESTEL(stat==KErrUrgentData, stat.Int());
	TESTL(rbuf==_L8("123"));
	
	// check that urgent data is now available
	ret = iEsockSuite->GetSocketHandle(iSockIndex3).GetOpt(KSoTcpRcvAtMark, KSolInetTcp, nRet);
	TESTEL(KErrNone == ret, ret);
	TESTEL(1 == nRet, nRet);
	
	// check the number of bytes to read before urgent data (server)
	ret = iEsockSuite->GetSocketHandle(iSockIndex3).GetOpt(KSOReadBytesPending, KSOLSocket, nRet);
	TESTEL(KErrNone == ret, ret);
	TESTEL(0 == nRet, nRet);
	
	// peek for urgent data (server)
	ret = iEsockSuite->GetSocketHandle(iSockIndex3).GetOpt(KSoTcpPeekUrgentData, KSolInetTcp, nRet);
	TESTEL(KErrNone == ret, ret);
	TESTL(nRet=='4');
	
	// read the urgent data (server)
	ret = iEsockSuite->GetSocketHandle(iSockIndex3).GetOpt(KSoTcpReadUrgentData, KSolInetTcp, nRet);
	TESTEL(KErrNone == ret, ret);
	TESTL(nRet=='4');
	
	User::After(1000);
	
	// check the number of bytes to read is now updated since we read out urgent data
	ret = iEsockSuite->GetSocketHandle(iSockIndex3).GetOpt(KSOReadBytesPending, KSOLSocket, nRet);
	TESTEL(KErrNone == ret, ret);
	TESTEL(10 == nRet, nRet);
	
	// read the rest of the data
	iEsockSuite->GetSocketHandle(iSockIndex3).Read(rbuf, stat);
	User::WaitForRequest(stat);
	TESTEL(stat==KErrNone, stat.Int());
	TESTL(rbuf==_L8("abcdefghij"));
	
	return EPass;
	}


// Test step 8.4
const TDesC& CEsockTest8_4::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test8.4");
	
	return ret;
	}

CEsockTest8_4::~CEsockTest8_4()
	{
	}

enum TVerdict CEsockTest8_4::easyTestStepPreambleL()
	{
	TInt connections;
	TRAPD(ret, connections = OpenConnectionsL(_L("Test_8.4"), iSockIndex2, iSockIndex3));
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

enum TVerdict CEsockTest8_4::easyTestStepL()
	{
	TESTL(EPass == TestStepResult());
	
	// Urgent data test - 8000 bytes (7998th is urgent) read out-of-band
	
	// send data from client - 8000 bytes with 7998th marked urgent
	TBuf8<100> wbuf;
	TRequestStatus stat;
	for (TInt i=0 ; i<80 ; i++)
		{
		wbuf.SetMax();
		wbuf.Fill(TChar((TUint8)i)); // fill buffer with data
		
		if (i==79) // send 100 chars with 98th marked urgent
			{
			iEsockSuite->GetSocketHandle(iSockIndex2).Write(wbuf.Mid(0,97), stat);
			User::WaitForRequest(stat);
			TESTEL(stat==KErrNone, stat.Int());
			
			TESTL(iEsockSuite->GetSocketHandle(iSockIndex2).SetOpt(KSoTcpNextSendUrgentData, KSolInetTcp, 1)==KErrNone);
			iEsockSuite->GetSocketHandle(iSockIndex2).Write(_L8("?"), stat);
			User::WaitForRequest(stat);
			TESTEL(stat==KErrNone, stat.Int());
			
			iEsockSuite->GetSocketHandle(iSockIndex2).Write(_L8("yz"), stat);
			User::WaitForRequest(stat);
			TESTEL(stat==KErrNone, stat.Int());
			}
		else // send 100 chars
			{
			iEsockSuite->GetSocketHandle(iSockIndex2).Write(wbuf, stat);
			User::WaitForRequest(stat);
			TESTEL(stat==KErrNone, stat.Int());
			}
		}
	
	User::After(1000000);
	
	// check on server that urgent data is not available
	TInt nRet;
	TInt ret = iEsockSuite->GetSocketHandle(iSockIndex3).GetOpt(KSoTcpRcvAtMark, KSolInetTcp, nRet);
	TESTEL(KErrNone == ret, ret);
	TESTEL(0 == nRet, nRet);
	
	// check the urgent data offset (server)
	ret = iEsockSuite->GetSocketHandle(iSockIndex3).GetOpt(KSOUrgentDataOffset, KSOLSocket, nRet);
	TESTEL(KErrNone == ret, ret);
	TESTEL(7997 == nRet, nRet);
	
	// check the number of bytes to read before urgent data(server)
	ret = iEsockSuite->GetSocketHandle(iSockIndex3).GetOpt(KSOReadBytesPending, KSOLSocket, nRet);
	TESTEL(KErrNone == ret, ret);
	TESTEL(7997 == nRet, nRet);
	
	// read the data - check all bytes are read
	TInt expectedRet = KErrNone;
	TBuf8<100> rbuf;
	TInt count;
	for (count = 0; count < 7997;)
		{
		iEsockSuite->GetSocketHandle(iSockIndex3).Read(rbuf, stat);
		User::WaitForRequest(stat);
		TESTL(stat==expectedRet);
		count+=rbuf.Length();
		if (count == 7900)
			{
			expectedRet = KErrUrgentData;
			}
		}
	TESTEL(7997 == count, count);
	
	// check that urgent data is now available
	ret = iEsockSuite->GetSocketHandle(iSockIndex3).GetOpt(KSoTcpRcvAtMark, KSolInetTcp, nRet);
	TESTEL(KErrNone == ret, ret);
	TESTEL(1 == nRet, nRet);
	
	// check the number of bytes to read before urgent data(server)
	ret = iEsockSuite->GetSocketHandle(iSockIndex3).GetOpt(KSOReadBytesPending, KSOLSocket, nRet);
	TESTEL(KErrNone == ret, ret);
	TESTEL(0 == nRet, nRet);
	
	// read the urgent data (server)
	ret = iEsockSuite->GetSocketHandle(iSockIndex3).GetOpt(KSoTcpReadUrgentData, KSolInetTcp, nRet);
	TESTEL(KErrNone == ret, ret);
	TESTL(nRet=='?');
	
	User::After(1000);
	
	// check the number of bytes to read (server)
	ret = iEsockSuite->GetSocketHandle(iSockIndex3).GetOpt(KSOReadBytesPending, KSOLSocket, nRet);
	TESTEL(KErrNone == ret, ret);
	TESTEL(2 == nRet, nRet);
	
	// read the rest of the data
	TBuf8<2> rbuf2;
	iEsockSuite->GetSocketHandle(iSockIndex3).Read(rbuf2, stat);
	User::WaitForRequest(stat);
	TESTEL(stat==KErrNone, stat.Int());
	TESTL(rbuf2==_L8("yz"));
	
	return EPass;
	}
	
	
// Test step 8.5
const TDesC& CEsockTest8_5::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test8.5");
	
	return ret;
	}

CEsockTest8_5::~CEsockTest8_5()
	{
	}

enum TVerdict CEsockTest8_5::easyTestStepPreambleL()
	{
	TInt connections;
	TRAPD(ret, connections = OpenConnectionsL(_L("Test_8.5"), iSockIndex2, iSockIndex3));
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

enum TVerdict CEsockTest8_5::easyTestStepL()
	{
	TESTL(EPass == TestStepResult());
	
	// Urgent data test - 1 byte of data read in-band
	
	// for server enable KSoTcpOobInline
	TInt ret = iEsockSuite->GetSocketHandle(iSockIndex3).SetOpt(KSoTcpOobInline, KSolInetTcp, 1);
	TESTEL(KErrNone == ret, ret);
	
	// for client enable KSoTcpNextSendUrgentData
	ret = iEsockSuite->GetSocketHandle(iSockIndex2).SetOpt(KSoTcpNextSendUrgentData, KSolInetTcp, 1);
	TESTEL(KErrNone == ret, ret);
	
	// send a single data byte from the client
	TRequestStatus stat;
	iEsockSuite->GetSocketHandle(iSockIndex2).Write(_L8("7"), stat);
	User::WaitForRequest(stat);
	TESTEL(stat==KErrNone, stat.Int());
	
	// delay for 1 second
	User::After(1000000);
	
	// check on server that urgent data is available
	TInt nRet;
	ret = iEsockSuite->GetSocketHandle(iSockIndex3).GetOpt(KSoTcpRcvAtMark, KSolInetTcp, nRet);
	TESTEL(KErrNone == ret, ret);
	TESTEL(1 == nRet, nRet);
	
	// check the urgent data offset (server)
	ret = iEsockSuite->GetSocketHandle(iSockIndex3).GetOpt(KSOUrgentDataOffset, KSOLSocket, nRet);
	TESTEL(KErrNone == ret, ret);
	TESTEL(0 == nRet, nRet);
	
	// check the number of bytes to read (server)
	ret = iEsockSuite->GetSocketHandle(iSockIndex3).GetOpt(KSOReadBytesPending, KSOLSocket, nRet);
	TESTEL(KErrNone == ret, ret);
	TESTEL(1 == nRet, nRet);
	
	// check KSoTcpNextSendUrgentData option is disabled (client)
	ret = iEsockSuite->GetSocketHandle(iSockIndex2).GetOpt(KSoTcpNextSendUrgentData, KSolInetTcp, nRet);
	TESTEL(KErrNone == ret, ret);
	TESTEL(0 == nRet, nRet);
	
	// check that there is no error state for the socket
	ret = iEsockSuite->GetSocketHandle(iSockIndex3).GetOpt(KSOSelectLastError, KSOLSocket, nRet);
	TESTEL(KErrNone == ret, ret);
	
	// peek for urgent data (server)
	ret = iEsockSuite->GetSocketHandle(iSockIndex3).GetOpt(KSoTcpPeekUrgentData, KSolInetTcp, nRet);
	TESTEL(KErrNotFound == ret, ret);
	
	// read urgent data (server)
	ret = iEsockSuite->GetSocketHandle(iSockIndex3).GetOpt(KSoTcpReadUrgentData, KSolInetTcp, nRet);
	TESTEL(KErrNotFound == ret, ret);
	
	// read the data - check only 1 byte is read
	TBuf8<1> rbuf;
	iEsockSuite->GetSocketHandle(iSockIndex3).Read(rbuf, stat);
	User::WaitForRequest(stat);
	TESTEL(stat==KErrNone, stat.Int());
	TESTL(rbuf==_L8("7"));
	
	return EPass;
	}


// Test step 8.6
const TDesC& CEsockTest8_6::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test8.6");
	
	return ret;
	}

CEsockTest8_6::~CEsockTest8_6()
	{
	}

enum TVerdict CEsockTest8_6::easyTestStepPreambleL()
	{
	TInt connections;
	TRAPD(ret, connections = OpenConnectionsL(_L("Test_8.6"), iSockIndex2, iSockIndex3));
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

enum TVerdict CEsockTest8_6::easyTestStepL()
	{
	TESTL(EPass == TestStepResult());
	
	// Urgent data test - 2 bytes one of urgent data read in-band
	
	// for server enable KSoTcpOobInline
	TInt ret = iEsockSuite->GetSocketHandle(iSockIndex3).SetOpt(KSoTcpOobInline, KSolInetTcp, 1);
	TESTEL(KErrNone == ret, ret);
	
	// for client enable KSoTcpNextSendUrgentData
	ret = iEsockSuite->GetSocketHandle(iSockIndex2).SetOpt(KSoTcpNextSendUrgentData, KSolInetTcp, 1);
	TESTEL(KErrNone == ret, ret);
	
	// send 2 bytes of data from the client
	TRequestStatus stat;
	iEsockSuite->GetSocketHandle(iSockIndex2).Write(_L8("89"), stat);
	User::WaitForRequest(stat);
	TESTEL(stat==KErrNone, stat.Int());
	
	User::After(1000000);
	
	// check on server that urgent data is not available
	TInt nRet;
	ret = iEsockSuite->GetSocketHandle(iSockIndex3).GetOpt(KSoTcpRcvAtMark, KSolInetTcp, nRet);
	TESTEL(KErrNone == ret, ret);
	TESTEL(0 == nRet, nRet);
	
	// check the urgent data offset (server)
	ret = iEsockSuite->GetSocketHandle(iSockIndex3).GetOpt(KSOUrgentDataOffset, KSOLSocket, nRet);
	TESTEL(KErrNone == ret, ret);
	TESTEL(1 == nRet, nRet);
	
	// check the number of bytes to read before urgent data (server)
	ret = iEsockSuite->GetSocketHandle(iSockIndex3).GetOpt(KSOReadBytesPending, KSOLSocket, nRet);
	TESTEL(KErrNone == ret, ret);
	TESTEL(1 == nRet, nRet);
	
	// read the data - check only 1 byte is read
	TBuf8<1> rbuf;
	iEsockSuite->GetSocketHandle(iSockIndex3).Read(rbuf, stat);
	User::WaitForRequest(stat);
	TESTEL(stat==KErrNone, stat.Int());
	TESTL(rbuf==_L8("8"));
	
	User::After(1000);
	
	// read again - check the remaining byte is read
	iEsockSuite->GetSocketHandle(iSockIndex3).Read(rbuf, stat);
	User::WaitForRequest(stat);
	TESTEL(stat==KErrNone, stat.Int());
	TESTL(rbuf==_L8("9"));
	
	// check urgent data is not available
	ret = iEsockSuite->GetSocketHandle(iSockIndex3).GetOpt(KSoTcpRcvAtMark, KSolInetTcp, nRet);
	TESTEL(KErrNone == ret, ret);
	TESTEL(0 == nRet, nRet);
	
	// peek for urgent data (server)
	ret = iEsockSuite->GetSocketHandle(iSockIndex3).GetOpt(KSoTcpPeekUrgentData, KSolInetTcp, nRet);
	TESTEL(KErrNotFound == ret, ret);
	
	// read urgent data (server)
	ret = iEsockSuite->GetSocketHandle(iSockIndex3).GetOpt(KSoTcpReadUrgentData, KSolInetTcp, nRet);
	TESTEL(KErrNotFound == ret, ret);
	
	return EPass;
	}


// Test step 8.7
const TDesC& CEsockTest8_7::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test8.7");
	
	return ret;
	}

CEsockTest8_7::~CEsockTest8_7()
	{
	}

enum TVerdict CEsockTest8_7::easyTestStepPreambleL()
	{
	TInt connections;
	TRAPD(ret, connections = OpenConnectionsL(_L("Test_8.7"), iSockIndex2, iSockIndex3));
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

enum TVerdict CEsockTest8_7::easyTestStepL()
	{
	TESTL(EPass == TestStepResult());
	
	// Urgent data test - 14 bytes (4th is urgent) read in-band
	
	// for server enable KSoTcpOobInline
	TInt ret = iEsockSuite->GetSocketHandle(iSockIndex3).SetOpt(KSoTcpOobInline, KSolInetTcp, 1);
	TESTEL(KErrNone == ret, ret);
	
	// for client enable KSoTcpNextSendUrgentData
	ret = iEsockSuite->GetSocketHandle(iSockIndex2).SetOpt(KSoTcpNextSendUrgentData, KSolInetTcp, 1);
	TESTEL(KErrNone == ret, ret);
	
	// send 4 bytes of data from the client
	TRequestStatus stat;
	iEsockSuite->GetSocketHandle(iSockIndex2).Write(_L8("1234"), stat);
	User::WaitForRequest(stat);
	TESTEL(stat==KErrNone, stat.Int());
	
	// send 10 more bytes of data from the client
	iEsockSuite->GetSocketHandle(iSockIndex2).Write(_L8("1234567890"), stat);
	User::WaitForRequest(stat);
	TESTEL(stat==KErrNone, stat.Int());
	
	User::After(1000000);
	
	// check on server that urgent data is not available
	TInt nRet;
	ret = iEsockSuite->GetSocketHandle(iSockIndex3).GetOpt(KSoTcpRcvAtMark, KSolInetTcp, nRet);
	TESTEL(KErrNone == ret, ret);
	TESTEL(0 == nRet, nRet);
	
	// check the urgent data offset (server)
	ret = iEsockSuite->GetSocketHandle(iSockIndex3).GetOpt(KSOUrgentDataOffset, KSOLSocket, nRet);
	TESTEL(KErrNone == ret, ret);
	TESTEL(3 == nRet, nRet);
	
	// check the number of bytes to read before urgent data(server)
	ret = iEsockSuite->GetSocketHandle(iSockIndex3).GetOpt(KSOReadBytesPending, KSOLSocket, nRet);
	TESTEL(KErrNone == ret, ret);
	TESTEL(3 == nRet, nRet);
	
	// read the data - check only 3 bytes are read
	TBuf8<11> rbuf;
	iEsockSuite->GetSocketHandle(iSockIndex3).Read(rbuf, stat);
	User::WaitForRequest(stat);
	TESTEL(stat==KErrNone, stat.Int());
	TESTL(rbuf==_L8("123"));
	
	// check urgent data is available
	ret = iEsockSuite->GetSocketHandle(iSockIndex3).GetOpt(KSoTcpRcvAtMark, KSolInetTcp, nRet);
	TESTEL(KErrNone == ret, ret);
	TESTEL(1 == nRet, nRet);
	
	// read again - check the remaining data is read
	iEsockSuite->GetSocketHandle(iSockIndex3).Read(rbuf, stat);
	User::WaitForRequest(stat);
	TESTEL(stat==KErrNone, stat.Int());
	TESTL(rbuf==_L8("41234567890"));
	
	// check urgent data is not available
	ret = iEsockSuite->GetSocketHandle(iSockIndex3).GetOpt(KSoTcpRcvAtMark, KSolInetTcp, nRet);
	TESTEL(KErrNone == ret, ret);
	TESTEL(0 == nRet, nRet);
	
	// peek for urgent data (server)
	ret = iEsockSuite->GetSocketHandle(iSockIndex3).GetOpt(KSoTcpPeekUrgentData, KSolInetTcp, nRet);
	TESTEL(KErrNotFound == ret, ret);
	
	// read urgent data (server)
	ret = iEsockSuite->GetSocketHandle(iSockIndex3).GetOpt(KSoTcpReadUrgentData, KSolInetTcp, nRet);
	TESTEL(KErrNotFound == ret, ret);
	
	return EPass;
	}


// Test step 8.8
const TDesC& CEsockTest8_8::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test8.8");
	
	return ret;
	}

CEsockTest8_8::~CEsockTest8_8()
	{
	}

enum TVerdict CEsockTest8_8::easyTestStepPreambleL()
	{
	TInt connections;
	TRAPD(ret, connections = OpenConnectionsL(_L("Test_8.8"), iSockIndex2, iSockIndex3));
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

enum TVerdict CEsockTest8_8::easyTestStepL()
	{
	TESTL(EPass == TestStepResult());
	
	// Urgent data test - 8000 bytes (7998th is urgent) read in-band
	
	// for server enable KSoTcpOobInline
	TInt ret = iEsockSuite->GetSocketHandle(iSockIndex3).SetOpt(KSoTcpOobInline, KSolInetTcp, 1);
	TESTEL(KErrNone == ret, ret);
	
	// send data from client - 8000 bytes with 7998th marked urgent
	TBuf8<100> wbuf;
	TRequestStatus stat;
	for (TInt i=0 ; i<80 ; i++)
		{
		wbuf.SetMax();
		wbuf.Fill(TChar((TUint8)i)); // fill buffer with data
		
		if (i==79) // send 100 chars with 98th marked urgent
			{
			iEsockSuite->GetSocketHandle(iSockIndex2).Write(wbuf.Mid(0,97), stat);
			User::WaitForRequest(stat);
			TESTEL(stat==KErrNone, stat.Int());
			
			TESTL(iEsockSuite->GetSocketHandle(iSockIndex2).SetOpt(KSoTcpNextSendUrgentData, KSolInetTcp, 1)==KErrNone);
			iEsockSuite->GetSocketHandle(iSockIndex2).Write(_L8("?"), stat);
			User::WaitForRequest(stat);
			TESTEL(stat==KErrNone, stat.Int());
			
			iEsockSuite->GetSocketHandle(iSockIndex2).Write(_L8("yz"), stat);
			User::WaitForRequest(stat);
			TESTEL(stat==KErrNone, stat.Int());
			}
		else // send 100 chars
			{
			iEsockSuite->GetSocketHandle(iSockIndex2).Write(wbuf, stat);
			User::WaitForRequest(stat);
			TESTEL(stat==KErrNone, stat.Int());
			}
		}
	
	// check on server that urgent data is not available
	TInt nRet;
	ret = iEsockSuite->GetSocketHandle(iSockIndex3).GetOpt(KSoTcpRcvAtMark, KSolInetTcp, nRet);
	TESTEL(KErrNone == ret, ret);
	TESTEL(0 == nRet, nRet);
	
	User::After(1000000);
	
	// check the urgent data offset (server)
	ret = iEsockSuite->GetSocketHandle(iSockIndex3).GetOpt(KSOUrgentDataOffset, KSOLSocket, nRet);
	TESTEL(KErrNone == ret, ret);
	TESTEL(7997 == nRet, nRet);
	
	// check the number of bytes to read before urgent data (server)
	ret = iEsockSuite->GetSocketHandle(iSockIndex3).GetOpt(KSOReadBytesPending, KSOLSocket, nRet);
	TESTEL(KErrNone == ret, ret);
	TESTEL(7997 == nRet, nRet);
	
	// read the data - check all 7997 bytes are read
	TBuf8<100> rbuf;
	TInt count;
	for (count = 0; count < 7997;)
		{
		iEsockSuite->GetSocketHandle(iSockIndex3).Read(rbuf, stat);
		User::WaitForRequest(stat);
		TESTEL(stat==KErrNone, stat.Int());
		count+=rbuf.Length();
		if (count == 7900)
			rbuf.SetLength(97);
		}
	TESTEL(7997 == count, count);
	
	// check that urgent data is available
	ret = iEsockSuite->GetSocketHandle(iSockIndex3).GetOpt(KSoTcpRcvAtMark, KSolInetTcp, nRet);
	TESTEL(KErrNone == ret, ret);
	TESTEL(1 == nRet, nRet);
	
	// attempt to read the urgent data (server)
	TESTL(iEsockSuite->GetSocketHandle(iSockIndex3).GetOpt(KSoTcpReadUrgentData, KSolInetTcp, nRet)==KErrNotFound);
	
	// read data - check only a single byte is returned
	TBuf8<1> rbuf2;
	iEsockSuite->GetSocketHandle(iSockIndex3).Read(rbuf2, stat);
	User::WaitForRequest(stat);
	TESTEL(stat==KErrNone, stat.Int());
	TESTL(rbuf2==_L8("?"));
	
	// check that urgent data is not available
	ret = iEsockSuite->GetSocketHandle(iSockIndex3).GetOpt(KSoTcpRcvAtMark, KSolInetTcp, nRet);
	TESTEL(KErrNone == ret, ret);
	TESTEL(0 == nRet, nRet);
	
	// check the number of bytes to read (server)
	ret = iEsockSuite->GetSocketHandle(iSockIndex3).GetOpt(KSOReadBytesPending, KSOLSocket, nRet);
	TESTEL(KErrNone == ret, ret);
	TESTEL(2 == nRet, nRet);
	
	// check the urgent data offset (server)
	ret = iEsockSuite->GetSocketHandle(iSockIndex3).GetOpt(KSOUrgentDataOffset, KSOLSocket, nRet);
	TESTEL(KErrNone == ret, ret);
	TESTEL(0 == nRet, nRet);
	
	// read the rest of the data - expecting 'y' followed by 'z'
	iEsockSuite->GetSocketHandle(iSockIndex3).Read(rbuf2, stat);
	User::WaitForRequest(stat);
	TESTEL(stat==KErrNone, stat.Int());
	TESTL(rbuf2==_L8("y"));
	
	iEsockSuite->GetSocketHandle(iSockIndex3).Read(rbuf2, stat);
	User::WaitForRequest(stat);
	TESTEL(stat==KErrNone, stat.Int());
	TESTL(rbuf2==_L8("z"));
	
	return EPass;
	}
	
	
// Test step 8.9
const TDesC& CEsockTest8_9::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test8.9");
	
	return ret;
	}

CEsockTest8_9::~CEsockTest8_9()
	{
	}

enum TVerdict CEsockTest8_9::easyTestStepPreambleL()
	{
	TInt connections;
	TRAPD(ret, connections = OpenConnectionsL(_L("Test_8.9"), iSockIndex2, iSockIndex3));
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

enum TVerdict CEsockTest8_9::easyTestStepL()
	{
	TESTL(EPass == TestStepResult());
	
	// Urgent data test - send 1 urgent byte and then another read out-of-band
	
	// send 4 bytes of data from the client
	TRequestStatus stat;
	iEsockSuite->GetSocketHandle(iSockIndex2).Write(_L8("1234"), stat);
	User::WaitForRequest(stat);
	TESTEL(stat==KErrNone, stat.Int());
	
	// for client enable KSoTcpNextSendUrgentData
	TInt ret = iEsockSuite->GetSocketHandle(iSockIndex2).SetOpt(KSoTcpNextSendUrgentData, KSolInetTcp, 1);
	TESTEL(KErrNone == ret, ret);
	
	// send 4 more bytes
	iEsockSuite->GetSocketHandle(iSockIndex2).Write(_L8("5678"), stat);
	User::WaitForRequest(stat);
	TESTEL(stat==KErrNone, stat.Int());
	
	User::After(2000000);	// Per the outcome of DEF036402, the stack behaviour is not to guarantee that urgent data
	// forces segment send. Hence here we for 5 seconds to ensure that it does get sent
	
	// send 5 more bytes
	iEsockSuite->GetSocketHandle(iSockIndex2).Write(_L8("90abc"), stat);
	User::WaitForRequest(stat);
	TESTEL(stat==KErrNone, stat.Int());
	
	// enable KSoTcpNextSendUrgentData again
	ret = iEsockSuite->GetSocketHandle(iSockIndex2).SetOpt(KSoTcpNextSendUrgentData, KSolInetTcp, 1);
	TESTEL(KErrNone == ret, ret);
	
	// send 4 more bytes
	iEsockSuite->GetSocketHandle(iSockIndex2).Write(_L8("defg"), stat);
	User::WaitForRequest(stat);
	TESTEL(stat==KErrNone, stat.Int());
	
	// send a final 4 bytes
	iEsockSuite->GetSocketHandle(iSockIndex2).Write(_L8("hijk"), stat);
	User::WaitForRequest(stat);
	TESTEL(stat==KErrNone, stat.Int());
	
	User::After(1000000);
	
	// check on server that urgent data is not available
	TInt nRet;
	ret = iEsockSuite->GetSocketHandle(iSockIndex3).GetOpt(KSoTcpRcvAtMark, KSolInetTcp, nRet);
	TESTEL(KErrNone == ret, ret);
	TESTEL(0 == nRet, nRet);
	
	// check the urgent data offset (server)
	ret = iEsockSuite->GetSocketHandle(iSockIndex3).GetOpt(KSOUrgentDataOffset, KSOLSocket, nRet);
	TESTEL(KErrNone == ret, ret);
	TESTEL(15 == nRet, nRet);
	
	// check the number of bytes to read before urgent data(server)
	ret = iEsockSuite->GetSocketHandle(iSockIndex3).GetOpt(KSOReadBytesPending, KSOLSocket, nRet);
	TESTEL(KErrNone == ret, ret);
	TESTEL(7 == nRet, nRet);
	
	// check KSoTcpNextSendUrgentData option is disabled (client)
	ret = iEsockSuite->GetSocketHandle(iSockIndex2).GetOpt(KSoTcpNextSendUrgentData, KSolInetTcp, nRet);
	TESTEL(KErrNone == ret, ret);
	TESTEL(0 == nRet, nRet);
	
	// Attempt to read 10 bytes of data (server). Due to urgent data should complete with KErrUrgentData and
	// 7 bytes
	TBuf8<10> rbuf;
	iEsockSuite->GetSocketHandle(iSockIndex3).Read(rbuf, stat);
	User::WaitForRequest(stat);
	TESTEL(stat==KErrUrgentData, stat.Int());
	TESTL(rbuf==_L8("1234567"));
	
	// the test code below matches the current stack behaviour but it doesn't seem very sensible/intuitive
	// In essence it is that the position of the urgent data mark is that of the most recently received urgent data
	// even though the stack tracks several urgent data marks in the stream. Specifically, the above test indicates
	// urgent data at byte 8, but the RcvAtMark is not set, the offset points to the second mark, and the byte at
	// that position is the second urgent write's "g". If the second urgent write is suppressed then here we see
	// that we are at the urgent mark, its offset is 0, and the byte is "8".
	
	// check that urgent data is not available
	ret = iEsockSuite->GetSocketHandle(iSockIndex3).GetOpt(KSoTcpRcvAtMark, KSolInetTcp, nRet);
	TESTEL(KErrNone == ret, ret);
	Logger().WriteFormat(_L("TcpRcvAtMark=%d."), nRet);
	TESTL(nRet==0);

	ret = iEsockSuite->GetSocketHandle(iSockIndex3).GetOpt(KSOUrgentDataOffset, KSOLSocket, nRet);
	TESTEL(KErrNone == ret, ret);
	Logger().WriteFormat(_L("KSOUrgentDataOffset=%d."), nRet);
	TESTL(nRet==8);

	ret = iEsockSuite->GetSocketHandle(iSockIndex3).GetOpt(KSOReadBytesPending, KSOLSocket, nRet);
	TESTEL(KErrNone == ret, ret);
	Logger().WriteFormat(_L("KSOReadBytesPending=%d."), nRet);
	TESTL(nRet==8);
	
	iEsockSuite->GetSocketHandle(iSockIndex3).Read(rbuf, stat);
	User::WaitForRequest(stat);
	TESTEL(stat==KErrNone, stat.Int());
	TESTL(rbuf==_L8("90abcdef"));

	ret = iEsockSuite->GetSocketHandle(iSockIndex3).GetOpt(KSoTcpRcvAtMark, KSolInetTcp, nRet);
	TESTEL(KErrNone == ret, ret);
	Logger().WriteFormat(_L("KSoTcpRcvAtMark=%d."), nRet);
	TESTL(nRet==1);

	ret = iEsockSuite->GetSocketHandle(iSockIndex3).GetOpt(KSOUrgentDataOffset, KSOLSocket, nRet);
	TESTEL(KErrNone == ret, ret);
	Logger().WriteFormat(_L("KSOUrgentDataOffset=%d."), nRet);
	TESTL(nRet==0);

	ret = iEsockSuite->GetSocketHandle(iSockIndex3).GetOpt(KSoTcpReadUrgentData, KSolInetTcp, nRet);
	TESTEL(KErrNone == ret, ret);
	Logger().WriteFormat(_L("KSoTcpReadUrgentData=%d."), nRet);
	TESTL(nRet=='g');
	
	User::After(1000);

	ret = iEsockSuite->GetSocketHandle(iSockIndex3).GetOpt(KSOReadBytesPending, KSOLSocket, nRet);
	TESTEL(KErrNone == ret, ret);
	Logger().WriteFormat(_L("KSOReadBytesPending=%d."), nRet);
	TESTL(nRet==4);
	
	// read the remaining data
	TBuf8<4> rbuf2;
	iEsockSuite->GetSocketHandle(iSockIndex3).Read(rbuf2, stat);
	User::WaitForRequest(stat);
	TESTEL(stat==KErrNone, stat.Int());
	TESTL(rbuf2==_L8("hijk"));
	
	return EPass;
	}
	
	
// Test step 8.10
const TDesC& CEsockTest8_10::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test8.10");
	
	return ret;
	}

CEsockTest8_10::~CEsockTest8_10()
	{
	}

enum TVerdict CEsockTest8_10::easyTestStepL()
	{
	TInetAddr addrLocal, addrRemote;
	TInt sockIndex1, sockIndex2, sockIndex3;
	
	// get local ip address
	TESTL(GetIpAddressFromConfig(_L("Test_8.10"), _L("ipAddressLocal"), addrLocal));
	
	// get ip address to connect to (usually loopback)
	TESTL(GetIpAddressFromConfig(_L("Test_8.10"), _L("ipAddressRemote"), addrRemote));
	addrRemote.SetPort(1);
	
	// open socket and listen for connect requests
	TESTL(KErrNone == OpenListeningSocketL(addrLocal, sockIndex1));
	
	RSocket sock;
	TInt nRet = sock.Open(iEsockSuite->iSocketServer, KAfInet, KSockStream, KProtocolInetTcp);
	TESTEL(KErrNone == nRet, nRet);
	
	// set receive window size
	TInt ret = sock.SetOpt(KSoTcpRecvWinSize, KSolInetTcp, 20);
	TESTEL(KErrNone == ret, ret);
	
	TRequestStatus stat;
	sock.Connect(addrRemote, stat);
	User::WaitForRequest(stat);
	TESTEL(stat==KErrNone, stat.Int());
	
	sockIndex2 = iEsockSuite->AddSocketToListL(sock);
	
	// accept connect request
	TESTL(KErrNone == AcceptConnectionL(sockIndex3, sockIndex1));
	
	// write 35 bytes (server to client)
	iEsockSuite->GetSocketHandle(sockIndex3).Write(_L8("abcdefghijklmnopqrstuvwxyz012345678"), stat);
	User::WaitForRequest(stat);
	TESTEL(stat==KErrNone, stat.Int());
	
	// client has nothing to read
	iEsockSuite->GetSocketHandle(sockIndex2).GetOpt(KSoTcpReadBytesPending, KSolInetTcp, nRet);
	TESTEL(0 == nRet, nRet);
	
	// server has data buffered for sending
	iEsockSuite->GetSocketHandle(sockIndex3).GetOpt(KSoTcpSendBytesPending, KSolInetTcp, nRet);
	TESTEL(35 == nRet, nRet);
	
	// write another 35 bytes
	iEsockSuite->GetSocketHandle(sockIndex3).Write(_L8("abcdefghijklmnopqrstuvwxyz012345678"), stat);
	User::WaitForRequest(stat);
	TESTEL(stat==KErrNone, stat.Int());
	
	// client has only 20 bytes to read
	iEsockSuite->GetSocketHandle(sockIndex2).GetOpt(KSoTcpReadBytesPending, KSolInetTcp, nRet);
	TESTEL(20 == nRet, nRet);
	
	// server has remainder buffered
	iEsockSuite->GetSocketHandle(sockIndex3).GetOpt(KSoTcpSendBytesPending, KSolInetTcp, nRet);
	TESTEL(50 == nRet, nRet);
	
	// read on client
	TBuf8<100> rbuf;
	TSockXfrLength len;
	iEsockSuite->GetSocketHandle(sockIndex2).RecvOneOrMore(rbuf, 0, stat, len);
	User::WaitForRequest(stat);
	TESTEL(stat==KErrNone, stat.Int());
	TESTEL(20 == len(), len());
	
	// client has only 20 bytes to read
	iEsockSuite->GetSocketHandle(sockIndex2).GetOpt(KSoTcpReadBytesPending, KSolInetTcp, nRet);
	TESTEL(20 == nRet, nRet);
	
	// server has remainder buffered
	iEsockSuite->GetSocketHandle(sockIndex3).GetOpt(KSoTcpSendBytesPending, KSolInetTcp, nRet);
	TESTEL(30 == nRet, nRet);
	
	iEsockSuite->GetSocketHandle(sockIndex2).RecvOneOrMore(rbuf, 0, stat, len);
	User::WaitForRequest(stat);
	TESTEL(stat==KErrNone, stat.Int());
	
	return EPass;
	}



// Test step 8.13
const TDesC& CEsockTest8_13::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test8.13");
	
	return ret;
	}

CEsockTest8_13::~CEsockTest8_13()
	{
	}

enum TVerdict CEsockTest8_13::easyTestStepL()
	{
	// Reuse of address test
	
	// get ip address
	TInetAddr addr;
	TESTL(GetIpAddressFromConfig(_L("Test_8.13"), _L("ipAddress"), addr));
	
	// get port number
	TInt port;
	TESTL(GetIntFromConfig(_L("Test_8.13"), _L("port"), port));
	
	RSocket sock1;
	CleanupClosePushL(sock1);
	TInt nRet = sock1.Open(iEsockSuite->iSocketServer, KAfInet, KSockStream, KProtocolInetTcp);
	TESTEL(KErrNone == nRet, nRet);
	// store socket handle
	TInt sockIndex1 = iEsockSuite->AddSocketToListL(sock1);
	CleanupStack::Pop();

	RSocket sock2;
	CleanupClosePushL(sock2);
	nRet = sock2.Open(iEsockSuite->iSocketServer, KAfInet, KSockStream, KProtocolInetTcp);
	TESTEL(KErrNone == nRet, nRet);
	// store socket handle
	TInt sockIndex2 = iEsockSuite->AddSocketToListL(sock2);
	CleanupStack::Pop();
	
	// set the port number
	addr.SetPort(port);
	
	// bind the socket
	nRet = sock1.Bind(addr);
	TESTEL(nRet==KErrNone, nRet);
	
	// bind the socket to the same address - check for failure
	nRet = sock2.Bind(addr);
	TESTEL(nRet==KErrInUse, nRet);
	
	// close the 2 sockets
	iEsockSuite->CloseSocket(sockIndex2);
	iEsockSuite->CloseSocket(sockIndex1);
	
	CleanupClosePushL(sock1);
	nRet = sock1.Open(iEsockSuite->iSocketServer, KAfInet, KSockStream, KProtocolInetTcp);
	TESTEL(KErrNone == nRet, nRet);
	sockIndex1=iEsockSuite->AddSocketToListL(sock1);
	CleanupStack::Pop();
	
	CleanupClosePushL(sock2);
	nRet = sock2.Open(iEsockSuite->iSocketServer, KAfInet, KSockStream, KProtocolInetTcp);
	TESTEL(KErrNone == nRet, nRet);
	sockIndex2=iEsockSuite->AddSocketToListL(sock2);
	CleanupStack::Pop();
	
	// for each of the 2 sockets enable the KSoReuseAddr option
	TESTL(sock1.SetOpt(KSoReuseAddr, KSolInetIp, 1)==KErrNone);
	TESTL(sock2.SetOpt(KSoReuseAddr, KSolInetIp, 1)==KErrNone);
	
	// bind the first socket
	nRet = sock1.Bind(addr);
	TESTEL(nRet==KErrNone, nRet);
	
	// bind the second socket - this time it is successful
	nRet = sock2.Bind(addr);
	TESTEL(nRet==KErrNone, nRet);
	
	// close the sockets  (run test 1.15 to close any sockets)
	
	return EPass;
	}


// Test step 8.14
const TDesC& CEsockTest8_14::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test8.14");
	
	return ret;
	}

CEsockTest8_14::~CEsockTest8_14()
	{
	}

enum TVerdict CEsockTest8_14::easyTestStepL()
	{
	TInetAddr addrLocal, addrRemote;
	TInt sockIndex1, sockIndex2, sockIndex3;
	
	// 2MSL wait test
	
	// get local ip address
	TESTL(GetIpAddressFromConfig(_L("Test_8.14"), _L("ipAddressLocal"), addrLocal));
	
	// get ip address to connect to (usually loopback)
	TESTL(GetIpAddressFromConfig(_L("Test_8.14"), _L("ipAddressRemote"), addrRemote));
	
	// open socket and listen for connect requests
	TESTL(KErrNone == OpenListeningSocketL(addrLocal, sockIndex1));
	
	// open active socket and make connect request
	TESTL(KErrNone == OpenActiveSocketL(addrRemote, sockIndex2));
	
	// accept connect request
	TESTL(KErrNone == AcceptConnectionL(sockIndex3, sockIndex1));
	
	// close the sockets - should now be in 2MSL wait state
	iEsockSuite->GetSocketHandle(sockIndex3).Close();
	iEsockSuite->GetSocketHandle(sockIndex2).Close();
	iEsockSuite->GetSocketHandle(sockIndex1).Close();
	
	// open a TCP socket
	RSocket sock;
	CleanupClosePushL(sock);
	TInt nRet = sock.Open(iEsockSuite->iSocketServer, KAfInet, KSockStream, KProtocolInetTcp);
	TESTEL(KErrNone == nRet, nRet);
	
	// bind the socket to the same address - check for failure
	addrLocal.SetPort(1);
	nRet = sock.Bind(addrLocal);
	TESTEL(nRet==KErrInUse, nRet);
	
	// check that the bind is successful after 2MSL wait period has expired
	do
		{
		User::After(1000000); // 1 second delay
		nRet = sock.Bind(addrLocal); // keep trying
		} while (nRet==KErrInUse);
		
	TESTEL(nRet==KErrNone, nRet); // check bind is successful after the delay
	
	// close the socket???
	CleanupStack::PopAndDestroy();
	
	// open socket and listen for connect requests
	TESTL(KErrNone == OpenListeningSocketL(addrLocal, sockIndex1));
	
	// open active socket and make connect request
	TESTL(KErrNone == OpenActiveSocketL(addrRemote, sockIndex2));
	
	// accept connect request
	TESTL(KErrNone == AcceptConnectionL(sockIndex3, sockIndex1));
	
	// close the client and server sockets - leaving them in a 2MSL wait state
	iEsockSuite->GetSocketHandle(sockIndex3).Close();
	iEsockSuite->GetSocketHandle(sockIndex2).Close();
	
	// open a TCP socket
	CleanupClosePushL(sock);
	nRet = sock.Open(iEsockSuite->iSocketServer, KAfInet, KSockStream, KProtocolInetTcp);
	TESTEL(KErrNone == nRet, nRet);
	
	// enable the KSoReuseAddr socket option
	TESTL(sock.SetOpt(KSoReuseAddr, KSolInetIp, 1)==KErrNone);
	
	// bind the socket to the same address - bind is now successful
	nRet = sock.Bind(addrLocal);
	TESTEL(nRet==KErrNone, nRet);
	
	// attempt to connect - check for failure
	TRequestStatus stat;
	addrRemote.SetPort(1);
	sock.Connect(addrRemote, stat);
	User::WaitForRequest(stat);
	TESTEL(stat==KErrInUse, stat.Int());  // still in 2MSL wait state
	
	// enable the KSoTcpAsync2MslWait option
	TESTL(sock.SetOpt(KSoTcpAsync2MslWait, KSolInetTcp, 1)==KErrNone);
	
	// attempt connection again - should succeed now
	sock.Connect(addrRemote, stat);
	User::WaitForRequest(stat);
	TESTEL(stat==KErrNone, stat.Int());
	
	// close the sockets
	CleanupStack::PopAndDestroy();
	iEsockSuite->GetSocketHandle(sockIndex1).Close();
	
	return EPass;
	}
	
	
// Test step 8.15
const TDesC& CEsockTest8_15::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test8.15");
	
	return ret;
	}

CEsockTest8_15::~CEsockTest8_15()
	{
	}

enum TVerdict CEsockTest8_15::easyTestStepL()
	{
	// Number of sockets test
	RSocket sock1, sock2, sock3;
	CleanupClosePushL(sock3);
	CleanupClosePushL(sock2);
	CleanupClosePushL(sock1);
	
	// open a TCP socket
	TInt nRet = sock1.Open(iEsockSuite->iSocketServer, KAfInet, KSockStream, KProtocolInetTcp);
	TESTEL(KErrNone == nRet, nRet);
	
	// get the KSoTcpNumSockets socket option
	TInt ret = sock1.GetOpt(KSoTcpNumSockets, KSolInetTcp, nRet);
	TESTEL(KErrNone == ret, ret);
	TESTEL(nRet == 1, nRet);
	
	// open another TCP socket
	nRet = sock2.Open(iEsockSuite->iSocketServer, KAfInet, KSockStream, KProtocolInetTcp);
	TESTEL(KErrNone == nRet, nRet);
	
	// get the KSoTcpNumSockets socket option for the second socket
	ret = sock2.GetOpt(KSoTcpNumSockets, KSolInetTcp, nRet);
	TESTEL(KErrNone == ret, ret);
	TESTEL(nRet == 2, nRet);
	
	// open a UDP socket
	nRet = sock3.Open(iEsockSuite->iSocketServer, KAfInet, KSockDatagram, KProtocolInetUdp);
	TESTEL(KErrNone == nRet, nRet);
	
	// get the KSoTcpNumSockets socket option for the UDP socket - not supported
	ret = sock3.GetOpt(KSoTcpNumSockets, KSolInetTcp, nRet);
	TESTEL(KErrNotSupported == ret, ret);
	
	// get the KSoTcpNumSockets socket option for the first TCP socket
	ret = sock1.GetOpt(KSoTcpNumSockets, KSolInetTcp, nRet);
	TESTEL(KErrNone == ret, ret);
	TESTEL(nRet == 2, nRet);
	
	// close the first TCP socket
	CleanupStack::PopAndDestroy(1, &sock1);
	
	// get the KSoTcpNumSockets socket option for the second TCP socket
	ret = sock2.GetOpt(KSoTcpNumSockets, KSolInetTcp, nRet);
	TESTEL(KErrNone == ret, ret);
	TESTEL(nRet == 1, nRet);
	
	// close the remaining TCP socket
	CleanupStack::PopAndDestroy(1, &sock2);
	
	// close the UDP socket
	CleanupStack::PopAndDestroy(1, &sock3);
	
	return EPass;
	}


// Test step 8.16
const TDesC& CEsockTest8_16::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test8.16");
	
	return ret;
	}

CEsockTest8_16::~CEsockTest8_16()
	{
	}

enum TVerdict CEsockTest8_16::easyTestStepL()
	{
	// socket listening test
	
	TInetAddr addrLocal, addrRemote;
	// get local ip address
	TESTL(GetIpAddressFromConfig(_L("Test_8.16"), _L("ipAddressLocal"), addrLocal));
	
	// get ip address to connect to (usually loopback)
	TESTL(GetIpAddressFromConfig(_L("Test_8.16"), _L("ipAddressRemote"), addrRemote));
	
	// get port number
	TInt port;
	TESTL(GetIntFromConfig(_L("Test_8.16"), _L("port"), port));
	
	// open a TCP socket
	RSocket sock1;
	CleanupClosePushL(sock1);
	TInt nRet = sock1.Open(iEsockSuite->iSocketServer, KAfInet, KSockStream, KProtocolInetTcp);
	TESTEL(KErrNone == nRet, nRet);
	
	// get the KSoTcpListening socket option - indicates socket is not listening
	TInt ret = sock1.GetOpt(KSoTcpListening, KSolInetTcp, nRet);
	TESTEL(KErrNone == ret, ret);
	TESTEL(0 == nRet, nRet);
	
	// bind the socket to local address
	addrLocal.SetPort(port);
	nRet = sock1.Bind(addrLocal);
	TESTEL(KErrNone == nRet, nRet);
	
	// listen on the socket
	nRet = sock1.Listen(5);
	TESTEL(KErrNone == nRet, nRet);
	
	// get the KSoTcpListening socket option - indicates socket is listening
	ret = sock1.GetOpt(KSoTcpListening, KSolInetTcp, nRet);
	TESTEL(KErrNone == ret, ret);
	TESTEL(1 == nRet, nRet);
	
	// open another socket (client)
	RSocket sock2;
	CleanupClosePushL(sock2);
	nRet = sock2.Open(iEsockSuite->iSocketServer, KAfInet, KSockStream, KProtocolInetTcp);
	TESTEL(KErrNone == nRet, nRet);
	
	// connect via loopback
	TRequestStatus stat;
	addrRemote.SetPort(port);
	sock2.Connect(addrRemote, stat);
	User::WaitForRequest(stat);
	TESTEL(stat==KErrNone, stat.Int());
	
	// open a blank socket
	RSocket sock3;
	CleanupClosePushL(sock3);
	nRet = sock3.Open(iEsockSuite->iSocketServer);
	TESTEL(KErrNone == nRet, nRet);
	
	// accept the connection
	sock1.Accept(sock3, stat);
	User::WaitForRequest(stat);
	TESTEL(stat==KErrNone, stat.Int());
	
	// get the KSoTcpListening socket option for the listening socket
	ret = sock1.GetOpt(KSoTcpListening, KSolInetTcp, nRet);
	TESTEL(KErrNone == ret, ret);
	TESTEL(1 == nRet, nRet);
	
	// get the KSoTcpListening socket option for the client socket
	ret = sock2.GetOpt(KSoTcpListening, KSolInetTcp, nRet);
	TESTEL(KErrNone == ret, ret);
	TESTEL(0 == nRet, nRet);
	
	// get the KSoTcpListening socket option for the server socket
	ret = sock3.GetOpt(KSoTcpListening, KSolInetTcp, nRet);
	TESTEL(KErrNone == ret, ret);
	TESTEL(0 == nRet, nRet);

	CleanupStack::PopAndDestroy(3, &sock1);
	return EPass;
	}


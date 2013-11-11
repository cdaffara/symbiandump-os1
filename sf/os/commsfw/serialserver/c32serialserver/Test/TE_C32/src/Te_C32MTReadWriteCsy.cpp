// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

#include <e32test.h>
#include <e32cmn.h>
#include "Te_C32MultiThreading.h"
#include "C32COMM.H"
#include "dummy.h"

// Test case id NET-C32-I-0011-HP
//Sending data through CSYs which is loaded in Dealer Thread
/**
 * Constructor
 */

CSendDatainDealerThr::CSendDatainDealerThr()
	{
	SetTestStepName(_L("SendDatainDealerThr"));
	}

TInt CSendDatainDealerThr::DataReadThread(TAny* aArg)
/**
 * Thread which attempts to read a chunk of data from loopback port
 */
	{
	CSendDatainDealerThr* stepTest = reinterpret_cast<CSendDatainDealerThr*> (aArg);

	_LIT(KPortName1,"LOOPBACK::1");
	_LIT(KCSYName,"LOOPBACK.CSY");

	//Creating a RcommSession, connecting to the C32 server
	RCommServ commServ;
	TInt res = commServ.Connect();
	if(res != KErrNone)
		{
		stepTest->iState1 = __LINE__ - 2;
		return(res);
		}
	//Loading communication system(CSY)
	res=commServ.LoadCommModule(KCSYName);
	if(res != KErrNone)
		{
		stepTest->iState2 = __LINE__ - 2;
		commServ.Close();
		return(res);
		}
	//Open Loopback port for reading and writing
	res=stepTest->iSerialPortList[0].Open(commServ, KPortName1, ECommShared);
	if(res != KErrNone)
		{
		stepTest->iState3 = __LINE__ - 2;
		commServ.Close();
		return(res);
		}

	TRequestStatus stat0;
	TBuf8<19> readBuf;

	for (TInt i=0; i<=200; i++)
		{
		//Reading the data from the opened port
		stepTest->iSerialPortList[0].ReadOneOrMore(stat0, readBuf);
		User::WaitForRequest(stat0);
		res = stat0.Int();
		if(res!=KErrNone)
			{
			stepTest->iState4 = __LINE__ - 2;
			stepTest->iSerialPortList[0].Close();
			commServ.Close();
			return(res);
			}
		}

	stepTest->iSem.Signal();
	stepTest->iSerialPortList[0].Close();

	commServ.Close();
	return KErrNone;

	}
/**
 * @see CSendDatainDealerThr test case NET-C32-I-0011-HP
 *
 * doTestStep function does the below action
 * Connect to a C32 server
 * Loading CSY in Dealer thread Read and write date into it
 * close the connection to C32
 */
TVerdict CSendDatainDealerThr::doTestStepL()
	{

	SetTestStepResult(EPass);
	
	INFO_PRINTF1(_L("=========================================================="));
	INFO_PRINTF1(_L("Sending data through CSYs which is loaded in Dealer Thread"));
	INFO_PRINTF1(_L("=========================================================="));

	_LIT(KCsyName,"LOOPBACK.CSY");
	_LIT(KPortName0,"LOOPBACK::0");
	_LIT8(KServerText,"Test Loopback : ");

	iSem.CreateLocal(0);
	//Creating a RcommSession, connecting to the C32 server
	RCommServ commServ;
	TInt ret = commServ.Connect();
	if(ret != KErrNone)
		{
		INFO_PRINTF2(_L("ERROR: could not connect RCommServ %d"), ret);
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	else
		{
		INFO_PRINTF2(_L("OK.'%d'Connection established"), ret);
		}
	ret=commServ.LoadCommModule(KCsyName);
	if(ret != KErrNone)
		{
		commServ.Close();
		}
	TESTCHECKL(ret,KErrNone);
	//Open LoopBack port for reading and writing
	ret=iSerialPortList[1].Open(commServ,KPortName0,ECommShared);
	if(ret != KErrNone)
		{
		commServ.Close();
		}
	TESTCHECKL(ret,KErrNone);
	//Creating a thread for reading data from Loopback port
	RThread readThread;

	TInt res=readThread.Create(_L("DataReadThread"),DataReadThread,KDefaultStackSize,KDefaultHeapSizeThread,KDefaultHeapSizeThread,this);
	if(ret != KErrNone)
		{
		iSerialPortList[1].Close();
		commServ.Close();
		}
	TESTCHECKL(res, KErrNone);

	TRequestStatus stat;
	readThread.Logon(stat);

	readThread.Resume();
	
	TRequestStatus stat0;
	//Writing a long data into the opened port
	for (TInt i=0; i<=200; i++)
		{
		TBuf8<20> writeBuf;
		writeBuf.Append(KServerText);
		writeBuf.AppendNum(i);

		if(i<=9)
			{
			writeBuf.AppendFill(' ',2);
			}
		else if (i<=99)
			{
			writeBuf.AppendFill(' ',1);
			}

		iSerialPortList[1].Write(stat0, writeBuf);
		User::WaitForRequest(stat0);
		TESTCHECKL(stat0.Int(), KErrNone);
		}

	iSem.Wait();

	User::WaitForRequest(stat);
	TESTCHECKL(stat.Int(), KErrNone);
	
	iSem.Close();
	CLOSE_AND_WAIT(readThread);
	iSerialPortList[1].Close();
	CLOSE_AND_WAIT(commServ);

	if(iState1 != KErrNone)
		{
		INFO_PRINTF2(_L("Error occured, iState1, in DataReadThread at line %d, expected KErrNone."), iState1);
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	if(iState2 != KErrNone)
		{
		INFO_PRINTF2(_L("Error occured, iState2, in DataReadThread at line %d, expected KErrNone."), iState2);
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	if(iState3 != KErrNone)
		{
		INFO_PRINTF2(_L("Error occured, iState3, in DataReadThread at line %d, expected KErrNone."), iState3);
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	if(iState4 != KErrNone)
		{
		INFO_PRINTF2(_L("Error occured, iState4, in DataReadThread at line %d, expected KErrNone."), iState4);
		SetTestStepResult(EFail);
		return TestStepResult();
		}

	return TestStepResult();

	}


// Test case id NET-C32-I-0012-HP
//Sending data through CSYs which is loaded in Player Thread
/**
 * Constructor
 */
CSendDatainPlayerThr::CSendDatainPlayerThr()
	{
	SetTestStepName(_L("SendDatainPlayerThr"));
	}

TInt CSendDatainPlayerThr::DataReadThread(TAny* aArg)
/**
 * Thread which attempts to read a chunk of data from in player thread
 */
	{
	CSendDatainPlayerThr* stepTest = reinterpret_cast<CSendDatainPlayerThr*> (aArg);
	_LIT(KPortName1,"LOOPBACK::1");
	_LIT(KCSYName,"LOOPBACK.CSY");

	//Creating a RcommSession, connecting to the C32 server
	RCommServ commServ;
	TInt res = commServ.Connect();
	if(res != KErrNone)
		{
		stepTest->iState1 = __LINE__ - 2;
		return(res);
		}
	//Loading communication system(CSY)
	res = commServ.LoadCommModule(KCSYName);
	if(res != KErrNone)
		{
		stepTest->iState2 = __LINE__ - 2;
		commServ.Close();
		return(res);
		}
	//Open Loopback port for reading and writing
	res = stepTest->iSerialPortList[0].Open(commServ, KPortName1, ECommShared);
	if(res != KErrNone)
		{
		stepTest->iState3 = __LINE__ - 2;
		commServ.Close();
		return(res);
		}
	//Writing a long data into the opened port
	TRequestStatus stat0;
	TBuf8<19> readBuf;
	//Writing a long data into the opened port
	for (TInt i=0; i<=200; i++)
		{
		//Reading the data from the opened port
		stepTest->iSerialPortList[0].ReadOneOrMore(stat0,readBuf);
		User::WaitForRequest(stat0);
		res = stat0.Int();
		if(res != KErrNone)
			{
			stepTest->iState4 = __LINE__ - 2;
			stepTest->iSerialPortList[0].Close();
			commServ.Close();
			return(res);
			}
		}

	stepTest->iSem.Signal();
	stepTest->iSerialPortList[0].Close();
	commServ.Close();
	return KErrNone;
	}

/**
 * @see CSendDatainPlayerThr test case NET-C32-I-0012-HP
 *
 * doTestStep function does the below action
 * Connect to a C32 server
 * Loading CSY in player thread, Read and write date into it
 * close the connection to C32
 */

TVerdict CSendDatainPlayerThr::doTestStepL()
	{
	INFO_PRINTF1(_L("==================================="));
	INFO_PRINTF1(_L("Sending data through CSYs which is loaded in Player Thread"));
	INFO_PRINTF1(_L("==================================="));

	_LIT(KCsyName,"LOOPBACK.CSY");
	_LIT(KPortName0,"LOOPBACK::0");
	_LIT8(KServerText,"Test Loopback : ");

	iSem.CreateLocal(0);
	//Creating a RcommSession, connecting to the C32 server
	RCommServ commServ;
	TInt ret = commServ.Connect();
	if(ret != KErrNone)
		{
		INFO_PRINTF2(_L("ERROR: could not connect RCommServ %d"), ret);
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	else
		{
		INFO_PRINTF2(_L("OK.'%d'Connection established"), ret);
		}

	ret=commServ.LoadCommModule(KCsyName);
	TESTCHECKL(ret,KErrNone);
	//Open LoopBack port for reading and writing
	ret=iSerialPortList[1].Open(commServ, KPortName0, ECommShared);
	if(ret != KErrNone)
		{
		commServ.Close();
		}
	TESTCHECKL(ret,KErrNone);
	//Creating a thread for reading data from Loopback port
	RThread readThread;
	TInt res = readThread.Create(_L("DataReadThread"), DataReadThread, KDefaultStackSize, KDefaultHeapSizeThread, KDefaultHeapSizeThread, this);
	TESTCHECKL(res, KErrNone);

	TRequestStatus stat;
	readThread.Logon(stat);

	readThread.Resume();

	TRequestStatus stat0;
	//Writing a long data into the opened port
	for (TInt i=0; i<=200; i++)
		{
		TBuf8<20> writeBuf;
		writeBuf.Append(KServerText);
		writeBuf.AppendNum(i);

		if(i<=9)
			{
			writeBuf.AppendFill(' ',2);
			}
		else if (i<=99)
			{
			writeBuf.AppendFill(' ',1);
			}

		iSerialPortList[1].Write(stat0,writeBuf);
		User::WaitForRequest(stat0);
		TESTCHECKL(stat0.Int(), KErrNone);
		}

	iSem.Wait();

	User::WaitForRequest(stat);
	TESTCHECKL(stat.Int(), KErrNone);

	iSem.Close();
	CLOSE_AND_WAIT(readThread);
	iSerialPortList[1].Close();
	commServ.Close();

	if(iState1 != KErrNone)
		{
		INFO_PRINTF2(_L("Error occured, iState1, in DataReadThread at line %d, expected KErrNone."), iState1);
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	if(iState2 != KErrNone)
		{
		INFO_PRINTF2(_L("Error occured, iState2, in DataReadThread at line %d, expected KErrNone."), iState2);
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	if(iState3 != KErrNone)
		{
		INFO_PRINTF2(_L("Error occured, iState3, in DataReadThread at line %d, expected KErrNone."), iState3);
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	if(iState4 != KErrNone)
		{
		INFO_PRINTF2(_L("Error occured, iState4, in DataReadThread at line %d, expected KErrNone."), iState4);
		SetTestStepResult(EFail);
		return TestStepResult();
		}

	return TestStepResult();

	}


//Test case id NET-C32-I-0017-HP
// Reading and writing data in both Dealer and Player thread

CDataReadDealerPlayer::CDataReadDealerPlayer()
/**
 * Constructor
 */
	{
	SetTestStepName(_L("DataReadDealerPlayer"));
	}
/**
 * @see CDataReadDealerPlayer test case NET-C32-I-0017-HP
 *
 * Session function does the below action
 * Connect to a C32 server
 * Load CSY in player thread, Open ports read and write data into it
 * close the connection to C32
 */
TInt CDataReadDealerPlayer::Session(TAny* aArg)
/**
 * Thread which attempts get number of CSY loaded from other session
 */
	{

	CDataReadDealerPlayer* stepTest = reinterpret_cast<CDataReadDealerPlayer*> (aArg);

	_LIT(KCsyName,"DummyCsy1");
	_LIT(KPortName0,"DummyCsy1::6");
	_LIT(KPortName1,"DummyCsy1::0");
    TRequestStatus stat0;
	TRequestStatus stat1;


	//Creating a RcommSession, connecting to the C32 server
	RCommServ commServ;
	TInt ret = commServ.Connect();
	if(ret != KErrNone)
		{
		stepTest->iState[0] = __LINE__ - 2;
		return(ret);
		}
	ret = commServ.LoadCommModule(KCsyName);
	if(ret != KErrNone)
		{
		stepTest->iState[1] = __LINE__ - 2;
		commServ.Close();
		return(ret);
		}

	//Open LoopBack port for reading and writing
	ret = stepTest->iPortList[0].Open(commServ, KPortName0, ECommShared, ECommRoleDTE);
	if(ret != KErrNone)
		{
		stepTest->iState[2] = __LINE__ - 2;
		commServ.Close();
		return(ret);
		}

	ret = stepTest->iPortList[1].Open(commServ, KPortName1, ECommShared, ECommRoleDTE);
	if(ret!=KErrNone)
		{
		stepTest->iState[3] = __LINE__ - 2;
		stepTest->iPortList[0].Close();
		commServ.Close();
		return(ret);
		}

	TBuf8<82> outbuf;
	_LIT8(KServerText,"This string has to be exactly like this othervice write returns KErrCommsLineFail.");
	outbuf.Copy(KServerText);

	// testing write
	stepTest->iPortList[0].Write(stat0, 5000000L, outbuf);
	User::WaitForRequest(stat0);
	ret = stat0.Int();
	if(stat0 != KErrNone)
		{
		stepTest->iState[4] = __LINE__ - 2;
		stepTest->iPortList[0].Close();
		stepTest->iPortList[1].Close();
		commServ.Close();
		return ret;
		}

	TBuf8<82> inbuf;
	stepTest->iPortList[1].Read(stat1, inbuf);
	User::WaitForRequest(stat1);
	ret = stat1.Int();
	if(stat1 != KErrNone)
		{
		stepTest->iState[5] = __LINE__ - 2;

		stepTest->iPortList[0].Close();
		stepTest->iPortList[1].Close();
		commServ.Close();

		return ret;
		}


	stepTest->iPortList[0].Close();
	stepTest->iPortList[1].Close();
	commServ.Close();


	return KErrNone;
	}
/**
 * @see CDataReadDealerPlayer test case NET-C32-I-0017-HP
 *
 * doTestStepL virtual function does the below action
 * Connect to a C32 server
 * Load CSY in Dealer Thread, Open the ports Read and Write on it
 * close the connection to C32
 */
TVerdict CDataReadDealerPlayer::doTestStepL()
	{
	INFO_PRINTF1(_L("=========================================================="));
	INFO_PRINTF1(_L("Sending data through CSYs which are loaded in Dealer Thread"));
	INFO_PRINTF1(_L("=========================================================="));

	_LIT(KCsyName,"LOOPBACK.CSY");
	_LIT(KPortName0,"LOOPBACK::0");
	_LIT(KPortName1,"LOOPBACK::1");
	_LIT8(KServerText,"C32DealerThread:");

	//Creating a thread for reading data from Loopback port
	RThread SessionThread;
    TInt res=SessionThread.Create(_L("SessionThread"), Session, KDefaultStackSize, KDefaultHeapSizeThread, KDefaultHeapSizeThread, this);
	TESTCHECKL(res, KErrNone);

	//Requests notification when thread dies
	TRequestStatus stat;
	SessionThread.Logon(stat);
	//Makes the thread eligible for execution.
	SessionThread.Resume();

	//Creating a RcommSession, connecting to the C32 server
	RCommServ commServ;
	TInt ret = commServ.Connect();
	if(ret != KErrNone)
		{
		INFO_PRINTF2(_L("Connect to C32 server from session 1 failed %d"), ret);
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	else
		{
		INFO_PRINTF2(_L("OK.'%d'Connection established"), ret);
		}

	ret = commServ.LoadCommModule(KCsyName);
	if(ret != KErrNone)
		{
		commServ.Close();
		}
	TESTCHECKL(ret,KErrNone);

	//Open LoopBack port for reading and writing
	ret=iSerialPortList[1].Open(commServ,KPortName0,ECommShared);
	if(ret != KErrNone)
		{
		commServ.Close();
		}
	TESTCHECKL(ret,KErrNone);

	ret=iSerialPortList[0].Open(commServ,KPortName1,ECommShared);
	if(ret != KErrNone)
		{
		iSerialPortList[1].Close();
		commServ.Close();
		}

	TESTCHECKL(ret,KErrNone);

	TRequestStatus stat0;
	TRequestStatus stat1;
	TBuf8<19> readBuf;
	//Writing a long data into the opened port
	for (TInt i = 0; i <= 200; i++)
		{
		TBuf8<30> writeBuf;
		writeBuf.Append(KServerText);
		writeBuf.AppendNum(i);

		if(i <= 9)
			{
			writeBuf.AppendFill(' ', 2);
			}
		else if (i <= 99)
			{
			writeBuf.AppendFill(' ', 1);
			}

		iSerialPortList[1].Write(stat0, writeBuf);
		User::WaitForRequest(stat0);
		TESTCHECKL(stat0.Int(), KErrNone);
		iSerialPortList[0].ReadOneOrMore(stat1, readBuf);
		User::WaitForRequest(stat1);
		res = stat1.Int();
		if(res != KErrNone)
			{
			INFO_PRINTF2(_L("Reading from the port fails %d "), res);

			SessionThread.Kill(KErrNone);
			User::WaitForRequest(stat);
			CLOSE_AND_WAIT(SessionThread);

			iSerialPortList[1].Close();
			iSerialPortList[0].Close();
			commServ.Close();

			SetTestStepResult(EFail);

			return TestStepResult();
			}

		}
		
	User::WaitForRequest(stat);
	TESTCHECKL(stat.Int(), KErrNone);

	CLOSE_AND_WAIT(SessionThread);
	iSerialPortList[0].Close();
	iSerialPortList[1].Close();
	commServ.Close();

	if(iState[0] != KErrNone)
		{
		INFO_PRINTF2(_L("Could not connect to C32 server in line %d, expected KErrNone."), iState[0]);
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	if(iState[1] != KErrNone)
		{
		INFO_PRINTF2(_L("DummyCsy1 Could not load at line %d, expected KErrNone."), iState[1]);
		SetTestStepResult(EFail);
		return TestStepResult();
				}
	if(iState[2] != KErrNone)
		{
		INFO_PRINTF2(_L("Port could not open for write, in line %d, expected KErrNone."), iState[2]);
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	if(iState[3] != KErrNone)
		{
		INFO_PRINTF2(_L("Port could not open for read in line %d, expected KErrNone."), iState[3]);
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	if(iState[4] != KErrNone)
		{
		INFO_PRINTF2(_L("Data could not write to the port. in line %d, expected KErrNone."), iState[4]);
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	if(iState[5] != KErrNone)
		{
		INFO_PRINTF2(_L("Data could not read from the port. in line %d, expected KErrNone."), iState[5]);
		SetTestStepResult(EFail);
		return TestStepResult();
		}

	return TestStepResult();

	}






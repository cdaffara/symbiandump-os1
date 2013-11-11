// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "TE_c32.h"
#include "TTHREAD.H"

#include <e32test.h>
#include "C32COMM.H"
#include "dummy.h"
#include <rsshared.h>

#include <e32cmn.h>

#if defined (__WINS__)
#define PDD_NAME _L("ECDRV")
#define LDD_NAME _L("ECOMM")
#define LDD_NAMETST _L("ECOMMTST")
#else
#define PDD_NAME _L("EUART1")  //< Physical Device Driver #1
#define LDD_NAME _L("ECOMM")   //< Logical Device Driver
#define LDD_NAMETST _L("ECOMMTST")
#endif

#if (!defined __WINS__ && defined EKA1)
#define PDD2_NAME _L("EUART2") //< Physical Device Driver #2
#endif

const TInt KInvalidArgument=-1;

CTestLoopback::CTestLoopback()
	{
	SetTestStepName(_L("TestLoopback"));
	}
	
TInt CTestLoopback::DataReadThread(TAny* aArg)
/**
 * Thread which attempts to read a chunk of data from loopback port
 */
	{
	CTestLoopback* stepTest = reinterpret_cast<CTestLoopback*> (aArg);
	
	_LIT(KPortName1,"LOOPBACK::1");
	_LIT(KCSYName,"LOOPBACK.CSY");
		
	RCommServ commServ;
	TInt res=commServ.Connect();
	if(res!=KErrNone)
		{
		stepTest->iState1 = __LINE__ - 2;
		return(res);
		}

	res=commServ.LoadCommModule(KCSYName);
	if(res!=KErrNone)
		{
		stepTest->iState2 = __LINE__ - 2;
		return(res);
		}
	
	res=stepTest->iSerialPortList[0].Open(commServ,KPortName1,ECommShared);
	if(res!=KErrNone)
		{
		stepTest->iState3 = __LINE__ - 2;
		return(res);
		}
		
	TRequestStatus stat0;
	TBuf8<20> readBuf;
	for (TInt i=0;i<=200;i++)
		{
		
		stepTest->iSerialPortList[0].ReadOneOrMore(stat0,readBuf);
		User::WaitForRequest(stat0);
		res = stat0.Int();
		if(res!=KErrNone)
			{
			stepTest->iState4 = __LINE__ - 2;
			return(res);
			}
		}
	
	stepTest->iSem.Signal();
	stepTest->iSerialPortList[0].Close();
	
	return KErrNone;
	
	}

TVerdict CTestLoopback::doTestStepL()
	{
	INFO_PRINTF1(_L("==================================="));
	INFO_PRINTF1(_L("Test : TestLoopback"));
	INFO_PRINTF1(_L("==================================="));
	
	_LIT(KCsyName,"LOOPBACK.CSY");
	_LIT(KPortName0,"LOOPBACK::0");
	_LIT8(KServerText,"Test Loopback : ");
	
	iSem.CreateLocal(0);

	RCommServ commServ;
	TInt ret=commServ.Connect();
	ret=commServ.LoadCommModule(KCsyName);
	TESTCHECKL(ret,KErrNone);
	
	ret=iSerialPortList[1].Open(commServ,KPortName0,ECommShared);
	TESTCHECKL(ret,KErrNone);
	
	RThread readThread;
	TInt res=readThread.Create(_L("DataReadThread"),DataReadThread,KDefaultStackSize,KDefaultHeapSizeThread,KDefaultHeapSizeThread,this);
	TESTCHECKL(res, KErrNone);
	
	TRequestStatus stat;
	readThread.Logon(stat);
	
	readThread.Resume();
	
	TRequestStatus stat0;
	
	for (TInt i=0;i<=200;i++)
		{
		TBuf8<20> writeBuf;
		writeBuf.Append(KServerText);
		writeBuf.AppendNum(i);
		
		if(i<=9)
			{
			writeBuf.AppendFill(' ',3);
			}
		
		if ((i >= 10) && (i<=99))
			{
			writeBuf.AppendFill(' ',2);
			}
			
		if((i >= 100) && (i <= 999))
			{
			writeBuf.AppendFill(' ', 1);
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
		}
	if(iState2 != KErrNone)
		{
		INFO_PRINTF2(_L("Error occured, iState2, in DataReadThread at line %d, expected KErrNone."), iState2);
		SetTestStepResult(EFail);
		}		
	if(iState3 != KErrNone)
		{
		INFO_PRINTF2(_L("Error occured, iState3, in DataReadThread at line %d, expected KErrNone."), iState3);
		SetTestStepResult(EFail);
		}				
	if(iState4 != KErrNone)
		{
		INFO_PRINTF2(_L("Error occured, iState4, in DataReadThread at line %d, expected KErrNone."), iState4);
		SetTestStepResult(EFail);
		}
						
	return TestStepResult();
	}
	
CTestAllocMoreWriteMem::CTestAllocMoreWriteMem()
	{
	SetTestStepName(_L("TestAllocMoreWriteMem"));
	}
	

TVerdict CTestAllocMoreWriteMem::doTestStepL()
	{
	INFO_PRINTF1(_L("==================================="));
	INFO_PRINTF1(_L("Test : TestAllocMoreWriteMem"));
	INFO_PRINTF1(_L("==================================="));

	_LIT(KCsyName,"LOOPBACK.CSY");
	_LIT(KPortName0,"LOOPBACK::0");
	_LIT8(KServerText,"Test Loopback : ");
	
	RCommServ commServ;
	TInt ret=commServ.Connect();
	ret=commServ.LoadCommModule(KCsyName);
	TESTCHECKL(ret,KErrNone);
	
	TRequestStatus stat0;										
	
	ret = iSerialPortList[1].Open(commServ,KPortName0,ECommShared);
	TESTCHECKL(ret,KErrNone);
	
	// This many loops ensures we need to allocate more memory for 
	// write buffer.
    const TInt KNumOfLoopIterations = 1500;
	
    for (TInt i = 0; i < KNumOfLoopIterations; i++)
		{
	    TBuf8<20> writeBuf;
		writeBuf.Append(KServerText);
		writeBuf.AppendNum(i);
		
		if(i <=9)
			{
			writeBuf.AppendFill(' ', 3);
			}
		
		if ((i >= 10) && (i <= 99))
			{
			writeBuf.AppendFill(' ', 2);
			}
			
		if((i >= 100) && (i <= 999))
			{
			writeBuf.AppendFill(' ', 1);
			}
		
		iSerialPortList[1].Write(stat0,writeBuf);
		User::WaitForRequest(stat0);
		TESTCHECKL(stat0.Int(), KErrNone);
		
		}
		
	TInt numOfBuffers = 0;
	
	// This method has been used to query the amount of data in the 
	// write buffer
	numOfBuffers = iSerialPortList[1].QueryReceiveBuffer();
	
	// The default buffer size for a write port is 4096. If we write
	// more than 4096 bytes we can confirm we have allocated more memory
	// for the write port
	TESTCHECKCONDITIONL(numOfBuffers == (20 * KNumOfLoopIterations));
	
	iSerialPortList[1].Close();

	commServ.Close();
									
	return TestStepResult();
	
	}	

CTestMemoryWriteFail::CTestMemoryWriteFail()
	{
	SetTestStepName(_L("TestMemoryWriteFail"));
	}

TVerdict CTestMemoryWriteFail::doTestStepL()
	{
	INFO_PRINTF1(_L("==================================="));
	INFO_PRINTF1(_L("Test : TestMemoryWriteFail"));
	INFO_PRINTF1(_L("==================================="));


#ifdef _DEBUG
	// The following test can only be tested under DEBUG situations as the 
	// __DbgFailNext returns KErrNone for the RELEASE situation
	_LIT(KCsyName,"LOOPBACK.CSY");
	_LIT(KPortName0,"LOOPBACK::0");
		
	RCommServ commServ;
	
	TInt ret=commServ.Connect();
	ret=commServ.LoadCommModule(KCsyName);
	TESTCHECKL(ret,KErrNone);
	
	TRequestStatus stat0;										
	
	ret = iSerialPortList[1].Open(commServ,KPortName0,ECommShared);
	TESTCHECKL(ret,KErrNone);
	
	// The following code writes in small chunks of data filling the write data
	// with X's
	TBuf8<64> writeData;
	writeData.Fill('X', 64);
	
	// 64 * 64 = 4096, currently the default buffer size
	const TInt KLoopFillBuffer = 64;
	
	for(int i = 0; i < KLoopFillBuffer; i++)
		{
		iSerialPortList[1].Write(stat0,writeData);	
		User::WaitForRequest(stat0);
		TESTCHECKL(stat0.Int(), KErrNone);
		}

    // if the size of the standard write buffer is 4096 bytes then
    // the above will fill in all the data
    // if we try and write more data a new memory allocation is required.
    commServ.__DbgFailNext(1);
	iSerialPortList[1].Write(stat0, writeData);
	User::WaitForRequest(stat0);
    TESTCHECKL(stat0.Int(), KErrNoMemory);
    commServ.__DbgFailNext(-1);
	
	iSerialPortList[1].Close();
	commServ.Close();
#else	
	INFO_PRINTF1(_L("Test : TestMemoryWriteFail not executed for non debug tests"));
#endif
	return TestStepResult();													
	}
	
	
CTestResetBuffers::CTestResetBuffers()
	{
	SetTestStepName(_L("TestResetBuffers"));
	}
	

TVerdict CTestResetBuffers::doTestStepL()
	{
	INFO_PRINTF1(_L("==================================="));
	INFO_PRINTF1(_L("Test : TestResetBuffers"));
	INFO_PRINTF1(_L("==================================="));
	
	_LIT(KCsyName,"LOOPBACK.CSY");
	_LIT(KPortName0,"LOOPBACK::0");
	_LIT8(KServerText,"Test Loopback : ");
	
	RCommServ commServ;
	
	TInt ret=commServ.Connect();
	ret=commServ.LoadCommModule(KCsyName);
	TESTCHECKL(ret,KErrNone);
	
	TRequestStatus stat0;										
	
	ret = iSerialPortList[1].Open(commServ,KPortName0,ECommShared);
	TESTCHECKL(ret,KErrNone);
	
	// This value is to ensure we exceed the standard write buffer size
	const TInt KNumOfLoopIterations = 1500;
    
    for (TInt i = 0; i < KNumOfLoopIterations; i++)
		{
	    TBuf8<20> writeBuf;
		writeBuf.Append(KServerText);
		writeBuf.AppendNum(i);
		
		if(i <=9)
			{
			writeBuf.AppendFill(' ', 3);
			}
		
		if ((i >= 10) && (i <= 99))
			{
			writeBuf.AppendFill(' ', 2);
			}
			
		if((i >= 100) && (i <= 999))
			{
			writeBuf.AppendFill(' ', 1);
			}
		
		iSerialPortList[1].Write(stat0,writeBuf);
		User::WaitForRequest(stat0);
		TESTCHECKL(stat0.Int(), KErrNone);
		
		}
	
	TInt numOfBuffers = 0;
	
	// This method has been used to query the amount of data in the 
	// write buffer
	numOfBuffers = iSerialPortList[1].QueryReceiveBuffer();
	
	// check the amount of data we have in our buffer. 
	// 20 bytes * number of times around the loop
	TESTCHECKCONDITION(numOfBuffers == (20 * KNumOfLoopIterations));
	
	// reset the amount of data in our buffer
	ret = iSerialPortList[1].ResetBuffers();
	TESTCHECKL(ret, KErrNone);
	
	numOfBuffers = iSerialPortList[1].QueryReceiveBuffer();
	
	TESTCHECKCONDITION(numOfBuffers == 0);
    
	iSerialPortList[1].Close();

	commServ.Close();
											
	return TestStepResult();	
	}

CTestCorrectDataRead::CTestCorrectDataRead()
	{
	SetTestStepName(_L("TestCorrectDataRead"));
	}	


TVerdict CTestCorrectDataRead::doTestStepL()
	{
	INFO_PRINTF1(_L("==================================="));
	INFO_PRINTF1(_L("Test : TestCorrectDataRead"));
	INFO_PRINTF1(_L("==================================="));
	
	_LIT(KCsyName,"LOOPBACK.CSY");
	_LIT(KPortName0,"LOOPBACK::0");
	_LIT(KPortName1,"LOOPBACK::1");
	_LIT8(KServerText,"Test Loopback : ");

	RCommServ commServ;

	
	TInt ret=commServ.Connect();
	ret=commServ.LoadCommModule(KCsyName);
	TESTCHECKL(ret,KErrNone);
	
	TRequestStatus stat, stat0;										
	
	ret = iSerialPortList[1].Open(commServ,KPortName0,ECommShared);
	TESTCHECKL(ret,KErrNone);
	
	ret = iSerialPortList[0].Open(commServ,KPortName1, ECommShared);
	TESTCHECKL(ret,KErrNone);
	
	// This value is to ensure we exceed the standard write buffer size
	const TInt KNumOfLoopIterations = 1500;
	    
    for (TInt i = 0; i < KNumOfLoopIterations; i++)
		{
	    TBuf8<20> writeBuf;
		writeBuf.Append(KServerText);
		writeBuf.AppendNum(i);
		
		if(i <=9)
			{
			writeBuf.AppendFill(' ', 3);
			}
		
		if ((i >= 10) && (i <= 99))
			{
			writeBuf.AppendFill(' ', 2);
			}
			
		if((i >= 100) && (i <= 999))
			{
			writeBuf.AppendFill(' ', 1);
			}
		
		iSerialPortList[1].Write(stat0,writeBuf);
		User::WaitForRequest(stat0);
		TESTCHECKL(stat0.Int(), KErrNone);
		
		}

	for (TInt i = 0; i < KNumOfLoopIterations; i++)
		{
		TBuf8<20> readBuf;
		TBuf8<20> compareBuf;
		
		compareBuf.Append(KServerText);
		compareBuf.AppendNum(i);
		
		if(i <= 9)
			{
			compareBuf.AppendFill(' ', 3);
			}
		
		if ((i >= 10) && (i <= 99))
			{
			compareBuf.AppendFill(' ', 2);
			}
			
		if((i >= 100) && (i <= 999))
			{
			compareBuf.AppendFill(' ', 1);
			}
				
		iSerialPortList[0].Read(stat0, readBuf);
		User::WaitForRequest(stat0);
		TESTCHECKL(stat0.Int(), KErrNone);
		
		// compare what we have read against what we are expecting. Remember
		// this is a first in last out buffer so the first read will read a 
		// "Loopback : 0"
		TESTCHECKCONDITION(readBuf == compareBuf);
		
		}
	
	iSerialPortList[0].Close();
	iSerialPortList[1].Close();

	
	commServ.Close();
		
	return TestStepResult();
	
	}

CTestMaxBufferOverflow::CTestMaxBufferOverflow()
	{
	SetTestStepName(_L("TestMaxBufferOverflow"));
	}	


TVerdict CTestMaxBufferOverflow::doTestStepL()
	{
	INFO_PRINTF1(_L("==================================="));
	INFO_PRINTF1(_L("Test : TestMaxBufferOverflow"));
	INFO_PRINTF1(_L("==================================="));
	
	_LIT(KCsyName,"LOOPBACK.CSY");
	_LIT(KPortName1,"LOOPBACK::0");

	RCommServ commServ;
	TRequestStatus stat0(KErrNone);
	
	TInt ret=commServ.Connect();
	ret=commServ.LoadCommModule(KCsyName);
	TESTCHECKL(ret,KErrNone);
										
	ret = iSerialPortList[1].Open(commServ,KPortName1, ECommShared);
	TESTCHECKL(ret,KErrNone);
	
	// At the time of writing the maximum size was 32768 bytes (0x8000)
	// Therefore write 32768 bytes then write once more to exceed the 
	// maximum buffer size
	const TInt KNumOfLoopIterations = 1024;
	    
   	TBuf8<32> writeBuf;
	writeBuf.Fill('X', 32);     
    
    for (TInt i = 0; i < KNumOfLoopIterations; i++)
		{		
		iSerialPortList[1].Write(stat0,writeBuf);
		User::WaitForRequest(stat0);
		TESTCHECKL(stat0.Int(), KErrNone);		
		}

	iSerialPortList[1].Write(stat0, writeBuf);
	User::WaitForRequest(stat0);
	TESTCHECKL(stat0.Int(), KErrNoMemory);
	
	iSerialPortList[1].Close();
	
	commServ.Close();
		
	return TestStepResult();
	
	}		


CTestInit::CTestInit()
	{
	SetTestStepName(_L("TestInit"));
	}

TVerdict CTestInit::doTestStepL()
/**
 * @test initializes the c32 drivers
 */
	{
	INFO_PRINTF1(_L("Loading LDD and PDD"));

	INFO_PRINTF1(_L("Load driver"));
	TInt r=User::LoadPhysicalDevice(PDD_NAME);
	TEST(r==KErrNone || r==KErrAlreadyExists);
#if defined PDD2_NAME
	INFO_PRINTF1(_L("Load COMM::2 driver"));
	r=User::LoadPhysicalDevice(PDD2_NAME);
	TEST(r==KErrNone || r==KErrAlreadyExists);
	INFO_PRINTF2(_L("Error: %d"), r);
#endif
	//7
	INFO_PRINTF1(_L("Load device"));
	r=User::LoadLogicalDevice(LDD_NAME);
	TEST(r==KErrNone|| r==KErrAlreadyExists);

	INFO_PRINTF1(_L("start comm server"));
	// PhBkSync can interfere with the OOM steps by spawning new SerComm sessions during them
	_LIT(KPhbkSyncCMI, "phbsync.cmi");
	
	r = WarmBootC32(KPhbkSyncCMI);
	
	TEST(r == KErrNone || r==KErrAlreadyExists);
	if (r!=KErrNone && r!=KErrAlreadyExists)
		INFO_PRINTF2(_L("Failed to start C32 %d"),r);

#if defined (ETNA_PDD_NAME)
	INFO_PRINTF1(_L("Loading Etna PDD "));
	r=User::LoadPhysicalDevice(ETNA_PDD_NAME);
	if (r!=KErrNone && r!=KErrAlreadyExists)
		{
		INFO_PRINTF2(_L("Failed %d!"),r);
		TEST(r==KErrNone);
		}
	else
		INFO_PRINTF1(_L("Loaded ETNA PDD"));
#endif

	return TestStepResult();
	}

CTestDoubleStart::CTestDoubleStart()
	{
	SetTestStepName(_L("TestDoubleStart"));
	}

TInt CTestDoubleStart::StartC32Thread(TAny* aArg)
/**
 * Thread which attempts to startC32 simpultaneously from this new thread
 */
	{
	CTestDoubleStart* stepTest = reinterpret_cast<CTestDoubleStart*> (aArg);
	
	TInt res = StartC32();
	if(res!=KErrNone)
		{
		stepTest->iState1 = __LINE__ - 2;
		return(res);
		}
	return KErrNone;
	}


TVerdict CTestDoubleStart::doTestStepL()
/**
 * @test Attempt to start C32 twice, using another thread to start C32 simultaneously as StartC32 is a sync call
 */
	{
	INFO_PRINTF1(_L("==================================="));
	INFO_PRINTF1(_L("Test : TestDoubleStart"));
	INFO_PRINTF1(_L("==================================="));
	INFO_PRINTF1(_L("Attempting to double start C32"));

	RThread startC32Thread;
	TInt res = startC32Thread.Create(_L("StartC32Thread"), StartC32Thread, KDefaultStackSize, KDefaultHeapSizeThread, KDefaultHeapSizeThread, this);
	TESTCHECKL(res, KErrNone);
	
	TRequestStatus stat;
	startC32Thread.Logon(stat);
	
	startC32Thread.Resume();

	res = StartC32();
	TESTCHECK(res, KErrNone);
	
	User::WaitForRequest(stat);
	CLOSE_AND_WAIT(startC32Thread);

	return TestStepResult();
	}

TVerdict CTestDoubleStart::doTestStepPreambleL( void )
	{
	return TestStepResult();
	}

TVerdict CTestDoubleStart::doTestStepPostambleL( void )
	{

	return TestStepResult();
	}

CTestOpen::CTestOpen()
	{
	SetTestStepName(_L("TestOpen"));
	}

TVerdict CTestOpen::doTestStepL()
/**
 * @test Check opening and closing 
 */
	{
	INFO_PRINTF1(_L("==================================="));
	INFO_PRINTF1(_L("Test : TestOpen"));
	INFO_PRINTF1(_L("==================================="));
	//
	INFO_PRINTF1(_L("Open/close ports"));

	TInt res = iSerialPortList[0].Open(iCommSession, KCommPort0, ECommShared);
	TESTCHECKL(res, KErrNone);

	INFO_PRINTF1(_L("Open another session"));
	RCommServ commSess2;
	res = commSess2.Connect();
	TESTCHECKL(res, KErrNone);

	res=iSerialPortList[1].Open(commSess2, KCommPort0, ECommShared);
	TESTCHECKL(res, KErrNone);

	INFO_PRINTF1(_L("Close ports"));
	iSerialPortList[0].Close();
	iSerialPortList[1].Close();

	INFO_PRINTF1(_L("Open port exclusive"));
	res = iSerialPortList[0].Open(iCommSession, KCommPort0, ECommExclusive);
	TESTCHECKL(res, KErrNone);

	INFO_PRINTF1(_L("Open port again"));

	res = iSerialPortList[1].Open(commSess2, KCommPort0, ECommShared);
	TESTCHECKL(res, KErrAccessDenied);

	iSerialPortList[0].Close();

	INFO_PRINTF1(_L("Close port without opening"));
	RComm port;
	port.Close();

	commSess2.Close();

	return TestStepResult(); 
	}

CTestInfo::CTestInfo()
	{
	SetTestStepName(_L("TestInfo"));
	}

TVerdict CTestInfo::doTestStepL()
/**
 * @test Test the information services
 */
	{
	INFO_PRINTF1(_L("==================================="));
	INFO_PRINTF1(_L("Test : TestInfo"));
	INFO_PRINTF1(_L("==================================="));

	INFO_PRINTF1(_L("Test the information services"));

	RCommServ commSess;
	INFO_PRINTF1(_L("Connect to the Comms server"));
	TInt res=commSess.Connect();
	TESTCHECKL(res, KErrNone);

	TInt num;
	res=commSess.NumPorts(num);
	INFO_PRINTF2(_L("Get number of loaded comms modules (should be 1) : %d"), num);
	TESTCHECKL(res, KErrNone);
	TESTCHECKL(num, 1);

	TPortName name;
	TSerialInfo protocolInfo;
	res=commSess.GetPortInfo(10000,name,protocolInfo);
	INFO_PRINTF1(_L("Get information on serial ports"));
	TESTCHECKL(res, KErrTooBig);

	res=commSess.GetPortInfo(0,name,protocolInfo);
	INFO_PRINTF1(_L("Get information on serial ports"));
	TESTCHECKL(res, KErrNone);
	TESTCHECKL(name.CompareF(_L("ECUART")), 0);

 	res = commSess.GetPortInfo(name,protocolInfo ); 
	INFO_PRINTF1(_L("Get information on serial ports using CSY name")); // INC052021
	TESTCHECKL(res, KErrNone);
	TESTCHECKL(protocolInfo.iName.CompareF(_L("COMM")), 0);

	commSess.Close();
	return TestStepResult();
	}


CTestDoubleRead::CTestDoubleRead()
	{
	SetTestStepName(_L("TestDoubleRead"));
	}

TInt CTestDoubleRead::DoubleReadThread(TAny* aArg)
/**
 * Thread which attempts to read twice and should panic
 */
	{	
	CTestDoubleRead* stepTest = reinterpret_cast<CTestDoubleRead*> (aArg);	
	RCommServ serv;
	TInt res=serv.Connect();
	if(res!=KErrNone)
		{
		stepTest->iState1 = __LINE__ - 2;
		return(res);
		} 

	res=stepTest->iSerialPortList[0].Open(serv, KCommPort0, ECommShared);
	if(res!=KErrNone)
		{
		stepTest->iState2 = __LINE__ - 2;
		return(res);
		} 

	
	res=stepTest->iSerialPortList[1].Open(serv, KCommPort0, ECommShared);
	if(res!=KErrNone)
		{
		stepTest->iState3 = __LINE__ - 2;
		return(res);
		} 

		
	TBuf8<100> buf;
	TRequestStatus status;
	stepTest->iSerialPortList[0].Read(status, buf);
	
	TRequestStatus status2;
	stepTest->iSerialPortList[0].Read(status2, buf);
	
	User::WaitForRequest(status2);
	stepTest->iState4 = __LINE__ - 2; //shouldn't get here.
	return KErrNone;
	}
	
TVerdict CTestDoubleRead::doTestStepL()
/**
 * @test Test that double reads panics
 */
	{
	User::SetJustInTime(EFalse);
	
	INFO_PRINTF1(_L("==================================="));
	INFO_PRINTF1(_L("Test : TestDoubleRead"));
	INFO_PRINTF1(_L("==================================="));

	INFO_PRINTF1(_L("Read twice on same port"));
	
	RThread t;
	TInt res=t.Create(_L("DoubleRead"),DoubleReadThread,KDefaultStackSize,KDefaultHeapSizeThread,KDefaultHeapSizeThread,this);
	TESTCHECKL(res, KErrNone);
	TRequestStatus stat;
	t.Logon(stat);
	t.Resume();
	User::WaitForRequest(stat);
	TESTCHECKSTRL(t.ExitCategory(), KCommServerPanic);	//"Comm server panic" from cc_utl.cpp
	TESTCHECKL(t.ExitReason(), 1);	// 1==EReadTwice from cs_std.h
	TESTCHECKL(t.ExitType(), EExitPanic);
	CLOSE_AND_WAIT(t);
			
	User::SetJustInTime(ETrue);
	
	// Check return values of tests within the thread
	if(iState1 != 0)
		{
		INFO_PRINTF2(_L("Error occured in DoubleReadThread at line %d, expected KErrNone."), iState1);
		SetTestStepResult(EFail);
		}
	if(iState2 != 0)
		{
		INFO_PRINTF2(_L("Error occured in DoubleReadThread at line %d, expected KErrNone."), iState2);
		SetTestStepResult(EFail);
		}		
	if(iState3 != 0)
		{
		INFO_PRINTF2(_L("Error occured in DoubleReadThread at line %d, expected KErrNone."), iState3);
		SetTestStepResult(EFail);
		}				

	if(iState4 != 0)
		{
		INFO_PRINTF2(_L("Error occured in doubleReadThread at line %d, didn't expected to reach code."), iState4);
		SetTestStepResult(EFail);
		}						
				
	return TestStepResult();
	}

CTestConfigPanic::CTestConfigPanic()
	{
	SetTestStepName(_L("TestConfigPanic"));
	}

TInt CTestConfigPanic::ConfigPanic(TAny* aArg)
/**
 * Thread which attempts to read twice and should panic 
 */
	{
	CTestConfigPanic* stepTest = reinterpret_cast<CTestConfigPanic*>(aArg);
	RCommServ serv;
	TUint res=serv.Connect();
	if(res!=(TUint)KErrNone)
		{
		stepTest->iState1 = __LINE__ - 2;
		return(res);
		}

	res=stepTest->iSerialPortList[0].Open(serv,KCommPort0,ECommShared);
	if(res!=(TUint)KErrNone)
		{
		stepTest->iState2 = __LINE__ - 2;
		return(res);
		}
		
	TRequestStatus status;
	TBuf8<100> buf;
	stepTest->iSerialPortList[0].Read(status,buf);

	TCommConfig cfg;
	TInt ret=stepTest->iSerialPortList[0].Config(cfg);
	if(ret!=KErrNone)
		{
		stepTest->iState3 = __LINE__ - 2;
		}
	TInt val=stepTest->iSerialPortList[0].SetConfig(cfg);
 	return val;
	}

TVerdict CTestConfigPanic::doTestStepL()
/**
 * @test Test thread is panicked if it tries to set configuration 
 */
	{
	User::SetJustInTime(EFalse);

	INFO_PRINTF1(_L("==================================="));
	INFO_PRINTF1(_L("Test : TestConfigPanic"));
	INFO_PRINTF1(_L("==================================="));

	INFO_PRINTF1(_L("Configuration panic"));

	RThread t;
	TInt res=t.Create(_L("ConfigPanic"),ConfigPanic,KDefaultStackSize,KDefaultHeapSizeThread,KDefaultHeapSizeThread,this);
	TESTCHECKL(res, KErrNone);
	TRequestStatus stat;
	t.Logon(stat);
	t.Resume();
	User::WaitForRequest(stat);
 	TESTCHECKL(stat.Int(), KErrInUse);
	CLOSE_AND_WAIT(t);

	User::SetJustInTime(ETrue);
	
	if(iState1 != 0)
		{
		INFO_PRINTF2(_L("Error occured in ConfigPanic at line %d, expected KErrNone."), iState1);
		SetTestStepResult(EFail);
		}
	if(iState2 != 0)
		{
		INFO_PRINTF2(_L("Error occured in ConfigPanic at line %d, expected KErrNone."), iState2);
		SetTestStepResult(EFail);
		}	
	if(iState3 != 0)
		{
		INFO_PRINTF2(_L("Error occured in ConfigPanic at line %d, expected KErrNone."), iState3);
		SetTestStepResult(EFail);
		}
	
  	return TestStepResult();
	}

CTestBadDescriptorPanic::CTestBadDescriptorPanic()
	{
	SetTestStepName(_L("TestBadDescriptorPanic"));
	}

TInt CTestBadDescriptorPanic::BadDescriptor(TAny* aArg)
/**
 * Thread which attempts to write with a bad descriptor should panic
 */
	{
	CTestBadDescriptorPanic* testStep = reinterpret_cast<CTestBadDescriptorPanic*>(aArg);
	RCommServ serv;
	TUint res=serv.Connect();
	if(res!=(TUint)KErrNone)
		{
		testStep->iState1 = __LINE__ - 2;
		return(res);
		}

	res=testStep->iSerialPortList[0].Open(serv,KCommPort0,ECommShared);
	if(res!=(TUint)KErrNone)
		{
		testStep->iState2 = __LINE__ - 2;
		return(res);
		}

		
	TRequestStatus status;
	TBufC8<100> buf = _L8("Hello");

	// scribble on type part of descriptor
	*(TInt*)&buf |= 0xf0000000;
	testStep->iSerialPortList[0].Write(status,buf);
	
	User::WaitForRequest(status);
	testStep->iState3 = __LINE__ - 2; // Shouldn't reach here
	return KErrNone;
	}
	
TVerdict CTestBadDescriptorPanic::doTestStepL()
/**
 * @test Test that double reads panics
 */
	{
	User::SetJustInTime(EFalse);
	
	INFO_PRINTF1(_L("==================================="));
	INFO_PRINTF1(_L("Test : TestBadDescriptorPanic"));
	INFO_PRINTF1(_L("==================================="));


	INFO_PRINTF1(_L("Test that double reads panics"));
	RThread t;
	TInt res=t.Create(_L("DescriptorPanic"),BadDescriptor,KDefaultStackSize,KDefaultHeapSizeThread,KDefaultHeapSizeThread,this);
	TESTCHECKL(res, KErrNone);
	TRequestStatus stat;
	t.Logon(stat);
	t.Resume();
	User::WaitForRequest(stat);
	TESTCHECKSTRL(t.ExitCategory(), KCommServerPanic);	//"Comm server panic" from cc_utl.cpp
	TESTCHECKL(t.ExitReason(), 4); // EBadDescriptor == 4
	TESTCHECKL(t.ExitType(), EExitPanic);
	CLOSE_AND_WAIT(t);
	
	// Check return values of tests within the thread
	if(iState1 != 0)
		{
		INFO_PRINTF2(_L("Error occured in BadDescriptor at line %d, expected KErrNone."), iState1);
		SetTestStepResult(EFail);
		}
	if(iState2 != 0)
		{
		INFO_PRINTF2(_L("Error occured in BadDescriptor at line %d, expected KErrNone."), iState2);
		SetTestStepResult(EFail);
		}		
	if(iState3 != 0)
		{
		INFO_PRINTF2(_L("Error occured in BadDescriptor at line %d, didn't expected to reach code."), iState3);
		SetTestStepResult(EFail);
		}						
	
	
	User::SetJustInTime(ETrue);
	return TestStepResult();
	}

CTestBusyReturn::CTestBusyReturn()
	{
	SetTestStepName(_L("TestBusyReturn"));
	}

TInt CTestBusyReturn::CreateThread(TAny* aArg)
/**
 * Thread which opens a port and then gets panicked
 */
	{
	__UHEAP_MARK;	
	CTrapCleanup *trap = CTrapCleanup::New();
	if (!trap)
		{
		return KErrNoMemory;
		}
	
	TInt ret(KErrUnknown);
	TRAPD(err, ret = OpenAndReadThreadL(aArg));
	__ASSERT_ALWAYS(!err, User::Panic(_L("OpenAndReadThreadL"), err));
	delete trap;
	__UHEAP_MARKEND;
	return ret;
	}
	
TInt CTestBusyReturn::OpenAndReadThreadL(TAny* aArg)
	{
	CTestBusyReturn* testStep = reinterpret_cast<CTestBusyReturn*>(aArg);
	testStep->iBuffer.Append(_L("Entering Second thread\n"));

	RCommServ serv;
	TInt res = serv.Connect();

	if(res != KErrNone) 
		{
		testStep->iState1 = __LINE__ - 2;
		return(res);
		}

	res = testStep->iSerialPortList[0].Open(serv, KCommPort1, ECommShared);

	if(res != KErrNone) 
		{
		testStep->iState2 = __LINE__ - 2;
		return(res);
		}
	
	TRequestStatus status;
	TUint8* b = new (ELeave) TUint8[100];
	TPtr8 buf(b, 100, 100);
	
	testStep->iBuffer.Append(_L("\t\t\t\t\tSecond thread : trying to write from second thread\n"));
	testStep->iBuffer.Append(_L("\t\t\t\t\tSecond thread : It should return KErrInUse\n"));
	testStep->iSerialPortList[0].Write(status, buf);
	User::WaitForRequest(status);
	
	if(status.Int() != KErrInUse) 
		{
		testStep->iState3 = __LINE__ - 2;
		}
	 
	
	testStep->iSerialPortList[0].Read(status,buf);
	testStep->iSem.Signal();

	User::WaitForRequest(status);

	delete b;
	return KErrNone;
	}

TVerdict CTestBusyReturn::doTestStepL()
/**
 * @test Test reading on a port from another thread doesn't panic us..
 */
	{
//	User::SetJustInTime(EFalse);

	INFO_PRINTF1(_L("=========================================="));
	INFO_PRINTF1(_L("Test : TestBusyReturn (multithreaded test)"));
	INFO_PRINTF1(_L("=========================================="));

	INFO_PRINTF1(_L("Main thread : Testing busy return when one thread already owns a port."));

	TInt res;
	RCommServ serv;
	res = serv.Connect();
	TESTCHECKL(res, KErrNone);

	INFO_PRINTF1(_L("Main thread : open in this thread"));

	res = iSerialPortList[1].Open(serv, KCommPort1, ECommShared);
	TESTCHECKL(res, KErrNone);

	TRequestStatus rstatus, wstatus;
	const TInt KBufferSize=150;
	TBuf8<KBufferSize> rbuf;
	TBufC8<KBufferSize> wbuf = _L8("This thing here is a 150 character buffer, designed to clog up COM1. With a bit of luck it should be long enough to turn around and bite its own tail!");
	rbuf.SetMax();

	iSem.CreateLocal(0);

	INFO_PRINTF1(_L("Main thread : write/read"));
	iSerialPortList[1].Write(wstatus, wbuf);

	RThread t;
	res = t.Create(_L("Open1"), CreateThread, KDefaultStackSize, KDefaultHeapSizeThread, KDefaultHeapSizeThread, this);
	TESTCHECKL(res, KErrNone);
	t.Resume();
	iSem.Wait();
	iSem.Close();

//	Log.SeekEnd();		// To compensate the output of the other thread

	INFO_PRINTF1(_L("Main thread : read when busy"));
	iSerialPortList[1].Read(rstatus, rbuf);
	User::WaitForRequest(rstatus);
	TESTCHECKL(rstatus.Int(), KErrInUse);


//	iSerialPortList[1].Break(rstatus,10000);		// temp removed cos DD has been broken
//	User::WaitForRequest(rstatus);
//	TESTCHECKL(rstatus, KErrNotSupported);
	
	INFO_PRINTF1(_L("Main thread : Kill second thread"));
	t.Kill(KErrNone);
	
	INFO_PRINTF1(iBuffer);

	// Check return values of tests within the thread
	if(iState1 != 0)
		{
		INFO_PRINTF2(_L("Error occured in CreateThread at line %d, expected KErrNone."), iState1);
		SetTestStepResult(EFail);
		}
	if(iState2 != 0)
		{
		INFO_PRINTF2(_L("Error occured in CreateThread at line %d, expected KErrNone."), iState2);
		SetTestStepResult(EFail);
		}		
	if(iState3 != 0)
		{
		INFO_PRINTF2(_L("Error occured in CreateThread at line %d, expected KErrInUse."), iState3);
		SetTestStepResult(EFail);
		}				
	else
		{
		INFO_PRINTF1(_L("Main thread : KErrInUse returned as expected."));
		}
				
	TInt ret = iSerialPortList[1].WriteCancel();
	TESTCHECKL(ret, KErrNone);
	User::WaitForRequest(wstatus);
	TESTCHECKL(wstatus.Int(), KErrCancel);

	rbuf.SetLength(KBufferSize);
	INFO_PRINTF1(_L("Main thread : And read OK"));
	iSerialPortList[1].Read(rstatus,rbuf);

	User::After(100000);

	TESTCHECKL(rstatus.Int(), KRequestPending);

	ret = iSerialPortList[1].ReadCancel();
	TESTCHECKL(ret, KErrNone);
	User::WaitForRequest(rstatus);

	//
	// close down
	//
	CLOSE_AND_WAIT(t);
	iSerialPortList[1].Close();
	serv.Close();
	
//	User::SetJustInTime(ETrue);
	return TestStepResult();
	}

CTestCancel::CTestCancel()
	{
	SetTestStepName(_L("TestCancel"));
	}

TInt CTestCancel::CancelThread(TAny* aArg)
/**
 * Thread which attempts to read twice and should panic
 */
	{
	CTestCancel* testStep = reinterpret_cast<CTestCancel*>(aArg);
	testStep->iBuffer.Append(_L("Second thread : CancelThread"));
	
	RCommServ serv;
	TUint res=serv.Connect();
	if(res!=(TUint)KErrNone)
		{
		testStep->iState1 = __LINE__ - 2;
		return(res);
		}

	res=testStep->iSerialPortList[0].Open(serv,KCommPort0,ECommShared);
	if(res!=(TUint)KErrNone)
		{
		testStep->iState2 = __LINE__ - 2;
		return(res);
		}

	res=testStep->iSerialPortList[1].Open(serv,KCommPort0,ECommShared);
	if(res!=(TUint)KErrNone)
		{
		testStep->iState3 = __LINE__ - 2;
		return(res);
		}
		
	TBuf8<100> buf;
	TRequestStatus status;
	testStep->iSerialPortList[0].Read(status,buf);
	TInt ret = testStep->iSerialPortList[0].ReadCancel();
	if(ret!=KErrNone)
		{
		testStep->iState5 = __LINE__ - 2;
		return(ret);
		}

	TRequestStatus status2;
	testStep->iSerialPortList[0].Read(status2,buf);	// Should not panic....
	
	User::WaitForRequest(status);
	if(status.Int() != KErrCancel)
		{
		testStep->iState4 = __LINE__ - 2;
		return(status.Int());
		}

	ret = testStep->iSerialPortList[0].ReadCancel();		
	if(ret!=KErrNone)
		{
		testStep->iState6 = __LINE__ - 2;
		return(ret);
		}
	User::WaitForRequest(status2);

	ret = testStep->iSerialPortList[0].ReadCancel(); // Test we can cancel a non existent request.
	if(ret!=KErrNone)
		{
		testStep->iState7 = __LINE__ - 2;
		return(ret);
		}

	return KErrNone;
	}


TVerdict CTestCancel::doTestStepL()
/**
 * @test Test cancel works...and doesn't panic
 */
	{
	INFO_PRINTF1(_L("==================================="));
	INFO_PRINTF1(_L("Test :TestCancel"));
	INFO_PRINTF1(_L("==================================="));

	INFO_PRINTF1(_L("Test cancel works...and doesn't panic"));

	INFO_PRINTF1(_L("Cancel"));
	RThread t;
	TInt res=t.Create(_L("Cancel"),CancelThread,KDefaultStackSize,KDefaultHeapSizeThread,KDefaultHeapSizeThread,this);
	TEST(res==KErrNone);

	TRequestStatus stat;
	t.Logon(stat);
	t.Resume();
	User::WaitForRequest(stat);
	TESTCHECKSTR(t.ExitCategory(), KCommServerSuccess);
	TESTCHECK(t.ExitReason(), KErrNone);	
	TESTCHECK(t.ExitType(), EExitKill);
	CLOSE_AND_WAIT(t);
	
	INFO_PRINTF1(iBuffer);
	
	// Check return values of tests within the thread
	if(iState1 != 0)
		{
		INFO_PRINTF2(_L("Error occured in CancelThread at line %d, expected KErrNone."), iState1);
		SetTestStepResult(EFail);
		}
	if(iState2 != 0)
		{
		INFO_PRINTF2(_L("Error occured in CancelThread at line %d, expected KErrNone."), iState2);
		SetTestStepResult(EFail);
		}		
	if(iState3 != 0)
		{
		INFO_PRINTF2(_L("Error occured in CancelThread at line %d, expected KErrNone."), iState3);
		SetTestStepResult(EFail);
		}	
	if(iState4 != 0)
		{
		INFO_PRINTF2(_L("Error occured in CancelThread at line %d, expected KErrCancel."), iState3);
		SetTestStepResult(EFail);
		}			
	if(iState5 != 0)
		{
		INFO_PRINTF2(_L("Error occured in CancelThread at line %d, expected KErrCancel."), iState5);
		SetTestStepResult(EFail);
		}			
	if(iState6 != 0)
		{
		INFO_PRINTF2(_L("Error occured in CancelThread at line %d, expected KErrCancel."), iState6);
		SetTestStepResult(EFail);
		}
	if(iState7 != 0)
		{
		INFO_PRINTF2(_L("Error occured in CancelThread at line %d, expected KErrCancel."), iState7);
		SetTestStepResult(EFail);
		}			
				
	return TestStepResult();
	}

CTestGetSetConfig::CTestGetSetConfig()
	{
	SetTestStepName(_L("TestGetSetConfig"));
	}

TVerdict CTestGetSetConfig::doTestStepL()
/**
 * @test Test setting and getting config
 */
	{
	INFO_PRINTF1(_L("==================================="));
	INFO_PRINTF1(_L("Test : TestGetSetConfig"));
	INFO_PRINTF1(_L("==================================="));

	INFO_PRINTF1(_L("test setting and getting config"));

	//
	// use COMM::1
	//
	INFO_PRINTF1(_L("Open comm session"));
	TInt res=iSerialPortList[0].Open(iCommSession, KCommPort1, ECommShared);
	TESTCHECKL(res, KErrNone);

	INFO_PRINTF1(_L("Open 2nd session"));
	RCommServ commSess2;
	res=commSess2.Connect();
	TESTCHECKL(res, KErrNone);

	res=iSerialPortList[1].Open(commSess2, KCommPort1, ECommShared);
	TESTCHECKL(res, KErrNone);

	TCommConfig cBuf0;
	TCommConfigV01 &c0=cBuf0();
	TCommConfig cBuf1;
	TCommConfigV01 &c1=cBuf1();

	INFO_PRINTF1(_L("Check that config for both ports is the same"));
	TInt ret=iSerialPortList[0].Config(cBuf0);
	TESTCHECKL(ret, KErrNone);
	ret=iSerialPortList[1].Config(cBuf1);
	TESTCHECKL(ret, KErrNone);

	TESTCHECK(c0.iRate, c1.iRate);
	TESTCHECK(c0.iDataBits, c1.iDataBits);
	TESTCHECK(c0.iStopBits, c1.iStopBits);
	TESTCHECK(c0.iParity, c1.iParity);
	TESTCHECK(c0.iHandshake, c1.iHandshake);
	TESTCHECK(c0.iParityError, c1.iParityError);
	TESTCHECK(c0.iSpecialRate, c1.iSpecialRate);
	TESTCHECK(c0.iTerminatorCount, c1.iTerminatorCount);
	TESTCHECK(c0.iTerminator[0], c1.iTerminator[0]);
	TESTCHECK(c0.iTerminator[1], c1.iTerminator[1]);
	TESTCHECK(c0.iTerminator[2], c1.iTerminator[2]);
	TESTCHECK(c0.iTerminator[3], c1.iTerminator[3]);
	TESTCHECK(c0.iXonChar, c1.iXonChar);
	TESTCHECK(c0.iXoffChar, c1.iXoffChar);
	TESTCHECK(c0.iParityErrorChar, c1.iParityErrorChar);
//	TESTCHECK(c0.iSpareChar, c1.iSpareChar);

	// Make a change...
//	c0.iXonChar++;
//	c0.iXoffChar++;
	c0.iRate = EBps19200;

	INFO_PRINTF1(_L("Check that changing one changes the other"));
	res = iSerialPortList[0].SetConfig(cBuf0);
	TESTCHECKL(res, KErrNone);
	ret=iSerialPortList[1].Config(cBuf1);
	TESTCHECKL(ret, KErrNone);

	TESTCHECK(c0.iRate, c1.iRate);
	TESTCHECK(c0.iDataBits, c1.iDataBits);
	TESTCHECK(c0.iStopBits, c1.iStopBits);
	TESTCHECK(c0.iParity, c1.iParity);
	TESTCHECK(c0.iHandshake, c1.iHandshake);
	TESTCHECK(c0.iParityError, c1.iParityError);
	TESTCHECK(c0.iSpecialRate, c1.iSpecialRate);
	TESTCHECK(c0.iTerminatorCount, c1.iTerminatorCount);
	TESTCHECK(c0.iTerminator[0], c1.iTerminator[0]);
	TESTCHECK(c0.iTerminator[1], c1.iTerminator[1]);
	TESTCHECK(c0.iTerminator[2], c1.iTerminator[2]);
	TESTCHECK(c0.iTerminator[3], c1.iTerminator[3]);
	TESTCHECK(c0.iXonChar, c1.iXonChar);
	TESTCHECK(c0.iXoffChar, c1.iXoffChar);
	TESTCHECK(c0.iParityErrorChar, c1.iParityErrorChar);
//	TESTCHECK(c0.iSpareChar, c1.iSpareChar);

	iSerialPortList[0].Close();
	iSerialPortList[1].Close();

	commSess2.Close();

	return TestStepResult();
	}

CTestTimeOut::CTestTimeOut()
	{
	SetTestStepName(_L("TestTimeOut"));
	}

TVerdict CTestTimeOut::doTestStepL()
/**
 * @test Test time outs...
 */
	{
	INFO_PRINTF1(_L("==================================="));
	INFO_PRINTF1(_L("Test : TestTimeOut"));
	INFO_PRINTF1(_L("==================================="));

	INFO_PRINTF1(_L("Test time outs...."));
	INFO_PRINTF1(_L("Open port"));

	TInt res=iSerialPortList[0].Open(iCommSession,KCommPort0,ECommShared);
	TESTCHECKL(res, KErrNone);

	// Turn handshaking on.

	TCommConfig cBuf;
	TCommConfigV01 &c=cBuf();

	c.iRate=EBps9600;
	c.iHandshake=KConfigObeyXoff|KConfigSendXoff|KConfigObeyCTS|KConfigObeyDSR|KConfigObeyDCD;
	c.iTerminatorCount=0;

	iSerialPortList[0].SetConfig(cBuf);
	const TUint KBufSize=0x5000;
	TUint8* buf = new (ELeave) TUint8[KBufSize];
	CleanupStack::PushL(buf);
	TPtr8 des(buf,KBufSize,KBufSize);

	TRequestStatus status;

	const TInt KTimeOutValue=1000000;
	TTime starttime;
	starttime.UniversalTime();
	INFO_PRINTF1(_L("Read with time out"));
	iSerialPortList[0].Read(status,KTimeOutValue,des,KBufSize);
	User::WaitForRequest(status);
	TESTCHECKL(status.Int(), KErrTimedOut);

	TTime endtime;
	endtime.UniversalTime();
    TTimeIntervalMicroSeconds interval = endtime.MicroSecondsFrom(starttime);
    TInt64 milliseconds = interval.Int64() / TInt64(1000);
    INFO_PRINTF2(_L("MilliSeconds taken %d"), I64LOW(milliseconds)); 

	des.SetMax();
	INFO_PRINTF1(_L("Write with time out"));
	starttime.UniversalTime();
	iSerialPortList[0].Write(status,KTimeOutValue,des,KBufSize);
	User::WaitForRequest(status);
	TESTCHECKL(status.Int(), KErrTimedOut);

	endtime.UniversalTime();
    interval = endtime.MicroSecondsFrom(starttime);
    milliseconds = interval.Int64() / TInt64(1000);
    INFO_PRINTF2(_L("MilliSeconds taken %d"), I64LOW(milliseconds)); 

	iSerialPortList[0].Close();

	res=iSerialPortList[0].Open(iCommSession,KCommPort0,ECommShared);
	TESTCHECKL(res, KErrNone);

	iSerialPortList[0].Read(status,KTimeOutValue,des);
	TInt ret = iSerialPortList[0].ReadCancel();
	TESTCHECKL(ret, KErrNone);
	User::WaitForRequest(status);
	iSerialPortList[0].Close();
	TESTCHECKL(status.Int(), KErrCancel);

	CleanupStack::PopAndDestroy(1); // buf
	return TestStepResult();
	
	}

CTestThreadPanic::CTestThreadPanic()
	{
	SetTestStepName(_L("TestThreadPanic"));
	}

TInt CTestThreadPanic::OpenExclusiveThread(TAny* aArg)
/**
 * Thread which opens a port and then get's panicked
 */
	{
	CTestThreadPanic* testStep = reinterpret_cast<CTestThreadPanic*>(aArg);
	testStep->iBuffer = _L("OpenExclusiveThread");
	RCommServ serv;
	TUint res=serv.Connect();
	if(res!=(TUint)KErrNone)
		{
		testStep->iState1 = __LINE__ - 2;
		return(res);
		}

	res=testStep->iSerialPortList[0].Open(serv,KCommPort0,ECommExclusive);
	if(res!=(TUint)KErrNone)
		{
		testStep->iState2 = __LINE__ - 2;
		return(res);
		}

	testStep->iSem1.Signal();
	User::WaitForAnyRequest();

	return 0;
	}

TInt CTestThreadPanic::OpenSharedThread(TAny* aArg)
/**
 * Thread which opens a port and then get's panicked
 */
	{
	CTestThreadPanic* testStep = reinterpret_cast<CTestThreadPanic*>(aArg);
    testStep->iBuffer = _L("OpenSharedThread");
	RCommServ serv;
	TUint res=serv.Connect();
	if(res!=(TUint)KErrNone)
		{
		testStep->iState3 = __LINE__ - 2;
		return(res);
		}

	res=testStep->iSerialPortList[0].Open(serv,KCommPort0,ECommShared);
	if(res!=(TUint)KErrNone)
		{
		testStep->iState4 = __LINE__ - 2;
		return(res);
		}


	testStep->iSem2.Signal();

	return 0;
	}

TVerdict CTestThreadPanic::doTestStepL()
/**
 * @test Test opening a port from a thread panicking the thread.
 */
	{	
	INFO_PRINTF1(_L("==================================="));
	INFO_PRINTF1(_L("Test : TestThreadPanic"));
	INFO_PRINTF1(_L("==================================="));

	INFO_PRINTF1(_L("Testing opening and closing."));

	iSem1.CreateLocal(0);
	
	INFO_PRINTF1(_L("Close due to panic"));
	RThread t;
	TInt res=t.Create(_L("Open2"),OpenExclusiveThread,KDefaultStackSize,KDefaultHeapSizeThread,KDefaultHeapSizeThread,reinterpret_cast<TAny*>(this));
	TESTCHECKL(res, KErrNone);
	t.Resume();

	RCommServ serv;
	res=serv.Connect();
	TESTCHECKL(res, KErrNone);

	User::SetJustInTime(EFalse);
	t.Panic(_L("Test Panic"),666);
	User::SetJustInTime(ETrue);

	res=iSerialPortList[1].Open(serv,KCommPort0,ECommExclusive);
	TESTCHECKL(res, KErrNone);

	iSerialPortList[1].Close();

	iSem1.Close();
	CLOSE_AND_WAIT(t);
	
	INFO_PRINTF1(iBuffer);
	
	// Check return values of tests within the thread
	if(iState1 != 0)
		{
		INFO_PRINTF2(_L("Error occured in OpenExclusiveThread at line %d, expected KErrNone."), iState1);
		SetTestStepResult(EFail);
		}
	if(iState2 != 0)
		{
		INFO_PRINTF2(_L("Error occured in OpenExclusiveThread at line %d, expected KErrNone."), iState2);
		SetTestStepResult(EFail);
		}		
	
	INFO_PRINTF1(_L("Open shared from two threads."));	

	res = iSerialPortList[1].Open(serv,KCommPort0,ECommShared);
	TESTCHECKL(res, KErrNone);

	iSem2.CreateLocal(0);
	RThread t2;
	res=t2.Create(_L("Open3"),OpenSharedThread,KDefaultStackSize,KDefaultHeapSizeThread,KDefaultHeapSizeThread,this);
	TESTCHECKL(res, KErrNone);
	t2.Resume();
	iSem2.Wait();

	serv.Close();
	CLOSE_AND_WAIT(t2);
	
	INFO_PRINTF1(iBuffer);
	
	// Check return values of tests within the thread
	if(iState3 != 0)
		{
		INFO_PRINTF2(_L("Error occured in OpenSharedThread at line %d, expected KErrNone."), iState3);
		SetTestStepResult(EFail);
		}
	if(iState4 != 0)
		{
		INFO_PRINTF2(_L("Error occured in OpenSharedThread at line %d, expected KErrNone."), iState4);
		SetTestStepResult(EFail);
		}		
		
	return TestStepResult();
	}

CTestBufferOptions::CTestBufferOptions()
	{
	SetTestStepName(_L("TestBufferOptions"));
	}

TVerdict CTestBufferOptions::doTestStepL()
/**
 * @test Test Buffered comm server options
 */
	{
	INFO_PRINTF1(_L("=========================================="));
	INFO_PRINTF1(_L("Test : TestBufferOptions"));
	INFO_PRINTF1(_L("=========================================="));

	INFO_PRINTF1(_L("Test Buffered comm server options"));

	INFO_PRINTF1(_L("Open port"));
	TInt res=iSerialPortList[0].Open(iCommSession,KCommPort0,ECommShared);
	TESTCHECKL(res, KErrNone);
	// enable buffering

	TCommServerConfig bmcPckg;
	TCommServerConfigV01 &bmc=bmcPckg();

	TUint len=iSerialPortList[0].ReceiveBufferLength();
	TInt ret = iSerialPortList[0].SetReceiveBufferLength(len*2);
	TESTCHECKL(ret, KErrNone);
	ret = iSerialPortList[0].SetReceiveBufferLength(len/2);
	TESTCHECKL(ret, KErrNone);

	bmc.iBufFlags=KCommBufferPartial;
	bmc.iBufSize=0x1000;

	iSerialPortList[0].SetMode(bmcPckg);

	bmc.iBufFlags=KCommBufferPartial;
	bmc.iBufSize=0x5;

	iSerialPortList[0].SetMode(bmcPckg);

	bmc.iBufFlags=KCommBufferPartial;
	bmc.iBufSize=0x2000;

	iSerialPortList[0].SetMode(bmcPckg);

	iSerialPortList[0].Close();

	return TestStepResult();
	}

CTestOOMConnection::CTestOOMConnection()
	{
	SetTestStepName(_L("TestOOMConnection"));
	}

TVerdict CTestOOMConnection::doTestStepL()
/**
 * @test Test connecting under OOM
 */
	{
	INFO_PRINTF1(_L("=========================================="));
	INFO_PRINTF1(_L("Test : TestOOMConnection"));
	INFO_PRINTF1(_L("=========================================="));

#ifdef _DEBUG
	INFO_PRINTF1(_L("Test connecting under OOM"));
	RCommServ cs;
	TInt ret=cs.Connect();
    TESTCHECKL(ret, KErrNone);

	INFO_PRINTF1(_L("Low memory connection"));
	TInt failure=0;
	RCommServ cs2;
	ret=-1;
	cs.__DbgMarkHeap();
	while (ret!=KErrNone)
		{
		INFO_PRINTF2(_L("Failing after %d Allocs"),failure);
		cs.__DbgFailNext(failure);
		ret=cs2.Connect();
		if (ret!=KErrNone)
			cs.__DbgCheckHeap(0);
		failure++;
		}
	INFO_PRINTF1(_L("Connected OK"));

	cs2.Close();

	// Flush any FailNext there might be hanging around.
	if (cs2.Connect()==KErrNone)
		cs2.Close();

	cs.__DbgMarkEnd(0);
	cs.__DbgFailNext(-1);
	cs.Close();
#else
	INFO_PRINTF1(_L("TestDisabled on release build."));
#endif
	return TestStepResult();
	}

CTestOOMPortCreation::CTestOOMPortCreation()
	{
	SetTestStepName(_L("TestOOMPortCreation"));
	}

TVerdict CTestOOMPortCreation::doTestStepL()
/**
 * @test Test creating ports under OOM
 */
	{
	INFO_PRINTF1(_L("=========================================="));
	INFO_PRINTF1(_L("Test : TestOOMPortCreation"));
	INFO_PRINTF1(_L("=========================================="));

#ifdef _DEBUG
	INFO_PRINTF1(_L("Test creating ports under OOM"));
	RCommServ cs;
	TInt ret=cs.Connect();
    TESTCHECKL(ret, KErrNone);

	INFO_PRINTF1(_L("Low memory port open no heap check"));
	TInt failure=0;
	RComm c;
	ret=-1;
	while (ret!=KErrNone)
		{
		INFO_PRINTF2(_L("Failing after %d Allocs"),failure);
		cs.__DbgFailNext(failure);
		ret=c.Open(cs,KCommPort0,ECommShared);
		failure++;
		}
	INFO_PRINTF1(_L("Created port OK"));

	c.Close();
	// Flush any FailNext there might be hanging around.
	if (c.Open(cs,KCommPort0,ECommShared)==KErrNone)
		c.Close();

	INFO_PRINTF1(_L("Low memory port open with heap check"));
	RComm ports[16];
	TInt i;
	for (i=0;i<16;i++)
		{

		TInt res=ports[i].Open(cs,KCommPort1,ECommShared);
		if(res==KErrNotFound)
			{
			INFO_PRINTF1(_L("Cannot do this test - no second serial port"));
			iSerialPortList[0].Close();
			SetTestStepResult(EInconclusive);			
			return TestStepResult();
			}
		}

	for (i=0;i<15;i++)
		ports[i].Close();

	failure=0;
	ret=-1;
	cs.__DbgMarkHeap();
	while (ret!=KErrNone)
		{
		INFO_PRINTF2(_L("Failing after %d Allocs"),failure);
		cs.__DbgFailNext(failure);
		ret=c.Open(cs,KCommPort0,ECommShared);
		if (ret!=KErrNone)
			cs.__DbgCheckHeap(0);
		failure++;
		}
	INFO_PRINTF1(_L("Created port OK"));

	c.Close();
	ports[15].Close();

	// Flush any FailNext there might be hanging around.
	if (c.Open(cs,KCommPort0,ECommShared)==KErrNone)
		c.Close();

	cs.__DbgMarkEnd(0);
	cs.__DbgFailNext(-1);
	cs.Close();
#else
	INFO_PRINTF1(_L("TestDisabled on release build."));
#endif
	return TestStepResult();
	}
	

CTestSetSignalsPanicBug::CTestSetSignalsPanicBug()
	{
	SetTestStepName(_L("TestSetSignalsPanicBug"));
	}

TVerdict CTestSetSignalsPanicBug::doTestStepL()
/**
 * @test Test SetSignals panic bug
 *
 * @note An ER5 defect report against C32 speaks of wrong behaviour when a client calls
 * SetSignals while another request is pending. Currently C32 panics the client
 * with ESetSignalsWhilePendingRequests in the mistaken belief that the device
 * driver would panic C32 otherwise. However it should be a valid action. This
 * tests that neither the bug-fixed C32  server is not panicked, and neither is
 * the client.
 */
	{

	INFO_PRINTF1(_L("=========================================="));
	INFO_PRINTF1(_L("Test : TestSetSignalsPanicBug"));
	INFO_PRINTF1(_L("=========================================="));

	INFO_PRINTF1(_L("Test SetSignals panic bug"));

	TInt res=iSerialPortList[0].Open(iCommSession,KCommPort0,ECommShared);
	TESTCHECKL(res, KErrNone);

	TRequestStatus status;
	TBuf8<40> buf;
	iSerialPortList[0].Read(status,buf);

	iSerialPortList[0].SetSignals(0,KSignalRTS|KSignalDTR);

	TInt ret = iSerialPortList[0].ReadCancel();
	TESTCHECKL(ret, KErrNone);
	User::WaitForRequest(status);
	TEST(status==KErrCancel);

	iSerialPortList[0].Close();
	return TestStepResult();
	}
	
CTestStartServerThread::CTestStartServerThread()
	{
	SetTestStepName(_L("TestStartServerThread"));
	}

TVerdict CTestStartServerThread::doTestStepL()
/**
 * @test Test Starting Thread within C32 Process
 *
 * @note This test requires TTHREAD, a separate test program, to have been built.
 */
	{	
	INFO_PRINTF1(_L("=========================================="));
	INFO_PRINTF1(_L("Test : TestStartServerThread"));
	INFO_PRINTF1(_L("=========================================="));
		
	INFO_PRINTF1(_L("Test Starting Thread within C32 Process"));
	
	_LIT(KLibraryName,"TThread");	
	_LIT(KThreadName,"newserver");
	TInt KDefaultStackSize=0x2000;
	TInt KHeapSize=0x1000;
	TInt KMaxHeapSize=0x40000;

		TInt res=iCommSession.CreateThreadInCommProc(KLibraryName,KThreadName,ServerThread,KDefaultStackSize,KHeapSize,KMaxHeapSize);
		TESTCHECKL(res, KErrNotSupported);

	return TestStepResult();
	}	

CTestCapsV02::CTestCapsV02()
	{
	SetTestStepName(_L("TestCapsV02"));
	}

TVerdict CTestCapsV02::doTestStepL()
/**
 * @test Test capabilities version 2
 */
	{
	INFO_PRINTF1(_L("=========================================="));
	INFO_PRINTF1(_L("Test : TestCapsV02"));
	INFO_PRINTF1(_L("=========================================="));

	INFO_PRINTF1(_L("Test capabilities version 2"));

	iCommSession.__DbgMarkHeap();

	//
	// try COMM::0 first, then COMM::1
	//
	TInt res = iSerialPortList[0].Open(iCommSession, KCommPort0, ECommShared);
	if(res != KErrNone)
		{
		res = iSerialPortList[0].Open(iCommSession, KCommPort1, ECommShared);
		TESTCHECKL(res, KErrNone);
		}

	iSerialPortList[0].Close();
	iCommSession.__DbgMarkHeap();
	res = iSerialPortList[0].Open(iCommSession, KCommPort0, ECommShared);
	if(res != KErrNone)
		{
		res = iSerialPortList[0].Open(iCommSession, KCommPort1, ECommShared);
		TESTCHECKL(res, KErrNone);
		}

	TCommCaps2 caps;

	TInt ret = iSerialPortList[0].Caps(caps);
	TESTCHECKL(ret, KErrNone);

	TUint notifycaps = caps().iNotificationCaps;
	INFO_PRINTF1(_L("Capabilities:"));
	if (notifycaps&KNotifySignalsChangeSupported)
		INFO_PRINTF1(_L("Notify Signals Change supported"));
	if (notifycaps&KNotifyRateChangeSupported)
		INFO_PRINTF1(_L("Notify Rate Change supported"));
	if (notifycaps&KNotifyDataFormatChangeSupported)
		INFO_PRINTF1(_L("Notify Data Format Change supported"));
	if (notifycaps&KNotifyHandshakeChangeSupported)
		INFO_PRINTF1(_L("Notify Handshake Change supported"));
	if (notifycaps&KNotifyBreakSupported)
		INFO_PRINTF1(_L("Notify Break supported"));
	if (notifycaps&KNotifyFlowControlChangeSupported)
		INFO_PRINTF1(_L("Notify Flow Control Change supported"));
	if (notifycaps&KNotifyDataAvailableSupported)
		INFO_PRINTF1(_L("Notify Data Available supported"));
	if (notifycaps&KNotifyOutputEmptySupported)
		INFO_PRINTF1(_L("Notify Output Empty supported"));

	if ((caps().iRoleCaps)&KCapsRoleSwitchSupported)
		INFO_PRINTF1(_L("Role switching is supported"));

	if ((caps().iFlowControlCaps)&KCapsFlowControlStatusSupported)
		INFO_PRINTF1(_L("Retrieve flow control status is supported"));

#ifdef __WINS__
	// not supported on Brutus
	TESTCHECKL((notifycaps==(KNotifySignalsChangeSupported|KNotifyDataAvailableSupported|KNotifyOutputEmptySupported)),1);
#endif
	TESTCHECKL(caps().iRoleCaps,KCapsRoleSwitchSupported);
	TESTCHECKL(caps().iFlowControlCaps,0);
	iSerialPortList[0].Close();
	iCommSession.__DbgMarkEnd(0);

	return TestStepResult();
	}

CTestOpenPortsWithHighNumbers::CTestOpenPortsWithHighNumbers()
	{
	SetTestStepName(_L("TestOpenPortsWithHighNumbers"));
	}

TVerdict CTestOpenPortsWithHighNumbers::doTestStepL()
/**
 *	@test Test that the RComm::Open can handle more than 0-9 comports
 *
 *  It was added on 9. Jan 2000 after the defect BOX-4RLH33 was fixed.
 *
 *  It covers:
 *
 *   o Open a comport with legal values
 *   o Open a comport with illegal values
 *   o Open a comport with garbage after ::
 */
	{
	INFO_PRINTF1(_L("=========================================="));
	INFO_PRINTF1(_L("Test : TestOpenPortsWithHighNumbers"));
	INFO_PRINTF1(_L("=========================================="));

	INFO_PRINTF1(_L("Test Opening Ports with values greater than 10"));
	
	TBool platsecEnforced = EFalse;
	PlatSec::TConfigSetting aSetting(PlatSec::EPlatSecEnforcement);
	if(PlatSec::ConfigSetting(aSetting)>0)
		{
		platsecEnforced = ETrue;
		}

	RCommServ serv;
	TInt res=serv.Connect();
	TESTCHECKL(res, KErrNone);

	_LIT(KCommPrefix, "COMM::" );

	// test with garbage after ::, expect Bad Name
	TInt expectedError;
	if(platsecEnforced)
		{
		INFO_PRINTF1(_L("open ports with garbage after :: - expect KErrBadName (-28)"));
		expectedError = KErrPermissionDenied;	
		}
	else
		{
		INFO_PRINTF1(_L("open ports with garbage after :: - expect KErrBadName (-28)"));
		expectedError = KErrBadName;
		}	
	
	TName portName;
	portName.Append(KCommPrefix);
	portName.AppendFormat(_L("garbage"));
	res=iSerialPortList[0].Open(serv, portName, ECommShared);
	
	
	
	TESTCHECK(res, expectedError);
	INFO_PRINTF2(_L("  open port w/garbage - res=%d "), res);

	if(platsecEnforced)
		{
		INFO_PRINTF1(_L("open ports with illegal values - expect KErrPermissionDenied (-46) "));
		expectedError = KErrPermissionDenied;	
		}
	else
		{
		INFO_PRINTF1(_L("open ports with illegal values - expect KErrBadName (-28) "));
		expectedError = KErrBadName;
		}
	// test with illegal values, expect Bad Name

	for(TInt i=-3;i<0;i++)
		{
		TName portName;
		portName.Append(KCommPrefix);
		portName.AppendFormat(_L("%d"), i);
		res=iSerialPortList[0].Open(serv, portName, ECommShared);
		INFO_PRINTF3(_L("  open port %d - res=%d "), i, res);
		TESTCHECK(res, expectedError);
		}

	// test with legal values, and expect KErrNone, KErrNotSupported, KErrNotReady, KErrUnknown or KErrAccessDenied
	// KMaxUnits is defined as 0x20 in e32std.h and means
	// that the RBusDevComm::Open only take unit numbers 
	// from 0...31
	// if 32 or higher is used, the Kernel with panic
	// with EBadUnitNumber=11
	//
	// TODOAEH: where is this KErrNotReady coming from?
	//          it says so on COMM::3 on the Brutus board...
	//
	INFO_PRINTF1(_L("open ports from 0 to 1 - expect KErrNone, KErrNotSupported, KErrNotReady, KErrUnknown or KErrAccessDenied "));
	for(TInt j=0;j<KMaxUnits;j++)
		{
		TName portName;
		portName.Append(KCommPrefix);
		portName.AppendFormat(_L("%d"), j);
		res=iSerialPortList[0].Open(serv, portName, ECommShared);
		INFO_PRINTF3(_L("  open port %d - res=%d "), j, res);
		TEST(res==KErrNone || res==KErrNotSupported || res==KErrNotReady || res==KErrUnknown || res==KErrAccessDenied );
		iSerialPortList[0].Close();
		}
	
	// test with illegal unit number, make sure that CSY picks this up
	// and return KErrNotSupported
	//
	TInt k = KMaxUnits;
	if(platsecEnforced)
		{
		INFO_PRINTF2(_L("open port %d, illegal unit number for device driver - expect KErrPermissionDenied "),k);
		expectedError = KErrPermissionDenied;	
		}
	else
		{
		INFO_PRINTF2(_L("open port %d, illegal unit number for device driver - expect KErrNotSupported "),k);
		expectedError = KErrNotSupported;
		}	
	
		{
		TName portName;
		portName.Append(KCommPrefix);
		portName.AppendFormat(_L("%d"), k);
		res=iSerialPortList[0].Open(serv, portName, ECommShared);
		INFO_PRINTF3(_L("  open port %d - res=%d "), k, res);
		TEST(res == expectedError);
		}

	//
	// cleanup
	//
	serv.Close();
		
	INFO_PRINTF1(_L("Ports opened OK"));
	return TestStepResult();
	}


CTestTimerAllocation::CTestTimerAllocation()
	{
	SetTestStepName(_L("TestTimerAllocation"));
	}

void CTestTimerAllocation::TestTimerAllocationL(TInt aAllocFailValue)
	{
	CActiveScheduler* scheduler = new (ELeave) CActiveScheduler;
	CleanupStack::PushL(scheduler);
	CActiveScheduler::Install(scheduler);
	
	const TInt KTimeOutValue=1000000;
	TDeltaTimerEntry timer;

	if (aAllocFailValue > 0)
		{
		__UHEAP_SETFAIL(RHeap::EDeterministic, aAllocFailValue);
		CommTimer::Queue(KTimeOutValue, timer);
 		__UHEAP_RESET;
		}
	else
		{
		CommTimer::Queue(KTimeOutValue, timer);
		}

	CleanupStack::Pop(scheduler);
	delete scheduler;
 	}
	
TInt CTestTimerAllocation::TimerThread(TAny* aArg) 
	{
	TInt  failNthAllocValue = *((TInt*) aArg);

	CTrapCleanup::New();
	TRAPD(retVal, TestTimerAllocationL(failNthAllocValue));

 	return retVal;
 	}
	
TVerdict CTestTimerAllocation::doTestStepL()
 /**
  * @test Test that attempts to allocate a timer under OOM conditions.
  */
 	{
 	User::SetJustInTime(EFalse);
  	
 	INFO_PRINTF1(_L("==================================="));
 	INFO_PRINTF1(_L("Test : TestTimerAllocation"));
 	INFO_PRINTF1(_L("==================================="));
 
#ifdef _DEBUG
 	INFO_PRINTF1(_L("Trying to handle a global timer allocation failure"));
 	
	//
	// Test for failure...
	//
 	RThread failureThread;
	TInt  failNthAllocValue(1);

 	TInt res = failureThread.Create(_L("TimerThreadFailure"), TimerThread,
									KDefaultStackSize, KDefaultHeapSizeThread,
									KDefaultHeapSizeThread, &failNthAllocValue);
 	TESTCHECKL(res, KErrNone);
 	TRequestStatus stat;
 	failureThread.Logon(stat);
 	failureThread.Resume();
 	User::WaitForRequest(stat);
 	TESTCHECKSTR(failureThread.ExitCategory(), KCommServerFault);	//"Comm server fault" from cc_utl.cpp
 	TESTCHECK(failureThread.ExitReason(), 7);	// 7==EDTimerAllocFailure from cs_std.h
 	TESTCHECK(failureThread.ExitType(), EExitPanic);
	CLOSE_AND_WAIT(failureThread);

	//
	// Test for success...
	//
 	INFO_PRINTF1(_L("Test global timer allocation success"));
 	
 	RThread successThread;
	failNthAllocValue = 0;

	res=successThread.Create(_L("TimerThreadSuccess"), TimerThread,
							 KDefaultStackSize, KDefaultHeapSizeThread,
							 KDefaultHeapSizeThread, &failNthAllocValue);
 	TESTCHECKL(res, KErrNone);
 	successThread.Logon(stat);
 	successThread.Resume();
 	User::WaitForRequest(stat);
 	TESTCHECKSTR(successThread.ExitCategory(), KCommServerSuccess);
 	TESTCHECK(successThread.ExitReason(), 0);
 	TESTCHECK(successThread.ExitType(), EExitKill);
	CLOSE_AND_WAIT(successThread);
#else
 	INFO_PRINTF1(_L("TestTimerAllocation not executed on release builds."));
#endif

	User::SetJustInTime(ETrue);
  	return TestStepResult();
 	}

CTestReadWrite::CTestReadWrite()
	{
	SetTestStepName(_L("TestReadWrite"));
	}

TVerdict CTestReadWrite::doTestStepL()
/**
 * @test Test Read and write
 */
	{
	INFO_PRINTF1(_L("==================================="));
	INFO_PRINTF1(_L("Test : TestReadWrite"));
	INFO_PRINTF1(_L("==================================="));

	INFO_PRINTF1(_L("Checking read and write"));
	INFO_PRINTF1(_L("This test needs a null modem cable"));

	INFO_PRINTF1(_L("Open ports"));
	TInt res=iSerialPortList[0].Open(iCommSession,KCommPort0,ECommShared);
	TESTCHECKL(res, KErrNone);

	res=iSerialPortList[1].Open(iCommSession, KCommPort1, ECommShared);
	if(res==KErrNotFound)
		{
		INFO_PRINTF1(_L("Cannot do this test - no second serial port"));
		iSerialPortList[0].Close();
		SetTestStepResult(EInconclusive);
		return TestStepResult();
		}

	TESTCHECKL(res, KErrNone);
#if 0
	TCommConfig cBuf;
	TCommConfigV01 &c=cBuf();

	TInt ret=iSerialPortList[0].Config(cBuf);
	TESTCHECKL(ret, KErrNone);
	c.iFifo=EFifoEnable;
	c.iRate=EBps9600;
//	c.iHandshake=0;//KConfigObeyCTS;
	c.iHandshake=KConfigFailDSR | KConfigFailCTS;
	c.iTerminatorCount=0;
	c.iDataBits=EData8;
	c.iParity=EParityNone;
	c.iStopBits=EStop1;
	TESTCHECKL(iSerialPortList[0].SetConfig(cBuf), KErrNone);

	ret=iSerialPortList[1].Config(cBuf);
	TESTCHECKL(ret, KErrNone);
	c.iRate=EBps9600;
//	c.iHandshake=0;//KConfigObeyCTS;
	c.iHandshake=KConfigFailDSR | KConfigFailCTS;
	c.iTerminatorCount=0;
	c.iDataBits=EData8;
	c.iParity=EParityNone;
	c.iStopBits=EStop1;
	TESTCHECKL(iSerialPortList[1].SetConfig(cBuf), KErrNone);
#endif
	const TUint KBufSize=0xff;
	TUint8* inBuf = new (ELeave) TUint8[KBufSize];
	CleanupStack::PushL(inBuf);
	TUint8* outBuf = new (ELeave) TUint8[KBufSize];
	CleanupStack::PushL(outBuf);
	TPtr8 outDes(outBuf,KBufSize,KBufSize);
	TPtr8 inDes(inBuf,KBufSize,KBufSize);

	TRequestStatus readStatus;
	TRequestStatus writeStatus;

	for(TUint i=0;i<10;i++)
		{
		outDes.Fill('A'+i);
		inDes.FillZ();

		iSerialPortList[1].Read(readStatus,KHugeTimeoutValue,inDes,KBufSize);
		
		INFO_PRINTF2(_L( "Write # %d of 10"),i+1);
		iSerialPortList[0].Write(writeStatus,KHugeTimeoutValue,outDes,KBufSize);
		
		User::WaitForRequest(writeStatus);
		if(writeStatus != KErrNone)
			User::WaitForRequest(readStatus); // flush reqs
		TESTCHECKL(writeStatus.Int(), KErrNone);
		
		User::WaitForRequest(readStatus);
		TESTCHECKL(readStatus.Int(), KErrNone);

		outDes.SetLength(inDes.Length());
		TESTCHECKL(inDes.Compare(outDes), 0);
		
		}

	outDes.Fill('B');
	inDes.FillZ();

	INFO_PRINTF1(_L("Read"));
	iSerialPortList[0].Read(readStatus,KHugeTimeoutValue,inDes,KBufSize);

	INFO_PRINTF1(_L("Write"));
	iSerialPortList[1].Write(writeStatus,KHugeTimeoutValue,outDes,KBufSize);
	
	User::WaitForRequest(writeStatus);
	TESTCHECKL(writeStatus.Int(), KErrNone);
	
	User::WaitForRequest(readStatus);
	TESTCHECKL(readStatus.Int(), KErrNone);

	outDes.SetLength(inDes.Length());
	TESTCHECKL(inDes.Compare(outDes), 0);

	TCommServerConfig bmcPckg;
	TCommServerConfigV01 &bmc=bmcPckg();
	bmc.iBufFlags=KCommBufferPartial;
	TInt ret = iSerialPortList[0].Mode(bmcPckg);
	TEST(ret==KErrNone);
	TESTCHECKL(bmc.iBufFlags!=KCommBufferPartial, 1);

	//
	// cleanup
	//
	iSerialPortList[0].Close();
	iSerialPortList[1].Close();
	CleanupStack::PopAndDestroy(2); // inBuf, outBuf
//	delete inBuf;
//	delete outBuf;
	return TestStepResult();	
	}



CTestReadWrite2::CTestReadWrite2()
	{
	SetTestStepName(_L("TestReadWrite"));
	}

TVerdict CTestReadWrite2::doTestStepL()
/**
 * @test Test Read and write
 */
 //This test is created for the purpose of testing Defect DEF113371
	{
	INFO_PRINTF1(_L("==================================="));
	INFO_PRINTF1(_L("Test : TestReadWrite2"));
	INFO_PRINTF1(_L("==================================="));
	
	INFO_PRINTF1(_L("Open ports"));
	TInt res=iSerialPortList[0].Open(iCommSession,KCommPort0,ECommShared);
	TESTCHECKL(res, KErrNone);

	res=iSerialPortList[1].Open(iCommSession,KCommPort1,ECommShared);
	if(res==KErrNotFound)
		{
		INFO_PRINTF1(_L("Cannot do this test - no second serial port"));
		iSerialPortList[0].Close();
		SetTestStepResult(EInconclusive);
		return TestStepResult();
		}
	TESTCHECKL(res, KErrNone);

	TCommConfig cBuf;
	TCommConfigV01 &c=cBuf();

	TInt ret=iSerialPortList[0].Config(cBuf);
	TESTCHECKL(ret, KErrNone);
	c.iRate=EBps9600;
	c.iHandshake=KConfigObeyCTS;
	c.iTerminatorCount=0;
	c.iParity=EParityNone;
	c.iStopBits=EStop1;
	c.iDataBits=EData8;
	res = iSerialPortList[0].SetConfig(cBuf);
	TESTCHECKL(res, KErrNone);

	ret=iSerialPortList[1].Config(cBuf);
	TESTCHECKL(ret, KErrNone);
	c.iRate=EBps9600;
	c.iHandshake=KConfigObeyCTS;
	c.iTerminatorCount=0;
	c.iParity=EParityNone;
	c.iStopBits=EStop1;
	c.iDataBits=EData8;
	TESTCHECKL(iSerialPortList[1].SetConfig(cBuf), KErrNone);
	// enable buffering

	TCommServerConfig bmcPckg;
	TCommServerConfigV01 &bmc=bmcPckg();

	//bmc.iBufFlags=KCommBufferPartial;
	bmc.iBufFlags=0;
	bmc.iBufSize=0x10;

	iSerialPortList[0].SetMode(bmcPckg);
	iSerialPortList[1].SetMode(bmcPckg);
	
	const TUint KBufSize=0x120; //0xff
	
	TUint8* inBuf = new (ELeave) TUint8[KBufSize];
	CleanupStack::PushL(inBuf);

	TUint8* outBuf = new (ELeave) TUint8[KBufSize];
	CleanupStack::PushL(outBuf);

	TPtr8 outDes(outBuf, KBufSize, KBufSize);
	TPtr8 inDes(inBuf, KBufSize, KBufSize);

	outDes.Fill('A');
	inDes.FillZ();	

	TRequestStatus readStatus;
	TRequestStatus writeStatus;

	INFO_PRINTF1(_L("Read"));
	iSerialPortList[0].Read(readStatus,KHugeTimeoutValue,inDes,KBufSize);

	INFO_PRINTF1(_L("Write"));
	iSerialPortList[1].Write(writeStatus,KHugeTimeoutValue,outDes,KBufSize);

	User::WaitForRequest(writeStatus);
	User::WaitForRequest(readStatus);
	outDes.SetLength(inDes.Length());
	TESTCHECKL(inDes.Compare(outDes), 0);

	bmc.iBufFlags=0;
	bmc.iBufSize=0xffffffff;

	ret=iSerialPortList[0].Mode(bmcPckg);
	TEST(ret==KErrNone);

	// cleanup
	//
	iSerialPortList[0].Close();
	iSerialPortList[1].Close();
	CleanupStack::PopAndDestroy(2); // inBuf, outBuf
	
	return TestStepResult();
	}



CTestSignalLines::CTestSignalLines()
	{
	SetTestStepName(_L("TestSignalLines"));
	}

TVerdict CTestSignalLines::doTestStepL()
/**
 * @test Tests of setting and clearing RS232 output lines (DTR and RTS)
 */
	{
	INFO_PRINTF1(_L("==================================="));
	INFO_PRINTF1(_L("Test : TestSignalLines"));
	INFO_PRINTF1(_L("==================================="));

	INFO_PRINTF1(_L("Checking signaling"));

	INFO_PRINTF1(_L("Open ports"));
	TInt res=iSerialPortList[0].Open(iCommSession,KCommPort0,ECommShared);
	TESTCHECKL(res, KErrNone);

	res=iSerialPortList[1].Open(iCommSession,KCommPort1,ECommShared);
	if(res==KErrNotFound)
		{
		INFO_PRINTF1(_L("Cannot do this test - no second serial port"));
		iSerialPortList[0].Close();
		SetTestStepResult(EInconclusive);
		return TestStepResult();
		}
	TESTCHECKL(res, KErrNone);

	TCommConfig conf;
	TInt ret=iSerialPortList[0].Config(conf);
	TESTCHECKL(ret, KErrNone);
	conf().iHandshake=KConfigFreeRTS|KConfigFreeDTR;
	res = iSerialPortList[0].SetConfig(conf);
	TESTCHECKL(res, KErrNone);

	ret=iSerialPortList[1].Config(conf);
	TESTCHECKL(ret, KErrNone);
	conf().iHandshake=KConfigFreeRTS|KConfigFreeDTR;
	TESTCHECKL(iSerialPortList[1].SetConfig(conf), KErrNone);

	iSerialPortList[0].SetSignals(0,KSignalRTS|KSignalDTR);

	iSerialPortList[0].SetSignals(KSignalRTS,KSignalDTR);
	TInt signals=iSerialPortList[1].Signals(0x0f);		// CTS, DSR, DCD, RNG
	TInt sig = KSignalCTS;
	TESTCHECKL(signals, sig);


	iSerialPortList[0].SetSignals(KSignalDTR,KSignalRTS);	// Raises DTR, Clears RTS and DCD
	signals=iSerialPortList[1].Signals(0x0f);				// CTS, DSR, DCD and RNG masked
	TBool pass=(signals==(TInt)KSignalDSR)||(signals==(TInt)(KSignalDSR|KSignalDCD));	// Data transfer cables just pull DSR up, NULL modems pull DSR and DCD up - either is acceptable
	TESTCHECKL(pass, (TInt)ETrue);

	iSerialPortList[0].SetSignals(KSignalDTR|KSignalRTS,0);
	signals=0;
	signals=iSerialPortList[1].Signals(0x0f);
	pass=(signals==(KSignalDSR|KSignalCTS))||(signals==(KSignalDSR|KSignalCTS|KSignalDCD));
	TESTCHECKL(pass,(TInt)ETrue);
	
	iSerialPortList[0].SetSignals(0,KSignalDTR|KSignalRTS);
	signals=0;
	signals=iSerialPortList[1].Signals(0x0f);
	TESTCHECKL(signals, 0);

	iSerialPortList[0].Close();
	iSerialPortList[1].Close();

	
	return TestStepResult();
	
	}

CTestQueryreceiveBuffer::CTestQueryreceiveBuffer()
	{
	SetTestStepName(_L("TestQueryreceiveBuffer"));
	}

TVerdict CTestQueryreceiveBuffer::doTestStepL()
/**
 * @test Test Query Receive Buffer method
 */
	{
	INFO_PRINTF1(_L("==================================="));
	INFO_PRINTF1(_L("Test : TestQueryreceiveBuffer"));
	INFO_PRINTF1(_L("==================================="));

	INFO_PRINTF1(_L("Checking QueryReceiveBuffer"));

	INFO_PRINTF1(_L("Open ports"));
	TInt res=iSerialPortList[0].Open(iCommSession,KCommPort0,ECommShared);
	TESTCHECKL(res, KErrNone);

	res=iSerialPortList[1].Open(iCommSession,KCommPort1,ECommShared);
	if(res==KErrNotFound)
		{
		INFO_PRINTF1(_L("Cannot do this test - no second serial port"));
		iSerialPortList[0].Close();		
		SetTestStepResult(EInconclusive);
		return TestStepResult();
		}
	TESTCHECKL(res, KErrNone);

	INFO_PRINTF1(_L("Zero before driver starts up"));
	res = iSerialPortList[0].QueryReceiveBuffer();
	TESTCHECKL(res, KErrNone);

	TCommConfig cBuf;
	TCommConfigV01 &c=cBuf();

	TInt ret=iSerialPortList[0].Config(cBuf);
	TESTCHECKL(ret, KErrNone);
	c.iFifo=EFifoEnable;
	c.iRate=EBps9600;
	c.iHandshake=KConfigObeyCTS;
	c.iTerminatorCount=0;
	c.iDataBits=EData8;
	c.iParity=EParityNone;
	c.iStopBits=EStop1;
	res = iSerialPortList[0].SetConfig(cBuf);
	TESTCHECKL(res, KErrNone);

	ret=iSerialPortList[1].Config(cBuf);
	TESTCHECKL(ret, KErrNone);
	c.iRate=EBps9600;
	c.iHandshake=KConfigObeyCTS;
	c.iTerminatorCount=0;
	c.iDataBits=EData8;
	c.iParity=EParityNone;
	c.iStopBits=EStop1;
	res = iSerialPortList[1].SetConfig(cBuf);
	TESTCHECKL(res, KErrNone);

	const TUint KBufSize=0xff;
	TUint8* outBuf=new (ELeave) TUint8[KBufSize];
	CleanupStack::PushL(outBuf);
	TPtr8 outDes(outBuf,KBufSize,KBufSize);
	TUint8* inBuf=new (ELeave) TUint8[KBufSize];
	CleanupStack::PushL(inBuf);
	TPtr8 inDes(inBuf,KBufSize,KBufSize);

	TRequestStatus writeStatus;
	TRequestStatus readStatus;

	INFO_PRINTF1(_L("Zero after driver starts"));
	iSerialPortList[0].Read(readStatus,inDes,0);
	User::WaitForRequest(readStatus);
	TESTCHECKL(iSerialPortList[0].QueryReceiveBuffer(), 0);

	INFO_PRINTF1(_L("zero after flush"));
	ret = iSerialPortList[0].ResetBuffers();
	TESTCHECKL(ret, KErrNone);
	TESTCHECKL(iSerialPortList[0].QueryReceiveBuffer(), 0);

	outDes.SetLength(10);
	inDes.SetLength(10);

	iSerialPortList[0].Read(readStatus,KHugeTimeoutValue,inDes,1);
	iSerialPortList[1].Write(writeStatus,KHugeTimeoutValue,outDes);
	User::WaitForRequest(readStatus);
	User::WaitForRequest(writeStatus);
	TESTCHECKL(readStatus.Int(), KErrNone);
	TESTCHECKL(writeStatus.Int(), KErrNone);

	User::After(1000000);	// Wait for everything to filter through the WINs driver - NT may be buffering

	INFO_PRINTF1(_L("test byte counts"));
	TESTCHECKL(iSerialPortList[0].QueryReceiveBuffer(), 9);

	iSerialPortList[0].Read(readStatus,KHugeTimeoutValue,inDes,1);
	iSerialPortList[1].Write(writeStatus,KHugeTimeoutValue,outDes);
	User::WaitForRequest(readStatus);
	User::WaitForRequest(writeStatus);
	TESTCHECKL(readStatus.Int(), KErrNone);
	TESTCHECKL(writeStatus.Int(), KErrNone);

	User::After(1000000);
	TESTCHECKL(iSerialPortList[0].QueryReceiveBuffer(), 18);

	INFO_PRINTF1(_L("zero after flush with data present"));
	ret = iSerialPortList[0].ResetBuffers();
	TESTCHECKL(ret, KErrNone);
	TESTCHECKL(iSerialPortList[0].QueryReceiveBuffer(), 0);

	iSerialPortList[0].Close();
	iSerialPortList[1].Close();

	CleanupStack::PopAndDestroy(2); // inBuf, outBuf
//	delete outBuf;
//	delete inBuf;
	
	return TestStepResult();
	
	}

CTestWrite0::CTestWrite0()
	{
	SetTestStepName(_L("TestWrite0"));
	}

TVerdict CTestWrite0::doTestStepL()
/**
 * @test Test Write 0 functionality
 */
	{
	INFO_PRINTF1(_L("==================================="));
	INFO_PRINTF1(_L("Test : TestWrite0n"));
	INFO_PRINTF1(_L("==================================="));

	INFO_PRINTF1(_L("Testing Write 0"));

	INFO_PRINTF1(_L("Open ports"));
	TInt res = iSerialPortList[0].Open(iCommSession, KCommPort0, ECommShared);
	TESTCHECKL(res, KErrNone);

	// Turn handshaking on.
	TCommConfig cBuf;
	TCommConfigV01 &c=cBuf();
	TInt ret=iSerialPortList[0].Config(cBuf);
	TESTCHECKL(ret, KErrNone);
	c.iRate=EBps9600;
	c.iHandshake=KConfigObeyCTS;
	c.iTerminatorCount=0;
	TESTCHECKL(iSerialPortList[0].SetConfig(cBuf), KErrNone);

	// Write 0 - should time out
	const TInt KTimeOutValue=1000000;
	INFO_PRINTF1(_L("Write(0) to timeout"));
	TRequestStatus writeStat;
	iSerialPortList[0].Write(writeStat, KTimeOutValue, TPtr8(NULL,0), 0);
	User::WaitForRequest(writeStat);
	TESTCHECKL(writeStat.Int(), KErrTimedOut);

	// Write 0 should complete KErrNone when we open the port.
	INFO_PRINTF1(_L("Write(0) to completion"));
	iSerialPortList[0].Write(writeStat, KHugeTimeoutValue, TPtr8(NULL,0), 0);
	res=iSerialPortList[1].Open(iCommSession, KCommPort1, ECommShared);
	if(res==KErrNotFound)
		{
		INFO_PRINTF1(_L("Cannot do this test - no second serial port"));
		iSerialPortList[0].Close();
		SetTestStepResult(EInconclusive);
		return TestStepResult();
		}
	TESTCHECKL(res, KErrNone);
	ret=iSerialPortList[1].Config(cBuf);
	TESTCHECKL(ret, KErrNone);
	c.iRate=EBps9600;
	c.iHandshake=KConfigObeyCTS;
	TESTCHECKL(iSerialPortList[1].SetConfig(cBuf), KErrNone);
	
	TBuf8<2> buf;
	TRequestStatus readStat;
	iSerialPortList[1].Read(readStat,buf,1);
	User::WaitForRequest(writeStat);
	ret = iSerialPortList[1].ReadCancel();
	TESTCHECKL(ret, KErrNone);
	if(writeStat != KErrNone)
		User::WaitForRequest(readStat); // flush
	TESTCHECKL(writeStat.Int(), KErrNone);
	iSerialPortList[1].Close();

	// Write 0 - should time out again
	INFO_PRINTF1(_L("Write(0) to timeout"));
	iSerialPortList[0].Write(writeStat, KTimeOutValue, TPtr8(NULL,0), 0);
	User::WaitForRequest(writeStat);
	TESTCHECKL(writeStat.Int(), KErrTimedOut);

	INFO_PRINTF1(_L("check read 0"));
	TPtr8 p(NULL,0);
	iSerialPortList[0].Read(readStat, 10, p, 0);
	User::WaitForRequest(writeStat);

	// cleanup
	ret = iSerialPortList[0].ReadCancel();
	TESTCHECKL(ret, KErrNone);
	User::WaitForRequest(readStat);
	iSerialPortList[0].Close();

	return TestStepResult();
	}	

CTestReadOneOrMore::CTestReadOneOrMore()
	{
	SetTestStepName(_L("TestReadOneOrMore"));
	}

TVerdict CTestReadOneOrMore::doTestStepL()
/**
 * @test Test Read One or more
 */
	{
	INFO_PRINTF1(_L("==================================="));
	INFO_PRINTF1(_L("Test : TestReadOneOrMore"));
	INFO_PRINTF1(_L("==================================="));

	INFO_PRINTF1(_L("Test Read One or more"));

	INFO_PRINTF1(_L("Open ports"));
	TInt res=iSerialPortList[0].Open(iCommSession,KCommPort0,ECommShared);
	TESTCHECKL(res, KErrNone);
	res=iSerialPortList[1].Open(iCommSession,KCommPort1,ECommShared);
	if(res==KErrNotFound)
		{
		INFO_PRINTF1(_L("Cannot do this test - no second serial port"));
		iSerialPortList[0].Close();		
		SetTestStepResult(EInconclusive);
		return TestStepResult();
		}
	TESTCHECKL(res, KErrNone);

	// Turn handshaking on.
	TCommConfig cBuf;
	TCommConfigV01 &c=cBuf();
	TInt ret=iSerialPortList[0].Config(cBuf);
	TESTCHECKL(ret, KErrNone);
	c.iRate=EBps9600;
	c.iHandshake=0;
	c.iTerminatorCount=0;
	TESTCHECKL(iSerialPortList[0].SetConfig(cBuf), KErrNone);

	ret=iSerialPortList[1].Config(cBuf);
	TESTCHECKL(ret, KErrNone);
	c.iRate=EBps9600;
	c.iHandshake=0;
	c.iTerminatorCount=0;
	TESTCHECKL(iSerialPortList[1].SetConfig(cBuf), KErrNone);

	const TUint KBufSize = 0x10;
	TUint8* inBuf = new (ELeave) TUint8[KBufSize];
	CleanupStack::PushL(inBuf);
	TUint8* outBuf = new (ELeave) TUint8[KBufSize];
	CleanupStack::PushL(outBuf);
	TPtr8 outDes(outBuf, KBufSize, KBufSize);
	TPtr8 inDes(inBuf, KBufSize, KBufSize);

	outDes.Fill('A');
	inDes.FillZ();

	outDes.Fill('B');
	inDes.FillZ();

	ret = iSerialPortList[0].ResetBuffers();
	TESTCHECKL(ret, KErrNone);

	TRequestStatus status;
	iSerialPortList[0].Read(status, KHugeTimeoutValue, inDes, 1);

	TRequestStatus writeStat;
	INFO_PRINTF1(_L("ReadOneorMore with data in buffer"));
	iSerialPortList[1].Write(writeStat, KHugeTimeoutValue, outDes);
	User::After(1000000);
	User::WaitForRequest(writeStat);
	User::WaitForRequest(status);

	TESTCHECKL(status.Int(), KErrNone);
	TESTCHECKL(iSerialPortList[0].QueryReceiveBuffer()>0, TRUE);
	TRequestStatus readStat;
	inDes.SetLength(10);
	iSerialPortList[0].ReadOneOrMore(readStat,inDes);
	User::WaitForRequest(readStat);
	TESTCHECKL((inDes.Length()>1), TRUE);

	ret = iSerialPortList[0].ResetBuffers();
	TESTCHECKL(ret, KErrNone);

	INFO_PRINTF1(_L("ReadOneorMore on empty buffer"));
	iSerialPortList[0].ReadOneOrMore(readStat,inDes);
	iSerialPortList[1].Write(writeStat,outDes);
	User::WaitForRequest(writeStat);
	User::WaitForRequest(readStat);
	TESTCHECKL(inDes.Length(), 1);

	ret = iSerialPortList[0].ResetBuffers();
	TESTCHECKL(ret, KErrNone);

	//
	// cleanup
	//
	iSerialPortList[1].Close();
	iSerialPortList[0].Close();
	CleanupStack::PopAndDestroy(2); // inBuf, outBuf
//	delete inBuf;
//	delete outBuf;

	return TestStepResult();
	}

CTestBufferReadWrite::CTestBufferReadWrite()
	{
	SetTestStepName(_L("TestBufferReadWrite"));
	}

TVerdict CTestBufferReadWrite::doTestStepL()
/**
 * @test Test Buffered Read and write
 */
	{
	INFO_PRINTF1(_L("=========================================="));
	INFO_PRINTF1(_L("Test : TestBufferReadWrite"));
	INFO_PRINTF1(_L("=========================================="));

	INFO_PRINTF1(_L("Buffered Read and write"));

	INFO_PRINTF1(_L("Open ports"));
	TInt res=iSerialPortList[0].Open(iCommSession,KCommPort0,ECommShared);
	TESTCHECKL(res, KErrNone);

	res=iSerialPortList[1].Open(iCommSession,KCommPort1,ECommShared);
	if(res==KErrNotFound)
		{
		INFO_PRINTF1(_L("Cannot do this test - no second serial port"));
		iSerialPortList[0].Close();
		SetTestStepResult(EInconclusive);
		return TestStepResult();
		}
	TESTCHECKL(res, KErrNone);

	TCommConfig cBuf;
	TCommConfigV01 &c=cBuf();

	TInt ret=iSerialPortList[0].Config(cBuf);
	TESTCHECKL(ret, KErrNone);
	c.iRate=EBps9600;
	c.iHandshake=KConfigObeyCTS;
	c.iTerminatorCount=0;
	c.iParity=EParityNone;
	c.iStopBits=EStop1;
	c.iDataBits=EData8;
	res = iSerialPortList[0].SetConfig(cBuf);
	TESTCHECKL(res, KErrNone);

	ret=iSerialPortList[1].Config(cBuf);
	TESTCHECKL(ret, KErrNone);
	c.iRate=EBps9600;
	c.iHandshake=KConfigObeyCTS;
	c.iTerminatorCount=0;
	c.iParity=EParityNone;
	c.iStopBits=EStop1;
	c.iDataBits=EData8;
	TESTCHECKL(iSerialPortList[1].SetConfig(cBuf), KErrNone);
	// enable buffering

	TCommServerConfig bmcPckg;
	TCommServerConfigV01 &bmc=bmcPckg();

	//bmc.iBufFlags=KCommBufferPartial;
	bmc.iBufFlags=0;
	bmc.iBufSize=0x10;

	iSerialPortList[0].SetMode(bmcPckg);
	iSerialPortList[1].SetMode(bmcPckg);
	
	const TUint KBufSize=0x100;
	TUint8* inBuf = new (ELeave) TUint8[KBufSize];
	CleanupStack::PushL(inBuf);
	TUint8* outBuf = new (ELeave) TUint8[KBufSize];
	CleanupStack::PushL(outBuf);
	TPtr8 outDes(outBuf, KBufSize, KBufSize);
	TPtr8 inDes(inBuf, KBufSize, KBufSize);

	outDes.Fill('A');
	inDes.FillZ();	

	TRequestStatus readStatus;
	TRequestStatus writeStatus;

	INFO_PRINTF1(_L("Read"));
	iSerialPortList[0].Read(readStatus, KHugeTimeoutValue, inDes, KBufSize);
	INFO_PRINTF1(_L("Write"));
	iSerialPortList[1].Write(writeStatus,KHugeTimeoutValue,outDes,KBufSize);
	User::WaitForRequest(writeStatus);
	if(writeStatus != KErrNone)
		User::WaitForRequest(readStatus); // flush
	TESTCHECKL(writeStatus.Int(), KErrNone);
	
	User::WaitForRequest(readStatus);
	TESTCHECKL(readStatus.Int(), KErrNone);

	outDes.SetLength(inDes.Length());
	TESTCHECKL(inDes.Compare(outDes), 0);

	INFO_PRINTF1(_L("Read With Invalid Parameter"));
	iSerialPortList[0].Read(readStatus, KHugeTimeoutValue, inDes, KInvalidArgument);

	User::WaitForRequest(readStatus);
	TESTCHECKL(readStatus.Int(), KErrArgument);

	INFO_PRINTF1(_L("Read With Parameter = MaxLength + 1"));
	iSerialPortList[0].Read(readStatus, KHugeTimeoutValue, inDes, inDes.MaxLength() + 1);

	User::WaitForRequest(readStatus);
	TESTCHECKL(readStatus.Int(), KErrArgument);

	INFO_PRINTF1(_L("Write With Invalid Argument"));
	iSerialPortList[1].Write(writeStatus,KHugeTimeoutValue,outDes,KInvalidArgument);

	User::WaitForRequest(writeStatus);
	TESTCHECKL(writeStatus.Int(), KErrArgument);

	INFO_PRINTF1(_L("Write With Parameter = Length + 1"));
	iSerialPortList[1].Write(writeStatus,KHugeTimeoutValue,outDes,outDes.Length() + 1);

	User::WaitForRequest(writeStatus);
	TESTCHECKL(writeStatus.Int(), KErrArgument);

	outDes.Fill('B');
	inDes.FillZ();

	INFO_PRINTF1(_L("Read"));
	iSerialPortList[0].Read(readStatus,inDes,KBufSize);

	INFO_PRINTF1(_L("Write"));
	iSerialPortList[1].Write(writeStatus,outDes,KBufSize);
	User::WaitForRequest(writeStatus);
	TESTCHECKL(writeStatus.Int(), KErrNone);
	
	User::WaitForRequest(readStatus);
	TESTCHECKL(readStatus.Int(), KErrNone);

	INFO_PRINTF1(_L("Read2 With Invalid Parameter"));
	iSerialPortList[0].Read(readStatus,inDes,KInvalidArgument);

	User::WaitForRequest(readStatus);
	TESTCHECKL(readStatus.Int(), KErrArgument);

	INFO_PRINTF1(_L("Read2 With Parameter = MaxLength + 1"));
	iSerialPortList[0].Read(readStatus,inDes,inDes.MaxLength() + 1);

	User::WaitForRequest(readStatus);
	TESTCHECKL(readStatus.Int(), KErrArgument);

	INFO_PRINTF1(_L("Write2 With Invalid Argument"));
	iSerialPortList[1].Write(writeStatus,outDes,KInvalidArgument);

	User::WaitForRequest(writeStatus);
	TESTCHECKL(writeStatus.Int(), KErrArgument);

	INFO_PRINTF1(_L("Write2 With Parameter = Length + 1"));
	iSerialPortList[1].Write(writeStatus,outDes,outDes.Length() + 1);

	User::WaitForRequest(writeStatus);
	TESTCHECKL(writeStatus.Int(), KErrArgument);

	outDes.SetLength(inDes.Length());
	TESTCHECKL(inDes.Compare(outDes), 0);

	bmc.iBufFlags=0;
	bmc.iBufSize=0xffffffff;

	ret=iSerialPortList[0].Mode(bmcPckg);
	TEST(ret==KErrNone);

	//test(bmc.iBufFlags==KCommBufferPartial);
	//test(bmc.iBufSize==0x10);

	//
	// cleanup
	//
	iSerialPortList[0].Close();
	iSerialPortList[1].Close();
	CleanupStack::PopAndDestroy(2); // inBuf, outBuf
	
	return TestStepResult();
	}

CTestBufferreadOneOrMore::CTestBufferreadOneOrMore()
	{
	SetTestStepName(_L("TestBufferreadOneOrMore"));
	}

TVerdict CTestBufferreadOneOrMore::doTestStepL()
/**
 * @test Test Buffered Read One or more
 */
	{
	INFO_PRINTF1(_L("=========================================="));
	INFO_PRINTF1(_L("Test : TestBufferreadOneOrMore"));
	INFO_PRINTF1(_L("=========================================="));

	INFO_PRINTF1(_L("Test Buffered Read One or more"));

	INFO_PRINTF1(_L("Open ports"));
	TInt res=iSerialPortList[0].Open(iCommSession,KCommPort0,ECommShared);
	TESTCHECKL(res, KErrNone);
	res=iSerialPortList[1].Open(iCommSession,KCommPort1,ECommShared);
	if(res==KErrNotFound)
		{
		INFO_PRINTF1(_L("Cannot do this test - no second serial port"));
		iSerialPortList[0].Close();
		SetTestStepResult(EInconclusive);
		return TestStepResult();
		}
	TESTCHECKL(res, KErrNone);

	// Turn handshaking on.
	TCommConfig cBuf;
	TCommConfigV01 &c=cBuf();
	TInt ret=iSerialPortList[0].Config(cBuf);
	TESTCHECKL(ret, KErrNone);
	c.iRate=EBps9600;
	c.iHandshake=0;
	c.iTerminatorCount=0;
	iSerialPortList[0].SetConfig(cBuf);

	ret=iSerialPortList[1].Config(cBuf);
	TESTCHECKL(ret, KErrNone);
	c.iRate=EBps9600;
	c.iHandshake=0;
	c.iTerminatorCount=0;
	iSerialPortList[1].SetConfig(cBuf);
	// enable buffering

	TCommServerConfig bmcPckg;
	TCommServerConfigV01 &bmc=bmcPckg();

	bmc.iBufFlags=KCommBufferPartial;
	bmc.iBufSize=0x10;

	iSerialPortList[0].SetMode(bmcPckg);
	iSerialPortList[1].SetMode(bmcPckg);

	const TUint KBufSize=0x100;
	TUint8* inBuf = new (ELeave) TUint8[KBufSize];
	CleanupStack::PushL(inBuf);
	TUint8* outBuf = new (ELeave) TUint8[KBufSize];
	CleanupStack::PushL(outBuf);
	TPtr8 outDes(outBuf, KBufSize, KBufSize);
	TPtr8 inDes(inBuf, KBufSize, KBufSize);

	outDes.Fill('A');
	inDes.FillZ();	

	outDes.Fill('B');
	inDes.FillZ();

	ret = iSerialPortList[0].ResetBuffers();
	TESTCHECKL(ret, KErrNone);

	TRequestStatus status;
	iSerialPortList[0].Read(status, KHugeTimeoutValue, inDes, 1);

	TRequestStatus writeStat;
	INFO_PRINTF1(_L("ReadOneorMore with data in buffer"));
	iSerialPortList[1].Write(writeStat, KHugeTimeoutValue, outDes);
	User::After(1000000);
	User::WaitForRequest(writeStat);
	User::WaitForRequest(status);

	TESTCHECKL(status.Int(), KErrNone);
	TESTCHECKL(iSerialPortList[0].QueryReceiveBuffer()>0, TRUE);
	TRequestStatus readStat;
	inDes.SetLength(10);
	iSerialPortList[0].ReadOneOrMore(readStat,inDes);
	User::WaitForRequest(readStat);
	TESTCHECKL((inDes.Length()>1), TRUE);

	ret = iSerialPortList[0].ResetBuffers();
	TESTCHECKL(ret, KErrNone);

	INFO_PRINTF1(_L("ReadOneorMore on empty buffer"));
	iSerialPortList[0].ReadOneOrMore(readStat,inDes);
	iSerialPortList[1].Write(writeStat,outDes);
	User::WaitForRequest(writeStat);
	User::WaitForRequest(readStat);
	TESTCHECKL(inDes.Length(), 1);

	ret = iSerialPortList[0].ResetBuffers();
	TESTCHECKL(ret, KErrNone);

	//
	// cleanup
	//
	iSerialPortList[1].Close();
	iSerialPortList[0].Close();
	CleanupStack::PopAndDestroy(2); // inBuf, outBuf

	return TestStepResult();
	}

CTestNotifySignals::CTestNotifySignals()
	{
	SetTestStepName(_L("TestNotifySignals"));
	}

TVerdict CTestNotifySignals::doTestStepL()
/**
 * @test Test notification of signals changing
 */
	{
	INFO_PRINTF1(_L("=========================================="));
	INFO_PRINTF1(_L("Test : TestNotifySignals"));
	INFO_PRINTF1(_L("=========================================="));

	INFO_PRINTF1(_L("Test notification of signals changing"));
// PRE-STRTECH sub-session containers START

	TInt res=iSerialPortList[0].Open(iCommSession,KCommPort0,ECommShared);
	TESTCHECKL(res, KErrNone);
	iSerialPortList[0].Close();

// PRE-STRECT END
	iCommSession.__DbgMarkHeap();
	res=iSerialPortList[0].Open(iCommSession,KCommPort0,ECommShared);
	TESTCHECKL(res, KErrNone);

	res=iSerialPortList[1].Open(iCommSession,KCommPort1,ECommShared);
	if(res==KErrNotFound)
		{
		INFO_PRINTF1(_L("Cannot do this test - no second serial port"));
		iSerialPortList[0].Close();
		SetTestStepResult(EInconclusive);
		return TestStepResult();
		}
	TESTCHECKL(res, KErrNone);
	
	TCommConfig conf;

	TInt ret=iSerialPortList[0].Config(conf);
	TESTCHECKL(ret, KErrNone);
	conf().iHandshake=KConfigFreeRTS|KConfigFreeDTR;		// we take control of outputs
	TESTCHECKL(iSerialPortList[0].SetConfig(conf), KErrNone);

	ret=iSerialPortList[1].Config(conf);
	TESTCHECKL(ret, KErrNone);
	conf().iHandshake=KConfigFreeRTS|KConfigFreeDTR;
	TESTCHECKL(iSerialPortList[1].SetConfig(conf), KErrNone);

	TRequestStatus status,status2;
	TUint signals;
	iSerialPortList[1].SetSignals(0,KSignalRTS|KSignalDTR);

	iSerialPortList[0].NotifySignalChange(status,signals);
	ret = iSerialPortList[1].SetSignalsToMark(KSignalRTS);
	TESTCHECKL(ret, KErrNone);
	
	User::WaitForRequest(status);
	TESTCHECKL(status.Int(), KErrNone);
	TESTCHECKL(signals,(KSignalCTS|KCTSChanged));

// Test that the notification signal goes off when signals lower as well as rise
	iSerialPortList[0].NotifySignalChange(status,signals);
	ret = iSerialPortList[1].SetSignalsToSpace(KSignalRTS);
	TESTCHECKL(ret, KErrNone);
	User::WaitForRequest(status);
	TESTCHECKL(status.Int(), KErrNone);
	TESTCHECKL(signals,KCTSChanged);

	iSerialPortList[0].NotifySignalChange(status,signals);
	ret = iSerialPortList[1].SetSignalsToMark(KSignalDTR);
	TESTCHECKL(ret, KErrNone);
	User::WaitForRequest(status);
	TESTCHECKL(status.Int(), KErrNone);
	TBool pass=(signals==(KSignalDSR|KDSRChanged))||(signals==(KSignalDSR|KSignalDCD|KDSRChanged|KDCDChanged));
	TESTCHECKL(pass,(TInt)ETrue);
	
	ret = iSerialPortList[1].SetSignalsToSpace(KSignalDTR);
	TESTCHECKL(ret, KErrNone);
	signals = iSerialPortList[0].Signals();
	TESTCHECKL(signals,0);

// Test the KErrInUse that should be returned when a notification of data available is posted at the same
// time as a Read - or visa versa.
	TBuf8<10> buf;
	iSerialPortList[0].ReadOneOrMore(status,buf);
	iSerialPortList[0].NotifyDataAvailable(status2);
	User::WaitForRequest(status2);
	TESTCHECKL(status2.Int(),KErrInUse);
	ret = iSerialPortList[0].ReadCancel();
	TESTCHECKL(ret, KErrNone);
	User::WaitForRequest(status);
	TESTCHECKL(status.Int(),KErrCancel);

	iSerialPortList[0].NotifyDataAvailable(status);
	iSerialPortList[0].ReadOneOrMore(status2,buf);
	User::WaitForRequest(status2);
	TESTCHECKL(status2.Int(),KErrInUse);
	ret = iSerialPortList[0].NotifyDataAvailableCancel();
	TESTCHECKL(ret, KErrNone);
	User::WaitForRequest(status);
	TESTCHECKL(status.Int(),KErrCancel);

	iSerialPortList[0].Close();
	iSerialPortList[1].Close();
	iCommSession.__DbgMarkEnd(0);

	return TestStepResult();
	}

CTestRxDataAvailable::CTestRxDataAvailable()
	{
	SetTestStepName(_L("TestRxDataAvailable"));
	}

TVerdict CTestRxDataAvailable::doTestStepL()
/**
 * @test Test notification of data being newly available to be read
 */
	{
	INFO_PRINTF1(_L("=========================================="));
	INFO_PRINTF1(_L("Test : TestRxDataAvailable"));
	INFO_PRINTF1(_L("=========================================="));

	INFO_PRINTF1(_L("Test notification of data being newly available to be read"));
// PRE-STRECT subsession containers START

	TInt res=iSerialPortList[0].Open(iCommSession,KCommPort0,ECommShared);
	TESTCHECKL(res, KErrNone);
	iSerialPortList[0].Close();

// PRE-STRETCH END
	iCommSession.__DbgMarkHeap();
	res=iSerialPortList[0].Open(iCommSession,KCommPort0,ECommShared);
	TESTCHECKL(res, KErrNone);
	
	res=iSerialPortList[1].Open(iCommSession, KCommPort1, ECommShared);
	if(res==KErrNotFound)
		{
		INFO_PRINTF1(_L("Cannot do this test - no second serial port"));
		iSerialPortList[0].Close();
		SetTestStepResult(EInconclusive);
		return TestStepResult();
		}
	TESTCHECKL(res, KErrNone);

	TRequestStatus status;
	TRequestStatus status2;
	iSerialPortList[0].NotifyDataAvailable(status);

	_LIT8(KBuffer,"Data");
	iSerialPortList[1].Write(status2, KHugeTimeoutValue, KBuffer);
	User::WaitForRequest(status2);
	if(status2 != KErrNone)
		{
		TInt ret = iSerialPortList[0].Cancel();
		TEST(ret==KErrNone);
		User::WaitForRequest(status); // flush
		}
	TESTCHECKL(status2.Int(), KErrNone);

	User::WaitForRequest(status);	// wait until the data is available
	TESTCHECKL(status.Int(), KErrNone);

	TBuf8<30> receive;
	iSerialPortList[0].ReadOneOrMore(status,receive);
	User::WaitForRequest(status);
	TESTCHECKL(status.Int(), KErrNone);
	TESTCHECKL((receive==KBuffer), 1); // check that the received data equals sent data

	iSerialPortList[0].Close();
	iSerialPortList[1].Close();
	iCommSession.__DbgMarkEnd(0);

	return TestStepResult();
	}	

CTestHelloEllo::CTestHelloEllo()
	{
	SetTestStepName(_L("TestHelloEllo"));
	}

TVerdict CTestHelloEllo::doTestStepL()
/**
 * @test Test that sending 'Hello' is not received as 'ello'
 *
 * @note attempt to reproduce defect CLE-4TVLRD 'Querying output buffer state loses a byte on timeout'
 */
	{
	INFO_PRINTF1(_L("==================================="));
	INFO_PRINTF1(_L("Test : TestHelloEllo"));
	INFO_PRINTF1(_L("==================================="));

	INFO_PRINTF1(_L("test Hello - Ello"));

	//
	// Open first com port
	//
	INFO_PRINTF1(_L("Open ports"));
	TInt res = iSerialPortList[0].Open(iCommSession, KCommPort0, ECommShared);
	TESTCHECKL(res, KErrNone);

	//
	// set config - 115200, 8N1, HW flowcontrol
	//
	TCommConfig cBuf;
	TCommConfigV01 &c=cBuf();

	TInt ret=iSerialPortList[0].Config(cBuf);
	TESTCHECKL(ret, KErrNone);
	c.iFifo=EFifoEnable;
	c.iRate=EBps115200;
	c.iHandshake=KConfigObeyCTS | KConfigObeyDSR | KConfigWriteBufferedComplete;
	c.iTerminatorCount=0;
	c.iDataBits=EData8;
	c.iParity=EParityNone;
	c.iStopBits=EStop1;
	TESTCHECKL(iSerialPortList[0].SetConfig(cBuf), KErrNone);

	//
	// flush and test
	//
	ret = iSerialPortList[0].ResetBuffers();
	TESTCHECKL(ret, KErrNone);
	TESTCHECKL(iSerialPortList[0].QueryReceiveBuffer(), 0);

	const TTimeIntervalMicroSeconds32 timeOut = 2000000;
	RTest gTest(_L("Test Window"));
	FOREVER
		{
		INFO_PRINTF1(_L("press a key"));
		TInt key = gTest.Getch();
		if(key == 'Q' || key == 'q')
			User::Leave(KErrAbort);

		TRequestStatus status;
		iSerialPortList[0].Write(status, timeOut, _L8("Hello"));
		User::WaitForRequest(status);
		INFO_PRINTF2(_L("iStatus=%d"), status.Int());
		INFO_PRINTF2(_L("recBuffer=%d"), iSerialPortList[0].QueryReceiveBuffer());

		if(status.Int() == KErrTimedOut)
			{
			INFO_PRINTF2(_L("recBuffer=%d"), iSerialPortList[0].QueryReceiveBuffer());
			iSerialPortList[0].Write(status, KNullDesC8); // length=0
			User::WaitForRequest(status);
			INFO_PRINTF1(_L("wrote 0 bytes"));
			INFO_PRINTF2(_L("recBuffer=%d"), iSerialPortList[0].QueryReceiveBuffer());
			}

		}

	// Unreachable code...
	//gTest.Close();
/*
	TRequestStatus notifyStatus;
	TUint signals;
	TUint signalMask = 0x3f;
	FOREVER
		{
		iSerialPortList[0].NotifySignalChange(notifyStatus, signals, signalMask);
		User::WaitForRequest(notifyStatus);
		if(notifyStatus.Int() != KErrNone)
			INFO_PRINTF2(_L("Hmm. iStatus=%d\n"), notifyStatus.Int());
		else
			INFO_PRINTF2(_L("sigs: 0x%08x\n"), signals);
		}


	//
	// cleanup
	//
	iSerialPortList[0].Close();
	iSerialPortList[1].Close();

	test.End();
*/	
	// Unreachable code...
	//return TestStepResult();
	}

CLsc32ListAvailablePorts::CLsc32ListAvailablePorts()
	{
	SetTestStepName(_L("Lsc32ListAvailablePorts"));
	}

TVerdict CLsc32ListAvailablePorts::doTestStepL()
/**
 * @test Test for listing available serial comm ports
 */
	{
	INFO_PRINTF1(_L("==================================="));
	INFO_PRINTF1(_L("Test : Lsc32ListAvailablePorts"));
	INFO_PRINTF1(_L("==================================="));

	INFO_PRINTF1(_L("available ports:"));

	const TInt KNumberOfPorts = 5;
	RComm ports[KNumberOfPorts];
	_LIT(KCommNamePrefix, "COMM::");
	
	INFO_PRINTF1(_L("--------  DTE  DCE"));
	for(TInt i=0;i<KNumberOfPorts;i++)
		{
		TBuf<128> comName;
		comName.Append(KCommNamePrefix);
		comName.AppendFormat(_L("%d"), i);
		TInt dte = ports[i].Open(iCommSession, comName, ECommShared, ECommRoleDTE);
		if(dte==KErrNone)
			ports[i].Close();
		TInt dce = ports[i].Open(iCommSession, comName, ECommShared, ECommRoleDCE);
		if(dce==KErrNone)
			ports[i].Close();
		INFO_PRINTF1(comName);
		INFO_PRINTF3(_L("   %d    %d"), dte, dce);
		}

	return TestStepResult();
	}	

	
CLsc32ListNumberOfLoadedCSYs::CLsc32ListNumberOfLoadedCSYs()
	{
	SetTestStepName(_L("Lsc32ListNumberOfLoadedCSYs"));
	}

TVerdict CLsc32ListNumberOfLoadedCSYs::doTestStepL()
/**
 * @test Tests number of loaded CSY modules  
 */
	{
	INFO_PRINTF1(_L("==================================="));
	INFO_PRINTF1(_L("Test : Lsc32ListNumberOfLoadedCSYs"));
	INFO_PRINTF1(_L("==================================="));

	TInt csys;
	TInt ret = iCommSession.NumPorts(csys);
	TEST(ret == KErrNone);
	if(ret != KErrNone)
		{
		INFO_PRINTF2(_L("ERROR: could not get number of loaded CSY's due to %d"), ret);
		SetTestStepResult(EFail);
		}		
	else
		INFO_PRINTF2(_L("number of loaded CSY's: %d"), csys);

	return TestStepResult();
	}

CLsc32LoadUnloadCsy::CLsc32LoadUnloadCsy()
	{
	SetTestStepName(_L("Lsc32LoadUnloadCsy"));
	}

TVerdict CLsc32LoadUnloadCsy::doTestStepL()
/**
 * @test Test that load and unload comms module
 */
	{
	INFO_PRINTF1(_L("==================================="));
	INFO_PRINTF1(_L("Test : Lsc32LoadUnloadCsy"));
	INFO_PRINTF1(_L("==================================="));
	
	INFO_PRINTF1(_L("Loading Dummy CSY..."));
	
	TInt ret = iCommSession.LoadCommModule(KCsyName);
	TEST(ret == KErrNone);
	if(ret != KErrNone)
		{
		INFO_PRINTF3(_L("ERROR: could not load CSY '%S' due to %d"), &KCsyName, ret);
		SetTestStepResult(EFail);
		}
	else
		INFO_PRINTF2(_L("OK. CSY '%S' loaded"), &KCsyName);

	INFO_PRINTF1(_L("Unloading Dummy CSY..."));		

	ret = iCommSession.UnloadCommModule(KCsyName);
	TEST(ret == KErrNone);
	if(ret != KErrNone)
		{
		INFO_PRINTF3(_L("ERROR: could not unload CSY '%S' due to %d"), &KCsyName, ret);
		SetTestStepResult(EFail);
		}
	else
		INFO_PRINTF2(_L("OK. CSY '%S' unloaded"), &KCsyName);

	INFO_PRINTF1(_L("Unloading Dummy CSY again should give error..."));		

	ret = iCommSession.UnloadCommModule(KCsyName);
	TEST(ret != KErrNone);
	if(ret != KErrNone)
		{
		INFO_PRINTF3(_L("TEST PASSED: could not unload CSY '%S' due to %d"), &KCsyName, ret);
		}
	else
		{
		INFO_PRINTF2(_L("ERROR: CSY '%S' unloaded twice!"), &KCsyName);
		SetTestStepResult(EFail);
		}
	return TestStepResult();
	}

CLsc32GetPortInfo::CLsc32GetPortInfo()
	{
	SetTestStepName(_L("Lsc32GetPortInfo"));
	}

TVerdict CLsc32GetPortInfo::doTestStepL()
/**
 * @test Test that gets port info for all comms modules loaded
 */
	{
	INFO_PRINTF1(_L("==================================="));
	INFO_PRINTF1(_L("Test : Lsc32GetPortInfo"));
	INFO_PRINTF1(_L("==================================="));
	
	TInt csys;
	TInt ret = iCommSession.NumPorts(csys);
	TEST(ret == KErrNone);
	if(ret != KErrNone)
		{
		INFO_PRINTF2(_L("ERROR: could not get number of loaded CSY's due to %d"), ret);
		SetTestStepResult(EFail);
		}

	for(TInt i=0;i<csys;i++)
		{
		TBuf<100> moduleName;
		TSerialInfo serialInfo;
		TInt ret = iCommSession.GetPortInfo(i, moduleName, serialInfo);
		if(ret!=KErrNone)
			INFO_PRINTF3(_L("index %d: GetPortInfo returned %d"), i, ret);
		else
			{
			INFO_PRINTF3(_L("index %d: '%S'"), i, &moduleName);
			INFO_PRINTF2(_L("  description: %S"), &serialInfo.iDescription);
			INFO_PRINTF2(_L("  port name:   %S"), &serialInfo.iName);
			INFO_PRINTF2(_L("  low unit:    %d"), serialInfo.iLowUnit);
			INFO_PRINTF2(_L("  high unit:   %d"), serialInfo.iHighUnit);
			}
		}
		
	return TestStepResult();
	}	

CDummyTest::CDummyTest()
	{
	SetTestStepName(_L("DummyTest"));
	}	

TVerdict CDummyTest::doTestStepL()
/**
 * @test Calls every C32 API methods, will panic if errors occurs on received
 *       parameters or  return values
 */
	{
	INFO_PRINTF1(_L("==================================="));
	INFO_PRINTF1(_L("Test : Dummy Tests"));
	INFO_PRINTF1(_L("==================================="));
	TBool theBool = EFalse;
	GetBoolFromConfig(ConfigSection(),_L("TheBool"),theBool);
	INFO_PRINTF1(_L("Testing DCE role"));
	DummyClientTestsL(ECommRoleDCE, theBool);
	INFO_PRINTF1(_L("Testing DTE role"));
	DummyClientTestsL(ECommRoleDTE, theBool);
	INFO_PRINTF1(_L("Finished OK"));

	return TestStepResult();
	}
	
void CDummyTest::DummyClientTestsL(TCommRole aRole, TBool aHeapCheck)
/**
 * @test Calls every C32 API methods, will panic if errors occurs on received
 *       parameters o  return values
 */
	{
	TRequestStatus stat,stat2,stat3,stat4,stat5,stat6,stat7,stat8,stat9;
	TUint inputs;
	TUint sig;
	
	INFO_PRINTF1(_L("Opening dummy"));	
	TInt res = iCommSession.Connect();
	TESTCHECKL(res, KErrNone);

	if(aHeapCheck == EFalse)
	{
		

// PRE-STRETCH START - Load dummy csy and open a CPort

	res = iCommSession.LoadCommModule(_L("dummy"));
	TESTCHECKL(res, KErrNone);

	TEST(iDPort0.Open(iCommSession,KDummyPortName,ECommShared,aRole)==KErrNone);
	iDPort0.Close();
	User::After(2000000);  // Give dummy time to do the deferred close
	TEST(iCommSession.UnloadCommModule(_L("dummy"))==KErrNone);

// PRE-STRETCH END	
	}
	iCommSession.__DbgMarkHeap();
	res = iCommSession.LoadCommModule(_L("dummy"));
	TESTCHECKL(res, KErrNone);

	// new tests
    WaitingTestsL(aRole);
	DummyPreemptableTestsL(aRole);

	if (aRole==ECommRoleDTE)
		INFO_PRINTF1(_L("Open dummy serial port in shared mode as DTE"));
	else
		INFO_PRINTF1(_L("Open dummy serial port in shared mode as DCE"));

	TEST(iDPort0.Open(iCommSession,KDummyPortName,ECommShared,aRole)==KErrNone);

	HBufC8* hbuf=HBufC8::NewL(2046);
	TPtr8 outbuf=hbuf->Des();
	outbuf	=_L8("This string has to be exactly like this othervice write returns KErrCommsLineFail.");
	
	if (aRole==ECommRoleDTE)
		inputs = KSignalDTEInputs;
	else
		inputs = KSignalDCEInputs;


	// testing write & signals
	INFO_PRINTF1(_L("*********************************"));
	INFO_PRINTF1(_L("testing write & signals"));

	iDPort0.Write(stat,5000000L,outbuf);
	INFO_PRINTF1(_L("About to notifysignalchange"));
	iDPort0.NotifySignalChange(stat2,sig,inputs);
	User::WaitForRequest(stat);
	INFO_PRINTF2(_L("Write completed with %d"),stat.Int());
	TEST(stat==KErrNone);
	User::WaitForRequest(stat2); 
	INFO_PRINTF2(_L("Notify completed with %d"),stat2.Int());
	INFO_PRINTF2(_L("Signals = %x"),sig);
	TEST(stat2==KErrNone);
	if (aRole==ECommRoleDTE)
		TEST(sig==(KSignalDTEInputs|KCTSChanged));
	else
		TEST(sig==(KSignalDCEInputs|KRTSChanged));

	//ShowSignals(sig,aRole);
	INFO_PRINTF1(_L("SetSignalsToSpace"));
	TInt ret;
	if (aRole==ECommRoleDTE)
		{
		ret = iDPort0.SetSignalsToSpace(KSignalDTEOutputs);
		}
	else
		{
		ret = iDPort0.SetSignalsToSpace(KSignalDCEOutputs);
		}
	TESTCHECKL(ret, KErrNone);

	INFO_PRINTF1(_L("SetSignalsToMark"));
	if (aRole==ECommRoleDTE)
		{
		ret = iDPort0.SetSignalsToMark(KSignalDTEOutputs);
		}
	else
		{
		ret = iDPort0.SetSignalsToMark(KSignalDCEOutputs);
		}
	TESTCHECKL(ret, KErrNone);
	User::After(2000000);
					
	INFO_PRINTF1(_L("NotifySignalChange"));
	iDPort0.NotifySignalChange(stat2,sig,inputs);
	User::WaitForRequest(stat2); 
	//ShowSignals(sig,aRole);
	if (aRole==ECommRoleDTE)
		{
		TEST(sig==(KSignalDTEInputs|KCTSChanged));
		}
	else
		{
		TEST(sig==(KSignalDCEInputs|KRTSChanged));
		}
		
	sig=iDPort0.Signals(KSignalDTEInputs|KSignalDCEInputs);
	//ShowSignals(sig,aRole);
	if (aRole==ECommRoleDTE)
		{
		TEST(sig==(KSignalDTEInputs));
		}
	else
		{
		TEST(sig==(KSignalDCEInputs));
		}

	// canceling
	stat2=KErrNone;
	INFO_PRINTF1(_L("Canceling signal notification"));
	iDPort0.NotifySignalChange(stat2,sig,inputs);
	ret = iDPort0.NotifySignalChangeCancel();
	TESTCHECKL(ret, KErrNone);
	User::WaitForRequest(stat2); 
	TEST(stat2==KErrCancel);
	stat2=KErrNone;
	iDPort0.NotifySignalChange(stat2,sig,inputs);
	ret = iDPort0.NotifySignalChangeCancel();
	TESTCHECKL(ret, KErrNone);
	User::WaitForRequest(stat2); 
	TEST(stat2==KErrCancel);


	// testing flow control & read
	INFO_PRINTF1(_L("*********************************"));
	INFO_PRINTF1(_L("testing flow control & read"));
	TFlowControl flow,flow2;
	iDPort0.NotifyFlowControlChange(stat2,flow);
	User::WaitForRequest(stat2); 
	iDPort0.GetFlowControlStatus(flow2);
	TEST(flow2==flow);
	TEST(flow==EFlowControlOn);
	//ShowFlow(flow);
	iDPort0.GetFlowControlStatus(flow2);
	TEST(flow2==flow);

	HBufC8* hbuf2=HBufC8::NewL(2048);
	TPtr8 inbuf=hbuf2->Des();
	inbuf.SetLength(0);
	INFO_PRINTF1(_L("NotifyFlowControlChange & read"));
	iDPort0.NotifyFlowControlChange(stat2,flow);
	iDPort0.ReadOneOrMore(stat,inbuf);
	User::WaitForRequest(stat); 
	User::WaitForRequest(stat2);
	TEST(inbuf.Length()==2048);
	//ShowFlow(flow);
	TEST(flow==EFlowControlOn);
	//INFO_PRINTF1(_L("Received>>>"));
	//ShowReceived(inbuf);
	iDPort0.GetFlowControlStatus(flow2);
	TEST(flow2==flow);

	// testing canceling
	stat2=KErrNone;
	INFO_PRINTF1(_L("NotifyFlowControlChange canceling"));
	iDPort0.NotifyFlowControlChange(stat2,flow);
	ret = iDPort0.NotifyFlowControlChangeCancel();
	TESTCHECKL(ret, KErrNone);
	User::WaitForRequest(stat2);
	TEST(stat2==KErrCancel);
	stat2=KErrNone;
	iDPort0.NotifyFlowControlChange(stat2,flow);
	ret = iDPort0.NotifyFlowControlChangeCancel();
	TESTCHECKL(ret, KErrNone);
	User::WaitForRequest(stat2);
	TEST(stat2==KErrCancel);


	// testing break
	INFO_PRINTF1(_L("*********************************"));
	INFO_PRINTF1(_L("testing break"));
	iDPort0.Break(stat,1);
	User::WaitForRequest(stat);

	INFO_PRINTF1(_L("NotifyBreak"));
	iDPort0.NotifyBreak(stat);
	User::WaitForRequest(stat);

	INFO_PRINTF1(_L("Sending break"));
	iDPort0.Break(stat,100);
	User::WaitForRequest(stat);
	
	INFO_PRINTF1(_L("NotifyBreak"));
	iDPort0.NotifyBreak(stat);
	User::WaitForRequest(stat);

	INFO_PRINTF1(_L("Sending break"));
	iDPort0.Break(stat,10000);
	User::WaitForRequest(stat);

	INFO_PRINTF1(_L("NotifyBreak"));
	iDPort0.NotifyBreak(stat);
	User::WaitForRequest(stat);

	// Testing canceling
	INFO_PRINTF1(_L("NotifyBreak canceling"));
	stat2=KErrNone;
	iDPort0.NotifyBreak(stat2);
	ret = iDPort0.NotifyBreakCancel();
	TESTCHECKL(ret, KErrNone);
	User::WaitForRequest(stat2); 
	TEST(stat2==KErrCancel);
	stat2=KErrNone;
	iDPort0.NotifyBreak(stat2);
	ret = iDPort0.NotifyBreakCancel();
	TESTCHECKL(ret, KErrNone);
	User::WaitForRequest(stat2); 
	TEST(stat2==KErrCancel);

	INFO_PRINTF1(_L("Break canceling"));
	stat2=KErrNone;
	iDPort0.Break(stat2,1000000);
	ret = iDPort0.BreakCancel();
	TESTCHECKL(ret, KErrNone);
	User::WaitForRequest(stat2);
	TEST(stat2==KErrCancel);
	stat2=KErrNone;
	iDPort0.Break(stat2,100000000);
	ret = iDPort0.BreakCancel();
	TESTCHECKL(ret, KErrNone);
	User::WaitForRequest(stat2);
	TEST(stat2==KErrCancel);


	// testing configuration changes & settings
	INFO_PRINTF1(_L("*********************************"));
	INFO_PRINTF1(_L("testing configuration changes & settings"));
	TCommNotificationV01 config;
	TPckg<TCommNotificationV01> cfg(config);

	// get config
	TCommConfig ownConfig;
	ret=iDPort0.Config(ownConfig);
	TESTCHECKL(ret, KErrNone);
	TEST(ownConfig().iRate==EBps9600);
	TEST(ownConfig().iDataBits==EData8);
	TEST(ownConfig().iParity==EParityNone);
	TEST(ownConfig().iStopBits==EStop1);
	if (aRole==ECommRoleDTE)
		TEST(ownConfig().iHandshake==KConfigObeyCTS);
	else
		TEST(ownConfig().iHandshake==KConfigObeyRTS);

	// set config
	//
	// If you receive a KErrGeneral (-1) error on this test, it is most likely
	// because the TThread.dll library is missing from your build.  An annoying
	// error to debug!!!
	//
	ownConfig().iStopBits=EStop2;
	TInt myRet = iDPort0.SetConfig(ownConfig);
	TEST(myRet==KErrNotSupported);
	
	
	// notify config
	iDPort0.NotifyConfigChange(stat2,cfg);
	User::WaitForRequest(stat2); 
	//ShowConfig(config);
	TEST(config.iChangedMembers=KHandshakeChanged);
	TEST(config.iRate==EBps9600);
	TEST(config.iDataBits==EData8);
	TEST(config.iParity==EParityNone);
	TEST(config.iStopBits==EStop1);
	TEST(config.iHandshake==(KConfigObeyCTS|KConfigObeyRTS));

	// testing canceling
	stat2=KErrNone;
	iDPort0.NotifyConfigChange(stat2,cfg);
	ret = iDPort0.NotifyConfigChangeCancel();
	TESTCHECKL(ret, KErrNone);
	User::WaitForRequest(stat2); // 
	TEST(stat2==KErrCancel);
	stat2=KErrNone;
	iDPort0.NotifyConfigChange(stat2,cfg);
	ret = iDPort0.NotifyConfigChangeCancel();
	TESTCHECKL(ret, KErrNone);
	User::WaitForRequest(stat2); // 
	TEST(stat2==KErrCancel);


	// testing capabilities querying
	INFO_PRINTF1(_L("*********************************"));
	INFO_PRINTF1(_L("testing capabilities querying"));
	TCommCapsV01 capsV01;
	TCommCapsV02 capsV02;
	TPckg<TCommCapsV01> cV01(capsV01);
	TPckg<TCommCapsV02> cV02(capsV02);
	INFO_PRINTF1(_L("Querying   TCommCapsV01"));
	INFO_PRINTF1(_L("-----------------------"));

	// TCommCapsV01
	ret = iDPort0.Caps(cV01);
	TESTCHECKL(ret, KErrNone);
	//ShowCapsV01(capsV01);
	TEST(capsV01.iRate==
		( KCapsBps50 | KCapsBps75 | KCapsBps110 | KCapsBps134 | KCapsBps150 | KCapsBps300
		| KCapsBps600 | KCapsBps1200 | KCapsBps1800 | KCapsBps2000 | KCapsBps2400 | KCapsBps3600
		| KCapsBps4800 | KCapsBps7200 | KCapsBps9600 | KCapsBps19200 | KCapsBps38400
		| KCapsBps57600 | KCapsBps115200));
	TEST(capsV01.iDataBits	==0xf);
	TEST(capsV01.iStopBits	==0x3);
	TEST(capsV01.iParity	==0x7);
	TEST(capsV01.iHandshake	==0x1ff);
	TEST(capsV01.iSignals	==0x3f);
	TEST(capsV01.iFifo		==KCapsHasFifo);
	TEST(capsV01.iSIR		==0);
	
	// TCommCapsV02
	INFO_PRINTF1(_L("Querying   TCommCapsV02"));
	ret = iDPort0.Caps(cV02);
	TESTCHECKL(ret, KErrNone);
	//ShowCapsV02(capsV02);
	TEST(capsV02.iRate==
		( KCapsBps50 | KCapsBps75 | KCapsBps110 | KCapsBps134 | KCapsBps150 | KCapsBps300
		| KCapsBps600 | KCapsBps1200 | KCapsBps1800 | KCapsBps2000 | KCapsBps2400 | KCapsBps3600
		| KCapsBps4800 | KCapsBps7200 | KCapsBps9600 | KCapsBps19200 | KCapsBps38400
		| KCapsBps57600 | KCapsBps115200));
	TEST(capsV02.iDataBits	==0xf);
	TEST(capsV02.iStopBits	==0x3);
	TEST(capsV02.iParity	==0x7);
	TEST(capsV02.iHandshake	==0x1ff);
	TEST(capsV02.iSignals	==0x3f);
	TEST(capsV02.iFifo		==KCapsHasFifo);
	TEST(capsV02.iSIR		==0);
	TEST(capsV02.iNotificationCaps==
		     (KNotifySignalsChangeSupported		| KNotifyRateChangeSupported 
			| KNotifyDataFormatChangeSupported	| KNotifyHandshakeChangeSupported
			| KNotifyBreakSupported				| KNotifyFlowControlChangeSupported));
	TEST(capsV02.iRoleCaps==KCapsRoleSwitchSupported);
	TEST(capsV02.iFlowControlCaps==KCapsFlowControlStatusSupported);


	iDPort0.Close();
	User::After(2000000);  // Give dummy time to do the deferred close
	TEST(iCommSession.UnloadCommModule(_L("dummy"))==KErrNone);
	User::After(500000);
	if(aHeapCheck == EFalse)
		{
		iCommSession.__DbgMarkEnd(0);	
		}
	else
		{
		iCommSession.__DbgMarkEnd(5);	
		}
	iCommSession.Close();
	
	User::After(2000000);	

	// test role conflicts
	INFO_PRINTF1(_L("*********************************"));
	INFO_PRINTF1(_L("test role conflicts"));
	res = iCommSession.Connect();
	TESTCHECKL(res, KErrNone);

	if(aHeapCheck == EFalse)
		{
		// PRE-STRETCH START - Load a CSY and open a CPort (new session)
		TEST(iCommSession.LoadCommModule(_L("dummy"))==KErrNone);
		TEST(iDPort0.Open(iCommSession,_L("dummy::0"),ECommShared,ECommRoleDCE)==KErrNone);
		iDPort0.Close();
		User::After( 1200000L * 2 );  // Give it long enough to do the deferred delete of dummy.
		TEST(iCommSession.UnloadCommModule(_L("dummy"))==KErrNone);
		// PRE-STRETCH END
		}
	iCommSession.__DbgMarkHeap();
	INFO_PRINTF1(_L("Loading dummy module"));
	TEST(iCommSession.LoadCommModule(_L("dummy"))==KErrNone);
	
	INFO_PRINTF1(_L("Open dummy serial port in shared mode as DCE"));
	TEST(iDPort0.Open(iCommSession,_L("dummy::0"),ECommShared,ECommRoleDCE)==KErrNone);
	INFO_PRINTF1(_L("Open dummy serial port in shared mode as DCE"));
	TEST(iDPort1.Open(iCommSession,_L("dummy::0"),ECommShared,ECommRoleDCE)==KErrNone);
	INFO_PRINTF1(_L("Open dummy serial port in shared mode as DTE"));
	TEST(iDPort2.Open(iCommSession,_L("dummy::0"),ECommShared,ECommRoleDTE)==KErrLocked);
	iDPort1.Close();
	iDPort2.Close();


	// test rest of reads of the API
	INFO_PRINTF1(_L("*********************************"));
	INFO_PRINTF1(_L("test rest of reads of the API"));
	iDPort0.Read(stat,inbuf);
	User::WaitForRequest(stat); 
	TEST(stat==KErrNone);
	//INFO_PRINTF1(_L("Received>>>"));
	//ShowReceived(inbuf);
	TEST(inbuf.Length()==2048);	

	iDPort0.Read(stat,inbuf,257);
	User::WaitForRequest(stat); 
	TEST(stat==KErrNone);
	//INFO_PRINTF1(_L("Received>>>"));
	//ShowReceived(inbuf);
	TEST(inbuf.Length()==257);	

	TTimeIntervalMicroSeconds32 time = 10;

	iDPort0.Read(stat,time,inbuf);
	User::WaitForRequest(stat); 
	TEST(stat==KErrTimedOut);
	//INFO_PRINTF1(_L("Received>>>"));
	//ShowReceived(inbuf);
	TEST(inbuf.Length()==2048);	

	time = 10000000;
	iDPort0.Read(stat,time,inbuf,2048);
	User::WaitForRequest(stat); 
	TEST(stat==KErrNone);
	//INFO_PRINTF1(_L("Received>>>"));
	//ShowReceived(inbuf);
	TEST(inbuf.Length()==2048);	

	// test rest of writes of the API
	INFO_PRINTF1(_L("*********************************"));
	INFO_PRINTF1(_L("test rest of writes of the API"));
	iDPort0.Write(stat,outbuf);
	User::WaitForRequest(stat);
	TEST(stat==KErrNone);

	iDPort0.Write(stat,outbuf,82);
	User::WaitForRequest(stat);
	TEST(stat==KErrNone);

	iDPort0.Write(stat,5000000L,outbuf,50);
	User::WaitForRequest(stat);
	TEST(stat==KErrCommsLineFail);
	

	// test SetMode & Mode
	INFO_PRINTF1(_L("*********************************"));
	INFO_PRINTF1(_L("test SetMode & Mode"));
	TCommServerConfig bmcPckg;
	TCommServerConfigV01 &bmc=bmcPckg();
	//bmc.iBufFlags=KCommBufferPartial;
	bmc.iBufFlags=KCommBufferPartial;
	bmc.iBufSize=0x10;
	ret = iDPort0.SetMode(bmcPckg);
	TEST(ret==KErrNone);
	bmc.iBufFlags=KCommBufferFull;
	bmc.iBufSize=0;
	
	ret = iDPort0.Mode(bmcPckg);
	TEST(ret==KErrNone);
	TEST(bmc.iBufFlags==KCommBufferPartial);
	TEST(bmc.iBufSize==0x10);


	// test NotifyDataAvailable
	INFO_PRINTF1(_L("*********************************"));
	INFO_PRINTF1(_L("test NotifyDataAvailable"));
	iDPort0.NotifyDataAvailable(stat);
	User::WaitForRequest(stat);
	TEST(stat==KErrNone);
	// testing canceling
	stat2=KErrNone;
	iDPort0.NotifyDataAvailable(stat2);
	ret = iDPort0.NotifyDataAvailableCancel();
	TESTCHECKL(ret, KErrNone);
	User::WaitForRequest(stat2); // 
	TEST(stat2==KErrCancel);
	stat2=KErrNone;
	iDPort0.NotifyDataAvailable(stat2);
	ret = iDPort0.NotifyDataAvailableCancel();
	TESTCHECKL(ret, KErrNone);
	User::WaitForRequest(stat2); // 
	TEST(stat2==KErrCancel);


	// test NotifyOutputEmpty
	INFO_PRINTF1(_L("*********************************"));
	INFO_PRINTF1(_L("test NotifyOutputEmpty"));
	iDPort0.NotifyOutputEmpty(stat);
	User::WaitForRequest(stat);
	TEST(stat==KErrNone);
	// testing canceling
	stat2=KErrNone;
	iDPort0.NotifyOutputEmpty(stat2);
	ret = iDPort0.NotifyOutputEmptyCancel();
	TESTCHECKL(ret, KErrNone);
	User::WaitForRequest(stat2); // 
	TEST(stat2==KErrCancel);
	stat2=KErrNone;
	iDPort0.NotifyOutputEmpty(stat2);
	ret = iDPort0.NotifyOutputEmptyCancel();
	TESTCHECKL(ret, KErrNone);
	User::WaitForRequest(stat2); // 
	TEST(stat2==KErrCancel);


	// test buffer lengths & ResetBuffers
	INFO_PRINTF1(_L("*********************************"));
	INFO_PRINTF1(_L("test test buffer lengths & ResetBuffers"));
	TInt bufferLength=2000;
	TInt bufferLength2=0;

	ret = iDPort0.SetReceiveBufferLength(bufferLength);
	TESTCHECKL(ret, KErrNone);	
	bufferLength2=iDPort0.ReceiveBufferLength();
	TEST(bufferLength2==bufferLength);
	ret = iDPort0.ResetBuffers();
	TESTCHECKL(ret, KErrNone);
	TEST(iDPort0.QueryReceiveBuffer()==0);
	iDPort0.Close();


	// test GetRole
	INFO_PRINTF1(_L("*********************************"));
	INFO_PRINTF1(_L("test GetRole"));
	TEST(iDPort0.Open(iCommSession,_L("dummy::0"),ECommExclusive,aRole)==KErrNone);
	TCommRole role;
	if (aRole==ECommRoleDTE)
		role=ECommRoleDCE;
	else
		role=ECommRoleDTE;
	ret = iDPort0.GetRole(role);
	TEST(ret==KErrNone);
	TEST(role==aRole);


	// test cancel
	INFO_PRINTF1(_L("*********************************"));
	INFO_PRINTF1(_L("test cancel"));
	iDPort0.Break(stat3,1);
	User::WaitForRequest(stat3);
	iDPort0.Break(stat3,100);
	User::WaitForRequest(stat3);
	iDPort0.Break(stat3,10000);
	User::WaitForRequest(stat3);
	
	iDPort0.Write(stat,outbuf);
	iDPort0.Break(stat2,1000000);
	iDPort0.Read(stat3,inbuf);
	iDPort0.NotifySignalChange(stat4,sig,inputs);
	iDPort0.NotifyConfigChange(stat5,cfg);
	iDPort0.NotifyFlowControlChange(stat6,flow);
	iDPort0.NotifyBreak(stat7);
	iDPort0.NotifyDataAvailable(stat8);
	iDPort0.NotifyOutputEmpty(stat9);

	ret = iDPort0.Cancel();
	TESTCHECKL(ret, KErrNone);
	
	User::WaitForRequest(stat);
	User::WaitForRequest(stat2);
	User::WaitForRequest(stat3);
	User::WaitForRequest(stat4);
	User::WaitForRequest(stat5);
	User::WaitForRequest(stat6);
	User::WaitForRequest(stat7);
	
	User::WaitForRequest(stat8);
	User::WaitForRequest(stat9);
	TEST(stat==KErrCancel);
	TEST(stat2==KErrCancel);
	TEST(stat3==KErrCancel);
	TEST(stat4==KErrCancel);
	TEST(stat5==KErrCancel);
	
	TEST(stat6==KErrCancel);
	TEST(stat7==KErrCancel);
	TEST(stat8==KErrCancel);
	TEST(stat9==KErrCancel);

	INFO_PRINTF1(_L("---------------------------------"));
	iDPort0.Close();
	User::After( 1200000L * 2 );  // Give it long enough to do the deferred delete of dummy.
	TEST(iCommSession.UnloadCommModule(_L("dummy"))==KErrNone);
	User::After( 500000 ); 
	if(aHeapCheck == EFalse)
		{
		iCommSession.__DbgMarkEnd(0);		
		}
	else
		{
		iCommSession.__DbgMarkEnd(5);
		}
	iCommSession.Close();

	delete hbuf;
	delete hbuf2;
	}	

void CDummyTest::WaitingTestsL(TCommRole aRole)
/**
 * @test Test the new waiting functionality
 */ 
	{
	RCommServ csB;
	RCommServ csC;
	RComm A;
	RComm B;
	RComm C;

	TEST(csB.Connect()==KErrNone);
	TEST(csC.Connect()==KErrNone);

	TRequestStatus stat;

	// *************************************************************

	INFO_PRINTF1(_L("Testing transition from waiting to preemptable"));

	TRequestStatus statusA;
	TRequestStatus statusB;
	A.OpenWhenAvailable(statusA,iCommSession,KDummyPortName);
	User::WaitForRequest(statusA);
	TEST(statusA==KErrNone);

	B.OpenWhenAvailable(statusB,csB,KDummyPortName);
	User::WaitForRequest(statusB);
	TEST(statusB==KErrNotSupported);

#ifdef _DEBUG
	TCommDebugInfo s;
	TInt ret = A.DebugState(s);
	TESTCHECKL(ret, KErrNone);
	TEST(s.iAccessCount == 1 );
	TEST(s.iMode == EIntCommPreemptable);
	TEST(s.iRole == KCommDefaultPreemptRole ); // reflects change in cc_cli.cpp
	TEST(s.iOutstandingCommands == 0);
#endif

	TEST(A.SetAccessMode(ECommExclusive)==KErrNone);
#ifdef _DEBUG
	ret = A.DebugState(s);
	TESTCHECKL(ret, KErrNone);
	TEST(s.iAccessCount == 1 );
	TEST(s.iMode == EIntCommExclusive);
	TEST(s.iRole == KCommDefaultPreemptRole ); // reflects change in cc_cli.cpp
	TEST(s.iOutstandingCommands == 0);
#endif

	B.OpenWhenAvailable(statusB,csB,KDummyPortName);
	TEST(statusB==KRequestPending);

	B.OpenWhenAvailableCancel();
	TEST(statusB==KErrCancel);
	User::WaitForRequest(statusB);

#ifdef _DEBUG
	ret = A.DebugState(s);
	TESTCHECKL(ret, KErrNone);
	TEST(s.iAccessCount == 1 );
	TEST(s.iMode == EIntCommExclusive);
	TEST(s.iRole == KCommDefaultPreemptRole ); // reflects change in cc_cli.cpp
	TEST(s.iOutstandingCommands == 0);
#endif

	B.OpenWhenAvailable(statusB,csB,KDummyPortName);
	TEST(statusB==KRequestPending);

	B.Write(stat,5000000L,testbufWrite);
	User::WaitForRequest(stat);
	TEST(stat==KErrNotReady);

	TEST(B.SetAccessMode(ECommExclusive)==KErrNotSupported);
	TEST(B.SetAccessMode(ECommShared)==KErrNotSupported);

	A.Close();
	TEST(statusB==KErrNone);
	User::WaitForRequest(statusB);

#ifdef _DEBUG
	ret = B.DebugState(s);
	TEST(ret==KErrNone);
	TEST(s.iAccessCount == 1 );
	TEST(s.iMode == EIntCommPreemptable);
	TEST(s.iRole == KCommDefaultPreemptRole ); // reflects change in cc_cli.cpp
	TEST(s.iOutstandingCommands == 0);
#endif

	B.Write(stat,5000000L,testbufWrite);
	User::WaitForRequest(stat);
	TEST(stat==KErrNone);

	B.Close();

	TEST(B.Open(csB,KDummyPortName,ECommShared,ECommRoleDCE)==KErrNone);
	A.OpenWhenAvailable(statusA,iCommSession,KDummyPortName);
	TEST(statusA==KRequestPending);

	B.Close();

	User::WaitForRequest(statusA);
	TEST(statusA==KErrNone);

#ifdef _DEBUG
	ret = A.DebugState(s);
	TESTCHECKL(ret, KErrNone);
	TEST(s.iAccessCount == 1 );
	TEST(s.iMode == EIntCommPreemptable);
	TEST(s.iRole == ECommRoleDCE );
	TEST(s.iOutstandingCommands == 0);
#endif

	A.Close();

	// *************************************************************

	INFO_PRINTF1(_L("Testing Asynchronous open"));

	TEST(B.Open(csB,KDummyPortName,ECommExclusive,aRole)==KErrNone);
	B.Write(stat,5000000L,testbufWrite);
	User::WaitForRequest(stat);
	TEST(stat==KErrNone);
	TRequestStatus statusC;

	A.OpenWhenAvailable(statusC,iCommSession,KDummyPortName);

	A.Write(stat,5000000L,testbufWrite);
	User::WaitForRequest(stat);
	TEST(stat==KErrNotReady);

	TEST(C.Open(csC,KDummyPortName,ECommExclusive,aRole)==KErrAccessDenied);
	TEST(C.Open(csC,KDummyPortName,ECommPreemptable,aRole)==KErrAccessDenied);
	C.OpenWhenAvailable(stat,csC,KDummyPortName);
	User::WaitForRequest(stat);
	TEST(stat==KErrAccessDenied);

	TInt ret1=B.WriteCancel(); 
	TESTCHECKL(ret1, KErrNone);
	TEST(statusC==KRequestPending);
	B.Close();
	TEST(statusC==KErrNone);  // OpenWhenAvailable completed.
	User::WaitForRequest(statusC);

	A.Write(stat,5000000L,testbufWrite);
	User::WaitForRequest(stat);
	TEST(stat==KErrNone);

	TEST(A.SetAccessMode(ECommExclusive)==KErrNone);

	B.OpenWhenAvailable(statusC,csB,KDummyPortName);
	TEST(statusC==KRequestPending);

	B.Write(stat,5000000L,testbufWrite);
	User::WaitForRequest(stat);
	TEST(stat==KErrNotReady);

	A.Close();

	TEST(statusC==KErrNone);
	User::WaitForRequest(statusC);

	B.Write(stat,5000000L,testbufWrite);
	User::WaitForRequest(stat);
	TEST(stat==KErrNone);

#ifdef _DEBUG
	ret = B.DebugState(s);
	TESTCHECKL(ret, KErrNone);
	TEST(s.iAccessCount == 1 );
	TEST(s.iMode == EIntCommPreemptable);
	TEST(s.iRole == aRole );
	TEST(s.iOutstandingCommands == 0);
#endif

	B.Close();

	// *************************************************************

	INFO_PRINTF1(_L("Testing Timing issues, Cancel and Sharing with Async open"));

	TRequestStatus statusE;
	A.OpenWhenAvailable(statusE,csC,KDummyPortName);

	TEST(B.Open(csC,KDummyPortName,ECommPreemptable,aRole)==KErrAccessDenied);
	TEST(statusE==KErrNone);

	User::WaitForRequest(statusE);

#ifdef _DEBUG
	ret = A.DebugState(s);
	TESTCHECKL(ret, KErrNone);
	TEST(s.iAccessCount == 1 );
	TEST(s.iMode == EIntCommPreemptable);
	TEST(s.iOutstandingCommands == 0);
#endif

	A.Close();
	
	csB.Close();
	csC.Close();	

	INFO_PRINTF1(_L("*** Waiting tests completed *** "));
	}
	
void CDummyTest::DummyPreemptableTestsL(TCommRole aRole)
/**
 * @test Test the new preemption functionality
 */ 
	{
	RCommServ csB;
	RComm A;
	RComm B;

	TEST(csB.Connect()==KErrNone);

	TRequestStatus stat,stat2;

	// *************************************************************

	INFO_PRINTF1(_L("Testing preemptable mode "));
	TInt erra = A.Open(iCommSession,KDummyPortName,ECommPreemptable,aRole);
	TEST(erra==KErrNone);

	HBufC8* hbuf2=HBufC8::NewL(2048);
	TPtr8 inbuf=hbuf2->Des();
	inbuf.SetLength(0);

	INFO_PRINTF1(_L("Doing read then write"));
	A.Read(stat,inbuf);
	User::WaitForRequest(stat); 
	TEST(stat==KErrNone);
	TEST(inbuf.Length() == 2048 && inbuf.Left( testbufRead().Length() ) ==testbufRead);
	A.Write(stat,5000000L,testbufWrite);
	User::WaitForRequest(stat);
	TEST(stat==KErrNone);

	// *************************************************************

	INFO_PRINTF1(_L("Testing simple preemption"));

	TEST(B.Open(csB,KDummyPortName,ECommExclusive,aRole)==KErrNone);
	A.Read(stat,inbuf);
	User::WaitForRequest(stat); 
	TEST(stat==KErrCancel);
	A.Write(stat,5000000L,testbufWrite);
	User::WaitForRequest(stat);
	TEST(stat==KErrCancel);
#ifdef _DEBUG
	TCommDebugInfo s;
	TInt ret = B.DebugState(s);
	TEST(ret==KErrNone);
	TEST(s.iAccessCount == 2 );
	TEST(s.iMode == EIntCommExclusive);
	TEST(s.iRole == aRole );
	TEST(s.iOutstandingCommands == 0);
#endif
	
	B.Read(stat,inbuf);
	User::WaitForRequest(stat); 
	TEST(stat==KErrNone);
	TEST(inbuf.Length() == 2048 && inbuf.Left( testbufRead().Length() ) ==testbufRead);
	B.Write(stat,5000000L,testbufWrite);
	User::WaitForRequest(stat);
	TEST(stat==KErrNone);

	A.Close();

	TEST(A.Open(iCommSession,KDummyPortName,ECommPreemptable,aRole)==KErrAccessDenied);
	TInt err = A.Open(iCommSession,KDummyPortName,ECommExclusive,aRole);
	TEST(err==KErrAccessDenied);
	B.Close();

	// *************************************************************

	INFO_PRINTF1(_L("Testing preemption with outstanding requests and role change"));

	TCommRole altRole = (aRole==ECommRoleDTE) ? ECommRoleDCE:ECommRoleDTE;
	TEST(A.Open(iCommSession,KDummyPortName,ECommPreemptable,altRole)==KErrNone);
#ifdef _DEBUG
	ret = A.DebugState(s);
	TESTCHECKL(ret, KErrNone);
	TEST(s.iRole == altRole );
	TEST(s.iOutstandingCommands == 0);
#endif
	A.Read(stat,inbuf);
	A.Write(stat2,5000000L,testbufWrite);
	TEST(B.Open(csB,KDummyPortName,ECommShared,aRole)==KErrNone);
	TEST(stat==KErrCancel);  // You've been preempted error!
	TEST(stat2==KErrCancel);


#ifdef _DEBUG
	ret = B.DebugState(s);
	TESTCHECKL(ret, KErrNone);
	TEST(s.iAccessCount == 2 );
	TEST(s.iMode == EIntCommShared);
	TEST(s.iRole == aRole );
	TEST(s.iOutstandingCommands == 0);
#endif
	B.Write(stat,5000000L,testbufWrite);
	B.Read(stat2,inbuf);
	User::WaitForRequest(stat2);
	TEST(stat2==KErrNone);
	User::WaitForRequest(stat); 
	TEST(stat==KErrNone);
	TEST(inbuf.Length() == 2048 && inbuf.Left( testbufRead().Length() ) ==testbufRead);


	B.Close();

	// *************************************************************

	INFO_PRINTF1(_L("Testing open when port has a deferred destruction"));

	A.Close();
	TEST(A.Open(iCommSession,KDummyPortName,ECommPreemptable,aRole)==KErrNone);
	A.Close();

	TEST(A.Open(iCommSession,KDummyPortName,ECommPreemptable,aRole)==KErrNone);
	err = A.SetAccessMode(ECommExclusive);
	TEST(err==KErrNone);


	TEST(B.Open(csB,KDummyPortName,ECommExclusive,aRole)==KErrAccessDenied);
	A.Close();

	TEST(A.Open(iCommSession,KDummyPortName,ECommPreemptable,aRole)==KErrNone);
	TEST(A.SetAccessMode(ECommShared)==KErrNone);

#ifdef _DEBUG
	ret = A.DebugState(s);
	TESTCHECKL(ret, KErrNone);
	TEST(s.iAccessCount == 1 );
	TEST(s.iMode == EIntCommShared);
	TEST(s.iRole == aRole );
	TEST(s.iOutstandingCommands == 0);
#endif
	A.Close();

	delete hbuf2;
	iDPort0.Close();
	csB.Close();

	INFO_PRINTF1(_L("*** Preemption tests complete *** "));
	}

void CDummyTest::ShowSignals(TUint aSig, TCommRole aRole)
/**
 * Displays DCE/DTE RS232 signal values
 */
	{
	INFO_PRINTF1(_L("Signals "));	
	if (aRole == ECommRoleDTE)
		{
		if (aSig&KSignalCTS)
			INFO_PRINTF1(_L("CTS1: "));
		else
			INFO_PRINTF1(_L("CTS0: "));

		if (aSig&KSignalDSR)
			INFO_PRINTF1(_L("DSR1: "));
		else
			INFO_PRINTF1(_L("DSR0: "));

		if (aSig&KSignalDCD)
			INFO_PRINTF1(_L("DCD1: "));
		else
			INFO_PRINTF1(_L("DCD0: "));

		if (aSig&KSignalRNG)
			INFO_PRINTF1(_L("RNG1: "));
		else
			INFO_PRINTF1(_L("RNG0: "));
		if (aSig&KSignalDCEInputs)
			INFO_PRINTF2(_L("Got DCE signals, this is wrong %X"),aSig);

		}
	else
		{
		if (aSig&KSignalRTS)
			INFO_PRINTF1(_L("RTS1: "));
		else
			INFO_PRINTF1(_L("RTS0: "));

		if (aSig&KSignalDTR)
			INFO_PRINTF1(_L("DTR1: "));
		else
			INFO_PRINTF1(_L("DTR0: "));

		if (aSig&KSignalDTEInputs)
			INFO_PRINTF2(_L("Got DCE signals, this is wrong %X"),aSig);
		}
	
	if (aRole == ECommRoleDTE)
		{
		if (aSig&(KCTSChanged|KDSRChanged|KDCDChanged|KRNGChanged))
			INFO_PRINTF1(_L("Deltas "));	
		}
	else
		{
		if (aSig&(KRTSChanged|KDTRChanged))
			INFO_PRINTF1(_L("Deltas "));	
		}

	
	if (aRole == ECommRoleDTE)
		{
		if (aSig&KCTSChanged)
			INFO_PRINTF1(_L("CTS "));
		if (aSig&KDSRChanged)
			INFO_PRINTF1(_L("DSR "));
		if (aSig&KDCDChanged)
			INFO_PRINTF1(_L("DCD "));
		if (aSig&KRNGChanged)
			INFO_PRINTF1(_L("RNG "));
		}
	else
		{
		if (aSig&KRTSChanged)
			INFO_PRINTF1(_L("RTS "));
		if (aSig&KDTRChanged)
			INFO_PRINTF1(_L("DTR "));
		}
	}

void CDummyTest::ShowFlow(TFlowControl aFlow)
/**
 * @test Displays flowcontrol info
 */
	{
	if (aFlow==EFlowControlOn)
		INFO_PRINTF1(_L("EFlowControlOn  <XOFF> or CTS is LO or RTS is LO"));
	else
		INFO_PRINTF1(_L("EFlowControlOff <XON>  or CTS is HI or RTS is HI"));
	}

void CDummyTest::ShowConfig(TCommNotificationV01 aConfig)
/**
 * Displays TCommNotificationV01 values
 */
	{
	
	INFO_PRINTF1(_L("---------------------------------------------------------------"));
	if (aConfig.iChangedMembers&KRateChanged)
		INFO_PRINTF1(_L("KRateChanged "));

	if (aConfig.iChangedMembers&KDataFormatChanged)
		INFO_PRINTF1(_L("KDataFormatChanged "));

	if (aConfig.iChangedMembers&KHandshakeChanged)
		INFO_PRINTF1(_L("KHandshakeChanged "));			
		
	INFO_PRINTF1(_L(" \n - "));
	switch (aConfig.iRate)
		{
		case EBps110:
			INFO_PRINTF1(_L("EBps110 "));
			break;
		case EBps300:
			INFO_PRINTF1(_L("EBps300 "));
			break;
		case EBps600:
			INFO_PRINTF1(_L("EBps600 "));
			break;
		case EBps1200:
			INFO_PRINTF1(_L("EBps1200 "));
			break;
		case EBps2400:
			INFO_PRINTF1(_L("EBps2400 "));
			break;
		case EBps4800:
			INFO_PRINTF1(_L("EBps4800 "));
			break;
		case EBps9600:
			INFO_PRINTF1(_L("EBps9600 "));
			break;
		case EBps19200:
			INFO_PRINTF1(_L("EBps19200 "));
			break;
		case EBps38400:
			INFO_PRINTF1(_L("EBps38400 "));
			break;
		case EBps57600:
			INFO_PRINTF1(_L("EBps57600 "));
			break;
		case EBps115200:
			INFO_PRINTF1(_L("EBps115200 "));
			break;
		case EBpsSpecial:
			INFO_PRINTF1(_L("EBpsSpecial "));
			break;
		default:
			INFO_PRINTF1(_L("Not OK! "));
		}
	
	switch (aConfig.iDataBits)
		{
		case EData5:
			INFO_PRINTF1(_L("EData5 "));
			break;
		case EData6:
			INFO_PRINTF1(_L("EData6 "));
			break;
		case EData7:
			INFO_PRINTF1(_L("EData7 "));
			break;
		case EData8:
			INFO_PRINTF1(_L("EData8 "));
			break;			
		default:
			INFO_PRINTF1(_L("Not OK! "));
		}

	switch (aConfig.iStopBits)
		{
		case EStop1:
			INFO_PRINTF1(_L("EStop1 "));
			break;
		case EStop2:
			INFO_PRINTF1(_L("EStop2 "));
			break;
		default:
			INFO_PRINTF1(_L("Not OK! "));
		}

	switch (aConfig.iParity)
		{
		case EParityNone:
			INFO_PRINTF1(_L("EParityNone "));
			break;
		case EParityEven:
			INFO_PRINTF1(_L("EParityEven "));
			break;
		case EParityOdd:
			INFO_PRINTF1(_L("EParityOdd "));
			break;
		case EParityMark:
			INFO_PRINTF1(_L("EParityMark "));
			break;
		case EParitySpace:
			INFO_PRINTF1(_L("EParitySpace "));
			break;
		default:
			INFO_PRINTF1(_L("Not OK! "));
		}
	
	if (aConfig.iHandshake&KConfigSendXoff)
		INFO_PRINTF1(_L("KConfigSendXoff "));

	if (aConfig.iHandshake&KConfigObeyXoff)
		INFO_PRINTF1(_L("KConfigObeyXoff "));

	if (aConfig.iHandshake&KConfigObeyCTS)
		INFO_PRINTF1(_L("KConfigObeyCTS "));

	if (aConfig.iHandshake&KConfigObeyRTS)
		INFO_PRINTF1(_L("KConfigObeyRTS "));

	if (aConfig.iHandshake&KConfigObeyDTR)
		INFO_PRINTF1(_L("KConfigObeyDTR "));

	if (aConfig.iHandshake&KConfigObeyDSR)
		INFO_PRINTF1(_L("KConfigObeyDSR "));
	
	INFO_PRINTF1(_L(""));
	}

void CDummyTest::ShowCapsV01(TCommCapsV01 aCaps)
/**
 * Displays capabilities TCommCapsV01 values
 */
	{
	INFO_PRINTF1(_L("iRates:    "));
	if (aCaps.iRate&EBps50)
		INFO_PRINTF1(_L("50 "));
	if (aCaps.iRate&EBps75)
		INFO_PRINTF1(_L("75 "));
	if (aCaps.iRate&EBps110)
		INFO_PRINTF1(_L("110 "));
	if (aCaps.iRate&EBps134)
		INFO_PRINTF1(_L("134 "));
	if (aCaps.iRate&EBps150)
		INFO_PRINTF1(_L("150 "));
	if (aCaps.iRate&EBps300)
		INFO_PRINTF1(_L("300 "));
	if (aCaps.iRate&EBps600)
		INFO_PRINTF1(_L("600 "));
	if (aCaps.iRate&EBps1200)
		INFO_PRINTF1(_L("1200 "));
	if (aCaps.iRate&EBps1800)
		INFO_PRINTF1(_L("1800 "));
	if (aCaps.iRate&EBps2000)
		INFO_PRINTF1(_L("2000 "));
	if (aCaps.iRate&EBps4800)
		INFO_PRINTF1(_L("4800 "));
	if (aCaps.iRate&EBps7200)
		INFO_PRINTF1(_L("7200 "));
	if (aCaps.iRate&EBps9600)
		INFO_PRINTF1(_L("           9600 "));
	if (aCaps.iRate&EBps19200)
		INFO_PRINTF1(_L("19200 "));
	if (aCaps.iRate&EBps38400)
		INFO_PRINTF1(_L("38400 "));
	if (aCaps.iRate&EBps57600)
		INFO_PRINTF1(_L("57600 "));
	if (aCaps.iRate&EBps115200)
		INFO_PRINTF1(_L("115200 "));
	if (aCaps.iRate& static_cast<TUint>(EBpsSpecial))
		INFO_PRINTF1(_L("Special "));
	INFO_PRINTF1(_L(""));

	INFO_PRINTF1(_L("iDataBits: "));
	if (aCaps.iDataBits&EData5)
		INFO_PRINTF1(_L("EData5 "));
	if (aCaps.iDataBits&EData6)
		INFO_PRINTF1(_L("EData6 "));
	if (aCaps.iDataBits&EData7)
		INFO_PRINTF1(_L("EData7 "));
	if (aCaps.iDataBits&EData8)
		INFO_PRINTF1(_L("EData8 "));
	INFO_PRINTF1(_L(""));

	INFO_PRINTF1(_L("iStopBits: "));
	if (aCaps.iStopBits&EStop1)
		INFO_PRINTF1(_L("EStop1 "));
	if (aCaps.iStopBits&EStop2)
		INFO_PRINTF1(_L("EStop2 "));
	INFO_PRINTF1(_L(""));

	INFO_PRINTF1(_L("iParity:   "));
	if (aCaps.iParity&EParityNone)
		INFO_PRINTF1(_L("EParityNone "));
	if (aCaps.iParity&EParityEven)
		INFO_PRINTF1(_L("EParityEven "));
	if (aCaps.iParity&EParityOdd)
		INFO_PRINTF1(_L("EParityOdd "));
	if (aCaps.iParity&EParityMark)
		INFO_PRINTF1(_L("EParityMark "));
	if (aCaps.iParity&EParitySpace)
		INFO_PRINTF1(_L("EParitySpace "));
	INFO_PRINTF1(_L(""));

	INFO_PRINTF1(_L("iHandshake:"));
	if (aCaps.iHandshake&KConfigSendXoff)
		INFO_PRINTF1(_L("KConfigSendXoff "));
	if (aCaps.iHandshake&KConfigObeyXoff)
		INFO_PRINTF1(_L("KConfigObeyXoff "));
	if (aCaps.iHandshake&KConfigObeyCTS)
		INFO_PRINTF1(_L("KConfigObeyCTS "));
	if (aCaps.iHandshake&KConfigObeyRTS)
		INFO_PRINTF1(_L("KConfigObeyRTS "));
	if (aCaps.iHandshake&KConfigObeyDTR)
		INFO_PRINTF1(_L("KConfigObeyDTR "));
	if (aCaps.iHandshake&KConfigObeyDSR)
		INFO_PRINTF1(_L("KConfigObeyDSR "));
	INFO_PRINTF1(_L(""));
	
	INFO_PRINTF2((_L("iSignals:  %X")),aCaps.iSignals);
	INFO_PRINTF1(_L(""));

	INFO_PRINTF2((_L("iFifo:     %X")),aCaps.iFifo);
	INFO_PRINTF1(_L(""));

	INFO_PRINTF2((_L("iSIR:      %X")),aCaps.iSIR);
	INFO_PRINTF1(_L(""));
	}


void CDummyTest::ShowCapsV02(TCommCapsV02 aCaps)
/**
 * Displays capabilities TCommCapsV02 values
 */
	{
	ShowCapsV01(aCaps);

	INFO_PRINTF2((_L("V02 iNotificationCaps: %X")),aCaps.iNotificationCaps);
	INFO_PRINTF1(_L(""));

	INFO_PRINTF2((_L("V02 iRoleCaps:         %X")),aCaps.iRoleCaps);
	INFO_PRINTF1(_L(""));
	
	INFO_PRINTF2((_L("V02 iFlowControlCaps:  %X")),aCaps.iFlowControlCaps);
	INFO_PRINTF1(_L(""));
	}
	

CTestDCEPort::CTestDCEPort()
	{
	SetTestStepName(_L("TestDCEPort"));
	}

TVerdict CTestDCEPort::doTestStepPreambleL()
	{
	return TestStepResult();	
	}	
	
TVerdict CTestDCEPort::doTestStepPostambleL()
	{
	return TestStepResult();	
	}
	
TVerdict CTestDCEPort::doTestStepL()
/**
 * @test Tests setting a comm port as a DCE
 */
	{
	INFO_PRINTF1(_L("==================================="));
	INFO_PRINTF1(_L("Test : TestDCEPort"));
	INFO_PRINTF1(_L("==================================="));

	return TestStepResult();
	}
	
CTestPlatSecOpenPortDummy::CTestPlatSecOpenPortDummy()
	{
	SetTestStepName(_L("CTestPlatSecOpenPortDummy"));
	}

TVerdict CTestPlatSecOpenPortDummy::doTestStepL()
/**
 * @test Check opening and closing ports with PlatSec
 */
	{
	INFO_PRINTF1(_L("==================================="));
	INFO_PRINTF1(_L("Test : TestPlatSecOpenPortDummy"));
	INFO_PRINTF1(_L("==================================="));

const TInt KPlatSecEnforced=1;

	//Only run this test if PlatSec enabled
	PlatSec::TConfigSetting aSetting(PlatSec::EPlatSecEnforcement);
	if(PlatSec::ConfigSetting(aSetting) == KPlatSecEnforced)
		{	
		
		INFO_PRINTF1(_L("Opening dummy CSY"));	
		RCommServ commSession;
		TInt res = commSession.Connect();
		CleanupClosePushL(commSession);
		TESTCHECKL(res, KErrNone);
		
		res = commSession.LoadCommModule(KCsyName);
		TESTCHECKL(res, KErrNone);
		
		_LIT(KCommPort, "Dummy::%d"); 
		RComm dummyport0;
		RComm dummyport1;
		RComm dummyport2;
		RComm dummyport3;
		
		CleanupClosePushL(dummyport0);
		CleanupClosePushL(dummyport1);
		CleanupClosePushL(dummyport2);
		CleanupClosePushL(dummyport3);
				
		TBuf<10> portnumber;
				
		//Open port 0 - should pass always
		portnumber.Format(KCommPort, PORT_0_NOCAPS);
		res = dummyport0.Open(commSession, portnumber, ECommShared);
		TESTCHECKL(res, KErrNone);
		
		//Open port 1 - should pas because it is nrequires NetworkServices
		portnumber.Format(KCommPort, PORT_1_NETWORKSERVICES);
		res = dummyport1.Open(commSession, portnumber, ECommShared);
		TESTCHECKL(res, KErrNone);
		
		//Open port 2 - should fail because it is not a supported port
		portnumber.Format(KCommPort, PORT_2_NOTSUPPORT);
		res = dummyport2.Open(commSession, portnumber, ECommShared);
		TESTCHECKL(res, KErrPermissionDenied);	
		
		//Open port 3 - should fail with lack of capabilities
		portnumber.Format(KCommPort, PORT_3_ALLCAPS);
		res = dummyport3.Open(commSession, portnumber, ECommShared);
		TESTCHECKL(res, KErrPermissionDenied);	
		
		//now test with a second session
		INFO_PRINTF1(_L("Open another session"));
		RComm ecuartport;		
		CleanupClosePushL(ecuartport);
		
		_LIT(KCommPortWild, "COMM::%d");
		TBuf<10> portstring;
		
		//first try and open port with incorrect name
		_LIT(KBadProtName, "IGLOO::0");
		res = ecuartport.Open(iCommSession, KBadProtName, ECommShared);
		TESTCHECKL(res, KErrPermissionDenied);
		
		//Go through each port 0-KMaxUnits and try to open it.
		for(TInt port = 0;port<=KMaxUnits;port++)
			{
			portstring.Format(KCommPortWild, port);
			res = ecuartport.Open(iCommSession, portstring, ECommShared);
			INFO_PRINTF3(_L("Tried opening port=%S. Return code = %d."), &portstring, res);
			if(res == KErrAccessDenied)
				{
				INFO_PRINTF1(_L("Error is KErrAccessDenied: Port is already in use disable the attachment and try again!"));
				}
			if(port < KMaxUnits )
				{
				//we don't mind if it fails because of KErrAccessDenied or KErrNotSupported.
				TESTCHECKCONDITIONL(res==KErrNone || res==KErrAccessDenied || res == KErrNotSupported);
				}
			else
				{
				TESTCHECKL(res, KErrPermissionDenied);	
				}		
			ecuartport.Close();			
			}		
		dummyport0.Close();
		dummyport1.Close();
		dummyport2.Close();
		dummyport3.Close();
		
		//delay added as dummy.csy has slow closure of ports becuase it kicks a 2sec timer off before deleting the port object.
		User::After(5000000);
	
		res = commSession.UnloadCommModule(_L("Dummy"));
		
		iCommSession.NumPorts(res);
		commSession.NumPorts(res);
		
		commSession.Close();
		
		CleanupStack::PopAndDestroy(6);
		}
	else
		{
		INFO_PRINTF1(_L("No tests run as PlatSec not enforced."));
		}

	return TestStepResult(); 
	}
	
	
TVerdict CTestOpenWhenAvailable::doTestStepL()
	{
	//
	// Note: The server is designed to support the OpenWhenAvailable request
	//       dispatched from a different RCommServ session than the one
	//       actively using the port. Testing follows this assumption.
	//
	_LIT(KCSYName, "LOOPBACK.CSY");
	_LIT(KPortName, "LOOPBACK::0");

	//
	// Connect to the server and load the CSY...
	//
	RCommServ  commServ1, commServ2;
	TInt  ret;

	ret = commServ1.Connect();
	TESTCHECKL(ret, KErrNone);
	CleanupClosePushL(commServ1);

	ret = commServ1.LoadCommModule(KCSYName);
	TESTCHECKL(ret, KErrNone);
	
	ret = commServ2.Connect();
	TESTCHECKL(ret, KErrNone);
	CleanupClosePushL(commServ2);

	ret = commServ2.LoadCommModule(KCSYName);
	TESTCHECKL(ret, KErrNone);
	
	//
	// First try OpenWhenAvailable() when nothing is holding the port...
	//
	INFO_PRINTF1(_L("Scenario: OpenWhenAvailable() when nothing is holding the port."));

	TRequestStatus  status;

	iSerialPortList[0].OpenWhenAvailable(status, commServ1, KPortName);
	User::WaitForRequest(status);
	TESTCHECK(status.Int(), KErrNone);

	ret = iSerialPortList[0].SetAccessMode(ECommExclusive);
	TESTCHECKL(ret, KErrNone);

	//
	// Now cancel the request (nothing should happen as the request was
	// completed). Check that the port is still open and in use by opening
	// the same port again.
	//
	INFO_PRINTF1(_L("Scenario: Cancel a completed request (nothing should happen & the port is still open)."));

	iSerialPortList[0].OpenWhenAvailableCancel();
	TESTCHECK(ret, KErrNone);

	ret = iSerialPortList[1].Open(commServ2, KPortName, ECommExclusive);
	TESTCHECK(ret, KErrAccessDenied);

	iSerialPortList[0].Close();
	iSerialPortList[1].Close();

	//
	// Perform the main use case of waiting for a port to become free.
	//
	INFO_PRINTF1(_L("Scenario: Waiting for a port to become free."));

	ret = iSerialPortList[0].Open(commServ1, KPortName, ECommExclusive);
	TESTCHECK(ret, KErrNone);

	iSerialPortList[1].OpenWhenAvailable(status, commServ2, KPortName);
	
	User::After(5*1000000);
	TESTCHECK(status.Int(), KRequestPending);

	iSerialPortList[0].Close();
	
	User::WaitForRequest(status);
	TESTCHECK(status.Int(), KErrNone);

	iSerialPortList[1].Close();

	//
	// Wait for the a port to become free but cancel the request.
	//
	INFO_PRINTF1(_L("Scenario: Wait for the a port to become free but cancel the request."));

	ret = iSerialPortList[0].Open(commServ1, KPortName, ECommExclusive);
	TESTCHECK(ret, KErrNone);

	iSerialPortList[1].OpenWhenAvailable(status, commServ2, KPortName);
	
	User::After(5*1000000);
	TESTCHECK(status.Int(), KRequestPending);

	iSerialPortList[1].OpenWhenAvailableCancel();
	
	User::WaitForRequest(status);
	TESTCHECK(status.Int(), KErrCancel);

	iSerialPortList[0].Close();

	//
	// Wait for the a port to become free but close the sub-session.
	//
	INFO_PRINTF1(_L("Scenario: Wait for the a port to become free but close the sub-session."));

	ret = iSerialPortList[0].Open(commServ1, KPortName, ECommExclusive);
	TESTCHECK(ret, KErrNone);

	iSerialPortList[1].OpenWhenAvailable(status, commServ2, KPortName);
	
	User::After(5*1000000);
	TESTCHECK(status.Int(), KRequestPending);

	iSerialPortList[1].Close();
	
	User::WaitForRequest(status);
	TESTCHECK(status.Int(), KErrCancel);

	iSerialPortList[0].Close();

	//
	// Clean up...
	//
	CleanupStack::PopAndDestroy(&commServ2);
	CleanupStack::PopAndDestroy(&commServ1);

	return TestStepResult();
	} // CTestOpenWhenAvailable::doTestStepL



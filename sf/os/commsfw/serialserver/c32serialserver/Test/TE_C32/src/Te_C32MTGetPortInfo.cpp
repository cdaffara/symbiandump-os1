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
#include "C32COMM.H"
#include "dummy.h"
#include "Te_C32MultiThreading.h"

//Test case id NET-C32-I-0016-HP
//Get the number of CSY loaded from different session
CNumPortsdiffSession::CNumPortsdiffSession()
/**
 * Constructor
 */
	{
	SetTestStepName(_L("NumPortsdiffSession"));
	}
/**
 * @see NumPortsdiffSession test case NET-C32-I-0016-HP
 *
 * Session function does the below action
 * Connect to a C32 server
 * get the Number of CSY loaded from other session
 * close the connection to C32
 */
TInt CNumPortsdiffSession::Session(TAny* aArg)
/**
 * Thread which attempts get number of CSY loaded from other session
 */
	{
	CNumPortsdiffSession*	 stepTest = reinterpret_cast<CNumPortsdiffSession*> (aArg);
	TName name;

	//Creating a RcommSession, connecting to the C32 server
	RCommServ commSession;
	TInt res = commSession.Connect();
	if(res != KErrNone)
		{
		stepTest->iState1 = __LINE__ - 2;
		return(res);
		}

	//Getting the number of CSY loaded
	TInt portNum;
	 res = commSession.NumPorts(portNum);
	if(res != KErrNone)
		{
		stepTest->iState2 = __LINE__ - 2;
		commSession.Close();
		return res;
		}
	stepTest->iNPorts = portNum;
	commSession.Close();
	return KErrNone;
	}
/**
 * @see NumPortsdiffSession test case NET-C32-I-0016-HP
 *
 * doTestStepL virtual function does the below action
 * Connect to a C32 server
 * Load the CSY(Communication provider module)
 * close the connection to C32
 * Expected:-Checking for the Number of CSY loaded
*/
TVerdict CNumPortsdiffSession::doTestStepL()
	{

	SetTestStepResult(EPass);
	INFO_PRINTF1(_L("==============================================="));
	INFO_PRINTF1(_L("Get Number of CSY loaded from different session"));
	INFO_PRINTF1(_L("==============================================="));

	_LIT(KCSYName,"LOOPBACK.CSY");
	_LIT(KCSYName1,"DummyCsy1.CSY");

	//Loading the communication system(CSY)
	TInt ret = iCommSession.LoadCommModule(KCSYName);
	if(ret != KErrNone)
		{
		INFO_PRINTF2(_L("ERROR: Load Comm module in session 1 failed %d"), ret);
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	else
		INFO_PRINTF2(_L("OK. '%S' CSY loaded"), &KCSYName);

	//Loading the communication system(CSY)
	ret = iCommSession.LoadCommModule(KCSYName1);
	if(ret != KErrNone)
		{
		INFO_PRINTF2(_L("ERROR: Load Comm module in session 1 failed %d"), ret);
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	else
		INFO_PRINTF2(_L("OK. '%S' CSY loaded"), &KCSYName1);

	//Creating thread to load the CSY in different session
	RThread sessionThread;
	TInt res = sessionThread.Create(_L("SessionPortThr"), Session, KDefaultStackSize, KDefaultHeapSizeThread, KDefaultHeapSizeThread, this);
	if(res != KErrNone)
		{
		INFO_PRINTF2(_L("ERROR: Creating a thread failed %d"), res);
		SetTestStepResult(EFail);
		return TestStepResult();
		}

	//Requests notification when thread dies
	TRequestStatus stat;
	sessionThread.Logon(stat);
	//Makes the thread eligible for execution.
	sessionThread.Resume();
	User::WaitForRequest(stat);
	TESTCHECKL(stat.Int(), KErrNone);

	//Closing the thread handler
	CLOSE_AND_WAIT(sessionThread);

	//Checking error condition for thread
	if(iState1 != KErrNone)
		{
		INFO_PRINTF2(_L("ERROR: occured, iState1, in session 2 at line %d, expected KErrNone."), iState1);
		SetTestStepResult(EFail);
		return TestStepResult();
		}

	//Checking error condition for thread
	if(iState2 != KErrNone)
		{
		INFO_PRINTF2(_L("ERROR: occured, iState2, in session 2 at line %d, expected KErrNone."), iState2);
		SetTestStepResult(EFail);
		return TestStepResult();
		}

	if(iNPorts != PORT_NUMBER)
		{
		INFO_PRINTF2(_L("Error:in session 2  Numport is    %d, expected 3."), iNPorts);
		SetTestStepResult(EFail);
		return TestStepResult();
		}

	return TestStepResult();

	}


// Test case id NET-C32-I-0019-HP
// CSY is communicating with anoter CSY which is loaded in another player Thread
/**
 * Constructor
 */
CTestStackableCSY::CTestStackableCSY()
	{
	SetTestStepName(_L("TestStackableCSY"));
	}
/**
 * @see TestStackableCSY test case NET-C32-I-0019
 *
 * doTestStep function does the below action
 * Connect to a C32 server
 * Load the csy in  Dealer thread, that internally load another csy
 * close the connection to C32
 */
TVerdict CTestStackableCSY::doTestStepL()
	{
	INFO_PRINTF1(_L("================================================"));
	INFO_PRINTF1(_L("    Test :PREQ890 C32 Load CSY by another csy  "));
	INFO_PRINTF1(_L("==============================================="));

	INFO_PRINTF1(_L("Loading CSY in C32 component"));
	_LIT(KCSYName,"DummyCsy10");

    //Loading the communication system(CSY)
	TInt ret = iCommSession.LoadCommModule(KCSYName);
	if(ret != KErrNone)
		{
		INFO_PRINTF3(_L("ERROR: could not load '%S' CSY in Dealer thread due to %d"), &KCSYName, ret);
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	else
		INFO_PRINTF2(_L("OK. '%S' CSY loaded"), &KCSYName);

	_LIT(KCSYName1, "DummyCsy10");
	TName name;
	name.Copy(KCSYName1);
	TSerialInfo protocolInfo;

	//getting the info of the each CSY loaded
	TInt result = iCommSession.GetPortInfo(name,protocolInfo);
	if(result != KErrNone)
		{
		INFO_PRINTF1(_L("ERROR: Getting a port info after loading Comms module failed\n"));
		INFO_PRINTF2(_L("ERROR: GetPortInfo returned failded%d"), result);
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	else
		{
		INFO_PRINTF2(_L("description:    %S"), &protocolInfo.iDescription);
		INFO_PRINTF2(_L("port name  :    %S"), &protocolInfo.iName);
		INFO_PRINTF2(_L("low unit   :    %d"), protocolInfo.iLowUnit);
		INFO_PRINTF2(_L("high unit  :    %d"), protocolInfo.iHighUnit);
		}

	return TestStepResult();
	}


//Test case id NET-C32-I-0027-HP
//Loading a CSY in which two CMI set Wildcard(*)

/**
 * Constructor
 */
CTwoCMIfileSetWildcard::CTwoCMIfileSetWildcard()

	{
	SetTestStepName(_L("TwoCMIfileSetWildcard"));
	}

/**
 * @see  TwoCMIfileSetWildcard test case NET-C32-I-0027-HP
 *
 * doTestStep function does the below action
 * Connect to a C32 server
 * Load the csy in Thread
 * close the connection to C32
 */
TVerdict CTwoCMIfileSetWildcard::doTestStepL()
	{

	SetTestStepResult(EPass);

	INFO_PRINTF1(_L("==================================="));
	INFO_PRINTF1(_L("Two CMI file set Wildcard set      "));
	INFO_PRINTF1(_L("==================================="));

	_LIT(KCSYName,"DummyCsy6.CSY");

	//Loading the communication system(CSY)
	TInt ret = iCommSession.LoadCommModule(KCSYName);
	if(ret != KErrNone)
		{
		INFO_PRINTF2(_L("ERROR: Load Comm module failed %d"), ret);
		SetTestStepResult(EFail);
		}
    else
    	INFO_PRINTF2(_L("OK. '%S' CSY loaded"), &KCSYName);

	return TestStepResult();

	}


// Test Case No NET-C32-I-0035-HP
// Loading  a CSYs make GetPortInfo API call(passing index parameter as -ve)
/**
 * Constructor
 */
CGetPortInfo::CGetPortInfo()
	{
	SetTestStepName(_L("GetPortInfo"));
	}

/**
 * @see GetPortInfo test case NET-C32-I-0035
 *
 * doTestStep function does the below action
 * Connect to a C32 server
 * Loading CSY in Dealer thread, pass -ve index value into GetPortInfo API
 * close the connection to C32
 */
TVerdict CGetPortInfo::doTestStepL()
	{

	SetTestStepResult(EPass);

	INFO_PRINTF1(_L("================================================================================================"));
	INFO_PRINTF1(_L("Test :PREQ890 C32 Loading CSYs make GetPortInfo API call(passing a -ve value as index parameter "));
	INFO_PRINTF1(_L("================================================================================================"));

	TName name;
	TSerialInfo protocolInfo;
	_LIT(KPortName,"COMM");
	TInt numIndex = -1;

	//getting the info of the each CSY loaded
	TInt result = iCommSession.GetPortInfo(numIndex,name,protocolInfo);
	if(result != KErrArgument)
		{
		INFO_PRINTF1(_L("ERROR: Getting a port info after loading Comms module\n"));
		INFO_PRINTF3(_L("ERROR: index %d: GetPortInfo returned %d"), numIndex, result);
		SetTestStepResult(EFail);
		return TestStepResult();
		}

	//UnLoading communication system(CSY)
 	TInt retValue = iCommSession.UnloadCommModule(KPortName);
 	if(	retValue != KErrNone)
 		{
 		INFO_PRINTF1(_L("UnLoading LOOPBACK CSY\n"));
 		SetTestStepResult(EFail);
 		return TestStepResult();
 		}

	return TestStepResult();
	}

// Test case id NET-C32-I-0032-HP
// Loading a CSY in player thread which will block other csy
/**
 * Constructor
 */
CTestBlockCSY::CTestBlockCSY()
	{
	SetTestStepName(_L("TestBlockCSY"));
	}


/**
 * @see TestBlockCSY test case NET-C32-I-0032*
 * doTestStep function does the below action
 * Connect to a C32 server
 * Load the blockcsy in  player thread
 * close the connection to C32
 */

TVerdict CTestBlockCSY::doTestStepL()
	{
	INFO_PRINTF1(_L("==========================================================="));
	INFO_PRINTF1(_L("Test :PREQ890 C32 Block CSY "));
	INFO_PRINTF1(_L("==========================================================="));

	RCommServ servSession;

	CleanupClosePushL(servSession);

    RComm dummyCSY1ReadPort;
    RComm dummyCSY1WritePort;
	RComm blockDummyReadPort;
	RComm blockDummyWritePort;

    // Connecting to the RCommServ
   	TInt res = servSession.Connect();
	if(res != KErrNone)
		{
		INFO_PRINTF2(_L("ERROR: could not connect RCommServ %d"), res);
		SetTestStepResult(EFail);
		}
	else
		INFO_PRINTF1(_L("RCommServ Connection established sucessfully"));

	_LIT(KCSYName1, "DummyCsy12");   			// blockcsy which has time delay
	_LIT(KBlockDummyWritePort,"DummyCsy12::6");	// Port for writing
	_LIT(KBlockDummyReadPort,"DummyCsy12::0");	// Port for reading

	_LIT(KCSYName2, "DummyCsy1");
	_LIT(KDummy1WritePort,"DummyCsy1::6");  	// Port for writing
	_LIT(KDummy1ReadPort,"DummyCsy1::0");	  	// Port for reading

	_LIT8(KDummyText,"This string has to be exactly like this othervice write returns KErrCommsLineFail.");

    TRequestStatus dummy1WriteResponse;
	TRequestStatus dummy1ReadResponse;
	TRequestStatus blockDummyWriteResponse;
	TRequestStatus blockDummyReadResponse;

	// Loading blockcsy and dummycsy1
	TInt ret1=servSession.LoadCommModule(KCSYName1);
	TESTCHECKL(ret1,KErrNone);
	TInt ret2=servSession.LoadCommModule(KCSYName2);
	TESTCHECKL(ret2,KErrNone);

	TBuf8<KNoOfCharsDummyExpectsToBeWrittern> dummyCSY1ReadBuf;
	TBuf8<KNoOfCharsDummyExpectsToBeWrittern> dummyCSY1WriteBuf;
	TBuf8<KNoOfCharsDummyExpectsToBeWrittern> blockDummyReadBuf;
	TBuf8<KNoOfCharsDummyExpectsToBeWrittern> blockDummyWriteBuf;

 	dummyCSY1WriteBuf.Copy(KDummyText);
	blockDummyWriteBuf.Copy(KDummyText);

 	 //Open Dummycsy port for writing
 	ret1=blockDummyWritePort.Open(servSession, KBlockDummyWritePort, ECommShared);

	//Open Dummycsy port for Reading
	ret2=blockDummyReadPort.Open(servSession, KBlockDummyReadPort, ECommShared);

	//Open Loopbackcsy port for writing
 	TInt ret3=dummyCSY1WritePort.Open(servSession, KDummy1WritePort, ECommShared);

 	//Open Loopbackcsy port for reading
	TInt ret4=dummyCSY1ReadPort.Open(servSession, KDummy1ReadPort, ECommShared);

	if(ret1 == KErrNone && ret2 == KErrNone && ret3 == KErrNone && ret4 == KErrNone)
		{
		/*
				Algorithm for blockcsy

		Issue a write request to blockcsy(dummycsy12) and wait until write
		request to be completed. Issue read request to blockcsy, which will
		be in block state upto 6 secs(6 secs delay is mentined in the dummycsy12
		startread function)
		Issue write request to dummycsy1 and wait until write request to be
		completed. Issue read request and enter in to the loop.
		Inside the loop, WaitForAnyRequest either dummycsy1 read request to
		be completed or blockcsy(dummycsy12) to be completed.

		if the blockDummyReadResponse is in KRequestPending state, Issue write
		request to write request to dummycsy1 and wait until write request to
		be completed. . Issue read request. loop should be continue until
		(dummy1ReadCount < KMaxBlockingTestWrites) && (blockDummyReadResponse.Int() == KRequestPending ))

		Test will be fail,
		1) if blockDummyReadResponse is not KRequestPending before dummycsy1 read write completion, t
		2) if Elapsed time exceeded the expected time (KDummy12ReadWait + KDummy12ReadWaitContingency)

		*/

		TTime startTime,stopTime;

		TInt dummy1ReadCount = 0;
		startTime.UniversalTime();

 		// testing write

 		blockDummyWritePort.Write(blockDummyWriteResponse, blockDummyWriteBuf);
 		User::WaitForRequest(blockDummyWriteResponse);
		if(blockDummyWriteResponse.Int() != KErrNone)
			{
			INFO_PRINTF2(_L("Port could not open for read write %d"),blockDummyWriteResponse.Int());
			CleanupStack::PopAndDestroy();
			return TestStepResult();
			}

  		blockDummyReadPort.Read(blockDummyReadResponse, blockDummyReadBuf);

 		dummyCSY1WritePort.Write(dummy1WriteResponse, dummyCSY1WriteBuf);
 		User::WaitForRequest(dummy1WriteResponse);

  		dummyCSY1ReadPort.Read(dummy1ReadResponse, dummyCSY1ReadBuf);
		while((dummy1ReadCount < KMaxBlockingTestWrites) && (blockDummyReadResponse.Int() == KRequestPending ))
			{
			User::WaitForAnyRequest();
			TESTCHECKL(dummy1ReadResponse.Int(),KErrNone);
			if((blockDummyReadResponse.Int() == KRequestPending ))
				{
				// Writing to the port
				dummyCSY1WritePort.Write(dummy1WriteResponse, dummyCSY1WriteBuf);
				User::WaitForRequest(dummy1WriteResponse);
				TESTCHECKL(dummy1WriteResponse.Int(),KErrNone);

				dummyCSY1ReadPort.Read(dummy1ReadResponse, dummyCSY1ReadBuf);
				}
			dummy1ReadCount++;
			}
			if(blockDummyReadResponse.Int() != KRequestPending)
				{
				SetTestStepResult(EFail);
				User::WaitForRequest(dummy1ReadResponse);      // It is required to completed the dummy1Read
				TESTCHECKL(dummy1ReadResponse.Int(),KErrNone); // Otherwise it will send KErrCancel -3
				INFO_PRINTF1(_L("Test is fail, blockDummyReadResponse status has changed before dummy1 Read complete"));
				}
			else
				{
				stopTime.UniversalTime();
				User::WaitForRequest(blockDummyReadResponse);// It is required to completed the blockDummyRead since it was KErrPending
				TESTCHECKL(blockDummyReadResponse.Int(),KErrNone);
				TESTCHECKL(dummy1ReadCount,KMaxBlockingTestWrites);  										  // Otherwise it will send KErrCancel -3
				TInt diffTime = stopTime.MicroSecondsFrom(startTime).Int64();

				if(diffTime >= ( KDummy12ReadWait + KDummy12ReadWaitContingency ))
					{
					INFO_PRINTF2(_L("ERROR:Elapsed time exceeded the expected time for new c32 %d microseconds"),diffTime);
					INFO_PRINTF2(_L("ERROR:Total time expected for new c32 is less than %d microseconds"),KDummy12ReadWait + KDummy12ReadWaitContingency);
					SetTestStepResult(EFail);
					}
				else
					{
					INFO_PRINTF2(_L("Elapsed time is within the expected bounds for Multithreaded c32 - %d microseconds"),diffTime);
					INFO_PRINTF1(_L("In single threaded c32 this testcase will abort due to block thread by csy"));
					SetTestStepResult(EPass);
					}
				}

		}
	else
		{
		INFO_PRINTF1(_L("ERROR:Port could not open for read write"));
		SetTestStepResult(EFail);
		}
	dummyCSY1ReadPort.Close();
	dummyCSY1WritePort.Close();
	blockDummyReadPort.Close();
	blockDummyWritePort.Close();
	User::After(3000000);	// delay of 3 sec since dummy csy 'port close' internally has a timer of 2 sec.
	CleanupStack::PopAndDestroy();
	return TestStepResult();
	}

// Test case id NET-C32-I-0033-HP
// Csy could not be loaded when it is corrupted
/**
 * Constructor
 */
CTestCorruptCsy::CTestCorruptCsy()
	{
	SetTestStepName(_L("TestCorruptCsy"));
	}
/**
 * @see  TestLoadDealerCSY test case NET-C32-I-0033
 *
 * doTestStep function does the below action
 * Connect to a C32 server
 * Load the csy in  Dealer thread
 * close the connection to C32
 */
TVerdict CTestCorruptCsy::doTestStepL()
	{
	INFO_PRINTF1(_L("================================================"));
	INFO_PRINTF1(_L("   Test :PREQ890 C32 Load CSY in Dealer thread  "));
	INFO_PRINTF1(_L("================================================"));

	INFO_PRINTF1(_L("Loading CSY in C32 component"));
	_LIT(KCSYName,"DummyCsy15");

    //Loading the communication system(CSY)
	TInt ret = iCommSession.LoadCommModule(KCSYName);
	if(ret == KErrNone)
		{
		INFO_PRINTF2(_L("ERROR: '%S' CSY loaded "), &KCSYName);
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	else
		{
		INFO_PRINTF2(_L("OK. '%S' CSY not loaded since it is corrupted"), &KCSYName);
		}

	return TestStepResult();
	}

// Test Case No NET-C32-I-0038-HP
// Loading and Unloading Different CSYs make GetPortInfo by name as a parameter API call
/**
 * Constructor
 */
CGetPortInfoByName::CGetPortInfoByName()
	{
	SetTestStepName(_L("GetPortInfoByName"));
	}

/**
 * @see GetPortInfoByName test case NET-C32-I-0038-HP
 *
 * doTestStep function does the below action
 * Connect to a C32 server
 * Loading and Unload CSY , GetPortInfo API by name as parameter call in to it
 * close the connection to C32
 */
TVerdict CGetPortInfoByName::doTestStepL()
	{


	SetTestStepResult(EPass);

	INFO_PRINTF1(_L("================================================================================="));
	INFO_PRINTF1(_L("Test :PREQ890 C32 Loading and Unloading Different CSYs make GetPortInfo API call "));
	INFO_PRINTF1(_L("================================================================================="));

	_LIT(KCSYName,"DummyCsy1");
	_LIT(KCSYName1,"LOOPBACK");

	TName name;
	TSerialInfo protocolInfo;
	RCommServ CommSession;
	//Creating a RcommSession, connecting to the C32 server
	TInt res=CommSession.Connect();
	if(res != KErrNone)
		{
		INFO_PRINTF2(_L("ERROR: could not connect RCommServ %d"), res);
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	else
		INFO_PRINTF2(_L("OK.'%d'Connection established"), res);

	//Loading communication system(CSY)
	TInt status = CommSession.LoadCommModule(KCSYName);
	if(status != KErrNone)
		{
		INFO_PRINTF2(_L("ERROR: could not Load DUMMY CSY %d"), status);
		SetTestStepResult(EFail);
		CommSession.Close();
		return TestStepResult();
		}

	status = CommSession.LoadCommModule(KCSYName1);
	if(status != KErrNone)
		{
		INFO_PRINTF2(_L("ERROR: could not Load LOOPBACK CSY %d"), status);
		SetTestStepResult(EFail);
		CommSession.Close();
		return TestStepResult();
		}

	//Getting the number of CSY loaded
	TInt num;
	TInt result=CommSession.NumPorts(num);
	if((result == KErrNone) && (num == 3))
		{
		INFO_PRINTF2(_L("Get number of loaded comms modules (should be 3) : %d"), num);
		}
   else
   		{
   		SetTestStepResult(EFail);
   		CommSession.Close();
   		return TestStepResult();
   		}

   for(TInt i = 0; i< num; i++)
		{
		//getting the info of the each CSY loaded
		result = CommSession.GetPortInfo(i,name,protocolInfo);
	   	if(result != KErrNone)
			{
			INFO_PRINTF1(_L("Getting a port info after loading Comms module\n"));
			INFO_PRINTF3(_L("index %d: GetPortInfo returned %d"), i, result);
			SetTestStepResult(EFail);
			CommSession.Close();
			return TestStepResult();
			}
		else
			{
			INFO_PRINTF3(_L("index    %d:   '%S'"), i, &name);
			INFO_PRINTF2(_L("description:    %S"), &protocolInfo.iDescription);
			INFO_PRINTF2(_L("port name  :    %S"), &protocolInfo.iName);
			INFO_PRINTF2(_L("low unit   :    %d"), protocolInfo.iLowUnit);
			INFO_PRINTF2(_L("high unit  :    %d"), protocolInfo.iHighUnit);
		 if(protocolInfo.iName.CompareF(_L("COMM")) != 0 && i == 0)
				{
				INFO_PRINTF2(_L("Comm module is '%S'"), &protocolInfo.iName);
				SetTestStepResult(EFail);
				CommSession.Close();
				return TestStepResult();
				}
			else
			if(name.CompareF(_L("DummyCsy1")) != 0 && i == 1)
				{
				INFO_PRINTF3(_L("index %d: Comm module is '%S'"), i, &name);
				SetTestStepResult(EFail);
				CommSession.Close();
				return TestStepResult();
				}
			else
			if(name.CompareF(_L("LOOPBACK")) != 0 && i ==2)
				{
				INFO_PRINTF3(_L("index %d: Comm module is '%S'"), i, &name);
				SetTestStepResult(EFail);
				CommSession.Close();
				return TestStepResult();
				}

			}
		}

	    //Closing Rcommsession
	    CommSession.Close();
		return TestStepResult();
 }


// Test case id NET-C32-I-0036-HP
// Loading a CSY in which Two CMI file set UNSwquential Worker ID.
//Three cmi file set WorkerID=0. WorkerID=1, other cmi file set WorkerID=3
/**
 * Constructor
 */
CUnSequentialWorkerID::CUnSequentialWorkerID()
	{
	SetTestStepName(_L("UnSequentialWorkerID"));
	}
/**
 * @see TestLoadDealerCSY test case NET-C32-I-0036-HP
 *
 * doTestStep function does the below action
 * Connect to a C32 server
 * Load the csy in  Dealer thread
 * close the connection to C32
 */
TVerdict CUnSequentialWorkerID::doTestStepL()
	{
	INFO_PRINTF1(_L("================================================"));
	INFO_PRINTF1(_L("   Test :CMI file set Unsequential worker ID   "));
	INFO_PRINTF1(_L("==============================================="));

	INFO_PRINTF1(_L("Loading CSY in C32 component"));
	_LIT(KCSYName,"DummyCsy1");

    //Loading the communication system(CSY)
	TInt ret = iCommSession.LoadCommModule(KCSYName);
	if(ret != KErrNone)
		{
		INFO_PRINTF3(_L("ERROR: could not load '%S' CSY  due to %d"), &KCSYName, ret);
		SetTestStepResult(EFail);
		}
	else
		INFO_PRINTF2(_L("OK. '%S' CSY loaded"), &KCSYName);

	return TestStepResult();
	}


// Test Case No NET-C32-I-0013-HP
// Loading and Unloading Different CSYs make GetPortInfo API call
/**
 * Constructor
 */
CTestGetPortInfo::CTestGetPortInfo()
	{
	SetTestStepName(_L("TestGetPortInfo"));
	}

/**
 * @see TestGetPortInfo test case NET-C32-I-0013-HP
 *
 * doTestStep function does the below action
 * Connect to a C32 server
 * Loading and Unload CSY , GetPortInfo API call in to it
 * close the connection to C32
 */
TVerdict CTestGetPortInfo::doTestStepL()
	{

	SetTestStepResult(EPass);

	INFO_PRINTF1(_L("================================================================================="));
	INFO_PRINTF1(_L("Test :PREQ890 C32 Loading and Unloading Different CSYs make GetPortInfo API call "));
	INFO_PRINTF1(_L("================================================================================="));

	_LIT(KCSYName,"DummyCsy1");
	_LIT(KCSYName1,"DummyCsy2");

	TName name;
	TSerialInfo protocolInfo;
	RCommServ CommSession;
	//Creating a RcommSession, connecting to the C32 server
	TInt res = iCommSession.Connect();
	if(res != KErrNone)
		{
		INFO_PRINTF2(_L("ERROR: could not connect RCommServ %d"), res);
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	else
		INFO_PRINTF2(_L("OK.'%d'Connection established"), res);

	//Loading communication system(CSY)
	TInt status = iCommSession.LoadCommModule(KCSYName);
	if(status != KErrNone)
		{
		INFO_PRINTF2(_L("ERROR: could not Load DUMMY CSY %d"), status);
		SetTestStepResult(EFail);
		iCommSession.Close();
		return TestStepResult();
		}

	status = iCommSession.LoadCommModule(KCSYName1);
	if(status != KErrNone)
		{
		INFO_PRINTF2(_L("ERROR: could not Load LOOPBACK CSY %d"), status);
		SetTestStepResult(EFail);
		iCommSession.Close();
		return TestStepResult();
		}

	//Getting the number of CSY loaded

	TInt num;
	TInt result = iCommSession.NumPorts(num);
	if((result == KErrNone) && (num == 3))
		{
		INFO_PRINTF2(_L("Get number of loaded comms modules (should be 3) : %d"), num);
		}
   else
   		{
   		INFO_PRINTF2(_L("Get number of loaded comms modules (should be 3) : %d"), num);
   		SetTestStepResult(EFail);
   		iCommSession.Close();
   		return TestStepResult();
   		}

   for(TInt i = 0 ; i< num; i++)
		{
		//getting the info of the each CSY loaded
		result = iCommSession.GetPortInfo(i, name, protocolInfo);
	   	if(result != KErrNone)
			{
			INFO_PRINTF1(_L("ERROR:Getting a port info after loading Comms module\n"));
			INFO_PRINTF3(_L("ERROR:index %d: GetPortInfo returned %d"), i, result);
			SetTestStepResult(EFail);
			iCommSession.Close();
			return TestStepResult();
			}
		else
			{
			INFO_PRINTF3(_L("index    %d:   '%S'"), i, &name);
			INFO_PRINTF2(_L("description:    %S"), &protocolInfo.iDescription);
			INFO_PRINTF2(_L("port name  :    %S"), &protocolInfo.iName);
			INFO_PRINTF2(_L("low unit   :    %d"), protocolInfo.iLowUnit);
			INFO_PRINTF2(_L("high unit  :    %d"), protocolInfo.iHighUnit);
		 if(protocolInfo.iName.CompareF(_L("COMM")) != 0 && i == 0)
				{
				INFO_PRINTF2(_L("ERROR:Comm module is '%S'"), &protocolInfo.iName);
				SetTestStepResult(EFail);
				iCommSession.Close();
				return TestStepResult();
				}
			else
			if(name.CompareF(_L("DummyCsy1")) != 0 && i == 1)
				{
				INFO_PRINTF3(_L("ERROR:index %d: Comm module is '%S'"), i, &name);
				SetTestStepResult(EFail);
				iCommSession.Close();
				return TestStepResult();
				}
			else
			if(name.CompareF(_L("DummyCsy2")) != 0 && i == 2)
				{
				INFO_PRINTF3(_L("ERROR:index %d: Comm module is '%S'"), i, &name);
				SetTestStepResult(EFail);
				iCommSession.Close();
				return TestStepResult();
				}

			}
		}

	    //Closing Rcommsession
	    iCommSession.Close();
		return TestStepResult();
	}


// Test case id NET-C32-I-0039-HP
// CSY is communicating with anoter CSY which is loaded in another player Thread.
// Session is not closed after unload csy
/**
 * Constructor
 */
CTestStackableCSYNoClose::CTestStackableCSYNoClose()
	{
	SetTestStepName(_L("TestStackableCSYNoClose"));
	}
/**
 * @see TestStackableCSYNoClose test case NET-C32-I-0039
 *
 * doTestStep function does the below action
 * Connect to a C32 server
 * Load the csy in  Dealer thread, that internally load another csy
 * close the connection to C32. session is not closed after unload
 */
TVerdict CTestStackableCSYNoClose::doTestStepL()
	{

	INFO_PRINTF1(_L("================================================"));
	INFO_PRINTF1(_L("    Test :PREQ890 C32 Load CSY by another csy  "));
	INFO_PRINTF1(_L("    This test case should panic the server thread "));
	INFO_PRINTF1(_L("==============================================="));
	//Session close is commented to test the whether the function
	//CCommSession::CC32SessionCloseTimer::RunL() is called?
	//server thread should panic since the session is not closed

	INFO_PRINTF1(_L("Loading CSY in C32 component"));
	_LIT(KCSYName,"DummyCsy17");

    //Loading the communication system(CSY)
	TInt ret = iCommSession.LoadCommModule(KCSYName);
	if(ret != KErrNone)
		{
		INFO_PRINTF3(_L("ERROR: could not load '%S' CSY in Dealer thread due to %d"), &KCSYName, ret);
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	else
		INFO_PRINTF2(_L("OK. '%S' CSY loaded"), &KCSYName);

	_LIT(KCSYName1, "DummyCsy17");
	TName name;
	name.Copy(KCSYName1);
	TSerialInfo protocolInfo;

	//getting the info of the each CSY loaded
	TInt result = iCommSession.GetPortInfo(name,protocolInfo);
	if(result != KErrNone)
		{
		INFO_PRINTF1(_L("ERROR: Getting a port info after loading Comms module\n"));
		INFO_PRINTF2(_L("ERROR: GetPortInfo returned %d"), result);
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	else
		{
		INFO_PRINTF2(_L("description:    %S"), &protocolInfo.iDescription);
		INFO_PRINTF2(_L("port name  :    %S"), &protocolInfo.iName);
		INFO_PRINTF2(_L("low unit   :    %d"), protocolInfo.iLowUnit);
		INFO_PRINTF2(_L("high unit  :    %d"), protocolInfo.iHighUnit);
		}

	return TestStepResult();
	}


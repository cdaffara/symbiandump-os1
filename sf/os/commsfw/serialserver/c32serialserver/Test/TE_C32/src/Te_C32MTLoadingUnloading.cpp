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

// Test case id NET-C32-I-001-HP
// Loading a CSY in dealer thread
/**
 * Constructor
 */
CTestLoadDealerCSY::CTestLoadDealerCSY()
	{
	SetTestStepName(_L("TestLoadDealerCSY"));
	}
/**
 * @see TestLoadDealerCSY test case NET-C32-I-001-HP
 *
 * doTestStep function does the below action
 * Connect to a C32 server
 * Load the csy in  Dealer thread
 * close the connection to C32
 */
TVerdict CTestLoadDealerCSY::doTestStepL()
	{

	INFO_PRINTF1(_L("================================================"));
	INFO_PRINTF1(_L("   Test :PREQ890 C32 Load CSY in Dealer thread  "));
	INFO_PRINTF1(_L("================================================"));

	INFO_PRINTF1(_L("Loading CSY in C32 component"));
	_LIT(KCSYName,"LOOPBACK");

    //Loading the communication system(CSY)
	TInt ret = iCommSession.LoadCommModule(KCSYName);
	if(ret != KErrNone)
		{
		INFO_PRINTF3(_L("ERROR: could not load '%S' CSY in Dealer thread due to %d"), &KCSYName, ret);
		SetTestStepResult(EFail);
		}
	else
		{
		INFO_PRINTF2(_L("OK. '%S' CSY loaded"), &KCSYName);
		}

	return TestStepResult();
	}


// Test case id NET-C32-I-002-HP
// Load CSY in new player thread
/**
 * Constructor
 */
CTestLoadPlayerCSY::CTestLoadPlayerCSY()
	{
	SetTestStepName(_L("TestLoadPlayerCSY"));
	}

/**
 * @see CTestLoadPlayerCSY test case NET-C32-I-002-HP
 *
 * doTestStep function does the below action
 * Connect to a C32 server
 * Load  csy in  Player thread OnDemand=1
 * close the connection to C32
 */
TVerdict CTestLoadPlayerCSY::doTestStepL()
	{
	INFO_PRINTF1(_L("================================================"));
	INFO_PRINTF1(_L("Test :PREQ890 C32 Load CSY in new player thread"));
	INFO_PRINTF1(_L("==============================================="));

	INFO_PRINTF1(_L("Loading DummyCsy1 CSY in C32 component"));
	_LIT(KCSYName,"DummyCsy1");

    //Loading the communication system(CSY)
	TInt ret = iCommSession.LoadCommModule(KCSYName);
	if(ret != KErrNone)
		{
		INFO_PRINTF3(_L("ERROR: could not load '%S' CSY in new (OnDemad =1) Player thread due to %d"), &KCSYName, ret);
		SetTestStepResult(EFail);
		}
	else
		{
		INFO_PRINTF2(_L("OK. '%S' CSY loaded"), &KCSYName);
		}

	return TestStepResult();
	}


// Test case id NET-C32-I-003-HP
// Loading a CSY in the existing player thread

/**
 * Constructor
 */
CTestLoadCSYthread::CTestLoadCSYthread()
	{
	SetTestStepName(_L("TestLoadCSYthread"));
	}

/**
 * @see CTestLoadCSYthread test case NET-C32-I-003-HP
 *
 * doTestStep function does the below action
 * Connect to a C32 server
 * Load the csy in  Player thread OnDemand=0
 * close the connection to C32
 */
TVerdict CTestLoadCSYthread::doTestStepL()
	{

	SetTestStepResult(EPass);

 	INFO_PRINTF1(_L("===================================================="));
	INFO_PRINTF1(_L("Test :PREQ890 C32 Load CSY in existing player thread"));
	INFO_PRINTF1(_L("===================================================="));

	INFO_PRINTF1(_L("Loading Dummy CSY in C32 component"));
	_LIT(KCSYName,"DummyCsy9");

	//Loading the communication system(CSY)
	TInt ret = iCommSession.LoadCommModule(KCSYName);
	if(ret != KErrNone)
		{
		INFO_PRINTF3(_L("ERROR: could not load '%S' CSY in existing player (OnDemad =0)thread due to %d"), &KCSYName, ret);
		SetTestStepResult(EFail);
		}
	else
		{
		INFO_PRINTF2(_L("OK. '%S' CSY loaded"), &KCSYName);
		}

	return TestStepResult();
	}


// Test case id NET-C32-I-004-HP
// Loading more than one CSYs
/**
 * Constructor
 */
CTestLoadMoreCSY::CTestLoadMoreCSY()
	{
	SetTestStepName(_L("TestLoadMoreCSY"));
	}
/**
 * @see CTestLoadMoreCSY test case NET-C32-I-004-HP
 *
 * doTestStep function does the below action
 * Connect to a C32 server
 * Load more than one csy in the existing player thread
 * close the connection to C32
 */
TVerdict CTestLoadMoreCSY::doTestStepL()
	{

	SetTestStepResult(EPass);

	INFO_PRINTF1(_L("========================================="));
	INFO_PRINTF1(_L("Test : PREQ890 C32 Load more than one CSY"));
	INFO_PRINTF1(_L("========================================="));

	INFO_PRINTF1(_L("Loading DummyCsy1 CSY in C32 component"));
	_LIT(KCSYName1,"DummyCsy1");
	_LIT(KCSYName2,"DummyCsy2");

	//Loading the communication system
	TInt ret = iCommSession.LoadCommModule(KCSYName1);
	if(ret != KErrNone)
		{
		INFO_PRINTF3(_L("ERROR: could not load '%S' CSY in the existing player thread due to %d"), &KCSYName1, ret);
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	else
		{
		INFO_PRINTF2(_L("OK. '%S' CSY loaded"), &KCSYName1);
		}

	INFO_PRINTF1(_L("Loading DummyCsy2 CSY in C32 component"));

	//Loading  communication system( CSY )
	ret = iCommSession.LoadCommModule(KCSYName2);
	if(ret != KErrNone)
		{
		INFO_PRINTF3(_L("ERROR: could not load '%S' CSY in the existing player thread due to %d"), &KCSYName2, ret);
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	else
		{
		INFO_PRINTF2(_L("OK. '%S' CSY loaded"), &KCSYName2);
		}

	return TestStepResult();
	}

//Test case ID NET-C32-I-005-HP
//UnLoading CSY in player thread and make a API call for other CSY in the same thread
/**
 * Constructor
 */
CTestApiCallCSY::CTestApiCallCSY()
	{
	SetTestStepName(_L("TestApiCallCSY"));
	}

/**
 * @see CTestLoadMoreCSY test case NET-C32-I-005-HP
 *
 * doTestStep function does the below action
 * Connect to a C32 server
 * Load,Unlload and call getportinfo API
 * close the connection to C32
 */
TVerdict CTestApiCallCSY::doTestStepL()
	{

	SetTestStepResult(EPass);

	INFO_PRINTF1(_L("==========================================================================="));
	INFO_PRINTF1(_L("Test : PREQ890 Unload the CSY and make a API call to CSY of the same thread"));
	INFO_PRINTF1(_L("==========================================================================="));

	_LIT(KCSYName,"DummyCsy1");
	_LIT(KCSYName1,"DummyCsy2");

	//Loading communication system(CSY)
	TInt ret = iCommSession.LoadCommModule(KCSYName);
	if(ret != KErrNone)
		{
		INFO_PRINTF3(_L("ERROR: could not load '%S' CSY due to %d"), &KCSYName, ret);
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	else
		{
		INFO_PRINTF2(_L("OK. '%S' CSY loaded"), &KCSYName);
		}

	//Loading communication system(CSY)
	TInt ret1 = iCommSession.LoadCommModule(KCSYName1);
	if(ret1 != KErrNone)
		{
		INFO_PRINTF3(_L("ERROR: could not load '%S' CSY due to %d"), &KCSYName1, ret1);
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	else
		{
		INFO_PRINTF2(_L("OK. '%S' CSY loaded"), &KCSYName1);
		}

	//Unloading communication system(CSY)
	TInt retValue = iCommSession.UnloadCommModule(KCSYName);
	if(retValue  != KErrNone)
		{
		INFO_PRINTF3(_L("ERROR: could not unload CSY '%S' due to %d"), &KCSYName, retValue);
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	else
		{
		INFO_PRINTF2(_L("OK. '%S' CSY  unloaded"), &KCSYName);
		}

	//Getting the port information
	TSerialInfo portInfo;
	TInt ret2 = iCommSession.GetPortInfo (KCSYName1, portInfo);
	if(ret2  != KErrNone)
		{
		INFO_PRINTF3(_L("ERROR: API call to  CSY  GetPortInfo '%S' due to %d"), &KCSYName1, ret2);
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	else
		{
		INFO_PRINTF2(_L("OK. API call to '%S' CSY  is sucessfull"), &KCSYName1);
		}

	return TestStepResult();
	}


//Test case id NET-C32-I-006-HP
//Loading the Unknown CSY
/**
 * Constructor
 */
CTestLoadUnknownCSY::CTestLoadUnknownCSY()
	{
	SetTestStepName(_L("TestLoadingunknowCSY"));
	}

/**
 * @see CTestLoadUnknownCSY test case NET-C32-I-006-HP
 * doTestStep function does the below action
 * Connect to a C32 server
 * Loading,an unknow CSY ,in CMI file CSYLIST will not contain DummyCsy1
 * close the connection to C32
 */

TVerdict CTestLoadUnknownCSY::doTestStepL()
	{

	SetTestStepResult(EPass);

	INFO_PRINTF1(_L("===================================="));
	INFO_PRINTF1(_L("Test: PREQ890 UnKnown Csy is loading"));
	INFO_PRINTF1(_L("===================================="));

	//Load Unknown CSY
    _LIT(KCSYName,"DummyCsy1");

    //Loading communication system(CSY)
	TInt ret = iCommSession.LoadCommModule(KCSYName);
	if(ret != KErrNone)
		{
		INFO_PRINTF3(_L("ERROR: could not load Unknown CSY '%S' due to %d"), &KCSYName, ret);
		SetTestStepResult(EFail);
		}
	else
		{
		INFO_PRINTF2(_L("OK. '%S' CSY loaded"), &KCSYName);
		}

    return TestStepResult();
	}


// Test case id NET-C32-I-009-HP
// Loading and Unloading the CSY in Dealer and Player thread in same order
/**
 * Constructor
 */
CTestLoadUnLoadCsySameOrder::CTestLoadUnLoadCsySameOrder()
	{
	SetTestStepName(_L("TestLoadUnLoadCsySameOrder"));
	}

/**
 * @see CTestLoadUnLoadCsySameOrder test case NET-C32-I-009-HP
 *
 * doTestStep function does the below action
 * Connect to a C32 server
 * Loading,and Unloading a CSY in Dealer and player thread
 * close the connection to C32
 */
TVerdict CTestLoadUnLoadCsySameOrder::doTestStepL()
	{

	SetTestStepResult(EPass);

	INFO_PRINTF1(_L("==================================================="));
	INFO_PRINTF1(_L("Test :PREQ890 C32 Load and UnLoad CSY in same order"));
	INFO_PRINTF1(_L("==================================================="));

	_LIT(KCSYName,"LOOPBACK.CSY");
	_LIT(KCSYName1,"Loopback");
	_LIT(KCSYName2,"DummyCsy1");
	_LIT(KCSYName3,"DummyCsy2");
	_LIT(KCSYName4,"DummyCsy3");
	_LIT(KCSYName5,"DummyCsy4");

    //Loading communication system(CSY)
	TInt retValue = iCommSession.LoadCommModule(KCSYName);
	//Loading communication system(CSY)
	TESTCHECKL(retValue,  KErrNone);
	//Loading communication system(CSY)
	TInt retValue2 = iCommSession.LoadCommModule(KCSYName2);
	TESTCHECKL(retValue2, KErrNone);
	//Loading communication system(CSY)
	TInt retValue3 = iCommSession.LoadCommModule(KCSYName3);
	TESTCHECKL(retValue3, KErrNone);
	//Loading communication system(CSY)
	TInt retValue4 = iCommSession.LoadCommModule(KCSYName4);
	TESTCHECKL(retValue4, KErrNone);
	//Loading communication system(CSY)
	TInt retValue5 = iCommSession.LoadCommModule(KCSYName5);
	TESTCHECKL(retValue5, KErrNone);

    if (retValue == KErrNone && retValue2 == KErrNone && retValue3 == KErrNone && retValue4 == KErrNone && retValue5 == KErrNone)
		{
		INFO_PRINTF1(_L("Loading CSYs successfully"));
		}
	else
		{
		INFO_PRINTF1(_L("Loading CSYs Unsuccessfully"));
		SetTestStepResult(EFail);
		return TestStepResult();
		}

	//Loading communication system(CSY)
	TInt retValue1 = iCommSession.UnloadCommModule(KCSYName1);
	TESTCHECKL(retValue1, KErrNone);
	//Loading communication system(CSY)
	retValue2 = iCommSession.UnloadCommModule(KCSYName2);
	TESTCHECKL(retValue2,  KErrNone);
	//Loading communication system(CSY)
	retValue3 = iCommSession.UnloadCommModule(KCSYName3);
	TESTCHECKL(retValue3,  KErrNone);
	//Loading communication system(CSY)
	retValue4 = iCommSession.UnloadCommModule(KCSYName4);
	TESTCHECKL(retValue4, KErrNone);
	//Loading communication system(CSY)
	retValue5 = iCommSession.UnloadCommModule(KCSYName5);
	TESTCHECKL(retValue5, KErrNone);

	if ((retValue1 == KErrNone)&&(retValue2 == KErrNone)&&(retValue2 == KErrNone) && (retValue3 == KErrNone) && (retValue4 == KErrNone )&&(retValue5 == KErrNone))
		{
		INFO_PRINTF1(_L("UnLoading CSYs successfully"));
		}
	else
		{
		INFO_PRINTF1(_L("UnLoading CSYs Unsuccessfully"));
		SetTestStepResult(EFail);
		}

	return TestStepResult();
	}



// Test case id NET-C32-I-0010-HP
// Loading and Unloading the CSY in Dealer and Player thread in Different order
/**
 * Constructor
 */
CTestLoadUnLoadCsyDiffOrder::CTestLoadUnLoadCsyDiffOrder()
	{
	SetTestStepName(_L("TestLoadUnLoadCsyDiffOrder"));
	}

/**
 * @see  CTestLoadUnLoadCsySameOrder test case NET-C32-I-0010-HP
 *
 * doTestStep function does the below action
 * Connect to a C32 server
 * Loading,and Unloading a CSY in Dealer and player thread in different order
 * close the connection to C32
 */
TVerdict CTestLoadUnLoadCsyDiffOrder::doTestStepL()
	{

	SetTestStepResult(EPass);

	INFO_PRINTF1(_L("==================================================="));
	INFO_PRINTF1(_L("Test :PREQ890 C32 Load and unload CSY in Diff order"));
	INFO_PRINTF1(_L("==================================================="));

	_LIT(KCSYName,"LOOPBACK.CSY");
	_LIT(KCSYName1,"Loopback");
	_LIT(KCSYName2,"DummyCsy1");
	_LIT(KCSYName3,"DummyCsy2");
	_LIT(KCSYName4,"DummyCsy3");
	_LIT(KCSYName5,"DummyCsy4");


	//Loading communication system(CSY)
	TInt retValue = iCommSession.LoadCommModule(KCSYName);
	TESTCHECKL(retValue,  KErrNone);

	//Loading communication system(CSY)
	TInt retValue2 = iCommSession.LoadCommModule(KCSYName2);
	TESTCHECKL(retValue2, KErrNone);
	//Loading communication system(CSY)
	TInt retValue3 = iCommSession.LoadCommModule(KCSYName3);
	TESTCHECKL(retValue3, KErrNone);
	//Loading communication system(CSY)
	TInt retValue4 = iCommSession.LoadCommModule(KCSYName4);
	TESTCHECKL(retValue4, KErrNone);
	//Loading communication system(CSY)
	TInt retValue5 = iCommSession.LoadCommModule(KCSYName5);
	TESTCHECKL(retValue5,  KErrNone);

	if ((retValue == KErrNone) &&  (retValue2 == KErrNone) && (retValue3 == KErrNone)&& (retValue4 == KErrNone) && (retValue5 == KErrNone))
		{
		INFO_PRINTF1(_L("Loading CSYs successfully"));
		}
	else
		{
		INFO_PRINTF1(_L("Loading CSYs Unsuccessfully"));
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	//UnLoading communication system(CSY)
	retValue5 = iCommSession.UnloadCommModule(KCSYName5);
	TESTCHECKL(retValue5, KErrNone);
	//UnLoading communication system(CSY)
	retValue4 = iCommSession.UnloadCommModule(KCSYName4);
	TESTCHECKL(retValue4, KErrNone);
	//UnLoading communication system(CSY)
	retValue3 = iCommSession.UnloadCommModule(KCSYName3);
	TESTCHECKL(retValue3, KErrNone);
	//UnLoading communication system(CSY)
	retValue2 = iCommSession.UnloadCommModule(KCSYName2);
	TESTCHECKL(retValue2, KErrNone);
	//UnLoading communication system(CSY)
	TInt retValue1 = iCommSession.UnloadCommModule(KCSYName1);
	TESTCHECKL(retValue1, KErrNone);


	if ((retValue1 == KErrNone) && (retValue2 == KErrNone) && (retValue3 == KErrNone) && (retValue4 == KErrNone) && (retValue5 == KErrNone))
		{
		INFO_PRINTF1(_L("UnLoading CSYs successfully"));
		}
	else
		{
		INFO_PRINTF1(_L("UnLoading CSYs Unsuccessfully"));
		SetTestStepResult(EFail);
		}
	return TestStepResult();
	}


// Test case id NET-C32-I-0014-HP
//Loading a two CSY from different session
/**
 * Constructor
 */
CLoadingCsydiffSession::CLoadingCsydiffSession()
	{
	SetTestStepName(_L("LoadingCsydiffSession"));
	}

TInt CLoadingCsydiffSession::Session(TAny* aArg)
/**
 * Thread which attempts Load CSY from different session
 */
	{
	CLoadingCsydiffSession* stepTest = reinterpret_cast<CLoadingCsydiffSession*> (aArg);

	_LIT(KCSYName,"DummyCsy1");

	//Creating a RcommSession, connecting to the C32 server
	RCommServ commServ;
	TInt res=commServ.Connect();
	if(res != KErrNone)
		{
		stepTest->iState1 = __LINE__ - 2;
		return(res);
		}
	//Loading the communication system(CSY)
	res=commServ.LoadCommModule(KCSYName);
	if(res != KErrNone)
		{
		stepTest->iState2 = __LINE__ - 2;
		commServ.Close();
		return(res);
		}
	//Closing the RcommServ handle
	commServ.Close();
	return KErrNone;
	}
/**
 * @see CLoadingCsydiffSession test case NET-C32-I-0014-HP
 *
 * doTestStep function does the below action
 * Connect to a C32 server
 * Loading,diff CSY from different session
 * close the connection to C32
 */
TVerdict CLoadingCsydiffSession::doTestStepL()
	{

	SetTestStepResult(EPass);

	INFO_PRINTF1(_L("==================================="));
	INFO_PRINTF1(_L("Loading 2 csy from different session"));
	INFO_PRINTF1(_L("==================================="));

	_LIT(KCSYName,"DummyCsy3");
	//Creating thread to load the CSY in different session
	RThread sessionThread;
	TInt res=sessionThread.Create(_L("Session"), Session, KDefaultStackSize, KDefaultHeapSizeThread, KDefaultHeapSizeThread, this);
	if(res != KErrNone)
		{
		INFO_PRINTF2(_L("Load Comm module in session 2 failed %d"), res);
		SetTestStepResult(EFail);
		return TestStepResult();
		}

	//Requests notification when thread dies
	TRequestStatus stat;
	sessionThread.Logon(stat);
	//Makes the thread eligible for execution.
	sessionThread.Resume();

	//Loading the communication system(CSY)
	TInt ret=iCommSession.LoadCommModule(KCSYName);
	if(ret != KErrNone)
		{
		INFO_PRINTF2(_L("Load Comm module in session 1 failed %d"), ret);

		sessionThread.Kill(KErrNone);
		User::WaitForRequest(stat);
		CLOSE_AND_WAIT(sessionThread);

		SetTestStepResult(EFail);

		return TestStepResult();
		}
	else
		{
		INFO_PRINTF2(_L("OK. '%S' CSY loaded"), &KCSYName);
		}
	
	//Closing the thread
	User::WaitForRequest(stat);
	TESTCHECKL(stat.Int(), KErrNone);
	CLOSE_AND_WAIT(sessionThread);

	//Checking error condition for thread
	if(iState1 != KErrNone)
		{
		INFO_PRINTF2(_L("Error occured, iState1, in session 2 at line %d, expected KErrNone."), iState1);
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	//Checking error condition for thread
	if(iState2 != KErrNone)
		{
		INFO_PRINTF2(_L("Error occured, iState2, in session 2 at line %d, expected KErrNone."), iState2);
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	
	return TestStepResult();

	}

// Test case id NET-C32-I-0015-HP
//Loading a same CSY from different session
/**
 * Constructor
 */
CLoadingSameCsydiffSession::CLoadingSameCsydiffSession()
	{
	SetTestStepName(_L("LoadingSameCsydiffSession"));
	}

TInt CLoadingSameCsydiffSession::Session(TAny* aArg)
/**
 * Thread which attempts Load same CSY from different session
 */
	{

	CLoadingSameCsydiffSession*	 stepTest = reinterpret_cast<CLoadingSameCsydiffSession*> (aArg);
	_LIT(KCSYName,"DummyCsy1");

	//Creating a RcommSession, connecting to the C32 server
	RCommServ commServ;
	TInt res=commServ.Connect();
	if(res != KErrNone)
		{
		stepTest->iState1 = __LINE__ - 2;
		return(res);
		}
	//Loading the communication system(CSY)
	res=commServ.LoadCommModule(KCSYName);
	if(res != KErrNone)
		{
		stepTest->iState2 = __LINE__ - 2;
		commServ.Close();
		return(res);
		}
	//Closing the RcommServ handle
	commServ.Close();
	return KErrNone;
	}
/**
 * @see CLoadingSameCsydiffSession test case NET-C32-I-0015-HP
 *
 * doTestStep function does the below action
 * Connect to a C32 server
 * Loading,same CSY from different session
 * close the connection to C32
 */
TVerdict CLoadingSameCsydiffSession::doTestStepL()
	{

	SetTestStepResult(EPass);

	INFO_PRINTF1(_L("======================================="));
	INFO_PRINTF1(_L("Loading same csy from different session"));
	INFO_PRINTF1(_L("======================================="));

	_LIT(KCSYName,"DummyCsy1");
	//Creating thread to load the CSY in different session
	RThread sessionThread;
	TInt res=sessionThread.Create(_L("Session"), Session, KDefaultStackSize, KDefaultHeapSizeThread, KDefaultHeapSizeThread, this);
	if(res != KErrNone)
		{
		INFO_PRINTF2(_L("Load Comm module in session 2 failed %d"), res);
		SetTestStepResult(EFail);
		return TestStepResult();
		}

	//Requests notification when thread dies
	TRequestStatus stat;
	sessionThread.Logon(stat);
	//Makes the thread eligible for execution.
	sessionThread.Resume();

	//Loading the communication system(CSY)
	TInt ret=iCommSession.LoadCommModule(KCSYName);
	if(ret != KErrNone)
		{
		INFO_PRINTF2(_L("Load Comm module in session 1 failed %d"), ret);
		SetTestStepResult(EFail);

		sessionThread.Kill(KErrNone);
		User::WaitForRequest(stat);
		CLOSE_AND_WAIT(sessionThread);

		return TestStepResult();
		}
	else
		{
		INFO_PRINTF2(_L("OK. '%S' CSY loaded"), &KCSYName);
		}
	
	//Closing the thread handler
	User::WaitForRequest(stat);
	TESTCHECKL(stat.Int(), KErrNone);
	CLOSE_AND_WAIT(sessionThread);

	//Checking error condition for thread
	if(iState1 != KErrNone)
		{
		INFO_PRINTF2(_L("Error occured, iState1, in session 2 at line %d, expected KErrNone."), iState1);
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	//Checking error condition for thread
	if(iState2 != KErrNone)
		{
		INFO_PRINTF2(_L("Error occured, iState2, in session 2 at line %d, expected KErrNone."), iState2);
		SetTestStepResult(EFail);
		return TestStepResult();
		}

	return TestStepResult();

	}


// Test case id NET-C32-I-0020-HP
// Loading a CSY with old CMi files
/**
 * Constructor
 */
CLoadCsywithOldCMI::CLoadCsywithOldCMI()
	{
	SetTestStepName(_L("LoadCsywithOldCMI"));
	}
/**
 * @see CLoadCsywithOldCMI test case NET-C32-I-0020-HP
 *
 * doTestStep function does the below action
 * Connect to a C32 server
 * Loading the csy with old CMI files
 * close the connection to C32
 */
TVerdict CLoadCsywithOldCMI::doTestStepL()
	{

	SetTestStepResult(EPass);
    
    INFO_PRINTF1(_L("================================================"));
	INFO_PRINTF1(_L("Test :   PREQ890 C32 Load CSY with old CMI file"));
	INFO_PRINTF1(_L("==============================================="));

	INFO_PRINTF1(_L("Loading CSY in C32 component"));
	_LIT(KCSYName,"DummyCsy1");
	_LIT(KCSYName1,"DummyCsy2");

    //Loading the communication system(CSY)
	TInt ret = iCommSession.LoadCommModule(KCSYName);
	if(ret != KErrNone)
		{
		INFO_PRINTF3(_L("ERROR: could not load '%S' CSY in single threaded cmi files to %d"), &KCSYName, ret);
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	else
		{
		INFO_PRINTF2(_L("OK. '%S' CSY loaded"), &KCSYName);
		}

	ret = iCommSession.LoadCommModule(KCSYName1);
	if(ret != KErrNone)
		{
		INFO_PRINTF3(_L("ERROR: could not load '%S' CSY in single threaded cmi files to %d"), &KCSYName1, ret);
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	else
		{
		INFO_PRINTF2(_L("OK. '%S' CSY loaded"), &KCSYName1);
		}

	return TestStepResult();
	}


// Test case id NET-C32-I-0021-HP
// Loading a CSY in SingleThreaded and Multiple threaded CMI files
// checking for the backward compatability by putting old and new CMI files in privat directory

/**
 * Constructor
 */
COldandNewCMIfiles::COldandNewCMIfiles()
	{
	SetTestStepName(_L("OldandNewCMIfiles"));
	}
/**
 * @see COldandNewCMIfiles test case NET-C32-I-0021-HP
 *
 * doTestStep function does the below action
 * Connect to a C32 server
 * Load the csy in player thread
 * close the connection to C32
 */

TVerdict COldandNewCMIfiles::doTestStepL()
	{

	SetTestStepResult(EPass);

	INFO_PRINTF1(_L("============================================================================="));
	INFO_PRINTF1(_L("  Loading csy in player thread which had old and new cmi files in private dir"));
	INFO_PRINTF1(_L("============================================================================="));

	_LIT(KCSYName,"DummyCsy1.CSY");
	_LIT(KCSYName1,"DummyCsy2.CSY");

	//Loading the communication system(CSY)
	TInt ret = iCommSession.LoadCommModule(KCSYName);
	if(ret != KErrNone)
		{
		INFO_PRINTF2(_L("ERROR: Load Comm module failed %d"), ret);
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	else
		{
		INFO_PRINTF2(_L("OK. '%S' CSY loaded"), &KCSYName);
		}

	//Loading the communication system(CSY)
	ret=iCommSession.LoadCommModule(KCSYName1);
	if(ret != KErrNone)
		{
		INFO_PRINTF2(_L("ERROR: Load Comm module failed %d"), ret);
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	else
		{
		INFO_PRINTF2(_L("OK. '%S' CSY loaded"), &KCSYName1);
		}

	return TestStepResult();

	}


//Test case id NET-C32-I-0022-HP
//Loading a CSY in SingleThreaded and Multiple threaded CMI files
//There should be minimum of two CMI file for Loading CSY in player thread one for dealer
//thread and other for player thread
/**
 * Constructor
 */
CLoadCsyPlayerThread::CLoadCsyPlayerThread()

	{
	SetTestStepName(_L("LoadCsyPlayerThread"));
	}

/**
 * @see CLoadCsyPlayerThread test case NET-C32-I-0022-HP
 *
 * doTestStep function does the below action
 * Connect to a C32 server
 * Load the csy in player thread
 * close the connection to C32
 */
TVerdict CLoadCsyPlayerThread::doTestStepL()
	{

	SetTestStepResult(EPass);

	INFO_PRINTF1(_L("==================================="));
	INFO_PRINTF1(_L("Loading 2 csy into player thread   "));
	INFO_PRINTF1(_L("==================================="));

	_LIT(KCSYName,"DummyCsy3.CSY");
	_LIT(KCSYName1,"DummyCsy4.CSY");

	//Loading the communication system(CSY)
	TInt ret = iCommSession.LoadCommModule(KCSYName);
	if(ret != KErrNone)
		{
		INFO_PRINTF2(_L("ERROR: Load Comm module failed %d"), ret);
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	else
		{
		INFO_PRINTF2(_L("OK. '%S' CSY loaded"), &KCSYName);
		}
	//Loading the communication system(CSY)
	ret=iCommSession.LoadCommModule(KCSYName1);
	if(ret != KErrNone)
		{
		INFO_PRINTF2(_L("ERROR: Load Comm module  failed %d"), ret);
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	else
		{
		INFO_PRINTF2(_L("OK. '%S' CSY loaded"), &KCSYName1);
		}

	return TestStepResult();

	}


//Test case id NET-C32-I-0023-HP
//Loading a CSY both Dealer and player thread,
//mentioned CMI files should be available in private directory
//old cmi file and new cmi files mentioned Role=Dealer and Role=Player

/**
 * Constructor
 */
CLoadCsyDealerPlayerThreads::CLoadCsyDealerPlayerThreads()

	{
	SetTestStepName(_L("LoadCsyDealerPlayerThreads"));
	}

/**
 * @see CLoadCsyDealerPlayerThreads test case NET-C32-I-0023-HP
 *
 * doTestStep function does the below action
 * Connect to a C32 server
 * Load the csy into player and Dealer thread
 * close the connection to C32
 */
TVerdict CLoadCsyDealerPlayerThreads::doTestStepL()
	{

	SetTestStepResult(EPass);

	INFO_PRINTF1(_L("==================================="));
	INFO_PRINTF1(_L("Loading 2 csy into player thread   "));
	INFO_PRINTF1(_L("==================================="));

	_LIT(KCSYName,"LOOPBACK.CSY");
	_LIT(KCSYName1,"DummyCsy1.CSY");

	//Loading the communication system(CSY)
	TInt ret = iCommSession.LoadCommModule(KCSYName);
	if(ret != KErrNone)
		{
		INFO_PRINTF2(_L("ERROR: Load Comm module failed %d"), ret);
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	else
		{
		INFO_PRINTF2(_L("OK. '%S' CSY loaded"), &KCSYName);
		}
	//Loading the communication system(CSY)
	ret=iCommSession.LoadCommModule(KCSYName1);
	if(ret != KErrNone)
		{
		INFO_PRINTF2(_L("ERROR: Load Comm module failed %d"), ret);
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	else
		{
		INFO_PRINTF2(_L("OK. '%S' CSY loaded"), &KCSYName1);
		}

	return TestStepResult();

	}


//Test case id NET-C32-I-0024-HP
//Loading and Unloading a CSY from different session
//Mentioned CMi file in the testcase should be available in private directory
/**
 * Constructor
 */
CLoadandUnloadDiffSession::CLoadandUnloadDiffSession()

	{
	SetTestStepName(_L("LoadandUnloadDiffSession"));
	}

/**
 * @see CLoadandUnloadDiffSession test case NET-C32-I-0024-HP
 *
 * Session function does the below action
 * Connect to a C32 server
 * Unload the CSY from different session which is loaded from other session
 * (should fail with KErrNotFound)
 * close the connection to C32
 */
TInt CLoadandUnloadDiffSession::Session(TAny* aArg)
/**
 * Thread which attempts get number of CSY loaded from other session
 */
	{

	CLoadandUnloadDiffSession	*stepTest = reinterpret_cast<CLoadandUnloadDiffSession*> (aArg);
	_LIT(KCSYName,"Loopback");

	//Creating a RcommSession, connecting to the C32 server
	RCommServ commSession;
	TInt res = commSession.Connect();
	if(res != KErrNone)
		{
		stepTest->iState1 = __LINE__ - 2;
		return(res);
		}

	res = commSession.UnloadCommModule(KCSYName);
	if(	res == KErrNotFound)
		{
		stepTest->iState2 = __LINE__ - 2;
		commSession.Close();
		return(res);
 		}

	commSession.Close();
	return KErrNone;
	}
/**
 * @see CLoadandUnloadDiffSession test case NET-C32-I-0024-HP
 *
 * doTestStep function does the below action
 * Connect to a C32 server
 * Load the csy in thread
 * close the connection to C32
 */
TVerdict CLoadandUnloadDiffSession::doTestStepL()
	{

	SetTestStepResult(EPass);

	INFO_PRINTF1(_L("============================================"));
	INFO_PRINTF1(_L("Loading and unloading csy from diff session "));
	INFO_PRINTF1(_L("============================================"));
	_LIT(KCSYName,"LOOPBACK.CSY");

	RThread sessionThread;
	TInt res = sessionThread.Create(_L("SessionThr"), Session, KDefaultStackSize, KDefaultHeapSizeThread, KDefaultHeapSizeThread, this);
	if(res != KErrNone)
		{
		INFO_PRINTF2(_L("Load Comm module in session 2 failed %d"), res);
		SetTestStepResult(EFail);
		return TestStepResult();
		}

	//Requests notification when thread dies
	TRequestStatus stat;
	sessionThread.Logon(stat);

	//Loading the communication system(CSY)
	TInt ret = iCommSession.LoadCommModule(KCSYName);
	if(ret != KErrNone)
		{
		INFO_PRINTF2(_L("Load Comm module in session 1 failed %d"), ret);

		sessionThread.Kill(KErrNone);
		User::WaitForRequest(stat);
		CLOSE_AND_WAIT(sessionThread);

		SetTestStepResult(EFail);

		return TestStepResult();
		}
	else
		{
		INFO_PRINTF2(_L("OK. '%S' CSY loaded"), &KCSYName);
		}

	//Makes the thread eligible for execution.
	sessionThread.Resume();

	//Closing the thread handler
	User::WaitForRequest(stat);
	CLOSE_AND_WAIT(sessionThread);

	//Checking error condition for thread
	if(iState1 != KErrNone)
		{
		INFO_PRINTF2(_L("Error occured, iState1, in session 2 at line %d, expected KErrNone."), iState1);
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	//Checking error condition for thread
	 if(iState2 != KErrNotFound)
		{
		SetTestStepResult(EPass);
		}
   	
   	return TestStepResult();

	}


// Test case id NET-C32-I-0034-HP
// Loading a CSYs in all Player Threads (set all 8 cmi files OnDemand=1)
/**
 * Constructor
 */
CLoadCsyAllPlayerThread::CLoadCsyAllPlayerThread()
	{
	SetTestStepName(_L("LoadCsyAllPlayerThread"));
	}
/**
 * @see CTestLoadDealerCSY test case NET-C32-I-0034
 *
 * doTestStep function does the below action
 * Connect to a C32 server
 * Load csy in  Different Player thread
 * close the connection to C32
 */
TVerdict CLoadCsyAllPlayerThread::doTestStepL()
	{
	SetTestStepResult(EPass);

	INFO_PRINTF1(_L("================================================"));
	INFO_PRINTF1(_L("Test :PREQ890 C32 Load CSY in Dealer thread"));
	INFO_PRINTF1(_L("==============================================="));

	INFO_PRINTF1(_L("Loading CSY in C32 component"));
	_LIT(KCSYName1,"DummyCsy1");
	_LIT(KCSYName2,"DummyCsy3");
	_LIT(KCSYName3,"DummyCsy4");
	_LIT(KCSYName4,"DummyCsy5");
	_LIT(KCSYName5,"DummyCsy6");
	_LIT(KCSYName6,"DummyCsy7");
	_LIT(KCSYName7,"DummyCsy8");
	_LIT(KCSYName8,"DummyCsy9");

    //Loading the communication system(CSY)
	TInt ret = iCommSession.LoadCommModule(KCSYName1);
	if(ret != KErrNone)
		{
		INFO_PRINTF3(_L("ERROR: could not load '%S' CSY in Dealer thread due to %d"), &KCSYName1, ret);
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	else
		INFO_PRINTF2(_L("OK. '%S' CSY loaded"), &KCSYName1);

	//Loading the communication system(CSY)
	ret = iCommSession.LoadCommModule(KCSYName2);
	if(ret != KErrNone)
		{
		INFO_PRINTF3(_L("ERROR: could not load '%S' CSY in Dealer thread due to %d"), &KCSYName2, ret);
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	else
		{
		INFO_PRINTF2(_L("OK. '%S' CSY loaded"), &KCSYName2);
		}

	//Loading the communication system(CSY)
	ret = iCommSession.LoadCommModule(KCSYName3);
	if(ret != KErrNone)
		{
		INFO_PRINTF3(_L("ERROR: could not load '%S' CSY in Dealer thread due to %d"), &KCSYName3, ret);
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	else
		{
		INFO_PRINTF2(_L("OK. '%S' CSY loaded"), &KCSYName3);
		}

	//Loading the communication system(CSY)
	ret = iCommSession.LoadCommModule(KCSYName4);
	if(ret != KErrNone)
		{
		INFO_PRINTF3(_L("ERROR: could not load '%S' CSY in Dealer thread due to %d"), &KCSYName4, ret);
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	else
		{
		INFO_PRINTF2(_L("OK. '%S' CSY loaded"), &KCSYName4);
		}

	//Loading the communication system(CSY)
    ret = iCommSession.LoadCommModule(KCSYName5);
	if(ret != KErrNone)
		{
		INFO_PRINTF3(_L("ERROR: could not load '%S' CSY in Dealer thread due to %d"), &KCSYName5, ret);
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	else
		{
		INFO_PRINTF2(_L("OK. '%S' CSY loaded"), &KCSYName5);
		}

	//Loading the communication system(CSY)
	ret = iCommSession.LoadCommModule(KCSYName6);
	if(ret != KErrNone)
		{
		INFO_PRINTF3(_L("ERROR: could not load '%S' CSY in Dealer thread due to %d"), &KCSYName6, ret);
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	else
		{
		INFO_PRINTF2(_L("OK. '%S' CSY loaded"), &KCSYName6);
		}

	//Loading the communication system(CSY)
	ret = iCommSession.LoadCommModule(KCSYName7);
	if(ret != KErrNone)
		{
		INFO_PRINTF3(_L("ERROR: could not load '%S' CSY in Dealer thread due to %d"), &KCSYName7, ret);
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	else
		{
		INFO_PRINTF2(_L("OK. '%S' CSY loaded"), &KCSYName7);
		}

	//Loading the communication system(CSY)
	ret = iCommSession.LoadCommModule(KCSYName8);
	if(ret != KErrNone)
		{
		INFO_PRINTF3(_L("ERROR: could not load '%S' CSY in Dealer thread due to %d"), &KCSYName8, ret);
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	else
		{
		INFO_PRINTF2(_L("OK. '%S' CSY loaded"), &KCSYName8);
		}

	return TestStepResult();
	}


//Test case id NET-C32-I-0028-HP
//Loading a CSY in which no CMI set Wildcard

/**
 * Constructor
 */
CNoCMIfileSetWildcard::CNoCMIfileSetWildcard()

	{
	SetTestStepName(_L("NoCMIfileSetWildcard"));
	}

/**
 * @see CNoCMIfileSetWildcard test case NET-C32-I-0028-HP
 *
 * doTestStep function does the below action
 * Connect to a C32 server
 * Load the csy in Thread
 * close the connection to C32
 */
TVerdict CNoCMIfileSetWildcard::doTestStepL()
	{

	SetTestStepResult(EPass);

	INFO_PRINTF1(_L("==================================="));
	INFO_PRINTF1(_L("No CMI file set Wildcard set      "));
	INFO_PRINTF1(_L("==================================="));

	_LIT(KCSYName,"DummyCsy1.CSY");

	//Loading the communication system(CSY)
	TInt ret = iCommSession.LoadCommModule(KCSYName);
	if(ret != KErrNone)
		{
		INFO_PRINTF2(_L("ERROR: Load Comm module failed %d"), ret);
		SetTestStepResult(EFail);
		}
	else
		{
		INFO_PRINTF2(_L("OK. '%S' CSY loaded"), &KCSYName);
		}

	return TestStepResult();

	}

//Test case id NET-C32-I-0031-HP
//Loading a CSY in which single threaded and multithread CMI files(Role=Dealer)
/**
 * Constructor
 */
COldNewCMIfileRoleDealer::COldNewCMIfileRoleDealer()
	{
	SetTestStepName(_L("OldNewCMIfileRoleDealer"));
	}

/**
 * @see COldNewCMIfileRoleDealer test case NET-C32-I-0031
 *
 * doTestStep function does the below action
 * Connect to a C32 server
 * Load the csy in Thread
 * close the connection to C32
 */
TVerdict COldNewCMIfileRoleDealer::doTestStepL()
	{

	SetTestStepResult(EPass);

	INFO_PRINTF1(_L("======================================================================     "));
	INFO_PRINTF1(_L("               old and new CMI file set Role=Dealer                        "));
	INFO_PRINTF1(_L("==========================================================================="));
	_LIT(KCSYName,"LOOPBACK.CSY");

	TInt ret = iCommSession.LoadCommModule(KCSYName);
	if(ret != KErrNone)
		{
		INFO_PRINTF3(_L("ERROR: could not load '%S' CSY in Dealer thread due to %d"), &KCSYName, ret);
		SetTestStepResult(EFail);
		}
	else
		{
		INFO_PRINTF2(_L("OK. '%S' CSY loaded"), &KCSYName);
		}

	return TestStepResult();

	}

//Test case id NET-C32-I-0030-HP
//Loading a CSY in which two new CMI file have Role=Dealer
/**
 * Constructor
 */
CTwoNewCMIfileRoleDealer::CTwoNewCMIfileRoleDealer()

	{
	SetTestStepName(_L("TwoNewCMIfileRoleDealer"));
	}

/**
 * @see CTwoCMIsameWorkerID test case NET-C32-I-0030-HP
 *
 * doTestStep function does the below action
 * Connect to a C32 server
 * Load the csy in Thread
 * close the connection to C32
 */
TVerdict CTwoNewCMIfileRoleDealer::doTestStepL()
	{

	SetTestStepResult(EPass);

	INFO_PRINTF1(_L("==========================================================================="));
	INFO_PRINTF1(_L("       	Loading CSY in which Two new CMI file set Role=Dealer			    "));
	INFO_PRINTF1(_L("==========================================================================="));
	_LIT(KCSYName,"LOOPBACK.CSY");

    TInt ret = iCommSession.LoadCommModule(KCSYName);
	if(ret != KErrNone)
		{
		INFO_PRINTF3(_L("ERROR: could not load '%S' CSY in Dealer thread due to %d"), &KCSYName, ret);
		SetTestStepResult(EFail);
		}
	else
		{
		INFO_PRINTF2(_L("OK. '%S' CSY loaded"), &KCSYName);
		}

	return TestStepResult();

	}


// Test case id NET-C32-I-0038-HP
// Loading a csy into unknown CPM
/**
 * Constructor
 */
CTestLoadUnknownCPM::CTestLoadUnknownCPM()
	{
	SetTestStepName(_L("TestLoadUnknownCPM"));
	}
/**
 * @see CTestLoadUnknownCPM test case NET-C32-I-0038-HP
 *
 * doTestStep function does the below action
 * Connect to a C32 server
 * Load the csy in  player thread thread
 * close the connection to C32
 */
TVerdict CTestLoadUnknownCPM::doTestStepL()
	{
	SetTestStepResult(EFail);

	INFO_PRINTF1(_L("===================================================== "));
	INFO_PRINTF1(_L("         Test :Trying to Load Unknown CPM             "));
	INFO_PRINTF1(_L("      cmi file mentioned FileName=c32Test.dll         "));
	INFO_PRINTF1(_L("======================================================"));

	INFO_PRINTF1(_L("Loading CSY in C32 component"));
	_LIT(KCSYName,"DUMMY");

    //Loading the communication system(CSY)
	TInt ret = iCommSession.LoadCommModule(KCSYName);
	if(ret == KErrNoMemory)
		{
		INFO_PRINTF3(_L("ERROR: could not load '%S' CPM due to %d"), &KCSYName, ret);
		SetTestStepResult(EPass);
		}
	else
		{
		INFO_PRINTF2(_L("OK. '%S' CSY loaded"), &KCSYName);
		}

	return TestStepResult();
	}

// Test case id NET-C32-I-0037-HP
// Loading a CSY having long port name.
/**
 * Constructor
 */
CTestLongPortName::CTestLongPortName()
	{
	SetTestStepName(_L("TestLongPortName"));
	}
/**
 * @see CTestLoadDealerCSY test case NET-C32-I-0038-HP
 *
 * doTestStep function does the below action
 * Connect to a C32 server
 * Load ,unload and opening and closing port with long port name
 * close the connection to C32
 */
TVerdict CTestLongPortName::doTestStepL()
	{
	INFO_PRINTF1(_L("============================================================"));
	INFO_PRINTF1(_L("   Test :PREQ890 C32 Opening and UnLoad with long port name "));
	INFO_PRINTF1(_L("============================================================"));

	INFO_PRINTF1(_L("Loading CSY in C32 component"));
	_LIT(KCSYName,"DummyCsy16");
	//Long Port Name
	_LIT(KLONGPortName,"DummyCsy16LongPortNameeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee::1");
	_LIT(KPortName,"DummyCsy16LongPortNameeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee");

	TName name;
	TSerialInfo protocolInfo;
	TInt count = 2;

    //Loading the communication system(CSY)
	TInt ret = iCommSession.LoadCommModule(KCSYName);
	if(ret != KErrNone)
		{
		INFO_PRINTF3(_L("ERROR: could not load '%S' CSY in Dealer thread due to %d"), &KCSYName, ret);
		SetTestStepResult(EFail);
		}
	else
		{
		INFO_PRINTF2(_L("OK. '%S' CSY loaded"), &KCSYName);
		}

 	for(TInt i = 0; i< count; i++)
	 	{

	 	ret = iCommSession.GetPortInfo(i, name, protocolInfo);
		if(ret != KErrNone)
			{
			INFO_PRINTF1(_L("Getting a port info after loading Comms module\n"));
			INFO_PRINTF3(_L("index %d: GetPortInfo returned %d"), i, ret);
			SetTestStepResult(EFail);
			}
		else
			{
			INFO_PRINTF3(_L("index    %d:   '%S'"), count, &name);
			INFO_PRINTF2(_L("description:    %S"), &protocolInfo.iDescription);
			INFO_PRINTF2(_L("port name  :    %S"), &protocolInfo.iName);
			INFO_PRINTF2(_L("low unit   :    %d"), protocolInfo.iLowUnit);
			INFO_PRINTF2(_L("high unit  :    %d"), protocolInfo.iHighUnit);
			if(protocolInfo.iName.CompareF(KCSYName) != 0 && i == 1)
				{
				INFO_PRINTF3(_L("index %d: Comm module name '%S' doesn't match expected"),i, &protocolInfo.iName);
				SetTestStepResult(EFail);
				}
			}
	 	}
	//Open Long Port Name
 	ret=iSerialPortList[1].Open(iCommSession,KLONGPortName,ECommShared);
 	if(ret != KErrNone)
		{
		INFO_PRINTF3(_L("ERROR: could not Open Long Port Name CSY '%S' due to %d"), &KLONGPortName, ret);
		SetTestStepResult(EFail);
		}
	else
		{
		INFO_PRINTF2(_L("OK. '%S' Long Port Name opened  Successfully"), &KLONGPortName);
		}

	iSerialPortList[1].Close();

	User::After(2000000);	// delay for the dummy port to close
	//Unloading a CSY having Long Portname
	ret = iCommSession.UnloadCommModule(KPortName);
	if(ret != KErrNone)
		{
		INFO_PRINTF3(_L("ERROR: could not unload Long Port Name CSY '%S' due to %d"), &KPortName, ret);
		SetTestStepResult(EFail);
		}
	else
		{
		INFO_PRINTF2(_L("OK. '%S' Long Port Name CSY  unloaded Successfully"), &KPortName);
		}

	return TestStepResult();
	}



// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Example CTestStep derived implementation
// 
//

/**
 @file loadcpmStep.cpp
*/
#include "loadcpmStep.h"
#include "Te_ConfiguratorSuiteDefs.h"
#include "Te_ConfiguratorSuiteServer.h"




//NET-CONFIGURATOR-I-0001-HP
//Loading a CPM in configurator with OnDemand option 1
/**
 * Destructor
 */
CLoadCpm::~CLoadCpm()
	{
	}

/**
 * Constructor
 */
CLoadCpm::CLoadCpm()
	{
	SetTestStepName(KLoadCpm);
	}
/**
 * @see LoadCpm test case NET-CONFIGURATOR-I-0001-HP
 *
 * doTestStepL virtual function does the below action
 * Connect to a configurator
 * Configurator Loads the DummyCpm CPM (Communication provider module)
 * Configurator to unload the DummyCpm
 * close the connection to configuator
 * Expected:-LoadCpm return kerrNone
*/
TVerdict CLoadCpm::doTestStepL()
	{
	SetTestStepResult(EFail);
    _LIT8(KNameDummyCpm,"DummyCpm");

	//Configurator Load the DummyCpm CPM
	iConfigurator.LoadCpm(KNameDummyCpm(),iStatus);
	User::WaitForRequest(iStatus);
	if(iStatus.Int() == KErrNone)
		{
		INFO_PRINTF1(_L("DummCpm Loaded Successfully \n"));
		SetTestStepResult(EPass);
		}
	else if(iStatus.Int() == KErrRSModuleUnknown)
		{
		INFO_PRINTF2(_L("LoadCpm (DummyCpm) returned KErrRSModuleUnknown(%d)  \n"), iStatus.Int());
		}
	else if(iStatus.Int() == KErrRSModuleAlreadyExist )
		{
		INFO_PRINTF2(_L("LoadCpm Loaded more than one returned KErrRSModuleAlreadyExist (%d)  \n"), iStatus.Int());
		}
	else
		{
		INFO_PRINTF2(_L("LoadCpm (DummyCpm) returned Error (%d)  \n"), iStatus.Int());
		}

	iConfigurator.UnloadCpm(KNameDummyCpm(), EImmediate, iStatus);
	//Waits for a specific asynchronous request to complete.
	User::WaitForRequest(iStatus);

	if(iStatus.Int() != KErrNone && iStatus.Int() != KErrCancel)
		{
		INFO_PRINTF1(_L("DummyCpm is not UnLoaded Successfully \n"));
		SetTestStepResult(EFail);
		}
	return TestStepResult();
	}

//NET-CONFIGURATOR-I-0002-HP
//Loading CPM More Than one time in a Configurator
/**
 * Destructor
 */
CLoadCpmMorethanOne::~CLoadCpmMorethanOne()
	{
	}

/**
 * Constructor
 */
CLoadCpmMorethanOne::CLoadCpmMorethanOne()
	{
	SetTestStepName(KLoadCpmMorethanOne);
	}
/**
 * @see LoadCpmMoreThanOne test case NET-CONFIGURATOR-I-0002-HP
 *
 * doTestStepL virtual function does the below action
 * Connect to a configurator
 * Configurator Loads the DummyCpm CPM (Communication provider module)
 * Configurator Loads the DummyCpm CPM
 * Configurator to unload the DummyCpm
 * close the connection to configuator
 * Expected:-LoadCpmMoreThanOne return KErrRSModuleAlreadyExist
*/
TVerdict CLoadCpmMorethanOne::doTestStepL()
	{

	SetTestStepResult(EFail);
	_LIT8(KNameDummyCpm,"DummyCpm");

	//Configurator Load the dummycpm CPM
	iConfigurator.LoadCpm(KNameDummyCpm(), iStatus);

	//Waits for a specific asynchronous request to complete.
	User::WaitForRequest(iStatus);

	if(iStatus.Int() == KErrNone)
		{
		INFO_PRINTF1(_L("DummyCpm Loaded Successfully \n"));
		}
	else if(iStatus.Int() == KErrRSModuleUnknown)
		{
		INFO_PRINTF2(_L("LoadCpm (DummyCpm) returned KErrRSModuleUnknown(%d)  \n"), iStatus.Int());
		}
	else
		{
		INFO_PRINTF2(_L("LoadCpm (DummyCpm) returned Error (%d)  \n"), iStatus.Int());
		}

	//Configurator Load the dummycpm CPM
	iConfigurator.LoadCpm(KNameDummyCpm(), iStatus);

	//Waits for a specific asynchronous request to complete.
	User::WaitForRequest(iStatus);

	if(iStatus.Int() == KErrNone)
		{
		INFO_PRINTF1(_L("DummyCpm Loaded more than one Successfully \n"));
		}
	else if(iStatus.Int() == KErrRSModuleUnknown)
		{
		INFO_PRINTF2(_L("DummyCpm Loaded more than one returned KErrRSModuleUnknown(%d)  \n"), iStatus.Int());
		}
	else if(iStatus.Int() == KErrRSModuleAlreadyExist )
		{
		INFO_PRINTF2(_L("DummyCpm Loaded more than one returned KErrRSModuleAlreadyExist (%d)  \n"), iStatus.Int());
		SetTestStepResult(EPass);
		}
	else
		{
		INFO_PRINTF2(_L("DummyCpm Loaded more than one returned Error (%d)  \n"), iStatus.Int());
		}

	//Configurator to unload the DummyCpm CPM
	iConfigurator.UnloadCpm(KNameDummyCpm(), EImmediate, iStatus);

	//Waits for a specific asynchronous request to complete.
	User::WaitForRequest(iStatus);

	if(iStatus.Int() != KErrNone)
		{
		INFO_PRINTF1(_L("DummyCpm is not UnLoaded Successfully \n"));
		SetTestStepResult(EFail);
		}
	return TestStepResult();
	}

//NET-CONFIGURATOR-I-0003-HP
//Loading a CPM in configurator on Boot time

/**
 * Destructor
 */
CLoadCpmBootTime::~CLoadCpmBootTime()
	{
	}

/**
 * Constructor
 */
CLoadCpmBootTime::CLoadCpmBootTime()
	{
	SetTestStepName(KLoadCpmBootTime);
	}
/**
 * @see LoadCpmBootTime test case NET-CONFIGURATOR-I-0003-HP
 *
 * doTestStepL virtual function does the below action
 * Connect to a configurator
 * Configurator Loads the DummyCpm CPM
 * Configurator to unload the DummyCpm
 * close the connection to configuator
 * Expected:-LoadCpmBootTime return KErrRSModuleAlreadyExist
*/
TVerdict CLoadCpmBootTime::doTestStepL()
	{

	SetTestStepResult(EFail);
	INFO_PRINTF1(_L("Configurator Connection Sucessful"));
    _LIT8(KNameDummyCpm8,"DummyCpm8");

	//Configurator Load the DummyCpm CPM
	iConfigurator.LoadCpm(KNameDummyCpm8(), iStatus);

	//Waits for a specific asynchronous request to complete.
	User::WaitForRequest(iStatus);

	if(iStatus.Int() == KErrNone)
		{
		INFO_PRINTF1(_L("DummyCpm8 is not Loaded during boot time \n"));
		}
	else if(iStatus.Int() == KErrRSModuleUnknown)
		{
		INFO_PRINTF2(_L("LoadCpm during boot time returned KErrRSModuleUnknown(%d)  \n"), iStatus.Int());
		}
	else
	if(iStatus.Int() == KErrRSModuleAlreadyExist)
		{
		INFO_PRINTF2(_L("LoadCpm returned KErrRSModuleAlreadyExist(%d)  \n"), iStatus.Int());
    	SetTestStepResult(EPass);
		}
	else
		{
		INFO_PRINTF2(_L("LoadCpm returned Error (%d)  \n"), iStatus.Int());
		}

	return TestStepResult();
	}

//NET-CONFIGURATOR-I-0004-HP
//Loading CPM in a Configurator Without CMI files
/**
 * Destructor
 */
CLoadCpmWithoutCMI::~CLoadCpmWithoutCMI()
	{
	}

/**
 * Constructor
 */
CLoadCpmWithoutCMI::CLoadCpmWithoutCMI()
	{
	SetTestStepName(KLoadCpmWithoutCMI);
	}
/**
 * @see LoadCpmWithoutCMI test case NET-CONFIGURATOR-I-0004-HP
 *
 * doTestStepL virtual function does the below action
 * Connect to a configurator
 * Configurator Loads the Test_NoCmi CPM (Communication provider module)
 * close the connection to configuator
 * Expected:-LoadCpm return KErrRSModuleUnknown
*/
TVerdict CLoadCpmWithoutCMI::doTestStepL()
	{

	SetTestStepResult(EFail);

	//Connection to the configurator
	_LIT8(KNameDummyNoCmi,"DummyNoCMI");

	//Configurator Load the dummycpm
	iConfigurator.LoadCpm(KNameDummyNoCmi(), iStatus);

	//Waits for a specific asynchronous request to complete.
	User::WaitForRequest(iStatus);
	if(iStatus.Int() == KErrNone)
		{
	    INFO_PRINTF2(_L("LoadCpmWithoutCMI returned KErrNone (%d) - Expected KErrRSModuleUnknown  \n"), iStatus.Int());
		}
	else if(iStatus.Int() == KErrRSModuleAlreadyExist)
		{
		INFO_PRINTF2(_L("DummyCpm (ENewHeap) Cpm returned KErrRSModuleAlreadyExist(%d)  \n"), iStatus.Int());
		}
	if(iStatus.Int() == KErrRSModuleUnknown)
		{
		INFO_PRINTF2(_L("LoadCpmWithoutCMI returned KErrRSModuleUnknown (%d)  \n"), iStatus.Int());
		SetTestStepResult(EPass);
		}
	else
		{
		INFO_PRINTF2(_L("LoadCpmWithoutCMI returned Error (%d)  \n"), iStatus.Int());
		}
	return TestStepResult();
	}

//NET-CONFIGURATOR-I-0005-HP
//Loading a CPM which has different heap option
/**
 * Destructor
 */
CLoadCpmWithDiffHeapOption::~CLoadCpmWithDiffHeapOption()
	{
	}

/**
 * Constructor
 */
CLoadCpmWithDiffHeapOption::CLoadCpmWithDiffHeapOption()
	{
	SetTestStepName(KLoadCpmWithDiffHeapOption);
	}
/**
 * @see LoadCpmWithDiffHeapOption test case NET-CONFIGURATOR-I-0005-HP
 *
 * doTestStepL virtual function does the below action
 * Connect to a configurator
 * Configurator Loads the CPMs (Communication provider module)
 * Configurator to unload the CPMs
 * close the connection to configuator
 * Expected:-LoadCpmWithDiffHeapOption return KerrNone
*/
TVerdict CLoadCpmWithDiffHeapOption::doTestStepL()
	{

	SetTestStepResult(EFail);
	TRequestStatus status2, status3;
	
	_LIT8(KNameDummyCpm,"DummyCpm");
	_LIT8(KNameDummyCpm2,"DummyCpm2");
	_LIT8(KNameDummyCpm3,"DummyCpm3");

	//Configurator Load the DummyCpm CPM
	iConfigurator.LoadCpm(KNameDummyCpm(), iStatus);
	//Waits for a specific asynchronous request to complete.
	User::WaitForRequest(iStatus);
	if(iStatus.Int() == KErrNone)
		{
		INFO_PRINTF1(_L("DummyCpm (ENewHeap) Cpm Loaded Successfully \n"));
		}
	else if(iStatus == KErrRSModuleAlreadyExist)
		{
		INFO_PRINTF2(_L("DummyCpm (ENewHeap) Cpm returned KErrRSModuleAlreadyExist(%d)  \n"), iStatus.Int());
		}
	else
	if(iStatus.Int() == KErrRSModuleUnknown)
		{
		INFO_PRINTF2(_L("DummyCpm (ENewHeap) Cpm returned KErrRSModuleUnknown(%d)  \n"), iStatus.Int());
		}
	else
		{
		INFO_PRINTF2(_L("DummyCpm (ENewHeap) Cpm returned Error (%d)  \n"), iStatus.Int());
		}

	//Configurator Load the DummyCpm2 CPM
	iConfigurator.LoadCpm(KNameDummyCpm2(), status2);
	User::WaitForRequest(status2);
	if(status2.Int() == KErrNone)
		{
		INFO_PRINTF1(_L("DummyCpm2 (EshareHeap) Cpm Loaded Successfully \n"));
		}
	else if(status2.Int() == KErrRSModuleAlreadyExist)
		{
		INFO_PRINTF2(_L("DummyCpm2 (EshareHeap) Cpm returned KErrRSModuleAlreadyExist(%d)  \n"), status2.Int());
		}
	else if(status2.Int() == KErrRSModuleUnknown)
		{
		INFO_PRINTF2(_L("DummyCpm2 (EshareHeap) Cpm returned KErrRSModuleUnknown(%d)  \n"), status2.Int());
		}
	else
		{
		INFO_PRINTF2(_L("DummyCpm2 (EshareHeap) Cpm returned Error (%d)  \n"), status2.Int());
		}

	//Configurator Load the DummyCpm3 CPM
	iConfigurator.LoadCpm(KNameDummyCpm3(), status3);
	//Waits for a specific asynchronous request to complete.
	User::WaitForRequest(status3);
	if(status3.Int() == KErrNone)
		{
		INFO_PRINTF1(_L("DummyCpm3 (EDefaultHeap) Cpm Loaded Successfully \n"));
		}
	else if(status3.Int() == KErrRSModuleUnknown)
		{
		INFO_PRINTF2(_L("DummyCpm3 (EDefaultHeap) Cpm returned KErrRSModuleUnknown(%d)  \n"), status3.Int());
		}
	else
		{
		INFO_PRINTF2(_L("DummyCpm3 (EDefaultHeap) Cpm returned Error (%d)  \n"), status3.Int());
		}
	if(iStatus.Int() == KErrNone && status2.Int() == KErrNone && status3.Int() == KErrNone )
		{
		INFO_PRINTF1(_L("DummyCpm (ENewHeap),DummyCpm2 (EshareHeap),DummyCpm3 (EDefaultHeap) are loading Successfully\n"));
		SetTestStepResult(EPass);
		}
	else
		{
		SetTestStepResult(EFail);
		}

	//Configurator unload the  CPMs
	iConfigurator.UnloadCpm(KNameDummyCpm(), EImmediate, iStatus);
	User::WaitForRequest(iStatus);

	iConfigurator.UnloadCpm(KNameDummyCpm2(), EImmediate, status2);
	User::WaitForRequest(status2);

	iConfigurator.UnloadCpm(KNameDummyCpm3(), EImmediate, status3);
	User::WaitForRequest(status3);
	return TestStepResult();
	}

//NET-CONFIGURATOR-I-0006-HP
//Loading of CPM at the boot time
/**
 * Destructor
 */
CLoadOnDemandCpmBootTime::~CLoadOnDemandCpmBootTime()
	{
	}

/**
 * Constructor
 */
CLoadOnDemandCpmBootTime::CLoadOnDemandCpmBootTime()
	{
		SetTestStepName(KLoadOnDemandCpmBootTime);
	}
/**
 * @see LoadOnDemandCpmBootTime test case NET-CONFIGURATOR-I-0006-HP
 *
 * doTestStepL virtual function does the below action
 * Connect to a configurator
 * Configurator Loads the CPMs (Communication provider module)
 * Configurator to unload the DummyCpm
 * close the connection to configuator
 * Expected:-LoadOnDemandCpmBootTime return KErrNone
*/
TVerdict CLoadOnDemandCpmBootTime::doTestStepL()
	{

	SetTestStepResult(EFail);
	INFO_PRINTF1(_L("Configurator Connection Sucessful"));
	_LIT8(KNameDummyCpm,"DummyCpm");
	//Configurator Load the DummyCpm CPM
	iConfigurator.LoadCpm(KNameDummyCpm(), iStatus);
	//Waits for a specific asynchronous request to complete.
	User::WaitForRequest(iStatus);
	if(iStatus.Int() == KErrNone)
		{
		INFO_PRINTF1(_L("DummyCpm Loaded Sucessfully"));
		SetTestStepResult(EPass);
		}
	else
		{
		INFO_PRINTF2(_L("DummyCpm Load Error %d"), iStatus.Int());
		}

	//Configurator to unload the DummyCpm CPM
	iConfigurator.UnloadCpm(KNameDummyCpm(), EImmediate, iStatus);

	//Waits for a specific asynchronous request to complete.
	User::WaitForRequest(iStatus);

	if(iStatus.Int() != KErrNone)
		{
		INFO_PRINTF1(_L("DummyCpm is not UnLoaded Successfully \n"));
		SetTestStepResult(EFail);
		}
	return TestStepResult();

	}

//NET-CONFIGURATOR-I-0007-HP
//Load same cpm with difference session simultaneously
/**
 * Destructor
 */
CLoadCpmDiffSession::~CLoadCpmDiffSession()
	{
	}

/**
 * Constructor
 */
CLoadCpmDiffSession::CLoadCpmDiffSession()
	{
	SetTestStepName(KLoadCpmDiffSession);
	}
/**
 * @see LoadCpmDiffSession test case NET-CONFIGURATOR-I-0007-HP
 *
 * doTestStepL virtual function does the below action
 * Connect to a configurator with two sessions
 * Configurator Loads the CPMs (Communication provider module)
 * Configurator to unload the DummyCpm2
 * close the connection to configuator
 * Expected:-LoadCpmDiffSession return KErrRSModuleAlreadyExist
*/
TVerdict CLoadCpmDiffSession::doTestStepL()
	{

	SetTestStepResult(EFail);
	TRequestStatus status1;
	RRsConfigurator configurator1;
	//Connection to the configurator
	TInt result1 = configurator1.Connect();
	if (result1 == KErrNone)
		{
		_LIT8(KNameDummyCpm,"DummyCpm");

		iConfigurator.LoadCpm(KNameDummyCpm(),iStatus);
		configurator1.LoadCpm(KNameDummyCpm(),status1);
		User::WaitForRequest(iStatus, status1);

		//Waits for a specific asynchronous request to complete
		if(iStatus.Int() == KErrNone || status1.Int() == KErrRSModuleAlreadyExist)
			{
			INFO_PRINTF1(_L("Session1 loaded DummyCpm Successfully \n"));
			SetTestStepResult(EPass);
			}
		else if(iStatus.Int() == KErrRSModuleAlreadyExist || status1.Int() == KErrNone)
			{
			INFO_PRINTF1(_L("Session2 loaded DummyCpm Successfully \n"));
		    SetTestStepResult(EPass);
			}
		else if(iStatus.Int() == KErrRSModuleUnknown)
			{
			INFO_PRINTF2(_L("Configurator1 Session Cpm returned KErrRSModuleUnknown(%d)  \n"), iStatus.Int());
			SetTestStepResult(EFail);
			}
		else if(status1.Int() == KErrRSModuleUnknown)
			{
			INFO_PRINTF2(_L("Configurator2 Session Cpm returned KErrRSModuleUnknown(%d)  \n"), status1.Int());
			SetTestStepResult(EFail);
			}
		else
			{
			INFO_PRINTF1(_L("Unknown Error \n"));
			SetTestStepResult(EFail);
			}
		
		// Wait for the remaining request.
		if(iStatus.Int() == KRequestPending)
			{
			User::WaitForRequest(iStatus);
			}
		else
			{
			User::WaitForRequest(status1);
			}

	  	if(iStatus.Int() == KErrNone)
			{
			//Configurator unload the DummyCpm
			iConfigurator.UnloadCpm(KNameDummyCpm(), EImmediate, iStatus);
			User::WaitForRequest(iStatus);
			}
		else if(status1.Int() == KErrNone)
			{
			//Configurator unload the DummyCpm
			configurator1.UnloadCpm(KNameDummyCpm(), EImmediate, status1);
			User::WaitForRequest(status1);
			}

		//Close to the configurator connection
		configurator1.Close();
		}
	else
		{
		INFO_PRINTF1(_L("Configurator Connection Failure \n"));
		}
	return TestStepResult();
	}

//NET-CONFIGURATOR-I-0008-HP
//Load all CPMs in a group where all modules are OnDemand
/**
 * Destructor
 */
CLoadallDemandCpm::~CLoadallDemandCpm()
	{
	}

/**
 * Constructor
 */
CLoadallDemandCpm::CLoadallDemandCpm()
	{
	SetTestStepName(KLoadallDemandCpm);
	}
/**
 * @see LoadallDemandCpm test case NET-CONFIGURATOR-I-0008-HP
 *
 * doTestStepL virtual function does the below action
 * Connect to a configurator with two sessions
 * Configurator Loads the onDemand CPMs (Communication provider module)
 * Configurator to unload the onDemand CPMs
 * close the connection to configuator
 * Expected:-LoadallDemandCpm return KErrNone
*/
TVerdict CLoadallDemandCpm::doTestStepL()
	{

	SetTestStepResult(EFail);
	TRequestStatus status5;

	_LIT8(KNameDummyCpm4,"DummyCpm4");
	_LIT8(KNameDummyCpm5,"DummyCpm5");

	//Configurator Load the dummycpm4
	iConfigurator.LoadCpm(KNameDummyCpm4(), iStatus);

	//Waits for a specific asynchronous request to complete
	User::WaitForRequest(iStatus);

	if(iStatus.Int() == KErrNone)
		{
		INFO_PRINTF1(_L("DummyCpm4 Loaded Successfully \n"));
	//	SetTestStepResult(EPass);
		}
	else if(iStatus == KErrRSModuleUnknown)
		{
		INFO_PRINTF2(_L("LoadCpm (DummyCpm4) returned KErrRSModuleUnknown(%d)  \n"), iStatus.Int());
		SetTestStepResult(EFail);
		}
	else
		{
		INFO_PRINTF2(_L("LoadCpm (DummyCpm4) returned Error (%d)  \n"), iStatus.Int());
		SetTestStepResult(EFail);
		}
	
	//Configurator Load the dummycpm
	iConfigurator.LoadCpm(KNameDummyCpm5(),status5);

	//Waits for a specific asynchronous request to complete
	User::WaitForRequest(status5);

	if(status5.Int() == KErrNone)
		{
		INFO_PRINTF1(_L("DummyCpm5 Loaded Successfully \n"));
	//	SetTestStepResult(EPass);
		}
	else if(status5 == KErrRSModuleUnknown)
		{
		INFO_PRINTF2(_L("LoadCpm (DummyCpm5) returned KErrRSModuleUnknown(%d)  \n"), status5.Int());
		SetTestStepResult(EFail);
		}
	else
		{
		INFO_PRINTF2(_L("LoadCpm (DummyCpm5) returned Error (%d)  \n"), status5.Int());
		SetTestStepResult(EFail);
		}

	if(status5.Int() == KErrNone && iStatus.Int() == KErrNone)
		{
		SetTestStepResult(EPass);
		}
	//Configurator unload the DummyCpm4
	iConfigurator.UnloadCpm(KNameDummyCpm4(), EImmediate, iStatus);

	//Waits for a specific asynchronous request to complete
	User::WaitForRequest(iStatus);

	//Configurator unload the DummyCpm5
	iConfigurator.UnloadCpm(KNameDummyCpm5(), EImmediate, status5);

	//Waits for a specific asynchronous request to complete
	User::WaitForRequest(status5);

	return TestStepResult();
	}

//NET-CONFIGURATOR-I-0009-HP
//Load all CPMs in a group where all modules are OnDemand
/**
 * Destructor
 */
CLoadCpmGrpDiffSession::~CLoadCpmGrpDiffSession()
	{
	}

/**
 * Constructor
 */
CLoadCpmGrpDiffSession::CLoadCpmGrpDiffSession()
	{
	SetTestStepName(KLoadCpmGrpDiffSession);
	}
/**
 * @see LoadCpmGrpDiffSession test case NET-CONFIGURATOR-I-0009-HP
 *
 * doTestStepL virtual function does the below action
 * Connect to a configurator with two sessions
 * Configurator Loads the CPMs (Communication provider module)
 * Configurator to unload the onDemand CPMs
 * close the connection to configuator
 * Expected:-LoadCpmGrpDiffSession return KErrNone
*/
TVerdict CLoadCpmGrpDiffSession::doTestStepL()
	{

	SetTestStepResult(EFail);
	_LIT8(KNameDummyCpm4,"DummyCpm4");
	_LIT8(KNameDummyCpm5,"DummyCpm5");
	
	//Configurator Load the dummycpm4 CPM
	iConfigurator.LoadCpm(KNameDummyCpm4(), iStatus);

	User::WaitForRequest(iStatus);
	if(iStatus.Int() == KErrNone)
		{
		INFO_PRINTF1(_L("DummyCpm4 Loaded Successfully \n"));
		}
	else if(iStatus.Int() == KErrRSModuleUnknown)
		{
		INFO_PRINTF2(_L("DummyCpm4 returned KErrRSModuleUnknown(%d)  \n"), iStatus.Int());
		SetTestStepResult(EFail);
		}
	else if(iStatus.Int() == KErrRSModuleAlreadyExist)
		{
		INFO_PRINTF2(_L("DummyCpm4 returned KErrRSModuleAlreadyExist(%d)  \n"), iStatus.Int());
		SetTestStepResult(EFail);
		}
	else
		{
		INFO_PRINTF2(_L("DummyCpm4 returned Error (%d)  \n"), iStatus.Int());
		SetTestStepResult(EFail);
		}

	//Connection to the configurator
	RRsConfigurator configurator2;
	TRequestStatus status2;
	TInt result2 = configurator2.Connect();
	if( result2 == KErrNone )
		{
		INFO_PRINTF1(_L("Configurator Connection Sucessful"));

		//Configurator Load the dummycpm5 CPM
		configurator2.LoadCpm(KNameDummyCpm5(), status2);
		User::WaitForRequest(status2);
		if(status2.Int() == KErrNone)
			{
			INFO_PRINTF1(_L("DummyCpm5 Loaded Successfully \n"));
			}
		else if(status2.Int() == KErrRSModuleUnknown)
			{
			INFO_PRINTF2(_L("DummyCpm5 returned KErrRSModuleUnknown(%d)  \n"), status2.Int());
			SetTestStepResult(EFail);
			}
		else if(status2.Int() == KErrRSModuleAlreadyExist)
			{
			INFO_PRINTF2(_L("DummyCpm5 returned KErrRSModuleAlreadyExist(%d)  \n"), status2.Int());
			SetTestStepResult(EFail);
			}
		else
			{
			INFO_PRINTF2(_L("DummyCpm5 returned Error (%d)  \n"), status2.Int());
			SetTestStepResult(EFail);
			}
	    if((iStatus == KErrNone) && (status2 == KErrNone))
			{
			INFO_PRINTF1(_L("Configurator session load Sucessful"));
			SetTestStepResult(EPass);
			}
		}
	iConfigurator.UnloadCpm(KNameDummyCpm4(), EImmediate, iStatus);
	User::WaitForRequest(iStatus);

	//Configurator Unload the dummycpm5
	configurator2.UnloadCpm(KNameDummyCpm5(), EImmediate, status2);
	User::WaitForRequest(status2);

	//Close  to the configurator 2nd session connection
	configurator2.Close();

	return TestStepResult();
	}

//NET-CONFIGURATOR-I-0010-HP
//Loading CPM With corrupted CMI files. Missed some mandatory field

/**
 * Destructor
 */
CLoadCpmCorruptCMI::~CLoadCpmCorruptCMI()
	{
	}

/**
 * Constructor
 */
CLoadCpmCorruptCMI::CLoadCpmCorruptCMI()
	{
	SetTestStepName(KLoadCpmCorruptCMI);
	}
/**
 * @see LoadCpmCorruptCMI test case NET-CONFIGURATOR-I-0010-HP
 *
 * doTestStepL virtual function does the below action
 * Connect to a configurator server
 * Configurator Loads the CPM (Communication provider module)
 * Configurator to unload the onDemand CPM
 * close the connection to configuator
 * Expected:-LoadCpmCorruptCMI return KErrRSModuleUnknown
*/
TVerdict CLoadCpmCorruptCMI::doTestStepL()
	{

	SetTestStepResult(EFail);
	INFO_PRINTF1(_L("Configurator Connection Sucessful"));
	_LIT8(KNameDummyCpm6,"DummyCpm6");

	//Configurator Load the DummyCpm6 CPM
	iConfigurator.LoadCpm(KNameDummyCpm6(), iStatus);

	//Waits for a specific asynchronous request to complete.
	User::WaitForRequest(iStatus);
	if(iStatus.Int() == KErrNone)
		{
		INFO_PRINTF1(_L("DummyCpm6 Loaded Successfully \n"));
		SetTestStepResult(EFail);
		}
	else if(iStatus.Int() == KErrRSModuleUnknown)
		{
		INFO_PRINTF2(_L("LoadCpm (DummyCpm6) returned KErrRSModuleUnknown(%d)  \n"), iStatus.Int());
		SetTestStepResult(EPass);
		}
	else if(iStatus.Int() == KErrRSModuleAlreadyExist )
		{
		INFO_PRINTF2(_L("LoadCpm Loaded more than one returned KErrRSModuleAlreadyExist (%d)  \n"), iStatus.Int());
		SetTestStepResult(EFail);
		}
	else
		{
		INFO_PRINTF2(_L("LoadCpm (DummyCpm6) returned Error (%d)  \n"), iStatus.Int());
		SetTestStepResult(EFail);
		}
	return TestStepResult();
	}

//NET-CONFIGURATOR-I-0011-HP
//Loading CPM in a Configurator With corrupted or broken CMI files ( fields with wrong spelling in Group as Groups)
/**
 * Destructor
 */
CLoadCpmBrokenCmi::~CLoadCpmBrokenCmi()
	{
	}

/**
 * Constructor
 */
CLoadCpmBrokenCmi::CLoadCpmBrokenCmi()
	{

	SetTestStepName(KLoadCpmBrokenCmi);

	}
/**
 * @see LoadCpmBrokenCmi test case NET-CONFIGURATOR-I-0011-HP
 *
 * doTestStepL virtual function does the below action
 * Connect to a configurator server
 * Configurator Loads the CPMs (Communication provider module)
 * close the connection to configuator
 * Expected:-LoadCpmBrokenCmi return KErrNone
*/
TVerdict CLoadCpmBrokenCmi::doTestStepL()
	{

	SetTestStepResult(EFail);
    _LIT8(KNameDummyCpm7,"DummyCpm7");

	//Configurator Load the DummyCpm7 CPM
	iConfigurator.LoadCpm(KNameDummyCpm7(), iStatus);

	//Waits for a specific asynchronous request to complete.
	User::WaitForRequest(iStatus);
	if(iStatus.Int() == KErrNone)
		{
		INFO_PRINTF1(_L("DummyCpm7 Loaded Successfully \n"));
		SetTestStepResult(EPass);
		}
	else if(iStatus.Int() == KErrRSModuleUnknown)
		{
		INFO_PRINTF2(_L("LoadCpm (DummyCpm7) returned KErrRSModuleUnknown(%d)  \n"), iStatus.Int());
		SetTestStepResult(EFail);
		}
	else if(iStatus.Int() == KErrRSModuleAlreadyExist )
		{
		INFO_PRINTF2(_L("LoadCpm Loaded more than one returned KErrRSModuleAlreadyExist (%d)  \n"), iStatus.Int());
		SetTestStepResult(EFail);
		}
	else
		{
		INFO_PRINTF2(_L("LoadCpm (DummyCpm7) returned Error (%d)  \n"), iStatus.Int());
		SetTestStepResult(EFail);
		}
	return TestStepResult();
	}

//NET-CONFIGURATOR-I-0012-HP
//Loading CPM in a Configurator With corrupted or broken CMI files ( fields with wrong spelling OnDemand)
/**
 * Destructor
 */
CLoadCpmCorruptOnDemand::~CLoadCpmCorruptOnDemand()
	{
	}

/**
 * Constructor
 */
CLoadCpmCorruptOnDemand::CLoadCpmCorruptOnDemand()
	{
	SetTestStepName(KLoadCpmCorruptOnDemand);
	}
/**
 * @see LoadCpmCorruptOnDemand test case NET-CONFIGURATOR-I-0012-HP
 *
 * doTestStepL virtual function does the below action
 * Connect to a configurator server
 * Configurator Loads the CPMs (Communication provider module)
 * close the connection to configuator
 * Expected:-LoadCpmCorruptOnDemand return KErrNone
*/
TVerdict CLoadCpmCorruptOnDemand::doTestStepL()
	{

	SetTestStepResult(EFail);
    _LIT8(KNameDummyCpm,"DummyCpm8");

	//Configurator Load the DummyCpm8 CPM
	iConfigurator.LoadCpm(KNameDummyCpm(), iStatus);

	//Waits for a specific asynchronous request to complete.
	User::WaitForRequest(iStatus);
	if(iStatus == KErrNone)
		{
		INFO_PRINTF1(_L("DummyCpm8 Loaded Successfully \n"));
		}
	else
	if(iStatus.Int() == KErrRSModuleUnknown)
		{
		INFO_PRINTF2(_L("LoadCpm (DummyCpm8) returned KErrRSModuleUnknown(%d)  \n"), iStatus.Int());
		SetTestStepResult(EFail);
		}
	else
	if(iStatus.Int() == KErrRSModuleAlreadyExist )
		{
		INFO_PRINTF2(_L("LoadCpm Loaded more than one returned KErrRSModuleAlreadyExist (%d)  \n"), iStatus.Int());
		SetTestStepResult(EPass);
		}
	else
		{
		INFO_PRINTF2(_L("LoadCpm (DummyCpm8) returned Error (%d)  \n"), iStatus.Int());
		SetTestStepResult(EFail);
		}

	//Configurator to unload the DummyCpm8 CPM
	iConfigurator.UnloadCpm(KNameDummyCpm(), EImmediate, iStatus);

	//Waits for a specific asynchronous request to complete.
	User::WaitForRequest(iStatus);

	if(iStatus.Int() != KErrNone)
		{
		INFO_PRINTF1(_L("DummyCpm is not UnLoaded Successfully \n"));
		SetTestStepResult(EFail);
		}
	return TestStepResult();
	}


//NET-CONFIGURATOR-I-0035-HP
//Loading CPM in a Configurator With corrupted or broken CMI files ( fields with wrong spelling OnDemand)
/**
 * Destructor
 */
CLoadCpmSameWorkerID::~CLoadCpmSameWorkerID()
	{
	}

/**
 * Constructor
 */
CLoadCpmSameWorkerID::CLoadCpmSameWorkerID()
	{
	SetTestStepName(KLoadCpmSameWorkerID);
	}
/**
 * @see LoadCpmSameWorkerID test case NET-CONFIGURATOR-I-0035-HP
 *
 * doTestStepL virtual function does the below action
 * Connect to a configurator server
 * Configurator Loads the CPMs (Communication provider module)
 * close the connection to configuator
 * Expected:-LoadCpmCorruptOnDemand return KErrNone
*/
TVerdict CLoadCpmSameWorkerID::doTestStepL()
	{

	SetTestStepResult(EFail);
	TRequestStatus status2;
    _LIT8(KNameDummyCpm15,"DummyCpm15");

	//Configurator Load the DummyCpm15 CPM
	iConfigurator.LoadCpm(KNameDummyCpm15(), iStatus);
		//Waits for a specific asynchronous request to complete.
	User::WaitForRequest(iStatus);
	if(iStatus.Int() == KErrNone)
		{
		INFO_PRINTF1(_L("DummyCpm15 Loaded Successfully \n"));
		}
	else if(iStatus.Int() == KErrRSModuleUnknown)
		{
		INFO_PRINTF2(_L("LoadCpm (DummyCpm15) returned KErrRSModuleUnknown(%d)  \n"), iStatus.Int());
		SetTestStepResult(EFail);
		}
	else if(iStatus.Int() == KErrRSModuleAlreadyExist )
		{
		INFO_PRINTF2(_L("LoadCpm Loaded more than one returned KErrRSModuleAlreadyExist (%d)  \n"), iStatus.Int());
		SetTestStepResult(EFail);
		}
	else
		{
		INFO_PRINTF2(_L("LoadCpm (DummyCpm15) returned Error (%d)  \n"), iStatus.Int());
		SetTestStepResult(EFail);
		}

	_LIT8(KNameDummyCpm16,"DummyCpm16");

	//Configurator Load the DummyCpm16 CPM
	iConfigurator.LoadCpm(KNameDummyCpm16(),status2);
	//Waits for a specific asynchronous request to complete.
	User::WaitForRequest(status2);
	if(iStatus.Int() == KErrNone && status2.Int() == KErrNone)
		{
		INFO_PRINTF1(_L("DummyCpm15 and DummyCpm16 Loaded Successfully \n"));
		SetTestStepResult(EPass);
		}
	else if(status2.Int() == KErrRSModuleUnknown)
		{
		INFO_PRINTF2(_L("LoadCpm (DummyCpm16) returned KErrRSModuleUnknown(%d)  \n"), status2.Int());
		SetTestStepResult(EFail);
		}
	else
	if(status2.Int() == KErrRSModuleAlreadyExist )
		{
		INFO_PRINTF2(_L("LoadCpm Loaded more than one returned KErrRSModuleAlreadyExist (%d)  \n"), status2.Int());
		SetTestStepResult(EFail);
		}
	else
		{
		INFO_PRINTF2(_L("LoadCpm (DummyCpm16) returned Error (%d)  \n"), status2.Int());
		SetTestStepResult(EFail);
		}

	//Configurator to unload the DummyCpm15 CPM
	iConfigurator.UnloadCpm(KNameDummyCpm15(), EImmediate, iStatus);

	//Waits for a specific asynchronous request to complete.
	User::WaitForRequest(iStatus);

	if(iStatus != KErrNone)
		{
		INFO_PRINTF1(_L("DummyCpm15 is not UnLoaded Successfully \n"));
		SetTestStepResult(EFail);
		}

	//Configurator to unload the DummyCpm16 CPM
	iConfigurator.UnloadCpm(KNameDummyCpm16(), EImmediate, status2);

	//Waits for a specific asynchronous request to complete.
	User::WaitForRequest(status2);

	if(status2 != KErrNone)
		{
		INFO_PRINTF1(_L("DummyCpm16 is not UnLoaded Successfully \n"));
		SetTestStepResult(EFail);
		}
	return TestStepResult();
	}

//NET-CONFIGURATOR-I-0001-HP
//Loading a CPM in configurator with OnDemand option 1, such that CPM fails to load
/**
 * Destructor
 */
CLoadCpmBindTimeOut::~CLoadCpmBindTimeOut()
	{
	}

/**
 * Constructor
 */
CLoadCpmBindTimeOut::CLoadCpmBindTimeOut()
	{
	SetTestStepName(KLoadCpmBindTimeOut);
	}
/**
 * @see LoadCpm test case NET-CONFIGURATOR-I-0001-HP
 *
 * doTestStepL virtual function does the below action
 * Connect to a configurator
 * Configurator Loads the DummyCpm CPM (Communication provider module)
 * Configurator to unload the DummyCpm
 * close the connection to configuator
 * Expected:-LoadCpm return kerrNone
*/
TVerdict CLoadCpmBindTimeOut::doTestStepL()
	{
	SetTestStepResult(EFail);
    _LIT8(KNameDummyBF,"DummyBF");

	iConfigurator.LoadCpm(KNameDummyBF(),iStatus);
	User::WaitForRequest(iStatus);
	if(iStatus.Int() == KErrRSRequestTimedOut)
		{
		INFO_PRINTF2(_L("DummCpm Load failed with (%d) Successfully \n"), iStatus.Int());
		SetTestStepResult(EPass);
		}
	else if(iStatus.Int() == KErrNone)
		{
		INFO_PRINTF2(_L("LoadCpm (DummyBF) returned KErrNone(%d)  \n"), iStatus.Int());
		}
	else if(iStatus.Int() == KErrRSModuleAlreadyExist )
		{
		INFO_PRINTF2(_L("LoadCpm Loaded more than one returned KErrRSModuleAlreadyExist (%d)  \n"), iStatus.Int());
		}
	else
		{
		INFO_PRINTF2(_L("LoadCpm (DummyBF) returned Error (%d)  \n"), iStatus.Int());
		}

	return TestStepResult();
	}

//NET-CONFIGURATOR-I-0001-HP
//Loading a CPM in configurator with OnDemand option 1, such that CPM fails to load
/**
 * Destructor
 */
CLoadOnDemandBootCPMFailToLoad::~CLoadOnDemandBootCPMFailToLoad()
	{
	}

/**
 * Constructor
 */
CLoadOnDemandBootCPMFailToLoad::CLoadOnDemandBootCPMFailToLoad()
	{
	SetTestStepName(KLoadOnDemandBootCPMFailToLoad);
	}
/**
 * @see LoadCpm test case NET-CONFIGURATOR-I-0001-HP
 *
 * doTestStepL virtual function does the below action
 * Connect to a configurator
 * Configurator Loads the DummyCpm CPM (Communication provider module)
 * Configurator to unload the DummyCpm
 * close the connection to configuator
 * Expected:-LoadCpm return kerrNone
*/
TVerdict CLoadOnDemandBootCPMFailToLoad::doTestStepL()
	{
	SetTestStepResult(EFail);
    _LIT8(KDummyODBI,"DummyODBI");

	iConfigurator.LoadCpm(KDummyODBI(),iStatus);
	User::WaitForRequest(iStatus);
	if(iStatus.Int() == KErrRSEitherModuleInBindingFailedToLoad)
		{
		INFO_PRINTF2(_L("DummyODBI Load failed with (%d) Successfully \n"), iStatus.Int());
		SetTestStepResult(EPass);
		}
	else if(iStatus.Int() == KErrNone)
		{
		INFO_PRINTF2(_L("LoadCpm (DummyODBI) returned KErrNone(%d)  \n"), iStatus.Int());
		}
	else if(iStatus.Int() == KErrRSModuleAlreadyExist )
		{
		INFO_PRINTF2(_L("LoadCpm Loaded more than one returned KErrRSModuleAlreadyExist (%d)  \n"), iStatus.Int());
		}
	else
		{
		INFO_PRINTF2(_L("LoadCpm (DummyODBI) returned Error (%d)  \n"), iStatus.Int());
		}

	return TestStepResult();
	}


//NET-CONFIGURATOR-I-0001-HP
//Loading a CPM in configurator with OnDemand option 1, such that CPM fails to load
/**
 * Destructor
 */
COnDemandCPMFailToLoad::~COnDemandCPMFailToLoad()
	{
	}

/**
 * Constructor
 */
COnDemandCPMFailToLoad::COnDemandCPMFailToLoad()
	{
	SetTestStepName(KOnDemandCPMFailToLoad);
	}
/**
 * @see LoadCpm test case NET-CONFIGURATOR-I-0001-HP
 *
 * doTestStepL virtual function does the below action
 * Connect to a configurator
 * Configurator Loads the DummyCpm CPM (Communication provider module)
 * Configurator to unload the DummyCpm
 * close the connection to configuator
 * Expected:-LoadCpm return kerrNone
*/
TVerdict COnDemandCPMFailToLoad::doTestStepL()
	{
	SetTestStepResult(EFail);
    _LIT8(KDummyLFODBI,"DummyLFODBI");

	iConfigurator.LoadCpm(KDummyLFODBI(),iStatus);
	User::WaitForRequest(iStatus);
	if(iStatus.Int() == KErrGeneral)
		{
		INFO_PRINTF2(_L("DummyLFODBI Load failed with (%d) Successfully \n"), iStatus.Int());
		SetTestStepResult(EPass);
		}
	else if(iStatus.Int() == KErrNone)
		{
		INFO_PRINTF2(_L("LoadCpm (DummyLFODBI) returned KErrNone(%d)  \n"), iStatus.Int());
		}
	else if(iStatus.Int() == KErrRSModuleAlreadyExist )
		{
		INFO_PRINTF2(_L("LoadCpm Loaded more than one returned KErrRSModuleAlreadyExist (%d)  \n"), iStatus.Int());
		}
	else
		{
		INFO_PRINTF2(_L("LoadCpm (DummyLFODBI) returned Error (%d)  \n"), iStatus.Int());
		}
	return TestStepResult();
	}

//NET-CONFIGURATOR-I-0001-HP
//Loading a CPM in configurator with OnDemand option 1, such that CPM fails to load
/**
 * Destructor
 */
COnDemandCPMFailToLoadBindingOrderChanged::~COnDemandCPMFailToLoadBindingOrderChanged()
	{
	}

/**
 * Constructor
 */
COnDemandCPMFailToLoadBindingOrderChanged::COnDemandCPMFailToLoadBindingOrderChanged()
	{
	SetTestStepName(KOnDemandCPMFailToLoadBindingOrderChanged);
	}
/**
 * @see LoadCpm test case NET-CONFIGURATOR-I-0001-HP
 *
 * doTestStepL virtual function does the below action
 * Connect to a configurator
 * Configurator Loads the DummyCpm CPM (Communication provider module)
 * Configurator to unload the DummyCpm
 * close the connection to configuator
 * Expected:-LoadCpm return kerrNone
*/
TVerdict COnDemandCPMFailToLoadBindingOrderChanged::doTestStepL()
	{
	SetTestStepResult(EFail);
    _LIT8(KDummyLFODBIOC,"DummyLFODBIOC");

	iConfigurator.LoadCpm(KDummyLFODBIOC(),iStatus);
	User::WaitForRequest(iStatus);
	if(iStatus.Int() == KErrGeneral)
		{
		INFO_PRINTF2(_L("DummyLFODBIOC Load failed with (%d) Successfully \n"), iStatus.Int());
		SetTestStepResult(EPass);
		}
	else if(iStatus.Int() == KErrNone)
		{
		INFO_PRINTF2(_L("LoadCpm (DummyLFODBIOC) returned KErrNone(%d)  \n"), iStatus.Int());
		}
	else if(iStatus.Int() == KErrRSModuleAlreadyExist )
		{
		INFO_PRINTF2(_L("LoadCpm Loaded more than one returned KErrRSModuleAlreadyExist (%d)  \n"), iStatus.Int());
		}
	else
		{
		INFO_PRINTF2(_L("LoadCpm (DummyLFODBIOC) returned Error (%d)  \n"), iStatus.Int());
		}
	return TestStepResult();
	}

//NET-CONFIGURATOR-I-0001-HP
//Loading a CPM in configurator with OnDemand option 1, such that CPM fails to load
/**
 * Destructor
 */
CLoadOnDemandCPMOneofTwoBindingsFail::~CLoadOnDemandCPMOneofTwoBindingsFail()
	{
	}

/**
 * Constructor
 */
CLoadOnDemandCPMOneofTwoBindingsFail::CLoadOnDemandCPMOneofTwoBindingsFail()
	{
	SetTestStepName(KLoadOnDemandBootCPMFailToLoad);
	}
/**
 * @see LoadCpm test case NET-CONFIGURATOR-I-0001-HP
 *
 * doTestStepL virtual function does the below action
 * Connect to a configurator
 * Configurator Loads the DummyCpm CPM (Communication provider module)
 * Configurator to unload the DummyCpm
 * close the connection to configuator
 * Expected:-LoadCpm return kerrNone
*/
TVerdict CLoadOnDemandCPMOneofTwoBindingsFail::doTestStepL()
	{
	SetTestStepResult(EFail);
    _LIT8(KDummyOD2B1MLF,"DummyOD2B1MLF");

	iConfigurator.LoadCpm(KDummyOD2B1MLF(),iStatus);
	User::WaitForRequest(iStatus);
	if(iStatus.Int() == KErrNone)
		{
		INFO_PRINTF2(_L("DummyOD2B1MLF Load passed (%d) Successfully \n"), iStatus.Int());
		SetTestStepResult(EPass);
		}
	else if(iStatus.Int() == KErrNone)
		{
		INFO_PRINTF2(_L("LoadCpm (DummyOD2B1MLF) returned KErrNone(%d)  \n"), iStatus.Int());
		}
	else if(iStatus.Int() == KErrRSModuleAlreadyExist )
		{
		INFO_PRINTF2(_L("LoadCpm Loaded more than one returned KErrRSModuleAlreadyExist (%d)  \n"), iStatus.Int());
		}
	else
		{
		INFO_PRINTF2(_L("LoadCpm (DummyOD2B1MLF) returned Error (%d)  \n"), iStatus.Int());
		}

	iConfigurator.UnloadCpm(KDummyOD2B1MLF(), EImmediate, iStatus);
	//Waits for a specific asynchronous request to complete.
	User::WaitForRequest(iStatus);

	if(iStatus.Int() != KErrNone && iStatus.Int() != KErrCancel)
		{
		INFO_PRINTF1(_L("DummyOD2B1MLF is not UnLoaded Successfully \n"));
		SetTestStepResult(EFail);
		}
	return TestStepResult();
	}





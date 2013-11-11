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
#include "TestAscynHandler.h"
#include "unloadcpmStep.h"
#include "Te_ConfiguratorSuiteDefs.h"

//NET-CONFIGURATOR-I-0013-HP
//UnLoading CPM which has been loaded  prevesouly

CUnloadCpm::~CUnloadCpm()
/**
 * Destructor
 */
	{
	}

CUnloadCpm::CUnloadCpm()
/**
 * Constructor
 */
	{
	SetTestStepName(KUnloadCpm);
	}
/**
 * @see UnloadCpm test case NET-CONFIGURATOR-I-0013-HP
 *
 * doTestStepL virtual function does the below action
 * Connect to a configurator server
 * Configurator Loads the CPMs (Communication provider module)
 * Configurator unloads the CPMs
 * close the connection to configuator
 * Expected:-UnloadCpm return KErrNone
*/
TVerdict CUnloadCpm::doTestStepL()
	{
	SetTestStepResult(EFail);
	_LIT8(KNameDummyCpm,"DummyCpm");

	//Configurator Load the tesCpm CPM
    iConfigurator.LoadCpm(KNameDummyCpm(), iStatus);
    //Waits for a specific asynchronous request to complete
	User::WaitForRequest(iStatus);
	if(iStatus.Int() == KErrNone)
		{
		INFO_PRINTF1(_L("DummyCpm Loaded Successfully \n"));
		}
	else if(iStatus.Int() == KErrRSModuleUnknown)
		{
		INFO_PRINTF2(_L("LoadCpm (DummyCpm) returned KErrRSModuleUnknown(%d)  \n"), iStatus.Int());
		SetTestStepResult(EFail);
		}
	else
		{
		INFO_PRINTF2(_L("LoadCpm (DummyCpm) returned Error (%d)  \n"), iStatus.Int());
		SetTestStepResult(EFail);
		}
	
	//Configurator to unload the DummyCpm CPM
	iConfigurator.UnloadCpm(KNameDummyCpm(), EImmediate, iStatus);
	
	//Waits for a specific asynchronous request to complete.
	User::WaitForRequest(iStatus);
	if(iStatus.Int() == KErrNone)
	  {
	  INFO_PRINTF1(_L("DummyCpm UnLoaded  Successfully "));
	  SetTestStepResult(EPass);
	  }
    else if(iStatus.Int() == KErrRSModuleUnknown)
 	  {
	  INFO_PRINTF2(_L("UnLoaded  (DummyCpm) returned KErrRSModuleUnknown(%d)  \n"), iStatus.Int());
	  SetTestStepResult(EFail);
      }
    else
	  {
	  INFO_PRINTF2(_L("UnLoaded  (DummyCpm) returned Error (%d)  \n"), iStatus.Int());
	  SetTestStepResult(EFail);
	  }
	return TestStepResult();
	}


//NET-CONFIGURATOR-I-0014-HP
//UnLoading CPM which has not been already loaded

CUnloadCpmPreNotLoaded::~CUnloadCpmPreNotLoaded()
/**
 * Destructor
 */
	{
	}

CUnloadCpmPreNotLoaded::CUnloadCpmPreNotLoaded()
/**
 * Constructor
 */
	{
	SetTestStepName(KUnloadCpmPreNotLoaded);
	}
/**
 * @see UnloadCpmPreNotLoaded test case NET-CONFIGURATOR-I-0014-HP
 *
 * doTestStepL virtual function does the below action
 * Connect to a configurator server
 * Configurator unloads the CPM
 * close the connection to configuator
 * Expected:-UnloadCpmPreNotLoaded return KErrRSModuleNotLoaded
*/
TVerdict CUnloadCpmPreNotLoaded::doTestStepL()
	{
	SetTestStepResult(EFail);
	_LIT8(KNameDummyCpm,"DummyCpm");

	//Configurator to unload the DummyCpm CPM
    iConfigurator.UnloadCpm(KNameDummyCpm(), EImmediate, iStatus);
    //Waits for a specific asynchronous request to complete
	User::WaitForRequest(iStatus);
    if(iStatus == KErrNone)
	 	{
	 	INFO_PRINTF1(_L("DummyCpm UnLoaded  Successfully returned \n"));
	 	SetTestStepResult(EFail);
	 	}
    else if(iStatus.Int() == KErrRSModuleUnknown)
		{
		INFO_PRINTF2(_L("UnLoaded  (DummyCpm) returned KErrRSModuleUnknown(%d)  \n"), iStatus.Int());
		SetTestStepResult(EFail);
		}
    else if(iStatus.Int() == KErrRSModuleNotLoaded)
		{
		INFO_PRINTF2(_L("UnLoaded  (DummyCpm) returned KErrRSModuleNotLoaded(%d)  \n"), iStatus.Int());
		SetTestStepResult(EPass);
		}
   	else
		{
		INFO_PRINTF2(_L("UnLoaded  (DummyCpm) returned Error (%d)  \n"), iStatus.Int());
		SetTestStepResult(EFail);
		}
	return TestStepResult();
	}

//NET-CONFIGURATOR-I-0015-HP
//Loading and unload different combination of cpm
// To be modified different combination Loading and unloading existing cpm
CLoadUnloadDiffCombination::~CLoadUnloadDiffCombination()
/**
 * Destructor
 */
	{
	}

CLoadUnloadDiffCombination::CLoadUnloadDiffCombination()
/**
 * Constructor
 */
	{
	SetTestStepName(KLoadUnloadDiffCombination);
	}
/**
 * @see LoadUnloadDiffCombination test case NET-CONFIGURATOR-I-0015-HP
 *
 * doTestStepL virtual function does the below action
 * Connect to a configurator server
 * Configurator  loads the CPM
 * Configurator unloads the CPM
 * Repeat the loading and unloading with different combination
 * close the connection to configuator
 * Expected:-LoadUnloadDiffCombination return KErrRSModuleNotLoaded
*/
TVerdict CLoadUnloadDiffCombination::doTestStepL()
	{

	SetTestStepResult(EFail);
	TRequestStatus	status3;
	TRequestStatus	status4;
	TInt failCount = 0;

	_LIT8(KNameDummyCpm,"DummyCpm");

	//Configurator Load the TesCpm CPM
	iConfigurator.LoadCpm(KNameDummyCpm(), iStatus);

	//Waits for a specific asynchronous request to complete
	User::WaitForRequest(iStatus);

	if(iStatus.Int() == KErrNone)
		{
		INFO_PRINTF1(_L("Cpm Loading Differenent order (DummyCpm) Loaded Successfully \n"));
		}
	else if(iStatus.Int() == KErrRSModuleUnknown)
		{
		INFO_PRINTF2(_L("Cpm Loading Differenent order (DummyCpm) returned KErrRSModuleUnknown(%d)  \n"), iStatus.Int());
		failCount++;
		}
	else
		{
		INFO_PRINTF2(_L("Cpm Loading Differenent order (DummyCpm) returned Error (%d)  \n"), iStatus.Int());
		failCount++;
		}

	_LIT8(KNameDummyCpm3,"DummyCpm3");

	//Configurator Load the DummyCpm3 CPM
	iConfigurator.LoadCpm(KNameDummyCpm3(), status3);

	//Waits for a specific asynchronous request to complete
	User::WaitForRequest(status3);
	if(status3.Int() == KErrNone)
		{
		INFO_PRINTF1(_L("Cpm Loading Differenent order (DummyCpm3) Loaded Successfully \n"));
		}
	else if(status3.Int() == KErrRSModuleUnknown)
		{
		INFO_PRINTF2(_L("Cpm Loading Differenent order (DummyCpm3) returned KErrRSModuleUnknown(%d)  \n"), status3.Int());
		failCount++;
		}
	else
		{
		INFO_PRINTF2(_L("Cpm Loading Differenent order (DummyCpm3) returned Error (%d)  \n"), status3.Int());
		failCount++;
		}

   	_LIT8(KNameDummyCpm4,"DummyCpm4");

	//Configurator Load the TesCpm4 CPM
	iConfigurator.LoadCpm(KNameDummyCpm4(),status4);

	//Waits for a specific asynchronous request to complete
	User::WaitForRequest(status4);

	if(status4.Int() == KErrNone)
		{
		INFO_PRINTF1(_L("Cpm Loading Differenent order (DummyCpm4) Loaded Successfully \n"));
		}
	else if(status4.Int() == KErrRSModuleUnknown)
		{
		INFO_PRINTF2(_L("Cpm Loading Differenent order  (DummyCpm4) returned KErrRSModuleUnknown(%d)  \n"), status4.Int());
		failCount++;
		}
	else
		{
		INFO_PRINTF2(_L("Cpm Loading Differenent order  (DummyCpm4) returned Error (%d)  \n"), status4.Int());
		failCount++;
		}

	//Configurator to unload the DummyCpm4 CPM
	iConfigurator.UnloadCpm(KNameDummyCpm4(), EImmediate, status4);
	User::WaitForRequest(status4);

	//Configurator to unload the DummyCpm3 CPM
	iConfigurator.UnloadCpm(KNameDummyCpm3(), EImmediate, status3);
	User::WaitForRequest(status3);

	//Configurator to unload the DummyCpm CPM
	iConfigurator.UnloadCpm(KNameDummyCpm(), EImmediate, iStatus);
	User::WaitForRequest(iStatus);

	//Configurator to load the DummyCpm4 CPM
	iConfigurator.LoadCpm(KNameDummyCpm4(), status4);
	User::WaitForRequest(status4);

	if(status4.Int() == KErrNone)
		{
		INFO_PRINTF1(_L("Cpm Loading Differenent order (DummyCpm4) Loaded Successfully \n"));
		}
	else if(status4.Int() == KErrRSModuleUnknown)
		{
		INFO_PRINTF2(_L("Cpm Loading Differenent order (DummyCpm4) returned KErrRSModuleUnknown(%d)  \n"), status4.Int());
		failCount++;
		}
	else
		{
		INFO_PRINTF2(_L("Cpm Loading Differenent order (DummyCpm4) returned Error (%d)  \n"), status4.Int());
		failCount++;
		}

  	//Configurator to unload the DummyCpm4
	iConfigurator.UnloadCpm(KNameDummyCpm4(), EImmediate, status4);
	User::WaitForRequest(status4);

	//Configurator to load the DummyCpm3
	iConfigurator.LoadCpm(KNameDummyCpm3(), status3);
	User::WaitForRequest(status3);

	if(status3.Int() == KErrNone)
		{
		INFO_PRINTF1(_L("Cpm Loading Differenent order (DummyCpm3) Loaded Successfully \n"));
		}
	else if(status3.Int() == KErrRSModuleUnknown)
		{
		INFO_PRINTF2(_L("Cpm Loading Differenent order  (DummyCpm3) returned KErrRSModuleUnknown(%d)  \n"), status3.Int());
		failCount++;
		}
	else
		{
		INFO_PRINTF2(_L("Cpm Loading Differenent order  (DummyCpm3) returned Error (%d)  \n"), status3.Int());
		failCount++;
		}

    //Configurator to unload the DummyCpm3 CPM
	iConfigurator.UnloadCpm(KNameDummyCpm3(), EImmediate, status3);
	User::WaitForRequest(status3);
	if(failCount == 0)
		{
		SetTestStepResult(EPass);
		}

	return TestStepResult();
	}

// NET-CONFIGURATOR-I-0016-HP
//Cancelling the Loading  CPM before and after loading
CCancelLoadCpm::~CCancelLoadCpm()
/**
 * Destructor
 */
	{
	}

CCancelLoadCpm::CCancelLoadCpm()
/**
 * Constructor
 */
	{
	SetTestStepName(KCancelLoadCpm);
	}
/**
 * @see CancelLoadCpm test case NET-CONFIGURATOR-I-0016-HP
 *
 * doTestStepL virtual function does the below action
 * Connect to a configurator server
 * Configurator  loads the CPM
 * Configurator unloads the CPM
 * Configurator call Cancel load to cancel the preveous load request
 * close the connection to configuator
 * Expected:-CancelLoadCpm return KErrNone or KerrCancelled
 * Expected result may not be always true it is wrote below mentioned in the production code
 *	There is no guarantee the module loading is canceled.
	A best effort is made and the asynchronous request waiting for the module
	to load returns with KErrCanceled on success.
 */
TVerdict CCancelLoadCpm::doTestStepL()
	{
	SetTestStepResult(EFail);

	// Construct and install the active scheduler
	CActiveScheduler*  scheduler = new (ELeave) CActiveScheduler;
	// Push onto the cleanup stack
	CleanupStack::PushL(scheduler);
	// Install as the active scheduler
	CActiveScheduler::Install(scheduler);

    CCanceledLoadCpm* synchHandler = CCanceledLoadCpm::NewL();
    CleanupStack::PushL(synchHandler);
	_LIT8(KNameDummyCpm,"DummyCpm");

	// issuing the Load request
	INFO_PRINTF1(_L("DummyCpm Trying to Load"));
	synchHandler->TryLoad(KNameDummyCpm());
    //synchHandler->Cancel();
    iConfigurator.CancelLoadCpm(KNameDummyCpm());
    
	CActiveScheduler::Start();
	if ((KErrCancel == synchHandler->iStatus.Int()) || (KErrNone == synchHandler->iStatus.Int()))
		{
		INFO_PRINTF2(_L("LoadCpm Success %d \n"),synchHandler->iStatus.Int());
		SetTestStepResult(EPass);
		}
    
    // Unload the CPM anyway in case we were too late to cancel the load.
    TRequestStatus status;
    iConfigurator.UnloadCpm(KNameDummyCpm(), EImmediate, status);
    User::WaitForRequest(status);

	CleanupStack::PopAndDestroy(synchHandler);
	CleanupStack::PopAndDestroy(scheduler);
	return TestStepResult();
	}

//NET-CONFIGURATOR-I-0017-HP
CCancelUnLoadCpm::~CCancelUnLoadCpm()
/**
 * Destructor
 */
	{
	}

 CCancelUnLoadCpm::CCancelUnLoadCpm()
/**
 * Constructor
 */
	{
	}

/**
 * @see CancelUnLoadCpm test case NET-CONFIGURATOR-I-0017-HP
 *
 * doTestStepL virtual function does the below action
 * Connect to a configurator server
 * Configurator  loads the CPM
 * Configurator unloads the CPM
 * Configurator call Cancel unload to cancel the preveous unload request
 * close the connection to configuator
 * Expected:-CancelUnLoadCpm return KErrCancel or KErrNone
 * Expected result may not be always true it is wrote below mentioned in the production code
 *	There is no guarantee the module loading is canceled.
	A best effort is made and the asynchronous request waiting for the module
	to load returns with KErrCanceled on success.
*/
TVerdict CCancelUnLoadCpm::doTestStepL()
	{

	SetTestStepResult(EFail);

	// Construct and install the active scheduler
	CActiveScheduler*  scheduler = new (ELeave) CActiveScheduler;

	// Push into the cleanup stack
	CleanupStack::PushL(scheduler);

	// Install as the active scheduler
	CActiveScheduler::Install(scheduler);

	// create a CanceledLoadCpm active objects
    CCanceledLoadCpm* synchHandler = CCanceledLoadCpm::NewL();
    CleanupStack::PushL(synchHandler);
	_LIT8(KNameDummyCpm,"DummyCpm");

	INFO_PRINTF1(_L("DummyCpm Trying to Load"));
	iConfigurator.LoadCpm(KNameDummyCpm(), iStatus);
	User::WaitForRequest(iStatus);

	synchHandler->TryUnLoad(KNameDummyCpm(), EImmediate);
	iConfigurator.CancelUnloadCpm(KNameDummyCpm());
	CActiveScheduler::Start();
	if ((KErrCancel == synchHandler->iStatus.Int()) ||
		(KErrNone == synchHandler->iStatus.Int()) ||
		(KErrRSModuleNotLoaded == synchHandler->iStatus.Int()))
		{
		INFO_PRINTF2(_L("Cancel UnloadCpm Success %d \n"), synchHandler->iStatus.Int());
		SetTestStepResult(EPass);
		}
	else
		{
		INFO_PRINTF2(_L("Cancel UnloadCpm Fail %d \n"), synchHandler->iStatus.Int());
		}

	CleanupStack::PopAndDestroy(synchHandler);
	CleanupStack::PopAndDestroy(scheduler);
	return TestStepResult();
	}


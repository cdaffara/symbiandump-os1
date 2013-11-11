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
 @file LoadDllStep.cpp
*/
#include "LoadDllStep.h"
//#include "Te_lbssupltestmoduleSuiteDefs.h"

const TUid KLbsAssistanceDataSourceInterfaceUid = {0x1028225A};

CConnectFailedStep::~CConnectFailedStep()
/**
 * Destructor
 */
	{
	}

CConnectFailedStep::CConnectFailedStep(CTe_lbssupltestmoduleSuite* aOwningServer): CTe_lbssupltestmoduleSuiteStepBase(aOwningServer)

	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KLoadDllStep);
	}

TVerdict CConnectFailedStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("Please delete this line or modify it. I am in Test Step Preamble in Class CConnectFailedStep"));
	// Install the active scheduler
    iActSchd = new (ELeave) CActiveScheduler;
	CActiveScheduler::Install(iActSchd);
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CConnectFailedStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	  if (TestStepResult()==EPass)
		{
		// Load the SUPL module
		TRAPD(err, suplModule = CAssistanceDataSourceModuleBase::NewL(*this));
    	if (err != KErrNone)
    		{
    		ERR_PRINTF2(_L("Failed to load pluging: %D"), err);
    		SetTestStepResult(EFail);
   			return TestStepResult();
    		}
		
		// Create a Supl Session
		if (suplModule != NULL)
			{
			suplSession = suplModule->NewDataSourceL();
			}
		else
			{
			ERR_PRINTF1(_L("assistDataSource doesn't exist"));
    		SetTestStepResult(EFail);
   			return TestStepResult();
			}

		if (suplSession == NULL)
			{
			ERR_PRINTF1(_L("suplSession doesn't exist"));
    		SetTestStepResult(EFail);
   			return TestStepResult();			
			}
		
		// Try and get assistance data
		suplSession->GetAssistanceData()
		}
	  return TestStepResult();
	}



TVerdict CConnectFailedStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("Please delete this line or modify it. I am in Test Step Postamble in Class CConnectFailedStep"));
	// process something post setting to the test step
	// uncomment the following line if you have common post setting to all the test steps in there
	// CTe_lbssupltestmoduleSuiteStepBase::doTestStepPostambleL();
	// uncomment the following line if you have post process or remove the following line if no post process
	// SetTestStepResult(EPass);		// or EFail
	
	// delete the server's module and session
	delete suplSession;
	delete suplModule;
    delete iActSchd;

	return TestStepResult();
	}

void CConnectFailedStep::DataSourceNotification(const TInt& /*aError*/)
{
    INFO_PRINTF1(_L("CallBack"));
}

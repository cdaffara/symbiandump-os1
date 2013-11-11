/**
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/



/**
 @file Te_LbsRootApiTestSuiteStepBase.h
*/

#if (!defined __TE_LBSROOTAPITEST_STEP_BASE__)
#define __TE_LBSROOTAPITEST_STEP_BASE__
#include <e32property.h>
#include <centralrepository.h>
#include <test/testexecutestepbase.h>
#include "lbsrootapi.h"
// Please add your include here if you have 

typedef TBuf<100> TLbsProcessName;
/****************************************************************************
* The reason to have a new step base is that it is very much possible
* that the all individual test steps have project related common variables 
* and members 
* and this is the place to define these common variable and members.
* 
****************************************************************************/
class CTe_LbsRootApiTestSuiteStepBase : public CTestStep
//, public MLbsTestStepObserver
	{
public:
	virtual ~CTe_LbsRootApiTestSuiteStepBase();
	CTe_LbsRootApiTestSuiteStepBase();
	virtual TVerdict doTestStepPreambleL(); 
	virtual TVerdict doTestStepPostambleL();

protected:
    TInt KillLbsRoot();
	void StartLbsRootL();
    
    void SetTestRepositorySettingsL(TInt aTestStep = 0);
		
protected:
    RLbsSystemController iLbsRootProcessApi;
    
    RProperty   iPanicRequest;
    RProperty   iPanicOnCloseDownRequest;
    RProperty   iSecondCloseDownRequest;
	RProperty iResetFunctionCalled;

	};

#endif

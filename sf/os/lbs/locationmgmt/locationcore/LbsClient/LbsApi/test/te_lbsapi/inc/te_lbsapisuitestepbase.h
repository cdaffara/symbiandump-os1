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
 @file Te_LbsApiSuiteStepBase.h
 @internalTechnology
*/

#if (!defined __TE_LBSAPI_STEP_BASE__)
#define __TE_LBSAPI_STEP_BASE__
#include <test/TestExecuteStepBase.h>
// Please add your include here if you have 

struct SPanicThreadFunctionData
{
    TThreadFunction     iPanicFunction;     // thread function that will panic
    TAny*               iPtr;               // data to pass to the function
};

/****************************************************************************
* The reason to have a new step base is that it is very much possible
* that the all individual test steps have project related common variables 
* and members 
* and this is the place to define these common variable and members.
* 
****************************************************************************/
class CTe_LbsApiSuiteStepBase : public CTestStep
	{
public:
	virtual ~CTe_LbsApiSuiteStepBase();
	CTe_LbsApiSuiteStepBase();
	virtual TVerdict doTestStepPreambleL(); 
	virtual TVerdict doTestStepPostambleL();

protected:
// Tool functions

    void StandardPrepareL();
    void StandardCleanup();

    TInt DoPanicTestL(TThreadFunction aThreadFunction, TExitCategoryName aExpectedExitCat,
                      TInt aExpectedExitReason, TTimeIntervalMicroSeconds32 aTimeoutValue,
                      TAny* aPtr);

    void CheckThereIsNoServerL();
	
//Please add your class members which will be common to all individual test steps:
protected:
	TInt 		iErrorCounter;
	HBufC8*		iReadData;
	HBufC8*		iWriteData;
	};

#endif

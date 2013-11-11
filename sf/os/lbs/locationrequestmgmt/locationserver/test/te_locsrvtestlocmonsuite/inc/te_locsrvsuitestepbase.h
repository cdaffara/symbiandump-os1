/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file 
 @internalTechnology
 @test
*/

#ifndef __TE_LOCSRVTESTLOCMON_STEP_BASE__
#define __TE_LOCSRVTESTLOCMON_STEP_BASE__
#include <test/TestExecuteStepBase.h>
#include "te_locsrvsuitedefs.h"

#include <lbs.h>
#include <lbserrors.h>
#include <lbscommon.h>
#include <lbssatellite.h>
#include <lbspositioninfo.h>
#include <lbs/epos_cposmodules.h>
#include <lbs/epos_cposmoduleidlist.h>
#include <lbs/epos_cposmoduleupdate.h>

#include "lastknowndummyvalues.h"

/****************************************************************************
	Copied from the other suite te_locsrv [many of the following may be redundant]
*****************************************************************************/

const TInt KTestHeapMinSize = 0x4000;
const TInt KTestHeapMaxSize = 0x8000;

const TInt KFifthOfSecond = 200000;
const TInt KFifthSecond = 200 * 1000;
const TInt KHalfSecond = 500 * 1000;
const TInt KSecondsToMicro = 1000*1000;
const TInt KSecond = KSecondsToMicro;

const TInt KDefaultTestTimeout = 20*1000*1000;  //20 seconds should be more than enough.

#define DO_PANIC_TEST_L(aThreadFunction, aExpectedExitCat, aExpectedExitReason, aTimeoutValue) \
    { \
    TExitCategoryName exitName = TPtrC(aExpectedExitCat).Left(KMaxExitCategoryName); \
    DoPanicTestL(aThreadFunction, exitName, aExpectedExitReason, aTimeoutValue); \
    }
#define CHECK_EQUAL_PROPS(obj1, obj2, prop) \
    CheckExpectedResult(obj1.prop, obj2.prop, KFailedPropertyCheck)

const TReal64 KErrorToleranceValue = KMinTReal64; //0.00001;

struct SPanicThreadFunctionData
{
    TThreadFunction     iPanicFunction;     // thread function that will panic
    TAny*               iPtr;               // data to pass to the function
};

class CTe_locsrvSuiteStepBase : public CTestStep
	{
public:
	virtual ~CTe_locsrvSuiteStepBase();
	CTe_locsrvSuiteStepBase();
	virtual TVerdict doTestStepPreambleL(); 
	virtual TVerdict doTestStepPostambleL();

protected:
    CActiveScheduler*   iScheduler;

	HBufC8*		iReadData;
	HBufC8*		iWriteData;
	
    RPositionServer iServer;
    RPositioner     iPositioner;
    CPosModules*    iModulesSettings;
    
    TRequestStatus iStatus;
	
	void CheckExpectedResult(TInt aResult, TInt aExpectedResult, const TDesC& aLabel = KFailedEqualCheck);
	void CheckExpectedResult(TUid aResult, TUid aExpectedResult, const TDesC& aLabel = KFailedEqualCheck);
	void CheckExpectedResult(TTime aResult, TTime aExpectedResult, const TDesC& aLabel = KFailedEqualCheck);
	void CheckExpectedResult(const TDesC& aResult, const TDesC& aExpectedResult, const TDesC& aLabel = KFailedEqualCheck);
	void CheckExpectedResult(TTimeIntervalMicroSeconds aResult, TTimeIntervalMicroSeconds aExpectedResult, const TDesC& aLabel = KFailedEqualCheck);
	void CheckExpectedRealResult(TReal32 aResult, TReal32 aExpectedResult, const TDesC& aLabel = KComparedValuesNaN);
	void CheckResult(TBool aResult);
	void PositionRequestWithCheck(TPositionInfo& _aInfo, TInt _aResult, TUid _aModuleId);
	void PositionRequestWithCheck(TPositionInfo& _aInfo, TRequestStatus& aStatus, TInt _aResult, TUid _aModuleId, RPositioner aPositioner);
	TInt DoPanicTestL(TThreadFunction aThreadFunction, TExitCategoryName aExpectedExitCat,
	                  TInt aExpectedExitReason, TTimeIntervalMicroSeconds32 aTimeoutValue);
	
	void StandardPrepareL();
	void CTe_locsrvSuiteStepBase::StandardCleanup();
	void CTe_locsrvSuiteStepBase::SetupPsyPriorityL(TUid aPsyUid, TInt aPriority);
	void DisableAllModulesL();
	void TogglePsyL(TUid aPsyUid, TBool aEnabled);
	
	};
#endif

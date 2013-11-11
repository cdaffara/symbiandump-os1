// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Base class for calculation tests
// 
//

#ifndef __CT_LBS_CALCULATION_STEP_BASE_H__
#define __CT_LBS_CALCULATION_STEP_BASE_H__

// User includes
#include <test/testexecutestepbase.h>

//CONSTANTS
const TInt KMaxLogLineLength = 255;

// FORWARD DECLARATIONS
class CT_LbsCalculationServer;
class T_LbsCalculationCheck;

// CLASS DECLARATION
class CT_LbsCalculationStepBase : public CTestStep
{

public:
	virtual ~CT_LbsCalculationStepBase();

	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();

protected: // New functions
    
    TInt ReadValues();

    void AssertTrueSecL(TBool aCondition, const TDesC& aErrorMsg, TInt aErrorCode);
    
    void LogError(const TBool aCondition, 
            const T_LbsCalculationCheck& aCalc, 
            const TInt aErrorCode,
            const TInt aRow=0);
    
    void LogErrorAndLeaveL(const TDesC& aErrorMsg);
    
    virtual void LogTestError(const T_LbsCalculationCheck& aCalc, 
            const TInt aError,
            const TInt aRow) = 0;
    
protected:

	void ConstructL();
    CT_LbsCalculationStepBase(CT_LbsCalculationServer& aParent);
	
protected:	

	CActiveScheduler* iScheduler;
    RArray<T_LbsCalculationCheck>   iCalculationChecks;
    RFs                         iFileServer;
	CT_LbsCalculationServer& 	iParent;
};

#endif // __CT_LBS_CALCULATION_STEP_BASE_H__

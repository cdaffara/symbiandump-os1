/**
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file SimPsyStep.h
 @internalTechnology
*/
#if (!defined __SIMPSY_STEP_H__)
#define __SIMPSY_STEP_H__
#include <test/testexecutestepbase.h>
#include "te_lbssimulationpsysuitestepbase.h"

#include <f32file.h>
#include <badesca.h>

class TPositionInfoBase;
class TPositionInfo;
class CPosFileReader;


class CSimPsyStep : public CTe_LbsSimulationPSYSuiteStepBase
	{
public:
	CSimPsyStep();
	~CSimPsyStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();


private:        
	void StartL(TInt aIndex);
	virtual void DoTestL(TInt aIndex);
    void TestSingleRequestL();             
    void TestSingleCancelTestL();
    void TestMultipleCancelTestL(
                    TBool aFirstWillCancel,
                    TBool aSecondWillCancel,
                    TBool aThirdWillCancel
                    );        

    void TestSimulatedMovementsL(TInt aNumberOfRequests);
    void TestDeterministicFailureL();        
    void TestRandomFailureL();        
    void TestMultipleRequestsL(TInt aNumberOfRequests);
    void TestMultipleRequestsAndCancelL(
                    TInt aNumberOfRequests,
                    TBool aFirstWillCancel,
                    TBool aSecondWillCancel,
                    TBool aThirdWillCancel
                    );        
    void TestLongitudeWrapAroundL();        
    void TestBadSimulationDataFileL();        
    void TestRequestTimeAfterCancelL();
	void TestRequestCancelRequestL();
	void TestStatusEventL();
	void TestModuleInfoL();
    void TestStandingStillL();
    TInt ComputeNewLongAndLat(
                    TPositionInfoBase& aPosition, 
                    TReal aAlpha, 
                    TReal& aLon, 
                    TReal& aLat
                    );
    void CheckRequestResultL(
                    TInt aStatus, 
                    TPositionInfo& aPosition);
	void CheckRequestTimeL(
                    const TDesC& aSimFile, TBool aPowerUp,
                    const TPositionInfo& aPos,
                    TTimeIntervalMicroSeconds& aReqTime, 
					TTimeIntervalMicroSeconds aTolerance = TTimeIntervalMicroSeconds(100000));

    void CheckThatPositionExistsL(TPositionInfoBase& aInfo);        
    void ReadSimDataFromFileL(const TDesC& aFileName);
    void SetSimDataFileL(const TDesC& aFileName);
    TInt Replace(const TDesC& aFilename, const TInt aFileMode);

private:
    TInt64              iSeed;    
    CDesC8Array*        iSimDataArray;
    CPosFileReader*     iFileReader;
	TPositionInfoBase*	iPositionInfo;
	};

_LIT(KSimPsyStep,"SimPsyStep");

#endif

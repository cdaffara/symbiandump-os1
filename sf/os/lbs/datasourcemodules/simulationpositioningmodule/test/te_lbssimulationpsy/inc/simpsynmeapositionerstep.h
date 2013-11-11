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
 @file SimPsyNmeaPositionerStep.h
 @internalTechnology
*/
#if (!defined __SIMPSYNMEAPOSITIONER_STEP_H__)
#define __SIMPSYNMEAPOSITIONER_STEP_H__
#include <test/testexecutestepbase.h>
#include "te_lbssimulationpsysuitestepbase.h"

#include <f32file.h>
#include <badesca.h>
#include <centralrepository.h>
#include <lbs/simulationpsyinternalcrkeys.h>



#include "CPosFileReader.h"

// FORWARD DECLARATIONS
class TPositionInfo;
class TPositionInfoBase;
class TPositionCourseInfo;
class HPositionGenericInfo;
class CPosFileReader;

class CSimPsyNmeaPositionerStep : public CTe_LbsSimulationPSYSuiteStepBase
	{
public:
	CSimPsyNmeaPositionerStep();
	~CSimPsyNmeaPositionerStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

private:        
	void StartL(TInt aIndex);
	virtual void DoTestL(TInt aIndex);
	void FixSettingsForSimPSY(const TDesC& aFileName);
	void ReadSimDataFromFileL();
    void TestSingleNmeaRequestL();
    void CancelSingleNmeaRequestL();
    void TestMultipleNmeaCancelTestL(
                    TBool aFirstWillCancel,
                    TBool aSecondWillCancel,
                    TBool aThirdWillCancel);
    void TestRequestUntilEofL();
    void TestMultipleRequestsL(TInt aNumberOfRequests);
    void TestMultipleRequestsAndCancelL(TInt aNumberOfRequests,
                    TBool aFirstWillCancel,
                    TBool aSecondWillCancel,
                    TBool aThirdWillCancel);
    void TestRequestNmeaDataL();
    void TestReadUntilEofL();
	void TestWrongSimModeL();
    void TestRequestCourseInfoL();
    void TestRequestSatelliteInfoL();
    void TestRequestGenericInfoL();
    void TestRequestBasicInfoL();
    void TestRequestMagellanL();
    void TestRequestPartialGenericInfoL();
    void TestClearAndSetDefaultPositionDataL();
    void TestPartialUpdateL();
	void TestErrorSituationsL();
    void TestSpecialCasesL();
    void RunMemoryTestL(TInt aIndex);
    void CheckRequestResultL(TInt aStatus, TPositionInfo& aPosition);
    TBool IsPartialPosition(TPositionInfoBase& aPosInfo);
    
private:
    TInt64              iSeed;    
    CDesC8Array*        iSimDataArray;        
    CPosFileReader*     iFileReader;
    TPositionInfo*      iPositionInfo;
    TInt                iCurrentItem;
    TBool               iSetCurrentItem;
	};

_LIT(KSimPsyNmeaPositionerStep,"SimPsyNmeaPositionerStep");

#endif

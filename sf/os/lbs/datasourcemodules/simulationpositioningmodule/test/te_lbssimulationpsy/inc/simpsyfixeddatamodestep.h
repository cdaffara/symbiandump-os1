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
 @file SimPsyFixedDataModeStep.h
 @internalTechnology
*/
#if (!defined __SIMPSYFIXEDDATAMODE_STEP_H__)
#define __SIMPSYFIXEDDATAMODE_STEP_H__
#include <test/testexecutestepbase.h>
#include "te_lbssimulationpsysuitestepbase.h"

#include <badesca.h>
#include <lbs.h>

#include <centralrepository.h>
#include <lbs/simulationpsyinternalcrkeys.h>

#include "CPosFileReader.h"
//#include "EPos_TDesTokeniser.h"
#include "CSimPsyMultiRequester.h"
#include "CSimPsyRequester.h"
//#include "CSimPsyMultiNmeaRequester.h"

class CSimPsyFixedDataModeStep : public CTe_LbsSimulationPSYSuiteStepBase
	{
public:
	CSimPsyFixedDataModeStep();
	~CSimPsyFixedDataModeStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

// Please add/modify your class members here:
private:
	void StartL(TInt aIndex);
	virtual void DoTestL(TInt aIndex);
	void TestSingleRequestL();
	void TestMultipleRequestsL(TInt aNumberOfRequests);
	void CheckRequestResultL(TInt aStatus, TPositionInfo& aPosition);
	void SetSimDataFileL(const TDesC& aFileName);
	
private:
	TInt64              iSeed;    
	CDesC8Array*        iSimDataArray;
	TPositionInfoBase*	iPositionInfo;
	};

_LIT(KSimPsyFixedDataModeStep,"SimPsyFixedDataModeStep");

#endif

/**
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
 @file TDBT1Step.h
 @internalTechnology
*/
#ifndef __TDBT1_STEP_H__
#define __TDBT1_STEP_H__
#include <test/TestExecuteStepBase.h>
#include "Te_LbsBtGpsPsyStepBase.h"

#include "LcfCommonProcedures.h"

// FORWARD DECLARATIONS
class TPositionModuleInfo;
class TPositionInfoBase;
class TPositionInfo;
class TPositionCourseInfo;

class CTDBT1Step : public CTe_LbsBtGpsPsyStepBase
	{
public:
	CTDBT1Step();
	~CTDBT1Step();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();
	// Sub test case functions:

private:	
	/**
	* Runs the test in the module
	*/
	void StartL(TInt aIndex);
	
    void Sub0TCB1001PSYInfoAndPositionRequestingL();
    void RunSubCase0L( CLcfCommonProcedures* aCommon );
    void RunSubCase0WithDefaultL( CLcfCommonProcedures* aCommon );

    void Sub1TCB1002PeriodicUpdatesL();
    void RunSubCase1L( CLcfCommonProcedures* aCommon );
    void RunSubCase1WithDefaultL( CLcfCommonProcedures* aCommon );

    void Sub2TCB2001ModuleStatusChangesL();
    void RunSubCase2L( CLcfCommonProcedures* aCommon );

    void Sub3TCB3002GPSAvailibilityL();
    void RunSubCase3L( CLcfCommonProcedures* aCommon );
    
    static void MakeTenPeriodicUpdatesFromBTGPSPsyL( TInt forLogging );
    };

_LIT(KTDBT1Step,"TDBT1Step");

#endif

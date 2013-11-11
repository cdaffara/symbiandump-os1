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
 @file TDBT4Step.h
 @internalTechnology
*/
#ifndef __TDBT4_STEP_H__
#define __TDBT4_STEP_H__
#include <test/TestExecuteStepBase.h>
#include "Te_LbsBtGpsPsyStepBase.h"
#include "LcfCommonProcedures.h"

class CTDBT4Step : public CTe_LbsBtGpsPsyStepBase
	{
public:
	CTDBT4Step();
	~CTDBT4Step();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

private:
	/**
	* Runs the test in the module
	*/
	void StartL(TInt aIndex);
	
    // Sub test case functions:
    void Sub0TCB1001PSYInfoAndPositionRequestingL();
    void RunSubCase0L( CLcfCommonProcedures* aCommon );
    
    void CheckEventsApiL();
    void RunSubCase6L( CLcfCommonProcedures* aCommon );

    void SetNightModeL(TBool aMode);
    void CheckNightModeSettingsL();
    
    static void MakeTenPeriodicUpdatesFromBTGPSPsyL( TInt forLogging );
	};

_LIT(KTDBT4Step,"TDBT4Step");

#endif

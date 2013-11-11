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
 @file TDBT2Step.h
 @internalTechnology
*/
#ifndef __TDBT2_STEP_H__
#define __TDBT2_STEP_H__
#include <test/TestExecuteStepBase.h>

#include "LbsSatellite.h"
#include "Te_LbsBtGpsPsyStepBase.h"
#include "LcfCommonProcedures.h"


class CTDBT2Step : public CTe_LbsBtGpsPsyStepBase
	{
public:
	CTDBT2Step();
	~CTDBT2Step();
	static CTDBT2Step* NewL();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();
	
private:
	void Sub0TCA1001MaxAgeWithMultipleClientsL();
	void Sub1TCA1002TimeoutsL();
	void Sub2TCA1003DataTypesL();
	void Sub3TCA1004PartialUpdatesL();
	void Sub4TCA1006CancelRequestsL();
	void Sub5TCA2001DataQualityStatusL();
	
	
	void RunSubCase0L(CLcfCommonProcedures* aCommon);
	void RunSubCase1L(CLcfCommonProcedures* aCommon);
	void RunSubCase2L(CLcfCommonProcedures* aCommon);
	void RunSubCase3L(CLcfCommonProcedures* aCommon);
	void RunSubCase4L(CLcfCommonProcedures* aCommon);
	void RunSubCase5L( CLcfCommonProcedures* aCommon);
	
	static void MakeTenPeriodicUpdatesFromBTGPSPsyL( TInt forLogging );
	
	};

_LIT(KTDBT2Step,"TDBT2Step"); // "TDBT2 - BT GPS PSY ATC"

#endif

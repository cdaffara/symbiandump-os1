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
 @file TDBT6Step.h
 @internalTechnology
*/
#ifndef __TDBT6_STEP_H__
#define __TDBT6_STEP_H__
#include <test/TestExecuteStepBase.h>
#include "te_lbsbtgpspsystepbase.h"
#include "te_lbsbtgpspsydefs.h"
#include "cbtpsypropertywatch.h"
#include <BtGpsPsyPrivateCRKeys.h>
#include "LcfCommonProcedures.h"
#include <lbs.h>
#include <lbsCommon.h>

class CTDBT6Step : public CTe_LbsBtGpsPsyStepBase
	{
public:
	CTDBT6Step();
	~CTDBT6Step();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

// Please add/modify your class members here:
private:
	void StartL(TInt aIndex);
    //Location req
    void LocationRequestTestL();
    void ConnectNeverPairedBeforeDeviceStepL();
    static TInt StartBTPSYWatch(TAny* parameter);
    static void RunBTPSYWatchL();
    void InvertNightModeL();
    void SetNightModeL(TBool aMode);
    void ConnectNeverPairedBeforeDeviceL( CLcfCommonProcedures* aCommon );
	};

_LIT(KTDBT6Step,"TDBT6Step");

#endif

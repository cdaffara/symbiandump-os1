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
// Test step which tests device list updates with the BT PSY
//

#if (!defined __CT_BTGPSPSY_LISTUPDATES_STEP_H__)
#define __CT_BTGPSPSY_LISTUPDATES_STEP_H__

//Includes
#include <lbs/lbsbtgpsconfig.h>
#include "ctlbsbtgpspsyteststepbase.h"

/** Class is used by the test to carry out a list update tests
 on the BT GPS PSY.
 */
class CT_LbsBtGpsPsyListUpdatesStep : public CT_LbsBtGpsPsyTestStepBase
	{
public:
	CT_LbsBtGpsPsyListUpdatesStep();
	~CT_LbsBtGpsPsyListUpdatesStep();

	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();
	
private:
	CLbsBtGpsConfig* iConfig;
	};

_LIT(KBTGPSPsyListUpdatesStep, "BtGpsPsyListUpdatesStep");

#endif	//__CT_BTGPSPSY_LISTUPDATES_STEP_H__

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
// Test step which tests reconnection with the BT PSY
//

#if (!defined __CT_BTGPSPSY_RECONNECTION_STEP_H__)
#define __CT_BTGPSPSY_RECONNECTION_STEP_H__

//Includes
#include "ctlbsbtgpspsyteststepbase.h"

/** Class is used by the test to carry out a reconnection tests
 on the BT GPS PSY.
 */
class CT_LbsBtGpsPsyReconnectionStep : public CT_LbsBtGpsPsyTestStepBase
	{
public:
	CT_LbsBtGpsPsyReconnectionStep();
	~CT_LbsBtGpsPsyReconnectionStep();

	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();
	
	};

_LIT(KBTGPSPsyReconnectionStep, "BtGpsPsyReconnectionStep");

#endif	//__CT_BTGPSPSY_RECONNECTION_STEP_H__

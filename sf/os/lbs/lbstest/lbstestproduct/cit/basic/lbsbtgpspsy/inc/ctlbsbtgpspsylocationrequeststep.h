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
// Test step which makes a standard Location Request
//

#if (!defined __CT_BTGPSPSY_REQUEST_LOCATION_STEP_H__)
#define __CT_BTGPSPSY_REQUEST_LOCATION_STEP_H__

//Includes
#include "ctlbsbtgpspsyteststepbase.h"

/** Class is used by the test to carry out a standard location request.
It expects a position with no error code to be returned.  Any other
status code will fail the test step.
 */
class CT_LbsBtGpsPsyLocationRequestStep : public CT_LbsBtGpsPsyTestStepBase 
	{
public:
	CT_LbsBtGpsPsyLocationRequestStep();
	~CT_LbsBtGpsPsyLocationRequestStep();

	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();
	
	};

_LIT(KBTGPSPsyLocationRequestStep, "BtGpsPsyLocationRequestStep");

#endif	//__CT_BTGPSPSY_REQUEST_LOCATION_STEP_H__

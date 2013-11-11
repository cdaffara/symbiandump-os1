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
// Test step which makes a standard Location Request whilst issuing PNOK commands
//

#if (!defined __CT_BTGPSPSY_PNOK_STEP_H__)
#define __CT_BTGPSPSY_PNOK_STEP_H__

//Includes
#include "ctlbsbtgpspsyteststepbase.h"
#include "ctlbsbtgpseventswatcher.h"

/** Class is used by the test to carry out PNOK tests.
 */
class CT_LbsBtGpsPsyPnokStep : public CT_LbsBtGpsPsyTestStepBase,
								public MBtGpsEventsEventObserver
	{
public:
	CT_LbsBtGpsPsyPnokStep();
	~CT_LbsBtGpsPsyPnokStep();

	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();
	
protected:
	virtual void OnEventsUpdated(const TUint aMask, TInt aValue);
	
private:
	CBtGpsEventsWatcher* iEventsWatcher;
	TInt iPnokBatteryLevel;
	};

_LIT(KBTGPSPsyPnokStep, "BtGpsPsyPnokStep");

#endif	//__CT_BTGPSPSY_PNOK_STEP_H__

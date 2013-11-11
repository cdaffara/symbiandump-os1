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
// The test step testing timing issues of notify position update scenarios.
// 
//

#if (!defined __TE_LBSPSYPOSUPDATETIMING_STEP__)
#define __TE_LBSPSYPOSUPDATETIMING_STEP__

#include <lbs.h>
#include <lbssatellite.h>

#include "te_lbspsybasestep.h"


/** The string name of the test step */
_LIT(KLbsNotifyPositionUpdateTimingStep, "NotifyPositionUpdateTiming");

/**
The test step testing timing issues of notify position update scenarios.
*/
class CTe_LbsNotifyPositionUpdateTimingStep : public CTe_PsyBaseStep
	{
public:
	IMPORT_C static CTe_LbsNotifyPositionUpdateTimingStep* NewL(MTe_LbsPsyStaticData* aPsyStaticData);
	IMPORT_C virtual ~CTe_LbsNotifyPositionUpdateTimingStep();
	
protected:
	virtual void ReallyDoTestStepImpL();
	
private:
	CTe_LbsNotifyPositionUpdateTimingStep(MTe_LbsPsyStaticData* aPsyStaticData);
	
private:
	
	};
		
#endif //__TE_LBSPSYPOSUPDATETIMING_STEP__

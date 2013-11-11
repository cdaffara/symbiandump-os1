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
// The test step testing the notify position update scenarios with errors.
// 
//

#if (!defined __TE_LBSPSYPOSUPDATEERR_STEP__)
#define __TE_LBSPSYPOSUPDATEERR_STEP__

#include <lbs.h>
#include <lbssatellite.h>

#include "te_lbspsybasestep.h"


/** The string name of the test step */
_LIT(KLbsNotifyPositionUpdateErrStep, "NotifyPositionUpdateErr");

/**
The test step testing simple notify position update scenarios.
*/
class CTe_LbsNotifyPositionUpdateErrStep : public CTe_PsyBaseStep
	{
public:
	IMPORT_C static CTe_LbsNotifyPositionUpdateErrStep* NewL(MTe_LbsPsyStaticData* aPsyStaticData);
	IMPORT_C virtual ~CTe_LbsNotifyPositionUpdateErrStep();
	
protected:
	IMPORT_C virtual void ReallyDoTestStepImpL();
	IMPORT_C CTe_LbsNotifyPositionUpdateErrStep(MTe_LbsPsyStaticData* aPsyStaticData);
	
	};
		

#endif //__TE_LBSPSYPOSUPDATEERR_STEP__

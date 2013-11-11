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
// The test step testing simple notify position update scenarios.
// 
//

#if (!defined __TE_LBSPSYPOSUPDATE_STEP__)
#define __TE_LBSPSYPOSUPDATE_STEP__

#include <lbs.h>
#include <lbssatellite.h>

#include "te_lbspsybasestep.h"


/** The string name of the test step */
_LIT(KLbsNotifyPositionUpdateStep, "NotifyPositionUpdate");

/**
The test step testing simple notify position update scenarios.
*/
class CTe_LbsNotifyPositionUpdateStep : public CTe_PsyBaseStep
	{
public:
	IMPORT_C static CTe_LbsNotifyPositionUpdateStep* NewL(MTe_LbsPsyStaticData* aPsyStaticData);
	IMPORT_C virtual ~CTe_LbsNotifyPositionUpdateStep();
	
protected:
	IMPORT_C virtual void ReallyDoTestStepImpL();
	
protected:
	IMPORT_C CTe_LbsNotifyPositionUpdateStep(MTe_LbsPsyStaticData* aPsyStaticData);
	
	};
		

#endif //__TE_LBSPSYPOSUPDATE_STEP__

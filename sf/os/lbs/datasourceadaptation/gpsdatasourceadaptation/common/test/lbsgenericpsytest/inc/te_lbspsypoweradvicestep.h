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
// The test step testing power advice scenarios for the psys.
// 
//

#if (!defined __TE_LBSPSYPOWERADVICE_STEP__)
#define __TE_LBSPSYPOWERADVICE_STEP__

#include "te_lbspsybasestep.h"

_LIT(KLbsPowerAdviceStep, "PowerAdvice");

/**
The test step testing simple notify position update scenarios for the AGPS PSY.
*/
class CTe_LbsPsyPowerAdviceStep : public CTe_PsyBaseStep
	{
public:
	IMPORT_C static CTe_LbsPsyPowerAdviceStep* NewL(MTe_LbsPsyStaticData* aPsyStaticData);
	IMPORT_C virtual ~CTe_LbsPsyPowerAdviceStep();
protected:
	IMPORT_C CTe_LbsPsyPowerAdviceStep(MTe_LbsPsyStaticData* aPsyStaticData);
	IMPORT_C virtual void ReallyDoTestStepImpL();
private:
	TUint GetInactivityTimeout();
	};
		

#endif //__TE_LBSPSYPOWERADVICE_STEP__

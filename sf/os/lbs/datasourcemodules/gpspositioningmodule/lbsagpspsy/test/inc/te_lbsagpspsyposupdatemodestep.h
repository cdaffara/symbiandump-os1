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
// The test step testing simple notify position update scenarios for the AGPS PSY.
// 
//

#if (!defined __TE_LBSAGPSPSYPOSUPDATEMODE_STEP__)
#define __TE_LBSAGPSPSYPOSUPDATEMODE_STEP__

#include "te_lbspsybasestep.h"

_LIT(KLbsNotifyPositionUpdateModeStep, "NotifyPositionUpdateMode");

/**
The test step testing simple notify position update scenarios for the AGPS PSY.
*/
class CTe_LbsAgpsPsyPosUpdateModeStep : public CTe_PsyBaseStep
	{
public:
	static CTe_LbsAgpsPsyPosUpdateModeStep* NewL(MTe_LbsPsyStaticData* aPsyStaticData);
	
protected:
	virtual void ReallyDoTestStepImpL();
	
private:
	CTe_LbsAgpsPsyPosUpdateModeStep(MTe_LbsPsyStaticData* aPsyStaticData);
		
	};
		

#endif //__TE_LBSAGPSPSYPOSUPDATEMODE_STEP__

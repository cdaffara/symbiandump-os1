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

#if (!defined __TE_LBSAGPSPSYPOSUPDATE_STEP__)
#define __TE_LBSAGPSPSYPOSUPDATE_STEP__

#include "te_lbspsyposupdatestep.h"

/**
The test step testing simple notify position update scenarios for the AGPS PSY.
*/
class CTe_LbsAgpsPsyPosUpdateStep : public CTe_LbsNotifyPositionUpdateStep
	{
public:
	static CTe_LbsAgpsPsyPosUpdateStep* NewL(MTe_LbsPsyStaticData* aPsyStaticData);
	
protected:
	virtual void ReallyDoTestStepImpL();
	
private:
	CTe_LbsAgpsPsyPosUpdateStep(MTe_LbsPsyStaticData* aPsyStaticData);
	
	};
		

#endif //__TE_LBSAGPSPSYPOSUPDATE_STEP__

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
// The test step testing the agps-specific notify position update scenarios with errors.
// 
//

#if (!defined __TE_LBSAGPSPSYPOSUPDATEERR_STEP__)
#define __TE_LBSAGPSPSYPOSUPDATEERR_STEP__

#include <lbs.h>
#include <lbssatellite.h>

#include "te_lbspsyposupdateerrstep.h"

/**
The test step testing simple notify position update scenarios.
*/
class CTe_LbsAgpsNotifyPositionUpdateErrStep : public CTe_LbsNotifyPositionUpdateErrStep
	{
public:
	IMPORT_C static CTe_LbsAgpsNotifyPositionUpdateErrStep* NewL(MTe_LbsPsyStaticData* aPsyStaticData);
	IMPORT_C virtual ~CTe_LbsAgpsNotifyPositionUpdateErrStep();
	
protected:
	virtual void ReallyDoTestStepImpL();
	
private:	
	CTe_LbsAgpsNotifyPositionUpdateErrStep(MTe_LbsPsyStaticData* aPsyStaticData);
	
	};
		

#endif //__TE_LBSAGPSPSYPOSUPDATEERR_STEP__

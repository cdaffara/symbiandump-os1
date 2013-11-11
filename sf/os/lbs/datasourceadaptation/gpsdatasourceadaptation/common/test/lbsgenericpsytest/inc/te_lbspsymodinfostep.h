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
// The test step checking module info related functionality.
// 
//

#if (!defined __TE_LBSPSYMODINFO_STEP__)
#define __TE_LBSPSYMODINFO_STEP__

#include <lbs.h>
#include <lbssatellite.h>

#include "te_lbspsybasestep.h"


/** The string name of the test step */
_LIT(KLbsModuleInfoStep, "ModuleInfo");

class CTe_PsyPositioner;

/**
The test step checking module info related functionality.
*/
class CTe_LbsModuleInfoStep : public CTe_PsyBaseStep
	{
public:
	IMPORT_C static CTe_LbsModuleInfoStep* NewL(MTe_LbsPsyStaticData* aPsyStaticData);
	IMPORT_C virtual ~CTe_LbsModuleInfoStep();
	
protected:
	virtual void ReallyDoTestStepImpL();
	
private:
	CTe_LbsModuleInfoStep(MTe_LbsPsyStaticData* aPsyStaticData);
	
	void RunModuleStatusTestL(CTe_PsyPositioner& aPositioner, TInt aDeviceStatus, 
			TInt aQualityStatus, TBool aExpectUpdate);
	
	};


#endif //__TE_LBSPSYMODINFO_STEP__

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
// The test for checking variability options for including/excluding PSYs and Location APIs 
// 
//

#ifndef __TE_LBSFEATURESPSY_STEP_H__
#define __TE_LBSFEATURESPSY_STEP_H__

#include "ctlbsfeaturesstep.h"

_LIT(KLbsFeaturesPsy, "LbsFeaturesPsy");

class CT_LbsFeaturesPsyStep : public CT_LbsFeaturesStep
{
public:
	static CT_LbsFeaturesPsyStep* New();
	virtual TVerdict doTestStepL();

protected:
	CT_LbsFeaturesPsyStep();
	
private:
	void TestFeatureL(const TDesC& aFeatureName,  const TUid& aFeatureUid);
	
	TBool iConfigUseGenerated;
};

#endif //__TE_LBSFEATURESPSY_STEP_H__

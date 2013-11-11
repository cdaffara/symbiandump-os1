// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// @file ctlbsfeaturesprivacystep.h
// 
//

#ifndef __CT_LBS_FEATURES_BTPSY_STEP_H__
#define __CT_LBS_FEATURES_BTPSY_STEP_H__

#include "ctlbsfeaturesstep.h"

// Literals used
_LIT(KLbsFeaturesBTPSY, "LbsFeaturesBTPSY");

class CT_LbsFeaturesBTPSYStep : public CT_LbsFeaturesStep
{
public:
	~CT_LbsFeaturesBTPSYStep();

	static CT_LbsFeaturesBTPSYStep* New();
	virtual TVerdict doTestStepL();

protected:
	CT_LbsFeaturesBTPSYStep();
	void ConstructL();
};

#endif // __CT_LBS_FEATURES_BTPSY_STEP_H__

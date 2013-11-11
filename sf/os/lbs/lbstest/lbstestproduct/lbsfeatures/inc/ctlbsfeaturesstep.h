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
// @file ctlbsfeaturesstep.h
// 
//

#ifndef __CT_LBS_FEATURES_STEP_H__
#define __CT_LBS_FEATURES_STEP_H__

#include <test/testexecutestepbase.h>

class CT_LbsFeaturesStep : public CTestStep
{
public:
enum TFeaturePresence
	{
	EFeatureAbsent,
	EFeaturePresent,
	EFeatureInconsistent
	};

public:
	virtual ~CT_LbsFeaturesStep();
	
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();

protected:
	CT_LbsFeaturesStep();
	void ConstructL();
	
	TFeaturePresence IsBTPSYFeaturePresent();
	
	TFeaturePresence IsLocationManagementFeaturePresent(TBool aIsLocationManagementFeaturePresent=ETrue);
	TFeaturePresence IsLocationPrivacyRequestAPIsFeaturePresent(TBool aIsLocationPrivacyRequestAPIsFeaturePresent=EFalse);
	TFeaturePresence IsLocationAdvancedDialogFeaturePresent(TBool aIsLocationAdvancedDialogFeaturePresent=EFalse);

	TBool IsFilePresent(const TDesC& aFileName);

private:
	RFs iFs;
};


#endif // __CT_LBS_FEATURES_STEP_H__

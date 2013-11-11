
// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// TestPlayerUtils.h
// Header file: Basic  tests.
// 
//

/**
 @file TestPlayerUtils.h
*/

#ifndef TESTPLAYERCAPS0006_H__
#define TESTPLAYERCAPS0006_H__

#include "TSI_MMF_SDEVSOUND_MPBASE.h"

/**
 * Load and initialise an audio file.
 *
 * @class CTestStepSDevSoundPlayDualTone
 *
 */
class CTestStepSDevSoundPlayDualToneCap : public CSDSMPTSBase
	{
public:
	static CTestStepSDevSoundPlayDualToneCap* NewL(const TDesC& aTestName, const TDesC& aSectionName, const TDesC& aKeyName);
	~CTestStepSDevSoundPlayDualToneCap();
	virtual TVerdict DoTestStepL();

private:
	CTestStepSDevSoundPlayDualToneCap(const TDesC& aTestName, const TDesC& aSectionName, const TDesC& aKeyName);

	TVerdict DoPlayDualTone();
	TVerdict TestPlayDualTone(TInt aFreq1, TInt aFreq2, TTimeIntervalMicroSeconds aDur);
	};
#endif

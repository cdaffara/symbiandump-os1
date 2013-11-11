
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

#ifndef TESTPLAYERCAPS0002_H__
#define TESTPLAYERCAPS0002_H__

#include "TSI_MMF_SDEVSOUND_MPBASE.h"

/**
 * Load and initialise an audio file.
 *
 * @class CTestStepSDevSoundPlaySimpleTone
 *
 */
class CTestStepSDevSoundPlaySimpleToneCap : public CSDSMPTSBase
	{
public:
	static CTestStepSDevSoundPlaySimpleToneCap* NewL(const TDesC& aTestName, const TDesC& aSectionName, const TDesC& aKeyName);
	~CTestStepSDevSoundPlaySimpleToneCap();
	virtual TVerdict DoTestStepL();

private:
	CTestStepSDevSoundPlaySimpleToneCap(const TDesC& aTestName, const TDesC& aSectionName, const TDesC& aKeyName);

	TVerdict DoPlaySimpleTone();
	TVerdict TestPlayTone(TInt aFreq, TTimeIntervalMicroSeconds aDur);

	};
#endif

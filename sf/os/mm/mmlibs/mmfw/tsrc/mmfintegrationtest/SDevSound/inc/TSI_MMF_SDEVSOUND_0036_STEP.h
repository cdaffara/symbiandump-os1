
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
// TestPlayerUtils.h
// Header file: Basic  tests.
// 
//

/**
 @file TestPlayerUtils.h
*/

#ifndef TESTPLAYERCAPS0036_H__
#define TESTPLAYERCAPS0036_H__

#include "TSI_MMF_SDEVSOUND_MPBASE.h"

/**
 * Load and initialise an audio file.
 *
 * @class CTestStepSDevSoundClientThreadCap
 *
 */
class CTestStepSDevSoundClientThreadNoCap : public CSDSMPTSBase
	{
public:
	static CTestStepSDevSoundClientThreadNoCap* NewL(const TDesC& aTestName, const TDesC& aProcessTestName,
													 const TDesC& aSectionName, const TDesC& aKeyName,
													 TBool aAttemptInitialCI);
	~CTestStepSDevSoundClientThreadNoCap();
	
	// From base class
	virtual TVerdict DoTestStepL();
	TInt DoPlaySimpleTone();
	TInt TestPlayTone(TInt aFreq, TTimeIntervalMicroSeconds aDur);

private:
	CTestStepSDevSoundClientThreadNoCap(const TDesC& aTestName, const TDesC& aProcessTestName, 
										const TDesC& aSectionName, const TDesC& aKeyName,
										TBool aAttemptInitialCI);
	TVerdict DoVerifyResult();
private:
	TBuf<KNameBufSize>	iProcessTestName;
	TBool  iServerResult;
	TThreadId iServerTid;
	TBool  iAttemptInitialCI;
	};


#endif

// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// TestStepMeasurePerformance.h
// This file contains Test step implementations for
// MMF performance assessment
//

#if (!defined __TESTSTEP_MMFPERFORMANCE_H__)
#define __TESTSTEP_MMFPERFORMANCE_H__

#include <MdaAudioSamplePlayer.h>

#include "TestStepMmfPfm.h"


enum TMmfProfileTestType
	{
	ENormal = 0,
	EWait = 1
	};

struct TMmfProfileTest
	{
	const TText* iTestName;
	const TText* iTestFileName;
	TMmfProfileTestType iTestType;
	};


/**
  *  RMmfPfmTest01
  */

class RMmfPfmTest001 : public RTestStepMmfPerformance, public MMdaAudioPlayerCallback
	{
public:
	RMmfPfmTest001(CTestSuite* aSuite);

	//From RTestStep
	TVerdict DoTestStepL();

	//From MMdaAudioPlayerCallback
	void MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration);
	void MapcPlayComplete(TInt aError);
	
	TVerdict DoProfileL(const TMmfProfileTest aProfileTest);
	void OpenAndPlayAudioFileL(const TDesC& aFileName);

public:
	TInt iInitError;
	TInt iPlayError;
	TTimeIntervalMicroSeconds iDuration;
	};

#endif (__TESTSTEP_MMFPERFORMANCE_H__)

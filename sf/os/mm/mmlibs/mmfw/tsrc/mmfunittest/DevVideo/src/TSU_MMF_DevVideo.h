
// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Header file: Basic  tests.
// 
//

/**
 @file TSU_MMF_DevVideo.h
*/

#ifndef __TSU_MMF_DEVVIDEO_H__
#define __TSU_MMF_DEVVIDEO_H__

#include <testframework.h>
#include <mmf/devvideo/devvideoplay.h>
#include <mmf/devvideo/devvideorecord.h>

class CTestSuite ;

// Flags for test step differentiation
enum TTestType
	{
	ETestValid = 0,
	ETestNegative,
	ETestDecoderOnly,
	ETestPostProcOnly,
	ETestNoPlugins,
	ETestDecoderFatal,
	ETestPostProcFatal,
	ETestBothFatal,
	ETestEncoderOnly,
	ETestPreProcOnly,
	ETestEncoderFatal,
	ETestPreProcFatal
	};


/**
 * It's a base class for all test steps.
 *
 * @class CTestMmfDevVideoStep 
 * @brief Test class that enables tests.
 *
 */
class CTestDevVideoStep : public CTestStep
	{
public:
	CTestDevVideoStep();
	~CTestDevVideoStep();
	static void CleanUp(TAny *aPtr) ;

protected:
	void WaitWithTimeout(TRequestStatus& aStatus, TInt aNumberOfMicroSeconds);
	TBool TimeComparison(const TUint aActual, const TUint aExpected, const TUint aDivation);
	// tests may optionally implement pre- and post-ambles
	virtual TVerdict DoTestStepPreambleL();
	virtual TVerdict DoTestStepPostambleL();

protected:
	CActiveScheduler* iActiveScheduler;
	};

#endif	// __TSU_MMF_DEVVIDEO_H__





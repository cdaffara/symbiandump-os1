// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This file contains testing steps for reading the bitrates of 
// audio files with different formats
// Tests opening sounds of various formats
// from a file with various parameters and constructor calls
// 
//

#ifndef __TEST_STEP_RECORDER_BITRATE__
#define __TEST_STEP_RECORDER_BITRATE__

#include <testframework.h>

#include "TestStepUnitMMFAudClient.h"
#include "TestSuiteUnitMMFAudClient.h"
#include "mmfclientaudiorecorder.h"

/**
 *
 * Read the bit rate of wav files.
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */

class CTestStepRecorderBitRateWav : public CTestStepUnitMMFAudClient,
							   	  	public MMdaObjectStateChangeObserver
	{
public:
	static CTestStepRecorderBitRateWav* NewL();
	CTestStepRecorderBitRateWav();
	~CTestStepRecorderBitRateWav();
	virtual TVerdict DoTestStepL();

	// from MMdaObjectStateChangeObserver
	void MoscoStateChangeEvent(CBase* /*aObject*/, 
								TInt /*aPreviousState*/, 
								TInt /*aCurrentState*/, 
								TInt aErrorcCode);

private:
	TInt iError;
	};
	
/**
 *
 * Read the bit rate of au files.
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */

class CTestStepRecorderBitRateAu : public CTestStepUnitMMFAudClient,
							   	   public MMdaObjectStateChangeObserver
	{
public:
	static CTestStepRecorderBitRateAu* NewL();
	CTestStepRecorderBitRateAu();
	~CTestStepRecorderBitRateAu();
	virtual TVerdict DoTestStepL();

	// from MMdaObjectStateChangeObserver
	void MoscoStateChangeEvent(CBase* /*aObject*/, 
								TInt /*aPreviousState*/, 
								TInt /*aCurrentState*/, 
								TInt aErrorcCode);

private:
	TInt iError;
	};
	
#endif //(__TEST_STEP_RECORDER_BITRATE__)

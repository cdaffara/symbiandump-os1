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
// This file contains Test step implementations for
// playing audio via the audio controller.
//

#if (!defined __TESTSTEP_PLAY_H__)
#define __TESTSTEP_PLAY_H__

#include <e32std.h>
#include <e32base.h>

#include "TS_MMF_ACTRL_TestStep.h"

/**
 *
 * CTestStepActrlPlay
 * This class provides a test step to play a audio clip
 *
 * @lib "TSU_MMF_ACTRL_01.lib"
 *
 *
 */
class CTestStepActrlPlay : public CTestStepActrl
{
public:
		CTestStepActrlPlay();
		~CTestStepActrlPlay();
		virtual TVerdict  DoTestStepL( void );
};

/**
 *
 * CTestStepPlayT
 * This class provides a test step to play an audio clip t times
 *
 * @lib "TSU_MMF_ACTRL_01.lib"
 *
 *
 */
class CTestStepPlayT : public CTestStepActrl
	 {
public:
	CTestStepPlayT( TInt aTimesToPlay );
	virtual ~CTestStepPlayT();
	virtual enum TVerdict DoTestStepL();
private:
	TInt iTimesToPlay ; 
	};

/**
 *
 * CTestStepPause
 * This class provides a test step to play an audio clip t times
 *
 * @lib "TSU_MMF_ACTRL_01.lib"
 *
 *
 */
class CTestStepPause : public CTestStepActrl
	 {
public:
	CTestStepPause();
	virtual ~CTestStepPause();
	virtual enum TVerdict DoTestStepL();
	};

/**
 *
 * CTestStepStop
 * This class provides a test step to test the stop function
 * on the Controller
 *
 * @lib "TSU_MMF_ACTRL_01.lib"
 *
 *
 */
class CTestStepStop : public CTestStepActrl
	 {
public:
	CTestStepStop();
	virtual ~CTestStepStop();
	virtual enum TVerdict DoTestStepL();
	};


// forward reference
class TConvParameters;

/**
 *
 * CTestStepConvertFormat
 * This class provides a test step to test format conversion
 *
 * @lib "TSU_MMF_ACTRL_01.lib"
 *
 *
 */
class CTestStepConvertFormat : public CTestStepActrl
	 {
public:
	CTestStepConvertFormat( TUint aTestIndex );
	virtual ~CTestStepConvertFormat();
	virtual enum TVerdict DoTestStepL();
	TVerdict DoConvertStepL( void );
	virtual TVerdict DoTestStepPreambleL(void);
	virtual TVerdict DoTestStepPostambleL(void);
	void SetRefFileNameL( const TDesC& aPathNameAndExtn );
	TMMFFileConfig& RefFileName();
	TVerdict CompareFilesL(const TDesC& aFile1,const TDesC& aFile2);
private:
	const TConvParameters *iTestParameters;

	TMMFFileConfig  iFileConfigRef;		// audio reference file 
	};

/**
 *
 * CTestStepRampVolume
 * This class tests the volume Ramp
 *
 * @lib "TSU_MMF_ACTRL_01.lib"
 *
 */
class CTestStepRampVolume : public CTestStepActrl
	 {
public:
	CTestStepRampVolume();
	virtual ~CTestStepRampVolume();
	virtual enum TVerdict DoTestStepL();
	virtual TVerdict DoTestStepPreambleL(void);
	virtual TVerdict DoTestStepPostambleL(void);
	};


#endif //(__TESTSTEP_PLAY_H__)

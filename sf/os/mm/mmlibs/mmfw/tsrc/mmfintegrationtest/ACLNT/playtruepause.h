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
//

#ifndef PLAYTRUEPAUSE_H_
#define PLAYTRUEPAUSE_H_

#include <testframework.h>
#include <mmf/server/sounddevice.h>
#include "mdaaudiosampleplayer.h" 
#include "TSI_MMFACLNT.h"
#include "truepauseconstants.h"

// Panic function
static void Panic(const TDesC &aCategory, TInt aReason)
	{
	User::Panic(aCategory, aReason);
	}

/*
 *========================================================================================================
 * Base class for TruePause Tone utility test steps in this suite.
 *========================================================================================================
 */

class RMdaPlayerUtilityTestBase: public RAsyncTestStep, public MMdaAudioPlayerCallback
	{
public:
	// from RAsyncTestStep
	void KickoffTestL();
	void CloseTest();
protected:
	RMdaPlayerUtilityTestBase(const TDesC& aTestName, const TDesC& aSectName);
	~RMdaPlayerUtilityTestBase();
	virtual void DoKickoffTestL() = 0;
	virtual void DoTimerCallback();
	static TInt TimerCallback(TAny*);
	void StartTimer(TTimeIntervalMicroSeconds32 aWaitTime);

	// Executes events of MdaToneUtility using a Finite State Machine
	virtual void Fsm(TMdaAudioUtilityEvent aMdaAudioUtilityEvent, TInt aError);
	
	// From MMdaAudioPlayerCallback
	void MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& /*aDuration*/);
	void MapcPlayComplete(TInt aError);

	//Auxiliary functions
	static void Panic(const TDesC &aCategory, TInt aReason);
protected:
	TMdaAudioUtilityState iAudioUtilityState;
	CMdaAudioPlayerUtility* iPlayerUtility;
	CPeriodic* iTimer;
	TFileName iFilename;
	TBuf<KNameBufSize> iSectName;		// Section name for retrieving filename
protected:
	TBool iStop;
	TBool iQuery;
	TInt iVolume;
	TInt iBalance;
	TBool iResume;
	TInt iPauseResumeTimes;
	TBool iPauseApplied;
	TInt iDuration;
	TInt iAllFormat;
	TBool iPositionSupported;
	TBool iMiliSec;
	};

/*
 *========================================================================================================
 * MM-MMF-ACLNT-I-0185-HP
 *========================================================================================================
 */

class RMdaPlayerUtiliyPauseStopAndPlayWavTest : public RMdaPlayerUtilityTestBase
	{
public:
	static RMdaPlayerUtiliyPauseStopAndPlayWavTest* NewL(const TDesC& aTestName, const TDesC& aSectName);
private:
	RMdaPlayerUtiliyPauseStopAndPlayWavTest(const TDesC& aTestName, const TDesC& aSectName);
	void DoKickoffTestL();
	};

/*
 *========================================================================================================
 * MM-MMF-ACLNT-I-0186-HP
 *========================================================================================================
 */

class RMdaPlayerUtilityQueryPauseAndPlayWavTest : public RMdaPlayerUtilityTestBase
	{
public:
	static RMdaPlayerUtilityQueryPauseAndPlayWavTest* NewL(const TDesC& aTestName, const TDesC& aSectName);
private:
	RMdaPlayerUtilityQueryPauseAndPlayWavTest(const TDesC& aTestName, const TDesC& aSectName);
	void DoKickoffTestL();
	};

/*
 *========================================================================================================
 * MM-MMF-ACLNT-I-0187-HP
 *========================================================================================================
 */

class RMdaPlayerUtilityPauseAndPlaySqnTest : public RMdaPlayerUtilityTestBase
	{
public:
	static RMdaPlayerUtilityPauseAndPlaySqnTest* NewL(const TDesC& aTestName, const TDesC& aSectName);
private:
	RMdaPlayerUtilityPauseAndPlaySqnTest(const TDesC& aTestName, const TDesC& aSectName);
	void DoKickoffTestL();
	};

/*
 *========================================================================================================
 * MM-MMF-ACLNT-I-0188-HP
 *========================================================================================================
 */

class RMdaPlayerUtilityPauseAndPlayThreeTimesSqnTest : public RMdaPlayerUtilityTestBase
	{
public:
	static RMdaPlayerUtilityPauseAndPlayThreeTimesSqnTest* NewL(const TDesC& aTestName, const TDesC& aSectName);
private:
	RMdaPlayerUtilityPauseAndPlayThreeTimesSqnTest(const TDesC& aTestName, const TDesC& aSectName);
	void DoKickoffTestL();
	};

/*
 *========================================================================================================
 * MM-MMF-ACLNT-I-0189-HP
 *========================================================================================================
 */

class RMdaPlayerUtilityPauseStopAndPlaySqnTest : public RMdaPlayerUtilityTestBase
	{
public:
	static RMdaPlayerUtilityPauseStopAndPlaySqnTest* NewL(const TDesC& aTestName, const TDesC& aSectName);
private:
	RMdaPlayerUtilityPauseStopAndPlaySqnTest(const TDesC& aTestName, const TDesC& aSectName);
	void DoKickoffTestL();
	};

/*
 *========================================================================================================
 * MM-MMF-ACLNT-I-0190-HP
 *========================================================================================================
 */

class RMdaPlayerUtilityGetVolumeAndBalancePauseAndPlaySqnTest : public RMdaPlayerUtilityTestBase
	{
public:
	static RMdaPlayerUtilityGetVolumeAndBalancePauseAndPlaySqnTest* NewL(const TDesC& aTestName, const TDesC& aSectName);
private:
	RMdaPlayerUtilityGetVolumeAndBalancePauseAndPlaySqnTest(const TDesC& aTestName, const TDesC& aSectName);
	void DoKickoffTestL();
	};

/*
 *========================================================================================================
 * MM-MMF-ACLNT-I-0191-HP
 *========================================================================================================
 */

class RMdaPlayerUtilityPauseAndPlayFormatsTest : public RMdaPlayerUtilityTestBase
	{
public:
	static RMdaPlayerUtilityPauseAndPlayFormatsTest* NewL(const TDesC& aTestName, const TDesC& aSectName);
private:
	RMdaPlayerUtilityPauseAndPlayFormatsTest(const TDesC& aTestName, const TDesC& aSectName);
	void DoKickoffTestL();
	// From MMdaAudioPlayerCallback
	void MapcPlayComplete(TInt aError);
	};

#endif// __PLAYTRUEPAUSE_H__


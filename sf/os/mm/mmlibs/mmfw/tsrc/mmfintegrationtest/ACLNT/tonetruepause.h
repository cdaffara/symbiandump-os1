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
//

#ifndef TONETRUEPAUSE_H
#define TONETRUEPAUSE_H

#include <testframework.h>
#include "TSI_MMFACLNT.h"
#include "truepauseconstants.h"

/*
 *========================================================================================================
 * Base class for TruePause Tone utility test steps in this suite.
 *========================================================================================================
 */

class RMdaToneUtilityTestBase: public RAsyncTestStep, public MMdaAudioToneObserver
	{
public:
	// from RAsyncTestStep
	void KickoffTestL();
	void CloseTest();
protected:
	RMdaToneUtilityTestBase(const TDesC& aTestName, const TDesC& aSectName);
	~RMdaToneUtilityTestBase();
	virtual void DoKickoffTestL() = 0;
	virtual void DoTimerCallback() = 0;
	static TInt TimerCallback(TAny*);
	void StartTimer(TTimeIntervalMicroSeconds32 aWaitTime);

	// Executes events of MdaToneUtility using a Finite State Machine
	virtual void Fsm(TMdaAudioUtilityEvent aMdaAudioUtilityEvent, TInt aError);
	
	// From MMdaAudioToneObserver
	virtual void MatoPrepareComplete(TInt aError);
	virtual void MatoPlayComplete(TInt aError);
	
protected:
	TMdaAudioUtilityState iToneUtilityState;
	CMdaAudioToneUtility* iToneUtility;
	CPeriodic* iTimer;
	TBool iWait;
	TBool iStop;
	TBool iConfig;
	TBool iPrepare;
	TBool iNegative;	
	TInt iCount;
	TInt iVolume;
	TInt iBalance;
	TInt iDuration;
	TInt iPause;
	TInt iExpected;
	
	//To track elapsed time
	TTime iEndTime;
	TTime iStartTime;
	
	// Handle for the audio file for playing and recording
	RFile iFile;
	TFileName iFilename;
	// Handle to a file server session
	RFs iFs;
	TBuf<KNameBufSize> iSectName;		// Section name for retrieving filename
	};

/*
 *========================================================================================================
 * MM-MMF-ACLNT-I-0168-HP
 *========================================================================================================
 */
class RMdaToneUtilityResumeTest : public RMdaToneUtilityTestBase
	{
public:
	static RMdaToneUtilityResumeTest* NewL(const TDesC& aTestName, const TDesC& aSectName);
private:
	RMdaToneUtilityResumeTest(const TDesC& aTestName, const TDesC& aSectName);
	void DoKickoffTestL();
	void DoTimerCallback();
	};

/*
 *========================================================================================================
 * MM-MMF-ACLNT-I-0169-HP
 *========================================================================================================
 */
class RMdaToneUtilityResumeThreeTest : public RMdaToneUtilityTestBase
	{
public:
	static RMdaToneUtilityResumeThreeTest* NewL(const TDesC& aTestName, const TDesC& aSectName);
private:
	RMdaToneUtilityResumeThreeTest(const TDesC& aTestName, const TDesC& aSectName);
	void DoKickoffTestL();
	void DoTimerCallback();
	};

/*
 *========================================================================================================
 * MM-MMF-ACLNT-I-0170-HP
 *========================================================================================================
 */
class RMdaToneUtilityPlayPauseStopPlayTest : public RMdaToneUtilityTestBase
	{
public:
	static RMdaToneUtilityPlayPauseStopPlayTest* NewL(const TDesC& aTestName, const TDesC& aSectName);
private:
	RMdaToneUtilityPlayPauseStopPlayTest(const TDesC& aTestName, const TDesC& aSectName);
	void DoKickoffTestL();
	void DoTimerCallback();
	};

/*
 *========================================================================================================
 * MM-MMF-ACLNT-I-0171-HP
 *========================================================================================================
 */
class RMdaToneUtilityConfigResumeTest : public RMdaToneUtilityTestBase
	{
public:
	static RMdaToneUtilityConfigResumeTest* NewL(const TDesC& aTestName, const TDesC& aSectName);
private:
	RMdaToneUtilityConfigResumeTest(const TDesC& aTestName, const TDesC& aSectName);
	void DoKickoffTestL();
	void DoTimerCallback();
	};

/*
 *========================================================================================================
 * MM-MMF-ACLNT-I-0172-HP
 *========================================================================================================
 */
class RMdaToneUtilityPrepareInPauseTest : public RMdaToneUtilityTestBase
	{
public:
	static RMdaToneUtilityPrepareInPauseTest* NewL(const TDesC& aTestName, const TDesC& aSectName);
private:
	RMdaToneUtilityPrepareInPauseTest(const TDesC& aTestName, const TDesC& aSectName);
	void DoKickoffTestL();
	void DoTimerCallback();
	// From MMdaAudioToneObserver
	void MatoPrepareComplete(TInt aError);
	};

/*
 *========================================================================================================
 * MM-MMF-ACLNT-I-0173-HP
 *========================================================================================================
 */
class RMdaToneUtilityPauseBeforePlayTest : public RMdaToneUtilityTestBase
	{
public:
	static RMdaToneUtilityPauseBeforePlayTest* NewL(const TDesC& aTestName, const TDesC& aSectName);
private:
	RMdaToneUtilityPauseBeforePlayTest(const TDesC& aTestName, const TDesC& aSectName);
	void DoKickoffTestL();
	void DoTimerCallback();
	// From MMdaAudioToneObserver
	void MatoPrepareComplete(TInt aError);
	// Executes events of MdaToneUtility using a Finite State Machine
	void Fsm(TMdaAudioUtilityEvent aMdaAudioUtilityEvent, TInt aError);
	};

/*
 *========================================================================================================
 * MM-MMF-ACLNT-I-0174-HP
 *========================================================================================================
 */
class RMdaToneUtilityNegPauseResumeTest : public RMdaToneUtilityTestBase
	{
public:
	static RMdaToneUtilityNegPauseResumeTest* NewL(const TDesC& aTestName, const TDesC& aSectName);
private:
	RMdaToneUtilityNegPauseResumeTest(const TDesC& aTestName, const TDesC& aSectName);
	void DoKickoffTestL();
	void DoTimerCallback();
	// From MMdaAudioToneObserver
	void MatoPlayComplete(TInt aError);
	};

/*
 *========================================================================================================
 * MM-MMF-ACLNT-I-0183-HP
 *========================================================================================================
 */
class RMdaToneUtilityPauseNonA3fTest : public RMdaToneUtilityTestBase
	{
public:
	static RMdaToneUtilityPauseNonA3fTest* NewL(const TDesC& aTestName, const TDesC& aSectName);
private:
	RMdaToneUtilityPauseNonA3fTest(const TDesC& aTestName, const TDesC& aSectName);
	void DoKickoffTestL();
	void DoTimerCallback();
	// From MMdaAudioToneObserver
	void MatoPrepareComplete(TInt aError);
	// Executes events of MdaToneUtility using a Finite State Machine
	void Fsm(TMdaAudioUtilityEvent aMdaAudioUtilityEvent, TInt aError);
	};
	
#endif// __TONETRUEPAUSE_H__

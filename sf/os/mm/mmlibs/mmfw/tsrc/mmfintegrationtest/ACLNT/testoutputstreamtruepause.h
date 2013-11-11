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

#ifndef TESTOUTPUTSTREAMTRUEPAUSE_H
#define TESTOUTPUTSTREAMTRUEPAUSE_H

#include <testframework.h>
#include "TSI_MMFACLNT.h"
#include "mdaaudiooutputstream.h"
#include "truepauseconstants.h"

/*
 *========================================================================================================
 * Constant declarations
 *========================================================================================================
 */

const TInt KBufSize = 1024; //1K buffer size
const TInt KBufferNum = 5; //5 buffers per array

/*
 *========================================================================================================
 * Base class for OutputStream test steps in this suite.
 *========================================================================================================
 */

class RMdaOutputStreamTestBase: public RAsyncTestStep, public MMdaAudioOutputStreamCallback
	{
public:
	// from RAsyncTestStep
	void KickoffTestL();
	void CloseTest();
	void CallStopTest(TInt aError);
protected:
	RMdaOutputStreamTestBase(const TDesC& aTestName, const TDesC& aSectName);
	~RMdaOutputStreamTestBase();
	virtual void DoKickoffTestL() = 0;
	virtual void DoTimerCallback() = 0;
	static TInt TimerCallback(TAny*);
	void StartTimer(TTimeIntervalMicroSeconds32 aWaitTime);

	// Executes events of MdaToneUtility using a Finite State Machine
	virtual void Fsm(TMdaAOSEvent aMdaAudioOutputStreamEvent, TInt aError);
	
	// From MMdaAudioOutputStreamObserver
	virtual void MaoscBufferCopied(TInt aError, const TDesC8 &aBuffer);
	virtual void MaoscPlayComplete(TInt aError);
	virtual void MaoscOpenComplete(TInt aError);
	
protected:
	TMdaAOSState iAudioOutputStreamState;
	CMdaAudioOutputStream* iAudioOutputStream;
	CPeriodic* iTimer;
	TBool iWait;
	TBool iStop;
	TBool iConfig;
	TBool iInvalidConfig;
	TBool iGetBytes;
	TInt iCount;
	TInt iVolume;
	TInt iBalance;
	TInt iDuration;
	TInt iPause;	
	TInt iBytes;
	TTimeIntervalMicroSeconds iPosition;
	
	TBuf<KNameBufSize> iSectName;		// Section name
	// Handle for the audio file for playing and recording
	RFile iFile;
	TFileName iFilename;
	// Handle to a file server session
	RFs iFs;
	RBuf8 iBuffer;		// Buffer to read data
	
	};

/*
 *========================================================================================================
 * MM-MMF-ACLNT-I-0175-HP
 *========================================================================================================
 */
class RMdaOutputStreamResumeTest : public RMdaOutputStreamTestBase
	{
public:
	static RMdaOutputStreamResumeTest* NewL(const TDesC& aTestName, const TDesC& aSectName);
private:
	RMdaOutputStreamResumeTest(const TDesC& aTestName, const TDesC& aSectName);
	void DoKickoffTestL();
	void DoTimerCallback();
	};

/*
 *========================================================================================================
 * MM-MMF-ACLNT-I-0176-HP
 *========================================================================================================
 */
class RMdaOutputStreamResumeThriceTest : public RMdaOutputStreamTestBase
	{
public:
	static RMdaOutputStreamResumeThriceTest* NewL(const TDesC& aTestName, const TDesC& aSectName);
private:
	RMdaOutputStreamResumeThriceTest(const TDesC& aTestName, const TDesC& aSectName);
	void DoKickoffTestL();
	void DoTimerCallback();
	};

/*
 *========================================================================================================
 * MM-MMF-ACLNT-I-0177-HP
 *========================================================================================================
 */
class RMdaOutputStreamConfigInPauseTest : public RMdaOutputStreamTestBase
	{
public:
	static RMdaOutputStreamConfigInPauseTest* NewL(const TDesC& aTestName, const TDesC& aSectName);
private:
	RMdaOutputStreamConfigInPauseTest(const TDesC& aTestName, const TDesC& aSectName);
	void DoKickoffTestL();
	void DoTimerCallback();
	};

/*
 *========================================================================================================
 * MM-MMF-ACLNT-I-0178-HP
 *========================================================================================================
 */
class RMdaOutputStreamInvalidConfigInPauseTest : public RMdaOutputStreamTestBase
	{
public:
	static RMdaOutputStreamInvalidConfigInPauseTest* NewL(const TDesC& aTestName, const TDesC& aSectName);
private:
	RMdaOutputStreamInvalidConfigInPauseTest(const TDesC& aTestName, const TDesC& aSectName);
	void DoKickoffTestL();
	void DoTimerCallback();
	};

/*
 *========================================================================================================
 * MM-MMF-ACLNT-I-0179-HP
 *========================================================================================================
 */
class RMdaOutputStreamGetBytesInPauseTest : public RMdaOutputStreamTestBase
	{
public:
	static RMdaOutputStreamGetBytesInPauseTest* NewL(const TDesC& aTestName, const TDesC& aSectName);
private:
	RMdaOutputStreamGetBytesInPauseTest(const TDesC& aTestName, const TDesC& aSectName);
	void DoKickoffTestL();
	void DoTimerCallback();
	};

/*
 *========================================================================================================
 * MM-MMF-ACLNT-I-0180-HP
 *========================================================================================================
 */
class RMdaOutputStreamPlayPauseStopPlayTest : public RMdaOutputStreamTestBase
	{
public:
	static RMdaOutputStreamPlayPauseStopPlayTest* NewL(const TDesC& aTestName, const TDesC& aSectName);
private:
	RMdaOutputStreamPlayPauseStopPlayTest(const TDesC& aTestName, const TDesC& aSectName);
	void DoKickoffTestL();
	void DoTimerCallback();
	// Executes events of MdaToneUtility using a Finite State Machine
	void Fsm(TMdaAOSEvent aMdaAudioOutputStreamEvent, TInt aError);
	// From MMdaAudioOutputStreamObserver
	void MaoscPlayComplete(TInt aError);
	};

/*
 *========================================================================================================
 * MM-MMF-ACLNT-I-0181-HP
 *========================================================================================================
 */
class RMdaOutputStreamPauseResumeBeforeWriteTest : public RMdaOutputStreamTestBase
	{
public:
	static RMdaOutputStreamPauseResumeBeforeWriteTest* NewL(const TDesC& aTestName, const TDesC& aSectName);
private:
	RMdaOutputStreamPauseResumeBeforeWriteTest(const TDesC& aTestName, const TDesC& aSectName);
	void DoKickoffTestL();
	void DoTimerCallback();
	// Executes events of MdaToneUtility using a Finite State Machine
	void Fsm(TMdaAOSEvent aMdaAudioOutputStreamEvent, TInt aError);
	};
	
/*
 *========================================================================================================
 * MM-MMF-ACLNT-I-0182-HP
 *========================================================================================================
 */
class RMdaOutputStreamInvalidFormatsTest : public RMdaOutputStreamTestBase
	{
public:
	static RMdaOutputStreamInvalidFormatsTest* NewL(const TDesC& aTestName, const TDesC& aSectName);
private:
	RMdaOutputStreamInvalidFormatsTest(const TDesC& aTestName, const TDesC& aSectName);
	void DoKickoffTestL();
	void DoTimerCallback();
	// Executes events of MdaToneUtility using a Finite State Machine
	void Fsm(TMdaAOSEvent aMdaAudioOutputStreamEvent, TInt aError);
	};

/*
 *========================================================================================================
 * MM-MMF-ACLNT-I-0184-HP
 *========================================================================================================
 */
class RMdaOutputStreamPauseNonA3FTest : public RMdaOutputStreamTestBase
	{
public:
	static RMdaOutputStreamPauseNonA3FTest* NewL(const TDesC& aTestName, const TDesC& aSectName);
private:
	RMdaOutputStreamPauseNonA3FTest(const TDesC& aTestName, const TDesC& aSectName);
	void DoKickoffTestL();
	void DoTimerCallback();
	// Executes events of MdaToneUtility using a Finite State Machine
	void Fsm(TMdaAOSEvent aMdaAudioOutputStreamEvent, TInt aError);
	};	

#endif// __TESTOUTPUTSTREAMTRUEPAUSE_H__

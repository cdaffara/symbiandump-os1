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
// Declarations of Integration test step classes for A3F DevSound Tone cases.
// 
//

#ifndef TSI_A3F_DEVSOUND_TONE_H
#define TSI_A3F_DEVSOUND_TONE_H

// User includes
#include "tsi_a3f_devsound_teststep.h"

// Enumerations
enum TDevSoundToneState
	{
	EStateToneStart = 0,
	EStateToneInitializing,
	EStateTonePlaying,
	EStateTonePaused
	};

enum TDevSoundToneEvents
	{
	EEventToneStart = 0,
	EEventInitCompleteTone,
	EEventToneTimerComplete
	};

class RTestStepA3FDevSoundToneBase : public RAsyncTestStep,
									 public MDevSoundObserver
	{
public:
	// From RAsyncTestStep
	virtual void KickoffTestL();
	void CloseTest();

protected:
	RTestStepA3FDevSoundToneBase(const TDesC& aTestName, const TDesC& aSectName);
	RTestStepA3FDevSoundToneBase(const TDesC& aTestName);

	virtual void DoKickoffTestL() = 0;

	// Executes the tone events of DevSound using a Finite State Machine
	virtual void Fsm(TDevSoundToneEvents aDevSoundEvent, TInt aError) = 0;

	// From MDevSoundObserver
	virtual void InitializeComplete(TInt aError);
	virtual void ToneFinished(TInt aError);
	virtual void BufferToBeFilled(CMMFBuffer* aBuffer);
	virtual void PlayError(TInt aError);
	virtual void BufferToBeEmptied(CMMFBuffer* aBuffer);
	virtual void RecordError(TInt aError);
	virtual void ConvertError(TInt aError);
	virtual void DeviceMessage(TUid aMessageType, const TDesC8& aMsg);
	
	// Timer Callback
	static TInt TimerCallback(TAny*);
	virtual void DoTimerCallback();

	TUint ConvertDesToTMMFSampleRate(const TDesC& aSampleRateString);
	
protected:

	CMMFDevSound* iMMFDevSound;
	TBuf<KNameSize> iSectName;
	CPeriodic* iTimer;
	TCallBack iCallBack;
	TDevSoundToneState iState;
	TPtrC iSampleRate;

	};

/*
 *
 * RStepA3FDevSoundDTMFTonePlay
 * - Set DTMF lengths and play DTMF string
 *
 */
class RStepA3FDevSoundDTMFTonePlay : public RTestStepA3FDevSoundToneBase
	{
public:
	static RStepA3FDevSoundDTMFTonePlay* NewL(const TDesC& aTestName);

private:
	RStepA3FDevSoundDTMFTonePlay(const TDesC& aTestName);

	// From RTestStepA3FDevSoundToneBase
	virtual void DoKickoffTestL();
	virtual void Fsm(TDevSoundToneEvents aDevSoundEvent, TInt aError);

	};

/*
 *
 * RStepA3FDevSoundNormalAndDualTonePlay
 * - Set tone repeats and play normal tone
 * - Set tone repeats and play dual tone
 *
 */
class RStepA3FDevSoundNormalAndDualTonePlay : public RTestStepA3FDevSoundToneBase
	{
public:
	static RStepA3FDevSoundNormalAndDualTonePlay* NewL(const TDesC& aTestName, const TDesC& aSectName);

private:
	RStepA3FDevSoundNormalAndDualTonePlay(const TDesC& aTestName, const TDesC& aSectName);

	// From RTestStepA3FDevSoundToneBase
	virtual void DoKickoffTestL();
	virtual void Fsm(TDevSoundToneEvents aDevSoundEvent, TInt aError);

private:
	TInt iRepeat;
	TBool iDualTone;

	};

/*
 *
 * RStepA3FDevSoundInitializeWhileTonePlaying
 *
 */
class RStepA3FDevSoundInitializeWhileTonePlaying : public RTestStepA3FDevSoundToneBase
	{
public:
	static RStepA3FDevSoundInitializeWhileTonePlaying* NewL(const TDesC& aTestName);

private:
	RStepA3FDevSoundInitializeWhileTonePlaying(const TDesC& aTestName);

	// From RTestStepA3FDevSoundToneBase
	virtual void DoKickoffTestL();
	virtual void Fsm(TDevSoundToneEvents aDevSoundEvent, TInt aError);

	};

/*
 *
 * RStepA3FDevSoundNotSupportedPlayFixedSeq
 *
 */
class RStepA3FDevSoundNotSupportedPlayFixedSeq : public RTestStepA3FDevSoundToneBase
	{
public:
	static RStepA3FDevSoundNotSupportedPlayFixedSeq* NewL(const TDesC& aTestName);

private:
	RStepA3FDevSoundNotSupportedPlayFixedSeq(const TDesC& aTestName);

	// From RTestStepA3FDevSoundToneBase
	virtual void DoKickoffTestL();
	virtual void Fsm(TDevSoundToneEvents aDevSoundEvent, TInt aError);

	};

/*
 *
 * RStepA3FDevSoundInvalidInitializeForTone
 *
 */
class RStepA3FDevSoundInvalidInitializeForTone : public RTestStepA3FDevSoundToneBase
	{
public:
	static RStepA3FDevSoundInvalidInitializeForTone* NewL(const TDesC& aTestName);

private:
	RStepA3FDevSoundInvalidInitializeForTone(const TDesC& aTestName);

	// From RTestStepA3FDevSoundToneBase
	virtual void DoKickoffTestL();
	virtual void Fsm(TDevSoundToneEvents aDevSoundEvent, TInt aError);

	};

/*
 *
 * RStepA3FDevSoundCancelInitializeTone
 *
 */
class RStepA3FDevSoundCancelInitializeTone : public RTestStepA3FDevSoundToneBase
	{
public:
	static RStepA3FDevSoundCancelInitializeTone* NewL(const TDesC& aTestName, const TDesC& aSectName);

private:
	RStepA3FDevSoundCancelInitializeTone(const TDesC& aTestName, const TDesC& aSectName);

	// From RTestStepA3FDevSoundToneBase
	virtual void DoKickoffTestL();
	virtual void Fsm(TDevSoundToneEvents aDevSoundEvent, TInt aError);
	
	// From TimerCallback
	virtual void DoTimerCallback();
		
	// From MDevSoundObserver
	virtual void InitializeComplete(TInt aError);
	
	};
	
/*
 *
 * RStepA3FDevSoundEmptyBuffersInitializedForTone
 *
 */
class RStepA3FDevSoundEmptyBuffersInitializedForTone : public RTestStepA3FDevSoundToneBase
	{
public:
	static RStepA3FDevSoundEmptyBuffersInitializedForTone* NewL(const TDesC& aTestName, const TDesC& aSectName);

private:
	RStepA3FDevSoundEmptyBuffersInitializedForTone(const TDesC& aTestName, const TDesC& aSectName);

	// From RTestStepA3FDevSoundToneBase
	virtual void DoKickoffTestL();
	virtual void Fsm(TDevSoundToneEvents aDevSoundEvent, TInt aError);
	
	};

/*
 *
 * RStepA3FDevSoundEmptyBuffersWhileTonePlaying
 *
 */
class RStepA3FDevSoundEmptyBuffersWhileTonePlaying : public RTestStepA3FDevSoundToneBase
	{
public:
	static RStepA3FDevSoundEmptyBuffersWhileTonePlaying* NewL(const TDesC& aTestName);

private:
	RStepA3FDevSoundEmptyBuffersWhileTonePlaying(const TDesC& aTestName);

	// From RTestStepA3FDevSoundToneBase
	virtual void DoKickoffTestL();
	virtual void Fsm(TDevSoundToneEvents aDevSoundEvent, TInt aError);

	};
	
#endif // TSI_A3F_DEVSOUND_TONE_H

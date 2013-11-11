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
//

#ifndef __CAPTESTSTEP_H__
#define __CAPTESTSTEP_H__

#include <mdaaudiosampleeditor.h>
#include <simulprocserver.h>
#include <mmf/server/sounddevice.h>

const TInt KDevSoundPriorityLow=0;
const TInt KDevSoundPriorityHigh=100;

/**
 *
 * CActiveListener provides the asynchronous operation
 * of an active object
 *
 * @lib "TSI_MMF_SDEVSOUND_SUITE.lib"
 *
 *
 */
class CActiveListener : public CActive
	{
public:
	CActiveListener() : CActive(CActive::EPriorityStandard) 
		{ 
		CActiveScheduler::Add(this);
		}
	void InitialiseActiveListener()
		{
		iStatus = KRequestPending; 
		SetActive();
		}
	// From CActive
	virtual void RunL()
		{
		CActiveScheduler::Stop();
		}
	virtual void DoCancel()
		{
		CActiveScheduler::Stop();
		}
	};

class CDevSoundTestStepBase : public CSimulProcTestStep, public MDevSoundObserver
	{
public:
	static CDevSoundTestStepBase* NewL();
	void ConstructL();
	virtual void StartProcessing(TRequestStatus& aStatus);
	virtual TVerdict EndProcessingAndReturnResult(TDes8& aMessage);		
	virtual ~CDevSoundTestStepBase();

protected:
	enum TCallback 
		{
		EInitComplete,
		EToneFinished,
		EBuffToFill,
		EPlayError,
		EBuffToEmpty,
		ERecError,
		EConvertMsg,
		EDeviceMsg
		};

	void InitializeComplete (TInt aError);
	void ToneFinished (TInt aError);
	void BufferToBeFilled (CMMFBuffer* aBuffer);
	void PlayError (TInt aError);
	void BufferToBeEmptied (CMMFBuffer* aBuffer);
	void RecordError (TInt aError);
	void ConvertError (TInt aError);
	void DeviceMessage (TUid aMessageType, const TDesC8& aMsg);

	TVerdict ConstructDevSound();
	TVerdict DoTestStepPreambleL();
	TVerdict TestInitialize(TMMFState aMode);
	TVerdict TestPlayTone(TInt aFreq, TTimeIntervalMicroSeconds aDur);
	void TestSetVolume(TInt aVol);
	void TestSetPriority(TInt aPriority);
	void TestSetGain(TInt aGain);

protected:
	CDevSoundTestStepBase();

	TRequestStatus* iStatus;
	TVerdict iVerdict;

	TFixedArray<TInt, 7> iCallbackArray;
	void ResetCallbacks();
	TInt GetCallbackTotal();

	CMMFBuffer* iBuffer;

	CMMFDevSound* iMMFDevSound;
	TInt iExpectedValue;
	TInt iCallbackError;
	};
	
#ifdef Required
//class CTestStep1 : public CSimulProcTestStep, /*public MMdaObjectStateChangeObserver,*/ public MDevSoundObserver
class CTestStep1 : public CDevSoundTestStepBase
	{
public:
	static CTestStep1* NewL();
	void ConstructL();
	virtual void StartProcessing(TRequestStatus& aStatus);
	virtual TVerdict EndProcessingAndReturnResult(TDes8& aMessage);		
	virtual ~CTestStep1();

protected:
	void InitializeComplete (TInt aError);
	void ToneFinished (TInt aError);

	TVerdict DoPlaySimpleTone();
	TVerdict TestInitialize(TMMFState aMode);
	TVerdict TestPlayTone(TInt aFreq, TTimeIntervalMicroSeconds aDur);

private:
	TBool iIsFirstPlayed;
	
	};
#endif

#endif



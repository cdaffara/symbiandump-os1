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


#ifndef __MMFCLIENTTONEPLAY_H__
#define __MMFCLIENTTONEPLAY_H__


#include <e32std.h>
#include <e32base.h>
#include <mdaaudiotoneplayer.h>
#include <mmf/server/sounddevice.h>
#include <caf/data.h>
#include <caf/content.h>
//Panic category and codes
_LIT(KMMFMdaAudioToneUtilityPanicCategory, "MMFMdaAudioToneUtility");
enum TMMFMdaAudioToneUtilityPanicCodes
	{
	EMMFMdaAudioToneUtilityAlreadyPrepared,
	EMMFMdaAudioToneUtilityBadToneConfig,
	EMMFMdaAudioToneUtilityBadMixinCall
	};

/**
Active object utility class to allow the callback to be called asynchronously.
This should help prevent re-entrant code in clients of the mediaframework.
*/
class CMMFMdaAudioToneObserverCallback : public CActive, public MMdaAudioToneObserver, public MMdaAudioTonePlayStartObserver
	{
public:
	static CMMFMdaAudioToneObserverCallback* NewL(MMdaAudioToneObserver& aCallback, MMdaAudioTonePlayStartObserver& aPlayStartCallback);
	~CMMFMdaAudioToneObserverCallback();
// From MMdaAudioToneObserver
	virtual void MatoPrepareComplete(TInt aError);
	virtual void MatoPlayComplete(TInt aError);
// From MMdaAudioTonePlayStartObserver
	virtual void MatoPlayStarted(TInt aError);
private:
	CMMFMdaAudioToneObserverCallback(MMdaAudioToneObserver& aCallback, MMdaAudioTonePlayStartObserver& aPlayStartCallback);
	void RunL();
	void DoCancel();
private:
	enum TMMFAudioToneObserverCallbackAction {EPrepareComplete, EPlayComplete, EPlayStarted};
	MMdaAudioToneObserver& iCallback;
	MMdaAudioTonePlayStartObserver& iPlayStartCallback; 
	TMMFAudioToneObserverCallbackAction iAction;
	TInt iErrorCode;
	RArray <TInt> iCallBackQueue;
	RArray <TInt> iCallBackQueueError;
	};

class CMMFToneConfig;

/**
Concrete implementation of the CMdaAudioToneUtility API.
@see CMdaAudioToneUtility
*/
class CMMFMdaAudioToneUtility;
NONSHARABLE_CLASS( CMMFMdaAudioToneUtility ): public CBase,
											  public MMdaAudioToneObserver,
											  public MDevSoundObserver, 
											  public MMdaAudioTonePlayStartObserver
	{
friend class CMdaAudioToneUtility;
// only for testing purposes
friend class CTestStepUnitMMFAudClient;

public:
	static CMMFMdaAudioToneUtility* NewL(MMdaAudioToneObserver& aObserver, CMdaServer* aServer = NULL,
											   TInt aPriority = EMdaPriorityNormal, 
											   TInt aPref = EMdaPriorityPreferenceTimeAndQuality);

	~CMMFMdaAudioToneUtility();
	
	TMdaAudioToneUtilityState State();
	TInt MaxVolume();
	TInt Volume();
	void SetVolume(TInt aVolume); 
	void SetPriority(TInt aPriority, TInt aPref);
	void SetDTMFLengths(TTimeIntervalMicroSeconds32 aToneLength, 
										 TTimeIntervalMicroSeconds32 aToneOffLength,
										 TTimeIntervalMicroSeconds32 aPauseLength);
	void SetRepeats(TInt aRepeatNumberOfTimes, const TTimeIntervalMicroSeconds& aTrailingSilence);
	void SetVolumeRamp(const TTimeIntervalMicroSeconds& aRampDuration);
	TInt FixedSequenceCount();
	const TDesC& FixedSequenceName(TInt aSequenceNumber);
	void PrepareToPlayTone(TInt aFrequency, const TTimeIntervalMicroSeconds& aDuration);
	void PrepareToPlayDualTone(TInt aFrequencyOne, TInt aFrequencyTwo, const TTimeIntervalMicroSeconds& aDuration);
	void PrepareToPlayDTMFString(const TDesC& aDTMF);
	void PrepareToPlayDesSequence(const TDesC8& aSequence);
	void PrepareToPlayFileSequence(const TDesC& aFileName);
	void PrepareToPlayFileSequence(RFile& aFile);
	void PrepareToPlayFixedSequence(TInt aSequenceNumber);
	void CancelPrepare();
	void Play();
	void CancelPlay();
	TInt Pause();
	TInt Resume();

	void SetBalanceL(TInt aBalance=KMMFBalanceCenter);
	TInt GetBalanceL();
// From MMdaAudioToneObserver
	void MatoPrepareComplete(TInt aError);
	void MatoPlayComplete(TInt aError);

// From DevSoundObserver
	void InitializeComplete(TInt aError);
	void ToneFinished(TInt aError); 
	void BufferToBeFilled(CMMFBuffer* /*aBuffer*/) {User::Panic(KMMFMdaAudioToneUtilityPanicCategory,EMMFMdaAudioToneUtilityBadMixinCall);}
	void PlayError(TInt /*aError*/)	{User::Panic(KMMFMdaAudioToneUtilityPanicCategory,EMMFMdaAudioToneUtilityBadMixinCall);}
	void BufferToBeEmptied(CMMFBuffer* /*aBuffer*/)	{User::Panic(KMMFMdaAudioToneUtilityPanicCategory,EMMFMdaAudioToneUtilityBadMixinCall);} 
	void RecordError(TInt /*aError*/)	{User::Panic(KMMFMdaAudioToneUtilityPanicCategory,EMMFMdaAudioToneUtilityBadMixinCall);}
	void ConvertError(TInt /*aError*/)  {User::Panic(KMMFMdaAudioToneUtilityPanicCategory,EMMFMdaAudioToneUtilityBadMixinCall);}
	void DeviceMessage(TUid /*aMessageId*/, const TDesC8& /*aMsg*/) {User::Panic(KMMFMdaAudioToneUtilityPanicCategory,EMMFMdaAudioToneUtilityBadMixinCall);}	
	void SendEventToClient(const TMMFEvent& /*aEvent*/);
	
	TAny* CustomInterface(TUid aInterfaceId);
	
	void PlayAfterInitialized();

	void RegisterPlayStartCallback(MMdaAudioTonePlayStartObserver& aObserver);

// From MMdaAudioTonePlayStartObserver
	void MatoPlayStarted(TInt aError);
	
protected:
	CMMFMdaAudioToneUtility(MMdaAudioToneObserver& aCallback, TInt aPriority, TInt aPref);
	void ConstructL();
private:
	// functions to convert between MediaServer Balance and SoundDev balance
	void CalculateBalance( TInt& aBalance, TInt aLeft, TInt aRight ) const;
	void CalculateLeftRightBalance( TInt& aLeft, TInt& aRight, TInt aBalance ) const;
	
private:
	CMMFDevSound* iDevSound;
	MMdaAudioToneObserver& iCallback;
	CMMFMdaAudioToneObserverCallback* iAsyncCallback;

	CMMFToneConfig* iToneConfig;
	// Devsound doesn't take copies of descriptors we pass to it, so we 
	// need to cache the config info until it has finished playing.
	CMMFToneConfig* iPlayingToneConfig; //unused remove when BC break allowed

	TMdaAudioToneUtilityState iState;

	TMMFPrioritySettings iPrioritySettings;
	
	TInt iInitializeState;   
	
	TInt iSequenceNumber;
	TBool iPlayCalled;
	
	TBool iInitialized;
	
	MMdaAudioTonePlayStartObserver* iPlayStartObserver;
	
#ifdef _DEBUG
	TBool iPlayCalledBeforeInitialized;
#endif
	};


// Tone configurations
class CMMFToneConfig : public CBase
	{
public:
	enum TMMFToneType 
		{	
		EMmfToneTypeSimple,
		EMmfToneTypeDTMF,
		EMmfToneTypeDesSeq,
		EMmfToneTypeFileSeq,
		EMmfToneTypeFixedSeq,
		EMmfToneTypeDual,
		};
	virtual ~CMMFToneConfig() {}
	TMMFToneType Type() {return iType;}
protected:
	CMMFToneConfig(TMMFToneType aType) : iType(aType) {}
private:
	TMMFToneType iType;
	};

class CMMFSimpleToneConfig : public CMMFToneConfig
	{
public:
	static CMMFToneConfig* NewL(TInt aFrequency, const TTimeIntervalMicroSeconds& aDuration);
	virtual ~CMMFSimpleToneConfig();
	TInt Frequency();
	const TTimeIntervalMicroSeconds& Duration();
protected:
	CMMFSimpleToneConfig(TInt aFrequency, const TTimeIntervalMicroSeconds& aDuration);
private:
	TInt iFrequency;
	TTimeIntervalMicroSeconds iDuration;
	};

class CMMFDualToneConfig : public CMMFToneConfig
	{
public:
	static CMMFToneConfig* NewL(TInt aFrequencyOne, TInt aFrequencyTwo, const TTimeIntervalMicroSeconds& aDuration);
	virtual ~CMMFDualToneConfig();
	TInt FrequencyOne();
	TInt FrequencyTwo();
	const TTimeIntervalMicroSeconds& Duration();
protected:
	CMMFDualToneConfig(TInt aFrequencyOne, TInt aFrequencyTwo, const TTimeIntervalMicroSeconds& aDuration);
private:
	TInt iFrequencyOne;
	TInt iFrequencyTwo;
	TTimeIntervalMicroSeconds iDuration;
	};

class CMMFDTMFStringToneConfig : public CMMFToneConfig
	{
public:
	static CMMFToneConfig* NewL(const TDesC& aDTMF);
	static CMMFToneConfig* NewL(RFile& aFile);
	virtual ~CMMFDTMFStringToneConfig();
	const TDesC& DTMF();
protected:
	CMMFDTMFStringToneConfig();
	void ConstructL(const TDesC& aDTMF);
private:
	HBufC* iDTMF;
	};

class CMMFDesSeqToneConfig : public CMMFToneConfig
	{
public:
	static CMMFToneConfig* NewL(const TDesC8& aDesSeq);
	virtual ~CMMFDesSeqToneConfig();
	const TDesC8& DesSeq();
protected:
	CMMFDesSeqToneConfig();
	void ConstructL(const TDesC8& aDesSeq);
private:
	HBufC8* iDesSeq;
	};


class CMMFFileSeqToneConfig : public CMMFToneConfig
	{
public:
	static CMMFToneConfig* NewL(const TDesC& aFileSeq);
	static CMMFToneConfig* NewL(RFile& aFile);
	virtual ~CMMFFileSeqToneConfig();
	const TDesC8& FileSeq();
	
	// CAF support for Tone Utility
	void ExecuteIntentL();
protected:
	CMMFFileSeqToneConfig();
	void ConstructL(const TDesC& aFileSeq);
	void ConstructL(RFile& aFile);
private:
	ContentAccess::CContent* iCAFContent;
	ContentAccess::CData* iCAFData;
	HBufC8* iDesSeq;
	};

class CMMFFixedSeqToneConfig : public CMMFToneConfig
	{
public:
	static CMMFToneConfig* NewL(TInt aSeqNo);
	virtual ~CMMFFixedSeqToneConfig();
	TInt SequenceNumber();
protected:
	CMMFFixedSeqToneConfig(TInt aSeqNo);
private:
	TInt iSequenceNumber;
	};

#endif

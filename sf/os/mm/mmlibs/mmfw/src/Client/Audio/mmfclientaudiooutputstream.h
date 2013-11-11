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

#ifndef MMFCLIENTAUDIOOUTPUTSTREAM_H
#define MMFCLIENTAUDIOOUTPUTSTREAM_H

#include <mmf/server/sounddevice.h>
#include <mmf/common/mmfstandardcustomcommands.h>

template<class T>
class CMMFFifo;
class CMMFBuffer;
class MMdaAudioOutputStreamCallback;
class TMdaPackage;
class MMMFAudioResourceNotificationCallback;

class CMMFMdaAudioOutputStream;
NONSHARABLE_CLASS( CMMFMdaAudioOutputStream ): public CBase,
								 			   public MDevSoundObserver
	{
	enum TIsOpenState
		{
		EIsNotOpen,
		EIsOpening,  // Open() called but not responded to
		EIsOpen,     // Open callback made and successfully opened
		};
public:
	static CMMFMdaAudioOutputStream* NewL(MMdaAudioOutputStreamCallback& aCallback);
	static CMMFMdaAudioOutputStream* NewL(MMdaAudioOutputStreamCallback& aCallback, TInt aPriority, TInt aPref);
	virtual ~CMMFMdaAudioOutputStream();
//
	void SetAudioPropertiesL(TInt aSampleRate, TInt aChannels);
	void Open(TMdaPackage* aSettings);
	void SetVolume(const TInt aVolume);
	TInt Volume();
	TInt MaxVolume();
	void SetBalanceL(TInt aBalance = KMMFBalanceCenter);
	TInt GetBalanceL() const;
	void SetPriority(TInt aPriority, TInt aPref);
	void WriteL(const TDesC8& aData);
	void Stop();
	TInt Pause();
	TInt Resume();
	const TTimeIntervalMicroSeconds& Position();
	TInt GetBytes();
	void SetDataTypeL(TFourCC aAudioType);
	TFourCC DataType() const;
	TInt RegisterAudioResourceNotification(MMMFAudioResourceNotificationCallback& aCallback,TUid aNotificationEventUid,const TDesC8& aNotificationRegistrationData = KNullDesC8);
	TInt CancelRegisterAudioResourceNotification(TUid aNotificationEventId);
	TInt WillResumePlay();
	TAny* CustomInterface(TUid aInterfaceId);
	TInt KeepOpenAtEnd();
	TInt RequestStop();
public:		
	// from MDevSoundObserver
	virtual void InitializeComplete(TInt aError);
	virtual void ToneFinished(TInt aError); 
	virtual void BufferToBeFilled(CMMFBuffer* aBuffer);
	virtual void PlayError(TInt aError); 
	virtual void BufferToBeEmptied(CMMFBuffer* aBuffer); 
	virtual void RecordError(TInt aError); 
	virtual void ConvertError(TInt aError);
	virtual void DeviceMessage(TUid aMessageType, const TDesC8& aMsg);
	virtual void SendEventToClient(const TMMFEvent& aEvent);
	
private:
	void RealSetAudioPropertiesL(TInt aSampleRate, TInt aChannels);

private:
	// class to allow asynchronous user callbacks
	class CActiveCallback : public CActive
		{
	public:
		~CActiveCallback();
		CActiveCallback(MMdaAudioOutputStreamCallback& aCallback);
		void Signal(const TInt aReason);
	protected:
		virtual void RunL();
		virtual void DoCancel();
	private:
		MMdaAudioOutputStreamCallback& iCallback;
		};
private:
	CMMFMdaAudioOutputStream(MMdaAudioOutputStreamCallback& aCallback);
	void ConstructL(TInt aPriority, TInt aPref);
	void StartPlayL();
	enum TState { EStopped, EPlaying, EStopping, EPaused};
	void EmptyFifo(TInt aError);
	void StartShutDownTimer();
	static TInt ShutDownTimerComplete(TAny* aSamplesPlayed);
	void DoShutDownTimerComplete();
private:
	MMdaAudioOutputStreamCallback&		iCallback;
	MMMFAudioResourceNotificationCallback* iAudioResourceNotificationCallBack;
	CMMFFifo<const TDesC8>*				iFifo;
	TInt								iFifoItemPos;
	CMMFDevSound*						iDevSound;
	TState								iState;
	TTimeIntervalMicroSeconds			iPosition;
	CActiveCallback*					iActiveCallback;
	TUid 								iEventHolder;
	TBuf8<256> 						iNotificationDataHolder;
	// only for testing
	friend class						CTestStepUnitMMFAudClient;
	CMMFBuffer*							iBuffer;
	TFourCC								iDataTypeCode;
	TIsOpenState						iIsOpenState;
	TBool								iValuesCached;
	TInt								iSampleRate;
	TInt								iChannels;
	TInt								iVolume;
	TBool								iInitCallFrmSetDataType;//Flag to indicate that InitializeL is called from SetDataTypeL.
	TInt								iInitializeState;
	CActiveSchedulerWait*				iActiveSchedulerWait;
	TBool								iKeepOpenAtEnd;
	CPeriodic*							iShutDownTimer;
	TInt								iCurrentSamplesPlayed;
	TBool 								iDevSoundIgnoresUnderflow;
	}; 

#endif

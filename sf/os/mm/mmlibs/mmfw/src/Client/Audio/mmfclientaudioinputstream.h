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


#ifndef MMFCLIENTAUDIOINPUTSTREAM_H
#define MMFCLIENTAUDIOINPUTSTREAM_H

#include <mmf/server/sounddevice.h>
#include <mmf/common/mmfstandardcustomcommands.h>
#include <mda/common/audio.h>

template<class T>
class CMMFFifo;
class CMMFBuffer;
class MMdaAudioInputStreamCallback;
class TMdaPackage;

class CMMFMdaAudioInputStream;
NONSHARABLE_CLASS( CMMFMdaAudioInputStream ): public CBase, 
											  public MDevSoundObserver
	{
	enum TIsOpenState
		{
		EIsNotOpen,
		EIsOpening,  // Open() called but not responded to
		EIsOpen,     // Open callback made and successfully opened
		};
public:
	static CMMFMdaAudioInputStream* NewL(MMdaAudioInputStreamCallback& aCallback);
	static CMMFMdaAudioInputStream* NewL(MMdaAudioInputStreamCallback& aCallback, TInt aPriority, TInt aPref);
	~CMMFMdaAudioInputStream();

	void SetAudioPropertiesL(TInt aSampleRate, TInt aChannels);
	void Open(TMdaPackage* aSettings);
	void SetGain(TInt aGain);
	TInt Gain() const;
	TInt MaxGain() const;
	void SetBalanceL(TInt aBalance = KMMFBalanceCenter);
	TInt GetBalanceL() const;
	void SetPriority(TInt aPriority, TInt aPref);
	void ReadL(TDes8& aData);
	void Stop();
	const TTimeIntervalMicroSeconds& Position();
	TInt GetBytes();
	void SetDataTypeL(TFourCC aAudioType);
	TFourCC DataType() const;
	void GetSupportedBitRatesL(RArray<TInt>& aSupportedBitRates);
	TInt BitRateL() const;
	void SetBitRateL(TInt aBitRate);
	TAny* CustomInterface(TUid aInterfaceId);
	void SetSingleBufferMode(TBool aSingleMode);
	void RequestStop();
	void ShutDown();

public:		
	// from MDevSoundObserver
	void InitializeComplete(TInt aError);
	void ToneFinished(TInt aError); 
	void BufferToBeFilled(CMMFBuffer* aBuffer);
	void PlayError(TInt aError); 
	void BufferToBeEmptied(CMMFBuffer* aBuffer); 
	void RecordError(TInt aError); 
	void ConvertError(TInt aError);
	void DeviceMessage(TUid aMessageType, const TDesC8& aMsg);

private:
	void RealSetAudioPropertiesL(TInt aSampleRate, TInt aChannels);
	
private:
	// class to allow asynchronous user callbacks
	class CActiveCallback : public CActive
		{
	public:
		CActiveCallback(MMdaAudioInputStreamCallback& aCallback);
		~CActiveCallback();
		void Signal(const TInt aReason);
	protected:
		void RunL();
		void DoCancel();
	private:
		MMdaAudioInputStreamCallback& iCallback;
		};
private:
	enum TState { EStopped, ERecording, EStopping };
private:
	CMMFMdaAudioInputStream(MMdaAudioInputStreamCallback& aCallback);
	void ConstructL(TInt aPriority, TInt aPref);
	TInt Read(TDes8& aData);
private:
	MMdaAudioInputStreamCallback&		iCallback;
	TTimeIntervalMicroSeconds			iPosition;
	CMMFDevSound*						iDevSound;
	CMMFFifo<TDes8>*					iFifo;
	TBool								iSingleBuffer;
	TPtr8								iStorageItem;
	TPtr8								iBufferPtr;
	TInt								iState;
	CActiveCallback*					iActiveCallback;
	TInt								iBytesRecorded;//tracks bytes supplied by devsound
	TFourCC								iDataTypeCode;
	TIsOpenState						iIsOpenState;
	TMdaAudioDataSettings				iSettings;
	TBool								iAudioDataStored;
	// only for testing
	friend class						CTestStepUnitMMFAudClient;
	TBool								iInitCallFrmSetDataType;//Flag to indicate that InitializeL is called from SetDataTypeL.
	TInt								iInitializeState;
	CActiveSchedulerWait*				iActiveSchedulerWait;
	}; 

#endif

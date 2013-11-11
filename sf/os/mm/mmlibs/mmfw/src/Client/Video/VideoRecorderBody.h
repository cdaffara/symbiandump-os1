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

#ifndef __VIDEORECORDERBODY_H__
#define __VIDEORECORDERBODY_H__

#include <mmf/common/mmfbase.h>
#include <mmf/common/mmfutilities.h>
#include <mmf/common/mmfcontroller.h>
#include <mmf/common/mmfstandardcustomcommands.h>
#include <mmf/common/mmfcontrollerframeworkbase.h>
#include <mda/common/base.h>      
#include "mmfclientutility.h"

class CMMFVideoRecorderCallback;

NONSHARABLE_CLASS( CVideoRecorderUtility::CBody ) : public CBase, 
									 				public MMMFControllerEventMonitorObserver,
								     				public MMMFFindAndOpenControllerObserver

	{
friend class CTestStepUnitMMFVidClient;

public: 
	~CBody();

	static CBody* NewL(CVideoRecorderUtility* aParent,
					   MVideoRecorderUtilityObserver& aObserver,
					   TInt aPriority=EMdaPriorityNormal,
					   TInt aPref=EMdaPriorityPreferenceTimeAndQuality);

	void OpenFileL(const TDesC& aFileName,
				   TInt aCameraHandle,
				   TUid aControllerUid,
				   TUid aVideoFormat,
				   const TDesC8& aVideoType = KNullDesC8,
				   TFourCC aAudioType = KMMFFourCCCodeNULL);
	void OpenFileL(const RFile& aFile,
				   TInt aCameraHandle,
				   TUid aControllerUid,
				   TUid aVideoFormat,
				   const TDesC8& aVideoType = KNullDesC8,
				   TFourCC aAudioType = KMMFFourCCCodeNULL);
	void OpenDesL(TDes8& aDescriptor,
				  TInt aCameraHandle,
				  TUid aControllerUid,
				  TUid aVideoFormat,
				  const TDesC8& aVideoType = KNullDesC8,
				  TFourCC aAudioType = KMMFFourCCCodeNULL);
	void OpenUrlL(const TDesC& aUrl,
				  TInt aIapId,
				  TInt aCameraHandle,
				  TUid aControllerUid,
				  TUid aVideoFormat,
				  const TDesC8& aVideoType = KNullDesC8,
				  TFourCC aAudioType = KMMFFourCCCodeNULL);

	void Close();

	void Prepare();
	void Record();
	TInt Stop();
	void PauseL();

	void SetPriorityL(TInt aPriority, TInt aPref);
	void GetPriorityL(TInt& aPriority, TMdaPriorityPreference& aPref) const;

	void SetVideoFrameRateL(TReal32 aFrameRate);
	TReal32 VideoFrameRateL() const;
	void SetVideoFrameSizeL(const TSize& aSize);
	void GetVideoFrameSizeL(TSize& aSize) const;
	void SetVideoBitRateL(TInt aBitRate);
	TInt VideoBitRateL();
	void SetAudioBitRateL(TInt aBitRate);
	TInt AudioBitRateL() const;
	void SetAudioEnabledL(TBool aEnabled);
	TBool AudioEnabledL() const;

	TTimeIntervalMicroSeconds DurationL() const;

	void SetMaxClipSizeL(TInt aClipSizeInBytes);

	void SetGainL(TInt aGain);
	TInt GainL() const;
	TInt MaxGainL() const;

	TInt NumberOfMetaDataEntriesL() const;
	CMMFMetaDataEntry* MetaDataEntryL(TInt aIndex) const;
	void AddMetaDataEntryL(const CMMFMetaDataEntry& aNewEntry);
	void RemoveMetaDataEntryL(TInt aIndex);
	void ReplaceMetaDataEntryL(TInt aIndex,const CMMFMetaDataEntry& aNewEntry);

	TFourCC AudioTypeL() const;
	void SetVideoTypeL(const TDesC8& aType);
	void SetAudioTypeL(TFourCC aType);
	void GetSupportedVideoTypesL(CDesC8Array& aVideoTypes) const;
	void GetSupportedAudioTypesL(RArray<TFourCC>& aAudioTypes) const;
	TTimeIntervalMicroSeconds RecordTimeAvailable() const;
	TInt CustomCommandSync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2, TDes8& aDataFrom);
	TInt CustomCommandSync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2);
	void CustomCommandAsync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2, TDes8& aDataFrom, TRequestStatus& aStatus);
	void CustomCommandAsync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2, TRequestStatus& aStatus);

	const TDesC8& VideoFormatMimeType() const;

	const CMMFControllerImplementationInformation& ControllerImplementationInformationL();
	
	void SetPixelAspectRatioL(const TVideoAspectRatio& aAspectRatio);
    void GetPixelAspectRatioL(TVideoAspectRatio& aAspectRatio) const;
    void GetSupportedPixelAspectRatiosL(RArray<TVideoAspectRatio>& aAspectRatios) const;
    void SetAudioChannelsL(const TUint aNumChannels);
    TUint AudioChannelsL() const;
    void GetSupportedAudioChannelsL(RArray<TUint>& aChannels) const;
    void SetAudioSampleRateL(const TUint aSampleRate);    
    TUint AudioSampleRateL() const;
    void GetSupportedAudioSampleRatesL(RArray<TUint> &aSampleRates) const;

    void SetVideoEnabledL(TBool aEnabled);
    TBool VideoEnabledL() const;
    void SetVideoQualityL(TInt aQuality);
    TInt VideoQualityL() const;
    void SetVideoFrameRateFixedL(TBool aFixedFrameRate);
    TBool VideoFrameRateFixedL() const;
private:
	CBody(CVideoRecorderUtility* aParent,
		  MVideoRecorderUtilityObserver& aObserver,
		  TInt aPriority=EMdaPriorityNormal,
		  TInt aPref=EMdaPriorityPreferenceTimeAndQuality);
	void ConstructL();

	//from MMMFControllerEventMonitorObserver
	virtual void HandleEvent(const TMMFEvent& aEvent);

	// from MMMFFindAndOpenControllerObserver 
	virtual void MfaocComplete(
		TInt& aError, 
		RMMFController* aController, 
		TUid aControllerUid, 
		TMMFMessageDestination* aSourceHandle, 
		TMMFMessageDestination* aSinkHandle);

	void Reset();

private:
	RMMFController iController;
	RMMFVideoControllerCustomCommands iVideoControllerCustomCommands;
	RMMFVideoRecordControllerCustomCommands iVideoRecordControllerCustomCommands;
	RMMFAudioRecordDeviceCustomCommands iAudioRecordDeviceCustomCommands;


	MVideoRecorderUtilityObserver& iObserver;
	CMMFControllerEventMonitor* iControllerEventMonitor;
	TMMFVideoRecorderState iState;
	TTimeIntervalMicroSeconds iDuration;
	TTimeIntervalMicroSeconds iPosition;
	TMMFPrioritySettings iPrioritySettings;
	CMMFVideoRecorderCallback* iAsyncCallback;
	CMMFControllerImplementationInformation* iControllerImplementationInformation;
	TUid iControllerUid;

	CVideoRecorderUtility* iParent;
	friend class CVideoRecorderUtility;

	HBufC8* iMimeType;

	// utility class to find and open a suitable controller asynchronously
	CMMFFindAndOpenController* iFindAndOpenController;

	// received a KMMFEventCategoryVideoOpenComplete event
	TBool iEventOpenReceived;
	
	// received a MfaocComplete() callback
	TBool iCallbackOpenReceived;

	TInt iOpenError;

	TInt iCameraHandle;
	TUid iVideoFormatUid;
	HBufC8* iVideoCodec;
	TFourCC iAudioCodec;

	RMMFVideoPixelAspectRatioCustomCommands iVideoPixelAspectRatioCustomCommands;
	RMMFVideoAudioSamplingRateAndChannelConfigCustomCommands iVideoAudioSamplingRateAndChannelConfigCustomCommands;
	RMMFVideoRecordControllerExtCustomCommands iVideoRecordControllerExtCustomCommands;
	};

#endif	// __VIDEORECORDERBODY_H__

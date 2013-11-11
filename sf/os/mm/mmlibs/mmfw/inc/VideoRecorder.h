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

#ifndef __VIDEORECORDER_H__
#define __VIDEORECORDER_H__

#include <f32file.h>
#include <mmf/common/mmfbase.h>
#include <mmf/common/mmfutilities.h>
#include <mmf/common/mmfcontroller.h>
#include <mmf/common/mmfstandardcustomcommands.h>
#include <mmf/common/mmfcontrollerframeworkbase.h>
#include <mda/common/base.h>
#include <mmfclntutility.h>      

class CMMFVideoRecorderCallback;

/**
@publishedAll
@released

An interface to a set of video recorder callback functions.

The class is a mixin and is intended to be inherited by the client
class which is observing the video recording operation. The functions
encapsulated by this class are called when specific events occur in
the process of initialising and recording a video clip. A reference to
this object is passed as a parameter when constructing a video recorder
utility object.

@since 7.0s
*/
class MVideoRecorderUtilityObserver
	{
public:

	/**
	Notification to the client that the opening of the video clip has completed,
	successfully, or otherwise.
	
	@param  aError
	        The status of the video recorder after initialisation.
	        This is either KErrNone if the open has completed successfully,
	        or one of the system wide error codes.
	*/
	virtual void MvruoOpenComplete(TInt aError) = 0;  

	/**
	Notification that video recorder is ready to begin recording. This callback
	is generated in response to a call to Prepare.

	@param  aError
	        This is either KErrNone if the video recorder has been prepared for
	        recording successfully, or one of the system wide error codes
	*/
	virtual void MvruoPrepareComplete(TInt aError) = 0;

	/**
	Notification that video recording has completed. This is not called if
	recording is explicitly stopped by calling Stop.
	
	@param  aError
	        This is either KErrNone if recording was completed successfully,
	        or one of the system wide error codes.
	*/
	virtual void MvruoRecordComplete(TInt aError) = 0;

	/**
	General event notification from controller. These events are specified by
	the supplier of the controller.

	@param  aEvent
	        The event sent by the controller.
	*/
	virtual void MvruoEvent(const TMMFEvent& aEvent) = 0;
	};

/**
@publishedAll
@released

Records video data.

The class offers a simple interface to record and set meta data and control information for a video
clip and save the result to a file, descriptor or URL.

Note:
Some video formats also allow the storing of audio data. To accommodate this, this class contains
audio functions that can manipulate such data.

While this class is abstract, NewL() constructs, initialises and returns pointers to instances of
concrete classes derived from this abstract class. This concrete class is part of the MMF
implementation and is private.

@since 7.0s
*/
class CVideoRecorderUtility : public CBase,
							  public MMMFClientUtility
	{
friend class CTestStepUnitMMFVidClient;

	class CBody;

public:


	~CVideoRecorderUtility();
	IMPORT_C static CVideoRecorderUtility* NewL(MVideoRecorderUtilityObserver& aObserver,
												TInt aPriority=EMdaPriorityNormal,
												TInt aPref=EMdaPriorityPreferenceTimeAndQuality);
	IMPORT_C void OpenFileL(const TDesC& aFileName,
							TInt aCameraHandle,
							TUid aControllerUid,
							TUid aVideoFormat,
							const TDesC8& aVideoType = KNullDesC8,
							TFourCC aAudioType = KMMFFourCCCodeNULL);
	IMPORT_C void OpenFileL(const RFile& aFile,
							TInt aCameraHandle,
							TUid aControllerUid,
							TUid aVideoFormat,
							const TDesC8& aVideoType = KNullDesC8,
							TFourCC aAudioType = KMMFFourCCCodeNULL);
    IMPORT_C void OpenDesL(TDes8& aDescriptor,
							TInt aCameraHandle,
							TUid aControllerUid,
							TUid aVideoFormat,
							const TDesC8& aVideoType = KNullDesC8,
							TFourCC aAudioType = KMMFFourCCCodeNULL);
    IMPORT_C void OpenUrlL(const TDesC& aUrl,
							TInt aIapId,
							TInt aCameraHandle,
							TUid aControllerUid,
							TUid aVideoFormat,
							const TDesC8& aVideoType = KNullDesC8,
							TFourCC aAudioType = KMMFFourCCCodeNULL);
    IMPORT_C void Close();
    IMPORT_C void Prepare();
    IMPORT_C void Record();
    IMPORT_C TInt Stop();
    IMPORT_C void PauseL();
    IMPORT_C void SetPriorityL(TInt aPriority, TInt aPref);
    IMPORT_C void GetPriorityL(TInt& aPriority, TMdaPriorityPreference& aPref) const;
    IMPORT_C void SetVideoFrameRateL(TReal32 aFrameRate);
    IMPORT_C TReal32 VideoFrameRateL() const;
    IMPORT_C void SetVideoFrameSizeL(const TSize& aSize);
    IMPORT_C void GetVideoFrameSizeL(TSize& aSize) const;
    IMPORT_C void SetVideoBitRateL(TInt aBitRate);
    IMPORT_C TInt VideoBitRateL();
    IMPORT_C void SetAudioBitRateL(TInt aBitRate);
    IMPORT_C TInt AudioBitRateL() const;
    IMPORT_C void SetAudioEnabledL(TBool aEnabled);
    IMPORT_C TBool AudioEnabledL() const;
    IMPORT_C TTimeIntervalMicroSeconds DurationL() const;
    IMPORT_C void SetMaxClipSizeL(TInt aClipSizeInBytes);
    IMPORT_C void SetGainL(TInt aGain);
    IMPORT_C TInt GainL() const;
    IMPORT_C TInt MaxGainL() const;
    IMPORT_C TInt NumberOfMetaDataEntriesL() const;
    IMPORT_C CMMFMetaDataEntry* MetaDataEntryL(TInt aIndex) const;
    IMPORT_C void AddMetaDataEntryL(const CMMFMetaDataEntry& aNewEntry);
    IMPORT_C void RemoveMetaDataEntryL(TInt aIndex);
    IMPORT_C void ReplaceMetaDataEntryL(TInt aIndex,const CMMFMetaDataEntry& aNewEntry);
    IMPORT_C TFourCC AudioTypeL() const;
    IMPORT_C void SetVideoTypeL(const TDesC8& aType);
    IMPORT_C void SetAudioTypeL(TFourCC aType);
    IMPORT_C void GetSupportedVideoTypesL(CDesC8Array& aVideoTypes) const;
    IMPORT_C void GetSupportedAudioTypesL(RArray<TFourCC>& aAudioTypes) const;
    IMPORT_C TTimeIntervalMicroSeconds RecordTimeAvailable() const;
    IMPORT_C const TDesC8& VideoFormatMimeType() const;
    IMPORT_C const CMMFControllerImplementationInformation& ControllerImplementationInformationL();
    IMPORT_C TInt CustomCommandSync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2, TDes8& aDataFrom);
    IMPORT_C TInt CustomCommandSync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2);
    IMPORT_C void CustomCommandAsync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2, TDes8& aDataFrom, TRequestStatus& aStatus);
    IMPORT_C void CustomCommandAsync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2, TRequestStatus& aStatus);
    IMPORT_C void SetPixelAspectRatioL(const TVideoAspectRatio& aAspectRatio);
    IMPORT_C void GetPixelAspectRatioL(TVideoAspectRatio& aAspectRatio) const;
    IMPORT_C void GetSupportedPixelAspectRatiosL(RArray<TVideoAspectRatio>& aAspectRatios) const;
    IMPORT_C void SetAudioChannelsL(const TUint aNumChannels);
    IMPORT_C TUint AudioChannelsL() const;
    IMPORT_C void GetSupportedAudioChannelsL(RArray<TUint>& aChannels) const;
    IMPORT_C void SetAudioSampleRateL(const TUint aSampleRate);    
    IMPORT_C TUint AudioSampleRateL() const;
    IMPORT_C void GetSupportedAudioSampleRatesL(RArray<TUint> &aSampleRates) const;
    IMPORT_C void SetVideoEnabledL(TBool aEnabled);
    IMPORT_C TBool VideoEnabledL() const;
    IMPORT_C void SetVideoQualityL(TInt aQuality);
    IMPORT_C TInt VideoQualityL() const;
    IMPORT_C void SetVideoFrameRateFixedL(TBool aFixedFrameRate);
    IMPORT_C TBool VideoFrameRateFixedL() const;

private:
	enum TMMFVideoRecorderState
		{
		EStopped,
		EOpening,
		EPaused,
		ERecording
		};

private:
	CBody* iBody;
	friend class CBody;
	};

#endif

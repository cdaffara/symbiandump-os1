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


#include <mmf/common/mmfstandardcustomcommands.h>  
#include <mmf/common/mmfvideo.h>
#include <mmf/server/mmfdes.h>
#include <mmf/server/mmffile.h>
#include <videorecorder.h>
#include "VideoRecorderBody.h"
#include "mmfclientvideocommon.h"
#include "mmfvideocallback.h"
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <mmf/common/mmfvideoenums.h>
#endif

CVideoRecorderUtility::CBody* CVideoRecorderUtility::CBody::NewL(CVideoRecorderUtility* aParent,
																 MVideoRecorderUtilityObserver& aObserver,
																 TInt aPriority,
																 TInt aPref)
	{
	CBody* self = new(ELeave) CBody(aParent, aObserver, aPriority, aPref);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

CVideoRecorderUtility::CBody::~CBody()
	{
	Close();
	delete iFindAndOpenController;
	delete iControllerEventMonitor;
	delete iAsyncCallback;	
	delete iControllerImplementationInformation;
	delete iMimeType;
	delete iVideoCodec;
	}

CVideoRecorderUtility::CBody::CBody(CVideoRecorderUtility* aParent,
									MVideoRecorderUtilityObserver& aObserver,
									TInt aPriority,
									TInt aPref) : 
	iVideoControllerCustomCommands(iController),
	iVideoRecordControllerCustomCommands(iController),
	iAudioRecordDeviceCustomCommands(iController),
	iObserver(aObserver),
	iVideoPixelAspectRatioCustomCommands(iController),
	iVideoAudioSamplingRateAndChannelConfigCustomCommands(iController),
	iVideoRecordControllerExtCustomCommands(iController)
	{
	iParent = aParent;
	iState = EStopped;
	iPrioritySettings.iPriority = aPriority;
	iPrioritySettings.iPref = aPref;
	}

void CVideoRecorderUtility::CBody::ConstructL()
	{
	iAsyncCallback = new (ELeave) CMMFVideoRecorderCallback(iObserver);
	iControllerEventMonitor = CMMFControllerEventMonitor::NewL(*this, iController);
	iMimeType = HBufC8::NewL( KMaxMimeTypeLength );

	iFindAndOpenController = CMMFFindAndOpenController::NewL(*this);
	iFindAndOpenController->Configure(KUidMediaTypeVideo, iPrioritySettings, CMMFPluginSelectionParameters::EAllowOtherMediaIds);
	iFindAndOpenController->ConfigureController(iController, *iControllerEventMonitor, CMMFFindAndOpenController::ERecord);
	}

void CVideoRecorderUtility::CBody::Reset() 
	{
	Close();
	
	// reset all state variables
	iEventOpenReceived = EFalse;
	iCallbackOpenReceived = EFalse;
	iOpenError = KErrNone;

	iCameraHandle = 0;
	iVideoFormatUid = KNullUid;
	delete iVideoCodec; iVideoCodec = NULL;
	iAudioCodec = KMMFFourCCCodeNULL;
	
	}

void CVideoRecorderUtility::CBody::MfaocComplete(		
	TInt& aError, 
	RMMFController* /*aController*/,
	TUid aControllerUid, 
	TMMFMessageDestination* /*aSourceHandle*/, 
	TMMFMessageDestination* /*aSinkHandle*/)
	{
	iCallbackOpenReceived = ETrue;

	// save the error in iOpenError -
	// unless HandleEvent(KMMFEventCategoryVideoOpenComplete) 
	// has reported an error already
	if (iOpenError == KErrNone)
		iOpenError = aError;

	if (iOpenError == KErrNone)
		{
		iControllerUid = aControllerUid;

		if (iOpenError == KErrNone && iVideoFormatUid!=KNullUid)
			iOpenError = iVideoRecordControllerCustomCommands.SetVideoFormat(iVideoFormatUid);

		if (iOpenError == KErrNone)
			iOpenError = iVideoRecordControllerCustomCommands.SetVideoCodec(*iVideoCodec);

		if (iOpenError == KErrNone)
			iOpenError = iVideoRecordControllerCustomCommands.SetAudioCodec(iAudioCodec);
		
		if (iOpenError == KErrNone)
			iOpenError = iVideoRecordControllerCustomCommands.SetCameraHandle(iCameraHandle);

		// If an error occurred in any of the above, close the controller.
		if (iOpenError != KErrNone)
			{
			Close();
			}
		}

	// if we've already received the open complete event
	// call the client now (otherwise wait for it)
	if (iEventOpenReceived || iOpenError != KErrNone)
		{
		// stop a subsequent HandleEvent(KMMFEventCategoryVideoOpenComplete) 
		// from issuing another callback to client
		iCallbackOpenReceived = EFalse;	
		iAsyncCallback->Signal(iOpenError, CMMFVideoRecorderCallback::EOpenCompleteEvent);
		}

	}

void CVideoRecorderUtility::CBody::OpenFileL(const TDesC& aFileName,
											 TInt aCameraHandle,
											 TUid aControllerUid,
											 TUid aVideoFormatUid,
											 const TDesC8& aVideoCodec,
											 TFourCC aAudioCodec)
	{
	// Make sure we are closed
	Reset();
	iCameraHandle = aCameraHandle;
	iVideoFormatUid = aVideoFormatUid;
	iVideoCodec = aVideoCodec.AllocL();
	iAudioCodec = aAudioCodec;

	iFindAndOpenController->ConfigureSourceSink(
		CMMFFindAndOpenController::TSourceSink(KUidMmfAudioInput, KNullDesC8),
		CMMFFindAndOpenController::TSourceSink(KUidMmfFileSink, CMMFFindAndOpenController::GetConfigFile(aFileName)));

	iFindAndOpenController->OpenByControllerUid(aControllerUid);
	}
	
void CVideoRecorderUtility::CBody::OpenFileL(const RFile& aFile,
											 TInt aCameraHandle,
											 TUid aControllerUid,
											 TUid aVideoFormatUid,
											 const TDesC8& aVideoCodec,
											 TFourCC aAudioCodec)
	{
	// Make sure we are closed
	Reset();
	iCameraHandle = aCameraHandle;
	iVideoFormatUid = aVideoFormatUid;
	iVideoCodec = aVideoCodec.AllocL();
	iAudioCodec = aAudioCodec;

	iFindAndOpenController->ConfigureSourceSink(
		CMMFFindAndOpenController::TSourceSink(KUidMmfAudioInput),
		CMMFFindAndOpenController::TSourceSink(KUidMmfFileSink, aFile));

	iFindAndOpenController->OpenByControllerUid(aControllerUid);
	}


//
// This method launches and initializes plugin controller based on the stream 
// source header passed in the descriptor buffer.
//
void CVideoRecorderUtility::CBody::OpenDesL(TDes8& aDescriptor,
											TInt aCameraHandle,
											TUid aControllerUid,
											TUid aVideoFormatUid,
											const TDesC8& aVideoCodec,
											TFourCC aAudioCodec)
	{
	// Make sure we are closed
	Reset();
	iCameraHandle = aCameraHandle;
	iVideoFormatUid = aVideoFormatUid;
	iVideoCodec = aVideoCodec.AllocL();
	iAudioCodec = aAudioCodec;

	iFindAndOpenController->ConfigureSourceSink(
		CMMFFindAndOpenController::TSourceSink(KUidMmfAudioInput,	KNullDesC8),
		CMMFFindAndOpenController::TSourceSink(KUidMmfDescriptorSink, CMMFFindAndOpenController::GetConfigDescriptor(aDescriptor)));

	iFindAndOpenController->OpenByControllerUid(aControllerUid);
	}

void CVideoRecorderUtility::CBody::OpenUrlL(const TDesC& aUrl,
											TInt aIapId,
											TInt aCameraHandle,
											TUid aControllerUid,
											TUid aVideoFormatUid,
											const TDesC8& aVideoCodec,
											TFourCC aAudioCodec)
	{
	// Make sure we are closed
	Reset();
	iCameraHandle = aCameraHandle;
	iVideoFormatUid = aVideoFormatUid;
	iVideoCodec = aVideoCodec.AllocL();
	iAudioCodec = aAudioCodec;

	CBufFlat* urlCfgBuffer = NULL;
	CMMFFindAndOpenController::GetConfigUrlL(urlCfgBuffer, aUrl, aIapId);

	iFindAndOpenController->ConfigureSourceSink(
		CMMFFindAndOpenController::TSourceSink(KUidMmfAudioInput, KNullDesC8),
		CMMFFindAndOpenController::TSourceSink(KUidMmfUrlSink, urlCfgBuffer->Ptr(0)));

	iFindAndOpenController->OpenByControllerUid(aControllerUid);
	delete urlCfgBuffer;
	}

void CVideoRecorderUtility::CBody::Record()
	{
	TInt err = KErrNone;
	
	// if controller in paused state then primed already
	if (iState != EPaused)
		{
		err = iController.Prime();
		
		// prime effectively puts controller into paused state
		// at start of file 
		if (err == KErrNone)
			iState = EPaused;
		}

	if (err == KErrNone)
		{
		err = iController.Play();
		}

	if (err != KErrNone)
		{
		iAsyncCallback->Signal(err,CMMFVideoRecorderCallback::ERecordCompleteEvent);
		}
	else
		{
		iState = ERecording;
		}
	}

TInt CVideoRecorderUtility::CBody::Stop()
	{
	TInt err = iController.Stop();
	iState = EStopped;
	return err;
	}

void CVideoRecorderUtility::CBody::Prepare()
	{
	TInt err = iVideoRecordControllerCustomCommands.Prepare();
	if (err != KErrNone)
		iAsyncCallback->Signal(err,CMMFVideoRecorderCallback::EPrepareCompleteEvent);
	else
		iState = EStopped;
	}

void CVideoRecorderUtility::CBody::Close()
	{
	if (iAsyncCallback) 
		iAsyncCallback->Cancel();
	if (iFindAndOpenController)
		iFindAndOpenController->Close();
	if (iControllerEventMonitor) 
		iControllerEventMonitor->Cancel();
	if(iControllerImplementationInformation)
		{
		delete iControllerImplementationInformation;
		iControllerImplementationInformation = NULL;
		}
  	iController.Close();	
	iControllerUid = KNullUid;
	}

void CVideoRecorderUtility::CBody::PauseL()
	{
	iController.Pause();
	iState = EPaused;
	}

void CVideoRecorderUtility::CBody::SetVideoFrameRateL(TReal32 aFramesPerSecond)
	{
	User::LeaveIfError(iVideoControllerCustomCommands.SetFrameRate(aFramesPerSecond));
	}

TReal32 CVideoRecorderUtility::CBody::VideoFrameRateL() const
	{
	TReal32 framerate = 0;
	User::LeaveIfError(iVideoControllerCustomCommands.GetFrameRate(framerate));
	return framerate;
	}

void CVideoRecorderUtility::CBody::GetVideoFrameSizeL(TSize& aSize) const
	{
	User::LeaveIfError(iVideoControllerCustomCommands.GetVideoFrameSize(aSize));
	}

TInt CVideoRecorderUtility::CBody::NumberOfMetaDataEntriesL() const
	{
	TInt num = 0;
	User::LeaveIfError(iController.GetNumberOfMetaDataEntries(num));
	return num;
	}

CMMFMetaDataEntry* CVideoRecorderUtility::CBody::MetaDataEntryL(TInt aMetaDataIndex) const
	{
	return iController.GetMetaDataEntryL(aMetaDataIndex);
	}

void CVideoRecorderUtility::CBody::AddMetaDataEntryL(const CMMFMetaDataEntry& aNewEntry)
	{
	iVideoRecordControllerCustomCommands.AddMetaDataEntryL(aNewEntry);
	}

void CVideoRecorderUtility::CBody::RemoveMetaDataEntryL(TInt aIndex)
	{
	User::LeaveIfError(iVideoRecordControllerCustomCommands.RemoveMetaDataEntry(aIndex));
	}

void CVideoRecorderUtility::CBody::ReplaceMetaDataEntryL(TInt aIndex, const CMMFMetaDataEntry& aNewEntry)
	{
	iVideoRecordControllerCustomCommands.ReplaceMetaDataEntryL(aIndex, aNewEntry);
	}

void CVideoRecorderUtility::CBody::SetPriorityL(TInt aPriority, TInt aPref)
	{
	iPrioritySettings.iPref = aPref;
	iPrioritySettings.iPriority = aPriority;

	User::LeaveIfError(iController.SetPrioritySettings(iPrioritySettings));
	}

TTimeIntervalMicroSeconds CVideoRecorderUtility::CBody::DurationL() const
	{
	TTimeIntervalMicroSeconds duration;
	User::LeaveIfError(iController.GetDuration(duration));
	return duration;
	}

TInt CVideoRecorderUtility::CBody::VideoBitRateL()
	{
	TInt bitRate;
	User::LeaveIfError(iVideoControllerCustomCommands.GetVideoBitRate(bitRate));
	return bitRate;
	}

void CVideoRecorderUtility::CBody::SetVideoBitRateL(TInt aVideoBitRate)
	{
	User::LeaveIfError(iVideoRecordControllerCustomCommands.SetVideoBitRate(aVideoBitRate));
	}

void CVideoRecorderUtility::CBody::SetAudioBitRateL(TInt aAudioBitRate)
	{
	User::LeaveIfError(iVideoRecordControllerCustomCommands.SetAudioBitRate(aAudioBitRate));
	}

TInt CVideoRecorderUtility::CBody::AudioBitRateL() const
	{
	TInt bitRate;
	User::LeaveIfError(iVideoControllerCustomCommands.GetAudioBitRate(bitRate));
	return bitRate;
	}

TBool CVideoRecorderUtility::CBody::AudioEnabledL() const
	{
	TBool enabled;
	User::LeaveIfError(iVideoRecordControllerCustomCommands.GetAudioEnabled(enabled));
	return enabled;
	}

void CVideoRecorderUtility::CBody::SetAudioEnabledL(TBool aEnabled)
	{
	User::LeaveIfError(iVideoRecordControllerCustomCommands.SetAudioEnabled(aEnabled));
	}

void CVideoRecorderUtility::CBody::SetGainL(TInt aGain)
	{
	User::LeaveIfError(iAudioRecordDeviceCustomCommands.SetGain(aGain));
	}

TInt CVideoRecorderUtility::CBody::GainL() const
	{
	TInt gain;
	User::LeaveIfError(iAudioRecordDeviceCustomCommands.GetGain(gain));
	return gain;
	}

TInt CVideoRecorderUtility::CBody::MaxGainL() const
	{
	TInt maxGain;
	User::LeaveIfError(iAudioRecordDeviceCustomCommands.GetMaxGain(maxGain));
	return maxGain;
	}

void CVideoRecorderUtility::CBody::SetMaxClipSizeL(TInt aClipSizeInBytes) 
	{
	User::LeaveIfError(iVideoRecordControllerCustomCommands.SetMaxFileSize(aClipSizeInBytes));
	}

void CVideoRecorderUtility::CBody::SetVideoFrameSizeL(const TSize& aSize) 
	{
	User::LeaveIfError(iVideoRecordControllerCustomCommands.SetVideoFrameSize(aSize));
	}

void CVideoRecorderUtility::CBody::GetPriorityL(TInt & aPriority, TMdaPriorityPreference &aPref) const 
	{
	aPriority = iPrioritySettings.iPriority;
	aPref = TMdaPriorityPreference(iPrioritySettings.iPref);
	}

void CVideoRecorderUtility::CBody::SetAudioTypeL(TFourCC aDataType)
	{
	User::LeaveIfError(iVideoRecordControllerCustomCommands.SetAudioCodec(aDataType));
	}

void CVideoRecorderUtility::CBody::SetVideoTypeL(const TDesC8& aDataType)
	{
	User::LeaveIfError(iVideoRecordControllerCustomCommands.SetVideoCodec(aDataType));
	}

TFourCC CVideoRecorderUtility::CBody::AudioTypeL() const
	{
	TFourCC dataType;
	User::LeaveIfError(iVideoControllerCustomCommands.GetAudioCodec(dataType));
	return dataType;
	}

void CVideoRecorderUtility::CBody::GetSupportedAudioTypesL(RArray<TFourCC>& aAudioTypes) const
	{
	User::LeaveIfError(iVideoRecordControllerCustomCommands.GetSupportedSinkAudioTypes(aAudioTypes));
	}

void CVideoRecorderUtility::CBody::GetSupportedVideoTypesL(CDesC8Array& aVideoTypes) const
	{
	User::LeaveIfError(iVideoRecordControllerCustomCommands.GetSupportedSinkVideoTypes(aVideoTypes));
	}

void CVideoRecorderUtility::CBody::HandleEvent(const TMMFEvent& aEvent)
	{
	if (aEvent.iEventType == KMMFEventCategoryVideoOpenComplete)  
		{
		// if we haven't had a MfaocComplete() callback yet,
		// we need to delay the call back to the client
		// because the source/sink will not have been opened yet.
		iEventOpenReceived = ETrue;
		if (iOpenError == KErrNone)
			iOpenError = aEvent.iErrorCode;
		if (iCallbackOpenReceived)
			iObserver.MvruoOpenComplete(aEvent.iErrorCode);
		}
	else if (aEvent.iEventType == KMMFEventCategoryVideoPrepareComplete)  
		{
		iObserver.MvruoPrepareComplete(aEvent.iErrorCode);
		}
	else if ((aEvent.iEventType == KMMFEventCategoryPlaybackComplete) ||
			(aEvent.iEventType == KMMFErrorCategoryControllerGeneralError))
		{
		TInt oldState = iState;
		iState = EStopped;

		//if we weren't recording, don't advise Client.
  		if((oldState == ERecording)  || (oldState == EPaused))
  			iObserver.MvruoRecordComplete(aEvent.iErrorCode);
		} 
	else 
		// Pass on all unrecognised events to the client
		iObserver.MvruoEvent(aEvent);
	}

TTimeIntervalMicroSeconds CVideoRecorderUtility::CBody::RecordTimeAvailable() const
	{
	TTimeIntervalMicroSeconds recordTime;
	iVideoRecordControllerCustomCommands.GetRecordTimeAvailable(recordTime);
	return recordTime;
	}

TInt CVideoRecorderUtility::CBody::CustomCommandSync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2, TDes8& aDataFrom)
	{
	return iController.CustomCommandSync(aDestination, aFunction, aDataTo1, aDataTo2, aDataFrom);
	}
	
TInt CVideoRecorderUtility::CBody::CustomCommandSync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2)
	{
	return iController.CustomCommandSync(aDestination, aFunction, aDataTo1, aDataTo2);
	}
	
void CVideoRecorderUtility::CBody::CustomCommandAsync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2, TDes8& aDataFrom, TRequestStatus& aStatus)
	{
	iController.CustomCommandAsync(aDestination, aFunction, aDataTo1, aDataTo2, aDataFrom, aStatus);
	}
	
void CVideoRecorderUtility::CBody::CustomCommandAsync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2, TRequestStatus& aStatus)
	{
	iController.CustomCommandAsync(aDestination, aFunction, aDataTo1, aDataTo2, aStatus);
	}
	
const CMMFControllerImplementationInformation& CVideoRecorderUtility::CBody::ControllerImplementationInformationL()
	{
	if (!iControllerImplementationInformation)
		{
		if (iControllerUid==KNullUid)
			User::Leave(KErrNotReady);
		iControllerImplementationInformation = CMMFControllerImplementationInformation::NewL(iControllerUid);
		}
	return *iControllerImplementationInformation;
	}

const TDesC8& CVideoRecorderUtility::CBody::VideoFormatMimeType() const
	{
	TPtr8 des = iMimeType->Des();
	TInt err = iVideoControllerCustomCommands.GetVideoMimeType(des);
	if (!err)
		return *iMimeType;
	else
		return KNullDesC8;
	}

void CVideoRecorderUtility::CBody::SetPixelAspectRatioL(const TVideoAspectRatio& aAspectRatio)
	{
	User::LeaveIfError(iVideoPixelAspectRatioCustomCommands.SetPixelAspectRatio(aAspectRatio));
	}
	
void CVideoRecorderUtility::CBody::GetPixelAspectRatioL(TVideoAspectRatio& aAspectRatio) const
	{
	User::LeaveIfError(iVideoPixelAspectRatioCustomCommands.GetPixelAspectRatio(aAspectRatio));
	}
	
void CVideoRecorderUtility::CBody::GetSupportedPixelAspectRatiosL(RArray<TVideoAspectRatio>& aAspectRatios) const
	{
	iVideoPixelAspectRatioCustomCommands.GetSupportedPixelAspectRatiosL(aAspectRatios);
	}
	
void CVideoRecorderUtility::CBody::SetAudioChannelsL(const TUint aNumChannels)
	{
	User::LeaveIfError(iVideoAudioSamplingRateAndChannelConfigCustomCommands.SetAudioChannels(aNumChannels));
	}
	
TUint CVideoRecorderUtility::CBody::AudioChannelsL() const
	{
	TUint numChannels = 0;
	User::LeaveIfError(iVideoAudioSamplingRateAndChannelConfigCustomCommands.GetAudioChannels(numChannels));
	return numChannels;
	}
	
void CVideoRecorderUtility::CBody::GetSupportedAudioChannelsL(RArray<TUint>& aChannels) const
	{
	iVideoAudioSamplingRateAndChannelConfigCustomCommands.GetSupportedAudioChannelsL(aChannels);
	}
	
void CVideoRecorderUtility::CBody::SetAudioSampleRateL(const TUint aSampleRate)
	{
	User::LeaveIfError(iVideoAudioSamplingRateAndChannelConfigCustomCommands.SetAudioSampleRate(aSampleRate));
	}
	
TUint CVideoRecorderUtility::CBody::AudioSampleRateL() const
	{
	TUint sampleRate = 0;
	User::LeaveIfError(iVideoAudioSamplingRateAndChannelConfigCustomCommands.GetAudioSampleRate(sampleRate));
	return sampleRate;
	}
	
void CVideoRecorderUtility::CBody::GetSupportedAudioSampleRatesL(RArray<TUint>& aSampleRates) const
	{
	iVideoAudioSamplingRateAndChannelConfigCustomCommands.GetSupportedAudioSampleRatesL(aSampleRates);
	}

void CVideoRecorderUtility::CBody::SetVideoEnabledL(TBool aEnabled)
	{
	//Leave if Open is not yet complete or not in stop state.
	if ((EFalse == iEventOpenReceived) || (EStopped != iState) )
		{
		User::Leave(KErrNotReady);
		}
	User::LeaveIfError(iVideoRecordControllerExtCustomCommands.SetVideoEnabled(aEnabled));
	}

TBool CVideoRecorderUtility::CBody::VideoEnabledL() const
	{
	TBool videoEnabled;
	TInt error;

	//Leave if Open is not yet complete.
	if ( EFalse == iEventOpenReceived )
		{
		User::Leave(KErrNotReady);
		}
	
	error = iVideoRecordControllerExtCustomCommands.VideoEnabled(videoEnabled);
	/* if customcommand is not implemented by controller pluggin return default value ETrue */
	if (KErrNotSupported == error)
		{
		videoEnabled = ETrue;
		}
	else
		{
		User::LeaveIfError(	error);	
		}
	return videoEnabled;
	}

void CVideoRecorderUtility::CBody::SetVideoQualityL(TInt aQuality)
	{
	//Leave with KErrArgument if input aQuality is not in 0-100 range;
	if ((aQuality < 0) || (aQuality > EVideoQualityLossless) )
		{
		User::Leave(KErrArgument);
		}

	//Leave if Open is not yet complete or not in stop state.
	if ((EFalse == iEventOpenReceived) || (EStopped != iState) )
		{
		User::Leave(KErrNotReady);
		}
	
	User::LeaveIfError(iVideoRecordControllerExtCustomCommands.SetVideoQuality(aQuality));
	}

TInt CVideoRecorderUtility::CBody::VideoQualityL() const
	{
	TInt quality;
	TInt error;

	//Leave if Open is not yet complete.
	if ( EFalse == iEventOpenReceived )
		{
		User::Leave(KErrNotReady);
		}

	error = iVideoRecordControllerExtCustomCommands.VideoQuality(quality);

	/* if customcommand is not implemented by controller pluggin return default value EVideoQualityNormal */
	if (KErrNotSupported == error)
		{
		quality = EVideoQualityNormal;
		}
	else
		{
		User::LeaveIfError(	error);	
		}	
	
	return quality;
	}

void CVideoRecorderUtility::CBody::SetVideoFrameRateFixedL(TBool aFixedFrameRate)
	{

	//Leave if Open is not yet complete or not in stop state.
	if ((EFalse == iEventOpenReceived) || (EStopped != iState) )
		{
		User::Leave(KErrNotReady);
		}
	
	User::LeaveIfError(iVideoRecordControllerExtCustomCommands.SetVideoFrameRateFixed(aFixedFrameRate));
	}

TBool CVideoRecorderUtility::CBody::VideoFrameRateFixedL() const
	{
	TBool fixedFrameRate;
	TInt error;

	//Leave if Open is not yet complete.
	if ( EFalse == iEventOpenReceived )
		{
		User::Leave(KErrNotReady);
		}
	
	error = iVideoRecordControllerExtCustomCommands.VideoFrameRateFixed(fixedFrameRate);
	/* if customcommand is not implemented by controller pluggin return default value EFalse */
	if (KErrNotSupported == error)
		{
		fixedFrameRate = EFalse;
		}
	else
		{
		User::LeaveIfError(error);
		}
	return fixedFrameRate;
	}

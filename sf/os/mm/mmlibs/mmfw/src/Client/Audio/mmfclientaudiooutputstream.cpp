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

#include <mdaaudiooutputstream.h>
#include <mda/common/audio.h>
#include "mmfclientaudiooutputstream.h"
#include "mmfclientaudiostreamutils.h"
#include "MmfFifo.h"
#include <mmf/common/mmfpaniccodes.h>

#define WAIT_FOR_READY_ACTIVE_OBJECTS

const TInt KMicroSecsInOneSec = 1000000; 
const TInt KUnknownVolume = -1; // means "don't set", must be negative
const TInt KShutDownTimeInterval = 100000; //100 milli seconds

/**
 *
 * Static NewL
 *
 * @return CMdaAudioOutputStream*
 *
 */
EXPORT_C CMdaAudioOutputStream* CMdaAudioOutputStream::NewL(MMdaAudioOutputStreamCallback& aCallback,
															CMdaServer* /*aServer = NULL*/)
	{
	return NewL(aCallback, EMdaPriorityNormal, EMdaPriorityPreferenceTimeAndQuality);
	}

/**
 *
 * Static NewL
 *
 * @return CMdaAudioOutputStream*
 *
 */
EXPORT_C CMdaAudioOutputStream* CMdaAudioOutputStream::NewL(MMdaAudioOutputStreamCallback& aCallback,
															TInt aPriority,
															TInt aPref /*=EMdaPriorityPreferenceTimeAndQuality*/)
	{
	CMdaAudioOutputStream* self = new(ELeave) CMdaAudioOutputStream();
	CleanupStack::PushL(self);
	self->iProperties = CMMFMdaAudioOutputStream::NewL(aCallback, aPriority, aPref);
	CleanupStack::Pop(self);
	return self;
	}

CMdaAudioOutputStream::CMdaAudioOutputStream()
	{
	}

CMdaAudioOutputStream::~CMdaAudioOutputStream()
	{
	delete iProperties;
	}

void CMdaAudioOutputStream::SetAudioPropertiesL(TInt aSampleRate, TInt aChannels)
	{
	ASSERT(iProperties);
	iProperties->SetAudioPropertiesL(aSampleRate, aChannels);
	}

void CMdaAudioOutputStream::Open(TMdaPackage* aSettings)
	{
	ASSERT(iProperties);
	iProperties->Open(aSettings);
	}

TInt CMdaAudioOutputStream::MaxVolume()
	{
	ASSERT(iProperties);
	return iProperties->MaxVolume();
	}

TInt CMdaAudioOutputStream::Volume()
	{
	ASSERT(iProperties);
	return iProperties->Volume();
	}

void CMdaAudioOutputStream::SetVolume(const TInt aNewVolume)
	{
	ASSERT(iProperties);
	iProperties->SetVolume(aNewVolume);
	}

void CMdaAudioOutputStream::SetPriority(TInt aPriority, TInt aPref)
	{
	ASSERT(iProperties);
	iProperties->SetPriority(aPriority, aPref);
	}

void CMdaAudioOutputStream::WriteL(const TDesC8& aData)
	{
	ASSERT(iProperties);
	iProperties->WriteL(aData);
	}

void CMdaAudioOutputStream::Stop()
	{
	ASSERT(iProperties);
	iProperties->Stop();
	}

EXPORT_C TInt CMdaAudioOutputStream::Pause()
	{
	ASSERT(iProperties);
	return iProperties->Pause();
	}

EXPORT_C TInt CMdaAudioOutputStream::Resume()
	{
	ASSERT(iProperties);
	return iProperties->Resume();
	}

const TTimeIntervalMicroSeconds& CMdaAudioOutputStream::Position()
	{
	ASSERT(iProperties);
	return iProperties->Position();
	}

EXPORT_C void CMdaAudioOutputStream::SetBalanceL(TInt aBalance)
	{
	ASSERT(iProperties);
	iProperties->SetBalanceL(aBalance);
	}

EXPORT_C TInt CMdaAudioOutputStream::GetBalanceL() const
	{
	ASSERT(iProperties);
	return iProperties->GetBalanceL();
	}

EXPORT_C TInt CMdaAudioOutputStream::GetBytes()
	{
	ASSERT(iProperties);
	return iProperties->GetBytes();
	}

EXPORT_C TInt CMdaAudioOutputStream::KeepOpenAtEnd()
	{
	ASSERT(iProperties);
	return iProperties->KeepOpenAtEnd();
	}

EXPORT_C TInt CMdaAudioOutputStream::RequestStop()
	{
	ASSERT(iProperties);
	return iProperties->RequestStop();
	}	
	
/**

*/
EXPORT_C void CMdaAudioOutputStream::SetDataTypeL(TFourCC aAudioType)
	{
	ASSERT(iProperties);
	iProperties->SetDataTypeL(aAudioType);
	}	


/**

*/
EXPORT_C TFourCC CMdaAudioOutputStream::DataType() const
	{
	ASSERT(iProperties);
	return iProperties->DataType();
	}
	
EXPORT_C TAny* CMdaAudioOutputStream::CustomInterface(TUid aInterfaceId)
	{
	ASSERT(iProperties);
	return iProperties->CustomInterface(aInterfaceId);
	}
	

/**
Registers the Event for Notification when resource is avaliable.

@param  aCallback
		The audio player observer interface.
@param  aNotificationEventUid
		The event uid to notify the client.
@param  aNotificationRegistrationData
		Notification registration specific data.

@return An error code indicating if the registration was successful. KErrNone on success, 
		otherwise another of the system-wide error codes.
*/
EXPORT_C TInt CMdaAudioOutputStream::RegisterAudioResourceNotification(MMMFAudioResourceNotificationCallback& aCallback,
																		TUid aNotificationEventUid,
																		const TDesC8& aNotificationRegistrationData)
	{
	ASSERT(iProperties);
	return iProperties->RegisterAudioResourceNotification(aCallback,aNotificationEventUid,aNotificationRegistrationData);
	}

/**
Cancels the registered notification event.

@param  aNotificationEventUid
	The Event to notify the client.
	
@return An error code indicating if the registration was successful. KErrNone on success, 
	otherwise another of the system-wide error codes.
*/
EXPORT_C TInt CMdaAudioOutputStream::CancelRegisterAudioResourceNotification(TUid aNotificationEventUid)	
	{
	ASSERT(iProperties);
	return iProperties->CancelRegisterAudioResourceNotification(aNotificationEventUid);
	}

/**
Waits for the client to resume the play even after the default timer expires.

@return An error code indicating if the registration was successful. KErrNone on success, 
		otherwise another of the system-wide error codes.
*/
EXPORT_C TInt CMdaAudioOutputStream::WillResumePlay()	
	{
	ASSERT(iProperties);
	return iProperties->WillResumePlay();
	}

enum TMmAosPanic
	{
	EToneFinishedNotSupported,
	EBufferToBeEmptiedNotSupported,
	ERecordErrorNotSupported,
	EConvertErrorNotSupported,
	EDeviceMessageNotSupported,
	EReservedCall,
	EAOSStoppingError
	};

_LIT(KMmfMdaAosCategory, "CMmfMdaAudioOutputStream");
LOCAL_C void Panic(const TMmAosPanic aReason)
	{
	User::Panic(KMmfMdaAosCategory, aReason);
	}

/**
 *
 * 2 phase construction function
 *
 */
CMMFMdaAudioOutputStream* CMMFMdaAudioOutputStream::NewL(MMdaAudioOutputStreamCallback& aCallback)
	{
	return NewL(aCallback, EMdaPriorityNormal, EMdaPriorityPreferenceTimeAndQuality);
	}

CMMFMdaAudioOutputStream* CMMFMdaAudioOutputStream::NewL(MMdaAudioOutputStreamCallback& aCallback, TInt aPriority, TInt aPref)
	{
	CMMFMdaAudioOutputStream* self = new(ELeave) CMMFMdaAudioOutputStream(aCallback);
	CleanupStack::PushL(self);
	self->ConstructL(aPriority, aPref);
	CleanupStack::Pop(); // self
	return self;
	}

/**
 *
 * Construct
 *
 * @param	"MMdaAudioOutputStreamCallback&"
 *			a reference to MMdaAudioOutputStreamCallback
 * @param	"TInt aPriority"
 *			a priority value			
 * @param	"TInt aPref"
 *			a perference value
 *
 */
CMMFMdaAudioOutputStream::CMMFMdaAudioOutputStream(MMdaAudioOutputStreamCallback& aCallback)
	: iCallback(aCallback), iState(EStopped)
	{
	iDataTypeCode.Set(TFourCC(' ','P','1','6'));
	}

/**
 *
 *	Second phase constructor
 *
 */
void CMMFMdaAudioOutputStream::ConstructL(TInt aPriority, TInt aPref)
	{
	iDevSound = CMMFDevSound::NewL();
	SetPriority(aPriority, aPref);
	iDevSoundIgnoresUnderflow = iDevSound->QueryIgnoresUnderflow();
	iFifo = new(ELeave) CMMFFifo<const TDesC8>();
	iActiveCallback = new(ELeave) CActiveCallback(iCallback);
	iActiveSchedulerWait = new(ELeave) CActiveSchedulerWait;
	iShutDownTimer = CPeriodic::NewL(CActive::EPriorityStandard);
	}

/**
 *
 *	Destructor
 *
 */
CMMFMdaAudioOutputStream::~CMMFMdaAudioOutputStream()
	{
	delete iFifo;
	delete iDevSound;
	delete iActiveCallback;
	delete iActiveSchedulerWait;
	delete iShutDownTimer;
	}

/**
 *
 *  Set audio output stream properties	
 *
 *	@param	"TInt aSampleRate"	
 *			a specified priority value
 *	@param	"TInt aChannels"		
 *			a specified preference value
 *
 */
void CMMFMdaAudioOutputStream::SetAudioPropertiesL(TInt aSampleRate, TInt aChannels)
	{
	if (iIsOpenState==EIsOpen)
		{
		RealSetAudioPropertiesL(aSampleRate, aChannels);
		}
	else
		{
		// cache params for application later
		iSampleRate = aSampleRate;
		iChannels = aChannels;
		iVolume = KUnknownVolume;
		iValuesCached = ETrue;
		}
	}

void CMMFMdaAudioOutputStream::RealSetAudioPropertiesL(TInt aSampleRate, TInt aChannels)
	{
	TMMFCapabilities config = iDevSound->Config();
	config.iChannels = StreamUtils::MapChannelsMdaToMMFL(aChannels);
	config.iRate = StreamUtils::MapSampleRateMdaToMMFL(aSampleRate);
	iDevSound->SetConfigL(config);
	}

/**
 *
 *  Open a audio ouptut stream	
 *
 *	@param	"TMdaPackage* Settings"	
 *			a pointer point to TMdaPackage
 *
 */
void CMMFMdaAudioOutputStream::Open(TMdaPackage* aSettings)
	{
	iIsOpenState = EIsOpening;
	
	// Use settings to set audio properties after the dev sound has been
	// successfully initialised. Note if the InitializeL() fails, something calls
	// InitializeComplete() and the iValuesCached flag is cleared. Also note
	// that if SetAudioPropertiesL() has already been called, there may already
	// be cached values
	if (aSettings && aSettings->Type().iUid == KUidMdaDataTypeSettingsDefine)
		{
		TMdaAudioDataSettings& audioSettings = *STATIC_CAST(TMdaAudioDataSettings*, aSettings);
		//Caching these values, which are later set in InitializeComplete
		iSampleRate = audioSettings.iSampleRate;
		iChannels = audioSettings.iChannels;
		iVolume = audioSettings.iVolume;
		iValuesCached = ETrue;
		}

	TRAPD(err, iDevSound->InitializeL(*this, iDataTypeCode, EMMFStatePlaying));
	if (err != KErrNone)
		{
		InitializeComplete(err);
		}
	}

/**
 *
 *  To get the maximum volume level	
 *
 *	@return	"TInt"	
 *			the maximum volume value in integer
 *
 */
TInt CMMFMdaAudioOutputStream::MaxVolume()
	{
	return iDevSound->MaxVolume();
	}

/**
 *
 *  To get the current volume level	
 *
 *	@return	"TInt"	
 *			the current volume value in integer
 *
 */
TInt CMMFMdaAudioOutputStream::Volume()
	{
	return iDevSound->Volume();
	} 

/**
 *
 *  Set audio output stream volume to the specified value
 *
 *	@param	"TInt aVolume"	
 *			a specified volume value
 *
 */
void CMMFMdaAudioOutputStream::SetVolume(TInt aVolume)
	{
	iDevSound->SetVolume(aVolume);
	}

/**
 *
 *  Set audio output stream balance	
 *
 *	@param	"TInt aBalance"	
 *			a specified balance value
 *
 */
void CMMFMdaAudioOutputStream::SetBalanceL(TInt aBalance)
	{
	// test and clip balance to min / max range [-100 <-> 100]
	// clip rather than leave as this isn't a leaving function
	if (aBalance < KMMFBalanceMaxLeft) aBalance = KMMFBalanceMaxLeft;
	if (aBalance > KMMFBalanceMaxRight) aBalance = KMMFBalanceMaxRight;

	// separate out left and right balance
	TInt left  = 0;
	TInt right = 0;
	StreamUtils::CalculateLeftRightBalance( left, right, aBalance );

	// send the balance to SoundDevice
	iDevSound->SetPlayBalanceL(left, right);
	}

/**
 *
 *  To get the current balance value. This function may not return the same value 
 *	as passed to SetBalanceL depending on the internal implementation in 
 *	the underlying components.
 *	
 *	@return	"TInt"	
 *			the current balance value in integer
 *
 */
TInt CMMFMdaAudioOutputStream::GetBalanceL() const
	{
	TInt rightBalance = 0;
	TInt leftBalance  = 0;
	iDevSound->GetPlayBalanceL(leftBalance, rightBalance);
	TInt balance  = 0;
	StreamUtils::CalculateBalance( balance, leftBalance, rightBalance );
	return balance;
	}

/**
 *
 *  Set audio output stream priority	
 *
 *	@param	"TInt aPriority"	
 *			a specified priority value
 *	@param	"TMdaPriorityPreference aPref"		
 *			a specified preference value
 *
 */
void CMMFMdaAudioOutputStream::SetPriority(TInt aPriority, TInt aPref)
	{
	TMMFPrioritySettings settings;
	settings.iPriority = aPriority;
	settings.iPref = aPref;
	iDevSound->SetPrioritySettings(settings);
	}

/**
 *
 *  To write data to output stream 	
 *  Note that if framed data eg gsm610 is being streamed then the buffer
 *  size of aData should contain an intiger number of frames
 *
 *	@param	"const TDesC8& aData"	
 *			a stream data 
 *
 */
void CMMFMdaAudioOutputStream::WriteL(const TDesC8& aData)
	{
	if(iState==EStopping)
		{
		User::Leave(KErrNotReady);
		}
	iShutDownTimer->Cancel();
	
	TMMFFifoItem<const TDesC8>* item = new(ELeave) TMMFFifoItem<const TDesC8>(aData);
	iFifo->AddToFifo(*item);

	if(iState == EStopped)
		StartPlayL();
	else if((iState == EPlaying) && (iBuffer != NULL))
		{ //if we are playing and we have a buffer waiting for data, use it.
		BufferToBeFilled(iBuffer);
		iBuffer = NULL;
		}
	if(iEventHolder != KNullUid)
		{
		TInt err = iDevSound->RegisterAsClient(iEventHolder,iNotificationDataHolder);
		iEventHolder = KNullUid;
		iNotificationDataHolder = KNullDesC8;
		if(err != KErrNone)
			{
			iCallback.MaoscPlayComplete(err);
			}
		}
	}

/**
 *
 * If the audio stream is stopped, then notify the CDevSound to initilised play.
 * The CDevSound will automatically start calling back for buffers.
 */
void CMMFMdaAudioOutputStream::StartPlayL()
{
	if (iState == EStopped)
		{
		iFifoItemPos = 0;
		iCurrentSamplesPlayed = 0;
		iDevSound->PlayInitL();
		iState = EPlaying;
		iBuffer = NULL;
		}
}


/**
 *
 *  To stop write data to stream 	
 *
 */
void CMMFMdaAudioOutputStream::Stop()
	{
	iShutDownTimer->Cancel();
	EmptyFifo(KErrAbort);

	if(iState == EStopped)
		{
		return;
		}
		
	// stop the operation
	iDevSound->Stop();
	iState = EStopped;
	iBuffer = NULL;

	iCallback.MaoscPlayComplete(KErrCancel);
	}

TInt CMMFMdaAudioOutputStream::RequestStop()
	{
	if(!iKeepOpenAtEnd)
		{
		return KErrNotSupported;
		}
	
	if(iState==EStopped || iState==EStopping)
		{
		return KErrNotReady;
		}
		
	if(iBuffer)
		{//means all the client buffers are used up and waiting for more data
		CMMFDataBuffer* dataBuffer = static_cast<CMMFDataBuffer*>(iBuffer);
		dataBuffer->Data().SetLength(0);
		dataBuffer->SetLastBuffer(ETrue);
		iDevSound->PlayData();
		iBuffer=NULL;
		}
	iState = EStopping;
	return KErrNone;
	}

TInt CMMFMdaAudioOutputStream::KeepOpenAtEnd()
	{
	if(!iDevSoundIgnoresUnderflow)
		{
		return KErrNotSupported;
		}
	else
		{
		iKeepOpenAtEnd = ETrue;
		return KErrNone;
		}
	}

/**
 *
 *  To pause send data to stream 	
 *
 */
TInt CMMFMdaAudioOutputStream::Pause()
	{
	if(iState != EPlaying)
		{
		return KErrNotReady;
		}

	else if(!iDevSound->IsResumeSupported())
		{
		return KErrNotSupported;
		}
	
	// pause the operation
	iDevSound->Pause();
	iState = EPaused;
	return KErrNone;
	}

/**
 *
 *  To resume send data to stream 	
 *
 */
TInt CMMFMdaAudioOutputStream::Resume()
	{
	TInt err = KErrNone;
	if(iState != EPaused)
		{
		err = KErrNotReady;
		}
	else if(!iDevSound->IsResumeSupported())
		{
		err = KErrNotSupported;
		}
	
	// resume the operation
	if(err == KErrNone)
		{
		err = iDevSound->Resume();
		}
	if(err == KErrNone)
		{
		iState = EPlaying;
		}

	return err;
	}


/**
 *
 *  To get the current position in the data stream	
 *
 *	@return	"TTimeIntervalMicroSeconds&"	
 *			the current position in integer
 *
 */
const TTimeIntervalMicroSeconds& CMMFMdaAudioOutputStream::Position()
	{
	TInt64 position = iDevSound->SamplesPlayed();
	position = position * KMicroSecsInOneSec / StreamUtils::SampleRateAsValue(iDevSound->Config());
	iPosition = (iState == EPlaying || iState == EStopping || iState == EPaused) ? position : 0; // Shouldn't need to check for playing but CMMFDevSound doesn't reset bytes played after a stop
	return iPosition;
	}

/**
 *
 *  To return the current number of bytes rendered by audio hardware
 *	@return "the current current number of bytes rendered by audio hardware in integer"	
 *
 */
TInt CMMFMdaAudioOutputStream::GetBytes()
	{
	return iDevSound->SamplesPlayed() * StreamUtils::BytesPerSample(iDevSound->Config());
	}

/**

*/
void CMMFMdaAudioOutputStream::SetDataTypeL(TFourCC aAudioType)
	{
	if(iState != EStopped)
		User::Leave(KErrServerBusy);

	if(aAudioType == iDataTypeCode)
		return;

	TMMFPrioritySettings prioritySettings; 	
	prioritySettings.iState = EMMFStatePlaying;
	RArray<TFourCC> supportedDataTypes;
		
	CleanupClosePushL(supportedDataTypes);
	
	TRAPD(err, iDevSound->GetSupportedInputDataTypesL(supportedDataTypes, prioritySettings));

	if (err == KErrNone)
		{
		if (supportedDataTypes.Find(aAudioType) == KErrNotFound)
			{
			User::Leave(KErrNotSupported);	
			}
		//if match, set the 4CC of AudioType to match
		iDataTypeCode.Set(aAudioType);
		}
	else  //we had a real leave error from GetSupportedOuputDataTypesL
		{
		User::Leave(err);
		}
	
	CleanupStack::PopAndDestroy(&supportedDataTypes);

	if(iIsOpenState!=EIsNotOpen)
		{
		// need to recall or restart InitializeL() process
		iDevSound->CancelInitialize(); // call just in case mid-InitializeL. No harm if not. 
									   // if not supported then assume old DevSound behaviour anyway
									   // where InitializeL() implicitly cancels, so no harm either way
		iIsOpenState = EIsOpening;
		iInitCallFrmSetDataType = ETrue;
		TRAPD(err, iDevSound->InitializeL(*this, iDataTypeCode, EMMFStatePlaying));
		if (err != KErrNone)
			{
			// Leave if error.
			iIsOpenState = EIsNotOpen;
			iInitCallFrmSetDataType = EFalse;
			User::Leave(err);
			}
		// In some implementations InitializeComplete is sent 
		// in context, so check before starting activeSchedulerWait.
		else if(iIsOpenState == EIsOpening)
			{
			iInitializeState = KRequestPending;
			iActiveSchedulerWait->Start();
			}
		iInitCallFrmSetDataType = EFalse;
		User::LeaveIfError(iInitializeState);
		}	
	}

/**

*/
TFourCC CMMFMdaAudioOutputStream::DataType() const
	{
	return iDataTypeCode;
	}

TInt CMMFMdaAudioOutputStream::RegisterAudioResourceNotification(MMMFAudioResourceNotificationCallback& aCallback,TUid aNotificationEventUid,const TDesC8& aNotificationRegistrationData)
	{
	iAudioResourceNotificationCallBack = &aCallback;
	TInt err = iDevSound->RegisterAsClient(aNotificationEventUid,aNotificationRegistrationData);
	if(err == KErrNotReady)
		{
		iEventHolder = 	aNotificationEventUid;
		iNotificationDataHolder = aNotificationRegistrationData;
		return KErrNone;
		}
	iEventHolder = KNullUid;
	iNotificationDataHolder = KNullDesC8;
	return err;
	}

TInt CMMFMdaAudioOutputStream::CancelRegisterAudioResourceNotification(TUid aNotificationEventId)
	{
	TInt err = iDevSound->CancelRegisterAsClient(aNotificationEventId);
	if(err == KErrNotReady)
		{
		if(aNotificationEventId != KMMFEventCategoryAudioResourceAvailable)	
			{
			return KErrNotSupported;
			}
		if(iEventHolder == KNullUid)	
			{
			return KErrCancel;
			}		
		iEventHolder = KNullUid;
		iNotificationDataHolder = KNullDesC8;
		return KErrNone;
		}
	return err;
	}
	
TInt CMMFMdaAudioOutputStream::WillResumePlay()
	{
	return iDevSound->WillResumePlay();
	}

/**
 *
 *  To be called when intialize stream complete	
 *
 *	@param	"TInt aError"	
 *			error code, initialize stream succeed when aError = 0
 *
 */
void CMMFMdaAudioOutputStream::InitializeComplete(TInt aError)
	{
	TInt err = aError;
	if(err == KErrNone && iValuesCached)
		{
		TRAP(err, RealSetAudioPropertiesL(iSampleRate, iChannels));
		if(err == KErrNone && iVolume>=0)
			{
			SetVolume(iVolume);
			}
		}
	iValuesCached = EFalse; // whatever clear our cache
	if(iIsOpenState == EIsOpening)
		{
		// Signal for the MaoscOpenComplete callback to be called asynchronously.Ignore if InitializeL is called from SetDataTypeL
		if(!iInitCallFrmSetDataType)
			{
			iActiveCallback->Signal(err);
			}
		iIsOpenState = err ? EIsNotOpen : EIsOpen;
		if(iInitializeState == KRequestPending)
			{
			iInitializeState = err;
			iActiveSchedulerWait->AsyncStop();
			}
		else
			{
			iInitializeState = err;//Set the error if InitializeComplete is called in context of InitializeL.
			}
		}
	}

/**
 *
 *  Do not support
 *
 */
void CMMFMdaAudioOutputStream::ToneFinished(TInt /*aError*/)
	{
	Panic(EToneFinishedNotSupported);
	}

/**
 *
 *  Called when sound device need data	
 *
 *	@param	"CMMFBuffer* aBuffer"	
 *			a pointer point to CMMFBuffer, which is used to stored data
 *
 */
void CMMFMdaAudioOutputStream::BufferToBeFilled(CMMFBuffer* aBuffer)
	{
	if (iState == EPlaying || iState == EStopping)
		{
		TMMFFifoItem<const TDesC8>* firstItem = iFifo->Get();
		//ASSERT firstItem != NULL
		if (iFifoItemPos >= firstItem->GetData().Length())
			{
			// We've played all segments of the first buffer in the fifo so we can delete it
			iFifo->RemoveFirstItem();
			iCallback.MaoscBufferCopied(KErrNone, firstItem->GetData());
			delete firstItem;
			iFifoItemPos = 0;
			}
		}
	TMMFFifoItem<const TDesC8>* firstItem = iFifo->Get();

	if (firstItem)
		{
		// Fill aBuffer with the next segment of the first buffer in the fifo
		TDes8& fillDes = STATIC_CAST(CMMFDataBuffer*, aBuffer)->Data();
		const TDesC8& readDes = firstItem->GetData();
		TInt readLen = Min(readDes.Length()-iFifoItemPos, aBuffer->RequestSize());
		fillDes = readDes.Mid(iFifoItemPos, readLen);
		iFifoItemPos+=readLen; // so we know where the next segment in the fifo item starts
		// Notify iDevSound the buffer is ready to be played
		iDevSound->PlayData();
		}
	else 
		{
		if(iBuffer ==NULL)
			{
			//keep a record of the supplied buffer and use it when/if more user data is in the FIFO
			iBuffer = aBuffer; 
			}
		if(!iKeepOpenAtEnd)
			{
			if (iDevSoundIgnoresUnderflow)
				{
				iCurrentSamplesPlayed = iDevSound->SamplesPlayed();
				StartShutDownTimer();
				}
			}
		else if(iState==EStopping)
			{
			CMMFDataBuffer* dataBuffer = static_cast<CMMFDataBuffer*>(aBuffer);
			dataBuffer->Data().SetLength(0);
			dataBuffer->SetLastBuffer(ETrue);
			iDevSound->PlayData();
			iBuffer=NULL;
			}
		}
	 
	}

void CMMFMdaAudioOutputStream::StartShutDownTimer()
	{
	iShutDownTimer->Start(KShutDownTimeInterval, KShutDownTimeInterval, TCallBack(ShutDownTimerComplete,this));
	}
	
TInt CMMFMdaAudioOutputStream::ShutDownTimerComplete(TAny* aAudioOutputStream)
	{
	CMMFMdaAudioOutputStream* audioOutputStream = static_cast<CMMFMdaAudioOutputStream*>(aAudioOutputStream);
	audioOutputStream->DoShutDownTimerComplete();
	return KErrNone;
	}

void CMMFMdaAudioOutputStream::DoShutDownTimerComplete()
	{
	iShutDownTimer->Cancel();
	TInt samplesPlayed = iDevSound->SamplesPlayed();
	if (samplesPlayed == iCurrentSamplesPlayed)
		{
		iDevSound->Stop();
		iState = EStopped;
		iBuffer = NULL;
		iCallback.MaoscPlayComplete(KErrUnderflow);
		}
	else
		{//desvound has not yet finished playing all the data. So wait for one more cycle
		//Restart Timer
		iCurrentSamplesPlayed = samplesPlayed;
		StartShutDownTimer();
		}
	}

/**
 *
 *  Called when play operation complete, successfully or otherwise	
 *
 *	@param	"TInt aError"	
 *			an error value which will indicate playing successfully complete
 *			if error value is 0
 *
 */
void CMMFMdaAudioOutputStream::PlayError(TInt aError)
	{
	TInt err=aError;
	 // if iDevSoundIgnoresUnderflow is true, then KErrUnderflow should not be produced by DevSound when we are not stopping 
	__ASSERT_DEBUG(!iDevSoundIgnoresUnderflow || !(err==KErrUnderflow && iState!=EStopping), Panic(EAOSStoppingError));
	iState = EStopped;
	iBuffer = NULL;
	iShutDownTimer->Cancel();
	if (err == KErrNone || err == KErrUnderflow)
		{
		if (!iFifo->IsEmpty())
			{
			// We live again - the Fifo still has some data. The sound device
			// will have to be opened again though so there might be an
			// audible click.
			TRAP(err, StartPlayL());
			if (err == KErrNone)
				{
				return;
				}
			}
		}
	
	EmptyFifo(err);
	
	// Note - KErrUnderflow will be reported even when all the buffers have
	// successfully played
	iCallback.MaoscPlayComplete(err);
	}

void CMMFMdaAudioOutputStream::EmptyFifo(TInt aError)
	{
	// Delete all buffers in the fifo and notify the observer
	TMMFFifoItem<const TDesC8>* firstItem;
	while((firstItem = iFifo->Get()) != NULL)
		{
		iFifo->RemoveFirstItem();
		iCallback.MaoscBufferCopied(aError, firstItem->GetData());
		delete firstItem;
		}
	}

void CMMFMdaAudioOutputStream::SendEventToClient(const TMMFEvent& aEvent)
	{
	if (aEvent.iEventType == KMMFEventCategoryAudioResourceAvailable)
		{
		// Retrieve the number of samples played
		// For the event type KMMFEventCategoryAudioResourceAvailable GetResourceNotificationData() returns
		// a package buffer as TMMFTimeIntervalMicroSecondsPckg, but the contents should be 
		// converted to an integer and interpreted as the data returned is samples played,
		// but not as a microsecond value.
		TBuf8<TMMFAudioConfig::KNotificationDataBufferSize> notificationData;
		if (KErrNone != iDevSound->GetResourceNotificationData(aEvent.iEventType, notificationData))
			{
			notificationData.SetLength(0);
			}
		iAudioResourceNotificationCallBack->MarncResourceAvailable(aEvent.iEventType, notificationData);
		}
	}

CMMFMdaAudioOutputStream::CActiveCallback::~CActiveCallback()
	{
	Cancel();
	}

CMMFMdaAudioOutputStream::CActiveCallback::CActiveCallback(MMdaAudioOutputStreamCallback& aCallback)
	: CActive(EPriorityStandard), iCallback(aCallback)
	{
	CActiveScheduler::Add(this);
	}

void CMMFMdaAudioOutputStream::CActiveCallback::RunL()
	{
	iCallback.MaoscOpenComplete(iStatus.Int());
	}

void CMMFMdaAudioOutputStream::CActiveCallback::DoCancel()
	{
	}

void CMMFMdaAudioOutputStream::CActiveCallback::Signal(const TInt aReason)
	{
	ASSERT(!IsActive());

	// Signal ourselves to run with the given completion code
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, aReason);
	SetActive();
	}


/**
 *
 *  Do not support
 *
 */
void CMMFMdaAudioOutputStream::BufferToBeEmptied(CMMFBuffer* /*aBuffer*/)
	{
	Panic(EBufferToBeEmptiedNotSupported);
	}

/**
 *
 *  Do not support
 *
 */
void CMMFMdaAudioOutputStream::RecordError(TInt /*aError*/)
	{
	Panic(ERecordErrorNotSupported);
	}

/**
 *
 *  Do not support
 *
 */
void CMMFMdaAudioOutputStream::ConvertError(TInt /*aError*/)
	{
	Panic(EConvertErrorNotSupported);
	}

/**
 *
 *  Do not support
 *
 */
void CMMFMdaAudioOutputStream::DeviceMessage(TUid /*aMessageType*/, const TDesC8& /*aMsg*/)
	{
	Panic(EDeviceMessageNotSupported);
	}

// CustomInferface - just pass on to DevSound. 
TAny* CMMFMdaAudioOutputStream::CustomInterface(TUid aInterfaceId)
	{
	return iDevSound->CustomInterface(aInterfaceId);
	}
	

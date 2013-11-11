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

#include "mmfclientaudioinputstream.h"
#include "mmfclientaudiostreamutils.h"
#include <mmf/server/devsoundstandardcustominterfaces.h>
#include <mmf/common/mmfpaniccodes.h>
#include "MmfFifo.h"
#include <mdaaudioinputstream.h>

const TInt KMicroSecsInOneSec = 1000000;


enum TMmfAudioInputPanic
	{
	EAudioInputPanicNotSupported
	};

_LIT(KAudioInputStreamCategory, "CMMFMdaAudioInputStream");
LOCAL_C void Panic(const TMmfAudioInputPanic aPanic)
	{
	User::Panic(KAudioInputStreamCategory, aPanic);
	}

/**
 *
 * Static NewL
 *
 * @return CMdaAudioInputStream*
 *
 */
EXPORT_C CMdaAudioInputStream* CMdaAudioInputStream::NewL(MMdaAudioInputStreamCallback& aCallback)
	{
	return NewL(aCallback, EMdaPriorityNormal, EMdaPriorityPreferenceTimeAndQuality);
	}

/**
 *
 * Static NewL
 *
 * @return CMdaAudioInputStream*
 *
 */
EXPORT_C CMdaAudioInputStream* CMdaAudioInputStream::NewL(MMdaAudioInputStreamCallback& aCallback, TInt aPriority, TInt aPref)
	{
	CMdaAudioInputStream* self = new(ELeave) CMdaAudioInputStream();
	CleanupStack::PushL(self);
	self->iProperties = CMMFMdaAudioInputStream::NewL(aCallback, aPriority, aPref);
	CleanupStack::Pop(self);
	return self;
	}

CMdaAudioInputStream::CMdaAudioInputStream()
	{
	}

CMdaAudioInputStream::~CMdaAudioInputStream()
	{
	if(iProperties)
		{
		iProperties->ShutDown();	
		}
	
	delete iProperties;
	}

EXPORT_C void CMdaAudioInputStream::SetAudioPropertiesL(TInt aSampleRate, TInt aChannels)
	{
	ASSERT(iProperties);
	iProperties->SetAudioPropertiesL(aSampleRate, aChannels);
	}

EXPORT_C void CMdaAudioInputStream::Open(TMdaPackage* aSettings)
	{
	ASSERT(iProperties);
	iProperties->Open(aSettings);
	}

EXPORT_C void CMdaAudioInputStream::SetGain(TInt aNewGain)
	{
	ASSERT(iProperties);
	iProperties->SetGain(aNewGain);
	}

EXPORT_C TInt CMdaAudioInputStream::Gain()  const
	{
	ASSERT(iProperties);
	return iProperties->Gain();
	}

EXPORT_C TInt CMdaAudioInputStream::MaxGain()  const
	{
	ASSERT(iProperties);
	return iProperties->MaxGain();
	}

EXPORT_C void CMdaAudioInputStream::SetBalanceL(TInt aBalance)
	{
	ASSERT(iProperties);
	iProperties->SetBalanceL(aBalance);
	}

EXPORT_C TInt CMdaAudioInputStream::GetBalanceL() const
	{
	ASSERT(iProperties);
	return iProperties->GetBalanceL();
	}

EXPORT_C void CMdaAudioInputStream::SetPriority(TInt aPriority, TInt aPref)
	{
	ASSERT(iProperties);
	iProperties->SetPriority(aPriority, aPref);
	}

EXPORT_C void CMdaAudioInputStream::ReadL(TDes8& aData)
	{
	ASSERT(iProperties);
	iProperties->ReadL(aData);
	}

EXPORT_C void CMdaAudioInputStream::Stop()
	{
	ASSERT(iProperties);
	iProperties->Stop();
	}

EXPORT_C void CMdaAudioInputStream::RequestStop()
	{
	ASSERT(iProperties);
	iProperties->RequestStop();
	}

EXPORT_C const TTimeIntervalMicroSeconds& CMdaAudioInputStream::Position()
	{
	ASSERT(iProperties);
	return iProperties->Position();
	}

EXPORT_C TInt CMdaAudioInputStream::GetBytes()
	{
	ASSERT(iProperties);
	return iProperties->GetBytes();
	}

EXPORT_C void CMdaAudioInputStream::SetSingleBufferMode(TBool aSingleMode)
	{
	ASSERT(iProperties);
	iProperties->SetSingleBufferMode(aSingleMode);
	}

/**

*/
EXPORT_C void CMdaAudioInputStream::SetDataTypeL(TFourCC aAudioType)
	{
	ASSERT(iProperties);
	iProperties->SetDataTypeL(aAudioType);
	}	

/**

*/
EXPORT_C TFourCC CMdaAudioInputStream::DataType() const
	{
	ASSERT(iProperties);
	return iProperties->DataType();
	}
	
	
EXPORT_C TInt CMdaAudioInputStream::BitRateL() const
	{
	ASSERT(iProperties);
	return iProperties->BitRateL();
	}
	
EXPORT_C void CMdaAudioInputStream::SetBitRateL(TInt aBitRate)
	{
	ASSERT(iProperties);
	iProperties->SetBitRateL(aBitRate);
	}

	
EXPORT_C void CMdaAudioInputStream::GetSupportedBitRatesL(RArray<TInt>& aSupportedBitRates)
	{
	ASSERT(iProperties);
	iProperties->GetSupportedBitRatesL(aSupportedBitRates);
	}

EXPORT_C TAny* CMdaAudioInputStream::CustomInterface(TUid aInterfaceId)
	{
	ASSERT(iProperties);
	return iProperties->CustomInterface(aInterfaceId);
	}

//

CMMFMdaAudioInputStream* CMMFMdaAudioInputStream::NewL(MMdaAudioInputStreamCallback& aCallback)
	{
	return CMMFMdaAudioInputStream::NewL(aCallback, EMdaPriorityNormal, EMdaPriorityPreferenceTimeAndQuality);
	}

CMMFMdaAudioInputStream* CMMFMdaAudioInputStream::NewL(MMdaAudioInputStreamCallback& aCallback, TInt aPriority, TInt aPref)
	{
	CMMFMdaAudioInputStream* self = new(ELeave) CMMFMdaAudioInputStream(aCallback);
	CleanupStack::PushL(self);
	self->ConstructL(aPriority, aPref);
	CleanupStack::Pop(self);
	return self;
	}
/**
 *
 * Construct
 *
 * @param	"MMdaAudioInputStreamCallback&"
 *			a reference to MMdaAudioInputStreamCallback
 *			a perference value
 *
 */
CMMFMdaAudioInputStream::CMMFMdaAudioInputStream(MMdaAudioInputStreamCallback& aCallback)
	: iCallback(aCallback), iStorageItem (NULL, 0), iBufferPtr(NULL, 0)
	// Depending on zero for construction (i.e. attribute of CBase)
	//   iSingleBuffer (EFalse)
	//   iState(EStopped)
	//   iIsOpened(EFalse)
	//   iCallbackMade(EFalse)
	//   iAudioDataStored(EFalse)
	{
	iDataTypeCode.Set(TFourCC(' ','P','1','6'));
	}

/**
 *
 *	Second phase constructor
 *
 */
void CMMFMdaAudioInputStream::ConstructL(TInt aPriority, TInt aPref)
	{
	iDevSound = CMMFDevSound::NewL();
	SetPriority(aPriority, aPref);
	iFifo = new(ELeave) CMMFFifo<TDes8>();
	iActiveCallback = new(ELeave) CActiveCallback(iCallback);
	iActiveSchedulerWait = new(ELeave) CActiveSchedulerWait;
	}

/**
 *
 *	Destructor
 *
 */
CMMFMdaAudioInputStream::~CMMFMdaAudioInputStream()
	{
	delete iFifo;
	delete iDevSound;
	delete iActiveCallback;
	delete iActiveSchedulerWait;
	}

/**
 *
 *  Set audio input stream properties	
 *
 *	@param	"TInt aSampleRate"	
 *			a specified priority value
 *	@param	"TInt aChannels"		
 *			a specified preference value
 *
 */
void CMMFMdaAudioInputStream::SetAudioPropertiesL(TInt aSampleRate, TInt aChannels)
	{
	if (iIsOpenState == EIsOpen)
		{
		RealSetAudioPropertiesL(aSampleRate, aChannels);
		}
	else
		{
		// cache for application later
		iSettings.iSampleRate = aSampleRate;
		iSettings.iChannels = aChannels;
		iAudioDataStored = ETrue;
		}
	}
	
void CMMFMdaAudioInputStream::RealSetAudioPropertiesL(TInt aSampleRate, TInt aChannels)
	{
	TMMFCapabilities capabilities = iDevSound->Config();
	capabilities.iChannels = StreamUtils::MapChannelsMdaToMMFL(aChannels);
	capabilities.iRate = StreamUtils::MapSampleRateMdaToMMFL(aSampleRate);
	iDevSound->SetConfigL(capabilities);
	}

/**
 *
 *  Open a audio ouptut stream	
 *
 *	@param	"TMdaPackage* Settings"	
 *			a pointer point to TMdaPackage
 *
 */
void CMMFMdaAudioInputStream::Open(TMdaPackage* aSettings)
	{
	iIsOpenState = EIsOpening;
	//store aSettings
	if (aSettings && (aSettings->Type().iUid == KUidMdaDataTypeSettingsDefine))
		{
		TMdaAudioDataSettings* tmpSettings = STATIC_CAST(TMdaAudioDataSettings*, aSettings);
		iSettings = *tmpSettings;
		iAudioDataStored = ETrue;
		}
	TRAPD(err, iDevSound->InitializeL(*this, iDataTypeCode, EMMFStateRecording));
	if (err != KErrNone)
		{
		// Signal for the MaiscOpenComplete callback to be called asynchronously
		iActiveCallback->Signal(err);
		iIsOpenState = EIsNotOpen;
		iAudioDataStored = EFalse; // reset - if was set we throw away due to failure
		}
	}

/**
 *
 *  To get the maximum gain level	
 *
 *	@return	"TInt"	
 *			the maximum gain value in integer
 *
 */
TInt CMMFMdaAudioInputStream::MaxGain() const
	{
	return iDevSound->MaxGain();
	}

/**
 *
 *  To get the current gain level	
 *
 *	@return	"TInt"	
 *			the current gain value in integer
 *
 */
TInt CMMFMdaAudioInputStream::Gain() const
	{
	return iDevSound->Gain();
	} 

/**
 *
 *  Set audio input stream gain to the specified value
 *
 *	@param	"TInt aGain"	
 *			a specified gain value
 *
 */
void CMMFMdaAudioInputStream::SetGain(TInt aGain)
	{
	iDevSound->SetGain(aGain);
	}

/**
 *
 *  Set audio input stream balance	
 *
 *	@param	"TInt aBalance"	
 *			a specified balance value
 *
 */
void CMMFMdaAudioInputStream::SetBalanceL(TInt aBalance)
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
	iDevSound->SetRecordBalanceL(left, right);
	}

/**
 *
 *  To get the current balance value.This function may not return the same value 
 *	as passed to SetBalanceL depending on the internal implementation in 
 *	the underlying components.
 *
 *	@return	"TInt"	
 *			the current balance value in integer
 *
 */
TInt CMMFMdaAudioInputStream::GetBalanceL() const
	{
	TInt rightBalance = 0;
	TInt leftBalance  = 0;
	iDevSound->GetRecordBalanceL(leftBalance, rightBalance);
	TInt balance  = 0;
	StreamUtils::CalculateBalance( balance, leftBalance, rightBalance );
	return balance;
	}

/**
 *
 *  Set audio input stream priority	
 *
 *	@param	"TInt aPriority"	
 *			a specified priority value
 *	@param	"TInt aPref"		
 *			a specified preference value
 *
 */
void CMMFMdaAudioInputStream::SetPriority(TInt aPriority, TInt aPref)
	{
	TMMFPrioritySettings settings;
	settings.iPriority = aPriority;
	settings.iPref = aPref;
	iDevSound->SetPrioritySettings(settings);
	}


/**
 *
 *  To read data from input stream 	
 *
 *	@param	"TDesC8& aData"	
 *			a stream data 
 * 
 *  @capability	UserEnvironment
 *			For recording - the requesting client process must have the 
 *			UserEnvironment capability.
 */
void CMMFMdaAudioInputStream::ReadL(TDes8& aData)
	{
	User::LeaveIfError(Read(aData));

	if (iState == EStopped)
		{
		iDevSound->RecordInitL();
		iState = ERecording;
		}
	}

/**
 *
 *  To read data from input stream 	
 *
 *	@param	"TDesC8& aData"	
 *			a stream data 
 * 
 *  @capability	UserEnvironment
 *			For recording - the requesting client process must have the 
 *			UserEnvironment capability.
 */
TInt CMMFMdaAudioInputStream::Read(TDes8& aData)
	{
	TMMFFifoItem<TDes8>* item = new TMMFFifoItem<TDes8>(aData);
	if (!item)
		{
		return KErrNoMemory;
		}

	iFifo->AddToFifo(*item);	// no issue with memory alloc
								// each element contains storage space for link to the next
	return KErrNone;
	}

/**
 *
 *  To stop write data to stream 	
 *
 */
void CMMFMdaAudioInputStream::Stop()
	{
	// Need to take for the case where Stop is invoked directly after a call to RequestStop.
	// We have chosen to allow the Stop to go through as this could be more important.
	// This is non-reentrant code but will suffice for our needs.
	if (iState != EStopped)
		{
		// Amend the state so RequestStop or Stop initiated by it (indirectly) won't function
		iState = EStopped;

		if (iSingleBuffer && iStorageItem.Ptr() != NULL)
			{
			iCallback.MaiscBufferCopied(KErrAbort, iStorageItem);
			iStorageItem.Set (NULL,0,0);
			}
	 
		// Delete all buffers in the fifo and notify the observer
		TMMFFifoItem<TDes8>* firstItem;
		while((firstItem = iFifo->Get()) != NULL)
			{
			iFifo->RemoveFirstItem();
			if (!iSingleBuffer)
				{
				iCallback.MaiscBufferCopied(KErrAbort,firstItem->GetData());		 		 		 		 
				}
			delete firstItem;
			}
	 
		iDevSound->Stop();
		}
	}


/**
 *
 *  To pause write data to stream 
 *  Allow yet unprocessed buffers to be processed and passed back via BufferToBeEmptied.
 *  When the last (empty) buffer arrives, Stop is called.
 */
void CMMFMdaAudioInputStream::RequestStop()
	{
	// [ precondition that we are not already stopped 
	// && if we are stopped do nothing.
	// If we are stopping a recording, we need to give the server a chance to 
	// process that data which has already been captured. We therefore stay in the EPause
	// state.
	if (iState != EStopped)
		{
		if (iState != EStopping)
			{
			// We can only be Recording, if we have other states later they can be tested here.
			iDevSound->Pause();
			iState = EStopping;
			}
		}
	}


/**
 *
 *  To get the current position in the data stream	
 *
 *	@return	"TTimeIntervalMicroSeconds&"	
 *			the current position in integer
 *
 */
const TTimeIntervalMicroSeconds& CMMFMdaAudioInputStream::Position()
	{
	TInt64 position = iDevSound->SamplesRecorded();
	position = position * KMicroSecsInOneSec / StreamUtils::SampleRateAsValue(iDevSound->Config());
	iPosition = (iState == ERecording) ? position : 0; // Shouldn't need to check for playing but CMMFDevSound doesn't reset bytes played after a stop
	return iPosition;
	}



/**
 *
 *  To return the current number of bytes recorded by audio hardware
 *	@return "the current current number of bytes rendered by audio hardware in integer"	
 *
 */
TInt CMMFMdaAudioInputStream::GetBytes()
	{
	return iBytesRecorded;
	}

void CMMFMdaAudioInputStream::SetDataTypeL(TFourCC aAudioType)
	{
	if(iState != EStopped)
		User::Leave(KErrServerBusy);

	if(aAudioType == iDataTypeCode)
		return;

	TMMFPrioritySettings prioritySettings ; 	
	prioritySettings.iState = EMMFStateRecording;
	
	RArray<TFourCC> supportedDataTypes;
	
	CleanupClosePushL(supportedDataTypes);

	TRAPD(err, iDevSound->GetSupportedOutputDataTypesL(supportedDataTypes, prioritySettings));

	if (err == KErrNone)
		{
		if (supportedDataTypes.Find(aAudioType) == KErrNotFound)
			{
			User::Leave(KErrNotSupported);	
			}
		//if match, set the 4CC of AudioType to match
		iDataTypeCode.Set(aAudioType);
		}
	else //we had a real leave error from GetSupportedInputDataTypesL
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
		TRAPD(err, iDevSound->InitializeL(*this, iDataTypeCode, EMMFStateRecording));
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
TFourCC CMMFMdaAudioInputStream::DataType() const
	{
	return iDataTypeCode;
	}
	
TInt CMMFMdaAudioInputStream::BitRateL() const
	{
	TAny* ptr = iDevSound->CustomInterface(KUidCustomInterfaceDevSoundBitRate);
	if (ptr == NULL)
		User::Leave(KErrNotSupported);
	
	MMMFDevSoundCustomInterfaceBitRate* bitrate = static_cast<MMMFDevSoundCustomInterfaceBitRate*>(ptr);
	return bitrate->BitRateL();
	}
	
	
void CMMFMdaAudioInputStream::SetBitRateL(TInt aBitRate)
	{
	TAny* ptr = iDevSound->CustomInterface(KUidCustomInterfaceDevSoundBitRate);
	if (ptr == NULL)
		User::Leave(KErrNotSupported);
	
	MMMFDevSoundCustomInterfaceBitRate* bitrate = static_cast<MMMFDevSoundCustomInterfaceBitRate*>(ptr);
	bitrate->SetBitRateL(aBitRate);
	}
	
void CMMFMdaAudioInputStream::GetSupportedBitRatesL(RArray<TInt>& aSupportedBitRates)
	{
	// ensure that the array is empty before passing it in
	aSupportedBitRates.Reset();
	TAny* ptr = iDevSound->CustomInterface(KUidCustomInterfaceDevSoundBitRate);
	if (ptr == NULL)
		User::Leave(KErrNotSupported);
	MMMFDevSoundCustomInterfaceBitRate* bitrate = static_cast<MMMFDevSoundCustomInterfaceBitRate*>(ptr);
	bitrate->GetSupportedBitRatesL(aSupportedBitRates);
	}


//

CMMFMdaAudioInputStream::CActiveCallback::CActiveCallback(MMdaAudioInputStreamCallback& aCallback)
	: CActive(EPriorityStandard), iCallback(aCallback)
	{
	CActiveScheduler::Add(this);
	}

CMMFMdaAudioInputStream::CActiveCallback::~CActiveCallback()
	{
	Cancel();
	}

void CMMFMdaAudioInputStream::CActiveCallback::RunL()
	{
	iCallback.MaiscOpenComplete(iStatus.Int());
	}

void CMMFMdaAudioInputStream::CActiveCallback::DoCancel()
	{
	}

void CMMFMdaAudioInputStream::CActiveCallback::Signal(const TInt aReason)
	{
	ASSERT(!IsActive());

	// Signal ourselves to run with the given completion code
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, aReason);
	SetActive();
	}

//

/**
 *
 *  To be called when intialize stream complete	
 *
 *	@param	"TInt aError"	
 *			error code, initialize stream succeed when aError = 0
 *
 */
void CMMFMdaAudioInputStream::InitializeComplete(TInt aError)
	{
	TInt err = aError;
	if(iIsOpenState == EIsOpening)
		{
		if (err == KErrNone)
			{
			// Use settings to set audio properties after the dev sound has been
			// successfully initialised
			if(iAudioDataStored)
				{
				TRAP(err, RealSetAudioPropertiesL(iSettings.iSampleRate, iSettings.iChannels));
				}

			}
			
		// Signal for the MaiscOpenComplete callback to be called asynchronously.Ignore if InitializeL is called from SetDataTypeL
		if(!iInitCallFrmSetDataType)
			{
			iActiveCallback->Signal(err);
			}
		iIsOpenState = err ? EIsNotOpen : EIsOpen;
		//reset iAudioDataStored flag if set - whatever don't want to use next time
		iAudioDataStored = EFalse;
		if(iInitializeState == KRequestPending)
			{
			iInitializeState = err;
			iActiveSchedulerWait->AsyncStop();
			}
		else
			{
			iInitializeState = err;//Set the error.		
			}
		}
	}

/**
 *
 *  Do not support
 *
 */
void CMMFMdaAudioInputStream::ToneFinished(TInt /*aError*/)
	{
	Panic(EAudioInputPanicNotSupported);
	}

/**
 *
 *  Called when sound device has filled data buffer
 *
 *	@param	"CMMFBuffer* aBuffer"	
 *			a pointer point to CMMFBuffer, which is used for recieved data
 *
 */
void CMMFMdaAudioInputStream::BufferToBeEmptied(CMMFBuffer* aBuffer)
	{
	// Simply put, tries to copy the data from aBuffer into the clients storage buffers. 
	//
	// The input stream iFifo data member is used to store the clients storage buffers
	// that are passed to it via a call to ReadL.
	//
	// If iSingleBuffer is False, the first buffer on the fifo is copied to. 
	// This buffer is then removed off the fifo.
	// The callback MaiscBufferCopied is invoked after each copy, passing that buffer.
	// If the data is greater than the buffer then this process repeats with the next buffer.
	//
	// If iSingleBuffer is True, it is assumed only one buffer is on the fifo.
	// The behaviour is the same as above except that a descriptor representing the 
	// buffers empty part is placed at the end of the fifo, and the callback 
	// MaiscBufferCopied is invoked only when the buffer is full.
	//
	// If the client sets iSingleBuffer to True and places more than one buffer on the fifo
	// the behaviour is undefined and unsupported.
	//
	// If there are no more storage buffers on the fifo, the callback 
	// MaiscRecordComplete(KErrOverflow) is invoked.

	const TDesC8& buffer = STATIC_CAST(CMMFDataBuffer*, aBuffer)->Data();
	
	TInt lengthCopied = 0;
	iBytesRecorded += buffer.Length();

	// A stop was requested after all the data has been received
	if (iState == EStopping && buffer.Length() == 0)
		{
	 	Stop();
		iCallback.MaiscRecordComplete(KErrNone);
		return;
		}
	else
		{
		// The fifo may have multiple storage buffers, i.e. one in each of its entries.
		// Fill what we can in each. If we get an empty buffer then we have finished recording.
		while (lengthCopied < buffer.Length())
			{
			// Chop up aBuffer into slices the buffers in iFifo can handle
			TMMFFifoItem<TDes8>* firstItem = iFifo->Get();
			
			if(firstItem != NULL)
				{
				TDes8& writeBuf = firstItem->GetData();
						
				// We have a spare buffer slot
				TInt slotLength = Min(buffer.Length()-lengthCopied, writeBuf.MaxLength());
				writeBuf = buffer.Mid(lengthCopied, slotLength);
				lengthCopied += slotLength;
				
				// Determine whether to callback the client or not.
				// I.e. if we have multiple small buffers that we want to process quickly or
				// when a singular buffer is FULL.
				// Note: That if the client asks to Stop, the buffer may not be filled!
				if (iSingleBuffer)
					{
					// Remember this item for later. 
					// We really only want the first item as this covers the entire
					// client storage buffer. We will adjust the actual length later.
					if (iStorageItem.Ptr() == NULL)
						{
						iStorageItem.Set (const_cast<TUint8*>(writeBuf.Ptr()), 0, writeBuf.MaxLength());
						}
					
					// In this iteration we may just be looking at a right-part of the original
					// buffer. Update the actual length of data.
					TInt actualLength = (writeBuf.Ptr()-iStorageItem.Ptr()) + writeBuf.Length();
					iStorageItem.SetLength(actualLength);

					// Is the buffer full?
					if (writeBuf.Length() == writeBuf.MaxLength())
						{
						// The singular buffer has been filled so pass it back to the client
						iCallback.MaiscBufferCopied(KErrNone, iStorageItem);
						iStorageItem.Set (NULL,0,0);
						}
					else
						{
						// Create a window to the 'remaining' free section of the storage buffer
						iBufferPtr.Set (const_cast<TUint8*>(writeBuf.Ptr())+lengthCopied, 0, writeBuf.MaxLength()-lengthCopied);
						
						// Add the window to the fifo
						TInt err = Read(iBufferPtr);
						if (err)
							{
							Stop();
							iCallback.MaiscRecordComplete(err);
							return;
							}
						ASSERT(iState == ERecording);
						}
					}
				else
					{
					// Notify client
					iCallback.MaiscBufferCopied(KErrNone, writeBuf);
					}
				//Check if client called Stop from the MaiscBufferCopied. 
				//If so, we should neither continue this loop nor delete the first item. Stop cleans up all the buffers
				if(iState == EStopped)
					{
					break;
					}
				else
					{
					// Remove this storage buffer from the fifo as we want to have access to any others behind it.
					iFifo->RemoveFirstItem();
					delete firstItem;						
					}
				}
			else
				{
				// run out of buffers - report an overflow error
				Stop();
				iCallback.MaiscRecordComplete(KErrOverflow);
				return;
				}
			}// while
		} // else
		
	// Keep recording if there are free buffers
	if (!iFifo->IsEmpty())
		iDevSound->RecordData();
	}

/**
 *
 *  Called when record operation complete, successfully or otherwise	
 *
 *	@param	"TInt aError"	
 *			an error value which will indicate playing successfully complete
 *			if error value is 0
 *
 */
void CMMFMdaAudioInputStream::PlayError(TInt /*aError*/)
	{
	Panic(EAudioInputPanicNotSupported);
	}


/**
 *
 *  Do not support
 *
 */
void CMMFMdaAudioInputStream::BufferToBeFilled(CMMFBuffer* /*aBuffer*/)
	{
	Panic(EAudioInputPanicNotSupported);
	}

/**
 *
 *  Do not support
 *
 */
void CMMFMdaAudioInputStream::RecordError(TInt aError)
	{
	if (iState == ERecording)
		{
		if (aError != KErrCancel)
			{
			iCallback.MaiscRecordComplete(aError);
			}
		// else must have been cancelled by client. Doesn't need to be notified

		iState = EStopped;
		}
	}

/**
 *
 *  Do not support
 *
 */
void CMMFMdaAudioInputStream::ConvertError(TInt /*aError*/)
	{
	Panic(EAudioInputPanicNotSupported);
	}

/**
 *
 *  Do not support
 *
 */
void CMMFMdaAudioInputStream::DeviceMessage(TUid /*aMessageType*/, const TDesC8& /*aMsg*/)
	{
	Panic(EAudioInputPanicNotSupported);
	}

// CustomInferface - just pass on to DevSound. 
TAny* CMMFMdaAudioInputStream::CustomInterface(TUid aInterfaceId)
	{
	return iDevSound->CustomInterface(aInterfaceId);
	}
	

void CMMFMdaAudioInputStream::SetSingleBufferMode(TBool aSingleMode)
	{
	iSingleBuffer = aSingleMode;
	}

void CMMFMdaAudioInputStream::ShutDown()
	{
	// Need to take for the case where Stop is invoked from the destructor of CMdaAudioInputStream 
	// Need to ensure that there are no callbacks to the client at this stage
	if (iState != EStopped)
		{
		iState = EStopped;

		if (iSingleBuffer && iStorageItem.Ptr() != NULL)
			{
			iStorageItem.Set (NULL,0,0);
			}
	 
		// Delete all buffers in the fifo
		TMMFFifoItem<TDes8>* firstItem;
		while((firstItem = iFifo->Get()) != NULL)
			{
			iFifo->RemoveFirstItem();
			delete firstItem;
			}
	 
		iDevSound->Stop();
		}
	}

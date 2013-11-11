// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "mmfbtswcodecwrapper.h"
#include "mmfBtSwCodecPlayDataPath.h"
#include "mmfBtSwCodecRecordDataPath.h"
#include "mmfBtSwCodecConvertDataPath.h"
#include <mmfswcodecwrappercustominterfacesuids.hrh>
#include "mmfBtswcodecwrapperCustomInterfaces.h"
#include "MMFBtRoutingSoundDevice.h"
#include <mmfpaniccodes.h>


/*
 *	AO to handle RSD initialisation
 *
 */
CRoutingSoundDeviceOpenHandler* CRoutingSoundDeviceOpenHandler::NewL(CMMFSwCodecWrapper* aObserver)
	{
	CRoutingSoundDeviceOpenHandler* self = new(ELeave) CRoutingSoundDeviceOpenHandler(aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
	
CRoutingSoundDeviceOpenHandler::~CRoutingSoundDeviceOpenHandler()
	{
	Cancel();
	}
		
void CRoutingSoundDeviceOpenHandler::RunL()
	{
	TInt err = iStatus.Int();
	if (iObserver)
		{
		iObserver->OpenComplete(err);
		}
	}
	
void CRoutingSoundDeviceOpenHandler::DoCancel()
	{
	if (iObserver)
		{
		iObserver->OpenComplete(KErrCancel);
		}
	}
	
CRoutingSoundDeviceOpenHandler::CRoutingSoundDeviceOpenHandler(CMMFSwCodecWrapper* aObserver) :
															 	CActive(EPriorityStandard),
															 	iObserver(aObserver)								 	
														
	{
	CActiveScheduler::Add(this);	
	}
	
void CRoutingSoundDeviceOpenHandler::ConstructL()
	{
	}

void CRoutingSoundDeviceOpenHandler::Start()
	{
	if (!IsActive())
		{
		SetActive();
		}
	}
 

/**
 * Internal panic
 * @internalComponent
 */
void Panic(TInt aPanicCode)
	{
	_LIT(KMMFSwCodecWrapperPanicCategory, "MMFSwCodecWrapper");
	User::Panic(KMMFSwCodecWrapperPanicCategory, aPanicCode);
	}


/**
 * This method is not be exported as it is only 
 * intended to be called within this DLL.
 * It's purpose is to assign an CRoutingSoundPlayDevice to the play
 * custom interface
 * @internalComponent
 */
void TPlayCustomInterface::SetDevice(CRoutingSoundPlayDevice* aDevice)
	{
	iDevice = aDevice;
	}

void TPlayCustomInterface::SetVolume(TUint aVolume)
	{
	iVolume = aVolume;
	if (iDevice && iDevice->Handle())
		{
		iDevice->SetVolume(iVolume);
		}
	}

/**
 * Procedure to get the number of bytes played by the device driver
 * If there is no handle available to the device driver then the 
 * procedure returns the last known value
 * @prototype
 * @return number of bytes played
 */
TUint TPlayCustomInterface::BytesPlayed()
	{
	if(iDevice)
		{
		if (iDevice->Handle())
			{
			iBytesPlayed = iDevice->BytesPlayed();
			}
		}
	return iBytesPlayed;
	}
	
TTaskConfig TPlayCustomInterface::Caps()
	{
	TTaskConfig taskConfig;
	taskConfig.iRate = 0;
	taskConfig.iStereoMode = 0;
	taskConfig.iUid = KUidRefDevSoundTaskConfig;
	
	if (iDevice && iDevice->Handle())
		{
		RArray<TUint> supportedSampleRates;
		RArray<TRange>supportedRateRanges;
		iDevice->GetSupportedSampleRates(supportedSampleRates,supportedRateRanges);	
		for (TUint i=0; i<supportedSampleRates.Count(); i++)
			{
			switch(supportedSampleRates[i])
				{
				case 8000:
					taskConfig.iRate |= ETask8000Hz;
					break;
				case 11025:
					taskConfig.iRate |= ETask11025Hz;
  					break;
				case 12000:
					taskConfig.iRate |= ETask12000Hz;
					break;
				case 16000:
					taskConfig.iRate |= ETask16000Hz;
					break;
				case 22050:
  					taskConfig.iRate |= ETask22050Hz;
  					break;
				case 24000:
  					taskConfig.iRate |= ETask24000Hz;
  					break;
				case 32000:
  					taskConfig.iRate |= ETask32000Hz;
  					break;
				case 44100:
					taskConfig.iRate |= ETask44100Hz;
  					break;
				case 48000:
					taskConfig.iRate |= ETask48000Hz;
					break;
				case 88200:
					taskConfig.iRate |= ETask88200Hz;
					break;
				case 96000:
					taskConfig.iRate |= ETask96000Hz;
					break;
				default: //do nothing
					break;	
				}//	switch(supportedSampleRates[i])
			supportedSampleRates.Close();
			supportedRateRanges.Close();
			}//for (TUint i=0; i<supportedSampleRates.Count(); i++)
		RArray<TUint> supportedChannels;
		TMMFStereoSupport stereoSupport;
		iDevice->GetSupportedChannels(supportedChannels,stereoSupport);	
		for (TUint i=0; i<supportedChannels.Count(); i++)
			{//note we don't have a TaskConfig enum for Joint stereo
			switch(supportedChannels[i])
				{
				case EMMFMono:
					taskConfig.iStereoMode |= ETaskMono;
					break;
				case EMMFStereo:
					if (stereoSupport == EMMFInterleavedOnly)
						{
						taskConfig.iStereoMode |= ETaskInterleaved;
						}
					else if (stereoSupport == EMMFNonInterleavedOnly)
						{
						taskConfig.iStereoMode |= ETaskNonInterleaved;
						}
					else if (stereoSupport == EMMFBothNonAndInterleaved)
						{
						taskConfig.iStereoMode |= (ETaskInterleaved|ETaskNonInterleaved);
						}
					break;
				default://do nothing
					break;
				}//switch(supportedChannels[i])
			supportedChannels.Close();
			}//	for (TUint i=0; i<supportedChannels.Count(); i++)
		}//if (iDevice && iDevice->Handle())
	return taskConfig;
	}

/**
 * Procedure to get the number of bytes recorded by the device  
 * @prototype
 * @return number of bytes recorded
 */
TUint TRecordCustomInterface::BytesRecorded()
	{
	if(iDataPath)
		{
		iBytesRecorded = iDataPath->RecordedBytesCount();
		}
	return iBytesRecorded;
	}

/**
Constructor.
*/
EXPORT_C CMMFSwCodecWrapper::CMMFSwCodecWrapper()
	{
	}

/**
Destructor.

The destructor is called by ECom framework allowing derived classes
to clean up implementation specific resources. The sound
device drivers are freed.
*/
EXPORT_C CMMFSwCodecWrapper::~CMMFSwCodecWrapper()
	{
	// AO to handle OpenDevice call
	if (iOpenHandler && iOpenHandler->IsActive())
		{
		iOpenHandler->Cancel();	
		}
	delete iOpenHandler;
	delete iRecordDevice;
	delete iPlayDevice;
	delete iDataPath;
	delete iCodec;
	delete iPlayCustomInterface;
	delete iRecordCustomInterface;
	}
	
/**
Initializes the hardware device tasks - in the case of a
sw codec wrapper 'hardware device' this consists of loading the
sound device drivers and creating the CMMFSwCodec.

@param  aDevInfo
        Device initialization parameters.
        Only the iHwDeviceObserver is used for CMFSwCodecWrapper
        derived CMMFHwDevices.
@return An error code indicating if the function call was successful. KErrNone on success, otherwise
        another of the system-wide error codes.
*/
EXPORT_C TInt CMMFSwCodecWrapper::Init(THwDeviceInitParams& aDevInfo)
	{
	TRequestStatus status;
	Init(aDevInfo,status);
	User::WaitForRequest(status);
	return status.Int();
	}


/**
Initializes the hardware device tasks - in the case of a
sw codec wrapper 'hardware device' this consists of loading the
sound device drivers and creating the CMMFSwCodec.

@param  aDevInfo
        Device initialization parameters.
        Only the iHwDeviceObserver is used for CMFSwCodecWrapper
        derived CMMFHwDevices.
@param	aStatus
		Status flag belonging to an Active Object that will have its
		RunL() called when this function completes
*/
EXPORT_C void CMMFSwCodecWrapper::Init(	THwDeviceInitParams &aDevInfo,
										TRequestStatus& aStatus)
	{
	aStatus = KRequestPending;
	TRequestStatus* status = &aStatus;
	if (!aDevInfo.iHwDeviceObserver)
		{
		User::RequestComplete(status, KErrArgument);
		return;
		}
	iHwDeviceObserver = aDevInfo.iHwDeviceObserver;

	TInt err = KErrNone;
	if (aDevInfo.iOutStream.iConnection.iId)
		{
		iDeviceUid = TUid::Uid(aDevInfo.iOutStream.iConnection.iId);
		// Play device
		delete iPlayDevice;
		iPlayDevice  = NULL;
		TRAP(err, iPlayDevice = CRoutingSoundPlayDevice::NewL());
		if (err == KErrNone)
			{
			iPlayDevice->Initialize(iDeviceUid,
									aDevInfo.iOutStream.iDeviceName, *status);
			if (!iPlayCustomInterface)
				{
				TRAP(err,iPlayCustomInterface = new(ELeave)TPlayCustomInterface());
				if (err)
					{
					delete iPlayDevice;
					iPlayDevice = NULL;
					User::RequestComplete(status, err);
					return;
					}
				}
			static_cast<TPlayCustomInterface*>(iPlayCustomInterface)->SetDevice(iPlayDevice);	
			}
		}
	if (aDevInfo.iInStream.iConnection.iId)
		{
		iDeviceUid = TUid::Uid(aDevInfo.iInStream.iConnection.iId);
			// Record device
		TRAP(err, iRecordDevice = CRoutingSoundRecordDevice::NewL());
		if (err == KErrNone)
			{
			iRecordDevice->Initialize(iDeviceUid, KNullDesC8, *status);
			if (!iRecordCustomInterface)
				{
				TRAP(err,iRecordCustomInterface = new(ELeave)TRecordCustomInterface());
				if (err)
					{
					delete iRecordDevice;
					iRecordDevice = NULL;
					User::RequestComplete(status, err);
					return;
					}
				}
			}
		}
	
	iCodec = &(Codec()); //create codec
		
	//[ assert the post condition ]
	if (!iCodec)
		{
		err = KErrNotSupported;
		}

	if (err != KErrNone)
		{
		User::RequestComplete(status, err);
		// Cancel initialisation too
		if (aDevInfo.iOutStream.iConnection.iId)
			{
			iPlayDevice->CancelInitialize(iDeviceUid);
			}
		if (aDevInfo.iInStream.iConnection.iId)
			{
			iRecordDevice->CancelInitialize(iDeviceUid);
			}		
		}		
	else if ((!aDevInfo.iOutStream.iConnection.iId)&& (!aDevInfo.iInStream.iConnection.iId))
		{
		//could be used for conversion so complete the request status
		User::RequestComplete(status, KErrNone);
		}
		
	}

/**
Starts Encoding or Decoding task(s) based on the parameter specified.

@param  aFuncCmd
        The device function specifying the requested service i.e. decode or encode
        where EDevEncode = Record, EDevDecode = Play and EDevNullFunc = Convert.
@param  aFlowCmd
        The device flow directions for requested service.
        This parameter is ignored for CMMFSwCodecWrapper CMMFHwDevicePlugins
@return An error code indicating if the function call was successful. KErrNone on success, otherwise
        another of the system-wide error codes.
*/
EXPORT_C TInt CMMFSwCodecWrapper::Start(TDeviceFunc aFuncCmd, TDeviceFlow /*aFlowCmd*/)
	{
	TInt error = KErrNone;

	// [ precondition that aFuncCmd is valid]
	if (!((aFuncCmd == EDevEncode)|(aFuncCmd == EDevDecode)|(aFuncCmd == EDevNullFunc)))
		{
		return KErrArgument;
		}

	// [ precondition that iCodec is present]
	if (!iCodec)
		{
		return KErrNotReady; //make sure the codec has been added
		}

    switch (aFuncCmd)
        {
        case EDevEncode: // Audio record
			{
			error = StartEncode();
			}
            break;
        case EDevDecode: // Audio play
			{
			error = StartDecode();
			}
            break;
		case EDevNullFunc: //Audio Convert
			{
			error = StartConvert();
			}
			break;
        default:
            error = KErrNotSupported;
            break;
		}

	//[ assert the post conditions ]
#ifdef DEBUG
	if (!error)
		{//only assert if no error otherwise post consitions not valid
		__ASSERT_DEBUG(iDataPath, Panic(EMMFSwCodecWrapperNoDataPath));
		if ((aFuncCmd == EDevEncode)||(aFuncCmd == EDevDecode))
			{
			__ASSERT_DEBUG(iDataPath->Device().Handle(), Panic(EMMFSwCodecWrapperNoDevice));
			}
		}
#endif

	return error;
	}

// Use AO to OpenDevice
TInt CMMFSwCodecWrapper::StartDecode()
	{
	TInt error = KErrNone;

	//[ assert precondition that play custom interface is present]
	//if there is no play custom interface then the user of the CMMFSwCodecWrapper
	//cannot have set any of the custom settings such as sample rate.
	if (!iPlayCustomInterface)
		{
		return KErrNotReady;
		}

	//play
	if (!iDataPath)
		{//create a datapath
		TRAP(error,iDataPath = CMMFSwCodecPlayDataPath::NewL(iPlayDevice, iDeviceUid));
		if ((iDataPath) && (error == KErrNone))
			{
			iDataPath->SetObserver(*iHwDeviceObserver);
			error = iDataPath->AddCodec(*iCodec);
			iDeviceBufferSize = iCodec->SinkBufferSize();
			static_cast<CMMFSwCodecPlayDataPath*>(iDataPath)->SetPlayCustomInterface(*iPlayCustomInterface);
			static_cast<CMMFSwCodecPlayDataPath*>(iDataPath)->SetConfigForAudioRamp(iSampleRate, iChannels);
			}
		}

	if ((error == KErrNone) &&
		(iDataPath->State() != CMMFSwCodecDataPath::EPlaying))
		{
		TBool asynchOpen = EFalse;	// Ensure we only call OpenPlayComplete once
		//datapath was created ok and we are not playing
		if (iDataPath->State() == CMMFSwCodecDataPath::EStopped)
			{
			//starting from 'fresh so set sound device settings
  				
			// Create the AO OpenDevice handler
			if (!iOpenHandler)
				{
				TRAP(error, iOpenHandler = CRoutingSoundDeviceOpenHandler::NewL(this));				
				}
			if (!error)
				{
 				asynchOpen = ETrue;
 				iPlayDevice->SetBufferSize(iDeviceBufferSize);
				iOpenHandler->Start();
	 			static_cast<CMMFSwCodecPlayDataPath*>(iDataPath)->Device()->
 										OpenDevice(iDeviceUid, iOpenHandler->iStatus);
				}
			}
		if (!asynchOpen)
			{
			error = OpenPlayComplete(error);
			}

		}//status == KErrNone

	return error;
	}

TInt CMMFSwCodecWrapper::StartEncode()
	{//record

	//[ assert precondition that record custom interface is present]
	//if there is no record custom interface then the user of the CMMFSwCodecWrapper
	//cannot have set any of the custom settings such as sample rate.
	if (!iRecordCustomInterface)
		{
		return KErrNotReady;
		}

	TInt error = KErrNone;
	if (!iDataPath)
		{
		TRAP(error,iDataPath = CMMFSwCodecRecordDataPath::NewL(iRecordDevice));
		if ((iDataPath)&&(error == KErrNone))
			{
			iDataPath->SetObserver(*iHwDeviceObserver);
			error = iDataPath->AddCodec(*iCodec);
			iDeviceBufferSize = (iCodec->SourceBufferSize());
			static_cast<TRecordCustomInterface*>(iRecordCustomInterface)->SetDataPath(static_cast<CMMFSwCodecRecordDataPath*>(iDataPath));
			}
		}
	if ((error == KErrNone)&&
		(iDataPath->State() != CMMFSwCodecDataPath::EPlaying))
		{
		TBool asynchOpen = EFalse;	// Ensure we only call OpenPlayComplete once
		if (!(static_cast<CMMFSwCodecRecordDataPath*>(iDataPath))->Device()->Handle())
  			{
			// Create the AO OpenDevice handler
			if (!iOpenHandler)
				{
				TRAP(error, iOpenHandler = CRoutingSoundDeviceOpenHandler::NewL(this));				
				}
			if (!error)
				{
				asynchOpen = ETrue;
				iRecordDevice->SetBufferSize(iDeviceBufferSize);
				iOpenHandler->Start();
 				static_cast<CMMFSwCodecRecordDataPath*>(iDataPath)->Device()->
									OpenDevice(iDeviceUid, iOpenHandler->iStatus);
				}
			}
			
		if (!asynchOpen)
			{
			error = OpenRecordComplete(error);
			}			
		}

	return error;
	}

TInt CMMFSwCodecWrapper::StartConvert()
	{//convert

	TInt error = KErrNone;
	if (!iDataPath)
		{
		TRAP(error,iDataPath = CMMFSwCodecConvertDataPath::NewL());
		if ((iDataPath)&&(error == KErrNone))
			{
			iDataPath->SetObserver(*iHwDeviceObserver);
			error = iDataPath->AddCodec(*iCodec);
			}
		}
    if (error == KErrNone)
    	{
		error = iDataPath->Start();
    	}
	return error;
	}

/**
Temporarily suspends the current task of decoding or encoding.

@return An error code indicating if the function call was successful. KErrNone on success, otherwise
        another of the system-wide error codes.
*/
EXPORT_C TInt CMMFSwCodecWrapper::Pause()
	{
	// [ precondition that datapath exists ]
	if (!iDataPath)
		{
		return KErrNotReady;
		}
		
	iDataPath->Pause();
	return KErrNone;
	}

/**
Stops the current on-going task.

@return An error code indicating if the function call was successful. KErrNone on success, otherwise
        another of the system-wide error codes.
*/
EXPORT_C TInt CMMFSwCodecWrapper::Stop()
	{
	// [ precondition that datapath exists ]
	if (!iDataPath)
		{
		return KErrNotReady;
		}
	iDataPath->Stop();
	return KErrNone;
	}


/**
Stops and deletes the codec.

This default implementation simply calls DeleteCodec() and then Stop()
but real hardware devices might use this method to free up resources.

@return An error code indicating if the function call was successful. KErrNone on success, otherwise
        another of the system-wide error codes.
*/
EXPORT_C TInt CMMFSwCodecWrapper::StopAndDeleteCodec()
	{
	TInt stopError = Stop();
	TInt deleteError = DeleteCodec();

	if (stopError != KErrNone)
		{
		return stopError;
		}
	else
		{
		return deleteError;
		}
	}

/**
Deletes the codec
This default implementation does nothing
but real hardware devices might use this method to free up resources.
@return		Error code. KErrNone if successful
*/
EXPORT_C TInt CMMFSwCodecWrapper::DeleteCodec()
	{
	return KErrNone;
	}

/**
Call this function to notify hardware device implementation that
data is available in aFillBufferPtr for decoding.

@param aFillBufferPtr
       The data buffer filled by the observer.

@return An error code indicating if the function call was successful. KErrNone on success, otherwise
        another of the system-wide error codes.
*/
EXPORT_C TInt CMMFSwCodecWrapper::ThisHwBufferFilled(CMMFBuffer& aFillBufferPtr)
	{
	TRAPD(err,iDataPath->BufferFilledL(STATIC_CAST(CMMFDataBuffer&, aFillBufferPtr)));
	return err;
	}

/**
Call this function to notify hardware device implementation that
data in aEmptyBufferPtr from encoding is processed.

@param  aBuffer
        The data buffer processed by observer.

@return An error code indicating if the function call was successful. KErrNone on success, otherwise
        another of the system-wide error codes.
*/
EXPORT_C TInt CMMFSwCodecWrapper::ThisHwBufferEmptied(CMMFBuffer& aBuffer)
	{
	TRAPD(err,iDataPath->BufferEmptiedL(STATIC_CAST(CMMFDataBuffer&, aBuffer)));
	return err;
	}


/**
Retrieves a custom interface to the device.
The reference CMMFSwCodecWrapper supports two standard custom interfaces,
TPlayCustomInterface and TRecordCustomInterface.

@param	aInterface
		Interface UID, defined with the custom interface.
		aInterface = KMmfPlayCustomInterface for TPlayCustomInterface,
		aInterface = KMmfRecordCustomInterface for TRecordCustomInterface.
		Actual device implementations of CMMFSwCodecWrapper may do this differently however.
@return A pointer to the interface implementation, or NULL if the device can not
		implement the interface requested. The return value must be cast to the
		correct type by the user.
*/
EXPORT_C TAny* CMMFSwCodecWrapper::CustomInterface(TUid aInterface)
	{
	TAny* ret = NULL;
	TInt err = KErrNone;
	if (aInterface.iUid == KMmfPlaySettingsCustomInterface)
		{
		if (!iPlayCustomInterface)
			{
			TRAP(err,iPlayCustomInterface = new(ELeave)TPlayCustomInterface());
			}
		if (err)
			{
			ret = NULL;
			}
		else
			{
			ret = static_cast<TAny*>(iPlayCustomInterface);
			}
		}
	else if (aInterface.iUid == KMmfRecordSettingsCustomInterface)
		{
		if (!iRecordCustomInterface)
			{
			TRAP(err,iRecordCustomInterface = new(ELeave)TRecordCustomInterface());
			}
		if (err)
			{
			ret = NULL;
			}
		else
			{
			ret = static_cast<TAny*>(iRecordCustomInterface);
			}
		}

	return ret;
	}


/**
Used to configure the sample rate and stereo mode of a CMMFHwDevice plugin.

The configuration of HwDevices is device specific and is not used in any of the reference
devices that return KErrNotSupported.

@param  aConfig
        The device configuration.
*/
EXPORT_C TInt CMMFSwCodecWrapper::SetConfig(TTaskConfig& aConfig)
	{
	TInt err = KErrNone;
	if (aConfig.iUid != KUidRefDevSoundTaskConfig)
		return KErrArgument;
	iSampleRate = aConfig.iRate;//note we're cheating a bit
	//iRate is actually a bit map enum but an actual value
	//is more useful here
	TMMFStereoSupport stereoSupport = EMMFNone;
	if (aConfig.iStereoMode == ETaskMono)
		{
		iChannels = 1;
		}
	else if (aConfig.iStereoMode == ETaskInterleaved) 
		{
		iChannels = 2;
		stereoSupport = EMMFInterleavedOnly;
		}
	else //don't support non interleaved
		{
		return KErrNotSupported;
		}
		
	//set values on routing sound device
	if (iPlayDevice)
		{
		err = iPlayDevice->SetChannels(iChannels, stereoSupport);
		if (!err)
			{
			err = iPlayDevice->SetSampleRate(iSampleRate);
			}
		if (!err)
			{//we'll set the data type while were at it for now pcm16 only
			TFourCC fourCC = KMMFFourCCCodePCM16;
			err = iPlayDevice->SetDataType(fourCC);
			}
		}
	if ((iRecordDevice) && (!err))
		{
		err = iRecordDevice->SetChannels(iChannels, stereoSupport);
		if (!err)
			{
			err = iRecordDevice->SetSampleRate(iSampleRate);
			}
		}
	
	return err;
	}

/**
Callback once CRoutingSoundPlayDevice or CRoutingSoundRecordDevice::Open has completed or continuation from
call to StartDecode / StartEncode, depending on the state.

@internalTechnology
@param	aError
		The status of the device.
@return A standard Symbian wide error code.
 */
TInt CMMFSwCodecWrapper::OpenComplete(TInt aError)
	{
	TInt err = KErrNotSupported;	//
	if (iPlayDevice)
		{
		err = OpenPlayComplete(aError);
		}
	else if (iRecordDevice)
		{
		err = OpenRecordComplete(aError);
		}
	return err;
	}
	
/**
Callback once CRoutingSoundPlayDevice::Open has completed or continuation from
call to StartDecode, depending on the state.

@param	aError
		The status of the device.
@return A standard Symbian wide error code.
 */
TInt CMMFSwCodecWrapper::OpenPlayComplete(TInt aError)
	{
	TInt error = aError;
	
	//datapath was created ok and we are not playing
	if (error == KErrNone && iDataPath->State() == CMMFSwCodecDataPath::EStopped)
		{
		//starting from fresh so set sound device settings
//		static_cast<TPlayCustomInterface*>(iPlayCustomInterface)->SetDevice(
//					static_cast<CMMFSwCodecPlayDataPath*>(iDataPath)->Device() );
		static_cast<CMMFSwCodecPlayDataPath*>(iDataPath)->Device()->SetVolume(iPlayCustomInterface->Volume());
		}
		
	// resuming from pause or ready to play
	if ((error == KErrNone) || (error == KErrInUse))
		{
		error = iDataPath->Start();
		}		
	
	return error;
	}

/**
Callback once CRoutingSoundRecordDevice::Open has completed or continuation from
call to StartDecode, depending on the state.

@param	aError
		The status of the device.
@return A standard Symbian wide error code.
 */
TInt CMMFSwCodecWrapper::OpenRecordComplete(TInt aError)
	{
	TInt error = aError;
	
	//datapath was created ok and we are not playing
	if (error == KErrNone && iDataPath->State() == CMMFSwCodecDataPath::EStopped)
		{
		//set sound driver settings
		static_cast<CMMFSwCodecRecordDataPath*>(iDataPath)->Device()
												->SetGain(iRecordCustomInterface->Gain());
		/*
		// Sample rate
		(static_cast<CMMFSwCodecRecordDataPath*>(iDataPath))->Device()->SetSampleRate(iSampleRate);
		// Channels
		TMMFStereoSupport stereoSupport = EMMFNone;	// assume mono
		if (iChannels == 2)
			{
			// Assume interleaved only
			stereoSupport = EMMFInterleavedOnly;
			}
		error = (static_cast<CMMFSwCodecRecordDataPath*>(iDataPath))->Device()->SetChannels(iChannels, stereoSupport);
		*/
		}

	// resuming from pause or ready to play
	if ((error == KErrNone) || (error == KErrInUse))
		{
		error = iDataPath->Start();
		}		
	
	return error;
	}

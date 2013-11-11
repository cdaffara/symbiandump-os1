// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <mmf/server/mmfswcodecwrapper.h>
#include "mmfSwCodecPlayDataPath.h"
#include "mmfSwCodecRecordDataPath.h"
#include "mmfSwCodecConvertDataPath.h"
#include <mmf/server/mmfswcodecwrappercustominterfacesuids.hrh>
#include "mmfswcodecwrapperCustomInterfaces.h"
#include <mmf/common/mmfpaniccodes.h>



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
 * It's purpose is to assign an RMdaDevSound to the play
 * custom interface
 * @internalComponent
 */
void TPlayCustomInterface::SetDevice(RMdaDevSound* aDevice)
	{
	iDevice = aDevice;
	}

void TPlayCustomInterface::SetVolume(TUint aVolume)
	{
	iVolume = aVolume;
	if (iDevice && iDevice->Handle())
		iDevice->SetPlayVolume(iVolume);
	}
	
/**
 * Procedure to get the number of bytes played by the device driver
 * If there is no handle available to the device driver then the 
 * procedure returns the last known value
 * @released
 * @return number of bytes played
 */
TUint TPlayCustomInterface::BytesPlayed()
	{
	if(iDevice)
		{
		if (iDevice->Handle())
			iBytesPlayed = iDevice->BytesPlayed();
		}
	return iBytesPlayed;
	}

/**
 * Procedure to get the number of bytes recorded by the device  
 * @released
 * @return The number of bytes recorded by an existing datapath.  If there
 * is no datapath, then the last known number of bytes recorded will be returned.
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
	delete iDataPath;
	delete iCodec;
	delete iPlayCustomInterface;
	delete iRecordCustomInterface;
	}

/**
Initializes the hardware device tasks - in the case of a
sw codec wrapper 'hardware device' this consits of loading the
sound device drivers and creating the CMMFSwCodec.

@param  aDevInfo
        Device initialization parameters.
        Only the iHwDeviceObserver is used for CMFSwCodecWrapper
        derived CMMFHwDevices.
@return An error code indicating if the function call was successful. KErrNone on success, otherwise
        another of the system-wide error codes.
*/
EXPORT_C TInt CMMFSwCodecWrapper::Init(THwDeviceInitParams &aDevInfo)
	{

	// [ precondition that aDevInfo has a valid observer ]
	if (!aDevInfo.iHwDeviceObserver) 
		return KErrArgument;

	iHwDeviceObserver = aDevInfo.iHwDeviceObserver;
#ifndef SYMBIAN_MDF_SHAREDCHUNK_SOUNDDRIVER //Adapter loads the drivers
	// Try to load the audio physical driver
    TInt ret = User::LoadPhysicalDevice(KPddFileName);
	if ((ret!=KErrNone) && (ret!=KErrAlreadyExists))
        return ret;

    // Try to load the audio logical driver
	ret = User::LoadLogicalDevice(KLddFileName);
    if ((ret!=KErrNone) && (ret!=KErrAlreadyExists))
        return ret;
#endif
	iCodec = &(Codec()); //create codec

	//[ assert the post condition ]
	if (!iCodec) 
		return KErrNotSupported;

	return KErrNone;
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
		return KErrArgument;

	// [ precondition that iCodec is present]
	if (!iCodec)
		return KErrNotReady; //make sure the codec has been added

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
			__ASSERT_DEBUG(iDataPath->Device().Handle(), Panic(EMMFSwCodecWrapperNoDevice));
		}
#endif
	if(error != KErrNone && iDataPath && aFuncCmd!=EDevEncode)
		{//if error happens after opening LDD close it
		if (iDataPath->Device().Handle()!= KNullHandle)
			{
			iDataPath->Device().Close();
			}
		}

	return error;
	}


TInt CMMFSwCodecWrapper::StartDecode()
	{
	TInt error = KErrNone;

	//[ assert precondition that play custom interface is present]
	//if there is no play custom interface then the user of the CMMFSwCodecWrapper
	//cannot have set any of the custom settings such as sample rate.
	if (!iPlayCustomInterface)
		return KErrNotReady;

	//play
	if (!iDataPath)
		{//create a datapath
		TRAP(error,iDataPath = CMMFSwCodecPlayDataPath::NewL());
		//if datapath could not be created, return error code
		if (error != KErrNone)
			{
			return error;
			}
		
		//here we are sure iDataPath has been correctly allocated		
		iDataPath->SetObserver(*iHwDeviceObserver);
		error = iDataPath->AddCodec(*iCodec);
		if (error == KErrNone)
			{
			iDeviceBufferSize = (iCodec->SinkBufferSize());
			static_cast<CMMFSwCodecPlayDataPath*>(iDataPath)->SetPlayCustomInterface(*iPlayCustomInterface);
			}
		else
			{
			// if could not add codec to datapath, return error code
			return error;
			}
		}
		
	//Here we know that error is KerrNone, now we can check the state of the datapath	
	if (iDataPath->State() != CMMFSwCodecDataPath::EPlaying)
		{//datapath was created ok and we are not playing
		if (iDataPath->State() == CMMFSwCodecDataPath::EStopped)
			{//starting from 'fresh so set sound device settings
			if (!iDataPath->Device().Handle())
  				{//if Device() is called then we need a valid sound device handle
  				error = iDataPath->Device().Open();
				if (error != KErrNone)
					return error;
				}
			static_cast<TPlayCustomInterface*>(iPlayCustomInterface)->SetDevice(&(iDataPath->Device()));
			iDataPath->Device().SetPlayVolume(iPlayCustomInterface->Volume());
			RMdaDevSound::TCurrentSoundFormatBuf soundDeviceSettings;
			soundDeviceSettings().iRate = iSampleRate;
			//this would normally be pcm16
			soundDeviceSettings().iEncoding = RMdaDevSound::EMdaSoundEncoding16BitPCM;
			//1 = mono 2 = stereo
			soundDeviceSettings().iChannels = iChannels;
			//tell sound driver what buffer size to expect
			//it is up the the implementor to make use the device can support
			//the required buffer size
			soundDeviceSettings().iBufferSize = iDeviceBufferSize;
			error = iDataPath->Device().SetPlayFormat(soundDeviceSettings);	
			}//iDataPath->State() == CMMFSwCodecDataPath::EStopped
		//else resuming from pause	
		if ((error == KErrNone)||(error == KErrInUse))
			error = iDataPath->Start();
		}//status == KErrNone
	return error;
	}


TInt CMMFSwCodecWrapper::StartEncode()
	{//record

	//[ assert precondition that record custom interface is present]
	//if there is no record custom interface then the user of the CMMFSwCodecWrapper
	//cannot have set any of the custom settings such as sample rate.
	if (!iRecordCustomInterface)
		return KErrNotReady;

	TInt error = KErrNone;
	if (!iDataPath)
		{
		TRAP(error,iDataPath = CMMFSwCodecRecordDataPath::NewL());
		//if datapath could not be created, return error code
		if (error != KErrNone)
			{
			return error;
			}

		//here we are sure iDataPath has been correctly allocated
		iDataPath->SetObserver(*iHwDeviceObserver);
		error = iDataPath->AddCodec(*iCodec);
		if (error == KErrNone)
			{
			iDeviceBufferSize = (iCodec->SourceBufferSize());
			static_cast<TRecordCustomInterface*>(iRecordCustomInterface)->SetDataPath(static_cast<CMMFSwCodecRecordDataPath*>(iDataPath));
			}
		else
			{
			// if could not add codec to datapath, return error code
			return error;
			}
		}
	
	//Here we know that error is KerrNone, now we can check the state of the datapath
	if (iDataPath->State() != CMMFSwCodecDataPath::EPlaying)
		{
		if (iDataPath->State() == CMMFSwCodecDataPath::EStopped)
			{
			MSwSetParamInterface* setParams = 
				static_cast<MSwSetParamInterface*>(iDataPath->CustomInterface(KUidSwSetParamInterface));
			ASSERT(!error); // should not get here if error set
			error = setParams->SetGain(iRecordCustomInterface->Gain());
			if (!error)
				{
				error = setParams->SetNumChannels(iChannels);
				}
			if (!error)
				{
				error = setParams->SetSampleRate(iSampleRate);
				}
			}
		if (error == KErrNone)
			{
			error = iDataPath->Start();
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
		if (error != KErrNone)
			{
			return error;
			}
		}
	
	//Here we know we are not dereferencing a null pointer as iDataPath has been correctly initialised
	iDataPath->SetObserver(*iHwDeviceObserver);
	error = iDataPath->AddCodec(*iCodec);
	
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
		return KErrNotReady;

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
		return KErrNotReady;

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
		return stopError;
	else
		return deleteError;
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
MPlayCustomInterface and MRecordCustomInterface.

@param	aInterface
		Interface UID, defined with the custom interface.
		aInterface = KMmfPlaySettingsCustomInterface for MPlayCustomInterface,
		aInterface = KMmfRecordSettingsCustomInterface for MRecordCustomInterface.
		aInterface = KMmfUidEmptyBuffersCustomInterface for MEmptyBuffersCustomInterface
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
			TRAP(err,iPlayCustomInterface = new(ELeave)TPlayCustomInterface());
		if (err)
			ret = NULL;
		else
			ret = static_cast<TAny*>(iPlayCustomInterface);
		}
	else if (aInterface.iUid == KMmfRecordSettingsCustomInterface)
		{
		if (!iRecordCustomInterface)
			TRAP(err,iRecordCustomInterface = new(ELeave)TRecordCustomInterface());
		if (err)
			ret = NULL;
		else 
			ret = static_cast<TAny*>(iRecordCustomInterface);
		}
		
	else if (aInterface.iUid == KMmfUidEmptyBuffersCustomInterface || aInterface == KTimePlayedCustomInterfaceTypeUid || aInterface == KIgnoreUnderflowCustomInterfaceTypeUid)
		{
		if (!iDataPath)
			{
			ret = NULL;			
			}
		else
			{
			ret = static_cast<CMMFSwCodecDataPath*>(iDataPath)->CustomInterface(aInterface);	
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
	if (aConfig.iUid != KUidRefDevSoundTaskConfig)
		return KErrArgument;
	iSampleRate = aConfig.iRate;
	
	if (aConfig.iStereoMode == ETaskMono)
		{
		iChannels = 1;
		}
	else if (aConfig.iStereoMode == ETaskInterleaved || aConfig.iStereoMode == ETaskNonInterleaved)
		{
		iChannels = 2;
		}
	else
		{
		return KErrArgument;
		}
	return KErrNone;
	}

/**
Used to set iVbrFlag on the datapath.

This method is used to set the iVbrFlag in datapath. This flag is added to datapath to avail the 
alternative dataflow wherein datapath makes sure that destinationbuffer is filled to its maximum length
before sending it to the sound driver. Sending the buffer directly to the device causes underflow incase of VBR codecs.
*/
EXPORT_C void CMMFSwCodecWrapper::SetVbrFlag()
	{
	if(iDataPath)
		{
		TUid cUid = TUid::Uid(KSetVbrFlagCustomInterfaceTypeUid);
		iDataPath->CustomInterface(cUid);
		}
	}



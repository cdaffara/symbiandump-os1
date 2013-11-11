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

#include <mmfbase.h>
#include <bttypes.h>
#include "MMFBtRoutingSoundDevice.h"

// Sound drivers
_LIT(KPddFileName,"ESDRV.PDD");
_LIT(KLddFileName,"ESOUND.LDD");




EXPORT_C CRoutingSoundPlayDevice* CRoutingSoundPlayDevice::NewL()
	{
	CRoutingSoundPlayDevice* self = new(ELeave) CRoutingSoundPlayDevice();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CRoutingSoundPlayDevice::ConstructL()
	{
	}
	
CRoutingSoundPlayDevice::CRoutingSoundPlayDevice()
	{
	}
	
EXPORT_C CRoutingSoundPlayDevice::~CRoutingSoundPlayDevice()
	{
	if (iDeviceUid != KDeviceUidA2dpHeadset)
		{
		if (iSpeakerDevice.Handle() > 0)
			{
			iSpeakerDevice.Close();
			}
		}
	else
		{
		iBTDevice.Close();
		}
	}

EXPORT_C void CRoutingSoundPlayDevice::Initialize(	TUid aDeviceUid,
													const TDesC8& aDeviceConfig,
													TRequestStatus& aStatus)
	{
	TInt err = KErrNone;
	iDeviceUid = aDeviceUid;
	iInitializeStatus = &aStatus;

	if (iDeviceUid == KDeviceUidA2dpHeadset)
		{
		// Connect to the Bluetooth client
		err = iBTDevice.Connect();
		if (!err)
			{
			// Get the Bluetooth device's address using the descriptor
			iBTAddress = TBTDevAddr(aDeviceConfig);
			// Initialize
			iBTDevice.Initialize(iBTAddress, *iInitializeStatus);
			}
		else
			{
			// Failed to connect
			User::RequestComplete(iInitializeStatus, err);
			}
		}

	else if ((iDeviceUid == KDeviceUidSpeaker)||(iDeviceUid == KDeviceUidMic))
		{
		// aDeviceConfig must be NULL to reach here (see 5.1 of design doc)
		// Load the drivers
		// Try to load the audio physical driver
	    err = User::LoadPhysicalDevice(KPddFileName);
		if ((err == KErrNone) || (err == KErrAlreadyExists))
			{
		    // Try to load the audio logical driver
			err = User::LoadLogicalDevice(KLddFileName);
		    }
		
		if (err == KErrAlreadyExists)
			{
			// It's not a problem if the drivers have already
			// been loaded so reset the error code accordingly
			err = KErrNone;
			}
		// Completed to load the drivers so signal
		User::RequestComplete(iInitializeStatus, err);
		}
	else
		{
		// Unexpected Uid
		User::RequestComplete(iInitializeStatus, KErrNotSupported);
		}
	}

EXPORT_C void CRoutingSoundPlayDevice::CancelInitialize(TUid aDeviceUid)
	{
	if (aDeviceUid == iDeviceUid)
		{//can only cancel bt headset initialize
		if (aDeviceUid == KDeviceUidA2dpHeadset)
			{
			iBTDevice.CancelInitialize();
			}
		if (iInitializeStatus && *iInitializeStatus == KRequestPending)
			{
			User::RequestComplete(iInitializeStatus, KErrCancel);				
			}		
		}
	}
	
/**
 *	OpenDevice() is used when a specific client needs a handle to the sound
 *	device and is called after permission has been given by the policy server.
 *	TODO - what's the required behaviour when calling OpenDevice repeatedly
 *	(without calling CloseDevice in between) - prevent it (as now by checking the 
 *	handle) or just return the KErrInUse code?
 *	TODO:
 *	Check differences in behaviour: RMdaDevSound must be Opened before any "Set"
 *	calls can be made on it, whereas BT headset's settings can be changed
 *	after it has been initialized but before it's been opened.
 */
EXPORT_C void CRoutingSoundPlayDevice::OpenDevice(	TUid aDeviceUid,
													TRequestStatus& aStatus)
	{
	iOpenDeviceStatus = &aStatus;
	if (aDeviceUid == KDeviceUidSpeaker)
		{
		TInt error = KErrNone;
		if (!Handle())
			{
			error = iSpeakerDevice.Open();
			if ((!error)||(error == KErrAlreadyExists))
				{
				error = KErrNone;//swallow KErrAlreadyExists
				//update the configuration here ignore error
				//it'll get picked up below if not opened due to error
				RMdaDevSound::TCurrentSoundFormatBuf soundDeviceSettings;
				iSpeakerDevice.GetPlayFormat(soundDeviceSettings);
				soundDeviceSettings().iEncoding = RMdaDevSound::EMdaSoundEncoding16BitPCM;
				if (iSampleRate)
					{
					soundDeviceSettings().iRate = iSampleRate;
					}
				if (iChannels)
					{
					soundDeviceSettings().iChannels = iChannels;
					}
				//tell sound driver what buffer size to expect
				//it is up the the implementor to make use the device can support
				//the required buffer size
				if (iBufferSize)
					{	
					soundDeviceSettings().iBufferSize = iBufferSize;
					}
				error = iSpeakerDevice.SetPlayFormat(soundDeviceSettings);
				if (error)
					{
					iSpeakerDevice.Close();
					}
				}
			}
		User::RequestComplete(iOpenDeviceStatus, error);
		}
	else if (aDeviceUid == KDeviceUidA2dpHeadset)
		{
		iBTDevice.OpenDevice(*iOpenDeviceStatus);
		}
	else
		{
		User::RequestComplete(iOpenDeviceStatus, KErrNotSupported);
		}
	}
	
EXPORT_C void CRoutingSoundPlayDevice::CancelOpenDevice(TUid aDeviceUid)
	{
	if (aDeviceUid == iDeviceUid)
		{
		if (aDeviceUid != KDeviceUidA2dpHeadset)
			{
			if(Handle() > 0)
			 	{
			 	iSpeakerDevice.Close();
			 	}
			}
		else
			{
			iBTDevice.CancelOpenDevice();
			}
		if (iOpenDeviceStatus && *iOpenDeviceStatus == KRequestPending)
			{
			User::RequestComplete(iOpenDeviceStatus, KErrCancel);				
			}		
		}
	}

EXPORT_C void CRoutingSoundPlayDevice::CloseDevice(TUid aDeviceUid, TRequestStatus& aStatus)
	{
	if (aDeviceUid != KDeviceUidA2dpHeadset)
		{
		aStatus = KRequestPending;
		if (Handle() > 0)
			{
			iSpeakerDevice.Close();
			}
		TRequestStatus* status = &aStatus;
		User::RequestComplete(status, KErrNone);
		}
	else
		{
		iCloseDeviceStatus = &aStatus;
		iBTDevice.CloseDevice(*iCloseDeviceStatus);
		}
	}
	
EXPORT_C TInt CRoutingSoundPlayDevice::GetSupportedDataTypes(RArray<TFourCC>& aSupportedDataTypes)
	{
	TInt err = KErrNone;
	if (iDeviceUid != KDeviceUidA2dpHeadset)
		{
		if (Handle() > 0)
			{
			RMdaDevSound::TSoundFormatsSupportedBuf formats;
			iSpeakerDevice.PlayFormatsSupported(formats);
			// Only PCM16 is supported (what about unsigned PCM16 and big-endian versions?)
			err = aSupportedDataTypes.Append(KMMFFourCCCodePCM16);
			if (!err)
				{
				aSupportedDataTypes.Reset();
				}
			}
		else
			{
			err = KErrNotReady;
			}
		}
	else
		{
		TRAP(err, iBTDevice.GetSupportedDataTypesL(aSupportedDataTypes));
		}

	return err;
	}
	
EXPORT_C TInt CRoutingSoundPlayDevice::GetSupportedSampleRates(RArray<TUint>& aSupportedDiscreteRates,
																RArray<TRange>& aSupportedRateRanges)
	{
	TInt err = KErrNone;
	aSupportedDiscreteRates.Reset();
	aSupportedRateRanges.Reset();
	if (iDeviceUid != KDeviceUidA2dpHeadset)
		{
		if (Handle() > 0)
			{
			RMdaDevSound::TSoundFormatsSupportedBuf formatsSupported;
			iSpeakerDevice.PlayFormatsSupported(formatsSupported);
			TRange range;
			range.iLow = formatsSupported().iMinRate;
			range.iHigh = formatsSupported().iMaxRate;
			err = aSupportedRateRanges.Append(range);
			if (err != KErrNone)
				{
				aSupportedRateRanges.Reset();
				}
			}
		else
			{
			err = KErrNotReady;
			}
		}
	else
		{
		TRAP(err, iBTDevice.GetSupportedSampleRatesL(aSupportedDiscreteRates,
													aSupportedRateRanges);)
		}
	
	return err;
	}
	
EXPORT_C TInt CRoutingSoundPlayDevice::GetSupportedChannels(RArray<TUint>& aSupportedChannels,
															TMMFStereoSupport& aStereoSupport)
	{
	TInt err = KErrNone;
	aSupportedChannels.Reset();
	if (iDeviceUid != KDeviceUidA2dpHeadset)
		{
		if (Handle() > 0)
			{
			RMdaDevSound::TCurrentSoundFormatBuf soundDeviceSettings;
			iSpeakerDevice.GetPlayFormat(soundDeviceSettings);
			//1 = mono 2 = stereo
			err = aSupportedChannels.Append(soundDeviceSettings().iChannels);
			if (soundDeviceSettings().iChannels != 2)
				{
				// No stereo support
				aStereoSupport = EMMFNone;
				}
			else
				{
				aStereoSupport = EMMFBothNonAndInterleaved;
				}
			}
		else
			{
			err = KErrNotReady;
			}
		}
	else
		{
		TRAP(err, iBTDevice.GetSupportedChannelsL(aSupportedChannels, aStereoSupport));
		}
	
	return err;
	}
	
EXPORT_C TInt CRoutingSoundPlayDevice::SetDataType(const TFourCC& aDataType)
	{
	TInt err = KErrNone;
	if (iDeviceUid != KDeviceUidA2dpHeadset)
		{
		if (Handle() > 0)
			{
			if (aDataType == KMMFFourCCCodePCM16)
				{
				// Section 5 of design doc => only PCM16 is supported
				RMdaDevSound::TCurrentSoundFormatBuf soundDeviceSettings;
				iSpeakerDevice.GetPlayFormat(soundDeviceSettings);
				soundDeviceSettings().iEncoding = RMdaDevSound::EMdaSoundEncoding16BitPCM;
				err = iSpeakerDevice.SetPlayFormat(soundDeviceSettings);
				}
			else
				{
				// Trying to set an unsupported data type
				err = KErrNotSupported;
				}
			}
		// else actually doesn't really matter - we always set the sound driver to pcm16 anyway
		}
	else
		{
		err = iBTDevice.SetDataType(aDataType);
		}
		
	return err;
	}

EXPORT_C TInt CRoutingSoundPlayDevice::SetSampleRate(TUint aSampleRate)
	{
	TInt err = KErrNone;

	if (iDeviceUid != KDeviceUidA2dpHeadset)
		{
		if (Handle())
			{
			RMdaDevSound::TCurrentSoundFormatBuf soundDeviceSettings;
			iSpeakerDevice.GetPlayFormat(soundDeviceSettings);
			soundDeviceSettings().iRate = aSampleRate;
			err = iSpeakerDevice.SetPlayFormat(soundDeviceSettings);
			}
	//should also check whether we support aSampleRate
		}
	else
		{
		err = iBTDevice.SetSampleRate(aSampleRate);
		}
	
	iSampleRate = aSampleRate;
	return err;
	}

EXPORT_C TInt CRoutingSoundPlayDevice::SetChannels(	TUint aChannels,
													TMMFStereoSupport aStereoSupport)
	{
	TInt err = KErrNone;
	
	if (iDeviceUid != KDeviceUidA2dpHeadset)
		{
		if (Handle() > 0)
			{
			RMdaDevSound::TCurrentSoundFormatBuf soundDeviceSettings;
			iSpeakerDevice.GetPlayFormat(soundDeviceSettings);
			// 1 = mono 2 = stereo
			soundDeviceSettings().iChannels = aChannels;			
			err = iSpeakerDevice.SetPlayFormat(soundDeviceSettings);
			}
		}
	else
		{
		err = iBTDevice.SetChannels(aChannels, aStereoSupport);
		}
	iChannels = aChannels;
	iStereoSupport = aStereoSupport;
	return err;	
	}
	
EXPORT_C TInt CRoutingSoundPlayDevice::SetBufferSize(TUint aBufferSize)
	{
	TInt err = KErrNone;
	
	if (iDeviceUid != KDeviceUidA2dpHeadset)
		{
		if (Handle() > 0)
			{
			RMdaDevSound::TCurrentSoundFormatBuf soundDeviceSettings;
			iSpeakerDevice.GetPlayFormat(soundDeviceSettings);
			// 1 = mono 2 = stereo
			soundDeviceSettings().iBufferSize = aBufferSize;			
			err = iSpeakerDevice.SetPlayFormat(soundDeviceSettings);
			}
		}
	iBufferSize = aBufferSize;
	//else we don't need this for bt headset
	return err;
	}

EXPORT_C void CRoutingSoundPlayDevice::FlushBuffer()
	{
	if (iDeviceUid != KDeviceUidA2dpHeadset)
		{
		if (Handle() > 0)
			{
			iSpeakerDevice.FlushPlayBuffer();
			}
		}
	else
		{
		iBTDevice.FlushBuffer();
		}
	}

EXPORT_C void CRoutingSoundPlayDevice::NotifyError(TRequestStatus& aStatus)
	{
	if (iDeviceUid != KDeviceUidA2dpHeadset)
		{
		if (Handle() > 0)
			{
			iSpeakerDevice.NotifyPlayError(aStatus);
			}
		}
	else
		{
		iBTDevice.NotifyError(aStatus);
		}
	}

EXPORT_C void CRoutingSoundPlayDevice::CancelNotifyError()
	{
	if (iDeviceUid != KDeviceUidA2dpHeadset)
		{
		// To avoid a KE0 panic if -ve handle is returned
		if (Handle() > 0)
			{		
			iSpeakerDevice.CancelNotifyPlayError();
			}
		}
	else
		{
		iBTDevice.CancelNotifyError();
		}	
	}

EXPORT_C TUint CRoutingSoundPlayDevice::Volume() const
	{
	TUint volume = 0;
	if (iDeviceUid != KDeviceUidA2dpHeadset)
		{
		if (Handle() > 0)
			{
			RMdaDevSound* mutableSpeakerDevice = const_cast<RMdaDevSound*>(&iSpeakerDevice);
			volume = mutableSpeakerDevice->PlayVolume();
			}
		}
	else
		{
		volume = iBTDevice.Volume();
		}
	return volume;
	}
	
EXPORT_C TInt CRoutingSoundPlayDevice::SetVolume(TUint aVolume)
	{
	TInt err = KErrNone;
	if (iDeviceUid != KDeviceUidA2dpHeadset)
		{
		if (Handle() > 0)
			{
			iSpeakerDevice.SetPlayVolume(aVolume);
			}
		}
	else
		{
		err = iBTDevice.SetVolume(aVolume);
		}
	return err;
	}
	
EXPORT_C void CRoutingSoundPlayDevice::PlayData(const TDesC8& aData, TRequestStatus& aStatus)
	{
	if (iDeviceUid != KDeviceUidA2dpHeadset)
		{
		if (Handle() > 0)
			{
			iSpeakerDevice.PlayData(aStatus, aData);
			}
		else
			{
			TRequestStatus* status = &aStatus;
			User::RequestComplete(status, KErrBadHandle);
			}
		}
	else
		{
		iBTDevice.PlayData(aData, aStatus);
		}
	}
	
EXPORT_C void CRoutingSoundPlayDevice::CancelPlayData()
	{
	if (iDeviceUid != KDeviceUidA2dpHeadset)
		{
		if (Handle() > 0)
			{
			iSpeakerDevice.CancelPlayData();
			}
		}
	else
		{
		iBTDevice.CancelPlayData();
		}
	}
	
EXPORT_C TUint CRoutingSoundPlayDevice::BytesPlayed()
	{
	TUint bytes = 0;
	if (iDeviceUid != KDeviceUidA2dpHeadset)
		{
		if (Handle() > 0)
			{
			bytes = iSpeakerDevice.BytesPlayed();
			}
		}
	else
		{
		bytes = iBTDevice.BytesPlayed();
		}

	return bytes;
	}
	
EXPORT_C TInt CRoutingSoundPlayDevice::ResetBytesPlayed()
	{
	if (iDeviceUid != KDeviceUidA2dpHeadset)
		{
		if (Handle() > 0)
			{		
			iSpeakerDevice.ResetBytesPlayed();
			}
		}
	else
		{
		iBTDevice.ResetBytesPlayed();
		}
	return KErrNone;	// why are we returing a value?
	}
	
EXPORT_C TInt CRoutingSoundPlayDevice::PauseBuffer()
	{
	if (iDeviceUid != KDeviceUidA2dpHeadset)
		{
		if (Handle() > 0)
			{
			iSpeakerDevice.PausePlayBuffer();
			}
		}
	else
		{
		iBTDevice.PauseBuffer();
		}
	return KErrNone;
	}

EXPORT_C TInt CRoutingSoundPlayDevice::ResumePlaying()
	{
	if (iDeviceUid != KDeviceUidA2dpHeadset)
		{
		if (Handle() > 0)
			{
			iSpeakerDevice.ResumePlaying();
			}
		}
	else
		{
		iBTDevice.ResumePlaying();
		}
	return KErrNone;
	}
	
EXPORT_C TInt CRoutingSoundPlayDevice::Handle() const
	{
	TInt handle;
	if (iDeviceUid != KDeviceUidA2dpHeadset)
		{
		handle = iSpeakerDevice.Handle();
		}
	else
		{
		handle = iBTDevice.Handle();
		}
	return handle;
	}
	
/**
 * Implementation of CRoutingSoundRecordDevice.
 */
EXPORT_C CRoutingSoundRecordDevice* CRoutingSoundRecordDevice::NewL()
	{
	CRoutingSoundRecordDevice* self = new(ELeave) CRoutingSoundRecordDevice();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CRoutingSoundRecordDevice::ConstructL()
	{
	//...
	}
	
CRoutingSoundRecordDevice::CRoutingSoundRecordDevice()
	{
	}

EXPORT_C CRoutingSoundRecordDevice::~CRoutingSoundRecordDevice()
	{
	if (iInputDevice.Handle())
		{
		iInputDevice.Close();
		}
	}
	

EXPORT_C void CRoutingSoundRecordDevice::Initialize(TUid aDeviceUid,
													const TDesC8& /*aDeviceConfig*/,
													TRequestStatus& aStatus)

	{
	iDeviceUid = aDeviceUid;
	TRequestStatus* status = &aStatus;
	TInt ret = KErrNone;
	if (iDeviceUid == KDeviceUidMic)
		{
		// Load the drivers
		// Try to load the audio physical driver
	    ret = User::LoadPhysicalDevice(KPddFileName);
		if ((ret == KErrNone) || (ret == KErrAlreadyExists))
			{
		    // Try to load the audio logical driver
			ret = User::LoadLogicalDevice(KLddFileName);
		    }
		
		if (ret == KErrAlreadyExists)
			{
			// It's not a problem if the drivers have already
			// been loaded so reset the error code accordingly
			ret = KErrNone;
			}
		}
	else
		{
		ret = KErrNotSupported;
		}

	User::RequestComplete(status, ret);
	}

EXPORT_C void CRoutingSoundRecordDevice::CancelInitialize(TUid /*aDeviceUid*/)
	{
	// Nothing to do
	}

EXPORT_C void CRoutingSoundRecordDevice::OpenDevice(TUid /*aDeviceUid*/, TRequestStatus& aStatus)
	{
	iOpenDeviceStatus = &aStatus;
	TInt error = KErrNone;
	if (!iInputDevice.Handle())
		{
		error = iInputDevice.Open();
		if ((!error)||(error == KErrAlreadyExists))
			{
			error = KErrNone;//swallow KErrAlreadyExists
			//update the configuration here ignore error
			//it'll get picked up below if not opened due to error
			RMdaDevSound::TCurrentSoundFormatBuf soundDeviceSettings;
			iInputDevice.GetRecordFormat(soundDeviceSettings);
			soundDeviceSettings().iEncoding = RMdaDevSound::EMdaSoundEncoding16BitPCM;
			if (iSampleRate)
				{
				soundDeviceSettings().iRate = iSampleRate;
				}
			if (iChannels)
				{
				soundDeviceSettings().iChannels = iChannels;
				}
			//tell sound driver what buffer size to expect
			//it is up the the implementor to make use the device can support
			//the required buffer size
			if (iBufferSize)
				{	
				soundDeviceSettings().iBufferSize = iBufferSize;
				}
			error = iInputDevice.SetRecordFormat(soundDeviceSettings);
			if (error)
				{
				iInputDevice.Close();
				}
			}
		}
	User::RequestComplete(iOpenDeviceStatus, error);
	}

EXPORT_C void CRoutingSoundRecordDevice::CancelOpenDevice(TUid aDeviceUid)
	{
	if (aDeviceUid == iDeviceUid)
		{
		if (iDeviceUid != KDeviceUidA2dpHeadset)
			{
			if(Handle() > 0)
			 	{
			 	iInputDevice.Close();
			 	}
			}
		if (iOpenDeviceStatus && *iOpenDeviceStatus == KRequestPending)
			{
			User::RequestComplete(iOpenDeviceStatus, KErrCancel);				
			}		
		}
	}
	
EXPORT_C void CRoutingSoundRecordDevice::CloseDevice(TUid /*aDeviceUid*/, TRequestStatus& aStatus)
	{
	aStatus = KRequestPending;//async not really necessary with RMdaDevSound since close is sync
	if (iInputDevice.Handle())
		{
		iInputDevice.Close();
		}
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
	}	

EXPORT_C TInt CRoutingSoundRecordDevice::GetSupportedSampleRates(RArray<TUint>& aSupportedDiscreteRates,
																RArray<TRange>& aSupportedRateRanges)
	{
	aSupportedDiscreteRates.Reset();
	aSupportedRateRanges.Reset();
	TInt err = KErrNone;
	if (Handle() > 0)
		{
		RMdaDevSound::TSoundFormatsSupportedBuf formatsSupported;
		iInputDevice.RecordFormatsSupported(formatsSupported);
		TRange range;
		range.iLow = formatsSupported().iMinRate;
		range.iHigh = formatsSupported().iMaxRate;
		err = aSupportedRateRanges.Append(range);
		if(!err)
			{
			aSupportedRateRanges.Reset();
			}
		}
	else
		{
		err = KErrNotReady;
		}

	return err;
	}
	
EXPORT_C TInt CRoutingSoundRecordDevice::GetSupportedChannels(	RArray<TUint>& aSupportedChannels,
																TMMFStereoSupport& aStereoSupport)
	{
	TInt err = KErrNone;
	if (Handle() > 0)
		{
		RMdaDevSound::TSoundFormatsSupportedBuf formatsSupported;
		iInputDevice.RecordFormatsSupported(formatsSupported);
		aSupportedChannels.Reset();
		err = aSupportedChannels.Append(formatsSupported().iChannels);
		
		if (err == KErrNone)
			{
			aStereoSupport = EMMFNone;	// assume no stereo support for starters

			if (formatsSupported().iChannels == 2)
				{
				aStereoSupport = EMMFBothNonAndInterleaved;		
				}
			}
		}
	else
		{
		err = KErrNotReady;
		}

	return err;
	}

EXPORT_C TInt CRoutingSoundRecordDevice::SetSampleRate(TUint aSampleRate)
	{
	// Assume we don't have a handle to the device
	TInt err = KErrNone;
	if (Handle())
		{
		RMdaDevSound::TCurrentSoundFormatBuf format;
		iInputDevice.GetRecordFormat(format);
		format().iRate = aSampleRate;
		err = iInputDevice.SetRecordFormat(format);		
		}
	iSampleRate = aSampleRate;
	return err;
	}

EXPORT_C TInt CRoutingSoundRecordDevice::SetChannels(TUint aChannels, TMMFStereoSupport aStereoSupport)
	{
	TInt err = KErrNone;
	
	if (Handle() > 0)
		{
		RMdaDevSound::TCurrentSoundFormatBuf format;
		iInputDevice.GetRecordFormat(format);
		format().iChannels = aChannels;
		err = iInputDevice.SetRecordFormat(format);			
		}
	iChannels = aChannels;
	iStereoSupport = aStereoSupport;	
	return err;
	}
	
EXPORT_C TInt CRoutingSoundRecordDevice::SetBufferSize(TUint aBufferSize)
	{
	TInt err = KErrNone;
	
	if (iDeviceUid != KDeviceUidA2dpHeadset)
		{
		if (Handle() > 0)
			{
			RMdaDevSound::TCurrentSoundFormatBuf format;
			iInputDevice.GetRecordFormat(format);
			// 1 = mono 2 = stereo
			format().iBufferSize = aBufferSize;			
			err = iInputDevice.SetRecordFormat(format);
			}
		}
	iBufferSize = aBufferSize;
	//else we don't need this for bt headset
	return err;
	}

EXPORT_C void CRoutingSoundRecordDevice::FlushBuffer()
	{
	if (Handle() > 0)
		{
		iInputDevice.FlushRecordBuffer();
		}
	}

EXPORT_C void CRoutingSoundRecordDevice::NotifyError(TRequestStatus& aStatus)
	{
	if (Handle() > 0)
		{
		iInputDevice.NotifyRecordError(aStatus);
		}
	}

EXPORT_C void CRoutingSoundRecordDevice::CancelNotifyError()
	{
	// To avoid a KE0 panic if -ve handle is returned
	if (Handle() > 0)
		{
		iInputDevice.CancelNotifyRecordError();
		}
	}

EXPORT_C TUint CRoutingSoundRecordDevice::Gain() const
	{
	TUint gain = 0;
	if (Handle() > 0)
		{
		RMdaDevSound* mutableInputDevice = const_cast<RMdaDevSound*>(&iInputDevice);
		gain = mutableInputDevice->RecordLevel();		
		}
	return gain;
	}
	
EXPORT_C TInt CRoutingSoundRecordDevice::SetGain(TUint aGain)
	{
	TInt err = KErrNone;
	if (Handle() > 0)
		{
		iInputDevice.SetRecordLevel(aGain);
		}
	else
		{
		err = KErrNotReady;
		}
	return err;
	}
	
EXPORT_C void CRoutingSoundRecordDevice::RecordData(TDes8& aData, TRequestStatus& aStatus)
	{
	//this handle check should really be removed since it impacts performance
	//, however there do seem to
	//be cases where a record error occus but the client sends an ack before
	//it has time to get the RecordError so keep it for now
	if (Handle() > 0)
		{
		iInputDevice.RecordData(aStatus, aData);
		}
	else
		{
		TRequestStatus* status = &aStatus;
		User::RequestComplete(status, KErrBadHandle);
		}
	}
	
EXPORT_C void CRoutingSoundRecordDevice::CancelRecordData()
	{
	if (Handle() > 0)
		{
		iInputDevice.CancelRecordData();
		}
	}
	
EXPORT_C TUint CRoutingSoundRecordDevice::BytesRecorded()
	{
	// Is there an equivalent call to bytes played for bytes recorded?
	// If not, why do we have this method or what should we return?
	TUint bytesPlayed = 0;
	if (Handle() > 0)
		{
		bytesPlayed = iInputDevice.BytesPlayed();
		}
	return bytesPlayed; 
	}

EXPORT_C TInt CRoutingSoundRecordDevice::Handle() const
	{
	return iInputDevice.Handle();
	}

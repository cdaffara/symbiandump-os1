// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @publishedPartner 
 @released
*/

#ifndef MMFHWDEVICESETUP_H
#define MMFHWDEVICESETUP_H

const TUid KUidHwDeviceSetupInterface = {0x102737A8};
class TFourCC;
	
//Custom Interface Uid for these Hw Device implementations
#define KMmfPlaySettingsCustomInterface		0x101F9FC1
#define KMmfRecordSettingsCustomInterface	0x101F9FC2
#define KSetVbrFlagCustomInterfaceTypeUid	0x10282647
const TUid KTimePlayedCustomInterfaceTypeUid = {0x10285D17};
const TUid KIgnoreUnderflowCustomInterfaceTypeUid = {0x10285E7D};
/**
Custom interface to pass the data types to a hardware device adapter
*/
class MMdfHwDeviceSetup
	{
public:
	/**
	Sets the data types into the hardware device adapter
	@param aSrcType Source FourCC data type
	@param aDestType Destination FourCC data type
	*/
	virtual void SetDataTypesL(TFourCC aSrcType, TFourCC aDestType) = 0;
	};
	
/**
Custom interface class for play functionality created by the
CMMFSwCodecWrapper::CustomInterface() method. It provides
access to miscellaneous functionality such as volume settings.
Ports of the sw codec wrapper may do this differently eg. via EAP.
Note Users should not use RMdaDevSound::TCurrentSoundFormatBuf to send
across the sample rate and channels because 1) it would mean
mmfswcodecwrapper.h would have to include mdasound.h 2)
the buffer size and encoding members are not required and 3) 3rd party ports
may not use RMdaDevSound.

Note also that this interface is just a simple interface to get
and set values. No checking is performed on the values sent (hence the
Set methods do not return an error code).
*/
class MPlayCustomInterface
	{
public:
	/**
	Set the audio volume.
	@param aVolume The specified audio volume.
	*/
	virtual void SetVolume(TUint aVolume) = 0;

	/**
	Get the audio volume.	
	*/
	virtual TUint Volume() = 0;

	/**
	Get the number of bytes played.	
	*/
	virtual TUint BytesPlayed() = 0;

	/**
	Set the period over which the volume level is to rise smoothly from nothing to the required volume level.
	@param aRampDuration The period over which the volume is to rise.
	*/
	virtual void SetVolumeRamp(const TTimeIntervalMicroSeconds& aRampDuration) = 0;

	/**
	Get the period over which the volume level is to rise smoothly from nothing to the required volume level.
	*/
	virtual TTimeIntervalMicroSeconds& VolumeRamp() = 0;
	};

/**
Custom interface class for record functionality created by the
CMMFSwCodecWrapper::CustomInterface() method. It provides
access to miscellaneous functionality such as volume settings.
Ports of the sw codec wrapper may do this differently eg. via EAP.
Note: Users should not use RMdaDevSound::TCurrentSoundFormatBuf so send
across the sample rate and channels because 1) it would mean
mmfswcodecwrapper.h would have to include mdasound.h 2)
the buffer size and encoding members are not required and 3) 3rd party ports
may not use RMdaDevSound.

Note also that this interface is just a simple interface to get
and set values. No checking is performed on the values sent (hence the
Set methods do not return an error code).
*/
class MRecordCustomInterface
	{
public:
	/**
	Set the gain.
	@param aGain The specified gain value.
	*/
	virtual void SetGain(TUint aGain) = 0;

	/**
	Get the gain value.
	*/
	virtual TUint Gain() = 0;

	/**
	Get the number of bytes recorded.
	*/
	virtual TUint BytesRecorded() = 0; 
	};
/**
Custom interface class for emptying the play buffers before playing.
*/
class MEmptyBuffersCustomInterface
	{
public:
	/**
	Empties the play buffers.
	@return An error code indicating if the function call was successful. KErrNone on success, KErrNotReady 
			if the device is not ready, otherwise another of the system-wide error codes.
	*/
	virtual TInt EmptyBuffers() = 0;
	};

/**
Custom interface class for setting VBR flag.
*/
class MSetVbrFlagCustomInterface
	{
public:
	/**
	Sets the VBR flag to ETrue.
	*/
	virtual void SetVbrFlag() = 0;
	};

/**
Custom interface class for querying current play time from the audio renderer.
*/
class MTimePlayedCustomInterface
	{
public:
	/**
	Gets the current play time from the sound driver.
	@param aTime A reference to TTimeIntervalMicroSeconds object which is filled with the current play time.
	@return An error code indicating if the function call was successful. KErrNone on success
	*/
	virtual TInt GetTimePlayed(TTimeIntervalMicroSeconds& aTime) = 0;
	};	

/**
Custom interface class for configuring hardware device to ignore all the underflow events from the sound driver 
except for the last buffer.
*/
class MIgnoreUnderflowEventsCustomInterface
	{
public:
	/**
	Configures the hardware device such that it ignores all the underflow events from the sound driver.
	However, it propagates the underflow error to devsound when the last buffer flag is set.
	*/
	virtual void IgnoreUnderflowEvents() = 0;
	};
#endif // MMFHWDEVICESETUP_H
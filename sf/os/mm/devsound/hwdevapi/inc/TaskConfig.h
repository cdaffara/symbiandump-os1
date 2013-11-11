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

/**
 @publishedPartner
 @released
*/

#ifndef __TASKCONFIG_H__
#define __TASKCONFIG_H__

/**
@publishedPartner
@released

TTaskSampleRate is a member of TTaskConfig which tells the HwDevice plugin the sample
rate of the audio data.
*/
enum TTaskSampleRate
	{
  	/** 8000 Hz Sampling Rate
	*/
	ETask8000Hz  = 0x00000001,
  	/** 11025 Hz Sampling Rate
	*/
	ETask11025Hz = 0x00000002,
  	/** 16000 Hz Sampling Rate
	*/
	ETask16000Hz = 0x00000004,
  	/** 22050 Hz Sampling Rate
	*/
	ETask22050Hz = 0x00000008,
  	/** 32000 Hz Sampling Rate
	*/
	ETask32000Hz = 0x00000010,
  	/** 44100 Hz Sampling Rate
	*/
	ETask44100Hz = 0x00000020,
  	/** 48000 Hz Sampling Rate
	*/
	ETask48000Hz = 0x00000040,
  	/** 88200 Hz Sampling Rate
	*/
	ETask88200Hz = 0x00000080,
  	/** 96000 Hz Sampling Rate
	*/
	ETask96000Hz = 0x00000100,
  	/** 12000 Hz Sampling Rate
	*/
	ETask12000Hz = 0x00000200,
  	/** 24000 Hz Sampling Rate
	*/
	ETask24000Hz = 0x00000400,
	/** 64000 Hz Sampling Rate
	*/
	ETask64000Hz = 0x00000800
	};

/**
@publishedPartner
@released

TTaskStereoMode is a member of TTaskConfig which tells the HwDevice plugin whether stereo audio
data is interleaved or not.
*/
enum TTaskStereoMode
	{
  	/** Audio data is mono.
	*/
	ETaskMono			= 0x00000001,
  	/** Audio data is interleaved.
	*/
	ETaskInterleaved	= 0x00000002,
  	/** Audio data is non-interleaved.
	*/
	ETaskNonInterleaved	= 0x00000004
	};

/**
@publishedPartner
@released

Used by the SetConfig() method to configure the sample rate and stereo mode of a CMMFHwDevice 
plugin. The configuration of HwDevices is device specific.
*/
class TTaskConfig
	{
	public:
		/** The rate of the audio data.

		@see TTaskSampleRate
	    */
		TInt iRate;
		/** The type of stereo audio data.

		@see TTaskStereoMode
		*/
		TInt iStereoMode;
		/** Uid for this TTaskConfig struct
		*/
		TInt iUid;
	};
	
/**
@publishedPartner
@released

Extended version of TTaskConfig used by the SetConfig() method to configure the sample rate, 
stereo mode and number of channels of a CMdfHwDevice plugin. The configuration of HwDevices 
is device specific.
*/	
class TTaskConfig2 : public TTaskConfig
	{
public:
	/** 
	The number of channels.
	*/
	TInt iNumberOfChannels;
	};	

#endif


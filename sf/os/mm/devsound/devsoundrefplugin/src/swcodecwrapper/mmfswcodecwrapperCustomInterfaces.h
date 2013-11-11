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

#ifndef __MMFSWCODECWRAPPERCUSTOMINTERFACES_H__
#define __MMFSWCODECWRAPPERCUSTOMINTERFACES_H__


/**
 * Implementation of custom interface class for play functionality created by the 
 * CMMFSwCodecWrapper::CustomInterface() method.  It provides 
 * access to miscellaneous functionality such as volume settings
 * Ports of sw codec wrapper may do this differently eg via EAP
 * note don't use RMdaDevSound::TCurrentSoundFormatBuf so send
 * across the sample rate and channels because 1)it would mean
 * mmfswcodecwrapper.h would have to include mdasound.h and 2)
 * the buffer size and encoding members are not required 3)3rd party ports
 * may not use RMdaDevSound
 * Note also that this interface is just a simple interface to get
 * and set values.  No checking is perfomed on the values sent (hence the
 * Set methods do not return an error code.)
 * @released
 */
class TPlayCustomInterface : public MPlayCustomInterface
	{
public:
	TPlayCustomInterface() : iVolume(0),iBytesPlayed(0),iDevice(NULL),iRampDuration(0) {}
	void SetVolume(TUint aVolume);
	TUint Volume() {return iVolume;};
	TUint BytesPlayed();
	void SetVolumeRamp(const TTimeIntervalMicroSeconds& aRampDuration) {iRampDuration = aRampDuration;};
	TTimeIntervalMicroSeconds& VolumeRamp() {return iRampDuration;};
	void SetDevice(RMdaDevSound* iDevice);//not for use outside codec wrapper
private:
	TUint iVolume;
	TUint iBytesPlayed;
	RMdaDevSound* iDevice;
	TTimeIntervalMicroSeconds iRampDuration;
	};

/**
 * Implementation of custom interface class for record functionality created by the 
 * CMMFSwCodecWrapper::CustomInterface() method.  It provides 
 * access to miscelaneous functionality such as volume settings
 * Ports of sw codec wrapper may do this differently eg via EAP
 * note don't use RMdaDevSound::TCurrentSoundFormatBuf so send
 * across the sample rate and channels because 1)it would mean
 * mmfswcodecwrapper.h would have to include mdasound.h and 2)
 * the buffer size and encoding members are not required 3)3rd party ports
 * may not use RMdaDevSound
 * Note also that this interface is just a simple interface to get
 * and set values.  No checking is perfomed on the values sent. (hence the
 * Set methods do not return an error code.)
 * @released
 */
class TRecordCustomInterface : public MRecordCustomInterface
	{
public:
	TRecordCustomInterface() : iGain(0), iBytesRecorded(0), iDataPath(NULL) {}
	void SetGain(TUint aGain) {iGain = aGain;};
	void SetDataPath(CMMFSwCodecRecordDataPath* aDataPath){iDataPath = aDataPath;};
	TUint Gain() {return iGain;};
	TUint BytesRecorded(); 
private:
	TUint iGain;
	TUint iBytesRecorded;
	CMMFSwCodecRecordDataPath* iDataPath;
	};


#endif


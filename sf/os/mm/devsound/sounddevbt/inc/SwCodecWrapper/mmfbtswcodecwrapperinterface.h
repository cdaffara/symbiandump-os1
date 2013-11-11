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

#ifndef __MMFBTSWCODECWRAPPERINTERFACE_H__
#define __MMFBTSWCODECWRAPPERINTERFACE_H__

#include <mmfbthwdevice2.h>

class CMMFSwCodecDataPath; //forward reference

class CRoutingSoundPlayDevice;
class CRoutingSoundRecordDevice;

/**
@publishedPartner
@prototype

Custom interface class for play functionality created by the
CMMFSwCodecWrapper::CustomInterface() method. It provides
access to miscellaneous functionality such as volume settings.
Ports of the sw codec wrapper may do this differently eg. via EAP.
Note Users should not use RMdaDevSound::TCurrentSoundFormatBuf to send
across the sample rate and channels because 1) it would mean
MmfBtSwCodecWrapper.h would have to include mdasound.h 2)
the buffer size and encoding members are not required and 3) 3rd party ports
may not use RMdaDevSound.

Note also that this interface is just a simple interface to get
and set values. No checking is perfomed on the values sent (hence the
Set methods do not return an error code).
*/
class MPlayCustomInterface
	{
public:
	virtual void SetVolume(TUint aVolume) = 0;
	virtual TUint Volume() = 0;
	virtual TUint BytesPlayed() = 0;
	virtual void SetVolumeRamp(const TTimeIntervalMicroSeconds& aRampDuration) = 0;
	virtual TTimeIntervalMicroSeconds& VolumeRamp() = 0;
	virtual TTaskConfig Caps() = 0;
	};


/**
@publishedPartner
@prototype

Custom interface class for record functionality created by the
CMMFSwCodecWrapper::CustomInterface() method. It provides
access to miscelaneous functionality such as volume settings.
Ports of the sw codec wrapper may do this differently eg. via EAP.
Note: Users shoule not use RMdaDevSound::TCurrentSoundFormatBuf so send
across the sample rate and channels because 1) it would mean
MmfBtSwCodecWrapper.h would have to include mdasound.h 2)
the buffer size and encoding members are not required and 3) 3rd party ports
may not use RMdaDevSound.

Note also that this interface is just a simple interface to get
and set values. No checking is perfomed on the values sent (hence the
Set methods do not return an error code).
*/
class MRecordCustomInterface
	{
public:
	virtual void SetGain(TUint aGain) = 0;
	virtual TUint Gain() = 0;
	virtual TUint BytesRecorded() = 0; 
	};

#endif //__MMFBTSWCODECWRAPPERINTERFACE_H__


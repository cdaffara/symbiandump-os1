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

#ifndef SOUNDDEVICEBODY_H
#define SOUNDDEVICEBODY_H

#include <e32base.h>
#include <mmf/server/sounddevice.h>

class MMMFDevSoundPlugin;

/**
@internalComponent

This is the Body for CMMFDevSound. For descriptions of methods, see 
MMFDevSound itself.
*/
NONSHARABLE_CLASS( CMMFDevSound::CBody ) : public CBase
	{
public:

	/**
	Factory function - returning instance of class.
	Leaves on failure.
	*/
	static CBody* NewL();

	/**
	Destructor.

	Deletes all objects and releases all resource owned by this
	instance.
	*/
	~CBody();

	// public functions forwarded from CMMFDevSound
    void InitializeL(MDevSoundObserver& aDevSoundObserver, TMMFState aMode);
	void InitializeL(MDevSoundObserver& aDevSoundObserver, TUid aHWDev, TMMFState aMode);
	void InitializeL(MDevSoundObserver& aDevSoundObserver, TFourCC aDesiredFourCC, TMMFState aMode);
	TMMFCapabilities Capabilities();
	TMMFCapabilities Config() const;
	void SetConfigL(const TMMFCapabilities& aCaps);
	TInt MaxVolume();
	TInt Volume();
	void SetVolume(TInt aVolume);
	TInt MaxGain();
	TInt Gain();
	void SetGain(TInt aGain);
	void GetPlayBalanceL(TInt& aLeftPercentage, TInt& aRightPercentage);
	void SetPlayBalanceL(TInt aLeftPercentage, TInt aRightPercentage);
	void GetRecordBalanceL(TInt& aLeftPercentage, TInt& aRightPercentage);
	void SetRecordBalanceL(TInt aLeftPercentage, TInt aRightPercentage);
	void PlayInitL();
	void RecordInitL();
	void PlayData();
	void RecordData();
	void Stop();
	void Pause();
	TInt SamplesRecorded();
	TInt SamplesPlayed();
	void PlayToneL(TInt aFrequency, const TTimeIntervalMicroSeconds& aDuration);
	void PlayDualToneL(TInt aFrequencyOne, TInt aFrequencyTwo, const TTimeIntervalMicroSeconds& aDuration);
	void PlayDTMFStringL(const TDesC& aDTMFString);
	void PlayToneSequenceL(const TDesC8& aData);
	void PlayFixedSequenceL(TInt aSequenceNumber);
	void SetToneRepeats(TInt aRepeatCount,
			const TTimeIntervalMicroSeconds& aRepeatTrailingSilence);
	void SetDTMFLengths(TTimeIntervalMicroSeconds32& aToneOnLength,
							TTimeIntervalMicroSeconds32& aToneOffLength,
							TTimeIntervalMicroSeconds32& aPauseLength);
	void SetVolumeRamp(const TTimeIntervalMicroSeconds& aRampDuration);
	void SetPrioritySettings(const TMMFPrioritySettings& aPrioritySettings);
	TAny* CustomInterface(TUid aInterfaceId);
	TInt FixedSequenceCount();
	const TDesC& FixedSequenceName(TInt aSequenceNumber);
	void GetSupportedInputDataTypesL(RArray<TFourCC>& aSupportedDataTypes, const TMMFPrioritySettings& aPrioritySettings) const;
	void GetSupportedOutputDataTypesL(RArray<TFourCC>& aSupportedDataTypes, const TMMFPrioritySettings& aPrioritySettings) const;
protected:

	CBody();
	void ConstructL();

	MMMFDevSoundPlugin* iPlugin;
	TUid iDestructorKey;
	};
	
#include "SoundDeviceBody.inl"

#endif // SOUNDDEVICEBODY_H

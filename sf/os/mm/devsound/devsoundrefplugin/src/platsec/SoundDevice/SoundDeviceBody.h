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

#ifndef __SOUNDDEVICEBODY__H__
#define __SOUNDDEVICEBODY__H__

#include "MmfDevSoundCallbackHandler.h" 
#include <mmf/plugin/devsoundplugin.h>
#include <mmf/server/mmfdevsoundcustominterfacesupport.h>
#include <mmf/server/mmfdevsoundcustominterface.h>
#include "MmfDevSoundCIMuxUtility.h"

 
/*
 *
 *	Class Name:	CMMFDevSoundClientImp
 *
 *	Reference internals for CMMFDevSound
 *  @internalComponent
 *
 */
class CMMFDevSoundClientImp : public CBase, 
							  public MMMFDevSoundPlugin,
							  public MAudioClientThreadInfo,
  							  public MAutoPauseResumeSupport,
							  public MMMFDevSoundCustomInterfaceChannel,
							  public MMMFDevSoundEmptyBuffers,
							  public MMMFDevSoundCustomInterfaceObserver,
							  public MMMFDevSoundTimePlayed

	{
public:
	static CMMFDevSoundClientImp* NewL();
	~CMMFDevSoundClientImp();

	// Initializes to raw audio data PCM16 and Sampling Rate of 8 KHz. 
	void InitializeL(MDevSoundObserver& aDevSoundObserver, TMMFState aMode);
	// Initializes with hardware device and mode (play, record, convert)
	void InitializeL(MDevSoundObserver& aDevSoundObserver, TUid aHWDev, TMMFState aMode);
	// Initializes with hardware device associated with specific FourCC code 
	// and desired mode (play, record, convert)
	void InitializeL(MDevSoundObserver& aDevSoundObserver, TFourCC aDesiredFourCC, TMMFState aMode); 

	// Returns device capabilities
	TMMFCapabilities Capabilities();
	// Returns device configuration
	TMMFCapabilities Config() const;
	// Sets device configuration
	void SetConfigL(const TMMFCapabilities& aCaps);

	// Returns maximum play volume device supports
	TInt MaxVolume();
	// Returns play volume device is configured to
	TInt Volume();
	// Sets device play volume
	void SetVolume(TInt aVolume);

	// Returns maximum record gain device supports
	TInt MaxGain();
	// Returns record gain device is configured to
	TInt Gain();
	// Sets device record gain
	void SetGain(TInt aGain);

	// Returns play balance
	void GetPlayBalanceL(TInt& aLeftPercentage, TInt& aRightPercentage);
	// Sets play balance
	void SetPlayBalanceL(TInt aLeftPercentage, TInt aRightPercentage);

	// Returns record balance
	void GetRecordBalanceL(TInt& aLeftPercentage, TInt& aRightPercentage);
	// Sets record balance
	void SetRecordBalanceL(TInt aLeftPercentage, TInt aRightPercentage);

	// Initializes audio device and start play process
	void PlayInitL();
	// Initializes audio device and start record process
	void RecordInitL();
	// Plays the data in the buffer
	void PlayData();
	// Records data into buffer
	void RecordData();
	// Stops the ongoing operation
	void Stop();
	// Tempororily Stops the ongoing operation
	void Pause();

	// Returns the Sample recorded so far
	TInt SamplesRecorded();
	// Returns the Sample played so far
	TInt SamplesPlayed();

	// Initializes device and start playing tone
	void PlayToneL(TInt aFrequency, const TTimeIntervalMicroSeconds& aDuration);
	// Initializes device and start playing dual tone
	void PlayDualToneL(TInt aFrequencyOne, TInt aFrequencyTwo, const TTimeIntervalMicroSeconds& aDuration);
	// Initializes device and start playing DTMF String
	void PlayDTMFStringL(const TDesC& aDTMFString);
	// Initializes device and start playing tone sequence
	void PlayToneSequenceL(const TDesC8& aData);
	// Initializes device and start playing fixed sequence
	void PlayFixedSequenceL(TInt aSequenceNumber);

	// Sets tone repeat attributes
	void SetToneRepeats(TInt aRepeatCount,
					const TTimeIntervalMicroSeconds& aRepeatTrailingSilence);
	// Sets DTMF tone attributes
	void SetDTMFLengths(TTimeIntervalMicroSeconds32& aToneOnLength,
									TTimeIntervalMicroSeconds32& aToneOffLength,
									TTimeIntervalMicroSeconds32& aPauseLength);
	// Sets volume ramp for playing
	void SetVolumeRamp(const TTimeIntervalMicroSeconds& aRampDuration);
	// Sets priority settings
	void SetPrioritySettings(const TMMFPrioritySettings& aPrioritySettings);

	// Provides interface for sending custom commands for the audio device
	TAny* CustomInterface(TUid aInterfaceId);

	// Provides interface for querying fixed sequence count
	TInt FixedSequenceCount();
	// Provides interface for querying fixed sequence name
	const TDesC& FixedSequenceName(TInt aSequenceNumber);
	// Gets the supported input datatypes
	void GetSupportedInputDataTypesL(RArray<TFourCC>& aSupportedDataTypesconst, const TMMFPrioritySettings& aPrioritySettings) const;
	// Gets the supported output datatypes
	void GetSupportedOutputDataTypesL(RArray<TFourCC>& aSupportedDataTypes, const TMMFPrioritySettings& aPrioritySettings) const;
	// Set the real client thread with thread Id
	TInt SetClientThreadInfo(TThreadId aTid);

	// Registers the client for notification
	TInt RegisterAsClient(TUid aEventType, const TDesC8& aNotificationRegistrationData = KNullDesC8);
	//cancels the registered notification
	TInt CancelRegisterAsClient(TUid aEventType);
	//get the notification data to resume 
	TInt GetResourceNotificationData(TUid aEventType,TDes8& aNotificationData);
	//if client need more than the default timeout period
	TInt WillResumePlay();
	
	// from MMMFDevSoundEmptyBuffers
	//Empties the buffers below DevSound without causing the codec to be deleted.
	TInt EmptyBuffers();

	// added to support custom command passing through DevSound
	TInt SyncCustomCommand(TUid aUid, const TDesC8& aParam1, const TDesC8& aParam2, TDes8* aOutParam);
	void AsyncCustomCommand(TUid aUid, TRequestStatus& aStatus, const TDesC8& aParam1, const TDesC8& aParam2, TDes8* aOutParam);

	//from MMMFDevSoundCustomInterfaceObserver
	void CloseCustomInterface(TInt aIndex);
	
	//from MMMFDevSoundTimePlayed
	TInt GetTimePlayed(TTimeIntervalMicroSeconds& aTime);
private:
	// So that nobody can extend 
	CMMFDevSoundClientImp();
	// Second phase constructor
	void ConstructL();

	TInt FindCustomInterface(TUid aInterfaceId);
private:
	// custom interface storage
	RArray<TMMFDevSoundCustomInterfaceData> iCustomInterfaceArray;
	CMMFDevSoundCIMuxUtility* iMuxUtility;
	
	RMMFDevSoundProxy* iDevSoundProxy;
	MDevSoundObserver* iDevSoundObserver;
	
	RMsgQueue<TMMFDevSoundQueueItem> iMsgQueue;
	CMsgQueueHandler* iMsgQueueHandler;
	};

#include "SoundDeviceBody.inl"

#endif // __SOUNDDEVICEBODY__H__

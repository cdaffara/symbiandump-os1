// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __MMFBTDEVSOUNDSESSIONBODY__H__
#define __MMFBTDEVSOUNDSESSIONBODY__H__

#include <e32base.h>
#include <mmfbthwdevice2.h>
#include "MmfBtDevSoundSession.h"
#include "MmfBtDevSoundSessionXtnd.h"
#include "BtToneGenerator.h"
#include "MmfBtAudioPolicyProxy.h"
#include "MmfBtDevSoundInfo.h"
#include "BtDevSoundUtility.h"
#include "mdasound.h"

//controls buffer sizes
const TInt KDevSoundDefaultFrameSize = 0x1000;
const TInt KDevSoundMinFrameSize = 0x800; //2K
const TInt KDevSoundMaxFrameSize = 0x4000;  //16K
const TInt KDevSoundDeltaFrameSize = 0x800; //2K
const TInt KDevSoundFramesPerSecond = 4;
const TUid KUidBTHeadsetRepository = {0x102070D7};


//Forward references:
class CDevSoundPlayer;
class CDevSoundError;
class CDevSoundRecorder;
class CDevSoundRecordError;
class CMMFDevSoundProxyXtnd;

class CMMFHwDevice;
class CMMFHwDeviceObserver;
class MPlayCustomInterface;
class MRecordCustomInterface;
class TBTDevAddr;

/*
 * AO to handle asynch RoutingSoundDevice Initialisation
 *
 */
class CRoutingSoundDeviceHandler : public CActive
	{
public:
	static CRoutingSoundDeviceHandler* NewL(MDevSoundObserver* aObserver);
	~CRoutingSoundDeviceHandler();
	void Start();
private:
	void RunL();
	void DoCancel();
	CRoutingSoundDeviceHandler(MDevSoundObserver* aObserver);
	void ConstructL();
private:
	MDevSoundObserver* iObserver;	// Not owned
	};

/*
 *
 *	Reference internals for CMMFDevSound
 *  @internalComponent
 *
 */
class CMMFDevSoundSvrImp;
NONSHARABLE_CLASS( CMMFDevSoundSvrImp ) : public CBase, public MMMFHwDeviceObserver
	{
public:
	static CMMFDevSoundSvrImp* NewL(CMMFDevSoundSessionXtnd* aParent);
	~CMMFDevSoundSvrImp();

	// Initializes to raw audio data PCM16 and Sampling Rate of 8 KHz. 
	void InitializeL(MDevSoundObserver& aDevSoundObserver, TMMFState aMode);
	// Initializes with hardware device and mode (play, record, convert)
	void InitializeL(MDevSoundObserver& aDevSoundObserver, TUid aHWDev, TMMFState aMode);
	// Initializes with chain of hardware devices and mode (play, record, convert)
	void InitializeL(MDevSoundObserver& aDevSoundObserver, CArrayPtr<TUid> aHWDevArray, TMMFState aMode);
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
	void RecordInitL(const RMmfIpcMessage& aMessage);
	// Plays the data in the buffer
	void PlayData();
	// Records data into buffer
	void RecordData(const RMmfIpcMessage& aMessage);
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

	// Provides interface for converting data in buffer
	void ConvertInitL();
	// Converts data in the buffer
	void ConvertData();

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

	// Interface for Audio Policy
	// Sets id for this DevSound instance
	void SetDevSoundId(TInt aDevSoundId);
	// Returns info about this DevSound instance
	TMMFDevSoundInfo DevSoundInfo();
	// Called by Audio Policy Server when play reqeust is granted
	void StartPlayDataL();
	// Called by Audio Policy Server when record reqeust is granted
	void StartRecordDataL();
	// Called by Audio Policy Server when play tone reqeust is granted
	void StartPlayToneL();
	// Called by Audio Policy Server when play dual tone request is granted
	void StartPlayDualToneL();
	// Called by Audio Policy Server when play DTMF reqeust is granted
	void StartPlayDTMFStringL();
	// Called by Audio Policy Server when play tone sequence reqeust is granted
	void StartPlayToneSequenceL();
	// Called by Audio Policy Server when DevSound instance looses device
	void SendEventToClient(const TMMFEvent& aEvent);
	// Updates the total bytes played from audio device
	void UpdateBytesPlayed();

	// CMMFHwDevice implementation:
	TInt FillThisHwBuffer(CMMFBuffer& aHwBuffer);
	TInt EmptyThisHwBuffer(CMMFBuffer& aHwBuffer);
	TInt MsgFromHwDevice(TUid aMessageType, const TDesC8& aMsg);
	void Stopped();
	void Error(TInt aError);
	// CMMFHwDevice implementation ends
	
	//provides interface for register a notification event
	TInt RegisterAsClient(TUid aEventType, const TDesC8& aNotificationRegistrationData = KNullDesC8);
	//provides interface to cancel the registered notification
	TInt CancelRegisterAsClient(TUid aEventType);
	//Gets the notification data for client to resume
	TInt GetResourceNotificationData(TUid aEventType, TDes8& aNotificationData);
	//waits for the client to resume playback even after the timeout expires
	TInt WillResumePlay();

private:
	// So that nobody can extend 
	CMMFDevSoundSvrImp(CMMFDevSoundSessionXtnd* aParent);
	// Second phase constructor
	void Construct3L(RServer2& aPolicyServerHandle);
	void PreInitializeL();
	// Initializes format based on format supported
	TInt InitializeFormat(RMdaDevSound::TSoundFormatsSupportedBuf& aSupportedFormat,
		RMdaDevSound::TCurrentSoundFormatBuf& aFormat);
	// Request Policy
	void RequestPolicy();
	// Play tone
	void DoPlayL();
	// Sets active tone
	void SetActiveToneBuffer();
	// Fills free tone buffer with tone data
	TInt FillFreeToneBuffer();
	// Updates the state of audio policy based on the audio policy settings
	TInt UpdatePolicyState();
	// HwDevice implementation begins
	TInt InitTask();
	// Returns Sampling Frequency the device is currently configured to
	TInt SamplingFrequency();
	// Returns number of channels the device is currently configured to
	TInt NumberOfChannels();
	//returns the number of bytes in each audio sample
	TInt BytesPerAudioSample();
	// HwDevice implementation ends
	TInt SetDeviceVolume(TInt aVolume);
	TInt SetPlayFormat(RMdaDevSound::TCurrentSoundFormatBuf& aPlayFormat);
	TInt SetRecordFormat(RMdaDevSound::TCurrentSoundFormatBuf& aRecordFormat);
	TInt SetDeviceRecordLevel(TInt aGain);
	TInt SetDeviceVolumeRamp(const TTimeIntervalMicroSeconds& aRampDuration);

private:

	CMMFDevSoundSessionXtnd& iParent;	

	//Objects (owned or referred to):
	RMMFAudioPolicyProxy*	iAudioPolicyProxy;
	CMMFAudioPolicyRequest*	iAudioPolicyRequest;
	MDevSoundObserver*		iDevSoundObserver;

	// Handles receiving events
	CMMFDevSoundEventHandler* iDevSoundEventHandler;

	//WINS Sound Device Structures
	RMdaDevSound::TSoundFormatsSupportedBuf iPlayFormatsSupported;
	RMdaDevSound::TCurrentSoundFormatBuf	iPlayFormat;
	RMdaDevSound::TSoundFormatsSupportedBuf iRecordFormatsSupported;
	RMdaDevSound::TCurrentSoundFormatBuf	iRecordFormat;

	// DevSound capabilities
	TMMFCapabilities		iDeviceCapabilities;
	// Current Configuration
	TMMFCapabilities		iDeviceConfig;

	// AudioPolicy priority used to communicate with server
	TMMFAudioPolicyPrioritySettings	iAudioPolicyPrioritySettings;

	MPlayCustomInterface* iPlayCustomInterface;
	MRecordCustomInterface* iRecordCustomInterface;

	// Bytes of audio data played in current play cycle
	TInt					iPlayedBytesCount;
	// Bytes of audio data recorded in current record cycle
	TInt					iRecordedBytesCount;
	TInt					iGain;
	TInt					iVolume;
	TMMFDevSoundInfo		iDevSoundInfo;

	//Tone Stuff:
	MMdaToneSynthesis*			iCurrentGenerator;
	TMdaSimpleToneGenerator		iToneGen;
	TMdaDualToneGenerator		iDualToneGen;
	TMdaDTMFGenerator			iDTMFGen;
	TMdaSequenceGenerator		iSequenceGen; // Not Supported
	TInt						iRepeatCount;
	TTimeIntervalMicroSeconds	iRepeatTrailingSilence;
	TTimeIntervalMicroSeconds	iRampDuration;
	// Double buffer tone playing
	CMMFDataBuffer*				iToneBuffer1;
	CMMFDataBuffer*				iToneBuffer2;
	// Reference to current tone buffer playing
	CMMFDataBuffer*				iActiveToneBuffer;

	// Hardware device implementation
	CMMFHwDevice2*				iCMMFHwDevice;

	// Mode in which DevSound instance will be used for
	TMMFState					iMode;
	// Reference to hardware buffer
	CMMFDataBuffer*				iHwDeviceBuffer;
	
	TBool						iLastBufferReceived;

	// Holds true of Audio data given to DevSound needs to ramped up
	CMMFDevSoundUtility*		iDevSoundUtil;

	// Fixed sequence related
	CPtrC8Array*				iFixedSequences;	// Holds the fixed sequence data
	TPtrC8						iFixedSequence;		// Holds reference to current sequence

	// Balance
	TInt						iLeftPlayBalance;
	TInt						iRightPlayBalance;

	TInt						iLeftRecordBalance;
	TInt						iRightRecordBalance;

	TBool						iPaused;

	TUid                        iHwDeviceID;

	// Hardware device initialization parameters
	THwDeviceInitParams			iDevInfo;

	TBool                      iFirstCallFromHwDevice;

	// AO to manage the initialisation of the RoutingSoundDevice
	CRoutingSoundDeviceHandler* iRSDHandler;
	
	friend class CMMFDevSoundSession;
	};

#include "MmfBtDevSoundSessionBody.inl"

#endif // __MMFBTDEVSOUNDSESSIONBODY__H__

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

#include <mmf/common/mmfpaniccodes.h>

#include "mmfclienttoneplayer.h"
using namespace ContentAccess;
enum TMmfMdaAudioToneUtility
	{
	EBadArgument,
	EPostConditionViolation, 
	EPlayStartedCalledWithError
	};

// declared in the recorder module
void Panic(TInt aPanicCode);

/**
Creates a new instance of the tone player utility.
The default  volume is set to MaxVolume() / 2.

@param  aObserver
        A class to receive notifications from the tone player.
@param  aServer
        This parameter is no longer used and should be NULL.

@return A pointer to the new audio tone player utility object.

@since 5.0
*/
EXPORT_C CMdaAudioToneUtility* CMdaAudioToneUtility::NewL(MMdaAudioToneObserver& aObserver, CMdaServer* aServer /*= NULL*/)
	{
	return CMdaAudioToneUtility::NewL(aObserver, aServer, EMdaPriorityNormal, EMdaPriorityPreferenceTimeAndQuality);
	}

/**
Creates a new instance of the tone player utility.
The default  volume is set to MaxVolume() / 2.

@param  aObserver
        A class to receive notifications from the tone player
@param  aServer
        This parameter is no longer used and should be NULL
@param  aPriority
        The Priority Value - this client's relative priority. This is a value between EMdaPriorityMin and 
        EMdaPriorityMax and represents a relative priority. A higher value indicates a more important request.
@param  aPref
        The Priority Preference - an additional audio policy parameter. The suggested default is 
        EMdaPriorityPreferenceNone. Further values are given by TMdaPriorityPreference, and additional 
        values may be supported by given phones and/or platforms, but should not be depended upon by 
        portable code.

@return A pointer to the new audio tone player utility object.

@since 5.0

Note: The Priority Value and Priority Preference are used primarily when deciding what to do when
several audio clients attempt to play or record simultaneously. In addition to the Priority Value and Preference, 
the adaptation may consider other parameters such as the SecureId and Capabilities of the client process. 
Whatever, the decision  as to what to do in such situations is up to the audio adaptation, and may
vary between different phones. Portable applications are advised not to assume any specific behaviour. 
*/
EXPORT_C CMdaAudioToneUtility* CMdaAudioToneUtility::NewL(MMdaAudioToneObserver& aObserver, CMdaServer* /*aServer = NULL*/,
														  TInt aPriority /*= EMdaPriorityNormal*/,
														  TInt aPref /*= EMdaPriorityPreferenceTimeAndQuality*/)
	{
	CMdaAudioToneUtility* self = new(ELeave) CMdaAudioToneUtility();
	CleanupStack::PushL(self);
	self->iProperties = CMMFMdaAudioToneUtility::NewL(aObserver, NULL, aPriority, aPref);
	CleanupStack::Pop(); //self
	return self;
	}

/**
Destructor. Frees any resources held by the tone player

@since 5.0
*/
CMdaAudioToneUtility::~CMdaAudioToneUtility()
	{
	delete iProperties;
	}

/**
Returns the current state of the audio tone utility.

@return The state of the audio tone utility.

@since  5.0
*/
TMdaAudioToneUtilityState CMdaAudioToneUtility::State()
	{
	ASSERT(iProperties);
	return iProperties->State();
	}
	
/**
Returns the maximum volume supported by the device. This is the maximum value which can be 
passed to CMdaAudioToneUtility::SetVolume().

@return The maximum volume. This value is platform dependent but is always greater than or equal to one.

@since  5.0
*/
TInt CMdaAudioToneUtility::MaxVolume()
	{
	ASSERT(iProperties);
	return iProperties->MaxVolume();
	}
	
/**
Returns an integer representing the current volume of the audio device.

@return The current volume.

@since 		5.0
*/
TInt CMdaAudioToneUtility::Volume()
	{
	ASSERT(iProperties);
	return iProperties->Volume();
	}
	
/**
Changes the volume of the audio device.

The volume can be changed before or during play and is effective
immediately.

@param  aVolume
        The volume setting. This can be any value from zero to
        the value returned by a call to
        CMdaAudioToneUtility::MaxVolume().
        Setting a zero value mutes the sound. Setting the
        maximum value results in the loudest possible sound.

@since  5.0
*/
void CMdaAudioToneUtility::SetVolume(TInt aVolume)
	{
	ASSERT(iProperties);
	iProperties->SetVolume(aVolume);
	}
	
/**
Changes the clients priority.

@param  aPriority
        The Priority Value.
@param  aPref
        The Priority Preference.

@see CMdaAudioToneUtility::NewL()

@since  5.0

*/
void CMdaAudioToneUtility::SetPriority(TInt aPriority, TInt aPref)
	{
	ASSERT(iProperties);
	iProperties->SetPriority(aPriority, aPref);
	}

/**
Changes the duration of DTMF tones, the gaps between DTMF tones and the
pauses.

@param  aToneLength
        The duration of the DTMF tone in microseconds.
@param  aToneOffLength
        The gap between DTFM tones in microseconds.
@param  aPauseLength
        Pauses in microseconds
*/
void CMdaAudioToneUtility::SetDTMFLengths(TTimeIntervalMicroSeconds32 aToneLength,
										  TTimeIntervalMicroSeconds32 aToneOffLength,
										  TTimeIntervalMicroSeconds32 aPauseLength)
	{
	ASSERT(iProperties);
	iProperties->SetDTMFLengths(aToneLength, aToneOffLength, aPauseLength);
	}

/**
Sets the number of times the tone sequence is to be repeated during
the play operation.

A period of silence can follow each playing of the tone sequence. The
tone sequence can be repeated indefinitely.

@param  aRepeatNumberOfTimes
        The number of times the tone sequence, together with
        the trailing silence, is to be repeated. If this is
        set to KMdaRepeatForever, then the tone
        sequence, together with the trailing silence, is
        repeated indefinitely. The behaviour is undefined for values other than  
		KMdaRepeatForever, zero and positive.
@param  aTrailingSilence
        The time interval of the training silence. The behaviour is undefined
        for values other than zero and positive.

@since  5.0
*/
void CMdaAudioToneUtility::SetRepeats(TInt aRepeatNumberOfTimes,
									  const TTimeIntervalMicroSeconds& aTrailingSilence)
	{
	ASSERT(iProperties);
	iProperties->SetRepeats(aRepeatNumberOfTimes, aTrailingSilence);
	}

/**
Defines the period over which the volume level is to rise smoothly
from nothing to the normal volume level.

@param  aRampDuration
        The period over which the volume is to rise. A zero
        value causes the tone to be played at the normal level
        for the full duration of the playback. A value which
        is longer than the duration of the tone sequence means
        that the tone never reaches its normal volume level.

@since  5.0
*/
void CMdaAudioToneUtility::SetVolumeRamp(const TTimeIntervalMicroSeconds& aRampDuration)
	{
	ASSERT(iProperties);
	iProperties->SetVolumeRamp(aRampDuration);
	}

/**
Returns the number of available pre-defined tone sequences.

@return The number of tone sequences. This value is implementation 
		dependent but is always greater than or equal to zero.

@since  5.0
*/
TInt CMdaAudioToneUtility::FixedSequenceCount()
	{
	ASSERT(iProperties);
	return iProperties->FixedSequenceCount();
	}

/**
Returns the name assigned to a specific pre-defined tone sequence.

@param  aSequenceNumber
        The index identifying the specific pre-defined tone sequence. 
        Index values are relative to zero. This can be any value from 
        zero to the value returned by a call to FixedSequenceCount() - 1.
        The function raises a panic if sequence number is not within this
 		range.

@see CMMFDevSound::FixedSequenceName(TInt aSequenceNumber)
@see FixedSequenceCount()

@return The name assigned to the tone sequence.

@since  5.0
*/
const TDesC& CMdaAudioToneUtility::FixedSequenceName(TInt aSequenceNumber)
	{
	ASSERT(iProperties);
	return iProperties->FixedSequenceName(aSequenceNumber);
	}

/**
Configures the audio tone player utility to play a single tone.

This function is asynchronous. On completion, the observer callback
function MMdaAudioToneObserver::MatoPrepareComplete() is
called, indicating the success or failure of the configuration
operation.The configuration operation can be cancelled by calling
CMdaAudioToneUtility::CancelPrepare(). The configuration
operation cannot be started if a play operation is in progress.

@param     aFrequency
           The frequency (pitch) of the tone in Hz.
@param     aDuration
           The duration of the tone in microseconds.
@since     5.0
*/
void CMdaAudioToneUtility::PrepareToPlayTone(TInt aFrequency, const TTimeIntervalMicroSeconds& aDuration)
	{
	ASSERT(iProperties);
	iProperties->PrepareToPlayTone(aFrequency, aDuration);
	}

/**
Configures the audio tone player utility to play a dual tone.
The generated tone consists of two sine waves of different
frequencies summed together.

This function is asynchronous. On completion, the observer callback
function MMdaAudioToneObserver::MatoPrepareComplete() is
called, indicating the success or failure of the configuration
operation. The configuration operation can be cancelled by calling
CMdaAudioToneUtility::CancelPrepare(). The configuration
operation cannot be started if a play operation is in progress.

@param  aFrequencyOne
        The first frequency (pitch) of the tone.
@param  aFrequencyTwo
        The second frequency (pitch) of the tone.
@param  aDuration
        The duration of the tone in microseconds.

@since  7.0sy
*/
EXPORT_C void CMdaAudioToneUtility::PrepareToPlayDualTone(TInt aFrequencyOne, TInt aFrequencyTwo, const TTimeIntervalMicroSeconds& aDuration)
	{
	ASSERT(iProperties);
	iProperties->PrepareToPlayDualTone(aFrequencyOne, aFrequencyTwo, aDuration);
	}

/**
Configures the audio tone utility player to play a DTMF (Dual-Tone
Multi-Frequency) string.

This function is asynchronous. On completion, the observer callback
function MMdaAudioToneObserver::MatoPrepareComplete() is
called, indicating the success or failure of the configuration
operation. The configuration operation can be cancelled by calling
CMdaAudioToneUtility::CancelPrepare(). The configuration
operation cannot be started if a play operation is in progress.

@param  aDTMF
        A descriptor containing the DTMF string.

@since  5.0
*/
void CMdaAudioToneUtility::PrepareToPlayDTMFString(const TDesC& aDTMF)
	{
	ASSERT(iProperties);
	iProperties->PrepareToPlayDTMFString(aDTMF);
	}

/**
Configures the audio tone player utility to play a tone sequence
contained in a descriptor.

This function is asynchronous. On completion, the observer callback
function MMdaAudioToneObserver::MatoPrepareComplete() is
called, indicating the success or failure of the configuration
operation. The configuration operation can be cancelled by calling
CMdaAudioToneUtility::CancelPrepare(). The configuration
operation cannot be started if a play operation is in progress.

@param  aSequence
        The descriptor containing the tone sequence. The
        format of the data is unspecified but is expected to
        be platform dependent. A device might support more
        than one form of sequence data.

@since  5.0
*/
void CMdaAudioToneUtility::PrepareToPlayDesSequence(const TDesC8& aSequence)
	{
	ASSERT(iProperties);
	iProperties->PrepareToPlayDesSequence(aSequence);
	}

/**
Configures the audio tone player utility to play a tone sequence
contained in a file.

This function is asynchronous. On completion, the observer callback
function MMdaAudioToneObserver::MatoPrepareComplete() is
called, indicating the success or failure of the configuration
operation. The configuration operation can be cancelled by calling
CMdaAudioToneUtility::CancelPrepare(). The configuration
operation cannot be started if a play operation is in progress.

@param  aFileName
        The full path name of the file containing the tone
        sequence. The format of the data is unspecified but is
        expected to be platform dependent. A device might
        support more than one form of sequence data.

@since  5.0
*/
void CMdaAudioToneUtility::PrepareToPlayFileSequence(const TDesC& aFileName)
	{
	ASSERT(iProperties);
	iProperties->PrepareToPlayFileSequence(aFileName);
	}
	
/**
Configures the audio tone player utility to play a tone sequence
contained in a file.

This function is asynchronous. On completion, the observer callback
function MMdaAudioToneObserver::MatoPrepareComplete() is
called, indicating the success or failure of the configuration
operation. The configuration operation can be cancelled by calling
CMdaAudioToneUtility::CancelPrepare(). The configuration
operation cannot be started if a play operation is in progress.

@param  aFile
        A handle to an open file containing the tone
        sequence. The format of the data is unspecified but is
        expected to be platform dependent. A device might
        support more than one form of sequence data.

@since  5.0
*/
EXPORT_C void CMdaAudioToneUtility::PrepareToPlayFileSequence(RFile& aFile)
	{
	ASSERT(iProperties);
	iProperties->PrepareToPlayFileSequence(aFile);
	}
	

/**
Configures the audio tone player utility to play the specified
pre-defined tone sequence.

This function is asynchronous. On completion, the observer callback
function MMdaAudioToneObserver::MatoPrepareComplete() is
called, indicating the success or failure of the configuration
operation. The configuration operation can be cancelled by calling
CMdaAudioToneUtility::CancelPrepare(). The configuration
operation cannot be started if a play operation is in progress.

@param  aSequenceNumber
        An index into the set of pre-defined tone sequences.
        This can be any value from zero to the value returned by a 
        call to FixedSequenceCount() - 1.
        If the sequence number is not within this range, a panic will be 
        raised when Play() is called later.

@see FixedSequenceCount()
@see CMMFDevSound::PlayFixedSequenceL(TInt aSequenceNumber)

@since  5.0
*/
void CMdaAudioToneUtility::PrepareToPlayFixedSequence(TInt aSequenceNumber)
	{
	ASSERT(iProperties);
	iProperties->PrepareToPlayFixedSequence(aSequenceNumber);
	}

/**
Cancels the configuration operation.

The observer callback function
MMdaAudioToneObserver::MatoPrepareComplete() is not
called.

@since  5.0
*/
void CMdaAudioToneUtility::CancelPrepare()
	{
	ASSERT(iProperties);
	iProperties->CancelPrepare();
	}

/**
Plays the tone.

The tone played depends on the current configuration.This function is
asynchronous. On completion, the observer callback function
MMdaAudioToneObserver::MatoPlayComplete() is called,
indicating the success or failure of the play operation.The play
operation can be cancelled by
calling CMdaAudioToneUtility::CancelPlay().

@since  5.0
*/
void CMdaAudioToneUtility::Play()
	{
	ASSERT(iProperties);
	iProperties->Play();
	}

EXPORT_C TInt CMdaAudioToneUtility::Pause()
	{
	ASSERT(iProperties);
	return iProperties->Pause();
	}

EXPORT_C TInt CMdaAudioToneUtility::Resume()
	{
	ASSERT(iProperties);
	return iProperties->Resume();
	}

/**
Cancels the tone playing operation.

The observer callback
function MMdaAudioToneObserver::MatoPlayComplete() is not
called.

@since  5.0
*/
void CMdaAudioToneUtility::CancelPlay()
	{
	ASSERT(iProperties);
	iProperties->CancelPlay();
	}

/**
Sets the stereo balance for playback.

@param 	aBalance
        The balance. Should be between KMMFBalanceMaxLeft and KMMFBalanceMaxRight.

@return An error code indicating if the function call was successful. KErrNone on success, otherwise
        another of the system-wide error codes.

@since 7.0s
*/
EXPORT_C void CMdaAudioToneUtility::SetBalanceL(TInt aBalance /*=KMMFBalanceCenter*/)
	{
	ASSERT(iProperties);
	iProperties->SetBalanceL(aBalance);
	}

/**
 *	Returns The current playback balance.This function may not return the same value 
 *			as passed to SetBalanceL depending on the internal implementation in 
 *			the underlying components.
 *
 *	@return The balance. Should be between KMMFBalanceMaxLeft and KMMFBalanceMaxRight.
 *		
 *  @since 	7.0s
 */
EXPORT_C TInt CMdaAudioToneUtility::GetBalanceL()
	{
	ASSERT(iProperties);
	return iProperties->GetBalanceL();
	}
	
/**
Retrieves a custom interface to the underlying device.

@param  aInterfaceId
        The interface UID, defined with the custom interface.

@return A pointer to the interface implementation, or NULL if the device does not
        implement the interface requested. The return value must be cast to the
        correct type by the user.
*/
EXPORT_C TAny* CMdaAudioToneUtility::CustomInterface(TUid aInterfaceId)
	{
	ASSERT(iProperties);
	return iProperties->CustomInterface(aInterfaceId);
	}

EXPORT_C void CMdaAudioToneUtility::RegisterPlayStartCallback(MMdaAudioTonePlayStartObserver& aObserver)
	{
	ASSERT(iProperties);
	iProperties->RegisterPlayStartCallback(aObserver);
	}



CMMFMdaAudioToneUtility* CMMFMdaAudioToneUtility::NewL(MMdaAudioToneObserver& aObserver, CMdaServer* /*aServer = NULL*/,
														  TInt aPriority /*= EMdaPriorityNormal*/, 
														  TInt aPref /*= EMdaPriorityPreferenceTimeAndQuality*/)
														  
	{
	CMMFMdaAudioToneUtility* self = new(ELeave) CMMFMdaAudioToneUtility(aObserver, aPriority, aPref);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}



CMMFMdaAudioToneUtility::CMMFMdaAudioToneUtility(MMdaAudioToneObserver& aCallback, TInt aPriority, TInt aPref) :
	iCallback(aCallback)
	{
	iPrioritySettings.iPref = aPref;
	iPrioritySettings.iPriority = aPriority;
	iState = EMdaAudioToneUtilityNotReady;
	iInitialized = EFalse;
	iPlayCalled = EFalse;

#ifdef _DEBUG
	iPlayCalledBeforeInitialized = EFalse;
#endif
	}

void CMMFMdaAudioToneUtility::ConstructL()
	{
	iAsyncCallback = CMMFMdaAudioToneObserverCallback::NewL(*this, *this);

	iDevSound = CMMFDevSound::NewL();
	iDevSound->InitializeL(*this,EMMFStateTonePlaying);
	
	// In some implementations InitializeComplete() returns in the InitializeL() context,
	// check the error
	User::LeaveIfError(iInitializeState);

	iDevSound->SetPrioritySettings(iPrioritySettings);
	SetVolume(MaxVolume()/2 ); // set the volume to an intermediate value 
	}

CMMFMdaAudioToneUtility::~CMMFMdaAudioToneUtility()
	{
	delete iDevSound;
	delete iAsyncCallback;
	delete iToneConfig;
	}



void CMMFMdaAudioToneUtility::InitializeComplete(TInt aError)
	{
#ifdef _DEBUG
	__ASSERT_ALWAYS(!iPlayCalledBeforeInitialized, User::Panic(_L("PlayInitialized called before InitializeComplete"), 0));
#endif
	iInitialized = ETrue;

	if (iPlayCalled)
		{
		// Play() is called before InitializeComplete()
		if (aError == KErrNone)
			{
			PlayAfterInitialized();
 			}
 		else 
 			{
 			// InitializeComplete() with error other than KErrNone
			iState = EMdaAudioToneUtilityNotReady;
			iAsyncCallback->MatoPlayComplete(aError);
 			}
 		iPlayCalled = EFalse;
		}
 	iInitializeState = aError;
	}

void CMMFMdaAudioToneUtility::ToneFinished(TInt aError)
	{
	if (aError != KErrCancel)
		{
		if (aError == KErrUnderflow)
			{
			aError = KErrNone;
			}

		iAsyncCallback->MatoPlayComplete(aError);
		}
	// else don't want to callback after a cancel
	}


TMdaAudioToneUtilityState CMMFMdaAudioToneUtility::State()
	{
	return iState;
	}

TInt CMMFMdaAudioToneUtility::MaxVolume()
	{
	return iDevSound->MaxVolume();
	}

TInt CMMFMdaAudioToneUtility::Volume()
	{
	return iDevSound->Volume();
	}

void CMMFMdaAudioToneUtility::SetVolume(TInt aVolume) 
	{
	iDevSound->SetVolume(aVolume);
	}

void CMMFMdaAudioToneUtility::SetPriority(TInt aPriority, TInt aPref)
	{
	iPrioritySettings.iPref = aPref;
	iPrioritySettings.iPriority = aPriority;
	iDevSound->SetPrioritySettings(iPrioritySettings);
	}

void CMMFMdaAudioToneUtility::SetDTMFLengths(TTimeIntervalMicroSeconds32 aToneLength, 
										 TTimeIntervalMicroSeconds32 aToneOffLength,
										 TTimeIntervalMicroSeconds32 aPauseLength)
	{
	iDevSound->SetDTMFLengths(aToneLength, aToneOffLength, aPauseLength);
	}

void CMMFMdaAudioToneUtility::SetRepeats(TInt aRepeatNumberOfTimes, const TTimeIntervalMicroSeconds& aTrailingSilence)
	{
	iDevSound->SetToneRepeats(aRepeatNumberOfTimes, aTrailingSilence);
	}

void CMMFMdaAudioToneUtility::SetVolumeRamp(const TTimeIntervalMicroSeconds& aRampDuration)
	{
	iDevSound->SetVolumeRamp(aRampDuration);
	}

TInt CMMFMdaAudioToneUtility::FixedSequenceCount()
	{
	return iDevSound->FixedSequenceCount();
	}

const TDesC& CMMFMdaAudioToneUtility::FixedSequenceName(TInt aSequenceNumber)
	{
	return iDevSound->FixedSequenceName(aSequenceNumber);
	}

/**
* CalculateBalance
* @param aBalance
* @param aLeft
* @param aRight
*
* follows a simple straight line transformation
* y = m x + c
* m = (KMMFBalanceMaxLeft-KMMFBalanceMaxRight)/ 100 
* c = KMMFBalanceMaxRight
* by substitution
* when aLeft = 0
*   KMMFBalanceMaxRight = m * 0 + c
*   c = KMMFBalanceMaxRight
* when aLeft = 100
* KMMFBalanceMaxLeft = m * 100 + KMMFBalanceMaxRight
* m = ( KMMFBalanceMaxLeft - KMMFBalanceMaxRight ) /100
*/
void CMMFMdaAudioToneUtility::CalculateBalance( TInt& aBalance, TInt aLeft, TInt aRight ) const
	{
	//[ assert pre conditions ]
	__ASSERT_ALWAYS( (( aLeft + aRight ) == 100 ), Panic( EBadArgument ));
	__ASSERT_ALWAYS( (( 0 <= aLeft) && ( 100 >= aLeft)), Panic( EBadArgument) );
	__ASSERT_ALWAYS( (( 0 <= aRight) && ( 100 >= aRight)), Panic( EBadArgument) );

	aBalance = (aLeft * (KMMFBalanceMaxLeft-KMMFBalanceMaxRight))/100 + KMMFBalanceMaxRight;

    //[ assert post condition that aBalance is within limits ]
	__ASSERT_ALWAYS( !(aBalance < KMMFBalanceMaxLeft || aBalance > KMMFBalanceMaxRight), Panic(EBadArgument));
	
	}


/**
* CalculateLeftRightBalance
* @param aLeft
* @param aRight
* @param aBalance
* Preconditions:
* !(aBalance < KMMFBalanceMaxLeft || aBalance > KMMFBalanceMaxRight)
* y = m x + c
* aLeft = m ( aBalance ) + c
* when aBalance = KMMFBalanceMaxLeft   aLeft = 100
* when aBalance = KMMFBalanceMaxRight  aLeft = 0
* 100 = m( KMMFBalanceMaxLeft ) + c
* 0   = m( KMMFBalanceMaxRight ) + c 
* c = -(KMMFBalanceMaxRight) m
* 100 = m(KMMFBalanceMaxLeft ) - m(KMMFBalanceMaxRight)
* m = 100/(KMMFBalanceMaxLeft - KMMFBalanceMaxRight )
* c = -(KMMFBalanceMaxRight) * 100 /(KMMFBalanceMaxLeft - KMMFBalanceMaxRight )
* aLeft = ( aBalance - KMMFBalanceMaxRight ) * 100 /( KMMFBalanceMaxLeft - KMMFBalanceMaxRight )
*/
void CMMFMdaAudioToneUtility::CalculateLeftRightBalance( TInt& aLeft, TInt& aRight, TInt aBalance ) const
	{
	// [ assert precondition that aBalance is within limits ]
    __ASSERT_ALWAYS( !(aBalance < KMMFBalanceMaxLeft || aBalance > KMMFBalanceMaxRight), Panic(EBadArgument));
	
	//[ Now separate percentage balances out from aBalance ]
	 aLeft = (100 * (aBalance-KMMFBalanceMaxRight)) / (KMMFBalanceMaxLeft-KMMFBalanceMaxRight);
     aRight = 100 - aLeft;

	 //[ assert post condition that left and right are within range ]
	 __ASSERT_ALWAYS( ( (aLeft <= 100) && (aLeft >= 0) ), Panic(EPostConditionViolation));
	 __ASSERT_ALWAYS( ( (aRight <= 100) && (aRight >= 0) ), Panic(EPostConditionViolation));
	}


void CMMFMdaAudioToneUtility::SetBalanceL(TInt aBalance) 
	{
	TInt left;
	TInt right;
	CalculateLeftRightBalance(left,right,aBalance);
	iDevSound->SetPlayBalanceL(left,right);
	}

TInt CMMFMdaAudioToneUtility::GetBalanceL() 
	{
	TInt left;
	TInt right;
	TInt balance;
	iDevSound->GetPlayBalanceL(left, right);
	CalculateBalance(balance,left,right);
	return balance; 
	}

void CMMFMdaAudioToneUtility::PrepareToPlayTone(TInt aFrequency, const TTimeIntervalMicroSeconds& aDuration)
	{
	delete iToneConfig;
	iToneConfig = NULL;
	TRAPD(error, iToneConfig = CMMFSimpleToneConfig::NewL(aFrequency, aDuration));
	iAsyncCallback->MatoPrepareComplete(error);
	}

void CMMFMdaAudioToneUtility::PrepareToPlayDualTone(TInt aFrequencyOne, TInt aFrequencyTwo, const TTimeIntervalMicroSeconds& aDuration)
	{
	delete iToneConfig; 
	iToneConfig = NULL;
	TRAPD(error, iToneConfig = CMMFDualToneConfig::NewL(aFrequencyOne, aFrequencyTwo, aDuration));
	iAsyncCallback->MatoPrepareComplete(error);
	}

void CMMFMdaAudioToneUtility::PrepareToPlayDTMFString(const TDesC& aDTMF)
	{
	delete iToneConfig;
	iToneConfig = NULL;
	TRAPD(error, iToneConfig = CMMFDTMFStringToneConfig::NewL(aDTMF));
	iAsyncCallback->MatoPrepareComplete(error);
	}

void CMMFMdaAudioToneUtility::PrepareToPlayDesSequence(const TDesC8& aSequence)
	{
	delete iToneConfig;
	iToneConfig = NULL;
	TRAPD(error, iToneConfig = CMMFDesSeqToneConfig::NewL(aSequence));
	iAsyncCallback->MatoPrepareComplete(error);
	}

void CMMFMdaAudioToneUtility::PrepareToPlayFileSequence(const TDesC& aFileName)
	{
	delete iToneConfig;
	iToneConfig = NULL;
	TRAPD(error, iToneConfig = CMMFFileSeqToneConfig::NewL(aFileName));
	iAsyncCallback->MatoPrepareComplete(error);
	}
	
void CMMFMdaAudioToneUtility::PrepareToPlayFileSequence(RFile& aFileName)
	{
	delete iToneConfig;
	iToneConfig = NULL;
	TRAPD(error, iToneConfig = CMMFFileSeqToneConfig::NewL(aFileName));
	iAsyncCallback->MatoPrepareComplete(error);
	}




void CMMFMdaAudioToneUtility::PrepareToPlayFixedSequence(TInt aSequenceNumber)
	{
	delete iToneConfig;
	iToneConfig = NULL;
	TRAPD(error, iToneConfig = CMMFFixedSeqToneConfig::NewL(aSequenceNumber));
	iSequenceNumber = aSequenceNumber;
	iAsyncCallback->MatoPrepareComplete(error);
	}

void CMMFMdaAudioToneUtility::CancelPrepare()
	{
	// xxx - do we need to cancel the callback?  What if the callback is actually calling back another error?  Probably best not to cancel...
	delete iToneConfig;
	iToneConfig = NULL;

	if (iState == EMdaAudioToneUtilityPrepared)
		{
		iState = EMdaAudioToneUtilityNotReady;
		}
	// Cancel the AO
	iAsyncCallback->Cancel();
	}

TInt CMMFMdaAudioToneUtility::Pause()
	{
	// Handle scenario when Pause is called before playback has started
	if (iState != EMdaAudioToneUtilityPlaying || (iState == EMdaAudioToneUtilityPlaying && !iInitialized))
		{
		return KErrNotReady;
		}

	else if(! iDevSound->IsResumeSupported() || iToneConfig->Type() != CMMFToneConfig::EMmfToneTypeFileSeq)
		{
		return KErrNotSupported;
		}
		
	iDevSound->Pause();
	iState = EMdaAudioToneUtilityPaused;
	return KErrNone;
	}

TInt CMMFMdaAudioToneUtility::Resume()
	{
	TInt err = KErrNone;
	if (iState != EMdaAudioToneUtilityPaused)
		{
		err = KErrNotReady;
		}

	else if( iDevSound->IsResumeSupported() == EFalse || iToneConfig->Type() != CMMFToneConfig::EMmfToneTypeFileSeq)
		{
		err = KErrNotSupported;
		}
		
	if(err == KErrNone)
		{
		err =  iDevSound->Resume();
		if(err == KErrNone)
			{
			iState = EMdaAudioToneUtilityPlaying;
			}
		}
	return err;
	}

void CMMFMdaAudioToneUtility::Play()
	{
	TInt error = KErrNone;

	if ((iState == EMdaAudioToneUtilityPlaying) || (iState == EMdaAudioToneUtilityPaused) || iPlayCalled)
		{
		error = KErrInUse;
		}
			
	if (!error)
		{
		if (!iToneConfig)
			{
			TRAP(error, iToneConfig = CMMFFixedSeqToneConfig::NewL(iSequenceNumber));
			}
		}
	// If there was an error, notify the client now.  Otherwise, client will be notified when
	// play has finished.
	if (error)
		{
		iState = EMdaAudioToneUtilityNotReady;
		iAsyncCallback->MatoPlayComplete(error);
		}
			
	if (!error)
		{
		iState = EMdaAudioToneUtilityPlaying;

		if (iInitialized)
			{
			// Play() is called after InitializeComplete()
			if (iInitializeState)
				{
				// InitializeComplete() with error other than KErrNone
				iState = EMdaAudioToneUtilityNotReady;
				iAsyncCallback->MatoPlayComplete(iInitializeState);
				}
			else
				{
				PlayAfterInitialized();
				}
			}
		else
			{
			// Play() is called before InitializeComplete()
			iPlayCalled = ETrue;
			}
		}
	}

void CMMFMdaAudioToneUtility::PlayAfterInitialized()
	{
#ifdef _DEBUG
	if (iInitialized == EFalse)
		{
		iPlayCalledBeforeInitialized = ETrue;
		}
#endif
	
	TInt error = KErrNone;
	switch (iToneConfig->Type())
		{
		case CMMFToneConfig::EMmfToneTypeSimple:
			{
			CMMFSimpleToneConfig* c = STATIC_CAST(CMMFSimpleToneConfig*, iToneConfig);
			TRAP(error, iDevSound->PlayToneL(c->Frequency(), c->Duration()));
			break;
			}
		case CMMFToneConfig::EMmfToneTypeDual:
			{
			CMMFDualToneConfig* c = STATIC_CAST(CMMFDualToneConfig*, iToneConfig);
			TRAP(error, iDevSound->PlayDualToneL(c->FrequencyOne(), c->FrequencyTwo(), c->Duration()));
			break;
			}
		case CMMFToneConfig::EMmfToneTypeDTMF:
			{
			CMMFDTMFStringToneConfig* c = STATIC_CAST(CMMFDTMFStringToneConfig*, iToneConfig);
			TRAP(error, iDevSound->PlayDTMFStringL(c->DTMF()));
			break;
			}
		case CMMFToneConfig::EMmfToneTypeDesSeq:
			{
			CMMFDesSeqToneConfig* c = STATIC_CAST(CMMFDesSeqToneConfig*, iToneConfig);
			TRAP(error, iDevSound->PlayToneSequenceL(c->DesSeq()));
			break;
			}
		case CMMFToneConfig::EMmfToneTypeFileSeq:
			{
			CMMFFileSeqToneConfig* c = STATIC_CAST(CMMFFileSeqToneConfig*, iToneConfig);

			// check we have rights to play
			TRAP(error, c->ExecuteIntentL());

			// if we have rights then go ahead and play
			if (error == KErrNone)
				{
				TRAP(error, iDevSound->PlayToneSequenceL(c->FileSeq()));
				}

			break;
			}
		case CMMFToneConfig::EMmfToneTypeFixedSeq:
			{
			CMMFFixedSeqToneConfig* c = STATIC_CAST(CMMFFixedSeqToneConfig*, iToneConfig);
			TRAP(error, iDevSound->PlayFixedSequenceL(c->SequenceNumber()));
			break;
			}
		default:
			{	
			User::Panic(KMMFMdaAudioToneUtilityPanicCategory, EMMFMdaAudioToneUtilityBadToneConfig);
			break;
			}
		}

	// If there was an error, notify the client now.  Otherwise, client will be notified when
	// play has finished.
	if (error)
		{
		iState = EMdaAudioToneUtilityNotReady;
		iAsyncCallback->MatoPlayComplete(error);
		}
	else
		{
        if(iPlayStartObserver)
            {
            iAsyncCallback->MatoPlayStarted(KErrNone);
            }
		}
	}
	
void CMMFMdaAudioToneUtility::CancelPlay()
	{
	iDevSound->Stop();

	if(iState == EMdaAudioToneUtilityPlaying || iState == EMdaAudioToneUtilityPaused)
		{
		iState = EMdaAudioToneUtilityPrepared;
		}
	// Cancel the AO
	iAsyncCallback->Cancel();
	iPlayCalled = EFalse;
	}
	

void CMMFMdaAudioToneUtility::SendEventToClient(const TMMFEvent& /*aEvent*/)
	{
	if(iState == EMdaAudioToneUtilityPlaying)
		{
		iState = EMdaAudioToneUtilityPrepared;
		}

	iAsyncCallback->MatoPlayComplete(KErrInUse);
	}


void CMMFMdaAudioToneUtility::RegisterPlayStartCallback(MMdaAudioTonePlayStartObserver& aObserver)
	{
	iPlayStartObserver = &aObserver;
	}

void CMMFMdaAudioToneUtility::MatoPrepareComplete(TInt aError)
	{
	if (!aError)
		{
		iState = EMdaAudioToneUtilityPrepared;
		}
	else 
		{
		iState = EMdaAudioToneUtilityNotReady;
		}

	iCallback.MatoPrepareComplete(aError);
	}

void CMMFMdaAudioToneUtility::MatoPlayComplete(TInt aError)
	{
	iState = EMdaAudioToneUtilityPrepared;
	iCallback.MatoPlayComplete(aError);
	}

void CMMFMdaAudioToneUtility::MatoPlayStarted(TInt aError)
	{
	__ASSERT_DEBUG(aError==KErrNone, Panic(EPlayStartedCalledWithError));
	
	// Not always there is an observer registered
	if(iPlayStartObserver)
		{
		iPlayStartObserver->MatoPlayStarted(aError);
		}
	}

// CustomInferface - just pass on to DevSound. 
TAny* CMMFMdaAudioToneUtility::CustomInterface(TUid aInterfaceId)
	{
	return iDevSound->CustomInterface(aInterfaceId);
	}


CMMFMdaAudioToneObserverCallback* CMMFMdaAudioToneObserverCallback::NewL(MMdaAudioToneObserver& aCallback, MMdaAudioTonePlayStartObserver& aPlayStartCallback)
	{
	return new(ELeave) CMMFMdaAudioToneObserverCallback(aCallback, aPlayStartCallback);
	}

CMMFMdaAudioToneObserverCallback::CMMFMdaAudioToneObserverCallback(MMdaAudioToneObserver& aCallback, MMdaAudioTonePlayStartObserver& aPlayStartCallback) :
	CActive(CActive::EPriorityHigh),
	iCallback(aCallback),
	iPlayStartCallback(aPlayStartCallback)
	{
	CActiveScheduler::Add(this);
	}

CMMFMdaAudioToneObserverCallback::~CMMFMdaAudioToneObserverCallback()
	{
	Cancel();
	}

void CMMFMdaAudioToneObserverCallback::MatoPrepareComplete(TInt aError)
	{
	if(!IsActive())
	    {
        iAction = EPrepareComplete;
        iErrorCode = aError;
        
        TRequestStatus* s = &iStatus;
        SetActive();
        User::RequestComplete(s, KErrNone);
	    }
	else
	    {
		// Since the default granularity is 8, failing of Append() is unusual, hence ignoring the return err.
	    iCallBackQueue.Append(EPrepareComplete);
	    iCallBackQueueError.Append(aError);
	    }
	}

void CMMFMdaAudioToneObserverCallback::MatoPlayComplete(TInt aError)
	{
    if(!IsActive())
        {
        iAction = EPlayComplete;
        iErrorCode = aError;
        
        TRequestStatus* s = &iStatus;
        SetActive();
        User::RequestComplete(s, KErrNone);
        }
    else
        {
        iCallBackQueue.Append(EPlayComplete);
        iCallBackQueueError.Append(aError);
        }
	}

void CMMFMdaAudioToneObserverCallback::MatoPlayStarted(TInt aError)
	{
    if(!IsActive())
        {
        iAction = EPlayStarted;
        iErrorCode = aError;
    
        TRequestStatus* s = &iStatus;
        SetActive();
        User::RequestComplete(s, KErrNone);
        }
    else
        {
        iCallBackQueue.Append(EPlayStarted);
        iCallBackQueueError.Append(aError);
        }
	}

void CMMFMdaAudioToneObserverCallback::RunL()
	{
	switch (iAction)
		{
		case EPrepareComplete:
			{
			iCallback.MatoPrepareComplete(iErrorCode);
			break;
			}
		case EPlayComplete:
			{
			iCallback.MatoPlayComplete(iErrorCode);
			break;
			}
		case EPlayStarted:
			iPlayStartCallback.MatoPlayStarted(iErrorCode);
			break;
		}
	if(iCallBackQueue.Count() > 0 & !IsActive() )
	    {
        iAction = TMMFAudioToneObserverCallbackAction(iCallBackQueue[0]);
        iCallBackQueue.Remove(0);
        iErrorCode = iCallBackQueueError[0];
        iCallBackQueueError.Remove(0);
        
        TRequestStatus* s = &iStatus;
        User::RequestComplete(s, KErrNone);
        SetActive();
	    }
	}

void CMMFMdaAudioToneObserverCallback::DoCancel()
	{
	//nothing to cancel
	}






// Tone config classes

// Simple Tone
CMMFToneConfig* CMMFSimpleToneConfig::NewL(TInt aFrequency, const TTimeIntervalMicroSeconds& aDuration)
	{
	return STATIC_CAST(CMMFToneConfig*, new(ELeave) CMMFSimpleToneConfig(aFrequency, aDuration));
	}

CMMFSimpleToneConfig::CMMFSimpleToneConfig(TInt aFrequency, const TTimeIntervalMicroSeconds& aDuration) :
	CMMFToneConfig(CMMFToneConfig::EMmfToneTypeSimple),
	iFrequency(aFrequency),
	iDuration(aDuration)
	{
	}

CMMFSimpleToneConfig::~CMMFSimpleToneConfig()
	{
	}

TInt CMMFSimpleToneConfig::Frequency()
	{
	return iFrequency;
	}

const TTimeIntervalMicroSeconds& CMMFSimpleToneConfig::Duration()
	{
	return iDuration;
	}


// Dual Tone 
CMMFToneConfig* CMMFDualToneConfig::NewL(TInt aFrequencyOne, TInt aFrequencyTwo, const TTimeIntervalMicroSeconds& aDuration)
	{
	return STATIC_CAST(CMMFToneConfig*, new(ELeave) CMMFDualToneConfig(aFrequencyOne, aFrequencyTwo, aDuration));
	}

CMMFDualToneConfig::CMMFDualToneConfig(TInt aFrequencyOne, TInt aFrequencyTwo, const TTimeIntervalMicroSeconds& aDuration) :
	CMMFToneConfig(CMMFToneConfig::EMmfToneTypeDual),
	iFrequencyOne(aFrequencyOne),
	iFrequencyTwo(aFrequencyTwo),
	iDuration(aDuration)
	{
	}

CMMFDualToneConfig::~CMMFDualToneConfig()
	{
	}

TInt CMMFDualToneConfig::FrequencyOne()
	{
	return iFrequencyOne;
	}

TInt CMMFDualToneConfig::FrequencyTwo()
	{
	return iFrequencyTwo;
	}

const TTimeIntervalMicroSeconds& CMMFDualToneConfig::Duration()
	{
	return iDuration;
	}


CMMFToneConfig* CMMFDTMFStringToneConfig::NewL(const TDesC& aDTMF)
	{
	CMMFDTMFStringToneConfig* s = new(ELeave) CMMFDTMFStringToneConfig;
	CleanupStack::PushL(s);
	s->ConstructL(aDTMF);
	CleanupStack::Pop();
	return STATIC_CAST(CMMFToneConfig*, s);
	}

CMMFDTMFStringToneConfig::CMMFDTMFStringToneConfig() :
	CMMFToneConfig(CMMFToneConfig::EMmfToneTypeDTMF)
	{
	}

LOCAL_C void validateDTMFL(const TDesC& aDTMF)
//
// Validate that the supplied DTMf string contains only playable characters
// 
	{
	TInt stringLength = aDTMF.Length();
	TChar ch;
	for (TInt index = 0; index < stringLength ; index++)
		{
		ch = aDTMF[index];
		if (!ch.IsDigit() && !ch.IsHexDigit() && !ch.IsSpace() &&
			(ch != '*') && (ch != '#') && (ch != ','))
			{
			User::Leave(KErrArgument); // Bad DTMF string
			}
		}
	}

void CMMFDTMFStringToneConfig::ConstructL(const TDesC& aDTMF)
	{
	validateDTMFL(aDTMF);
	iDTMF = aDTMF.AllocL();
	}

CMMFDTMFStringToneConfig::~CMMFDTMFStringToneConfig()
	{
	delete iDTMF;
	}

const TDesC& CMMFDTMFStringToneConfig::DTMF()
	{
	return *iDTMF;
	}


CMMFToneConfig* CMMFDesSeqToneConfig::NewL(const TDesC8& aDesSeq)
	{
	CMMFDesSeqToneConfig* s = new(ELeave) CMMFDesSeqToneConfig;
	CleanupStack::PushL(s);
	s->ConstructL(aDesSeq);
	CleanupStack::Pop();
	return STATIC_CAST(CMMFToneConfig*, s);
	}

CMMFDesSeqToneConfig::CMMFDesSeqToneConfig() :
	CMMFToneConfig(CMMFToneConfig::EMmfToneTypeDesSeq)
	{
	}

void CMMFDesSeqToneConfig::ConstructL(const TDesC8& aDesSeq)
	{
	iDesSeq = aDesSeq.AllocL();
	}

CMMFDesSeqToneConfig::~CMMFDesSeqToneConfig()
	{
	delete iDesSeq;
	}

const TDesC8& CMMFDesSeqToneConfig::DesSeq()
	{
	return *iDesSeq;
	}


CMMFToneConfig* CMMFFileSeqToneConfig::NewL(const TDesC& aFileSeq)
	{
	CMMFFileSeqToneConfig* s = new(ELeave) CMMFFileSeqToneConfig;
	CleanupStack::PushL(s);
	s->ConstructL(aFileSeq);
	CleanupStack::Pop();
	return STATIC_CAST(CMMFToneConfig*, s);
	}

CMMFFileSeqToneConfig::CMMFFileSeqToneConfig() :
	CMMFToneConfig(CMMFToneConfig::EMmfToneTypeFileSeq)
	{
	}

void CMMFFileSeqToneConfig::ConstructL(const TDesC& aFileSeq)
	{
	// get access to DRM content through filename
	iCAFContent = CContent::NewL(aFileSeq);
	
	// open the CAF source with play intent
	iCAFData = iCAFContent->OpenContentL(ContentAccess::EPlay, KDefaultContentObject);

	// read into a descriptor
	TInt dataSize = 0;
	iCAFData->DataSizeL(dataSize);

	iDesSeq = HBufC8::NewL(dataSize);
	TPtr8 desSeqPtr = iDesSeq->Des();
	iCAFData->Read(desSeqPtr);	
	}
	
	

CMMFToneConfig* CMMFFileSeqToneConfig::NewL(RFile& aFile)
	{
	CMMFFileSeqToneConfig* s = new(ELeave) CMMFFileSeqToneConfig;
	CleanupStack::PushL(s);
	s->ConstructL(aFile);
	CleanupStack::Pop();
	return STATIC_CAST(CMMFToneConfig*, s);
	}


void CMMFFileSeqToneConfig::ConstructL(RFile& aFile)
	{
	// get DRM access to file handle
	iCAFContent = CContent::NewL(aFile);
	
	// open the CAF source with play intent
	iCAFData = iCAFContent->OpenContentL(ContentAccess::EPlay, KDefaultContentObject);

	// read into a descriptor
	TInt dataSize = 0;
	iCAFData->DataSizeL(dataSize);

	iDesSeq = HBufC8::NewL(dataSize);
	TPtr8 desSeqPtr = iDesSeq->Des();
	iCAFData->Read(desSeqPtr);	
	}


CMMFFileSeqToneConfig::~CMMFFileSeqToneConfig()
	{
	delete iCAFData;
	iCAFData = NULL;

	delete iCAFContent;
	iCAFContent = NULL;

	delete iDesSeq;
	}

const TDesC8& CMMFFileSeqToneConfig::FileSeq()
	{
	return *iDesSeq;
	}

void CMMFFileSeqToneConfig::ExecuteIntentL()
	{
	if (iCAFData)
		{
		User::LeaveIfError(iCAFData->ExecuteIntent(ContentAccess::EPlay));
		}
	}

CMMFToneConfig* CMMFFixedSeqToneConfig::NewL(TInt aSeqNo)
	{
	return STATIC_CAST(CMMFToneConfig*, new(ELeave) CMMFFixedSeqToneConfig(aSeqNo));
	}

CMMFFixedSeqToneConfig::CMMFFixedSeqToneConfig(TInt aSeqNo) :
	CMMFToneConfig(CMMFToneConfig::EMmfToneTypeFixedSeq),
	iSequenceNumber(aSeqNo)
	{
	}

CMMFFixedSeqToneConfig::~CMMFFixedSeqToneConfig()
	{
	}

TInt CMMFFixedSeqToneConfig::SequenceNumber()
	{
	return iSequenceNumber;
	}

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
// Initializes DevSound object for the mode aMode for processing audio data
// with hardware device aHWDev.
// On completion of Initialization, the observer will be notified via call back
// InitializeComplete().
// Leaves on failure.
// @param	"MDevSoundObserver& aDevSoundObserver"
// A reference to DevSound Observer instance.
// @param	"TUid aHWDev"
// CMMFHwDevice implementation identifier.
// @param	"TMMFState aMode"
// Mode for which this object will be used.
// 
//

inline void CMMFDevSoundClientImp::InitializeL(MDevSoundObserver& aDevSoundObserver, TUid aHWDev, TMMFState aMode)
	{
	TInt initError = KErrNone;
	iDevSoundObserver = &aDevSoundObserver;
	initError = iDevSoundProxy->InitializeL(aHWDev, aMode);

	if (initError)
		{
		User::Leave(initError);
		}

	if (iMsgQueueHandler)
		{
		iMsgQueueHandler->Cancel();
		iMsgQueueHandler->SetObserver(*iDevSoundObserver);
		}
	else
		{
		iMsgQueueHandler = CMsgQueueHandler::NewL(iDevSoundProxy, *iDevSoundObserver, &iMsgQueue, *this);
		}

	iMsgQueueHandler->ReceiveEvents();
	}

/*
 *
 *	Initializes DevSound object for the mode aMode for processing audio data
 *	with hardware device supporting FourCC aDesiredFourCC.
 *
 *	On completion of Initialization, the observer will be notified via call back
 *	InitializeComplete().
 *
 *	Leaves on failure.
 *  
 *	@param	"MDevSoundObserver& aDevSoundObserver"
 *			A reference to DevSound Observer instance.
 *
 *	@param	"TFourCC aDesiredFourCC"
 *			CMMFHwDevice implementation FourCC.
 *
 *	@param	"TMMFState aMode"
 *			Mode for which this object will be used.
 *
 */
inline void CMMFDevSoundClientImp::InitializeL(MDevSoundObserver& aDevSoundObserver, 
										  TFourCC aDesiredFourCC, TMMFState aMode)
	{
	TInt initError = KErrNone;
	iDevSoundObserver = &aDevSoundObserver;

	initError = iDevSoundProxy->InitializeL(aDesiredFourCC, aMode);

	if (initError)
		{
		User::Leave(initError);
		}

	if (iMsgQueueHandler)
		{
		iMsgQueueHandler->Cancel();
		iMsgQueueHandler->SetObserver(*iDevSoundObserver);
		}
	else
		{
		iMsgQueueHandler = CMsgQueueHandler::NewL(iDevSoundProxy, *iDevSoundObserver, &iMsgQueue, *this);
		}
	iMsgQueueHandler->ReceiveEvents();
	}

/*
 *
 *	Returns the supported Audio settings.
 *  
 *	@return	"TMMFCapabilities"
 *			Device settings.
 *
 */
inline TMMFCapabilities CMMFDevSoundClientImp::Capabilities()
	{
	return iDevSoundProxy->Capabilities();
	}

/*
 *
 *	Returns the current audio settings.
 *  
 *	@return	"TMMFCapabilities"
 *			Device settings.
 *
 */
inline TMMFCapabilities CMMFDevSoundClientImp::Config() const
	{
	return iDevSoundProxy->Config();
	}

/*
 *
 *	Returns an integer representing the maximum volume.
 *
 *	This is the maximum value which can be passed to CMMFDevSound::SetVolume.
 *  
 *	@return	"TInt"
 *			The maximum volume. This value is platform dependent but is always
 *			greater than or equal to one.
 *
 */
inline TInt CMMFDevSoundClientImp::MaxVolume()
	{
	return iDevSoundProxy->MaxVolume();
	}

/*
 *
 *	Returns an integer representing the current volume.
 * 
 *	@return	"TInt"
 *			The current volume level.
 *
 */
inline TInt CMMFDevSoundClientImp::Volume()
	{
	return iDevSoundProxy->Volume();
	}

/*
 *
 *	Returns an integer representing the maximum gain.
 *
 *	This is the maximum value which can be passed to CMMFDevSound::SetGain.
 * 
 *	@return	"TInt"
 *			The maximum gain. This value is platform dependent but is always
 *			greater than or equal to one.
 *
 */
inline TInt CMMFDevSoundClientImp::MaxGain()
	{
	return iDevSoundProxy->MaxGain();
	}

/*
 *
 *	Returns an integer representing the current gain.
 *
 *	@return	"TInt"
 *			The current gain level.
 *
 */
inline TInt CMMFDevSoundClientImp::Gain()
	{
	return iDevSoundProxy->Gain();
	}

/*
 *
 *	Returns the speaker balance set for playing.
 *
 *	Leaves on failure.
 *
 *	@param	"TInt& aLeftPercentage"
 *			On return contains the left speaker volume percentage.
 *
 *	@param	"TInt& aRightPercentage"
 *			On return contains the right speaker volume percentage.
 *
 */
inline void CMMFDevSoundClientImp::GetPlayBalanceL(TInt& aLeftPercentage, TInt& aRightPercentage)
	{
	iDevSoundProxy->GetPlayBalanceL(aLeftPercentage, aRightPercentage);
	}

/*
 *
 *	Returns the microphone gain balance set for recording.
 *
 *	Leaves on failure.
 *
 *	@param	"TInt& aLeftPercentage"
 *			On return contains the left microphone gain percentage.
 *
 *	@param	"TInt& aRightPercentage"
 *			On return contains the right microphone gain percentage.
 *
 */
inline void CMMFDevSoundClientImp::GetRecordBalanceL(TInt& aLeftPercentage, TInt& aRightPercentage)
	{
	iDevSoundProxy->GetRecordBalanceL(aLeftPercentage, aRightPercentage);
	}

/*
 *
 *	Contine the process of recording. Once the buffer is filled with recorded
 *	data, the Observer gets reference to buffer along with callback
 *	BufferToBeEmptied(). After processing the buffer (copying over to a
 *	different buffer or writing to file) the client should call this
 *	method to continue recording process.
 *
 */
inline void CMMFDevSoundClientImp::RecordData()
	{
	ASSERT(iDevSoundObserver);
	iDevSoundProxy->RecordData();
	}

/*
 *
 *	Defines the number of times the audio is to be repeated during the tone
 *	playback operation.
 *
 *	A period of silence can follow each playing of tone. The tone playing can
 *	be repeated indefinitely.
 *
 *	@param	"TInt aRepeatCount"
 *			The number of times the tone, together with the trailing silence,
 *			is to be repeated. If this is set to KMdaRepeatForever, then the
 *			tone, together with the trailing silence, is repeated indefinitely
 *			or until Stop() is called. If this is set to zero, then the tone is
 *			not repeated.
 *
 *			Supported only during tone playing.
 *
 */
inline void CMMFDevSoundClientImp::SetToneRepeats(TInt aRepeatCount,
				const TTimeIntervalMicroSeconds& aRepeatTrailingSilence)
	{
	iDevSoundProxy->SetToneRepeats(aRepeatCount, aRepeatTrailingSilence);
	}

/*
 *
 *	Defines the priority settings that should be used for this instance.
 *
 *	@param	"const TMMFPrioritySettings& aPrioritySettings"
 *			An class type representing the client's priority, priority 
 *			preference and state.
 *
 */
inline void CMMFDevSoundClientImp::SetPrioritySettings(const TMMFPrioritySettings& aPrioritySettings)
	{
	iDevSoundProxy->SetPrioritySettings(aPrioritySettings);
	}


/*
 *
 *	Returns the number of available pre-defined tone sequences.
 *
 *	This is the number of fixed sequence supported by DevSound by default.
 *
 *	@return	"TInt"
 *			The fixed sequence count. This value is implementation dependent
 *			but is always greater than or equal to zero.
 *
 */
inline TInt CMMFDevSoundClientImp::FixedSequenceCount()
	{

	return iDevSoundProxy->FixedSequenceCount();
	}

/*
 *
 *	Returns the name assigned to a specific pre-defined tone sequence.
 *
 *	This is the number of fixed sequence supported by DevSound by default.
 *
 *	The function raises a panic if sequence number specified invalid.
 *
 *	@return	"TDesC&"
 *			A reference to a Descriptor containing the fixed sequence
 *			name indexed by aSequenceNumber.
 *
 *	@param	"TInt aSequenceNumber"
 *			The index identifying the specific pre-defined tone sequence. Index
 *			values are relative to zero.
 *			This can be any value from zero to the value returned by a call to
 *			CMdaAudioPlayerUtility::FixedSequenceCount() - 1.
 *			The function raises a panic if sequence number is not within this
 *			range.
 *
 */
inline const TDesC& CMMFDevSoundClientImp::FixedSequenceName(TInt aSequenceNumber)
	{
	return iDevSoundProxy->FixedSequenceName(aSequenceNumber);
	}


inline TInt CMMFDevSoundClientImp::SyncCustomCommand(TUid aUid, const TDesC8& aParam1, const TDesC8& aParam2, TDes8* aOutParam)
	{
	return iDevSoundProxy->SyncCustomCommand(aUid, aParam1, aParam2, aOutParam);
	}

inline void CMMFDevSoundClientImp::AsyncCustomCommand(TUid aUid, TRequestStatus& aStatus, const TDesC8& aParam1, const TDesC8& aParam2, TDes8* aOutParam)
	{
	iDevSoundProxy->AsyncCustomCommand(aUid, aStatus, aParam1, aParam2, aOutParam);
	}


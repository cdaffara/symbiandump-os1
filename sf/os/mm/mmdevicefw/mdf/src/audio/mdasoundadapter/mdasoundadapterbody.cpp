// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include "mdasoundadapterconsts.h"
#include "mdasoundadapterbody.h"
#include <e32debug.h>

#include "mmf/utils/rateconvert.h" // if we need to resample

#include <hal.h>

_LIT(KPddFileName,"SOUNDSC.PDD");
_LIT(KLddFileName,"ESOUNDSC.LDD");


const TInt KBytesPerSample = 2;
const TInt KMinBufferSize = 2;

/**
This function raises a panic
EDeviceNotOpened is raised when any of the RMdaDevSound APIs are called before opening the device. 
*/
GLDEF_C void Panic(TSoundAdapterPanicCodes aPanicCode)
	{
	User::Panic(KSoundAdapterPanicCategory, aPanicCode);
	}


const TText8 *RMdaDevSound::CBody::TState::Name() const
	{
	#ifdef SYMBIAN_SOUNDADAPTER_DEBUG	 
	switch(iState)
		{
		case ENotReady:				return _S8("ENotReady");
		case EStopped:				return _S8("EStopped");
		case ERecording:			return _S8("ERecording");
		case ERecordingPausedInHw:	return _S8("ERecordingPausedInHw");
		case ERecordingPausedInSw:	return _S8("ERecordingPausedInSw");
		case EPlaying:				return _S8("EPlaying");
		case EPlayingPausedInHw: 	return _S8("EPlayingPausedInHw");
		case EPlayingPausedInSw:	return _S8("EPlayingPausedInSw");
		case EPlayingUnderrun:		return _S8("EPlayingUnderrun");
		}
	return _S8("CorruptState");
	#else
	return _S8("");
	#endif
	}

	

RMdaDevSound::CBody::TState &RMdaDevSound::CBody::TState::operator=(TStateEnum aNewState)
	{
    if(iState != aNewState)
        {
        #ifdef SYMBIAN_SOUNDADAPTER_DEBUG    
        RDebug::Printf("RMdaDevSound state %s -> %s", Name(), TState(aNewState).Name());
        #endif
        iState = aNewState;
        }
	return *this;
	}

RMdaDevSound::CBody* RMdaDevSound::CBody::NewL()
	{
	CBody* self = new(ELeave) CBody();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

RMdaDevSound::CBody::~CBody()
	{
	for(TInt i = 0; i < KPlaySharedChunkBuffers; i++)
		{
		delete iPlayers[i];
		iPlayers[i] = NULL;
		}
	delete iRecorder;
	iRecorder = NULL;
	delete iPlayFormatData.iConverter;
	delete iRecordFormatData.iConverter;
	iPlayChunk.Close();
	iPlaySoundDevice.Close();
	iRecordChunk.Close();
	iRecordSoundDevice.Close();
	iConvertedPlayData.Close();
	iSavedTrailingData.Close();
	iBufferedRecordData.Close();
	}
	
RMdaDevSound::CBody::CBody()
	:iState(ENotReady), iBufferOffset(-1)
	{
	
	}

TVersion RMdaDevSound::CBody::VersionRequired() const
	{
	if(iPlaySoundDevice.Handle())
		{
		return iPlaySoundDevice.VersionRequired();
		}
	else
		{
		return TVersion();
		}
	}

TInt RMdaDevSound::CBody::IsMdaSound()
	{
	return ETrue;
	}
	
void RMdaDevSound::CBody::ConstructL()
	{
	// Try to load the audio physical driver
    TInt err = User::LoadPhysicalDevice(KPddFileName);
	if ((err!=KErrNone) && (err!=KErrAlreadyExists))
		{
		User::Leave(err);
		}
    // Try to load the audio logical driver
	err = User::LoadLogicalDevice(KLddFileName);
    if ((err!=KErrNone) && (err!=KErrAlreadyExists))
    	{
    	User::Leave(err);
    	}
	for(TInt i=0; i<KPlaySharedChunkBuffers; i++)
		{
		iPlayers[i] = new(ELeave) CPlayer(CActive::EPriorityUserInput, *this, i);
		iFreePlayers.Push(iPlayers[i]);
		}
	
	iRecorder = new(ELeave) CRecorder(CActive::EPriorityUserInput, *this);
	
	TInt tmp;
	User::LeaveIfError(HAL::Get(HAL::ENanoTickPeriod, tmp));
	iNTickPeriodInUsec = tmp;
	}

TInt RMdaDevSound::CBody::Open(TInt /*aUnit*/)
	{
    #ifdef SYMBIAN_SOUNDADAPTER_DEBUG	
        RDebug::Print(_L("RMdaDevSound::CBody::Open "));
    #endif	
	TInt err = KErrNone;
	//Default behavior of this method is to open both the play and record audio devices.
	if(!iPlaySoundDevice.Handle() && !iRecordSoundDevice.Handle())
        {
		err = iPlaySoundDevice.Open(KSoundScTxUnit0);
    	if(err == KErrNone)
    		{
    		err = iRecordSoundDevice.Open(KSoundScRxUnit0);
    		}
		}
	if(err != KErrNone)
		{
		Close();
		}
	else
	    {
		TSoundFormatsSupportedV02Buf capsBuf;
		iPlaySoundDevice.Caps(capsBuf);
		TInt minBufferSize = KMinBufferSize;
		#ifdef SYMBIAN_FORCE_32BIT_LENGTHS
		minBufferSize = Max(minBufferSize, 4); // force to 32-bit buffer align
		#endif
		iRequestMinSize = Max(capsBuf().iRequestMinSize, minBufferSize); 
		// work out mask so that x&iRequestMinMask is equiv to x/iRequestMinSize*iRequestMinSize
		iRequestMinMask = ~(iRequestMinSize-1); // assume iRequestMinSize is power of 2
		iSavedTrailingData.Close();
		iSavedTrailingData.Create(iRequestMinSize);
	
	    iState = EStopped;
		iBytesPlayed = 0;
	    }

	return err;
	}
		
TInt RMdaDevSound::CBody::PlayVolume()
	{
	__ASSERT_DEBUG(iPlaySoundDevice.Handle(), Panic(EDeviceNotOpened));
	return iPlaySoundDevice.Volume();	
	}
	
void RMdaDevSound::CBody::SetPlayVolume(TInt aVolume)
	{
	__ASSERT_DEBUG(iPlaySoundDevice.Handle(), Panic(EDeviceNotOpened));
	if(aVolume >=0 && aVolume<=KSoundMaxVolume)
		{
		iPlaySoundDevice.SetVolume(KLinerToDbConstantLookup[aVolume].iDBValue);
		}
	}
void RMdaDevSound::CBody::SetVolume(TInt aLogarithmicVolume)
	{
	__ASSERT_DEBUG(iPlaySoundDevice.Handle(), Panic(EDeviceNotOpened));
	if(aLogarithmicVolume >= 0 && aLogarithmicVolume <= KSoundMaxVolume)
		{
		iPlaySoundDevice.SetVolume(aLogarithmicVolume);
		}
	}
	
void RMdaDevSound::CBody::CancelPlayData()
	{
    #ifdef SYMBIAN_SOUNDADAPTER_DEBUG	
    RDebug::Printf("RMdaDevSound::CBody::CancelPlayData: state %s", iState.Name());
    #endif	
	__ASSERT_DEBUG(iPlaySoundDevice.Handle(), Panic(EDeviceNotOpened));

    // If there is a client request, cancel it
    // Must do this before canceling players because otherwise they may just restart!
    if(iClientPlayStatus)
        {
        #ifdef SYMBIAN_SOUNDADAPTER_DEBUG    
        RDebug::Printf("msp PlayCancelled complete iClientPlayStatus");
		#endif
        User::RequestComplete(iClientPlayStatus, KErrCancel); // Call also sets iClientPlayStatus to NULL
        }
    
    // Discard any buffered data
    iClientPlayData.Set(0,0);
	// Discard any saved trailing data (ie. data saved due driver requiring all requests to be a multiple of iRequestMinSize).
	iSavedTrailingData.SetLength(0);

    // Emulator RSoundSc PDD when running without a soundcard has a major
    // issue with cancelling whilst paused. It will not clear the pending
    // list (because the timer is not active) and therefore this list will
    // later overflow causing hep corruption.
    // This means that, for now, we MUST Resume before calling CancelPlayData
    // to avoid kernel panics...
    
    // The device driver will not cancel a request which is in progress...
    // So, if we are paused in hw, we must resume before cancelling the
    // player otherwise it will hang in CActive::Cancel
    if(iState == EPlayingPausedInHw)
        {
        #ifdef SYMBIAN_SOUNDADAPTER_DEBUG    
        RDebug::Printf("msp Resume to avoid hang");
        #endif
        (void) iPlaySoundDevice.Resume();
        }
    
    // Update state
	iState = EStopped;
	

    // The RSoundSc driver will not cancel a request which is in progress (or paused).
    // If we just loop across the players, cancelling each individual request and waiting for it to complete,
    // several of them will actually play, which is both wrong and time consuming....
    // Issue a block cancel upfront to avoid this
    iPlaySoundDevice.CancelPlayData();
 
	// Cancel all players
	for (TUint playerIndex=0; playerIndex<KPlaySharedChunkBuffers; ++playerIndex)
	    {
	    // If the player is active it will call PlayRequestCompleted with aDueToCancelCommand true
	    // to update the iFreePlayers and iActivePlayRequestSizes FIFOs.
        iPlayers[playerIndex]->Cancel();
	    }
	
	iBufferOffset = -1;
	iBufferLength = 0;
	
	return;
	}
	
TInt RMdaDevSound::CBody::RecordLevel()
	{
	__ASSERT_DEBUG(iRecordSoundDevice.Handle(), Panic(EDeviceNotOpened));
	return iRecordSoundDevice.Volume();
	}
	
void RMdaDevSound::CBody::SetRecordLevel(TInt aLevel)
	{
	__ASSERT_DEBUG(iRecordSoundDevice.Handle(), Panic(EDeviceNotOpened));
	iRecordSoundDevice.SetVolume(aLevel);	
	}
	
void RMdaDevSound::CBody::CancelRecordData()
	{
	__ASSERT_DEBUG(iRecordSoundDevice.Handle(), Panic(EDeviceNotOpened));
    #ifdef SYMBIAN_SOUNDADAPTER_DEBUG	
    RDebug::Printf("RMdaDevSound::CBody::CancelRecordData: state %s", iState.Name());
    #endif

    // Stop recorder object (and its request)
    iRecorder->Cancel();
    
    // Stop driver from recording
    iRecordSoundDevice.CancelRecordData();
             
    // If there is a client request, cancel it
    if(iClientRecordStatus)
   		{
        User::RequestComplete(iClientRecordStatus, KErrNone); // Call also sets iClientPlayStatus to NULL
        }

    iState = EStopped;
    return;
	}
	
void RMdaDevSound::CBody::FlushRecordBuffer()
	{
	__ASSERT_DEBUG(iRecordSoundDevice.Handle(), Panic(EDeviceNotOpened));
    #ifdef SYMBIAN_SOUNDADAPTER_DEBUG	
        RDebug::Print(_L("RMdaDevSound::CBody::FlushRecordBuffer - implemented by calling PauseRecordBuffer"));
    #endif

	PauseRecordBuffer();
	}
	
TInt RMdaDevSound::CBody::BytesPlayed()
	{
    #ifdef SYMBIAN_SOUNDADAPTER_DEBUG    
    RDebug::Printf("RMdaDevSound::BytesPlayed %s", iState.Name());
	#endif

	return I64LOW(BytesPlayed64());
	}


TUint64 RMdaDevSound::CBody::BytesPlayed64()
	{
	__ASSERT_DEBUG(iPlaySoundDevice.Handle(), Panic(EDeviceNotOpened));

	TUint64 currentBytesPlayed = KMaxTUint64;

	switch(iState)
	{
	case ENotReady:
		Panic(EDeviceNotOpened);
		break;

	case EStopped:
		currentBytesPlayed = iBytesPlayed;
		break;

	case ERecording:
	case ERecordingPausedInHw:
	case ERecordingPausedInSw:
		Panic(EBadState);
		break;

	case EPlayingPausedInHw: // ie. Play request pending on h/w and paused
		// Paused, so use pause time
        #ifdef SYMBIAN_SOUNDADAPTER_DEBUG    
		RDebug::Printf("EPlayingPausedInHw: iPausedBytes %x %x", I64HIGH(iPausedBytesPlayed), I64LOW(iPausedBytesPlayed));
		#endif
		currentBytesPlayed = iPausedBytesPlayed;
		break;

	case EPlayingPausedInSw: // ie. Driver not playing or paused
		#ifdef SYMBIAN_SOUNDADAPTER_DEBUG	 
		RDebug::Printf("EPlayingPausedInSw: iPausedBytesPlayed %x %x", I64HIGH(iPausedBytesPlayed), I64LOW(iPausedBytesPlayed));
		#endif
		currentBytesPlayed = iPausedBytesPlayed;
		break;
	case EPlayingUnderrun:
		#ifdef SYMBIAN_SOUNDADAPTER_DEBUG	 
		RDebug::Printf("EPlayingUnderrun: iBytesPlayed %x %x", I64HIGH(iBytesPlayed), I64LOW(iBytesPlayed));
		#endif
		currentBytesPlayed = iBytesPlayed;
	    break;

	case EPlaying:
		{
		// Playing so calculate time since last update to iBytesPlayed
		TUint32 curTime = CurrentTimeInMsec();
		TUint32 curRequestSize = iActivePlayRequestSizes.Peek();

		TUint32 extraPlayTime = (curTime >= iStartTime) ? (curTime-iStartTime) : (KMaxTUint32 - (iStartTime-curTime));
        #ifdef SYMBIAN_SOUNDADAPTER_DEBUG	 
		RDebug::Printf("iStartTime %d curTime %d extraPlayTime %d", iStartTime, curTime, extraPlayTime);
		
		RDebug::Printf("iPlayFormatData.iSampleRate %d KBytesPerSample %d iNTickPeriodInUsec %d",
					   iPlayFormatData.iSampleRate, KBytesPerSample, iNTickPeriodInUsec);
        #endif
		TUint32 extraBytesPlayed = TUint32((TUint64(extraPlayTime) * iPlayFormatData.iSampleRate * iPlayFormatData.iRequestedChannels * KBytesPerSample)/1000);
		if(extraBytesPlayed > curRequestSize)
			{
            #ifdef SYMBIAN_SOUNDADAPTER_DEBUG	 
			RDebug::Printf("caping extraBytes played from %d to %d", extraBytesPlayed, curRequestSize);
            #endif
			extraBytesPlayed = curRequestSize;
			}

		#ifdef SYMBIAN_SOUNDADAPTER_DEBUG
		RDebug::Printf("iBytesPlayed %d extraBytesPlayed %d (curRequestSize %d) -> currentBytesPlayed %x %x",
                iBytesPlayed, extraBytesPlayed, curRequestSize, I64HIGH(currentBytesPlayed), I64LOW(currentBytesPlayed));
        #endif

		currentBytesPlayed = iBytesPlayed + extraBytesPlayed;
		break;
		}
	
	default:
		Panic(EBadState);
		break;
	}
 

    #ifdef SYMBIAN_SOUNDADAPTER_DEBUG
	RDebug::Printf("iPlayFormatData.iConverter %x", iPlayFormatData.iConverter);
    #endif

	if (iPlayFormatData.iConverter)
		{
		// need to scale bytes played to fit with requested rate and channels, not actual
		if (iPlayFormatData.iActualChannels != iPlayFormatData.iRequestedChannels)
			{
			if (iPlayFormatData.iActualChannels == 2)
				{
				// requested was mono, we have stereo
				currentBytesPlayed /= 2;
				}
			else 
				{
				// requested was stereo, we have mono
				currentBytesPlayed *= 2;
				}
			}
		if (iPlayFormatData.iSampleRate != iPlayFormatData.iActualRate)
			{
			currentBytesPlayed = TUint64(currentBytesPlayed*
					TReal(iPlayFormatData.iSampleRate)/TReal(iPlayFormatData.iActualRate)); // don't round
			}
		}

    #ifdef SYMBIAN_SOUNDADAPTER_DEBUG
	RDebug::Printf("currentBytesPlayed %x %x", I64HIGH(currentBytesPlayed), I64LOW(currentBytesPlayed));
    #endif
	return currentBytesPlayed;
	}

void RMdaDevSound::CBody::ResetBytesPlayed()
	{
    #ifdef SYMBIAN_SOUNDADAPTER_DEBUG    
    RDebug::Printf("RMdaDevSound::CBody::ResetBytesPlayed %s", iState.Name());
	#endif
	__ASSERT_DEBUG(iPlaySoundDevice.Handle(), Panic(EDeviceNotOpened));
	iBytesPlayed = 0;
	iPlaySoundDevice.ResetBytesTransferred();
	return;
	}
	
void RMdaDevSound::CBody::PausePlayBuffer()
	{
#ifdef SYMBIAN_SOUNDADAPTER_DEBUG   
    RDebug::Printf("RMdaDevSound::CBody::PausePlayBuffer %s", iState.Name());
#endif  
	switch(iState)
		{
		case ENotReady:
			Panic(EDeviceNotOpened);
			break;

		case EStopped:
			// Driver is not playing so pause in s/w
			break;

		case ERecording:
		case ERecordingPausedInHw:
		case ERecordingPausedInSw:
			Panic(EBadState);
			break;

		case EPlayingPausedInHw: // ie. Play request pending on h/w and paused
		case EPlayingPausedInSw: // ie. Driver not playing or paused
			// Already paused so nothing to do.
			break;
		case EPlayingUnderrun:
			iState = EPlayingPausedInSw;
			break;
			
		case EPlaying:
			{
			iPauseTime = CurrentTimeInMsec();
			iPausedBytesPlayed = BytesPlayed64();
			TInt res = iPlaySoundDevice.Pause();
			#ifdef SYMBIAN_SOUNDADAPTER_DEBUG   
			RDebug::Printf("iPlaySoundDevice.Pause res = %d", res);
			#endif
 			if(res == KErrNone)
				{
				iState = EPlayingPausedInHw;
				}
			else
				{
			    #ifdef SYMBIAN_SOUNDADAPTER_DEBUG    
				RDebug::Printf("msp PausePlayBuffer hw pause unexpectedly failed, doing sw pause");
				#endif
				iState = EPlayingPausedInSw;
				}
			break;
			}
		
		default:
			Panic(EBadState);
			break;
		}
	
	return;
	}
	
void RMdaDevSound::CBody::ResumePlaying()
	{
	#ifdef SYMBIAN_SOUNDADAPTER_DEBUG   
	RDebug::Printf("RMdaDevSound::CBody::ResumePlaying %s", iState.Name());
	#endif	
    __ASSERT_DEBUG(iPlaySoundDevice.Handle(), Panic(EDeviceNotOpened));

	switch(iState)
		{
		case ENotReady:
			Panic(EDeviceNotOpened);
			break;
				
		case EStopped:
			// No change
			break;
	
		case ERecording:
		case ERecordingPausedInHw:
		case ERecordingPausedInSw:
			Panic(EBadState);
			break;
			
		case EPlaying:
			// No change
			break;

		case EPlayingPausedInHw: // ie. Play request pending on h/w and paused
			{
			// Re-enable reporting of KErrUnderflow (will re-raise KErrUnderflow if nothing to start playing).
			iUnderFlowReportedSinceLastPlayOrRecordRequest = EFalse;

			TInt res = iPlaySoundDevice.Resume();
#ifdef SYMBIAN_SOUNDADAPTER_DEBUG	
			RDebug::Printf("ResumePlayBuffer EPlayingPausedInHw res = %d", res);
#endif
			if(res == KErrNone)
				{
				// Resume ok so a pending request will complete
				iState = EPlaying;
	            // Update iStartTime to allow for time spent paused
	            TUint32 curTime = CurrentTimeInMsec();
	            TUint32 timePaused = (curTime >= iPauseTime) ? (curTime-iPauseTime) : (KMaxTUint32 - (iPauseTime-curTime));
	            iStartTime += timePaused; // nb. It is harmless if this wraps.
				}
			else
				{
				// Resume failed, therefore driver is not playing
                // No need to update iStartTime/iPauseTime because these are only used within a driver request
                // Change state to Stopped
                iState = EStopped;
                //  Attempt to start a new (pending) request.
                StartPlayersAndUpdateState();
				}
			break;
			}
		case EPlayingPausedInSw: // ie. Driver not playing/paused
			{
			// Driver not playing
			// Re-enable reporting of KErrUnderflow (will re-raise KErrUnderflow if nothing to start playing).
			iUnderFlowReportedSinceLastPlayOrRecordRequest = EFalse;
			// No need to update iStartTime/iPauseTime because these are only used within a driver request
			// Change state to Stopped
            iState = EStopped;
            //	Attempt to start a new (pending) request.
			StartPlayersAndUpdateState();
			break;
			}
		case EPlayingUnderrun:
			break;
				
		default:
			Panic(EBadState);
			break;
		}
	
	return;	
	}

void RMdaDevSound::CBody::PauseRecordBuffer()
	{
    #ifdef SYMBIAN_SOUNDADAPTER_DEBUG   
    RDebug::Printf("RMdaDevSound::CBody::PauseRecordBuffer %s", iState.Name());
    #endif
	
	switch(iState)
		{
		case ENotReady:
			Panic(EDeviceNotOpened);
			break;
			
		case EStopped:
			// Driver is not recording so pause in s/w
		    // Do not pause because that will cause problems when CAudioDevice::Pause calls
			break;

		case ERecording:
			{
			TInt res = iRecordSoundDevice.Pause();
			#ifdef SYMBIAN_SOUNDADAPTER_DEBUG	
			RDebug::Printf("PauseRecordBuffer EPlaying res = %d", res);
			#endif
			if(res == KErrNone)
				{
				iState = ERecordingPausedInHw;
				}
			else
				{
				#ifdef SYMBIAN_SOUNDADAPTER_DEBUG	
				RDebug::Printf("PauseRecordBuffer hw pause unexpectedly failed, doing sw pause");
				#endif
				iState = ERecordingPausedInSw;
				}
			break;
			}
		
		case ERecordingPausedInHw:
		case ERecordingPausedInSw:
			// Already paused so nothing to do.
			break;
			
		case EPlaying:
		case EPlayingPausedInHw: // ie. Play request pending on h/w and paused
            Panic(EBadState);
            break;
		    
		case EPlayingPausedInSw: 
		    // This is an ugly hack to maintain compatibility with CAudioDevice::Pause which
		    // calls both PausePlayBuffer and PauseRecordBuffer whilst in stopped, then later calls ResumePlaying
		    break;
		case EPlayingUnderrun: // ie. Play request pending on h/w and paused
			Panic(EBadState);
		    break;
		    
		default:
			Panic(EBadState);
			break;
		}

	return;	
	}

void RMdaDevSound::CBody::ResumeRecording()
	{
    #ifdef SYMBIAN_SOUNDADAPTER_DEBUG   
    RDebug::Printf("RMdaDevSound::CBody::ResumeRecording %s", iState.Name());
    #endif
	__ASSERT_DEBUG(iPlaySoundDevice.Handle(), Panic(EDeviceNotOpened));

	switch(iState)
		{
		case ENotReady:
			Panic(EDeviceNotOpened);
			break;
				
		case EStopped:
			// No change
			break;
	
		case ERecording:
			// No change
			break;

		case ERecordingPausedInHw:
			{
			TInt res = iRecordSoundDevice.Resume();
			#ifdef SYMBIAN_SOUNDADAPTER_DEBUG	
			RDebug::Printf("ResumeRecordBuffer ERecordingPausedInHw res = %d", res);
			#endif
			if(res == KErrNone)
				{
				// Resume ok so a pending request will complete
				iState = ERecording;
				}
			else
				{
				iState = EStopped;
				// Resume failed, so attempt to start a new (pending) request.
				// If this works, it will update the state to ERecording.
				StartRecordRequest();
				}
			break;
			}
		case ERecordingPausedInSw:
			{
			// Update state to stopped and attempt to start any pending request
			iState = EStopped;
			// If this works, it will update the state to ERecording.
			StartRecordRequest();
			break;
			}

		case EPlaying:
		case EPlayingPausedInHw: // ie. Play request pending on h/w and paused
		case EPlayingPausedInSw: // ie. Driver not playing/paused
		case EPlayingUnderrun:
		default:
			Panic(EBadState);
			break;
		}
		
		return; 


	}

TInt RMdaDevSound::CBody::GetTimePlayed(TTimeIntervalMicroSeconds& aTimePlayed)
	{
	__ASSERT_DEBUG(iPlaySoundDevice.Handle(), Panic(EDeviceNotOpened));


	TUint64 bytesPlayed = BytesPlayed64();

	TUint64 timePlayed = 1000 * 1000 * bytesPlayed / (iPlayFormatData.iSampleRate * iPlayFormatData.iRequestedChannels * KBytesPerSample);

	aTimePlayed = TTimeIntervalMicroSeconds(timePlayed);

	return KErrNone;
	}

	
void RMdaDevSound::CBody::FormatsSupported(TSoundFormatsSupportedBuf& aFormatsSupported, RSoundSc& aSoundDevice)
	{
	TSoundFormatsSupportedV02Buf supportedFormat;
	aSoundDevice.Caps(supportedFormat);
	TUint32 rates = supportedFormat().iRates;
	
	for(TInt i = KNumSampleRates-1; i > 0 ;i--)//min to max
		{
		if(rates & KRateEnumLookup[i].iRateConstant)
			{
			aFormatsSupported().iMinRate = KRateEnumLookup[i].iRate;
			break;
			}
		}
	for(TInt i = 0; i < KNumSampleRates; i++)//max to min
		{
		if(rates & KRateEnumLookup[i].iRateConstant)
			{
			aFormatsSupported().iMaxRate = KRateEnumLookup[i].iRate;
			break;
			}
		}
	TUint32 enc = supportedFormat().iEncodings;
	
	if (enc & KSoundEncoding16BitPCM)
		{
		aFormatsSupported().iEncodings = EMdaSoundEncoding16BitPCM;// Always defaults to this
		}
	if (enc & KSoundEncoding8BitPCM)
		{
		aFormatsSupported().iEncodings |= EMdaSoundEncoding8BitPCM;
		}
	TUint32 channels = supportedFormat().iChannels;
	
	if (channels & KSoundStereoChannel)
		{
		aFormatsSupported().iChannels = 2;
		}
	else
		{
		aFormatsSupported().iChannels = 1;
		}
	aFormatsSupported().iMinBufferSize = supportedFormat().iRequestMinSize;
	aFormatsSupported().iMaxBufferSize = KMaxBufferSize;
	aFormatsSupported().iMinVolume = 0;
	aFormatsSupported().iMaxVolume = KSoundMaxVolume;
	}
	
void RMdaDevSound::CBody::GetFormat(TCurrentSoundFormatBuf& aFormat, 
									RSoundSc& /*aSoundDevice*/,
									const TFormatData &aFormatData)
	{
	// always return the requested, or the initial, not current device setting
	aFormat().iChannels = aFormatData.iRequestedChannels; // never clear if this is bitmap or value, but effectively the same
	aFormat().iRate = aFormatData.iSampleRate;
	}
	
void RMdaDevSound::CBody::StartPlayersAndUpdateState()
	{
	__ASSERT_DEBUG(iPlaySoundDevice.Handle(), Panic(EDeviceNotOpened));

	switch(iState)
		{
		case ENotReady:
			Panic(EDeviceNotOpened);
			break;
				
		case EStopped:
 			// Allow following code to queue more driver play requests and check for stopped
			break;
	
		case ERecording:
		case ERecordingPausedInHw:
		case ERecordingPausedInSw:
			Panic(EBadState);
			break;
			
		case EPlaying:
           // Allow following code to queue more driver play requests  and check for stopped
		    break;
		case EPlayingPausedInHw: // ie. Play request pending on h/w and paused
			// Allow following code to queue more driver play requests
			break;
		
		case EPlayingPausedInSw:
			// Paused but driver not playing+paused, therefore do not queue new requests until ResumePlaying
			return;
		case EPlayingUnderrun:
			break;
				
		default:
			Panic(EBadState);
			break;
		}

	// iState is now either EStopped, EPlaying or EPlayingPausedInHw
    __ASSERT_DEBUG(((iState == EStopped) || (iState == EPlaying) || (iState == EPlayingPausedInHw) || (iState == EPlayingUnderrun)), Panic(EBadState));

	while( (iClientPlayData.Length() != 0) && (! iFreePlayers.IsEmpty()))
		{
		// More data to play and more players,  so issue another request 

		bool wasIdle = iFreePlayers.IsFull();
		// Get a free player		
		CPlayer *player = iFreePlayers.Pop();
		// Calculate length of request
		TUint32 lengthToPlay = iClientPlayData.Length();
		if(lengthToPlay > iDeviceBufferLength)
		    {
            lengthToPlay = iDeviceBufferLength;
		    }

		// Remember request length, so we can update bytes played when it finishes
		iActivePlayRequestSizes.Push(lengthToPlay);

		// Find offset to copy data to
		TUint playerIndex = player->GetPlayerIndex();
		ASSERT(playerIndex < KPlaySharedChunkBuffers);
		TUint chunkOffset = iPlayBufferConfig.iBufferOffsetList[playerIndex];

		// Copy data
		TPtr8 destPtr(iPlayChunk.Base()+ chunkOffset, 0, iDeviceBufferLength);
		destPtr.Copy(iClientPlayData.Mid(0, lengthToPlay));

		// Update iClientPlayData to remove the data just queued
		iClientPlayData.Set(iClientPlayData.Right(iClientPlayData.Length()-lengthToPlay));

		// Start the CPlayer
		player->PlayData(chunkOffset, lengthToPlay);
		if(wasIdle)
			{
			iState = EPlaying;
			iStartTime = CurrentTimeInMsec();
			
			}
		
		}

	// Check if the client request is now complete
	if(iClientPlayData.Length() == 0 && iClientPlayStatus)
		{
		// We have queued all the client play data to the driver so we can now complete the client request.
		// If actual playback fails, we will notify the client via the Play Error notification mechanism.
		#ifdef SYMBIAN_SOUNDADAPTER_DEBUG	
		RDebug::Printf("RMdaDevSound::CBody::StartPlayersAndUpdateState completing client request");
		#endif
		User::RequestComplete(iClientPlayStatus, KErrNone); // This call also sets iClientPlayStatus to NULL
		}
	
    //nb. iState is now either EStopped, EPlaying or EPlayingPausedInHw (see previous switch and assert)
	if(iState != EPlayingPausedInHw)
	    {
        if(iFreePlayers.IsFull())
            {
            // Free fifo is full, therefore there are no active players
            iState = EPlayingUnderrun;
			if(! iUnderFlowReportedSinceLastPlayOrRecordRequest)
				{
				// We report KErrUnderflow if we have not already reported it since the last PlayData call.
				// Note that 
				// i) We do NOT report driver underflows.
				// ii) We report underflow when we run out of data to pass to the driver.
				// iii) We throttle this reporting
				// iv) We WILL report KErrUnderflow if already stopped and asked to play a zero length buffer
				// The last point is required because the client maps a manual stop command into a devsound play with a 
				// zero length buffer and the last buffer flag set, this in turn is mapped to a Playdata calll with an empty buffer
				// which is expected to complete ok and cause a KErrUnderflow error to be reported...
				iUnderFlowReportedSinceLastPlayOrRecordRequest = ETrue;
				#ifdef SYMBIAN_SOUNDADAPTER_DEBUG	
		        RDebug::Printf("RMdaDevSound::CBody::StartPlayersAndUpdateState stopped and iUnderFlowReportedSinceLastPlayOrRecordRequest false so raising KErrUnderflow");
				#endif
				
				// Discard any saved trailing data (ie. data saved due driver requiring all requests to be a multiple of iRequestMinSize).
				// This maybe because client is delibrately letting us underflow to play silence. In that case we do not want to
				// play the trailing data at the beginning of the new data issued after the silence...
				iSavedTrailingData.SetLength(0);

				SoundDeviceError(KErrUnderflow);
				}
            }
        else
            {
            // Free fifo not full, therefore there are active players
            iState = EPlaying;
            }
	    }
	return;
	}

TInt RMdaDevSound::CBody::SetFormat(const TCurrentSoundFormatBuf& aFormat, 
									RSoundSc& aSoundDevice,
									TFormatData &aFormatData)
	{
	TInt err = KErrNotFound;
	TCurrentSoundFormatV02Buf formatBuf;
	
	delete aFormatData.iConverter; 
	aFormatData.iConverter = NULL; // setting this to NULL indicates we are not using converter. No other flag
	iConvertedPlayData.Close();
	
	TInt wantedRate = aFormat().iRate;
	for(TInt index = 0; index < KNumSampleRates; index++ )
		{
		if(wantedRate == KRateEnumLookup[index].iRate)
			{
			formatBuf().iRate = KRateEnumLookup[index].iRateEnum;
			aFormatData.iSampleRate = wantedRate;
			err = KErrNone;
			break;
			}
		}
	
	if(err == KErrNone)
		{
		// Assume, for now, we support the requested channels and rate
		aFormatData.iActualChannels = aFormatData.iRequestedChannels;
		aFormatData.iActualRate = aFormatData.iSampleRate;

		// Attempt to configure driver
		formatBuf().iChannels = aFormat().iChannels;
		formatBuf().iEncoding = ESoundEncoding16BitPCM;
		formatBuf().iDataFormat = ESoundDataFormatInterleaved;
		err = aSoundDevice.SetAudioFormat(formatBuf);
        #if defined(SYMBIAN_SOUNDADAPTER_FORCECDRATES) || defined (SYMBIAN_SOUNDADAPTER_FORCESTEREO)
            err = KErrNotSupported; // force Negotiate - for debugging
        #endif
		if (err==KErrNotSupported)
			{
			// don't support directly. Perhaps can rate convert?
			err = NegotiateFormat(aFormat, aSoundDevice, aFormatData);
			}
		}
	return err;	
	}
	
TInt RMdaDevSound::CBody::NegotiateFormat(const TCurrentSoundFormatBuf& aFormat, 
										  RSoundSc& aSoundDevice, 
										  TFormatData &aFormatData)
	{
	ASSERT(!aFormatData.iConverter); // we don't clear on fail - so assuming NULL to start with
	
	TInt err = KErrNotFound;
	TCurrentSoundFormatV02Buf formatBuf;

	// find out first what the driver supports
	TSoundFormatsSupportedV02Buf supportedFormat;
	aSoundDevice.Caps(supportedFormat);
	TUint32 supportedRates = supportedFormat().iRates;
    #ifdef SYMBIAN_SOUNDADAPTER_FORCECDRATES
        supportedRates &= KSoundRate11025Hz| KSoundRate22050Hz | KSoundRate44100Hz; // only use CD rates - for debugging
    #endif
	
	// For PlayCase:
	// 		first try to find the first rate below or equal to the requested that is supported
	// 		initially go down and be fussy, but if we pass the requested rate find the first that
	// 		is supported
	// For RecordCase:
	//		We want the next rate above consistently - we go down from this to the requested rate.
	//		If there is one, we don't support - we _never_ upsample.
	// note that the table is given in descending order, so we start with the highest
	TInt wantedRate = aFormat().iRate;
	TInt takeTheFirst = EFalse;
	TInt nextUpValidIndex = -1;
	for(TInt index = 0; index < KNumSampleRates; index++ )
		{
		TBool lookingAtRequestedRate = wantedRate == KRateEnumLookup[index].iRate;
		TSoundRate wantedEnum = KRateEnumLookup[index].iRateEnum;
		TUint32 equivBitmap = KRateEnumLookup[index].iRateConstant;
		TBool isSupported = (equivBitmap & supportedRates) != EFalse;
		if (lookingAtRequestedRate || takeTheFirst)
			{
			if (isSupported)
				{
				// this rate is supported
				formatBuf().iRate = wantedEnum;
				aFormatData.iActualRate = KRateEnumLookup[index].iRate;
				err = KErrNone;
				break;				
				}
			}
		else if (!takeTheFirst)
			{
			// while we are still looking for the rate, want to cache any supported index
			// at end of loop, this will be the first rate above ours that is supported
			// use for fallback if required
			if (isSupported)
				{
				nextUpValidIndex = index;
				}
			}
		if (lookingAtRequestedRate)
			{
			// if we get this far we've gone passed the wanted rate. For play we enable
			// "takeTheFirst". For record we just abort.
			if (&aSoundDevice==&iPlaySoundDevice)
				{
				takeTheFirst = ETrue;
				}
			else
				{
				break;
				}
			}
		}
		
	if (err)
		{
		// if there is one above the requested rate, use that
		if (nextUpValidIndex>=0)
			{
			TSoundRate wantedEnum = KRateEnumLookup[nextUpValidIndex].iRateEnum;
			formatBuf().iRate = wantedEnum;
			aFormatData.iActualRate = KRateEnumLookup[nextUpValidIndex].iRate;
			err = KErrNone;		
			}
		}
		
	if (err)
		{
		// should have something!
		return err;
		}
		
	aFormatData.iSampleRate = wantedRate; // iSampleRate is our requested/apparent rate, not the device rate.
	
	TUint32 channelsSupported = supportedFormat().iChannels;
    #ifdef SYMBIAN_SOUNDADAPTER_FORCESTEREO
        channelsSupported &= KSoundStereoChannel; // don't use mono - for debugging
    #endif
	if(KSoundAdapterForceStereo==1)
	    {
	    channelsSupported &= KSoundStereoChannel;
#ifdef SYMBIAN_SOUNDADAPTER_DEBUG
	    RDebug::Print(_L("Added stereo support."));
#endif
	    }
	if (aFormat().iChannels == 1)
		{
		aFormatData.iRequestedChannels = 1;
		// want mono
		if (channelsSupported & KSoundMonoChannel)
			{
			// mono is supported, as usual
			aFormatData.iActualChannels = 1;
			}
		else if (channelsSupported & KSoundStereoChannel)
			{
			aFormatData.iActualChannels = 2;
			}
		else
			{
			return KErrNotSupported; // should not get this far for real
			}
		}
	else if (aFormat().iChannels == 2)
		{
		aFormatData.iRequestedChannels = 2;
		// want stereo
		if (channelsSupported & KSoundStereoChannel)
			{
			// stereo is supported, as usual
			aFormatData.iActualChannels = 2;
			}
		else if (channelsSupported & KSoundMonoChannel)
			{
			aFormatData.iActualChannels = 1;
			}
		else
			{
			return KErrNotSupported; // should not get this far for real
			}
		}
	else
		{
		return KErrNotSupported; // unknown number of channels requested!
		}
	
	formatBuf().iChannels = aFormatData.iActualChannels;
	
	formatBuf().iEncoding = ESoundEncoding16BitPCM;
	formatBuf().iDataFormat = ESoundDataFormatInterleaved;
	err = aSoundDevice.SetAudioFormat(formatBuf);
	
	if (!err)
		{
		ASSERT(!aFormatData.iConverter); // pre-condition at top of function anyway
		if (&aSoundDevice==&iPlaySoundDevice)
			{
            #ifdef SYMBIAN_SOUNDADAPTER_DEBUG
                RDebug::Print(_L("RMdaDevSound::CBody::NegotiateFormat: Convert:CreateL from %d/%d to %d/%d"), 
                            aFormatData.iSampleRate, aFormatData.iRequestedChannels, 
                            aFormatData.iActualRate, aFormatData.iActualChannels);
            #endif																	       
			// when playing we convert from requested to actual
			TRAP(err, aFormatData.iConverter = CChannelAndSampleRateConverter::CreateL(aFormatData.iSampleRate, 
																		   aFormatData.iRequestedChannels, 
																	       aFormatData.iActualRate, 
																	       aFormatData.iActualChannels));
			}
		else
			{
			// when recording we convert from actual to requested
			TInt outputRateToUse = aFormatData.iSampleRate;
            #ifdef SYMBIAN_SKIP_RESAMPLE_ON_RECORD
                // with this macro just channel convert at most
                outputRateToUse = aFormatData.iActualRate;
            #endif
            #ifdef SYMBIAN_SOUNDADAPTER_DEBUG
                RDebug::Print(_L("RMdaDevSound::CBody::NegotiateFormat: Convert:CreateL from %d/%d to %d/%d"), 
                            aFormatData.iActualRate, aFormatData.iActualChannels,
                            aFormatData.iSampleRate, aFormatData.iRequestedChannels); 
            #endif																	       
			TRAP(err, aFormatData.iConverter = CChannelAndSampleRateConverter::CreateL(aFormatData.iActualRate, 
																	       aFormatData.iActualChannels,
																	       outputRateToUse, 
																		   aFormatData.iRequestedChannels));
			}
		}
	if(err != KErrNone)
		{
		delete aFormatData.iConverter;
		aFormatData.iConverter= NULL;
		iConvertedPlayData.Close();
		}
	
	return err;
	}

void RMdaDevSound::CBody::StartRecordRequest()
	{
	__ASSERT_DEBUG(iRecordSoundDevice.Handle(), Panic(EDeviceNotOpened));
	
	iRecorder->RecordData(iBufferLength);
	}

// Note both InRecordMode and InPlayMode return EFalse for ENotReady and EStopped
TBool RMdaDevSound::CBody::InRecordMode()const
	{
	switch(iState)
		{
		case ENotReady:
		case EStopped:
			return EFalse;
			
		case ERecording:
		case ERecordingPausedInHw:
		case ERecordingPausedInSw:
			return ETrue;
			
		case EPlaying:
		case EPlayingPausedInHw: 
		case EPlayingPausedInSw:
		case EPlayingUnderrun:
			return EFalse;
			
		default:
			Panic(EBadState);
			break;
		}
	return EFalse;
	}

TBool RMdaDevSound::CBody::InPlayMode() const
	{
	switch(iState)
		{
		case ENotReady:
		case EStopped:
			return EFalse;
			
		case ERecording:
		case ERecordingPausedInHw:
		case ERecordingPausedInSw:
			return EFalse;
			
		case EPlaying:
		case EPlayingPausedInHw: 
		case EPlayingPausedInSw:
		case EPlayingUnderrun:
			return ETrue;
			
		default:
			Panic(EBadState);
			break;
		}
	
	return EFalse;
	}


TUint32 RMdaDevSound::CBody::CurrentTimeInMsec() const
	{
	TUint64 tmp = User::NTickCount();
	tmp *= iNTickPeriodInUsec;
	tmp /= 1000;
	return TUint32(tmp);
	}

void RMdaDevSound::CBody::PlayFormatsSupported(TSoundFormatsSupportedBuf& aFormatsSupported)
	{
	__ASSERT_DEBUG(iPlaySoundDevice.Handle(), Panic(EDeviceNotOpened));
	FormatsSupported(aFormatsSupported, iPlaySoundDevice);
	}
	
void RMdaDevSound::CBody::GetPlayFormat(TCurrentSoundFormatBuf& aFormat)
	{
	__ASSERT_DEBUG(iPlaySoundDevice.Handle(), Panic(EDeviceNotOpened));
	GetFormat(aFormat, iPlaySoundDevice, iPlayFormatData);
	}
	
TInt RMdaDevSound::CBody::SetPlayFormat(const TCurrentSoundFormatBuf& aFormat)
	{
	__ASSERT_DEBUG(iPlaySoundDevice.Handle(), Panic(EDeviceNotOpened));
	return SetFormat(aFormat, iPlaySoundDevice, iPlayFormatData);
	}

void RMdaDevSound::CBody::RecordFormatsSupported(TSoundFormatsSupportedBuf& aFormatsSupported)
	{
	__ASSERT_DEBUG(iRecordSoundDevice.Handle(), Panic(EDeviceNotOpened));
	FormatsSupported(aFormatsSupported, iRecordSoundDevice);
	}

void RMdaDevSound::CBody::GetRecordFormat(TCurrentSoundFormatBuf& aFormat)
	{
	__ASSERT_DEBUG(iRecordSoundDevice.Handle(), Panic(EDeviceNotOpened));
	GetFormat(aFormat, iRecordSoundDevice, iRecordFormatData);	
	}

TInt RMdaDevSound::CBody::SetRecordFormat(const TCurrentSoundFormatBuf& aFormat)
	{
	__ASSERT_DEBUG(iRecordSoundDevice.Handle(), Panic(EDeviceNotOpened));
	return SetFormat(aFormat, iRecordSoundDevice, iRecordFormatData);
	}
	
void RMdaDevSound::CBody::Close()
	{
    #ifdef SYMBIAN_SOUNDADAPTER_DEBUG
        RDebug::Printf("void RMdaDevSound::CBody::Close() started");
    #endif
	iBufferOffset = -1;
	iBufferLength = 0;

	if(iPlaySoundDevice.Handle() != KNullHandle)
	    {
        // Make sure all player objects are idle
        CancelPlayData();
        iPlayChunk.Close();
        iPlaySoundDevice.Close();
	    }

    if(iRecordSoundDevice.Handle() != KNullHandle)
        {
        CancelRecordData();
        iRecordChunk.Close();
        iRecordSoundDevice.Close();
        }
	
	iState = ENotReady;
    #ifdef SYMBIAN_SOUNDADAPTER_DEBUG
        RDebug::Printf("void RMdaDevSound::CBody::Close() ended");
    #endif
	}

TInt RMdaDevSound::CBody::Handle()
	{//This method is actually used to check whether the device is opened. Below logic should work
	if(iPlaySoundDevice.Handle())
		{
		return iPlaySoundDevice.Handle();
		}
	if(iRecordSoundDevice.Handle())
		{
		return iRecordSoundDevice.Handle();
		}
	return 0;
	}


void RMdaDevSound::CBody::PlayData(TRequestStatus& aStatus, const TDesC8& aData)
	{
	#ifdef SYMBIAN_SOUNDADAPTER_DEBUG
    RDebug::Printf("RMdaDevSound::CBody::PlayData(0x%x,%d) State=%s Handle=%d.",&aStatus, 
                   aData.Length(), iState.Name(), iPlayChunk.Handle());
	#endif
	
	__ASSERT_DEBUG(iPlaySoundDevice.Handle(), Panic(EDeviceNotOpened));
	aStatus = KRequestPending;

	if((iClientPlayStatus != NULL) || InRecordMode())
		{
		// We only support one outstanding request
		// No support for simultaneous play and record in RMdaDevSound
		TRequestStatus *pRequest = &aStatus;
		User::RequestComplete(pRequest, KErrInUse);
		return;
		}
	iClientPlayStatus = &aStatus;//store the status of datapath player

	if(iPlayFormatData.iConverter || iSavedTrailingData.Length() != 0)
		{
		// Need a conversion buffer
        // Needs to hold any trailing data truncated from the previous request (due
        // to alignment requirements) and either the new data, or the new rate adapted data
		TUint32 spaceRequired = iSavedTrailingData.Length();
		if(iPlayFormatData.iConverter)
			{
			// Doing rate conversion so also need space for the converted data
			spaceRequired += iPlayFormatData.iConverter->MaxConvertBufferSize(aData.Length());
			}
		else
			{
			// Not doing rate adaptation therefore only need to allow for the new incoming data
			spaceRequired += aData.Length();
			}
		// Check if existing buffer exists and is big enough
		if(iConvertedPlayData.MaxLength() < spaceRequired)
			{
			iConvertedPlayData.Close();
			TInt err = iConvertedPlayData.Create(spaceRequired);
			if(err)
				{
				User::RequestComplete(iClientPlayStatus, err);
				return;
				}
			}

		// Truncate iConvertedPlayData and copy in saved trailing data (if any)
		iConvertedPlayData = iSavedTrailingData;
		iSavedTrailingData.SetLength(0);
		
		// Now append rate adapted data or incoming data
		if (iPlayFormatData.iConverter)
			{
            // The convertor will panic if it fails to convert any data, therefore
            // we avoid passing it an empty source buffer
			if(aData.Length() != 0)
				{
                TPtr8 destPtr((TUint8 *)iConvertedPlayData.Ptr()+iConvertedPlayData.Length(), 0, iConvertedPlayData.MaxLength()-iConvertedPlayData.Length());
				TInt len = iPlayFormatData.iConverter->Convert(aData, destPtr);
				iConvertedPlayData.SetLength(iConvertedPlayData.Length() + destPtr.Length());
				if(len != aData.Length())
					{
					#ifdef SYMBIAN_SOUNDADAPTER_DEBUG
					RDebug::Printf("RMdaDevSound::CBody::PlayData converted %d	but expected to convert %d", len, aData.Length());
					#endif
					}
				}
			}
		else
			{
			iConvertedPlayData.Append(aData);
			}
		iClientPlayData.Set(iConvertedPlayData);
		}
	else
		{
		// Do not need a conversion buffer so just aim the descriptor at the data
		iClientPlayData.Set(aData);
		}
	iUnderFlowReportedSinceLastPlayOrRecordRequest = EFalse;

	// All driver requests must be an exact multiple of iRequestMinSize
	TUint32 trailingDataLen = iClientPlayData.Length() % iRequestMinSize;
	if(trailingDataLen)
		{
		// Not a multiple of iRequestMinSize, so need to truncate current request, and save trailing bytes for 
		// inclusion at the beginning of the next request
		iSavedTrailingData = iClientPlayData.Right(trailingDataLen);
		iClientPlayData.Set(iClientPlayData.Left(iClientPlayData.Length()-trailingDataLen));
		}

    #ifdef SYMBIAN_FORCE_32BIT_LENGTHS
	if (iClientPlayData.Length()%4 != 0)
	    {
        // simulate the limitation of some hardware, where -6 is generated if the
        // buffer length is not divisible by 4.
        TRequestStatus *pRequest = &aStatus;
        User::RequestComplete(pRequest, KErrArgument);
	}
    #endif

	iRecordChunk.Close();
	if(!iPlayChunk.Handle())
		{
		//This is where we setup to play. 
		//Configure the shared chunk for two buffers with iBufferSize each
		iPlayBufferConfig.iNumBuffers = KPlaySharedChunkBuffers;
		iDeviceBufferLength = KPlaySharedChunkBufferSize;
		#ifdef SYMBIAN_SOUNDADAPTER_DEBUG	
		RDebug::Printf("iDeviceBufferLength %d", iDeviceBufferLength);
		#endif
		iPlayBufferConfig.iFlags = 0;//data will be continuous
		// If required, use rate converter etc
		iPlayBufferConfig.iBufferSizeInBytes = iDeviceBufferLength;
        #ifdef SYMBIAN_SOUNDADAPTER_DEBUG
            RDebug::Printf("number of buffers: [%d]",iPlayBufferConfig.iNumBuffers);
            RDebug::Printf("BufferSize in Bytes [%d]",iPlayBufferConfig.iBufferSizeInBytes);
        #endif
		TPckg<TPlaySharedChunkBufConfig> bufferConfigBuf(iPlayBufferConfig);
		TInt error = iPlaySoundDevice.SetBufferChunkCreate(bufferConfigBuf,iPlayChunk);
		if(error == KErrNone)
			{
			iPlaySoundDevice.GetBufferConfig(bufferConfigBuf);
			}
		if (error)
			{
			SoundDeviceError(error);
			return;
			}
		}

    StartPlayersAndUpdateState();

	return;	
	}

void RMdaDevSound::CBody::RecordData(TRequestStatus& aStatus, TDes8& aData)
	{
	__ASSERT_DEBUG(iRecordSoundDevice.Handle(), Panic(EDeviceNotOpened));
	aStatus = KRequestPending;
	if((iClientPlayStatus != NULL) || InPlayMode())
		{
		// We only support one outstanding request
		// No support for simultaneous play and record in RMdaDevSound
		TRequestStatus *pRequest = &aStatus;
		User::RequestComplete(pRequest, KErrInUse);
		return;
		}
	iClientRecordStatus = &aStatus;
	iClientRecordData = &aData;
	iUnderFlowReportedSinceLastPlayOrRecordRequest = EFalse;

	iPlayChunk.Close();
	if(!iRecordChunk.Handle())
		{
		//Configure the shared chunk for two buffers with iBufferSize each
		iRecordBufferConfig.iNumBuffers = KRecordMaxSharedChunkBuffers;
		iDeviceBufferLength = KRecordSharedChunkBufferSize; // initial size - resize if needs be
		if (iRecordFormatData.iConverter)
			{
			// if number of channels used differs from request, resize buffer
			// assume we have nice rounded values for buffer.
			if (iRecordFormatData.iActualChannels>iRecordFormatData.iRequestedChannels)
				{
				iDeviceBufferLength *= 2; // will record at stereo and convert to mono 
				}
			else if (iRecordFormatData.iActualChannels<iRecordFormatData.iRequestedChannels)
				{
				iDeviceBufferLength /= 2; // will record at mono and convert to stereo 
				}
			}
		iRecordBufferConfig.iBufferSizeInBytes = iDeviceBufferLength;
		iRecordBufferConfig.iFlags = 0;
		TPckg<TRecordSharedChunkBufConfig> bufferConfigBuf(iRecordBufferConfig);
		TInt error = iRecordSoundDevice.SetBufferChunkCreate(bufferConfigBuf,iRecordChunk);
		if(error == KErrNone)
			{
			iRecordSoundDevice.GetBufferConfig(bufferConfigBuf);
			}
		else
			{
			SoundDeviceError(error);
			return;
			}
		iState = ERecording;
		}		
    #ifdef SYMBIAN_SOUNDADAPTER_DEBUG
        RDebug::Printf("RMdaDevSound::CBody::RecordData,iBufferOffset[%d]",iBufferOffset);
    #endif

	switch(iState)
		{
		case ENotReady:
			Panic(EBadState);
			break;

		case EStopped:
		case ERecording:
			// Either idle or recording is in progress, therefore we can issue another request			
			StartRecordRequest();
			break;
			
		case ERecordingPausedInHw:
			// Driver is paused, therefore we can issue a request which will immediately return buffered data
			// or be aborted (in the driver) with KErrCancelled if there is no more data). nb. That KErrCancelled should not be
			// returned to the client because the old RMdaDevSound driver would have completed with KErrNone and zero data length.
			StartRecordRequest();
			break;

		case ERecordingPausedInSw:
			// Paused in s/w but driver is not paused, therefore can not issue a new request to driver because
			// it would re-start recording.
			// This implies we were paused whilst the h/w was not recording, so there is no buffered data.
			
			// Complete the request with KErrNone and no data.
			iClientRecordData->SetLength(0);
			User::RequestComplete(iClientRecordStatus, KErrNone);
			break;
			
		case EPlaying:
		case EPlayingPausedInHw:
		case EPlayingPausedInSw: 
		case EPlayingUnderrun:
			Panic(EBadState);
			break;
			
		default:
			Panic(EBadState);
			break;
		}
	}
	
/**
	Notify client of error.
	
	Note that we continue playing/recording if possible.
	
	We do not maintain information which could map the error back to a particular client play/record request
	and therefore we have to notify the client of error every time it happens.
	
	nb. A client play/record request is completed with KErrNone if it queues ok - All errors are reported via the Notify*Error
	mechanism.
 */
void RMdaDevSound::CBody::SoundDeviceError(TInt aError)
	{
    #ifdef SYMBIAN_SOUNDADAPTER_DEBUG
	RDebug::Printf("RMdaDevSound::CBody::SoundDeviceError: Error[%d] state %s", aError, iState.Name());
    #endif

	ASSERT(aError != KErrNone);
	
	if(iClientPlayErrorStatus)
		{
        #ifdef SYMBIAN_SOUNDADAPTER_DEBUG
            RDebug::Printf("RMdaDevSound::CBody::SoundDeviceError Completing iPlayerErrorStatus");
        #endif

		User::RequestComplete(iClientPlayErrorStatus, aError); // nb call also sets iClientPlayErrorStatus to NULL
		}

  	if(iClientRecordErrorStatus)
		{
        #ifdef SYMBIAN_SOUNDADAPTER_DEBUG
            RDebug::Printf("RMdaDevSound::CBody::SoundDeviceError Completing iClientRecordErrorStatus");
        #endif
		User::RequestComplete(iClientRecordErrorStatus, aError); // nb call also sets iClientRecordErrorStatus to NULL
		}

	return;
	}

void RMdaDevSound::CBody::NotifyRecordError(TRequestStatus& aStatus)
	{
	aStatus = KRequestPending;
	iClientRecordErrorStatus = &aStatus;
	}

void RMdaDevSound::CBody::NotifyPlayError(TRequestStatus& aStatus)
	{
	aStatus = KRequestPending;
	iClientPlayErrorStatus = &aStatus;
	}

void RMdaDevSound::CBody::CancelNotifyPlayError()
	{
	if(iClientPlayErrorStatus)
		{
		User::RequestComplete(iClientPlayErrorStatus, KErrCancel);
		}
	}

void RMdaDevSound::CBody::CancelNotifyRecordError()
	{
	if(iClientRecordErrorStatus)
		{
		User::RequestComplete(iClientRecordErrorStatus, KErrCancel);
		}
	else
	    {
		#ifdef SYMBIAN_SOUNDADAPTER_DEBUG	
        RDebug::Printf("msp BufferEmptied but iClientPlayStatus==NULL");
		#endif
	    }
	}

void RMdaDevSound::CBody::FlushPlayBuffer()
	{
	#ifdef SYMBIAN_SOUNDADAPTER_DEBUG	
    RDebug::Printf("RMdaDevSound::CBody::FlushPlayBuffer calling CancelPlayData");
	#endif	
	CancelPlayData();
	}

RSoundSc& RMdaDevSound::CBody::PlaySoundDevice()
	{
	return iPlaySoundDevice;
	}

RSoundSc& RMdaDevSound::CBody::RecordSoundDevice()
	{
	return iRecordSoundDevice;
	}
	
const RMdaDevSound::CBody::TState &RMdaDevSound::CBody::State() const
	{
	return iState;
	}


void RMdaDevSound::CBody::BufferFilled(TInt aBufferOffset)
	{
    #ifdef SYMBIAN_SOUNDADAPTER_DEBUG	
        RDebug::Print(_L("RMdaDevSound::CBody::BufferFilled:"));
    #endif	

	ASSERT(aBufferOffset>=0 || aBufferOffset==KErrCancel);
	ASSERT(iClientRecordData); // request should not get this without

	if(aBufferOffset==KErrCancel)
		{
		//we can get KErrCancel when we call pause and there is no more data left with the driver
		//we send the empty buffer to the HwDevice, where this should trigger the shutdown mechanism
		iClientRecordData->SetLength(0);
		User::RequestComplete(iClientRecordStatus, KErrNone);
		iClientRecordStatus = NULL;
		return;
		}
		
	iBufferOffset = aBufferOffset;
	//when last buffer is flushed, new driver sometimes gives buffer size of odd number. One of our codecs
	//expects that the buffer size should always be even. Base suggested that we fix in multimedia
	//as it is quite complicated to fix in overthere.
	iBufferLength = iBufferLength & 0xfffffffe;
	TPtr8 dataPtr(iRecordChunk.Base()+ iBufferOffset, iBufferLength, iClientRecordData->MaxLength());
	if (iRecordFormatData.iConverter)
		{
		iRecordFormatData.iConverter->Convert(dataPtr, *iClientRecordData);
		}
	else
		{
		iClientRecordData->Copy(dataPtr);
		}
    #ifdef SYMBIAN_SOUNDADAPTER_DEBUG
        RDebug::Print(_L("RMdaDevSound::CBody::BufferFilled: BufferOffset[%d] BufferLen[%d]"), iBufferOffset, iBufferLength);
    #endif
	if(iBufferOffset >= 0)
		{
		iRecordSoundDevice.ReleaseBuffer(iBufferOffset);
		}
	if(iClientRecordStatus)
		{
		User::RequestComplete(iClientRecordStatus, KErrNone);
		iClientRecordStatus = NULL;
		}
	else
	    {
        RDebug::Printf("msp PlayCancelled but iClientPlayStatus==NULL");
	    }
	}
		
/*
	This function is called to notify us that a CPlayer's request has completed and what its status was.

	It is important to note that:-
	1) RSoundSc driver PlayData requests are guaranteed to complete in order, oldest first
	2) If we are overloaded, it is possible for more than one request to complete before any CPlayer::RunL is ran. In
	this situation the CPlayer::RunL functions, and hence this callback, maybe invoked in non-oldest first order

	but

	a) It is impossible for callback for the second oldest CPlayer to occur before the driver request for the oldest has
	been complete (because of 1)
	b) We will always get exactly one callback for every complete request.

	Therefore this callback notifies us of two subtly separate things:-

	i) The oldest request has been completed (so we can reduce can increase the bytes played counter by its length
	ii) CPlayer aPlayerIndex is free for re-use

	but we can not assume that aPlayerIndex is the oldest request, therefore we save the play request lengths outside of
	the CPlayer object.
*/
void RMdaDevSound::CBody::PlayRequestHasCompleted(CPlayer *aPlayer, TInt aStatus, TBool aDueToCancelCommand)
	{
	// CPlayer is done so put it on the free queue
	iFreePlayers.Push(aPlayer);

	TUint32 bytesPlayed = iActivePlayRequestSizes.Pop();
	// Request has finished therefore now timing the following request to simulate bytes played
    iStartTime = CurrentTimeInMsec();
	if(aDueToCancelCommand)
	    {
        // Callback due to CPlayer::Cancel/DoCancel being called, therefore we
        // do not want to update bytes played, process state, report a error or start new players
        return;
	    }
	
	// Update iBytesPlayed by the length of the oldest request (which might not be the one that CPlayer was 
	// handling).
	iBytesPlayed += bytesPlayed;
	#ifdef SYMBIAN_SOUNDADAPTER_DEBUG	
    RDebug::Printf("PlayRequestHasCompleted increasing iBytesPlayed by %d to %d", bytesPlayed, iBytesPlayed);
	#endif
	
    // Process state
	switch(iState)
		{
		case ENotReady:
			Panic(EDeviceNotOpened);
			break;
				
		case EStopped:
			// Will happen if we are doing CancelPlayData processing with active players
			break;
		
		case ERecording:
		case ERecordingPausedInHw:
		case ERecordingPausedInSw:
			Panic(EBadState);
			break;
			
		case EPlaying:
			// Normal situation
			break;

		case EPlayingPausedInHw: 
			// H/W was/is paused, but there must have been an already complete request that we had not 
			// processed yet.
			// There must be at least one more pending request on h/w, otherwise the h/w would have refused to pause
			// I would expect this be rare, but it happens quite often...
            #ifdef SYMBIAN_SOUNDADAPTER_DEBUG
			ASSERT(iActivePlayRequestSizes.Length() != 0);
            #endif
			// Need to update the start and pause time to now because we have just updated the actual iBytesPlayed
			// and logically the h/w is paused at the beginning of the next request
			iStartTime = CurrentTimeInMsec();
			iPauseTime = iStartTime;
			break;
		
		case EPlayingPausedInSw:
			// This will happen if there is only a single hw request outstanding, and the hardware has finished it, but the
			// corresponding RunL has not run yet (in which case PausePlayBuffer will have attempted to use h/w pause,
			// but the driver call would have failed, and the state changed to EPlayingPausedInSw).
			iStartTime = CurrentTimeInMsec();
			iPauseTime = iStartTime;
			return;
		case EPlayingUnderrun:
			break;
				
		default:
			Panic(EBadState);
			break;
		}


	// If we have an error, report it to the client
	// We NEVER report driver underflow, instead we report KErrUnderflow if we run out of data to pass to driver.
	if( (aStatus != KErrNone) && (aStatus != KErrUnderflow) )
		{
		SoundDeviceError(aStatus);
		}

    // If appropriate start more players
	StartPlayersAndUpdateState();
	return;
	}

RMdaDevSound::CBody::CPlayer::CPlayer(TInt aPriority, RMdaDevSound::CBody& aParent, TInt aIndex):
	CActive(aPriority), iParent(aParent), iIndex(aIndex), iBufferOffset(-1), iBufferLength(0)
	{
	CActiveScheduler::Add(this);
	}

RMdaDevSound::CBody::CPlayer::~CPlayer()
	{
	Cancel();
	}


void RMdaDevSound::CBody::CPlayer::RunL()
	{
    #ifdef SYMBIAN_SOUNDADAPTER_DEBUG	
    RDebug::Printf("****RMdaDevSound::CBody::CPlayer(%d)::RunL: Error[%d] ParentState[%s]", 
                     iIndex, iStatus.Int(), iParent.State().Name());
	RDebug::Printf("iActivePlayRequestSizes.Length() = %d iFreePlayers.Length() = %d (including this one as active)", 
					iParent.iActivePlayRequestSizes.Length(), 
					iParent.iFreePlayers.Length());
    #endif
	iParent.PlayRequestHasCompleted(this, iStatus.Int(), EFalse);
	return;
	}

TInt RMdaDevSound::CBody::CPlayer::RunError(TInt aError)
	{
	iParent.PlayRequestHasCompleted(this, aError, EFalse);
	return KErrNone;
	}

void RMdaDevSound::CBody::CPlayer::DoCancel()
	{
#ifdef SYMBIAN_SOUNDADAPTER_DEBUG
	RDebug::Printf("RMdaDevSound::CBody::CPlayer(%d)::DoCancel", iIndex);
#endif
	if(iStatus == KRequestPending)
	    {
        // Avoid cancelling requests which have already completed.
        // It wastes time, and might provoke a sound driver problem
	    #ifdef SYMBIAN_SOUNDADAPTER_DEBUG	
        RDebug::Printf("RMdaDevSound::CBody::CPlayer::DoCancel - would have cancelled driver request");
		#endif
        iParent.PlaySoundDevice().Cancel(iStatus);
	    }
	iParent.PlayRequestHasCompleted(this, KErrCancel, ETrue);
	}

void RMdaDevSound::CBody::CPlayer::PlayData(TUint aChunkOffset, TInt aLength)
	{
    #ifdef SYMBIAN_SOUNDADAPTER_DEBUG	
	RDebug::Print(_L("RMdaDevSound::CBody::CPlayer(%d)::PlayData : IsActive[%d]"),
				  iIndex,    IsActive());
	RDebug::Printf("iActivePlayRequestSizes.Length() = %d iFreePlayers.Length() = %d (inc this player)", 
					iParent.iActivePlayRequestSizes.Length(), 
					iParent.iFreePlayers.Length());
    #endif	
	
	iBufferOffset = aChunkOffset;
	iBufferLength = aLength;

    //Make sure the length is a multiple of 4 to work around an h6 limitation.
	iBufferLength = iBufferLength & 0xfffffffc;

	// Issue the RSoundSc request
	iParent.PlaySoundDevice().PlayData(iStatus, iBufferOffset, iBufferLength, EFalse);
	SetActive();
	return;
	}
	
TUint RMdaDevSound::CBody::CPlayer::GetPlayerIndex() const
	{
	return iIndex;
	}

RMdaDevSound::CBody::CRecorder::CRecorder(TInt aPriority, RMdaDevSound::CBody& aParent):
    CActive(aPriority), iParent(aParent), iBufferOffset(-1), iBufferLength(0)
    {
    CActiveScheduler::Add(this);
    }

RMdaDevSound::CBody::CRecorder::~CRecorder()
    {
    Cancel();
    }

void RMdaDevSound::CBody::CRecorder::RecordData(TInt& aBufferLength)
	{
	if (!IsActive())
	    {
	    iStatus = KRequestPending;
        #ifdef SYMBIAN_SOUNDADAPTER_DEBUG
            RDebug::Printf("Recording request: BufferLength[%d]", aBufferLength);
        #endif
	    iParent.RecordSoundDevice().RecordData(iStatus, aBufferLength);
	    SetActive();
	    }
	}

void RMdaDevSound::CBody::CRecorder::RunL()
	{
    #ifdef SYMBIAN_SOUNDADAPTER_DEBUG	
    RDebug::Printf("****RMdaDevSound::CBody::CRecorder()::RunL: Error[%d] ParentState[%s]", 
                     iStatus.Int(), iParent.State().Name());
    #endif

	
	TInt error = iStatus.Int();
	
	if((error >= 0) || (error == KErrCancel))
		{//we can get KErrCancel when we call pause and there is no more data left with the driver
		iParent.BufferFilled(error);
		}
	else 
		{
        #ifdef SYMBIAN_SOUNDADAPTER_DEBUG	
            RDebug::Print(_L("RMdaDevSound::CBody::CPlayer()::RunL: Error[%d]"), error);
        #endif
		iParent.SoundDeviceError(error);
		}
	}

	
TInt RMdaDevSound::CBody::CRecorder::RunError(TInt aError)
    {
    iParent.SoundDeviceError(aError);
    return KErrNone;
    }

void RMdaDevSound::CBody::CRecorder::DoCancel()
    {
#ifdef SYMBIAN_SOUNDADAPTER_DEBUG
    RDebug::Printf("RMdaDevSound::CBody::CRecorder()::DoCancel");
#endif
    iParent.RecordSoundDevice().Cancel(iStatus);
    }


// End of file

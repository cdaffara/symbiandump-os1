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
// source\server\mmfdatapath2.cpp
// 
//

#include <e32math.h>
#include <mmf/common/mmffourcc.h>
#include <mmf/common/mmfpaniccodes.h>
#include <mmf/server/mmfaudiooutput.h>
#include <mmf/server/mmfaudioinput.h>
#include "mmfdatapath2.h"
#include "mmfclientaudiostreamutils.h"
#include <mmf/common/mmfaudio.h>
#include <mmf/plugin/mmfcodecimplementationuids.hrh> // KUidMmfCodecAudioSettings
#include <mmf/server/devsoundstandardcustominterfaces.h>
#include <mmf/server/mmffile.h>
#include <mda/client/resource.h>

static void Panic(TMMFDataPathPanicCode aPanicCode, TInt aSourceLineNumber)
	{
	_LIT(KMMFDataPathPanicCategory, "MMFDataPath2");
	User::Panic(KMMFDataPathPanicCategory, STATIC_CAST(TInt,aPanicCode) + aSourceLineNumber);
	}

/**
Allocates and constructs a data path.

Use this function if the codec UID is not already known by CMMFController
and there is no data path ambiguity - ie only one data path is possible.

Will create codec via fourCC.

@param  aEventHandler
        Installs an event handler to provide message passing between clients and sources/sinks.

@return Newly constructed data path object.
*/

EXPORT_C CMMFDataPath2* CMMFDataPath2::NewL(MAsyncEventHandler& aEventHandler)
	{
	CMMFDataPath2* self = new(ELeave) CMMFDataPath2(TMediaId(), aEventHandler);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}


/**
Allocates and constructs a data path according to the specified media ID.

Use this function if the codec UID is not already known by CMMFController
and there is ambiguity with the data path ie. there is more than one possible data path.

@param  aMediaId
        Optional media ID parameter when there are multiple media types.
@param  aEventHandler
        Installs an event handler to provide message passing between clients and sources/sinks.

@return A newly constructed data path object.
*/

EXPORT_C CMMFDataPath2* CMMFDataPath2::NewL(TMediaId aMediaId, MAsyncEventHandler& aEventHandler)
	{
	CMMFDataPath2* self = new(ELeave) CMMFDataPath2(aMediaId, aEventHandler);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

/**
Allocates and constructs a data path according to the specified codec UID.

Use this function if the codec UID is already known by CMMFController
and there is no data path ambiguity ie. only one data path is possible
will create codec explicitly using the supplied codec Uid

@param  aCodecUid
        Optional mediaID parameter when there are multiple media types
@param  aEventHandler
        Installs an event handler to provide message passing between clients and sources/sinks.

@return A newly constructed data path object.
*/

EXPORT_C CMMFDataPath2* CMMFDataPath2::NewL(TUid aCodecUid, MAsyncEventHandler& aEventHandler)
	{
	CMMFDataPath2* self = new(ELeave) CMMFDataPath2(TMediaId(), aEventHandler);
	CleanupStack::PushL(self);
	self->ConstructL(aCodecUid);
	CleanupStack::Pop();
	return self;
	}


/**
Allocates and constructs a data path according to the specified codec UID.

Use this function if the codec UID is already known by CMMFController
and there is ambiguity ie. more than one possible data path.
TMediaId used to select the path.

@param  aCodecUid
		The codec UID.
@param  aMediaId
        Optional mediaID parameter when there are multiple media types.
@param  aEventHandler
        Installs an event handler to provide message passing between clients and sources/sinks.

@return A newly constructed data path object.
*/
EXPORT_C CMMFDataPath2* CMMFDataPath2::NewL(TUid aCodecUid, TMediaId aMediaId, MAsyncEventHandler& aEventHandler)
	{
	CMMFDataPath2* self = new(ELeave) CMMFDataPath2(aMediaId, aEventHandler);
	CleanupStack::PushL(self);
	self->ConstructL(aCodecUid);
	CleanupStack::Pop();
	return self;
	}

CMMFDataPath2::CMMFDataPath2(TMediaId aMediaId, MAsyncEventHandler& aEventHandler) 
	: CMMFDataPath(aMediaId, aEventHandler), iTimeLeftToPlayComplete(-1)
	{
	}
		
void CMMFDataPath2::ConstructL(TUid aCodecUid)
	{
	CMMFDataPath::ConstructL(aCodecUid);
	iRepeatTrailingSilenceTimer = CPeriodic::NewL(CActive::EPriorityStandard); 
	}
	
/**
Standard destructor.
*/

CMMFDataPath2::~CMMFDataPath2()
	{
	if(iRepeatTrailingSilenceTimer)
		{
		iRepeatTrailingSilenceTimer->Cancel();
		delete iRepeatTrailingSilenceTimer;
		}
	}

TInt CMMFDataPath2::RepeatTrailingSilenceTimerComplete(TAny* aDataPath)
	{
	CMMFDataPath2* dataPath = static_cast<CMMFDataPath2*>(aDataPath);
	
	TRAPD(err, dataPath->DoRepeatTrailingSilenceTimerCompleteL());
	if (err != KErrNone)
		{
		dataPath->DoSendEventToClient(KMMFEventCategoryPlaybackComplete, err);
		}	
	return KErrNone;
	}
	
TInt CMMFDataPath2::DoRepeatTrailingSilenceTimerCompleteL()
	{
	//cancel this periodic timer
	iRepeatTrailingSilenceTimer->Cancel();
	if(iTimeLeftToPlayComplete.Int64()>0)
		{
		iTimeLeftToPlayComplete=0;
		}
		
	if (iTrailingSilenceLeftToPlay.Int64() > 0)
		{
		PlaySilence();
		}
	else
		{
		SetPositionL(iPlayWindowStartPosition);
		iTimeLeftToPlayComplete=-1;
		FillSourceBufferL();		
		}
	return KErrNone;
	}
	
void CMMFDataPath2::PlaySilence()
	{		
	// iRepeatTrailingSilenceTimer->After() takes a TTimeIntervalMicroSeconds32
	// so for longer periods of silence call it repeatedly with KMaxTInt lengths
	TTimeIntervalMicroSeconds32 silence;
	if(iTimeLeftToPlayComplete.Int64() > 0)
		{
		silence = I64INT(iTimeLeftToPlayComplete.Int64());
		}
	else if (iTrailingSilenceLeftToPlay.Int64() > KMaxTInt)
		{
		silence = KMaxTInt;
		iTrailingSilenceLeftToPlay = iTrailingSilenceLeftToPlay.Int64() - KMaxTInt;
		}
	else
		{
		silence = I64INT(iTrailingSilenceLeftToPlay.Int64());
		iTrailingSilenceLeftToPlay = 0;
		}
	iRepeatTrailingSilenceTimer->Start(silence, silence , TCallBack(RepeatTrailingSilenceTimerComplete, this));
	}

/* 
 *  FillSourceBufferL
 * 
 *	Function to get data from the datapath's iDataSource
 */

void CMMFDataPath2::DoFillSourceBufferL()
	{
#ifdef _DP_DEBUG
	RDebug::Print(_L("DP::FillSourceBufferL tick-%d   (this 0x%x)\n"),User::TickCount(),this);
#endif

	__ASSERT_DEBUG((iState == EPlaying || iState == EConverting || iState == ERecording || (iState == EPrimed && iPauseCalled && iIsUsingResumeSupport)), Panic(EMMFDataPathPanicBadState,__LINE__)); 

	// clear the no-more-source flag here (as well as in PlayL()) because 
	// there may have been a re-position since the last call to BufferFilledL()
	iNoMoreSourceData = EFalse;

	if(!iObtainingAsyncSourceBuffer) 
		{//this is a normal request for data. 
		//If we are getting asynchronous buffers, then can't do this as iSourceBuffer == NULL
		iSourceBuffer->SetFrameNumber(++iCurrentSourceFrameNumber); //so source knows which data to load buffer with
		iSourceBuffer->SetStatus(EBeingFilled);
		iSourceBuffer->SetLastBuffer(EFalse);
		}

#ifdef _DP_DEBUG
	RDebug::Print(_L("DP asking for buffer %d  - ptr=0x%x   (this 0x%x)\n"), iCurrentSourceFrameNumber, iSourceBuffer,this);	
#endif

	iSourceBufferWithSource = ETrue;

	// wait for BufferFilled callback from source. Do this here as some sources cause
	//re-entrancy into data path via BufferFilledL
	ChangeDataPathTransferState(EWaitSource);  

	iDataSource->FillBufferL(iSourceBuffer, this, iMediaId);

#ifdef _DP_DEBUG
	RDebug::Print(_L("DP::FillSourceBufferL - DONE tick-%d   (this 0x%x)\n"),User::TickCount(),this);
#endif
	}

/**
Runs the clip depending on the current data path and transfer state.

For example, fills the sink buffer if TDataPathState is EPlaying and TTransferState is ENeedSinkData.
*/
void CMMFDataPath2::RunL()
	{
#ifdef _DP_DEBUG 
	RDebug::Print(_L("DP::RunL transfer state %d, iPausedCalled %d, tick-%d   (this 0x%x)\n"),iTransferState, iPauseCalled, User::TickCount(),this);
#endif
	
	switch (iState)
		{
	case EStopped:
		break;
	case EPrimed: // In the paused state we still continue to feed buffers to the sink. The sink (DevSound) handles the logic of whether the buffers should be emptied
	    {
	    if (!iPauseCalled || !iIsUsingResumeSupport)
	        break;
	    }	    
	    // fall-through
	case EPlaying:
	case ERecording:
	case EConverting:
		switch (iTransferState)
			{
		case EWaitSink:
		case EWaitSource:
			break;
		case EInitializeSink:
			InitializeSinkL();
			break;
		case EInitializeSource:
			InitializeSourceL();
			break;
		case ENeedSourceData:
			FillSourceBufferL();
			break;
		case ENeedSinkData:
			FillSinkBufferL();
			break;
		case ENeedToMatchSourceToSink:
			FillSinkBufferL();
			break;
		case ESendDataToSink:
			EmptySinkBufferL();
			break;
		case EEndOfData:
			EndOfData();
			break;
			}
		break;
	default:
		break;
		}
#ifdef _DP_DEBUG
	RDebug::Print(_L("DP::RunL DONE\n"));
#endif
	}
	
/* 
 *  FillSourceBufferL
 * 
 *	Function to get data from the datapath's iDataSource
 */

void CMMFDataPath2::FillSourceBufferL()
	{
	__ASSERT_DEBUG((iState == EPlaying || iState == EConverting || iState == ERecording || (iState == EPrimed && iPauseCalled && iIsUsingResumeSupport) ), Panic(EMMFDataPathPanicBadState,__LINE__)); 

	//if the silence timer is active then dont propagate the request
	if(iRepeatTrailingSilenceTimer->IsActive())
		{
		return;
		}
	
	//play the silence period and dont propagate the request
	if(iTrailingSilenceLeftToPlay>0 || iVerifyPlayComplete)
		{
		if(iVerifyPlayComplete)//case when the trailing silence is zero
			{
			if (!*iDisableAutoIntent && iDrmSource)
				{
				CMMFFile* file = static_cast<CMMFFile*>(iDrmSource);
				TInt err = file->ExecuteIntent(ContentAccess::EPlay);
				if (err != KErrNone)
					{
					DoSendEventToClient(KMMFEventCategoryPlaybackComplete, err);
					return;
					}
				}
			
			//Retrieve the current play time and add "duration-currentplaytime" to the silence period
			//This is to ensure that silence timer is not started before the previous play is actually completed by the devsound
			TTimeIntervalMicroSeconds currentTime = CalculateAudioOutputPosition();
			if(currentTime.Int64()>iPlayWindowStartPosition.Int64())
				{
				iTimeLeftToPlayComplete = iPlayWindowEndPosition.Int64()-currentTime.Int64();
				}
			else
				{
				iTimeLeftToPlayComplete = 0;
				}

			iVerifyPlayComplete = EFalse;
			}
		if(iTrailingSilenceLeftToPlay==0 && iTimeLeftToPlayComplete==0)
			{
			SetPositionL(iPlayWindowStartPosition);
			iTimeLeftToPlayComplete=-1;
			}
		else
			{
			PlaySilence();
			return;
			}
		}
	
	DoFillSourceBufferL();
	}


/** 
Indicates the data source has filled the specified buffer.

Called by the CMMFDataPath2's MDataSource when it has filled the buffer.

@param aBuffer
       A pointer to the filled buffer.
*/
void CMMFDataPath2::BufferFilledL(CMMFBuffer* aBuffer)
	{	
#ifdef _DP_DEBUG
	RDebug::Print(_L("DP::BufferFilledL src has filled buffer %d (ptr=0x%x) with %d bytes EoF = %d  tick-%d    (this 0x%x)\n"),aBuffer->FrameNumber(),aBuffer, aBuffer->BufferSize(),aBuffer->LastBuffer(), User::TickCount(),this);
#endif
	
	TBool isInTruePause = (iState == EPrimed  && iPauseCalled && iIsUsingResumeSupport);
	//state only used if we are passing data
	__ASSERT_DEBUG((iState == EPlaying || iState == EConverting || iState == ERecording || isInTruePause), Panic(EMMFDataPathPanicBadState,__LINE__));

	__ASSERT_DEBUG((!iNoMoreSourceData), Panic(EMMFDataPathPanicBadState,__LINE__)); 
	
	//if we have been asked to repeat and this is the last buffer, reset last buffer flag and send to the device
	if(aBuffer!= NULL && aBuffer->LastBuffer())
		{
		iNumberOfTimesPlayed++;
		if ((iNumberOfTimesPlayed <= iNumberOfTimesToRepeat) || iNumberOfTimesToRepeat == KMdaRepeatForever)
			{
			aBuffer->SetLastBuffer(EFalse);
			
			//this will trigger the trailing silence timer next time a buffer is requested.
			iTrailingSilenceLeftToPlay = iTrailingSilence;
			iVerifyPlayComplete = ETrue;
			}
		}		
		
	iSourceBufferWithSource = EFalse;

	//Has the datapath stopped running, if so were not interested in any callbacks.
	if(iState == EStopped || (iState == EPrimed  && !isInTruePause))
		{
#ifdef _DP_DEBUG
		RDebug::Print(_L("DP::BufferFilledL called while not expecting callback iState=%d  iPauseCalled=%d  (this 0x%x)\n"),iState, iPauseCalled,this);
#endif
		return;
		}

#ifdef REPOSITION_SPEEDUP
	// if the source has been re-positioned, then go & get some more source data now
	if (!iObtainingAsyncSourceBuffer && iSourceBuffer->FrameNumber() != iCurrentSourceFrameNumber)
		{
#ifdef _DP_DEBUG
		RDebug::Print(_L("DP::BufferFilledL source was re-positioned re-requesting source data (this 0x%x)\n"),this);
#endif
		ChangeDataPathTransferState(ENeedSourceData);
		return;
		}
#endif //REPOSITION_SPEEDUP

	//bufer is NULL, indicating no more source data.
	if (!aBuffer)
		{
		//If we only hold a reference to the source buffer, set that to NULL
		if(iSnkBufRef)
			{
			iSourceBuffer = NULL;
			}
		
		iNoMoreSourceData = ETrue;

		if(!iCodec || //there's only one buffer and that has been returned as NULL, so must be end of data
		  iSinkBufferWithSink) //buffer is with sink, we don't have any more data to put in it, so must be end of data
			{
			ChangeDataPathTransferState(EEndOfData);
			}
		else //sink buffer is with datapath, see if there is anything to send to sink
			{
			ChangeDataPathTransferState(ENeedToMatchSourceToSink);
			}
		
#ifdef _DP_DEBUG
		RDebug::Print(_L("DP::BufferFilledL DONE aBuffer==NULL tick-%d (this 0x%x)\n"),User::TickCount(),this);
#endif
		return;
		} 

	
	//We were waiting for a response from the source to get an asynchronous buffer.
	//We now have it, and we proceed to transfer this data to the sink.
	if	(iObtainingAsyncSourceBuffer)
		{
		iObtainingAsyncSourceBuffer = EFalse;
		}
	

	aBuffer->SetStatus(EFull);

	if(iSourceBuffer != aBuffer)
		{//buffer has been changed by the source
		iSourceBuffer = aBuffer;
		if (!(iBuffersToUse & ENeedSinkBuffer))
			{//we only need one buffer and use source
			iSinkBuffer = iSourceBuffer;
			iSnkBufRef = ETrue;
			}
#ifdef _DP_DEBUG
	RDebug::Print(_L("DP::BufferFilledL - iSourceBuffer=0x%x ref=%d   iSinkBuffer=0x%x ref=%d (this 0x%x)\n"),iSourceBuffer,iSrcBufRef,iSinkBuffer,iSnkBufRef, this);
#endif	
		}
	//Is this the last buffer from the source (0 length or LastBuffer flag set)
	//or have reached the end of the play window; we only look at the play window here
	//if we are converting. For conversion we look at the data we have read. This is then passed onto 
	//the source
	if (!iSourceBuffer->BufferSize() || iSourceBuffer->LastBuffer() ||
		(((iState == EConverting) || (iState == EPlaying)) && (iPlayWindowEndPosition < iCachedSourceDuration) && ( InputPosition() >= iPlayWindowEndPosition ))) 
		{
		//When it resumes in silence , position of the buffer is in end so we need to skip the increament.
		if(!iPauseCalledInsilence) 
		{    
			iNumberOfTimesPlayed++;
		}else
		{
			iPauseCalledInsilence=EFalse;
		}
		if ((iNumberOfTimesPlayed <= iNumberOfTimesToRepeat) || iNumberOfTimesToRepeat == KMdaRepeatForever)
			{
			iSourceBuffer->SetLastBuffer(EFalse);
			//this will trigger the trailing silence timer next time a buffer is requested.
			iTrailingSilenceLeftToPlay = iTrailingSilence;
			iVerifyPlayComplete = ETrue;
			}
		else
			{		
	#ifdef _DP_DEBUG
			RDebug::Print(_L("DP::BufferFilledL end of input data  tick-%d   (this 0x%x)\n"),User::TickCount(),this);
			RDebug::Print(_L("iSourceBuffer->BufferSize()=%d\n"),iSourceBuffer->BufferSize());
			RDebug::Print(_L("iSourceBuffer->LastBuffer()=%d\n"),iSourceBuffer->LastBuffer());
			RDebug::Print(_L("InputPosition()=%d  >= iPlayWindowEndPosition=%d\n"),I64INT(InputPosition().Int64()),I64INT(iPlayWindowEndPosition.Int64()));
	#endif
			iNoMoreSourceData = ETrue;
			iSourceBuffer->SetLastBuffer(ETrue); //just in-case we are terminating on BufferSize == 0 or play window
			}
		}


	if (!iCodec)
		{
		ChangeDataPathTransferState(ESendDataToSink);
		}
	else if(!iSinkBufferWithSink) //sink buffer is with data path, can try to fill it
		{
		ChangeDataPathTransferState(ENeedToMatchSourceToSink);	
		}
	//else wait for sink to return buffer BufferEmptied will send us into ENeedToMatchSourceToSink state

#ifdef _DP_DEBUG
	RDebug::Print(_L("DP::BufferFilledL - DONE tick-%d   (this 0x%x)\n"),User::TickCount(),this);
#endif
	}
	
/** 
Sets the data path position.

@param  aPosition
		The data path position.
*/
void CMMFDataPath2::SetPositionL(const TTimeIntervalMicroSeconds& aPosition)
	{//need to map to source position to frame position 
#ifdef _DP_DEBUG
	RDebug::Print(_L("DP::SetPositionL = %d  ticks-%d   (this 0x%x)\n"),I64INT(aPosition.Int64()), User::TickCount(),this);
#endif

	if (iState == EStopped)
		{
		User::Leave(KErrNotReady); //can only set position if primed
		}
	
	if(iGetTimePlayedSupported)
		{
		TTimeIntervalMicroSeconds timePlayed(0);
		if(iState == EPlaying && iDataSink->DataSinkType() == KUidMmfAudioOutput)
			{
			CMMFAudioOutput* audioOutput = STATIC_CAST(CMMFAudioOutput*,iDataSink);
			CMMFDevSound& devSound = audioOutput->SoundDevice();
			TInt err= devSound.GetTimePlayed(timePlayed);
			if(err == KErrNone)
				{
				iDevSoundRepositionTime = timePlayed.Int64();
				}
			}
		else
			{
			iDevSoundRepositionTime = 0;
			}
		}
	else //roll back to samplesplayed
		{
		//As this will affect the position, we need to know how many bytes were 
		//played when position was updated. Future Position() requests will
		//then use this refernce to determine the current position.
		iReferenceAudioSamplesPlayed = AudioSamplesPlayed();
		}
	
	// Force the new position to be inside the play window (also within the file duration)
	if ( aPosition < iPlayWindowStartPosition )
		{
		iStartPosition = iPlayWindowStartPosition;
		}
	else if ( aPosition > iPlayWindowEndPosition )
		{
		iStartPosition = iPlayWindowEndPosition; //clearly this will cause nothing to be played
		}
	else
		{
		iStartPosition = aPosition;
		}
	
	TTimeIntervalMicroSeconds interval;

	//can only set the position on an MDataSource that is a format object
	//Note: position defaults to source if both source & sink are clips
	if (iDataSource->DataSourceType() == KUidMmfFormatDecode)
		{
		//position is not beyond the end of file
		interval = ((CMMFFormatDecode*)iDataSource)->FrameTimeInterval(iMediaId);

		// for some reason this code won't compile without these intermediate steps
		TInt64 position = iStartPosition.Int64();
		TInt64 interval64 = interval.Int64();
		if (interval64 == 0)
			User::Leave(KErrDivideByZero); 
		TInt64 datapos64 = position/interval64; 
		iCurrentSourceFrameNumber = I64LOW(datapos64);


        // Try to set the position directly on the format
        TRAP_IGNORE(((CMMFFormatDecode*)iDataSource)->SetPositionL(iStartPosition));
        //NB: don't worry about error, since we'll reposition anyway when we get the next buffer
		}
	else if (iDataSink->DataSinkType() == KUidMmfFormatEncode)
		{			
		//position is not beyond the end of file
		interval = ((CMMFFormatEncode*)iDataSink)->FrameTimeInterval(iMediaId);

		//convert to TUint - for some reason it won't compile without these intermediate steps
		TInt64 position = iStartPosition.Int64();
		TInt64 interval64 = interval.Int64();
		if (interval64 == 0)
			User::Leave(KErrDivideByZero); 
		TInt64 datapos64 = position/interval64; 
		iCurrentSinkFrameNumber = I64LOW(datapos64);


        // Try to set the position directly on the format
        TRAP_IGNORE(((CMMFFormatEncode*)iDataSink)->SetPositionL(iStartPosition));
        //NB: don't worry about error, since we'll reposition anyway when we get the next buffer
		}
	else
		{//can only set position if source or sink is a format
		//If both source and sink are formats position is relative to the source
		User::Leave(KErrNotSupported); //can't set position if neither source nor sink are clips
		}

	if(iCodec) //we have a real codec, must reset it
		{
		iCodec->ResetL(); // Need to preserve sync when resuming play
		}
		
	// Once we've sent the last buffer to the sink it's too late to start
	// changing the state since we may get a RunError(KErrUnderflow) at any time.
	// Once this happens, the sound driver may have unloaded etc..and recovery
	// would be complicated.
	if (iAllDataSentToSink)
		{
		return;
		}
#ifdef _DP_DEBUG
	RDebug::Print(_L("DP::SetPosition - Done iCurrentSourceFrameNumber=%d  iStartPosition=%d  ticks-%d   (this 0x%x)\n"),iCurrentSourceFrameNumber, I64INT(iStartPosition.Int64()), User::TickCount(),this);	
#endif
	}

/** 
Gets the data path position.

@return The data path position.
*/
TTimeIntervalMicroSeconds CMMFDataPath2::Position() const
	{
	if ((iState == ERecording) || (iState == EConverting))
		{
		return InputPosition();
		}
	else if(iState == EPlaying)
		{
		return OutputPosition();
		}
	else
		{
		return iStartPosition;
		}
	}

TTimeIntervalMicroSeconds CMMFDataPath2::OutputPosition() const
	{
	TTimeIntervalMicroSeconds interval;
	TTimeIntervalMicroSeconds position;

    if (iDataSink->DataSinkType() == KUidMmfAudioOutput)
        {
		position = CalculateAudioOutputPosition();
#ifdef _DP_DEBUG
		RDebug::Print(_L("DP::OutputPosition from audio output= %d\n"),I64INT(position.Int64()));
#endif
        }
	else if (iDataSink->DataSinkType() == KUidMmfFormatEncode)
		{
		//note Encode format position takes priority if both source & sink are formats?
        // try to get the position directly from the format. If that fails, work it out here
        TRAPD(error, position = ((CMMFFormatEncode*)iDataSink)->PositionL());
        if (error)//getting the position from the format didn't work so calculate it here
            {
		    interval = ((CMMFFormatEncode*)iDataSink)->FrameTimeInterval(iMediaId);
			TInt64 position64 = interval.Int64() * iCurrentSinkFrameNumber;
			position = position64;
            }

		TTimeIntervalMicroSeconds duration = CONST_CAST(CMMFDataPath2*,this)->Duration(); //need this to check position doesn't exceed duration
		if (position > duration)//this can happen on last buffer 
			{
			position = duration;
			}
#ifdef _DP_DEBUG
		RDebug::Print(_L("DP::OutputPosition  from format= %d\n"),I64INT(position.Int64()));
#endif
		}
	else
		{//can only read output position if sink is a format or an audio output
		return TTimeIntervalMicroSeconds(0);
		}

	return position;
	}
			
TTimeIntervalMicroSeconds CMMFDataPath2::CalculateAudioOutputPosition() const
    {
	//This operation can only be carried out on an Audio Output
	__ASSERT_ALWAYS(iDataSink->DataSinkType() == KUidMmfAudioOutput, Panic(EMMFDataPathPanicProgrammingError,__LINE__));


	//If we are not playing, simply return where we will play from
	if(iState != EPlaying || iCurrentSinkFrameNumber == 0 || iStartPosition == iPlayWindowEndPosition )
		{
		return iStartPosition;
		}
#ifdef _DP_DEBUG
	RDebug::Print(_L("DP::CalculateAudioOutputDuration from %d\n"),iReferenceAudioSamplesPlayed);
#endif

	CMMFAudioOutput* audioOutput = STATIC_CAST(CMMFAudioOutput*,iDataSink);
	CMMFDevSound& devSound = audioOutput->SoundDevice();
	
	TTimeIntervalMicroSeconds devSoundTimePlayed(0);
	TInt64 timePlayed(0);
	TInt err = KErrNone;
	if(iGetTimePlayedSupported)
		{
		err= devSound.GetTimePlayed(devSoundTimePlayed);
		if(err == KErrNone)
			{
			timePlayed = devSoundTimePlayed.Int64()-iDevSoundRepositionTime.Int64();
			}
		}
	else //Roll back to SamplesPlayed
		{
		TReal samplesPlayed = AudioSamplesPlayed() - iReferenceAudioSamplesPlayed;
		TMMFCapabilities devSoundConfig = devSound.Config();
		TInt samplingFreq = StreamUtils::SampleRateAsValue(devSoundConfig);
	#ifdef _DP_DEBUG
		RDebug::Print(_L("samplingFreq %d\n"),samplingFreq);
	#endif
		
		TReal timePlayedSeconds = 0;
		if(samplesPlayed)
			{
			timePlayedSeconds = samplesPlayed/samplingFreq;
			}
		timePlayed = I64DOUBLECAST(timePlayedSeconds * 1000000);

	#ifdef _DP_DEBUG
		RDebug::Print(_L("timePlayed %d\n"), I64LOW(timePlayed));
	#endif
		}
	if(err == KErrNone)
		{
		// When Resume is not supported. Datapath simulates Pause() through DevSound's Stop
		// the time played is lost. So we need to saved the last position
		// On the opposite, when Resume is supported the time played returned by DevSound 
		// reflects the real position, so there is no needed to recalculate at least playwindow is being used
		// Finally, if Resume is supported but is not used the position also need to be saved
		if(!iIsUsingResumeSupport || iPlayWindowStartPosition.Int64() > 0)
			{
			timePlayed = timePlayed + iStartPosition.Int64();	
			}
		
		//During repeats. we need to reset the positin manually to playstart once playend is reached
		//this is because the bytes returned by devsound are not accurate in all the cases
		if(iRepeatTrailingSilenceTimer->IsActive() || iTimeLeftToPlayComplete==0)//loop play
			{
			if(iTimeLeftToPlayComplete==0)
				{
				timePlayed = iPlayWindowStartPosition.Int64();
				}
			}
		else if(timePlayed>=(iPlayWindowEndPosition.Int64()+1))
			{
			timePlayed = iPlayWindowStartPosition.Int64();
			}
		}
	
	return TTimeIntervalMicroSeconds(timePlayed);
    }
   
/** 
Stops playing.

Resets datapath position - currently does not clean up buffers. Sends KMMFErrorCategoryDataPathGeneralError 
to the client if an error occurs.
*/
void CMMFDataPath2::Stop()
	{ 
#ifdef _DP_DEBUG
	RDebug::Print(_L("DP::Stop current state=%d  tick-%d   (this 0x%x)\n"), iTransferState, User::TickCount(),this);
#endif
	if ((iDataPathCreated)  && (iState != EStopped))
		{ 
		TRAPD(err, DoStopL());
		if (err)
			{
			DoSendEventToClient(KMMFErrorCategoryDataPathGeneralError, err);
			}
		}
	}

void CMMFDataPath2::DoStopL()
	{
	CMMFDataPath::DoStopL();
	iRepeatTrailingSilenceTimer->Cancel();
	iIsUsingResumeSupport = EFalse;
	}
	
/** 
Pauses playing.

Sends KMMFErrorCategoryDataPathGeneralError to the client if an error occurs.
*/
void CMMFDataPath2::Pause()
	{
#ifdef _DP_DEBUG
	RDebug::Print(_L("DP::Pause, on src buff %d  sink buf %d   (this 0x%x)\n"), iCurrentSourceFrameNumber, iCurrentSinkFrameNumber,this);			
	RDebug::Print(_L("DP::Pause current state=%d  tick-%d    (this 0x%x)\n"),iTransferState, User::TickCount(),this);
#endif

	TRAPD(err, DoPauseL());
	
	if (err)
		{
		DoSendEventToClient(KMMFErrorCategoryDataPathGeneralError, err);
		}
#ifdef _DP_DEBUG
	RDebug::Print(_L("DP::Pause - DONE tick-%d   (this 0x%x)\n"),User::TickCount(),this);
#endif
	}

EXPORT_C void CMMFDataPath2::PreEmptionPause()
    {
    TRAPD(err, DoPreEmptionPauseL());
    
    if (err)
        {
        DoSendEventToClient(KMMFErrorCategoryDataPathGeneralError, err);
        }
    }

/** 
Pauses playing.

Sends KMMFErrorCategoryDataPathGeneralError to the client if an error occurs.
*/
void CMMFDataPath2::DoPreEmptionPauseL()
	{

    if ((iDataPathCreated) && ((iState == EPlaying) || (iState == EConverting) && ( iDataSink->DataSinkType() == KUidMmfAudioOutput )))
        {
        iDataSource->SourcePauseL();        // propagate state change to source
        SetPositionL(Position());
        iIsUsingResumeSupport = EFalse;

        iDataSink->SinkStopL();
	    iPauseCalled = ETrue;               // indicate pause is called
     
	    iState = EPrimed;                   // go back to primed state (we're not playing)
		
		iSinkBufferWithSink = EFalse;
		iSourceBufferWithSource = EFalse;
  
		ResetRefBuffers();                  // buffer references may not be valid any more
  
		Cancel(); //Stop the state machine		
	    }
	if(iState == ERecording)
	     {
	     User::Leave(KErrNotSupported);
	     }
	 iRepeatTrailingSilenceTimer->Cancel();
	}
	
void CMMFDataPath2::DoPauseL()
	{
#ifdef _DP_DEBUG
	RDebug::Print(_L("DP::DoPauseL tick-%d   (this 0x%x)\n"),User::TickCount(),this);
#endif


	if ((iDataPathCreated) && ((iState == EPlaying) || (iState == EConverting)))
		{
		if (iDataSink->DataSinkType() == KUidMmfAudioOutput && iIsResumeSupported)
			{
			iDataSink->SinkPauseL();
			// When true pause is supported only the datapath's position 
			// should be updated, MDataSource position should be changed
			iStartPosition = Position();
			iIsUsingResumeSupport = ETrue;
			if(iRepeatTrailingSilenceTimer->IsActive())
			{   
				iPauseCalledInsilence=ETrue;
			}   
			// If we wait for the sink to complete play, then we do not proceed with supplying the buffers to the sink
			// In this case we need to reset the buffers so that InitializeSinkL won't attempt bringing in new ones
			if (iTransferState == EWaitSink)
				{
				ResetRefBuffers();
				}
			}
		else
			{
			// If we use resume support, then there is no need to pause source as we would continue to supply buffers to the sink
			// Here we are not using resume support, thus we're pausing the source
			iDataSource->SourcePauseL();		
			SetPositionL(Position());
			iDataSink->SinkStopL();
			ResetRefBuffers();                  // buffer references may not be valid any more
  
			Cancel(); //Stop the state machine					
			}
		iPauseCalled = ETrue;				// indicate pause is called
		
		iState = EPrimed;					// go back to primed state (we're not playing)
		}
	else if(iState == ERecording)
		{
		iPauseCalled = ETrue;
#ifdef _DP_DEBUG
		RDebug::Print(_L("DP::DoPauseL Recording, pause datasource\n"));
#endif
		iDataSource->SourcePauseL();
		}
	iRepeatTrailingSilenceTimer->Cancel();
#ifdef _DP_DEBUG
	RDebug::Print(_L("DP::DoPauseL - Done iReferenceAudioSamplesPlayed = %d\n"),iReferenceAudioSamplesPlayed);
	RDebug::Print(_L("DP::DoPauseL - Done restart at %d tick-%d   (this 0x%x)\n"),I64INT(iStartPosition.Int64()), User::TickCount(),this);
#endif
	}

/**
Cancels the silence timer.
*/
void CMMFDataPath2::DoCancel()
	{
	//cancel repeats
	iRepeatTrailingSilenceTimer->Cancel();
	iNumberOfTimesToRepeat=0;
	}

/**
Allocates buffers in preparation to play.

Must be called before calling PlayL().

iSnkBufRef and iSrcBufRef contain ETrue if these buffers are created and owned by a MDataSource or MDataSink
For clean-up purposes, datapath only cleans up buffers allocated directly by PrimeL().
*/
void CMMFDataPath2::PrimeL()
	{
	CMMFDataPath::PrimeL();
	if(iDataSink->DataSinkType() == KUidMmfAudioOutput)
		{
		CMMFAudioOutput* audioOutput = STATIC_CAST(CMMFAudioOutput*,iDataSink);
		CMMFDevSound& devSound = audioOutput->SoundDevice();
		iGetTimePlayedSupported = devSound.IsGetTimePlayedSupported();
		iIsResumeSupported = devSound.IsResumeSupported();
		iIsUsingResumeSupport = EFalse;
		iPauseCalledInsilence = EFalse;
		}
	}

/**
Starts an active scheduler 'play' loop.

Can only play from the primed state.
*/
void CMMFDataPath2::PlayL()
	{

#if defined(__PROFILING)
	RDebug::ProfileEnd(1);
#endif  // defined(__PROFILING)

#ifdef _DP_DEBUG
	RDebug::Print(_L("DP::PlayL, on src buff %d  sink buf %d (this 0x%x)\n"), iCurrentSourceFrameNumber, iCurrentSinkFrameNumber,this);		
	RDebug::Print(_L("iStartPosition = %d\n"), I64INT(iStartPosition.Int64()));		
#endif

	if ((iDataPathCreated) && (iState == EPrimed))
		{
		//can only play from the primed state

		if(iPauseCalled) //sink and source will have been stopped, and we will not have been re-primed
			{
			//When pause is called silence,we need to send the buffer while resume so icansendbuffer should enabled
			if(!iPauseCalledInsilence)
			{
				iDataSink->SinkPrimeL(); //propagate change down to sink
			}
			iPauseCalled = EFalse;
			}

		iCurrentSourceFrameNumber = 0; //reset to beginning
		iCurrentSinkFrameNumber = 0; //reset to beginning

		iSourceBufferWithSource = EFalse;
		iSinkBufferWithSink = EFalse;

		iNoMoreSourceData = EFalse;
		iAllDataSentToSink=EFalse;
		iDataPathCompletedErrorCode=KErrNone;

		if(!iIsResumeSupported || !iIsUsingResumeSupport)
			{
			SetPositionL( iStartPosition );
			}
		iReferenceAudioSamplesPlayed = 0;
		iReferenceAudioSamplesRecorded = 0;
		
		//complete a request on iStatus to invoke play code
		iDataSource->SourcePlayL(); //propagate state change to source

		// This need to be done always since CMMFAudioOutput::EmptyBuffer
		// doesn't start playback anymore
		iDataSink->SinkPlayL(); //propogate state change to sink

		if (iDataSink->DataSinkType() == KUidMmfAudioOutput)
			iState = EPlaying;
		else if(iDataSource->DataSourceType() == KUidMmfAudioInput)
			iState = ERecording;
		else
			iState = EConverting;

		//need to re-initialize any buffer(s) that we only own references to
		ChangeDataPathTransferState(EInitializeSink);
		}
	iDevSoundRepositionTime = 0;
	if(!iRetainRepeatInfo)
		{
		iNumberOfTimesPlayed = 0;
		iTimeLeftToPlayComplete = -1;
		iVerifyPlayComplete = EFalse;
		}
	iRetainRepeatInfo = EFalse;
#ifdef _DP_DEBUG
	RDebug::Print(_L("DP::Play - DONE\n"));		
#endif
	}

/**
Deletes buffers if this datapath's sources and sinks own the buffers returned by PrimeL().
Typically if buffers are created asychronously, the datapath doesn't own the buffer
so leaves cleanup handling to the owner sources/sinks.

Called when source and sink needs to be de-referenced. Sets iDataPathCreated, iSinkCanReceive, 
iSnkBufRef and iSrcBufRef to EFalse; sets iState to EStopped.
*/
void CMMFDataPath2::ResetL()
	{
	CMMFDataPath::ResetL();
	iDrmSource = NULL;
	}

/**
Sets the number of times the audio sample is to be repeated during the
playback operation.

A period of silence can follow each playing of the sample. The audio
sample can be repeated indefinitely.

@param   aRepeatNumberOfTimes
         The number of times the audio sample, together with
         the trailing silence, is to be repeated. If this is
         set to KMdaRepeatForever, then the audio
         sample, together with the trailing silence, is
         repeated indefinitely or until Stop() is
         called. If this is set to zero, then the audio sample
         is not repeated.
@param   aTrailingSilence
         The time interval of the trailing silence in microseconds.

*/
EXPORT_C void CMMFDataPath2::SetRepeats(TInt aRepeatNumberOfTimes, const TTimeIntervalMicroSeconds& aTrailingSilence)
	{
	iNumberOfTimesToRepeat=aRepeatNumberOfTimes;
	iTrailingSilence=aTrailingSilence;
	}
	
/**
Sets the Drm file source and the automatic execute intent flag. This method is used by the controller plugin
to pass these to the datapath in order to execute the play intent during loop play.

@param  aSource
        Data Source on which the play intent needs to be executed. This is usually the CMMFFile source
@param  aDisableAutoIntent
		Boolean variable which states whether the controller plugin or the datapath needs to execute play intent
		automatically or not.
*/	
EXPORT_C void CMMFDataPath2::SetDrmProperties(MDataSource* aSource, TBool *aDisableAutoIntent)
	{
	iDrmSource = aSource;
	iDisableAutoIntent = aDisableAutoIntent;
	}

/**
This call indicates PlayL not to reset the iNumberOfTimesPlayed property. This method is used by the controller plugin
during repositioning. PlayL call during seeking should not reset the iNumberOfTimesPlayed property.
*/
EXPORT_C void CMMFDataPath2::RetainRepeatInfo()
	{
	iRetainRepeatInfo = ETrue;
	}

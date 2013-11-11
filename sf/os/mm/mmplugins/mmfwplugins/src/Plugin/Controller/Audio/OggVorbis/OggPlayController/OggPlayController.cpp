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

#include "OggPlayController.h"
#include <mda/client/resource.h>

//Table that maps given samplerate with the MMF samplerate
const TSampleRateTable KRateLookup[]=  {
										{96000,EMMFSampleRate96000Hz},
        						  		{88200,EMMFSampleRate88200Hz},
                   				  		{48000,EMMFSampleRate48000Hz},
			                   	  		{44100,EMMFSampleRate44100Hz},
				                   	  	{32000,EMMFSampleRate32000Hz},
				                   	  	{22050,EMMFSampleRate22050Hz},
				                   	  	{16000,EMMFSampleRate16000Hz},
				                   	  	{11025,EMMFSampleRate11025Hz},
				                   	  	{8000, EMMFSampleRate8000Hz},
			                   	  		{12000,EMMFSampleRate12000Hz},
			                   	  		{24000, EMMFSampleRate24000Hz}
                   				       };

/**
This method generates a panic

@param aPanicCode internal to this dll.
*/
void Panic(TInt aPanicCode)
	{
	_LIT(KOggControllerPanicCategory, "OggPlayController");
	User::Panic(KOggControllerPanicCategory, aPanicCode);
	}

COggPlayController* COggPlayController::NewL()
    {
    COggPlayController* self = new(ELeave) COggPlayController;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }
    
COggPlayController::COggPlayController()
    :iState(EStopped), iTimeLeftToPlayComplete(-1)
    {
    }
    
COggPlayController::~COggPlayController()
    {
    if(iSink)
    	{
    	iSink->SinkThreadLogoff();
    	}
    if(iClip)
    	{
    	iClip->SourceThreadLogoff();
    	}
	if(iRepeatTrailingSilenceTimer)
		{
		iRepeatTrailingSilenceTimer->Cancel();
		delete iRepeatTrailingSilenceTimer;
		}
    delete iOggSource;
    delete iMessage;
    delete iEventHandler;
    delete iActiveSchedulerWait;
    }
    
void COggPlayController::ConstructL()
    {
	// Construct custom command parsers
	//audPlayDevParser
	CMMFAudioPlayDeviceCustomCommandParser* audPlayDevParser = CMMFAudioPlayDeviceCustomCommandParser::NewL(*this);
	CleanupStack::PushL(audPlayDevParser);
	AddCustomCommandParserL(*audPlayDevParser);
	CleanupStack::Pop( audPlayDevParser );

	//audPlayConParser
	CMMFAudioPlayControllerCustomCommandParser* audPlayConParser = CMMFAudioPlayControllerCustomCommandParser::NewL(*this);
	CleanupStack::PushL(audPlayConParser);
	AddCustomCommandParserL(*audPlayConParser);
	CleanupStack::Pop(audPlayConParser);
    
    //DRMCustomCommands
	CMMFDRMCustomCommandParser* drmParser = CMMFDRMCustomCommandParser::NewL(*this);
	CleanupStack::PushL(drmParser);
	AddCustomCommandParserL(*drmParser);
	CleanupStack::Pop(drmParser);

	//audio resource Notification Parser
	CMMFResourceNotificationCustomCommandParser* NotiParser = CMMFResourceNotificationCustomCommandParser::NewL(*this);
	CleanupStack::PushL(NotiParser);
	AddCustomCommandParserL(*NotiParser);
	CleanupStack::Pop(NotiParser);
    
    //audiocontroller parser
    CMMFAudioControllerCustomCommandParser* audConParser = CMMFAudioControllerCustomCommandParser::NewL(*this);
 	CleanupStack::PushL(audConParser);
 	AddCustomCommandParserL(*audConParser);
 	CleanupStack::Pop(audConParser);

	//audPlayConSetRepeatsParser
	CMMFAudioPlayControllerSetRepeatsCustomCommandParser* audPlayConSetRepeatsParser = CMMFAudioPlayControllerSetRepeatsCustomCommandParser::NewL(*this);
	CleanupStack::PushL(audPlayConSetRepeatsParser);
	AddCustomCommandParserL(*audPlayConSetRepeatsParser);
	CleanupStack::Pop(audPlayConSetRepeatsParser);
    
    iEventHandler = new(ELeave) CSourceSinkEventHandler(*this);
    iRepeatTrailingSilenceTimer = CPeriodic::NewL(CActive::EPriorityStandard); 
    iActiveSchedulerWait = new(ELeave) CActiveSchedulerWait; 
    }
    
void COggPlayController::AddDataSourceL(MDataSource& aSource)
    {
    if (iState != EStopped) 
    	{
    	User::Leave(KErrNotReady);
    	}
    if (iClip)
    	{
    	User::Leave(KErrAlreadyExists);
    	}
	if ((aSource.DataSourceType()==KUidMmfDescriptorSource)||
			(aSource.DataSourceType()==KUidMmfFileSource))
	    {
	    User::LeaveIfError(aSource.SourceThreadLogon(*iEventHandler));
	    iClip = static_cast<CMMFClip*>(&aSource);
	    /*
	     The reason behind the TRAP below is that whenever AddDataSourceL() leaves, MMF deletes the datasource 
	     immediately i.e even before the ResetL() called. So we will have to do the 
	     SourceThreadLogoff() and setting the iClip to NULL before leaving from here.
	     */
	    TInt err = KErrNone;
	    TRAP(err, iOggSource = COggDecode::NewL(*iClip, *this));
	    if(err == KErrNone)
	    	{
	    	TRAP(err, iPlayWindowEnd = DurationL());
	    	}
	    if(err)
	    	{
	    	aSource.SourceThreadLogoff();
	    	iClip = NULL;
	    	User::Leave(err);
	    	}
	    }
	else 
		{
		User::Leave(KErrNotSupported);
		}
    }
    
void COggPlayController::AddDataSinkL(MDataSink& aSink)
    {
    if (iState != EStopped) 
    	{
    	User::Leave(KErrNotReady);	
    	}
    if(iMMFDevSound)
    	{
    	User::Leave(KErrAlreadyExists);
    	}
    if (aSink.DataSinkType()!=KUidMmfAudioOutput) 
    	{
    	User::Leave(KErrNotSupported);	
    	}
    User::LeaveIfError(aSink.SinkThreadLogon(*iEventHandler));
    iSink = &aSink;
    MMMFAudioOutput* audioOutput = static_cast<MMMFAudioOutput*>(&aSink);
    iMMFDevSound = &(audioOutput->SoundDevice());
	iMMFDevSound->SetPrioritySettings(iPrioritySettings);	
	if (IsSecureDrmModeL())
		{
		User::LeaveIfError(iMMFDevSound->SetClientThreadInfo(ClientThreadIdL()));
		}
	}

void COggPlayController::RemoveDataSourceL(MDataSource& aSource)
    {
    if (iState != EStopped) 
    	{
    	User::Leave(KErrNotReady);	
    	}
    if(!iClip)
    	{
    	User::Leave(KErrNotReady);
    	}
	
    if (iClip != &aSource)
    	{
    	User::Leave(KErrArgument);
    	}
    else
    	{
    	delete iOggSource;
    	iOggSource = NULL;
    	iClip->SourceThreadLogoff();
    	iClip = NULL;
    	}
    }

void COggPlayController::RemoveDataSinkL(MDataSink& aSink)
    {
    if (iState != EStopped) 
    	{
    	User::Leave(KErrNotReady);	
    	}
	if (aSink.DataSinkType()!=KUidMmfAudioOutput) 
	   	{
	   	User::Leave(KErrNotSupported);	
	   	}
	if(!iMMFDevSound)
	   	{
	   	User::Leave(KErrNotReady);
	   	}
	MMMFAudioOutput* audioOutput = static_cast<MMMFAudioOutput*>(&aSink);
	CMMFDevSound& devSound = audioOutput->SoundDevice();
	if (iMMFDevSound != &devSound) 
		{
		User::Leave(KErrArgument);
		}
	else
		{
		iSink->SinkThreadLogoff();
		iMMFDevSound = NULL;
		iSink = NULL;
		}
    }
    
void COggPlayController::ResetL()
    {
    if(iState != EStopped)
    	{
    	StopL();
    	}
    if(iSink)
    	{
    	iSink->SinkThreadLogoff();
    	}
    if(iClip)
    	{
    	iClip->SourceThreadLogoff();
    	}
    iMMFDevSound = NULL;
    iSink = NULL;
    iClip=NULL;
    delete iOggSource;
    iOggSource = NULL;
    delete iMessage;
    iMessage = NULL;
    }
    
void COggPlayController::PrimeL(TMMFMessage& aMessage)
	{
	if ((iMMFDevSound==NULL)||(iClip==NULL)||(iState != EStopped))
    	{
    	User::Leave(KErrNotReady);
    	}
	ASSERT(!iMessage);
	iMessage = CMMFMessageHolder::NewL(aMessage);
	StartPrimeL();
	iState = EPriming;
	}

void COggPlayController::PrimeL()
    {
    iClip->SourcePrimeL();
    iInitializeState = KRequestPending;    
    iMMFDevSound->InitializeL(*this, KMMFFourCCCodeVORB, EMMFStatePlaying);    
	if(iInitializeState==KRequestPending)
		{//means InitializeComplete is not called in context. Wait.
		iActiveSchedulerWait->Start();
		}
    User::LeaveIfError(iInitializeState);
    }
    
void COggPlayController::StartPrimeL()
    {
    iClip->SourcePrimeL();
    iMMFDevSound->InitializeL(*this, KMMFFourCCCodeVORB, EMMFStatePlaying);
    }
    
void COggPlayController::PlayL()
    {
    if ((iState!=EPrimed)&&(iState != EPaused)&&(iState != ESeek))
    	{
    	User::Leave(KErrNotReady);
    	}
    // Execute play intent
	if(iState == EPrimed || iState == EPaused)
		{
		if (!iDisableAutoIntent && iClip->DataSourceType()==KUidMmfFileSource)
			{
			CMMFFile* file = static_cast<CMMFFile*>(iClip);
			User::LeaveIfError(file->ExecuteIntent(iState == EPaused? ContentAccess::EContinue : ContentAccess::EPlay));
			}	
		}
	iClip->SourcePlayL();
    iDevSoundResetTime = 0;
    iDevSoundResetSamples = 0;
    if(iState != ESeek && iState != EPaused)
    	{
    	iNumberOfTimesPlayed = 0;
    	iTimeLeftToPlayComplete = -1;
		iVerifyPlayComplete = EFalse;
    	}
    iMMFDevSound->PlayInitL();
    iState=EPlaying;
    }
    
void COggPlayController::PauseL(TMMFMessage& aMessage)
	{
	if (iState!=EPlaying)
    	{
    	User::Leave(KErrNotReady);
    	}
	ASSERT(!iMessage);
	iMessage = CMMFMessageHolder::NewL(aMessage);
	PauseL();
	}
	
void COggPlayController::PauseL()
    {
    if (!iDisableAutoIntent && iClip->DataSourceType()==KUidMmfFileSource)
		{
		CMMFFile* file = static_cast<CMMFFile*>(iClip);
		TInt err = file->ExecuteIntent(ContentAccess::EPause);
		User::LeaveIfError(file->ExecuteIntent(ContentAccess::EPause));
		}
    
    //Store the current position in time.And will be added to PositionL if play resumed.
    iStoredPosition = PositionL();
	iState = EPausing;
	StopL();
	StartPrimeL();
    }
    
void COggPlayController::StopL()
    {
    if (iState == EStopped)
    	{
    	return;
    	}
    iMMFDevSound->Stop();
    iOggSource->ResetL();
    iRepeatTrailingSilenceTimer->Cancel();
    if(iState != EPausing && iState != ESeeking)
    	{
    	iStoredPosition = iPlayWindowStart;
    	iState=EStopped;
    	}
    }
/**
 Determines position from the Devsound SamplesPlayed.And updates the position if play resumed from pause.
 @return The TTimeIntervalMicroSeconds current position.
 */
TTimeIntervalMicroSeconds COggPlayController::PositionL() const
    {
    if(iState == EStopped)
    	{
    	User::Leave(KErrNotReady);
    	}
    if(iState == EPlaying)
    	{
    	TTimeIntervalMicroSeconds devSoundTimePlayed(0);
    	TTimeIntervalMicroSeconds seekTime(0);
    	TInt64 totalTime=0;
    	if(iStoredPosition.Int64() > 0)
    		{
    		seekTime = iOggSource->SeekingTime();
    		}
    	if(iGetTimePlayedSupported)
    		{
    		TInt err = iMMFDevSound->GetTimePlayed(devSoundTimePlayed);
	    	if(err == KErrNone)
	    		{
	       		devSoundTimePlayed = devSoundTimePlayed.Int64()-iDevSoundResetTime.Int64();
	       		totalTime = devSoundTimePlayed.Int64()+seekTime.Int64();
	    		}
    		}
    	else
    		{//Devsound does not support TimePlayed CI. Revert back to SamplesPlayed()
	    	TInt64 samplesPlayed = iMMFDevSound->SamplesPlayed();
			TInt sampleRate = iOggSource->Info()->SampleRate();
			samplesPlayed = samplesPlayed - iDevSoundResetSamples;
			devSoundTimePlayed = (samplesPlayed* 1000000)/sampleRate;
    		totalTime = devSoundTimePlayed.Int64()+seekTime.Int64();
   			}
		//During repeats. we need to reset to playstart once playend is reached
		//In other cases SetPosition takes care of that
		if(iRepeatTrailingSilenceTimer->IsActive() || iTimeLeftToPlayComplete==0)//loop play
			{
			if(iTimeLeftToPlayComplete==0)
				{
				totalTime = iPlayWindowStart.Int64();
				}
			}
		else if( totalTime>iPlayWindowEnd.Int64() )
			{
			if( iNumberOfTimesToRepeat>0 )
				{
				totalTime = iPlayWindowStart.Int64();
				}
			else
				{
				totalTime = iPlayWindowEnd.Int64();
				}
			}
	    		
    	return TTimeIntervalMicroSeconds(totalTime);
    	}
    else
    	{
    	return iStoredPosition;
    	}
    }
    
void COggPlayController::SetPositionL(const TTimeIntervalMicroSeconds& aPosition)
    {
    if((iState == EStopped) || (iState == ESeeking))//if we have not finished prev. seeking return KErrNotReady
    	{
    	User::Leave(KErrNotReady);
    	}
    if(aPosition < iPlayWindowStart)
    	{
    	iStoredPosition = iPlayWindowStart;
    	}
    else if(aPosition > iPlayWindowEnd)
    	{
    	iStoredPosition = iPlayWindowEnd;
    	}
    else
    	{
    	iStoredPosition = aPosition;
    	}
    if(iState == EPlaying)
    	{
    	iState = ESeeking;
    	StopL();
    	PrimeL(); //synchronous version; play is resumed from the stored position in initializecomplete()
    	}
	else
    	{
    	TRAPD(err, iOggSource->SetPositionL(iStoredPosition));
    	if(err)
  			{
  			User::Leave(err);
  			}
    	}
  	}
    
TTimeIntervalMicroSeconds COggPlayController::DurationL() const
    {
    LeaveIfNullL(iOggSource);	
    return iOggSource->DurationL();
    }
    
void COggPlayController::SetPrioritySettings(const TMMFPrioritySettings& aSettings)
    {
    iPrioritySettings = aSettings;
    if (iMMFDevSound)
    	{
    	iMMFDevSound->SetPrioritySettings(aSettings);
    	}
    }
    
void COggPlayController::GetNumberOfMetaDataEntriesL(TInt& aNumber)
    {
    LeaveIfNullL(iOggSource);
    aNumber = iOggSource->Info()->Comments();
    }
    
CMMFMetaDataEntry* COggPlayController::GetMetaDataEntryL(TInt aIndex)
    {
    LeaveIfNullL(iOggSource);
    if(!iOggSource->Info()->Comments())
    	{
    	User::Leave(KErrNotFound);
    	}
    CVorbisComment& c = iOggSource->Info()->GetComment(aIndex);
    return CMMFMetaDataEntry::NewL(c.Name(), c.Value());
    }
    
void COggPlayController::LeaveIfNullL(TAny* aPtr)
	{
	if (!aPtr) 
    	{
    	User::Leave(KErrNotReady);
    	}
	}

void COggPlayController::SendErrorToClient(TInt aError)
	{
	TInt err = aError;
	if(iState != EStopped)
		{
		/* 
		 Stop at the utility layer has no notion of failing. So the Stop at the controller layer 
		 is treated as the housekeeping action and we ignore all the errors here during Stopping the controller.
		 (taken from JF's comments for INC080041)
		 */
		TRAP_IGNORE(StopL());
		}

	if(iMessage)
		{
		iMessage->Complete(err);
		delete iMessage;
		iMessage = NULL;
		}
	else
		{
		TMMFEvent controllerEvent;
		controllerEvent.iEventType = KMMFEventCategoryPlaybackComplete;
		controllerEvent.iErrorCode = err;
		SendEventToClient(controllerEvent);	
		}
	}
//_____________________________________________________________________________
//                  Volume/Balance parser implementation

void COggPlayController::MapdSetVolumeL(TInt aVolume)
    {
    LeaveIfNullL(iMMFDevSound);
    iMMFDevSound->SetVolume(aVolume);
	}
    
void COggPlayController::MapdGetMaxVolumeL(TInt& aMaxVolume)
    {
    LeaveIfNullL(iMMFDevSound);
    aMaxVolume = iMMFDevSound->MaxVolume();
    }
    
void COggPlayController::MapdGetVolumeL(TInt& aVolume)
    {
    LeaveIfNullL(iMMFDevSound);
    aVolume = iMMFDevSound->Volume();
    }
    
void COggPlayController::MapdSetVolumeRampL(const TTimeIntervalMicroSeconds& aRampDuration)
    {
    LeaveIfNullL(iMMFDevSound);
    iMMFDevSound->SetVolumeRamp(aRampDuration);
    }
    
void COggPlayController::MapdSetBalanceL(TInt aBalance)
    {
    LeaveIfNullL(iMMFDevSound);
    if (aBalance < KMMFBalanceMaxLeft)
		{
    	aBalance = KMMFBalanceMaxLeft;	
    	}
	else if (aBalance > KMMFBalanceMaxRight)
		{
		aBalance = KMMFBalanceMaxRight;
		}
	TInt left = (100 * (aBalance-KMMFBalanceMaxRight)) / (KMMFBalanceMaxLeft-KMMFBalanceMaxRight);
    TInt right = 100 - left;
    iMMFDevSound->SetPlayBalanceL(left, right);
    }
    
void COggPlayController::MapdGetBalanceL(TInt& aBalance)
    {
    LeaveIfNullL(iMMFDevSound);
    TInt left = 50; // arbitrary values 
	TInt right = 50;
	iMMFDevSound->GetPlayBalanceL(left, right); 
    if ((left > 0) && (right > 0))
		{
		aBalance = (left * (KMMFBalanceMaxLeft-KMMFBalanceMaxRight))/100 + KMMFBalanceMaxRight;
		}
	else if ((left == 0) && (right == 0))
		{
		aBalance = 0;
		}
	else if ((left == 0) && (right > 0))
		{
		aBalance = 100;
		}
	else if ((left > 0) && (right == 0))
		{
		aBalance = -100;
		}
    }
    
void COggPlayController::MapcSetPlaybackWindowL(const TTimeIntervalMicroSeconds& aStart, 
                                                  const TTimeIntervalMicroSeconds& aEnd)
    {
    if(iState == EStopped)
    	{
    	User::Leave(KErrNotReady);
    	}
	LeaveIfNullL(iOggSource);
	// Check that the parameters are legitimate.  0 <= startpos < endpos <= duration & update member variables
	if(aStart >= aEnd)
		{
		User::Leave(KErrArgument);
		}
	TTimeIntervalMicroSeconds duration = DurationL();
	if (aStart < TTimeIntervalMicroSeconds(0))
		{
		iPlayWindowStart = TTimeIntervalMicroSeconds(0);
		}
	else if (aStart > duration)
		{
		iPlayWindowStart = duration;
		}
	else 
		{
		iPlayWindowStart = aStart;	
		}
	if (aEnd < TTimeIntervalMicroSeconds(0))
		{
		iPlayWindowEnd = TTimeIntervalMicroSeconds(0);
		}
	else if (aEnd > duration)
		{
		iPlayWindowEnd = duration;
		}
	else 
		{
		iPlayWindowEnd = aEnd;
		}
    iOggSource->SetPlayWindowL(iPlayWindowStart, iPlayWindowEnd);
    iStoredPosition = iPlayWindowStart;
    if(iState == EPlaying)
	    {
	    TTimeIntervalMicroSeconds currentPosition = PositionL();
		if (currentPosition < iPlayWindowStart)
			{
			SetPositionL(aStart);
			}
		else if (currentPosition > iPlayWindowEnd)
			{
			SetPositionL(aEnd);
			}	
	    }
    }
    
void COggPlayController::MapcDeletePlaybackWindowL()
    {
    if(iState == EStopped)
    	{
    	User::Leave(KErrNotReady);
    	}
    LeaveIfNullL(iOggSource);
    iPlayWindowStart = 0;
    iPlayWindowEnd = DurationL();
    iOggSource->ClearPlayWindowL();
    if(iState != EPlaying)
		{
		iStoredPosition = 0;
		}
	}
    
void COggPlayController::MapcGetLoadingProgressL(TInt& /*aPercentageComplete*/)
    {
    User::Leave(KErrNotSupported);
    }

void COggPlayController::ConvertError(TInt /*aError*/)
	{
	//Not applicable during playing. Should never get called.
	Panic(EBadCall);
	}

void COggPlayController::DeviceMessage(TUid /*aMessageType*/, const TDesC8& /*aMsg*/)
	{
	//Not applicable during playing. Should never get called.
	Panic(EBadCall);
	}

void COggPlayController::ToneFinished(TInt /*aError*/)
	{
	//Not applicable during playing. Should never get called.
	Panic(EBadCall);
	}

void COggPlayController::RecordError(TInt /*aError*/)
	{
	//Not applicable during playing. Should never get called.
	Panic(EBadCall);
	}

/**
InitializeComplete MMFDevSoundObserver called when devsound initialisation completed.
*/
void COggPlayController::InitializeComplete(TInt aError)
	{
	TInt err = aError;
	if (err == KErrNone)
		{
		iGetTimePlayedSupported = iMMFDevSound->IsGetTimePlayedSupported();
		TMMFCapabilities devSoundCaps = iMMFDevSound->Capabilities();
		TMMFCapabilities caps;
		TInt rate = iOggSource->Info()->SampleRate();
		TBool found = EFalse;
		for( TInt index =0; index < KNumSampleRates; index++)
			{
			if((rate == KRateLookup[index].iRate) && (devSoundCaps.iRate & KRateLookup[index].iRateEnum))
				{
				caps.iRate = KRateLookup[index].iRateEnum;
				found = ETrue;
				}
			}
    	if(!found)
    		{
    		err = KErrNotFound;
    		}
    	if(err == KErrNone)
    		{
    		TInt channels = iOggSource->Info()->Channels();
			if (channels == 1) 
				{
				caps.iChannels = EMMFMono;
				}
			else if (channels == 2)
				{
				caps.iChannels = EMMFStereo;
				}
			else 
				{
				err = KErrNotFound;
				}
	    	if(err == KErrNone)
		    	{
		    	caps.iEncoding = EMMFSoundEncoding16BitPCM;
				TRAP(err,iMMFDevSound->SetConfigL(caps));
		        if(err == KErrNone)
					{
					if(iState == EPriming)
		        		{
		        		iState = EPrimed;
		        		iMessage->Complete(KErrNone);
						delete iMessage;
						iMessage = NULL;
		        		}
		        	else if(iState == EPausing)
		        		{
		        		//we have the granulePos to seek in the file. So use granulePos version of SetPosition here.
		        		TRAP(err, iOggSource->SetPositionL(iStoredPosition));
		        		if(err == KErrNone)
							{
							iState = EPaused;
							iMessage->Complete(KErrNone);
							delete iMessage;
							iMessage = NULL;
							}
						//Next BufferToBeFilled should ignore the repeats when seeking
						if(iTrailingSilenceLeftToPlay > 0 || iVerifyPlayComplete)
							{
							iTrailingSilenceLeftToPlay = 0;
							iNumberOfTimesPlayed--;
							iVerifyPlayComplete = EFalse;
							}
						}
		        	else
		        		{
		        		TRAP(err, iOggSource->SetPositionL(iStoredPosition));
						if(err == KErrNone)
							{
							iState = ESeek;
							TRAP(err, PlayL());
							//Next BufferToBeFilled should ignore the repeats when seeking
							if(iTrailingSilenceLeftToPlay > 0 || iVerifyPlayComplete)
								{
								iTrailingSilenceLeftToPlay = 0;
								iNumberOfTimesPlayed--;
								iVerifyPlayComplete = EFalse;
								}							
							}									
		        		}	
					}	
		    	}	    	
    		}
		}

		if(iInitializeState == KRequestPending)
			{
			iInitializeState = err;
			if(iActiveSchedulerWait->IsStarted())
				{ 
				iActiveSchedulerWait->AsyncStop(); 
				}
			}
		else if(err)
			{
			SendErrorToClient(err);
			}	
	}

void COggPlayController::BufferToBeEmptied(CMMFBuffer* /*aBuffer*/)
	{
	//Not applicable during playing. Should never get called.
	Panic(EBadCall);
	}

void COggPlayController::DoFillThisBufferL(CMMFBuffer* aBuffer)
	{
	iOggSource->FillThisBufferL(aBuffer);
	}

void COggPlayController::BufferToBeFilled(CMMFBuffer* aBuffer)
	{
	TInt err = KErrNone;
	//if the silence timer is active then dont propagate the request
	iSourceBuffer = aBuffer;
	if(iRepeatTrailingSilenceTimer->IsActive())
		{
		return;
		}
	
	//play the silence period and dont propagate the request
	if(iTrailingSilenceLeftToPlay>0 || iVerifyPlayComplete)
		{
		if(iVerifyPlayComplete)//case when the trailing silence is zero
			{
			if (!iDisableAutoIntent && iClip->DataSourceType()==KUidMmfFileSource)
				{
				CMMFFile* file = static_cast<CMMFFile*>(iClip);
				err = file->ExecuteIntent(ContentAccess::EPlay);
				}
			//Retrieve the current play time and add "duration-currentplaytime" to the silence period
			//This is to ensure that silence timer is not started before the previous play is actually completed by the devsound
			if(err == KErrNone)
				{
				TTimeIntervalMicroSeconds currentTime(0);
				TRAP(err, currentTime = PositionL());
				if(err==KErrNone)
					{
					if(currentTime.Int64()>iPlayWindowStart.Int64())
						{
						iTimeLeftToPlayComplete = iPlayWindowEnd.Int64()-currentTime.Int64();
						}
					else
						{
						iTimeLeftToPlayComplete = 0;
						}
					iVerifyPlayComplete = EFalse;
					}	
				}
			
			}
		if(err == KErrNone)
			{
			if(iTrailingSilenceLeftToPlay==0 && iTimeLeftToPlayComplete==0)
				{
				ResetDevSoundTime();
				iTimeLeftToPlayComplete=-1;
				}
			else
				{
				PlaySilence();
				return;
				}
			}
		}
	
	if(err == KErrNone)
		{
		TRAP(err, DoFillThisBufferL(iSourceBuffer));
		}
	//if error, stop devsound and send the error to client
	if(err != KErrNone)
		{
		SendErrorToClient(err);
		}
	}

void COggPlayController::PlayError(TInt aError)
	{
	if(aError == KErrUnderflow)
		{
		if(iOggSource->IsAllDataSent())//we receive KErrUnderflow when all data is 
			{
			aError = KErrNone;
			}
		}
	SendErrorToClient(aError);
	}

void COggPlayController::SendEventToClient(const TMMFEvent& aEvent)
	{
	DoSendEventToClient(aEvent);
	}
	
void COggPlayController::BufferFilled()
	{
	//if we have been asked to repeat and this is the last buffer, reset last buffer flag and send to the device
	if(iSourceBuffer->LastBuffer())
		{
		iNumberOfTimesPlayed++;
		if ((iNumberOfTimesPlayed <= iNumberOfTimesToRepeat) || iNumberOfTimesToRepeat == KMdaRepeatForever)
			{
			iSourceBuffer->SetLastBuffer(EFalse);
			
			//this will trigger the trailing silence timer next time a buffer is requested.
		    iTrailingSilenceLeftToPlay = iTrailingSilence;
			iVerifyPlayComplete = ETrue;
			}
		}		
	iMMFDevSound->PlayData();
	}

void COggPlayController::MarnRegisterAsClientL(TUid aEventType,const TDesC8& aNotificationRegistrationData)
	{
	LeaveIfNullL(iMMFDevSound);
	//[register the notification ]
	User::LeaveIfError(iMMFDevSound->RegisterAsClient(aEventType, aNotificationRegistrationData));
	}
	
void COggPlayController::MarnCancelRegisterAsClientL(TUid aEventType)
	{
	LeaveIfNullL(iMMFDevSound);
	User::LeaveIfError(iMMFDevSound->CancelRegisterAsClient(aEventType));
	}

void COggPlayController::MarnGetResourceNotificationDataL(TUid aEventType,TDes8& aNotificationData)
	{
	LeaveIfNullL(iMMFDevSound);
	TMMFTimeIntervalMicroSecondsPckg pckg;
	User::LeaveIfError(iMMFDevSound->GetResourceNotificationData(aEventType, pckg));
	
	//DevSound returns samplesplayed which we need to convert into vorbis granulePos for 
	//the oggpage alignment. 
	TTimeIntervalMicroSeconds samplesPlayed = pckg();
	TInt64 vorbisGranulePos = 0;
	iOggSource->GetStreamPositionL(samplesPlayed.Int64(), vorbisGranulePos);
	pckg() = TTimeIntervalMicroSeconds(vorbisGranulePos * 1000000 / iOggSource->Info()->SampleRate());
	aNotificationData = pckg;
	}

void COggPlayController::MarnWillResumePlayL()
	{
	LeaveIfNullL(iMMFDevSound);
	User::LeaveIfError(iMMFDevSound->WillResumePlay());
	}
	
TInt COggPlayController::MdcEvaluateIntent(ContentAccess::TIntent aIntent) 
	{
	if (iClip->DataSourceType()==KUidMmfFileSource)
		{
		CMMFFile* file = static_cast<CMMFFile*>(iClip);
		TInt err = file->EvaluateIntent(aIntent);
		return err;
		}
	else
		{
		// Evaluating intent will always succeed on sinks that 
		// don't support DRM
		return KErrNone;
		}
	
	}
	
TInt COggPlayController::MdcExecuteIntent(ContentAccess::TIntent aIntent)
	{
	if (iClip->DataSourceType()==KUidMmfFileSource)
		{
		CMMFFile* file = static_cast<CMMFFile*>(iClip);
		TInt err = file->ExecuteIntent(aIntent);
		return err;
		}
	else
		{
		// Executing intent will always succeed on sinks that 
		// don't support DRM
		return KErrNone;
		}
	}
	
TInt COggPlayController::MdcDisableAutomaticIntent(TBool aDisableAutoIntent)
	{
	iDisableAutoIntent = aDisableAutoIntent;
	return KErrNone;
	}
	
	
TInt COggPlayController::MdcSetAgentProperty(ContentAccess::TAgentProperty aProperty, TInt aValue)
	{
	if (iClip->DataSourceType()==KUidMmfFileSource)
		{
		CMMFFile* file = static_cast<CMMFFile*>(iClip);
		TInt err = file->SetAgentProperty(aProperty, aValue);
		return err;
		}
	else
		{
		return KErrNone;
		}
	}

void COggPlayController::MacSetSourceSampleRateL(TUint aSampleRate)
 	{
 	if(( iState != EStopped ) || (!iOggSource))
 		{
 		User::Leave(KErrNotReady);
 		}
 	/*
 	 Setting the samplerate during playing is not allowed as the samperate is read from the Clip. But
 	 we have a version of OpenFileL() with CMdaAudioRecorderUtility which takes samplerate and recorderutility 
 	 tries to set the sampelerate using this customcommand. We just check that matches with the sampelerate in the clip
 	 and return KErrNotSupported fi it differs from samplerate in the clip.
 	 */
 	if(aSampleRate != iOggSource->Info()->SampleRate())
 		{
 		User::Leave(KErrNotSupported);	
 		}
 	}
 
 void COggPlayController::MacSetSourceNumChannelsL(TUint aNumChannels)
 	{
 	if(( iState != EStopped ) || (!iOggSource))
 		{
 		User::Leave(KErrNotReady);
 		}
 	/*
 	Setting the channels during playing is not allowed as the channels is read from the Clip. But
 	we have a version of OpenFileL() with CMdaAudioRecorderUtility which takes channels and recorderutility 
 	tries to set the channels using this customcommand. We just check that matches with the channels in the clip
 	*/
 	if(aNumChannels != iOggSource->Info()->Channels())
 		{
 		User::Leave(KErrNotSupported);	
 		}
  	}
 
 void COggPlayController::MacSetSourceFormatL(TUid aFormatUid)
 	{
 	if(( iState != EStopped ) || (!iOggSource))
 		{
 		User::Leave(KErrNotReady);
 		}
 	//OggPlayController supports only the Ogg format
 	if(aFormatUid != TUid::Uid(KOggDecodeUid))
 		{
 		User::Leave(KErrNotSupported);	
 		}
 	}
 
 void COggPlayController::MacSetSinkSampleRateL(TUint /*aSampleRate*/)
 	{
 	User::Leave(KErrNotSupported);
 	}
 
 void COggPlayController::MacSetSinkNumChannelsL(TUint /*aNumChannels*/)
 	{
 	User::Leave(KErrNotSupported);
 	}
 
 void COggPlayController::MacSetSinkFormatL(TUid /*aFormatUid*/)
 	{
 	User::Leave(KErrNotSupported);
 	}
 
 void COggPlayController::MacSetCodecL(TFourCC aSourceDataType, TFourCC /*aSinkDataType*/)
 	{
 	if(( iState != EStopped ) || (!iOggSource))
 		{
 		User::Leave(KErrNotReady);
 		}
 	//OggPlayController supports only the Ogg format with the Vorbis data 
 	if(aSourceDataType != KMMFFourCCCodeVORB)
 		{
 		User::Leave(KErrNotSupported);	
 		}
 	}
 
 void COggPlayController::MacSetSourceBitRateL(TUint aBitRate)
 	{
 	if(( iState != EStopped ) || (!iOggSource))
 		{
 		User::Leave(KErrNotReady);
 		}
 	//check if the bitrate passed matches with the bitrate in the clip
 	if(aBitRate != iOggSource->Info()->BitRateNominal())
 		{
 		User::Leave(KErrNotSupported);	
 		}
 	}
 
 void COggPlayController::MacSetSourceDataTypeL(TFourCC aDataType)
 	{
 	if(( iState != EStopped ) || (!iOggSource))
 		{
 		User::Leave(KErrNotReady);
 		}
 	//OggPlayController supports only the Ogg format with the Vorbis data 
 	if(aDataType != KMMFFourCCCodeVORB)
 		{
 		User::Leave(KErrNotSupported);	
 		}
 	}
 
 
 void COggPlayController::MacSetSinkBitRateL(TUint /*aRate*/)
 	{
 	User::Leave(KErrNotSupported);
 	}
 
 
 void COggPlayController::MacSetSinkDataTypeL(TFourCC /*aDataType*/)
 	{
 	User::Leave(KErrNotSupported);
 	}
 
 
 void COggPlayController::MacGetSourceSampleRateL(TUint& aRate)
 	{
 	LeaveIfNullL(iOggSource);
 	aRate = iOggSource->Info()->SampleRate();
 	}
 
 void COggPlayController::MacGetSourceBitRateL(TUint& aRate)
 	{
 	LeaveIfNullL(iOggSource);
 	aRate = iOggSource->Info()->BitRateNominal();
 	}
 
 void COggPlayController::MacGetSourceNumChannelsL(TUint& aNumChannels)
 	{
 	LeaveIfNullL(iOggSource);
 	aNumChannels = iOggSource->Info()->Channels();
  	}
 
 void COggPlayController::MacGetSourceFormatL(TUid& aFormat)
 	{
 	aFormat = TUid::Uid(KOggDecodeUid);
  	}
 
 void COggPlayController::MacGetSourceDataTypeL(TFourCC& aDataType)
 	{
 	aDataType = KMMFFourCCCodeVORB;
 	}
 
 void COggPlayController::MacGetSinkSampleRateL(TUint& /*aRate*/)
 	{
 	User::Leave(KErrNotSupported);
 	}
 
 void COggPlayController::MacGetSinkBitRateL(TUint& /*aRate*/)
 	{
 	User::Leave(KErrNotSupported);
 	}
 
 void COggPlayController::MacGetSinkNumChannelsL(TUint& /*aNumChannels*/)
 	{
 	User::Leave(KErrNotSupported);
 	}
 
 void COggPlayController::MacGetSinkFormatL(TUid& /*aFormat*/)
 	{
 	User::Leave(KErrNotSupported);
 	}
 
 void COggPlayController::MacGetSinkDataTypeL(TFourCC& /*aDataType*/)
 	{
 	User::Leave(KErrNotSupported);
 	}
 
 void COggPlayController::MacGetSupportedSourceSampleRatesL(RArray<TUint>& /*aSupportedRates*/)
 	{
 	User::Leave(KErrNotSupported);
 	}
 
 void COggPlayController::MacGetSupportedSourceBitRatesL(RArray<TUint>& /*aSupportedRates*/)
 	{
 	User::Leave(KErrNotSupported);
 	}
 
 void COggPlayController::MacGetSupportedSourceNumChannelsL(RArray<TUint>& /*aSupportedChannels*/)
 	{
 	User::Leave(KErrNotSupported);
 	}
 
 void COggPlayController::MacGetSupportedSourceDataTypesL(RArray<TFourCC>& /*aSupportedDataTypes*/)
 	{
 	User::Leave(KErrNotSupported);
 	}
 
 void COggPlayController::MacGetSupportedSinkSampleRatesL(RArray<TUint>& /*aSupportedRates*/)
 	{
 	User::Leave(KErrNotSupported);
 	}
 
 void COggPlayController::MacGetSupportedSinkBitRatesL(RArray<TUint>& /*aSupportedRates*/)
 	{
 	User::Leave(KErrNotSupported);
 	}
 
 void COggPlayController::MacGetSupportedSinkNumChannelsL(RArray<TUint>& /*aSupportedChannels*/)
 	{
 	User::Leave(KErrNotSupported);
 	}
 
 void COggPlayController::MacGetSupportedSinkDataTypesL(RArray<TFourCC>& /*aSupportedDataTypes*/)
 	{
 	User::Leave(KErrNotSupported);
 	}
 /**
* MapcSetRepeats
* @param aRepeatNumberOfTimes
* @param aTrailingSilence
*
*/
TInt COggPlayController::MapcSetRepeats(TInt aRepeatNumberOfTimes, const TTimeIntervalMicroSeconds& aTrailingSilence)
	{
	TInt err = KErrNone;
	if(!iMMFDevSound)
		{
		err = KErrNotReady;
		}
	else if(iMMFDevSound->QueryIgnoresUnderflow())
		{
		iNumberOfTimesToRepeat = aRepeatNumberOfTimes;
		iTrailingSilence = aTrailingSilence;
		}
	else
		{
		err = KErrNotSupported;
		}
	return err;
	}

TInt COggPlayController::RepeatTrailingSilenceTimerComplete(TAny* aController)
	{
	COggPlayController* controller = static_cast<COggPlayController*>(aController);
	controller->DoRepeatTrailingSilenceTimerComplete();
	return KErrNone;
	}
	
void COggPlayController::DoRepeatTrailingSilenceTimerComplete()
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
		ResetDevSoundTime();
		iTimeLeftToPlayComplete=-1;
		BufferToBeFilled(iSourceBuffer);
		}	
	}
	

void COggPlayController::PlaySilence()
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

void COggPlayController::ResetDevSoundTime()
	{
	TTimeIntervalMicroSeconds time(0);
	if(iGetTimePlayedSupported)
		{
		TInt err = iMMFDevSound->GetTimePlayed(time);
		if(err == KErrNone)
			{
			iDevSoundResetTime = time;
			}
		}
	else //roll back to SamplesPlayed()
		{
		iDevSoundResetSamples = iMMFDevSound->SamplesPlayed();
		}	
	iOggSource->ResetFile();
	}
	
COggPlayController::CMMFMessageHolder* COggPlayController::CMMFMessageHolder::NewL(TMMFMessage& aMessage)
	{
	return new(ELeave) CMMFMessageHolder(aMessage);
	}
		
void COggPlayController::CMMFMessageHolder::Complete(TInt aError) 
	{
	iMessage.Complete(aError);
	}
	
COggPlayController::CMMFMessageHolder::CMMFMessageHolder(TMMFMessage& aMessage): CBase(), iMessage(aMessage)
	{
	}
	
COggPlayController::CSourceSinkEventHandler::CSourceSinkEventHandler(MDevSoundObserver& aParent):iParent(aParent)
	{
	}

COggPlayController::CSourceSinkEventHandler::~CSourceSinkEventHandler()
	{
	}

TInt COggPlayController::CSourceSinkEventHandler::SendEventToClient(const TMMFEvent& aEvent)
	{
	iParent.SendEventToClient(aEvent);
	return KErrNone;
	}

// __________________________________________________________________________
// Exported proxy for instantiation method resolution
// Define the interface UIDs

const TImplementationProxy ImplementationTable[] =
	{
    IMPLEMENTATION_PROXY_ENTRY(KOggPlayControllerUid,	COggPlayController::NewL)
    };

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
    {
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
    return ImplementationTable;
    }

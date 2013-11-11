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


#include <mmf/server/mmfformat.h>
#include <mmf/server/mmfclip.h>
#include <mdaaudiosampleeditor.h>
#include <mmfcontrollerimplementationuids.hrh>
#include <mmf/common/mmffourcc.h>
#include <mmf/common/mmfpaniccodes.h>
#include "MmfAudioToneController.h"
#include <mmf/server/mmfaudiooutput.h>

/*
 A list of panic codes for the Audio Tone Controller
@internalTechnology

 TMmfAudioToneControllerPanics is an enumeration with the following entries:
 EBadArgument indicates a bad argument
 EBadState indicates a state viaolation
 EBadInvariant indicates an invariant violation
 EBadReset indicates failed reset
 EPostConditionViolation indicates a post condition violation
*/
enum TMmfAudioToneControllerPanics
	{
	EBadArgument,
	EBadState,
	EBadInvariant,
	EBadReset,
	EPostConditionViolation,
	EBadCall,
	ENoDataSource,
	ENoDataSink,
	EMessageAlreadyBeingProcessed,
	ENoMessageToProcess,
	EStateNotReadyToPlay,
	EStateNotConstructed,
	EBadStateToGetDataSource,
	EBadStateToGetDataSink,
	EBadStateForPrime,
	EStateNotPrimed,
	EBadResetState,
	EBadStateToReset,
	EBadPlayState,
	EBadPauseState,
	EBadStateToPause,
	EBadStateToStop,
	EBadStopState,
	ENotReadyForDataSourceRemoval,
	EBadDataSourceRemoval,
	ENotReadyForDataSinkRemoval,
	EBadDataSinkRemoval,
	ENotReadyForCustomCommand,
	EBadStateAfterNegotiate,
	EBadStateToSetPriority,
	EBadPriorityState,
	EBadInitializeState,
	EBadStateToSetVolume,
	EBadStateAfterVolumeSet,
	EBadStateToGetMaxVolume,
	EBadStateAfterGetMaxVolume,
	EBadStateToGetVolume,
	EBadStateAfterGetVolume,
	EBadStateToSetVolumeRamp,
	EBadStateAfterSetVolumeRamp,
	EBadStateToSetBalance,
	EBadStateAfterSetBalance,
	EBadStateToGetBalance,
	EBadStateAfterGetBalance,
	EBadStateForTransition,
	EBadStateAfterTransition,
	EStateNotValid,
	};


/**
 Instantiates a new Audio Tone Controller. Usually invoked from ECom
 
 @internalTechnology
 @return CMMFController* bas class for all plugin controllers
 */
CMMFController* CMMFAudioToneController::NewL()
	{
	CMMFAudioToneController* self = new(ELeave) CMMFAudioToneController;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop( self );
	return STATIC_CAST( CMMFController*, self );
	}



/**
2nd Phase constructor

@internalComponent
*/
void CMMFAudioToneController::ConstructL()
	{
	iSourceAndSinkAdded = EFalse;

	// Construct custom command parsers
	CMMFAudioPlayDeviceCustomCommandParser* audPlayDevParser = CMMFAudioPlayDeviceCustomCommandParser::NewL(*this);
	CleanupStack::PushL(audPlayDevParser);
	AddCustomCommandParserL(*audPlayDevParser);
	CleanupStack::Pop( audPlayDevParser );//audPlayDevParser

	CMMFAudioPlayControllerCustomCommandParser* audPlayConParser = CMMFAudioPlayControllerCustomCommandParser::NewL(*this);
	CleanupStack::PushL(audPlayConParser);
	AddCustomCommandParserL(*audPlayConParser);
	CleanupStack::Pop(audPlayConParser);//audPlayConParser
	
	CMMFAudioPlayControllerSetRepeatsCustomCommandParser* audPlayConSetRepeatsParser = CMMFAudioPlayControllerSetRepeatsCustomCommandParser::NewL(*this);
	CleanupStack::PushL(audPlayConSetRepeatsParser);
	AddCustomCommandParserL(*audPlayConSetRepeatsParser);
	CleanupStack::Pop(audPlayConSetRepeatsParser);
	
	// [ assert the invariant now that we are constructed ]
	__ASSERT_ALWAYS( Invariant(), Panic( EStateNotConstructed));
	}


/**
1st Phase constructor

@internalComponent
*/

CMMFAudioToneController::CMMFAudioToneController() : iState(EStopped)
	{
	}

/**
Destructor

@internalTechnology
*/
CMMFAudioToneController::~CMMFAudioToneController()
	{
	delete iMMFDevSound;
	delete iToneSequenceData;
	delete iMessage;
	}

/**
Adds a data source to the controller 

@internalTechnology

@param aSource will provide the data the controller plays
*/
void CMMFAudioToneController::AddDataSourceL(MDataSource& aSource)
	{
	//[ assert the invariant ]
	__ASSERT_ALWAYS( Invariant(), Panic( EBadStateToGetDataSource));

	// [ precondition that the controller is stopped ]
    if( State() != EStopped )
		User::Leave( KErrNotReady );
	
	//[ precondition iData source is not already configured ]
	if (iDataSource)
		User::Leave(KErrAlreadyExists);

	//Only works with files or descriptors
	if ((aSource.DataSourceType() != KUidMmfFileSource ) && (aSource.DataSourceType() != KUidMmfDescriptorSource))
		User::Leave( KErrNotSupported ) ;


	//extract the tone data into a buffer ready to play.
	iToneSequenceData = CMMFDataBuffer::NewL(STATIC_CAST(CMMFClip*, &aSource)->Size());

	aSource.SourcePrimeL();
	STATIC_CAST(CMMFClip*, &aSource)->ReadBufferL(iToneSequenceData,0);
	aSource.SourceStopL();


	//[ its now safe to set the source ]
	iDataSource = &aSource ;

	//[ assert the post condition ]
	__ASSERT_ALWAYS(iDataSource, Panic(EMMFAudioControllerPanicDataSourceDoesNotExist));

	iDataSource->SetSourcePrioritySettings(iPrioritySettings);
	}



/**
Adds a data sink to the controller 

@internalTechnology

@param aSink will accept the data the controller plays
*/
void CMMFAudioToneController::AddDataSinkL(MDataSink& aSink)
	{
	//[ assert the invariant ]
	__ASSERT_ALWAYS( Invariant(), Panic( EBadStateToGetDataSink));

	// [ precondition that the controller is stopped ]
    if( State() != EStopped )
		User::Leave( KErrNotReady );

	// [ assert precondition that sink does not exist ]
	if (iMMFDevSound)
		User::Leave(KErrAlreadyExists);

	//Only support playing to audio output 
	if (aSink.DataSinkType() != KUidMmfAudioOutput)
		User::Leave( KErrNotSupported );

	iMMFDevSound = CMMFDevSound::NewL();

	// [ assert post conditions that a sink has been added ]
	__ASSERT_ALWAYS(iMMFDevSound, Panic(EMMFAudioControllerPanicDataSinkDoesNotExist));
	}
	
/**
Primes the controller, ready for playing

@internalTechnology

@param aMessage allows response to client upon completion or error
*/
void CMMFAudioToneController::PrimeL(TMMFMessage& aMessage)
	{
	//[ assert the invariant ]
	__ASSERT_ALWAYS( Invariant(), Panic( EBadStateForPrime));

	//[ assert the precondition ( in a friendly way for this api 
	// that we are either stopped or primed already ]
	if(!(( State() == EStopped ) || (State() == EPrimed )))
		User::Leave( KErrNotReady );

	// [ precondition we have a data source & sink and aren't already processing a message]
	__ASSERT_ALWAYS( iDataSource, Panic( ENoDataSource));
	__ASSERT_ALWAYS( iMMFDevSound, Panic( ENoDataSink));
	__ASSERT_ALWAYS( !iMessage, Panic( EMessageAlreadyBeingProcessed ));

	if (iState == EStopped)
		{
	
		iMessage = new(ELeave) TMMFMessage(aMessage); //store message
		__ASSERT_ALWAYS( iMessage, Panic( EPostConditionViolation )); //check if message created sucessfully
		SetState(EPriming);

		TRAPD(err,NegotiateL());
		if(err != KErrNone)
			{
			SetState( EStopped );
			delete iMessage;
			iMessage = NULL;
			User::Leave(err);
			}
		}
	
	__ASSERT_ALWAYS( Invariant(), Panic( EStateNotPrimed ) );

	}

/**
Primes the controller, ready for playingAdds a data sink to the controller 

@internalTechnology
*/
void CMMFAudioToneController::PrimeL()
	{
	Panic(EBadCall);
	}

/**
This method resets the controller

@internalTechnology
*/
void CMMFAudioToneController::ResetL()
	{
	__ASSERT_ALWAYS( Invariant(), Panic( EBadStateToReset ) );

	// Stop recording if it's not stopped,
	if (State() != EStopped)
		{
		StopL();
		}

	//[ ensure loggoff of source and sink ]
	iDataSource = NULL ;
	delete iMMFDevSound; iMMFDevSound = NULL ;
	iSourceAndSinkAdded = EFalse;
	delete iMessage;
	iMessage = NULL;
	

	// [ assert the invariant]
	__ASSERT_ALWAYS( Invariant(), Panic( EBadResetState ) );

	__ASSERT_ALWAYS( ResetPostCondition(), Panic( EBadReset ));
	__ASSERT_ALWAYS( Invariant(), Panic(EBadState));
	}

/**
This function determnines if the reset post condition is valid

@internalComponent
*/
TBool CMMFAudioToneController::ResetPostCondition() const
	{
     TBool result = EFalse ;
	if((iMMFDevSound == NULL)  &&
	(iDataSource == NULL)  && 
	(State() == EStopped))
		{
         result = ETrue;
		}
    return result;
	}


/**
Start playing the audio tone, passing the data to Dev Sound

The controller will be put into the EPlaying state

@internalTechnology
*/
void CMMFAudioToneController::PlayL()
	{
	// [ assert the precondition that the
	//   play command is only activated in the primed state]
	if ( State() != EPrimed && State() != EPausePlaying)
		User::Leave(KErrNotReady);

	// [ assert the Invariant ]
	__ASSERT_ALWAYS( Invariant(), Panic(EStateNotReadyToPlay));

	if(State() == EPausePlaying && iIsResumeSupported)
		{
		User::LeaveIfError(iMMFDevSound->Resume());
		}
	else
		{
		iMMFDevSound->PlayToneSequenceL(iToneSequenceData->Data());	
		}
	
	SetState( EPlaying );
	
	//[ assert the post condition we are playing ]
	__ASSERT_ALWAYS( (State() == EPlaying ), Panic( EBadState));
	//[ assert the invariant ]
	__ASSERT_ALWAYS( Invariant(), Panic(EBadPlayState));
	}

/**
This should Pause playing of the audio tone. HOWEVER, this is not possible with a 
tone sequence. This method is therefore provided as a NOP purely to allow audio
clients to operate correctly.

The controller will be put into the EPrimed state

@internalTechnology
 */
void CMMFAudioToneController::PauseL()
	{
	//[ assert the invariant ]
	__ASSERT_ALWAYS( Invariant(), Panic(EBadStateToPause));

	__ASSERT_ALWAYS(iMMFDevSound, Panic(EMMFAudioControllerPanicDataSinkDoesNotExist));

	if(iIsResumeSupported)
		{
		iMMFDevSound->Pause();
		SetState(EPausePlaying);
		}
	else
		{
		// If Resume is not supported 
		// this method can't do anything, as we have no interface to restart DevSound 
		// after pausing a tone. It need to be here however as client utility does
		// Pause() Stop() when stopping.
		SetState(EPrimed);
		}

	//[ assert the post condition we are stopped ]
	__ASSERT_ALWAYS( (State() == EPrimed || State() == EPausePlaying), Panic( EBadState));
	//[ assert the invariant ]
	__ASSERT_ALWAYS( Invariant(), Panic(EBadPauseState));
	}

/**
This stops the tone that is currently playing
The controller will be put into the EStopped state

@internalTechnology
*/
void CMMFAudioToneController::StopL()
	{
	//[ assert the invariant ]
	__ASSERT_ALWAYS( Invariant(), Panic(EBadStateToStop));

	// [ precondition that we are not already stopped 
	// && if we are stopped do nothing.
	// Due to the asynchronous nature of the controller
	// interaction the response to stopped when stopped 
	// should not be an error ]
	if (State() != EStopped)
		{
		//[ update state to stopped propogate to devsound ]
		iMMFDevSound->Stop();
		SetState(EStopped);
		}

	//[ assert the post condition we are stopped ]
	__ASSERT_ALWAYS( (State() == EStopped), Panic( EBadState));
	//[ assert the invariant ]
	__ASSERT_ALWAYS( Invariant(), Panic(EBadStopState));
	}


/**
Removes a data source form the controller

@internalTechnology

@param aDataSource The source that is to be removed.
*/
void CMMFAudioToneController::RemoveDataSourceL(MDataSource& aDataSource )
	{
	//[ assert the invariant ]
	__ASSERT_ALWAYS( Invariant(), Panic(ENotReadyForDataSourceRemoval) );

	//[ precondition is that we have a data source ]
	if( !iDataSource )
		User::Leave(KErrNotReady);

	//[precondition the data source is the data source we have]
	if( iDataSource != &aDataSource )
		User::Leave(KErrArgument);

	//[ the controller is in the stopped state ]
	if(State() != EStopped)
		User::Leave(KErrNotReady);

	//[ remove the data sink from the controller and delete the format]
     if( iSourceAndSinkAdded )
		{
		iMMFDevSound->Stop();
		iSourceAndSinkAdded = EFalse ;
		}

	 iDataSource = NULL ;
		
	// [ assert postcondition we are stopped ]
	__ASSERT_ALWAYS( (State() == EStopped), Panic(EPostConditionViolation) );

	//[ assert postcondition the SourceAndSinkAdded is false ]
	__ASSERT_ALWAYS( !iSourceAndSinkAdded, Panic( EPostConditionViolation ));
	
	//[ assert postcondition the data sink  is null ]
	__ASSERT_ALWAYS( (iDataSource == NULL ), Panic( EPostConditionViolation ));

	//[ assert the invariant ]
	__ASSERT_ALWAYS( Invariant(), Panic(EBadDataSourceRemoval));
	}


/**
Removes a data sink form the controller

@internalTechnology

@param aDataSink The sink that is to be removed. We donot 
use this value, as we only support a single sink. 
*/
void CMMFAudioToneController::RemoveDataSinkL(MDataSink& /*aDataSink*/)
	{
	//[ assert the invariant ]
	__ASSERT_ALWAYS( Invariant(), Panic(ENotReadyForDataSinkRemoval) );

	//[ precondition is that we have a data sink ]
	if(!iMMFDevSound)
		User::Leave(KErrNotReady);

	//[ the controller is in the stopped state ]
	if(State() != EStopped)
		User::Leave(KErrNotReady);

	//[ remove the data sink from the controller and delete the format]
     if(iSourceAndSinkAdded)
		{
		iMMFDevSound->Stop();
		iSourceAndSinkAdded = EFalse;
		}

	delete iMMFDevSound; iMMFDevSound = NULL;

		
	// [ assert postcondition we are stopped ]
	__ASSERT_ALWAYS( (State() == EStopped), Panic(EPostConditionViolation) );

	//[ assert postcondition the SourceAndSinkAdded is false ]
	__ASSERT_ALWAYS( !iSourceAndSinkAdded, Panic( EPostConditionViolation ));
	
	//[ assert the invariant ]
	__ASSERT_ALWAYS( Invariant(), Panic(EBadDataSinkRemoval));
	}

/**
Handles a CustomCommand for the controller. This controller doesn't support Custom Commands

@internalTechnology
@param aMessage
*/
void CMMFAudioToneController::CustomCommand(TMMFMessage& aMessage)
	{
	//[ assert the invariant ]
	__ASSERT_ALWAYS( Invariant(), Panic(ENotReadyForCustomCommand));
	// [ We do not have any custom commands ]
	aMessage.Complete(KErrNotSupported);
	}

/**
Configures Dev Sound, ready for playing.

@internalComponent
*/
void CMMFAudioToneController::NegotiateL()
	{
	if (!iMMFDevSound)
		Panic(EMMFAudioOutputDevSoundNotLoaded);

	iMMFDevSound->InitializeL(*this, EMMFStateTonePlaying);
	iMMFDevSound->SetPrioritySettings(iPrioritySettings);

	//[ assert the invariant ]
	__ASSERT_ALWAYS( Invariant(), Panic(EBadStateAfterNegotiate));
	}

/**
Set the priority settings that this controller should use to access Dev Sound

@internalTechnology
@param aPrioritySettings The requires priority settings
*/
void CMMFAudioToneController::SetPrioritySettings(const TMMFPrioritySettings& aPrioritySettings)
	{
	//[ assert the invariant ]
	__ASSERT_ALWAYS( Invariant(), Panic(EBadStateToSetPriority));

	//[ assert the precondition ]
	if(State() != EStopped)
		{
		ASSERT(EFalse);		// used to leave here with KErrNotReady
		return;
		}

	//[ update the priority settings of the controller]
	iPrioritySettings = aPrioritySettings;

	if (iMMFDevSound)
		{
		iMMFDevSound->SetPrioritySettings(iPrioritySettings);
		}

	__ASSERT_ALWAYS( Invariant(), Panic(EBadPriorityState));
	}



/**
This method is called by DevSound after initialization, indicating that it has completed and 
whether there was an error

@internalTechnology
@param aError the error supplied by Dev Sound
*/
void CMMFAudioToneController::InitializeComplete(TInt aError)
	{
	//[ assert the state is EPriming ]
	__ASSERT_ALWAYS( ( State() == EPriming ), Panic( EBadState ));
	__ASSERT_ALWAYS( iMessage, Panic( ENoMessageToProcess ));
	
	if(aError != KErrNone)
		{
		SetState( EStopped );
		iMessage->Complete(aError);
		}
	else 
		{
		SetState( EPrimed );
		iMessage->Complete(aError);
		iIsResumeSupported = iMMFDevSound->IsResumeSupported();
		}
	
	delete iMessage;
	iMessage = NULL;
	
	// [ assert the invariant]	
	__ASSERT_ALWAYS( Invariant(), Panic( EBadInitializeState ) );
	}


/**
This method is called by DevSound after it has finished playing a tone sequence, indicating 
whether there was an error

@internalTechnology
@param aError the error supplied by DevSound
*/
void CMMFAudioToneController::ToneFinished(TInt aError)
	{
	// NB KErrInUse, KErrDied OR KErrAccessDenied may be returned 
	// to indicate that the sound device is in use  by another higher 
	// priority client.
	if (aError == KErrCancel || aError == KErrInUse || 
	    aError == KErrDied || aError == KErrAccessDenied)
		return;

	if (aError == KErrUnderflow)
		aError = KErrNone;

	if (State() != EStopped)
		{
		iMMFDevSound->Stop();
		SetState( EStopped );
		}

	//now send event to client...
	TMMFEvent event;
	event.iEventType = KMMFEventCategoryPlaybackComplete;
	event.iErrorCode = aError;
	DoSendEventToClient(event);	
	}



/**
Called my DevSound to indicate we have been thrown off H/W by a higher priority

@internalTechnology
@param aEvent contains the reason we have been contacted by DevSound
*/
void CMMFAudioToneController::SendEventToClient(const TMMFEvent& aEvent)
	{
	if(State() == EPlaying)
		SetState(EStopped);

	DoSendEventToClient(aEvent);	
	}




/**
Sets the playback volume

@internalTechnology
@param aVolume the required volume
*/
void CMMFAudioToneController::MapdSetVolumeL(TInt aVolume)
	{
	//[ assert the invariant ]
	__ASSERT_ALWAYS( Invariant(), Panic(EBadStateToSetVolume));

	// [  precondition is true for state 
	//    we can set the volume in any state ]

	//[ precondition we have a data sink ]
	if (!iMMFDevSound)
		User::Leave(KErrNotReady);


	// [ assert the precondition that aVolume is in range ]
	TInt maxVolume = iMMFDevSound->MaxVolume();
	if( ( aVolume < 0 ) || ( aVolume > maxVolume ))
		User::Leave(KErrArgument);
	
	//[ set the volume on the device ]
	iMMFDevSound->SetVolume(aVolume);

	//[ assert the post condition volume is equal to a volume]
	TInt soundVolume = 0;
	soundVolume = iMMFDevSound->Volume();

    __ASSERT_ALWAYS( ( soundVolume == aVolume), Panic(EPostConditionViolation));

	//[ assert the invariant ]
	__ASSERT_ALWAYS( Invariant(), Panic(EBadStateAfterVolumeSet));
	}

/**
Gets the maximum level the playback volume can be set to

@internalTechnology
@param aMaxVolume contains the maximum volume setting
*/
void CMMFAudioToneController::MapdGetMaxVolumeL(TInt& aMaxVolume)
	{
	// [ assert the invariant ]
	__ASSERT_ALWAYS( Invariant(), Panic(EBadStateToGetMaxVolume));

	//[ precondition we have a data sink ]
	if (!iMMFDevSound)
		User::Leave(KErrNotReady);

	//[ get the volume from the device ]
	aMaxVolume = iMMFDevSound->MaxVolume();

	//[ assert the invariant ]
	__ASSERT_ALWAYS( Invariant(), Panic(EBadStateAfterGetMaxVolume));

	}


/**
Gets the current playback volume

@internalTechnology
@param aVolume the current volume
*/
void CMMFAudioToneController::MapdGetVolumeL(TInt& aVolume)
	{
	// [ assert the invariant ]
	__ASSERT_ALWAYS( Invariant(), Panic(EBadStateToGetVolume));

	//[  precondition that we have a data sink ]
	if (!iMMFDevSound)
		User::Leave(KErrNotReady);

	aVolume = iMMFDevSound->Volume();
	
	// [ assert precondition that the volume is in range
	//     0.. aMaxVolume ]
	TInt aMaxVolume = iMMFDevSound->MaxVolume();
	__ASSERT_ALWAYS( (aVolume <= aMaxVolume), Panic(EBadState));
	__ASSERT_ALWAYS( (aVolume >= 0), Panic(EBadState));

	// [ assert the invariant ]
	__ASSERT_ALWAYS( Invariant(), Panic(EBadStateAfterGetVolume));

	}

/**
Sets the duration over which the volume should increase

@internalTechnology
@param aRampDuration the time over which the volume should ramp
*/
void CMMFAudioToneController::MapdSetVolumeRampL(const TTimeIntervalMicroSeconds& aRampDuration)
	{
     // [ assert the invariant ]
	__ASSERT_ALWAYS( Invariant(), Panic(EBadStateToSetVolumeRamp));

	//[ precondition that we have a data sink ]
	if (!iMMFDevSound)
		User::Leave(KErrNotReady);

	iMMFDevSound->SetVolumeRamp(aRampDuration);
	
	//[ assert the invariant ]
	__ASSERT_ALWAYS( Invariant(), Panic(EBadStateAfterSetVolumeRamp));
		
	}


/**
Sets the balance of the tone playback

@internalTechnology
@param aBalance the required balance level (KMMFBalanceMaxLeft <= aBalance <= KMMFBalanceMaxRight)
*/
void CMMFAudioToneController::MapdSetBalanceL(TInt aBalance)
	{
	//[ assert the invariant ]
	__ASSERT_ALWAYS( Invariant(), Panic( EBadStateToSetBalance));

	// [ precondition is that we have a data sink ]
	if (!iMMFDevSound)
		User::Leave(KErrNotReady);
	
	
	// [ separate out left and right balance ]
	TInt left  = 0;
	TInt right = 0;
	CalculateLeftRightBalance( left, right, aBalance );
	
	//[ set the balance ]
	iMMFDevSound->SetPlayBalanceL(left, right); 

	// [assert the post condition that the balance is set correctly]
	TInt rightBalance = 0;
	TInt leftBalance  = 0;
	iMMFDevSound->GetPlayBalanceL(leftBalance, rightBalance); 

	//[ assert post condition holds]
	TBool postCondition = (( rightBalance == right) && ( leftBalance == left));
	__ASSERT_ALWAYS( postCondition, Panic( EPostConditionViolation ) );

	//[ assert the invariant ]
	__ASSERT_ALWAYS( Invariant(), Panic( EBadStateAfterSetBalance));
	}

/**
Converts the balance from a range to a left/right form.

Balance is provided to devsound using left and right levels, but supplied to the controller as a range 
(KMMFBalanceMaxLeft --> KMMFBalanceMaxRight). This method converts the range into a left/right form

@internalComponent

@param aLeft set to the left setting
@param aRight set to the right setting
@param aBalance the range required
*/
void CMMFAudioToneController::CalculateLeftRightBalance( TInt& aLeft, TInt& aRight, TInt aBalance ) const
	{
	// Check the balance is within limits & modify to min or max values if necessary
	if (aBalance < KMMFBalanceMaxLeft)
		aBalance = KMMFBalanceMaxLeft;
	if (aBalance > KMMFBalanceMaxRight)
		aBalance = KMMFBalanceMaxRight;
	
	//[ Now separate percentage balances out from aBalance ]
	 aLeft = (100 * (aBalance-KMMFBalanceMaxRight)) / (KMMFBalanceMaxLeft-KMMFBalanceMaxRight);
     aRight = 100 - aLeft;

	 //[ assert post condition that left and right are within range ]
	 __ASSERT_ALWAYS( ( (aLeft <= 100) && (aLeft >= 0) ), Panic(EPostConditionViolation));
	 __ASSERT_ALWAYS( ( (aRight <= 100) && (aRight >= 0) ), Panic(EPostConditionViolation));
	}


/**
Gets the balance of the tone playback

@internalTechnology
@param aBalance set to the current balance level (KMMFBalanceMaxLeft <= aBalance <= KMMFBalanceMaxRight)
*/
void CMMFAudioToneController::MapdGetBalanceL(TInt& aBalance)
	{
	//[ assert the invariant ]
	__ASSERT_ALWAYS( Invariant(), Panic(EBadStateToGetBalance));

	//[ precondition that we have a sink]
	if (!iMMFDevSound)
		User::Leave(KErrNotReady);
	
	
	TInt left = 50; // arbitrary values 
	TInt right = 50;
	iMMFDevSound->GetPlayBalanceL(left, right); 

    CalculateBalance( aBalance, left, right );

	//[ assert the invariant ]
	__ASSERT_ALWAYS( Invariant(), Panic(EBadStateAfterGetBalance));
	}


/**
Converts the balance from a left/right form to a range.

Balance is obtained from  DevSound using left and right levels, but supplied to the client as a range 
(KMMFBalanceMaxLeft --> KMMFBalanceMaxRight).

@internalComponent

@param aLeft the current left setting
@param aRight current right setting
@param aBalance set to the balance range
*/
void CMMFAudioToneController::CalculateBalance( TInt& aBalance, TInt aLeft, TInt aRight ) const
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
The function validates a state transition from iState to aState and 
returns ETrue if the transition is allowed.

@internalComponent
@param TControllerState
@return Valid controller state or not 
*/
TBool CMMFAudioToneController::IsValidStateTransition( TControllerState aState ) const
	{
	 TBool result = ETrue ;
	//[ assert the precondition that aState is a valid State ]
	__ASSERT_ALWAYS( IsValidState(aState), Panic( EBadArgument ) );
	//[ assert the invariant that iState is a valid State ]
	__ASSERT_ALWAYS( Invariant(), Panic( EBadStateForTransition ));

	// [ check the valid state transitions ]
	  // the only invalid transition is
	  // stopped to playing
	if( ( iState == EStopped ) && ( aState == EPlaying ))
		{
         result = EFalse ;
		}
  
	//[ assert the invariant that iState is a valid State ]
	__ASSERT_ALWAYS( Invariant(), Panic( EBadStateAfterTransition ));

	return result ;
	}

/*
This function returns whether the invariant is valid

@internalComponent
@return Is the class in a good condition
*/
TBool  CMMFAudioToneController::Invariant() const
	{
	//[ The invariant is for now defined 
	// as simply being in the correct state
	return IsValidState( iState);
	}

/*
This function sets the state of the controller.

@internalComponent
@return Whether ths state transition is valid
*/
TBool CMMFAudioToneController::SetState(TControllerState aState)
	{
	TBool result = ETrue;
	//[ assert the precondition that the state is a valid state ]
   	__ASSERT_ALWAYS( IsValidState( aState),  Panic( EBadArgument ) );
	//[ assert the invariant the current state is valid ]
	__ASSERT_ALWAYS( Invariant(),  Panic( EStateNotValid ) );
    //[ only allow valid state transitions ]
	if( IsValidStateTransition( aState ) )	
		{
		//[ valid state transition set the state]
		iState = aState ;
		}
	else
		{
		//[ invalid state transition return EFalse ]
		result = EFalse;         
		}
	// [ assert the invariant on the state ]
	__ASSERT_ALWAYS( Invariant(), Panic( EBadState ));
	
	return result ;
	}

/*
checks whether a state is valid 

@internalComponent
@return Is the state a valid one
*/
TBool  CMMFAudioToneController::IsValidState( TControllerState aState ) const 
	{
	TBool result = EFalse;
     if(( aState >= EStopped ) && ( aState <= EPlaying ))
		 {
          result = ETrue;
		 }
	 return result;
	}

/**
The function State returns the current state of the audio controller

@internalComponent
@return State of the controller
*/
CMMFAudioToneController::TControllerState CMMFAudioToneController::State() const
	{
	__ASSERT_ALWAYS( Invariant(), Panic( EBadState ) );
	return iState;
	}


/**
* MapcSetRepeatsL
* @param aRepeatNumberOfTimes
* @param aTrailingSilence
*
*/
TInt CMMFAudioToneController::MapcSetRepeats(TInt aRepeatNumberOfTimes, const TTimeIntervalMicroSeconds& aTrailingSilence)
	{
	TInt err = KErrNone;
	if (!iMMFDevSound)
		{
		return KErrNotReady;
		}		
	else
		{
		if(iMMFDevSound->QueryIgnoresUnderflow())
			{
			iMMFDevSound->SetToneRepeats(aRepeatNumberOfTimes, aTrailingSilence);
			}
		else
			{
			err = KErrNotSupported;
			}
		}
	return err;
	}

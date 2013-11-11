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
#include <mmf/plugin/mmfcontrollerimplementationuids.hrh>
#include <mmf/common/mmffourcc.h>
#include <mmf/common/mmfpaniccodes.h>
#include "TestUseOldCodecAudioController.h"

const TUint KSampleRate8000Hz = 8000;
const TUint KSampleRate11025Hz = 11025;
const TUint KSampleRate16000Hz = 16000;
const TUint KSampleRate22050Hz = 22050;
const TUint KSampleRate32000Hz = 32000;
const TUint KSampleRate44100Hz = 44100;
const TUint KSampleRate48000Hz = 48000;
const TUint KSampleRate88200Hz = 88200;
const TUint KSampleRate96000Hz = 96000;
const TUint KNumChannelsMono = 1;
const TUint KNumChannelsStereo = 2;

/*
 TMmfAudioControllerPanics is an enumeration with the following entries:
 EBadArgument indicates a bad argument
 EBadState indicates a state viaolation
 EBadInvariant indicates an invariant violation
 EBadReset indicates failed reset
 EPostConditionViolation indicates a post condition violation
*/
enum TMmfAudioControllerPanics
	{
	EBadArgument,
	EBadState,
	EBadInvariant,
	EBadReset,
	EPostConditionViolation
	};

/**
* Panic
* This method generates a panic
* @param "TInt"
* @xxxx
*/
void Panic(TInt aPanicCode)
	{
	_LIT(KMMFAudioControllerPanicCategory, "MMFAudioController");
	User::Panic(KMMFAudioControllerPanicCategory, aPanicCode);
	}

/**
 * Static NewL
 *
 * @return CMMFTestUseOldCodecAudioController*
 */
CMMFController* CMMFTestUseOldCodecAudioController::NewL()
	{
	CMMFTestUseOldCodecAudioController* self = new(ELeave) CMMFTestUseOldCodecAudioController;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop( self );
	return STATIC_CAST( CMMFController*, self );
	}

/**
* ConstructL 
* 
* @xxxx
*/
void CMMFTestUseOldCodecAudioController::ConstructL()
	{
	iDataSource         = NULL;
	iDataSink           = NULL;
	iDataPath           = CMMFDataPath::NewL(iMediaId, *this);
	iSourceFormat       = NULL;
	iSinkFormat         = NULL;
	iSourceAndSinkAdded = EFalse;
	iStoppingRecording  = EFalse;


	//iMediaId has already been set up 
	SetState( EStopped );
	//iPrioritySettings  not initialised because they are held by the controller framework

	// Construct custom command parsers
	CMMFAudioPlayDeviceCustomCommandParser* audPlayDevParser = CMMFAudioPlayDeviceCustomCommandParser::NewL(*this);
	CleanupStack::PushL(audPlayDevParser);
	AddCustomCommandParserL(*audPlayDevParser);
	CleanupStack::Pop( audPlayDevParser );//audPlayDevParser

	CMMFAudioRecordDeviceCustomCommandParser* audRecDevParser = CMMFAudioRecordDeviceCustomCommandParser::NewL(*this);
	CleanupStack::PushL(audRecDevParser);
	AddCustomCommandParserL(*audRecDevParser);
	CleanupStack::Pop(audRecDevParser);//audRecDevParser

	CMMFAudioPlayControllerCustomCommandParser* audPlayConParser = CMMFAudioPlayControllerCustomCommandParser::NewL(*this);
	CleanupStack::PushL(audPlayConParser);
	AddCustomCommandParserL(*audPlayConParser);
	CleanupStack::Pop(audPlayConParser);//audPlayConParser

	CMMFAudioRecordControllerCustomCommandParser* audRecConParser = CMMFAudioRecordControllerCustomCommandParser::NewL(*this);
	CleanupStack::PushL(audRecConParser);
	AddCustomCommandParserL(*audRecConParser);
	CleanupStack::Pop(audRecConParser);//audRecParser

	CMMFAudioControllerCustomCommandParser* audConParser = CMMFAudioControllerCustomCommandParser::NewL(*this);
	CleanupStack::PushL(audConParser);
	AddCustomCommandParserL(*audConParser);
	CleanupStack::Pop(audConParser);//audConParser
	
	iWaitForAsyncService = new (ELeave) CActiveSchedulerWait;

	// [ assert the invariant now that we are constructed ]
	__ASSERT_ALWAYS( Invariant(), Panic( EBadInvariant));
	}

/**
*
* CMMFTestUseOldCodecAudioController
*
*/

CMMFTestUseOldCodecAudioController::CMMFTestUseOldCodecAudioController() : iMediaId(KUidMediaTypeAudio), iState(EStopped)
	{
	}

/**
*
* ~CMMFTestUseOldCodecAudioController
*
*/

CMMFTestUseOldCodecAudioController::~CMMFTestUseOldCodecAudioController()
	{
	// [ ensure we have logged off the thread ]
	if( iDataPath ) 	
		{
		iDataPath->ResetL();	// this does not leave
		}
	delete iDataPath;
	delete iSourceFormat;
	delete iSinkFormat;
	delete iStoppingMessage;
	delete iWaitForAsyncService;
	}

/**
 *  AddDataSourceL
 *
 *	Adds a data source to the controller
 *
 *	@param aSource
 * Preconditions:
 * We are stopped
 * Source does not already exist
 * Postconditions:
 * iDataSource != NULL
 * iDataSourceAdded == ETrue
 */
void CMMFTestUseOldCodecAudioController::AddDataSourceL(MDataSource& aSource)
	{
	//[ assert the invariant ]
	__ASSERT_ALWAYS( Invariant(), Panic( EBadInvariant));

	// [ precondition that the controller is stopped ]
    if( State() != EStopped )
		User::Leave( KErrNotReady );
	
	//[ precondition iData source is not already configured ]
	if (iDataSource)
		User::Leave(KErrAlreadyExists);

	// Note that this code is not generic for sources
	// It it only checks for file, des clips and audio inputs
	// If a new source type eg a Url Clip then this needs to be added to the supported source Uids
	if ( SourceFormatRequired( aSource) ) 
		{
		// Get the format from the Source if possible from no specific supplier
		TRAPD(err, iSourceFormat = CMMFFormatDecode::NewL(&aSource, KNullDesC));
		//[ we want to complete silently for KErrNotSupported
		// because there is a possibility that the client
		// wants to add the data format later, see audio api for
		// a description of this feature]
		if ((err != KErrNotSupported) && (err != KErrNone))
			{
			User::Leave(err);
			}
		}
	else if (aSource.DataSourceType()==KUidMmfAudioInput)
		{
		//[ ensure that the audio input has a pointer to dev sound ]
		CMMFAudioInput* audioInput = STATIC_CAST(CMMFAudioInput*, &aSource);
		__ASSERT_ALWAYS( audioInput, Panic(EBadInvariant));
		// [ lets load dev sound ]
		User::LeaveIfError(audioInput->SourceThreadLogon( *this ));
		}
	else
		{
		User::Leave(KErrNotSupported);
		}

	//[ its now safe to set the source ]
	iDataSource = &aSource ;
	iDataSource->SetSourcePrioritySettings(iPrioritySettings);

	//[ assert the post condition ]
	__ASSERT_ALWAYS(iDataSource, Panic(EMMFAudioControllerPanicDataSourceDoesNotExist));

	}


/**
 *  AddDataSinkL
 *
 *	Adds a data sink to the controller
 *
 *	@param aSink
 *
 */
void CMMFTestUseOldCodecAudioController::AddDataSinkL(MDataSink& aSink)
	{
	//[ assert the invariant ]
	__ASSERT_ALWAYS( Invariant(), Panic( EBadInvariant));

	// [ precondition that the controller is stopped ]
    if( State() != EStopped )
		User::Leave( KErrNotReady );

	// [ assert precondition that sink does not exist ]
	if (iDataSink)
		User::Leave(KErrAlreadyExists);


	// Note that this code is not generic for sinks
	// It it only checks for file,des clips and audio outputs
	// If a new sink type eg a Url Clip then this needs to be added to the supported source Uids
	if ( SinkFormatRequired( aSink ) )
		{//the sink is a clip

		// Get the format from the Sink if possible from no specific supplier
		TRAPD(err, iSinkFormat = CMMFFormatEncode::NewL(&aSink, KNullDesC));
		//[ we want to complete silently for KErrNotSupported
		// because there is a possibility that the client
		// wants to add the data format later, see audio api for
		// a description of this feature]
		if ((err != KErrNotSupported) && (err != KErrNone))
			{
			User::Leave(err);
			}
		}
	else if (aSink.DataSinkType()==KUidMmfAudioOutput)
		{

		//[ ensure that the audio output has a pointer to dev sound ]
		CMMFAudioOutput* audioOutput = STATIC_CAST(CMMFAudioOutput*, &aSink);
		__ASSERT_ALWAYS( audioOutput, Panic(EBadInvariant));
		// [ lets load dev sound ]
		User::LeaveIfError(audioOutput->SinkThreadLogon( *this ));
		}
	else
		{
		User::Leave(KErrNotSupported);
		}

	//[ now that we are sure we have not left we can update the sink
	// transactionally ]
	iDataSink = &aSink;
	iDataSink->SetSinkPrioritySettings(iPrioritySettings);

	// [ assert post conditions that a sink has been added ]
	__ASSERT_ALWAYS(iDataSink, Panic(EMMFAudioControllerPanicDataSinkDoesNotExist));

	}

/**
 *  PrimeL
 *
 *  If Prime fails the client should reset the controller
 *  because as noted below this code is not transactional.
 *
 */
void CMMFTestUseOldCodecAudioController::PrimeL()
	{
	//[ assert the invariant ]
	__ASSERT_ALWAYS( Invariant(), Panic( EBadInvariant));

	//[ assert the precondition ( in a friendly way for this api 
	// that we are either stopped or primed already ]
	if(!(( State() == EStopped ) || (State() == EPrimed )))
		User::Leave( KErrNotReady );

	// [ precondition we have a data source & sink ]
	__ASSERT_ALWAYS( iDataSource, Panic( EBadInvariant));
	__ASSERT_ALWAYS( iDataSink, Panic( EBadInvariant));


	//[ precondition that we need a source format ]
	if ( SourceFormatRequired(*iDataSource) && !(iSourceFormat))
		User::Leave( KErrNotSupported );

	// [ check the precondition if we need a data sink format ]
	if ( SinkFormatRequired(*iDataSink) && !( iSinkFormat ))
		User::Leave( KErrNotSupported );


	// [ ideally this code should be transaction based and
	//   if failure occurs we roll back to the previous state
	// in the code below this is not the case and the controller
	// can be left in an unstable state should any part of prime fail]
	if (iState == EStopped)
		{ //datapath propagates prime to sink & source
		
		TFourCC sinkFormatDataType;
		if ((iSinkFormat)&&(iDataSource))
			{//audio input so recording
			//need to assign the sink format 4CC to pcm16
			//even though it isn't pcm16 , this will cause the audio input
			//in negotiation to instantiate the null CMMFHwDevice plugin
			//after the negotiation set it back to the correct datatype
			//forcing the datapath to use a CMMFCodec
			sinkFormatDataType = iSinkFormat->SinkDataTypeCode(KUidMediaTypeAudio); //reset this after negotiation
			iSinkFormat->SetSinkDataTypeCode(KMMFFourCCCodePCM16, KUidMediaTypeAudio);
			}

		NegotiateL();

		if (!iSourceAndSinkAdded)
			{
			//add data source and sinks to datapath - Note cant do this in AddDataSource/Sink
			//because the sources and sinks aren't configured at this point
			if (iSourceFormat)
				iDataPath->AddDataSourceL(iSourceFormat);
			else if (iDataSource)
				{//record
				iDataPath->AddDataSourceL(iDataSource);
				if (iSinkFormat) //restore the format data type
					{	//this will force datapth to use CMMFCodec
					iSinkFormat->SetSinkDataTypeCode(sinkFormatDataType,KUidMediaTypeAudio);
					}
				}
			if (iSinkFormat)
				iDataPath->AddDataSinkL(iSinkFormat);
			else if (iDataSink)
				{//play
				//need to set the audio output fourCC code to pcm16
				//in order to force the data path to use a CMMFCodec
				CMMFAudioOutput* ao = static_cast<CMMFAudioOutput*>(iDataSink);
				CMMFDevSound* devSound = &(ao->SoundDevice());

				//PrimeL() needs to run synchronously but DevSound initialisation is 
				//asynchronous.  iWaitForAsyncService halts execution until 
				//iDevSoundEventHandler informs us this process has completed.
				iDevSoundEventHandler.SetInterceptedDevSoundObserver(ao);
				iDevSoundEventHandler.SetEventObserver(this);
				//need to reinitialize the devsound to load up a null pcm16->16
				//hw device - note this will delete the previous
				//non pcm16 hw device
				//note can't set this on the audio output as this won't update
				//the hw device
				devSound->InitializeL(iDevSoundEventHandler,KMMFFourCCCodePCM16,EMMFStatePlaying);
				iWaitForAsyncService->Start();

				//now tell audio output were pcm16
				iDataSink->SetSinkDataTypeCode(KMMFFourCCCodePCM16, KUidMediaTypeAudio);
				iDataPath->AddDataSinkL(iDataSink);
				}
			iSourceAndSinkAdded = ETrue ;
			}
		
		iDataPath->PrimeL();
		if ((iSinkFormat) && (!iSourceFormat))
			{//we are recording to a clip so the data path position is the sink
			//need to set datapath position to end of format pos (incase sink clip already exists
			TTimeIntervalMicroSeconds duration = iSinkFormat->Duration(iMediaId);
			if (duration != TTimeIntervalMicroSeconds(0))
				{//the file already exists and has a duration so set data path position to the end of the file
				iDataPath->SetPositionL(duration);
				}
			}
		//[ it is now safe to make the transition to primed ]
		SetState( EPrimed );		
		}
	else if (State() == EPrimed)
		{ //controller is already primed so just pass prime onto DP
		iDataPath->PrimeL();
		}
	
	//[ assert the post condition that we are in the state primed]
	__ASSERT_ALWAYS( SetState( EPrimed ), Panic( EPostConditionViolation ));
	// [ assert the invariant]
	__ASSERT_ALWAYS( Invariant(), Panic( EBadInvariant ) );
	}

/**
 *  ResetL
 *  This method resets the controller
 *
 */
void CMMFTestUseOldCodecAudioController::ResetL()
	{
	__ASSERT_ALWAYS( Invariant(), Panic( EBadInvariant ) );

	// Stop recording if it's not stopped,
	if (State() != EStopped)
		{
		iDataPath->Stop();
		SetState(EStopped);
		}

	// Remove references to source and sink
	iDataPath->ResetL();

	delete iSourceFormat; iSourceFormat = NULL  ;
	delete iSinkFormat;	iSinkFormat = NULL  ;

	//[ ensure loggoff of source and sink ]
	iDataSource = NULL ;
	iDataSink = NULL ;
	iSourceAndSinkAdded = EFalse;

	// [ assert the invariant]
	__ASSERT_ALWAYS( Invariant(), Panic( EBadInvariant ) );

	// [ assert the post condition
	//   state == stopped
	//   iDataSource is NULL
	//   iSourceFormat is NULL
	//   iSinkFormat is NULL ]
	__ASSERT_ALWAYS( ResetPostCondition(), Panic( EBadReset ));
	__ASSERT_ALWAYS( Invariant(), Panic(EBadState));
	}

/**
* ResetPostCondition
* This function determnines if the reset post condition is valid
* @xxxx
*/
TBool CMMFTestUseOldCodecAudioController::ResetPostCondition() const
	{

     TBool result = EFalse ;
	if((iSourceFormat     == NULL)  &&
	(iDataSink            == NULL)  &&
	(iDataSource          == NULL)  && 
	(iSinkFormat          == NULL)  &&
	(State() == EStopped))
		{
         result = ETrue;
		}

    return result;
	}


/**
 *
 * PlayL
 *
 */
void CMMFTestUseOldCodecAudioController::PlayL()
	{
	// [ assert the precondition that the
	//   play command is only activated in the primed state]
	if ( State() != EPrimed)
		User::Leave(KErrNotReady);

	// [ assert the Invariant ]
	__ASSERT_ALWAYS( Invariant(), Panic(EBadInvariant));

	//[datapath propogates play to sink & source]
	iDataPath->PlayL();
	SetState( EPlaying );
	
	//[ assert the post condition we are playing ]
	//No - this assumption is not always true if an error occurs eg OOM
	//the state could be EStopped
	//	__ASSERT_ALWAYS( (State() == EPlaying ), Panic( EBadState));
	//[ assert the invariant ]
	__ASSERT_ALWAYS( Invariant(), Panic(EBadInvariant));
	}

/**
 *  PauseL
 *
 */
void CMMFTestUseOldCodecAudioController::PauseL()
	{
	// [ assert the invariant ]
	__ASSERT_ALWAYS( Invariant(), Panic(EBadInvariant));
	
	//[ assert the precondition that we are playing ]
	if ( State() != EPlaying)
		User::Leave(KErrNotReady);
	
	//[ datapath propogates pause to sink & source ]
	iDataPath->Pause();
	SetState(EPrimed);

	//[ assert the post condition we are primed ]
	__ASSERT_ALWAYS( (State() == EPrimed ), Panic( EBadState));
	//[ assert the invariant ]
	__ASSERT_ALWAYS( Invariant(), Panic(EBadInvariant));
	}

/**
 *  StopL
 *
 */
void CMMFTestUseOldCodecAudioController::StopL(TMMFMessage& aMessage)
	{
	//[ assert the invariant ]
	__ASSERT_ALWAYS( Invariant(), Panic(EBadInvariant));
	// [ precondition that we are not already stopped 
	// && if we are stopped do nothing.
	//If we are stopping a recording, we need to give the datapath chance to 
	//process that data which has already been captured. We therefore stay in the EPlaying
	//state, but use iStoppingRecording to indicate that we are stopping.

	if ((State() != EStopped) && !iStoppingRecording)
		{
		if((State() == EPlaying) && (iDataSource->DataSourceType()==KUidMmfAudioInput)) //we are recording
			{
			// datapath is requested to stop recording but process any alreay captured buffers,
			// the pause method is used for this purpose and as such, the data path must 
			// determine that it is recording to be able to act accordingly.
			// aMessgae is not completed until datapath advises that it has completed.
			iDataPath->Pause();
			iStoppingMessage = CMMFMessageHolder::NewL(aMessage);
			iStoppingRecording = ETrue;
			}
		else
			{
			//  datapath propogates stop to sink & source
			iDataPath->Stop();
			SetState(EStopped);
			}
		}

	//complete message as request is complete.
	if(State() == EStopped && !IsUnderTest())
		{
		aMessage.Complete(KErrNone);
		}

	//[ assert the invariant ]
	__ASSERT_ALWAYS( Invariant(), Panic(EBadInvariant));
	}

/**
 *  PositionL
 * Preconditions:
 * The Controller is in the state EPrimed
 * @return TTimeIntervalMicroSeconds
 *
 */
TTimeIntervalMicroSeconds CMMFTestUseOldCodecAudioController::PositionL() const
	{
	//[ assert the invariant ]
	__ASSERT_ALWAYS( Invariant(), Panic(EBadInvariant));
	// [ precondition that we are playing or primed ]
	if( !((State() == EPrimed) || (State() == EPlaying)))
			User::Leave(KErrNotReady);

    TTimeIntervalMicroSeconds position = iDataPath->Position();
	
	//[ assert the invariant ]
	__ASSERT_ALWAYS( Invariant(), Panic(EBadInvariant));
	
	return position;
	}

/**
* SetPositionL
*
* @param aPosition
*
*/
void CMMFTestUseOldCodecAudioController::SetPositionL(const TTimeIntervalMicroSeconds& aPosition)
	{
	//[ assert the invariant ]
	__ASSERT_ALWAYS( Invariant(), Panic(EBadInvariant));
	
	// [ precondition that we are not already stopped ]
	if (iState == EStopped)
		User::Leave(KErrNotReady);

	//[ precondition that the position is >= 0 && <= Duration ]
		{
		TTimeIntervalMicroSeconds theDuration(0);
		if (iSourceFormat)
			{ //if the source is a clip then the duration always refers to the source - even if the sink is a clip
			theDuration = iSourceFormat->Duration(iMediaId);
			}
		else if (iSinkFormat)
			{ //duration of recorded clip
			theDuration = iSinkFormat->Duration(iMediaId);
			}
		TTimeIntervalMicroSeconds theStart(0);
		if( ( aPosition < theStart) || ( aPosition > theDuration) )
			//[ invalid position before start and after end]
			User::Leave(KErrArgument); 
		}

	//[ set the position on the data path ]
	iDataPath->SetPositionL(aPosition);
	
	//[ assert the invariant ]
	__ASSERT_ALWAYS( Invariant(), Panic(EBadInvariant));

    // [ post condition not checked ]
	//[ we do not compare the set position with get postion
    //  because the interface to do so is poor ]
	}

/**
*
* DurationL
*
* @returns TTimeIntervalMicroSeconds 
*
*/
TTimeIntervalMicroSeconds CMMFTestUseOldCodecAudioController::DurationL() const
	{
	//[ assert the invariant ]
	__ASSERT_ALWAYS( Invariant(), Panic(EBadInvariant));


	// [ assert we have a format that supports duration ]
	if( !( iSourceFormat || iSinkFormat ) )
		User::Leave(KErrNotSupported);
	
	//[ now do the real work of getting the duration ]
	// ------------------------------------------------
	TTimeIntervalMicroSeconds theDuration(0);
	if (iSourceFormat)
		{ //if the source is a clip then the duration always refers to the source - even if the sink is a clip
		theDuration = iSourceFormat->Duration(iMediaId);
		}
	else if (iSinkFormat)
		{ //duration of recorded clip
		theDuration = iSinkFormat->Duration(iMediaId);
		}

	//[ assert the invariant ]
	__ASSERT_ALWAYS( Invariant(), Panic(EBadInvariant));

	return theDuration; 
	}

/**
*
* GetNumberOfMetaDataEntriesL
*
* @param "TInt"
*
*/
void CMMFTestUseOldCodecAudioController::GetNumberOfMetaDataEntriesL(TInt& aNumberOfEntries )
	{

	//[ assert the invariant ]
	__ASSERT_ALWAYS( Invariant(), Panic(EBadInvariant));

	//[ precondition that we are in the primed state or stopped ]
	if( !((State() == EPrimed) || ( State() == EStopped)))
		User::Leave(KErrNotReady);

    // [ precondition there is a sink format ]
	if (!iDataSink)
		User::Leave(KErrNotSupported);

	// [ precondition the sink format is an encode format ]
	if ((iDataSink->DataSinkType()!=KUidMmfAudioOutput) &&
		(iDataSource->DataSourceType()!= KUidMmfAudioInput) )
		User::Leave(KErrNotSupported);

	if (iDataSink->DataSinkType()==KUidMmfAudioOutput)
		{

		//[ precondition the format exists ]
		if( !iSourceFormat )
			User::Leave(KErrNotSupported);

		//[ Get the Number of meta data entries from the sink format ]
		iSourceFormat->GetNumberOfMetaDataEntriesL( aNumberOfEntries );
		}
	else if (iDataSource->DataSourceType()==KUidMmfAudioInput)
		{
		if( !iSinkFormat )
			User::Leave(KErrNotSupported);

		iSinkFormat->GetNumberOfMetaDataEntriesL( aNumberOfEntries );
		}

	//[ assert the invariant ]
	__ASSERT_ALWAYS( Invariant(), Panic(EBadInvariant));

	}

/**
* GetMetaDataEntryL
* @param aIndex
* @returns "CMMFMetaDataEntry*"
*/
CMMFMetaDataEntry* CMMFTestUseOldCodecAudioController::GetMetaDataEntryL(TInt aIndex )
	{
		//[ assert the invariant ]
	__ASSERT_ALWAYS( Invariant(), Panic(EBadInvariant));

	//[ precondition that we are in the primed state or stopped ]
	if( !((State() == EPrimed) || ( State() == EStopped)))
		User::Leave(KErrNotReady);

    // [ precondition there is a sink format ]
	if (!iDataSink)
		User::Leave(KErrNotSupported);

	iDataSink->DataSinkType();
	iDataSource->DataSourceType();

	// [ precondition the sink or source is either an audio output or input ]
	if ((iDataSink->DataSinkType()!= KUidMmfAudioOutput) &&
		(iDataSource->DataSourceType()!= KUidMmfAudioInput ))
		User::Leave(KErrNotSupported);

	//[ Get the meta data entry from the sink format ]
	CMMFMetaDataEntry*  theEntry = NULL;

	if (iDataSink->DataSinkType()==KUidMmfAudioOutput)
		{ 
		//[ precondition the format exists ]
		if( !iSourceFormat )
			User::Leave(KErrNotSupported);

		//[ Get the Number of meta data entries from the sink format ]
		theEntry = iSourceFormat->MetaDataEntryL(aIndex);
		}
	else if (iDataSource->DataSourceType()==KUidMmfAudioInput)
		{
		//[ precondition the format exits ]
		if( !iSinkFormat )
			User::Leave(KErrNotSupported);
		theEntry = iSinkFormat->MetaDataEntryL(aIndex);
		}

	//[ assert the post condition that the entry is not null ]
	__ASSERT_ALWAYS( theEntry, Panic(EBadInvariant));

	//[ assert the invariant ]
	__ASSERT_ALWAYS( Invariant(), Panic(EBadInvariant));

	return theEntry;
	}

/**
* RemoveDataSourceL
* @param aDataSource
* 
*/
void CMMFTestUseOldCodecAudioController::RemoveDataSourceL(MDataSource& aDataSource )
	{
	//[ assert the invariant ]
	__ASSERT_ALWAYS( Invariant(), Panic(EBadInvariant) );

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
         __ASSERT_ALWAYS( iDataPath, Panic( EBadState )); 
	     //[ Remove references to source and sink ]
	     iDataPath->ResetL();
		 iSourceAndSinkAdded = EFalse ;
		 }

	 // [ delete the data sink and format ]
	 iDataSource = NULL ;
	 delete iSourceFormat;
	 iSourceFormat = NULL;
		
	// [ assert postcondition we are stopped ]
	__ASSERT_ALWAYS( (State() == EStopped), Panic(EPostConditionViolation) );

	//[ assert postcondition the SourceAndSinkAdded is false ]
	__ASSERT_ALWAYS( !iSourceAndSinkAdded, Panic( EPostConditionViolation ));
	
	//[ assert postcondition the data sinkformat  is null ]
	__ASSERT_ALWAYS( (iSourceFormat == NULL ), Panic( EPostConditionViolation ));

	//[ assert postcondition the data sink  is null ]
	__ASSERT_ALWAYS( (iDataSource == NULL ), Panic( EPostConditionViolation ));

	//[ assert the invariant ]
	__ASSERT_ALWAYS( Invariant(), Panic(EBadInvariant));

	}

/**
* RemoveDataSinkL
* 
* @param aDataSink
*
*/
void CMMFTestUseOldCodecAudioController::RemoveDataSinkL(MDataSink& aDataSink )
	{
	//[ assert the invariant ]
	__ASSERT_ALWAYS( Invariant(), Panic(EBadInvariant) );

	//[ precondition is that we have a data sink ]
	if( !iDataSink )
		User::Leave(KErrNotSupported);

	//[precondition the data sink is the data sink we have]
	if( iDataSink != &aDataSink )
		User::Leave(KErrNotSupported);

	//[ the controller is in the stopped state ]
	if(State() != EStopped)
		User::Leave(KErrNotReady);

	//[ remove the data sink from the controller and delete the format]
     if( iSourceAndSinkAdded )
		 {
         __ASSERT_ALWAYS( iDataPath, Panic( EBadState ));
         //[ Remove references to source and sink ]
	     iDataPath->ResetL();
		 iSourceAndSinkAdded = EFalse ;
		 }

	 // [ reset data sink referenece and remove the format ]
	 iDataSink = NULL ;
	 delete iSinkFormat;
	 iSinkFormat = NULL;
		
	// [ assert postcondition we are stopped ]
	__ASSERT_ALWAYS( (State() == EStopped), Panic(EPostConditionViolation) );

	//[ assert postcondition the SourceAndSinkAdded is false ]
	__ASSERT_ALWAYS( !iSourceAndSinkAdded, Panic( EPostConditionViolation ));
	
	//[ assert postcondition the data sinkformat  is null ]
	__ASSERT_ALWAYS( (iSinkFormat == NULL ), Panic( EPostConditionViolation ));

	//[ assert postcondition the data sink  is null ]
	__ASSERT_ALWAYS( (iDataSink == NULL ), Panic( EPostConditionViolation ));

	//[ assert the invariant ]
	__ASSERT_ALWAYS( Invariant(), Panic(EBadInvariant));
	}

/**
 *  CustomCommand
 *  @param aMessage
 */
void CMMFTestUseOldCodecAudioController::CustomCommand(TMMFMessage& aMessage)
	{
	//[ assert the invariant ]
	__ASSERT_ALWAYS( Invariant(), Panic(EBadInvariant));
	// [ We do not have any custom commands ]
	aMessage.Complete(KErrNotSupported);
	}

/**
* NegotiateL
* 
*/
void CMMFTestUseOldCodecAudioController::NegotiateL()
	{
	//[ assert the invariant ]
	__ASSERT_ALWAYS( Invariant(), Panic(EBadInvariant));

	//utility function used by custom to negotiate source sink settings after a change
	if ((iSourceFormat)&&(iSinkFormat)) //convert
		{
		iSinkFormat->NegotiateL(*iSourceFormat);
		iSourceFormat->NegotiateSourceL(*iSinkFormat);
		iSinkFormat->NegotiateL(*iSourceFormat);

		// check for upsampling attempts
		if (iSinkFormat->SampleRate() > iSourceFormat->SampleRate())
			{
			// we don't support upsampling
			User::Leave( KErrNotSupported );
			}
		}
	else if ((iDataSource)&&(iSinkFormat)) //record
		{
		// need two step negotiation for record
		// first try to set the audio input settings to match the required settings for recording
		iDataSource->NegotiateSourceL(*iSinkFormat);
		// now call negotiateL on the sink in order to tell it what the audio input was set to.
		iSinkFormat->NegotiateL(*iDataSource);
		}
	else if ((iSourceFormat)&&(iDataSink)) //play
		iDataSink->NegotiateL(*iSourceFormat);

	//[ assert the invariant ]
	__ASSERT_ALWAYS( Invariant(), Panic(EBadInvariant));
	}

/**
 *  SetPrioritySettings
 *
 *	@param aPrioritySettings
 */
void CMMFTestUseOldCodecAudioController::SetPrioritySettings(const TMMFPrioritySettings& aPrioritySettings)
	{
	//[ assert the invariant ]
	__ASSERT_ALWAYS( Invariant(), Panic(EBadInvariant));

	//[ assert the precondition ]
	if(State() != EStopped)
		{
		ASSERT(EFalse);		// used to leave here with KErrNotReady
		return;
		}

	//[ update the priority settings of the controller]
	iPrioritySettings = aPrioritySettings;

	//pass settings on to source and sink
	if (iDataSource)
		{
		iDataSource->SetSourcePrioritySettings(iPrioritySettings);
		}
	if (iDataSink)
		{
		iDataSink->SetSinkPrioritySettings(iPrioritySettings);
		}

    // assert the post condition
	//__ASSERT_ALWAYS( (iPrioritySettings == aPrioritySettings), Panic( ));
	//[ assert the invariant ]
	__ASSERT_ALWAYS( Invariant(), Panic(EBadInvariant));
	}

/**
 *  SendEventToClient
 *
 *	@param aEvent
 */
TInt CMMFTestUseOldCodecAudioController::SendEventToClient(const TMMFEvent& aEvent)
	{
	//[ assert the invariant ]
	__ASSERT_ALWAYS( Invariant(), Panic(EBadInvariant));

	TMMFEvent controllerEvent;
	//Were going to stop playing, force event type to be the correct type
	controllerEvent.iEventType = KMMFEventCategoryPlaybackComplete;
	controllerEvent.iErrorCode = aEvent.iErrorCode;



	//If we receive KErrNone from the DataPath, it indicates that it has 
	//successfully completed playing/converting/recording.
	if ((aEvent.iEventType == KMMFEventCategoryPlaybackComplete) && 
		(aEvent.iErrorCode == KErrNone))
		{
		if(iStoppingRecording)
			{
			iStoppingRecording = EFalse;
			iDataPath->Stop();
			SetState( EStopped );
			
			//complete the clients stop request
			iStoppingMessage->Complete(KErrNone);
			delete iStoppingMessage; iStoppingMessage=NULL;

			//we don't want to send an event to the client
			return KErrNone;
			}
		else
			{//datapath has reached end of file so set internal state to primed
			SetState( EPrimed );
			}
		}
	else
		{
		if ( State()!= EStopped)
			{
			//datapath propogates stop to sink & source
			iDataPath->Stop();
			SetState( EStopped );

			if(iStoppingRecording)
				{// an error has occurred while we were waiting for recording to stop, 
				 //must complete clients request
				iStoppingRecording = EFalse;
				iStoppingMessage->Complete(aEvent.iErrorCode);
				delete iStoppingMessage; iStoppingMessage=NULL;
				}
			}
		}

	//now send event to client...
	TInt result = DoSendEventToClient(controllerEvent);
	
	//[ assert the invariant ]
	__ASSERT_ALWAYS( Invariant(), Panic(EBadInvariant));

	return result;
	}


/**
* MapdSetVolumeL
*
*  @param aVolume
*
*/
void CMMFTestUseOldCodecAudioController::MapdSetVolumeL(TInt aVolume)
	{
	//[ assert the invariant ]
	__ASSERT_ALWAYS( Invariant(), Panic(EBadInvariant));

	// [  precondition is true for state 
	//    we can set the volume in any state ]

	//[ precondition we have a data sink ]
	if (!iDataSink)
		User::Leave(KErrNotReady);

    // [ precondition that the data sink is an audio output ]
	// Make sure that iDataSink is an Audio Output
	if (iDataSink->DataSinkType() != KUidMmfAudioOutput)
				User::Leave(KErrNotSupported);

	MMMFAudioOutput* audioOutput = STATIC_CAST(MMMFAudioOutput*, iDataSink);

	// [ assert the precondition that aVolume is in range ]
	TInt maxVolume = audioOutput->SoundDevice().MaxVolume();
	if( ( aVolume < 0 ) || ( aVolume > maxVolume ))
			User::Leave(KErrArgument);
	
	//[ set the volume on the device ]
	audioOutput->SoundDevice().SetVolume(aVolume);

	//[ assert the post condition volume is equal to a volume]
	TInt soundVolume = 0;
	soundVolume = audioOutput->SoundDevice().Volume();

    __ASSERT_ALWAYS( ( soundVolume == aVolume), Panic(EPostConditionViolation));

	//[ assert the invariant ]
	__ASSERT_ALWAYS( Invariant(), Panic(EBadInvariant));
	}

/**
*
* MapdGetMaxVolumeL
*
* @param aMaxVolume
*
*/
void CMMFTestUseOldCodecAudioController::MapdGetMaxVolumeL(TInt& aMaxVolume)
	{
	// [ assert the invariant ]
	__ASSERT_ALWAYS( Invariant(), Panic(EBadInvariant));

	//[ we can get max volume in any state ]

	// [ precondition we must have a data sink ]
	if (!iDataSink)
		User::Leave(KErrNotReady);

	//[ precondition the sink must be an audio output]
	// Make sure that iDataSink is an Audio Output
	if (iDataSink->DataSinkType() != KUidMmfAudioOutput)
			User::Leave(KErrNotSupported);

	//[ get the volume from the device ]
	MMMFAudioOutput* audioOutput = STATIC_CAST(MMMFAudioOutput*, iDataSink);	
	aMaxVolume = audioOutput->SoundDevice().MaxVolume();

	//[ assert the invariant ]
	__ASSERT_ALWAYS( Invariant(), Panic(EBadInvariant));

	}


/**
*
* MapdGetVolumeL
*
* @param aVolume
*
*/
void CMMFTestUseOldCodecAudioController::MapdGetVolumeL(TInt& aVolume)
	{
	// [ assert the invariant ]
	__ASSERT_ALWAYS( Invariant(), Panic(EBadInvariant));

	//[  precondition that we have a data sink ]
	if (!iDataSink)
		User::Leave(KErrNotReady);

	//[ precondition iDataSink is an Audio Output ]
	if (iDataSink->DataSinkType() != KUidMmfAudioOutput)
		User::Leave(KErrNotSupported);

	// [ get the volume ]
	MMMFAudioOutput* audioOutput = STATIC_CAST(MMMFAudioOutput*, iDataSink);
	aVolume = audioOutput->SoundDevice().Volume();
	
	// [ assert precondition that the volume is in range
	//     0.. aMaxVolume ]
	TInt aMaxVolume = audioOutput->SoundDevice().MaxVolume();
	__ASSERT_ALWAYS( (aVolume <= aMaxVolume), Panic(EBadState));
	__ASSERT_ALWAYS( (aVolume >= 0), Panic(EBadState));

	// [ assert the invariant ]
	__ASSERT_ALWAYS( Invariant(), Panic(EBadInvariant));

	}

/**
*
* MapdSetVolumeRampL
*
* @param aRampDuration
*
*/
void CMMFTestUseOldCodecAudioController::MapdSetVolumeRampL(const TTimeIntervalMicroSeconds& aRampDuration)
	{
     // [ assert the invariant ]
	__ASSERT_ALWAYS( Invariant(), Panic(EBadInvariant));

	//[ precondition that we have a data sink ]
	if (!iDataSink)
		User::Leave(KErrNotReady);

	// [ precondition iDataSink is an Audio Output ]
	if (iDataSink->DataSinkType() != KUidMmfAudioOutput)
		User::Leave(KErrNotSupported);

	//[ set the volume ramp ]
	MMMFAudioOutput* audioOutput = STATIC_CAST(MMMFAudioOutput*, iDataSink);
	audioOutput->SoundDevice().SetVolumeRamp(aRampDuration);
	
	//[ assert the invariant ]
	__ASSERT_ALWAYS( Invariant(), Panic(EBadInvariant));
		
	}


/**
*
* MapdSetBalanceL
*
* @param aBalance
*
*/
void CMMFTestUseOldCodecAudioController::MapdSetBalanceL(TInt aBalance)
	{
	//[ assert the invariant ]
	__ASSERT_ALWAYS( Invariant(), Panic( EBadInvariant));

	// [ precondition is that we have a data sink ]
	if (!iDataSink)
		User::Leave(KErrNotReady);
	
	// [ precondition is that the data sink is an audio output]
	if (iDataSink->DataSinkType() != KUidMmfAudioOutput)
		User::Leave(KErrNotSupported);
	
	//[ get the audio output ]
	MMMFAudioOutput* audioOutput = STATIC_CAST(MMMFAudioOutput*, iDataSink);

	// [ separate out left and right balance ]
	TInt left  = 0;
	TInt right = 0;
	CalculateLeftRightBalance( left, right, aBalance );
	
	//[ set the balance ]
	audioOutput->SoundDevice().SetPlayBalanceL(left, right); 

	// [assert the post condition that the balance is set correctly]
	TInt rightBalance = 0;
	TInt leftBalance  = 0;
	audioOutput->SoundDevice().GetPlayBalanceL(leftBalance, rightBalance); 

	//[ assert post condition holds]
	TBool postCondition = (( rightBalance == right) && ( leftBalance == left));
	__ASSERT_ALWAYS( postCondition, Panic( EPostConditionViolation ) );

	//[ assert the invariant ]
	__ASSERT_ALWAYS( Invariant(), Panic( EBadInvariant));
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
* @xxxx
*/
void CMMFTestUseOldCodecAudioController::CalculateLeftRightBalance( TInt& aLeft, TInt& aRight, TInt aBalance ) const
	{
	// Check the balance is within limits & modify to min or max values if necessary
	if (aBalance < KMMFBalanceMaxLeft)
		aBalance = KMMFBalanceMaxLeft;
	if (aBalance > KMMFBalanceMaxRight)
		aBalance = KMMFBalanceMaxRight;

	// [ assert precondition that aBalance is within limits ]
    __ASSERT_ALWAYS( !(aBalance < KMMFBalanceMaxLeft || aBalance > KMMFBalanceMaxRight), Panic(EBadArgument));
	
	//[ Now separate percentage balances out from aBalance ]
	 aLeft = (100 * (aBalance-KMMFBalanceMaxRight)) / (KMMFBalanceMaxLeft-KMMFBalanceMaxRight);
     aRight = 100 - aLeft;

	 //[ assert post condition that left and right are within range ]
	 __ASSERT_ALWAYS( ( (aLeft <= 100) && (aLeft >= 0) ), Panic(EPostConditionViolation));
	 __ASSERT_ALWAYS( ( (aRight <= 100) && (aRight >= 0) ), Panic(EPostConditionViolation));
	}


/**
* MapdGetBalanceL
* @param aBalance
*
*/
void CMMFTestUseOldCodecAudioController::MapdGetBalanceL(TInt& aBalance)
	{
	//[ assert the invariant ]
	__ASSERT_ALWAYS( Invariant(), Panic(EBadInvariant));

	//[ precondition that we have a sink]
	if (!iDataSink)
		User::Leave(KErrNotReady);
	
	// [ iDataSink is an Audio Output ]
	if (iDataSink->DataSinkType() != KUidMmfAudioOutput)
		User::Leave(KErrNotSupported);
	
	// [ get the play balance ]
	MMMFAudioOutput* audioOutput = STATIC_CAST(MMMFAudioOutput*, iDataSink);
	TInt left = 50; // arbitrary values 
	TInt right = 50;
	audioOutput->SoundDevice().GetPlayBalanceL(left, right); 
    CalculateBalance( aBalance, left, right );

	//[ assert the invariant ]
	__ASSERT_ALWAYS( Invariant(), Panic(EBadInvariant));
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
void CMMFTestUseOldCodecAudioController::CalculateBalance( TInt& aBalance, TInt aLeft, TInt aRight ) const
	{
	//[ assert pre conditions ]
	__ASSERT_ALWAYS( (( 0 <= aLeft) && ( 100 >= aLeft)), Panic( EBadArgument) );
	__ASSERT_ALWAYS( (( 0 <= aRight) && ( 100 >= aRight)), Panic( EBadArgument) );
	
	if ((aLeft > 0) && (aRight > 0))
		{
		__ASSERT_ALWAYS( (( aLeft + aRight ) == 100 ), Panic( EBadArgument ));
		aBalance = (aLeft * (KMMFBalanceMaxLeft-KMMFBalanceMaxRight))/100 + KMMFBalanceMaxRight;
		}
	else if ((aLeft == 0) && (aRight == 0))
		{
		aBalance = 0;
		}
	else if ((aLeft == 0) && (aRight > 0))
		{
		aBalance = 100;
		}
	else if ((aLeft > 0) && (aRight == 0))
		{
		aBalance = -100;
		}

    //[ assert post condition that aBalance is within limits ]
	__ASSERT_ALWAYS( !(aBalance < KMMFBalanceMaxLeft || aBalance > KMMFBalanceMaxRight), Panic(EBadArgument));
	
	}

/**
* MardSetGainL
* @param aGain
*
*/
void CMMFTestUseOldCodecAudioController::MardSetGainL(TInt aGain)
	{
	// [ assert the invariant ]
	__ASSERT_ALWAYS( Invariant(), Panic(EBadInvariant));

	//[ precondition we are in the state stopped ]
	if(State() != EStopped)
		User::Leave(KErrNotReady);
	
	// [ assert the precondition that we have a data sink ]
	if (!iDataSource)
		User::Leave(KErrNotSupported);

	//[ assert the precondition that the data sink is an audio input ]
	if (iDataSource->DataSourceType() != KUidMmfAudioInput)
		User::Leave(KErrNotReady);

	// Set gain of sound device
	MMMFAudioInput* audioInput = STATIC_CAST(MMMFAudioInput*, iDataSource);
	audioInput->SoundDevice().SetGain(aGain);
	
	//[ assert the invariant ]
	__ASSERT_ALWAYS( Invariant(), Panic(EBadInvariant));

	}
		
/**
* MardGetMaxGainL
* @param aMaxGain
*
*/
void CMMFTestUseOldCodecAudioController::MardGetMaxGainL(TInt& aMaxGain)
	{
	// [ assert the invariant ]
	__ASSERT_ALWAYS( Invariant(), Panic(EBadInvariant));

	// [ assert the precondition that we have a source ]
	if (!iDataSource)
		User::Leave(KErrNotReady);

	//[ assert the precondition that iDataSink is an Audio Input]
	if (iDataSource->DataSourceType() != KUidMmfAudioInput)
		User::Leave(KErrNotSupported);

	MMMFAudioInput* audioInput = STATIC_CAST(MMMFAudioInput*, iDataSource);
	aMaxGain = audioInput->SoundDevice().MaxGain();

	//[ assert the invariant ]
	__ASSERT_ALWAYS( Invariant(), Panic(EBadInvariant));
	
	}

/**
* MardGetGainL
* @param aGain
*
*/
void CMMFTestUseOldCodecAudioController::MardGetGainL(TInt& aGain)
	{
	//[ assert the invariant ]
	__ASSERT_ALWAYS( Invariant(), Panic(EBadInvariant));

	// [ assert the precondition that we have a sink ]
	if (!iDataSource)
		User::Leave(KErrNotReady);

	// [ assert the precondition that we have an audio input sink]
	if (iDataSource->DataSourceType() != KUidMmfAudioInput)
			User::Leave(KErrNotSupported);

	MMMFAudioInput* audioInput = STATIC_CAST(MMMFAudioInput*, iDataSource);
	aGain = audioInput->SoundDevice().Gain();
		
	//[ assert the invariant ]
	__ASSERT_ALWAYS( Invariant(), Panic(EBadInvariant));
	}


/**
 *
 * MardSetBalanceL
 *   @param aBalance
 */
void CMMFTestUseOldCodecAudioController::MardSetBalanceL(TInt aBalance)
	{
	// [ assert the invaraiant ]
	__ASSERT_ALWAYS( Invariant(), Panic(EBadInvariant));

	// [ precondition is that we have a data sink ]
	if (!iDataSink)
		User::Leave(KErrNotReady);
	
	// [ precondition is that the balance is in range ]
	// Make sure aBalance is in the range -100 <-> 100
	if (aBalance < KMMFBalanceMaxLeft || aBalance > KMMFBalanceMaxRight)
		User::Leave(KErrArgument);
	
	// [ precondition is that the data sink is an audio output]
	if (iDataSource->DataSourceType() != KUidMmfAudioInput)
		User::Leave(KErrNotSupported);
	
    
	//[ get the audio output ]
	MMMFAudioInput* audioInput = STATIC_CAST(MMMFAudioInput*, iDataSource);

	// [ separate out left and right balance ]
	TInt left  = 0;
	TInt right = 0;
	CalculateLeftRightBalance( left, right, aBalance );
	
	//[ set the balance ]
	audioInput->SoundDevice().SetRecordBalanceL(left, right); 

	// [assert the post condition that the balance is set correctly]
	TInt rightBalance = 0;
	TInt leftBalance  = 0;
	audioInput->SoundDevice().GetRecordBalanceL(leftBalance, rightBalance); 

	//[ assert post condition holds]
	TBool postCondition = (( rightBalance == right) && ( leftBalance == left));
	__ASSERT_ALWAYS( postCondition, Panic( EPostConditionViolation ) );

	//[ assert the invariant ]
	__ASSERT_ALWAYS( Invariant(), Panic( EBadInvariant));
	
	}

/**
*
* MardGetBalanceL
* @param aBalance
*
*/
void CMMFTestUseOldCodecAudioController::MardGetBalanceL(TInt& aBalance)
	{
	//[ assert the invariant ]
	__ASSERT_ALWAYS( Invariant(), Panic(EBadInvariant));

	//[ precondition that we have a sink]
	if (!iDataSink)
		User::Leave(KErrNotReady);
	
	// [ iDataSink is an Audio Output ]
	if (iDataSource->DataSourceType() != KUidMmfAudioInput)
		User::Leave(KErrNotSupported);
	
	// [ get the play balance ]
	MMMFAudioInput* audioInput = STATIC_CAST(MMMFAudioInput*, iDataSource);
	TInt left = 50; // arbitrary values 
	TInt right = 50;
	audioInput->SoundDevice().GetRecordBalanceL(left, right); 
    CalculateBalance( aBalance, left, right );

	//[ assert the invariant ]
	__ASSERT_ALWAYS( Invariant(), Panic(EBadInvariant));

	}

/**
* MapcSetPlaybackWindowL
* @param aStart
* @param aEnd
*
*/
void CMMFTestUseOldCodecAudioController::MapcSetPlaybackWindowL(const TTimeIntervalMicroSeconds& aStart, const TTimeIntervalMicroSeconds& aEnd)
	{
	iDataPath->SetPlayWindowL(aStart, aEnd);
	}

/**
* MapcDeletePlaybackWindowL
*/
void CMMFTestUseOldCodecAudioController::MapcDeletePlaybackWindowL()
	{
	iDataPath->ClearPlayWindowL();
	}

/**
* MapcGetLoadingProgressL
*/
void CMMFTestUseOldCodecAudioController::MapcGetLoadingProgressL(TInt& /*aPercentageComplete*/)
	{
	User::Leave(KErrNotSupported);
	}


/**
* MarcGetRecordTimeAvailableL
* @param aTime
*
*/
void CMMFTestUseOldCodecAudioController::MarcGetRecordTimeAvailableL(TTimeIntervalMicroSeconds& aTime)
	{
	//[ assert the invariant ]
	__ASSERT_ALWAYS( Invariant(), Panic(EBadInvariant));
	
	//[ assert the precondition ( in a friendly way for this api 
	// that we minimally have a data sink ]
	if( !iDataSink )
		User::Leave( KErrNotReady );
	
	// Use the FormatEncode to get the bytes per second and the sink (clip) to get the bytes available
	// return the calculated value.
	if	((iDataSink->DataSinkType() != KUidMmfFileSink) && (iDataSink->DataSinkType() != KUidMmfDescriptorSink))
		User::Leave(KErrNotSupported) ;
	
	// [ max file size ]
	//[ pre condition is that we have a sink ]
   	
	// In order to get the record time available we need to take into consideration
	// that there may be a max file size ]
	TInt64 bytesFree       = STATIC_CAST(CMMFClip*, iDataSink)->BytesFree() ;
	TInt64 bytesPerSecond  = TInt64(0);
	//[ set default time available ]
	       aTime           = TTimeIntervalMicroSeconds( 0 ) ; // just return zero

	if( iSinkFormat )
		{
		TInt maxFileSize = STATIC_CAST(CMMFFormatEncode*, iSinkFormat)->MaximumClipSize();
		//[ if maxFileSize > 0 we need to limit the bytes free to this value - size ]
		if( maxFileSize > 0 )
			{
			// [ strangely the size of data written is a TInt ]
			TInt fileSize = STATIC_CAST(CMMFClip*, iDataSink)->Size();
			bytesFree = maxFileSize - fileSize;
			// [ note it can occur that the fileSize id greater than the MaxFileSize
			//  due to someone setting the max file size on an existing file ]
			if( bytesFree < 0 ) bytesFree = 0;
			__ASSERT_DEBUG( ( bytesFree <= maxFileSize), Panic(	EBadInvariant) );
			}		
		bytesPerSecond = STATIC_CAST(CMMFFormatEncode*, iSinkFormat)->BytesPerSecond() ;
		}

	//[ now lets perform the calculation of time available ]
	if ( bytesPerSecond != TInt64(0) )
		{
		aTime = TTimeIntervalMicroSeconds( bytesFree * KOneSecondInMicroSeconds / bytesPerSecond ) ;
		}
	
	//[ assert the invariant ]
	__ASSERT_ALWAYS( Invariant(), Panic(EBadInvariant));
	}

/**
* MarcSetMaxDurationL
* @param aMaxDuration
*/
void CMMFTestUseOldCodecAudioController::MarcSetMaxDurationL(const TTimeIntervalMicroSeconds& )
	{
	//[ this method is deprecated and no longer supported ]
	User::Leave(KErrNotSupported);
	}

/**
* MarcSetMaxFileSizeL
* @param aFileSize
* @precondition 
* The argument aFileSize must be greater than -1
* zero is used as a sentinel value which means that the file
* can grow without limit
*/
void CMMFTestUseOldCodecAudioController::MarcSetMaxFileSizeL(TInt aFileSize )
	{
	//[ assert the invariant ]
	__ASSERT_ALWAYS( Invariant(), Panic(EBadInvariant));

	//[ assert the state is not playing since this opens open 
	// nefarious posibilities
	if(State() == EPlaying )
		User::Leave( KErrNotReady );
	
	//[ assert we have a sink format ]
	if( !iSinkFormat )
		User::Leave( KErrNotReady );

	//[ assert file size > -1, as a basic sanity filter
	// 0 is the sentinel value which allows a file to grow
	// as needed ]
	if( aFileSize < 0 )
		User::Leave( KErrArgument );

    //[ pre condition is that we have a sink ]
    STATIC_CAST(CMMFFormatEncode*, iSinkFormat)->SetMaximumClipSizeL( aFileSize );

	// [ assert the post condition ]
	// [since we have no means of querying the value
	// we have to assume all went well for now or we left]

	//[ assert the invariant ]
	__ASSERT_ALWAYS( Invariant(), Panic(EBadInvariant));
	}

/**
* MarcCropL
* @param aToEnd
*/
void CMMFTestUseOldCodecAudioController::MarcCropL(TBool aToEnd)
	{
	//[ assert the invariant ]
	__ASSERT_ALWAYS( Invariant(), Panic(EBadInvariant));

	//[ precondition there is a sink format]
	if (!iSinkFormat)
		User::Leave(KErrNotSupported);

	iSinkFormat->CropL( PositionL(), aToEnd );

	//[ assert the invariant ]
	__ASSERT_ALWAYS( Invariant(), Panic(EBadInvariant));
	}

/**
* MarcAddMetaDataEntryL
* @param aNewEntry
*/
void CMMFTestUseOldCodecAudioController::MarcAddMetaDataEntryL(const CMMFMetaDataEntry& aNewEntry )
	{
	//[ assert the invariant ]
	__ASSERT_ALWAYS( Invariant(), Panic(EBadInvariant));

	//[ precondition the format exists ]
	if( !iSinkFormat )
		User::Leave(KErrNotSupported);

	//[ Add the meta data entry ]
	iSinkFormat->AddMetaDataEntryL( aNewEntry );

	//[ assert the invariant ]
	__ASSERT_ALWAYS( Invariant(), Panic(EBadInvariant));

	}

/**
* MarcRemoveMetaDataEntryL
* @param aIndex
*/
void CMMFTestUseOldCodecAudioController::MarcRemoveMetaDataEntryL(TInt aIndex)
	{
	//[ assert the invariant ]
	__ASSERT_ALWAYS( Invariant(), Panic(EBadInvariant));

	//[ precondition that we are in the primed state ]
	if( State() != EPrimed)
		User::Leave(KErrNotReady);

    //[ precondition the format exists ]
	if( !iSinkFormat )
		User::Leave(KErrNotSupported);

	//[ remove the meta data entry ]
	iSinkFormat->RemoveMetaDataEntry( aIndex );

	//[ assert the invariant ]
	__ASSERT_ALWAYS( Invariant(), Panic(EBadInvariant));

	}

/**
* MarcReplaceMetaDataEntryL
* @param aIndex
* @param aNewEntry
*/
void CMMFTestUseOldCodecAudioController::MarcReplaceMetaDataEntryL(TInt aIndex, const CMMFMetaDataEntry& aNewEntry)
	{
	//[ assert the invariant ]
	__ASSERT_ALWAYS( Invariant(), Panic(EBadInvariant));

	//[ precondition that we are in the primed state ]
	if( State() != EPrimed)
		User::Leave(KErrNotReady);

   	//[ precondition the format exists ]
	if( !iSinkFormat )
		User::Leave(KErrNotSupported);

	//[ replace meta data entry ]
	iSinkFormat->ReplaceMetaDataEntryL( aIndex, aNewEntry );

	//[ assert the invariant ]
	__ASSERT_ALWAYS( Invariant(), Panic(EBadInvariant));

	}

/**
* MacSetSourceSampleRateL
* @param aSampleRate
*/
void CMMFTestUseOldCodecAudioController::MacSetSourceSampleRateL(TUint aSampleRate)
	{
	// [ assert the invariant ]
	__ASSERT_ALWAYS( Invariant(), Panic( EBadInvariant));
	
	// [ assert the precondition we are stopped ]
	if( State() != EStopped )
		User::Leave(KErrNotReady);


	if (iSourceFormat)
		{//only applicable to formats
		// don't throw an error if the clip already exists with a different sample rate
		TInt error = iSourceFormat->SetSampleRate(aSampleRate);
		if (error != KErrNone && error != KErrAlreadyExists)
			User::Leave(error);
		}
	else if (iDataSource && (iDataSource->DataSourceType()==KUidMmfAudioInput))
		{
		// cast iDataSource to audio input and set sample rate
		MMMFAudioInput* ai = STATIC_CAST(MMMFAudioInput*, iDataSource);
		
		//note that it is safe to call SoundDevice()
		//as the controller logs onto the iDataSource when it is added
		TMMFCapabilities devSoundConfig = ai->SoundDevice().Config();

		ConvertFromSampleRateToDevSoundCapsL(aSampleRate, devSoundConfig);
		ai->SoundDevice().SetConfigL(devSoundConfig);
		//ReNegotiateL();
		}
	else 
		User::Leave(KErrNotSupported);

	// [assert the post condition ]
	__ASSERT_ALWAYS( Invariant(), Panic( EBadInvariant));

	}

/**
* MacSetSourceNumChannelsL
* @param aNumChannels
*/
void CMMFTestUseOldCodecAudioController::MacSetSourceNumChannelsL(TUint aNumChannels)
	{
	// [ assert the invariant ]
	__ASSERT_ALWAYS( Invariant(), Panic( EBadInvariant));
  
	// [assert the precondition that we are stopped ]
	if( State() != EStopped )
		User::Leave(KErrNotReady);

	if (iSourceFormat)
		{//only applicable to formats
		// don't throw an error if the clip already exists with a different number of channels
		TInt error = iSourceFormat->SetNumChannels(aNumChannels);
		if (error != KErrNone && error != KErrAlreadyExists)
			User::Leave(error);
		}
	else if (iDataSource && (iDataSource->DataSourceType()==KUidMmfAudioInput))
		{
		// cast iDataSource to audio input and set num channels
		MMMFAudioInput* ai = STATIC_CAST(MMMFAudioInput*, iDataSource);

		//note that it is safe to call SoundDevice()
		//as the controller logs onto the iDataSource when it is added
		TMMFCapabilities devSoundConfig = ai->SoundDevice().Config();

		ConvertFromNumChannelsToDevSoundCapsL(aNumChannels, devSoundConfig);
		ai->SoundDevice().SetConfigL(devSoundConfig);
		//ReNegotiateL();
		}
	else 
		User::Leave(KErrNotSupported);
	
	// [ assert the invariant ]
	__ASSERT_ALWAYS( Invariant(), Panic( EBadInvariant)); 

	}

/**
* MacSetSourceFormatL
* @param aFormatUid
*
*/
void CMMFTestUseOldCodecAudioController::MacSetSourceFormatL(TUid aFormatUid)
	{
     //[ assert the invaraint ]
	__ASSERT_ALWAYS( Invariant(), Panic( EBadInvariant)); 
	
	// [ precondition that the controller is stopped ]
    if( State() != EStopped )
		User::Leave( KErrNotReady );

	//[ precondition that the data source exists]
	if (!iDataSource)
		User::Leave(KErrNotReady);
	
	//[ precondition that we need a format ]
	if( !SourceFormatRequired( *iDataSource ) )
		User::Leave(KErrNotSupported); //cant set source format if source isn't a clip

	//[ if the format exists and the uid of the requested
	//	format is the same as the existing format then simply 
	// return otherwise create a new format ]
	if( !((iSourceFormat) && ( iSourceFormat->ImplementationUid() == aFormatUid)))
		{
		// [ delete the old format regardless ]
		delete iSourceFormat;
		iSourceFormat = NULL;
		iSourceFormat = CMMFFormatDecode::NewL(aFormatUid, iDataSource);
		}

	//[ assert the invariant ]
	__ASSERT_ALWAYS( Invariant(), Panic( EBadInvariant)); 

	//[ assert the post condition that a source format has been constructed ]
	__ASSERT_ALWAYS( (iSourceFormat != NULL), Panic( EPostConditionViolation ));
	}

/**
* MacSetSinkSampleRateL
* @param aSampleRate
*/
void CMMFTestUseOldCodecAudioController::MacSetSinkSampleRateL(TUint aSampleRate)
	{
	//[ assert the invariant ]
	__ASSERT_ALWAYS( Invariant(), Panic( EBadInvariant));

	// [ assert the precondition that we are stopped ]
	if (State() != EStopped )
		User::Leave(KErrNotReady);

	if (iSinkFormat)
		{//only applicable to formats
		// don't throw an error if the clip already exists with a different sample rate
		TInt error = iSinkFormat->SetSampleRate(aSampleRate);
		if (error != KErrNone && error != KErrAlreadyExists)
			User::Leave(error);
		}
	else if (iDataSink && (iDataSink->DataSinkType()==KUidMmfAudioOutput))
		{
		// cast iDataSink to audio output and set sample rate
		MMMFAudioOutput* ao = STATIC_CAST(MMMFAudioOutput*, iDataSink);



		TMMFCapabilities devSoundConfig = ao->SoundDevice().Config();
		ConvertFromSampleRateToDevSoundCapsL(aSampleRate, devSoundConfig);
		ao->SoundDevice().SetConfigL(devSoundConfig);
		}
	else 
		User::Leave(KErrNotSupported);

	//[ assert the invariant ]
	__ASSERT_ALWAYS( Invariant(), Panic( EBadInvariant));
	}

/**
* MacSetSinkNumChannelsL
* @param aNumChannels
*
*/
void CMMFTestUseOldCodecAudioController::MacSetSinkNumChannelsL(TUint aNumChannels)
	{
	//[ assert the invariant ]
	__ASSERT_ALWAYS( Invariant(), Panic( EBadInvariant));

	// [ assert the precondition that we are stopped ]
	if (State() != EStopped )
		User::Leave(KErrNotReady);

	if (iSinkFormat)
		{//only applicable to formats
		// don't throw an error if the clip already exists with a different number of channels
		TInt error = iSinkFormat->SetNumChannels(aNumChannels);
		if (error != KErrNone && error != KErrAlreadyExists)
			User::Leave(error);
		}
	else if (iDataSink && (iDataSink->DataSinkType()==KUidMmfAudioOutput))
		{
		// cast iDataSink to audio output and set num channels
		MMMFAudioOutput* ao = STATIC_CAST(MMMFAudioOutput*, iDataSink);

		//note that it is safe to call SoundDevice()
		//as the controller logs onto the iDataSource when it is added
		TMMFCapabilities devSoundConfig = ao->SoundDevice().Config();
		ConvertFromNumChannelsToDevSoundCapsL(aNumChannels, devSoundConfig);
		ao->SoundDevice().SetConfigL(devSoundConfig);
		}
	else 
		User::Leave(KErrNotSupported);

	// [assert the invariant ]
	__ASSERT_ALWAYS( Invariant(), Panic( EBadInvariant));

	}

/**
* MacSetSinkFormatL 
* @param aFormatUid
*
*/
void CMMFTestUseOldCodecAudioController::MacSetSinkFormatL(TUid aFormatUid)
	{
    //[ assert the invariant ]
	__ASSERT_ALWAYS( Invariant(), Panic( EBadInvariant));

	// [ precondition that the controller is stopped ]
    if( State() != EStopped )
		User::Leave( KErrNotReady );

	//[ precondition that the data sink exists]
	if (!iDataSink)
		User::Leave(KErrNotReady);

	//[ precondition that we need a format ]
	if (!SinkFormatRequired( *iDataSink))
		User::Leave(KErrNotSupported);

	//[ if the format exists and the uid of the requested
	//	format is the same as the existing format then simply 
	// return ]
	if( !((iSinkFormat) && ( iSinkFormat->ImplementationUid() == aFormatUid)))
		{
		// [ delete the old format regardless ]
		delete iSinkFormat;
		iSinkFormat = NULL;
		iSinkFormat = CMMFFormatEncode::NewL(aFormatUid, iDataSink);
		}

	//[ assert the invariant ]
	__ASSERT_ALWAYS( Invariant(), Panic( EBadInvariant));

	//[ assert the post condition that a sink format has been constructed ]
	__ASSERT_ALWAYS( (iSinkFormat != NULL), Panic( EPostConditionViolation ));
	}


/**
* MacSetCodecL
* @param aSourceDataType
* @param aSinkDataType
*
*/
void CMMFTestUseOldCodecAudioController::MacSetCodecL(TFourCC aSourceDataType, TFourCC aSinkDataType)
	{
	//[ assert the invariant ]
	__ASSERT_ALWAYS( Invariant(), Panic(EBadInvariant));

	//[ assert the precondition ]
	if(State() != EStopped)
		User::Leave(KErrNotReady);

	//don't set codec directly  -just set source & sink fourCC codes
	//[  ]
	TInt error(KErrNone);
	if ((iSinkFormat)&&(aSinkDataType != KMMFFourCCCodeNULL))
		{
		error = iSinkFormat->SetSinkDataTypeCode(aSinkDataType,iMediaId);
		}
	else if((iDataSink) && (aSinkDataType != KMMFFourCCCodeNULL))
		{
		error = iDataSink->SetSinkDataTypeCode(aSinkDataType,iMediaId);
		}
	if ((iSourceFormat)&&(!error)&&(aSourceDataType != KMMFFourCCCodeNULL))
		{
		error = iSourceFormat->SetSourceDataTypeCode(aSourceDataType,iMediaId);
		}
	else if ((iDataSource) && (aSourceDataType != KMMFFourCCCodeNULL))
		{
		error = iDataSource->SetSourceDataTypeCode(aSourceDataType,iMediaId);
		}

	//[ leave if we are not ready or there was an error ]
	User::LeaveIfError(error);

	//[ assert the invariant ]
	__ASSERT_ALWAYS( Invariant(), Panic(EBadInvariant));
	}

/**
* MacSetSourceBitRateL
* @param "TUint"
* Sets the source bit rate
*
*/
void CMMFTestUseOldCodecAudioController::MacSetSourceBitRateL(TUint aBitRate)
	{
	//[ assert the invariant ]
	__ASSERT_ALWAYS( Invariant(), Panic(EBadInvariant));

	//[ assert the precondition ]
	if(State() != EStopped)
		User::Leave(KErrNotReady);

	//[ pre condition  that we have a source format]
	if (!iSourceFormat)
		User::Leave(KErrNotSupported);

	//only applicable to formats
	User::LeaveIfError(iSourceFormat->SetBitRate(aBitRate));

	//[ assert the set bit rate is the bit rate ]
	__ASSERT_ALWAYS( (aBitRate == iSourceFormat->BitRate()), Panic( EPostConditionViolation ));
		
	//[ assert the invariant ]
	__ASSERT_ALWAYS( Invariant(), Panic( EBadInvariant));
	}


/**
*
* MacSetSourceDataTypeL
* @param "TFourCC"
*
*/
void CMMFTestUseOldCodecAudioController::MacSetSourceDataTypeL(TFourCC aDataType)
	{
	MacSetCodecL(aDataType, KMMFFourCCCodeNULL);
	}

/**
*
* MacSetSinkBitRateL
* @param "TUint"
*
*/
void CMMFTestUseOldCodecAudioController::MacSetSinkBitRateL(TUint aRate)
	{
	//[ assert the invariant ]
	__ASSERT_ALWAYS( Invariant(), Panic( EBadInvariant));

    // [ assert we are stopped ]
	if( State() != EStopped)
		User::Leave( KErrNotReady );

	//[ pre condition we have a sink format ]
	if (!iSinkFormat)
		User::Leave(KErrNotSupported);

	//only applicable to formats
	User::LeaveIfError(iSinkFormat->SetBitRate(aRate));

	//[ assert the set bit rate is the bit rate ]
	__ASSERT_ALWAYS( (aRate == iSinkFormat->BitRate()), Panic( EBadInvariant));

	//[ assert the invariant ]
	__ASSERT_ALWAYS( Invariant(), Panic( EBadInvariant));
	}

/**
*
* MacSetSinkDataTypeL
* @param "TFourCC"
*
*/
void CMMFTestUseOldCodecAudioController::MacSetSinkDataTypeL(TFourCC aDataType)
	{
	MacSetCodecL(KMMFFourCCCodeNULL, aDataType);

	if (iDataSink && (iDataSink->DataSinkType()==KUidMmfAudioOutput))
		{
		MMMFAudioOutput* ao = STATIC_CAST(MMMFAudioOutput*, iDataSink);
		TMMFCapabilities devSoundConfig = ao->SoundDevice().Config();
		RArray<TFourCC> dataTypes;
		CleanupClosePushL(dataTypes);		
		
		//[ set dev sound data type here ]
		ConvertFromDataTypeToDevSoundCapsL(aDataType, devSoundConfig);
		ao->SoundDevice().SetConfigL(devSoundConfig);
		CleanupStack::PopAndDestroy();//dataTypes
		}
	}

/**
*
* MacGetSourceSampleRateL
* @param "TUint"
* 
*/
void CMMFTestUseOldCodecAudioController::MacGetSourceSampleRateL(TUint& aRate)
	{
	//[ assert the invariant ]
	__ASSERT_ALWAYS( Invariant(), Panic( EBadInvariant));

	//[ precondition is that we have a source format ||
	// we have a data source and its an audio input ]
	if( !((iDataSource && (iDataSource->DataSourceType()==KUidMmfAudioInput)) ||
		( iSourceFormat )))
		User::Leave(KErrNotSupported);
	
	if (iSourceFormat)
		{
		aRate = iSourceFormat->SampleRate();
		}
	else
		{
		// cast iDataSource to audio input and query sample rate
		MMMFAudioInput* ai = STATIC_CAST(MMMFAudioInput*, iDataSource);
		TMMFCapabilities devSoundConfig = ai->SoundDevice().Config();
		RArray<TUint> rates;
		CleanupClosePushL(rates);
		ConvertFromDevSoundCapsToSampleRatesL(devSoundConfig, rates);
		ASSERT(rates.Count()==1);//make sure we've been given the correct config by devsound
		aRate = rates[0];
		CleanupStack::PopAndDestroy();//rates
		}

	//[ assert the invariant ]
	__ASSERT_ALWAYS( Invariant(), Panic( EBadInvariant));
	}

/**
*
* MacGetSourceBitRateL
* @param "TUint"
*
*/
void CMMFTestUseOldCodecAudioController::MacGetSourceBitRateL(TUint& aRate)
	{
	//[ assert the invariant ]
	__ASSERT_ALWAYS( Invariant(), Panic( EBadInvariant));

	// Can only query formats for bit rate - devsound doesn't do bit rates.
	if (!iSourceFormat)
		User::Leave(KErrNotSupported);

	aRate = iSourceFormat->BitRate();
	
	//[ assert the invariant ]
	__ASSERT_ALWAYS( Invariant(), Panic( EBadInvariant));
	
	}

/**
*
* MacGetSourceNumChannelsL
* @param "TUint&"
*
*/
void CMMFTestUseOldCodecAudioController::MacGetSourceNumChannelsL(TUint& aNumChannels)
	{
	//[ assert the invariant ]
	__ASSERT_ALWAYS( Invariant(), Panic( EBadInvariant));

	//[ assert the precondition ]
	if( !((iSourceFormat) ||
		(iDataSource && (iDataSource->DataSourceType()==KUidMmfAudioInput))))
		User::Leave(KErrNotSupported);

	if (iSourceFormat)
		{
		aNumChannels = iSourceFormat->NumChannels();
		}
	else
		{
		// cast iDataSource to audio input and query num channels
		MMMFAudioInput* ai = STATIC_CAST(MMMFAudioInput*, iDataSource);
		TMMFCapabilities devSoundConfig = ai->SoundDevice().Config();
		RArray<TUint> numChannels;
		CleanupClosePushL(numChannels);
		ConvertFromDevSoundCapsToNumChannelsL(devSoundConfig, numChannels);
		ASSERT(numChannels.Count()==1);
		aNumChannels = numChannels[0];
		CleanupStack::PopAndDestroy();//numChannels
		}
	
	//[ assert the invariant ]
	__ASSERT_ALWAYS( Invariant(), Panic( EBadInvariant));

	}

/**
*
* MacGetSourceFormatL
* @param "TUid"
*/
void CMMFTestUseOldCodecAudioController::MacGetSourceFormatL(TUid& aFormat)
	{
	//[ assert the invariant ]
	__ASSERT_ALWAYS( Invariant(), Panic( EBadInvariant));

	//[ precondition we have a format ]
	if (!iSourceFormat)
		User::Leave(KErrNotSupported);

	// [ get the source format uid ]
	aFormat = iSourceFormat->ImplementationUid();

	//[ assert the invariant ]
	__ASSERT_ALWAYS( Invariant(), Panic( EBadInvariant));
	
	}

/**
*
* MacGetSourceDataTypeL
* @param "TFourCC&"
*
*/
void CMMFTestUseOldCodecAudioController::MacGetSourceDataTypeL(TFourCC& aDataType)
	{
	if (iSourceFormat)
		aDataType = iSourceFormat->SourceDataTypeCode(TMediaId(KUidMediaTypeAudio));
	else if (iDataSource && (iDataSource->DataSourceType()==KUidMmfAudioInput))
		{
		// cast iDataSource to audio input and query num channels
		MMMFAudioInput* ai = STATIC_CAST(MMMFAudioInput*, iDataSource);
		TMMFCapabilities devSoundConfig = ai->SoundDevice().Config();
		RArray<TFourCC> dataTypes;
		CleanupClosePushL(dataTypes);
		ConvertFromDevSoundCapsToDataTypesL(devSoundConfig, dataTypes);
		ASSERT(dataTypes.Count()==1);
		aDataType = dataTypes[0];
		CleanupStack::PopAndDestroy();//dataTypes
		}
	else
		User::Leave(KErrNotSupported);
	}

/**
*
* MacGetSinkSampleRateL
* @param "TUint&"
*
*/

void CMMFTestUseOldCodecAudioController::MacGetSinkSampleRateL(TUint& aRate)
	{
	if (iSinkFormat)
		aRate = iSinkFormat->SampleRate();
	else if (iDataSink && (iDataSink->DataSinkType()==KUidMmfAudioOutput))
		{
		// cast iDataSink to audio output and query sample rate
		MMMFAudioOutput* ao = STATIC_CAST(MMMFAudioOutput*, iDataSink);
		TMMFCapabilities devSoundConfig = ao->SoundDevice().Config();
		RArray<TUint> rates;
		CleanupClosePushL(rates);
		ConvertFromDevSoundCapsToSampleRatesL(devSoundConfig, rates);
		ASSERT(rates.Count()==1);//make sure we've been given the correct config by devsound
		aRate = rates[0];
		CleanupStack::PopAndDestroy();//rates
		}
	else
		User::Leave(KErrNotSupported);
	}

/**
*
* MacGetSinkBitRateL
* @param "TUint&"
*
*/
void CMMFTestUseOldCodecAudioController::MacGetSinkBitRateL(TUint& aRate)
	{
	if (iSinkFormat)
		aRate = iSinkFormat->BitRate();
	else
		User::Leave(KErrNotSupported);
	}

/**
*
* MacGetSinkNumChannelsL
* @param "TUint&"
*
*/
void CMMFTestUseOldCodecAudioController::MacGetSinkNumChannelsL(TUint& aNumChannels)
	{
	if (iSinkFormat)
		aNumChannels = iSinkFormat->NumChannels();
	else if (iDataSink && (iDataSink->DataSinkType()==KUidMmfAudioOutput))
		{
		// cast iDataSink to audio output and query num channels
		MMMFAudioOutput* ao = STATIC_CAST(MMMFAudioOutput*, iDataSink);
		TMMFCapabilities devSoundConfig = ao->SoundDevice().Config();
		RArray<TUint> numChannels;
		CleanupClosePushL(numChannels);
		ConvertFromDevSoundCapsToNumChannelsL(devSoundConfig, numChannels);
		ASSERT(numChannels.Count()==1);
		aNumChannels = numChannels[0];
		CleanupStack::PopAndDestroy();//numChannels
		}
	else
		User::Leave(KErrNotSupported);
	}

/**
*
* MacGetSinkFormatL
* @param "TUid&"
*
*/
void CMMFTestUseOldCodecAudioController::MacGetSinkFormatL(TUid& aFormat)
	{
	if (iSinkFormat)
		aFormat = iSinkFormat->ImplementationUid();
	else 
		User::Leave(KErrNotSupported);
	}

/**
*
* MacGetSinkDataTypeL
* @param "TFourCC&"
*
*/
void CMMFTestUseOldCodecAudioController::MacGetSinkDataTypeL(TFourCC& aDataType)
	{
	if (iSinkFormat)
		aDataType = iSinkFormat->SinkDataTypeCode(TMediaId(KUidMediaTypeAudio));
	else if (iDataSink && (iDataSink->DataSinkType()==KUidMmfAudioOutput))
		{
		// cast iDataSink to audio output and query data type
		MMMFAudioOutput* ao = STATIC_CAST(MMMFAudioOutput*, iDataSink);
		TMMFCapabilities devSoundConfig = ao->SoundDevice().Config();
		RArray<TFourCC> dataTypes;
		CleanupClosePushL(dataTypes);
		ConvertFromDevSoundCapsToDataTypesL(devSoundConfig, dataTypes);
		ASSERT(dataTypes.Count()==1);
		aDataType = dataTypes[0];
		CleanupStack::PopAndDestroy();//dataTypes
		}
	else
		User::Leave(KErrNotSupported);
	}

/**
* 
* MacGetSupportedSourceSampleRatesL
* @param "RArray<TUint>&"
* 
*/
void CMMFTestUseOldCodecAudioController::MacGetSupportedSourceSampleRatesL(RArray<TUint>& aSupportedRates)
	{
	aSupportedRates.Reset();
	if (iSourceFormat)
		iSourceFormat->GetSupportedSampleRatesL(aSupportedRates);
	else if (iDataSource && (iDataSource->DataSourceType()==KUidMmfAudioInput))
		{
		// cast iDataSource to audio input and query supported sample rates
		MMMFAudioInput* ai = STATIC_CAST(MMMFAudioInput*, iDataSource);
		TMMFCapabilities devSoundConfig = ai->SoundDevice().Capabilities();
		ConvertFromDevSoundCapsToSampleRatesL(devSoundConfig, aSupportedRates);
		}
	else
		User::Leave(KErrNotSupported);
	}

/**
*
* MacGetSupportedSourceBitRatesL
* @param "RArray<TUint>&"
*
*/
void CMMFTestUseOldCodecAudioController::MacGetSupportedSourceBitRatesL(RArray<TUint>& aSupportedRates)
	{
	aSupportedRates.Reset();
	if (iSourceFormat)
		iSourceFormat->GetSupportedBitRatesL(aSupportedRates);
	else
		User::Leave(KErrNotSupported);
	}

/***
*
* MacGetSupportedSourceNumChannelsL
* @param "RArray<TUint>&"
*
*/
void CMMFTestUseOldCodecAudioController::MacGetSupportedSourceNumChannelsL(RArray<TUint>& aSupportedChannels)
	{
	aSupportedChannels.Reset();
	if (iSourceFormat)
		iSourceFormat->GetSupportedNumChannelsL(aSupportedChannels);
	else if (iDataSource && (iDataSource->DataSourceType()==KUidMmfAudioInput))
		{
		// cast iDataSource to audio input and query supported num channels
		MMMFAudioInput* ai = STATIC_CAST(MMMFAudioInput*, iDataSource);
		TMMFCapabilities devSoundConfig = ai->SoundDevice().Capabilities();
		ConvertFromDevSoundCapsToNumChannelsL(devSoundConfig, aSupportedChannels);
		}
	else
		User::Leave(KErrNotSupported);
	}

/***
*
* MacGetSupportedSourceDataTypesL
* @param "RArray<TFourCC>&"
*
*/
void CMMFTestUseOldCodecAudioController::MacGetSupportedSourceDataTypesL(RArray<TFourCC>& aSupportedDataTypes)
	{
	aSupportedDataTypes.Reset();
	if (iSourceFormat)
		iSourceFormat->GetSupportedDataTypesL(TMediaId(KUidMediaTypeAudio), aSupportedDataTypes);
	else if (iDataSource && (iDataSource->DataSourceType()==KUidMmfAudioInput))
		{
		// cast iDataSource to audio input and query supported data types
		MMMFAudioInput* ai = STATIC_CAST(MMMFAudioInput*, iDataSource);
		TMMFCapabilities devSoundConfig = ai->SoundDevice().Capabilities();
		ConvertFromDevSoundCapsToDataTypesL(devSoundConfig, aSupportedDataTypes);
		}
	else
		User::Leave(KErrNotSupported);
	}

/***
*
* MacGetSupportedSinkSampleRatesL
* @param "RArray<TUint>& "
*
*/
void CMMFTestUseOldCodecAudioController::MacGetSupportedSinkSampleRatesL(RArray<TUint>& aSupportedRates)
	{
	aSupportedRates.Reset();
	if (iSinkFormat)
		iSinkFormat->GetSupportedSampleRatesL(aSupportedRates);
	else if (iDataSink && (iDataSink->DataSinkType()==KUidMmfAudioOutput))
		{
		// cast iDataSink to audio output and query supported sample rates
		MMMFAudioOutput* ao = STATIC_CAST(MMMFAudioOutput*, iDataSink);
		TMMFCapabilities devSoundConfig = ao->SoundDevice().Capabilities();
		ConvertFromDevSoundCapsToSampleRatesL(devSoundConfig, aSupportedRates);
		}
	else
		User::Leave(KErrNotSupported);
	}

/***
*
* MacGetSupportedSinkBitRatesL
* @param RArray<TUint>& 
*
*/
void CMMFTestUseOldCodecAudioController::MacGetSupportedSinkBitRatesL(RArray<TUint>& aSupportedRates)
	{
	if (iSinkFormat)
		iSinkFormat->GetSupportedBitRatesL(aSupportedRates);
	else
		User::Leave(KErrNotSupported);
	}

/***
*
* MacGetSupportedSinkNumChannelsL
* @param RArray<TUint>&
*
*/
void CMMFTestUseOldCodecAudioController::MacGetSupportedSinkNumChannelsL(RArray<TUint>& aSupportedChannels)
	{
	aSupportedChannels.Reset();
	if (iSinkFormat)
		iSinkFormat->GetSupportedNumChannelsL(aSupportedChannels);
	else if (iDataSink && (iDataSink->DataSinkType()==KUidMmfAudioOutput))
		{
		// cast iDataSink to audio output and query supported channels
		MMMFAudioOutput* ao = STATIC_CAST(MMMFAudioOutput*, iDataSink);
		TMMFCapabilities devSoundConfig = ao->SoundDevice().Capabilities();
		ConvertFromDevSoundCapsToNumChannelsL(devSoundConfig, aSupportedChannels);
		}
	else
		User::Leave(KErrNotSupported);
	}

/***
*
* MacGetSupportedSinkDataTypesL
* @param "RArray<TFourCC>&"
*/
void CMMFTestUseOldCodecAudioController::MacGetSupportedSinkDataTypesL(RArray<TFourCC>& aSupportedDataTypes)
	{
	aSupportedDataTypes.Reset();
	if (iSinkFormat)
		iSinkFormat->GetSupportedDataTypesL(TMediaId(KUidMediaTypeAudio), aSupportedDataTypes);
	else if (iDataSink && (iDataSink->DataSinkType()==KUidMmfAudioOutput))
		{
		// cast iDataSink to audio output and query supported data types
		MMMFAudioOutput* ao = STATIC_CAST(MMMFAudioOutput*, iDataSink);
		TMMFCapabilities devSoundConfig = ao->SoundDevice().Capabilities();
		ConvertFromDevSoundCapsToDataTypesL(devSoundConfig, aSupportedDataTypes);
		}
	else
		User::Leave(KErrNotSupported);
	}

/**
*
* ConvertFromDevSoundCapsToSampleRatesL
* @param "const TMMFCapabilities& "
* @param "RArray<TUint>&"
*
*/
void CMMFTestUseOldCodecAudioController::ConvertFromDevSoundCapsToSampleRatesL(const TMMFCapabilities& aDevSoundCaps, RArray<TUint>& aSampleRates)
	{
	if (aDevSoundCaps.iRate & EMMFSampleRate8000Hz)
		User::LeaveIfError(aSampleRates.Append(KSampleRate8000Hz));
	if (aDevSoundCaps.iRate & EMMFSampleRate11025Hz)
		User::LeaveIfError(aSampleRates.Append(KSampleRate11025Hz));
	if (aDevSoundCaps.iRate & EMMFSampleRate16000Hz)
		User::LeaveIfError(aSampleRates.Append(KSampleRate16000Hz));
	if (aDevSoundCaps.iRate & EMMFSampleRate22050Hz)
		User::LeaveIfError(aSampleRates.Append(KSampleRate22050Hz));
	if (aDevSoundCaps.iRate & EMMFSampleRate32000Hz)
		User::LeaveIfError(aSampleRates.Append(KSampleRate32000Hz));
	if (aDevSoundCaps.iRate & EMMFSampleRate44100Hz)
		User::LeaveIfError(aSampleRates.Append(KSampleRate44100Hz));
	if (aDevSoundCaps.iRate & EMMFSampleRate48000Hz)
		User::LeaveIfError(aSampleRates.Append(KSampleRate48000Hz));
	if (aDevSoundCaps.iRate & EMMFSampleRate88200Hz)
		User::LeaveIfError(aSampleRates.Append(KSampleRate88200Hz));
	if (aDevSoundCaps.iRate & EMMFSampleRate96000Hz)
		User::LeaveIfError(aSampleRates.Append(KSampleRate96000Hz));
	}

/**
*
* ConvertFromDevSoundCapsToNumChannelsL
* @param "const TMMFCapabilities&"
* @param "RArray<TUint>&"
*
*/
void CMMFTestUseOldCodecAudioController::ConvertFromDevSoundCapsToNumChannelsL(const TMMFCapabilities& aDevSoundCaps, RArray<TUint>& aNumChannels)
	{
	if (aDevSoundCaps.iChannels & EMMFMono)
		User::LeaveIfError(aNumChannels.Append(KNumChannelsMono));
	if (aDevSoundCaps.iChannels & EMMFStereo)
		User::LeaveIfError(aNumChannels.Append(KNumChannelsStereo));
	}

/**
*
* ConvertFromDevSoundCapsToDataTypesL
* @param "const TMMFCapabilities&"
* @param "TMMFCapabilities& aDevSoundCaps, RArray<TFourCC>&"
*
*/
void CMMFTestUseOldCodecAudioController::ConvertFromDevSoundCapsToDataTypesL(const TMMFCapabilities& aDevSoundCaps, RArray<TFourCC>& aDataTypes)
	{
	if (aDevSoundCaps.iEncoding & EMMFSoundEncoding8BitPCM)
		User::LeaveIfError(aDataTypes.Append(KMMFFourCCCodePCM8));
	if (aDevSoundCaps.iEncoding & EMMFSoundEncoding16BitPCM)
		User::LeaveIfError(aDataTypes.Append(KMMFFourCCCodePCM16));
	if (aDevSoundCaps.iEncoding & EMMFSoundEncoding8BitALaw)
		User::LeaveIfError(aDataTypes.Append(KMMFFourCCCodeALAW));
	if (aDevSoundCaps.iEncoding & EMMFSoundEncoding8BitMuLaw)
		User::LeaveIfError(aDataTypes.Append(KMMFFourCCCodeMuLAW));
	}

/**
*
* ConvertFromSampleRateToDevSoundCapsL
* @param "TUint"
* @param "TMMFCapabilities&"
*
*/
void CMMFTestUseOldCodecAudioController::ConvertFromSampleRateToDevSoundCapsL(TUint aSampleRate, TMMFCapabilities& aDevSoundCaps)
	{
	if (aSampleRate == KSampleRate8000Hz)
		aDevSoundCaps.iRate = EMMFSampleRate8000Hz;
	else if (aSampleRate == KSampleRate11025Hz)
		aDevSoundCaps.iRate = EMMFSampleRate11025Hz;
	else if (aSampleRate == KSampleRate16000Hz)
		aDevSoundCaps.iRate = EMMFSampleRate16000Hz;
	else if (aSampleRate == KSampleRate22050Hz)
		aDevSoundCaps.iRate = EMMFSampleRate22050Hz;
	else if (aSampleRate == KSampleRate32000Hz)
		aDevSoundCaps.iRate = EMMFSampleRate32000Hz;
	else if (aSampleRate == KSampleRate44100Hz)
		aDevSoundCaps.iRate = EMMFSampleRate44100Hz;
	else if (aSampleRate == KSampleRate48000Hz)
		aDevSoundCaps.iRate = EMMFSampleRate48000Hz;
	else if (aSampleRate == KSampleRate88200Hz)
		aDevSoundCaps.iRate = EMMFSampleRate88200Hz;
	else if (aSampleRate == KSampleRate96000Hz)
		aDevSoundCaps.iRate = EMMFSampleRate96000Hz;
	else
		User::Leave(KErrNotSupported);
	}

/**
*
* ConvertFromNumChannelsToDevSoundCapsL
* @param "TUint"
* @param  "TMMFCapabilities&"
*
*/
void CMMFTestUseOldCodecAudioController::ConvertFromNumChannelsToDevSoundCapsL(TUint aNumChannels, TMMFCapabilities& aDevSoundCaps)
	{
	if (aNumChannels == KNumChannelsMono)
		aDevSoundCaps.iChannels = EMMFMono;
	else if (aNumChannels == KNumChannelsStereo)
		aDevSoundCaps.iChannels = EMMFStereo;
	else
		User::Leave(KErrNotSupported);
	}

/**
*
* ConvertFromDataTypeToDevSoundCapsL
* @param "TFourCC"
* @param "TMMFCapabilities&"
*
*/
void CMMFTestUseOldCodecAudioController::ConvertFromDataTypeToDevSoundCapsL(TFourCC aDataType, TMMFCapabilities& aDevSoundCaps)
	{
	if (aDataType == KMMFFourCCCodePCM8)
		aDevSoundCaps.iEncoding = EMMFSoundEncoding8BitPCM;
	else if (aDataType == KMMFFourCCCodePCM16)
		aDevSoundCaps.iEncoding = EMMFSoundEncoding16BitPCM;
	else if (aDataType == KMMFFourCCCodeALAW)
		aDevSoundCaps.iEncoding = EMMFSoundEncoding8BitALaw;
	else if (aDataType == KMMFFourCCCodeMuLAW)
		aDevSoundCaps.iEncoding = EMMFSoundEncoding8BitMuLaw;
	else
		User::Leave(KErrNotSupported);
	}

/**
* IsValidStateTransition
* The function validates a state transition from iState to aState
* and returns ETrue if the transition is allowed.
* @param TControllerState
* @returns "TBool"
* @xxxx
*/
TBool CMMFTestUseOldCodecAudioController::IsValidStateTransition( TControllerState aState ) const
	{
	 TBool result = ETrue ;
	//[ assert the precondition that aState is a valid State ]
	__ASSERT_ALWAYS( IsValidState(aState), Panic( EBadArgument ) );
	//[ assert the invariant that iState is a valid State ]
	__ASSERT_ALWAYS( Invariant(), Panic( EBadInvariant ));

	// [ check the valid state transitions ]
	  // the only invalid transition is
	  // stopped to playing
	if( ( iState == EStopped ) && ( aState == EPlaying ))
		{
         result = EFalse ;
		}
  
	//[ assert the invariant that iState is a valid State ]
	__ASSERT_ALWAYS( Invariant(), Panic( EBadInvariant ));

	return result ;
	}

/*
* Invariant
* @returns "TBool"
* This function returns whether the invariant is valid
* @xxxx
*/
TBool  CMMFTestUseOldCodecAudioController::Invariant() const
	{
	//[ The invariant is for now defined 
	// as simply being in the correct state and
	// having iDataPath defined ]
	return ( iDataPath )&& IsValidState( iState);
	}

/*
* SetState
*  This function sets the state of the controller.
* @returns "TBool"
* @xxxx
*/
TBool CMMFTestUseOldCodecAudioController::SetState(TControllerState aState)
	{
	TBool result = ETrue;
	//[ assert the precondition that the state is a valid state ]
   	__ASSERT_ALWAYS( IsValidState( aState),  Panic( EBadArgument ) );
	//[ assert the invariant the current state is valid ]
	__ASSERT_ALWAYS( Invariant(),  Panic( EBadInvariant ) );
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
* IsValidState 
* checks whether a state is a valid 
* @returns "TBool"
* @param TControllerState
* @xxxx
*/
TBool  CMMFTestUseOldCodecAudioController::IsValidState( TControllerState aState ) const 
	{
	TBool result = EFalse;
     if(( aState >= EStopped ) && ( aState <= EPlaying ))
		 {
          result = ETrue;
		 }
	 return result;
	}

/**
* State
* The function State returns the current state of the audio controller
* @returns "TControllerState"
* @xxxx
*/
CMMFTestUseOldCodecAudioController::TControllerState CMMFTestUseOldCodecAudioController::State() const
	{
	__ASSERT_ALWAYS( Invariant(), Panic( EBadInvariant ) );
	return iState;
	}

/**
*
* SinkFormatRequired
*
*/
TBool CMMFTestUseOldCodecAudioController::SinkFormatRequired( MDataSink& aDataSink ) const
	{
     return (aDataSink.DataSinkType()==KUidMmfFileSink || 
		     aDataSink.DataSinkType()==KUidMmfDescriptorSink);
	}

/**
*
* SourceFormatRequired
*
*/

TBool CMMFTestUseOldCodecAudioController::SourceFormatRequired(MDataSource& aDataSource) const
	{
	return (aDataSource.DataSourceType()==KUidMmfFileSource || 
		    aDataSource.DataSourceType()==KUidMmfDescriptorSource);
	}
/**
*
* DevSoundInitialised
* Used in certain configurations to resume completion of PrimeL() when it is pending DevSound initialisation.
*
*/
void CMMFTestUseOldCodecAudioController::DevSoundInitialised(TInt /*aError*/)
	{
	iWaitForAsyncService->AsyncStop();
	}

CMMFTestUseOldCodecAudioController::TDevSoundEventHandler::TDevSoundEventHandler()
: iInterceptedDevSoundObserver(NULL), iEventObserver(NULL)
	{
	}	

void CMMFTestUseOldCodecAudioController::TDevSoundEventHandler::SetInterceptedDevSoundObserver(MDevSoundObserver* aObserver)
	{
	iInterceptedDevSoundObserver = aObserver;
	}
	
void CMMFTestUseOldCodecAudioController::TDevSoundEventHandler::SetEventObserver(CMMFTestUseOldCodecAudioController* aObserver)
	{
	iEventObserver = aObserver;
	}
		
void CMMFTestUseOldCodecAudioController::TDevSoundEventHandler::InitializeComplete(TInt aError)
	{
	iInterceptedDevSoundObserver->InitializeComplete(aError);
	iEventObserver->DevSoundInitialised(aError);
	}

void CMMFTestUseOldCodecAudioController::TDevSoundEventHandler::ToneFinished(TInt aError)
	{
	iInterceptedDevSoundObserver->ToneFinished(aError);
	}

void CMMFTestUseOldCodecAudioController::TDevSoundEventHandler::BufferToBeFilled(CMMFBuffer* aBuffer)
	{
	iInterceptedDevSoundObserver->BufferToBeFilled(aBuffer);
	}

void CMMFTestUseOldCodecAudioController::TDevSoundEventHandler::PlayError(TInt aError)
	{
	iInterceptedDevSoundObserver->PlayError(aError);
	}

void CMMFTestUseOldCodecAudioController::TDevSoundEventHandler::BufferToBeEmptied(CMMFBuffer* aBuffer)
	{
	iInterceptedDevSoundObserver->BufferToBeEmptied(aBuffer);
	}

void CMMFTestUseOldCodecAudioController::TDevSoundEventHandler::RecordError(TInt aError)
	{
	iInterceptedDevSoundObserver->RecordError(aError);
	}

void CMMFTestUseOldCodecAudioController::TDevSoundEventHandler::ConvertError(TInt aError)
	{
	iInterceptedDevSoundObserver->ConvertError(aError);
	}

void CMMFTestUseOldCodecAudioController::TDevSoundEventHandler::DeviceMessage(TUid aMessageType, const TDesC8& aMsg)
	{
	iInterceptedDevSoundObserver->DeviceMessage(aMessageType, aMsg);
	}



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

#include "CustomMmfASRController.h"

#include <mmfcontrollerimplementationuids.hrh>
#include "customcontrollertestconstants.h"
#include "Database/testdatabaseconstants.h"

/*
	TMmfAsrControllerPanics is an enumeration with the following entries:
	
	EBadArgument indicates a bad argument
	EBadState indicates a state viaolation
	EBadInvariant indicates an invariant violation
	EBadReset indicates failed reset
	EPostConditionViolation indicates a post condition violation

*/
enum TMmfAsrControllerPanics
	{
	EBadArgument,
	EBadState,
	EBadInvariant,
	EPreConditionViolation,
	EPostConditionViolation,
	EDeprecatedFunction
	};

/**
* Panic
* This method generates a panic
* @param "TInt"
* @xxxx
*/
void Panic(TMmfAsrControllerPanics aPanicCode)
	{
	_LIT(KMMFAsrControllerPanicCategory, "MMFAsrTestController");
	User::Panic(KMMFAsrControllerPanicCategory, aPanicCode);
	}


/**
*
* NewL
*
*/
CMMFController* CCustomMmfASRController::NewL()
	{
    CCustomMmfASRController* self = new(ELeave) CCustomMmfASRController;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop( self );	
	return STATIC_CAST( CMMFController*, self );
	}


/**
*
* SetState
* @param aState
*
*/
void CCustomMmfASRController::SetState( TControllerState aState )
	{
	iState=aState;
	}

/**
*
* ConstructL
*
*/
void CCustomMmfASRController::ConstructL()
	{
	//[ Construct custom command parsers]
	CSpeechRecognitionCustomCommandParser* asrParser = CSpeechRecognitionCustomCommandParser::NewL(*this);
	CleanupStack::PushL(asrParser);
	AddCustomCommandParserL(*asrParser);
	CleanupStack::Pop( asrParser ); //asrParser

	SetState( ESTraining );  //state of the controller
	iRecognitionMode =	ESdMode; // recognition mode which is nonzero for now
	iRecordTime = 0; // the duration of the recording for both recognition and trainning

	iRecognizeComplete = EFalse;
	iLoadedGrammarID = 0;
	iLoadedLexiconID = 0;
	iLoadedModelBankID = 0;
	
  	RFs fsSession;
  	RFile file;
  	User::LeaveIfError(fsSession.Connect());
    CleanupClosePushL(fsSession);
    TInt err = file.Open(fsSession, KFileNameTestDatabase, EFileRead);
  	file.Close();
  	if(err == KErrNone)
  		{
	    iDatabase = CSDDatabase::NewL(KFileNameTestDatabase);
  		}
   	else if(err == KErrNotFound)
  		{
	    iDatabase = CSDDatabase::NewL(KFileNameTestDatabase, EFalse);		
  		}
	else if(err == KErrPathNotFound)
  		{
  		TParse fullFileName;
 		if(fullFileName.Set(KFileNameTestDatabase, NULL, NULL) == KErrNone)
			{
		 	err = fsSession. MkDirAll(fullFileName.DriveAndPath());
		 	if(err == KErrNone)
		 		{
			    iDatabase = CSDDatabase::NewL(KFileNameTestDatabase,EFalse);		
		 		}
		 	else
		 		{
			    User::LeaveIfError(err); 			
		 		}	
	  		}  				
 		}
  	else
  		{
	    User::LeaveIfError(err);
  		}
  	CleanupStack::PopAndDestroy(); //fsSession 
	}



/**
*
* ~CCustomMmfAudioController
*
*/
CCustomMmfASRController::~CCustomMmfASRController()
	{
	delete iDatabase;
	}

/**
*
* CCustomMmfAudioController
*
*/
CCustomMmfASRController::CCustomMmfASRController()
	{	
	}

/**
*
* CustomCommand
*
*/
void CCustomMmfASRController::CustomCommand(TMMFMessage& /*aMessage*/)
	{
     // The custom command is the final stopping off point
	 // in the command processing chain and in this case it does
	 // nothing!
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
 *
 */
void CCustomMmfASRController::AddDataSourceL(MDataSource& /*aSource*/)
	{
	 //[This function is deprecated and should not have been called]
     Panic(EDeprecatedFunction);
	}

/**
 *  AddDataSinkL
 *
 *	Adds a data sink to the controller
 *
 *	@param aSink
 *
 */
void CCustomMmfASRController::AddDataSinkL(MDataSink& )
	{
	 //[This function is deprecated and should not have been called]
     Panic(EDeprecatedFunction);
	}

/**
 *  PrimeL
 *
 *  If Prime fails the client should reset the controller
 *  because as noted below this code is not transactional.
 *
 */
void CCustomMmfASRController::PrimeL()
	{
	//[This function is deprecated and should not have been called]
    Panic(EDeprecatedFunction);
	}

/**
 *  ResetL
 *  This method resets the controller
 *
 */
void CCustomMmfASRController::ResetL()
	{
	//[This function is deprecated and should not have been called]
    Panic(EDeprecatedFunction);
	}

/**
 *
 * PlayL
 *
 */
void CCustomMmfASRController::PlayL()
	{
	//[This function is deprecated and should not have been called]
    Panic(EDeprecatedFunction);
	}

/**
 *  PauseL
 *
 */
void CCustomMmfASRController::PauseL()
	{
	//[This function is deprecated and should not have been called]
    Panic(EDeprecatedFunction);
	}

/**
 *  StopL
 *
 */
void CCustomMmfASRController::StopL()
	{
	//[This function is deprecated and should not have been called]
    Panic(EDeprecatedFunction);
	}

/**
 *  PositionL
 * Preconditions:
 * The Controller is in the state EPrimed
 * @return TTimeIntervalMicroSeconds
 *
 */
TTimeIntervalMicroSeconds CCustomMmfASRController::PositionL() const
	{
	//[This function is deprecated and should not have been called]
    Panic(EDeprecatedFunction);
	TTimeIntervalMicroSeconds position(0);
	return position;
	}

/**
* SetPositionL
*
* @param aPosition
*
*/
void CCustomMmfASRController::SetPositionL(const TTimeIntervalMicroSeconds& )
	{
	//[This function is deprecated and should not have been called]
    Panic(EDeprecatedFunction);
	}

/**
*
* MSrAddPronunciationL
* @param aLexiconID
* @param aModelID
* @param aPronunciationID
* Add pronunciation to the lexicon
* @precondition lexicon is loaded
* @precondition lexicon has specified id
* @post condition lexicon has pronunciation
*
*/
void CCustomMmfASRController::MSrAddPronunciationL( TLexiconID aLexiconID, TModelBankID aModelBankID,
							  TModelID aModelID, TPronunciationID& aPronunciationID )
	{
	//[ assert the InVariant ]
	__ASSERT_ALWAYS(InVariant(),Panic(EBadInvariant));       

	//[ precondition that are in training mode ]
    __ASSERT_ALWAYS( State() == ESTraining, Panic(EBadState) );

	//[ add new pronunciation ]
	TInt err = KErrNone;

	CSDDatabase::TPronunciationData pronunciationData=CSDDatabase::TPronunciationData();
	pronunciationData.iModelBankID = aModelBankID;
	pronunciationData.iModelID = aModelID;
	
	TRAP(err, aPronunciationID = iDatabase->CreatePronunciationL(aLexiconID, pronunciationData));
	
	TMMFEvent addPronunciationComplete( KUidAsrEventAddPronunciation, err );
	SendEventToClient( addPronunciationComplete );

	// [ assert the Invariant  ]
	__ASSERT_ALWAYS(InVariant(),Panic(EBadInvariant));

	}
	
	

void CCustomMmfASRController::MSrSetClientUid( TUid aClientUid)
	{
	//[ assert the InVariant ]
	__ASSERT_ALWAYS(InVariant(),Panic(EBadInvariant));
	
	iDatabase->SetClientUid(aClientUid);
       
	}


/**
*
* MSrAddRule
* @param aGrammarID
* @param aLexiconID
* @param aPronunciationID
* @param aRuleID
* @postcondition grammar has rule
*/
void CCustomMmfASRController::MSrAddRuleL( 
				TGrammarID aGrammarID, TLexiconID aLexiconID,
				TPronunciationID aPronunciationID, TRuleID& aRuleID ) 
	{
	//[ assert the InVariant ]
	__ASSERT_ALWAYS(InVariant(),Panic(EBadInvariant));

	//[ precondition that are in training mode ]
    __ASSERT_ALWAYS( State() == ESTraining, Panic(EBadState) );
  

	//[ if the grammar has the rule ]
	TInt err = KErrNone;
	CSDDatabase::TRuleData ruleData=CSDDatabase::TRuleData();
	ruleData.iLexiconID = aLexiconID;
	ruleData.iPronunciationID = aPronunciationID;

	TRAP( err, aRuleID = iDatabase->CreateRuleL( aGrammarID, ruleData));


	TMMFEvent event( KUidAsrEventAddRule, err );
	SendEventToClient( event );

	// [ assert the Invariant  ]
	__ASSERT_ALWAYS(InVariant(),Panic(EBadInvariant));

	}


/**
*
* MSrCancel
*
*/
void CCustomMmfASRController::MSrCancel() 
	{
	__ASSERT_ALWAYS(InVariant(),Panic(EBadInvariant));
	// Send back a test event to the client to indicate this has been called, note this is test controller specific event
	TMMFEvent testCancelEvent( KUidTestASRCancel, KErrNone );
	SendEventToClient( testCancelEvent );
	}

/**
*
* MSrCommitChangesL
*
*/
void CCustomMmfASRController::MSrCommitChangesL() 
	{
	//[ assert the Invariant ]
	__ASSERT_ALWAYS(InVariant(),Panic(EBadInvariant));

	iDatabase->CommitChangesL();

	// [ assert the invariant ]
	__ASSERT_ALWAYS(InVariant(),Panic(EBadInvariant));
	}

/**
*
* MSrCreateGrammarL
* Creates a Grammar and makes it the currently loaded grammar
* @param aGrammarID
* @precondition State == ESTraining
* @postcondition iAsrSchema has grammar with id aGrammarID
*
*/
void CCustomMmfASRController::MSrCreateGrammarL( TGrammarID& aGrammarID ) 
	{
    //[ assert the Invariant ]
	__ASSERT_ALWAYS(InVariant(),Panic(EBadInvariant));

	//[ precondition that we are in trainning state ]
	__ASSERT_ALWAYS( State() == ESTraining, Panic(EBadState) );
	
	aGrammarID = iDatabase->CreateGrammarL();
	
	 TMMFEvent event( KUidAsrEventCreateGrammar, KErrNone );
	 SendEventToClient( event );
	
	//[ assert the invariant ]
	__ASSERT_ALWAYS(InVariant(),Panic(EBadInvariant));
	}

/**
*
* MSrCreateLexiconL
* @param aLexiconID
* Creates a Lexicon with Id aLexiconId
*
*/
void CCustomMmfASRController::MSrCreateLexiconL( TLexiconID& aLexiconID ) 
	{
	//[ assert the Invariant ]
	__ASSERT_ALWAYS(InVariant(),Panic(EBadInvariant));

	//[ precondition that we are in trainning state ]
	__ASSERT_ALWAYS( State() == ESTraining, Panic(EBadState) );

	//[ create the Lexicon ]
	aLexiconID = iDatabase->CreateLexiconL();
	
	TMMFEvent event( KUidAsrEventCreateLexicon, KErrNone );
	SendEventToClient( event );

	//[ assert the invariant ]
	__ASSERT_ALWAYS(InVariant(),Panic(EBadInvariant));

	}

/**
*
* MSrCreateModelBankL
* @param aModelBankID
* @precondition state == ESTraining
* @precondition invariant holds
* @postcondition currentModelbank id == modelbank id
* 
*/
void CCustomMmfASRController::MSrCreateModelBankL( TModelBankID& aModelBankID ) 
	{
	//[ assert the Invariant ]
	__ASSERT_ALWAYS(InVariant(),Panic(EBadInvariant));

	//[ precondition that we are in trainning state ]
	__ASSERT_ALWAYS( State() == ESTraining, Panic(EBadState) );

	//[ create the new ModelBank]
    TRAPD(err, aModelBankID = iDatabase->CreateModelBankL());
    
   	TMMFEvent event( KUidAsrEventCreateModelBank, err);
	SendEventToClient( event );

	//[ assert the invariant ]
	__ASSERT_ALWAYS(InVariant(),Panic(EBadInvariant));
	}

/**
*
* MSrEndRecSessionL
* End a recognition session
* @precondition State is ESRecognition
* @postcondition State is Idle
*/
void CCustomMmfASRController::MSrEndRecSessionL()
	{
	//[ The Invariant must hold ]
	__ASSERT_ALWAYS(InVariant(),Panic(EBadInvariant));
	//[ precondition that the state is recognise ]
	__ASSERT_ALWAYS( State() == ESRecognition, Panic(EBadState) );
	//[ transitionn to Idle state ]
    SetState( ESTraining );

	__ASSERT_ALWAYS(InVariant(),Panic(EBadInvariant));
	}

/**
*
* MSrGetAllModelIDsL
* Gets all the model ids in a model bank
* @precondition iModelBank is loaded
* @postcondition model ids are returned
* @postcondition invariant holds
*/
void CCustomMmfASRController::MSrGetAllModelIDsL( TModelBankID aModelBankID, RArray<TModelID>& aModelIDs ) 
	{
	__ASSERT_ALWAYS(InVariant(),Panic(EBadInvariant));

	TRAPD(err, iDatabase->GetAllModelIDsL(aModelBankID, aModelIDs));

	TMMFEvent event( KUidAsrEventGetAllModelIDs, err );
	SendEventToClient( event );

	__ASSERT_ALWAYS(InVariant(),Panic(EBadInvariant));
	}

/**
*
* MSrGetAllPronunciationIDsL
* @precondition InVariant holds
* @postcondition InVariantHolds
*/
void CCustomMmfASRController::MSrGetAllPronunciationIDsL( TLexiconID aLexiconID, RArray<TPronunciationID>& aPronunciationIDs ) 
	{
	__ASSERT_ALWAYS(InVariant(),Panic(EBadInvariant));
	
	TRAPD(err, iDatabase->GetAllPronunciationIDsL(aLexiconID, aPronunciationIDs));
	
	TMMFEvent event( KUidAsrEventGetAllPronunciationIDs, err );
	SendEventToClient( event );

	__ASSERT_ALWAYS(InVariant(),Panic(EBadInvariant));
	
	}

/**
*
* MSrGetAllRuleIDsL
* Get all the rule ids contained in the grammar
* @param aLexiconID
* @param aRuleIDs
* @precondition Invariant holds
* @precondition Lexicon is Loaded
* @postcondition Invariant holds
*/
void CCustomMmfASRController::MSrGetAllRuleIDsL( TLexiconID aLexiconID, RArray<TRuleID>& aRuleIDs ) 
	{
	__ASSERT_ALWAYS(InVariant(),Panic(EBadInvariant));
	
	TRAPD(err, iDatabase->GetAllRuleIDsL(aLexiconID, aRuleIDs));
	
	TMMFEvent event( KUidAsrEventGetAllRuleIDs, err );
	SendEventToClient( event );
	
	__ASSERT_ALWAYS(InVariant(),Panic(EBadInvariant));
	}
	
	
void CCustomMmfASRController::MSrGetAllClientModelBankIDsL(RArray<TModelBankID>& aModelBankIDs ) 
	{
     __ASSERT_ALWAYS(InVariant(),Panic(EBadInvariant));

	TRAPD(err, iDatabase->GetAllModelBankIDsL(aModelBankIDs,ETrue));

	TMMFEvent event( KUidAsrEventGetAllClientModelBankIDs, err );
	SendEventToClient( event );

	__ASSERT_ALWAYS(InVariant(),Panic(EBadInvariant));
	}

void CCustomMmfASRController::MSrGetAllClientLexiconIDsL(RArray<TLexiconID>& aLexiconIDs ) 
	{
	__ASSERT_ALWAYS(InVariant(),Panic(EBadInvariant));
	
	TRAPD(err, iDatabase->GetAllLexiconIDsL(aLexiconIDs, ETrue));

	TMMFEvent event( KUidAsrEventGetAllClientLexiconIDs, err );
	SendEventToClient( event );

	__ASSERT_ALWAYS(InVariant(),Panic(EBadInvariant));
	
	}

void CCustomMmfASRController::MSrGetAllClientGrammarIDsL(RArray<TGrammarID>& aGrammarIDs ) 
	{
	__ASSERT_ALWAYS(InVariant(),Panic(EBadInvariant));
	
	TRAPD(err, iDatabase->GetAllGrammarIDsL(aGrammarIDs, ETrue));

	TMMFEvent event( KUidAsrEventGetAllClientGrammarIDs, err );
	SendEventToClient( event );
	
	__ASSERT_ALWAYS(InVariant(),Panic(EBadInvariant));
	}
	
void CCustomMmfASRController::MSrGetAllModelBankIDsL(RArray<TModelBankID>& aModelBankIDs ) 
	{
     __ASSERT_ALWAYS(InVariant(),Panic(EBadInvariant));

	TRAPD(err, iDatabase->GetAllModelBankIDsL(aModelBankIDs));

	TMMFEvent event( KUidAsrEventGetAllModelBankIDs, err );
	SendEventToClient( event );

	__ASSERT_ALWAYS(InVariant(),Panic(EBadInvariant));
	}

void CCustomMmfASRController::MSrGetAllLexiconIDsL(RArray<TLexiconID>& aLexiconIDs ) 
	{
	__ASSERT_ALWAYS(InVariant(),Panic(EBadInvariant));
	
	TRAPD(err, iDatabase->GetAllLexiconIDsL(aLexiconIDs));

	TMMFEvent event( KUidAsrEventGetAllLexiconIDs, err );
	SendEventToClient( event );

	__ASSERT_ALWAYS(InVariant(),Panic(EBadInvariant));
	
	}

void CCustomMmfASRController::MSrGetAllGrammarIDsL(RArray<TGrammarID>& aGrammarIDs ) 
	{
	__ASSERT_ALWAYS(InVariant(),Panic(EBadInvariant));
	
	TRAPD(err, iDatabase->GetAllGrammarIDsL(aGrammarIDs));

	TMMFEvent event( KUidAsrEventGetAllGrammarIDs, err );
	SendEventToClient( event );
	
	__ASSERT_ALWAYS(InVariant(),Panic(EBadInvariant));
	}


/**
*
* MSrGetAvailableStorageL
* gets the available storage
* For this simulation the function always returns zero space available
* @parameter aCount
* @precondition Invariant holds
*
*/
void CCustomMmfASRController::MSrGetAvailableStorageL( TInt& aCount ) 
	{
	//[ assert the invariant ]
	__ASSERT_ALWAYS(InVariant(),Panic(EBadInvariant));
	
	//[ for now we will return no space available ]
	aCount = 0;
	
	__ASSERT_ALWAYS(InVariant(),Panic(EBadInvariant));
	}

/**
*
* MSrGetEnginePropertiesL
* @param aProperties
* @precondition Invariant holds
*
*/
void CCustomMmfASRController::MSrGetEnginePropertiesL( const RArray<TInt>& aPropertyId,
						RArray<TInt>& aPropertyValue ) 
	{
	//[ assert the invariant ]
	__ASSERT_ALWAYS(InVariant(),Panic(EBadInvariant));

	//[ empty the src array ]
    aPropertyValue.Reset();

	//[ for all the ids specified ]
    for( TInt count = 0; count < aPropertyId.Count(); count++ )
		{
		//[ we are just faking this for the moment ]
		aPropertyValue.Append( count );
		}

    //[assert there are properties for all the ids]
	__ASSERT_ALWAYS( aPropertyValue.Count() == aPropertyId.Count(), Panic(EPostConditionViolation) );
	__ASSERT_ALWAYS(InVariant(),Panic(EBadInvariant));
	}

/**
*
* MSrLoadEngineParametersL
*
*/
void CCustomMmfASRController::MSrLoadEngineParametersL( const RArray<TInt>& aParameterId, 
						const RArray<TInt>& aParameterValue )
	{
	TBool pass = ETrue;
	if (aParameterId.Count()!=KNumParameters ||
		aParameterValue.Count()!=KNumParameters)
		pass = EFalse;
	else 
		{
		TInt i;
		for (i=0;i<aParameterId.Count() && pass;i++)
			if (aParameterId[i]!=KParameterIDs[i])
				pass = EFalse;
				
		for (i=0;i<aParameterValue.Count() && pass;i++)
			if (aParameterValue[i]!=KParameterValues[i])
				pass = EFalse;
				
	
		}
	if (pass)
		{
		TMMFEvent event( KUidTestASRLoadParameters, KErrNone );
		SendEventToClient(event);					
		}
	else
		{
		TMMFEvent event( KUidTestASRLoadParameters, KErrArgument );
		SendEventToClient(event);					
		}
	}

/**
*
* MSrGetModelCountL
* @param aModelBankID
* @param aCount
* @precondition Invariant holds
* @precondition model is loaded
* @postcondition Invariant holds
* @postcondition modelcount >=0
*/
void CCustomMmfASRController::MSrGetModelCountL( TModelBankID aModelBankID, TInt& aCount ) 
	{
	//[ assert the invariant ]
	__ASSERT_ALWAYS(InVariant(),Panic(EBadInvariant));

	RArray<TPronunciationID> ids;
	CleanupClosePushL(ids);
	TRAPD(err, iDatabase->GetAllModelIDsL(aModelBankID,ids));
	
	if (err==KErrNone)
		aCount = ids.Count();
		
	CleanupStack::PopAndDestroy(&ids);
	
	TMMFEvent event( KUidAsrEventGetModelCount, err );
	SendEventToClient( event );


	//[ assert the invariant ]
	__ASSERT_ALWAYS(InVariant(),Panic(EBadInvariant));
	}



/**
*
* MSrGetRuleValidityL
* @param aGrammarID
* @param aRuleID
* @param aValid
* @precondition InVariantHolds
* @precondition state is training or state is recognition
* @precondition grammar is loaded
* @postcondition InVariantHolds
*/
void CCustomMmfASRController::MSrGetRuleValidityL( TGrammarID aGrammarID, TRuleID aRuleID, TBool& aValid ) 
	{
	//[ assert the invariant ]
	__ASSERT_ALWAYS(InVariant(),Panic(EBadInvariant));
	//[ precondition the schema exists ]

	CSDDatabase::RSDRuleTable ruleTable(*iDatabase);
	CleanupClosePushL(ruleTable);
	ruleTable.OpenL(aGrammarID);
	TRAPD(err, aValid = ruleTable.IsRuleValidL(aRuleID));
	CleanupStack::PopAndDestroy(&ruleTable);

   	TMMFEvent event( KUidAsrEventGetRuleValidity, err );
	SendEventToClient( event );

	//[ assert the invariant ]
	__ASSERT_ALWAYS(InVariant(),Panic(EBadInvariant));
	}

/**
*
* MSrGetUtteranceDurationL
* @param aModelBankID
* @param aModelID
* @param aDuration
* @param aFromMemory
* 
*/
void CCustomMmfASRController::MSrGetUtteranceDurationL( TModelBankID aModelBankID, TModelID aModelID,
								   TTimeIntervalMicroSeconds32& aDuration) 
	{
    //[ assert the Invariant ]
	__ASSERT_ALWAYS(InVariant(),Panic(EBadInvariant));

	CSDDatabase::TModelData modelData = CSDDatabase::TModelData();

	TInt err;
	TRAP(err, iDatabase->GetModelDataL(aModelBankID, aModelID, modelData));
	if (err==KErrNone)
		{
		aDuration = modelData.iUtteranceDurationMicroSeconds;
		}

	TMMFEvent event( KUidAsrEventGetUtteranceDuration, err );
	SendEventToClient( event );


	//[ assert the Invariant ]
	__ASSERT_ALWAYS(InVariant(),Panic(EBadInvariant));;
	}

/**
*
* MSrLoadGrammarL
* @param aGrammarID
* @precondition aGrammarId is supported by the test controller
* @postcondition aGrammarId == id of loaded grammar
*/
void CCustomMmfASRController::MSrLoadGrammarL( TGrammarID aGrammarID ) 
	{
	//[ assert the Invariant ]
	__ASSERT_ALWAYS(InVariant(),Panic(EBadInvariant));;

	//[ must be in trainning or recognition state ]
	__ASSERT_ALWAYS( (State() == ESRecognition ) || ( State() == ESTraining ), Panic(EBadState) );

	TInt err = KErrNone;	
	if (iDatabase->GrammarExistsL(aGrammarID))
		{
		iLoadedGrammarID = aGrammarID;
		}
	else
		err = KErrNotFound;
		
	TMMFEvent event( KUidAsrEventLoadGrammar, err );
	SendEventToClient( event );

	//[ assert the Invariant ]
	__ASSERT_ALWAYS(InVariant(),Panic(EBadInvariant));;
	}

/**
*
* MSrLoadLexiconL
* @param aLexiconID
* @precondition Invariant Holds
* @precondition state is Training or Recognition
* @precondition controller supports the lexicon
* @postcondition iLexicon has id aLexiconId
* @psotcondition InVariant holds
*/
void CCustomMmfASRController::MSrLoadLexiconL( TLexiconID aLexiconID ) 
	{
	//[ assert the Invariant ]
	__ASSERT_ALWAYS(InVariant(),Panic(EBadInvariant));;

	//[ must be in trainning or recognition state ]
	__ASSERT_ALWAYS( (State() == ESRecognition ) || ( State() == ESTraining ), Panic(EBadState));

	TInt err = KErrNone;
	if (iDatabase->LexiconExistsL(aLexiconID))
		{
		iLoadedLexiconID = aLexiconID;
		}
	else
		err = KErrNotFound;

	TMMFEvent event( KUidAsrEventLoadLexicon, err );
	SendEventToClient( event );

	//[ assert the Invariant ]
	__ASSERT_ALWAYS(InVariant(),Panic(EBadInvariant));;
	}

/**
*
* MSrLoadModelsL
* @param aModelBankID
* @precondition InVariant holds
* @precondition state is training or recognition
* @precondition model bank is is supported by the controller
* @postcondition modelbank id is equal to aModelBankId
* @postcondition InVariant holds
*/
void CCustomMmfASRController::MSrLoadModelsL( TModelBankID aModelBankID ) 
	{
	//[ assert the Invariant ]
	__ASSERT_ALWAYS(InVariant(),Panic(EBadInvariant));;

	//[ must be in trainning or recognition state ]
	__ASSERT_ALWAYS( (State() == ESRecognition ) || ( State() == ESTraining ), Panic(EBadState));

	TInt err = KErrNone;
	if (iDatabase->ModelBankExistsL(aModelBankID))
		{
		iLoadedModelBankID = aModelBankID;
		}
	else
		err = KErrNotFound;

	TMMFEvent event( KUidAsrEventLoadModels, err );
	SendEventToClient( event );

	//[ assert the Invariant ]
	__ASSERT_ALWAYS(InVariant(),Panic(EBadInvariant));;

	}

/**
*
* MSrPlayUtteranceL
* @param aModelBankID 
* @param aModelID
* @param aPlayFromMemory
* Always plays froms memory
*/
void CCustomMmfASRController::MSrPlayUtteranceL( TModelBankID aModelBankID, TModelID aModelID ) 
	{
	//[ assert the InVariant ]
	__ASSERT_ALWAYS(InVariant(),Panic(EBadInvariant));;

	if (!iDatabase->ModelExistsL(aModelBankID, aModelID))
		{
		User::Leave(KErrNotFound);
		}
	else
		{
		TMMFEvent aPlayStarted( KUidAsrEventPlayStarted, KErrNone );
		SendEventToClient( aPlayStarted );
		
		TMMFEvent aPlayComplete( KUidAsrEventPlay, KErrNone );
		SendEventToClient( aPlayComplete );
		}

	//[ assert the InVariant ]
	__ASSERT_ALWAYS(InVariant(),Panic(EBadInvariant));;
	}

/**
*
* MSrRecognizeL
* @param aResultSet
*
*/
void CCustomMmfASRController::MSrRecognizeL( CSDClientResultSet& aResultSet ) 
	{
	//[ assert the Invariant ]
	__ASSERT_ALWAYS(InVariant(),Panic(EBadInvariant));;

	//[ Precondition that we are in a recognition session ]
	if( State() != ESRecognition )
		{
		TMMFEvent recognitionFailed( KUidAsrEventRecognition, KErrNotReady );
		SendEventToClient( recognitionFailed );
		return;
		}

	TMMFEvent recognitionReady( KUidAsrEventRecognitionReady,KErrNone );
	SendEventToClient( recognitionReady );

	//[ return a result set ]
	iMaxResults = aResultSet.MaxResults();
    iResultSet = &aResultSet;

    iRecognizeComplete = ETrue;
	
	//[ assert the Invariant ]
	__ASSERT_ALWAYS(InVariant(),Panic(EBadInvariant));;
	}

/*
*
* MSrRecordL
* @param aRecordTime
* @precondition InVariant holds
* @postcondition 
*/
void CCustomMmfASRController::MSrRecordL( TTimeIntervalMicroSeconds32 aRecordTime ) 
	{
	//[ assert the invariant ]
	__ASSERT_ALWAYS(InVariant(),Panic(EBadInvariant));;
	
	//[ remember the record time 
	// and for the purpose of simulation recording takes zero time ]
	iRecordTime = aRecordTime ;

	TMMFEvent aRecordStartedEvent( KUidAsrEventRecordStarted, KErrNone );
	SendEventToClient( aRecordStartedEvent );

	TMMFEvent aEndOfUtteranceEvent( KUidAsrEventEouDetected, KErrNone );
	SendEventToClient( aEndOfUtteranceEvent );

	if( iRecognizeComplete )
		{
		iRecognizeComplete = EFalse;
		
		for (TInt i=0;i<iMaxResults && i<KMaxAvailableResults;i++)
		{
			CSDClientResult& result = CONST_CAST(CSDClientResult&, iResultSet->At(i));
			result.SetGrammarID(KResultsGrammarID[i]);
			result.SetRuleID(KResultsRuleID[i]);
		}
		
		TMMFEvent recognitionComplete( KUidAsrEventRecognition, KErrNone );
		SendEventToClient( recognitionComplete );
		}

 	//[ assert the invaraint ]
	__ASSERT_ALWAYS(InVariant(),Panic(EBadInvariant));;
	}

/**
*
* MSrRemoveGrammarL
* Interpreted as unload from memory here 
*
*/
void CCustomMmfASRController::MSrRemoveGrammarL( TGrammarID aGrammarID ) 
	{
	//[ assert the invaraint ]
	__ASSERT_ALWAYS(InVariant(),Panic(EBadInvariant));
    

	if (iLoadedGrammarID == aGrammarID)
		{
		TMMFEvent event( KUidAsrEventRemoveGrammar, KErrInUse );
		SendEventToClient( event );
		}		
	else
		{		
		TRAPD(err, iDatabase->RemoveGrammarL(aGrammarID));
		//[ send message to client of success ]
		TMMFEvent event( KUidAsrEventRemoveGrammar, err );
		SendEventToClient( event );
		}

	//[ assert the invaraint ]
	__ASSERT_ALWAYS(InVariant(),Panic(EBadInvariant));;
	}

/**
*
* MSrRemoveLexiconL
* @param aLexiconID
* slightly different interpretation here 
*/
void CCustomMmfASRController::MSrRemoveLexiconL( TLexiconID aLexiconID ) 
	{
    //[ assert the invaraint ]
	__ASSERT_ALWAYS(InVariant(),Panic(EBadInvariant));;


	if (iLoadedLexiconID == aLexiconID)
		{
		TMMFEvent event( KUidAsrEventRemoveLexicon, KErrInUse );
		SendEventToClient( event );
		}		
	else
		{		
		TRAPD(err, iDatabase->RemoveLexiconL(aLexiconID));
		//[ send message to client of success ]
		TMMFEvent event( KUidAsrEventRemoveLexicon, err );
		SendEventToClient( event );
		}
	
	//[ assert the invaraint ]
	__ASSERT_ALWAYS(InVariant(),Panic(EBadInvariant));;
	

	}

/**
*
* MSrRemoveModelBankL
*
*
*/
void CCustomMmfASRController::MSrRemoveModelBankL( TModelBankID aModelBankID )
	{
    //[ assert the invaraint ]
	__ASSERT_ALWAYS(InVariant(),Panic(EBadInvariant));;


	if (iLoadedModelBankID == aModelBankID)
		{
		TMMFEvent event( KUidAsrEventRemoveModelBank, KErrInUse );
		SendEventToClient( event );
		}		
	else
		{		
		TRAPD(err, iDatabase->RemoveModelBankL(aModelBankID));
		//[ send message to client of success ]
		TMMFEvent event( KUidAsrEventRemoveModelBank, err );
		SendEventToClient( event );
		}
	
	//[ assert the invaraint ]
	__ASSERT_ALWAYS(InVariant(),Panic(EBadInvariant));;

	}

/**
*
* MSrRemoveModelL
* @param aModelBankID
* @param aModeID
* @precondition InVariant holds
* @precondition model bank exists
* @precondition modelbank contains the model
* @postcondition model bank does not contain model
*
*/
void CCustomMmfASRController::MSrRemoveModelL( TModelBankID aModelBankID, TModelID aModelID )
	{
	//[ assert the invariant ]
	__ASSERT_ALWAYS(InVariant(),Panic(EBadInvariant));;
	
	if (iLoadedModelBankID == aModelBankID)
		{
		TMMFEvent event( KUidAsrEventRemoveModelBank, KErrInUse );
		SendEventToClient( event );
		}		
	else
		{	
		TInt err = KErrNone;
		if (!iDatabase->ModelBankExistsL(aModelBankID))
			err = KErrNotFound;
		else 
			{
			CSDDatabase::RSDModelTable view(*iDatabase);
			CleanupClosePushL(view);
			view.OpenL(aModelBankID);
			if (view.FindModelL(aModelID))
				{
				view.RemoveModelL(aModelID);
				}
			else
				err = KErrNotFound;
				
			CleanupStack::PopAndDestroy(&view);
			}
		//[ send message to client of success ]
		TMMFEvent event( KUidAsrEventRemoveModel, err );
		SendEventToClient( event );
		}
	
	//[ assert the invaraint ]
	__ASSERT_ALWAYS(InVariant(),Panic(EBadInvariant));;
	}

/**
*
* MSrRemovePronunciationL
* @param aLexiconID
* @param aPronunciationID
* @precondition lexicon exists
* @precondition lexicon has id specified
* @postconfition lexicon does not have pronunication
*
*/
void CCustomMmfASRController::MSrRemovePronunciationL( TLexiconID aLexiconID, TPronunciationID aPronunciationID )
	{
   	//[ assert the invaraint ]
	__ASSERT_ALWAYS(InVariant(),Panic(EBadInvariant));;
	
	if (iLoadedLexiconID == aLexiconID)
		{
		TMMFEvent event( KUidAsrEventRemovePronunciation, KErrInUse );
		SendEventToClient( event );
		}		
	else
		{	
		TInt err = KErrNone;
		if (!iDatabase->LexiconExistsL(aLexiconID))
			err = KErrNotFound;
		else 
			{
			CSDDatabase::RSDPronunciationTable view(*iDatabase);
			CleanupClosePushL(view);
			view.OpenL(aLexiconID);
			if (view.FindPronunciationL(aPronunciationID))
				{
				view.RemovePronunciationL(aPronunciationID);
				}
			else
				err = KErrNotFound;
				
			CleanupStack::PopAndDestroy(&view);
			}
		//[ send message to client of success ]
		TMMFEvent event( KUidAsrEventRemovePronunciation, err );
		SendEventToClient( event );
		}
	
	//[ assert the invaraint ]
	__ASSERT_ALWAYS(InVariant(),Panic(EBadInvariant));;
	}

/**
*
* MSrRemoveRuleL
* @param aLexiconID
* @param aRuleID
* @precondition InVariant holds
* @precondition grammar is loaded
* @precondition grammar has id specified
* @postcondition grammar does not contain rule
*
*/
void CCustomMmfASRController::MSrRemoveRuleL( TGrammarID aGrammarID, TRuleID aRuleID )
	{
	//[ assert the invaraint ]
	__ASSERT_ALWAYS(InVariant(),Panic(EBadInvariant));;
	
	if (iLoadedGrammarID == aGrammarID)
		{
		TMMFEvent event( KUidAsrEventRemoveRule, KErrInUse );
		SendEventToClient( event );
		}		
	else
		{		
		TInt err = KErrNone;
		if (!iDatabase->GrammarExistsL(aGrammarID))
			err = KErrNotFound;
		else 
			{
			CSDDatabase::RSDRuleTable view(*iDatabase);
			CleanupClosePushL(view);
			view.OpenL(aGrammarID);
			if (view.FindRuleL(aRuleID))
				{
				view.RemoveRuleL(aRuleID);
				}
			else
				err = KErrNotFound;
				
			CleanupStack::PopAndDestroy(&view);
			}
		//[ send message to client of success ]
		TMMFEvent event( KUidAsrEventRemoveRule, err );
		SendEventToClient( event );
		}
	
	//[ assert the invaraint ]
	__ASSERT_ALWAYS(InVariant(),Panic(EBadInvariant));;
	}

/**
*
* MSrStartRecSessionL
* @param aMode
* @precondition mode is valid
*
*/
void CCustomMmfASRController::MSrStartRecSessionL( TRecognitionMode aMode )
	{
	//[ assert the invariant holds ]
	__ASSERT_ALWAYS(InVariant(),Panic(EBadInvariant));;

	//[ precondition the mode is valid ]
	if( (aMode < ESdMode ) || ( aMode > ESdSiMode) )
		{
		User::Leave( KErrArgument );
		}

	//[ it does not matter which state we are in we can
	// always transition to recognition ]
	SetState( ESRecognition );

	//[ set the recognition mode ]
	iRecognitionMode = aMode ;

	//[ assert the Invariant holds ]
	__ASSERT_ALWAYS(InVariant(),Panic(EBadInvariant));;

	//[ assert the post condition that the mode has been en
	__ASSERT_ALWAYS( iRecognitionMode == aMode, Panic(EPostConditionViolation) );
	}

/**
*
* MSrTrainL
* @param aModelBankID
* @param aModelID
*
*/
void CCustomMmfASRController::MSrTrainL( TModelBankID aModelBankID, TModelID& aModelID )
	{
	// fixed duration to return for training
	const TInt KUtteranceDuration = 2000;
     //[ must be in training mode
	//   must have a lexicon that is not empty 
	//   must have a grammar
	//   must have a model bank loaded ]
	//[ presume train adds a new model id to the bank ]
	//[ finally send event to client ]
	__ASSERT_ALWAYS(InVariant(),Panic(EBadInvariant));;
	TMMFEvent event( KUidAsrEventTrainReady, KErrNone );
	SendEventToClient( event );

	// [ precondition model bank is loaded ]
	if( iLoadedModelBankID != aModelBankID )
		 {
	 	 TMMFEvent event( KUidAsrEventTrain, KErrNotReady );
		 SendEventToClient( event );
		 return;
		 }

	TInt err = KErrNone;
	
	CSDDatabase::TModelData modelData=CSDDatabase::TModelData();
	modelData.iUtteranceDurationMicroSeconds = KUtteranceDuration;
	TRAP(err, aModelID = iDatabase->CreateModelL( aModelBankID, modelData));

	TMMFEvent trainCompleteEvent( KUidAsrEventTrain, err );
	SendEventToClient( trainCompleteEvent );
	__ASSERT_ALWAYS(InVariant(),Panic(EBadInvariant));;
	}

/**
*
* MSrUnloadRuleL
* @param aGrammarID
* @param aRuleID
* @precondition state == ESTraining
* @postcondition grammar does not contain rule
*
*/
void CCustomMmfASRController::MSrUnloadRuleL( TGrammarID aGrammarID, TRuleID aRuleID )
	{
	__ASSERT_ALWAYS(InVariant(),Panic(EBadInvariant));;

	//[ precondition grammar loaded is grammar with id ]
	__ASSERT_ALWAYS( iLoadedGrammarID == aGrammarID, Panic(EPreConditionViolation) );

	TBool ruleExists = EFalse;

	TRAPD(err, ruleExists = iDatabase->RuleExistsL(aGrammarID, aRuleID));

	if (err == KErrNone)
		{
		if (!ruleExists)
			err = KErrNotFound;
		}

	//[ send message to client of success ]
	TMMFEvent event( KUidAsrEventUnloadRule, err );
	SendEventToClient( event );

	__ASSERT_ALWAYS(InVariant(),Panic(EBadInvariant));;
	}
	
/**
 *  SendEventToClient
 *
 *	@param aEvent
 */
TInt CCustomMmfASRController::SendEventToClient(const TMMFEvent& aEvent)
	{
	//now send event to client...
	return DoSendEventToClient(aEvent);
	}

/**
*
* GetNumberOfMetaDataEntriesL
*
* @param "TInt"
*
*/
void CCustomMmfASRController::GetNumberOfMetaDataEntriesL(TInt& /*aNumberOfEntries*/ ) 
	{
	 //[This function is deprecated and should not have been called]
     Panic(EDeprecatedFunction);
	}

/**
* GetMetaDataEntryL
* @param aIndex
* @returns "CMMFMetaDataEntry*"
*/
CMMFMetaDataEntry* CCustomMmfASRController::GetMetaDataEntryL(TInt /*aIndex*/ ) 
	{
	//[This function is deprecated and should not have been called]
    Panic(EDeprecatedFunction);
	return NULL;
	}

/**
* RemoveDataSourceL
* @param aDataSource
* 
*/
void CCustomMmfASRController::RemoveDataSourceL(MDataSource& /*aDataSource*/ )
	{
	//[This function is deprecated and should not have been called]
    Panic(EDeprecatedFunction);
	}

/**
* RemoveDataSinkL
* 
* @param aDataSink
*
*/
void CCustomMmfASRController::RemoveDataSinkL(MDataSink& /*aDataSink*/ )
	{
	 //[This function is deprecated and should not have been called]
     Panic(EDeprecatedFunction);
	}

/**
 *  SetPrioritySettings
 *
 *	@param aPrioritySettings
 */
void CCustomMmfASRController::SetPrioritySettings(const TMMFPrioritySettings& /*aPrioritySettings*/)
	{
	}

/**
*
* DurationL
*
* @returns TTimeIntervalMicroSeconds 
*
*/
TTimeIntervalMicroSeconds CCustomMmfASRController::DurationL() const
	{
	TTimeIntervalMicroSeconds xx(0);
	return xx; 
	}

/**
*
* InVariant
* @returns TBool
*
*/
TBool CCustomMmfASRController::InVariant() const
	{
	 TBool status = EFalse;
	 //[ for now the invariant is simply that we have a valid state ]
	 if( (iState == ESTraining ) || ( iState == ESTraining ) || ( iState == ESRecognition ) )
		 {
		 status = ETrue;
		 }
	 
	 return status;
	}

/**
*
* State
* @returns TControllerState
*
*/
CCustomMmfASRController::TControllerState CCustomMmfASRController::State() const
	{
	return iState;	
	}

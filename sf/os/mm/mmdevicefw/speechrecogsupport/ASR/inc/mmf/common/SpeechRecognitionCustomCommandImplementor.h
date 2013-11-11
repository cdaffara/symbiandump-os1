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

#ifndef __SPEECHRECOGNITIONCUSTOMCOMMANDIMPLEMENTOR_H__
#define __SPEECHRECOGNITIONCUSTOMCOMMANDIMPLEMENTOR_H__

// INCLUDES

#include <mmf/common/speechrecognitiondataclient.h>

// CLASS DECLARATION

/**
@publishedAll
@released

This is the main class of Speech Recognition Custom Commands Implementor.

@since  8.0
*/

class MSpeechRecognitionCustomCommandImplementor
	{
public:
	/**
	Adds a new pronunciation for the given model into the specified
	lexicon.

	@param  aLexiconID
	        The lexicon ID to where the new pronunciation is added.
	@param  aModelBankID
	        The model bank ID.
	@param  aModelID
	        The model ID.
	@param  aPronunciationID
	        A reference to where a new pronunciation ID is assigned.

	@since  8.0
	*/
	virtual void MSrAddPronunciationL( TLexiconID aLexiconID,
					TModelBankID aModelBankID, TModelID aModelID,
					TPronunciationID& aPronunciationID ) = 0;

	/**
	Adds a new rule for the given pronunciation into the specified
	grammar.

	@param  aGrammarID			
	        The grammar ID, where the new rule is added to
	@param  aLexiconID
	        The lexicon ID.
	@param  aPronunciationID
	        The pronunciation ID.
	@param  aRuleID
	        A reference where a new rule ID is assigned to.

	@since  8.0
	*/
	virtual void MSrAddRuleL( TGrammarID aGrammarID, TLexiconID aLexiconID,
					TPronunciationID aPronunciationID,
					TRuleID& aRuleID ) = 0;

	/**
	Cancels the current operation.

	@since  8.0
	*/
	virtual void MSrCancel() = 0;

	/**
	Commit uncommited changes to the database.

	@since  8.0
	*/
	virtual void MSrCommitChangesL() = 0;

	/**
	Creates a new grammar.

	@param  aGrammarID			
	        A reference where the grammar ID is stored.

	@since  8.0
	*/
	virtual void MSrCreateGrammarL( TGrammarID& aGrammarID ) = 0;

	/**
	Creates a new lexicon.

	@param  aLexiconID			
	        A reference where the lexicon ID is stored.

	@since  8.0
	*/
	virtual void MSrCreateLexiconL( TLexiconID& aLexiconID ) = 0;

	/**
	Creates a new model bank.

	@param  aModelBankID		
	        A reference to where a new model bank ID is assigned.

	@since	8.0
	*/
	virtual void MSrCreateModelBankL( TModelBankID& aModelBankID ) = 0;

	/**
	Ends the current recognition session. Resources allocated
	for recognition are freed.

	@since  8.0
	*/
	virtual void MSrEndRecSessionL() = 0;

	/**
	Returns all grammar IDs that are owned by the client.

	@param  aGrammarIDs			
	        A reference where grammar IDs are stored.

	@since  8.0
	*/
	virtual void MSrGetAllClientGrammarIDsL(RArray<TGrammarID>& aGrammarIDs ) = 0;

	/**
	Returns all lexicon IDs that are owned by the client.

	@param  aLexiconIDs	
	        A reference where the lexicon IDs are stored.

	@since  8.0
	*/
	virtual void MSrGetAllClientLexiconIDsL(RArray<TLexiconID>& aLexiconIDs ) = 0;
	/**
	Returns all modelbank IDs that are owned by the client.

	@param  aModelBankIDs	
	        A reference where model bank IDs are stored.

	@since  8.0
	*/
	virtual void MSrGetAllClientModelBankIDsL(RArray<TModelBankID>& aModelBankIDs ) = 0;

	/**
	Returns all grammar IDs.

	@param  aGrammarIDs			
	        A reference where grammar IDs are stored.
	*/
	virtual void MSrGetAllGrammarIDsL(RArray<TGrammarID>& aGrammarIDs ) = 0;

	/**
	Returns all lexicon IDs.

	@param  aLexiconIDs	
	        A reference where lexicon IDs are stored.
	*/
	virtual void MSrGetAllLexiconIDsL(RArray<TLexiconID>& aLexiconIDs ) = 0;

	/**
	Returns all modelbank IDs.

	@param  aModelBankIDs	
	        A reference where the model bank IDs are stored.
	
	@since  8.0
	*/
	virtual void MSrGetAllModelBankIDsL(RArray<TModelBankID>& aModelBankIDs ) = 0;

	/**
	Returns all model IDs that exist in the specified model bank.

	@param  aModelBankID
			The model bank ID.
	@param  aModelIDs
	        A reference where model IDs are stored.

	@since  8.0
	*/
	virtual void MSrGetAllModelIDsL( TModelBankID aModelBankID,
					RArray<TModelID>& aModelIDs ) = 0;

	/**
	Returns all pronunciation IDs that exist in the specified lexicon.

	@param  aLexiconID
	        The lexicon ID.
	@param  aPronunciationIDs
	        A reference where pronunciation IDs are stored.

	@since  8.0
	*/
	virtual void MSrGetAllPronunciationIDsL( TLexiconID aLexiconID,
					RArray<TPronunciationID>& aPronunciationIDs ) = 0;

	/**
	Returns all rule IDs that exist in the specified grammar.

	@param  aGrammarID
	        The grammar ID.
	@param  aRuleIDs
	        A reference where rule IDs are stored.

	@since  8.0
	*/
	virtual void MSrGetAllRuleIDsL( TGrammarID aGrammarID,
					RArray<TRuleID>& aRuleIDs ) = 0;

	/**
	Returns the number of models available for training system wide,
	based on available disk space.


	@param  aCount				
	        A reference where the number of available models is set.

	@since  8.0
	*/
	virtual void MSrGetAvailableStorageL( TInt& aCount ) = 0;

	/**
	Returns the engine properties specified by the engine property
	ID array. 

	On return aPropertyValue contains an array of engine properties.
	
	@param  aPropertyId		
	        A constant reference to an array of engine property IDs.
	@param  aPropertyValue
	        A reference to an array of engine property values.

	@since  8.0
	*/
	virtual void MSrGetEnginePropertiesL( const RArray<TInt>& aPropertyId,
					RArray<TInt>& aPropertyValue ) = 0;

	/**
	Returns the number of models in the specified model bank.

	@param  aModelBankID		
	        The model bank ID.
	@param  aCount				
	        A reference where the number of models is set.

	@since  8.0
	*/
	virtual void MSrGetModelCountL( TModelBankID aModelBankID, TInt& aCount ) = 0;

	/**
	Checks if the rule is valid or not.

	@param  aGrammarID
	        The grammar ID.
	@param  aRuleID
	        The rule ID.
	@param  aValid
	        A reference where the validity of the rule is set.

	@since  8.0
	*/
	virtual void MSrGetRuleValidityL( TGrammarID aGrammarID,
					TRuleID aRuleID, TBool& aValid ) = 0;

	/**
	Returns the duration of the utterance for the specified model.

	@param  aModelBankID		
	        The model bank ID.
	@param  aModelID			
	        The model ID.
	@param  aDuration			
	        A reference where the duration of utterance is set.

	@since  8.0
	*/
	virtual void MSrGetUtteranceDurationL( TModelBankID aModelBankID,
					TModelID aModelID,
					TTimeIntervalMicroSeconds32& aDuration) = 0;
	/**
	Loads the specified grammar in into the recognizer; done prior to recognition.

	@param  aGrammarID			
	        The grammar ID.

	@since  8.0
	*/
	virtual void MSrLoadGrammarL( TGrammarID aGrammarID ) = 0;

	/**
	Loads the specified lexicon into the recognizer; done prior to recognition.

	@param  aLexiconID			
	        The lexicon ID.

	@since  8.0
	*/
	virtual void MSrLoadLexiconL( TLexiconID aLexiconID ) = 0;

	/**
	Loads the specified model bank into the recognizer; done prior to recognition.

	@param  aModelBankID		
	        The model bank ID.

	@since  8.0
	*/
	virtual void MSrLoadModelsL( TModelBankID aModelBankID ) = 0;

	/**
	Plays the previously trained utterance.

	@param  aModelBankID		
	        The model bank ID.
	@param  aModelID			
	        The model ID, whose utterance is played.

	@since  8.0
	*/
	virtual void MSrPlayUtteranceL( TModelBankID aModelBankID,
					TModelID aModelID ) = 0;

	/**
	Initiates recognition; performed following loading of model bank, lexicon, and grammar.

	@param  aResultSet			
	        A reference where the recognition result is set.

	@since  8.0
	*/
	virtual void MSrRecognizeL( CSDClientResultSet& aResultSet ) = 0;

	/**
	Records user utterance for training and recognition.

	@param  aRecordTime			
	        A recording time in microseconds.

	@since  8.0
	*/
	virtual void MSrRecordL( TTimeIntervalMicroSeconds32 aRecordTime ) = 0;

	/**
	Removes the specified grammar from the permanent storage.

	Removing a grammar will remove all rules within the grammar.

	@param  aGrammarID			
	        The grammar ID.

	@since  8.0
	*/
	virtual void MSrRemoveGrammarL( TGrammarID aGrammarID ) = 0;

	/**
	Removes the specified lexicon from the permanent storage.
	
	Removing a lexicon will remove all pronunciations within the lexicon.

	@param  aLexiconID			
	        The lexicon ID.

	@since  8.0
	*/
	virtual void MSrRemoveLexiconL( TLexiconID aLexiconID ) = 0;

	/**
	Removes the specified model bank from the permanent storage.

	Removing a model bank will remove all models within the model bank.

	@param  aModelBankID		
	        The model bank ID.

	@since  8.0
	*/
	virtual void MSrRemoveModelBankL( TModelBankID aModelBankID ) = 0;

	/**
	Removes the specified model from the specified model bank
	permanently.

	@param  aModelBankID		
	        The model bank ID.
	@param  aModelID			
	        The model ID.

	@since  8.0
	*/
	virtual void MSrRemoveModelL( TModelBankID aModelBankID,
					TModelID aModelID ) = 0;

	/**
	Removes the specified pronunciation from the specified lexicon permanently.

	@param  aLexiconID			
	        The lexicon ID.
	@param  aPronunciationID	
	        The pronunciation ID.

	@since  8.0
	*/
	virtual void MSrRemovePronunciationL( TLexiconID aLexiconID,
					TPronunciationID aPronunciationID ) = 0;

	/**
	Removes the specified rule from the specified grammar permanently.

	@param  aGrammarID			
	        The grammar ID.
	@param  aRuleID				
	        The rule ID.

	@since  8.0
	*/
	virtual void MSrRemoveRuleL( TGrammarID aGrammarID,
					TRuleID aRuleID ) = 0;
					
	/**
	Sets the specified grammar client UID.

	@param  aClientUid The client UID.
	*/
	virtual void MSrSetClientUid(TUid aClientUid) = 0;

	/**
	Starts a new recognition session.

	@param  aMode				
	        The recognition mode.

	@since  8.0
	*/
	virtual void MSrStartRecSessionL( TRecognitionMode aMode ) = 0;

	/**
	Trains a new model into the specified model bank.

	@param  aModelBankID		
	        The model bank ID.
	@param  aModelID			
	        A reference to where the new model ID is assigned.

	@since  8.0
	*/
	virtual void MSrTrainL( TModelBankID aModelBankID,
					TModelID& aModelID ) = 0;

	/**
	Unloads the specified rule from the specified grammar in temporary
	memory, previously loaded with LoadGrammarL().

	The rule in the permanent storage remains intact.

	@param  aGrammarID
	        The grammar ID.
	@param  aRuleID
	        The rule ID.

	@since  8.0
	*/
	virtual void MSrUnloadRuleL( TGrammarID aGrammarID,
					TRuleID aRuleID ) = 0;

	/**
	Unloads the specified parameters to the engine.

	@param  aParameterId		
	        An array of parameter IDs.
	@param  aParameterValue		
	        An array of parameter values.

	@since  8.0
	*/
	virtual void MSrLoadEngineParametersL( 
					const RArray<TInt>& aParameterId, 
					const RArray<TInt>& aParameterValue ) = 0;
					
	};

#endif  // __SPEECHRECOGNITIONCUSTOMCOMMANDIMPLEMENTOR_H__

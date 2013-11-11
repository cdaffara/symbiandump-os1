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

#ifndef __SPEECHRECOGNITIONCUSTOMCOMMANDS_H__
#define __SPEECHRECOGNITIONCUSTOMCOMMANDS_H__

//  INCLUDES
#include	<mmf/common/mmfcontroller.h>
#include 	<mmf/common/speechrecognitiondataclient.h>

/**
@publishedAll
@released

This is the main class of Speech Recognition Custom Commands.

@since  8.0
*/
class RSpeechRecognitionCustomCommands : public RMMFCustomCommandsBase
	{
public:
	/**
	Public C++ constructor for RSpeechRecognitionCustomCommands class.

	@param  aController			
	        A reference to a RMMFController object.

	@since  8.0
	*/
	IMPORT_C RSpeechRecognitionCustomCommands(RMMFController& aController);
	
	/** 
	Adds a new pronunciation for the given model into the specified lexicon.

	@param  aLexiconID
	        The lexicon ID to where the new pronunciation is added.
	@param  aModelBankID
	        The model bank ID.
	@param  aModelID
	        The model ID.
	@param  aPronunciationID
	        A reference to where a new pronunciation ID is assigned.

	@return An error code indicating if the function call was successful. KErrNone on success, otherwise
	        another of the system-wide error codes.

	@since  8.0
	*/
	IMPORT_C TInt AddPronunciation(TLexiconID aLexiconID,
					TModelBankID aModelBankID, TModelID aModelID,
					TPronunciationID& aPronunciationID);

	/**
	Adds a new rule for the given pronunciation into the specified grammar.

	@param  aGrammarID			
	        The grammar ID to where the new rule is added.
	@param  aLexiconID			
	        The lexicon ID.
	@param  aPronunciationID	
	        The pronunciation ID.
	@param  aRuleID				
	        A reference to where a new rule ID is assigned.

	@return An error code indicating if the function call was successful. KErrNone on success, otherwise
	        another of the system-wide error codes.

	@since  8.0
	*/
	IMPORT_C TInt AddRule(TGrammarID aGrammarID, TLexiconID aLexiconID,
					TPronunciationID aPronunciationID, TRuleID& aRuleID);

	/**
	Cancels the current operation.

	@since  8.0
	*/
	IMPORT_C void Cancel();

	/**
	Commits the current train operation to the database.

	@return An error code indicating if the function call was successful. KErrNone on success, otherwise
	        another of the system-wide error codes.

	@since  8.0
	*/
	IMPORT_C TInt CommitChanges();

	/**
	Creates a new grammar.

	@param  aGrammarID
	        A reference to where the grammar ID is stored.

	@return An error code indicating if the function call was successful. KErrNone on success, otherwise
	        another of the system-wide error codes.

	@since  8.0
    */
	IMPORT_C TInt CreateGrammar(TGrammarID& aGrammarID);

	/**
	Creates a new lexicon.

	@param  aLexiconID			
	        A reference where lexicon ID is stored.

	@return An error code indicating if the function call was successful. KErrNone on success, otherwise
	        another of the system-wide error codes.

	@since  8.0
    */
	IMPORT_C TInt CreateLexicon(TLexiconID& aLexiconID);

    /**
	Creates a new model bank.

	@param  aModelBankID		
	        A reference to where a new model bank ID is assigned.

	@return An error code indicating if the function call was successful. KErrNone on success, otherwise
	        another of the system-wide error codes.

	@since  8.0
    */
	IMPORT_C TInt CreateModelBank(TModelBankID& aModelBankID);

	/**
	Ends the current recognition session.
	
	The resources allocated for recognition are freed.

	@return An error code indicating if the function call was successful. KErrNone on success, otherwise
	        another of the system-wide error codes.

	@since  8.0
    */
	IMPORT_C TInt EndRecSession();

	/**
	Returns all grammar IDs owned by the client.

	@return An error code indicating if the function call was successful. KErrNone on success, otherwise
	        another of the system-wide error codes.

	@since  8.0
    */
	IMPORT_C TInt GetAllClientGrammarIDs();

	/**
	Returns all lexicon IDs owned by the client.

	@return An error code indicating if the function call was successful. KErrNone on success, otherwise
	        another of the system-wide error codes.
	
	@since  8.0
    */
	IMPORT_C TInt GetAllClientLexiconIDs();

	/**
	Returns all modelBank IDs owned by the client.

    @return An error code indicating if the function call was successful. KErrNone on success, otherwise
	           another of the system-wide error codes.

	@since  8.0
    */
	IMPORT_C TInt GetAllClientModelBankIDs();

	/**
	Returns all grammar IDs owned by the client.

	@return An error code indicating if the function call was successful. KErrNone on success, otherwise
	        another of the system-wide error codes.

	@since  8.0
    */
	IMPORT_C TInt GetAllGrammarIDs();

	/**
	Returns all lexicon IDs owned by the client.

	@return An error code indicating if the function call was successful. KErrNone on success, otherwise
	        another of the system-wide error codes.

	@since  8.0
    */
	IMPORT_C TInt GetAllLexiconIDs();


	/**
	Returns all modelBank IDs owned by the client.

	@return An error code indicating if the function call was successful. KErrNone on success, otherwise
	        another of the system-wide error codes.

	@since  8.0
    */
	IMPORT_C TInt GetAllModelBankIDs();

	/**
	Returns all model IDs that exist in the specified model bank.

	@param  aModelBankID		
	        The model bank ID.

	@return An error code indicating if the function call was successful. KErrNone on success, otherwise
	        another of the system-wide error codes.

	@since  8.0
    */
	IMPORT_C TInt GetAllModelIDs(TModelBankID aModelBankID);

	/**
	Returns all pronunciation IDs that exist in the specified lexicon.

	@param  aLexiconID			
	        The lexicon ID.

	@return An error code indicating if the function call was successful. KErrNone on success, otherwise
	        another of the system-wide error codes.

	@since  8.0
    */
	IMPORT_C TInt GetAllPronunciationIDs(TLexiconID aLexiconID);

	/**
	Returns all rule IDs that exist in the specified grammar.

    @param  aGrammarID			
	        The grammar ID.

	@return An error code indicating if the function call was successful. KErrNone on success, otherwise
	        another of the system-wide error codes.

	@since  8.0
    */
	IMPORT_C TInt GetAllRuleIDs(TGrammarID aGrammarID);

	/**
	Returns the number of models available for training system wide,
	based on available disk space.

	@param  aAvailableStorage	
	        The number of models that can be held in available storage space.

	@return An error code indicating if the function call was successful. KErrNone on success, otherwise
	        another of the system-wide error codes.

	@since  8.0
    */
	IMPORT_C TInt GetAvailableStorage(TInt& aAvailableStorage);

	/**
	Returns the engine properties specified by the engine property
	ID array. On return aPropertyValue contains an array of engine
	properties.

	@param  aPropertyId		
	        A constant reference to an array of engine property IDs.
	@param  aPropertyValue	
	        A reference to an array of engine property values.

	@return An error code indicating if the function call was successful. KErrNone on success, otherwise
	        another of the system-wide error codes.

	@since  8.0
    */
	IMPORT_C TInt GetEngineProperties(const RArray<TInt>& aPropertyId,
					RArray<TInt>& aPropertyValue);

	/**
	Returns the number of models in the specified model bank.

	@param  aModelBankID		
	        The model bank ID.
	@param  aModelCount			
	        The reference where the number of models is set.

	@return An error code indicating if the function call was successful. KErrNone on success, otherwise
	        another of the system-wide error codes.

	@since  8.0
    */
	IMPORT_C TInt GetModelCount(TModelBankID aModelBankID,
					TInt& aModelCount);


	/**
	Checks if the rule is valid or not.

	@param  aGrammarID			
            The grammar ID.
	@param  aRuleID				
	        The rule ID.
	@param  aValid				
	        A reference where the validity of the rule is set.

	@return An error code indicating if the function call was successful. KErrNone on success, otherwise
	        another of the system-wide error codes.

	@since  8.0
    */
	IMPORT_C TInt GetRuleValidity(TGrammarID aGrammarID, TRuleID aRuleID, 
					TBool& aValid);

	/**
    *
    *	Returns the duration of the utterance for the specified model.
    * @since 8.0
    * @param	aModelBankID		model bank Id
	* @param	aModelID			model Id
	* @param	aDuration			reference where the duration of 
	*								utterance is set
    * @return	System-wide errors
    */
	IMPORT_C TInt GetUtteranceDuration(TModelBankID aModelBankID, 
				TModelID aModelID, TTimeIntervalMicroSeconds32& aDuration);

	/**
    Loads the specified grammar into the recognizer; done prior to recognition.

    @param  aGrammarID			
	        The grammar ID.

	@return An error code indicating if the function call was successful. KErrNone on success, otherwise
	        another of the system-wide error codes.

    @since  8.0
    */
	IMPORT_C TInt LoadGrammar(TGrammarID aGrammarID);

	/**
	Loads the specified lexicon into the recognizer; done prior to recognition.

	@param  aLexiconID			
	        The lexicon ID.

	@return An error code indicating if the function call was successful. KErrNone on success, otherwise
	        another of the system-wide error codes.

	@since  8.0
    */
	IMPORT_C TInt LoadLexicon(TLexiconID aLexiconID);

	/**
	Loads the specified model bank into the recognizer; done prior to recognition.

	@param  aModelBankID		
	        The model bank ID.

	@return An error code indicating if the function call was successful. KErrNone on success, otherwise
	        another of the system-wide error codes.

	@since  8.0
    */
	IMPORT_C TInt LoadModels(TModelBankID aModelBankID);

	/**
	Plays the previously trained utterance.

	@param  aModelBankID		
	        The model bank ID.
	@param  aModelID			
	        The model ID, whose utterance is played.

	@return An error code indicating if the function call was successful. KErrNone on success, otherwise
	        another of the system-wide error codes.

	@since  8.0
    */
	IMPORT_C TInt PlayUtterance(TModelBankID aModelBankID,
					TModelID aModelID);

	/**
	Initiates recognition; performed following loading of model bank, lexicon, and grammar.

	@param  aResultSet			
	        A reference where the recognition result is set.

    @return An error code indicating if the function call was successful. KErrNone on success, otherwise
            another of the system-wide error codes.

	@since  8.0
    */
	IMPORT_C TInt Recognize(CSDClientResultSet& aResultSet);

	/**
	Records the user utterance for training and recognition.

	@param  aRecordTime			
	        The recording time in microseconds.

	@return An error code indicating if the function call was successful. KErrNone on success, otherwise
	         another of the system-wide error codes.

	@since  8.0
    */
	IMPORT_C TInt Record(TTimeIntervalMicroSeconds32 aRecordTime);

	/**
	Removes the specified grammar from the permanent storage.

	Removing a grammar will remove all rules within the grammar.

	@param  aGrammarID			
	        The grammar ID.

	@return An error code indicating if the function call was successful. KErrNone on success, otherwise
	        another of the system-wide error codes.
	
	@since  8.0
    */
	IMPORT_C TInt RemoveGrammar(TGrammarID aGrammarID);

	/**
	Removes the specified lexicon from the permanent storage.

	Removing a lexicon will remove all pronunciations within the lexicon.

	@param  aLexiconID			
	        The lexicon ID.

	@return An error code indicating if the function call was successful. KErrNone on success, otherwise
	        another of the system-wide error codes.
	@since  8.0
    */
	IMPORT_C TInt RemoveLexicon(TLexiconID aLexiconID);

	/**
	Removes the specified model bank from the permanent storage.

	Removing a model bank will remove all models within the model bank.

	@param  aModelBankID		
	        The model bank ID.

	@return An error code indicating if the function call was successful. KErrNone on success, otherwise
	        another of the system-wide error codes.

	@since  8.0
    */
	IMPORT_C TInt RemoveModelBank(TModelBankID aModelBankID);

	/**
	Removes the specified model from the specified model bank permanently.

	@param  aModelBankID		
	        The model bank ID.
	@param  aModelID			
	        The model ID.

	@return An error code indicating if the function call was successful. KErrNone on success, otherwise
	        another of the system-wide error codes.

	@since  8.0
    */
	IMPORT_C TInt RemoveModel(TModelBankID aModelBankID,
								TModelID aModelID);

	/**
	Removes the specified pronunciation from the specified lexicon permanently.

	@param  aLexiconID
	        The lexicon ID.
	@param  aPronunciationID
	        The pronunciation ID.

	@return An error code indicating if the function call was successful. KErrNone on success, otherwise
	        another of the system-wide error codes.

	@since	8.0
    */
	IMPORT_C TInt RemovePronunciation(TLexiconID aLexiconID,
								TPronunciationID aPronunciationID);

	/**
	Removes the specified rule from the specified grammar permanently.

	@param  aGrammarID			
	        The grammar ID.
	@param  aRuleID				
	        The rule ID.

	@return An error code indicating if the function call was successful. KErrNone on success, otherwise
	        another of the system-wide error codes.

	@since  8.0
    */
	IMPORT_C TInt RemoveRule(TGrammarID aGrammarID, TRuleID aRuleID);

	/**
	Sets the client UID for client.

	@param  aClientUid			
	        The client UID.

	@return An error code indicating if the function call was successful. KErrNone on success, otherwise
	        another of the system-wide error codes.

	@since  8.0
	*/
	IMPORT_C TInt SetClientUid(TUid aClientUid);

	/**
	Starts a new recognition session.

	@param  aMode				
	        The recognition mode.

	@return An error code indicating if the function call was successful. KErrNone on success, otherwise
	        another of the system-wide error codes.

	@since  8.0
    */
	IMPORT_C TInt StartRecSession(TRecognitionMode aMode);

    /**
	Trains a new model into the specified model bank.

	@param  aModelBankID		
	        The model bank ID.
	@param  aModelID			
	        The reference to where a new model ID is assigned.

	@return An error code indicating if the function call was successful. KErrNone on success, otherwise
	        another of the system-wide error codes.

	@since  8.0
    */
	IMPORT_C TInt Train(TModelBankID aModelBankID, TModelID& aModelID);

	/**
	Unloads the specified rule from the specified grammar in temporary
	memory, previously loaded with LoadGrammarL(). The rule in the
	permanent storage remains intact.

	@param  aGrammarID
	        The grammar ID.
	@param  aRuleID
	        The rule ID.

	@return An error code indicating if the function call was successful. KErrNone on success, otherwise
	        another of the system-wide error codes.

	@since  8.0
    */
	IMPORT_C TInt UnloadRule(TGrammarID aGrammarID, TRuleID aRuleID);

	/**
	Uploads the specified parameters into the engine.

	@param  aParameterId		
	        An array of parameter IDs.
	@param  aParameterValue
	        An array of parameter values.

	@return An error code indicating if the function call was successful. KErrNone on success, otherwise
	        another of the system-wide error codes.

	@since  8.0
    */
	IMPORT_C TInt LoadEngineParameters(const RArray<TInt>& aParameterId,
								const RArray<TInt>& aParameterValue);

	/**
	Gets an array of pronunciation IDs previously requested by GetAllPronunciationIDs.

	@param  aPronunciationIDs
	        An array to store pronunciation IDs.

    @since  8.0
    */
	IMPORT_C void GetPronunciationIDArrayL(RArray<TPronunciationID>& aPronunciationIDs);

	/**
	Gets an array of rule IDs previously requested by GetAllRuleIDs.

	@param  aRuleIDs
	        An array to store rule IDs.

    @since  8.0
    */
	IMPORT_C void GetRuleIDArrayL(RArray<TRuleID>& aRuleIDs);

	/**
	Gets an array of model IDs previously requested by GetAllModelIDs.

	@param  aModelIDs
	        An array to store model IDs.

    @since  8.0
    */
	IMPORT_C void GetModelIDArrayL(RArray<TModelID>& aModelIDs);

	/**
	Gets an array of grammar IDs previously requested by GetAllGrammarIDs.

	@param  aGrammarIDs
	        An array to store grammar IDs.

    @since  8.0
    */
	IMPORT_C void GetGrammarIDArrayL(RArray<TGrammarID>& aGrammarIDs);

	/**
	Gets an array of lexicon IDs previously requested by GetAllLexiconIDs.

	@param  aLexiconIDs
	        An array to store lexicon IDs.

	@since  8.0
    */
	IMPORT_C void GetLexiconIDArrayL(RArray<TLexiconID>& aLexiconIDs);


	/**
	Gets an array of modelBank IDs previously requested by GetAllModelBankIDs.

	@param  aModelBankIDs
	        An array to store modelBank IDs.

    @since  8.0
    */
	IMPORT_C void GetModelBankIDArrayL(RArray<TModelBankID>& aModelBankIDs);


	/**
	Gets a client result set previously created by a call to Recognise().

	@param  aResultSet
	        The ResultSet used to contain the recognition results

    @since  8.0
    */
	IMPORT_C void GetResultSetL(CSDClientResultSet& aResultSet);
	
private:
	void DoGetEnginePropertiesL(const RArray<TInt>& aPropertyId, 
					RArray<TInt>& aPropertyValue);
	void DoLoadEngineParametersL(const RArray<TInt>& aParameterId, 
								const RArray<TInt>& aParameterValue);

					
	CBufFlat* ExternalizeIntArrayL(const RArray<TInt>& aArray);
	void InternalizeIntArrayL(TDes8& aDes, TInt aNumberElements, RArray<TInt>& aArray);
	
	void DoRecognizeL(CSDClientResultSet& aResultSet);
private:
	TAny* iReservedPtr_1;	// reserved for future expansion
	TAny* iReservedPtr_2;	// reserved for future expansion
};

#endif  // __SPEECHRECOGNITIONCUSTOMCOMMANDS_H__

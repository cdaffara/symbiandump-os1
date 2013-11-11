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

#ifndef __SPEECHRECOGNITIONUTILITY_H__
#define __SPEECHRECOGNITIONUTILITY_H__

// INCLUDES

#include <e32base.h>
#include <mmf/common/mmfcontroller.h>
#include <mmf/common/speechrecognitioncustomcommands.h>
#include <mmf/common/speechrecognitiondataclient.h>
#include <speechrecognitionutilityobserver.h>


// CLASS DECLARATION

/**
@publishedAll
@released

This is the main class of Speech Recognition Utility.

@since  8.0
*/
class CSpeechRecognitionUtility : public CBase
	{
	class CBody;
public:

	/**
	Factory function for creating this class.

	@param	aClientUid
	        The UID of the client.
	@param 	aSpeechRecognitionUtilityObserver
	        A reference to the event observer class.

	@return	A pointer to CSpeechRecognitionUtility object

	@since  8.0
	*/
	IMPORT_C static CSpeechRecognitionUtility* NewL(
							MSpeechRecognitionUtilityObserver&
							aSpeechRecognitionUtilityObserver, TUid aClientUid );

	/**
	Factory function for creating this class. Leaves a pointer on the cleanup stack.

	@param  aSpeechRecognitionUtilityObserver		
	        A reference to the event observer class.
	@param  aClientUid
	        A client UID for data ownership.

	@return A pointer to a CSpeechRecognitionUtility object.

	@since  8.0
	*/
	IMPORT_C static CSpeechRecognitionUtility* NewLC(
							MSpeechRecognitionUtilityObserver&
							aSpeechRecognitionUtilityObserver, TUid aClientUid );

	/**
	Destructor.

	@since  8.0
	*/
	IMPORT_C ~CSpeechRecognitionUtility();

	/**
	Returns the engine properties specified by the engine property
	ID array. On return aPropertyValue contains an array of engine
	properties.

	@param  aPropertyId		
	        A constant reference to an array of engine property ID's.
	@param  aPropertyValue
	        A reference to an array of engine property values.

	@return An error code indicating if the function call was successful. KErrNone on success, otherwise
	        another of the system-wide error codes.

	@since  8.0
	*/
	IMPORT_C TInt GetEngineProperties( const RArray<TInt>& aPropertyId,
		RArray<TInt>& aPropertyValue );

	/**
	Adds a new pronunciation for the given model into the
	specified lexicon.
	
	@param  aLexiconID			
	        The lexicon ID, where the new pronunciation is added to.
	@param  aModelBankID		
	        The model bank ID.
	@param	aModelID			
	        The model ID.
	@param  aPronunciationID	
	        A reference where the new pronunciation ID is assigned to.

	@return An error code indicating if the function call was successful. KErrNone on success, otherwise
	        another of the system-wide error codes.

	@since  8.0
	*/
	IMPORT_C TInt AddPronunciation( TLexiconID aLexiconID,
		TModelBankID aModelBankID, TModelID aModelID,
		TPronunciationID& aPronunciationID );

	/**
	Adds a new rule for the given pronunciation into the
	specified grammar.

	@param  aGrammarID			
	        The grammar ID, where the new rule is added to.
	@param  aLexiconID			
	        The lexicon ID.
	@param  aPronunciationID	
	        The pronunciation ID.
	@param  aRuleID				
	        A reference where the new rule ID is assigned to.

	@return An error code indicating if the function call was successful. KErrNone on success, otherwise
	        another of the system-wide error codes.

	@since  8.0
	*/
	IMPORT_C TInt AddRule( TGrammarID aGrammarID, TLexiconID aLexiconID,
		TPronunciationID aPronunciationID, TRuleID& aRuleID );

	/**
	Cancels the current operation.

	@since 8.0
	*/
	IMPORT_C void Cancel();

	/**
	Commit the current train operation to the database.

	@return An error code indicating if the function call was successful. KErrNone on success, otherwise
	        another of the system-wide error codes.

	@since  8.0
	*/
	IMPORT_C TInt CommitChanges();

	/**
	Creates a new grammar.

	@param  aGrammarID			
	        A reference where grammar ID is stored.

	@return An error code indicating if the function call was successful. KErrNone on success, otherwise
	        another of the system-wide error codes.

	@since  8.0
	*/
	IMPORT_C TInt CreateGrammar( TGrammarID& aGrammarID );

	/**
	Creates a new lexicon.

	@param  aLexiconID			
	        A reference where lexicon ID is stored.

	@return An error code indicating if the function call was successful. KErrNone on success, otherwise
	        another of the system-wide error codes.

	@since  8.0
	*/
	IMPORT_C TInt CreateLexicon( TLexiconID& aLexiconID );

	/**
	Loads the specified model bank into the recognizer;
	done prior to recognition.

	@param  aModelBankID
			The Model bank ID

	@return An error code indicating if the function call was successful. KErrNone on success, otherwise
	        another of the system-wide error codes.

	@since  8.0
	*/
	IMPORT_C TInt LoadModels( TModelBankID aModelBankID );

	/**
	Loads the specified grammar into the recognizer;
	done prior to recognition.
	
	@param  aGrammarID			
	        The grammar ID.

	@return An error code indicating if the function call was successful. KErrNone on success, otherwise
	        another of the system-wide error codes.

	@since  8.0
	*/
	IMPORT_C TInt LoadGrammar( TGrammarID aGrammarID );

	/**
	Loads the specified lexicon into the recognizer;
	done prior to recognition.

	@param  aLexiconID			
	        The lexicon ID.

	@return An error code indicating if the function call was successful. KErrNone on success, otherwise
	        another of the system-wide error codes.

	@since  8.0
	*/
	IMPORT_C TInt LoadLexicon( TLexiconID aLexiconID );

	/**
	Returns the duration of the utterance for the specified model.

	@param  aModelBankID		
	        The model bank ID.
	@param  aModelID			
	        The model ID.
	@param  aDuration			
	        The reference where the duration of utterance is set.

	@return An error code indicating if the function call was successful. KErrNone on success, otherwise
	        another of the system-wide error codes.

	@since  8.0
	*/
	IMPORT_C TInt GetUtteranceDuration( TModelBankID aModelBankID,
		TModelID aModelID,
		TTimeIntervalMicroSeconds32& aDuration);

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
	IMPORT_C TInt PlayUtterance( TModelBankID aModelBankID,
		TModelID aModelID);

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
	IMPORT_C TInt GetModelCount( TModelBankID aModelBankID, TInt& aModelCount );

	/**
	Ends the current recognition session. Resources allocated
	for recognition are freed.

	@return An error code indicating if the function call was successful. KErrNone on success, otherwise
	        another of the system-wide error codes.

	@since  8.0
	*/
	IMPORT_C TInt EndRecSession();

	/**
	Starts a new recognition session.

	@param  aMode				
	        The recognition mode.

	@return An error code indicating if the function call was successful. KErrNone on success, otherwise
	        another of the system-wide error codes.

	@since  8.0
	*/
	IMPORT_C TInt StartRecSession( TRecognitionMode aMode );

	/**
	Initiates recognition; performed following loading of
	model bank, lexicon, and grammar.

	@param  aResultSet			
	        A reference where the recognition result is set.

	@return An error code indicating if the function call was successful. KErrNone on success, otherwise
	        another of the system-wide error codes.

	@since  8.0
	*/
	IMPORT_C TInt Recognize( CSDClientResultSet& aResultSet );

	/**
	Records user utterance for training and recognition.

	@param  aRecordTime			
	        The recording time in microseconds.

	@return An error code indicating if the function call was successful. KErrNone on success, otherwise
	        another of the system-wide error codes.

	@since  8.0
	*/
	IMPORT_C TInt Record( TTimeIntervalMicroSeconds32 aRecordTime );

	/**
	Removes the specified grammar from the permanent storage. 

	Removing a grammar will remove all rules within the grammar.

	@param  aGrammarID			
	        The grammar ID.

	@return An error code indicating if the function call was successful. KErrNone on success, otherwise
	        another of the system-wide error codes.

	@since  8.0
	*/
	IMPORT_C TInt RemoveGrammar( TGrammarID aGrammarID );

	/**
	Removes the specified pronunciation from the specified lexicon permanently.

	@param  aLexiconID			
	        The lexicon ID.
	@param  aPronunciationID	
	        The pronunciation ID.

	@return An error code indicating if the function call was successful. KErrNone on success, otherwise
	        another of the system-wide error codes.

	@since  8.0
	*/
	IMPORT_C TInt RemovePronunciation( TLexiconID aLexiconID,
		TPronunciationID aPronunciationID );

	/**
	Removes the specified lexicon from the permanent storage.
	
	Removing a lexicon will remove all pronunciations within the lexicon.

	@param  aLexiconID			
	        The lexicon ID.

	@return An error code indicating if the function call was successful. KErrNone on success, otherwise
	        another of the system-wide error codes.

	@since  8.0
	*/
	IMPORT_C TInt RemoveLexicon( TLexiconID aLexiconID );

	/**
	Removes the specified model from the specified model bank
	permanently.

	@param  aModelBankID		
	        The model bank ID.
	@param  aModelID			
	        The model ID.

	@return An error code indicating if the function call was successful. KErrNone on success, otherwise
	        another of the system-wide error codes.

	@since  8.0
	*/
	IMPORT_C TInt RemoveModel( TModelBankID aModelBankID,
		TModelID aModelID );

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
	IMPORT_C TInt RemoveRule( TGrammarID aGrammarID, TRuleID aRuleID );

	/**
	Trains a new model into the specified model bank.

	@param  aModelBankID		
	        The model bank ID.
	@param  aModelID			
	        A reference where a new model ID is assigned to.

	@return An error code indicating if the function call was successful. KErrNone on success, otherwise
	        another of the system-wide error codes.

	@since  8.0
	*/
	IMPORT_C TInt Train( TModelBankID aModelBankID, TModelID& aModelID );

	/**
	Unloads the specified rule from the specified grammar in temporary memory, previously loaded 
	with LoadGrammarL.

	The rule in the permanent storage remains intact.

	@param  aGrammarID			
	        The grammar ID.
	@param  aRuleID				
	        The rule ID.

	@return An error code indicating if the function call was successful. KErrNone on success, otherwise
	        another of the system-wide error codes.

	@since  8.0
	*/
	IMPORT_C TInt UnloadRule( TGrammarID aGrammarID, TRuleID aRuleID );

	/**
	Sets the event handler for asynchronous events.

	@param  aSpeechRecognitionUtilityObserver		
	        A pointer to an observer.

	@since  8.0
	*/
	IMPORT_C void SetEventHandler( MSpeechRecognitionUtilityObserver*
		aSpeechRecognitionUtilityObserver );

	/**
	Returns all pronunciation IDs that exist in the specified lexicon.

	@param  aLexiconID
	        The lexicon ID.
	@param  aPronunciationIDs
	        A reference where pronunciation IDs are stored.

	@return An error code indicating if the function call was successful. KErrNone on success, otherwise
	        another of the system-wide error codes.

	@since  8.0
	*/
	IMPORT_C TInt GetAllPronunciationIDs( TLexiconID aLexiconID,
		RArray <TPronunciationID>& aPronunciationIDs );

	/**
	Returns all grammar IDs owned by the client.

	@param  aGrammarIDs			
	        A reference where rule IDs are stored.

	@return An error code indicating if the function call was successful. KErrNone on success, otherwise
	        another of the system-wide error codes.

	@since  8.0
	*/
	IMPORT_C TInt GetAllClientGrammarIDs(RArray <TGrammarID>& aGrammarIDs );

	/**
	Returns all lexicons owned by the client.

	@param  aLexiconIDs	
	        A reference where lexicon IDs are stored.

	@return An error code indicating if the function call was successful. KErrNone on success, otherwise
	        another of the system-wide error codes.

	@since  8.0
	*/
	IMPORT_C TInt GetAllClientLexiconIDs(RArray <TLexiconID>& aLexiconIDs );

	/**
	Returns all modelbank IDs owned by the client.

	@param  aModelBankIDs			
	        A reference where modelbank IDs are stored.

	@return An error code indicating if the function call was successful. KErrNone on success, otherwise
	        another of the system-wide error codes.

	@since  8.0
	*/
		IMPORT_C TInt GetAllClientModelBankIDs(
							RArray<TModelBankID>& aModelBankIDs );
	/**
	Returns all grammar IDs.

	@param  aGrammarIDs			
	        A reference where rule IDs are stored.

	@return An error code indicating if the function call was successful. KErrNone on success, otherwise
	        another of the system-wide error codes.

	@since  8.0
	*/
	IMPORT_C TInt GetAllGrammarIDs(RArray <TGrammarID>& aGrammarIDs );
	
	/**
	Returns all lexicons IDs.

	@param  aLexiconIDs	
	        A reference where lexicon IDs are stored.

	@return An error code indicating if the function call was successful. KErrNone on success, otherwise
	        another of the system-wide error codes.

	@since  8.0
	*/
	IMPORT_C TInt GetAllLexiconIDs(RArray <TLexiconID>& aLexiconIDs );

	/**
	Returns all modelbank IDs.

	@param  aModelBankIDs			
	        A reference where modelbank IDs are stored.

	@return An error code indicating if the function call was successful. KErrNone on success, otherwise
	        another of the system-wide error codes.

	@since  8.0
	*/
	IMPORT_C TInt GetAllModelBankIDs(
		RArray<TModelBankID>& aModelBankIDs );

	/**
	Returns all model IDs that exist in the specified model bank.

	@param  aModelBankID		
	        The model bank ID.
	@param  aModelIDs			
	        A reference where model IDs are stored.

	@return An error code indicating if the function call was successful. KErrNone on success, otherwise
	        another of the system-wide error codes.

	@since  8.0
	*/
	IMPORT_C TInt GetAllModelIDs( TModelBankID aModelBankID,
		RArray <TModelID>& aModelIDs );

	/**
	Checks if the rule is valid or not.

	@param  aGrammarID
	        The grammar ID.
	@param	aRuleID
			The rule ID.
	@param  aValid
	        A reference where the validity of the rule is set.

	@return An error code indicating if the function call was successful. KErrNone on success, otherwise
	        another of the system-wide error codes.

	@since  8.0
	*/
	IMPORT_C TInt GetRuleValidity( TGrammarID aGrammarID, TRuleID aRuleID,
		TBool& aValid );

	/**
	Returns all rule IDs that exist in the specified grammar.

	@param  aGrammarID			
	        The grammar ID.
	@param  aRuleIDs			
	        A reference where rule IDs are stored.

	@return An error code indicating if the function call was successful. KErrNone on success, otherwise
	        another of the system-wide error codes.

	@since  8.0
	*/
	IMPORT_C TInt GetAllRuleIDs( TGrammarID aGrammarID,
		RArray <TRuleID>& aRuleIDs );

	/**
	Creates a new model bank.

	@param  aModelBankID		
	        A reference where a new model bank IF is assigned to.

	@return An error code indicating if the function call was successful. KErrNone on success, otherwise
	        another of the system-wide error codes.

	@since  8.0
	*/
	IMPORT_C TInt CreateModelBank( TModelBankID& aModelBankID );

	/**
	Removes the specified model bank from the permanent storage.

	Removing a model bank will remove all models within the model bank.

	@param  aModelBankID		
	        The model bank ID.

	@return An error code indicating if the function call was successful. KErrNone on success, otherwise
	        another of the system-wide error codes.

	@since	8.0
	*/
	IMPORT_C TInt RemoveModelBank( TModelBankID aModelBankID );

	/**
	Returns the number of models available for training system model storage, based on available 
	disk space.

	@param  aAvailableStorage	
	        The number of models that can be contained in available storage.

	@return An error code indicating if the function call was successful. KErrNone on success, otherwise
	        another of the system-wide error codes.

	@since  8.0
	*/
	IMPORT_C TInt GetAvailableStorage( TInt& aAvailableStorage );

	/**
	Loads the specified parameters into the recognition engine.

	@param  aParameterId		
	        The array of parameter IDs.
	@param  aParameterValue		
	        The array of parameter values.

	@return An error code indicating if the function call was successful. KErrNone on success, otherwise
	        another of the system-wide error codes.

	@since  8.0
	*/
	IMPORT_C TInt LoadEngineParameters( const RArray<TInt>& aParameterId,
					const RArray<TInt>& aParameterValue );

	/**
	Sets the audio priority and preference for train, playback and recognition.

	@param  aPriority				
	        The priority for voice recognition.
	@param  aTrainPreference		
	        The preference for training.
	@param  aPlaybackPreference		
	        The preference for playback.
	@param  aRecognitionPreference	
	        The preference for recognition.

	@return An error code indicating if the function call was successful. KErrNone on success, otherwise
	        another of the system-wide error codes.

	@since  8.0

	*/
	IMPORT_C TInt SetAudioPriority( TInt aPriority, TInt aTrainPreference,
		TInt aPlaybackPreference, TInt aRecognitionPreference );

private:
	/**
	Private C++ constructor for this class.

	@since  8.0
	*/
	CSpeechRecognitionUtility();

	/**
	Second phase constructor for this class.

	@param  aSpeechRecognitionUtilityObserver		
	        A reference to the event observer class.

	@since  8.0
	*/
	void ConstructL(MSpeechRecognitionUtilityObserver& aSpeechRecognitionUtilityObserver);

private:
	CBody* iBody;
	friend class CBody;
};

#endif	// of __SPEECHRECOGNITIONUTILITY_H__

	// End of File

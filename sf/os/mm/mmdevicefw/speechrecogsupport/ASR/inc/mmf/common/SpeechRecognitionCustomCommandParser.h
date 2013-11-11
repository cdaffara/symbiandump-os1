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

#ifndef __SPEECHRECOGNITIONCUSTOMCOMMANDPARSER_H__
#define __SPEECHRECOGNITIONCUSTOMCOMMANDPARSER_H__

// INCLUDE FILES
#include <mmf/common/mmfcontroller.h>
#include <mmf/common/speechrecognitioncustomcommandimplementor.h>
#include <mmf/common/speechrecognitiondataclient.h>

// CLASS DECLARATION

/**
@publishedAll
@released

This is the main class of Speech Recognition Custom Commands Parser.

@since  8.0
*/
class CSpeechRecognitionCustomCommandParser : public CMMFCustomCommandParserBase
	{
public:

	/**
	Factory function for creating this class.

	@param  aImplementor
	        A reference to a custom commands implementor.

	@return	A pointer to a CSpeechRecognitionCustomCommandParser object.

    @since  8.0
	*/
	IMPORT_C static CSpeechRecognitionCustomCommandParser* NewL(
						MSpeechRecognitionCustomCommandImplementor&
						aImplementor);

	/**
	Destructor.

	@since  8.0
	*/
	IMPORT_C  ~CSpeechRecognitionCustomCommandParser();

	/**
    @internalTechnology

	Handles the request from the client.

	@param  aMessage			
	        The message containing the information of the request.

	@since  8.0
	*/
	void HandleRequest(TMMFMessage& aMessage);

	private:
	/**
	C++ constructor for a CSpeechRecognitionCustomCommandParser class.

	@param  aImplementor
	        A reference to a custom commands implementor.

	@since  8.0
	*/
	CSpeechRecognitionCustomCommandParser(
		MSpeechRecognitionCustomCommandImplementor& aImplementor);

	/**
	Private method to handle the request from the client.

	@param  aMessage			
	        The message containing the information of the request.

	@since  8.0
	*/
	void  DoHandleRequestL(TMMFMessage& aMessage);

	/**
	Adds a new pronunciation for the given model into the specified lexicon.

	@param  aMessage			
	        The message containing the information of the request.

	@since  8.0
	*/
	void DoAddPronunciationL(TMMFMessage& aMessage);

	/**
	Adds a new rule for the given pronunciation into the specified grammar.

	@param  aMessage			
	        The message containing the information of the request.

	@since  8.0
	*/
	void DoAddRuleL(TMMFMessage& aMessage);

	/**
	Cancels the current operation.

	@since  8.0
	*/
	void DoCancel();

	/**
	Commits uncommitted database operations to the database.

	@since  8.0
	*/
	void DoCommitChangesL();

	/**
	Creates a new grammar.

	@param  aMessage
	        A message containing the information of the request.

	@since  8.0
	*/
	void DoCreateGrammarL(TMMFMessage& aMessage);

	/**
	Creates a new lexicon.

	@param  aMessage
	        The message containing the information of the request.

	@since  8.0
	*/
	void DoCreateLexiconL(TMMFMessage& aMessage);

	/**
	Creates a new model bank.

	@param  aMessage
	        The message containing the information of the request.

	@since  8.0
	*/
	void DoCreateModelBankL(TMMFMessage& aMessage);

	/**
	Ends the current recognition session. Resources allocated for recognition are freed.

	@since	8.0
	*/
	void DoEndRecSessionL();

	/**
	Returns all grammar IDs that belong to the current client, whose UID was set with
	SetClientUid().

	@since  8.0
	*/
	void DoGetAllClientGrammarIDsL();

	/**
	Returns all lexicon IDs that belong to the current client, whose UID was set with
	SetClientUid().

	@since  8.0
	*/
	void DoGetAllClientLexiconIDsL();

	/**
	Returns all model bank IDs that belong to the current client, whose UID was set with
	SetClientUid().

	@since  8.0
	*/
	void DoGetAllClientModelBankIDsL();

	/**
	Returns all grammar IDs that exist (for all clients).

	@return An error code indicating if the function call was successful. KErrNone on success, otherwise
	        another of the system-wide error codes.

	@since  8.0
    */
	void DoGetAllGrammarIDsL();

	/**
	Returns all lexicon IDs that exist (for all clients).

	@since  8.0
    */
	void DoGetAllLexiconIDsL();

	/**
	Returns all model bank IDs that exist (for all clients).

	@since 8.0
    */
	void DoGetAllModelBankIDsL();

	/**
	Returns all model IDs that exist in the specified model bank.

	@param  aMessage				
	        The message containing the information of the request.

	@since  8.0
	*/
	void DoGetAllModelIDsL(TMMFMessage& aMessage);

	/**
	Returns all pronunciation IDs that exist in the specified lexicon.

	@param  aMessage				
	        The message containing the information of the request.

	@since  8.0
	*/
	void DoGetAllPronunciationIDsL(TMMFMessage& aMessage);

	/**
	Returns all rule IDs that exist in the specified grammar.

	@param  aMessage				
	        The message containing the information of the request.

	@since  8.0
	*/
	void DoGetAllRuleIDsL(TMMFMessage& aMessage);

	/**
	Returns the number of models available for training system wide, based on available disk space.

	@param  aMessage				
	        The message containing the information of the request.

	@since  8.0
	*/
	void DoGetAvailableStorageL(TMMFMessage& aMessage);

	/**
	Returns the engine properties.

	@param  aMessage				
	        The message containing the information of the request.

	@since  8.0
	*/
	void DoGetEnginePropertiesL(TMMFMessage& aMessage);

	/**
	Returns the number of models in the specified model bank.

	@param  aMessage				
	        The message containing the information of the request.

	@since  8.0
	*/
	void DoGetModelCountL(TMMFMessage& aMessage);

	/**
	Checks if the rule is valid or not.

	@param  aMessage				
	        The message containing the information of the request.

	@since  8.0
	*/
	void DoGetRuleValidityL(TMMFMessage& aMessage);

	/**
	Returns the duration of the utterance for the specified model.

	@param  aMessage				
	        The message containing the information of the request.

	@since  8.0
	*/
	void DoGetUtteranceDurationL(TMMFMessage& aMessage);

	/**
	Loads the specified grammar into the recognizer; done prior to recognition.

	@param  aMessage				
	        The message containing the information of the request.

	@since  8.0
	*/
	void DoLoadGrammarL(TMMFMessage& aMessage);

	/**
	Loads the specified lexicon into the recognizer; done prior to recognition.

	@param  aMessage				
	        The message containing the information of the request.

	@since  8.0
	*/
	void DoLoadLexiconL(TMMFMessage& aMessage);

	/**
	Loads the specified model bank into the recognizer; done prior to recognition.

	@param  aMessage				
	        The message containing the information of the request.

	@since  8.0
	*/
	void DoLoadModelsL(TMMFMessage& aMessage);

	/**
	Plays the previously trained utterance.

	@param  aMessage				
	        The message containing the information of the request.

	@since  8.0
	*/
	void DoPlayUtteranceL(TMMFMessage& aMessage);

	/**
	Initiates recognition; performed following loading of model bank, lexicon, and grammar.

	@param  aMessage				
	        The message containing the information of the request.

	@since  8.0
	*/
	void DoRecognizeL(TMMFMessage& aMessage);

	/**
	Records user utterance for training and recognition.

	@param  aMessage				
	        The message containing the information of the request.

	@since  8.0
	*/
	void DoRecordL(TMMFMessage& aMessage);

	/**
	Removes the specified grammar from the permanent storage.

	Removing a grammar will remove all rules within the grammar.

	@param  aMessage				
	        The message containing the information of the request.

	@since  8.0
	*/
	void DoRemoveGrammarL(TMMFMessage& aMessage);

	/**
	Removes the specified lexicon from the permanent storage.

	Removing a lexicon will remove all pronunciations within the lexicon.

	@param  aMessage				
	        The message containing the information of the request.

	@since  8.0
	*/
	void DoRemoveLexiconL(TMMFMessage& aMessage);

	/**
	Removes the specified model bank from the permanent storage.

	Removing a model bank will remove all models within the model bank.

	@param  aMessage				
	        The message containing the information of the request.

	@since  8.0
	*/
	void DoRemoveModelBankL(TMMFMessage& aMessage);

	/**
	Removes the specified model from the specified model bank permanently.

	@param  aMessage
	        The message containing the information of the request.

	@since  8.0
	*/
	void DoRemoveModelL(TMMFMessage& aMessage);

	/**
	Removes the specified pronunciation from the specified lexicon permanently.

	@param  aMessage				
	        The message containing the information of the request.

	@since  8.0
	*/
	void DoRemovePronunciationL(TMMFMessage& aMessage);

	/**
	Removes the specified rule from the specified grammar permanently.

	@param  aMessage				
	        The message containing the information of the request.

	@since  8.0
	*/
	void DoRemoveRuleL(TMMFMessage& aMessage);

	/**
	Sets the UID of the client.

	@param  aMessage				
	        The message containing the information of the request.

	@since  8.0
	*/
	void DoSetClientUidL(TMMFMessage& aMessage);

	/**
	Starts a new recognition session.

	@param  aMessage				
	        The message containing the information of the request.

	@since  8.0
	*/
	void DoStartRecSessionL(TMMFMessage& aMessage);

	/**
	Trains a new model into the specified model bank.

	@param  aMessage				
	        The message containing the information of the request.

	@since  8.0
	*/
	void DoTrainL(TMMFMessage& aMessage);

	/**
	Unloads the specified rule from the specified grammar in temporary
	memory, previously loaded with LoadGrammarL().

	The rule in the permanent storage remains intact.

	@param  aMessage
	        The message containing the information of the request.

	@since  8.0
	*/
	void DoUnloadRuleL(TMMFMessage& aMessage);

	/**
	Loads the specified parameters into the engine.

	@param  aMessage				
	        The message containing the information of the request.

	@since  8.0
	*/
	void DoLoadEngineParametersL(TMMFMessage& aMessage);

	/**
	Copies the pronunciation ID array stored by a previous GetAllPronunciationIDs call.

	@param  aMessage
	        The message containing the information of the request.

	@since  8.0
	*/
	void DoCopyPronunciationIDArrayL(TMMFMessage& aMessage);

	/**
	Copy the model ID array stored by a previous GetAllModelIDs call.

	@param  aMessage				
	        The message containing the information of the request.

	@since  8.0
	*/
	void DoCopyModelIDArrayL(TMMFMessage& aMessage);

	/**
	Copies the rule ID array stored by a previous GetAllRuleIDs call.

	@param  aMessage				
	        The message containing the information of the request.

	@since  8.0
	*/
	void DoCopyRuleIDArrayL(TMMFMessage& aMessage);
	/**
	Copies the grammar ID array stored by a previous GetAllGrammarIDs call.

	@param  aMessage				
	        The message containing the information of the request.

	@since  8.0
	*/
	void DoCopyGrammarIDArrayL(TMMFMessage& aMessage);

	/**
	Copies the lexicon ID array stored by a previous GetAllLexiconIDs call.

	@param  aMessage				
	        The message containing the information of the request.

	@since  8.0
	*/
	void DoCopyLexiconIDArrayL(TMMFMessage& aMessage);

	/**
	Copies the modelBank ID array stored by a previous GetAllModelBankIDs call.

	@param  aMessage
	        The message containing the information of the request.

	@since  8.0
	*/
	void DoCopyModelBankIDArrayL(TMMFMessage& aMessage);

	/**
	Externalises the result set into a descriptor, so that it can be copied into an array.
	*/
	void DoExternalizeResultSetL();

	/**
	Copies the previously externalised result set into a message. The message must be the same size as
	the stored externalized result set.
	*/
	void DoCopyResultSetL(TMMFMessage& aMessage);


	/**
	Extracts an array from first parameter of TMMFMessage.

	@param  aMessage
	        The message to copy the array from.
	@param  aArray
	        The array to copy into.

	@since  8.0
	*/
	void DoExtractIntArrayFromData1L(TMMFMessage& aMessage, RArray<TInt>& aArray);

	/**
	Extracts an array from second parameter of TMMFMessage.

	@param  aMessage				
	        The message to copy array from.
	@param  aArray					
	        The array to copy into.

	@since  8.0
	*/
	void DoExtractIntArrayFromData2L(TMMFMessage& aMessage, RArray<TInt>& aArray);

	/**
	Extracts an integer array from the descriptor.

	@param  aDescriptor				
	        The descriptor containing an integer.
	@param  aArray
	        The array to copy into.

	@since  8.0
	*/
	void DoExtractIntArrayL(TDes8& aDescriptor, RArray<TInt>& aArray);



	/**
	Copies the specified integer array given into a TMMFMessage.

	@param  aMessage
	        The message to copy the array into.
	@param  aArray
	        The array to copy from.

	@since  8.0
	*/
	void DoCopyIntArrayL(TMMFMessage& aMessage, const RArray<TInt>& aArray);


private:
	// reference to the Custom Command Implementor class
	MSpeechRecognitionCustomCommandImplementor& iImplementor; 

	RArray<TPronunciationID> iPronunciationIDs;
	RArray<TModelID> iModelIDs;
	RArray<TRuleID> iRuleIDs;
	
	RArray<TGrammarID> iGrammarIDs;
	RArray<TLexiconID> iLexiconIDs;
	RArray<TModelBankID> iModelBankIDs;


	CSDClientResultSet* iResultSet;
	CBufFlat* iResultCopyBuffer;

//  Remove?
//	TAny* iReservedPtr_1;	// reserved for future expansion
//	TAny* iReservedPtr_2;	// reserved for future expansion
	};
#endif  // of __SPEECHRECOGNITIONCUSTOMCOMMANDPARSER_H__

// End of file

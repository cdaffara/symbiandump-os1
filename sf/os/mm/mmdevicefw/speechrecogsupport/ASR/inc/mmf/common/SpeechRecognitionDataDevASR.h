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

#ifndef __SPEECHRECOGNITIONDATADEVASR_H__
#define __SPEECHRECOGNITIONDATADEVASR_H__

//  INCLUDES
#include <mmf/common/speechrecognitiondatacommon.h>


// FORWARD DECLARATION
class TDiagnostic;


// CLASS DECLARATION

/**
@publishedAll
@released

Class to encapsulate a Speaker Dependent Rule.

@since 8.0
*/
class CSDRule : public CBase
{
public:// Constructors and destructor

	/**
    Destructor.
    */
	IMPORT_C ~CSDRule();

    /**
    Instantiates an instance of CSDRule.
	
	@param  aRuleID
			The rule identifier.
	@param  aLexiconID
	        The lexicon identifier.
	@param  aPronunciationID
	        The pronunciation identifier.

	@return A pointer to a CSDRule object.
    */
	IMPORT_C static CSDRule* NewL(TRuleID aRuleID, TLexiconID aLexiconID, TPronunciationID aPronunciationID);

	/**
    Instantiates an instance of CSDRule and leaves a pointer on the cleanup stack.

	@param  aRuleID
	        The rule identifier.
	@param  aLexiconID
	        The lexicon identifier.
	@param  aPronunciationID
	        The pronunciation identifier.

	@return A pointer to a CSDRule object.
	*/
	IMPORT_C static CSDRule* NewLC(TRuleID aRuleID, TLexiconID aLexiconID, TPronunciationID aPronunciationID);


	/**
	Returns the rule ID of the result.

	@return The rule identifier.

	@since 8.0
	*/
	IMPORT_C TRuleID RuleID() const;

	/**
	Returns the lexicon ID of the result.

	@return The lexicon identifier.

    @since 8.0
	*/
	IMPORT_C TLexiconID LexiconID() const;

	/**
	Returns the pronunciation ID of the result.

	@return The pronunciation identifier.

	@since 8.0
	*/
	IMPORT_C TPronunciationID PronunciationID() const;

private:
	void ConstructL();
	CSDRule(TRuleID aRuleID, TLexiconID aLexiconID, TPronunciationID aPronunciationID);

private:
	TRuleID iRuleID;
	TLexiconID iLexiconID;
	TPronunciationID iPronunciationID;
};


/**
@publishedAll
@released

Class to encapsulate a Speaker Dependent Grammar.

A grammar consists of a list of rules.

@since  8.0
*/
class CSDGrammar : public CBase
{
public:// Constructors and destructor

	/**
	Destructor.
	*/
	IMPORT_C ~CSDGrammar();

    /**
    Instantiates an instance of CSDGrammar.

	@param  aGrammarID
	        The grammar identifier.

	@return A pointer to a CSDGrammar object.
	*/
	IMPORT_C static CSDGrammar* NewL(TGrammarID aGrammarID);

	/**
	Instantiates an instance of CSDGrammar and leaves a pointer on the cleanup stack.

	@param  aGrammarID
	        The grammar identifier.

	@return A pointer to CSDGrammar object.
	*/
	IMPORT_C static CSDGrammar* NewLC(TGrammarID aGrammarID);

	/**
	Returns the grammar ID of the result.

	@return The grammar identifier.

	@since  8.0
	*/
	IMPORT_C TGrammarID GrammarID() const;

	/**
    Adds a rule to the grammar.

	@param  aRule
	        A reference to a CSDRule object.

	@return An error code indicating if the function call was successful. KErrNone on success, otherwise
	        another of the system-wide error codes.

	@since 8.0
	*/
	IMPORT_C TInt Add(CSDRule& aRule);

	/**
	Deletes a rule from the grammar.

	@param  aRuleID
	        The rule identifier to delete.

	@since  8.0
    */
	IMPORT_C void Delete(TRuleID aRuleID);

	/**
	Returns a rule based at an index.

	@param  aIndex
	        An index value. The value must not be negative and must not be greater than the number 
	        of rules currently in the grammar, otherwise the function will panic.

	@return A reference to a CSDRule object.

	@since  8.0
	*/
	IMPORT_C const CSDRule& At(TInt aIndex) const;

	/**
	Finds a rule in the grammar based on the rule ID.

	@param  aRuleID
	        The rule identifier.

	@return The index of the first matching rule within the grammar. Returns KErrNotFound, if a 
	        matching rule can not be found.

	@since  8.0
	*/
	IMPORT_C TInt FindRule(TRuleID aRuleID) const;

	/**
	Returns the number of rules in the grammar

	@return The number of rules.

	@since  8.0
	*/
	IMPORT_C TInt Count() const;

private:
	void ConstructL();
	CSDGrammar(TGrammarID aGrammarID);

private:
	TGrammarID iGrammarID;
	RPointerArray<CSDRule> iRuleArray;
};


/**
@publishedAll
@released

Class to encapsulate a Speaker Dependent Pronunciation.

@since  8.0
*/
class CSDPronunciation : public CBase
{
public:// Constructors and destructor

	/**
	Destructor.
	*/
	IMPORT_C ~CSDPronunciation();

	/**
    Instantiates an instance of CSDPronunciation.

	@param  aPronunciationID
	        The pronunciation identifier.
	@param  aModelBankID
	        The model bank identifier.
	@param  aPhonemeID
	        The phoneme identifier.

	@return A pointer to a CSDPronunciation object.
	*/
	IMPORT_C static CSDPronunciation* NewL(TPronunciationID aPronunciationID, TModelBankID aModelBankID, TPhonemeID aPhonemeID);

	/**
	Instantiates an instance of CSDPronunciation and leaves a pointer on the cleanup stack.

	@param  aPronunciationID
	        The pronunciation identifier.
	@param  aModelBankID
	        The model bank identifier.
	@param  aPhonemeID
	        The phoneme identifier.

	@return A pointer to a CSDPronunciation object.
	*/
	IMPORT_C static CSDPronunciation* NewLC(TPronunciationID aPronunciationID, TModelBankID aModelBankID, TPhonemeID aPhonemeID);

	/**
	Returns the pronunciation ID of this lexicon entry.

	@return The pronunciation identifier.

	@since  8.0
	*/
	IMPORT_C TPronunciationID PronunciationID() const;

	/**
	Returns the ID of the model bank.

	@return The model bank identifier.

	@since  8.0
	*/
	IMPORT_C TModelBankID ModelBankID() const;

	/**
	Returns the phoneme ID of this lexicon entry.

	@return The phoneme identifier.

	@since 8.0
	*/
	IMPORT_C TPhonemeID PhonemeID() const;

private:
	void ConstructL();
	CSDPronunciation(TPronunciationID aPronunciationID, TModelBankID aModelBankID, TPhonemeID aPhonemeID);

private:
	TPronunciationID iPronunciationID;
	TModelBankID iModelBankID;
	TPhonemeID iPhonemeID;
};


/**
@publishedAll
@released

Class to encapsulate a Speaker Dependent Lexicon.

@since 8.0
*/
class CSDLexicon : public CBase
{
public:// Constructors and destructor

	/**
    Destructor.
	*/
	IMPORT_C ~CSDLexicon();

	/**
	Instantiates an instance of CSDLexicon.

	@param  aLexiconID
	        The lexicon identifier.

	@return A pointer to a CSDLexicon object.
    */
	IMPORT_C static CSDLexicon* NewL(TLexiconID aLexiconID);

	/**
    Instantiates an instance of CSDGrammar and leaves a pointer on the cleanup stack.

	@param  aLexiconID
	        The lexicon identifier.

	@return A pointer to a CSDLexicon object.
	*/
	IMPORT_C static CSDLexicon* NewLC(TLexiconID aLexiconID);

	/**
	Returns the ID of the lexicon.

	@return The lexicon identifier.

	@since 8.0
	*/
	IMPORT_C TLexiconID LexiconID() const;

	/**
	Adds a lexicon entry to the lexicon.

	@param aPronunciation
	       A reference to a CSDPronunciation object.

	@return An error code indicating if the function call was successful. KErrNone on success, otherwise
	        another of the system-wide error codes.

	@since  8.0
	*/
	IMPORT_C TInt Add(CSDPronunciation& aPronunciation);

	/**
	Returns a pronunciation based at an index.

	@param  aIndex
	        The pronunciation index. This value must not be negative and must not be greater than 
	        the number of pronunciations currently in the lexicon, otherwise the function will 
	        panic.

	@return A reference to a CSDPronunciation object.

	@since  8.0
	*/
	IMPORT_C const CSDPronunciation& At(TInt aIndex) const;

	/**
	Finds a pronunciation based on the specified pronunciation ID.

	@param  aPronunciationID
	        The pronunciation identifier.

	@return The index of the first matching pronunciation within the lexicon. Returns KErrNotFound, 
	        if a matching pronunciation can not be found.
    */
	IMPORT_C TInt FindPronunciation(TPronunciationID aPronunciationID) const;

	/**
    Returns the number of lexicon entries in the lexicon.

	@return The number of pronunciations.

	@since  8.0
	*/
	IMPORT_C TInt Count() const;

private:
	void ConstructL();
	CSDLexicon(TLexiconID aLexiconID);

private:
	TLexiconID iLexiconID;
	RPointerArray<CSDPronunciation> iPronunciationArray;
};


/**
@publishedAll
@released

Class to encapsulate a Speaker Dependent Model created during the training process.

@since  8.0
*/
class CSDModel : public CBase
{
public:// Constructors and destructor

	/**
    Destructor.
	*/
	IMPORT_C ~CSDModel();

	/**
    Instantiates an instance of a CSDModel.

	@return A pointer to a CSDModel object.
	*/
	IMPORT_C static CSDModel* NewL();

	/**
    Instantiates an instance of a CSDModel.

	@return A pointer to a CSDModel object.
	*/
	IMPORT_C static CSDModel* NewLC();

	/**
    Instantiates an instance of a CSDModel.

	@param  aPhonemeID
	        The phoneme identifier.

	@return A pointer to a CSDModel object.
	*/
	IMPORT_C static CSDModel* NewL(TPhonemeID aPhonemeID);

	/**
    Instantiates an instance of a CSDModel.

	@param  aPhonemeID
	        The phoneme identifier.

	@return A pointer to a CSDModel object.
	*/
	IMPORT_C static CSDModel* NewLC(TPhonemeID aPhonemeID);

	/**
	Sets the phoneme ID of this model.

	@param aPhonemeID
	       The phoneme identifier.

	@since  8.0
	*/
	IMPORT_C void SetPhonemeID(TPhonemeID aPhonemeID);

	/**
    Returns the phoneme ID of this model.

	@return The phoneme identifier.

	@since  8.0
	*/
	IMPORT_C TPhonemeID PhonemeID() const;

	/**
	Sets the acoustic model of the model.

	@param  aAcousticModel
	        A pointer descriptor containing the acoustic model (binary).

    @since  8.0
	*/
	IMPORT_C void SetAcousticModel(TPtr8& aAcousticModel);

	/**
    Gets a pointer to the acoustic model of this model.

	@return A descriptor to the acoustic model data.

	@since  8.0
	*/
	IMPORT_C const TDesC8& AcousticModel() const;

private:
	void ConstructL();
	CSDModel();
	CSDModel(TPhonemeID aPhonemeID);

private:
	TPhonemeID iPhonemeID;
	TPtr8 iAcousticModelPtr;
};


/**
@publishedAll
@released

Class to encapsulate a Speaker Dependent Model Bank used by the recognizer during the
recognition process.

@since 8.0
*/
class CSDModelBank : public CBase
{
public:// Constructors and destructor

	/**
    Destructor.
	*/
	IMPORT_C ~CSDModelBank();

	/**
	Instantiates an instance of a CSDModelBank.

	@param  aModelBankID
	        The model bank identifier.

	@return A pointer to a CSDModelBank object.
	*/
	IMPORT_C static CSDModelBank* NewL(TModelBankID aModelBankID);

	/**
	Instantiates an instance of a CSDModelBank and leaves a pointer on the cleanup stack.

	@param  aModelBankID
	        The model bank identifier.

	@return A pointer to a CSDModelBank object.
	*/
	IMPORT_C static CSDModelBank* NewLC(TModelBankID aModelBankID);

    /**
	Returns the model bank ID of the model bank.

	@return The model bank identifier.

	@since 8.0
	*/
	IMPORT_C TModelBankID ModelBankID() const;

	/**
	Adds a model to the model bank.

	@param aModel
	       The reference to CSDModel object.

	@return An error code indicating if the function call was successful. KErrNone on success, otherwise
	        another of the system-wide error codes.

	@since 8.0
	*/
	IMPORT_C TInt Add(CSDModel& aModel);

	/**
    Returns a model at the given index.

	@param  aIndex
	        A reference to a CSDModel object.

    @return The index of the first matching model within the model bank. Returns KErrNotFound, if a
	        matching model can not be found.

    @since  8.0
    */
	IMPORT_C const CSDModel& At(TInt aIndex) const;

	/**
	Finds a model based on the specified phoneme ID.

	@param  aPhonemeID
	        The phoneme ID.

	@return The index of the first matching model within the model bank. Returns KErrNotFound, if a
	        matching model can not be found.

	@since  8.0
	*/
	IMPORT_C TInt FindModel(TPhonemeID aPhonemeID) const;

	/**
	Returns the number of models in the model bank.

	@return The number of models in the model bank.

	@since  8.0
	*/
	IMPORT_C TInt Count() const;

private:
	void ConstructL();
	CSDModelBank(TModelBankID aModelBankID);

private:
	TModelBankID iModelBankID;
	RPointerArray<CSDModel> iModelArray;
};


/**
@publishedAll
@released

Class to encapsulate a Recognition Result

@since 8.0
*/
class CSDResult : public CBase
{
public:// Constructors and destructor

	/**
	Destructor.
	*/
	IMPORT_C ~CSDResult();

	/**
	Instantiates an instance of a CSDResult.

	@return A pointer to a CSDResult object.
	*/
	IMPORT_C static CSDResult* NewL();

	/**
	Instantiates an instance of CSDResult and leaves a pointer on the cleanup stack.

	@return A pointer to a CSDResult object.
	*/
	IMPORT_C static CSDResult* NewLC();

	/**
	Instantiates an instance of a CSDResult.

	@param  aGrammarID
	        The grammar identifier.
	@param  aRuleID
	        The rule identifier.

	@return A pointer to a CSDResult object.
	*/
	IMPORT_C static CSDResult* NewL(TGrammarID aGrammarID, TRuleID aRuleID);

	/**
	Instantiates an instance of a CSDResult and leaves a pointer on the cleanup stack.

	@param  aGrammarID
	        The grammar identifier.
	@param  aRuleID
	        The rule identifier.

	@return A pointer to a CSDResult object.
	*/
	IMPORT_C static CSDResult* NewLC(TGrammarID aGrammarID, TRuleID aRuleID);

	/**
	Sets the grammar ID on the result.

	@param  aGrammarID
	        The grammar identifier.

	@since  8.0
	*/
	IMPORT_C void SetGrammarID(TGrammarID aGrammarID);

	/**
    Returns the grammar ID of the result.

	@return The grammar identifier.

	@since 8.0
	*/
	IMPORT_C TGrammarID GrammarID() const;

	/**
    Sets the rule ID of the result.

	@param  aRuleID
	        The rule identifier.

	@since  8.0
	*/
	IMPORT_C void SetRuleID(TRuleID aRuleID);

	/**
    Returns the rule ID of the result.

	@return The rule identifier.

	@since  8.0
	*/
	IMPORT_C TRuleID RuleID() const;

	/**
    Sets the score for this result.

	@param aScore
	       The score of this result.

	@since 8.0
	*/
	IMPORT_C void SetScore(const TInt32 aScore);

	/**
	Returns the score of this result.

	@return The score of this result.

	@since 8.0
	*/
	IMPORT_C TInt32 Score() const;

private:
	void ConstructL();
	CSDResult();
	CSDResult(TGrammarID aGrammarID, TRuleID aRuleID);

private:
	TGrammarID iGrammarID;
	TRuleID iRuleID;
	TInt32 iScore;
};


/**
@publishedAll
@released

CSDResultSet encapsulates a set of result objects returned by the recognizer at the end of the
recognition process. A result set may include additional diagnostic information for error analysis.

@since  8.0
*/
class CSDResultSet : public CBase
{
public:// Constructors and destructor

	/**
    Destructor.
	*/
	IMPORT_C ~CSDResultSet();

	/**
	Instantiates an instance of CSDResultSet.

	@return A pointer to a CSDResultSet object.
	*/
	IMPORT_C static CSDResultSet* NewL();

	/**
	Instantiates an instance of CSDResultSet and leaves a pointer on the cleanup stack.

	@return A pointer to a CSDResultSet object.
	*/
	IMPORT_C static CSDResultSet* NewLC();

	/**
	Adds a result to the rule set.

	@param  aResult
	        A reference to the result object to be added.

	@return An error code indicating if the function call was successful. KErrNone on success, otherwise
	        another of the system-wide error codes.

	@since  8.0
	*/
	IMPORT_C TInt Add(CSDResult& aResult);

	/**
    Returns a result at a given index within the result set.

	@param  aIndex
	        The index in the result set.

	@return  A reference to CSDResult object.

	@since  8.0
	*/
	IMPORT_C const CSDResult& At(TInt aIndex);

	/**
    Returns the number of results in the result set.

	@return The number of results in the result set.

    @since  8.0
	*/
	IMPORT_C TInt Count() const;

	/**
	Sets the diagnostic of this result set.

	@param  aDiagnostic
	        A reference to the diagostic object.

	@since  8.0
	*/
	IMPORT_C void SetDiagnostic(TDiagnostic& aDiagnostic);

	/**
    Gets the diagnostic of this result set.

	@return A reference to A TDiagnostic object.

	@since  8.0
	*/
	IMPORT_C const TDiagnostic& Diagnostic() const;

private:
	void ConstructL();
	CSDResultSet();

private:
	RPointerArray<CSDResult> iResultArray;
	TDiagnostic* iDiagnostic;
};


#endif

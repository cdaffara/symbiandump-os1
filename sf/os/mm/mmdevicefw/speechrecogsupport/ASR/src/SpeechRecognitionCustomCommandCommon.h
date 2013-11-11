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

#ifndef	__SPEECHRECOGNITIONCUSTOMCOMMANDCOMMON_H__
#define __SPEECHRECOGNITIONCUSTOMCOMMANDCOMMON_H__

#include <e32std.h>
// INCLUDE FILES
#include <mmf/common/speechrecognitiondataclient.h>

// CONSTANTS
static const TUid KUidInterfaceSpeechRecognition = {0x101F8CA6};

// FORWARD DECLARATIONS
class TSrsRecognize;
class TSrsAddPronunciation;
class TSrsAddRule;
class TSrsRuleValidity;
class TSrsGetUtteranceDuration;
class TSrsGetUtteranceDuration;
class TSrsPlayUtterance;

// DATA TYPES

/**
* @internalComponent
* Custom command message definitions
*/
enum TSrCustomCommandMessages
	{
	ESrAddPronunciation,
	ESrAddRule,
	ESrCancel,
	ESrCreateGrammar,
	ESrCreateLexicon,
	ESrCreateModelBank,
	ESrEndRecSession,
	ESrGetAllModelIDs,
	ESrGetAllPronunciationIDs,
	ESrGetAllRuleIDs,
	ESrGetAvailableStorage,
	ESrGetEngineProperties,
	ESrGetModelCount,
	ESrGetPronunciationCount,
	ESrGetRuleCount,
	ESrGetRuleValidity,
	ESrUtteranceDuration,
	ESrLoadGrammar,
	ESrLoadLexicon,
	ESrLoadModels,
	ESrPlayUtterance,
	ESrRecognize,
	ESrRecord,
	ESrRemoveGrammar,
	ESrRemoveLexicon,
	ESrRemoveModelBank,
	ESrRemoveModel,
	ESrRemovePronunciation,
	ESrRemoveRule,
	ESrStartRecSession,
	ESrTrain,
	ESrUnloadRule,
	ESrLoadEngineParameters,
	ESrGetPronunciationIDArraySize,
	ESrGetPronunciationIDArrayContents,
	ESrGetRuleIDArraySize,
	ESrGetRuleIDArrayContents,
	ESrGetModelIDArraySize,
	ESrGetModelIDArrayContents,
	ESrGetClientResultSetSize,
	ESrGetClientResultSet,
	ESrSetClientUid,
	ESrCommitChanges,
	ESrGetAllClientGrammarIDs,
	ESrGetAllClientLexiconIDs,
	ESrGetAllClientModelBankIDs,
	ESrGetAllGrammarIDs,
	ESrGetAllLexiconIDs,
	ESrGetAllModelBankIDs,
	ESrGetGrammarIDArraySize,
	ESrGetGrammarIDArrayContents,
	ESrGetLexiconIDArraySize,
	ESrGetLexiconIDArrayContents,
	ESrGetModelBankIDArraySize,
	ESrGetModelBankIDArrayContents
	};

typedef TPckgBuf<TSrsRecognize> TSrsRecognizePckg;
typedef TPckgBuf<TSrsAddPronunciation> TSrsAddPronunciationPckg;
typedef TPckgBuf<TSrsAddRule> TSrsAddRulePckg;
typedef TPckgBuf<TSrsRuleValidity> TSrsRuleValidityPckg;
typedef TPckgBuf<TGrammarID> TSrsGrammarIDPckg;
typedef TPckgBuf<TGrammarID *> TSrsGrammarIDPtrPckg;
typedef TPckgBuf<TLexiconID> TSrsLexiconIDPckg;
typedef TPckgBuf<TLexiconID *> TSrsLexiconIDPtrPckg;
typedef TPckgBuf<TModelBankID> TSrsModelBankIDPckg;
typedef TPckgBuf<TModelBankID *> TSrsModelBankIDPtrPckg;
typedef TPckgBuf<TSrsGetUtteranceDuration> TSrsGetUtteranceDurationPckg;
typedef TPckgBuf<TTimeIntervalMicroSeconds32> TSrsTimeIntervalMicroSeconds32Pckg;
typedef TPckgBuf<TSrsPlayUtterance> TSrsPlayUtterancePckg;
typedef TPckgBuf<TRecognitionMode> TSrsRecognitionModePckg;
typedef TPckgBuf<TPronunciationID> TSrsPronunciationIDPckg;
typedef TPckgBuf<TModelID> TSrsModelIDPckg;
typedef TPckgBuf<TRuleID> TSrsRuleIDPckg;
typedef TPckgBuf<TModelID *> TSrsModelIDPtrPckg;
typedef TPckgBuf<TInt *> TSrsIntPtrPckg;
typedef TPckgBuf<TUid> TSrsUidPckg;

// CLASS DECLARATION

/**
*  @internalComponent
*  This is the TSrsRecognize class declaration.
*
*  @since 8.0
*/
class TSrsRecognize
{
public:

	/**
	*	C++ constructor for this class.
    * @since	8.0
    */
	TSrsRecognize();

	/**
	*	C++ constructor for this class.
    * @since	8.0
    * @param	aClientResultSet	Pointer to client result set object.
    */
	TSrsRecognize(CSDClientResultSet* aClientResultSet);
		
	// member variable pointer to result set
	CSDClientResultSet* iClientResultSet;

};

/**
*  @internalComponent
*  This is the TSrsAddPronunciation class declaration.
*
*  @since 8.0
*/
class TSrsAddPronunciation
{
public:

	/**
	*	C++ constructor for this class.
    * @since	8.0
    */
	TSrsAddPronunciation();

	/**
	*	C++ constructor for this class.
    * @since	8.0
	* @param	aLexiconID			lexicon ID
	* @param	aModelBankID		model bank ID
	* @param    aModelID			model ID
	* @param	aPronunciationID	pronunciation ID 
    */
	TSrsAddPronunciation(TLexiconID aLexiconID, TModelBankID aModelBankID,
		TModelID aModelID, TPronunciationID* aPronunciationID);

	// lexicon ID member variable 
	TLexiconID iLexiconID;
	// model bank ID member variable 
	TModelBankID iModelBankID;
	// model ID member variable 
	TModelID iModelID;
	// pronunciation ID pointer member variable
	TPronunciationID* iPronunciationID;
};


/**
*  @internalComponent
*  This is the TSrsAddRule class declaration.
*
*  @since 8.0
*/
class TSrsAddRule
{
public:
	/**
	*	C++ constructor for this class.
    * @since	8.0
    */
	TSrsAddRule();

	/**
	*	C++ constructor for this class.
    * @since	8.0
	* @param	aGrammarID			grammar ID
	* @param	aLexiconID			lexicon ID
	* @param	aPronunciationID	pronunciation ID 
	* @param	aRuleID				rule ID
    */
	TSrsAddRule(TGrammarID aGrammarID, TLexiconID aLexiconID,
		TPronunciationID aPronunciationID, TRuleID* aRuleID);

	// grammar ID member variable
	TGrammarID iGrammarID;
	// lexicon ID member variable
	TLexiconID iLexiconID;
	// pronunciation ID member variable
	TPronunciationID iPronunciationID;
	// rule ID pointer member variable
	TRuleID* iRuleID;

};


/**
*  @internalComponent
*  This is the TSrsRuleValidity class declaration.
*
*  @since 8.0
*/
class TSrsRuleValidity
	{
public:
	/**
	*	C++ constructor for this class.
    * @since	8.0
    */
	TSrsRuleValidity();

	/**
	*	C++ constructor for this class.
    * @since	8.0
	* @param	aGrammarID			grammar ID
	* @param	aRuleID				rule ID
	& @param	aRuldValid			indicates if the rule is valid
    */
	TSrsRuleValidity(TGrammarID aGrammarID, TRuleID aRuleID, TBool* aRuleValid);

	// grammar ID member variable
	TGrammarID iGrammarID;
	// rule ID pointer member variable
	TRuleID iRuleID;
	// rule valid member variable
	TBool* iRuleValid; 
	};

/**
*  @internalComponent
*  This is the TSrsGetUtteranceDuration class declaration.
*
*  @since 8.0
*/
class TSrsGetUtteranceDuration
	{
public:
	/**
	*	C++ constructor for this class.
    * @since	8.0
    */
	TSrsGetUtteranceDuration();

	/**
	*	C++ constructor for this class.
    * @since	8.0
	* @param	aModelBankID		model bank ID
	* @param	aModelID			model ID
	* @param	aDuration			duration of the recorded utterance
    */
	TSrsGetUtteranceDuration(TModelBankID aModelBankID, TModelID aModelID,
		TTimeIntervalMicroSeconds32* aDuration); 

	// model bank ID member variable
	TModelBankID iModelBankID;
	// model ID member variable
	TModelID iModelID;
	// utterance duration
	TTimeIntervalMicroSeconds32* iDuration;
	};

/**
*  @internalComponent
*  This is the TSrsPlayUtterance class declaration.
*
*  @since 8.0
*/
class TSrsPlayUtterance
	{
public:
	/**
	*	C++ constructor for this class.
    * @since	8.0
    */
	TSrsPlayUtterance();

	/**
	*	C++ constructor for this class.
    * @since	8.0
	* @param	aModelBankID		model bank ID
	* @param	aModelID			model ID
	* @param	aFromMemory			indicates if the utterance is in memory, 
	*								ETrue if in memory
    */
	TSrsPlayUtterance(TModelBankID aModelBankID, TModelID aModelID);

	// model bank ID member variable
	TModelBankID iModelBankID;
	// model ID member variable
	TModelID iModelID;
	};
	
// inlines

inline TSrsRecognize::TSrsRecognize() 
	{
	}
	
inline TSrsRecognize::TSrsRecognize(CSDClientResultSet* aClientResultSet) 
	: iClientResultSet(aClientResultSet)
	{
	}
	
inline TSrsAddPronunciation::TSrsAddPronunciation()
	{
	}
	
inline TSrsAddPronunciation::TSrsAddPronunciation(TLexiconID aLexiconID, TModelBankID aModelBankID,
		TModelID aModelID, TPronunciationID* aPronunciationID) 
	: iLexiconID(aLexiconID), iModelBankID(aModelBankID), 
		iModelID(aModelID), iPronunciationID(aPronunciationID)
	{
	}

inline TSrsAddRule::TSrsAddRule()
	{
	}

inline TSrsAddRule::TSrsAddRule(TGrammarID aGrammarID, TLexiconID aLexiconID,
		TPronunciationID aPronunciationID, TRuleID* aRuleID) 
	: iGrammarID(aGrammarID), iLexiconID(aLexiconID), 
		iPronunciationID(aPronunciationID), iRuleID(aRuleID)
	{
	}
	
inline TSrsRuleValidity::TSrsRuleValidity() 
	{
	}

inline TSrsRuleValidity::TSrsRuleValidity(TGrammarID aGrammarID, TRuleID aRuleID, TBool* aRuleValid) 
	: iGrammarID(aGrammarID), iRuleID(aRuleID), iRuleValid(aRuleValid) 
	{
	}
	
inline TSrsGetUtteranceDuration::TSrsGetUtteranceDuration()
	{
	}

inline TSrsGetUtteranceDuration::TSrsGetUtteranceDuration(TModelBankID aModelBankID, TModelID aModelID,
	TTimeIntervalMicroSeconds32* aDuration) 
	: iModelBankID(aModelBankID), iModelID(aModelID), iDuration(aDuration) 
	{
	}
		
inline TSrsPlayUtterance::TSrsPlayUtterance() 
	{
	}


inline TSrsPlayUtterance::TSrsPlayUtterance(TModelBankID aModelBankID, TModelID aModelID) 
	: iModelBankID(aModelBankID), 
	iModelID(aModelID) 
	{
	}


#endif	// __SPEECHRECOGNITIONCUSTOMCOMMANDCOMMON_H__

// end of file

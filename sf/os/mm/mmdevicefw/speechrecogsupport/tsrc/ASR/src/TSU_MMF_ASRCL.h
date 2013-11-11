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

#ifndef __TSU_MMF_ASRCL_H__
#define __TSU_MMF_ASRCL_H__

#include <e32base.h>
#include <e32test.h>
#include <e32keys.h>
#include <c32comm.h>
#include <f32file.h>
#include <etel.h>
#include <etelmm.h>
#include <testframework.h>

#include "TSU_MMF_ASRCLStep.h"

class CTestSuite ;


//------------------------------------------------------------------ 

// Creates a new CSpeechRecognitionUtility object.
class RTestASRCLNewL : public RTestMMF_ASRCLStep
	{
public:
	RTestASRCLNewL();
	virtual TVerdict DoTestStepL();
	virtual TVerdict OpenL();
	virtual void Close();
	};

//------------------------------------------------------------------ 

// Creates a new CSpeechRecognitionUtility object. The object is placed on the cleanup stack.
class RTestASRCLNewLC : public RTestMMF_ASRCLStep
	{
public:
	RTestASRCLNewLC() ;
	virtual TVerdict DoTestStepL();
	virtual TVerdict OpenL();
	virtual void Close();
	} ;

//------------------------------------------------------------------ 

// Count the number of models.
class RTestASRCLGetModelCount : public RTestMMF_ASRCLStep
	{
public:
	RTestASRCLGetModelCount() ;
	virtual TVerdict DoTestStepL();
	} ;

//------------------------------------------------------------------ 

// Check the available space in the file system.
class RTestASRCLGetAvailableStorage : public RTestMMF_ASRCLStep
	{
public:
	RTestASRCLGetAvailableStorage() ;
	virtual TVerdict DoTestStepL();
	} ;

//------------------------------------------------------------------ 

// Creates a model bank.
class RTestASRCLCreateModelBank : public RTestMMF_ASRCLStep
	{
public:
	RTestASRCLCreateModelBank() ;
	virtual TVerdict DoTestStepL();
	} ;

//------------------------------------------------------------------ 

// Trains a model.
class RTestASRCLTrain : public RTestMMF_ASRCLStep
	{
public:
	RTestASRCLTrain() ;
	virtual TVerdict DoTestStepL();
	} ;

//------------------------------------------------------------------ 

// Get all pronunciation IDs
class RTestASRCLGetAllPronunciationIDs : public RTestMMF_ASRCLStep
	{
public:
	RTestASRCLGetAllPronunciationIDs() ;
	virtual TVerdict DoTestStepL();
	} ;

//------------------------------------------------------------------ 

// Get all rule IDs
class RTestASRCLGetAllRuleIDs : public RTestMMF_ASRCLStep
	{
public:
	RTestASRCLGetAllRuleIDs() ;
	virtual TVerdict DoTestStepL();
	} ;

//------------------------------------------------------------------ 

// Get all model IDs
class RTestASRCLGetAllModelIDs : public RTestMMF_ASRCLStep
	{
public:
	RTestASRCLGetAllModelIDs() ;
	virtual TVerdict DoTestStepL();
	} ;

//------------------------------------------------------------------ 

// Records user utterance.
class RTestASRCLRecordUtterance : public RTestMMF_ASRCLStep
	{
public:
	RTestASRCLRecordUtterance() ;
	virtual TVerdict DoTestStepL();
	} ;

//------------------------------------------------------------------ 

// Plays an utterance.
class RTestASRCLPlayUtterance : public RTestMMF_ASRCLStep
	{
public:
	RTestASRCLPlayUtterance() ;
	virtual TVerdict DoTestStepL();
	} ;

//------------------------------------------------------------------ 

// Commits a model.
class RTestASRCLCommitChanges : public RTestMMF_ASRCLStep
	{
public:
	RTestASRCLCommitChanges() ;
	virtual TVerdict DoTestStepL();
	} ;

//------------------------------------------------------------------ 

// Creates a lexicon.
class RTestASRCLCreateLexicon : public RTestMMF_ASRCLStep
	{
public:
	RTestASRCLCreateLexicon() ;
	virtual TVerdict DoTestStepL();
	} ;

//------------------------------------------------------------------ 

// Adds a pronunciation
class RTestASRCLAddPronunciation : public RTestMMF_ASRCLStep
	{
public:
	RTestASRCLAddPronunciation() ;
	virtual TVerdict DoTestStepL();
	} ;

//------------------------------------------------------------------ 

// Creates a grammar.
class RTestASRCLCreateGrammar : public RTestMMF_ASRCLStep
	{
public:
	RTestASRCLCreateGrammar() ;
	virtual TVerdict DoTestStepL();
	} ;

//------------------------------------------------------------------ 

// Adds a rule
class RTestASRCLAddRule : public RTestMMF_ASRCLStep
	{
public:
	RTestASRCLAddRule() ;
	virtual TVerdict DoTestStepL();
	} ;

//------------------------------------------------------------------ 

// Check a rule
class RTestASRCLGetRuleValidity : public RTestMMF_ASRCLStep
	{
public:
	RTestASRCLGetRuleValidity() ;
	virtual TVerdict DoTestStepL();
	} ;

//------------------------------------------------------------------ 

// Starts a recognition session.
class RTestASRCLStartRecSession : public RTestMMF_ASRCLStep
	{
public:
	RTestASRCLStartRecSession() ;
	virtual TVerdict DoTestStepL();
	} ;

//------------------------------------------------------------------ 

// Loads a model bank
class RTestASRCLLoadModelBank : public RTestMMF_ASRCLStep
	{
public:
	RTestASRCLLoadModelBank() ;
	virtual TVerdict DoTestStepL();
	} ;

//------------------------------------------------------------------ 

// Loads a lexicon.
class RTestASRCLLoadLexicon : public RTestMMF_ASRCLStep
	{
public:
	RTestASRCLLoadLexicon() ;
	virtual TVerdict DoTestStepL();
	} ;

//------------------------------------------------------------------ 

// Loads a grammar.
class RTestASRCLLoadGrammar : public RTestMMF_ASRCLStep
	{
public:
	RTestASRCLLoadGrammar() ;
	virtual TVerdict DoTestStepL();
	} ;

//------------------------------------------------------------------ 

// Unloads a rule.
class RTestASRCLUnloadRule : public RTestMMF_ASRCLStep
	{
public:
	RTestASRCLUnloadRule() ;
	virtual TVerdict DoTestStepL();
	} ;

//------------------------------------------------------------------ 

// Initiates recognition.
class RTestASRCLRecognize : public RTestMMF_ASRCLStep
	{
public:
	RTestASRCLRecognize() ;
	virtual TVerdict DoTestStepL();
	} ;

//------------------------------------------------------------------ 

// Terminates the recognition session.
class RTestASRCLEndRecSession : public RTestMMF_ASRCLStep
	{
public:
	RTestASRCLEndRecSession() ;
	virtual TVerdict DoTestStepL();
	} ;

//------------------------------------------------------------------ 

// Cancel asynchronous event. 
class RTestASRCLCancel : public RTestMMF_ASRCLStep
	{
public:
	RTestASRCLCancel() ;
	virtual TVerdict DoTestStepL();
	} ;

//------------------------------------------------------------------ 

// Removes a rule.
class RTestASRCLRemoveRule : public RTestMMF_ASRCLStep
	{
public:
	RTestASRCLRemoveRule() ;
	virtual TVerdict DoTestStepL();
	} ;

//------------------------------------------------------------------ 

// Removes a pronunciation
class RTestASRCLRemovePronunciation : public RTestMMF_ASRCLStep
	{
public:
	RTestASRCLRemovePronunciation() ;
	virtual TVerdict DoTestStepL();
	} ;

//------------------------------------------------------------------ 

// Removes a grammar
class RTestASRCLRemoveGrammar : public RTestMMF_ASRCLStep
	{
public:
	RTestASRCLRemoveGrammar() ;
	virtual TVerdict DoTestStepL();
	} ;

//------------------------------------------------------------------ 

// Removes a lexicon.
class RTestASRCLRemoveLexicon : public RTestMMF_ASRCLStep
	{
public:
	RTestASRCLRemoveLexicon() ;
	virtual TVerdict DoTestStepL();
	} ;

//------------------------------------------------------------------ 

// Removes a model.
class RTestASRCLRemoveModel : public RTestMMF_ASRCLStep
	{
public:
	RTestASRCLRemoveModel() ;
	virtual TVerdict DoTestStepL();
	} ;

//------------------------------------------------------------------ 

// Removes a model bank
class RTestASRCLRemoveModelBank : public RTestMMF_ASRCLStep
	{
public:
	RTestASRCLRemoveModelBank() ;
	virtual TVerdict DoTestStepL();
	} ;

//------------------------------------------------------------------ 

// Get utterance duration. 
class RTestASRCLGetUtteranceDuration : public RTestMMF_ASRCLStep
	{
public:
	RTestASRCLGetUtteranceDuration() ;
	virtual TVerdict DoTestStepL();
	} ;

//------------------------------------------------------------------ 

// Get engine properties.
class RTestASRCLGetEngineProperties : public RTestMMF_ASRCLStep
	{
public:
	RTestASRCLGetEngineProperties() ;
	virtual TVerdict DoTestStepL();
	} ;
	
//------------------------------------------------------------------ 

// Load Engine Parameters
class RTestASRCLLoadEngineParameters : public RTestMMF_ASRCLStep
	{
public:
	RTestASRCLLoadEngineParameters() ;
	virtual TVerdict DoTestStepL();
	} ;

//--------------------------------------------------------------
// Set Event Handler
class RTestASRCLSetEventHandler : public RTestMMF_ASRCLStep
	{
public:
	RTestASRCLSetEventHandler() ;
	virtual TVerdict DoTestStepL();
	} ;

//------------------------------------------------------------------ 

// Negative testing of async calls
class RTestASRCLNegAsyncCalls : public RTestMMF_ASRCLStep
	{
public:
	RTestASRCLNegAsyncCalls() ;
	virtual TVerdict DoTestStepL();
	} ;

//------------------------------------------------------------------ 

// Retrieve all the lexicon IDs owned by a specific client
class RTestASRCLGetAllClientLexiconIDs : public RTestMMF_ASRCLStep
	{
public:
	RTestASRCLGetAllClientLexiconIDs() ;
	virtual TVerdict DoTestStepL();
	} ;

//------------------------------------------------------------------ 

// Retrieve all the grammar IDs owned by a specific client
class RTestASRCLGetAllClientGrammarIDs : public RTestMMF_ASRCLStep
	{
public:
	RTestASRCLGetAllClientGrammarIDs() ;
	virtual TVerdict DoTestStepL();
	} ;

//------------------------------------------------------------------ 

// Retrieve all the model bank IDs owned by a specific client
class RTestASRCLGetAllClientModelBankIDs : public RTestMMF_ASRCLStep
	{
public:
	RTestASRCLGetAllClientModelBankIDs() ;
	virtual TVerdict DoTestStepL();
	} ;

//------------------------------------------------------------------ 

// Retrieve all the lexicon IDs in the system
class RTestASRCLGetAllLexiconIDs : public RTestMMF_ASRCLStep
	{
public:
	RTestASRCLGetAllLexiconIDs() ;
	virtual TVerdict DoTestStepL();
	} ;

//------------------------------------------------------------------ 

// Retrieve all the grammar IDs in the system
class RTestASRCLGetAllGrammarIDs : public RTestMMF_ASRCLStep
	{
public:
	RTestASRCLGetAllGrammarIDs() ;
	virtual TVerdict DoTestStepL();
	} ;

//------------------------------------------------------------------ 

// Retrieve all the model bank IDs in the system
class RTestASRCLGetAllModelBankIDs : public RTestMMF_ASRCLStep
	{
public:
	RTestASRCLGetAllModelBankIDs() ;
	virtual TVerdict DoTestStepL();
	} ;


//------------------------------------------------------------------ 

// Alloc testing: Get all pronunciation IDs
class RTestASRCLGetAllPronunciationIDsAlloc : public RTestMMF_ASRCLStep
	{
public:
	RTestASRCLGetAllPronunciationIDsAlloc() ;
	virtual TVerdict DoTestStepL();
	
private:
    // Called by DoTestStepL() -
    // returns the err code & the GetAllPronunciationIDsAlloc result.
	TInt GetTestResults(TLexiconID& aLexiconId, RArray <TPronunciationID>& aPronunciationIDs, TVerdict& aResult);
	} ;

//------------------------------------------------------------------ 

// Alloc testing:  Get all model IDs
class RTestASRCLGetAllModelIDsAlloc : public RTestMMF_ASRCLStep
	{
public:
	RTestASRCLGetAllModelIDsAlloc() ;
	virtual TVerdict DoTestStepL();
private:
	// Called by DoTestStepL() -
    // returns the err code & the  GetAllModelIDsAlloc result.
	TInt GetTestResults(TModelBankID& aModelBankId, RArray <TModelID>& aModelIDs, TVerdict& aResult);
	} ;

//------------------------------------------------------------------ 

// Alloc testing: Get all rule IDs
class RTestASRCLGetAllRuleIDsAlloc : public RTestMMF_ASRCLStep
	{
public:
	RTestASRCLGetAllRuleIDsAlloc() ;
	virtual TVerdict DoTestStepL();
private:
	// Called by DoTestStepL() -
    // returns the err code & the  GetAllRuleIDsAlloc result.
	TInt GetTestResults(TGrammarID& aGrammarId, RArray <TRuleID>& aRuleIDs, TVerdict& aResult);
	} ;

//------------------------------------------------------------------ 

// Alloc testing: Get all grammar IDs
class RTestASRCLGetAllGrammarIDsAlloc : public RTestMMF_ASRCLStep
	{
public:
	RTestASRCLGetAllGrammarIDsAlloc() ;
	virtual TVerdict DoTestStepL();
private:
	// Called by DoTestStepL() -
    // returns the err code & the GetAllGrammarIDsAlloc result.
	TInt GetTestResults(RArray <TGrammarID>& aGrammarIDs, TVerdict& aResult);
	} ;

//------------------------------------------------------------------ 

// Alloc testing: Get all lexicon IDs
class RTestASRCLGetAllLexiconIDsAlloc : public RTestMMF_ASRCLStep
	{
public:
	RTestASRCLGetAllLexiconIDsAlloc() ;
	virtual TVerdict DoTestStepL();
private:
	// Called by DoTestStepL() -
    // returns the err code & the GetAllLexiconIDsAlloc result.
	TInt GetTestResults(RArray <TLexiconID>& aLexiconIDs, TVerdict& aResult);
	} ;

//------------------------------------------------------------------ 

// Alloc testing: Get all modelBank IDs
class RTestASRCLGetAllModelBankIDsAlloc : public RTestMMF_ASRCLStep
	{
public:
	RTestASRCLGetAllModelBankIDsAlloc() ;
	virtual TVerdict DoTestStepL();
private:
	// Called by DoTestStepL() -
    // returns the err code & the GetAllModelBankIDsAlloc result.
	TInt GetTestResults(RArray <TModelBankID>& aModelBankIDs, TVerdict& aResult);
	} ;

//------------------------------------------------------------------ 

// Alloc testing: Get all client grammar IDs
class RTestASRCLGetAllClientGrammarIDsAlloc : public RTestMMF_ASRCLStep
	{
public:
	RTestASRCLGetAllClientGrammarIDsAlloc() ;
	virtual TVerdict DoTestStepL();
private:
	// Called by DoTestStepL() -
    // returns the err code & the GetAllClientGrammarIDsAlloc result.
	TInt GetTestResults(RArray <TGrammarID>& aGrammarIDs, TVerdict& aResult);
	} ;

//------------------------------------------------------------------ 

// Alloc testing: Get all client lexicon IDs
class RTestASRCLGetAllClientLexiconIDsAlloc : public RTestMMF_ASRCLStep
	{
public:
	RTestASRCLGetAllClientLexiconIDsAlloc() ;
	virtual TVerdict DoTestStepL();
private:
	// Called by DoTestStepL() -
    // returns the err code & the GetAllClientLexiconIDsAlloc result.
	TInt GetTestResults(RArray <TLexiconID>& aLexiconIDs, TVerdict& aResult);
	} ;

//------------------------------------------------------------------ 

// Alloc testing: Get all client modelBank IDs
class RTestASRCLGetAllClientModelBankIDsAlloc : public RTestMMF_ASRCLStep
	{
public:
	RTestASRCLGetAllClientModelBankIDsAlloc() ;
	virtual TVerdict DoTestStepL();
private:
	// Called by DoTestStepL() -
    // returns the err code & the GetAllClientModelBankIDsAlloc result.
	TInt GetTestResults(RArray <TModelBankID>& aModelBankIDs, TVerdict& aResult);
	} ;

//------------------------------------------------------------------ 
// Alloc Test: Recognize

class RTestASRCLRecognizeAlloc : public RTestMMF_ASRCLStep
	{
public:
	RTestASRCLRecognizeAlloc() ;
	virtual TVerdict DoTestStepL();
	} ;


//------------------------------------------------------------------ 
// Alloc Test: GetEngineProperties

class RTestASRCLGetEnginePropertiesAlloc : public RTestMMF_ASRCLStep
	{
public:
	RTestASRCLGetEnginePropertiesAlloc() ;
	virtual TVerdict DoTestStepL();
	} ;

//------------------------------------------------------------------ 
// SDData: CSDRule test
// test the CSDRule class

class RTestASRCLSDDataRule : public RTestMMF_ASRCLStep
	{
public:
	RTestASRCLSDDataRule() ;
	virtual TVerdict DoTestStepL();
	} ;

//------------------------------------------------------------------ 
// SDData: Pronunciation test
// test the CSDPronunciation class

class RTestASRCLSDDataPronunciation : public RTestMMF_ASRCLStep
	{
public:
	RTestASRCLSDDataPronunciation() ;
	virtual TVerdict DoTestStepL();
	} ;

//------------------------------------------------------------------ 
// SDData: Model test
// test the CSDModel class

class RTestASRCLSDDataModel : public RTestMMF_ASRCLStep
	{
public:
	RTestASRCLSDDataModel() ;
	virtual TVerdict DoTestStepL();
	} ;


//------------------------------------------------------------------ 
// SDData: Grammar test
// test the CSDGrammar class

class RTestASRCLSDDataGrammar : public RTestMMF_ASRCLStep
	{
public:
	RTestASRCLSDDataGrammar() ;
	virtual TVerdict DoTestStepL();
	} ;


//------------------------------------------------------------------ 
// SDData: Grammar test
// test CSDGrammar Add Rule

class RTestASRCLSDDataGrammarAdd : public RTestMMF_ASRCLStep
	{
public:
	RTestASRCLSDDataGrammarAdd() ;
	virtual TVerdict DoTestStepL();
	} ;

//------------------------------------------------------------------ 
// SDData: Grammar test
// test the CSDGrammar and CSDRule classes

class RTestASRCLSDDataGrammarFind : public RTestMMF_ASRCLStep
	{
public:
	RTestASRCLSDDataGrammarFind() ;
	virtual TVerdict DoTestStepL();
	} ;

//------------------------------------------------------------------ 
// SDData: Grammar test
// test the CSDGrammar and CSDRule classes

class RTestASRCLSDDataGrammarDelete : public RTestMMF_ASRCLStep
	{
public:
	RTestASRCLSDDataGrammarDelete() ;
	virtual TVerdict DoTestStepL();
	} ;

//------------------------------------------------------------------ 
// SDData: Grammar test
// test CSDGrammar Alloc test

class RTestASRCLSDDataGrammarAlloc : public RTestMMF_ASRCLStep
	{
public:
	RTestASRCLSDDataGrammarAlloc() ;
	virtual TVerdict DoTestStepL();
	} ;



//------------------------------------------------------------------ 
// SDData: Lexicon test
// test the CSDLexicon class

class RTestASRCLSDDataLexicon : public RTestMMF_ASRCLStep
	{
public:
	RTestASRCLSDDataLexicon() ;
	virtual TVerdict DoTestStepL();
	} ;


//------------------------------------------------------------------ 
// SDData: Lexicon test
// test CSDLexicon Add Rule

class RTestASRCLSDDataLexiconAdd : public RTestMMF_ASRCLStep
	{
public:
	RTestASRCLSDDataLexiconAdd() ;
	virtual TVerdict DoTestStepL();
	} ;


//------------------------------------------------------------------ 
// SDData: Lexicon test
// test the CSDLexicon and CSDRule classes

class RTestASRCLSDDataLexiconFind : public RTestMMF_ASRCLStep
	{
public:
	RTestASRCLSDDataLexiconFind() ;
	virtual TVerdict DoTestStepL();
	} ;

//------------------------------------------------------------------ 
// SDData: Lexicon test
// Alloc testing on CSDLexicon

class RTestASRCLSDDataLexiconAlloc : public RTestMMF_ASRCLStep
	{
public:
	RTestASRCLSDDataLexiconAlloc() ;
	virtual TVerdict DoTestStepL();
	} ;


//------------------------------------------------------------------ 
// SDData: ModelBank test
// test the CSDModelBank class

class RTestASRCLSDDataModelBank : public RTestMMF_ASRCLStep
	{
public:
	RTestASRCLSDDataModelBank() ;
	virtual TVerdict DoTestStepL();
	} ;


//------------------------------------------------------------------ 
// SDData: ModelBank test
// test CSDModelBank Add Rule

class RTestASRCLSDDataModelBankAdd : public RTestMMF_ASRCLStep
	{
public:
	RTestASRCLSDDataModelBankAdd() ;
	virtual TVerdict DoTestStepL();
	} ;


//------------------------------------------------------------------ 
// SDData: ModelBank test
// test the CSDModelBank and CSDRule classes

class RTestASRCLSDDataModelBankFind : public RTestMMF_ASRCLStep
	{
public:
	RTestASRCLSDDataModelBankFind() ;
	virtual TVerdict DoTestStepL();
	} ;

//------------------------------------------------------------------ 
// SDData: ModelBank test
// Alloc testing on CSDModelBank

class RTestASRCLSDDataModelBankAlloc : public RTestMMF_ASRCLStep
	{
public:
	RTestASRCLSDDataModelBankAlloc() ;
	virtual TVerdict DoTestStepL();
	} ;


//------------------------------------------------------------------ 
// SDData: CSDResult test
// test the CSDResult class

class RTestASRCLSDDataResult : public RTestMMF_ASRCLStep
	{
public:
	RTestASRCLSDDataResult() ;
	virtual TVerdict DoTestStepL();
	} ;

//------------------------------------------------------------------ 
// SDData: CSDResultSet add test
// test adding results to the CSDResultSet class

class RTestASRCLSDDataResultSetAdd : public RTestMMF_ASRCLStep
	{
public:
	RTestASRCLSDDataResultSetAdd() ;
	virtual TVerdict DoTestStepL();
	} ;

//------------------------------------------------------------------ 
// SDData: CSDResultSet add test
// alloc testing for the CSDResultSet class

class RTestASRCLSDDataResultSetAlloc : public RTestMMF_ASRCLStep
	{
public:
	RTestASRCLSDDataResultSetAlloc() ;
	virtual TVerdict DoTestStepL();
	} ;


#endif// __TSU_MMF_ASRCL_H__


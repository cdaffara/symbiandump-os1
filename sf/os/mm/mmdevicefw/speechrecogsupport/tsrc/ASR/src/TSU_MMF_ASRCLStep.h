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

#ifndef __TSU_MMF_ASRCLSTEP_H__
#define __TSU_MMF_ASRCLSTEP_H__

#include <testframework.h>
#include <speechrecognitionutility.h>

class CTestSuite ;

class TDummyObserver : public MSpeechRecognitionUtilityObserver
	{
public:	
	// from MSpeechRecognitionUtilityObserver
	void MsruoEvent(TUid aEvent, TInt aErrorCode);
	void WaitForEvent();
public:
	TUid iEvent;
	TSpeechRecognitionResult iErrorCode;
	};


class RTestMMF_ASRCLStep : public RTestStep, public MSpeechRecognitionUtilityObserver
	{
public:
	enum TInitialDatabaseState 
		{
		EReset,
		ETestDatabase,
		ENoDatabase
		};
public:
	virtual ~RTestMMF_ASRCLStep();
	RTestMMF_ASRCLStep();
	static void CleanUp(TAny *aPtr) ;
	void SetTestSuite(const CTestSuite* aTestSuite )
		{ iTestSuite = aTestSuite;}  
		
	virtual TVerdict OpenL();
	void Close();
		
	void CreateSpeechClientL(TUid aClientUID);
	void ResetDatabaseL();
	void CreateTestDatabaseL();

	TVerdict CheckModelCount(TModelBankID aModelBankID, TInt aExpectedModelCount);
	TVerdict StartTraining(TModelBankID aModelBankID, TModelID& aModelID);
	TVerdict StartTrainingBadModelBankID();
	TVerdict RecordUtterance();
	TVerdict CommitChanges( TBool aExpectedPass = ETrue );
	TVerdict PlayUtterance(TModelBankID aModelBankID, TModelID aModelID, TBool ExpectPass = ETrue );
	TVerdict StartRecSession(TRecognitionMode aMode, TBool aSuccessExpected = ETrue );
	TVerdict EndRecSession();
	
	TVerdict LoadModels(TModelBankID aModelBankID, TBool aSuccessExpected = ETrue);
	TVerdict LoadLexicon(TLexiconID aLexiconID, TBool aSuccessExpected = ETrue);
	TVerdict LoadGrammar(TGrammarID aGrammarID, TBool aSuccessExpected = ETrue);
	TVerdict RemoveRule(TGrammarID aGrammarID, TRuleID aRuleID);
	TVerdict UnloadRule(TGrammarID aGrammarID, TRuleID aRuleID);
	TVerdict RemovePronunciation(TLexiconID aLexiconID, TPronunciationID aPronunciationID);
	TVerdict RemoveModel(TModelBankID aModelBank, TModelID aModelID);
	TVerdict RemoveGrammar(TGrammarID aGrammarID );
	TVerdict RemoveLexicon(TLexiconID aLexiconID );
	TVerdict RemoveModelBank(TModelBankID aModelBank );

	TVerdict Recognize(CSDClientResultSet& aClientResultSet);
	

	TVerdict AddPronunciation(TLexiconID aLexiconID, TModelBankID aModelBankID, TModelID aModelID, 
		TPronunciationID& aPronunciationID, TBool aSuccessExpected = ETrue );

	TVerdict CreateLexicon(TLexiconID& aLexiconID);	
	TVerdict CreateGrammar(TGrammarID& aGrammarID);
	TVerdict CreateModelBank(TModelBankID& aModelBankID);
	TVerdict AddRule(TGrammarID aGrammarID, TLexiconID aLexiconID, TPronunciationID aPronunciationID, 
		TRuleID& aRuleID, TBool aSuccessExpected=ETrue );

	TVerdict CheckRuleValidity(TGrammarID aGrammarID, TRuleID aRuleID, TBool aExpectedValidity = ETrue, TBool aExpectedError = KErrNone);
	TVerdict LoadEngineParameters(const RArray<TInt>& aParameterIDs, const RArray<TInt>& aParameterValues);
	
	TVerdict CancelTest();
   
   	TVerdict GetAllRuleIDs(TGrammarID aGrammarID, RArray<TRuleID>& aRuleIDArray); 
   	TVerdict GetAllPronunciationIDs(TLexiconID aLexiconID, RArray<TPronunciationID>& aPronunciationIDArray); 
   	TVerdict GetAllModelIDs(TModelBankID aModelBankID, RArray<TModelID>& modelIDArray); 
   	
   	TVerdict GetAllGrammarIDs(RArray<TGrammarID>& aGrammarIDArray, TBool aOwnedByClient=EFalse); 
   	TVerdict GetAllLexiconIDs(RArray<TLexiconID>& aLexiconIDArray, TBool aOwnedByClient=EFalse); 
   	TVerdict GetAllModelBankIDs(RArray<TModelBankID>& modelBankIDArray, TBool aOwnedByClient=EFalse); 

   	 	
   	TPtrC TranslateEventCode(TInt aEvent);
   	

    
protected:
	// from MSpeechRecognitionUtilityObserver
	virtual void MsruoEvent( TUid aEvent, TInt aError );
	TBool WaitForEvent(TUid aEvent, TInt aExpectedError = KErrNone);

protected:
	const CTestSuite* iTestSuite; // not owned

	TUid iLastEvent;
	TInt iLastError;
	
	CSpeechRecognitionUtility* iSpeechClient;
	CActiveScheduler* iScheduler;

	TBool iNegativeAsyncCallTesting; // perform negative testing for asynchronous calls
	TBool iCreateClient; // Whether the client should be created by open, by default ETrue
	TInitialDatabaseState iInitialDatabaseState;
	};


#endif //__TSU_MMF_ASRCLSTEP_H__

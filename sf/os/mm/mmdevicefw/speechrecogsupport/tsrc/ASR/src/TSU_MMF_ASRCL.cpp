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

#include <mmf/common/speechrecognitiondatadevasr.h>
#include "TSU_MMF_ASRCL.h"
#include "ASRController/customcontrollertestconstants.h"
#include "Database/testdatabaseconstants.h"


//------------------------------------------------------------------

RTestASRCLNewL::RTestASRCLNewL()
	{
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ASRCL-U-0001");
	}
//------------------------------------------------------------------

TVerdict RTestASRCLNewL::DoTestStepL()
/** Creates a new CSpeechRecognitionUtility object.
 * @test Req. under test 
 */
	{
	CSpeechRecognitionUtility* speechClient = NULL;
	TUid uid = KUidTestASRClient;
	TRAPD(err, speechClient = CSpeechRecognitionUtility::NewL(*this, uid));
	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("Failure Creating Speech Recognition Utility and error is %d"), err);
		return EFail;
		}
	delete speechClient;
	return	EPass ;	
	}
//------------------------------------------------------------------

TVerdict RTestASRCLNewL::OpenL()
	{
	iCreateClient = EFalse;
	return RTestMMF_ASRCLStep::OpenL(); // don't create the client - or the active scheduler
	}
//------------------------------------------------------------------

void RTestASRCLNewL::Close()
	{
	RTestMMF_ASRCLStep::Close();
	}
//------------------------------------------------------------------

RTestASRCLNewLC::RTestASRCLNewLC()
	{
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ASRCL-U-0002");
	}
//------------------------------------------------------------------

TVerdict RTestASRCLNewLC::DoTestStepL()
/** Creates a new CSpeechRecognitionUtility object. The object is placed on the cleanup stack.
 * @test Req. under test 
 */
	{
	CSpeechRecognitionUtility* speechClient=NULL;
	TUid uid = KUidTestASRClient;
	TRAPD(err, speechClient = CSpeechRecognitionUtility::NewLC(*this, uid);
				if (err==KErrNone)
					CleanupStack::PopAndDestroy(speechClient));
	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("Failure Creating Speech Recognition Utility and the error is %d"), err);
		return EFail;
		}
	return EPass;
	}
//------------------------------------------------------------------

TVerdict RTestASRCLNewLC::OpenL()
	{
	iCreateClient = EFalse;
	return RTestMMF_ASRCLStep::OpenL(); 
	}
//------------------------------------------------------------------

void RTestASRCLNewLC::Close()
	{
	RTestMMF_ASRCLStep::Close();
	}
//------------------------------------------------------------------

RTestASRCLGetModelCount::RTestASRCLGetModelCount()
	{
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ASRCL-U-0003");
	}
//------------------------------------------------------------------

TVerdict RTestASRCLGetModelCount::DoTestStepL()
/** Count the number of models.
 * Use case:  * Training1 - model
 * @test Req. under test 
 */
	{
	TModelBankID modelBank;
	
	modelBank = KModelBankIDs[KIndex0];

	if (LoadModels(modelBank)!=EPass)
		return EFail;
		
	if (CheckModelCount(modelBank, KNumModels)!=EPass)
		return EFail;
		
	modelBank = KModelBankIDs[KIndex1];

	if (LoadModels(modelBank)!=EPass)
		return EFail;
		
	if (CheckModelCount(modelBank, KNumModels)!=EPass)
		return EFail;

	return	EPass ;
	}
//------------------------------------------------------------------
RTestASRCLGetAvailableStorage::RTestASRCLGetAvailableStorage()
	{

	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ASRCL-U-0004");
	}
//------------------------------------------------------------------

TVerdict RTestASRCLGetAvailableStorage::DoTestStepL()
/** Check the available space in the file system.
 * Use case:  * Training1 - model
 * @test Req. under test 
 */
	{
	TVerdict result = EFail;

	TInt numberOfModels;
	TInt err = iSpeechClient->GetAvailableStorage(numberOfModels);
	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("Unexpected error: %d"),err);
		return result;
		}
	
	//[ Check available storage against known number of models
	// since we know the test controller always returns zero
	// free space ]
	if( numberOfModels != KNumberOfModelsInFreeStorage )
		{
		INFO_PRINTF2(_L("Unexpected Available storage: %d"),numberOfModels);
		return result;
		}

	result = EPass;

	return	result;
	}
//------------------------------------------------------------------

RTestASRCLCreateModelBank::RTestASRCLCreateModelBank()
	{
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ASRCL-U-0005");
	}
//------------------------------------------------------------------

TVerdict RTestASRCLCreateModelBank::DoTestStepL()
/** Creates a model bank.
 * Use case:  * Training1 - model
 * @test Req. under test 
 */
	{
    //[ lets get the model id's in the bank before ]

	TModelBankID modelBankID;
	if (CreateModelBank(modelBankID)!=EPass)
		{
		INFO_PRINTF1(_L("Failed creating model bank"));
		return EFail;
		}
	
   //[ get all the model ids from the new bank
   // ensure the model count is zero ]

	//[ load the Model bank ]
	if( LoadModels( modelBankID ) != EPass )
		{
		return EFail;
		}

//	TVerdict result = EPass;							// EABI warning removal
	RArray <TModelID> modelIDs;
	if (GetAllModelIDs( modelBankID, modelIDs)==EPass)
		{
		if( modelIDs.Count() != 0)
			{
			INFO_PRINTF3(_L("Newly Created ModelBank %d is not empty, with a model count = %d"), modelBankID, modelIDs.Count() );
//			result = EFail;	// Should this be "return EFail"?
			}
		}
	else
		{
		INFO_PRINTF1(_L("Unexpected failure: %d"));
//		result = EFail; // Should this be "return EFail"?
		}

		
	modelIDs.Close();
	return EPass;
	}
//------------------------------------------------------------------

RTestASRCLTrain::RTestASRCLTrain()
	{
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ASRCL-U-0006");
	}
//------------------------------------------------------------------

TVerdict RTestASRCLTrain::DoTestStepL()
/** Trains a model.
 * Use case:  * Training1 - model
 * @test Req. under test 
 */
	{
	TModelBankID modelBankID = KModelBankIDs[KIndex0];
	TModelID modelID;
	if (LoadModels(modelBankID)!=EPass)
		return EFail;
	if (RecordUtterance()!=EPass)
		return EFail;
	if (StartTraining(modelBankID, modelID)!=EPass)
		return EFail;

	if( CommitChanges() != EPass )
		{
		return EFail;
		}

	//[ check for error when bad modelbankid is used ]
	if( StartTrainingBadModelBankID() != EPass )
		{
        return EFail;
		}

	return EPass;
	}
//------------------------------------------------------------------
RTestASRCLGetAllPronunciationIDs::RTestASRCLGetAllPronunciationIDs()
	{
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ASRCL-U-0031");
	}
//------------------------------------------------------------------

TVerdict RTestASRCLGetAllPronunciationIDs::DoTestStepL()
/** Get all pronunciation IDs
 * Use case:  * Get_Info1
 * @test Req. under test 
 */
	{
     TLexiconID lexiconId = KLexiconIDs[KIndex0];
	 //[ load a known lexicon ]
	 if( LoadLexicon( lexiconId ) != EPass )
		{
		return EFail;
		}
	TVerdict result = EPass;
	RArray <TPronunciationID> pronunciationIDs;
	if (GetAllPronunciationIDs( lexiconId, pronunciationIDs)!=EPass)
		{
		result = EFail;
		}
	
	//[ check that the id's contained in the list 
	// conform to those added to the lexicon in the controller ]
    if ((result == EPass) && (pronunciationIDs.Count() != KNumPronunciations))
    	{
    	INFO_PRINTF3(_L("Expected %d pronuciations, received %d pronunciations"), KNumPronunciations, pronunciationIDs.Count());
    	result = EFail;
    	}

    if (result == EPass)
    	{
		INFO_PRINTF2(_L("Pronunciation Ids Count: %d"),pronunciationIDs.Count() );
	    for( TInt index = 0; index < pronunciationIDs.Count() && result == EPass; index++ )
			{
	    	if (pronunciationIDs[index]!=KPronunciationIDs[KIndex0][index].iPronunciationID)
	    		{
	    		INFO_PRINTF3(_L("failed - expected pronunciation id: %d, received pronunciation id: %d"),KPronunciationIDs[KIndex0][index],pronunciationIDs[index]);
		    	result = EFail;	    	
		    	}
			}
			
		
		}

	pronunciationIDs.Close();
	return result;
	}
//------------------------------------------------------------------

RTestASRCLGetAllRuleIDs::RTestASRCLGetAllRuleIDs()
	{
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ASRCL-U-0032");
	}
//------------------------------------------------------------------

TVerdict RTestASRCLGetAllRuleIDs::DoTestStepL()
/** Get all rule IDs
 * Use case:  * Get_Info1
 * @test Req. under test 
 */
	{
	TGrammarID grammarId = KGrammarIDs[KIndex0];
	 //[ load a known lexicon ]
	 if( LoadGrammar( grammarId ) != EPass )
		{
		return EFail;
		}

	TVerdict result = EPass;
	RArray <TRuleID> ruleIDs;
	if (GetAllRuleIDs( grammarId, ruleIDs)!=EPass)
		{
		INFO_PRINTF1(_L("Unexpected failure"));
		result = EFail;
		}

	//[ check that the id's contained in the list 
	// conform to those added to the lexicon in the controller ]
    if ((result == EPass) && (ruleIDs.Count() != KNumRules))
    	{
    	INFO_PRINTF3(_L("Expected %d Rules, received %d rules"), KNumRules, ruleIDs.Count());
    	result = EFail;
    	}

    	
    if (result == EPass)
    	{
	    for( TInt index = 0; index < ruleIDs.Count() && result == EPass; index++ )
			{
	    	if (ruleIDs[index]!=KRuleIDs[KIndex0][index].iRuleID)
	    		{
	    		INFO_PRINTF3(_L("failed - expected rule id: %d, received rule id: %d"),KRuleIDs[KIndex0][index],ruleIDs[index]);
		    	result = EFail;	    	
		    	}
			}
		}		
	ruleIDs.Close();

	return result;
	}
//------------------------------------------------------------------

RTestASRCLGetAllModelIDs::RTestASRCLGetAllModelIDs()
	{
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ASRCL-U-0033");
	}
//------------------------------------------------------------------

TVerdict RTestASRCLGetAllModelIDs::DoTestStepL()
/** Get all model IDs
 * Use case:  * Get_Info1
 * @test Req. under test 
 */
	{
	TModelBankID modelBankId = KModelBankIDs[KIndex1];
	 //[ load a known lexicon ]
	 if( LoadModels( modelBankId ) != EPass )
		{
		return EFail;
		}

	TVerdict result = EPass;
	RArray <TModelID> modelIDs;
	if (GetAllModelIDs( modelBankId, modelIDs)!=EPass)
		{
		INFO_PRINTF1(_L("Unexpected failure"));
		result = EFail;
		}

    if (result == EPass && modelIDs.Count() != KNumModels)
    	{
    	INFO_PRINTF3(_L("Expected %d Models, received %d models"), KNumModels, modelIDs.Count());
    	result = EFail;
    	}

	
	if (result == EPass)
		{    	
	    for( TInt index = 0; index < modelIDs.Count() && result == EPass; index++ )
			{
	    	if (modelIDs[index]!=KModelIDs[KIndex1][index].iModelID)
	    		{
	    		INFO_PRINTF3(_L("failed - expected model id: %d, received model id: %d"),KModelIDs[KIndex1][index],modelIDs[index]);
		    	result = EFail;	    	
		    	}
			}
		} 
   	return result;
	}
//------------------------------------------------------------------

RTestASRCLRecordUtterance::RTestASRCLRecordUtterance()
	{
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ASRCL-U-0007");
	}
//------------------------------------------------------------------

TVerdict RTestASRCLRecordUtterance::DoTestStepL()
/** Records user utterance.
 * Use case:  * Training1 - model
 * @test Req. under test 
 */
	{
	TModelBankID modelBankID = KModelBankIDs[KIndex0];
	TModelID modelID;

	if (LoadModels(modelBankID)!=EPass)
		return EFail;
	if (StartTraining(modelBankID, modelID)!=EPass)
		return EFail;
	if (RecordUtterance()!=EPass)
		return EFail;
	return	EPass;
	}
//------------------------------------------------------------------

RTestASRCLPlayUtterance::RTestASRCLPlayUtterance()
	{
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ASRCL-U-0008");
	}
//------------------------------------------------------------------

TVerdict RTestASRCLPlayUtterance::DoTestStepL()
/** Plays an utterance.
 * Use case:  * Training1 - model
 * @test Req. under test 
 */
	{
	TModelBankID modelBankID = KModelBankIDs[KIndex1];
	TModelID modelID = KModelIDs[KIndex1][KIndex0].iModelID;
	TVerdict result;

	//[ precondition model bank is loaded ]
	result = LoadModels( modelBankID );
	if (result!=EPass)
		return result;

	//[ play the utterance from memory ]
	result = PlayUtterance(modelBankID, modelID, ETrue );
	if (result!=EPass)
		return result;

	//[ play the utterance from memory ]
	//set the model bank id to a known invalid model
	modelBankID = KBadModelBankID;
	TInt err = iSpeechClient->PlayUtterance(modelBankID, modelID);
	if (err!=KErrNotFound)
		return EFail;

	return EPass;
	}
//------------------------------------------------------------------


RTestASRCLCommitChanges::RTestASRCLCommitChanges()
	{
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ASRCL-U-0009");
	iCreateClient = EFalse;
	}
//------------------------------------------------------------------

TVerdict RTestASRCLCommitChanges::DoTestStepL()
/** Commits a model.
 * Use case:  * Training1 - model
 * @test Req. under test 
 */
	{
	CreateSpeechClientL(KUidTestASRClient);
	
	TGrammarID grammarID = KGrammarIDs[KIndex1];
	TRuleID ruleID;
	TVerdict result = EPass;
	
	RArray<TRuleID> rulesBefore;
	RArray<TRuleID> rulesAfter;
	
	CleanupClosePushL(rulesBefore);
	CleanupClosePushL(rulesAfter);
	
	if (AddRule(grammarID, 0,0, ruleID)!=EPass)
		result = EInconclusive;
	
	if ((result == EPass) && (GetAllRuleIDs(grammarID, rulesBefore)!=EPass))
		result = EInconclusive;
		
	if ((result == EPass) && (CommitChanges()!=EPass))
		result = EFail;
		
	if ((result == EPass) && (AddRule(grammarID, 0,0, ruleID)!=EPass))
		result = EInconclusive;


	if (result == EPass)
		{
		// delete the speech client and recreate it. The last update to the database should be lost as CommitChanges was not called
		delete iSpeechClient;
		iSpeechClient = NULL;
		CreateSpeechClientL(KUidTestASRClient);	
	
		if (GetAllRuleIDs(grammarID, rulesAfter)!=EPass)
			result = EInconclusive;
	
		if ((result == EPass) && (rulesAfter.Count() != rulesBefore.Count()))
			{
			INFO_PRINTF3(_L("Expected %d rules in database, but %d rules returned"),rulesBefore.Count(),rulesAfter.Count());
			result = EFail;
			}
	
		}
		
	CleanupStack::PopAndDestroy(2, &rulesBefore);
	return result;
	}
//------------------------------------------------------------------


RTestASRCLCreateLexicon::RTestASRCLCreateLexicon()
	{
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ASRCL-U-0010");
	}
//------------------------------------------------------------------

TVerdict RTestASRCLCreateLexicon::DoTestStepL()
/** Creates a lexicon.
 * Use case:  * Training2 - lexicon
 * @test Req. under test 
 */
	{
	TLexiconID lexiconId = KLexiconIDs[KIndex1];
	if( LoadLexicon( lexiconId ) != EPass)
		{
		return EFail ;
		}
	
	lexiconId = KBadLexiconID;
	if( LoadLexicon( lexiconId, EFalse ) != EPass)
		{
		return EFail ;
		}
	
	return EPass;
	}
//------------------------------------------------------------------


RTestASRCLAddPronunciation::RTestASRCLAddPronunciation()
	{
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ASRCL-U-0011");
	}
//------------------------------------------------------------------

TVerdict RTestASRCLAddPronunciation::DoTestStepL()
/** Adds a pronunciation
 * Use case:  * Training2 - lexicon
 * @test Req. under test 
 */
	{
	TLexiconID lexiconID = KLexiconIDs[KIndex0];
	TModelBankID modelBankID = KModelBankIDs[KIndex0];
	TModelID modelID = 0;
	TPronunciationID pronunciationID;
	
	if (LoadModels(modelBankID)!=EPass)
		return EInconclusive;
		
	if (LoadLexicon(lexiconID)!=EPass)
		return EInconclusive;
	
	if (AddPronunciation( lexiconID, modelBankID, modelID, pronunciationID)!=EPass)
		return EFail;


	lexiconID = KBadLexiconID;
	if (AddPronunciation( lexiconID, modelBankID, modelID, pronunciationID, EFalse)!=EPass)
		return EFail;
		
	return	EPass ;
	}
//------------------------------------------------------------------

RTestASRCLCreateGrammar::RTestASRCLCreateGrammar()
	{
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ASRCL-U-0012");
	}
//------------------------------------------------------------------

TVerdict RTestASRCLCreateGrammar::DoTestStepL()
/** Creates a grammar.
 * Use case:  * Training3 - grammar
 * @test Req. under test 
 */
	{
	TGrammarID grammarID;
	if (CreateGrammar(grammarID)!=EPass)
		return EFail;
	return	EPass ;
	}
//------------------------------------------------------------------

RTestASRCLAddRule::RTestASRCLAddRule()
	{
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ASRCL-U-0013");
	}
//------------------------------------------------------------------

TVerdict RTestASRCLAddRule::DoTestStepL()
/** Adds a rule
 * Use case:  * Training3 - grammar
 * @test Req. under test 
 */
	{
	
	TGrammarID grammarID = KGrammarIDs[KIndex0];

	TLexiconID lexiconID = KLexiconIDs[KIndex1];
	TPronunciationID pronunciationID = 0;
	TRuleID ruleID;
	
	if (LoadGrammar(grammarID)!=EPass)
		return EInconclusive;
	if (LoadLexicon(lexiconID)!=EPass)
		return EInconclusive;
		
	if (AddRule(grammarID, lexiconID, pronunciationID, ruleID, ETrue)!=EPass)
		return EFail;

	grammarID = KBadGrammarID;
	if (AddRule(grammarID, lexiconID, pronunciationID, ruleID, EFalse)!=EPass)
		return EFail;

	return	EPass ;
	}
//------------------------------------------------------------------

RTestASRCLGetRuleValidity::RTestASRCLGetRuleValidity()
	{
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ASRCL-U-0014");
	}
//------------------------------------------------------------------

TVerdict RTestASRCLGetRuleValidity::DoTestStepL()
/** Check a rule
 * Use case:  * Training3 - grammar
 * @test Req. under test 
 */
	{
	TGrammarID grammarID = KGrammarIDs[KIndex1];
	TRuleID ruleID = KRuleIDs[KIndex1][KIndex0].iRuleID;
	
	if (CheckRuleValidity(grammarID,ruleID,ETrue)!=EPass)
		return EFail;

	ruleID = KBadRuleID;
	if (CheckRuleValidity(grammarID,ruleID,ETrue, KErrNotFound)!=EPass)
		return EFail;

	
	//check for invalid rule
	AddRule(grammarID, KBadLexiconID,KBadPronunciationID, ruleID);
	if (CheckRuleValidity(grammarID,ruleID,EFalse)!=EPass)
		return EFail;

	return	EPass ;
	}
//------------------------------------------------------------------

RTestASRCLStartRecSession::RTestASRCLStartRecSession()
	{
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ASRCL-U-0016");
	}
//------------------------------------------------------------------

TVerdict RTestASRCLStartRecSession::DoTestStepL()
/** Starts a recognition session.
 * Use case:  * Recognition1
 * @test Req. under test 
 */
	{
	if (StartRecSession(ESdMode, ETrue)!=EPass)
		return EFail;
	
	TRecognitionMode badMode;
	badMode = (TRecognitionMode)KBadMode;
	if (StartRecSession(badMode, EFalse)!=EPass)
		return EFail;
	return	EPass ;
	}
//------------------------------------------------------------------

RTestASRCLLoadModelBank::RTestASRCLLoadModelBank()
	{
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ASRCL-U-0017");
	}
//------------------------------------------------------------------

TVerdict RTestASRCLLoadModelBank::DoTestStepL()
/** Loads a model bank
 * Use case:  * Recognition1
 * @test Req. under test 
 */
	{
	TModelBankID modelBankID = KModelBankIDs[KIndex1];
	if (LoadModels(modelBankID, ETrue)!=EPass)
		return EFail;
	
	modelBankID = KBadModelID;
	if (LoadModels(modelBankID, EFalse)!=EPass)
		return EFail;
	return	EPass ;
	}
//------------------------------------------------------------------

RTestASRCLLoadLexicon::RTestASRCLLoadLexicon()
	{
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ASRCL-U-0018");
	}
//------------------------------------------------------------------

TVerdict RTestASRCLLoadLexicon::DoTestStepL()
/** Loads a lexicon.
 * Use case:  * Recognition1
 * @test Req. under test 
 */
	{
	TLexiconID lexiconID = KLexiconIDs[KIndex0];
	if (LoadLexicon(lexiconID)!=EPass)
		return EFail;
		
	lexiconID = KBadLexiconID;
	
	if (LoadLexicon(lexiconID, EFalse)!=EPass)
		return EFail;
	return	EPass ;
	}
//------------------------------------------------------------------

RTestASRCLLoadGrammar::RTestASRCLLoadGrammar()
	{
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ASRCL-U-0019");
	}
//------------------------------------------------------------------

TVerdict RTestASRCLLoadGrammar::DoTestStepL()
/** Loads a grammar.
 * Use case:  * Recognition1
 * @test Req. under test 
 */
	{
	TGrammarID grammarID = KGrammarIDs[KIndex1];
	if (LoadGrammar(grammarID)!=EPass)
		return EFail;
		
	grammarID = KBadGrammarID;
	if (LoadGrammar(grammarID, EFalse)!=EPass)
		return EFail;

	return	EPass ;
	}
//------------------------------------------------------------------

RTestASRCLUnloadRule::RTestASRCLUnloadRule()
	{
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ASRCL-U-0020");
	}
//------------------------------------------------------------------

TVerdict RTestASRCLUnloadRule::DoTestStepL()
/** Unloads a rule.
 * Use case:  * Recognition1
 * @test Req. under test 
 */
	{
	TGrammarID grammarID = KGrammarIDs[KIndex0];
	if (LoadGrammar(grammarID)!=EPass)
		return EInconclusive;
		
	TRuleID ruleID = KRuleIDs[KIndex0][KIndex0].iRuleID;
	if (UnloadRule(grammarID,ruleID)!=EPass)
		return EFail;

	return	EPass ;
	}
//------------------------------------------------------------------

RTestASRCLRecognize::RTestASRCLRecognize()
	{
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ASRCL-U-0021");
	}
//------------------------------------------------------------------

TVerdict RTestASRCLRecognize::DoTestStepL()
/** Initiates recognition.
 * Use case:  * Recognition1
 * @test Req. under test 
 */
	{
	TModelBankID modelBankID = KModelBankIDs[KIndex0];
	TGrammarID grammarID = KGrammarIDs[KIndex1];
	TLexiconID lexiconID = KLexiconIDs[KIndex1];

	// start the recognition session
	if (StartRecSession(ESdMode)!=EPass)
		return EInconclusive;
	// load grammar, lexicon and model bank for recognition
	if (LoadGrammar(grammarID)!=EPass)
		return EInconclusive;
	if (LoadLexicon(lexiconID)!=EPass)
		return EInconclusive;
	if (LoadModels(modelBankID)!=EPass)
		return EInconclusive;

		
	CSDClientResultSet* results = CSDClientResultSet::NewL();
	CleanupStack::PushL(results);
	results->SetMaxResultsL(2);
	
	TVerdict result = EPass;
	if (Recognize(*results)!=EPass)
		result = EFail;		
		
	INFO_PRINTF2(_L("Number of results returned %d"),results->ResultCount());
		
	CleanupStack::PopAndDestroy(results);
	
	return result;
	}
//------------------------------------------------------------------

RTestASRCLEndRecSession::RTestASRCLEndRecSession()
	{
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ASRCL-U-0023");
	}
//------------------------------------------------------------------

TVerdict RTestASRCLEndRecSession::DoTestStepL()
/** Terminates the recognition session.
 * Use case:  * Recognition1
 * @test Req. under test 
 */
	{
	if (StartRecSession(ESdMode)!=EPass)
		return EFail;
	if (EndRecSession()!=EPass)
		return EFail;
	return	EPass ;
	}
//------------------------------------------------------------------

RTestASRCLCancel::RTestASRCLCancel()
	{
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ASRCL-U-0024");
	}
//------------------------------------------------------------------

TVerdict RTestASRCLCancel::DoTestStepL()
/** Cancel asynchronous event. 
 * Use case:  * Cancel1
 * @test Req. under test 
 */
	{
	return CancelTest();
	}
//------------------------------------------------------------------

RTestASRCLRemoveRule::RTestASRCLRemoveRule()
	{
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ASRCL-U-0025");
	}
//------------------------------------------------------------------

TVerdict RTestASRCLRemoveRule::DoTestStepL()
/** Removes a rule.
 * Use case:  * Remove1
 * @test Req. under test 
 */
	{
	TGrammarID theGrammarID = KGrammarIDs[KIndex0];
	TRuleID theRuleID = KRuleIDs[KIndex0][KIndex0].iRuleID;
	
	if (RemoveRule( theGrammarID, theRuleID )!=EPass)
		return EFail;
	RArray<TRuleID> ruleIDs;
	TVerdict result = EPass;
	if (GetAllRuleIDs(theGrammarID,ruleIDs)!=EPass)
		result = EInconclusive; // inconclusive, getting the rule ids is just to test the operation of RemoveRule
	if (result == EPass && (ruleIDs.Find(theRuleID)!=-1))
		{
		INFO_PRINTF1(_L("Problem: Rule not removed from grammar"));
		result = EFail;
		}
	else
		INFO_PRINTF1(_L("Rule removed from grammar"));
	
	ruleIDs.Close();
	return result;
	}
//------------------------------------------------------------------

RTestASRCLRemovePronunciation::RTestASRCLRemovePronunciation()
	{
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ASRCL-U-0026");
	}
//------------------------------------------------------------------

TVerdict RTestASRCLRemovePronunciation::DoTestStepL()
/** Removes a pronunciation
 * Use case:  * Remove1
 * @test Req. under test 
 */
	{
	TLexiconID theLexiconID = KLexiconIDs[KIndex1];
	TPronunciationID thePronunciationID = KPronunciationIDs[KIndex1][KIndex0].iPronunciationID;

	if (RemovePronunciation( theLexiconID, thePronunciationID )!=EPass)
		return EFail;
	RArray<TPronunciationID> pronunciationIDs;
	TVerdict result = EPass;
	if (GetAllPronunciationIDs(theLexiconID,pronunciationIDs)!=EPass)
		result = EInconclusive; // inconclusive, getting the pronunciation ids is just to test the operation of RemovePronunciation
		
	if (result == EPass)
		{ 
		if (pronunciationIDs.Find(thePronunciationID)!=KErrNotFound)
			{
			INFO_PRINTF1(_L("Problem: Pronunciation not removed from lexicon"));
			result = EFail;
			}
		else
			INFO_PRINTF1(_L("Pronunciation removed from lexicon"));
		}
		
	pronunciationIDs.Close();
	return result;
	}
//------------------------------------------------------------------

RTestASRCLRemoveGrammar::RTestASRCLRemoveGrammar()
	{
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ASRCL-U-0027");
	}
//------------------------------------------------------------------

TVerdict RTestASRCLRemoveGrammar::DoTestStepL()
/** Removes a grammar
 * Use case:  * Remove1
 * @test Req. under test 
 */
	{
	TGrammarID theGrammarID = KGrammarIDs[0];
	return RemoveGrammar( theGrammarID );
	}
//------------------------------------------------------------------

RTestASRCLRemoveLexicon::RTestASRCLRemoveLexicon()
	{
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ASRCL-U-0028");
	}
//------------------------------------------------------------------

TVerdict RTestASRCLRemoveLexicon::DoTestStepL()
/** Removes a lexicon.
 * Use case:  * Remove1
 * @test Req. under test 
 */
	{
	TLexiconID theLexiconID = KLexiconIDs[KIndex0];
	return RemoveLexicon( theLexiconID );
	}
//------------------------------------------------------------------

RTestASRCLRemoveModel::RTestASRCLRemoveModel()
	{
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ASRCL-U-0029");
	}
//------------------------------------------------------------------

TVerdict RTestASRCLRemoveModel::DoTestStepL()
/** Removes a model.
 * Use case:  * Remove1
 * @test Req. under test 
 */
	{
	TModelBankID theModelBankID = KModelBankIDs[KIndex1];
	TModelID theModelID = KModelIDs[KIndex1][KIndex1].iModelID;
	if (RemoveModel( theModelBankID, theModelID )!=EPass)
		return EFail;
	RArray<TModelID> modelIDs;
	if (GetAllModelIDs(theModelBankID,modelIDs)!=EPass)
		{
		modelIDs.Close();
		return EInconclusive; // inconclusive, getting the model ids is just to test the operation of RemoveModel
		}
	if (modelIDs.Find(theModelID)!=KErrNotFound)
		{
		INFO_PRINTF1(_L("Problem: Model not removed from model bank"));
		modelIDs.Close();
		return EFail;
		}
	modelIDs.Close();
	INFO_PRINTF1(_L("Model removed from model bank"));
	return EPass;	
	}
//------------------------------------------------------------------

RTestASRCLRemoveModelBank::RTestASRCLRemoveModelBank()
	{
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ASRCL-U-0030");
	}
//------------------------------------------------------------------

TVerdict RTestASRCLRemoveModelBank::DoTestStepL()
/** Removes a model bank
 * Use case:  * Remove1
 * @test Req. under test 
 */
	{
	TModelBankID theModelBankID = KModelBankIDs[KIndex1];
	return RemoveModelBank( theModelBankID );
	}
//------------------------------------------------------------------


RTestASRCLGetUtteranceDuration::RTestASRCLGetUtteranceDuration()
	{
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ASRCL-U-0034");
	}
//------------------------------------------------------------------

TVerdict RTestASRCLGetUtteranceDuration::DoTestStepL()
/** Get utterance duration. 
 * Use case:  * Get_Info2
 * @test Req. under test 
 */
	{
	//[ get utterance duration ]
	TModelBankID modelBankId = KModelBankIDs[KIndex0];
	TModelID     modelId     = KModelIDs[KIndex0][KIndex0].iModelID;
	TTimeIntervalMicroSeconds32 expectedDuration = KModelIDs[KIndex0][KIndex0].iUtteranceDurationMicroSeconds;
	TTimeIntervalMicroSeconds32 theDuration = -1;

    TInt err = iSpeechClient->GetUtteranceDuration( modelBankId,modelId,theDuration );
	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("Unexpected failure: %d"),err);
		return EFail;
		}

	if (!WaitForEvent(KUidAsrEventGetUtteranceDuration))
		{
		return EFail;
		}

	INFO_PRINTF2(_L("Utterance Duration: %d"), theDuration.Int() );
	if( theDuration != expectedDuration)
		{
		INFO_PRINTF3(_L("Expected Utterance Duration: %u, received utterance duration %u"), expectedDuration.Int(), theDuration.Int() );
		return EFail;
		}

	INFO_PRINTF1(_L("GetUtteranceDuration Test passed"));
	
	return	EPass ;
	}
//------------------------------------------------------------------

RTestASRCLGetEngineProperties::RTestASRCLGetEngineProperties()
	{
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ASRCL-U-0035");
	}
//------------------------------------------------------------------

TVerdict RTestASRCLGetEngineProperties::DoTestStepL()
/** Get engine properties.
 * Use case:  * Get_Info2
 * @test Req. under test 
 */
	{
	const TInt KNumberEngineProperties = 5;

	TInt err = KErrNone;
	TInt i;
	RArray<TInt> properties;
	RArray<TInt> values;
	
	CleanupClosePushL(properties);
	CleanupClosePushL(values);
	
	for (i=0;i<KNumberEngineProperties;i++)
		User::LeaveIfError(properties.Append(i));
		
	TVerdict result = EPass;
	
	err = iSpeechClient->GetEngineProperties(properties, values);
	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("Unexpected error: %d"),err);
		result = EFail;
		}
		
	if (result == EPass)
		{
		if (values.Count()==properties.Count())
			{
			for (i=0;i<KNumberEngineProperties;i++)
				if (values[i] != i)
					{
					INFO_PRINTF4(_L("%d: expected %d received %d"),i,i,values[i]);
					return EFail;
					}
	
			}
		else
			{
			INFO_PRINTF3(_L("%d Properties requested, but %d values returned"),properties.Count(),values.Count());
			result = EFail;
			}
		}
			
	CleanupStack::PopAndDestroy(2); //properties, values
	return result;
	}
//------------------------------------------------------------------

RTestASRCLLoadEngineParameters::RTestASRCLLoadEngineParameters()
	{
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ASRCL-U-0036");
	}
//------------------------------------------------------------------

TVerdict RTestASRCLLoadEngineParameters::DoTestStepL()
/** Count the number of models.
 * Use case:  * Training1 - model
 * @test Req. under test 
 */
	{
	TInt i;
	RArray<TInt> parameterIDs;
	RArray<TInt> parameterValues;
	CleanupClosePushL(parameterIDs);
	CleanupClosePushL(parameterValues);
	
	for (i=0;i<KNumParameters;i++)
		User::LeaveIfError(parameterIDs.Append(KParameterIDs[i]));
		
	for (i=0;i<KNumParameters;i++)
		User::LeaveIfError(parameterValues.Append(KParameterValues[i]));

	if (LoadEngineParameters(parameterIDs, parameterValues)!=EPass)
		return EFail;
		
	CleanupStack::PopAndDestroy(2, &parameterIDs); // parameterIDs, parameterValues

	return EPass;
	}
//------------------------------------------------------------------

RTestASRCLSetEventHandler::RTestASRCLSetEventHandler()
	{
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ASRCL-U-0037");
	}
//------------------------------------------------------------------

TVerdict RTestASRCLSetEventHandler::DoTestStepL()
/** Count the number of models.
 * Use case:  * Training1 - model
 * @test Req. under test 
 */
	{
	TDummyObserver observer;
	TModelBankID modelBankID = KModelBankIDs[KIndex0];
	
	iSpeechClient->SetEventHandler(&observer);
	if (iSpeechClient->LoadModels(modelBankID)!=KErrNone)
		return EFail;
	observer.WaitForEvent();
	
	if (observer.iEvent != KUidAsrEventLoadModels)
		{
		INFO_PRINTF2(_L("SetEventHandler Test failed, incorrect event %d returned"),observer.iEvent);
		return EFail;
		}
	
	iSpeechClient->SetEventHandler(NULL);	

	INFO_PRINTF1(_L("SetEventHandler Test passed"));
	return	EPass ;
	}
//------------------------------------------------------------------

RTestASRCLNegAsyncCalls::RTestASRCLNegAsyncCalls()
	{
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ASRCL-U-0040");
	iCreateClient = EFalse;
	}
	
//------------------------------------------------------------------

TVerdict RTestASRCLNegAsyncCalls::DoTestStepL()
/** Count the number of models.
 * Use case:  * Training1 - model
 * @test Req. under test 
 */
	{
	TInt grammarIndex = KIndex0;
	TInt lexiconIndex = KIndex1;
	TInt modelBankIndex = KIndex1;
	TGrammarID grammarId = KGrammarIDs[grammarIndex];
	TLexiconID lexiconId = KLexiconIDs[lexiconIndex];
	TModelBankID modelBankId = KModelBankIDs[modelBankIndex];
	
	
	iNegativeAsyncCallTesting = ETrue;
	
	CreateSpeechClientL(KUidTestASRClient);

	//[ load a known lexicon ]
	if( LoadLexicon( lexiconId ) != EPass )
		{
		return EInconclusive;
		}
		
	delete iSpeechClient;
	iSpeechClient = NULL;
	CreateSpeechClientL(KUidTestASRClient);
	
	 //[ load a known model Bank ]
	 if( LoadModels( modelBankId ) != EPass )
		{
		return EFail;
		}
		
	delete iSpeechClient;
	iSpeechClient = NULL;
	CreateSpeechClientL(KUidTestASRClient);
		
	 //[ load a known lexicon ]
	 if( LoadGrammar( grammarId ) != EPass )
		{
		return EFail;
		}
		
	delete iSpeechClient;
	iSpeechClient = NULL;
	CreateSpeechClientL(KUidTestASRClient);

	TVerdict overallVerdict = EPass;
	TVerdict verdict;
	
	RArray <TPronunciationID> pronunciationIDs;
	verdict = GetAllPronunciationIDs( lexiconId, pronunciationIDs);
	
	if (verdict==EFail)
		overallVerdict = EFail;
		
	pronunciationIDs.Close();
	
	delete iSpeechClient;
	iSpeechClient = NULL;
	CreateSpeechClientL(KUidTestASRClient);
		
	RArray <TModelID> modelIDs;
	verdict = GetAllModelIDs( modelBankId, modelIDs);

	if (verdict==EFail)
		overallVerdict = EFail;
		
	modelIDs.Close();
	
	delete iSpeechClient;
	iSpeechClient = NULL;
	CreateSpeechClientL(KUidTestASRClient);

		
	RArray <TRuleID> ruleIDs;
	verdict = GetAllRuleIDs(grammarId, ruleIDs);

	if (verdict==EFail)
		overallVerdict = EFail;
		
	ruleIDs.Close();
	
	delete iSpeechClient;
	iSpeechClient = NULL;
	CreateSpeechClientL(KUidTestASRClient);

		
	CSDClientResultSet* resultSet = CSDClientResultSet::NewL();
	CleanupStack::PushL(resultSet);
	
	resultSet->SetMaxResultsL(2);
	
	
	// start the recognition session
	verdict = StartRecSession(ESdMode);
	
	if (verdict==EFail)
		overallVerdict = EFail;
	
	verdict = Recognize(*resultSet);
	
	if (verdict==EFail)
		overallVerdict = EFail;
		
	CleanupStack::PopAndDestroy(resultSet);	

		
	delete iSpeechClient;
	iSpeechClient = NULL;
	CreateSpeechClientL(KUidTestASRClient);

		
	verdict = RemoveRule(grammarId,KRuleIDs[grammarIndex][KIndex0].iRuleID);

	if (verdict==EFail)
		overallVerdict = EFail;
		
	delete iSpeechClient;
	iSpeechClient = NULL;
	CreateSpeechClientL(KUidTestASRClient);


		
	verdict = RemoveGrammar(grammarId);

	if (verdict==EFail)
		overallVerdict = EFail;
		
	delete iSpeechClient;
	iSpeechClient = NULL;
	CreateSpeechClientL(KUidTestASRClient);

		
	verdict = RemovePronunciation(lexiconId,KPronunciationIDs[lexiconIndex][KIndex0].iPronunciationID);

	if (verdict==EFail)
		overallVerdict = EFail;
		
	delete iSpeechClient;
	iSpeechClient = NULL;
	CreateSpeechClientL(KUidTestASRClient);



	verdict = RemoveLexicon(lexiconId);

	if (verdict==EFail)
		overallVerdict = EFail;
		
	delete iSpeechClient;
	iSpeechClient = NULL;
	CreateSpeechClientL(KUidTestASRClient);

		
		
	verdict = RemoveModel(modelBankId,KModelIDs[modelBankIndex][KIndex0].iModelID);

	if (verdict==EFail)
		overallVerdict = EFail;
		
	delete iSpeechClient;
	iSpeechClient = NULL;
	CreateSpeechClientL(KUidTestASRClient);


		
	verdict = RemoveModelBank(modelBankId);

	if (verdict==EFail)
		overallVerdict = EFail;
		
	delete iSpeechClient;
	iSpeechClient = NULL;
	CreateSpeechClientL(KUidTestASRClient);

	delete iSpeechClient;
	iSpeechClient = NULL;

		
	return overallVerdict;
	}
	
//------------------------------------------------------------------
RTestASRCLGetAllLexiconIDs::RTestASRCLGetAllLexiconIDs()
	{
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ASRCL-U-0051");
	iCreateClient = EFalse;
	}
//------------------------------------------------------------------

TVerdict RTestASRCLGetAllLexiconIDs::DoTestStepL()
/** Get all lexicon IDs
 * Use case:  * Get_Info1
 * @test Req. under test 
 */
	{
	const TInt KNumberAddedLexicons = 4;
	
	CreateSpeechClientL(KUidTestASRClient2);
	
    TVerdict result = EPass;
    TLexiconID addedLexicons[KNumberAddedLexicons];
    for (TInt i=0;i< KNumberAddedLexicons;i++)
    	{
	    if (CreateLexicon(addedLexicons[i])!=EPass)
    		return EInconclusive;
		}
    
	RArray <TLexiconID> lexiconIDs;
	if (GetAllLexiconIDs(lexiconIDs, ETrue)!=EPass)
		{
		result = EFail;
		}
	
	//[ check that the id's contained in the list 
	// conform to those added to the lexicon in the controller ]
    if ((result == EPass) && (lexiconIDs.Count() != KNumberAddedLexicons))
    	{
    	INFO_PRINTF3(_L("Expected %d lexicons, received %d lexicons"), KNumberAddedLexicons, lexiconIDs.Count());
    	result = EFail;
    	}

    if (result == EPass)
    	{
		INFO_PRINTF2(_L("Lexicon Ids Count: %d"),lexiconIDs.Count() );
	    for( TInt index = 0; index < lexiconIDs.Count() && result == EPass; index++ )
			{
	    	if (lexiconIDs[index]!=addedLexicons[index])
	    		{
	    		INFO_PRINTF3(_L("failed - expected lexicon id: %d, received lexicon id: %d"),KLexiconIDs[KIndex0],lexiconIDs[index]);
		    	result = EFail;	    	
		    	}
			}
			
		
		}

	lexiconIDs.Close();
	return result;
	}


//------------------------------------------------------------------

RTestASRCLGetAllGrammarIDs::RTestASRCLGetAllGrammarIDs()
	{
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ASRCL-U-0052");
	iCreateClient = EFalse;
	}
//------------------------------------------------------------------

TVerdict RTestASRCLGetAllGrammarIDs::DoTestStepL()
/** Get all grammar IDs
 * Use case:  * Get_Info1
 * @test Req. under test 
 */
	{
	const TInt KNumberAddedGrammars = 4;
	
	CreateSpeechClientL(KUidTestASRClient2);
	
    TVerdict result = EPass;
    TGrammarID addedGrammars[KNumberAddedGrammars];
    for (TInt i=0;i<KNumberAddedGrammars;i++)
    	{
	    if (CreateGrammar(addedGrammars[i])!=EPass)
    		return EInconclusive;
		}
    
	RArray <TGrammarID> grammarIDs;
	if (GetAllGrammarIDs(grammarIDs, ETrue)!=EPass)
		{
		result = EFail;
		}
	
	//[ check that the id's contained in the list 
	// conform to those added to the grammar in the controller ]
    if ((result == EPass) && (grammarIDs.Count() != KNumberAddedGrammars))
    	{
    	INFO_PRINTF3(_L("Expected %d grammars, received %d grammars"), KNumberAddedGrammars, grammarIDs.Count());
    	result = EFail;
    	}

    if (result == EPass)
    	{
		INFO_PRINTF2(_L("Grammar Ids Count: %d"),grammarIDs.Count() );
	    for( TInt index = 0; index < grammarIDs.Count() && result == EPass; index++ )
			{
	    	if (grammarIDs[index]!=addedGrammars[index])
	    		{
	    		INFO_PRINTF3(_L("failed - expected grammar id: %d, received grammar id: %d"),KGrammarIDs[KIndex0],grammarIDs[index]);
		    	result = EFail;	    	
		    	}
			}
			
		
		}

	grammarIDs.Close();
	return result;
	}
//------------------------------------------------------------------

RTestASRCLGetAllModelBankIDs::RTestASRCLGetAllModelBankIDs()
	{
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ASRCL-U-0053");
	iCreateClient = EFalse;
	}
//------------------------------------------------------------------

TVerdict RTestASRCLGetAllModelBankIDs::DoTestStepL()
/** Get all model IDs
 * Use case:  * Get_Info1
 * @test Req. under test 
 */
	{
	const TInt KNumberAddedModelBanks = 2;
	
	CreateSpeechClientL(KUidTestASRClient2);
	
	
	
	// train some new modelBanks with the new client ID
    TVerdict result = EPass;
    TModelID addedModelBanks[KNumberAddedModelBanks];
    for (TInt i=0;i<KNumberAddedModelBanks;i++)
    	{
    	CreateModelBank(addedModelBanks[i]);
		}
    
	RArray <TModelBankID> modelBankIDs;
	if (GetAllModelBankIDs( modelBankIDs, ETrue)!=EPass)
		{
		result = EFail;
		}
	
	// check that the IDs in the list are the ones just trained
    if ((result == EPass) && (modelBankIDs.Count() != KNumberAddedModelBanks))
    	{
    	INFO_PRINTF3(_L("Expected %d modelBanks, received %d modelBanks"), KNumberAddedModelBanks, modelBankIDs.Count());
    	result = EFail;
    	}

    if (result == EPass)
    	{
		INFO_PRINTF2(_L("Model Ids Count: %d"),modelBankIDs.Count() );
	    for( TInt index = 0; index < modelBankIDs.Count() && result == EPass; index++ )
			{
	    	if (modelBankIDs[index]!=addedModelBanks[index])
	    		{
	    		INFO_PRINTF3(_L("failed - expected model id: %d, received model id: %d"),KModelBankIDs[KIndex0],modelBankIDs[index]);
		    	result = EFail;	    	
		    	}
			}
			
		
		}

	modelBankIDs.Close();
	return result;
	}

	

//------------------------------------------------------------------
RTestASRCLGetAllClientLexiconIDs::RTestASRCLGetAllClientLexiconIDs()
	{
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ASRCL-U-0054");
	iCreateClient = EFalse;
	}
//------------------------------------------------------------------

TVerdict RTestASRCLGetAllClientLexiconIDs::DoTestStepL()
/** Get all lexicon IDs
 * Use case:  * Get_Info1
 * @test Req. under test 
 */
	{
	const TInt KNumberAddedLexicons = 4;
	
	CreateSpeechClientL(KUidTestASRClient2);
	
    TVerdict result = EPass;
    TLexiconID addedLexicons[KNumberAddedLexicons];
    for (TInt i=0;i<4;i++)
    	{
	    if (CreateLexicon(addedLexicons[i])!=EPass)
    		return EInconclusive;
		}
    
	RArray <TLexiconID> lexiconIDs;
	if (GetAllLexiconIDs(lexiconIDs, ETrue)!=EPass)
		{
		result = EFail;
		}
	
	//[ check that the id's contained in the list 
	// conform to those added to the lexicon in the controller ]
    if ((result == EPass) && (lexiconIDs.Count() != KNumberAddedLexicons))
    	{
    	INFO_PRINTF3(_L("Expected %d lexicons, received %d lexicons"), KNumberAddedLexicons, lexiconIDs.Count());
    	result = EFail;
    	}

    if (result == EPass)
    	{
		INFO_PRINTF2(_L("Lexicon Ids Count: %d"),lexiconIDs.Count() );
	    for( TInt index = 0; index < lexiconIDs.Count() && result == EPass; index++ )
			{
	    	if (lexiconIDs[index]!=addedLexicons[index])
	    		{
	    		INFO_PRINTF3(_L("failed - expected lexicon id: %d, received lexicon id: %d"),KLexiconIDs[KIndex0],lexiconIDs[index]);
		    	result = EFail;	    	
		    	}
			}
			
		
		}

	lexiconIDs.Close();
	return result;
	}
//------------------------------------------------------------------

RTestASRCLGetAllClientGrammarIDs::RTestASRCLGetAllClientGrammarIDs()
	{
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ASRCL-U-0055");
	iCreateClient = EFalse;
	}
//------------------------------------------------------------------

TVerdict RTestASRCLGetAllClientGrammarIDs::DoTestStepL()
/** Get all grammar IDs
 * Use case:  * Get_Info1
 * @test Req. under test 
 */
	{
	const TInt KNumberAddedGrammars = 4;
	
	CreateSpeechClientL(KUidTestASRClient2);
	
    TVerdict result = EPass;
    TGrammarID addedGrammars[KNumberAddedGrammars];
    for (TInt i=0;i<4;i++)
    	{
	    if (CreateGrammar(addedGrammars[i])!=EPass)
    		return EInconclusive;
		}
    
	RArray <TGrammarID> grammarIDs;
	if (GetAllGrammarIDs(grammarIDs, ETrue)!=EPass)
		{
		result = EFail;
		}
	
	//[ check that the id's contained in the list 
	// conform to those added to the grammar in the controller ]
    if ((result == EPass) && (grammarIDs.Count() != KNumberAddedGrammars))
    	{
    	INFO_PRINTF3(_L("Expected %d grammars, received %d grammars"), KNumberAddedGrammars, grammarIDs.Count());
    	result = EFail;
    	}

    if (result == EPass)
    	{
		INFO_PRINTF2(_L("Grammar Ids Count: %d"),grammarIDs.Count() );
	    for( TInt index = 0; index < grammarIDs.Count() && result == EPass; index++ )
			{
	    	if (grammarIDs[index]!=addedGrammars[index])
	    		{
	    		INFO_PRINTF3(_L("failed - expected grammar id: %d, received grammar id: %d"),KGrammarIDs[KIndex0],grammarIDs[index]);
		    	result = EFail;	    	
		    	}
			}
			
		
		}

	grammarIDs.Close();
	return result;
	}
//------------------------------------------------------------------

RTestASRCLGetAllClientModelBankIDs::RTestASRCLGetAllClientModelBankIDs()
	{
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ASRCL-U-0056");
	iCreateClient = EFalse;
	}
//------------------------------------------------------------------

TVerdict RTestASRCLGetAllClientModelBankIDs::DoTestStepL()
/** Get all model IDs
 * Use case:  * Get_Info1
 * @test Req. under test 
 */
	{
	const TInt KNumberAddedModelBanks = 2;
	
	CreateSpeechClientL(KUidTestASRClient2);
	
	
	// train some new modelBanks with the new client ID
    TVerdict result = EPass;
    TModelID addedModelBanks[KNumberAddedModelBanks];
    for (TInt i=0;i<KNumberAddedModelBanks;i++)
    	{
    	CreateModelBank(addedModelBanks[i]);
		}
    
	RArray <TModelID> modelBankIDs;
	if (GetAllModelBankIDs( modelBankIDs, ETrue)!=EPass)
		{
		result = EFail;
		}
	
	// check that the IDs in the list are the ones just trained
    if ((result == EPass) && (modelBankIDs.Count() != KNumberAddedModelBanks))
    	{
    	INFO_PRINTF3(_L("Expected %d modelBanks, received %d modelBanks"), KNumberAddedModelBanks, modelBankIDs.Count());
    	result = EFail;
    	}

    if (result == EPass)
    	{
		INFO_PRINTF2(_L("Model Ids Count: %d"),modelBankIDs.Count() );
	    for( TInt index = 0; index < modelBankIDs.Count() && result == EPass; index++ )
			{
	    	if (modelBankIDs[index]!=addedModelBanks[index])
	    		{
	    		INFO_PRINTF3(_L("failed - expected model id: %d, received model id: %d"),KModelBankIDs[KIndex0],modelBankIDs[index]);
		    	result = EFail;	    	
		    	}
			}
			
		
		}

	modelBankIDs.Close();
	return result;
	}
	
//------------------------------------------------------------------
RTestASRCLGetAllPronunciationIDsAlloc::RTestASRCLGetAllPronunciationIDsAlloc()
	{
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ASRCL-U-0061");
	iCreateClient = EFalse;
	}
//------------------------------------------------------------------

TVerdict RTestASRCLGetAllPronunciationIDsAlloc::DoTestStepL()
/** Get all pronunciation IDs
 * Use case:  * Get_Info1
 * @test Req. under test 
 */
	{
	TInt failCount = 1;
	TInt err = KErrNoMemory;
	TBool completed = EFalse;
	TBool badResult = EFalse; 
	TVerdict getPronunciationIDsResult = EPass;
	TBool heapTestComplete = EFalse;
	
	TLexiconID lexiconId = KLexiconIDs[KIndex0];
	RArray <TPronunciationID> pronunciationIDs;

	__MM_HEAP_MARK;
	
	for (;;)
		{
		__UHEAP_SETFAIL(RHeap::EFailNext ,failCount);
		
		completed = EFalse;
	
		err = GetTestResults(lexiconId, pronunciationIDs, getPronunciationIDsResult);
	
		if ((getPronunciationIDsResult != EPass) || 
			((err != KErrNoMemory) && (err != KErrNone))) 
			{
			badResult = ETrue;	
			}
						
		TAny* testAlloc = User::Alloc(1);
		if (NULL == testAlloc)
			{
			heapTestComplete = ETrue;
			failCount -= 1;
			}
		else
			{
			User::Free(testAlloc);
			}
		
		completed = badResult || heapTestComplete;	
		
		
		__UHEAP_SETFAIL(RHeap::ENone ,0);	
	
		if (completed)
			{
			break;
			}
		
		failCount++;
		}
		
	
	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("Alloc testing on GetAllPronunciationIDs passed after %d memory allocations"),failCount-1);
		getPronunciationIDsResult = EFail;
		}
	else
		{
		INFO_PRINTF2(_L("Alloc testing on GetAllPronunciationIDs passed after %d memory allocations"),failCount-1);
		}
	
	//[ check that the id's contained in the list 
	// conform to those added to the lexicon in the controller ]
    if ((getPronunciationIDsResult == EPass) && (pronunciationIDs.Count() != KNumPronunciations))
    	{
    	INFO_PRINTF3(_L("Expected %d pronunciations, received %d pronunciations"), KNumPronunciations, pronunciationIDs.Count());
    	getPronunciationIDsResult = EFail;
    	}

    if (getPronunciationIDsResult == EPass)
    	{
		INFO_PRINTF2(_L("Pronunciation Ids Count: %d"),pronunciationIDs.Count() );
	    for( TInt index = 0; index < pronunciationIDs.Count() && getPronunciationIDsResult == EPass; index++ )
			{
	    	if (pronunciationIDs[index]!=KPronunciationIDs[KIndex0][index].iPronunciationID)
	    		{
	    		INFO_PRINTF3(_L("failed - expected pronunciation id: %d, received pronunciation id: %d"),KPronunciationIDs[KIndex0][index],pronunciationIDs[index]);
		    	getPronunciationIDsResult = EFail;	    	
		    	}
			}
			
		
		}
	pronunciationIDs.Close();

	__MM_HEAP_MARKEND;
	
	return getPronunciationIDsResult;
	}
		
	
TInt RTestASRCLGetAllPronunciationIDsAlloc::GetTestResults(TLexiconID& aLexiconId, RArray <TPronunciationID>& aPronunciationIDs, TVerdict& aResult)
	{
	
	TInt err = KErrNone;
	
	TRAP(err, CreateSpeechClientL(KUidTestASRClient));
	
	if (err == KErrNone)
		{
		err = iSpeechClient->GetAllPronunciationIDs( aLexiconId, aPronunciationIDs);
		}
		
	
	if (err == KErrNone)
		{
		CActiveScheduler::Start();			
		if (iLastEvent != KUidAsrEventGetAllPronunciationIDs)
			{
			aResult = EFail;
			}
		else
			{
			//Checks the results to make sure we did not miss an alloc failure 
			if ((iLastError == KErrNone) && (aPronunciationIDs.Count() != KNumPronunciations))
				{
				INFO_PRINTF3(_L("Expected %d pronunciations, received %d pronunciations"), KNumPronunciations, aPronunciationIDs.Count());
				aResult = EFail;
				}
			}
			
		if (iLastError != KErrNone)
			{
			err = iLastError;
			}
		}

	delete iSpeechClient;
	iSpeechClient=NULL;
	   	
	return err;
	}

//------------------------------------------------------------------
RTestASRCLGetAllModelIDsAlloc::RTestASRCLGetAllModelIDsAlloc()
	{
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ASRCL-U-0062");
	iCreateClient = EFalse;
	}
//------------------------------------------------------------------

TVerdict RTestASRCLGetAllModelIDsAlloc::DoTestStepL()
/** Get all model IDs
 * Use case:  * Get_Info1
 * @test Req. under test 
 */
	{
	TInt failCount = 1;
	TInt err = KErrNoMemory;
	
	TModelBankID modelBankId = KModelBankIDs[KIndex0];
	RArray <TModelID> modelIDs;
	
	TBool completed = EFalse;
	TBool badResult = EFalse; 
	TVerdict getModelIDsResult = EPass;
	TBool heapTestComplete = EFalse;

	__MM_HEAP_MARK;
	
	for (;;)
		{
		__UHEAP_SETFAIL(RHeap::EFailNext ,failCount);
		
		completed = EFalse;
		
		err = GetTestResults(modelBankId, modelIDs, getModelIDsResult);
		
		if ((getModelIDsResult != EPass)|| 
			((err != KErrNoMemory) && (err != KErrNone)))
			{
			badResult = ETrue;
			}
				
		TAny* testAlloc = User::Alloc(1);
		if (NULL == testAlloc)
			{
			heapTestComplete = ETrue;
			failCount -= 1;
			}
		else
			{
			User::Free(testAlloc);
			}
		
		completed = badResult || heapTestComplete;	
							
		__UHEAP_SETFAIL(RHeap::ENone ,0);	
		
		if (completed)
			{
			break;
			}
			
		failCount++;
		}

	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("Alloc testing on GetAllModelIDs passed after %d memory allocations"),failCount-1);
		getModelIDsResult = EFail;
		}
	else
		{
		INFO_PRINTF2(_L("Alloc testing on GetAllModelIDs passed after %d memory allocations"),failCount-1);
		}
	
	//[ check that the id's contained in the list 
	// conform to those added to the modelBank in the controller ]
    if ((getModelIDsResult == EPass) && (modelIDs.Count() != KNumModels))
    	{
    	INFO_PRINTF3(_L("Expected %d models, received %d models"), KNumModels, modelIDs.Count());
    	getModelIDsResult = EFail;
    	}

    if (getModelIDsResult == EPass)
    	{
		INFO_PRINTF2(_L("Model Ids Count: %d"),modelIDs.Count() );
	    for( TInt index = 0; index < modelIDs.Count() && getModelIDsResult == EPass; index++ )
			{
	    	if (modelIDs[index]!=KModelIDs[KIndex0][index].iModelID)
	    		{
	    		INFO_PRINTF3(_L("failed - expected model id: %d, received model id: %d"),KModelIDs[KIndex0][index],modelIDs[index]);
		    	getModelIDsResult = EFail;	    	
		    	}
			}
			
		
		}
	modelIDs.Close();

	__MM_HEAP_MARKEND;
	
	return getModelIDsResult;
	}


TInt RTestASRCLGetAllModelIDsAlloc::GetTestResults(TModelBankID& aModelBankId, RArray <TModelID>& aModelIDs, TVerdict& aResult)
	{
	
	TInt err = KErrNone;
	
	TRAP(err, CreateSpeechClientL(KUidTestASRClient));
	
	if (err == KErrNone)
		{
		err = iSpeechClient->GetAllModelIDs( aModelBankId, aModelIDs);
		}
		
	if (err == KErrNone)
		{
		CActiveScheduler::Start();			
		if (iLastEvent != KUidAsrEventGetAllModelIDs)
			{
			aResult = EFail;
			}
		else 
			{
			//Checks the results to make sure we did not miss an alloc failure 
			if ((iLastError == KErrNone) && (aModelIDs.Count() != KNumModels))
    			{
    			INFO_PRINTF3(_L("Expected %d models, received %d models"), KNumModels, aModelIDs.Count());
    			aResult = EFail;
    			}
			}
    	
    	if (iLastError != KErrNone)
			{
			err = iLastError;
			}
		}

	delete iSpeechClient;
	iSpeechClient=NULL;	
	
	return err;
	}
//------------------------------------------------------------------
RTestASRCLGetAllRuleIDsAlloc::RTestASRCLGetAllRuleIDsAlloc()
	{
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ASRCL-U-0063");
	iCreateClient = EFalse;
	}
//------------------------------------------------------------------

TVerdict RTestASRCLGetAllRuleIDsAlloc::DoTestStepL()
/** Get all rule IDs
 * Use case:  * Get_Info1
 * @test Req. under test 
 */
	{
	TInt failCount = 1;
	TInt err = KErrNoMemory;
	
	TGrammarID grammarId = KGrammarIDs[KIndex0];

	RArray <TRuleID> ruleIDs;
	TBool completed = EFalse;
	TBool badResult = EFalse; 
	TVerdict getRuleIDsResult = EPass;	// the test result
	TBool heapTestComplete = EFalse;

	__MM_HEAP_MARK;
	
	for (;;)
		{
		__UHEAP_SETFAIL(RHeap::EFailNext ,failCount);
		
		completed = EFalse;
	
		err = GetTestResults(grammarId, ruleIDs, getRuleIDsResult);
	
		if ((getRuleIDsResult != EPass)|| 
			((err != KErrNoMemory) && (err != KErrNone)))
			{
			badResult = ETrue;	
			}
				
		TAny* testAlloc = User::Alloc(1);
		if (NULL == testAlloc)
			{
			heapTestComplete = ETrue;
			failCount -= 1;
			}
		else
			{
			User::Free(testAlloc);
			}
			
		completed = badResult || heapTestComplete;	
					
		__UHEAP_SETFAIL(RHeap::ENone ,0);	
	
		if (completed)
			{
			break;
			}
		
		failCount++;
		}
	
	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("Alloc testing on GetAllRuleIDs passed after %d memory allocations"),failCount-1);
		getRuleIDsResult = EFail;
		}
	else
		{
		INFO_PRINTF2(_L("Alloc testing on GetAllRuleIDs passed after %d memory allocations"),failCount-1);
		}
	
	//[ check that the id's contained in the list 
	// conform to those added to the grammar in the controller ]
    if ((getRuleIDsResult == EPass) && (ruleIDs.Count() != KNumRules))
    	{
    	INFO_PRINTF3(_L("Expected %d rule, received %d rules"), KNumRules, ruleIDs.Count());
    	getRuleIDsResult = EFail;
    	}

    if (getRuleIDsResult == EPass)
    	{
		INFO_PRINTF2(_L("Rule Ids Count: %d"),ruleIDs.Count() );
	    for( TInt index = 0; index < ruleIDs.Count() && getRuleIDsResult == EPass; index++ )
			{
	    	if (ruleIDs[index]!=KRuleIDs[KIndex0][index].iRuleID)
	    		{
	    		INFO_PRINTF3(_L("failed - expected rule id: %d, received rule id: %d"),KRuleIDs[KIndex0][index],ruleIDs[index]);
		    	getRuleIDsResult = EFail;	    	
		    	}
			}
			
		
		}
	ruleIDs.Close();

	__MM_HEAP_MARKEND;
	
	return getRuleIDsResult;
	}

	
TInt RTestASRCLGetAllRuleIDsAlloc::GetTestResults(TGrammarID& aGrammarId, RArray <TRuleID>& aRuleIDs, TVerdict& aResult)
	{
	TInt err = KErrNone;
	
	TRAP(err, CreateSpeechClientL(KUidTestASRClient));
	
	if (err == KErrNone)
		{
		err = iSpeechClient->GetAllRuleIDs(aGrammarId, aRuleIDs);
		}
			
	if (err == KErrNone)
		{
		CActiveScheduler::Start();			
		if (iLastEvent != KUidAsrEventGetAllRuleIDs)
			{
			aResult = EFail;
			}
		else 
			{
			if ((iLastError == KErrNone) && (aRuleIDs.Count() != KNumRules))
    			{
	    		INFO_PRINTF3(_L("Expected %d rule, received %d rules"), KNumRules, aRuleIDs.Count());
    			aResult = EFail;
    			}
			}
			
		if (iLastError != KErrNone)
			{
			err = iLastError;
			}
		}
	
	delete iSpeechClient;
	iSpeechClient=NULL;

	return err;
	}
//------------------------------------------------------------------
RTestASRCLGetAllGrammarIDsAlloc::RTestASRCLGetAllGrammarIDsAlloc()
	{
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ASRCL-U-0064");
	iCreateClient = EFalse;
	}
//------------------------------------------------------------------

TVerdict RTestASRCLGetAllGrammarIDsAlloc::DoTestStepL()
/** Get all grammar IDs
 * Use case:  * Get_Info1
 * @test Req. under test 
 */
	{
	TInt failCount = 1;
	TInt err = KErrNoMemory;
	TBool completed = EFalse;
	TBool badResult = EFalse; 
	TVerdict getGrammarIDsResult = EPass;	// the test result
	TBool heapTestComplete = EFalse;
	
	RArray <TGrammarID> grammarIDs;
	
	__MM_HEAP_MARK;
		
	for (;;)
		{
		
		__UHEAP_SETFAIL(RHeap::EFailNext ,failCount);
		
		completed = EFalse;
		
		err = GetTestResults(grammarIDs, getGrammarIDsResult);
	
		if ((getGrammarIDsResult != EPass)|| 
			((err != KErrNoMemory) && (err != KErrNone)))
			{
			badResult = ETrue;	
			}
		
		TAny* testAlloc = User::Alloc(1);
		if (NULL == testAlloc)
			{
			heapTestComplete = ETrue;
			failCount -= 1;
			}
		else
			{
			User::Free(testAlloc);
			}
			
		completed = badResult || heapTestComplete;	
					
		__UHEAP_SETFAIL(RHeap::ENone ,0);	
		
		if (completed)
			{
			break;
			}
			
		failCount++;
		}
	
	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("Alloc testing on GetAllGrammarIDs passed after %d memory allocations"),failCount-1);
		getGrammarIDsResult = EFail;
		}
	else
		{
		INFO_PRINTF2(_L("Alloc testing on GetAllGrammarIDs passed after %d memory allocations"),failCount-1);
		}
	
	//[ check that the id's contained in the list 
	// conform to those added to the grammar in the controller ]
    if ((getGrammarIDsResult == EPass) && (grammarIDs.Count() != KNumGrammars))
    	{
    	INFO_PRINTF3(_L("Expected %d grammars, received %d grammars"), KNumGrammars, grammarIDs.Count());
    	getGrammarIDsResult = EFail;
    	}

    if (getGrammarIDsResult == EPass)
    	{
		INFO_PRINTF2(_L("Grammar Ids Count: %d"),grammarIDs.Count() );
	    for( TInt index = 0; index < grammarIDs.Count() && getGrammarIDsResult == EPass; index++ )
			{
	    	if (grammarIDs[index]!=KGrammarIDs[index])
	    		{
	    		INFO_PRINTF3(_L("failed - expected grammar id: %d, received grammar id: %d"),KGrammarIDs[index],grammarIDs[index]);
		    	getGrammarIDsResult = EFail;	    	
		    	}
			}
			
		
		}
	grammarIDs.Close();

	__MM_HEAP_MARKEND;
	
	return getGrammarIDsResult;
	}
	
	
	
TInt RTestASRCLGetAllGrammarIDsAlloc::GetTestResults(RArray <TGrammarID>& aGrammarIDs, TVerdict& aResult)
	{
	TInt err = KErrNone;
	
	TRAP(err, CreateSpeechClientL(KUidTestASRClient));
	
	if (err == KErrNone)
		{
		err = iSpeechClient->GetAllGrammarIDs(aGrammarIDs);
		}
	 
	if (err == KErrNone)
		{
		CActiveScheduler::Start();			
		if (iLastEvent != KUidAsrEventGetAllGrammarIDs)
			{
			aResult = EFail;
			}
		else
			{
			//Checks the results to make sure we did not miss an alloc failure 
			if ((iLastError == KErrNone) && (aGrammarIDs.Count() != KNumGrammars))
    			{
    			INFO_PRINTF3(_L("Expected %d grammars, received %d grammars"), KNumGrammars, aGrammarIDs.Count());
    			aResult = EFail;
    			}
			}
			
		if (iLastError != KErrNone)
			{
			err = iLastError;
			}
			
		}

	delete iSpeechClient;
	iSpeechClient=NULL;
	
	return err;
	}

//------------------------------------------------------------------
RTestASRCLGetAllLexiconIDsAlloc::RTestASRCLGetAllLexiconIDsAlloc()
	{
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ASRCL-U-0065");
	iCreateClient = EFalse;
	}
//------------------------------------------------------------------

TVerdict RTestASRCLGetAllLexiconIDsAlloc::DoTestStepL()
/** Get all lexicon IDs
 * Use case:  * Get_Info1
 * @test Req. under test 
 */
	{
	TInt failCount = 1;
	TInt err = KErrNoMemory;
	TBool completed = EFalse;
	TBool badResult = EFalse; 
	TVerdict getLexiconIDsResult = EPass;	// the test result
	TBool heapTestComplete = EFalse;
	
	RArray <TLexiconID> lexiconIDs;


	__MM_HEAP_MARK;
	
	for (;;)
		{
		
		__UHEAP_SETFAIL(RHeap::EFailNext ,failCount);
		
		completed = EFalse;
		
		err = GetTestResults(lexiconIDs, getLexiconIDsResult);
	
		if ((getLexiconIDsResult != EPass)|| 
			((err != KErrNoMemory) && (err != KErrNone)))
			{
			badResult = ETrue;	
			}
					
		TAny* testAlloc = User::Alloc(1);
		if (NULL == testAlloc)
			{
			heapTestComplete = ETrue;
			failCount -= 1;
			}
		else
			{
			User::Free(testAlloc);
			}
			
		completed = badResult || heapTestComplete;	
						
		__UHEAP_SETFAIL(RHeap::ENone ,0);	
		
		if (completed)
			{
			break;
			}
			
		failCount++;
		}

	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("Alloc testing on GetAllLexiconIDs passed after %d memory allocations"),failCount-1);
		getLexiconIDsResult = EFail;
		}
	else
		{
		INFO_PRINTF2(_L("Alloc testing on GetAllLexiconIDs passed after %d memory allocations"),failCount-1);
		}
	
	//[ check that the id's contained in the list 
	// conform to those added to the lexicon in the controller ]
    if ((getLexiconIDsResult == EPass) && (lexiconIDs.Count() != KNumLexicons))
    	{
    	INFO_PRINTF3(_L("Expected %d lexicons, received %d lexicons"), KNumLexicons, lexiconIDs.Count());
    	getLexiconIDsResult = EFail;
    	}

    if (getLexiconIDsResult == EPass)
    	{
		INFO_PRINTF2(_L("Lexicon Ids Count: %d"),lexiconIDs.Count() );
	    for( TInt index = 0; index < lexiconIDs.Count() && getLexiconIDsResult == EPass; index++ )
			{
	    	if (lexiconIDs[index]!=KLexiconIDs[index])
	    		{
	    		INFO_PRINTF3(_L("failed - expected lexicon id: %d, received lexicon id: %d"),KLexiconIDs[index],lexiconIDs[index]);
		    	getLexiconIDsResult = EFail;	    	
		    	}
			}
			
		
		}
	lexiconIDs.Close();

	__MM_HEAP_MARKEND;
	
	return getLexiconIDsResult;
	}


TInt RTestASRCLGetAllLexiconIDsAlloc::GetTestResults(RArray <TLexiconID>& aLexiconIDs, TVerdict& aResult)
	{
	TInt err = KErrNone;
	
	TRAP(err, CreateSpeechClientL(KUidTestASRClient));
	
	if (err == KErrNone)
		{
		err = iSpeechClient->GetAllLexiconIDs(aLexiconIDs);
		}
	
	if (err == KErrNone)
		{
		CActiveScheduler::Start();			
		if (iLastEvent != KUidAsrEventGetAllLexiconIDs)
			{
			aResult = EFail;
			}
		else
			{
			//Checks the results to make sure we did not miss an alloc failure 
			if ((iLastError == KErrNone) && (aLexiconIDs.Count() != KNumLexicons))
    			{
    			INFO_PRINTF3(_L("Expected %d lexicons, received %d lexicons"), KNumLexicons, aLexiconIDs.Count());
    			aResult = EFail;
    			}
			}
				
		if (iLastError != KErrNone)		
			{
			err = iLastError;	
			}
			
		}

	delete iSpeechClient;
	iSpeechClient = NULL;
		
	return err;
	}
//------------------------------------------------------------------
RTestASRCLGetAllModelBankIDsAlloc::RTestASRCLGetAllModelBankIDsAlloc()
	{
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ASRCL-U-0066");
	iCreateClient = EFalse;
	}
//------------------------------------------------------------------

TVerdict RTestASRCLGetAllModelBankIDsAlloc::DoTestStepL()
/** Get all modelBank IDs
 * Use case:  * Get_Info1
 * @test Req. under test 
 */
	{
	
	TInt failCount = 1;
	TInt err = KErrNoMemory;
	TBool completed = EFalse;
	TBool badResult = EFalse; 
	TVerdict getModelBankIDsResult = EPass;	// the test result
	TBool heapTestComplete = EFalse;
	
	RArray <TModelBankID> modelBankIDs;
	
	
	__MM_HEAP_MARK;
	
	for (;;)
		{
		
		__UHEAP_SETFAIL(RHeap::EFailNext ,failCount);
		
		completed = EFalse;
		
		err = GetTestResults(modelBankIDs, getModelBankIDsResult);
		
		if ((getModelBankIDsResult != EPass)|| 
			((err != KErrNoMemory) && (err != KErrNone)))
			{
			badResult = ETrue;	
			}
				
		TAny* testAlloc = User::Alloc(1);
		if (NULL == testAlloc)
			{
			heapTestComplete = ETrue;
			failCount -= 1;
			}
		else
			{
			User::Free(testAlloc);
			}
			
		completed = badResult || heapTestComplete;	
		
		__UHEAP_SETFAIL(RHeap::ENone ,0);	
		
		if (completed)
			{
			break;
			}
			
		failCount++;
		}

	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("Alloc testing on GetAllModelBankIDs passed after %d memory allocations"),failCount-1);
		getModelBankIDsResult = EFail;
		}
	else
		{
		INFO_PRINTF2(_L("Alloc testing on GetAllModelBankIDs passed after %d memory allocations"),failCount-1);
		}
	
	//[ check that the id's contained in the list 
	// conform to those added to the modelBank in the controller ]
    if ((getModelBankIDsResult == EPass) && (modelBankIDs.Count() != KNumModelBanks))
    	{
    	INFO_PRINTF3(_L("Expected %d modelBanks, received %d modelBanks"), KNumModelBanks, modelBankIDs.Count());
    	getModelBankIDsResult = EFail;
    	}

    if (getModelBankIDsResult == EPass)
    	{
		INFO_PRINTF2(_L("ModelBank Ids Count: %d"),modelBankIDs.Count() );
	    for( TInt index = 0; index < modelBankIDs.Count() && getModelBankIDsResult == EPass; index++ )
			{
	    	if (modelBankIDs[index]!=KModelBankIDs[index])
	    		{
	    		INFO_PRINTF3(_L("failed - expected modelBank id: %d, received modelBank id: %d"),KModelBankIDs[index],modelBankIDs[index]);
		    	getModelBankIDsResult = EFail;	    	
		    	}
			}	
		
		}
	modelBankIDs.Close();

	__MM_HEAP_MARKEND;
	
	return getModelBankIDsResult;
	}


TInt RTestASRCLGetAllModelBankIDsAlloc::GetTestResults(RArray <TModelBankID>& aModelBankIDs, TVerdict& aResult)
	{
	TInt err = KErrNone;
	
	TRAP(err, CreateSpeechClientL(KUidTestASRClient));
	
	if (err == KErrNone)
		{
		err = iSpeechClient->GetAllModelBankIDs(aModelBankIDs);
		}
	
	if (err == KErrNone)
		{
		CActiveScheduler::Start();			
		if (iLastEvent != KUidAsrEventGetAllModelBankIDs)
			{
			aResult = EFail;
			}
		else
			{
			//Checks the results to make sure we did not miss an alloc failure 
			if ((iLastError == KErrNone) && (aModelBankIDs.Count() != KNumModelBanks))
    			{
    			INFO_PRINTF3(_L("Expected %d modelBanks, received %d modelBanks"), KNumModelBanks, aModelBankIDs.Count());
    			aResult = EFail;
    			}
			}	
				
		if (iLastError != KErrNone)
			{
			err = iLastError;
			}
		}
		
	delete iSpeechClient;
	iSpeechClient = NULL;
		
	return err;
	}
//------------------------------------------------------------------
RTestASRCLGetAllClientGrammarIDsAlloc::RTestASRCLGetAllClientGrammarIDsAlloc()
	{
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ASRCL-U-0067");
	iCreateClient = EFalse;
	}
//------------------------------------------------------------------

TVerdict RTestASRCLGetAllClientGrammarIDsAlloc::DoTestStepL()
/** Get all grammar IDs
 * Use case:  * Get_Info1
 * @test Req. under test 
 */
	{
	
	TInt failCount = 1;
	TInt err = KErrNoMemory;
	TBool completed = EFalse;
	TBool badResult = EFalse; 
	TVerdict getClientGrammarIDsResult = EPass;	// the test result
	TBool heapTestComplete = EFalse;
	
	RArray <TGrammarID> grammarIDs;
	
	

	__MM_HEAP_MARK;
	
		for (;;)
		{
		
		__UHEAP_SETFAIL(RHeap::EFailNext ,failCount);
		
		completed = EFalse;
		
		err = GetTestResults(grammarIDs, getClientGrammarIDsResult);
		
		if ((getClientGrammarIDsResult != EPass)|| 
			((err != KErrNoMemory) && (err != KErrNone)))
			{
			badResult = ETrue;	
			}
					
		TAny* testAlloc = User::Alloc(1);
		if (NULL == testAlloc)
			{
			heapTestComplete = ETrue;
			failCount -= 1;
			}
		else
			{
			User::Free(testAlloc);
			}
			
		completed = badResult || heapTestComplete;	
				
		__UHEAP_SETFAIL(RHeap::ENone ,0);	
		
		if (completed)
			{
			break;
			}
			
		failCount++;
		}
	
	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("Alloc testing on GetAllClientGrammarIDs passed after %d memory allocations"),failCount-1);
		getClientGrammarIDsResult = EFail;
		}
	else
		{
		INFO_PRINTF2(_L("Alloc testing on GetAllClientGrammarIDs passed after %d memory allocations"),failCount-1);
		}
	
	//[ check that the id's contained in the list 
	// conform to those added to the grammar in the controller ]
    if ((getClientGrammarIDsResult == EPass) && (grammarIDs.Count() != KNumGrammars))
    	{
    	INFO_PRINTF3(_L("Expected %d grammars, received %d grammars"), KNumGrammars, grammarIDs.Count());
    	getClientGrammarIDsResult = EFail;
    	}

    if (getClientGrammarIDsResult == EPass)
    	{
		INFO_PRINTF2(_L("Grammar Ids Count: %d"),grammarIDs.Count() );
	    for( TInt index = 0; index < grammarIDs.Count() && getClientGrammarIDsResult == EPass; index++ )
			{
	    	if (grammarIDs[index]!=KGrammarIDs[index])
	    		{
	    		INFO_PRINTF3(_L("failed - expected grammar id: %d, received grammar id: %d"),KGrammarIDs[index],grammarIDs[index]);
		    	getClientGrammarIDsResult = EFail;	    	
		    	}
			}
			
		
		}
	grammarIDs.Close();

	__MM_HEAP_MARKEND;
	
	return getClientGrammarIDsResult;
	}


TInt RTestASRCLGetAllClientGrammarIDsAlloc::GetTestResults(RArray <TGrammarID>& aGrammarIDs, TVerdict& aResult)
	{
	TInt err = KErrNone;
	
	TRAP(err, CreateSpeechClientL(KUidTestASRClient));
	
	if (err == KErrNone)
		{
		err = iSpeechClient->GetAllClientGrammarIDs(aGrammarIDs);
		}
	
	if (err == KErrNone)
		{
		CActiveScheduler::Start();			
		if (iLastEvent != KUidAsrEventGetAllClientGrammarIDs)
			{
			aResult = EFail;
			}
		else
			{
			//Checks the results to make sure we did not miss an alloc failure 
			if ((iLastError == KErrNone) && (aGrammarIDs.Count() != KNumGrammars))
    			{
    			INFO_PRINTF3(_L("Expected %d grammars, received %d grammars"), KNumGrammars, aGrammarIDs.Count());
    			aResult = EFail;
    			}
			}
		
		if (iLastError != KErrNone)
			{
			err = iLastError;
			}
		}

	delete iSpeechClient;
	iSpeechClient = NULL;
	
	return err;	
	}
//------------------------------------------------------------------
RTestASRCLGetAllClientLexiconIDsAlloc::RTestASRCLGetAllClientLexiconIDsAlloc()
	{
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ASRCL-U-0068");
	iCreateClient = EFalse;
	}
//------------------------------------------------------------------

TVerdict RTestASRCLGetAllClientLexiconIDsAlloc::DoTestStepL()
/** Get all lexicon IDs
 * Use case:  * Get_Info1
 * @test Req. under test 
 */
	{
	
	TInt failCount = 1;
	TInt err = KErrNoMemory;
	TBool completed = EFalse;
	TBool badResult = EFalse; 
	TVerdict getClientLexiconIDsResult = EPass;	// the test result
	TBool heapTestComplete = EFalse;
	
	RArray <TLexiconID> lexiconIDs;
		

	__MM_HEAP_MARK;
	
	for (;;)
		{
		
		__UHEAP_SETFAIL(RHeap::EFailNext ,failCount);
		
		completed = EFalse;
		
		err = GetTestResults(lexiconIDs, getClientLexiconIDsResult);
		
		if ((getClientLexiconIDsResult != EPass)|| 
			((err != KErrNoMemory) && (err != KErrNone)))
			{
			badResult = ETrue;	
			}
				
		TAny* testAlloc = User::Alloc(1);
		if (NULL == testAlloc)
			{
			heapTestComplete = ETrue;
			failCount -= 1;
			}
		else
			{
			User::Free(testAlloc);
			}
			
		completed = badResult || heapTestComplete;	
				
		__UHEAP_SETFAIL(RHeap::ENone ,0);	
		
		if (completed)
			{
			break;
			}
			
		failCount++;
		}

	
	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("Alloc testing on GetAllClientLexiconIDs passed after %d memory allocations"),failCount-1);
		getClientLexiconIDsResult = EFail;
		}
	else
		{
		INFO_PRINTF2(_L("Alloc testing on GetAllClientLexiconIDs passed after %d memory allocations"),failCount-1);
		}
	
	//[ check that the id's contained in the list 
	// conform to those added to the lexicon in the controller ]
    if ((getClientLexiconIDsResult == EPass) && (lexiconIDs.Count() != KNumLexicons))
    	{
    	INFO_PRINTF3(_L("Expected %d lexicons, received %d lexicons"), KNumLexicons, lexiconIDs.Count());
    	getClientLexiconIDsResult = EFail;
    	}

    if (getClientLexiconIDsResult == EPass)
    	{
		INFO_PRINTF2(_L("Lexicon Ids Count: %d"),lexiconIDs.Count() );
	    for( TInt index = 0; index < lexiconIDs.Count() && getClientLexiconIDsResult == EPass; index++ )
			{
	    	if (lexiconIDs[index]!=KLexiconIDs[index])
	    		{
	    		INFO_PRINTF3(_L("failed - expected lexicon id: %d, received lexicon id: %d"),KLexiconIDs[index],lexiconIDs[index]);
		    	getClientLexiconIDsResult = EFail;	    	
		    	}
			}
			
		
		}
	lexiconIDs.Close();

	__MM_HEAP_MARKEND;
	
	return getClientLexiconIDsResult;
	}
	


TInt RTestASRCLGetAllClientLexiconIDsAlloc::GetTestResults(RArray <TLexiconID>& aLexiconIDs, TVerdict& aResult)
	{

	TInt err = KErrNone;
	
	TRAP(err, CreateSpeechClientL(KUidTestASRClient));
	
	if (err == KErrNone)
		{
		err = iSpeechClient->GetAllClientLexiconIDs(aLexiconIDs);
		}
			
	if (err == KErrNone)
		{
		CActiveScheduler::Start();			
		if (iLastEvent != KUidAsrEventGetAllClientLexiconIDs)
			{
			aResult = EFail;
			}
		else
			{
			//Checks the results to make sure we did not miss an alloc failure 
			if ((iLastError == KErrNone) && (aLexiconIDs.Count() != KNumLexicons))
    			{
    			INFO_PRINTF3(_L("Expected %d lexicons, received %d lexicons"), KNumLexicons, aLexiconIDs.Count());
    			aResult = EFail;
    			}
			}
			
		if (iLastError != KErrNone)
			{
			err = iLastError;
			}
			
		}

	delete iSpeechClient;
	iSpeechClient = NULL;
		
	return err;
	}


//------------------------------------------------------------------
RTestASRCLGetAllClientModelBankIDsAlloc::RTestASRCLGetAllClientModelBankIDsAlloc()
	{
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ASRCL-U-0069");
	iCreateClient = EFalse;
	}
//------------------------------------------------------------------

TVerdict RTestASRCLGetAllClientModelBankIDsAlloc::DoTestStepL()
/** Get all modelBank IDs
 * Use case:  * Get_Info1
 * @test Req. under test 
 */
	{
	
	TInt failCount = 1;
	TInt err = KErrNoMemory;
	TBool completed = EFalse;
	TBool badResult = EFalse; 
	TVerdict getClientModelBankIDsResult = EPass;	// the test result
	TBool heapTestComplete = EFalse;

	RArray <TModelBankID> modelBankIDs;
	
	__MM_HEAP_MARK;
	
	for (;;)
		{
		
		__UHEAP_SETFAIL(RHeap::EFailNext ,failCount);
		
		completed = EFalse;
		
		err = GetTestResults(modelBankIDs, getClientModelBankIDsResult);
		
		if ((getClientModelBankIDsResult != EPass)|| 
			((err != KErrNoMemory) && (err != KErrNone)))
			{
			badResult = ETrue;	
			}
			
		TAny* testAlloc = User::Alloc(1);
		if (NULL == testAlloc)
			{
			heapTestComplete = ETrue;
			failCount -= 1;
			}
		else
			{
			User::Free(testAlloc);
			}
			
		completed = badResult || heapTestComplete;	
				
		__UHEAP_SETFAIL(RHeap::ENone ,0);	
		
		if (completed)
			{
			break;
			}
			
		failCount++;
		}

	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("Alloc testing on GetAllClientModelBankIDs passed after %d memory allocations"),failCount-1);
		getClientModelBankIDsResult = EFail;
		}
	else
		{
		INFO_PRINTF2(_L("Alloc testing on GetAllClientModelBankIDs passed after %d memory allocations"),failCount-1);
		}
	
	//[ check that the id's contained in the list 
	// conform to those added to the modelBank in the controller ]
    if ((getClientModelBankIDsResult == EPass) && (modelBankIDs.Count() != KNumModelBanks))
    	{
    	INFO_PRINTF3(_L("Expected %d modelBanks, received %d modelBanks"), KNumModelBanks, modelBankIDs.Count());
    	getClientModelBankIDsResult = EFail;
    	}

    if (getClientModelBankIDsResult == EPass)
    	{
		INFO_PRINTF2(_L("ModelBank Ids Count: %d"),modelBankIDs.Count() );
	    for( TInt index = 0; index < modelBankIDs.Count() && getClientModelBankIDsResult == EPass; index++ )
			{
	    	if (modelBankIDs[index]!=KModelBankIDs[index])
	    		{
	    		INFO_PRINTF3(_L("failed - expected modelBank id: %d, received modelBank id: %d"),KModelBankIDs[index],modelBankIDs[index]);
		    	getClientModelBankIDsResult = EFail;	    	
		    	}
			}
			
		
		}
	modelBankIDs.Close();

	__MM_HEAP_MARKEND;
	
	return getClientModelBankIDsResult;
	}


TInt RTestASRCLGetAllClientModelBankIDsAlloc::GetTestResults(RArray <TModelBankID>& aModelBankIDs, TVerdict& aResult)
	{
	
	TInt err = KErrNone;
	
	TRAP(err, CreateSpeechClientL(KUidTestASRClient));
	
	if (err == KErrNone)
		{
		err = iSpeechClient->GetAllClientModelBankIDs(aModelBankIDs);
		}
			
	if (err == KErrNone)
		{
		CActiveScheduler::Start();			
		if (iLastEvent != KUidAsrEventGetAllClientModelBankIDs)
			{
			aResult = EFail;
			}
		else 
			{
			//Checks the results to make sure we did not miss an alloc failure 
			if ((iLastError == KErrNone) && (aModelBankIDs.Count() != KNumModelBanks))
    			{
    			INFO_PRINTF3(_L("Expected %d modelBanks, received %d modelBanks"), KNumModelBanks, aModelBankIDs.Count());
    			aResult = EFail;
    			}
			}
		
		if (iLastError != KErrNone)				
			{
			err = iLastError;
			}
		
		}
		
	delete iSpeechClient;
	iSpeechClient = NULL;
	
	return err;
	}

//------------------------------------------------------------------
RTestASRCLRecognizeAlloc::RTestASRCLRecognizeAlloc()
	{
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ASRCL-U-0070");
	iCreateClient = EFalse;
	}
//------------------------------------------------------------------

TVerdict RTestASRCLRecognizeAlloc::DoTestStepL()
/** Get all modelBank IDs
 * Use case:  * Get_Info1
 * @test Req. under test 
 */
	{
	TModelBankID modelBankID = KModelBankIDs[KIndex0];
	TGrammarID grammarID = KGrammarIDs[KIndex1];
	TLexiconID lexiconID = KLexiconIDs[KIndex1];

	
	TInt failCount = 0;

	TInt err = KErrNoMemory;
	
	TVerdict result = EPass;
	TBool heapTestComplete = EFalse;

	__MM_HEAP_MARK;
	
	while (err == KErrNoMemory || !heapTestComplete)
		{
		__UHEAP_SETFAIL(RHeap::EFailNext, failCount);

		TRAP(err, CreateSpeechClientL(KUidTestASRClient));		
		if (err == KErrNone)
			err = StartRecSession(ESdMode);
		if (err == KErrNone)
			{
			err = iSpeechClient->LoadGrammar(grammarID);
			if (err == KErrNone)
				{
				CActiveScheduler::Start();			
				if (iLastEvent != KUidAsrEventLoadGrammar)
					err = KErrGeneral;
				else	
					err = iLastError;
				}
			}

		if (err == KErrNone)
			{
			err = iSpeechClient->LoadLexicon(lexiconID);
			if (err == KErrNone)
				{
				CActiveScheduler::Start();			
				if (iLastEvent != KUidAsrEventLoadLexicon)
					err = KErrGeneral;
				else	
					err = iLastError;
				}
			}

		if (err == KErrNone)
			{
			err = iSpeechClient->LoadModels(modelBankID);
			if (err == KErrNone)
				{
				CActiveScheduler::Start();			
				if (iLastEvent != KUidAsrEventLoadModels)
					err = KErrGeneral;
				else	
					err = iLastError;
				}
			}

		if (err == KErrNone)
			{
			CSDClientResultSet* results = NULL;
			TRAP(err, results = CSDClientResultSet::NewL());

			if (err == KErrNone)
				TRAP(err, results->SetMaxResultsL(2));
			
			if (err == KErrNone)
				err = iSpeechClient->Recognize(*results);
			
			if (err == KErrNone)
				{
				CActiveScheduler::Start();			
				if (iLastEvent != KUidAsrEventRecognitionReady)
					err = KErrGeneral;
				else	
					err = iLastError;

				}
			delete results;
			}
		delete iSpeechClient;
		iSpeechClient = NULL;
		
		TAny* testAlloc = User::Alloc(1);
		heapTestComplete = (testAlloc==NULL);
		User::Free(testAlloc);
		failCount++;
		}

		
	__UHEAP_SETFAIL(RHeap::ENone ,0);
	__MM_HEAP_MARKEND;

	INFO_PRINTF2(_L("Number of memory allocations: %u"), failCount-1);

	if (err==KErrNone)
		INFO_PRINTF1(_L("memory alloc test on recognize passed"));
	else
		INFO_PRINTF1(_L("memory alloc test on recognize failed"));
	

	return result;
	}


//------------------------------------------------------------------
RTestASRCLGetEnginePropertiesAlloc::RTestASRCLGetEnginePropertiesAlloc()
	{
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ASRCL-U-0071");
	iCreateClient = EFalse;
	}
//------------------------------------------------------------------

TVerdict RTestASRCLGetEnginePropertiesAlloc::DoTestStepL()
	{
	TInt failCount = 0;
	TInt err = KErrNoMemory;


	RArray<TInt> properties;
	RArray<TInt> values;
	
	CleanupClosePushL(properties);
	CleanupClosePushL(values);
	TInt i;
	for (i=0;i<5;i++)
		User::LeaveIfError(properties.Append(i));
		
	TVerdict result = EPass;
	TBool heapTestComplete = EFalse;

	__MM_HEAP_MARK;
	
	while (err == KErrNoMemory || !heapTestComplete )
		{
		__UHEAP_SETFAIL(RHeap::EFailNext ,failCount);
		
		TRAP(err, CreateSpeechClientL(KUidTestASRClient));		
		if (err == KErrNone)
			err = iSpeechClient->GetEngineProperties(properties, values);

		delete iSpeechClient;
		iSpeechClient = NULL;
			
		TAny* testAlloc = User::Alloc(1);
		heapTestComplete = (testAlloc==NULL);
		User::Free(testAlloc);
		failCount++;
		}
		
	__UHEAP_SETFAIL(RHeap::ENone ,0);

	if (result == EPass)
		{
		if (values.Count()==properties.Count())
			{
			for (i=0;i<5;i++)
				if (values[i] != i)
					{
					INFO_PRINTF4(_L("%d: expected %d received %d"),i,i,values[i]);
					return EFail;
					}
	
			}
		else
			{
			INFO_PRINTF3(_L("%d Properties requested, but %d values returned"),properties.Count(),values.Count());
			result = EFail;
			}
		}

	CleanupStack::PopAndDestroy(2,&properties); //properties, values

	__MM_HEAP_MARKEND;


	return result;
	}

//--------------------------------------------------------------------

RTestASRCLSDDataRule::RTestASRCLSDDataRule()
	{
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ASRCL-U-0100");
	iCreateClient = EFalse;
	iInitialDatabaseState = ENoDatabase;
	}
//------------------------------------------------------------------

TVerdict RTestASRCLSDDataRule::DoTestStepL()
	{
	TVerdict verdict = EPass;

	TRuleID ruleID = 12;
	TLexiconID lexiconID = 13;
	TPronunciationID pronunciationID = 14;

	CSDRule* rule = CSDRule::NewL(ruleID,lexiconID,pronunciationID);
	if (rule->RuleID() != ruleID)
		verdict = EFail;
	if (rule->LexiconID() != lexiconID)
		verdict = EFail;
	if (rule->PronunciationID() != pronunciationID)
		verdict = EFail;

	delete rule;
	rule = NULL;

	rule = CSDRule::NewLC(ruleID,lexiconID,pronunciationID);
	if (rule->RuleID() != ruleID)
		verdict = EFail;
	if (rule->LexiconID() != lexiconID)
		verdict = EFail;
	if (rule->PronunciationID() != pronunciationID)
		verdict = EFail;

	CleanupStack::PopAndDestroy(rule);
	return verdict;
	}
//------------------------------------------------------------------
RTestASRCLSDDataPronunciation::RTestASRCLSDDataPronunciation()
	{
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ASRCL-U-0101");
	iCreateClient = EFalse;
	iInitialDatabaseState = ENoDatabase;


	}
//------------------------------------------------------------------

TVerdict RTestASRCLSDDataPronunciation::DoTestStepL()
	{
	TVerdict verdict = EPass;

	TPronunciationID pronunciationID = 14;
	TModelBankID modelBankID = 15;
	TPhonemeID phonemeID = 16;

	CSDPronunciation* pronunciation = CSDPronunciation::NewL(pronunciationID,modelBankID,phonemeID);
	if (pronunciation->PronunciationID() != pronunciationID)
		verdict = EFail;
	if (pronunciation->ModelBankID() != modelBankID)
		verdict = EFail;
	if (pronunciation->PhonemeID() != phonemeID)
		verdict = EFail;

	delete pronunciation;
	pronunciation = NULL;

	pronunciation = CSDPronunciation::NewLC(pronunciationID,modelBankID,phonemeID);
	if (pronunciation->PronunciationID() != pronunciationID)
		verdict = EFail;
	if (pronunciation->ModelBankID() != modelBankID)
		verdict = EFail;
	if (pronunciation->PhonemeID() != phonemeID)
		verdict = EFail;

	CleanupStack::PopAndDestroy(pronunciation);
	return verdict;
	}
//------------------------------------------------------------------
RTestASRCLSDDataModel::RTestASRCLSDDataModel()
	{
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ASRCL-U-0102");
	iCreateClient = EFalse;
	iInitialDatabaseState = ENoDatabase;
	}
//------------------------------------------------------------------

TVerdict RTestASRCLSDDataModel::DoTestStepL()
	{
	TVerdict verdict = EPass;
	const TInt KTestDataSize = 10;

	TPhonemeID phonemeID = 20;
	const TUint8 KModelData[KTestDataSize] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
	
	CSDModel* model = CSDModel::NewL(phonemeID);
	if (model->PhonemeID() != phonemeID)
		verdict = EFail;


	TUint8* data = new (ELeave) TUint8[KTestDataSize];
	Mem::Copy(data,KModelData,KTestDataSize);
	TPtr8 ptr(data,KTestDataSize,KTestDataSize);

	model->SetAcousticModel(ptr);
	if (model->AcousticModel().Compare(ptr)!=0)
		verdict = EFail;

	delete model;
	model = NULL;

	model = CSDModel::NewLC(phonemeID);
	if (model->PhonemeID() != phonemeID)
		verdict = EFail;

	data = new (ELeave) TUint8[KTestDataSize];
	Mem::Copy(data,KModelData,KTestDataSize);
	TPtr8 ptr2(data,KTestDataSize,KTestDataSize);

	model->SetAcousticModel(ptr2);
	if (model->AcousticModel().Compare(ptr)!=0)
		verdict = EFail;


	CleanupStack::PopAndDestroy(model);

	return verdict;
	}


//------------------------------------------------------------------

RTestASRCLSDDataGrammar::RTestASRCLSDDataGrammar()
	{
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ASRCL-U-0110");
	iCreateClient = EFalse;
	iInitialDatabaseState = ENoDatabase;
	}
//------------------------------------------------------------------

TVerdict RTestASRCLSDDataGrammar::DoTestStepL()
	{
	TVerdict verdict = EPass;

	TInt grammarIndex = 0; // use first grammar in test data
	TGrammarID grammarID = KGrammarIDs[grammarIndex];

	CSDGrammar* grammar = CSDGrammar::NewLC(grammarID);

	if (grammarID !=grammar->GrammarID())
		verdict = EFail;

	CleanupStack::PopAndDestroy(grammar);

	grammar = CSDGrammar::NewL(grammarID);

	if (grammarID !=grammar->GrammarID())
		verdict = EFail;

	delete grammar;


	return verdict;
	}
//------------------------------------------------------------------

RTestASRCLSDDataGrammarAdd::RTestASRCLSDDataGrammarAdd()
	{
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ASRCL-U-0111");
	iCreateClient = EFalse;
	iInitialDatabaseState = ENoDatabase;
	}
//------------------------------------------------------------------

TVerdict RTestASRCLSDDataGrammarAdd::DoTestStepL()
	{
	TVerdict verdict = EPass;

	TInt grammarIndex = 0; // use first grammar in test data
	TGrammarID grammarID = KGrammarIDs[grammarIndex];
	
	CSDDatabase::TRuleData rules[KNumRules];
	CSDGrammar* grammar = CSDGrammar::NewLC(grammarID);

	for (TInt i=0; i < KNumRules; i++) 
		{
		rules[i] = KRuleIDs[grammarIndex][i];
		CSDRule* rule = CSDRule::NewL(rules[i].iRuleID, rules[i].iLexiconID, rules[i].iPronunciationID);
		CleanupStack::PushL(rule);
		User::LeaveIfError(grammar->Add(*rule));
		CleanupStack::Pop(rule);
		}

	// now try adding the first rule again - should fail
	TInt firstRule = 0;
	CSDRule* rule = CSDRule::NewL(rules[firstRule].iRuleID, rules[firstRule].iLexiconID, rules[firstRule].iPronunciationID);
	CleanupStack::PushL(rule);
	TInt err = grammar->Add(*rule);
	if (err == KErrNone)
		{
		CleanupStack::Pop(rule);
		verdict = EFail; // add should have failed
		}
	else
		{
		CleanupStack::PopAndDestroy(rule); // not added so destroy
		if (err != KErrAlreadyExists)
			verdict = EFail;
		}

	if (grammar->Count()!=KNumRules) 
		verdict = EFail;

	CleanupStack::PopAndDestroy(grammar);   

	return verdict;
	}

//------------------------------------------------------------------

RTestASRCLSDDataGrammarFind::RTestASRCLSDDataGrammarFind()
	{
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ASRCL-U-0112");
	iCreateClient = EFalse;
	iInitialDatabaseState = ENoDatabase;
	}
//------------------------------------------------------------------

TVerdict RTestASRCLSDDataGrammarFind::DoTestStepL()
	{
	TVerdict verdict = EPass;

	TInt grammarIndex = 0; // use first grammar in test data
	TGrammarID grammarID = KGrammarIDs[grammarIndex];
	
	CSDDatabase::TRuleData rules[KNumRules];
	CSDGrammar* grammar = CSDGrammar::NewLC(grammarID);

	for (TInt i=0; i < KNumRules; i++) 
		{
		rules[i] = KRuleIDs[grammarIndex][i];

		TInt err = grammar->FindRule(rules[i].iRuleID);
		if (err != KErrNotFound)
			{
			INFO_PRINTF2(_L("Error: Rule found before added to Grammar %d"), err);
			verdict = EFail;
			}

		CSDRule* rule = CSDRule::NewL(rules[i].iRuleID, rules[i].iLexiconID, rules[i].iPronunciationID);
		CleanupStack::PushL(rule);
		User::LeaveIfError(grammar->Add(*rule));
		CleanupStack::Pop(rule);
		}

	if (grammar->Count()==KNumRules) 
		{
		for (TInt i = 0; i < KNumRules; i++) 
			{
			TInt index = grammar->FindRule(rules[i].iRuleID);
			if (grammar->At(index).RuleID() != rules[i].iRuleID)
				verdict = EFail;
			}
		}
	else
		verdict = EInconclusive;

	CleanupStack::PopAndDestroy(grammar);   

	return verdict;
	}


//------------------------------------------------------------------

RTestASRCLSDDataGrammarDelete::RTestASRCLSDDataGrammarDelete()
	{
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ASRCL-U-0113");
	iCreateClient = EFalse;
	iInitialDatabaseState = ENoDatabase;
	}
//------------------------------------------------------------------

TVerdict RTestASRCLSDDataGrammarDelete::DoTestStepL()
	{
TVerdict verdict = EPass;

	TInt grammarIndex = 0; // use first grammar in test data
	TGrammarID grammarID = KGrammarIDs[grammarIndex];
	
	CSDDatabase::TRuleData rules[KNumRules];
	CSDGrammar* grammar = CSDGrammar::NewLC(grammarID);

	for (TInt i=0; i < KNumRules; i++) 
		{
		rules[i] = KRuleIDs[grammarIndex][i];
		CSDRule* rule = CSDRule::NewL(rules[i].iRuleID, rules[i].iLexiconID, rules[i].iPronunciationID);
		CleanupStack::PushL(rule);
		User::LeaveIfError(grammar->Add(*rule));
		CleanupStack::Pop(rule);
		}

	TInt count = grammar->Count();
	if (count==KNumRules) 
		{
		for (TInt i = 0; i < KNumRules; i++) 
			{
			count--;
			grammar->Delete(rules[i].iRuleID);
			if (grammar->Count()!=count)
				verdict = EFail;
			}
		}

	CleanupStack::PopAndDestroy(grammar);   

	return verdict;
	}

//------------------------------------------------------------------

RTestASRCLSDDataLexicon::RTestASRCLSDDataLexicon()
	{
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ASRCL-U-0120");
	iCreateClient = EFalse;
	iInitialDatabaseState = ENoDatabase;
	}
//------------------------------------------------------------------

TVerdict RTestASRCLSDDataLexicon::DoTestStepL()
	{
	TVerdict verdict = EPass;

	TInt lexiconIndex = 0; // use first lexicon in test data
	TLexiconID lexiconID = KLexiconIDs[lexiconIndex];

	CSDLexicon* lexicon = CSDLexicon::NewLC(lexiconID);

	if (lexiconID !=lexicon->LexiconID())
		verdict = EFail;

	CleanupStack::PopAndDestroy(lexicon);

	lexicon = CSDLexicon::NewL(lexiconID);

	if (lexiconID !=lexicon->LexiconID())
		verdict = EFail;

	delete lexicon;


	return verdict;
	}
//------------------------------------------------------------------

RTestASRCLSDDataLexiconAdd::RTestASRCLSDDataLexiconAdd()
	{
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ASRCL-U-0121");
	iCreateClient = EFalse;
	iInitialDatabaseState = ENoDatabase;
	}
//------------------------------------------------------------------

TVerdict RTestASRCLSDDataLexiconAdd::DoTestStepL()
	{
	TVerdict verdict = EPass;

	TInt lexiconIndex = 0; // use first lexicon in test data
	TLexiconID lexiconID = KLexiconIDs[lexiconIndex];
	
	CSDDatabase::TPronunciationData pronunciations[KNumPronunciations];
	CSDLexicon* lexicon = CSDLexicon::NewLC(lexiconID);

	for (TInt i=0; i < KNumPronunciations; i++) 
		{
		pronunciations[i] = KPronunciationIDs[lexiconIndex][i];
		CSDPronunciation* pronunciation = CSDPronunciation::NewL(pronunciations[i].iPronunciationID, pronunciations[i].iModelBankID, pronunciations[i].iModelID);
		CleanupStack::PushL(pronunciation);
		User::LeaveIfError(lexicon->Add(*pronunciation));
		CleanupStack::Pop(pronunciation);
		}

	// now try adding the first pronunciation again - should fail
	CSDPronunciation* pronunciation = CSDPronunciation::NewL(pronunciations[0].iPronunciationID, pronunciations[0].iModelBankID, pronunciations[0].iModelID);
	CleanupStack::PushL(pronunciation);
	TInt err = lexicon->Add(*pronunciation);
	if (err == KErrNone)
		{
		CleanupStack::Pop(pronunciation);
		verdict = EFail; // add should have failed
		}
	else
		{
		CleanupStack::PopAndDestroy(pronunciation); // not added so destroy
		if (err != KErrAlreadyExists)
			verdict = EFail;
		}


	if (lexicon->Count()!=KNumPronunciations) 
		verdict = EFail;

	CleanupStack::PopAndDestroy(lexicon);   

	return verdict;
	}

//------------------------------------------------------------------

RTestASRCLSDDataLexiconFind::RTestASRCLSDDataLexiconFind()
	{
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ASRCL-U-0122");
	iCreateClient = EFalse;
	iInitialDatabaseState = ENoDatabase;
	}
//------------------------------------------------------------------

TVerdict RTestASRCLSDDataLexiconFind::DoTestStepL()
	{
	TVerdict verdict = EPass;

	TInt lexiconIndex = 0; // use first lexicon in test data
	TLexiconID lexiconID = KLexiconIDs[lexiconIndex];
	
	CSDDatabase::TPronunciationData pronunciations[KNumPronunciations];
	CSDLexicon* lexicon = CSDLexicon::NewLC(lexiconID);

	
	for (TInt i=0; i < KNumPronunciations; i++) 
		{		
		pronunciations[i] = KPronunciationIDs[lexiconIndex][i];
		// try to find pronunciation before we add it
		TInt err = lexicon->FindPronunciation(pronunciations[i].iPronunciationID);
		if (err != KErrNotFound)
			{
			INFO_PRINTF2(_L("Error: Pronunciation found before added to Lexicon %d"), err);
			verdict = EFail;
			}
		CSDPronunciation* pronunciation = CSDPronunciation::NewL(pronunciations[i].iPronunciationID, pronunciations[i].iModelBankID, pronunciations[i].iModelID);
		CleanupStack::PushL(pronunciation);
		User::LeaveIfError(lexicon->Add(*pronunciation));
		CleanupStack::Pop(pronunciation);
		}

	if (lexicon->Count()==KNumPronunciations) 
		{
		for (TInt i = 0; i < KNumPronunciations; i++) 
			{
			TInt index = lexicon->FindPronunciation(pronunciations[i].iPronunciationID);
			if (lexicon->At(index).PronunciationID() != pronunciations[i].iPronunciationID)
				verdict = EFail;
			}
		}
	else
		verdict = EInconclusive;

	CleanupStack::PopAndDestroy(lexicon);   

	return verdict;
	}


//------------------------------------------------------------------

RTestASRCLSDDataModelBank::RTestASRCLSDDataModelBank()
	{
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ASRCL-U-0130");
	iCreateClient = EFalse;
	iInitialDatabaseState = ENoDatabase;
	}
//------------------------------------------------------------------

TVerdict RTestASRCLSDDataModelBank::DoTestStepL()
	{
	TVerdict verdict = EPass;

	TInt modelBankIndex = 0; // use first model bank in test data
	TModelBankID modelBankID = KModelBankIDs[modelBankIndex];

	CSDModelBank* modelBank = CSDModelBank::NewLC(modelBankID);

	if (modelBankID !=modelBank->ModelBankID())
		verdict = EFail;

	CleanupStack::PopAndDestroy(modelBank);

	modelBank = CSDModelBank::NewL(modelBankID);

	if (modelBankID !=modelBank->ModelBankID())
		verdict = EFail;

	delete modelBank;


	return verdict;
	}
//------------------------------------------------------------------

RTestASRCLSDDataModelBankAdd::RTestASRCLSDDataModelBankAdd()
	{
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ASRCL-U-0131");
	iCreateClient = EFalse;
	iInitialDatabaseState = ENoDatabase;
	}
//------------------------------------------------------------------

TVerdict RTestASRCLSDDataModelBankAdd::DoTestStepL()
	{
	TVerdict verdict = EPass;

	TModelBankID modelBankID = KModelBankIDs[KIndex0];
	
	CSDDatabase::TModelData models[KNumModels];
	CSDModelBank* modelBank = CSDModelBank::NewLC(modelBankID);

	for (TInt i=0; i < KNumModels; i++) 
		{
		models[i] = KModelIDs[KIndex0][i];
		CSDModel* model = CSDModel::NewL(models[i].iModelID);
		CleanupStack::PushL(model);
		User::LeaveIfError(modelBank->Add(*model));
		CleanupStack::Pop(model);
		}

	// now try adding the first model again - should fail
	CSDModel* model = CSDModel::NewL(models[0].iModelID);
	CleanupStack::PushL(model);
	TInt err = modelBank->Add(*model);
	if (err == KErrNone)
		{
		CleanupStack::Pop(model);
		verdict = EFail; // add should have failed
		}
	else
		{
		CleanupStack::PopAndDestroy(model); // not added so destroy
		if (err != KErrAlreadyExists)
			verdict = EFail;
		}

	if (modelBank->Count()!=KNumModels) 
		verdict = EFail;

	CleanupStack::PopAndDestroy(modelBank);   

	return verdict;
	}

//------------------------------------------------------------------

RTestASRCLSDDataModelBankFind::RTestASRCLSDDataModelBankFind()
	{
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ASRCL-U-0132");
	iCreateClient = EFalse;
	iInitialDatabaseState = ENoDatabase;
	}
//------------------------------------------------------------------

TVerdict RTestASRCLSDDataModelBankFind::DoTestStepL()
	{
	TVerdict verdict = EPass;

	TInt modelBankIndex = 0; // use first model bank in test data
	TModelBankID modelBankID = KModelBankIDs[modelBankIndex];
	
	CSDDatabase::TModelData models[KNumModels];
	CSDModelBank* modelBank = CSDModelBank::NewLC(modelBankID);

	for (TInt i=0; i < KNumModels; i++) 
		{
		models[i] = KModelIDs[modelBankIndex][i];

		// try to find model before we add it
		TInt err = modelBank->FindModel(models[i].iModelID);
		if (err != KErrNotFound)
			{
			INFO_PRINTF2(_L("Error: Model found before added to ModelBank %d"), err);
			verdict = EFail;
			}

		CSDModel* model = CSDModel::NewL(models[i].iModelID);
		CleanupStack::PushL(model);
		User::LeaveIfError(modelBank->Add(*model));
		CleanupStack::Pop(model);
		}

	if (modelBank->Count()==KNumModels) 
		{
		for (TInt i = 0; i < KNumModels; i++) 
			{
			TInt index = modelBank->FindModel(models[i].iModelID);
			if (modelBank->At(index).PhonemeID() != models[i].iModelID)
				verdict = EFail;
			}
		}
	else
		verdict = EInconclusive;

	CleanupStack::PopAndDestroy(modelBank);   

	return verdict;
	}


//------------------------------------------------------------------

RTestASRCLSDDataGrammarAlloc::RTestASRCLSDDataGrammarAlloc()
	{
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ASRCL-U-0114");
	iCreateClient = EFalse;
	iInitialDatabaseState = ENoDatabase;
	}
//------------------------------------------------------------------

TVerdict RTestASRCLSDDataGrammarAlloc::DoTestStepL()
	{
	TVerdict verdict = EPass;

	TInt grammarIndex = 0; // use the first grammar in the test data
	TGrammarID grammarID = KGrammarIDs[grammarIndex];
	
	CSDDatabase::TRuleData rules[KNumRules];


	TBool heapTestComplete = EFalse;
	TBool failCount = 0;
	TInt count=0;
	TInt err = KErrNoMemory;

	__MM_HEAP_MARK;

	while (err==KErrNoMemory || !heapTestComplete) 
		{
		CSDRule* rule = NULL;
		CSDGrammar* grammar = NULL;
 		TInt i;

		__UHEAP_SETFAIL(RHeap::EFailNext ,failCount);
		TRAP(err, grammar = CSDGrammar::NewL(grammarID));

		for (i=0; i < KNumRules && err == KErrNone; i++) 
			{
			rules[i] = KRuleIDs[grammarIndex][i];
			TRAP(err, rule = CSDRule::NewL(rules[i].iRuleID, rules[i].iLexiconID, rules[i].iPronunciationID));
			if (err == KErrNone)
				{
				err = grammar->Add(*rule);
				if (err != KErrNone)
					delete rule;
				}
			}

		if (err==KErrNone)
			count = grammar->Count();

		delete grammar;

		TAny* testAlloc = User::Alloc(1);
		heapTestComplete = (testAlloc==NULL);
		User::Free(testAlloc);
		failCount++;
		}

	if (err != KErrNone)
			verdict = EFail;
		else 
			if (count!=KNumRules) 
				verdict = EFail;

	__UHEAP_SETFAIL(RHeap::ENone ,0);
	__MM_HEAP_MARKEND;

	INFO_PRINTF2(_L("Alloc testing exited after %d memory allocations"),failCount-1);

	return verdict;
	}

//------------------------------------------------------------------

RTestASRCLSDDataLexiconAlloc::RTestASRCLSDDataLexiconAlloc()
	{
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ASRCL-U-0124");
	iCreateClient = EFalse;
	iInitialDatabaseState = ENoDatabase;
	}
//------------------------------------------------------------------

TVerdict RTestASRCLSDDataLexiconAlloc::DoTestStepL()
	{
	TVerdict verdict = EPass;

	TInt lexiconIndex = 0; // use first lexicon in test data
	TLexiconID lexiconID = KLexiconIDs[lexiconIndex];
	
	CSDDatabase::TPronunciationData pronunciations[KNumPronunciations];


	TBool heapTestComplete = EFalse;
	TBool failCount = 0;
	TInt count=0;
	TInt err = KErrNoMemory;

	__MM_HEAP_MARK;

	while (err==KErrNoMemory || !heapTestComplete) 
		{
		CSDPronunciation* pronunciation = NULL;
		CSDLexicon* lexicon = NULL;
		TInt i;

		__UHEAP_SETFAIL(RHeap::EFailNext ,failCount);

		TRAP(err, lexicon = CSDLexicon::NewL(lexiconID));

		for (i=0; i < KNumPronunciations && err == KErrNone; i++) 
			{
			pronunciations[i] = KPronunciationIDs[lexiconIndex][i];
			TRAP(err, pronunciation = CSDPronunciation::NewL(pronunciations[i].iPronunciationID, pronunciations[i].iModelBankID, pronunciations[i].iModelID));
			if (err == KErrNone)
				{
				err = lexicon->Add(*pronunciation);
				if (err != KErrNone)
					delete pronunciation;
				}
			}

		if (err==KErrNone)
			count = lexicon->Count();

		delete lexicon;

		TAny* testAlloc = User::Alloc(1);
		heapTestComplete = (testAlloc==NULL);
		User::Free(testAlloc);
		failCount++;
	
		}

	if (err != KErrNone)
			verdict = EFail;
		else 
			if (count!=KNumPronunciations) 
				verdict = EFail;

	__UHEAP_SETFAIL(RHeap::ENone ,0);
	__MM_HEAP_MARKEND;

	INFO_PRINTF2(_L("Alloc testing exited after %d memory allocations"),failCount-1);

	return verdict;
	}
//------------------------------------------------------------------

RTestASRCLSDDataModelBankAlloc::RTestASRCLSDDataModelBankAlloc()
	{
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ASRCL-U-0134");
	iCreateClient = EFalse;
	iInitialDatabaseState = ENoDatabase;
	}
//------------------------------------------------------------------

TVerdict RTestASRCLSDDataModelBankAlloc::DoTestStepL()
	{
	TVerdict verdict = EPass;

	TInt modelBankIndex = 0; // use first model bank in test data
	TModelBankID modelBankID = KModelBankIDs[modelBankIndex];
	
	CSDDatabase::TModelData models[KNumModels];


	TBool heapTestComplete = EFalse;
	TBool failCount = 0;
	TInt count=0;
	TInt err = KErrNoMemory;


	__MM_HEAP_MARK;

	while (err==KErrNoMemory || !heapTestComplete) 
		{
		CSDModel* model = NULL;
		CSDModelBank* modelBank = NULL;
		TInt i;

		__UHEAP_SETFAIL(RHeap::EFailNext ,failCount);

		TRAP(err, modelBank = CSDModelBank::NewL(modelBankID));

		for (i=0; i < KNumModels && err == KErrNone; i++) 
			{
			models[i] = KModelIDs[modelBankIndex][i];
			TRAP(err, model = CSDModel::NewL(models[i].iModelID));
			if (err == KErrNone)
				{
				err = modelBank->Add(*model);
				if (err != KErrNone)
					delete model;
				}
			}

		if (err==KErrNone)
			count = modelBank->Count();

		delete modelBank;

		TAny* testAlloc = User::Alloc(1);
		heapTestComplete = (testAlloc==NULL);
		User::Free(testAlloc);
		failCount++;
	
		}

	if (err != KErrNone)
			verdict = EFail;
		else 
			if (count!=KNumModels) 
				verdict = EFail;

	__UHEAP_SETFAIL(RHeap::ENone ,0);
	__MM_HEAP_MARKEND;

	INFO_PRINTF2(_L("Alloc testing exited after %d memory allocations"),failCount-1);

	return verdict;
	}


//--------------------------------------------------------------------

RTestASRCLSDDataResult::RTestASRCLSDDataResult()
	{
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ASRCL-U-0140");
	iCreateClient = EFalse;
	iInitialDatabaseState = ENoDatabase;
	}
//------------------------------------------------------------------

TVerdict RTestASRCLSDDataResult::DoTestStepL()
	{
	TVerdict verdict = EPass;

	// sum dummy test values
	const TGrammarID KTestGrammarID = 12;
	const TRuleID KTestRuleID = 13;

	const TInt KTestScore = 345;

	CSDResult* result = CSDResult::NewL(KTestGrammarID,KTestRuleID);
	if (result->GrammarID() != KTestGrammarID)
		verdict = EFail;
	if (result->RuleID() != KTestRuleID)
		verdict = EFail;

	result->SetScore(KTestScore);
	if (result->Score() != KTestScore)
		verdict = EFail;

	delete result;
	result = NULL;

	result = CSDResult::NewLC(KTestGrammarID,KTestRuleID);
	if (result->GrammarID() != KTestGrammarID)
		verdict = EFail;
	if (result->RuleID() != KTestRuleID)
		verdict = EFail;

	CleanupStack::PopAndDestroy(result);
	return verdict;
	}

//------------------------------------------------------------------

RTestASRCLSDDataResultSetAdd::RTestASRCLSDDataResultSetAdd()
	{
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ASRCL-U-0150");
	iCreateClient = EFalse;
	iInitialDatabaseState = ENoDatabase;
	}
//------------------------------------------------------------------

TVerdict RTestASRCLSDDataResultSetAdd::DoTestStepL()
	{
	TVerdict verdict = EPass;

	CSDResultSet* resultSet = CSDResultSet::NewL();
	CleanupStack::PushL(resultSet);

	TInt err = KErrNone;
	
	for (TInt i=0; i < KNumGrammars && err == KErrNone; i++) 
		{
		CSDResult* result = CSDResult::NewLC(KGrammarIDs[i], KRuleIDs[i][0].iRuleID);
		User::LeaveIfError(resultSet->Add(*result));
		CleanupStack::Pop(result);
		}

	if (err != KErrNone)
		verdict = EFail;
	else 
		if (resultSet->Count()!=KNumGrammars) 
			verdict = EFail;

	CleanupStack::PopAndDestroy(resultSet);   

	return verdict;
	}

//------------------------------------------------------------------

RTestASRCLSDDataResultSetAlloc::RTestASRCLSDDataResultSetAlloc()
	{
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ASRCL-U-0151");
	iCreateClient = EFalse;
	iInitialDatabaseState = ENoDatabase;
	}
//------------------------------------------------------------------

TVerdict RTestASRCLSDDataResultSetAlloc::DoTestStepL()
	{
	TVerdict verdict = EPass;

	

	TInt err = KErrNoMemory;

	TInt failCount =0;
	TBool heapTestComplete = EFalse;
	TInt count = 0;

	__MM_HEAP_MARK;
	

	while (err == KErrNoMemory || !heapTestComplete) 
		{
		CSDResult* result = NULL;
		CSDResultSet* resultSet = NULL;

		__UHEAP_SETFAIL(RHeap::EFailNext, failCount);

		TRAP(err, resultSet = CSDResultSet::NewL());

		for (TInt i=0; i < KNumGrammars && err == KErrNone; i++) 
			{
			TRAP(err,  result = CSDResult::NewL(KGrammarIDs[i], KRuleIDs[i][0].iRuleID));
			if (err==KErrNone) 
				{
				err = resultSet->Add(*result);
				if (err != KErrNone)
					delete result;
					result = NULL;
				}
			}

		if (resultSet)
			count = resultSet->Count();

		delete resultSet;

		TAny* testAlloc = User::Alloc(1);
		heapTestComplete = (testAlloc==NULL);
		User::Free(testAlloc);
		failCount++;
		}

	__UHEAP_SETFAIL(RHeap::ENone, 0);
	__MM_HEAP_MARKEND;

	if (err != KErrNone)
		verdict = EFail;
	else 
		if (count!=KNumGrammars) 
			verdict = EFail;


	return verdict;
	}

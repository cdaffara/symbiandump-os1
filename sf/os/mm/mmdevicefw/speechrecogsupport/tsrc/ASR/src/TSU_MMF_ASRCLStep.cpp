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

#include "TSU_MMF_ASRCLStep.h"
#include "ASRController/customcontrollertestconstants.h"
#include "Database/testdatabaseconstants.h"
#include "Database/sddatabase.h"

//const TInt KTenPartOfSec = 100000 ;	// EABI warning removal
//const TInt KOneSecond=1000000;	// Used in a time out function, 1 second (in microSeconds)		// EABI warning removal
//const TInt KTenSeconds=10000000;	// Used in a time out function, 10 seconds (in microSeconds)	// EABI warning removal 
//const TInt KFiveSeconds=5000000;	// 5 seconds (in microSeconds)									// EABI warning removal
//const TInt KFifteenSeconds=15000000;	// 15 seconds (in microSeconds)								// EABI warning removal
const TInt32 KMaxUtteranceMicroSeconds = 10000000;

const TInt KNullPointer = 1;

// text descriptions of every event - for easier test diagnosis
_LIT(KUnrecognizedEvent,"Unrecognized EventVal");
_LIT(KTrainReady,"KUidAsrEventTrainReadyVal");
_LIT(KTrain,"KUidAsrEventTrainVal");
//_LIT(KTrainFailed,"KUidAsrEventTrainFailedVal");		// EABI warning removal
_LIT(KCreateGrammar,"KUidAsrEventCreateGrammarVal");
_LIT(KCreateLexicon,"KUidAsrEventCreateLexiconVal");
_LIT(KCreateModelBank,"KUidAsrEventCreateModelBankVal");

_LIT(KLoadGrammar,"KUidAsrEventLoadGrammarVal");
_LIT(KLoadLexicon,"KUidAsrEventLoadLexiconVal");
_LIT(KLoadModels,"KUidAsrEventLoadModelsVal");

_LIT(KRemoveGrammar,"KUidAsrEventRemoveGrammarVal");
_LIT(KRemoveLexicon,"KUidAsrEventRemoveLexiconVal");
_LIT(KRemoveModelBank,"KUidAsrEventRemoveModelBankVal");

_LIT(KRemoveRule,"KUidAsrEventRemoveRuleVal");
_LIT(KRemovePronunciation,"KUidAsrEventRemovePronunciationVal");
_LIT(KRemoveModel,"KUidAsrEventRemoveModelVal");

_LIT(KRecognitionReady,"KUidAsrEventRecognitionReadyVal");
_LIT(KRecognition,"KUidAsrEventRecognitionVal");

//_LIT(KCommit,"KUidAsrEventCommitVal");// EABI warning removal

_LIT(KAddPronunciation,"KUidAsrEventAddPronunciationVal");
_LIT(KAddRule,"KUidAsrEventAddRuleVal");
_LIT(KUnloadRule,"KUidAsrEventUnloadRuleVal");

_LIT(KRecordStarted,"KUidAsrEventRecordStartedVal");
_LIT(KEouDetected,"KUidAsrEventEouDetectedVal");
_LIT(KPlayStarted,"KUidAsrEventPlayStartedVal");
_LIT(KPlay,"KUidAsrEventPlayVal");
//_LIT(KGetPronunciationCount,"KUidAsrEventGetPronunciationCountVal"); // EABI warning removal
//_LIT(KGetRuleCount,"KUidAsrEventGetRuleCountVal");					// EABI warning removal
_LIT(KGetModelCount,"KUidAsrEventGetModelCountVal");
_LIT(KGetAllPronunciationIDs,"KUidAsrEventGetAllPronunciationIDsVal");
_LIT(KGetAllRuleIDs,"KUidAsrEventGetAllRuleIDsVal");
_LIT(KGetAllModelIDs,"KUidAsrEventGetAllModelIDsVal");
_LIT(KGetAllGrammarIDs,"KUidAsrEventGetAllGrammarIDsVal");
_LIT(KGetAllLexiconIDs,"KUidAsrEventGetAllLexiconIDsVal");
_LIT(KGetAllModelBankIDs,"KUidAsrEventGetAllModelBankIDsVal");
_LIT(KGetAllClientGrammarIDs,"KUidAsrEventGetAllClientGrammarIDsVal");
_LIT(KGetAllClientLexiconIDs,"KUidAsrEventGetAllClientLexiconIDsVal");
_LIT(KGetAllClientModelBankIDs,"KUidAsrEventGetAllClientModelBankIDsVal");
_LIT(KGetAvailableStorage,"KUidAsrEventGetAvailableStorageVal");
_LIT(KGetRuleValidity,"KUidAsrEventGetRuleValidityVal");
_LIT(KGetUtteranceDuration,"KUidAsrEventGetUtteranceDurationVal");
//_LIT(KDiskFull,"KUidAsrEventDiskFullVal");							// EABI warning removal
//_LIT(KCancelled,"KUidAsrEventCancelledVal");							// EABI warning removal
_LIT(KTestASRLoadParametersSuccess,"KTestASRLoadParametersSuccessVal");
_LIT(KTestASRCancel,"KTestASRLoadParametersCancelVal");


void TDummyObserver::MsruoEvent(TUid aEvent, TInt aErrorCode)
	{
	iEvent = aEvent;
	iErrorCode = aErrorCode;
	CActiveScheduler::Stop();
	}
void TDummyObserver::WaitForEvent()
	{
	CActiveScheduler::Start();
	}

TPtrC RTestMMF_ASRCLStep::TranslateEventCode(TInt aEvent)
	{
	TPtrC ptr;

	switch (aEvent)
		{
	case KUidAsrEventTrainReadyVal:
		ptr.Set(KTrainReady);
		break;
	case KUidAsrEventTrainVal:
		ptr.Set(KTrain);
		break;
	case KUidAsrEventAddPronunciationVal:
		ptr.Set(KAddPronunciation);
		break;
	case KUidAsrEventAddRuleVal:
		ptr.Set(KAddRule);
		break;
	case KUidAsrEventLoadModelsVal:
		ptr.Set(KLoadModels);
		break;	
	case KUidAsrEventLoadLexiconVal:
		ptr.Set(KLoadLexicon);
		break;	
	case KUidAsrEventLoadGrammarVal:
		ptr.Set(KLoadGrammar);
		break;
	case KUidAsrEventRecognitionReadyVal:
		ptr.Set(KRecognitionReady);
		break;
	case KUidAsrEventRecognitionVal:
		ptr.Set(KRecognition);
		break;
	case KUidAsrEventUnloadRuleVal:
		ptr.Set(KUnloadRule);
		break;
	case KUidAsrEventRecordStartedVal:
		ptr.Set(KRecordStarted);
		break;
	case KUidAsrEventEouDetectedVal:
		ptr.Set(KEouDetected);
		break;
	case KUidAsrEventPlayStartedVal:
		ptr.Set(KPlayStarted);
		break;
	case KUidAsrEventPlayVal:
		ptr.Set(KPlay);
		break;
	case KUidAsrEventCreateGrammarVal:
		ptr.Set(KCreateGrammar);
		break;
	case KUidAsrEventCreateLexiconVal:
		ptr.Set(KCreateLexicon);
		break;
	case KUidAsrEventCreateModelBankVal:
		ptr.Set(KCreateModelBank);
		break;
	case KUidAsrEventRemoveGrammarVal:
		ptr.Set(KRemoveGrammar);
		break;
	case KUidAsrEventRemoveLexiconVal:
		ptr.Set(KRemoveLexicon);
		break;
	case KUidAsrEventRemoveModelBankVal:
		ptr.Set(KRemoveModelBank);
		break;
	case KUidAsrEventRemoveRuleVal:
		ptr.Set(KRemoveRule);
		break;
	case KUidAsrEventRemovePronunciationVal:
		ptr.Set(KRemovePronunciation);
		break;
	case KUidAsrEventRemoveModelVal:
		ptr.Set(KRemoveModel);
		break;
	case KUidAsrEventGetAvailableStorageVal:
		ptr.Set(KGetAvailableStorage);
		break;
	case KUidAsrEventGetRuleValidityVal:
		ptr.Set(KGetRuleValidity);
		break;
	case KUidAsrEventGetUtteranceDurationVal:
		ptr.Set(KGetUtteranceDuration);
		break;
	case KUidAsrEventGetModelCountVal:
		ptr.Set(KGetModelCount);
		break;
	case KUidAsrEventGetAllRuleIDsVal:
		ptr.Set(KGetAllRuleIDs);
		break;
	case KUidAsrEventGetAllPronunciationIDsVal:
		ptr.Set(KGetAllPronunciationIDs);
		break;
	case KUidAsrEventGetAllModelIDsVal:
		ptr.Set(KGetAllModelIDs);
		break;
	case KUidAsrEventGetAllGrammarIDsVal:
		ptr.Set(KGetAllGrammarIDs);
		break;
	case KUidAsrEventGetAllLexiconIDsVal:
		ptr.Set(KGetAllLexiconIDs);
		break;
	case KUidAsrEventGetAllModelBankIDsVal:
		ptr.Set(KGetAllModelBankIDs);
		break;
	case KUidAsrEventGetAllClientGrammarIDsVal:
		ptr.Set(KGetAllClientGrammarIDs);
		break;
	case KUidAsrEventGetAllClientLexiconIDsVal:
		ptr.Set(KGetAllClientLexiconIDs);
		break;
	case KUidAsrEventGetAllClientModelBankIDsVal:
		ptr.Set(KGetAllClientModelBankIDs);
		break;

	case KUidTestASRLoadParametersVal:
		ptr.Set(KTestASRLoadParametersSuccess);
		break;
	case KUidTestASRCancelVal:
		ptr.Set(KTestASRCancel);
		break;

	default:
		ptr.Set(KUnrecognizedEvent);
		break;
		}
	return ptr;
	}
	
	
//
// RTestMMF_ASRCLStep Implementation
//

RTestMMF_ASRCLStep::RTestMMF_ASRCLStep()
	{
	// these need to be set up since it is a RClass, not a CBase class
	iSpeechClient = NULL;
	iScheduler = NULL;
	iCreateClient = ETrue;
	iInitialDatabaseState = ETestDatabase;
	iNegativeAsyncCallTesting = EFalse;
	}
	
RTestMMF_ASRCLStep::~RTestMMF_ASRCLStep()
	{
	}
	
void RTestMMF_ASRCLStep::CreateSpeechClientL(TUid aClientUID)
	{
	ASSERT(iSpeechClient==NULL);
	iSpeechClient = CSpeechRecognitionUtility::NewL(*this, aClientUID);
	}
	
void RTestMMF_ASRCLStep::CreateTestDatabaseL()
	{
	TFileName filename(KFileNameTestDatabase);
	CSDDatabase* database = CSDDatabase::NewL(filename, EFalse);
	CleanupStack::PushL(database);
	database->SetClientUid(KUidTestASRClient);
	database->CreateTestDatabaseL();
	CleanupStack::PopAndDestroy(database);
	}
	
void RTestMMF_ASRCLStep::ResetDatabaseL()
	{
	TFileName filename(KFileNameTestDatabase);
	CSDDatabase* database = CSDDatabase::NewL(filename, EFalse);
	CleanupStack::PushL(database);
	database->SetClientUid(KUidTestASRClient);
	CleanupStack::PopAndDestroy(database);
	}


void RTestMMF_ASRCLStep::MsruoEvent(TUid aEvent, TInt aErrorCode)
	{
	iLastEvent = aEvent;
	iLastError = aErrorCode;
	CActiveScheduler::Stop();
	}
	
TBool RTestMMF_ASRCLStep::WaitForEvent(TUid aEvent, TInt aExpectedErrorCode)
	{
	CActiveScheduler::Start();
	TPtrC lastEventDescription = TranslateEventCode(iLastEvent.iUid);
	TPtrC expectedEventDescription = TranslateEventCode(aEvent.iUid);
	if (iLastEvent!=aEvent)
		{
		INFO_PRINTF3(_L("Unexpected event (%S) received, expected (%S)"), &lastEventDescription,&expectedEventDescription);
		return EFalse;
		}
	else if (iLastError != aExpectedErrorCode)
		{
		INFO_PRINTF3(_L("Unexpected error (%d) received, expected (%d)"), iLastError,aExpectedErrorCode);
		return EFalse;
		}
		
	INFO_PRINTF2(_L("Expected event (%S) received"),&lastEventDescription);
	INFO_PRINTF2(_L("error code (%d)"),iLastError);
	return ETrue;
	}

	
TVerdict RTestMMF_ASRCLStep::RecordUtterance()
	{
	INFO_PRINTF1(_L("Record Utterance"));
	iSpeechClient->Record(KMaxUtteranceMicroSeconds);
	
	if (!WaitForEvent(KUidAsrEventRecordStarted))
		return EFail;
		
	INFO_PRINTF1(_L("Record Started"));
	if (!WaitForEvent(KUidAsrEventEouDetected))
		return EFail;
	INFO_PRINTF1(_L("End of utterance detected"));
	return EPass;
	}

/**
*
* CommitChanges
* @return TVerdict
*
*/
TVerdict RTestMMF_ASRCLStep::CommitChanges( TBool aExpectedPass )
	{
	if( aExpectedPass )
		{
		if (iSpeechClient->CommitChanges()!=KErrNone)
			return EFail;
		//[ wait for trainning commited event ]
		INFO_PRINTF1(_L("Commit Completed"));
		}
	else
		{
		// start a recognition session since commit Train should fail in this mode
		if( StartRecSession( ESdMode ) != EPass )
			{
			return EFail;
			}
		
		if (iSpeechClient->CommitChanges()!=KErrNone)
			return EFail;
		//[ wait for changes commited event ]
		INFO_PRINTF1(_L("Commit Failed as Expected"));
		}
	return EPass;
	}
	
TVerdict RTestMMF_ASRCLStep::StartRecSession(TRecognitionMode aMode, TBool aSuccessExpected )
	{
	TVerdict result = EPass;
	INFO_PRINTF1(_L("Start recognition session"));	
	TInt error;
	error = iSpeechClient->StartRecSession(aMode);
	if (error != KErrNone)
		{
		if( aSuccessExpected )
			{
			INFO_PRINTF2(_L("failed to start recognition session - error: %d"),error);
			result = EFail;
			}
		else
			{
			INFO_PRINTF2(_L("failed to start recognition session (expected): %d"),error);
			result = EPass;
			}
		}
	return result;
	}
	
TVerdict RTestMMF_ASRCLStep::EndRecSession()
	{
	INFO_PRINTF1(_L("End recognition session"));	
	TInt error;
	error = iSpeechClient->EndRecSession();
	if (error != KErrNone)
		{
		INFO_PRINTF2(_L("failed to end recognition session - error: %d"),error);
		return EFail;
		}
	return EPass;
	}


TVerdict RTestMMF_ASRCLStep::LoadModels(TModelBankID aModelBankID, TBool aSuccessExpected)
	{
	INFO_PRINTF2(_L("Load Model Bank id :%d"), aModelBankID);
	TInt error;
	error = iSpeechClient->LoadModels(aModelBankID);
	if (error != KErrNone)
		{
		INFO_PRINTF2(_L("Loading model bank - error: %d"), error);
		return EFail;
		}	
		
	if (iNegativeAsyncCallTesting)
		{	
		INFO_PRINTF1(_L("Performing negative testing on async call"));
		error = iSpeechClient->LoadModels(aModelBankID);
		if (error != KErrInUse)
			{
			INFO_PRINTF2(_L("Expected KErrInUse, instead error: %d returned"),error);
			return EFail;
			}
		}

	if (aSuccessExpected)
		{
		if (!WaitForEvent(KUidAsrEventLoadModels))
			return EFail;
		INFO_PRINTF1(_L("Model Bank Loaded"));
		}
	else
		{
		if (!WaitForEvent(KUidAsrEventLoadModels, KErrNotFound))
			return EFail;
		INFO_PRINTF1(_L("Model Bank failed to load as expected"));
		}
	return EPass;
	}

TVerdict RTestMMF_ASRCLStep::LoadLexicon(TLexiconID aLexiconID, TBool aSuccessExpected)
	{
	INFO_PRINTF2(_L("Load Lexicon id: %d"), aLexiconID);
	TInt error;
	error = iSpeechClient->LoadLexicon(aLexiconID);
	if (error !=KErrNone)
		{
		INFO_PRINTF2(_L("Loading lexicon - error: %d"), error);
		return EFail;
		}	
	if (iNegativeAsyncCallTesting)
		{
		INFO_PRINTF1(_L("Performing negative testing on async call"));
		error = iSpeechClient->LoadLexicon(aLexiconID);
		if (error != KErrInUse)
			{
			INFO_PRINTF2(_L("Expected KErrInUse, instead error: %d returned"),error);
			return EFail;
			}
		}

	if (aSuccessExpected)
		{
		if (!WaitForEvent(KUidAsrEventLoadLexicon))
			return EFail;
		INFO_PRINTF1(_L("Lexicon Loaded"));
		}
	else
		{
		if (!WaitForEvent(KUidAsrEventLoadLexicon, KErrNotFound))
			return EFail;
		INFO_PRINTF1(_L("Lexicon failed to load as expected"));
		}
	
	return EPass;
	}
	
TVerdict RTestMMF_ASRCLStep::LoadGrammar(TGrammarID aGrammarID, TBool aSuccessExpected)
	{
	INFO_PRINTF2(_L("Load Grammar id: %d"), aGrammarID);
	TInt error;
	error = iSpeechClient->LoadGrammar(aGrammarID);
	if (error !=KErrNone)
		{
		INFO_PRINTF2(_L("Loading grammar - error: %d"), error);
		return EFail;
		}	
	
	if (iNegativeAsyncCallTesting)
		{
		INFO_PRINTF1(_L("Performing negative testing on async call"));
		error = iSpeechClient->LoadGrammar(aGrammarID);
		if (error != KErrInUse)
			{
			INFO_PRINTF2(_L("Expected KErrInUse, instead error: %d returned"),error);
			return EFail;
			}
		}

	
	if (aSuccessExpected)
		{
		if (!WaitForEvent(KUidAsrEventLoadGrammar))
			return EFail;
		INFO_PRINTF1(_L("Grammar Loaded"));
		}
	else
		{
		if (!WaitForEvent(KUidAsrEventLoadGrammar, KErrNotFound))
			return EFail;
		INFO_PRINTF1(_L("Grammar failed to load as expected"));
		}

	return EPass;
	}


TVerdict RTestMMF_ASRCLStep::Recognize(CSDClientResultSet& aClientResultSet) 
	{
	INFO_PRINTF1(_L("Recognize - negative test"));
	TInt err = iSpeechClient->Recognize(aClientResultSet);
	
	if (err!=KErrNone)
		{
		INFO_PRINTF2(_L("Unexpected error: %d"),err);
		return EFail;
		}
		
	if (iNegativeAsyncCallTesting)
		{
		INFO_PRINTF1(_L("Performing negative testing on async call"));
		err = iSpeechClient->Recognize(aClientResultSet);
		if (err!=KErrInUse)
			{
			INFO_PRINTF3(_L("Expected KErrInUse(%d), but received errorcode: %d"),KErrInUse,err);
			return EFail;
			}
		}
	
	if (!WaitForEvent(KUidAsrEventRecognitionReady))
		return EFail;
		
	INFO_PRINTF1(_L("Recognition Ready"));
	

	if (RecordUtterance() != EPass)
		return EFail;

	if (!WaitForEvent(KUidAsrEventRecognition))
		return EFail;

	INFO_PRINTF1(_L("Recognition test completed successfully"));
	return EPass;
	}
	
TVerdict RTestMMF_ASRCLStep::LoadEngineParameters(const RArray<TInt>& aParameterIDs, const RArray<TInt>& aParameterValues)
	{
	TInt err = iSpeechClient->LoadEngineParameters(aParameterIDs, aParameterValues);
	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("Unexpected error: %d"),err);
		return EFail;
		}
		
	if (!WaitForEvent(KUidTestASRLoadParameters))
		return EFail;
		
	INFO_PRINTF1(_L("Load engine parameters succeeded"));
			
	return EPass;
	}

	
	
TVerdict RTestMMF_ASRCLStep::RemoveRule(TGrammarID aGrammarID, TRuleID aRuleID)
	{
	INFO_PRINTF3(_L("Remove rule id :%d from Grammar id: %d"), aRuleID, aGrammarID);
	TInt error;
	__ASSERT_ALWAYS(iSpeechClient, User::Panic(_L("TSU_MMF_ASRCL"),KNullPointer));
	error = iSpeechClient->RemoveRule(aGrammarID, aRuleID);
	if (error != KErrNone)
		{
		INFO_PRINTF2(_L("Remove rule - error: %d"), error);
		return EFail;
		}	
		
	if (iNegativeAsyncCallTesting)
		{
		INFO_PRINTF1(_L("Performing negative testing on async call"));
		error = iSpeechClient->RemoveRule(aGrammarID, aRuleID);
		if (error!=KErrInUse)
			{
			INFO_PRINTF2(_L("Expected KErrInUse(-14), but received errorcode: %d"),error);
			return EFail;
			}
		}

	if (!WaitForEvent(KUidAsrEventRemoveRule))
		return EFail;
	INFO_PRINTF1(_L("Rule Removed"));
	return EPass;
	}

TVerdict RTestMMF_ASRCLStep::RemoveGrammar(TGrammarID aGrammarID )
	{
	INFO_PRINTF2(_L("Remove Grammar id :%d"), aGrammarID );
	TInt error;
	__ASSERT_ALWAYS(iSpeechClient, User::Panic(_L("TSU_MMF_ASRCL"),KNullPointer));
	error = iSpeechClient->RemoveGrammar(aGrammarID );
	if (error != KErrNone)
		{
		INFO_PRINTF2(_L("Remove Grammar - error: %d"), error);
		return EFail;
		}	
	if (iNegativeAsyncCallTesting)
		{
		INFO_PRINTF1(_L("Performing negative testing on async call"));
		error = iSpeechClient->RemoveGrammar(aGrammarID );
		if (error!=KErrInUse)
			{
			INFO_PRINTF2(_L("Expected KErrInUse(-14), but received errorcode: %d"),error);
			return EFail;
			}
		}

		
	if (!WaitForEvent(KUidAsrEventRemoveGrammar))
		return EFail;


	INFO_PRINTF1(_L("Grammar Removed"));
	return EPass;
	}

TVerdict RTestMMF_ASRCLStep::RemoveLexicon(TLexiconID aLexiconID )
	{
	INFO_PRINTF2(_L("Remove Lexicon id :%d"), aLexiconID );
	TInt error;
	__ASSERT_ALWAYS(iSpeechClient, User::Panic(_L("TSU_MMF_ASRCL"),KNullPointer));
	error = iSpeechClient->RemoveLexicon(aLexiconID );
	if (error != KErrNone)
		{
		INFO_PRINTF2(_L("Remove Lexicon - error: %d"), error);
		return EFail;
		}	
	if (iNegativeAsyncCallTesting)
		{
		INFO_PRINTF1(_L("Performing negative testing on async call"));
		error = iSpeechClient->RemoveLexicon(aLexiconID );
		if (error!=KErrInUse)
			{
			INFO_PRINTF2(_L("Expected KErrInUse(-14), but received errorcode: %d"),error);
			return EFail;
			}
		}

		
	if (!WaitForEvent(KUidAsrEventRemoveLexicon))
		return EFail;
	
	INFO_PRINTF2(_L("Lexicon Removed: %d"), aLexiconID );
	return EPass;
	}

TVerdict RTestMMF_ASRCLStep::RemoveModelBank(TModelBankID aModelBankID )
	{
	INFO_PRINTF2(_L("Remove ModelBank id :%d"), aModelBankID );
	TInt error;
	__ASSERT_ALWAYS(iSpeechClient, User::Panic(_L("TSU_MMF_ASRCL"),KNullPointer));
	error = iSpeechClient->RemoveModelBank(aModelBankID );
	if (error != KErrNone)
		{
		INFO_PRINTF2(_L("Remove ModelBank - error: %d"), error);
		return EFail;
		}	
	if (iNegativeAsyncCallTesting)
		{
		INFO_PRINTF1(_L("Performing negative testing on async call"));
		error = iSpeechClient->RemoveModelBank(aModelBankID );
		if (error!=KErrInUse)
			{
			INFO_PRINTF2(_L("Expected KErrInUse(-14), but received errorcode: %d"),error);
			return EFail;
			}
		}

		
	if (!WaitForEvent(KUidAsrEventRemoveModelBank))
		return EFail;
	
	INFO_PRINTF2(_L("ModelBank Removed: %d"), aModelBankID );
	return EPass;
	}
	
TVerdict RTestMMF_ASRCLStep::UnloadRule(TGrammarID aGrammarID, TRuleID aRuleID)
	{
	INFO_PRINTF3(_L("Unload rule id :%d from Grammar id: %d"), aRuleID, aGrammarID);
	TInt error;
	__ASSERT_ALWAYS(iSpeechClient, User::Panic(_L("TSU_MMF_ASRCL"),KNullPointer));
	error = iSpeechClient->UnloadRule(aGrammarID, aRuleID);
	if (error != KErrNone)
		{
		INFO_PRINTF2(_L("Unload rule - error: %d"), error);
		return EFail;
		}	
	if (iNegativeAsyncCallTesting)
		{
		INFO_PRINTF1(_L("Performing negative testing on async call"));
		error = iSpeechClient->UnloadRule(aGrammarID, aRuleID);
		if (error!=KErrInUse)
			{
			INFO_PRINTF2(_L("Expected KErrInUse(-14), but received errorcode: %d"),error);
			return EFail;
			}
		}

	if (!WaitForEvent(KUidAsrEventUnloadRule))
		return EFail;
	INFO_PRINTF1(_L("Rule unloaded"));
	return EPass;
	}

	
TVerdict RTestMMF_ASRCLStep::RemovePronunciation(TLexiconID aLexiconID, TPronunciationID aPronunciationID)
	{
	INFO_PRINTF3(_L("Remove pronunciation id :%d from Lexicon id: %d"), aPronunciationID, aLexiconID);
	TInt error;
	__ASSERT_ALWAYS(iSpeechClient, User::Panic(_L("TSU_MMF_ASRCL"),KNullPointer));
	error = iSpeechClient->RemovePronunciation(aLexiconID, aPronunciationID);
	if (error != KErrNone)
		{
		INFO_PRINTF2(_L("Remove pronunciation - error: %d"), error);
		return EFail;
		}	
		
	if (iNegativeAsyncCallTesting)
		{
		INFO_PRINTF1(_L("Performing negative testing on async call"));
		error = iSpeechClient->RemovePronunciation(aLexiconID, aPronunciationID);
		if (error!=KErrInUse)
			{
			INFO_PRINTF2(_L("Expected KErrInUse(-14), but received errorcode: %d"),error);
			return EFail;
			}
		}

	if (!WaitForEvent(KUidAsrEventRemovePronunciation))
		return EFail;

	INFO_PRINTF1(_L("Pronunciation Removed"));
	return EPass;
	}

TVerdict RTestMMF_ASRCLStep::RemoveModel(TModelBankID aModelBankID, TModelID aModelID)
	{
	INFO_PRINTF3(_L("Remove model id :%d from Model bank id: %d"), aModelID, aModelBankID);
	TInt error;
	__ASSERT_ALWAYS(iSpeechClient, User::Panic(_L("TSU_MMF_ASRCL"),KNullPointer));
	error = iSpeechClient->RemoveModel(aModelBankID, aModelID);
	if (error != KErrNone)
		{
		INFO_PRINTF2(_L("Remove model - error: %d"), error);
		return EFail;
		}
	if (iNegativeAsyncCallTesting)
		{
		INFO_PRINTF1(_L("Performing negative testing on async call"));
		error = iSpeechClient->RemoveModel(aModelBankID, aModelID);
		if (error!=KErrInUse)
			{
			INFO_PRINTF2(_L("Expected KErrInUse(-14), but received errorcode: %d"),error);
			return EFail;
			}
		}

		
	if (!WaitForEvent(KUidAsrEventRemoveModel))
		return EFail;
	INFO_PRINTF1(_L("Model Removed"));
	return EPass;
	}

TVerdict RTestMMF_ASRCLStep::PlayUtterance(TModelBankID aModelBankID, TModelID aModelID, TBool aExpectPass )
	{
	INFO_PRINTF1(_L("Play Utterance"));
	__ASSERT_ALWAYS(iSpeechClient, User::Panic(_L("TSU_MMF_ASRCL"),KNullPointer));
	iSpeechClient->PlayUtterance(aModelBankID, aModelID);
	
	if (!WaitForEvent(KUidAsrEventPlayStarted))
		return EFail;

	if( aExpectPass )
		{
		INFO_PRINTF1(_L("Playback started"));
		if (!WaitForEvent(KUidAsrEventPlay))
			return EFail;
		INFO_PRINTF1(_L("Playback completed successfully"));
		}
	else
		{
		//[ expected failure ]
		if (!WaitForEvent(KUidAsrEventPlay, KErrNotReady))
			return EFail;
		INFO_PRINTF1(_L("Playback failed as expected"));
		}

	return EPass;
	}

TVerdict RTestMMF_ASRCLStep::StartTraining(TModelBankID aModelBankID, TModelID& aModelID) 
	{
	INFO_PRINTF2(_L("Train new model in model bank %d"),aModelBankID);
	__ASSERT_ALWAYS(iSpeechClient, User::Panic(_L("TSU_MMF_ASRCL"),KNullPointer));
	iSpeechClient->Train(aModelBankID, aModelID);
	
	if (!WaitForEvent(KUidAsrEventTrainReady))
		return EFail;
		
	INFO_PRINTF1(_L("Model ready for training"));

	if (!WaitForEvent(KUidAsrEventTrain))
		return EFail;
		
	INFO_PRINTF1(_L("Training Complete"));
		
	return EPass;
	}

/**
*
* StartTrainingBadModelID
* This method uses a bad mode bank id to start trainning
* an error is expected
*/	
TVerdict RTestMMF_ASRCLStep::StartTrainingBadModelBankID()
	{
	//[ use an unlikely id ]
	TModelBankID aModelBankID = KBadModelBankID;
	TModelID aModelID = KBadModelID;
	INFO_PRINTF2(_L("Train new model with bad model bank %d"),aModelBankID);
	__ASSERT_ALWAYS(iSpeechClient, User::Panic(_L("TSU_MMF_ASRCL"),KNullPointer));
	iSpeechClient->Train(aModelBankID, aModelID);
	
	if (!WaitForEvent(KUidAsrEventTrainReady, KErrNone))
		return EFail;
	
	INFO_PRINTF1(_L("Model not ready for training"));
	
	if (!WaitForEvent(KUidAsrEventTrain, KErrNotReady))
		return EFail;
	
	INFO_PRINTF1(_L("Training Failed as expected"));
	
	return EPass;
	}

TVerdict RTestMMF_ASRCLStep::CheckModelCount(TModelBankID aModelBankID,	TInt aExpectedModelCount)
	{
	TInt modelCount;
	__ASSERT_ALWAYS(iSpeechClient, User::Panic(_L("TSU_MMF_ASRCL"),KNullPointer));

	TInt err = iSpeechClient->GetModelCount(aModelBankID,modelCount);
	
	if (err !=KErrNone)
		{
		INFO_PRINTF2(_L("Unexpected error returning model count error: %d"),err);
		return EFail;
		}
		
	if (!WaitForEvent(KUidAsrEventGetModelCount))
		return EFail;
		
	if (modelCount!=aExpectedModelCount)
		{
		INFO_PRINTF4(_L("model bank:%d model count %d, expected %d"),aModelBankID, modelCount,aExpectedModelCount);
		return EFail;
		}
		
	return EPass;
	}		

TVerdict RTestMMF_ASRCLStep::CreateLexicon(TLexiconID& aLexiconID)
	{
	TInt err;
	__ASSERT_ALWAYS(iSpeechClient, User::Panic(_L("TSU_MMF_ASRCL"),KNullPointer));
	err = iSpeechClient->CreateLexicon(aLexiconID);
	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("Error creating lexicon: %d"),err);
		return EFail;
		}
	if (!WaitForEvent(KUidAsrEventCreateLexicon))
		{
		return EFail;
		}

	return EPass;
	}
	
TVerdict RTestMMF_ASRCLStep::CreateGrammar(TGrammarID& aGrammarID)
	{
	TInt err;
	__ASSERT_ALWAYS(iSpeechClient, User::Panic(_L("TSU_MMF_ASRCL"),KNullPointer));
	err = iSpeechClient->CreateGrammar(aGrammarID);
	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("Error creating grammar: %d"),err);
		return EFail;
		}
	if (!WaitForEvent(KUidAsrEventCreateGrammar))
		{
		return EFail;
		}
		
		
	return EPass;
	}
	
TVerdict RTestMMF_ASRCLStep::CreateModelBank(TModelBankID& aModelBankID)
	{
	TInt err;
	__ASSERT_ALWAYS(iSpeechClient, User::Panic(_L("TSU_MMF_ASRCL"),KNullPointer));
	err = iSpeechClient->CreateModelBank(aModelBankID);
	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("Error creating model bank: %d"),err);
		return EFail;
		}
	if (!WaitForEvent(KUidAsrEventCreateModelBank))
		{
		return EFail;
		}
		
		
	return EPass;
	}

TVerdict RTestMMF_ASRCLStep::CancelTest()
	{
	__ASSERT_ALWAYS(iSpeechClient, User::Panic(_L("TSU_MMF_ASRCL"),KNullPointer));
	
	// Don't actually cancel a real event, as our limited test controller can't actually cancel events
	// We just wait for the test controller TEST callback to know that the controller received the cancel event

	iSpeechClient->Cancel();

	if (!WaitForEvent(KUidTestASRCancel))
		{
		INFO_PRINTF1(_L("Cancel not received by controller"));
		return EFail;
		}
	
	iSpeechClient->Cancel();
	
	if (!WaitForEvent(KUidTestASRCancel))
		{
		INFO_PRINTF1(_L("Cancel not received by controller"));
		return EFail;
		}

	return EPass;
	}

TVerdict RTestMMF_ASRCLStep::AddPronunciation(TLexiconID aLexiconID, TModelBankID aModelBankID, TModelID aModelID, 
			TPronunciationID& aPronunciationID, TBool aSuccessExpected)
	{
	TInt err;
	TVerdict result = EPass;

	__ASSERT_ALWAYS(iSpeechClient, User::Panic(_L("TSU_MMF_ASRCL"),KNullPointer));
	
	if( aSuccessExpected )
		{
		//[ lets get id's before ]
		RArray<TPronunciationID> oldIds;
		CleanupClosePushL(oldIds);
        
		if( GetAllPronunciationIDs( aLexiconID, oldIds ) != EPass )
			{
			INFO_PRINTF1(_L("Error adding pronunciation"));	
			result = EFail;
			}
		
		if (result==EPass)
			{
			err = iSpeechClient->AddPronunciation(aLexiconID, aModelBankID, aModelID, aPronunciationID);
			if (err != KErrNone )
				{
				INFO_PRINTF2(_L("Error adding pronunciation: %d"),err);	
				result = EFail;
				}
			}
			
		if (result == EPass && iNegativeAsyncCallTesting)
			{	
			INFO_PRINTF1(_L("Performing negative testing on async call"));
			err = iSpeechClient->AddPronunciation(aLexiconID, aModelBankID, aModelID, aPronunciationID);
			if (err != KErrInUse)
				{
				INFO_PRINTF2(_L("Expected KErrInUse, instead error: %d returned"),err);
				result = EFail;
				}
			}

		
		if (result == EPass && WaitForEvent(KUidAsrEventAddPronunciation))
			INFO_PRINTF2(_L("Pronunciation Added, id:%d"),aPronunciationID);
		else
			result = EFail;
		
		RArray<TPronunciationID> newIds;
		CleanupClosePushL(newIds);

		if( result == EPass && GetAllPronunciationIDs(aLexiconID, newIds ) != EPass )
			{
			result = EFail;
			}

		if(result == EPass && ( (!newIds.Find(aPronunciationID)) || ( oldIds.Count() >= newIds.Count())))
			{
			INFO_PRINTF1(_L("Error adding pronunciation"));
			result = EFail;
			}
		CleanupStack::PopAndDestroy(2); //newIDs, oldIDs
		}
	else
		{
		//[ expected failure processing]
		err = iSpeechClient->AddPronunciation(aLexiconID, aModelBankID, aModelID, aPronunciationID);
		if (err != KErrNone )
			{
			result = EFail;
			}
		
		if (result == EPass && !WaitForEvent(KUidAsrEventAddPronunciation,KErrNotFound))
			{
			return EFail;
			}
		}
	
	return result;
	}
	
	
TVerdict RTestMMF_ASRCLStep::AddRule(TGrammarID aGrammarID, TLexiconID aLexiconID, TPronunciationID aPronunciationID,
									 TRuleID& aRuleID, TBool aSuccessExpected)
	{
	TInt err;
	TVerdict result = EPass;
	
	__ASSERT_ALWAYS(iSpeechClient, User::Panic(_L("TSU_MMF_ASRCL"),KNullPointer));
	if( aSuccessExpected )
		{
		RArray<TRuleID> oldIds;		
		CleanupClosePushL(oldIds); 
		
		if(GetAllRuleIDs( aGrammarID, oldIds ) != EPass )
			{
			INFO_PRINTF1(_L("Error adding Rule"));	
			result = EFail;
			}
		
		if (result == EPass)
			{
			err = iSpeechClient->AddRule(aGrammarID, aLexiconID, aPronunciationID, aRuleID);
			if (err != KErrNone)
				{
				INFO_PRINTF2(_L("Error adding rule: %d"),err);
				result = EFail;
				}
			}
		
		if (result == EPass && !WaitForEvent(KUidAsrEventAddRule))
			result = EFail;
		
		RArray<TRuleID> newIds;
		CleanupClosePushL(newIds); 

		if( result == EPass && GetAllRuleIDs( aGrammarID, newIds ) != EPass )
			{
			INFO_PRINTF1(_L("Error adding Rule"));	
			result = EFail;
			}
		
		if (result == EPass && ( (!newIds.Find(aRuleID)) || ( oldIds.Count() >= newIds.Count())))
			{
			INFO_PRINTF1(_L("Error adding Rule"));
			result = EFail;
			}
			
		CleanupStack::PopAndDestroy(2);//oldIDs, newIDs
		if (result == EPass)
			{
			INFO_PRINTF2(_L("Rule added, id:%d"),aRuleID);
			}
		}
	else
		{
		err = iSpeechClient->AddRule(aGrammarID, aLexiconID, aPronunciationID, aRuleID);
		if (err != KErrNone)
			{
			result = EFail;
			}

		if (result == EPass && (WaitForEvent(KUidAsrEventAddRule, KErrNotFound)))
			INFO_PRINTF1(_L("Add Rule Failed as Expected"));	
		else
			result = EFail;
		}

	return result;
	}

	
TVerdict RTestMMF_ASRCLStep::CheckRuleValidity(TGrammarID aGrammarID, TRuleID aRuleID, TBool aExpectedValidity, TBool aExpectedError)
	{
	TInt err;
	__ASSERT_ALWAYS(iSpeechClient, User::Panic(_L("TSU_MMF_ASRCL"),KNullPointer));
	TBool aValidity;
	err = iSpeechClient->GetRuleValidity(aGrammarID, aRuleID, aValidity);
	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("Error retrieving rule validity: %d"),err);
		return EFail;
		}
	
	if (iNegativeAsyncCallTesting)
		{	
		INFO_PRINTF1(_L("Performing negative testing on async call"));
		err = iSpeechClient->GetRuleValidity(aGrammarID, aRuleID, aValidity);
		if (err != KErrInUse)
			{
			INFO_PRINTF2(_L("Expected KErrInUse, instead error: %d returned"),err);
			return EFail;
			}
		}
		
	if (!WaitForEvent(KUidAsrEventGetRuleValidity, aExpectedError))
		return EFail;

	if (iLastError != KErrNone)
		{
		INFO_PRINTF2(_L("Error code %d returned as expected"),iLastError);
		return EPass;
		}
		
	if (aValidity != aExpectedValidity)
		{
		if (aExpectedValidity)
			INFO_PRINTF1(_L("Rule expected to be valid, but invalid rule returned"));
		else
			INFO_PRINTF1(_L("Rule expected to be invalid, but valid rule returned"));
		return EFail;
		}

	// Expected value returned, note this and return
	if (aValidity)
		INFO_PRINTF1(_L("Rule is valid (Expected)"));
	else
		INFO_PRINTF1(_L("Rule is invalid (Expected)"));

	return EPass;
	}

	
	
TVerdict RTestMMF_ASRCLStep::GetAllModelIDs(TModelBankID aModelBankID, RArray<TModelID>& aModelIDArray)
	{
	TInt err;
	__ASSERT_ALWAYS(iSpeechClient, User::Panic(_L("TSU_MMF_ASRCL"),KNullPointer));
	err = iSpeechClient->GetAllModelIDs(aModelBankID, aModelIDArray);
	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("Error retrieving model IDs: %d"),err);
		return EFail;
		}
	if (iNegativeAsyncCallTesting)
		{
		INFO_PRINTF1(_L("Performing negative testing on async call"));
		err = iSpeechClient->GetAllModelIDs(aModelBankID, aModelIDArray);
		if (err != KErrInUse)
			{
			INFO_PRINTF2(_L("Expected KErrInUse, instead error: %d returned"),err);
			return EFail;
			}
		}

	if (!WaitForEvent(KUidAsrEventGetAllModelIDs))
		return EFail;
		
	INFO_PRINTF1(_L("GetAllModelIDs Test passed"));
	
	return EPass;
	}

	
	
TVerdict RTestMMF_ASRCLStep::GetAllPronunciationIDs(TLexiconID aLexiconID, RArray<TPronunciationID>& aPronunciationIDArray)
	{
	TInt err;
	__ASSERT_ALWAYS(iSpeechClient, User::Panic(_L("TSU_MMF_ASRCL"),KNullPointer));
	err = iSpeechClient->GetAllPronunciationIDs(aLexiconID, aPronunciationIDArray);
		
	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("Error retrieving pronuciation IDs: %d"),err);
		return EFail;
		}
	if (iNegativeAsyncCallTesting)
		{
		INFO_PRINTF1(_L("Performing negative testing on async call"));
		err = iSpeechClient->GetAllPronunciationIDs(aLexiconID, aPronunciationIDArray);
		if (err != KErrInUse)
			{
			INFO_PRINTF2(_L("Expected KErrInUse, instead error: %d returned"),err);
			return EFail;
			}
		}

	if (!WaitForEvent(KUidAsrEventGetAllPronunciationIDs))
		return EFail;
		
	INFO_PRINTF1(_L("GetAllPronunciationIDs Test passed"));
	return EPass;
	}



TVerdict RTestMMF_ASRCLStep::GetAllRuleIDs(TGrammarID aGrammarID, RArray<TRuleID>& aRuleIDArray)
	{
	TInt err;
	__ASSERT_ALWAYS(iSpeechClient, User::Panic(_L("TSU_MMF_ASRCL"),KNullPointer));
	err = iSpeechClient->GetAllRuleIDs(aGrammarID, aRuleIDArray);
	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("Error retrieving rule IDs: %d"),err);
		return EFail;
		}
	if (iNegativeAsyncCallTesting)
		{
		INFO_PRINTF1(_L("Performing negative testing on async call"));
		err = iSpeechClient->GetAllRuleIDs(aGrammarID, aRuleIDArray);
		if (err != KErrInUse)
			{
			INFO_PRINTF2(_L("Expected KErrInUse, instead error: %d returned"),err);
			return EFail;
			}
		}

	if (!WaitForEvent(KUidAsrEventGetAllRuleIDs))
		return EFail;
		
	INFO_PRINTF1(_L("GetAllRuleIDs test passed"));
	
	return EPass;
	}


TVerdict RTestMMF_ASRCLStep::GetAllModelBankIDs(RArray<TModelID>& aModelBankIDArray, TBool aOwnedByClient)
	{
	TInt err;
	__ASSERT_ALWAYS(iSpeechClient, User::Panic(_L("TSU_MMF_ASRCL"),KNullPointer));
	if (aOwnedByClient)
		err = iSpeechClient->GetAllClientModelBankIDs(aModelBankIDArray);
	else
		err = iSpeechClient->GetAllModelBankIDs(aModelBankIDArray);
		
	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("Error retrieving model IDs: %d"),err);
		return EFail;
		}
	if (iNegativeAsyncCallTesting)
		{
		INFO_PRINTF1(_L("Performing negative testing on async call"));
		if (aOwnedByClient)
			err = iSpeechClient->GetAllClientModelBankIDs(aModelBankIDArray);
		else
			err = iSpeechClient->GetAllModelBankIDs(aModelBankIDArray);		
			
		if (err != KErrInUse)
			{
			INFO_PRINTF2(_L("Expected KErrInUse, instead error: %d returned"),err);
			return EFail;
			}
		}

	if (!WaitForEvent(aOwnedByClient? KUidAsrEventGetAllClientModelBankIDs : KUidAsrEventGetAllModelBankIDs))
		return EFail;
		
	INFO_PRINTF1(_L("GetAllModelBankIDs Test passed"));
	
	return EPass;
	}

	
	
TVerdict RTestMMF_ASRCLStep::GetAllLexiconIDs(RArray<TPronunciationID>& aLexiconIDArray, TBool aOwnedByClient)
	{
	TInt err;
	__ASSERT_ALWAYS(iSpeechClient, User::Panic(_L("TSU_MMF_ASRCL"),KNullPointer));
	if (aOwnedByClient)
		err = iSpeechClient->GetAllClientLexiconIDs(aLexiconIDArray);
	else
		err = iSpeechClient->GetAllLexiconIDs(aLexiconIDArray);
		
	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("Error retrieving pronuciation IDs: %d"),err);
		return EFail;
		}
	if (iNegativeAsyncCallTesting)
		{
		INFO_PRINTF1(_L("Performing negative testing on async call"));
		if (aOwnedByClient)
			err = iSpeechClient->GetAllClientLexiconIDs(aLexiconIDArray);
		else
			err = iSpeechClient->GetAllLexiconIDs(aLexiconIDArray);

		if (err != KErrInUse)
			{
			INFO_PRINTF2(_L("Expected KErrInUse, instead error: %d returned"),err);
			return EFail;
			}
		}

	if (!WaitForEvent(aOwnedByClient? KUidAsrEventGetAllClientLexiconIDs : KUidAsrEventGetAllLexiconIDs))
		return EFail;
		
	INFO_PRINTF1(_L("GetAllLexiconIDs Test passed"));
	return EPass;
	}



TVerdict RTestMMF_ASRCLStep::GetAllGrammarIDs(RArray<TGrammarID>& aGrammarIDArray, TBool aOwnedByClient)
	{
	TInt err;
	__ASSERT_ALWAYS(iSpeechClient, User::Panic(_L("TSU_MMF_ASRCL"),KNullPointer));
	if (aOwnedByClient)
		err = iSpeechClient->GetAllClientGrammarIDs(aGrammarIDArray);
	else
		err = iSpeechClient->GetAllGrammarIDs(aGrammarIDArray);
		
	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("Error retrieving grammar IDs: %d"),err);
		return EFail;
		}
	if (iNegativeAsyncCallTesting)
		{
		INFO_PRINTF1(_L("Performing negative testing on async call"));
		if (aOwnedByClient)
			err = iSpeechClient->GetAllClientGrammarIDs(aGrammarIDArray);
		else
			err = iSpeechClient->GetAllGrammarIDs(aGrammarIDArray);

		if (err != KErrInUse)
			{
			INFO_PRINTF2(_L("Expected KErrInUse, instead error: %d returned"),err);
			return EFail;
			}
		}

	if (!WaitForEvent(aOwnedByClient?KUidAsrEventGetAllClientGrammarIDs : KUidAsrEventGetAllGrammarIDs))
		return EFail;
		
	INFO_PRINTF1(_L("GetAllGrammarIDs test passed"));
	
	return EPass;
	}



TVerdict RTestMMF_ASRCLStep::OpenL()
	{	
	iScheduler = new(ELeave)CActiveScheduler;
	//[install the active scheduler ]
	CActiveScheduler::Install( iScheduler );
	
	switch (iInitialDatabaseState)
		{
	case EReset:
		ResetDatabaseL();
		break;
	case ETestDatabase:
		CreateTestDatabaseL();
		break;
	case ENoDatabase:
		break;
	default:
		ASSERT(EFalse);
		};
	
	if (iCreateClient)
		CreateSpeechClientL(KUidTestASRClient);

	return RTestStep::OpenL();
	}
	
	
void RTestMMF_ASRCLStep::Close()
	{
	delete iScheduler;
	iScheduler = NULL;
	delete iSpeechClient;
	iSpeechClient = NULL;
	}

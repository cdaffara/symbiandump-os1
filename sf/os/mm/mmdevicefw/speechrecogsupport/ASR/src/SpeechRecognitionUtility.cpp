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

#include <e32base.h>
#include <mmf/common/mmfcontroller.h>
#include <mmf/common/speechrecognitioncustomcommands.h>
#include <mmf/common/speechrecognitiondataclient.h>

#include <speechrecognitionutility.h>
#include "SpeechRecognitionUtilityBody.h"

_LIT(KMMFSpeechPanicCategory, "SpeechClient");
GLDEF_C void Panic(TInt aError)
	{
	User::Panic(KMMFSpeechPanicCategory, aError);
	}

EXPORT_C CSpeechRecognitionUtility* CSpeechRecognitionUtility::NewL(MSpeechRecognitionUtilityObserver& aSpeechRecognitionUtilityObserver, TUid aClientUid)
	{
	CSpeechRecognitionUtility* s = CSpeechRecognitionUtility::NewLC(aSpeechRecognitionUtilityObserver, aClientUid);
	CleanupStack::Pop(s);
	return s;
	}
	
EXPORT_C CSpeechRecognitionUtility* CSpeechRecognitionUtility::NewLC(MSpeechRecognitionUtilityObserver& aSpeechRecognitionUtilityObserver, TUid aClientUid)
	{
	CSpeechRecognitionUtility* s = new(ELeave) CSpeechRecognitionUtility;
	CleanupStack::PushL(s);
	s->iBody = CSpeechRecognitionUtility::CBody::NewL(aClientUid, aSpeechRecognitionUtilityObserver);
	return s;	
	}
	
CSpeechRecognitionUtility::CSpeechRecognitionUtility()
	{
	}

EXPORT_C CSpeechRecognitionUtility::~CSpeechRecognitionUtility()
	{
	delete iBody;
	}

EXPORT_C TInt CSpeechRecognitionUtility::GetEngineProperties(const RArray<TInt>& aArray1, RArray<TInt>& aArray2)
	{
	ASSERT(iBody);
	return iBody->GetEngineProperties(aArray1, aArray2);
	}

EXPORT_C TInt CSpeechRecognitionUtility::AddPronunciation(TLexiconID aLexiconID, TModelBankID aModelBankID, TModelID aModelID, TPronunciationID& aPronunciationID)
	{
	ASSERT(iBody);
	return iBody->AddPronunciation(aLexiconID, aModelBankID, aModelID, aPronunciationID);
	}

EXPORT_C TInt CSpeechRecognitionUtility::AddRule(TGrammarID aGrammarID, TLexiconID aLexiconID, TPronunciationID aPronunciationID, TRuleID& aRuleID)
	{
	ASSERT(iBody);
	return iBody->AddRule(aGrammarID, aLexiconID, aPronunciationID, aRuleID);
	}

EXPORT_C void CSpeechRecognitionUtility::Cancel()
	{
	ASSERT(iBody);
	iBody->Cancel();
	}

EXPORT_C TInt CSpeechRecognitionUtility::CommitChanges()
	{
	ASSERT(iBody);
	return iBody->CommitChanges();
	}

EXPORT_C TInt CSpeechRecognitionUtility::CreateGrammar(TGrammarID& aGrammarID)
	{
	ASSERT(iBody);
	return iBody->CreateGrammar(aGrammarID);
	}

EXPORT_C TInt CSpeechRecognitionUtility::CreateLexicon(TLexiconID& aLexiconID)
	{
	ASSERT(iBody);
	return iBody->CreateLexicon(aLexiconID);
	}

EXPORT_C TInt CSpeechRecognitionUtility::LoadModels(TModelBankID aModelBankID)
	{
	ASSERT(iBody);
	return iBody->LoadModels(aModelBankID);
	}

EXPORT_C TInt CSpeechRecognitionUtility::LoadGrammar(TGrammarID aGrammarID)
	{
	ASSERT(iBody);
	return iBody->LoadGrammar(aGrammarID);
	}

EXPORT_C TInt CSpeechRecognitionUtility::LoadLexicon(TLexiconID aLexiconID)
	{
	ASSERT(iBody);
	return iBody->LoadLexicon(aLexiconID);
	}

EXPORT_C TInt CSpeechRecognitionUtility::GetUtteranceDuration(TModelBankID aModelBankID, TModelID aModelID, TTimeIntervalMicroSeconds32& aDuration)
	{
	ASSERT(iBody);
	return iBody->GetUtteranceDuration(aModelBankID, aModelID, aDuration);
	}

EXPORT_C TInt CSpeechRecognitionUtility::PlayUtterance(TModelBankID aModelBankID, TModelID aModelID)
	{
	ASSERT(iBody);
	return iBody->PlayUtterance(aModelBankID, aModelID);
	}

EXPORT_C TInt CSpeechRecognitionUtility::GetModelCount(TModelBankID aModelBankID, TInt& aModelCount)
	{
	ASSERT(iBody);
	return iBody->GetModelCount(aModelBankID, aModelCount);
	}

EXPORT_C TInt CSpeechRecognitionUtility::EndRecSession()
	{
	ASSERT(iBody);
	return iBody->EndRecSession();
	}

EXPORT_C TInt CSpeechRecognitionUtility::StartRecSession(TRecognitionMode aMode)
	{
	ASSERT(iBody);
	return iBody->StartRecSession(aMode);
	}

EXPORT_C TInt CSpeechRecognitionUtility::Recognize(CSDClientResultSet& aResultSet)
	{
	ASSERT(iBody);
	return iBody->Recognize(aResultSet);
	}

EXPORT_C TInt CSpeechRecognitionUtility::Record(TTimeIntervalMicroSeconds32 aRecordTime)
	{
	ASSERT(iBody);
	return iBody->Record(aRecordTime);
	}

EXPORT_C TInt CSpeechRecognitionUtility::RemoveGrammar(TGrammarID aGrammarID)
	{
	ASSERT(iBody);
	return iBody->RemoveGrammar(aGrammarID);
	}

EXPORT_C TInt CSpeechRecognitionUtility::RemovePronunciation(TLexiconID aLexiconID, TPronunciationID aPronunciationID)
	{
	ASSERT(iBody);
	return iBody->RemovePronunciation(aLexiconID, aPronunciationID);
	}

EXPORT_C TInt CSpeechRecognitionUtility::RemoveLexicon(TLexiconID aLexiconID)
	{
	ASSERT(iBody);
	return iBody->RemoveLexicon(aLexiconID);
	}

EXPORT_C TInt CSpeechRecognitionUtility::RemoveModel(TModelBankID aModelBankID, TModelID aModelID)
	{
	ASSERT(iBody);
	return iBody->RemoveModel(aModelBankID, aModelID);
	}

EXPORT_C TInt CSpeechRecognitionUtility::RemoveRule(TGrammarID aGrammarID, TRuleID aRuleID)
	{
	ASSERT(iBody);
	return iBody->RemoveRule(aGrammarID, aRuleID);
	}
EXPORT_C TInt CSpeechRecognitionUtility::Train(TModelBankID aModelBankID, TModelID& aModelID)
	{
	ASSERT(iBody);
	return iBody->Train(aModelBankID, aModelID);
	}

EXPORT_C TInt CSpeechRecognitionUtility::UnloadRule(TGrammarID aGrammarID, TRuleID aRuleID)
	{
	ASSERT(iBody);
	return iBody->UnloadRule(aGrammarID, aRuleID);
	}

EXPORT_C void CSpeechRecognitionUtility::SetEventHandler(MSpeechRecognitionUtilityObserver* aSpeechRecognitionUtilityObserver)
	{
	ASSERT(iBody);
	iBody->SetEventHandler(aSpeechRecognitionUtilityObserver);
	}
	
EXPORT_C TInt CSpeechRecognitionUtility::GetAllPronunciationIDs(TLexiconID aLexiconID, RArray <TPronunciationID>& aPronunciationIDs)
	{
	ASSERT(iBody);
	return iBody->GetAllPronunciationIDs(aLexiconID, aPronunciationIDs);
	}

EXPORT_C TInt CSpeechRecognitionUtility::GetAllModelIDs(TModelBankID aModelBankID, RArray <TModelID>& aModelIDs)
	{
	ASSERT(iBody);
	return iBody->GetAllModelIDs(aModelBankID, aModelIDs);
	}


EXPORT_C TInt CSpeechRecognitionUtility::GetAllRuleIDs(TGrammarID aGrammarID, RArray <TRuleID>& aRuleIDs)
	{
	ASSERT(iBody);
	return iBody->GetAllRuleIDs(aGrammarID, aRuleIDs);
	}
	
EXPORT_C TInt CSpeechRecognitionUtility::GetAllClientLexiconIDs(RArray <TLexiconID>& aLexiconIDs)
	{
	ASSERT(iBody);
	return iBody->GetAllClientLexiconIDs(aLexiconIDs);
	}

EXPORT_C TInt CSpeechRecognitionUtility::GetAllClientModelBankIDs(RArray <TModelBankID>& aModelBankIDs)
	{
	ASSERT(iBody);
	return iBody->GetAllClientModelBankIDs(aModelBankIDs);
	}


EXPORT_C TInt CSpeechRecognitionUtility::GetAllClientGrammarIDs(RArray <TGrammarID>& aGrammarIDs)
	{
	ASSERT(iBody);
	return iBody->GetAllClientGrammarIDs(aGrammarIDs);
	}
	
EXPORT_C TInt CSpeechRecognitionUtility::GetAllLexiconIDs(RArray <TLexiconID>& aLexiconIDs)
	{
	ASSERT(iBody);
	return iBody->GetAllLexiconIDs(aLexiconIDs);
	}

EXPORT_C TInt CSpeechRecognitionUtility::GetAllModelBankIDs(RArray <TModelBankID>& aModelBankIDs)
	{
	ASSERT(iBody);
	return iBody->GetAllModelBankIDs(aModelBankIDs);
	}


EXPORT_C TInt CSpeechRecognitionUtility::GetAllGrammarIDs(RArray <TGrammarID>& aGrammarIDs)
	{
	ASSERT(iBody);
	return iBody->GetAllGrammarIDs(aGrammarIDs);
	}


EXPORT_C TInt CSpeechRecognitionUtility::GetRuleValidity(TGrammarID aGrammarID, TRuleID aRuleID, TBool& aValid)
	{
	ASSERT(iBody);
	return iBody->GetRuleValidity(aGrammarID, aRuleID, aValid);
	} 

EXPORT_C TInt CSpeechRecognitionUtility::CreateModelBank(TModelBankID& aModelBankID)
	{
	ASSERT(iBody);
	return iBody->CreateModelBank(aModelBankID);
	}
EXPORT_C TInt CSpeechRecognitionUtility::RemoveModelBank(TModelBankID aModelBankID)
	{
	ASSERT(iBody);
	return iBody->RemoveModelBank(aModelBankID);
	}

EXPORT_C TInt CSpeechRecognitionUtility::GetAvailableStorage(TInt& aAvailableStorage) 
	{
	ASSERT(iBody);
	return iBody->GetAvailableStorage(aAvailableStorage);
	}

EXPORT_C TInt CSpeechRecognitionUtility::LoadEngineParameters(const RArray<TInt>& aParameterId, const RArray<TInt>& aParameterValue)
	{
	ASSERT(iBody);
	return iBody->LoadEngineParameters(aParameterId, aParameterValue);
	}

EXPORT_C TInt CSpeechRecognitionUtility::SetAudioPriority(TInt aPriority, TInt aTrainPreference, TInt aPlaybackPreference, TInt aRecognitionPreference)
	{
	ASSERT(iBody);
	return iBody->SetAudioPriority(aPriority, aTrainPreference, aPlaybackPreference, aRecognitionPreference);
	}
	
enum TDllReason {};
EXPORT_C TInt E32Dll(TDllReason /* aReason */)	
	{
	return (KErrNone);
	}

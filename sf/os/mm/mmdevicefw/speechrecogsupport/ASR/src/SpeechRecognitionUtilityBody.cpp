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
// SpeechRecognitionUtility.cpp
// 
//

#include <e32base.h>
#include <mmf/common/mmfcontroller.h>
#include <mmf/common/speechrecognitioncustomcommands.h>
#include <mmf/common/speechrecognitiondatacommon.h>

#include <speechrecognitionutility.h>
#include "SpeechRecognitionUtilityBody.h"

//_LIT(KMMFSpeechRecognitionUtilityPanic,"Speech Recognition Utility Bad State");
enum ASRPanics 
	{
	KMMFSpeechRecognitionUtilityPanicBadState,
	KMMFSpeechRecognitionUtilityPanicUnrecognisedEvent
	};

CSpeechRecognitionUtility::CBody* CSpeechRecognitionUtility::CBody::NewL( TUid aClientUid, MSpeechRecognitionUtilityObserver& aSpeechRecognitionUtilityObserver)
	{
	CSpeechRecognitionUtility::CBody* self = CSpeechRecognitionUtility::CBody::NewLC(aClientUid, aSpeechRecognitionUtilityObserver);
	CleanupStack::Pop(self);
	return self;
	}
	
CSpeechRecognitionUtility::CBody* CSpeechRecognitionUtility::CBody::NewLC(TUid aClientUid, MSpeechRecognitionUtilityObserver& aSpeechRecognitionUtilityObserver)
	{
	CSpeechRecognitionUtility::CBody* self = new(ELeave) CSpeechRecognitionUtility::CBody(aClientUid, aSpeechRecognitionUtilityObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}
	
void CSpeechRecognitionUtility::CBody::ConstructL()
	{
	iAudioPriority = EMdaPriorityNormal;
	iTrainPreference = EMdaPriorityPreferenceNone;
	iRecognitionPreference = EMdaPriorityPreferenceNone;
	iPlaybackPreference = EMdaPriorityPreferenceNone;
	
	iPrioritySettings.iPriority = iAudioPriority; 
	iPrioritySettings.iPref = iTrainPreference;

	iControllerEventMonitor = CMMFControllerEventMonitor::NewL(*this, iMMFController);
	
	RMMFControllerImplInfoArray controllers;
	CleanupResetAndDestroyPushL(controllers);

	CMMFControllerPluginSelectionParameters* cSelect = CMMFControllerPluginSelectionParameters::NewLC();

	// Select the media IDs to allow
	RArray<TUid> mediaIds;
	CleanupClosePushL(mediaIds);
	User::LeaveIfError(mediaIds.Append(KUidMediaTypeASR));
	cSelect->SetMediaIdsL(mediaIds,CMMFPluginSelectionParameters::EAllowOnlySuppliedMediaIds);
	CleanupStack::PopAndDestroy(&mediaIds);

	cSelect->ListImplementationsL(controllers);	
	
	//[ Open and configure a controller]
	iControllerEventMonitor->Cancel();
	iMMFController.Close();
	//[check controller array is not empty]
    if( ! controllers.Count() )
		User::Leave( KErrNotFound );

	//[ simply open the first controller in the array ]
	User::LeaveIfError( iMMFController.Open(controllers[0]->Uid(), iPrioritySettings));
	iControllerEventMonitor->Start();
	
	iSrCustomCommands.SetClientUid(iClientUid);

	CleanupStack::PopAndDestroy(2, &controllers);//controllers, cSelect
	
	}

CSpeechRecognitionUtility::CBody::CBody(TUid aClientUid, MSpeechRecognitionUtilityObserver& aSpeechRecognitionUtilityObserver)
	: iSrCustomCommands(iMMFController),
	  iSpeechRecognitionUtilityObserver(&aSpeechRecognitionUtilityObserver),
	  iClientUid(aClientUid)
	{
	}
	
CSpeechRecognitionUtility::CBody::~CBody()
	{
	if (iControllerEventMonitor)
		{
		iControllerEventMonitor->Cancel();
		delete iControllerEventMonitor;
		}
	iMMFController.Close();
	}

TInt CSpeechRecognitionUtility::CBody::GetEngineProperties(const RArray<TInt>& aPropertyIDs, RArray<TInt>& aPropertyValues)
	{
	aPropertyValues.Reset();
	return iSrCustomCommands.GetEngineProperties(aPropertyIDs, aPropertyValues);
	}

TInt CSpeechRecognitionUtility::CBody::AddPronunciation(TLexiconID aLexiconID, TModelBankID aModelBankID, TModelID aModelID, TPronunciationID& aPronunciationID)
	{
	if (iAsyncCallBusy)
		return KErrInUse;

	TInt err = iSrCustomCommands.AddPronunciation(aLexiconID, aModelBankID, aModelID, aPronunciationID);
	
	if (err == KErrNone)
		iAsyncCallBusy=ETrue;
	return err;
	}

TInt CSpeechRecognitionUtility::CBody::AddRule(TGrammarID aGrammarID, TLexiconID aLexiconID, TPronunciationID aPronunciationID, TRuleID& aRuleID)
	{
	if (iAsyncCallBusy)
		return KErrInUse;

	TInt err = iSrCustomCommands.AddRule(aGrammarID, aLexiconID, aPronunciationID, aRuleID);

	if (err == KErrNone)
		iAsyncCallBusy=ETrue;
	return err;

	}

void CSpeechRecognitionUtility::CBody::Cancel()
	{
	iSrCustomCommands.Cancel();
	// Now cancel has been called, so remove any pointers storing references to arrays to be filled
	// These arrays are not owned by this object, so they must not be destroyed
	iPronunciationIDs = NULL;
	iRuleIDs = NULL;
	iModelIDs = NULL;
	iAsyncCallBusy = EFalse;
	}

TInt CSpeechRecognitionUtility::CBody::CommitChanges()
	{
	if (iAsyncCallBusy)
		return KErrInUse;

	return iSrCustomCommands.CommitChanges();
	}

TInt CSpeechRecognitionUtility::CBody::CreateGrammar(TGrammarID& aGrammarID)
	{
	if (iAsyncCallBusy)
		return KErrInUse;
	TInt err = iSrCustomCommands.CreateGrammar(aGrammarID);
	
	if (err == KErrNone)
		iAsyncCallBusy=ETrue;
	return err;
	}

TInt CSpeechRecognitionUtility::CBody::CreateLexicon(TLexiconID& aLexiconID)
	{
	if (iAsyncCallBusy)
		return KErrInUse;
	TInt err = iSrCustomCommands.CreateLexicon(aLexiconID);
	
	if (err == KErrNone)
		iAsyncCallBusy=ETrue;
	return err;
	}

TInt CSpeechRecognitionUtility::CBody::LoadModels(TModelBankID aModelBankID)
	{
	if (iAsyncCallBusy)
		return KErrInUse;

	TInt err = iSrCustomCommands.LoadModels(aModelBankID);
	
	if (err == KErrNone)
		iAsyncCallBusy=ETrue;
		
	return err;
	}
	
TInt CSpeechRecognitionUtility::CBody::LoadGrammar(TGrammarID aGrammarID)
	{
	if (iAsyncCallBusy)
		return KErrInUse;

	TInt err = iSrCustomCommands.LoadGrammar(aGrammarID);
	
	if (err == KErrNone)
		iAsyncCallBusy=ETrue;
		
	return err;

	}

TInt CSpeechRecognitionUtility::CBody::LoadLexicon(TLexiconID aLexiconID)
	{
	if (iAsyncCallBusy)
		return KErrInUse;

	TInt err = iSrCustomCommands.LoadLexicon(aLexiconID);
	
	if (err == KErrNone)
		iAsyncCallBusy=ETrue;
		
	return err;

	}


TInt CSpeechRecognitionUtility::CBody::GetUtteranceDuration(TModelBankID aModelBankID, TModelID aModelID, TTimeIntervalMicroSeconds32& aDuration)
	{
	if (iAsyncCallBusy)
		return KErrInUse;

	TInt err = iSrCustomCommands.GetUtteranceDuration(aModelBankID, aModelID, aDuration);
	
	if (err == KErrNone)
		iAsyncCallBusy=ETrue;
		
	return err;
	}


TInt CSpeechRecognitionUtility::CBody::PlayUtterance(TModelBankID aModelBankID, TModelID aModelID)
	{
	if (iAsyncCallBusy)
		return KErrInUse;
		
	iPrioritySettings.iPriority = iAudioPriority;
	iPrioritySettings.iPref = iPlaybackPreference;
	
	TInt err = iMMFController.SetPrioritySettings(iPrioritySettings);
	
	if (err == KErrNone)
		err = iSrCustomCommands.PlayUtterance(aModelBankID, aModelID);
	
	if (err == KErrNone)
		iAsyncCallBusy=ETrue;
		
	return err;
	}

TInt CSpeechRecognitionUtility::CBody::GetModelCount(TModelBankID aModelBankID, TInt& aModelCount)
	{
	if (iAsyncCallBusy)
		return KErrInUse;

	TInt err = iSrCustomCommands.GetModelCount(aModelBankID, aModelCount);
	
	if (err == KErrNone)
		iAsyncCallBusy=ETrue;
		
	return err;
	}

TInt CSpeechRecognitionUtility::CBody::StartRecSession(TRecognitionMode aMode)
	{
	if (iAsyncCallBusy)
		return KErrInUse;
	
	return iSrCustomCommands.StartRecSession(aMode);
	}
	
TInt CSpeechRecognitionUtility::CBody::EndRecSession()
	{
	if (iAsyncCallBusy)
		return KErrInUse;
		
	return iSrCustomCommands.EndRecSession();
	}

TInt CSpeechRecognitionUtility::CBody::Recognize(CSDClientResultSet& aResultSet)
	{
	if (iAsyncCallBusy)
		return KErrInUse;
		
	ASSERT(iResultSet==NULL); // this should be NULL unless we have problems
	
	iPrioritySettings.iPriority = iAudioPriority;
	iPrioritySettings.iPref = iRecognitionPreference;
	TInt err = iMMFController.SetPrioritySettings(iPrioritySettings);
	
	if (err == KErrNone)
		err =  iSrCustomCommands.Recognize(aResultSet);

	if (err == KErrNone)
		{
		iResultSet = &aResultSet;
		iAsyncCallBusy = ETrue;
		}
	return err;	
	}

TInt CSpeechRecognitionUtility::CBody::Record(TTimeIntervalMicroSeconds32 aRecordTime)
	{
	if (iAsyncCallBusy)
		return KErrInUse;

	TInt err = iSrCustomCommands.Record(aRecordTime);
	
	if (err == KErrNone)
		iAsyncCallBusy=ETrue;
		
	return err;
	}

TInt CSpeechRecognitionUtility::CBody::RemoveGrammar(TGrammarID aGrammarID)
	{
	if (iAsyncCallBusy)
		return KErrInUse;

	TInt err = iSrCustomCommands.RemoveGrammar(aGrammarID);
	
	if (err == KErrNone)
		iAsyncCallBusy = ETrue;
	return err;
	}

TInt CSpeechRecognitionUtility::CBody::RemovePronunciation(TLexiconID aLexiconID, TPronunciationID aPronunciationID)
	{
	if (iAsyncCallBusy)
		return KErrInUse;

	TInt err = iSrCustomCommands.RemovePronunciation(aLexiconID, aPronunciationID);
	
	if (err == KErrNone)
		iAsyncCallBusy = ETrue;
	return err;
	}

TInt CSpeechRecognitionUtility::CBody::RemoveLexicon(TLexiconID aLexiconID)
	{
	if (iAsyncCallBusy)
		return KErrInUse;

	TInt err = iSrCustomCommands.RemoveLexicon(aLexiconID);
	
	if (err == KErrNone)
		iAsyncCallBusy = ETrue;
	return err;
	}

TInt CSpeechRecognitionUtility::CBody::RemoveModel(TModelBankID aModelBankID, TModelID aModelID)
	{
	if (iAsyncCallBusy)
		return KErrInUse;

	TInt err = iSrCustomCommands.RemoveModel(aModelBankID, aModelID);
	
	if (err == KErrNone)
		iAsyncCallBusy = ETrue;
		
	return err;
	}

TInt CSpeechRecognitionUtility::CBody::RemoveRule(TGrammarID aGrammarID, TRuleID aRuleID)
	{
	if (iAsyncCallBusy)
		return KErrInUse;

	TInt err = iSrCustomCommands.RemoveRule(aGrammarID, aRuleID);
	
	if (err == KErrNone)
		iAsyncCallBusy = ETrue;
		
	return err;

	}
TInt CSpeechRecognitionUtility::CBody::Train(TModelBankID aModelBankID, TModelID& aModelID)
	{
	if (iAsyncCallBusy)
		return KErrInUse;
		
	iPrioritySettings.iPriority = iAudioPriority;
	iPrioritySettings.iPref = iTrainPreference;
	TInt err = iMMFController.SetPrioritySettings(iPrioritySettings);

	if (err == KErrNone)
		err = iSrCustomCommands.Train(aModelBankID, aModelID);
	
	if (err == KErrNone)
		iAsyncCallBusy = ETrue;

	return err;
	}

TInt CSpeechRecognitionUtility::CBody::UnloadRule(TGrammarID aGrammarID, TRuleID aRuleID)
	{
	if (iAsyncCallBusy)
		return KErrInUse;

	TInt err = iSrCustomCommands.UnloadRule(aGrammarID, aRuleID); 
	
	if (err == KErrNone)
		iAsyncCallBusy = ETrue;
		
	return err;

	}

void CSpeechRecognitionUtility::CBody::SetEventHandler(MSpeechRecognitionUtilityObserver* aSpeechRecognitionUtilityObserver)
	{
	iSpeechRecognitionUtilityObserver = aSpeechRecognitionUtilityObserver;
	}
	

TInt CSpeechRecognitionUtility::CBody::GetAllPronunciationIDs(TLexiconID aLexiconID, RArray <TPronunciationID>& aPronunciationIDs)
	{
	if (iAsyncCallBusy)
		return KErrInUse;

	ASSERT(iPronunciationIDs==NULL);

	TInt err = iSrCustomCommands.GetAllPronunciationIDs(aLexiconID);
	
	if (err==KErrNone)
		{
		iPronunciationIDs = &aPronunciationIDs;
		iPronunciationIDs->Reset();
		iAsyncCallBusy = ETrue;
		}
		
	return err;
	}

TInt CSpeechRecognitionUtility::CBody::GetAllModelIDs(TModelBankID aModelBankID, RArray <TModelID>& aModelIDs)
	{
	if (iAsyncCallBusy)
		return KErrInUse;

	ASSERT(iModelIDs==NULL);
	
	TInt err = iSrCustomCommands.GetAllModelIDs(aModelBankID);
	
	if (err==KErrNone) 
		{
		iModelIDs = &aModelIDs;
		iModelIDs->Reset();
		iAsyncCallBusy = ETrue;
		}
	return err;
	}


TInt CSpeechRecognitionUtility::CBody::GetAllRuleIDs(TGrammarID aGrammarID, RArray <TRuleID>& aRuleIDs)
	{
	if (iAsyncCallBusy)
		return KErrInUse;

	ASSERT(iRuleIDs==NULL);
		
	TInt err = iSrCustomCommands.GetAllRuleIDs(aGrammarID);
	
	if (err==KErrNone)	
		{
		iRuleIDs = &aRuleIDs;
		iRuleIDs->Reset();
		iAsyncCallBusy = ETrue;
		}
	return err;
	}
	
	
TInt CSpeechRecognitionUtility::CBody::GetAllClientLexiconIDs(RArray <TLexiconID>& aLexiconIDs)
	{
	if (iAsyncCallBusy)
		return KErrInUse;

	ASSERT(iLexiconIDs==NULL);

	TInt err = iSrCustomCommands.GetAllClientLexiconIDs();
	
	if (err==KErrNone)
		{
		iLexiconIDs = &aLexiconIDs;
		iLexiconIDs->Reset();
		iAsyncCallBusy = ETrue;
		}
		
	return err;
	}

TInt CSpeechRecognitionUtility::CBody::GetAllClientModelBankIDs(RArray <TModelBankID>& aModelBankIDs)
	{
	if (iAsyncCallBusy)
		return KErrInUse;

	ASSERT(iModelBankIDs==NULL);
	
	TInt err = iSrCustomCommands.GetAllClientModelBankIDs();
	
	if (err==KErrNone) 
		{
		iModelBankIDs = &aModelBankIDs;
		iModelBankIDs->Reset();
		iAsyncCallBusy = ETrue;
		}
	return err;
	}


TInt CSpeechRecognitionUtility::CBody::GetAllClientGrammarIDs(RArray <TGrammarID>& aGrammarIDs)
	{
	if (iAsyncCallBusy)
		return KErrInUse;

	ASSERT(iGrammarIDs==NULL);
		
	TInt err = iSrCustomCommands.GetAllClientGrammarIDs();
	
	if (err==KErrNone)	
		{
		iGrammarIDs = &aGrammarIDs;
		iGrammarIDs->Reset();
		iAsyncCallBusy = ETrue;
		}
	return err;
	}
	
	
TInt CSpeechRecognitionUtility::CBody::GetAllLexiconIDs(RArray <TLexiconID>& aLexiconIDs)
	{
	if (iAsyncCallBusy)
		return KErrInUse;

	ASSERT(iLexiconIDs==NULL);

	TInt err = iSrCustomCommands.GetAllLexiconIDs();
	
	if (err==KErrNone)
		{
		iLexiconIDs = &aLexiconIDs;
		iLexiconIDs->Reset();
		iAsyncCallBusy = ETrue;
		}
		
	return err;
	}

TInt CSpeechRecognitionUtility::CBody::GetAllModelBankIDs(RArray <TModelBankID>& aModelBankIDs)
	{
	if (iAsyncCallBusy)
		return KErrInUse;

	ASSERT(iModelBankIDs==NULL);
	
	TInt err = iSrCustomCommands.GetAllModelBankIDs();
	
	if (err==KErrNone) 
		{
		iModelBankIDs = &aModelBankIDs;
		iModelBankIDs->Reset();
		iAsyncCallBusy = ETrue;
		}
	return err;
	}


TInt CSpeechRecognitionUtility::CBody::GetAllGrammarIDs(RArray <TGrammarID>& aGrammarIDs)
	{
	if (iAsyncCallBusy)
		return KErrInUse;

	ASSERT(iGrammarIDs==NULL);
		
	TInt err = iSrCustomCommands.GetAllGrammarIDs();
	
	if (err==KErrNone)	
		{
		iGrammarIDs = &aGrammarIDs;
		iGrammarIDs->Reset();
		iAsyncCallBusy = ETrue;
		}
	return err;
	}

	
TInt CSpeechRecognitionUtility::CBody::GetRuleValidity(TGrammarID aGrammarID, TRuleID aRuleID, TBool& aValid)
	{
	if (iAsyncCallBusy)
		return KErrInUse;

	TInt err = iSrCustomCommands.GetRuleValidity(aGrammarID, aRuleID, aValid);
	
	if (err == KErrNone)
		iAsyncCallBusy = ETrue;
	return err;
	} 


TInt CSpeechRecognitionUtility::CBody::CreateModelBank(TModelBankID& aModelBankID)
	{
	if (iAsyncCallBusy)
		return KErrInUse;
	TInt err = iSrCustomCommands.CreateModelBank(aModelBankID);
	if (err == KErrNone)
		iAsyncCallBusy = ETrue;
	return err;
	}

TInt CSpeechRecognitionUtility::CBody::RemoveModelBank(TModelBankID aModelBankID)
	{
	if (iAsyncCallBusy)
		return KErrInUse;

	TInt err = iSrCustomCommands.RemoveModelBank(aModelBankID);
	if (err == KErrNone)
		iAsyncCallBusy = ETrue;
	return err;
	}

TInt CSpeechRecognitionUtility::CBody::GetAvailableStorage(TInt& aAvailableStorage) 
	{
	if (iAsyncCallBusy)
		return KErrInUse;
		
	TInt err = iSrCustomCommands.GetAvailableStorage(aAvailableStorage);
	
	if (err == KErrNone)
		iAsyncCallBusy = ETrue;
		
	return err;
	}

TInt CSpeechRecognitionUtility::CBody::LoadEngineParameters(const RArray<TInt>& aParameterId, const RArray<TInt>& aParameterValue)
	{
	if (iAsyncCallBusy)
		return KErrInUse;
		
	return iSrCustomCommands.LoadEngineParameters(aParameterId, aParameterValue);
	}

TInt CSpeechRecognitionUtility::CBody::SetAudioPriority(TInt aPriority, TInt aTrainPreference, TInt aPlaybackPreference, TInt aRecognitionPreference)
	{
	iAudioPriority = aPriority;
	iTrainPreference = (TMdaPriorityPreference)aTrainPreference;
	iPlaybackPreference = (TMdaPriorityPreference)aPlaybackPreference;
	iRecognitionPreference = (TMdaPriorityPreference)aRecognitionPreference;
	
	return KErrNone;	
	}

void CSpeechRecognitionUtility::CBody::HandleEvent(const TMMFEvent& aEvent)
	{
	TBool cancelled = EFalse;
	TBool asyncCallComplete = ETrue;

	TUid event = aEvent.iEventType;
	TInt errorCode = aEvent.iErrorCode;
	
		
	switch (event.iUid)
		{
		case KUidAsrEventGetAllPronunciationIDsVal :
			{
			if (iPronunciationIDs)
				{
				if (errorCode==KErrNone)
					TRAP(errorCode, iSrCustomCommands.GetPronunciationIDArrayL(*iPronunciationIDs));
				// async operation complete, so get rid of reference to array
				iPronunciationIDs = NULL;
				}
			else 
				cancelled = ETrue;
			} 
			break;
		case KUidAsrEventGetAllRuleIDsVal :
			{
			if (iRuleIDs)
				{
				if (errorCode==KErrNone)
					TRAP(errorCode, iSrCustomCommands.GetRuleIDArrayL(*iRuleIDs));		
				// async operation complete, so get rid of reference to array
				iRuleIDs = NULL;
				}
			else
				cancelled = ETrue;
			}
			break;
		case KUidAsrEventGetAllModelIDsVal :
			{
			if (iModelIDs)
				{
				if (errorCode==KErrNone)
					TRAP(errorCode, iSrCustomCommands.GetModelIDArrayL(*iModelIDs));
				// async operation complete, so get rid of reference to array
				iModelIDs = NULL;
				}
			else
				cancelled = ETrue;
			}
			break;
		case KUidAsrEventGetAllClientModelBankIDsVal :
		case KUidAsrEventGetAllModelBankIDsVal :
			{
			if (iModelBankIDs)
				{
				if (errorCode==KErrNone)
					TRAP(errorCode, iSrCustomCommands.GetModelBankIDArrayL(*iModelBankIDs));
				// async operation complete, so get rid of reference to array
				iModelBankIDs = NULL;
				}
			else
				cancelled = ETrue;
			}
			break;
		case KUidAsrEventGetAllClientGrammarIDsVal :
		case KUidAsrEventGetAllGrammarIDsVal :
			{
			if (iGrammarIDs)
				{
				if (errorCode==KErrNone)
					TRAP(errorCode, iSrCustomCommands.GetGrammarIDArrayL(*iGrammarIDs));
				// async operation complete, so get rid of reference to array
				iGrammarIDs = NULL;
				}
			else
				cancelled = ETrue;
			}
			break;
		case KUidAsrEventGetAllClientLexiconIDsVal :
		case KUidAsrEventGetAllLexiconIDsVal :
			{
			if (iLexiconIDs)
				{
				if (errorCode==KErrNone)
					TRAP(errorCode, iSrCustomCommands.GetLexiconIDArrayL(*iLexiconIDs));
				// async operation complete, so get rid of reference to array
				iLexiconIDs = NULL;
				}
			else
				cancelled = ETrue;
			}
			break;

		case KUidAsrEventRecognitionVal :
			{
			if (iResultSet)
				{
				if (errorCode==KErrNone)
					TRAP(errorCode, iSrCustomCommands.GetResultSetL(*iResultSet));
				iResultSet = NULL;
				}
			else
				cancelled = ETrue;
			}
			break;
		case KUidAsrEventTrainVal:
			break;
		case KUidAsrEventPlayStartedVal:
		case KUidAsrEventRecordStartedVal:
			asyncCallComplete = EFalse;
			break;
		default:
			break;

			}
		if (event == KMMFErrorCategoryControllerGeneralError)
			{
			// clear these, as the error callback
			iPronunciationIDs = NULL;
			iRuleIDs = NULL;
			iModelIDs = NULL;
			iLexiconIDs = NULL;
			iGrammarIDs = NULL;
			iModelBankIDs = NULL;
			}
		if (asyncCallComplete)
			iAsyncCallBusy = EFalse;
			
		if (iSpeechRecognitionUtilityObserver && !cancelled)
			iSpeechRecognitionUtilityObserver->MsruoEvent(event, errorCode);

	}


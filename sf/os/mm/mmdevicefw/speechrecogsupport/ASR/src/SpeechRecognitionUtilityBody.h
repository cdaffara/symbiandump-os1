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

#ifndef __SPEECHRECOGNITIONUTILITYBODY_H__
#define __SPEECHRECOGNITIONUTILITYBODY_H__

// INCLUDES

#include <e32base.h>
#include <mmf/common/mmfcontroller.h>
#include <mmf/common/speechrecognitioncustomcommands.h>
#include <mmf/common/speechrecognitiondataclient.h>
#include <speechrecognitionutility.h>


class CSpeechRecognitionUtility::CBody : public CBase, public MMMFControllerEventMonitorObserver
{
public:
	static CSpeechRecognitionUtility::CBody* NewL(TUid aClientUid, MSpeechRecognitionUtilityObserver& aSpeechRecognitionUtilityObserver);
	static CSpeechRecognitionUtility::CBody* NewLC(TUid aClientUid, MSpeechRecognitionUtilityObserver& aSpeechRecognitionUtilityObserver);
	
	 ~CBody();
	 TInt GetEngineProperties( const RArray<TInt>& aPropertyId, RArray<TInt>& aPropertyValue );
	 TInt AddPronunciation(TLexiconID aLexiconID, TModelBankID aModelBankID, TModelID aModelID, TPronunciationID& aPronunciationID);
	 TInt AddRule(TGrammarID aGrammarID, TLexiconID aLexiconID, TPronunciationID aPronunciationID, TRuleID& aRuleID);
	 void Cancel();
	 TInt CommitChanges();
	 TInt CreateGrammar(TGrammarID& aGrammarID);
	 TInt CreateLexicon(TLexiconID& aLexiconID);
	 TInt LoadModels(TModelBankID aModelBankID);
	 TInt LoadGrammar(TGrammarID aGrammarID);

	 TInt LoadLexicon(TLexiconID aLexiconID);

	 TInt GetUtteranceDuration(TModelBankID aModelBankID, TModelID aModelID, TTimeIntervalMicroSeconds32& aDuration);
	 TInt PlayUtterance(TModelBankID aModelBankID, TModelID aModelID);
	 TInt GetModelCount(TModelBankID aModelBankID, TInt& aModelCount);
	 TInt EndRecSession();

	 TInt StartRecSession(TRecognitionMode aMode);
	 TInt Recognize(CSDClientResultSet& aResultSet);

	 TInt Record(TTimeIntervalMicroSeconds32 aRecordTime);

	 TInt RemoveGrammar(TGrammarID aGrammarID);

	 TInt RemovePronunciation(TLexiconID aLexiconID, TPronunciationID aPronunciationID);

	 TInt RemoveLexicon(TLexiconID aLexiconID);

	 TInt RemoveModel(TModelBankID aModelBankID, TModelID aModelID);

	 TInt RemoveRule(TGrammarID aGrammarID, TRuleID aRuleID);
	 TInt Train(TModelBankID aModelBankID, TModelID& aModelID);
	 TInt UnloadRule(TGrammarID aGrammarID, TRuleID aRuleID);
	 void SetEventHandler(MSpeechRecognitionUtilityObserver* aSpeechRecognitionUtilityObserver);
	 TInt GetAllPronunciationIDs(TLexiconID aLexiconID, RArray <TPronunciationID>& aPronunciationIDs);
	 TInt GetAllModelIDs(TModelBankID aModelBankID, RArray <TModelID>& aModelIDs);
	 TInt GetAllRuleIDs(TGrammarID aGrammarID, RArray <TRuleID>& aRuleIDs);
	 
  	 TInt GetAllLexiconIDs(RArray <TLexiconID>& aPronunciationIDs);
	 TInt GetAllModelBankIDs(RArray <TModelBankID>& aModelIDs);
	 TInt GetAllGrammarIDs(RArray <TGrammarID>& aRuleIDs);

 	 TInt GetAllClientLexiconIDs(RArray <TLexiconID>& aPronunciationIDs);
	 TInt GetAllClientModelBankIDs(RArray <TModelBankID>& aModelIDs);
	 TInt GetAllClientGrammarIDs(RArray <TGrammarID>& aRuleIDs);

 	 TInt GetRuleValidity(TGrammarID aGrammarID, TRuleID aRuleID, TBool& aValid); 

	 TInt CreateModelBank(TModelBankID& aModelBankID);
	 TInt RemoveModelBank(TModelBankID aModelBankID);
	 TInt GetAvailableStorage(TInt& aAvailableStorage); 
	 TInt LoadEngineParameters( const RArray<TInt>& aParameterId, 
								const RArray<TInt>& aParameterValue );

	 TInt SetAudioPriority( TInt aPriority, TInt aTrainPreference, 
					TInt aPlaybackPreference, TInt aRecognitionPreference );

	void HandleEvent(const TMMFEvent& aEvent);

private:
	CBody(TUid aClientUid, MSpeechRecognitionUtilityObserver& aSpeechRecognitionUtilityObserver);
	
	void ConstructL();

	// member variable MMF controller
	RMMFController iMMFController;
	// member variable for custom commands
	RSpeechRecognitionCustomCommands iSrCustomCommands;
	// member variable pointer for observer class
	MSpeechRecognitionUtilityObserver* iSpeechRecognitionUtilityObserver;

	// member variable pointer for controller event monitor class
	CMMFControllerEventMonitor* iControllerEventMonitor;
	// used to indicate if an event is the result of an internal command
	TBool iAsyncCallBusy;

	// member variable pointer for array of rule IDs - not owned
	RArray <TRuleID>* iRuleIDs;
	// member variable pointer for array of pronunciation IDs  - not owned
	RArray <TPronunciationID>* iPronunciationIDs;
	// member variable pointer for array of model IDs  - not owned
	RArray <TModelID>* iModelIDs;
	
	// member variable pointer for array of Grammar IDs  - not owned
	RArray <TGrammarID>* iGrammarIDs;
	// member variable pointer for array of Lexicon IDs  - not owned
	RArray <TLexiconID>* iLexiconIDs;
	// member variable pointer for array of Model Bank IDs  - not owned
	RArray <TModelBankID>* iModelBankIDs;


	// member variable for audio priority
	TInt iAudioPriority;
	// member variable for train preference
	TMdaPriorityPreference iTrainPreference; 
	// member variable for playback preference
	TMdaPriorityPreference iPlaybackPreference;
	// member variable for recognition preference
	TMdaPriorityPreference iRecognitionPreference;
	
	CSDClientResultSet* iResultSet;

	// settings used in the construction of the asr controlller 
	TMMFPrioritySettings   iPrioritySettings;
	
	TUid iClientUid;

};

#endif	// __SPEECHRECOGNITIONUTILITY_H__

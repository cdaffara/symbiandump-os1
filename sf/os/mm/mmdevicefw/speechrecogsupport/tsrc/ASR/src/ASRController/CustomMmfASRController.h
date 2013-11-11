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

#ifndef __TEST_MMF_CUSTOM_MMF_ASR_CONTROLLER_H__
#define __TEST_MMF_CUSTOM_MMF_ASR_CONTROLLER_H__

//include all the header files for now
//
#include <mmf/common/speechrecognitioncustomcommands.h>
#include <mmf/common/speechrecognitiondataclient.h>
#include <mmf/common/speechrecognitioncustomcommandimplementor.h>
#include <mmf/common/speechrecognitioncustomcommands.h>
#include <mmf/common/speechrecognitioncustomcommandparser.h>

#include <speechrecognitionutility.h>

#include "Database/sddatabase.h"


// custom asr controller plugin class
class CCustomMmfASRController:	public CMMFController, 
								public MAsyncEventHandler,			
								public MSpeechRecognitionCustomCommandImplementor

	{
public:
	static CMMFController* NewL() ;
          ~CCustomMmfASRController();

	//implement CMMFController ecom plugin interface
	virtual void AddDataSourceL(MDataSource& aDataSource);
	virtual void AddDataSinkL(MDataSink& aDataSink);
	virtual void RemoveDataSourceL(MDataSource& aDataSource);
	virtual void RemoveDataSinkL(MDataSink& aDataSink);
	virtual void ResetL();
	virtual void PrimeL();
	virtual void PlayL();
	virtual void PauseL();
	virtual void StopL();
	virtual TTimeIntervalMicroSeconds PositionL() const;
	virtual void SetPositionL(const TTimeIntervalMicroSeconds& aPosition);
	virtual TTimeIntervalMicroSeconds DurationL() const;
	virtual void CustomCommand(TMMFMessage& aMessage);
	virtual void SetPrioritySettings(const TMMFPrioritySettings& aPrioritySettings);
	virtual void GetNumberOfMetaDataEntriesL(TInt& aNumberOfEntries) ;
	virtual CMMFMetaDataEntry* GetMetaDataEntryL(TInt aIndex) ;

	//from MAsyncEventHandler
	virtual TInt SendEventToClient(const TMMFEvent& aEvent);

	//
	virtual void MSrAddPronunciationL( TLexiconID aLexiconID, TModelBankID aModelBankID,
										  TModelID aModelID, TPronunciationID& aPronunciationID ) ;
	virtual void MSrAddRuleL( TGrammarID aGrammarID, TLexiconID aLexiconID,
								 TPronunciationID aPronunciationID, TRuleID& aRuleID );
	virtual void MSrCancel() ;
	virtual void MSrCommitChangesL() ;
	virtual void MSrCreateGrammarL( TGrammarID& aGrammarID ) ;
	virtual void MSrCreateLexiconL( TLexiconID& aLexiconID ) ;
	virtual void MSrCreateModelBankL( TModelBankID& aModelBankID ) ;
	virtual void MSrEndRecSessionL() ;
	virtual void MSrGetAllModelIDsL( TModelBankID aModelBankID, RArray<TModelID>& aModelIDs ) ;
	virtual void MSrGetAllPronunciationIDsL( TLexiconID aLexiconID, RArray<TPronunciationID>& aPronunciationIDs ) ;
	virtual void MSrGetAllRuleIDsL( TGrammarID aGrammarID, RArray<TRuleID>& aRuleIDs ) ;
	
	virtual void MSrGetAllModelBankIDsL( RArray<TModelBankID>& aModelBankIDs ) ;
	virtual void MSrGetAllLexiconIDsL( RArray<TLexiconID>& aLexiconIDs ) ;
	virtual void MSrGetAllGrammarIDsL( RArray<TGrammarID>& aGrammarIDs ) ;

	virtual void MSrGetAllClientModelBankIDsL( RArray<TModelBankID>& aModelBankIDs ) ;
	virtual void MSrGetAllClientLexiconIDsL( RArray<TLexiconID>& aLexiconIDs ) ;
	virtual void MSrGetAllClientGrammarIDsL( RArray<TGrammarID>& aGrammarIDs ) ;

	virtual void MSrGetAvailableStorageL( TInt& aCount ) ;
	virtual void MSrGetEnginePropertiesL(  const RArray<TInt>& aPropertyId, 
						RArray<TInt>& aPropertyValue) ;
	virtual void MSrLoadEngineParametersL( const RArray<TInt>& aParameterId, 
						const RArray<TInt>& aParameterValue );
	virtual void MSrGetModelCountL( TModelBankID aModelBankID, TInt& aCount ) ;
	virtual void MSrGetRuleValidityL( TGrammarID aGrammarID, TRuleID aRuleID, TBool& aValid ) ;
	virtual void MSrGetUtteranceDurationL( TModelBankID aModelBankID, TModelID aModelID,
											   TTimeIntervalMicroSeconds32& aDuration) ;
	virtual void MSrLoadGrammarL( TGrammarID aGrammarID ) ;
	virtual void MSrLoadLexiconL( TLexiconID aLexiconID ) ;
	virtual void MSrLoadModelsL( TModelBankID aModelBankID ) ;
	virtual void MSrPlayUtteranceL( TModelBankID aModelBankID, TModelID aModelID) ;
	virtual void MSrRecognizeL( CSDClientResultSet& aResultSet ) ;
	virtual void MSrRecordL( TTimeIntervalMicroSeconds32 aRecordTime ) ;
	virtual void MSrRemoveGrammarL( TGrammarID aGrammarID ) ;
	virtual void MSrRemoveLexiconL( TLexiconID aLexiconID ) ;
	virtual void MSrRemoveModelBankL( TModelBankID aModelBankID );
	virtual void MSrRemoveModelL( TModelBankID aModelBankID, TModelID aModelID );
	virtual void MSrRemovePronunciationL( TLexiconID aLexiconID, TPronunciationID aPronunciationID );
	virtual void MSrRemoveRuleL( TGrammarID aGrammarID, TRuleID aRuleID );
	virtual void MSrStartRecSessionL( TRecognitionMode aMode );
	virtual void MSrTrainL( TModelBankID aModelBankID, TModelID& aModelID );
	virtual void MSrUnloadRuleL( TGrammarID aGrammarID, TRuleID aRuleID );
	virtual void MSrSetClientUid(TUid aClientUid);
	
public:
	
	enum TControllerState
	{
	ESTraining,
	ESRecognition
	};

private:
	TBool IsMemoryAllocCmd(TMMFMessage& aMessage );
	TInt  AllocMemoryTestL();

	//[ state access functions ]
	TControllerState State() const;
	void SetState( TControllerState aState );
	
	//[ Invariant function ]
	TBool InVariant() const;
protected:
     CCustomMmfASRController();
	 void ConstructL();
	 
private:
	TControllerState iState;    // main state of the controller
	TRecognitionMode iRecognitionMode; // recognition mode the controller is in
	TTimeIntervalMicroSeconds32	iRecordTime; // the duration of the recording for both recognition and trainning

	TGrammarID iLoadedGrammarID;
	TLexiconID iLoadedLexiconID;
	TModelBankID iLoadedModelBankID;
	
	CSDClientResultSet* iResultSet; // data structure containing the result set
	TInt iMaxResults;				// maximum number of results

	TBool iRecognizeComplete;  //a flag to rememeber that we are in recognize mode
	CSDDatabase* iDatabase;
	};

#endif

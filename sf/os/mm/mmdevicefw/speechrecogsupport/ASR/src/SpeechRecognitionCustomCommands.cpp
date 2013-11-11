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

//  INCLUDES
#include <mmf/common/speechrecognitioncustomcommands.h>
#include "SpeechRecognitionCustomCommandCommon.h"

const TInt KExpandSize = 10;


EXPORT_C RSpeechRecognitionCustomCommands::RSpeechRecognitionCustomCommands(RMMFController& aController)
	:RMMFCustomCommandsBase(aController, KUidInterfaceSpeechRecognition)
	{
	}

EXPORT_C TInt RSpeechRecognitionCustomCommands::AddPronunciation(TLexiconID aLexiconID, 
			TModelBankID aModelBankID, TModelID aModelID,
			TPronunciationID& aPronunciationID)
	{
	TSrsAddPronunciationPckg pckg(TSrsAddPronunciation(aLexiconID,aModelBankID,aModelID,&aPronunciationID));
	TInt err =  iController.CustomCommandSync(iDestinationPckg, 
												ESrAddPronunciation, 
												pckg,
												KNullDesC8);
												
	return err;
	}
	
EXPORT_C TInt RSpeechRecognitionCustomCommands::SetClientUid(TUid aClientUid)
	{
	TSrsUidPckg pckg(aClientUid);
	
	TInt err =  iController.CustomCommandSync(iDestinationPckg, 
												ESrSetClientUid, 
												pckg,
												KNullDesC8);
	return err;
	}

EXPORT_C TInt RSpeechRecognitionCustomCommands::AddRule(TGrammarID aGrammarID, TLexiconID aLexiconID,
				TPronunciationID aPronunciationID, TRuleID& aRuleID)
				
	{
	TSrsAddRulePckg pckg(TSrsAddRule(aGrammarID, aLexiconID, aPronunciationID, &aRuleID));
	TInt err =  iController.CustomCommandSync(iDestinationPckg, 
												ESrAddRule, 
												pckg,
												KNullDesC8);
												
	return err;
	}

EXPORT_C void RSpeechRecognitionCustomCommands::Cancel()
	{
	iController.CustomCommandSync(iDestinationPckg, 
									ESrCancel, 
									KNullDesC8,
									KNullDesC8);
												
	}

EXPORT_C TInt RSpeechRecognitionCustomCommands::CommitChanges()
	{
	TInt err =  iController.CustomCommandSync(iDestinationPckg, 
												ESrCommitChanges, 
												KNullDesC8,
												KNullDesC8);
												
	return err;

	}

EXPORT_C TInt RSpeechRecognitionCustomCommands::CreateGrammar(TGrammarID& aGrammarID)
	{
	TSrsGrammarIDPtrPckg pckg(&aGrammarID);
	TInt err =  iController.CustomCommandSync(iDestinationPckg, 
												ESrCreateGrammar, 
												pckg,
												KNullDesC8);
												
	return err;
	}

EXPORT_C TInt RSpeechRecognitionCustomCommands::CreateLexicon(TLexiconID& aLexiconID)
	{
	TSrsLexiconIDPtrPckg pckg(&aLexiconID);
	
	TInt err =  iController.CustomCommandSync(iDestinationPckg, 
												ESrCreateLexicon, 
												pckg,
												KNullDesC8);
												
	return err;


	}

EXPORT_C TInt RSpeechRecognitionCustomCommands::CreateModelBank(TModelBankID& aModelBankID)
	{
	TSrsModelBankIDPtrPckg pckg(&aModelBankID);
	TInt err =  iController.CustomCommandSync(iDestinationPckg, 
												ESrCreateModelBank, 
												pckg,
												KNullDesC8);
												
	return err;


	}

EXPORT_C TInt RSpeechRecognitionCustomCommands::EndRecSession()

	{
	TInt err =  iController.CustomCommandSync(iDestinationPckg, 
												ESrEndRecSession, 
												KNullDesC8,
												KNullDesC8);
												
	return err;


	}
EXPORT_C TInt RSpeechRecognitionCustomCommands::GetAllModelIDs(TModelBankID aModelBankID)
	{
	TSrsModelBankIDPckg pckg(aModelBankID);
	TInt err =  iController.CustomCommandSync(iDestinationPckg, 
												ESrGetAllModelIDs, 
												pckg,
												KNullDesC8);
												
	return err;
	}

EXPORT_C TInt RSpeechRecognitionCustomCommands::GetAllPronunciationIDs(TLexiconID aLexiconID)
	{
	TSrsLexiconIDPckg pckg(aLexiconID);
	TInt err = iController.CustomCommandSync(iDestinationPckg, 
												ESrGetAllPronunciationIDs, 
												pckg,
												KNullDesC8);
	return err;
												
	}

EXPORT_C TInt RSpeechRecognitionCustomCommands::GetAllRuleIDs(TGrammarID aGrammarID)
	{
	TSrsGrammarIDPckg pckg(aGrammarID);
	TInt err =  iController.CustomCommandSync(iDestinationPckg, 
												ESrGetAllRuleIDs, 
												pckg,
												KNullDesC8);
												
	return err;
	}

EXPORT_C TInt RSpeechRecognitionCustomCommands::GetAvailableStorage(TInt& aAvailableStorage)

	{
	TSrsIntPtrPckg pckg(&aAvailableStorage);
	TInt err =  iController.CustomCommandSync(iDestinationPckg, 
												ESrGetAvailableStorage, 
												pckg,
												KNullDesC8);
		
	return err;	

	}
EXPORT_C TInt RSpeechRecognitionCustomCommands::GetEngineProperties(const RArray<TInt>& aPropertyId, 
				RArray<TInt>& aPropertyValue)
	{
	
	TRAPD(err, DoGetEnginePropertiesL(aPropertyId, aPropertyValue));
	return err;	
	}

EXPORT_C TInt RSpeechRecognitionCustomCommands::GetModelCount(TModelBankID aModelBankID, 
				TInt& aModelCount)
	{
	TSrsModelBankIDPckg pckg(aModelBankID);
	TSrsIntPtrPckg resultpckg(&aModelCount);
	TInt err =  iController.CustomCommandSync(iDestinationPckg, 
												ESrGetModelCount, 
												pckg,
												resultpckg);
												
	return err;
	}

EXPORT_C TInt RSpeechRecognitionCustomCommands::GetRuleValidity(TGrammarID aGrammarID, TRuleID aRuleID, 
				TBool& aValid)
	{
	TSrsRuleValidityPckg pckg(TSrsRuleValidity(aGrammarID, aRuleID, &aValid));
	
	TInt err =  iController.CustomCommandSync(iDestinationPckg, 
												ESrGetRuleValidity, 
												pckg,
												KNullDesC8);
	return err;	

	}
EXPORT_C TInt RSpeechRecognitionCustomCommands::GetUtteranceDuration(TModelBankID aModelBankID, 
			TModelID aModelID, TTimeIntervalMicroSeconds32& aDuration)
	{
	TSrsGetUtteranceDurationPckg pckg(TSrsGetUtteranceDuration(aModelBankID, aModelID, &aDuration));
	
	TInt err =  iController.CustomCommandSync(iDestinationPckg, 
												ESrUtteranceDuration, 
												pckg,
												KNullDesC8);
	return err;	

	}

EXPORT_C TInt RSpeechRecognitionCustomCommands::LoadGrammar(TGrammarID aGrammarID)
	{
	TSrsGrammarIDPckg pckg(aGrammarID);
	TInt err =  iController.CustomCommandSync(iDestinationPckg, 
												ESrLoadGrammar, 
												pckg,
												KNullDesC8);
												
	return err;
	}

EXPORT_C TInt RSpeechRecognitionCustomCommands::LoadLexicon(TLexiconID aLexiconID)
	{
	TSrsLexiconIDPckg pckg(aLexiconID);
	TInt err =  iController.CustomCommandSync(iDestinationPckg, 
												ESrLoadLexicon, 
												pckg,
												KNullDesC8);
												
	return err;
	}

EXPORT_C TInt RSpeechRecognitionCustomCommands::LoadModels(TModelBankID aModelBankID)
	{
	TSrsModelBankIDPckg pckg(aModelBankID);
	TInt err =  iController.CustomCommandSync(iDestinationPckg, 
												ESrLoadModels, 
												pckg,
												KNullDesC8);
												
	return err;
	}

EXPORT_C TInt RSpeechRecognitionCustomCommands::PlayUtterance(TModelBankID aModelBankID, 
				TModelID aModelID)
	{
	TSrsPlayUtterancePckg pckg(TSrsPlayUtterance(aModelBankID, aModelID));
	
	TInt err =  iController.CustomCommandSync(iDestinationPckg, 
												ESrPlayUtterance, 
												pckg,
												KNullDesC8);
	return err;	

	}

EXPORT_C TInt RSpeechRecognitionCustomCommands::Recognize(CSDClientResultSet& aResultSet)
	{
	TRAPD(err, DoRecognizeL(aResultSet));
	return err;
	}


EXPORT_C TInt RSpeechRecognitionCustomCommands::Record(TTimeIntervalMicroSeconds32 aRecordTime)
	{
	TSrsTimeIntervalMicroSeconds32Pckg pckg(aRecordTime);
	
	TInt err =  iController.CustomCommandSync(iDestinationPckg,
												ESrRecord,
												pckg,
												KNullDesC8);
	return err;	
	}

EXPORT_C TInt RSpeechRecognitionCustomCommands::RemoveGrammar(TGrammarID aGrammarID)
	{
	TSrsGrammarIDPckg pckg(aGrammarID);
	TInt err =  iController.CustomCommandSync(iDestinationPckg, 
												ESrRemoveGrammar, 
												pckg,
												KNullDesC8);
												
	return err;
	}

EXPORT_C TInt RSpeechRecognitionCustomCommands::RemoveLexicon(TLexiconID aLexiconID)
	{
	TSrsLexiconIDPckg pckg(aLexiconID);
	TInt err =  iController.CustomCommandSync(iDestinationPckg, 
												ESrRemoveLexicon, 
												pckg,
												KNullDesC8);
												
	return err;
	}

EXPORT_C TInt RSpeechRecognitionCustomCommands::RemoveModelBank(TModelBankID aModelBankID)
	{
	TSrsModelBankIDPckg pckg(aModelBankID);
	TInt err =  iController.CustomCommandSync(iDestinationPckg, 
												ESrRemoveModelBank, 
												pckg,
												KNullDesC8);
												
	return err;
	}

EXPORT_C TInt RSpeechRecognitionCustomCommands::RemoveModel(TModelBankID aModelBankID, 
							TModelID aModelID)
	{
	TSrsModelBankIDPckg pckgModelBankID(aModelBankID);
	TSrsModelIDPckg pckgModelID(aModelID);
	TInt err =  iController.CustomCommandSync(iDestinationPckg, 
												ESrRemoveModel, 
												pckgModelBankID,
												pckgModelID);
												
	return err;
	}
EXPORT_C TInt RSpeechRecognitionCustomCommands::RemovePronunciation(TLexiconID aLexiconID, 
							TPronunciationID aPronunciationID)
	{
	TSrsLexiconIDPckg pckgLexiconID(aLexiconID);
	TSrsPronunciationIDPckg pckgPronunciationID(aPronunciationID);
	TInt err =  iController.CustomCommandSync(iDestinationPckg, 
												ESrRemovePronunciation, 
												pckgLexiconID,
												pckgPronunciationID);
												
	return err;
	}

EXPORT_C TInt RSpeechRecognitionCustomCommands::RemoveRule(TGrammarID aGrammarID, TRuleID aRuleID)
	{
	TSrsGrammarIDPckg pckgGrammarID(aGrammarID);
	TSrsRuleIDPckg pckgRuleID(aRuleID);
	TInt err =  iController.CustomCommandSync(iDestinationPckg, 
												ESrRemoveRule, 
												pckgGrammarID,
												pckgRuleID);
												
	return err;
	}

EXPORT_C TInt RSpeechRecognitionCustomCommands::StartRecSession(TRecognitionMode aMode)
	{
	TSrsRecognitionModePckg pckg(aMode);
	TInt err =  iController.CustomCommandSync(iDestinationPckg, 
												ESrStartRecSession, 
												pckg,
												KNullDesC8);
												
	return err;
	
	}


EXPORT_C TInt RSpeechRecognitionCustomCommands::Train(TModelBankID aModelBankID, TModelID& aModelID)
	{
	TSrsModelBankIDPckg pckgModelBankID(aModelBankID);
	TSrsModelIDPtrPckg pckgModelID(&aModelID);
	TInt err =  iController.CustomCommandSync(iDestinationPckg, 
												ESrTrain, 
												pckgModelBankID,
												pckgModelID);
												
	return err;
	}
EXPORT_C TInt RSpeechRecognitionCustomCommands::UnloadRule(TGrammarID aGrammarID, TRuleID aRuleID)
	{
	TSrsGrammarIDPckg pckgGrammarID(aGrammarID);
	TSrsRuleIDPckg pckgRuleID(aRuleID);
	TInt err =  iController.CustomCommandSync(iDestinationPckg, 
												ESrUnloadRule, 
												pckgGrammarID,
												pckgRuleID);
												
	return err;
	}

EXPORT_C TInt RSpeechRecognitionCustomCommands::LoadEngineParameters(const RArray<TInt>& aParameterId, 
		const RArray<TInt>& aParameterValue)
	{
	TRAPD(err, DoLoadEngineParametersL(aParameterId, aParameterValue));
	return err;	
	}
	
void RSpeechRecognitionCustomCommands::DoLoadEngineParametersL(const RArray<TInt>& aParameterId, 
		const RArray<TInt>& aParameterValue)
	{
	CBufFlat* param1 = ExternalizeIntArrayL(aParameterId);
	CleanupStack::PushL(param1);
	CBufFlat* param2 = ExternalizeIntArrayL(aParameterValue);
	CleanupStack::PushL(param2);
	User::LeaveIfError(iController.CustomCommandSync(iDestinationPckg, 
												ESrLoadEngineParameters, 
												param1->Ptr(0),
												param2->Ptr(0)));
												
	CleanupStack::PopAndDestroy(2,param1); //param1, param2	
	}

	
	
EXPORT_C void RSpeechRecognitionCustomCommands::GetPronunciationIDArrayL(RArray<TPronunciationID>& aPronunciationIDs)
	{
	TPckgBuf<TInt> pckgSize;
	
	User::LeaveIfError(iController.CustomCommandSync(iDestinationPckg, 
												ESrGetPronunciationIDArraySize, 
												KNullDesC8,
												KNullDesC8,
												pckgSize));
												
	HBufC8* buf = HBufC8::NewLC(pckgSize()*sizeof(TPronunciationID));
	TPtr8 ptr = buf->Des();

	User::LeaveIfError(iController.CustomCommandSync(iDestinationPckg, 
													 ESrGetPronunciationIDArrayContents,
													 KNullDesC8,
													 KNullDesC8,
													 ptr));
													 
	RDesReadStream stream(ptr);
	CleanupClosePushL(stream);

	for (TInt i=0; i<pckgSize(); i++)
		{
		User::LeaveIfError(aPronunciationIDs.Append(stream.ReadInt32L()));
		}

	CleanupStack::PopAndDestroy(2);//stream, buf
		
	}
	
	
EXPORT_C void RSpeechRecognitionCustomCommands::GetRuleIDArrayL(RArray<TRuleID>& aRuleIDs)
	{
	TPckgBuf<TInt> pckgSize;
	
	User::LeaveIfError(iController.CustomCommandSync(iDestinationPckg, 
												ESrGetRuleIDArraySize, 
												KNullDesC8,
												KNullDesC8,
												pckgSize));
												
	HBufC8* buf = HBufC8::NewLC(pckgSize()*sizeof(TRuleID));
	TPtr8 ptr = buf->Des();

	User::LeaveIfError(iController.CustomCommandSync(iDestinationPckg, 
													 ESrGetRuleIDArrayContents,
													 KNullDesC8,
													 KNullDesC8,
													 ptr));
													 
	RDesReadStream stream(ptr);
	CleanupClosePushL(stream);

	for (TInt i=0; i<pckgSize(); i++)
		{
		User::LeaveIfError(aRuleIDs.Append(stream.ReadInt32L()));
		}

	CleanupStack::PopAndDestroy(2);//stream, buf
	}

	
EXPORT_C void RSpeechRecognitionCustomCommands::GetModelIDArrayL(RArray<TModelID>& aModelIDs)
	{
	TPckgBuf<TInt> pckgSize;
	
	User::LeaveIfError(iController.CustomCommandSync(iDestinationPckg, 
												ESrGetModelIDArraySize, 
												KNullDesC8,
												KNullDesC8,
												pckgSize));
												
	HBufC8* buf = HBufC8::NewLC(pckgSize()*sizeof(TModelID));
	TPtr8 ptr = buf->Des();

	User::LeaveIfError(iController.CustomCommandSync(iDestinationPckg, 
													 ESrGetModelIDArrayContents,
													 KNullDesC8,
													 KNullDesC8,
													 ptr));
													 
	RDesReadStream stream(ptr);
	CleanupClosePushL(stream);

	for (TInt i=0; i<pckgSize(); i++)
		{
		User::LeaveIfError(aModelIDs.Append(stream.ReadInt32L()));
		}

	CleanupStack::PopAndDestroy(2);//stream, buf
	}
	
	
EXPORT_C void RSpeechRecognitionCustomCommands::GetResultSetL(CSDClientResultSet& aResultSet)
	{
	TPckgBuf<TInt> pckgSize;
	
	User::LeaveIfError(iController.CustomCommandSync(iDestinationPckg, 
												ESrGetClientResultSetSize, 
												KNullDesC8,
												KNullDesC8,
												pckgSize));
												
	HBufC8* buf = HBufC8::NewLC(pckgSize());
	TPtr8 ptr = buf->Des();

	User::LeaveIfError(iController.CustomCommandSync(iDestinationPckg, 
													 ESrGetClientResultSet,
													 KNullDesC8,
													 KNullDesC8,
													 ptr));
													 
	RDesReadStream stream(ptr);
	CleanupClosePushL(stream);

	aResultSet.InternalizeL(stream);
	CleanupStack::PopAndDestroy(2, buf);//buf, stream
	}
	
void RSpeechRecognitionCustomCommands::DoRecognizeL(CSDClientResultSet& aResultSet)
	{
	CBufFlat* dataCopyBuffer = CBufFlat::NewL(KExpandSize);
	CleanupStack::PushL(dataCopyBuffer);
	RBufWriteStream stream;
	stream.Open(*dataCopyBuffer);
	CleanupClosePushL(stream);
	
	aResultSet.ExternalizeL(stream);
	
	User::LeaveIfError(iController.CustomCommandSync(iDestinationPckg,
												ESrRecognize,
												dataCopyBuffer->Ptr(0),
												KNullDesC8));
											
	CleanupStack::PopAndDestroy(2); //stream,dataCopyBuffer
	}

	
void RSpeechRecognitionCustomCommands::DoGetEnginePropertiesL(const RArray<TInt>& aPropertyId, 
				RArray<TInt>& aPropertyValue)
	{
	CBufFlat* param1 = ExternalizeIntArrayL(aPropertyId);
	CleanupStack::PushL(param1);
	
	TInt aNumberResults = aPropertyId.Count(); // same number of values as properties
	
	HBufC8* buf = HBufC8::NewLC(aNumberResults *sizeof(TInt));
	TPtr8 ptr = buf->Des();

	User::LeaveIfError(iController.CustomCommandSync(iDestinationPckg, 
												ESrGetEngineProperties, 
												param1->Ptr(0),
												KNullDesC8,
												ptr));

	InternalizeIntArrayL(ptr,aNumberResults,aPropertyValue);
	
	CleanupStack::PopAndDestroy(2,param1);//param1, buf
	}

CBufFlat* RSpeechRecognitionCustomCommands::ExternalizeIntArrayL(const RArray<TInt>& aArray)
	{
	CBufFlat* dataCopyBuffer = CBufFlat::NewL(KExpandSize);
	CleanupStack::PushL(dataCopyBuffer);
	RBufWriteStream stream;
	stream.Open(*dataCopyBuffer);
	CleanupClosePushL(stream);
	
	stream.WriteInt32L(aArray.Count());
	
	for (TInt i=0;i<aArray.Count();i++)
		stream.WriteUint32L(aArray[i]);

	CleanupStack::PopAndDestroy(1); //stream
	CleanupStack::Pop(); //dataCopyBuffer;
	return dataCopyBuffer;	
	}
	
void RSpeechRecognitionCustomCommands::InternalizeIntArrayL(TDes8& aDes, TInt aNumberElements, RArray<TInt>& aArray)
	{
	RDesReadStream stream(aDes);
	CleanupClosePushL(stream);

	for (TInt i=0; i<aNumberElements; i++)
		{
		User::LeaveIfError(aArray.Append(stream.ReadInt32L()));
		}

	CleanupStack::PopAndDestroy();//stream
	}
	
EXPORT_C TInt RSpeechRecognitionCustomCommands::GetAllModelBankIDs()
	{
	TInt err =  iController.CustomCommandSync(iDestinationPckg, 
												ESrGetAllModelBankIDs, 
												KNullDesC8,
												KNullDesC8);
												
	return err;
	}

EXPORT_C TInt RSpeechRecognitionCustomCommands::GetAllLexiconIDs()
	{
	TInt err = iController.CustomCommandSync(iDestinationPckg, 
												ESrGetAllLexiconIDs, 
												KNullDesC8,
												KNullDesC8);
	return err;
												
	}

EXPORT_C TInt RSpeechRecognitionCustomCommands::GetAllGrammarIDs()
	{
	TInt err =  iController.CustomCommandSync(iDestinationPckg, 
												ESrGetAllGrammarIDs, 
												KNullDesC8,
												KNullDesC8);
												
	return err;
	}
	
EXPORT_C TInt RSpeechRecognitionCustomCommands::GetAllClientModelBankIDs()
	{
	TInt err =  iController.CustomCommandSync(iDestinationPckg, 
												ESrGetAllClientModelBankIDs, 
												KNullDesC8,
												KNullDesC8);
												
	return err;
	}

EXPORT_C TInt RSpeechRecognitionCustomCommands::GetAllClientLexiconIDs()
	{
	TInt err = iController.CustomCommandSync(iDestinationPckg, 
												ESrGetAllClientLexiconIDs, 
												KNullDesC8,
												KNullDesC8);
	return err;
												
	}

EXPORT_C TInt RSpeechRecognitionCustomCommands::GetAllClientGrammarIDs()
	{
	TInt err =  iController.CustomCommandSync(iDestinationPckg, 
												ESrGetAllClientGrammarIDs, 
												KNullDesC8,
												KNullDesC8);
												
	return err;
	}
	
	
EXPORT_C void RSpeechRecognitionCustomCommands::GetModelBankIDArrayL(RArray<TModelBankID>& aModelBankIDs)
	{
	TPckgBuf<TInt> pckgSize;
	
	User::LeaveIfError(iController.CustomCommandSync(iDestinationPckg, 
												ESrGetModelBankIDArraySize, 
												KNullDesC8,
												KNullDesC8,
												pckgSize));
												
	HBufC8* buf = HBufC8::NewLC(pckgSize()*sizeof(TModelID));
	TPtr8 ptr = buf->Des();

	User::LeaveIfError(iController.CustomCommandSync(iDestinationPckg, 
													 ESrGetModelBankIDArrayContents,
													 KNullDesC8,
													 KNullDesC8,
													 ptr));
													 
	RDesReadStream stream(ptr);
	CleanupClosePushL(stream);

	for (TInt i=0; i<pckgSize(); i++)
		{
		User::LeaveIfError(aModelBankIDs.Append(stream.ReadInt32L()));
		}

	CleanupStack::PopAndDestroy(2);//stream, buf
	}


EXPORT_C void RSpeechRecognitionCustomCommands::GetGrammarIDArrayL(RArray<TGrammarID>& aGrammarIDs)
	{
	TPckgBuf<TInt> pckgSize;
	
	User::LeaveIfError(iController.CustomCommandSync(iDestinationPckg, 
												ESrGetGrammarIDArraySize, 
												KNullDesC8,
												KNullDesC8,
												pckgSize));
												
	HBufC8* buf = HBufC8::NewLC(pckgSize()*sizeof(TGrammarID));
	TPtr8 ptr = buf->Des();

	User::LeaveIfError(iController.CustomCommandSync(iDestinationPckg, 
													 ESrGetGrammarIDArrayContents,
													 KNullDesC8,
													 KNullDesC8,
													 ptr));
													 
	RDesReadStream stream(ptr);
	CleanupClosePushL(stream);

	for (TInt i=0; i<pckgSize(); i++)
		{
		User::LeaveIfError(aGrammarIDs.Append(stream.ReadInt32L()));
		}

	CleanupStack::PopAndDestroy(2);//stream, buf
	}


EXPORT_C void RSpeechRecognitionCustomCommands::GetLexiconIDArrayL(RArray<TLexiconID>& aLexiconIDs)
	{
	TPckgBuf<TInt> pckgSize;
	
	User::LeaveIfError(iController.CustomCommandSync(iDestinationPckg, 
												ESrGetLexiconIDArraySize, 
												KNullDesC8,
												KNullDesC8,
												pckgSize));
												
	HBufC8* buf = HBufC8::NewLC(pckgSize()*sizeof(TLexiconID));
	TPtr8 ptr = buf->Des();

	User::LeaveIfError(iController.CustomCommandSync(iDestinationPckg, 
													 ESrGetLexiconIDArrayContents,
													 KNullDesC8,
													 KNullDesC8,
													 ptr));
													 
	RDesReadStream stream(ptr);
	CleanupClosePushL(stream);

	for (TInt i=0; i<pckgSize(); i++)
		{
		User::LeaveIfError(aLexiconIDs.Append(stream.ReadInt32L()));
		}

	CleanupStack::PopAndDestroy(2);//stream, buf
	}


enum TDllReason {};
EXPORT_C TInt E32Dll(TDllReason /* aReason */)	
	{
	return (KErrNone);
	}


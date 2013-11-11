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


#include <mmf/common/speechrecognitioncustomcommandparser.h>
#include "SpeechRecognitionCustomCommandCommon.h"

const TInt KExpandSize = 10;


EXPORT_C CSpeechRecognitionCustomCommandParser* CSpeechRecognitionCustomCommandParser::NewL(
							MSpeechRecognitionCustomCommandImplementor& aImplementor)
	{
	CSpeechRecognitionCustomCommandParser* self = new (ELeave) CSpeechRecognitionCustomCommandParser(aImplementor);
	return self;
	}

EXPORT_C CSpeechRecognitionCustomCommandParser::~CSpeechRecognitionCustomCommandParser()
	{
	iPronunciationIDs.Close();
	iRuleIDs.Close();
	iModelIDs.Close();
	iModelBankIDs.Close();
	iGrammarIDs.Close();
	iLexiconIDs.Close();
	delete iResultSet;
	delete iResultCopyBuffer;	
	}

void CSpeechRecognitionCustomCommandParser::HandleRequest(TMMFMessage& aMessage)
	{
	TInt err;
	if (aMessage.Destination().InterfaceId() == KUidInterfaceSpeechRecognition)
		{
		TRAP(err, DoHandleRequestL(aMessage));
		aMessage.Complete(err);
		}
	else
		aMessage.Complete(KErrNotSupported);
	}


CSpeechRecognitionCustomCommandParser::CSpeechRecognitionCustomCommandParser(MSpeechRecognitionCustomCommandImplementor& aImplementor)
	: CMMFCustomCommandParserBase(KUidInterfaceSpeechRecognition), iImplementor(aImplementor)
	{
	
	}

void CSpeechRecognitionCustomCommandParser::DoHandleRequestL(TMMFMessage& aMessage)
	{	
	TPckgBuf<TInt> pckgSize;
	
	switch (aMessage.Function())
		{
		case ESrAddPronunciation:
			DoAddPronunciationL(aMessage);
			break;
		case ESrAddRule:
			DoAddRuleL(aMessage);
			break;
		case ESrCancel:
			DoCancel();
			break;
		case ESrCommitChanges:
			DoCommitChangesL();
			break;
		case ESrCreateGrammar:
			DoCreateGrammarL(aMessage);
			break;
		case ESrCreateLexicon:
			DoCreateLexiconL(aMessage);
			break;
		case ESrCreateModelBank:
			DoCreateModelBankL(aMessage);
			break;
		case ESrEndRecSession:
			DoEndRecSessionL();
			break;
		case ESrGetAllModelIDs:
			DoGetAllModelIDsL(aMessage);
			break;
		case ESrGetAllPronunciationIDs:
			DoGetAllPronunciationIDsL(aMessage);
			break;
		case ESrGetAllRuleIDs:
			DoGetAllRuleIDsL(aMessage);
			break;
		case ESrGetAllModelBankIDs:
			DoGetAllModelBankIDsL();
			break;
		case ESrGetAllLexiconIDs:
			DoGetAllLexiconIDsL();
			break;
		case ESrGetAllGrammarIDs:
			DoGetAllGrammarIDsL();
			break;
		case ESrGetAllClientModelBankIDs:
			DoGetAllClientModelBankIDsL();
			break;
		case ESrGetAllClientLexiconIDs:
			DoGetAllClientLexiconIDsL();
			break;
		case ESrGetAllClientGrammarIDs:
			DoGetAllClientGrammarIDsL();
			break;
		case ESrGetAvailableStorage:
			DoGetAvailableStorageL(aMessage);
			break;
		case ESrGetEngineProperties:
			DoGetEnginePropertiesL(aMessage);
			break;
		case ESrGetModelCount:
			DoGetModelCountL(aMessage);
			break;
		case ESrGetRuleValidity:
			DoGetRuleValidityL(aMessage);
			break;
		case ESrUtteranceDuration:
			DoGetUtteranceDurationL(aMessage);
			break;
		case ESrLoadGrammar:
			DoLoadGrammarL(aMessage);
			break;
		case ESrLoadLexicon:
			DoLoadLexiconL(aMessage);
			break;
		case ESrLoadModels:
			DoLoadModelsL(aMessage);
			break;
		case ESrPlayUtterance:
			DoPlayUtteranceL(aMessage);
			break;
		case ESrRecognize:
			DoRecognizeL(aMessage);
			break;
		case ESrRecord:
			DoRecordL(aMessage);
			break;
		case ESrRemoveGrammar:
			DoRemoveGrammarL(aMessage);
			break;
		case ESrRemoveLexicon:
			DoRemoveLexiconL(aMessage);
			break;
		case ESrRemoveModelBank:
			DoRemoveModelBankL(aMessage);
			break;
		case ESrRemoveModel:
			DoRemoveModelL(aMessage);
			break;
		case ESrRemovePronunciation:
			DoRemovePronunciationL(aMessage);
			break;
		case ESrRemoveRule:
			DoRemoveRuleL(aMessage);
			break;
		case ESrStartRecSession:
			DoStartRecSessionL(aMessage);
			break;
		case ESrTrain:
			DoTrainL(aMessage);
			break;
		case ESrUnloadRule:
			DoUnloadRuleL(aMessage);
			break;
		case ESrLoadEngineParameters:
			DoLoadEngineParametersL(aMessage);
			break;
		case ESrGetPronunciationIDArraySize:
			pckgSize() = iPronunciationIDs.Count();
			aMessage.WriteDataToClientL(pckgSize);
			break;
		case ESrGetPronunciationIDArrayContents:
			DoCopyPronunciationIDArrayL(aMessage);
			break;
		case ESrGetRuleIDArraySize:
			pckgSize() = iRuleIDs.Count();
			aMessage.WriteDataToClientL(pckgSize);
			break;
		case ESrGetRuleIDArrayContents:
			DoCopyRuleIDArrayL(aMessage);			
			break;
		case ESrGetModelIDArraySize:
			pckgSize() = iModelIDs.Count();
			aMessage.WriteDataToClientL(pckgSize);
			break;
		case ESrGetModelIDArrayContents:
			DoCopyModelIDArrayL(aMessage);
			break;
		case ESrGetLexiconIDArraySize:
			pckgSize() = iLexiconIDs.Count();
			aMessage.WriteDataToClientL(pckgSize);
			break;
		case ESrGetLexiconIDArrayContents:
			DoCopyLexiconIDArrayL(aMessage);
			break;
		case ESrGetGrammarIDArraySize:
			pckgSize() = iGrammarIDs.Count();
			aMessage.WriteDataToClientL(pckgSize);
			break;
		case ESrGetGrammarIDArrayContents:
			DoCopyGrammarIDArrayL(aMessage);			
			break;
		case ESrGetModelBankIDArraySize:
			pckgSize() = iModelBankIDs.Count();
			aMessage.WriteDataToClientL(pckgSize);
			break;
		case ESrGetModelBankIDArrayContents:
			DoCopyModelBankIDArrayL(aMessage);
			break;
		case ESrGetClientResultSetSize:
			DoExternalizeResultSetL();
			pckgSize() = iResultCopyBuffer->Size();
			aMessage.WriteDataToClientL(pckgSize);
			break;
		case ESrGetClientResultSet:
			DoCopyResultSetL(aMessage);
			break;
		case ESrSetClientUid:
			DoSetClientUidL(aMessage);
			break;
		default:
			User::Leave(KErrNotSupported);
			break;
		}
	}

void CSpeechRecognitionCustomCommandParser::DoAddPronunciationL(TMMFMessage& aMessage)
	{
	TSrsAddPronunciationPckg pckg;
	aMessage.ReadData1FromClientL(pckg);
	iImplementor.MSrAddPronunciationL(pckg().iLexiconID, pckg().iModelBankID, pckg().iModelID, *pckg().iPronunciationID);
	}
	
void CSpeechRecognitionCustomCommandParser::DoAddRuleL(TMMFMessage& aMessage)
	{
	TSrsAddRulePckg pckg;
	aMessage.ReadData1FromClientL(pckg);
	iImplementor.MSrAddRuleL(pckg().iGrammarID, pckg().iLexiconID, pckg().iPronunciationID, *pckg().iRuleID);
	}

void CSpeechRecognitionCustomCommandParser::DoCancel()
	{
	iImplementor.MSrCancel();
	}

void CSpeechRecognitionCustomCommandParser::DoCommitChangesL()
	{
	iImplementor.MSrCommitChangesL();
	}

void CSpeechRecognitionCustomCommandParser::DoCreateGrammarL(TMMFMessage& aMessage)
	{
	TSrsGrammarIDPtrPckg pckg;
	aMessage.ReadData1FromClientL(pckg);
	iImplementor.MSrCreateGrammarL(*pckg());
	}

void CSpeechRecognitionCustomCommandParser::DoCreateLexiconL(TMMFMessage& aMessage)
	{
	TSrsLexiconIDPtrPckg pckg;
	aMessage.ReadData1FromClientL(pckg);
	iImplementor.MSrCreateLexiconL(*pckg());
	}

void CSpeechRecognitionCustomCommandParser::DoCreateModelBankL(TMMFMessage& aMessage)
	{
	TSrsModelBankIDPtrPckg pckg;
	aMessage.ReadData1FromClientL(pckg);
	iImplementor.MSrCreateModelBankL(*pckg());
	}

void CSpeechRecognitionCustomCommandParser::DoEndRecSessionL()
	{
	iImplementor.MSrEndRecSessionL();
	}

void CSpeechRecognitionCustomCommandParser::DoGetAllModelIDsL(TMMFMessage& aMessage)
	{
	TSrsModelBankIDPckg pckgModelBank;
	iModelIDs.Reset();
	aMessage.ReadData1FromClientL(pckgModelBank);
	iImplementor.MSrGetAllModelIDsL(pckgModelBank(), iModelIDs);
	}

void CSpeechRecognitionCustomCommandParser::DoGetAllPronunciationIDsL(TMMFMessage& aMessage)
	{
	TSrsLexiconIDPckg pckgLexicon;
	iPronunciationIDs.Reset();
	aMessage.ReadData1FromClientL(pckgLexicon);
	iImplementor.MSrGetAllPronunciationIDsL(pckgLexicon(), iPronunciationIDs);
	}

void CSpeechRecognitionCustomCommandParser::DoGetAllRuleIDsL(TMMFMessage& aMessage)
	{
	TSrsGrammarIDPckg pckgGrammar;
	iRuleIDs.Reset();
	aMessage.ReadData1FromClientL(pckgGrammar);
	iImplementor.MSrGetAllRuleIDsL(pckgGrammar(), iRuleIDs);
	}

void CSpeechRecognitionCustomCommandParser::DoGetAllModelBankIDsL()
	{
	iModelBankIDs.Reset();
	iImplementor.MSrGetAllModelBankIDsL(iModelBankIDs);
	}

void CSpeechRecognitionCustomCommandParser::DoGetAllLexiconIDsL()
	{
	iLexiconIDs.Reset();
	iImplementor.MSrGetAllLexiconIDsL(iLexiconIDs);
	}

void CSpeechRecognitionCustomCommandParser::DoGetAllGrammarIDsL()
	{
	iGrammarIDs.Reset();
	iImplementor.MSrGetAllGrammarIDsL(iGrammarIDs);
	}
	
	
void CSpeechRecognitionCustomCommandParser::DoGetAllClientModelBankIDsL()
	{
	iModelBankIDs.Reset();
	iImplementor.MSrGetAllClientModelBankIDsL(iModelBankIDs);
	}

void CSpeechRecognitionCustomCommandParser::DoGetAllClientLexiconIDsL()
	{
	iLexiconIDs.Reset();
	iImplementor.MSrGetAllClientLexiconIDsL(iLexiconIDs);
	}

void CSpeechRecognitionCustomCommandParser::DoGetAllClientGrammarIDsL()
	{
	iGrammarIDs.Reset();
	iImplementor.MSrGetAllClientGrammarIDsL(iGrammarIDs);
	}


void CSpeechRecognitionCustomCommandParser::DoGetAvailableStorageL(TMMFMessage& aMessage)
	{
	TSrsIntPtrPckg pckg;
	aMessage.ReadData1FromClientL(pckg);
	iImplementor.MSrGetAvailableStorageL(*pckg());
	}


void CSpeechRecognitionCustomCommandParser::DoGetModelCountL(TMMFMessage& aMessage)
	{
	TSrsModelBankIDPckg pckgModelBank;
	TSrsIntPtrPckg pckgResult;
	aMessage.ReadData1FromClientL(pckgModelBank);
	aMessage.ReadData2FromClientL(pckgResult);
	iImplementor.MSrGetModelCountL(pckgModelBank(),*pckgResult());
	}

void CSpeechRecognitionCustomCommandParser::DoGetRuleValidityL(TMMFMessage& aMessage)
	{
	TSrsRuleValidityPckg pckg;
	aMessage.ReadData1FromClientL(pckg);
	iImplementor.MSrGetRuleValidityL(pckg().iGrammarID, pckg().iRuleID, *pckg().iRuleValid);
	}

void CSpeechRecognitionCustomCommandParser::DoGetUtteranceDurationL(TMMFMessage& aMessage)
	{
	TSrsGetUtteranceDurationPckg pckg;
	aMessage.ReadData1FromClientL(pckg);
	iImplementor.MSrGetUtteranceDurationL(pckg().iModelBankID, pckg().iModelID, *pckg().iDuration);
	}

void CSpeechRecognitionCustomCommandParser::DoLoadGrammarL(TMMFMessage& aMessage)
	{
	TSrsGrammarIDPckg pckg;
	aMessage.ReadData1FromClientL(pckg);
	iImplementor.MSrLoadGrammarL(pckg());
	}

void CSpeechRecognitionCustomCommandParser::DoLoadLexiconL(TMMFMessage& aMessage)
	{
	TSrsLexiconIDPckg pckg;
	aMessage.ReadData1FromClientL(pckg);
	iImplementor.MSrLoadLexiconL(pckg());
	}

void CSpeechRecognitionCustomCommandParser::DoLoadModelsL(TMMFMessage& aMessage)
	{
	TSrsModelBankIDPckg pckg;
	aMessage.ReadData1FromClientL(pckg);
	iImplementor.MSrLoadModelsL(pckg());
	}

void CSpeechRecognitionCustomCommandParser::DoPlayUtteranceL(TMMFMessage& aMessage)
	{
	TSrsPlayUtterancePckg pckg;
	aMessage.ReadData1FromClientL(pckg);
	iImplementor.MSrPlayUtteranceL(pckg().iModelBankID, pckg().iModelID);
	}
	
void CSpeechRecognitionCustomCommandParser::DoSetClientUidL(TMMFMessage& aMessage)
	{
	TSrsUidPckg pckg;
	aMessage.ReadData1FromClientL(pckg);
	iImplementor.MSrSetClientUid(pckg());
	}


void CSpeechRecognitionCustomCommandParser::DoRecognizeL(TMMFMessage& aMessage)
	{
	if (iResultSet)
		User::Leave(KErrInUse);

	TInt size;
	size = User::LeaveIfError(aMessage.SizeOfData1FromClient());
	HBufC8* dataBuffer = HBufC8::NewLC(size);
	TPtr8 buf = dataBuffer->Des();
	aMessage.ReadData1FromClientL(buf);
	RDesReadStream stream(buf);
	CleanupClosePushL(stream);
	iResultSet = CSDClientResultSet::NewL();
	TInt err = KErrNone;
	TRAP(err, iResultSet->InternalizeL(stream));
	if (err == KErrNone)
		{
		TRAP(err, iImplementor.MSrRecognizeL(*iResultSet));
		}
	if (err!=KErrNone)
		{
		delete iResultSet;
		iResultSet = NULL;
		User::Leave(err);
		}
	CleanupStack::PopAndDestroy(2, dataBuffer);//dataBuffer, stream
	}

void CSpeechRecognitionCustomCommandParser::DoRecordL(TMMFMessage& aMessage)
	{
	TSrsTimeIntervalMicroSeconds32Pckg pckg;
	aMessage.ReadData1FromClientL(pckg);
	iImplementor.MSrRecordL(pckg());
	}

void CSpeechRecognitionCustomCommandParser::DoRemoveGrammarL(TMMFMessage& aMessage)
	{
	TSrsGrammarIDPckg pckg;
	aMessage.ReadData1FromClientL(pckg);
	iImplementor.MSrRemoveGrammarL(pckg());
	}

void CSpeechRecognitionCustomCommandParser::DoRemoveLexiconL(TMMFMessage& aMessage)
	{
	TSrsLexiconIDPckg pckg;
	aMessage.ReadData1FromClientL(pckg);
	iImplementor.MSrRemoveLexiconL(pckg());
	}

void CSpeechRecognitionCustomCommandParser::DoRemoveModelBankL(TMMFMessage& aMessage)
	{
	TSrsModelBankIDPckg pckg;
	aMessage.ReadData1FromClientL(pckg);
	iImplementor.MSrRemoveModelBankL(pckg());
	}

void CSpeechRecognitionCustomCommandParser::DoRemoveModelL(TMMFMessage& aMessage)
	{
	TSrsModelBankIDPckg pckgModelBankID;
	TSrsModelIDPckg pckgModelID;
	aMessage.ReadData1FromClientL(pckgModelBankID);
	aMessage.ReadData2FromClientL(pckgModelID);
	iImplementor.MSrRemoveModelL(pckgModelBankID(), pckgModelID());
	}

void CSpeechRecognitionCustomCommandParser::DoRemovePronunciationL(TMMFMessage& aMessage)
	{
	TSrsLexiconIDPckg pckgLexicon;
	TSrsPronunciationIDPckg pckgPronunciation;
	aMessage.ReadData1FromClientL(pckgLexicon);
	aMessage.ReadData2FromClientL(pckgPronunciation);
	iImplementor.MSrRemovePronunciationL(pckgLexicon(), pckgPronunciation());
	}

void CSpeechRecognitionCustomCommandParser::DoRemoveRuleL(TMMFMessage& aMessage)
	{
	TSrsGrammarIDPckg pckgGrammar;
	TSrsRuleIDPckg pckgRule;
	aMessage.ReadData1FromClientL(pckgGrammar);
	aMessage.ReadData2FromClientL(pckgRule);
	iImplementor.MSrRemoveRuleL(pckgGrammar(), pckgRule());
	}

void CSpeechRecognitionCustomCommandParser::DoStartRecSessionL(TMMFMessage& aMessage)
	{
	TSrsRecognitionModePckg pckg;
	aMessage.ReadData1FromClientL(pckg);
	iImplementor.MSrStartRecSessionL(pckg());
	}

void CSpeechRecognitionCustomCommandParser::DoTrainL(TMMFMessage& aMessage)
	{
	TSrsModelBankIDPckg pckgModelBankID;
	TSrsModelIDPtrPckg pckgModelIDPtr;
	aMessage.ReadData1FromClientL(pckgModelBankID);
	aMessage.ReadData2FromClientL(pckgModelIDPtr);
	iImplementor.MSrTrainL(pckgModelBankID(),*pckgModelIDPtr());
	}

void CSpeechRecognitionCustomCommandParser::DoUnloadRuleL(TMMFMessage& aMessage)
	{
	TSrsGrammarIDPckg pckgGrammar;
	TSrsRuleIDPckg pckgRule;
	aMessage.ReadData1FromClientL(pckgGrammar);
	aMessage.ReadData2FromClientL(pckgRule);
	iImplementor.MSrUnloadRuleL(pckgGrammar(), pckgRule());
	}

void CSpeechRecognitionCustomCommandParser::DoLoadEngineParametersL(TMMFMessage& aMessage)
	{
	RArray<TInt> parameters;
	RArray<TInt> values;
	CleanupClosePushL(parameters);
	CleanupClosePushL(values);
	DoExtractIntArrayFromData1L(aMessage, parameters);
	DoExtractIntArrayFromData2L(aMessage, values);
	iImplementor.MSrLoadEngineParametersL(parameters,values);
	CleanupStack::PopAndDestroy(2, &parameters); // parameters, values
	}
	
void CSpeechRecognitionCustomCommandParser::DoGetEnginePropertiesL(TMMFMessage& aMessage)
	{
	RArray<TInt> propertyIDs;
	RArray<TInt> propertyValues;
	CleanupClosePushL(propertyIDs);
	CleanupClosePushL(propertyValues);
	DoExtractIntArrayFromData1L(aMessage, propertyIDs);
	
	iImplementor.MSrGetEnginePropertiesL(propertyIDs,propertyValues);
	// the custom command is aware of the size of the result, so it can copy the data back directly
	DoCopyIntArrayL(aMessage, propertyValues);	
	CleanupStack::PopAndDestroy(2, &propertyIDs); // property IDs, property values
	}

	
void CSpeechRecognitionCustomCommandParser::DoCopyPronunciationIDArrayL(TMMFMessage& aMessage)
	{
	CBufFlat* dataBuffer = CBufFlat::NewL(KExpandSize);
	CleanupStack::PushL(dataBuffer);
	RBufWriteStream stream;
	stream.Open(*dataBuffer);
	CleanupClosePushL(stream);
	for (TInt i=0;i<iPronunciationIDs.Count();i++)
		stream.WriteUint32L(iPronunciationIDs[i]);

	aMessage.WriteDataToClientL(dataBuffer->Ptr(0));
	
	iPronunciationIDs.Close();
	CleanupStack::PopAndDestroy(2, dataBuffer); //dataBuffer, stream
	}
void CSpeechRecognitionCustomCommandParser::DoCopyModelIDArrayL(TMMFMessage& aMessage)
	{
	CBufFlat* dataBuffer = CBufFlat::NewL(KExpandSize);
	CleanupStack::PushL(dataBuffer);
	RBufWriteStream stream;
	stream.Open(*dataBuffer);
	CleanupClosePushL(stream);
	for (TInt i=0;i<iModelIDs.Count();i++)
		stream.WriteUint32L(iModelIDs[i]);

	aMessage.WriteDataToClientL(dataBuffer->Ptr(0));
	
	iModelIDs.Close();
	CleanupStack::PopAndDestroy(2, dataBuffer); //dataBuffer, stream
	}

void CSpeechRecognitionCustomCommandParser::DoCopyRuleIDArrayL(TMMFMessage& aMessage)
	{
	CBufFlat* dataBuffer = CBufFlat::NewL(KExpandSize);
	CleanupStack::PushL(dataBuffer);
	RBufWriteStream stream;
	stream.Open(*dataBuffer);
	CleanupClosePushL(stream);
	for (TInt i=0;i<iRuleIDs.Count();i++)
		stream.WriteUint32L(iRuleIDs[i]);

	aMessage.WriteDataToClientL(dataBuffer->Ptr(0));
	
	iRuleIDs.Close();
	CleanupStack::PopAndDestroy(2, dataBuffer); //dataBuffer, stream
	}

//------------------------------------------------------------------------------------------------------------------

void CSpeechRecognitionCustomCommandParser::DoCopyLexiconIDArrayL(TMMFMessage& aMessage)
	{
	CBufFlat* dataBuffer = CBufFlat::NewL(KExpandSize);
	CleanupStack::PushL(dataBuffer);
	RBufWriteStream stream;
	stream.Open(*dataBuffer);
	CleanupClosePushL(stream);
	for (TInt i=0;i<iLexiconIDs.Count();i++)
		stream.WriteUint32L(iLexiconIDs[i]);

	aMessage.WriteDataToClientL(dataBuffer->Ptr(0));
	
	iLexiconIDs.Close();
	CleanupStack::PopAndDestroy(2, dataBuffer); //dataBuffer, stream
	}
void CSpeechRecognitionCustomCommandParser::DoCopyModelBankIDArrayL(TMMFMessage& aMessage)
	{
	CBufFlat* dataBuffer = CBufFlat::NewL(KExpandSize);
	CleanupStack::PushL(dataBuffer);
	RBufWriteStream stream;
	stream.Open(*dataBuffer);
	CleanupClosePushL(stream);
	for (TInt i=0;i<iModelBankIDs.Count();i++)
		stream.WriteUint32L(iModelBankIDs[i]);

	aMessage.WriteDataToClientL(dataBuffer->Ptr(0));
	
	iModelBankIDs.Close();
	CleanupStack::PopAndDestroy(2, dataBuffer); //dataBuffer, stream
	}

void CSpeechRecognitionCustomCommandParser::DoCopyGrammarIDArrayL(TMMFMessage& aMessage)
	{
	CBufFlat* dataBuffer = CBufFlat::NewL(KExpandSize);
	CleanupStack::PushL(dataBuffer);
	RBufWriteStream stream;
	stream.Open(*dataBuffer);
	CleanupClosePushL(stream);
	for (TInt i=0;i<iGrammarIDs.Count();i++)
		stream.WriteUint32L(iGrammarIDs[i]);

	aMessage.WriteDataToClientL(dataBuffer->Ptr(0));
	
	iGrammarIDs.Close();
	CleanupStack::PopAndDestroy(2, dataBuffer); //dataBuffer, stream
	}


//--------------------------------------------------------------------------------------------------------------------

	
void CSpeechRecognitionCustomCommandParser::DoCopyIntArrayL(TMMFMessage& aMessage, const RArray<TInt>& aArray)
	{
	CBufFlat* dataBuffer = CBufFlat::NewL(KExpandSize);
	CleanupStack::PushL(dataBuffer);
	RBufWriteStream stream;
	stream.Open(*dataBuffer);
	CleanupClosePushL(stream);
	for (TInt i=0;i<aArray.Count();i++)
		stream.WriteUint32L(aArray[i]);

	aMessage.WriteDataToClientL(dataBuffer->Ptr(0));
	CleanupStack::PopAndDestroy(2, dataBuffer); //dataBuffer, stream
	}
	
void CSpeechRecognitionCustomCommandParser::DoExtractIntArrayFromData1L(TMMFMessage& aMessage, RArray<TInt>& aArray)
	{
	TInt size = User::LeaveIfError(aMessage.SizeOfData1FromClient());
	HBufC8* buf = HBufC8::NewL(size);
	CleanupStack::PushL(buf);
	TPtr8 ptr = buf->Des();
	aMessage.ReadData1FromClientL(ptr);
	DoExtractIntArrayL(ptr,aArray);
	CleanupStack::PopAndDestroy(buf);
	}
	
void CSpeechRecognitionCustomCommandParser::DoExtractIntArrayFromData2L(TMMFMessage& aMessage, RArray<TInt>& aArray)
	{
	TInt size = User::LeaveIfError(aMessage.SizeOfData2FromClient());
	HBufC8* buf = HBufC8::NewL(size);
	CleanupStack::PushL(buf);
	TPtr8 ptr = buf->Des();
	aMessage.ReadData2FromClientL(ptr);
	DoExtractIntArrayL(ptr,aArray);
	CleanupStack::PopAndDestroy(buf);
	}

	
void CSpeechRecognitionCustomCommandParser::DoExtractIntArrayL(TDes8& aDescriptor, RArray<TInt>& aArray)
	{
	RDesReadStream stream(aDescriptor);
	CleanupClosePushL(stream);
	
	TInt numberElements;
	numberElements = stream.ReadInt32L();

	for (TInt i=0; i<numberElements; i++)
		{
		User::LeaveIfError(aArray.Append(stream.ReadInt32L()));
		}
	CleanupStack::PopAndDestroy(&stream); //stream
	}

	
void CSpeechRecognitionCustomCommandParser::DoExternalizeResultSetL()
	{
	ASSERT(iResultSet);
	iResultCopyBuffer = CBufFlat::NewL(KExpandSize);
	RBufWriteStream stream;
	stream.Open(*iResultCopyBuffer);
	CleanupClosePushL(stream);
	iResultSet->ExternalizeL(stream);
	delete iResultSet;
	iResultSet = NULL;
	CleanupStack::PopAndDestroy(&stream); //stream
	}
void CSpeechRecognitionCustomCommandParser::DoCopyResultSetL(TMMFMessage& aMessage)
	{
	aMessage.WriteDataToClientL(iResultCopyBuffer->Ptr(0));
	delete iResultCopyBuffer;
	iResultCopyBuffer = NULL;
	}

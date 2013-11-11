// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "char_audiorec_testsuite.h"
#include "char_audiorec_common.h"
#include "audiorec_teststep.h"


// Entry function - create a test suite object
EXPORT_C CTestSuiteCharAudioRec* NewTestSuiteL() 
    { 
	return CTestSuiteCharAudioRec::NewL();
    }
    
CTestSuiteCharAudioRec* CTestSuiteCharAudioRec::NewL() 
    { 
	CTestSuiteCharAudioRec* self = new (ELeave) CTestSuiteCharAudioRec;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
    }

CTestSuiteCharAudioRec::CTestSuiteCharAudioRec()
	{
	iSuiteName = _L("CHAR_MMF_AUDIOREC");
	}

CTestSuiteCharAudioRec::~CTestSuiteCharAudioRec()
	{
	}

void CTestSuiteCharAudioRec::InitialiseL( void )
	{

	AddTestStepL(RAudioRecTestStep::NewL(_L("MM-MMF-ARCLNT-CHRTZ-I-0001"),_L("Basic")));
	
	//Open an audio file in Opened state
	AddTestStepL(RTestStepAudioRecOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-I-0002"),_L("Basic")));
	
	AddTestStepL(RTestStepAudioRecPlayInPlaying::NewL(_L("MM-MMF-ARCLNT-CHRTZ-I-0004"),_L("Basic")));
	AddTestStepL(RTestStepAudioRecPlayBeforeOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-I-0005"),_L("Basic")));
	
	AddTestStepL(RTestStepAudioRecRecordBeforeOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-I-0008"),_L("Basic")));
	AddTestStepL(RTestStepAudioRecRecordMaxSize::NewL(_L("MM-MMF-ARCLNT-CHRTZ-I-0009"),_L("Basic")));
	AddTestStepL(RTestStepAudioRecRecordWhileInRecording::NewL(_L("MM-MMF-ARCLNT-CHRTZ-I-0010"),_L("Basic")));
	
	AddTestStepL(RTestStepAudioRecMaxVolBeforeOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-I-0012"),_L("Basic")));
	AddTestStepL(RTestStepAudioRecMaxVolBeforeOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-I-0013"),_L("Basic")));
	AddTestStepL(RTestStepAudioRecMaxVolBeforeOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-I-0014"),_L("Basic")));
	
	AddTestStepL(RTestStepAudioRecSetGetVolBeforeOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-I-0017"),_L("Basic")));
	AddTestStepL(RTestStepAudioRecSetVolInOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-I-0015"),_L("Basic")));
	AddTestStepL(RTestStepAudioRecSetVolInOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-I-0016"),_L("Basic")));
	AddTestStepL(RTestStepAudioRecSetVolInOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-I-0018"),_L("Basic")));
	AddTestStepL(RTestStepAudioRecSetVolInOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-I-0019"),_L("Basic")));
	
	AddTestStepL(RTestStepAudioRecSetVolRampBeforeOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-I-0021"),_L("Basic")));
	AddTestStepL(RTestStepAudioRecSetVolRampBeforeOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-I-0022"),_L("Basic")));
	
	AddTestStepL(RTestStepAudioRecMaxGainBeforeOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-I-0023"),_L("Basic")));
	AddTestStepL(RTestStepAudioRecMaxGainBeforeOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-I-0024"),_L("Basic")));
	AddTestStepL(RTestStepAudioRecMaxGainBeforeOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-I-0025"),_L("Basic")));
	
	AddTestStepL(RTestStepAudioRecSetAndGetGaininAllCond::NewL(_L("MM-MMF-ARCLNT-CHRTZ-I-0026"),_L("Basic")));
	AddTestStepL(RTestStepAudioRecSetAndGetGaininAllCond::NewL(_L("MM-MMF-ARCLNT-CHRTZ-I-0027"),_L("Basic")));
	AddTestStepL(RTestStepAudioRecSetAndGetGaininAllCond::NewL(_L("MM-MMF-ARCLNT-CHRTZ-I-0028"),_L("Basic")));
	AddTestStepL(RTestStepAudioRecSetAndGetGaininAllCond::NewL(_L("MM-MMF-ARCLNT-CHRTZ-I-0029"),_L("BasicOne")));
	AddTestStepL(RTestStepAudioRecSetAndGetGaininAllCond::NewL(_L("MM-MMF-ARCLNT-CHRTZ-I-0030"),_L("BasicOne")));
	
	AddTestStepL(RTestStepAudioRecSetAndGetPositionInAllCond::NewL(_L("MM-MMF-ARCLNT-CHRTZ-I-0031"),_L("Basic")));
	AddTestStepL(RTestStepAudioRecSetAndGetPositionInAllCond::NewL(_L("MM-MMF-ARCLNT-CHRTZ-I-0032"),_L("Basic")));
	AddTestStepL(RTestStepAudioRecSetAndGetPositionInAllCond::NewL(_L("MM-MMF-ARCLNT-CHRTZ-I-0033"),_L("Basic")));
	AddTestStepL(RTestStepAudioRecSetAndGetPositionInAllCond::NewL(_L("MM-MMF-ARCLNT-CHRTZ-I-0034"),_L("Basic")));
	AddTestStepL(RTestStepAudioRecSetAndGetPositionInAllCond::NewL(_L("MM-MMF-ARCLNT-CHRTZ-I-0126"),_L("BasicOne")));
	//RecodTimeAvailable 
	AddTestStepL(RTestStepAudioRecRecodTimeAvailInRecordingAndBeforeOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-I-0036"),_L("Basic")));
	AddTestStepL(RTestStepAudioRecRecodTimeAvailInSetMaxWriteLength::NewL(_L("MM-MMF-ARCLNT-CHRTZ-I-0035"),_L("Basic")));
	AddTestStepL(RTestStepAudioRecRecodTimeAvailInRecordingAndBeforeOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-I-0037"),_L("Basic")));
	
	
	AddTestStepL(RTestStepAudioRecDurationBeforeOpenRecordingPlaying::NewL(_L("MM-MMF-ARCLNT-CHRTZ-I-0038"),_L("Basic")));
	AddTestStepL(RTestStepAudioRecDurationBeforeOpenRecordingPlaying::NewL(_L("MM-MMF-ARCLNT-CHRTZ-I-0039"),_L("Basic")));
	AddTestStepL(RTestStepAudioRecDurationBeforeOpenRecordingPlaying::NewL(_L("MM-MMF-ARCLNT-CHRTZ-I-0040"),_L("Basic")));
	AddTestStepL(RTestStepAudioRecDurationBeforeOpenRecordingPlaying::NewL(_L("MM-MMF-ARCLNT-CHRTZ-I-0041"),_L("Basic")));
	
	//SetPlayWindow in Open state
	AddTestStepL(RTestStepAudioRecSetPlayWindowOpenAndPlaying::NewL(_L("MM-MMF-ARCLNT-CHRTZ-I-0042"),_L("Basic")));
	//SetPlayWindow set starting value Less Than to ZERO in Open state
	AddTestStepL(RTestStepAudioRecSetPlayWindowOpenAndPlaying::NewL(_L("MM-MMF-ARCLNT-CHRTZ-I-0043"),_L("BasicOne")));
	//SetPlayWindow set End value to a Max in Open state
	AddTestStepL(RTestStepAudioRecSetPlayWindowOpenAndPlaying::NewL(_L("MM-MMF-ARCLNT-CHRTZ-I-0044"),_L("BasicTwo")));
	//SetPlayWindow while in Playing state
	AddTestStepL(RTestStepAudioRecSetPlayWindowOpenAndPlaying::NewL(_L("MM-MMF-ARCLNT-CHRTZ-I-0045"),_L("Basic")));
	//SetPlayWindow while before Open,i.e ENotReady state
	AddTestStepL(RTestStepAudioRecSetPlayWindowbeforeOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-I-0046"),_L("Basic")));
	
	//ClearPlayWindow in Opened State
	AddTestStepL(RTestStepAudioRecClearPlayWindow::NewL(_L("MM-MMF-ARCLNT-CHRTZ-I-0047"),_L("Basic")));
	//ClearPlayWindow in Playing State
	AddTestStepL(RTestStepAudioRecClearPlayWindow::NewL(_L("MM-MMF-ARCLNT-CHRTZ-I-0048"),_L("Basic")));
	//ClearPlayWindow in Before Open State
	AddTestStepL(RTestStepAudioRecClearPlayWindowBeforeOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-I-0049"),_L("Basic")));
	
	//SetRepeats in Opened State
	AddTestStepL(RTestStepAudioRecSetRepeatsInOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-I-0050"),_L("Basic")));
	//SetRepeats in Playing State
	AddTestStepL(RTestStepAudioRecSetRepeatsInPlay::NewL(_L("MM-MMF-ARCLNT-CHRTZ-I-0051"),_L("Basic")));
	//SetRepeats in Less than Zero while Playing State
	AddTestStepL(RTestStepAudioRecSetRepeatsInPlay::NewL(_L("MM-MMF-ARCLNT-CHRTZ-I-0052"),_L("BasicOne")));
	//SetRepeats in ENotReady state
	AddTestStepL(RTestStepAudioRecSetRepeatsBeforeOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-I-0130"),_L("Basic")));
	
	//SetMaxWriteLength in Boundary(Less Than ZERO) condition in Open state
	AddTestStepL(RTestStepAudioRecMinSizeInOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-I-0055"),_L("BasicOne")));
	//SetMaxWriteLength in Recording state 
	AddTestStepL(RTestStepAudioRecMaxSizeInRecord::NewL(_L("MM-MMF-ARCLNT-CHRTZ-I-0054"),_L("Basic")));
	//SetMaxWriteLength in Boundary(Maximum) condition in Open state
	AddTestStepL(RTestStepAudioRecMaxSizeInOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-I-0053"),_L("Basic")));
	//SetMaxWriteLength in Before Open state
	AddTestStepL(RTestStepAudioRecMaxSizeInBeforeOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-I-0056"),_L("Basic")));
	
	//SetPriority in Open state( Can not be testes, production code required to change for CMMFMdaAudioRecorderUtility)
	AddTestStepL(RTestStepAudioRecSetPriorityInOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-I-0058"),_L("Basic")));
	
	//SetPlayBackBalance while Before Open an audio file
	AddTestStepL(RTestStepAudioRecSetPlayBackBalBeforeOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-I-0063"),_L("Basic")));
	//SetPlayBackBalance(Boundary Condition setting to Max Value) while in Opened state
	AddTestStepL(RTestStepAudioRecSetPlayBackBalInOpenedMaxValue::NewL(_L("MM-MMF-ARCLNT-CHRTZ-I-0064"),_L("BasicOne")));
	//SetPlayBackBalance(Boundary Condition setting to Minimum Value) while in Opened state
	AddTestStepL(RTestStepAudioRecSetPlayBackBalInOpenedMinValue::NewL(_L("MM-MMF-ARCLNT-CHRTZ-I-0065"),_L("BasicOne")));
	
	//SetRecordBalance while Before Open an audio file
	AddTestStepL(RTestStepAudioRecSetRecordBalBeforeOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-I-0068"),_L("Basic")));
	//SetRecordBalance while in Opened state
	AddTestStepL(RTestStepAudioRecSetRecordBalInOpenMaxValue::NewL(_L("MM-MMF-ARCLNT-CHRTZ-I-0066"),_L("BasicOne")));
	//SetRecordBalance(Boundary Condition setting to Minimum Value) while in Opened state
	AddTestStepL(RTestStepAudioRecSetRecordBalInOpenMaxValue::NewL(_L("MM-MMF-ARCLNT-CHRTZ-I-0070"),_L("BasicOne")));
	//SetRecordBalance(Boundary Condition setting to Minimum Value) while in Opened state

	AddTestStepL(RTestStepAudioRecSetRecordBalInOpenMinValue::NewL(_L("MM-MMF-ARCLNT-CHRTZ-I-0069"),_L("BasicOne")));
	//SetRecordBalance while in Recording state
	AddTestStepL(RTestStepAudioRecSetRecordBalInRecording::NewL(_L("MM-MMF-ARCLNT-CHRTZ-I-0067"),_L("Basic")));
	
	//GetSupportedDestinationDataTypes while in Opened state
	AddTestStepL(RTestStepAudioRecGetSupportDestDataTypeInRecording::NewL(_L("MM-MMF-ARCLNT-CHRTZ-I-0071"),_L("Basic")));
	//GetSupportedDestinationDataTypes while in Recording state
	AddTestStepL(RTestStepAudioRecGetSupportDestDataTypeInRecording::NewL(_L("MM-MMF-ARCLNT-CHRTZ-I-0072"),_L("Basic")));
	//GetSupportedDestinationDataTypes Before Open an Audio file.
	AddTestStepL(RTestStepAudioRecGetSupportDestDataTypeBeforeOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-I-0073"),_L("Basic")));
	
	//SetDestinationDataType/DestinationDataType After Opened an Audio file.
	AddTestStepL(RTestStepAudioRecSetGetDestnDataTypeBeforeOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-I-0074"),_L("Basic")));
	//SetDestinationDataType/DestinationDataType Before Open an Audio file.
	AddTestStepL(RTestStepAudioRecSetGetDestnDataTypeBeforeOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-I-0075"),_L("Basic")));
	//SetDestinationDataType While Recording.
	AddTestStepL(RTestStepAudioRecGetDestnDataTypeInRecording::NewL(_L("MM-MMF-ARCLNT-CHRTZ-I-0076"),_L("Basic")));
	
	//SetDestinationBitRate/DestinationBitRate while Open an Audio file.
	AddTestStepL(RTestStepAudioRecSetGetDestnBitRateBeforeOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-I-0077"),_L("Basic")));
	//SetDestinationBitRate/DestinationBitRate Before Open an Audio file.
	AddTestStepL(RTestStepAudioRecSetGetDestnBitRateBeforeOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-I-0078"),_L("Basic")));
	//DestinationBitRate While in Recording State.
	AddTestStepL(RTestStepAudioRecGetDestnBitRateInRecording::NewL(_L("MM-MMF-ARCLNT-CHRTZ-I-0079"),_L("Basic")));
	
	//GetSupportedBitRatesL() Before Open state.
	AddTestStepL(RTestStepAudioRecGetSupportedBitRateBeforeOpenAndRec::NewL(_L("MM-MMF-ARCLNT-CHRTZ-I-0082"),_L("Basic")));
	//GetSupportedBitRatesL() while after Open an Audio file.
	AddTestStepL(RTestStepAudioRecGetSupportedBitRateBeforeOpenAndRec::NewL(_L("MM-MMF-ARCLNT-CHRTZ-I-0080"),_L("Basic")));
	//GetSupportedBitRatesL() While in Recording an Audio file.
	AddTestStepL(RTestStepAudioRecGetSupportedBitRateBeforeOpenAndRec::NewL(_L("MM-MMF-ARCLNT-CHRTZ-I-0081"),_L("Basic")));
	
	//GetSupportedBitRatesL() While in Playing State.
	AddTestStepL(RTestStepAudioRecSourceBitRateBeforeOpenAndPalying::NewL(_L("MM-MMF-ARCLNT-CHRTZ-I-0084"),_L("Basic")));
	//GetSupportedBitRatesL() While After Open an Audio file.
	AddTestStepL(RTestStepAudioRecSourceBitRateBeforeOpenAndPalying::NewL(_L("MM-MMF-ARCLNT-CHRTZ-I-0083"),_L("Basic")));
	//GetSupportedBitRatesL() Before Open.
	AddTestStepL(RTestStepAudioRecSourceBitRateBeforeOpenAndPalying::NewL(_L("MM-MMF-ARCLNT-CHRTZ-I-0085"),_L("BasicOne")));

	//SetDestinationSampleRateL()/DestinationSampleRateL() While After Open an Audio file.
	AddTestStepL(RTestStepAudioRecSetGetDestnSampleRateBeforeOpenAndRec::NewL(_L("MM-MMF-ARCLNT-CHRTZ-I-0086"),_L("Basic")));
	//SetDestinationSampleRateL()/DestinationSampleRateL() While in Recording State.
	AddTestStepL(RTestStepAudioRecSetGetDestnSampleRateBeforeOpenAndRec::NewL(_L("MM-MMF-ARCLNT-CHRTZ-I-0087"),_L("Basic")));
	//SetDestinationSampleRateL()/DestinationSampleRateL() Before Open.
	AddTestStepL(RTestStepAudioRecSetGetDestnSampleRateBeforeOpenAndRec::NewL(_L("MM-MMF-ARCLNT-CHRTZ-I-0088"),_L("BasicOne")));

	//GetSupportedSampleRatesL() While After Open an Audio file.
	AddTestStepL(RTestStepAudioRecGetSupportedSampleRateBeforeOpenAndRec::NewL(_L("MM-MMF-ARCLNT-CHRTZ-I-0089"),_L("Basic")));
	//GetSupportedSampleRatesL() While in Recording State.
	AddTestStepL(RTestStepAudioRecGetSupportedSampleRateBeforeOpenAndRec::NewL(_L("MM-MMF-ARCLNT-CHRTZ-I-0090"),_L("Basic")));
	//GetSupportedSampleRatesL() Before Open.
	AddTestStepL(RTestStepAudioRecGetSupportedSampleRateBeforeOpenAndRec::NewL(_L("MM-MMF-ARCLNT-CHRTZ-I-0091"),_L("BasicOne")));

	//SetDestinationFormatL()/DestinationFormatL() After Open an Audio file.
	AddTestStepL(RTestStepAudioRecSetGetDestnFormatBeforeOpenAndRec::NewL(_L("MM-MMF-ARCLNT-CHRTZ-I-0092"),_L("Basic")));
	//SetDestinationFormatL()/DestinationFormatL() Before Open.
	AddTestStepL(RTestStepAudioRecSetGetDestnFormatBeforeOpenAndRec::NewL(_L("MM-MMF-ARCLNT-CHRTZ-I-0093"),_L("Basic")));
	//SetDestinationFormatL()/DestinationFormatL() While in Recording State.
	AddTestStepL(RTestStepAudioRecSetGetDestnFormatBeforeOpenAndRec::NewL(_L("MM-MMF-ARCLNT-CHRTZ-I-0094"),_L("BasicOne")));
	
	//SetDestinationNumberOfChannelsL()/DestinationNumberOfChannelsL() While After Open an Audio file.
	AddTestStepL(RTestStepAudioRecSetGetDestnNumberChannelBeforeOpenAndRec::NewL(_L("MM-MMF-ARCLNT-CHRTZ-I-0095"),_L("BasicOne")));
	//SetDestinationNumberOfChannelsL()/DestinationNumberOfChannelsL() While in Recording State.
	AddTestStepL(RTestStepAudioRecSetGetDestnNumberChannelBeforeOpenAndRec::NewL(_L("MM-MMF-ARCLNT-CHRTZ-I-0096"),_L("BasicOne")));
	//SetDestinationNumberOfChannelsL()/DestinationNumberOfChannelsL() Before Open.
	AddTestStepL(RTestStepAudioRecSetGetDestnNumberChannelBeforeOpenAndRec::NewL(_L("MM-MMF-ARCLNT-CHRTZ-I-0097"),_L("Basic")));
	
	//GetSupportedNumberOfChannelsL() While After Open an Audio file.
	AddTestStepL(RTestAudioRecGetSuptedNoOfChanlsB4OpenAndRec::NewL(_L("MM-MMF-ARCLNT-CHRTZ-I-0098"),_L("BasicOne")));
	//GetSupportedNumberOfChannelsL() While in Recording State.
	AddTestStepL(RTestAudioRecGetSuptedNoOfChanlsB4OpenAndRec::NewL(_L("MM-MMF-ARCLNT-CHRTZ-I-0099"),_L("BasicOne")));
	//GetSupportedNumberOfChannelsL() Before Open.
	AddTestStepL(RTestAudioRecGetSuptedNoOfChanlsB4OpenAndRec::NewL(_L("MM-MMF-ARCLNT-CHRTZ-I-0100"),_L("Basic")));
	
	//GetSupportedNumberOfChannelsL() While in Recording And in Playing State.
	AddTestStepL(RTestAudioRecGetNumOfMetaDataEntB4OpenAndRec::NewL(_L("MM-MMF-ARCLNT-CHRTZ-I-0102"),_L("BasicOne")));
	//GetSupportedNumberOfChannelsL() Before Open.
	AddTestStepL(RTestAudioRecGetNumOfMetaDataEntB4OpenAndRec::NewL(_L("MM-MMF-ARCLNT-CHRTZ-I-0103"),_L("Basic")));
	
	//GetMetaDataEntryL() While in Recording And while in Playing State.
	AddTestStepL(RTestAudioRecGetMetaDataEntB4OpenAndPly::NewL(_L("MM-MMF-ARCLNT-CHRTZ-I-0105"),_L("BasicOne")));
	//GetMetaDataEntryL() Before Open.
	AddTestStepL(RTestAudioRecGetMetaDataEntB4OpenAndPly::NewL(_L("MM-MMF-ARCLNT-CHRTZ-I-0106"),_L("Basic")));
	
	//AddMetaDataEntryL() While in Recording State.
	AddTestStepL(RTestAudioRecAddMetaDataEntB4OpenAndRec::NewL(_L("MM-MMF-ARCLNT-CHRTZ-I-0108"),_L("BasicOne")));
	//AddMetaDataEntryL() Before Open.
	AddTestStepL(RTestAudioRecAddMetaDataEntB4OpenAndRec::NewL(_L("MM-MMF-ARCLNT-CHRTZ-I-0109"),_L("Basic")));
	
	//RemoveMetaDataEntry() While in Recording And while in Playing State.
	AddTestStepL(RTestAudioRecRemvMetaDataEntB4OpenAndRec::NewL(_L("MM-MMF-ARCLNT-CHRTZ-I-0111"),_L("BasicOne")));
	//RemoveMetaDataEntry() Before Open.
	AddTestStepL(RTestAudioRecRemvMetaDataEntB4OpenAndRec::NewL(_L("MM-MMF-ARCLNT-CHRTZ-I-0112"),_L("Basic")));
	
	//ReplaceMetaDataEntryL() While in Recording And while in Playing State.
	AddTestStepL(RTestAudioRecRepMetaDataEntB4OpenAndRec::NewL(_L("MM-MMF-ARCLNT-CHRTZ-I-0114"),_L("BasicOne")));
	//ReplaceMetaDataEntryL() Before Open.
	AddTestStepL(RTestAudioRecRepMetaDataEntB4OpenAndRec::NewL(_L("MM-MMF-ARCLNT-CHRTZ-I-0115"),_L("Basic")));
	//WillResumePlay() While in Open State.
	AddTestStepL(RTestAudioRecWillResumePlayInOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-I-0124"),_L("Basic")));
	
	
	
	/*************************************************************************************
	*    						Test Suite For OGG Controller
	**************************************************************************************/
	 
	AddTestStepL(RAudioRecTestStep::NewL(_L("MM-MMF-ARCLNT-CHRTZ-OGG-I-0001"),_L("BasicOgg")));
	
	//Open an audio file in Opened state
	AddTestStepL(RTestStepAudioRecOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-OGG-I-0002"),_L("BasicOgg")));
	
	AddTestStepL(RTestStepAudioRecPlayInPlaying::NewL(_L("MM-MMF-ARCLNT-CHRTZ-OGG-I-0004"),_L("BasicOgg")));
	AddTestStepL(RTestStepAudioRecPlayBeforeOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-OGG-I-0005"),_L("BasicOgg")));
	
	AddTestStepL(RTestStepAudioRecRecordBeforeOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-OGG-I-0008"),_L("BasicOgg")));
	AddTestStepL(RTestStepAudioRecRecordMaxSize::NewL(_L("MM-MMF-ARCLNT-CHRTZ-OGG-I-0009"),_L("BasicOgg")));
	AddTestStepL(RTestStepAudioRecRecordWhileInRecording::NewL(_L("MM-MMF-ARCLNT-CHRTZ-OGG-I-0010"),_L("BasicOgg")));
	
	AddTestStepL(RTestStepAudioRecMaxVolBeforeOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-OGG-I-0012"),_L("BasicOgg")));
	AddTestStepL(RTestStepAudioRecMaxVolBeforeOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-OGG-I-0013"),_L("BasicOgg")));
	AddTestStepL(RTestStepAudioRecMaxVolBeforeOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-OGG-I-0014"),_L("BasicOgg")));
	
	AddTestStepL(RTestStepAudioRecSetGetVolBeforeOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-OGG-I-0017"),_L("BasicOgg")));
	AddTestStepL(RTestStepAudioRecSetVolInOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-OGG-I-0015"),_L("BasicOgg")));
	AddTestStepL(RTestStepAudioRecSetVolInOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-OGG-I-0016"),_L("BasicOgg")));
	AddTestStepL(RTestStepAudioRecSetVolInOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-OGG-I-0018"),_L("BasicOgg")));
	AddTestStepL(RTestStepAudioRecSetVolInOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-OGG-I-0019"),_L("BasicOgg")));
	
	AddTestStepL(RTestStepAudioRecSetVolRampBeforeOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-OGG-I-0021"),_L("BasicOgg")));
	AddTestStepL(RTestStepAudioRecSetVolRampBeforeOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-OGG-I-0022"),_L("BasicOgg")));
	
	AddTestStepL(RTestStepAudioRecMaxGainBeforeOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-OGG-I-0023"),_L("BasicOgg")));
	AddTestStepL(RTestStepAudioRecMaxGainBeforeOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-OGG-I-0024"),_L("BasicOgg")));
	AddTestStepL(RTestStepAudioRecMaxGainBeforeOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-OGG-I-0025"),_L("BasicOgg")));
	
	AddTestStepL(RTestStepAudioRecSetAndGetGaininAllCond::NewL(_L("MM-MMF-ARCLNT-CHRTZ-OGG-I-0026"),_L("BasicOgg")));
	AddTestStepL(RTestStepAudioRecSetAndGetGaininAllCond::NewL(_L("MM-MMF-ARCLNT-CHRTZ-OGG-I-0027"),_L("BasicOgg")));
	AddTestStepL(RTestStepAudioRecSetAndGetGaininAllCond::NewL(_L("MM-MMF-ARCLNT-CHRTZ-OGG-I-0028"),_L("BasicOgg")));
	AddTestStepL(RTestStepAudioRecSetAndGetGaininAllCond::NewL(_L("MM-MMF-ARCLNT-CHRTZ-OGG-I-0029"),_L("BasicOggOne")));
	AddTestStepL(RTestStepAudioRecSetAndGetGaininAllCond::NewL(_L("MM-MMF-ARCLNT-CHRTZ-OGG-I-0030"),_L("BasicOggOne")));
	
	AddTestStepL(RTestStepAudioRecSetAndGetPositionInAllCond::NewL(_L("MM-MMF-ARCLNT-CHRTZ-OGG-I-0031"),_L("BasicOgg")));
	AddTestStepL(RTestStepAudioRecSetAndGetPositionInAllCond::NewL(_L("MM-MMF-ARCLNT-CHRTZ-OGG-I-0032"),_L("BasicOgg")));
	AddTestStepL(RTestStepAudioRecSetAndGetPositionInAllCond::NewL(_L("MM-MMF-ARCLNT-CHRTZ-OGG-I-0033"),_L("BasicOgg")));
	AddTestStepL(RTestStepAudioRecSetAndGetPositionInAllCond::NewL(_L("MM-MMF-ARCLNT-CHRTZ-OGG-I-0034"),_L("BasicOgg")));
	AddTestStepL(RTestStepAudioRecSetAndGetPositionInAllCond::NewL(_L("MM-MMF-ARCLNT-CHRTZ-OGG-I-0126"),_L("BasicOggOne")));
	//RecodTimeAvailable 
	AddTestStepL(RTestStepAudioRecRecodTimeAvailInRecordingAndBeforeOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-OGG-I-0036"),_L("BasicOgg")));
	AddTestStepL(RTestStepAudioRecRecodTimeAvailInSetMaxWriteLength::NewL(_L("MM-MMF-ARCLNT-CHRTZ-OGG-I-0035"),_L("BasicOgg")));
	AddTestStepL(RTestStepAudioRecRecodTimeAvailInRecordingAndBeforeOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-OGG-I-0037"),_L("BasicOgg")));
	
	
	AddTestStepL(RTestStepAudioRecDurationBeforeOpenRecordingPlaying::NewL(_L("MM-MMF-ARCLNT-CHRTZ-OGG-I-0038"),_L("BasicOgg")));
	AddTestStepL(RTestStepAudioRecDurationBeforeOpenRecordingPlaying::NewL(_L("MM-MMF-ARCLNT-CHRTZ-OGG-I-0039"),_L("BasicOgg")));
	AddTestStepL(RTestStepAudioRecDurationBeforeOpenRecordingPlaying::NewL(_L("MM-MMF-ARCLNT-CHRTZ-OGG-I-0040"),_L("BasicOgg")));
	AddTestStepL(RTestStepAudioRecDurationBeforeOpenRecordingPlaying::NewL(_L("MM-MMF-ARCLNT-CHRTZ-OGG-I-0041"),_L("BasicOgg")));
	
	//SetPlayWindow in Open state
	AddTestStepL(RTestStepAudioRecSetPlayWindowOpenAndPlaying::NewL(_L("MM-MMF-ARCLNT-CHRTZ-OGG-I-0042"),_L("BasicOgg")));
	//SetPlayWindow set starting value Less Than to ZERO in Open state
	AddTestStepL(RTestStepAudioRecSetPlayWindowOpenAndPlaying::NewL(_L("MM-MMF-ARCLNT-CHRTZ-OGG-I-0043"),_L("BasicOggOne")));
	//SetPlayWindow set End value to a Max in Open state
	AddTestStepL(RTestStepAudioRecSetPlayWindowOpenAndPlaying::NewL(_L("MM-MMF-ARCLNT-CHRTZ-OGG-I-0044"),_L("BasicOggTwo")));
	//SetPlayWindow while in Playing state
	AddTestStepL(RTestStepAudioRecSetPlayWindowOpenAndPlaying::NewL(_L("MM-MMF-ARCLNT-CHRTZ-OGG-I-0045"),_L("BasicOgg")));
	//SetPlayWindow while before Open,i.e ENotReady state
	AddTestStepL(RTestStepAudioRecSetPlayWindowbeforeOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-OGG-I-0046"),_L("BasicOgg")));
	
	//ClearPlayWindow in Opened State
	AddTestStepL(RTestStepAudioRecClearPlayWindow::NewL(_L("MM-MMF-ARCLNT-CHRTZ-OGG-I-0047"),_L("BasicOgg")));
	//ClearPlayWindow in Playing State
	AddTestStepL(RTestStepAudioRecClearPlayWindow::NewL(_L("MM-MMF-ARCLNT-CHRTZ-OGG-I-0048"),_L("BasicOgg")));
	//ClearPlayWindow in Before Open State
	AddTestStepL(RTestStepAudioRecClearPlayWindowBeforeOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-OGG-I-0049"),_L("BasicOgg")));
	
	//SetRepeats in Opened State
	AddTestStepL(RTestStepAudioRecSetRepeatsInOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-OGG-I-0050"),_L("BasicOgg")));
	//SetRepeats in Playing State
	AddTestStepL(RTestStepAudioRecSetRepeatsInPlay::NewL(_L("MM-MMF-ARCLNT-CHRTZ-OGG-I-0051"),_L("BasicOgg")));
	//SetRepeats in Less than Zero while Playing State
	AddTestStepL(RTestStepAudioRecSetRepeatsInPlay::NewL(_L("MM-MMF-ARCLNT-CHRTZ-OGG-I-0052"),_L("BasicOggOne")));
	//SetRepeats in ENotReady state
	AddTestStepL(RTestStepAudioRecSetRepeatsBeforeOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-OGG-I-0130"),_L("BasicOgg")));
	
	//SetMaxWriteLength in Boundary(Less Than ZERO) condition in Open state
	AddTestStepL(RTestStepAudioRecMinSizeInOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-OGG-I-0055"),_L("BasicOggOne")));
	//SetMaxWriteLength in Recording state 
	AddTestStepL(RTestStepAudioRecMaxSizeInRecord::NewL(_L("MM-MMF-ARCLNT-CHRTZ-OGG-I-0054"),_L("BasicOgg")));
	//SetMaxWriteLength in Boundary(Maximum) condition in Open state
	AddTestStepL(RTestStepAudioRecMaxSizeInOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-OGG-I-0053"),_L("BasicOgg")));
	//SetMaxWriteLength in Before Open state
	AddTestStepL(RTestStepAudioRecMaxSizeInBeforeOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-OGG-I-0056"),_L("BasicOgg")));
	
	//SetPriority in Open state( Can not be testes, production code required to change for CMMFMdaAudioRecorderUtility)
	AddTestStepL(RTestStepAudioRecSetPriorityInOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-OGG-I-0058"),_L("BasicOgg")));
	
	//SetPlayBackBalance while Before Open an audio file
	AddTestStepL(RTestStepAudioRecSetPlayBackBalBeforeOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-OGG-I-0063"),_L("BasicOgg")));
	//SetPlayBackBalance(Boundary Condition setting to Max Value) while in Opened state
	AddTestStepL(RTestStepAudioRecSetPlayBackBalInOpenedMaxValue::NewL(_L("MM-MMF-ARCLNT-CHRTZ-OGG-I-0064"),_L("BasicOggOne")));
	//SetPlayBackBalance(Boundary Condition setting to Minimum Value) while in Opened state
	AddTestStepL(RTestStepAudioRecSetPlayBackBalInOpenedMinValue::NewL(_L("MM-MMF-ARCLNT-CHRTZ-OGG-I-0065"),_L("BasicOggOne")));
	
	//SetRecordBalance while Before Open an audio file
	AddTestStepL(RTestStepAudioRecSetRecordBalBeforeOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-OGG-I-0068"),_L("BasicOgg")));
	//SetRecordBalance while in Opened state
	AddTestStepL(RTestStepAudioRecSetRecordBalInOpenMaxValue::NewL(_L("MM-MMF-ARCLNT-CHRTZ-OGG-I-0066"),_L("BasicOggOne")));
	//SetRecordBalance(Boundary Condition setting to Minimum Value) while in Opened state
	AddTestStepL(RTestStepAudioRecSetRecordBalInOpenMaxValue::NewL(_L("MM-MMF-ARCLNT-CHRTZ-OGG-I-0070"),_L("BasicOggOne")));
	//SetRecordBalance(Boundary Condition setting to Minimum Value) while in Opened state

	AddTestStepL(RTestStepAudioRecSetRecordBalInOpenMinValue::NewL(_L("MM-MMF-ARCLNT-CHRTZ-OGG-I-0069"),_L("BasicOggOne")));
	//SetRecordBalance while in Recording state
	AddTestStepL(RTestStepAudioRecSetRecordBalInRecording::NewL(_L("MM-MMF-ARCLNT-CHRTZ-OGG-I-0067"),_L("BasicOgg")));
	
	//GetSupportedDestinationDataTypes while in Opened state
	AddTestStepL(RTestStepAudioRecGetSupportDestDataTypeInRecording::NewL(_L("MM-MMF-ARCLNT-CHRTZ-OGG-I-0071"),_L("BasicOgg")));
	//GetSupportedDestinationDataTypes while in Recording state
	AddTestStepL(RTestStepAudioRecGetSupportDestDataTypeInRecording::NewL(_L("MM-MMF-ARCLNT-CHRTZ-OGG-I-0072"),_L("BasicOgg")));
	//GetSupportedDestinationDataTypes Before Open an Audio file.
	AddTestStepL(RTestStepAudioRecGetSupportDestDataTypeBeforeOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-OGG-I-0073"),_L("BasicOgg")));
	
	//SetDestinationDataType/DestinationDataType After Opened an Audio file.
	AddTestStepL(RTestStepAudioRecSetGetDestnDataTypeBeforeOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-OGG-I-0074"),_L("BasicOgg")));
	//SetDestinationDataType/DestinationDataType Before Open an Audio file.
	AddTestStepL(RTestStepAudioRecSetGetDestnDataTypeBeforeOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-OGG-I-0075"),_L("BasicOgg")));
	//SetDestinationDataType While Recording.
	AddTestStepL(RTestStepAudioRecGetDestnDataTypeInRecording::NewL(_L("MM-MMF-ARCLNT-CHRTZ-OGG-I-0076"),_L("BasicOgg")));
	
	//SetDestinationBitRate/DestinationBitRate while Open an Audio file.
	AddTestStepL(RTestStepAudioRecSetGetDestnBitRateBeforeOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-OGG-I-0077"),_L("BasicOgg")));
	//SetDestinationBitRate/DestinationBitRate Before Open an Audio file.
	AddTestStepL(RTestStepAudioRecSetGetDestnBitRateBeforeOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-OGG-I-0078"),_L("BasicOgg")));
	//DestinationBitRate While in Recording State.
	AddTestStepL(RTestStepAudioRecGetDestnBitRateInRecording::NewL(_L("MM-MMF-ARCLNT-CHRTZ-OGG-I-0079"),_L("BasicOgg")));
	
	//GetSupportedBitRatesL() Before Open state.
	AddTestStepL(RTestStepAudioRecGetSupportedBitRateBeforeOpenAndRec::NewL(_L("MM-MMF-ARCLNT-CHRTZ-OGG-I-0082"),_L("BasicOgg")));
	//GetSupportedBitRatesL() while after Open an Audio file.
	AddTestStepL(RTestStepAudioRecGetSupportedBitRateBeforeOpenAndRec::NewL(_L("MM-MMF-ARCLNT-CHRTZ-OGG-I-0080"),_L("BasicOgg")));
	//GetSupportedBitRatesL() While in Recording an Audio file.
	AddTestStepL(RTestStepAudioRecGetSupportedBitRateBeforeOpenAndRec::NewL(_L("MM-MMF-ARCLNT-CHRTZ-OGG-I-0081"),_L("BasicOgg")));
	
	//GetSupportedBitRatesL() While in Playing State.
	AddTestStepL(RTestStepAudioRecSourceBitRateBeforeOpenAndPalying::NewL(_L("MM-MMF-ARCLNT-CHRTZ-OGG-I-0084"),_L("BasicOgg")));
	//GetSupportedBitRatesL() While After Open an Audio file.
	AddTestStepL(RTestStepAudioRecSourceBitRateBeforeOpenAndPalying::NewL(_L("MM-MMF-ARCLNT-CHRTZ-OGG-I-0083"),_L("BasicOgg")));
	//GetSupportedBitRatesL() Before Open.
	AddTestStepL(RTestStepAudioRecSourceBitRateBeforeOpenAndPalying::NewL(_L("MM-MMF-ARCLNT-CHRTZ-OGG-I-0085"),_L("BasicOggOne")));

	//SetDestinationSampleRateL()/DestinationSampleRateL() While After Open an Audio file.
	AddTestStepL(RTestStepAudioRecSetGetDestnSampleRateBeforeOpenAndRec::NewL(_L("MM-MMF-ARCLNT-CHRTZ-OGG-I-0086"),_L("BasicOgg")));
	//SetDestinationSampleRateL()/DestinationSampleRateL() While in Recording State.
	AddTestStepL(RTestStepAudioRecSetGetDestnSampleRateBeforeOpenAndRec::NewL(_L("MM-MMF-ARCLNT-CHRTZ-OGG-I-0087"),_L("BasicOgg")));
	//SetDestinationSampleRateL()/DestinationSampleRateL() Before Open.
	AddTestStepL(RTestStepAudioRecSetGetDestnSampleRateBeforeOpenAndRec::NewL(_L("MM-MMF-ARCLNT-CHRTZ-OGG-I-0088"),_L("BasicOggOne")));

	//GetSupportedSampleRatesL() While After Open an Audio file.
	AddTestStepL(RTestStepAudioRecGetSupportedSampleRateBeforeOpenAndRec::NewL(_L("MM-MMF-ARCLNT-CHRTZ-OGG-I-0089"),_L("BasicOgg")));
	//GetSupportedSampleRatesL() While in Recording State.
	AddTestStepL(RTestStepAudioRecGetSupportedSampleRateBeforeOpenAndRec::NewL(_L("MM-MMF-ARCLNT-CHRTZ-OGG-I-0090"),_L("BasicOgg")));
	//GetSupportedSampleRatesL() Before Open.
	AddTestStepL(RTestStepAudioRecGetSupportedSampleRateBeforeOpenAndRec::NewL(_L("MM-MMF-ARCLNT-CHRTZ-OGG-I-0091"),_L("BasicOggOne")));

	//SetDestinationFormatL()/DestinationFormatL() After Open an Audio file.
	AddTestStepL(RTestStepAudioRecSetGetDestnFormatBeforeOpenAndRec::NewL(_L("MM-MMF-ARCLNT-CHRTZ-OGG-I-0092"),_L("BasicOgg")));
	//SetDestinationFormatL()/DestinationFormatL() Before Open.
	AddTestStepL(RTestStepAudioRecSetGetDestnFormatBeforeOpenAndRec::NewL(_L("MM-MMF-ARCLNT-CHRTZ-OGG-I-0093"),_L("BasicOgg")));
	//SetDestinationFormatL()/DestinationFormatL() While in Recording State.
	AddTestStepL(RTestStepAudioRecSetGetDestnFormatBeforeOpenAndRec::NewL(_L("MM-MMF-ARCLNT-CHRTZ-OGG-I-0094"),_L("BasicOggOne")));
	
	//SetDestinationNumberOfChannelsL()/DestinationNumberOfChannelsL() While After Open an Audio file.
	AddTestStepL(RTestStepAudioRecSetGetDestnNumberChannelBeforeOpenAndRec::NewL(_L("MM-MMF-ARCLNT-CHRTZ-OGG-I-0095"),_L("BasicOggOne")));
	//SetDestinationNumberOfChannelsL()/DestinationNumberOfChannelsL() While in Recording State.
	AddTestStepL(RTestStepAudioRecSetGetDestnNumberChannelBeforeOpenAndRec::NewL(_L("MM-MMF-ARCLNT-CHRTZ-OGG-I-0096"),_L("BasicOggOne")));
	//SetDestinationNumberOfChannelsL()/DestinationNumberOfChannelsL() Before Open.
	AddTestStepL(RTestStepAudioRecSetGetDestnNumberChannelBeforeOpenAndRec::NewL(_L("MM-MMF-ARCLNT-CHRTZ-OGG-I-0097"),_L("BasicOgg")));
	
	//GetSupportedNumberOfChannelsL() While After Open an Audio file.
	AddTestStepL(RTestAudioRecGetSuptedNoOfChanlsB4OpenAndRec::NewL(_L("MM-MMF-ARCLNT-CHRTZ-OGG-I-0098"),_L("BasicOggOne")));
	//GetSupportedNumberOfChannelsL() While in Recording State.
	AddTestStepL(RTestAudioRecGetSuptedNoOfChanlsB4OpenAndRec::NewL(_L("MM-MMF-ARCLNT-CHRTZ-OGG-I-0099"),_L("BasicOggOne")));
	//GetSupportedNumberOfChannelsL() Before Open.
	AddTestStepL(RTestAudioRecGetSuptedNoOfChanlsB4OpenAndRec::NewL(_L("MM-MMF-ARCLNT-CHRTZ-OGG-I-0100"),_L("BasicOgg")));
	
	//GetSupportedNumberOfChannelsL() While in Recording And in Playing State.
	AddTestStepL(RTestAudioRecGetNumOfMetaDataEntB4OpenAndRec::NewL(_L("MM-MMF-ARCLNT-CHRTZ-OGG-I-0102"),_L("BasicOggOne")));
	//GetSupportedNumberOfChannelsL() Before Open.
	AddTestStepL(RTestAudioRecGetNumOfMetaDataEntB4OpenAndRec::NewL(_L("MM-MMF-ARCLNT-CHRTZ-OGG-I-0103"),_L("BasicOgg")));
	
	//GetMetaDataEntryL() While in Recording And while in Playing State.
	AddTestStepL(RTestAudioRecGetMetaDataEntB4OpenAndPly::NewL(_L("MM-MMF-ARCLNT-CHRTZ-OGG-I-0105"),_L("BasicOggOne")));
	//GetMetaDataEntryL() Before Open.
	AddTestStepL(RTestAudioRecGetMetaDataEntB4OpenAndPly::NewL(_L("MM-MMF-ARCLNT-CHRTZ-OGG-I-0106"),_L("BasicOgg")));
	
	//AddMetaDataEntryL() While in Recording State.
	AddTestStepL(RTestAudioRecAddMetaDataEntB4OpenAndRec::NewL(_L("MM-MMF-ARCLNT-CHRTZ-OGG-I-0108"),_L("BasicOggOne")));
	//AddMetaDataEntryL() Before Open.
	AddTestStepL(RTestAudioRecAddMetaDataEntB4OpenAndRec::NewL(_L("MM-MMF-ARCLNT-CHRTZ-OGG-I-0109"),_L("BasicOgg")));
	
	//RemoveMetaDataEntry() While in Recording And while in Playing State.
	AddTestStepL(RTestAudioRecRemvMetaDataEntB4OpenAndRec::NewL(_L("MM-MMF-ARCLNT-CHRTZ-OGG-I-0111"),_L("BasicOggOne")));
	//RemoveMetaDataEntry() Before Open.
	AddTestStepL(RTestAudioRecRemvMetaDataEntB4OpenAndRec::NewL(_L("MM-MMF-ARCLNT-CHRTZ-OGG-I-0112"),_L("BasicOgg")));
	
	//ReplaceMetaDataEntryL() While in Recording And while in Playing State.
	AddTestStepL(RTestAudioRecRepMetaDataEntB4OpenAndRec::NewL(_L("MM-MMF-ARCLNT-CHRTZ-OGG-I-0114"),_L("BasicOggOne")));
	//ReplaceMetaDataEntryL() Before Open.
	AddTestStepL(RTestAudioRecRepMetaDataEntB4OpenAndRec::NewL(_L("MM-MMF-ARCLNT-CHRTZ-OGG-I-0115"),_L("BasicOgg")));
	//WillResumePlay() While in Open State.
	AddTestStepL(RTestAudioRecWillResumePlayInOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-OGG-I-0124"),_L("BasicOgg")));

#ifdef SYMBIAN_MULTIMEDIA_A3FDEVSOUND	
	_LIT(KBasicAmr, "BasicAmr");
	_LIT(KBasicAac, "BasicAac");

	/*************************************************************************************
	*    						Test Suite For AMR Controller
	**************************************************************************************/
	 
	AddTestStepL(RAudioRecTestStep::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AMR-I-0001"),KBasicAmr));
	
	//Open an audio file in Opened state
	AddTestStepL(RTestStepAudioRecOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AMR-I-0002"),KBasicAmr));
	
	AddTestStepL(RTestStepAudioRecPlayInPlaying::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AMR-I-0004"),KBasicAmr));
	AddTestStepL(RTestStepAudioRecPlayBeforeOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AMR-I-0005"),KBasicAmr));
	
	AddTestStepL(RTestStepAudioRecRecordBeforeOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AMR-I-0008"),KBasicAmr));
	AddTestStepL(RTestStepAudioRecRecordMaxSize::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AMR-I-0009"),KBasicAmr));
	AddTestStepL(RTestStepAudioRecRecordWhileInRecording::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AMR-I-0010"),KBasicAmr));
	
	AddTestStepL(RTestStepAudioRecMaxVolBeforeOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AMR-I-0012"),KBasicAmr));
	AddTestStepL(RTestStepAudioRecMaxVolBeforeOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AMR-I-0013"),KBasicAmr));
	AddTestStepL(RTestStepAudioRecMaxVolBeforeOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AMR-I-0014"),KBasicAmr));
	
	AddTestStepL(RTestStepAudioRecSetGetVolBeforeOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AMR-I-0017"),KBasicAmr));
	AddTestStepL(RTestStepAudioRecSetVolInOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AMR-I-0015"),KBasicAmr));
	AddTestStepL(RTestStepAudioRecSetVolInOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AMR-I-0016"),KBasicAmr));
	AddTestStepL(RTestStepAudioRecSetVolInOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AMR-I-0018"),KBasicAmr));
	AddTestStepL(RTestStepAudioRecSetVolInOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AMR-I-0019"),KBasicAmr));
	
	AddTestStepL(RTestStepAudioRecSetVolRampBeforeOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AMR-I-0021"),KBasicAmr));
	AddTestStepL(RTestStepAudioRecSetVolRampBeforeOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AMR-I-0022"),KBasicAmr));
	
	AddTestStepL(RTestStepAudioRecMaxGainBeforeOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AMR-I-0023"),KBasicAmr));
	AddTestStepL(RTestStepAudioRecMaxGainBeforeOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AMR-I-0024"),KBasicAmr));
	AddTestStepL(RTestStepAudioRecMaxGainBeforeOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AMR-I-0025"),KBasicAmr));
	
	AddTestStepL(RTestStepAudioRecSetAndGetGaininAllCond::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AMR-I-0026"),KBasicAmr));
	AddTestStepL(RTestStepAudioRecSetAndGetGaininAllCond::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AMR-I-0027"),KBasicAmr));
	AddTestStepL(RTestStepAudioRecSetAndGetGaininAllCond::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AMR-I-0028"),KBasicAmr));
	AddTestStepL(RTestStepAudioRecSetAndGetGaininAllCond::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AMR-I-0029"),_L("BasicAmrOne")));
	AddTestStepL(RTestStepAudioRecSetAndGetGaininAllCond::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AMR-I-0030"),_L("BasicAmrOne")));
	
	AddTestStepL(RTestStepAudioRecSetAndGetPositionInAllCond::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AMR-I-0031"),KBasicAmr));
	AddTestStepL(RTestStepAudioRecSetAndGetPositionInAllCond::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AMR-I-0032"),KBasicAmr));
	AddTestStepL(RTestStepAudioRecSetAndGetPositionInAllCond::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AMR-I-0033"),KBasicAmr));
	AddTestStepL(RTestStepAudioRecSetAndGetPositionInAllCond::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AMR-I-0034"),KBasicAmr));
	AddTestStepL(RTestStepAudioRecSetAndGetPositionInAllCond::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AMR-I-0126"),_L("BasicAmrOne")));
	//RecodTimeAvailable 
	AddTestStepL(RTestStepAudioRecRecodTimeAvailInRecordingAndBeforeOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AMR-I-0036"),KBasicAmr));
	AddTestStepL(RTestStepAudioRecRecodTimeAvailInSetMaxWriteLength::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AMR-I-0035"),KBasicAmr));
	AddTestStepL(RTestStepAudioRecRecodTimeAvailInRecordingAndBeforeOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AMR-I-0037"),KBasicAmr));
	
	
	AddTestStepL(RTestStepAudioRecDurationBeforeOpenRecordingPlaying::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AMR-I-0038"),KBasicAmr));
	AddTestStepL(RTestStepAudioRecDurationBeforeOpenRecordingPlaying::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AMR-I-0039"),KBasicAmr));
	AddTestStepL(RTestStepAudioRecDurationBeforeOpenRecordingPlaying::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AMR-I-0040"),KBasicAmr));
	AddTestStepL(RTestStepAudioRecDurationBeforeOpenRecordingPlaying::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AMR-I-0041"),KBasicAmr));
	
	//SetPlayWindow in Open state
	AddTestStepL(RTestStepAudioRecSetPlayWindowOpenAndPlaying::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AMR-I-0042"),KBasicAmr));
	//SetPlayWindow set starting value Less Than to ZERO in Open state
	AddTestStepL(RTestStepAudioRecSetPlayWindowOpenAndPlaying::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AMR-I-0043"),_L("BasicAmrOne")));
	//SetPlayWindow set End value to a Max in Open state
	AddTestStepL(RTestStepAudioRecSetPlayWindowOpenAndPlaying::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AMR-I-0044"),_L("BasicAmrTwo")));
	//SetPlayWindow while in Playing state
	AddTestStepL(RTestStepAudioRecSetPlayWindowOpenAndPlaying::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AMR-I-0045"),KBasicAmr));
	//SetPlayWindow while before Open,i.e ENotReady state
	AddTestStepL(RTestStepAudioRecSetPlayWindowbeforeOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AMR-I-0046"),KBasicAmr));
	
	//ClearPlayWindow in Opened State
	AddTestStepL(RTestStepAudioRecClearPlayWindow::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AMR-I-0047"),KBasicAmr));
	//ClearPlayWindow in Playing State
	AddTestStepL(RTestStepAudioRecClearPlayWindow::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AMR-I-0048"),KBasicAmr));
	//ClearPlayWindow in Before Open State
	AddTestStepL(RTestStepAudioRecClearPlayWindowBeforeOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AMR-I-0049"),KBasicAmr));
	
	//SetRepeats in Opened State
	AddTestStepL(RTestStepAudioRecSetRepeatsInOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AMR-I-0050"),KBasicAmr));
	//SetRepeats in Playing State
	AddTestStepL(RTestStepAudioRecSetRepeatsInPlay::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AMR-I-0051"),KBasicAmr));
	//SetRepeats in Less than Zero while Playing State
	AddTestStepL(RTestStepAudioRecSetRepeatsInPlay::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AMR-I-0052"),_L("BasicAmrOne")));
	//SetRepeats in ENotReady state
	AddTestStepL(RTestStepAudioRecSetRepeatsBeforeOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AMR-I-0130"),KBasicAmr));
	
	//SetMaxWriteLength in Boundary(Less Than ZERO) condition in Open state
	AddTestStepL(RTestStepAudioRecMinSizeInOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AMR-I-0055"),_L("BasicAmrOne")));
	//SetMaxWriteLength in Recording state 
	AddTestStepL(RTestStepAudioRecMaxSizeInRecord::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AMR-I-0054"),KBasicAmr));
	//SetMaxWriteLength in Boundary(Maximum) condition in Open state
	AddTestStepL(RTestStepAudioRecMaxSizeInOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AMR-I-0053"),KBasicAmr));
	//SetMaxWriteLength in Before Open state
	AddTestStepL(RTestStepAudioRecMaxSizeInBeforeOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AMR-I-0056"),KBasicAmr));
	
	//SetPriority in Open state( Can not be testes, production code required to change for CMMFMdaAudioRecorderUtility)
	AddTestStepL(RTestStepAudioRecSetPriorityInOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AMR-I-0058"),KBasicAmr));
	
	//SetPlayBackBalance while Before Open an audio file
	AddTestStepL(RTestStepAudioRecSetPlayBackBalBeforeOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AMR-I-0063"),KBasicAmr));
	//SetPlayBackBalance(Boundary Condition setting to Max Value) while in Opened state
	AddTestStepL(RTestStepAudioRecSetPlayBackBalInOpenedMaxValue::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AMR-I-0064"),_L("BasicAmrOne")));
	//SetPlayBackBalance(Boundary Condition setting to Minimum Value) while in Opened state
	AddTestStepL(RTestStepAudioRecSetPlayBackBalInOpenedMinValue::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AMR-I-0065"),_L("BasicAmrOne")));
	
	//SetRecordBalance while Before Open an audio file
	AddTestStepL(RTestStepAudioRecSetRecordBalBeforeOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AMR-I-0068"),KBasicAmr));
	//SetRecordBalance while in Opened state
	AddTestStepL(RTestStepAudioRecSetRecordBalInOpenMaxValue::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AMR-I-0066"),_L("BasicAmrOne")));
	//SetRecordBalance(Boundary Condition setting to Minimum Value) while in Opened state
	AddTestStepL(RTestStepAudioRecSetRecordBalInOpenMaxValue::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AMR-I-0070"),_L("BasicAmrOne")));
	//SetRecordBalance(Boundary Condition setting to Minimum Value) while in Opened state

	AddTestStepL(RTestStepAudioRecSetRecordBalInOpenMinValue::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AMR-I-0069"),_L("BasicAmrOne")));
	//SetRecordBalance while in Recording state
	AddTestStepL(RTestStepAudioRecSetRecordBalInRecording::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AMR-I-0067"),KBasicAmr));
	
	//GetSupportedDestinationDataTypes while in Opened state
	AddTestStepL(RTestStepAudioRecGetSupportDestDataTypeInRecording::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AMR-I-0071"),KBasicAmr));
	//GetSupportedDestinationDataTypes while in Recording state
	AddTestStepL(RTestStepAudioRecGetSupportDestDataTypeInRecording::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AMR-I-0072"),KBasicAmr));
	//GetSupportedDestinationDataTypes Before Open an Audio file.
	AddTestStepL(RTestStepAudioRecGetSupportDestDataTypeBeforeOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AMR-I-0073"),KBasicAmr));
	
	//SetDestinationDataType/DestinationDataType After Opened an Audio file.
	AddTestStepL(RTestStepAudioRecSetGetDestnDataTypeBeforeOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AMR-I-0074"),KBasicAmr));
	//SetDestinationDataType/DestinationDataType Before Open an Audio file.
	AddTestStepL(RTestStepAudioRecSetGetDestnDataTypeBeforeOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AMR-I-0075"),KBasicAmr));
	//SetDestinationDataType While Recording.
	AddTestStepL(RTestStepAudioRecGetDestnDataTypeInRecording::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AMR-I-0076"),KBasicAmr));
	
	//SetDestinationBitRate/DestinationBitRate while Open an Audio file.
	AddTestStepL(RTestStepAudioRecSetGetDestnBitRateBeforeOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AMR-I-0077"),KBasicAmr));
	//SetDestinationBitRate/DestinationBitRate Before Open an Audio file.
	AddTestStepL(RTestStepAudioRecSetGetDestnBitRateBeforeOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AMR-I-0078"),KBasicAmr));
	//DestinationBitRate While in Recording State.
	AddTestStepL(RTestStepAudioRecGetDestnBitRateInRecording::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AMR-I-0079"),KBasicAmr));
	
	//GetSupportedBitRatesL() Before Open state.
	AddTestStepL(RTestStepAudioRecGetSupportedBitRateBeforeOpenAndRec::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AMR-I-0082"),KBasicAmr));
	//GetSupportedBitRatesL() while after Open an Audio file.
	AddTestStepL(RTestStepAudioRecGetSupportedBitRateBeforeOpenAndRec::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AMR-I-0080"),KBasicAmr));
	//GetSupportedBitRatesL() While in Recording an Audio file.
	AddTestStepL(RTestStepAudioRecGetSupportedBitRateBeforeOpenAndRec::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AMR-I-0081"),KBasicAmr));
	
	//GetSupportedBitRatesL() While in Playing State.
	AddTestStepL(RTestStepAudioRecSourceBitRateBeforeOpenAndPalying::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AMR-I-0084"),KBasicAmr));
	//GetSupportedBitRatesL() While After Open an Audio file.
	AddTestStepL(RTestStepAudioRecSourceBitRateBeforeOpenAndPalying::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AMR-I-0083"),KBasicAmr));
	//GetSupportedBitRatesL() Before Open.
	AddTestStepL(RTestStepAudioRecSourceBitRateBeforeOpenAndPalying::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AMR-I-0085"),_L("BasicAmrOne")));

	//SetDestinationSampleRateL()/DestinationSampleRateL() While After Open an Audio file.
	AddTestStepL(RTestStepAudioRecSetGetDestnSampleRateBeforeOpenAndRec::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AMR-I-0086"),KBasicAmr));
	//SetDestinationSampleRateL()/DestinationSampleRateL() While in Recording State.
	AddTestStepL(RTestStepAudioRecSetGetDestnSampleRateBeforeOpenAndRec::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AMR-I-0087"),KBasicAmr));
	//SetDestinationSampleRateL()/DestinationSampleRateL() Before Open.
	AddTestStepL(RTestStepAudioRecSetGetDestnSampleRateBeforeOpenAndRec::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AMR-I-0088"),_L("BasicAmrOne")));

	//GetSupportedSampleRatesL() While After Open an Audio file.
	AddTestStepL(RTestStepAudioRecGetSupportedSampleRateBeforeOpenAndRec::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AMR-I-0089"),KBasicAmr));
	//GetSupportedSampleRatesL() While in Recording State.
	AddTestStepL(RTestStepAudioRecGetSupportedSampleRateBeforeOpenAndRec::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AMR-I-0090"),KBasicAmr));
	//GetSupportedSampleRatesL() Before Open.
	AddTestStepL(RTestStepAudioRecGetSupportedSampleRateBeforeOpenAndRec::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AMR-I-0091"),_L("BasicAmrOne")));

	//SetDestinationFormatL()/DestinationFormatL() After Open an Audio file.
	AddTestStepL(RTestStepAudioRecSetGetDestnFormatBeforeOpenAndRec::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AMR-I-0092"),KBasicAmr));
	//SetDestinationFormatL()/DestinationFormatL() Before Open.
	AddTestStepL(RTestStepAudioRecSetGetDestnFormatBeforeOpenAndRec::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AMR-I-0093"),KBasicAmr));
	//SetDestinationFormatL()/DestinationFormatL() While in Recording State.
	AddTestStepL(RTestStepAudioRecSetGetDestnFormatBeforeOpenAndRec::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AMR-I-0094"),_L("BasicAmrOne")));
	
	//SetDestinationNumberOfChannelsL()/DestinationNumberOfChannelsL() While After Open an Audio file.
	AddTestStepL(RTestStepAudioRecSetGetDestnNumberChannelBeforeOpenAndRec::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AMR-I-0095"),_L("BasicAmrOne")));
	//SetDestinationNumberOfChannelsL()/DestinationNumberOfChannelsL() While in Recording State.
	AddTestStepL(RTestStepAudioRecSetGetDestnNumberChannelBeforeOpenAndRec::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AMR-I-0096"),_L("BasicAmrOne")));
	//SetDestinationNumberOfChannelsL()/DestinationNumberOfChannelsL() Before Open.
	AddTestStepL(RTestStepAudioRecSetGetDestnNumberChannelBeforeOpenAndRec::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AMR-I-0097"),KBasicAmr));
	
	//GetSupportedNumberOfChannelsL() While After Open an Audio file.
	AddTestStepL(RTestAudioRecGetSuptedNoOfChanlsB4OpenAndRec::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AMR-I-0098"),_L("BasicAmrOne")));
	//GetSupportedNumberOfChannelsL() While in Recording State.
	AddTestStepL(RTestAudioRecGetSuptedNoOfChanlsB4OpenAndRec::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AMR-I-0099"),_L("BasicAmrOne")));
	//GetSupportedNumberOfChannelsL() Before Open.
	AddTestStepL(RTestAudioRecGetSuptedNoOfChanlsB4OpenAndRec::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AMR-I-0100"),KBasicAmr));
	
	//GetSupportedNumberOfChannelsL() While in Recording And in Playing State.
	AddTestStepL(RTestAudioRecGetNumOfMetaDataEntB4OpenAndRec::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AMR-I-0102"),_L("BasicAmrOne")));
	//GetSupportedNumberOfChannelsL() Before Open.
	AddTestStepL(RTestAudioRecGetNumOfMetaDataEntB4OpenAndRec::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AMR-I-0103"),KBasicAmr));
	
	//GetMetaDataEntryL() While in Recording And while in Playing State.
	AddTestStepL(RTestAudioRecGetMetaDataEntB4OpenAndPly::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AMR-I-0105"),_L("BasicAmrOne")));
	//GetMetaDataEntryL() Before Open.
	AddTestStepL(RTestAudioRecGetMetaDataEntB4OpenAndPly::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AMR-I-0106"),KBasicAmr));
	
	//AddMetaDataEntryL() While in Recording State.
	AddTestStepL(RTestAudioRecAddMetaDataEntB4OpenAndRec::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AMR-I-0108"),_L("BasicAmrOne")));
	//AddMetaDataEntryL() Before Open.
	AddTestStepL(RTestAudioRecAddMetaDataEntB4OpenAndRec::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AMR-I-0109"),KBasicAmr));
	
	//RemoveMetaDataEntry() While in Recording And while in Playing State.
	AddTestStepL(RTestAudioRecRemvMetaDataEntB4OpenAndRec::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AMR-I-0111"),_L("BasicAmrOne")));
	//RemoveMetaDataEntry() Before Open.
	AddTestStepL(RTestAudioRecRemvMetaDataEntB4OpenAndRec::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AMR-I-0112"),KBasicAmr));
	
	//ReplaceMetaDataEntryL() While in Recording And while in Playing State.
	AddTestStepL(RTestAudioRecRepMetaDataEntB4OpenAndRec::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AMR-I-0114"),_L("BasicAmrOne")));
	//ReplaceMetaDataEntryL() Before Open.
	AddTestStepL(RTestAudioRecRepMetaDataEntB4OpenAndRec::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AMR-I-0115"),KBasicAmr));
	//WillResumePlay() While in Open State.
	AddTestStepL(RTestAudioRecWillResumePlayInOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AMR-I-0124"),KBasicAmr));


	/*************************************************************************************
	*    						Test Suite For AAC Controller
	**************************************************************************************/
	
	AddTestStepL(RAudioRecTestStep::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AAC-I-0001"),KBasicAac));
	
	//Open an audio file in Opened state
	AddTestStepL(RTestStepAudioRecOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AAC-I-0002"),KBasicAac));
	
	AddTestStepL(RTestStepAudioRecPlayInPlaying::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AAC-I-0004"),KBasicAac));
	AddTestStepL(RTestStepAudioRecPlayBeforeOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AAC-I-0005"),KBasicAac));
	
	AddTestStepL(RTestStepAudioRecRecordBeforeOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AAC-I-0008"),KBasicAac));
	AddTestStepL(RTestStepAudioRecRecordMaxSize::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AAC-I-0009"),KBasicAac));
	AddTestStepL(RTestStepAudioRecRecordWhileInRecording::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AAC-I-0010"),KBasicAac));
	
	AddTestStepL(RTestStepAudioRecMaxVolBeforeOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AAC-I-0012"),KBasicAac));
	AddTestStepL(RTestStepAudioRecMaxVolBeforeOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AAC-I-0013"),KBasicAac));
	AddTestStepL(RTestStepAudioRecMaxVolBeforeOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AAC-I-0014"),KBasicAac));
	
	AddTestStepL(RTestStepAudioRecSetGetVolBeforeOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AAC-I-0017"),KBasicAac));
	AddTestStepL(RTestStepAudioRecSetVolInOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AAC-I-0015"),KBasicAac));
	AddTestStepL(RTestStepAudioRecSetVolInOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AAC-I-0016"),KBasicAac));
	AddTestStepL(RTestStepAudioRecSetVolInOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AAC-I-0018"),KBasicAac));
	AddTestStepL(RTestStepAudioRecSetVolInOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AAC-I-0019"),KBasicAac));
	
	AddTestStepL(RTestStepAudioRecSetVolRampBeforeOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AAC-I-0021"),KBasicAac));
	AddTestStepL(RTestStepAudioRecSetVolRampBeforeOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AAC-I-0022"),KBasicAac));
	
	AddTestStepL(RTestStepAudioRecMaxGainBeforeOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AAC-I-0023"),KBasicAac));
	AddTestStepL(RTestStepAudioRecMaxGainBeforeOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AAC-I-0024"),KBasicAac));
	AddTestStepL(RTestStepAudioRecMaxGainBeforeOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AAC-I-0025"),KBasicAac));
	
	AddTestStepL(RTestStepAudioRecSetAndGetGaininAllCond::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AAC-I-0026"),KBasicAac));
	AddTestStepL(RTestStepAudioRecSetAndGetGaininAllCond::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AAC-I-0027"),KBasicAac));
	AddTestStepL(RTestStepAudioRecSetAndGetGaininAllCond::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AAC-I-0028"),KBasicAac));
	AddTestStepL(RTestStepAudioRecSetAndGetGaininAllCond::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AAC-I-0029"),_L("BasicAacOne")));
	AddTestStepL(RTestStepAudioRecSetAndGetGaininAllCond::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AAC-I-0030"),_L("BasicAacOne")));
	
	AddTestStepL(RTestStepAudioRecSetAndGetPositionInAllCond::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AAC-I-0031"),KBasicAac));
	AddTestStepL(RTestStepAudioRecSetAndGetPositionInAllCond::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AAC-I-0032"),KBasicAac));
	AddTestStepL(RTestStepAudioRecSetAndGetPositionInAllCond::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AAC-I-0033"),KBasicAac));
	AddTestStepL(RTestStepAudioRecSetAndGetPositionInAllCond::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AAC-I-0034"),KBasicAac));
	AddTestStepL(RTestStepAudioRecSetAndGetPositionInAllCond::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AAC-I-0126"),_L("BasicAacOne")));
	//RecodTimeAvailable 
	AddTestStepL(RTestStepAudioRecRecodTimeAvailInRecordingAndBeforeOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AAC-I-0036"),KBasicAac));
	AddTestStepL(RTestStepAudioRecRecodTimeAvailInSetMaxWriteLength::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AAC-I-0035"),KBasicAac));
	AddTestStepL(RTestStepAudioRecRecodTimeAvailInRecordingAndBeforeOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AAC-I-0037"),KBasicAac));
	
	
	AddTestStepL(RTestStepAudioRecDurationBeforeOpenRecordingPlaying::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AAC-I-0038"),KBasicAac));
	AddTestStepL(RTestStepAudioRecDurationBeforeOpenRecordingPlaying::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AAC-I-0039"),KBasicAac));
	AddTestStepL(RTestStepAudioRecDurationBeforeOpenRecordingPlaying::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AAC-I-0040"),KBasicAac));
	AddTestStepL(RTestStepAudioRecDurationBeforeOpenRecordingPlaying::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AAC-I-0041"),KBasicAac));
	
	//SetPlayWindow in Open state
	AddTestStepL(RTestStepAudioRecSetPlayWindowOpenAndPlaying::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AAC-I-0042"),KBasicAac));
	//SetPlayWindow set starting value Less Than to ZERO in Open state
	AddTestStepL(RTestStepAudioRecSetPlayWindowOpenAndPlaying::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AAC-I-0043"),_L("BasicAacOne")));
	//SetPlayWindow set End value to a Max in Open state
	AddTestStepL(RTestStepAudioRecSetPlayWindowOpenAndPlaying::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AAC-I-0044"),_L("BasicAacTwo")));
	//SetPlayWindow while in Playing state
	AddTestStepL(RTestStepAudioRecSetPlayWindowOpenAndPlaying::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AAC-I-0045"),KBasicAac));
	//SetPlayWindow while before Open,i.e ENotReady state
	AddTestStepL(RTestStepAudioRecSetPlayWindowbeforeOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AAC-I-0046"),KBasicAac));
	
	//ClearPlayWindow in Opened State
	AddTestStepL(RTestStepAudioRecClearPlayWindow::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AAC-I-0047"),KBasicAac));
	//ClearPlayWindow in Playing State
	AddTestStepL(RTestStepAudioRecClearPlayWindow::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AAC-I-0048"),KBasicAac));
	//ClearPlayWindow in Before Open State
	AddTestStepL(RTestStepAudioRecClearPlayWindowBeforeOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AAC-I-0049"),KBasicAac));
	
	//SetRepeats in Opened State
	AddTestStepL(RTestStepAudioRecSetRepeatsInOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AAC-I-0050"),KBasicAac));
	//SetRepeats in Playing State
	AddTestStepL(RTestStepAudioRecSetRepeatsInPlay::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AAC-I-0051"),KBasicAac));
	//SetRepeats in Less than Zero while Playing State
	AddTestStepL(RTestStepAudioRecSetRepeatsInPlay::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AAC-I-0052"),_L("BasicAacOne")));
	//SetRepeats in ENotReady state
	AddTestStepL(RTestStepAudioRecSetRepeatsBeforeOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AAC-I-0130"),KBasicAac));
	

	//SetMaxWriteLength in Boundary(Less Than ZERO) condition in Open state
	AddTestStepL(RTestStepAudioRecMinSizeInOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AAC-I-0055"),_L("BasicAacOne")));
	//SetMaxWriteLength in Recording state 
	AddTestStepL(RTestStepAudioRecMaxSizeInRecord::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AAC-I-0054"),KBasicAac));
	
	//SetMaxWriteLength in Boundary(Maximum) condition in Open state
	AddTestStepL(RTestStepAudioRecMaxSizeInOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AAC-I-0053"),KBasicAac));
	//SetMaxWriteLength in Before Open state
	AddTestStepL(RTestStepAudioRecMaxSizeInBeforeOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AAC-I-0056"),KBasicAac));
	
	//SetPriority in Open state( Can not be testes, production code required to change for CMMFMdaAudioRecorderUtility)
	AddTestStepL(RTestStepAudioRecSetPriorityInOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AAC-I-0058"),KBasicAac));
	
	//SetPlayBackBalance while Before Open an audio file
	AddTestStepL(RTestStepAudioRecSetPlayBackBalBeforeOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AAC-I-0063"),KBasicAac));
	//SetPlayBackBalance(Boundary Condition setting to Max Value) while in Opened state
	AddTestStepL(RTestStepAudioRecSetPlayBackBalInOpenedMaxValue::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AAC-I-0064"),_L("BasicAacOne")));
	//SetPlayBackBalance(Boundary Condition setting to Minimum Value) while in Opened state
	AddTestStepL(RTestStepAudioRecSetPlayBackBalInOpenedMinValue::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AAC-I-0065"),_L("BasicAacOne")));
	
	//SetRecordBalance while Before Open an audio file
	AddTestStepL(RTestStepAudioRecSetRecordBalBeforeOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AAC-I-0068"),KBasicAac));
	//SetRecordBalance while in Opened state
	AddTestStepL(RTestStepAudioRecSetRecordBalInOpenMaxValue::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AAC-I-0066"),_L("BasicAacOne")));
	//SetRecordBalance(Boundary Condition setting to Minimum Value) while in Opened state
	AddTestStepL(RTestStepAudioRecSetRecordBalInOpenMaxValue::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AAC-I-0070"),_L("BasicAacOne")));
	//SetRecordBalance(Boundary Condition setting to Minimum Value) while in Opened state

	AddTestStepL(RTestStepAudioRecSetRecordBalInOpenMinValue::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AAC-I-0069"),_L("BasicAacOne")));
	//SetRecordBalance while in Recording state
	AddTestStepL(RTestStepAudioRecSetRecordBalInRecording::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AAC-I-0067"),KBasicAac));
	
	//GetSupportedDestinationDataTypes while in Opened state
	AddTestStepL(RTestStepAudioRecGetSupportDestDataTypeInRecording::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AAC-I-0071"),KBasicAac));
	//GetSupportedDestinationDataTypes while in Recording state
	AddTestStepL(RTestStepAudioRecGetSupportDestDataTypeInRecording::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AAC-I-0072"),KBasicAac));
	//GetSupportedDestinationDataTypes Before Open an Audio file.
	AddTestStepL(RTestStepAudioRecGetSupportDestDataTypeBeforeOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AAC-I-0073"),KBasicAac));
	
	//SetDestinationDataType/DestinationDataType After Opened an Audio file.
	AddTestStepL(RTestStepAudioRecSetGetDestnDataTypeBeforeOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AAC-I-0074"),KBasicAac));
	//SetDestinationDataType/DestinationDataType Before Open an Audio file.
	AddTestStepL(RTestStepAudioRecSetGetDestnDataTypeBeforeOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AAC-I-0075"),KBasicAac));
	//SetDestinationDataType While Recording.
	AddTestStepL(RTestStepAudioRecGetDestnDataTypeInRecording::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AAC-I-0076"),KBasicAac));
	
	//SetDestinationBitRate/DestinationBitRate while Open an Audio file.
	AddTestStepL(RTestStepAudioRecSetGetDestnBitRateBeforeOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AAC-I-0077"),KBasicAac));
	//SetDestinationBitRate/DestinationBitRate Before Open an Audio file.
	AddTestStepL(RTestStepAudioRecSetGetDestnBitRateBeforeOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AAC-I-0078"),KBasicAac));
	//DestinationBitRate While in Recording State.
	AddTestStepL(RTestStepAudioRecGetDestnBitRateInRecording::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AAC-I-0079"),KBasicAac));
	
	//GetSupportedBitRatesL() Before Open state.
	AddTestStepL(RTestStepAudioRecGetSupportedBitRateBeforeOpenAndRec::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AAC-I-0082"),KBasicAac));
	//GetSupportedBitRatesL() while after Open an Audio file.
	AddTestStepL(RTestStepAudioRecGetSupportedBitRateBeforeOpenAndRec::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AAC-I-0080"),KBasicAac));
	//GetSupportedBitRatesL() While in Recording an Audio file.
	AddTestStepL(RTestStepAudioRecGetSupportedBitRateBeforeOpenAndRec::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AAC-I-0081"),KBasicAac));
	
	//GetSupportedBitRatesL() While in Playing State.
	AddTestStepL(RTestStepAudioRecSourceBitRateBeforeOpenAndPalying::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AAC-I-0084"),KBasicAac));
	//GetSupportedBitRatesL() While After Open an Audio file.
	AddTestStepL(RTestStepAudioRecSourceBitRateBeforeOpenAndPalying::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AAC-I-0083"),KBasicAac));
	//GetSupportedBitRatesL() Before Open.
	AddTestStepL(RTestStepAudioRecSourceBitRateBeforeOpenAndPalying::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AAC-I-0085"),_L("BasicAacOne")));

	//SetDestinationSampleRateL()/DestinationSampleRateL() While After Open an Audio file.
	AddTestStepL(RTestStepAudioRecSetGetDestnSampleRateBeforeOpenAndRec::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AAC-I-0086"),KBasicAac));
	//SetDestinationSampleRateL()/DestinationSampleRateL() While in Recording State.
	AddTestStepL(RTestStepAudioRecSetGetDestnSampleRateBeforeOpenAndRec::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AAC-I-0087"),KBasicAac));
	//SetDestinationSampleRateL()/DestinationSampleRateL() Before Open.
	AddTestStepL(RTestStepAudioRecSetGetDestnSampleRateBeforeOpenAndRec::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AAC-I-0088"),_L("BasicAacOne")));

	//GetSupportedSampleRatesL() While After Open an Audio file.
	AddTestStepL(RTestStepAudioRecGetSupportedSampleRateBeforeOpenAndRec::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AAC-I-0089"),KBasicAac));
	//GetSupportedSampleRatesL() While in Recording State.
	AddTestStepL(RTestStepAudioRecGetSupportedSampleRateBeforeOpenAndRec::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AAC-I-0090"),KBasicAac));
	//GetSupportedSampleRatesL() Before Open.
	AddTestStepL(RTestStepAudioRecGetSupportedSampleRateBeforeOpenAndRec::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AAC-I-0091"),_L("BasicAacOne")));

	//SetDestinationFormatL()/DestinationFormatL() After Open an Audio file.
	AddTestStepL(RTestStepAudioRecSetGetDestnFormatBeforeOpenAndRec::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AAC-I-0092"),KBasicAac));
	//SetDestinationFormatL()/DestinationFormatL() Before Open.
	AddTestStepL(RTestStepAudioRecSetGetDestnFormatBeforeOpenAndRec::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AAC-I-0093"),KBasicAac));
	//SetDestinationFormatL()/DestinationFormatL() While in Recording State.
	AddTestStepL(RTestStepAudioRecSetGetDestnFormatBeforeOpenAndRec::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AAC-I-0094"),_L("BasicAacOne")));
	
	//SetDestinationNumberOfChannelsL()/DestinationNumberOfChannelsL() While After Open an Audio file.
	AddTestStepL(RTestStepAudioRecSetGetDestnNumberChannelBeforeOpenAndRec::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AAC-I-0095"),_L("BasicAacOne")));
	//SetDestinationNumberOfChannelsL()/DestinationNumberOfChannelsL() While in Recording State.
	AddTestStepL(RTestStepAudioRecSetGetDestnNumberChannelBeforeOpenAndRec::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AAC-I-0096"),_L("BasicAacOne")));
	//SetDestinationNumberOfChannelsL()/DestinationNumberOfChannelsL() Before Open.
	AddTestStepL(RTestStepAudioRecSetGetDestnNumberChannelBeforeOpenAndRec::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AAC-I-0097"),KBasicAac));
	
	//GetSupportedNumberOfChannelsL() While After Open an Audio file.
	AddTestStepL(RTestAudioRecGetSuptedNoOfChanlsB4OpenAndRec::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AAC-I-0098"),_L("BasicAacOne")));
	//GetSupportedNumberOfChannelsL() While in Recording State.
	AddTestStepL(RTestAudioRecGetSuptedNoOfChanlsB4OpenAndRec::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AAC-I-0099"),_L("BasicAacOne")));
	//GetSupportedNumberOfChannelsL() Before Open.
	AddTestStepL(RTestAudioRecGetSuptedNoOfChanlsB4OpenAndRec::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AAC-I-0100"),KBasicAac));
	
	//GetSupportedNumberOfChannelsL() While in Recording And in Playing State.
	AddTestStepL(RTestAudioRecGetNumOfMetaDataEntB4OpenAndRec::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AAC-I-0102"),_L("BasicAacOne")));
	//GetSupportedNumberOfChannelsL() Before Open.
	AddTestStepL(RTestAudioRecGetNumOfMetaDataEntB4OpenAndRec::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AAC-I-0103"),KBasicAac));
	
	//GetMetaDataEntryL() While in Recording And while in Playing State.
	AddTestStepL(RTestAudioRecGetMetaDataEntB4OpenAndPly::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AAC-I-0105"),_L("BasicAacOne")));
	//GetMetaDataEntryL() Before Open.
	AddTestStepL(RTestAudioRecGetMetaDataEntB4OpenAndPly::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AAC-I-0106"),KBasicAac));
	
	//AddMetaDataEntryL() While in Recording State.
	AddTestStepL(RTestAudioRecAddMetaDataEntB4OpenAndRec::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AAC-I-0108"),_L("BasicAacOne")));
	//AddMetaDataEntryL() Before Open.
	AddTestStepL(RTestAudioRecAddMetaDataEntB4OpenAndRec::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AAC-I-0109"),KBasicAac));
	
	//RemoveMetaDataEntry() While in Recording And while in Playing State.
	AddTestStepL(RTestAudioRecRemvMetaDataEntB4OpenAndRec::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AAC-I-0111"),_L("BasicAacOne")));
	//RemoveMetaDataEntry() Before Open.
	AddTestStepL(RTestAudioRecRemvMetaDataEntB4OpenAndRec::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AAC-I-0112"),KBasicAac));
	
	//ReplaceMetaDataEntryL() While in Recording And while in Playing State.
	AddTestStepL(RTestAudioRecRepMetaDataEntB4OpenAndRec::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AAC-I-0114"),_L("BasicAacOne")));
	//ReplaceMetaDataEntryL() Before Open.
	AddTestStepL(RTestAudioRecRepMetaDataEntB4OpenAndRec::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AAC-I-0115"),KBasicAac));
	//WillResumePlay() While in Open State.
	AddTestStepL(RTestAudioRecWillResumePlayInOpen::NewL(_L("MM-MMF-ARCLNT-CHRTZ-AAC-I-0124"),KBasicAac));
#endif // SYMBIAN_MULTIMEDIA_A3FDEVSOUND
	}



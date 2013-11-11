// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32svr.h>
#include <mmf/common/mmfaudio.h>
#include <mmf/server/mmfurl.h>
#include "Ts_cmmftestcontroller.h"

const TInt KReloadingSimulatorInterval = 10;
const TInt KReloadingSimulatorIncrement = 10;



CMMFTestController* CMMFTestController::NewL(void)
	{
	CMMFTestController* self = CMMFTestController::NewLC();
	CleanupStack::Pop();
	return self;
	}

CMMFTestController* CMMFTestController::NewLC(void)
	{
	CMMFTestController* self = new (ELeave) CMMFTestController();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}
	
CMMFTestController::CMMFTestController(void)
	{	
	}

CMMFTestController::~CMMFTestController(void)
	{
	if (iAsyncCallback)
		iAsyncCallback->Cancel();
	delete iAsyncCallback;
	
	if (iReloadingSimulator)
		iReloadingSimulator->Cancel();
	delete iReloadingSimulator;

	iDataSources.Reset();
	iDataSinks.Reset();
	}
                    
void CMMFTestController::ConstructL (void)
	{

	iAsyncCallback = new (ELeave) CAsyncCallback;
	iReloadingSimulator = CReloadingSimulator::NewL(this);
	//----------------------------//
	// Test Custom Command Parser //
	//----------------------------//

	CMMFTestCustomCommandParser* testParser = CMMFTestCustomCommandParser::NewL(*this);
	CleanupStack::PushL(testParser);
	AddCustomCommandParserL(*testParser);
	CleanupStack::Pop();


	//--------------------------//
	// Standard Custom Commands //
	//--------------------------//

	// Construct custom command parsers
	CMMFAudioPlayDeviceCustomCommandParser* audPlayDevParser = CMMFAudioPlayDeviceCustomCommandParser::NewL(*this);
	CleanupStack::PushL(audPlayDevParser);
	AddCustomCommandParserL(*audPlayDevParser);
	CleanupStack::Pop();//audPlayDevParser

	CMMFAudioRecordDeviceCustomCommandParser* audRecDevParser = CMMFAudioRecordDeviceCustomCommandParser::NewL(*this);
	CleanupStack::PushL(audRecDevParser);
	AddCustomCommandParserL(*audRecDevParser);
	CleanupStack::Pop();//audRecDevParser

	CMMFAudioPlayControllerCustomCommandParser* audPlayConParser = CMMFAudioPlayControllerCustomCommandParser::NewL(*this);
	CleanupStack::PushL(audPlayConParser);
	AddCustomCommandParserL(*audPlayConParser);
	CleanupStack::Pop();//audPlayConParser

	CMMFAudioRecordControllerCustomCommandParser* audRecConParser = CMMFAudioRecordControllerCustomCommandParser::NewL(*this);
	CleanupStack::PushL(audRecConParser);
	AddCustomCommandParserL(*audRecConParser);
	CleanupStack::Pop();//audRecParser

	CMMFAudioControllerCustomCommandParser* audConParser = CMMFAudioControllerCustomCommandParser::NewL(*this);
	CleanupStack::PushL(audConParser);
	AddCustomCommandParserL(*audConParser);
	CleanupStack::Pop();//audConParser

/*XXX - need to put this back in when the video custom commands have stabilised.
	CMMFVideoControllerCustomCommandParser* vidConParser = CMMFVideoControllerCustomCommandParser::NewL(*this);
	CleanupStack::PushL(vidConParser);
	AddCustomCommandParserL(*vidConParser);
	CleanupStack::Pop();//vidConParser
*/

#ifdef SYMBIAN_BUILD_GCE	
	CMMFVideoPlaySurfaceSupportCustomCommandParser* vidSurSptParser = CMMFVideoPlaySurfaceSupportCustomCommandParser::NewL(*this);
	CleanupStack::PushL(vidSurSptParser);
	AddCustomCommandParserL(*vidSurSptParser);
	CleanupStack::Pop();//vidSurSptParser
#endif // SYMBIAN_BUILD_GCE
	
#ifdef SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT
	CMMFVideoPlaySubtitleSupportCustomCommandParser* subtitleSupParser = CMMFVideoPlaySubtitleSupportCustomCommandParser::NewL(*this);
	CleanupStack::PushL(subtitleSupParser);
	AddCustomCommandParserL(*subtitleSupParser);
	CleanupStack::Pop(subtitleSupParser);
#endif //SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT

	//------------------------------//
	// Initial Member Function Text //
	//------------------------------//
	_LIT8(KConstructText, "CMMFTestController Construction");
	iMemFunctionText.SetLength(64);
	iMemFunctionText = KConstructText;

	//-------------------------//
	// Set Default Object Node //
	//-------------------------//
	iControllerMode = ENormal;
	}

// The method below need to be filled in with the correct responses

void CMMFTestController::AddDataSourceL(MDataSource& aDataSource)
	{
	iLastDataSourceAdded = &aDataSource;
	iSourceAdded = ETrue;

	iDataSources.Append(&aDataSource);

	_LIT8(KFuncText, "AddDataSourceL Called");
	iMemFunctionText = KFuncText;

	if (iControllerMode == EFailure)
		User::Leave(KAddDataSourceFailure);

	}

void CMMFTestController::AddDataSinkL(MDataSink& aDataSink)
	{

	iLastDataSinkAdded = &aDataSink;
	iSinkAdded = ETrue;

	iDataSinks.Append(&aDataSink);

	_LIT8(KFuncText, "AddDataSinkL Called");
	iMemFunctionText = KFuncText;

	if (iControllerMode == EFailure)
		User::Leave(KAddDataSinkFailure);

	}

void CMMFTestController::RemoveDataSourceL(MDataSource& aDataSource)
	{
	_LIT8(KFuncText, "RemoveDataSourceL Called");
	iMemFunctionText = KFuncText;
	TBool sourceFound = EFalse;

	// Search for correct Data Source and Remove
	for(TInt a = 0; a < iDataSources.Count(); a++)
		{
		if (iDataSources[a] == &aDataSource) 
			{
			iDataSources.Remove(a);
			sourceFound = ETrue;
			}
		}

	if ((iControllerMode == EFailure) ||(sourceFound == EFalse))
		User::Leave(KRemoveDataSourceFailure);

	}

void CMMFTestController::RemoveDataSinkL(MDataSink& aDataSink)
	{
	_LIT8(KFuncText, "RemoveDataSinkL Called");
	iMemFunctionText = KFuncText;
	TBool sinkFound = EFalse;

	// Search for correct Data Source and Remove
	for(TInt a = 0; a < iDataSinks.Count(); a++)
		{
		if (iDataSinks[a] == &aDataSink) 
			{
			iDataSinks.Remove(a);
			sinkFound = ETrue;
			}
		}

	if ((iControllerMode == EFailure) || (sinkFound == EFalse))
		User::Leave(KRemoveDataSinkFailure);
	}

void CMMFTestController::RemoveSourceSinkL()
	{
	_LIT8(KFuncText, "RemoveSourceSinkL Called");
	iMemFunctionText = KFuncText;
	}

void CMMFTestController::ResetL() //XXX - replacement for RemoveSourceSink()
	{
	_LIT8(KFuncText, "ResetL Called");
	iMemFunctionText = KFuncText;
	}

void CMMFTestController::PrimeL()
	{
	_LIT8(KFuncText, "PrimeL Called");
	iMemFunctionText = KFuncText;

	if (iControllerMode == EFailure)
		User::Leave(KPrimeFailure);
	}

void CMMFTestController::PlayL()
	{
	_LIT8(KFuncText, "PlayL Called");
	iMemFunctionText = KFuncText;
	}

void CMMFTestController::PauseL()
	{
	_LIT8(KFuncText, "PauseL Called");
	iMemFunctionText = KFuncText;
	}

void CMMFTestController::StopL()
	{
	_LIT8(KFuncText, "StopL Called");
	iMemFunctionText = KFuncText;
	}

TTimeIntervalMicroSeconds CMMFTestController::PositionL() const
	{
	return TTimeIntervalMicroSeconds(16);
	}

void CMMFTestController::SetPositionL(const TTimeIntervalMicroSeconds& aPosition)
	{
	_LIT8(KFuncText, "SetPositionL Called");
	iMemFunctionText = KFuncText;

	if (aPosition.Int64() == 2)
		{
		// Check for correct Value
		}
	}

TTimeIntervalMicroSeconds CMMFTestController::DurationL() const
	{
	return TTimeIntervalMicroSeconds(36);
	}

void CMMFTestController::CustomCommand(TMMFMessage& aMessage)
	{
	_LIT8(KDummyFunc1Text, "KDummyFunc1 Called");
	_LIT8(KDummyFunc2Text, "KDummyFunc2 Called");

	switch (aMessage.Function())
		{
	case KDescriptorSource:
		iLastDataSourceAdded->SourceCustomCommand(aMessage);
		break;
		
	case KDescriptorSink:
		iLastDataSinkAdded->SinkCustomCommand(aMessage);
		break;

	case KDummyFunc1:
		iMemFunctionText = KDummyFunc1Text;
		aMessage.Complete(KFuncComplete);
		break;
	case KDummyFunc2:
		iMemFunctionText = KDummyFunc2Text;
		aMessage.Complete(KFuncComplete);
		break;
	case KLogFunction:
		GetMemberFunctionCalled(aMessage);
		break;
	case KModeFunction:
		SetControllerMode(aMessage);
		break;
#ifdef SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT
	case KTestSetSubtitleAvailable:
		SetSubtitlesAvailable(aMessage);
		break;
#endif //SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT
	case KFuncThreadStackSize:
		ThreadStackSize(aMessage);
		break;
	default:
		aMessage.Complete(KErrNotSupported);
		}
	}

#ifdef SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT
void CMMFTestController::SetSubtitlesAvailable(TMMFMessage& aMessage)
	{
	TPckgBuf<TInt> availPckg;
	aMessage.ReadData1FromClient(availPckg);
	iSubtitlesAvailableErr = availPckg();
	aMessage.Complete(KErrNone);
	}
#endif //SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT

void CMMFTestController::SetPrioritySettings(const TMMFPrioritySettings& aPrioritySettings)
	{
	_LIT8(KFuncText, "SetPrioritySettings Called");
	iMemFunctionText = KFuncText;

	if (aPrioritySettings.iPriority == 1)
		{
		// Check Parameters
		}
	}

void CMMFTestController::GetNumberOfMetaDataEntriesL(TInt& aNumberOfEntries)
	{
	_LIT8(KFuncText, "GetNumberOfMetaDataEntriesL Called");
	iMemFunctionText = KFuncText;

	aNumberOfEntries = 4;
	}

CMMFMetaDataEntry* CMMFTestController::GetMetaDataEntryL(TInt aIndex)
	{ 
	_LIT8(KFuncText, "GetMetaDataEntryL Called");
	iMemFunctionText = KFuncText;

	CMMFMetaDataEntry* metaDataEntry = NULL;

	if (aIndex == 1)
		{
		_LIT(KMetaData, "AudioLevel");
		_LIT(KMetaValue, "HIGH");
		metaDataEntry = CMMFMetaDataEntry::NewL(KMetaData,KMetaValue);
		}
	else if (aIndex == 2)
		{
		_LIT(KMetaData, "AudioQuality");
		_LIT(KMetaValue, "LOW");
		metaDataEntry = CMMFMetaDataEntry::NewL(KMetaData,KMetaValue);
		}

	return metaDataEntry;
	}

TInt CMMFTestController::MtccSetAudioQuality(TInt aQuality)
	{
	_LIT8(KFuncText, "SetAudioQuality Called");
	iMemFunctionText = KFuncText;

	if (aQuality == 1)
		{
		// XXX, Check it's the correct value.
		}

	return 1;
	}
		
TInt CMMFTestController::MtccGetAudioQuality(TInt& aQuality)
	{
	_LIT8(KFuncText, "GetAudioQuality Called");
	iMemFunctionText = KFuncText;

	aQuality = 1; // Value to return

	return 1;
	}


//-----------------------------------//
// RMMFAudioPlayDeviceCustomCommands //
//-----------------------------------//

void CMMFTestController::MapdSetVolumeL(TInt aVolume)
	{
	_LIT8(KFuncText, "MapdSetVolumeL Called");
	iMemFunctionText = KFuncText;

	if (aVolume == 1)
		{
		//XXX, check it's the correct value
		}
	}


void CMMFTestController::MapdGetMaxVolumeL(TInt& aMaxVolume)
	{
	_LIT8(KFuncText, "MapdGetMaxVolumeL Called");
	iMemFunctionText = KFuncText;

	aMaxVolume = 1;
	}

void CMMFTestController::MapdGetVolumeL(TInt& aVolume)
	{
	_LIT8(KFuncText, "MapdGetVolumeL Called");
	iMemFunctionText = KFuncText;

	aVolume = 1;
	}

void CMMFTestController::MapdSetVolumeRampL(const TTimeIntervalMicroSeconds& aRampDuration)
	{
	_LIT8(KFuncText, "MapdSetVolumeRampL Called");
	iMemFunctionText = KFuncText;

	if (aRampDuration.Int64() == 10)
		{
		// XXX, check it's the correct value
		} 
	}

void CMMFTestController::MapdSetBalanceL(TInt aBalance)
	{
	_LIT8(KFuncText, "MapdSetBalanceL Called");
	iMemFunctionText = KFuncText;

	if (aBalance == 10)
		{
		//XXX, check it's the correct value
		}
	}

void CMMFTestController::MapdGetBalanceL(TInt& aBalance)
	{
	_LIT8(KFuncText, "MapdGetBalanceL Called");
	iMemFunctionText = KFuncText;

	aBalance = 1;
	}


//-------------------------------------//
// RMMFAudioRecordDeviceCustomCommands //
//-------------------------------------//

void CMMFTestController::MardSetGainL(TInt aGain)
	{
	_LIT8(KFuncText, "MardSetGainL Called");
	iMemFunctionText = KFuncText;

	if (aGain == 1)
		{
		// XXX, check it's the correct value
		}
	}

void CMMFTestController::MardGetMaxGainL(TInt& aMaxGain)
	{
	_LIT8(KFuncText, "MardGetMaxGainL Called");
	iMemFunctionText = KFuncText;

	aMaxGain = 10;
	}

void CMMFTestController::MardGetGainL(TInt& aGain)
	{
	_LIT8(KFuncText, "MardGetGainL Called");
	iMemFunctionText = KFuncText;

	aGain = 1;
	}

void CMMFTestController::MardSetBalanceL(TInt aBalance)
	{
	_LIT8(KFuncText, "MardSetBalanceL Called");
	iMemFunctionText = KFuncText;

	if (aBalance == 8)
		{
		// XXX, check it's the correct value
		}
	}

void CMMFTestController::MardGetBalanceL(TInt& aBalance)
	{
	_LIT8(KFuncText, "MardGetBalanceL Called");
	iMemFunctionText = KFuncText;

	aBalance = 8;
	}

//-------------------------------------------------//
// MMMFAudioPlayControllerCustomCommandImplementor //
//-------------------------------------------------//

void CMMFTestController::MapcSetPlaybackWindowL(const TTimeIntervalMicroSeconds& aStart, const TTimeIntervalMicroSeconds& aEnd)
	{
	_LIT8(KFuncText, "MapdSetPlaybackWindowL Called");
	iMemFunctionText = KFuncText;

	if ((aStart.Int64() == 1) || (aEnd.Int64() == 10))
		{
		// XXX, check it's the correct value
		}
	}
	
void CMMFTestController::MapcDeletePlaybackWindowL()
	{
	_LIT8(KFuncText, "MapdDeletePlaybackWindowL Called");
	iMemFunctionText = KFuncText;
	}

void CMMFTestController::MapcGetLoadingProgressL(TInt& aPercentageComplete)
	{
	aPercentageComplete = iReloadingSimulator->LoadingProgress();
	}

//---------------------------------------------------//
// MMMFAudioRecordControllerCustomCommandImplementor //
//---------------------------------------------------//

void CMMFTestController::MarcGetRecordTimeAvailableL(TTimeIntervalMicroSeconds& aTime)
	{
	_LIT8(KFuncText, "MarcGetRecordTimeAvailableL Called");
	iMemFunctionText = KFuncText;

	TInt64 someTime = 12;
	aTime = someTime;
	}

void CMMFTestController::MarcSetMaxDurationL(const TTimeIntervalMicroSeconds& aMaxDuration)
	{
	_LIT8(KFuncText, "MarcSetMaxDurationL Called");
	iMemFunctionText = KFuncText;

	if (aMaxDuration.Int64() == 10)
		{
		// XXX, check it's the correct value
		}
	}

void CMMFTestController::MarcSetMaxFileSizeL(TInt aMaxSize)
	{
	_LIT8(KFuncText, "MarcSetMaxFileSizeL Called");
	iMemFunctionText = KFuncText;

	if (aMaxSize == 10)
		{
		// XXX, check it's the correct value
		}
	}

void CMMFTestController::MarcCropL(TBool aToEnd)
	{
	_LIT8(KFuncText, "MarcCropL Called");
	iMemFunctionText = KFuncText;

	if (aToEnd)
		{
		// XXX, check it's the correct value
		}
	}

void CMMFTestController::MarcAddMetaDataEntryL(const CMMFMetaDataEntry& aNewEntry)
	{
	_LIT8(KFuncText, "MarcAddMetaDataEntryL Called");
	iMemFunctionText = KFuncText;

	_LIT(KName, "A ENTRY");
	if (aNewEntry.Name() == KName)
		{
		// XXX, check it's the correct value
		}
	}

void CMMFTestController::MarcRemoveMetaDataEntryL(TInt aIndex)
	{
	_LIT8(KFuncText, "MarcRemoveMetaDataEntryL Called");
	iMemFunctionText = KFuncText;

	if (aIndex == 8)
		{
		// XXX, check it's the correct value
		}
	}

void CMMFTestController::MarcReplaceMetaDataEntryL(TInt aIndex, const CMMFMetaDataEntry& aNewEntry)
	{
	_LIT8(KFuncText, "MarcReplaceMetaDataEntryL Called");
	iMemFunctionText = KFuncText;
	if (aIndex == 8)
		{
		// XXX, check it's the correct value
		}

	_LIT(KName, "A New ENTRY");
	if (aNewEntry.Name() == KName)
		{
		// XXX, check it's the correct value
		}

	}


	//---------------------------------------------//
	// MMMFAudioControllerCustomCommandImplementor //
	//---------------------------------------------//

void CMMFTestController::MacSetSourceSampleRateL(TUint aSampleRate)
	{
	_LIT8(KFuncText, "MacSetSourceSampleRateL Called");
	iMemFunctionText = KFuncText;

	if (aSampleRate == KExpectedUint)
		iTestPassed=ETrue;
	else
		iTestPassed=EFalse;
	}

void CMMFTestController::MacSetSourceNumChannelsL(TUint aNumChannels)
	{
	_LIT8(KFuncText, "MacSetSourceNumChannelsL Called");
	iMemFunctionText = KFuncText;

	if (aNumChannels == KExpectedUint)
		iTestPassed=ETrue;
	else
		iTestPassed=EFalse;
	}

void CMMFTestController::MacSetSourceFormatL(TUid aFormatUid)
	{
	_LIT8(KFuncText, "MacSetSourceFormatL Called");
	iMemFunctionText = KFuncText;

	if (aFormatUid == KExpectedUid)
		iTestPassed=ETrue;
	else
		iTestPassed=EFalse;
	}

void CMMFTestController::MacSetSinkSampleRateL(TUint aSampleRate)
	{
	_LIT8(KFuncText, "MacSetSinkSampleRateL Called");
	iMemFunctionText = KFuncText;

	if (aSampleRate == KExpectedUint)
		iTestPassed=ETrue;
	else
		iTestPassed=EFalse;
	}

void CMMFTestController::MacSetSinkNumChannelsL(TUint aNumChannels)
	{
	_LIT8(KFuncText, "MacSetSinkNumChannelsL Called");
	iMemFunctionText = KFuncText;

	if (aNumChannels == KExpectedUint)
		iTestPassed=ETrue;
	else
		iTestPassed=EFalse;
	}

void CMMFTestController::MacSetSinkFormatL(TUid aFormatUid)
	{
	_LIT8(KFuncText, "MacSetSinkFormatL Called");
	iMemFunctionText = KFuncText;

	if (aFormatUid == KExpectedUid)
		iTestPassed=ETrue;
	else
		iTestPassed=EFalse;
	}

void CMMFTestController::MacSetCodecL(TFourCC aSourceDataType, TFourCC aSinkDataType)
	{
	_LIT8(KFuncText, "MacSetCodecL Called");
	iMemFunctionText = KFuncText;

	if ((aSourceDataType == KExpectedFourCC1) && (aSinkDataType == KExpectedFourCC2))
		iTestPassed=ETrue;
	else
		iTestPassed=EFalse;
	}

void CMMFTestController::MacSetSourceBitRateL(TUint aBitRate)
	{
	_LIT8(KFuncText, "MacSetSourceBitRateL Called");
	iMemFunctionText = KFuncText;

	if (aBitRate == KExpectedUint)
		iTestPassed=ETrue;
	else
		iTestPassed=EFalse;
	}

void CMMFTestController::MacSetSourceDataTypeL(TFourCC aDataType)
	{
	_LIT8(KFuncText, "MacSetSourceDataTypeL Called");
	iMemFunctionText = KFuncText;

	if (aDataType == KExpectedFourCC1)
		iTestPassed=ETrue;
	else
		iTestPassed=EFalse;
	}

void CMMFTestController::MacSetSinkBitRateL(TUint aRate)
	{
	_LIT8(KFuncText, "MacSetSinkBitRateL Called");
	iMemFunctionText = KFuncText;

	if (aRate == KExpectedUint)
		iTestPassed=ETrue;
	else
		iTestPassed=EFalse;
	}

void CMMFTestController::MacSetSinkDataTypeL(TFourCC aDataType)
	{
	_LIT8(KFuncText, "MacSetSinkDataTypeL Called");
	iMemFunctionText = KFuncText;

	if (aDataType == KExpectedFourCC1)
		iTestPassed=ETrue;
	else
		iTestPassed=EFalse;
	}

void CMMFTestController::MacGetSourceSampleRateL(TUint& aRate)
	{
	_LIT8(KFuncText, "MacGetSourceSampleRateL Called");
	iMemFunctionText = KFuncText;

	aRate = KExpectedUint;//client to check this is copied back correctly
	iTestPassed=ETrue;
	}

void CMMFTestController::MacGetSourceBitRateL(TUint& aRate)
	{
	_LIT8(KFuncText, "MacGetSourceBitRateL Called");
	iMemFunctionText = KFuncText;

	aRate = KExpectedUint;//client to check this is copied back correctly
	iTestPassed=ETrue;
	}

void CMMFTestController::MacGetSourceNumChannelsL(TUint& aNumChannels)
	{
	_LIT8(KFuncText, "MacGetSourceNumChannelsL Called");
	iMemFunctionText = KFuncText;

	aNumChannels = KExpectedUint;//client to check this is copied back correctly
	iTestPassed=ETrue;
	}

void CMMFTestController::MacGetSourceFormatL(TUid& aFormat)
	{
	_LIT8(KFuncText, "MacGetSourceFormatL Called");
	iMemFunctionText = KFuncText;

	aFormat = KExpectedUid;//client to check this is copied back correctly
	iTestPassed=ETrue;
	}

void CMMFTestController::MacGetSourceDataTypeL(TFourCC& aDataType)
	{
	_LIT8(KFuncText, "MacGetSourceDataTypeL Called");
	iMemFunctionText = KFuncText;

	aDataType = KExpectedFourCC1;//client to check this is copied back correctly
	iTestPassed=ETrue;
	}

void CMMFTestController::MacGetSinkSampleRateL(TUint& aRate)
	{
	_LIT8(KFuncText, "MacGetSinkSampleRateL Called");
	iMemFunctionText = KFuncText;

	aRate = KExpectedUint;//client to check this is copied back correctly
	iTestPassed=ETrue;
	}

void CMMFTestController::MacGetSinkBitRateL(TUint& aRate)
	{
	_LIT8(KFuncText, "MacGetSinkBitRateL Called");
	iMemFunctionText = KFuncText;

	aRate = KExpectedUint;//client to check this is copied back correctly
	iTestPassed=ETrue;
	}

void CMMFTestController::MacGetSinkNumChannelsL(TUint& aNumChannels)
	{
	_LIT8(KFuncText, "MacGetSinkNumChannelsL Called");
	iMemFunctionText = KFuncText;

	aNumChannels = KExpectedUint;//client to check this is copied back correctly
	iTestPassed=ETrue;
	}

void CMMFTestController::MacGetSinkFormatL(TUid& aFormat)
	{
	_LIT8(KFuncText, "MacGetSinkFormatL Called");
	iMemFunctionText = KFuncText;

	aFormat = KExpectedUid;//client to check this is copied back correctly
	iTestPassed=ETrue;
	}

void CMMFTestController::MacGetSinkDataTypeL(TFourCC& aDataType)
	{
	_LIT8(KFuncText, "MacGetSinkDataTypeL Called");
	iMemFunctionText = KFuncText;

	aDataType = KExpectedFourCC1;//client to check this is copied back correctly
	iTestPassed=ETrue;
	}

void CMMFTestController::MacGetSupportedSourceSampleRatesL(RArray<TUint>& aSupportedRates)
	{
	_LIT8(KFuncText, "MacGetSupportedSourceSampleRatesL Called");
	iMemFunctionText = KFuncText;

	User::LeaveIfError(aSupportedRates.Append(KExpectedUint));
	User::LeaveIfError(aSupportedRates.Append(KExpectedUint+1));
	User::LeaveIfError(aSupportedRates.Append(KExpectedUint+2));
	
	iTestPassed=ETrue;
	}

void CMMFTestController::MacGetSupportedSourceBitRatesL(RArray<TUint>& aSupportedRates)
	{
	_LIT8(KFuncText, "MacGetSupportedSourceBitRatesL Called");
	iMemFunctionText = KFuncText;

	User::LeaveIfError(aSupportedRates.Append(KExpectedUint));
	User::LeaveIfError(aSupportedRates.Append(KExpectedUint+1));
	User::LeaveIfError(aSupportedRates.Append(KExpectedUint+2));
	
	iTestPassed=ETrue;
	}

void CMMFTestController::MacGetSupportedSourceNumChannelsL(RArray<TUint>& aSupportedChannels)
	{
	_LIT8(KFuncText, "MacGetSupportedSourceNumChannelsL Called");
	iMemFunctionText = KFuncText;

	User::LeaveIfError(aSupportedChannels.Append(KExpectedUint));
	User::LeaveIfError(aSupportedChannels.Append(KExpectedUint+1));
	User::LeaveIfError(aSupportedChannels.Append(KExpectedUint+2));
	
	iTestPassed=ETrue;
	}

void CMMFTestController::MacGetSupportedSourceDataTypesL(RArray<TFourCC>& aSupportedDataTypes)
	{
	_LIT8(KFuncText, "MacGetSupportedSourceDataTypesL Called");
	iMemFunctionText = KFuncText;

	User::LeaveIfError(aSupportedDataTypes.Append(KExpectedFourCC1));
	User::LeaveIfError(aSupportedDataTypes.Append(KExpectedFourCC2));
	
	iTestPassed=ETrue;
	}

void CMMFTestController::MacGetSupportedSinkSampleRatesL(RArray<TUint>& aSupportedRates)
	{
	_LIT8(KFuncText, "MacGetSupportedSinkSampleRatesL Called");
	iMemFunctionText = KFuncText;

	User::LeaveIfError(aSupportedRates.Append(KExpectedUint));
	User::LeaveIfError(aSupportedRates.Append(KExpectedUint+1));
	User::LeaveIfError(aSupportedRates.Append(KExpectedUint+2));
	
	iTestPassed=ETrue;
	}

void CMMFTestController::MacGetSupportedSinkBitRatesL(RArray<TUint>& aSupportedRates)
	{
	_LIT8(KFuncText, "MacGetSupportedSinkBitRatesL Called");
	iMemFunctionText = KFuncText;

	User::LeaveIfError(aSupportedRates.Append(KExpectedUint));
	User::LeaveIfError(aSupportedRates.Append(KExpectedUint+1));
	User::LeaveIfError(aSupportedRates.Append(KExpectedUint+2));
	
	iTestPassed=ETrue;
	}

void CMMFTestController::MacGetSupportedSinkNumChannelsL(RArray<TUint>& aSupportedChannels)
	{
	_LIT8(KFuncText, "MacGetSupportedSinkNumChannelsL Called");
	iMemFunctionText = KFuncText;

	User::LeaveIfError(aSupportedChannels.Append(KExpectedUint));
	User::LeaveIfError(aSupportedChannels.Append(KExpectedUint+1));
	User::LeaveIfError(aSupportedChannels.Append(KExpectedUint+2));
	
	iTestPassed=ETrue;
	}

void CMMFTestController::MacGetSupportedSinkDataTypesL(RArray<TFourCC>& aSupportedDataTypes)
	{
	_LIT8(KFuncText, "MacGetSupportedSinkDataTypesL Called");
	iMemFunctionText = KFuncText;

	User::LeaveIfError(aSupportedDataTypes.Append(KExpectedFourCC1));
	User::LeaveIfError(aSupportedDataTypes.Append(KExpectedFourCC2));
	
	iTestPassed=ETrue;
	}




	//---------------------------------------------//
	// MMMFVideoControllerCustomCommandImplementor //
	//---------------------------------------------//
void CMMFTestController::MvcSetFrameRateL(TReal32 aFramesPerSecond)
	{
	_LIT8(KFuncText, "MvcSetFrameRateL Called");
	iMemFunctionText = KFuncText;

	if (aFramesPerSecond == 5.00)
		{
		// XXX, Check it's the correct value.
		}
	}

void CMMFTestController::MvcGetFrameRateL(TReal32& aFramesPerSecond)
	{
	_LIT8(KFuncText, "MvcGetFrameRateL Called");
	iMemFunctionText = KFuncText;

	if (aFramesPerSecond == 5.00)
		{
		// XXX, Check it's the correct value.
		}
	}

void CMMFTestController::MvcGetFrameL(TInt aFrameNumber)
	{
	_LIT8(KFuncText, "MvcGetFrameL Called");
	iMemFunctionText = KFuncText;

	if (aFrameNumber == 5)
		{
		// XXX, Check it's the correct value.
		}
	}

void CMMFTestController::MvcGetVideoDimensionL(TRect& aVideoDimension)
	{
	_LIT8(KFuncText, "MvcGetVideoDimensionL Called");
	iMemFunctionText = KFuncText;

	if (aVideoDimension.Width() == 10)
		{
		// XXX, Check it's the correct value.
		}
	}

void CMMFTestController::MvcSetDisplayWindowL(RWindow& /*aWindow*/, const TRect& aWindowRect)
	{
	_LIT8(KFuncText, "MvcSetDisplayWindowL Called");
	iMemFunctionText = KFuncText;

	if (aWindowRect.Width() == 10)
		{
		// XXX, Check it's the correct value.
		}

	}

void CMMFTestController::MvcAddMetaDataEntryL(const CMMFMetaDataEntry& aNewEntry)
	{
	_LIT8(KFuncText, "MvcSetFrameRateL Called");
	iMemFunctionText = KFuncText;

	_LIT(KName, "A New Entry");
	if (aNewEntry.Name() == KName)
		{
		// XXX, Check it's the correct value.
		}
	}

void CMMFTestController::MvcRemoveMetaDataEntryL(TInt aIndex)
	{
	_LIT8(KFuncText, "MvcAddMetaDataEntryL Called");
	iMemFunctionText = KFuncText;

	if (aIndex == 2)
		{
		// XXX, Check it's the correct value.
		}
	}

void CMMFTestController::MvcReplaceMetaDataEntryL(TInt aIndex, const CMMFMetaDataEntry& aNewEntry)
	{
	_LIT8(KFuncText, "MvcReplaceMetaDataEntryL Called");
	iMemFunctionText = KFuncText;

	if (aIndex == 2)
		{
		// XXX, Check it's the correct value.
		}

	_LIT(KName, "A New Entry");
	if (aNewEntry.Name() == KName)
		{
		// XXX, Check it's the correct value.
		}
	}

#ifdef SYMBIAN_BUILD_GCE
//---------------------------------------------//
// MMMFVideoPlaySurfaceSupportCustomCommandImplementor //
//---------------------------------------------//
void CMMFTestController::MvpssUseSurfacesL()
	{
	_LIT8(KFuncText, "MvpssUseSurfacesL Called");
	iMemFunctionText = KFuncText;

	iTestPassed=ETrue;
	}

void CMMFTestController::MvpssGetSurfaceParametersL(TSurfaceId& aSurfaceId, 
							TRect& aCropRect, TVideoAspectRatio& aPixelAspectRatio)

	{
	_LIT8(KFuncText, "MvpssGetSurfaceParametersL Called");
	iMemFunctionText = KFuncText;

	TSurfaceId surfaceId = {11111, 22222, 33333, 44444};
	TRect cropRect(100, 50, 600, 200);
	TVideoAspectRatio par(1,2);
	aSurfaceId = surfaceId;
	aCropRect = cropRect;
	aPixelAspectRatio = par;
	}

void CMMFTestController::MvpssSurfaceRemovedL(const TSurfaceId& aSurfaceId)
	{
	_LIT8(KFuncText, "MvpssSurfaceRemovedL Called");
	iMemFunctionText = KFuncText;

	TSurfaceId surfaceId = {11111, 22222, 33333, 44444};
	if (aSurfaceId == surfaceId)
		{
		iTestPassed=ETrue;
		}
	else
		{
		User::Leave(KErrArgument);
		}
	}

#endif // SYMBIAN_BUILD_GCE

#ifdef SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT
void CMMFTestController::MvpsusGetCrpParametersL(TInt aWindowId, TWsGraphicId& aId, TRect& aCrpRect)
	{
	iMemFunctionText = KGetCrpParams();
	User::LeaveIfError(iSubtitlesAvailableErr);
	
	if (KSubtitleWindowId == aWindowId)
		{
		aId = TWsGraphicId(KSubtitleWsGraphicId);
		aCrpRect = KSubtitleCrpRect;
		}
	else
		{
		User::Leave(KErrArgument);
		}
	}

void CMMFTestController::MvpsusUpdateSubtitleConfigL(const TMMFSubtitleWindowConfig& aConfig)
	{
	iMemFunctionText = KUpdateSubConfig();
	User::LeaveIfError(iSubtitlesAvailableErr);
	
	if (KSubtitleWindowId != aConfig.iWindowId ||
			KSubtitleCrpRect != aConfig.iWindowClipRect ||
			KSubtitleDisplayMode != aConfig.iDisplayMode ||
			KSubtitleRotation != aConfig.iRotation)
		{
		User::Leave(KErrArgument);
		}
	}

void CMMFTestController::MvpsusAddSubtitleConfigL(const TMMFSubtitleWindowConfig& aConfig)
	{
	iMemFunctionText = KAddSubConfig();
	User::LeaveIfError(iSubtitlesAvailableErr);
	
	if (KSubtitleWindowId != aConfig.iWindowId ||
			KSubtitleCrpRect != aConfig.iWindowClipRect ||
			KSubtitleDisplayMode != aConfig.iDisplayMode ||
			KSubtitleRotation != aConfig.iRotation)
		{
		User::Leave(KErrArgument);
		}
	}

void CMMFTestController::MvpsusRemoveSubtitleConfigL(TInt aWindowId)
	{
	iMemFunctionText = KRemoveSubConfig();
	User::LeaveIfError(iSubtitlesAvailableErr);
	
	if (KSubtitleWindowId != aWindowId)
		{
		User::Leave(KErrArgument);
		}
	}

void CMMFTestController::MvpsusGetSubtitlesAvailableL(TBool& aAvailable)
	{
	iMemFunctionText = KSubAvailable();
	aAvailable = (KErrNone == iSubtitlesAvailableErr);
	}

void CMMFTestController::MvpsusDisableSubtitlesL()
	{
	iMemFunctionText = KDisableSub();
	User::LeaveIfError(iSubtitlesAvailableErr);
	}

void CMMFTestController::MvpsusEnableSubtitlesL()
	{
	iMemFunctionText = KEnableSub();
	User::LeaveIfError(iSubtitlesAvailableErr);
	}

void CMMFTestController::MvpsusGetSubtitleLanguageL(TLanguage& aLanguage)
	{
	iMemFunctionText = KGetSubLang();
	User::LeaveIfError(iSubtitlesAvailableErr);
	aLanguage = KSubtitleLanguage0;
	}

void CMMFTestController::MvpsusGetSupportedSubtitleLanguagesL(RArray<TLanguage>& aAvailable)
	{
	iMemFunctionText = KGetSubLangSup();
	User::LeaveIfError(iSubtitlesAvailableErr);
	
	aAvailable.AppendL(KSubtitleLanguage0);
	aAvailable.AppendL(KSubtitleLanguage1);
	aAvailable.AppendL(KSubtitleLanguage2);
	}

void CMMFTestController::MvpsusSetSubtitleLanguageL(TLanguage aLanguage)
	{
	iMemFunctionText = KSetSubLang();
	User::LeaveIfError(iSubtitlesAvailableErr);
	
	if (KSubtitleLanguage0 != aLanguage)
		{
		User::Leave(KErrArgument);
		}
	}
#endif // SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT

//--------------------------------------------------------//
// Custom Command to Determine the Member Function called //
//--------------------------------------------------------//

TInt CMMFTestController::GetMemberFunctionCalled(TMMFMessage& aMessage)
	{
	TPtrC8 ptr;
	ptr.Set(iMemFunctionText);

	TRAPD(err, aMessage.WriteDataToClientL(ptr));
	if (err != KErrNone)
		aMessage.Complete(err);
	else
		aMessage.Complete(KFuncComplete);

	return 1;
	}


//------------------------------------------------------------------//
// Custom Command to set the Mode of the Object, default is ENormal //
//------------------------------------------------------------------//

void CMMFTestController::SetControllerMode(TMMFMessage& aMessage)
	{
	TPckgBuf<TUint8> pckg;
	TRAPD(err, aMessage.ReadData1FromClientL(pckg));

	if (err != KErrNone)
		{
		aMessage.Complete(err);
		return;
		}

	TUint8 value = pckg();

	if (value == 0)
		iControllerMode = ENormal;
	else if (value == 1)
		iControllerMode = EFailure;
	else if (value == 2)
		iControllerMode = EMemoryFailure;

	aMessage.Complete(KFuncComplete);

	}

void CMMFTestController::ThreadStackSize(TMMFMessage& aMessage)
	{
	RThread me;
	TThreadStackInfo stackInfo;
	TInt err2=me.StackInfo(stackInfo);
	if(err2==KErrNone)
		{
		TUint32 stacksize=stackInfo.iBase - stackInfo.iLimit;
		TPckg<TUint> stsize(stacksize);
		aMessage.WriteDataToClient(stsize);
		aMessage.Complete(KFuncComplete);
		}
	else
		{
		aMessage.Complete(err2);
		}
	
	}
	
void CMMFTestController::MtccCustomCommandSyncL(TMMFMessage& aMessage)
	{
	TBuf8<256> buf;
	TInt err = aMessage.ReadData1FromClient(buf);

	if (err!=KErrNone)
		{
		aMessage.Complete(err);
		return;
		}
	if (buf.Compare(KFirstParameter)!=0)
		{
		aMessage.Complete(KTestCustomCommandFailure);
		return;
		}
	err = aMessage.ReadData2FromClient(buf);
	if (err!=KErrNone)
		{
		aMessage.Complete(err);
		return;
		}
	if (buf.Compare(KSecondParameter)!=0)
		{
		aMessage.Complete(KTestCustomCommandFailure);
		return;
		}
	// no failures, so indicate success
	aMessage.Complete(KTestCustomCommandSuccess);
	}
	
void CMMFTestController::MtccCustomCommandSyncWithReturnL(TMMFMessage& aMessage)
	{
	TBuf8<256> buf;
	TInt err = aMessage.ReadData1FromClient(buf);
	if (err!=KErrNone)
		{
		aMessage.Complete(err);
		return;
		}
	if (buf.Compare(KFirstParameter)!=0)
		{
		aMessage.Complete(KTestCustomCommandFailure);
		return;
		}
	err = aMessage.ReadData2FromClient(buf);
	if (err!=KErrNone)
		{
		aMessage.Complete(err);
		return;
		}
	if (buf.Compare(KSecondParameter)!=0)
		{
		aMessage.Complete(KTestCustomCommandFailure);
		return;
		}
	err = aMessage.WriteDataToClient(KReturnParameter);
	if (err!=KErrNone)
		{
		aMessage.Complete(KTestCustomCommandFailure);
		return;
		}
	// no failures, so indicate success
	aMessage.Complete(KTestCustomCommandSuccess);
	}
	
void CMMFTestController::MtccCustomCommandRequestGeneralEventL(TMMFMessage& aMessage)
	{
	// use the same event code for both player and recorder. It doesn't matter, as, to the client utility it is just an unrecognized event
	TMMFEvent event(KMMFEventCategoryVideoPlayerGeneralError,KTestGeneralError);
	DoSendEventToClient(event);
	aMessage.Complete(KErrNone);
	}

	
void CMMFTestController::MtccCustomCommandAsyncL(TMMFMessage& aMessage)
	{
	TBuf8<256> buf;
	TInt err = aMessage.ReadData1FromClient(buf);
	if (err!=KErrNone)
		{
		iAsyncCallback->RequestCallback(aMessage, err);
		return;
		}
	if (buf.Compare(KFirstParameter)!=0)
		{
		iAsyncCallback->RequestCallback(aMessage, KTestCustomCommandFailure);
		return;
		}
	err = aMessage.ReadData2FromClient(buf);
	if (err!=KErrNone)
		{
		iAsyncCallback->RequestCallback(aMessage, err);
		return;
		}
	if (buf.Compare(KSecondParameter)!=0)
		{
		iAsyncCallback->RequestCallback(aMessage, KTestCustomCommandFailure);
		aMessage.Complete(KTestCustomCommandFailure);
		return;
		}
	// no failures, so indicate success
	iAsyncCallback->RequestCallback(aMessage, KTestCustomCommandSuccess);
	}

	
void CMMFTestController::MtccCustomCommandAsyncWithReturnL(TMMFMessage& aMessage)
	{
	TBuf8<256> buf;
	TInt err = aMessage.ReadData1FromClient(buf);
	if (err!=KErrNone)
		{
		iAsyncCallback->RequestCallback(aMessage, err);
		return;
		}
	if (buf.Compare(KFirstParameter)!=0)
		{
		iAsyncCallback->RequestCallback(aMessage, KTestCustomCommandFailure);
		return;
		}
	err = aMessage.ReadData2FromClient(buf);
	if (err!=KErrNone)
		{
		iAsyncCallback->RequestCallback(aMessage, err);
		return;
		}
	if (buf.Compare(KSecondParameter)!=0)
		{
		iAsyncCallback->RequestCallback(aMessage, KTestCustomCommandFailure);
		aMessage.Complete(KTestCustomCommandFailure);
		return;
		}
	err = aMessage.WriteDataToClient(KReturnParameter);
	if (err!=KErrNone)
		{
		aMessage.Complete(KTestCustomCommandFailure);
		return;
		}
	// no failures, so indicate success
	iAsyncCallback->RequestCallback(aMessage, KTestCustomCommandSuccess);
	}
	
	
void CMMFTestController::MtccCustomCommandSimulateReloadingL(TMMFMessage& aMessage)
	{
	// no failures, so indicate success
	iReloadingSimulator->StartReloading(TTimeIntervalMicroSeconds(KReloadingSimulatorInterval),KReloadingSimulatorIncrement);
	aMessage.Complete(KErrNone);
	}
	
void CMMFTestController::MtccCustomCommandCheckUrlL(TMMFMessage& aMessage)
	{
	if (iLastDataSourceAdded->DataSourceType() == KUidMmfUrlSource) 
		{ // video playback
		CMMFUrlSource* urlSource = STATIC_CAST(CMMFUrlSource*, iLastDataSourceAdded);
		if ((urlSource->Url().Compare(KTestURL)==0) &&
			urlSource->IAPId() == KTestIap &&
			urlSource->UseIAPId())
			aMessage.Complete(KErrNone);
		else
			aMessage.Complete(KErrArgument);
		
		}
	else if (iLastDataSinkAdded->DataSinkType() == KUidMmfUrlSink)
		{
		CMMFUrlSink* urlSink = STATIC_CAST(CMMFUrlSink*, iLastDataSinkAdded);
		if ((urlSink->Url().Compare(KTestURL)==0) &&
			urlSink->IAPId() == KTestIap &&
			urlSink->UseIAPId())
			aMessage.Complete(KErrNone);
		else
			aMessage.Complete(KErrArgument);
		}
	else
		aMessage.Complete(KErrGeneral);
	}
	
void CMMFTestController::MtccCustomCommandCheckLongUrlL(TMMFMessage& aMessage)
	{
	if (iLastDataSourceAdded->DataSourceType() == KUidMmfUrlSource) 
		{ // video playback
		CMMFUrlSource* urlSource = STATIC_CAST(CMMFUrlSource*, iLastDataSourceAdded);
		if ((urlSource->Url().Compare(KTestLongURL)==0) &&
			urlSource->IAPId() == KTestIap &&
			urlSource->UseIAPId())
			aMessage.Complete(KErrNone);
		else
			aMessage.Complete(KErrArgument);
		
		}
	else if (iLastDataSinkAdded->DataSinkType() == KUidMmfUrlSink)
		{
		CMMFUrlSink* urlSink = STATIC_CAST(CMMFUrlSink*, iLastDataSinkAdded);
		if ((urlSink->Url().Compare(KTestLongURL)==0) &&
			urlSink->IAPId() == KTestIap &&
			urlSink->UseIAPId())
			aMessage.Complete(KErrNone);
		else
			aMessage.Complete(KErrArgument);
		}
	else
		aMessage.Complete(KErrGeneral);
	}	
	
TInt CMMFTestController::StartReloading()
	{
	TMMFEvent reloading(KMMFEventCategoryAudioLoadingStarted, KErrNone);
	return DoSendEventToClient(reloading);
	}

TInt CMMFTestController::CompleteReloading()
	{
	TMMFEvent reloading(KMMFEventCategoryAudioLoadingComplete, KErrNone);
	return DoSendEventToClient(reloading);
	}

	
CMMFTestController::CAsyncCallback::CAsyncCallback()
	 : CActive(EPriorityNormal)
	 {
	 CActiveScheduler::Add(this);
	 }

CMMFTestController::CAsyncCallback::~CAsyncCallback()
	{
	delete iMessage;
	}

		
void CMMFTestController::CAsyncCallback::RequestCallback(TMMFMessage& aMessage, TInt aError) 
	{
	delete iMessage;
	iMessage = NULL;
	iMessage = new TMMFMessage(aMessage);
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status,aError);
	SetActive();
	}
	
void CMMFTestController::CAsyncCallback::RunL() 
	{
	iMessage->Complete(iStatus.Int());
	}
	
void CMMFTestController::CAsyncCallback::DoCancel()
	{
	}
	
	
CMMFTestController::CReloadingSimulator* CMMFTestController::CReloadingSimulator::NewL(CMMFTestController* aController)
	{
	CReloadingSimulator* self = new (ELeave) CReloadingSimulator(aController);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}



CMMFTestController::CReloadingSimulator::CReloadingSimulator(CMMFTestController* aController)
	 : CTimer(EPriorityNormal), iController(aController)
	 {
	 CActiveScheduler::Add(this);
	 iProgress = 100;
	 }

CMMFTestController::CReloadingSimulator::~CReloadingSimulator()
	{
	}
		
void CMMFTestController::CReloadingSimulator::StartReloading(TTimeIntervalMicroSeconds aInterval, TInt aIncrement) 
	{
	iState = ETmrStartReloading;
	iInterval = aInterval;
	iIncrement = aIncrement;
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status,KErrNone);
	SetActive();
	}
	
TInt CMMFTestController::CReloadingSimulator::LoadingProgress()
	{
	return iProgress;
	}

void CMMFTestController::CReloadingSimulator::RunL() 
	{
	switch (iState)
		{
		case ETmrStartReloading:
			iController->StartReloading();
			iProgress = 0;
			iState = ETmrReloading;
			After(I64INT(iInterval.Int64()));
			break;
		case ETmrReloading:
			iProgress+=iIncrement;
			if (iProgress>=100)
				iController->CompleteReloading();
			else
				After(I64INT(iInterval.Int64()));
			break;
		};
			
	}
	
void CMMFTestController::CReloadingSimulator::DoCancel()
	{
	}


// __________________________________________________________________________
// Exported proxy for instantiation method resolution
// Define the interface UIDs


const TImplementationProxy ImplementationTable[] =
	{
		IMPLEMENTATION_PROXY_ENTRY(KMmfTestControllerUid,	CMMFTestController::NewL)
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

	return ImplementationTable;
	}


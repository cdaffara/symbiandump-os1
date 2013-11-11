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
#include "TSI_MMFController.h"



CTSIMmfController* CTSIMmfController::NewL(void)
	{
	CTSIMmfController* self = CTSIMmfController::NewLC();
	CleanupStack::Pop();
	return self;
	}

CTSIMmfController* CTSIMmfController::NewLC(void)
	{
	CTSIMmfController* self = new (ELeave) CTSIMmfController();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CTSIMmfController::CTSIMmfController(void)
	{	
	}

CTSIMmfController::~CTSIMmfController(void)
	{
	}
                    
void CTSIMmfController::ConstructL (void)
	{

	//----------------------------//
	// Test Custom Command Parser //
	//----------------------------//

	TUid uid = {KTSIMmfCustomCommandParserUid};
	CTSIMmfCustomCommandParser* testParser = CTSIMmfCustomCommandParser::NewL(uid);
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

	CMMFVideoControllerCustomCommandParser* vidConParser = CMMFVideoControllerCustomCommandParser::NewL(*this);
	CleanupStack::PushL(vidConParser);
	AddCustomCommandParserL(*vidConParser);
	CleanupStack::Pop();//vidConParser

	//------------------------------//
	// Initial Member Function Text //
	//------------------------------//
	_LIT8(KConstructText, "CTSIMmfController Construction");
	iMemFunctionText.SetLength(KTextBufLen);
	iMemFunctionText = KConstructText;

	//-------------------------//
	// Set Default Object Node //
	//-------------------------//
	iControllerMode = ENormal;
	}

void CTSIMmfController::AddDataSourceL(MDataSource& /* aDataSource */)
	{
	_LIT8(KFuncText, "AddDataSourceL Called");
	iMemFunctionText = KFuncText;

	if (iControllerMode == EFailure)
		User::Leave(KAddDataSourceFailure);

	}

void CTSIMmfController::AddDataSinkL(MDataSink& /* aDataSink */)
	{
	_LIT8(KFuncText, "AddDataSinkL Called");
	iMemFunctionText = KFuncText;

	if (iControllerMode == EFailure)
		User::Leave(KAddDataSinkFailure);

	}

void CTSIMmfController::RemoveDataSourceL(MDataSource& /* aDataSource */)
	{
	_LIT8(KFuncText, "RemoveDataSourceL Called");
	iMemFunctionText = KFuncText;

	if (iControllerMode == EFailure)
		User::Leave(KRemoveDataSourceFailure);

	}

void CTSIMmfController::RemoveDataSinkL(MDataSink& /* aDataSink */)
	{
	_LIT8(KFuncText, "RemoveDataSinkL Called");
	iMemFunctionText = KFuncText;

	if (iControllerMode == EFailure)
		User::Leave(KRemoveDataSinkFailure);
	}

void CTSIMmfController::RemoveSourceSinkL()
	{
	_LIT8(KFuncText, "RemoveSourceSinkL Called");
	iMemFunctionText = KFuncText;
	}

void CTSIMmfController::ResetL()
	{
	_LIT8(KFuncText, "ResetL Called");
	iMemFunctionText = KFuncText;
	}

void CTSIMmfController::PrimeL()
	{
	_LIT8(KFuncText, "PrimeL Called");
	iMemFunctionText = KFuncText;

	if (iControllerMode == EFailure)
		User::Leave(KPrimeFailure);
	}

void CTSIMmfController::PlayL()
	{
	_LIT8(KFuncText, "PlayL Called");
	iMemFunctionText = KFuncText;
	}

void CTSIMmfController::PauseL()
	{
	_LIT8(KFuncText, "PauseL Called");
	iMemFunctionText = KFuncText;
	}

void CTSIMmfController::StopL()
	{
	_LIT8(KFuncText, "StopL Called");
	iMemFunctionText = KFuncText;
	}

TTimeIntervalMicroSeconds CTSIMmfController::PositionL() const
	{
	// This controller returns a default value of 16
	const TInt64& intervalSetting = 16;
	TTimeIntervalMicroSeconds* interval = new (ELeave) TTimeIntervalMicroSeconds(intervalSetting);
	return *interval; // de-reference to return
	}

void CTSIMmfController::SetPositionL(const TTimeIntervalMicroSeconds& /* aPosition */)
	{
	_LIT8(KFuncText, "SetPositionL Called");
	iMemFunctionText = KFuncText;
	}

TTimeIntervalMicroSeconds CTSIMmfController::DurationL() const
	{
	// This controller returns a default value of 36
	const TInt64& durationSetting = 36;
	TTimeIntervalMicroSeconds* duration = new (ELeave) TTimeIntervalMicroSeconds(durationSetting);
	return *duration;  // de-reference to return
	}

void CTSIMmfController::CustomCommand(TMMFMessage& aMessage)
	{
	switch (aMessage.Function())
		{
	case KDummyFunc1:
		{
		_LIT8(KFuncText, "DummyFunc1 Called");
		iMemFunctionText = KFuncText;
		aMessage.Complete(KDummyFunc1Return);
		}
		break;
	case KDummyFunc2:
		{
		_LIT8(KFuncText, "DummyFunc2 Called");
		iMemFunctionText = KFuncText;
		aMessage.Complete(KDummyFunc2Return);
		}
		break;
	case KLogFunction:
		GetMemberFunctionCalled(aMessage);
		break;
	case KModeFunction:
		SetControllerMode(aMessage);
		break;

	// New functions for Integration Testing - JW
	case KHeapAddressFunction:
		GetHeapAddress(aMessage);
		break;
	case KThreadIdFunction:
		GetThreadId(aMessage);
		break;
	case KPluginUidFunction:
		GetPluginUid(aMessage);
		break;

	default:
		aMessage.Complete(KErrNotSupported);
		}
	
	}

void CTSIMmfController::SetPrioritySettings(const TMMFPrioritySettings& /* aPrioritySettings */)
	{
	_LIT8(KFuncText, "SetPrioritySettings Called");
	iMemFunctionText = KFuncText;
	}

void CTSIMmfController::GetNumberOfMetaDataEntriesL(TInt& aNumberOfEntries)
	{
	_LIT8(KFuncText, "GetNumberOfMetaDataEntriesL Called");
	iMemFunctionText = KFuncText;

	// this controller has 4 entries
	aNumberOfEntries = 4;
	}

CMMFMetaDataEntry* CTSIMmfController::GetMetaDataEntryL(TInt aIndex)
	{ 
	_LIT8(KFuncText, "GetMetaDataEntryL Called");
	iMemFunctionText = KFuncText;

	// entry no. 1
	if (aIndex == 1)
		return CMMFMetaDataEntry::NewL(_L("AudioLevel"), _L("HIGH"));

	// entry no. 2
	else if (aIndex == 2)
		return CMMFMetaDataEntry::NewL(_L("AudioQuality"), _L("LOW"));


	// entries 3 and 4 are 'Text' and 'Text'
	return CMMFMetaDataEntry::NewL(_L("Name"), _L("Value"));
	}

TInt CTSIMmfController::SetAudioQuality(TInt /* aQuality */)
	{
	_LIT8(KFuncText, "SetAudioQuality Called");
	iMemFunctionText = KFuncText;

	return 1;
	}
		
TInt CTSIMmfController::GetAudioQuality(TInt& /* aQuality */)
	{
	_LIT8(KFuncText, "GetAudioQuality Called");
	iMemFunctionText = KFuncText;

	return 1;
	}


//-----------------------------------//
// RMMFAudioPlayDeviceCustomCommands //
//-----------------------------------//

void CTSIMmfController::MapdSetVolumeL(TInt /* aVolume */)
	{
	_LIT8(KFuncText, "MapdSetVolumeL Called");
	iMemFunctionText = KFuncText;
	}


void CTSIMmfController::MapdGetMaxVolumeL(TInt& /* aMaxVolume */)
	{
	_LIT8(KFuncText, "MapdGetMaxVolumeL Called");
	iMemFunctionText = KFuncText;
	}

void CTSIMmfController::MapdGetVolumeL(TInt& /* aVolume */)
	{
	_LIT8(KFuncText, "MapdGetVolumeL Called");
	iMemFunctionText = KFuncText;
	}

void CTSIMmfController::MapdSetVolumeRampL(const TTimeIntervalMicroSeconds& /* aRampDuration */)
	{
	_LIT8(KFuncText, "MapdSetVolumeRampL Called");
	iMemFunctionText = KFuncText;
	}

void CTSIMmfController::MapdSetBalanceL(TInt /* aBalance */)
	{
	_LIT8(KFuncText, "MapdSetBalanceL Called");
	iMemFunctionText = KFuncText;
	}

void CTSIMmfController::MapdGetBalanceL(TInt& /* aBalance */)
	{
	_LIT8(KFuncText, "MapdGetBalanceL Called");
	iMemFunctionText = KFuncText;
	}


//-------------------------------------//
// RMMFAudioRecordDeviceCustomCommands //
//-------------------------------------//

void CTSIMmfController::MardSetGainL(TInt /* aGain */)
	{
	_LIT8(KFuncText, "MardSetGainL Called");
	iMemFunctionText = KFuncText;
	}

void CTSIMmfController::MardGetMaxGainL(TInt& /* aMaxGain */)
	{
	_LIT8(KFuncText, "MardGetMaxGainL Called");
	iMemFunctionText = KFuncText;
	}

void CTSIMmfController::MardGetGainL(TInt& /* aGain */)
	{
	_LIT8(KFuncText, "MardGetGainL Called");
	iMemFunctionText = KFuncText;
	}

void CTSIMmfController::MardSetBalanceL(TInt /* aBalance */)
	{
	_LIT8(KFuncText, "MardSetBalanceL Called");
	iMemFunctionText = KFuncText;
	}

void CTSIMmfController::MardGetBalanceL(TInt& /* aBalance */)
	{
	_LIT8(KFuncText, "MardGetBalanceL Called");
	iMemFunctionText = KFuncText;
	}

//-------------------------------------------------//
// MMMFAudioPlayControllerCustomCommandImplementor //
//-------------------------------------------------//

void CTSIMmfController::MapcSetPlaybackWindowL(const TTimeIntervalMicroSeconds& /* aStart */, const TTimeIntervalMicroSeconds& /* aEnd */)
	{
	_LIT8(KFuncText, "MapcSetPlaybackWindowL Called");
	iMemFunctionText = KFuncText;
	}
	
void CTSIMmfController::MapcDeletePlaybackWindowL()
	{
	_LIT8(KFuncText, "MapcDeletePlaybackWindowL Called");
	iMemFunctionText = KFuncText;
	}
void CTSIMmfController::MapcGetLoadingProgressL(TInt& /*aPercentageComplete*/)
	{
	_LIT8(KFuncText, "MapcGetLoadingProgressL Called");
	iMemFunctionText = KFuncText;
	}


//---------------------------------------------------//
// MMMFAudioRecordControllerCustomCommandImplementor //
//---------------------------------------------------//

void CTSIMmfController::MarcGetRecordTimeAvailableL(TTimeIntervalMicroSeconds& /* aTime */)
	{
	_LIT8(KFuncText, "MarcGetRecordTimeAvailableL Called");
	iMemFunctionText = KFuncText;
	}

void CTSIMmfController::MarcSetMaxDurationL(const TTimeIntervalMicroSeconds& /* aMaxDuration */)
	{
	_LIT8(KFuncText, "MarcSetMaxDurationL Called");
	iMemFunctionText = KFuncText;
	}

void CTSIMmfController::MarcSetMaxFileSizeL(TInt /* aMaxSize */)
	{
	_LIT8(KFuncText, "MarcSetMaxFileSizeL Called");
	iMemFunctionText = KFuncText;
	}

void CTSIMmfController::MarcCropL(TBool /* aToEnd */)
	{
	_LIT8(KFuncText, "MarcCropL Called");
	iMemFunctionText = KFuncText;
	}

void CTSIMmfController::MarcAddMetaDataEntryL(const CMMFMetaDataEntry& /* aNewEntry */)
	{
	_LIT8(KFuncText, "MarcAddMetaDataEntryL Called");
	iMemFunctionText = KFuncText;
	}

void CTSIMmfController::MarcRemoveMetaDataEntryL(TInt /* aIndex */)
	{
	_LIT8(KFuncText, "MarcRemoveMetaDataEntryL Called");
	iMemFunctionText = KFuncText;
	}

void CTSIMmfController::MarcReplaceMetaDataEntryL(TInt /* aIndex */, const CMMFMetaDataEntry& /* aNewEntry */)
	{
	_LIT8(KFuncText, "MarcReplaceMetaDataEntryL Called");
	iMemFunctionText = KFuncText;
	}


	//---------------------------------------------//
	// MMMFAudioControllerCustomCommandImplementor //
	//---------------------------------------------//

void CTSIMmfController::MacSetSourceSampleRateL(TUint /* aSampleRate */)
	{
	_LIT8(KFuncText, "MacSetSourceSampleRateL Called");
	iMemFunctionText = KFuncText;
	}

void CTSIMmfController::MacSetSourceNumChannelsL(TUint /* aNumChannels */)
	{
	_LIT8(KFuncText, "MacSetSourceNumChannelsL Called");
	iMemFunctionText = KFuncText;
	}

void CTSIMmfController::MacSetSourceFormatL(TUid /* aFormatUid */)
	{
	_LIT8(KFuncText, "MacSetSourceFormatL Called");
	iMemFunctionText = KFuncText;
	}

void CTSIMmfController::MacSetSinkSampleRateL(TUint /* aSampleRate */)
	{
	_LIT8(KFuncText, "MacSetSinkSampleRateL Called");
	iMemFunctionText = KFuncText;
	}

void CTSIMmfController::MacSetSinkNumChannelsL(TUint /* aNumChannels */)
	{
	_LIT8(KFuncText, "MacSetSinkNumChannelsL Called");
	iMemFunctionText = KFuncText;
	}

void CTSIMmfController::MacSetSinkFormatL(TUid /* aFormatUid */)
	{
	_LIT8(KFuncText, "MacSetSinkFormatL Called");
	iMemFunctionText = KFuncText;
	}

void CTSIMmfController::MacSetCodecL(TFourCC /* aSourceDataType */, TFourCC /* aSinkDataType */)
	{
	_LIT8(KFuncText, "MacSetCodecL Called");
	iMemFunctionText = KFuncText;
	}

void CTSIMmfController::MacSetSourceBitRateL(TUint /* aBitRate */)
	{
	_LIT8(KFuncText, "MacSetSourceBitRateL Called");
	iMemFunctionText = KFuncText;
	}

void CTSIMmfController::MacSetSourceDataTypeL(TFourCC /* aDataType */)
	{
	_LIT8(KFuncText, "MacSetSourceDataTypeL Called");
	iMemFunctionText = KFuncText;
	}

void CTSIMmfController::MacSetSinkBitRateL(TUint /* aRate */)
	{
	_LIT8(KFuncText, "MacSetSinkBitRateL Called");
	iMemFunctionText = KFuncText;
	}

void CTSIMmfController::MacSetSinkDataTypeL(TFourCC /* aDataType */)
	{
	_LIT8(KFuncText, "MacSetSinkDataTypeL Called");
	iMemFunctionText = KFuncText;
	}

void CTSIMmfController::MacGetSourceSampleRateL(TUint& /* aRate */)
	{
	_LIT8(KFuncText, "MacGetSourceSampleRateL Called");
	iMemFunctionText = KFuncText;
	}

void CTSIMmfController::MacGetSourceBitRateL(TUint& /* aRate */)
	{
	_LIT8(KFuncText, "MacGetSourceBitRateL Called");
	iMemFunctionText = KFuncText;
	}

void CTSIMmfController::MacGetSourceNumChannelsL(TUint& /* aNumChannels */)
	{
	_LIT8(KFuncText, "MacGetSourceNumChannelsL Called");
	iMemFunctionText = KFuncText;
	}

void CTSIMmfController::MacGetSourceFormatL(TUid& /* aFormat */)
	{
	_LIT8(KFuncText, "MacGetSourceFormatL Called");
	iMemFunctionText = KFuncText;
	}

void CTSIMmfController::MacGetSourceDataTypeL(TFourCC& /* aDataType */)
	{
	_LIT8(KFuncText, "MacGetSourceDataTypeL Called");
	iMemFunctionText = KFuncText;
	}

void CTSIMmfController::MacGetSinkSampleRateL(TUint& /* aRate */)
	{
	_LIT8(KFuncText, "MacGetSinkSampleRateL Called");
	iMemFunctionText = KFuncText;
	}

void CTSIMmfController::MacGetSinkBitRateL(TUint& /* aRate */)
	{
	_LIT8(KFuncText, "MacGetSinkBitRateL Called");
	iMemFunctionText = KFuncText;
	}

void CTSIMmfController::MacGetSinkNumChannelsL(TUint& /* aNumChannels */)
	{
	_LIT8(KFuncText, "MacGetSinkNumChannelsL Called");
	iMemFunctionText = KFuncText;
	}

void CTSIMmfController::MacGetSinkFormatL(TUid& /* aFormat */)
	{
	_LIT8(KFuncText, "MacGetSinkFormatL Called");
	iMemFunctionText = KFuncText;
	}

void CTSIMmfController::MacGetSinkDataTypeL(TFourCC& /* aDataType */)
	{
	_LIT8(KFuncText, "MacGetSinkDataTypeL Called");
	iMemFunctionText = KFuncText;
	}

void CTSIMmfController::MacGetSupportedSourceSampleRatesL(RArray<TUint>& /* aSupportedRates */)
	{
	_LIT8(KFuncText, "MacGetSupportedSourceSampleRatesL Called");
	iMemFunctionText = KFuncText;
	}

void CTSIMmfController::MacGetSupportedSourceBitRatesL(RArray<TUint>& /* aSupportedRates */)
	{
	_LIT8(KFuncText, "MacGetSupportedSourceBitRatesL Called");
	iMemFunctionText = KFuncText;
	}

void CTSIMmfController::MacGetSupportedSourceNumChannelsL(RArray<TUint>& /* aSupportedChannels */)
	{
	_LIT8(KFuncText, "MacGetSupportedSourceNumChannelsL Called");
	iMemFunctionText = KFuncText;
	}

void CTSIMmfController::MacGetSupportedSourceDataTypesL(RArray<TFourCC>& /* aSupportedDataTypes */)
	{
	_LIT8(KFuncText, "MacGetSupportedSourceDataTypesL Called");
	iMemFunctionText = KFuncText;
	}

void CTSIMmfController::MacGetSupportedSinkSampleRatesL(RArray<TUint>& /* aSupportedRates */)
	{
	_LIT8(KFuncText, "MacGetSupportedSinkSampleRatesL Called");
	iMemFunctionText = KFuncText;
	}

void CTSIMmfController::MacGetSupportedSinkBitRatesL(RArray<TUint>& /* aSupportedRates */)
	{
	_LIT8(KFuncText, "MacGetSupportedSinkBitRatesL Called");
	iMemFunctionText = KFuncText;
	}

void CTSIMmfController::MacGetSupportedSinkNumChannelsL(RArray<TUint>& /* aSupportedChannels */)
	{
	_LIT8(KFuncText, "MacGetSupportedSinkNumChannelsL Called");
	iMemFunctionText = KFuncText;
	}

void CTSIMmfController::MacGetSupportedSinkDataTypesL(RArray<TFourCC>& /* aSupportedDataTypes */)
	{
	_LIT8(KFuncText, "MacGetSupportedSinkDataTypesL Called");
	iMemFunctionText = KFuncText;
	}

	//---------------------------------------------//
	// MMMFVideoControllerCustomCommandImplementor //
	//---------------------------------------------//
void CTSIMmfController::MvcGetVideoFrameSizeL(TSize& /* aVideoFrameSize */)
	{
	_LIT8(KFuncText, "MvcGetVideoFrameSizeL Called");
	iMemFunctionText = KFuncText;
	}

void CTSIMmfController::MvcSetFrameRateL(TReal32 /* aFramesPerSecond */)
	{
	_LIT8(KFuncText, "MvcSetFrameRateL Called");
	iMemFunctionText = KFuncText;
	}

void CTSIMmfController::MvcGetFrameRateL(TReal32& /* aFramesPerSecond */)
	{
	_LIT8(KFuncText, "MvcGetFrameRateL Called");
	iMemFunctionText = KFuncText;
	}

void CTSIMmfController::MvcGetFrameL(TInt /* aFrameNumber */)
	{
	_LIT8(KFuncText, "MvcGetFrameL Called");
	iMemFunctionText = KFuncText;
	}

void CTSIMmfController::MvcGetVideoDimensionL(TRect& /* aVideoDimension */)
	{
	_LIT8(KFuncText, "MvcGetVideoDimensionL Called");
	iMemFunctionText = KFuncText;
	}

void CTSIMmfController::MvcGetAudioCodecL(TFourCC& /* aCodec */)
	{
	_LIT8(KFuncText, "MvcGetAudioCodecL Called");
	iMemFunctionText = KFuncText;
	}

void CTSIMmfController::MvcGetVideoBitRateL(TInt& /* aBitRate */)
	{
	_LIT8(KFuncText, "MvcGetVideoBitRateL Called");
	iMemFunctionText = KFuncText;
	}

void CTSIMmfController::MvcGetAudioBitRateL(TInt& /* aBitRate */)
	{
	_LIT8(KFuncText, "MvcGetAudioBitRateL Called");
	iMemFunctionText = KFuncText;
	}

void CTSIMmfController::MvcSetDisplayWindowL(const TPoint& /* aPoint */, const TRect& /* aWindowRect */)
	{
	_LIT8(KFuncText, "MvcSetDisplayWindowL Called");
	iMemFunctionText = KFuncText;
	}

void CTSIMmfController::MvcAddMetaDataEntryL(const CMMFMetaDataEntry& /* aNewEntry */)
	{
	_LIT8(KFuncText, "MvcSetFrameRateL Called");
	iMemFunctionText = KFuncText;
	}

void CTSIMmfController::MvcRemoveMetaDataEntryL(TInt /* aIndex */)
	{
	_LIT8(KFuncText, "MvcAddMetaDataEntryL Called");
	iMemFunctionText = KFuncText;
	}

void CTSIMmfController::MvcReplaceMetaDataEntryL(TInt /* aIndex */, const CMMFMetaDataEntry& /* aNewEntry */)
	{
	_LIT8(KFuncText, "MvcReplaceMetaDataEntryL Called");
	iMemFunctionText = KFuncText;
	}

void CTSIMmfController::MvcSetSinkFormatL(TUid /* aVideoFormatUid */)
	{
	_LIT8(KFuncText, "MvcSetSinkFormatL Called");
	iMemFunctionText = KFuncText;
	}

void CTSIMmfController::MvcSetSinkCodecsL(TFourCC /* aVideoCodec */, TFourCC /* aAudioCodec */)
	{
	_LIT8(KFuncText, "MvcSetSinkCodecsL Called");
	iMemFunctionText = KFuncText;
	}

void CTSIMmfController::MvcGetVideoMimeTypeL(TDes8& /*aMimeType */)
	{
	_LIT8(KFuncText, "MvcGetVideoMimeTypeL Called");
	iMemFunctionText = KFuncText;
	}

//--------------------------------------------------------//
// Custom Command to Determine the Member Function called //
//--------------------------------------------------------//

TInt CTSIMmfController::GetMemberFunctionCalled(TMMFMessage& aMessage)
	{
	TPtrC8 ptr;
	ptr.Set(iMemFunctionText);

	TRAP_IGNORE(aMessage.WriteDataToClientL(ptr));
	aMessage.Complete(KFuncComplete);

	return 1;
	}

//--------------------------------------------------------//
// Custom Command to return Heap Address				  //
//--------------------------------------------------------//

TInt CTSIMmfController::GetHeapAddress(TMMFMessage& aMessage)
	{
	TPtrC8 ptr;

	RHeap& theHeap = User::Heap();
	TUint8* theHeapBase = theHeap.Base();

	// send the address back as a hex string
	iMemFunctionText.SetMax();
	iMemFunctionText.Fill(0x0);
	iMemFunctionText.Num(REINTERPRET_CAST(TUint, theHeapBase), EHex);
	ptr.Set(iMemFunctionText);


	TRAP_IGNORE(aMessage.WriteDataToClientL(ptr));
	aMessage.Complete(KFuncComplete);

	return 1;
	}

//--------------------------------------------------------//
// Custom Command to return Thread ID				  //
//--------------------------------------------------------//

TInt CTSIMmfController::GetThreadId(TMMFMessage& aMessage)
	{
	TPtrC8 ptr;

	TThreadId theId = RThread().Id();

	iMemFunctionText.SetMax();
	iMemFunctionText.Fill(0x0);
	iMemFunctionText.Num(theId, EHex);
	ptr.Set(iMemFunctionText);

	TRAP_IGNORE(aMessage.WriteDataToClientL(ptr));
	aMessage.Complete(KFuncComplete);

	return 1;
	}

//--------------------------------------------------------//
// Custom Command to return UID							  //
//--------------------------------------------------------//
TInt CTSIMmfController::GetPluginUid(TMMFMessage& aMessage)
	{
	TPtrC8 ptr;

	// send the UID back as a hex string
	iMemFunctionText.SetMax();
	iMemFunctionText.Fill(0x0);
//  we'd like to do this, but iDtor_ID_Key is private!
//	iMemFunctionText.Num(REINTERPRET_CAST(TUint, iDtor_ID_Key), EHex);

// instead, we return a code which we define ourselves -
// as this function is unique to this plugin, we can use it to 
// verify that the plugin loaded is the one asked for

	_LIT8(KPluginUIDSubstitute, "TSI_MMFController");
	iMemFunctionText = KPluginUIDSubstitute;
	ptr.Set(iMemFunctionText);

	TRAP_IGNORE(aMessage.WriteDataToClientL(ptr));
	aMessage.Complete(KFuncComplete);

	return 1;
	}

//------------------------------------------------------------------//
// Custom Command to set the Mode of the Object, default is ENormal //
//------------------------------------------------------------------//

void CTSIMmfController::SetControllerMode(TMMFMessage& aMessage)
	{
	TUint8 value = 0;
	TPtr8 modePtr(&value, 1);

	TRAPD(err, aMessage.ReadData1FromClientL(modePtr));
	if(err)
		iControllerMode = EFailure;

	if (value == 0)
		iControllerMode = ENormal;
	else if (value == 1)
		iControllerMode = EFailure;
	else if (value == 2)
		iControllerMode = EMemoryFailure;

	// NB has been adapted to exercise the ReadDataNFromClient*() methods

	aMessage.ReadData1FromClient(modePtr);
	aMessage.ReadData2FromClient(modePtr);
	TRAPD(err2, aMessage.ReadData2FromClientL(modePtr));
	
	_LIT8(KFuncText, "SetControllerMode completed successfully");
	_LIT8(KFuncErrText, "SetControllerMode completed with errors");

	if(!err2)
		iMemFunctionText = KFuncText;
	else
		iMemFunctionText = KFuncErrText;

	aMessage.Complete(KFuncComplete);

	}


// __________________________________________________________________________
// Exported proxy for instantiation method resolution
// Define the interface UIDs


const TImplementationProxy ImplementationTable[] =
	{
		IMPLEMENTATION_PROXY_ENTRY(KTSIMmfControllerUid,	CTSIMmfController::NewL)
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

	return ImplementationTable;
	}


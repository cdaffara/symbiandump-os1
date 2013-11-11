// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include <mmf/common/mmfvideo.h>
#include <mmf/server/mmffile.h>
#include <mmf/server/mmfurl.h>
#include <badesca.h>
#include <fbs.h>
#ifdef SYMBIAN_BUILD_GCE
// #include <pixelformats.h> // May not on project branch
#include "gceavailable.h"
#endif
#include "Ts_cmmfvideotestcontroller.h"
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <mmf/common/mmfstandardcustomcommandsenums.h>
#include <mmf/common/mmfstandardcustomcommandsimpl.h>
#endif

const TInt KOpenDelay =500000;
const TInt KPrepareDelay = 500000;
const TInt KFrameReadyDelay = 500000;
const TInt KDuration = 1000000;
const TInt KFrameLength = 100000;
#ifdef SYMBIAN_BUILD_GCE
const TInt KSurfaceEventDelay = 10;
#endif

const TInt KMaxVolume = 100;
const TInt KFrameWidth = 100;
const TInt KFrameHeight = 50;

const TInt KBufMimeTypeGranularity = 4;

#ifdef SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT
const TLanguage KSupportedLang0 = ELangEnglish;
const TLanguage KSupportedLang1 = ELangFrench;
const TLanguage KSupportedLang2 = ELangSpanish;
#endif //SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT

_LIT(KMmfTestControllerCategory, "CMMFTestController");

GLDEF_C void Panic(TMmfTestControllerPanic aReason)
	{
	User::Panic(KMmfTestControllerCategory, aReason);
	}

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

CMMFTestController::CMMFTestController(void) : tmpTimeIntervalMicroSecs(0),
											   tmpInt1(0),
											   tmpInt2(0),
											   tmpReal32(0)
	{
	iRecordPrepareComplete = EFalse;
	iPlayPrepareComplete = EFalse;
	// default mode for Typhoon ideo testing
	iControllerMode = EModeNotSupported;
	iRecPlayMode = EModeUnassigned;
	iDuration = TTimeIntervalMicroSeconds(KDuration);
	iFrameLength = TTimeIntervalMicroSeconds(KFrameLength);
	iPosition = TTimeIntervalMicroSeconds(0);
	iFrameSize = TSize(KFrameWidth,KFrameHeight);
	iVolume = 0;
	iGain = 10;
	iAudioEnabled = ETrue;
	iRecordTimeAvail = KMMFVideoRecordTimeAvailable;
	}

CMMFTestController::~CMMFTestController(void)
	{
#ifdef SYMBIAN_BUILD_GCE
	iDisplayIds.Close();
	if (!iSurfaceId.IsNull())
		{
		iSurfaceManager.CloseSurface(iSurfaceId);
		}
		
	iSurfaceManager.Close();
#endif	
	delete iDelayEvent;
	delete iBitmap;
	if (iAsyncCallback)
		iAsyncCallback->Cancel();
	delete iAsyncCallback;
	delete iVideoCodec;
	delete iSinkVideoMimeTypes;
#ifdef SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT
	iSubtitleWindows.Close();
	iExpectedSubtitleWindows.Close();
#endif //SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT
	}
                    
void CMMFTestController::ConstructL (void)
	{

	//----------------------------//
	// Test Custom Command Parser //
	//----------------------------//
	
	iDelayEvent = CControllerTimer::NewL(this);
	iAsyncCallback = new (ELeave) CAsyncCallback;
	CActiveScheduler::Add(iDelayEvent);

	TUid uid = {0x101f72B4};
	CMMFTestCustomCommandParser* testParser = CMMFTestCustomCommandParser::NewL(uid, *this);
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
	CleanupStack::Pop(audRecDevParser);//audRecDevParser

	CMMFVideoControllerCustomCommandParser* vidConParser = CMMFVideoControllerCustomCommandParser::NewL(*this);
	CleanupStack::PushL(vidConParser);
	AddCustomCommandParserL(*vidConParser);
	CleanupStack::Pop(vidConParser);//vidConParser

	CMMFVideoPlayControllerCustomCommandParser* vidPlayConParser = CMMFVideoPlayControllerCustomCommandParser::NewL(*this);
	CleanupStack::PushL(vidPlayConParser);
	AddCustomCommandParserL(*vidPlayConParser);
	CleanupStack::Pop(vidPlayConParser);//vidPlayConParser

	CMMFVideoRecordControllerCustomCommandParser* vidRecordConParser = CMMFVideoRecordControllerCustomCommandParser::NewL(*this);
	CleanupStack::PushL(vidRecordConParser);
	AddCustomCommandParserL(*vidRecordConParser);
	CleanupStack::Pop(vidRecordConParser);//vidRecordConParser

	CMMFVideoDRMExtCustomCommandParser* drmVideoExtParser = CMMFVideoDRMExtCustomCommandParser::NewL(*this);
	CleanupStack::PushL(drmVideoExtParser);
	AddCustomCommandParserL(*drmVideoExtParser);
	CleanupStack::Pop(drmVideoExtParser);
	
    CMMFDurationInfoCustomCommandParser2* durationInfoCustomParser = CMMFDurationInfoCustomCommandParser2::NewL(*this);
	CleanupStack::PushL(durationInfoCustomParser);
	AddCustomCommandParserL(*durationInfoCustomParser);
	CleanupStack::Pop(durationInfoCustomParser);

#ifdef SYMBIAN_BUILD_GCE 
	if(GCEAvailable())
		{
		CMMFVideoPlaySurfaceSupportCustomCommandParser* vidPlaySurfaceSupParser = CMMFVideoPlaySurfaceSupportCustomCommandParser::NewL(*this);
		CleanupStack::PushL(vidPlaySurfaceSupParser);
		AddCustomCommandParserL(*vidPlaySurfaceSupParser);
		CleanupStack::Pop(vidPlaySurfaceSupParser);

#ifdef SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT
		CMMFVideoPlaySubtitleSupportCustomCommandParser* vidPlaySubtitleSupParser = CMMFVideoPlaySubtitleSupportCustomCommandParser::NewL(*this);
		CleanupStack::PushL(vidPlaySubtitleSupParser);
		AddCustomCommandParserL(*vidPlaySubtitleSupParser);
		CleanupStack::Pop(vidPlaySubtitleSupParser);
#endif //SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT		
		}
#endif //SYMBIAN_BUILD_GCE

	//------------------------------//
	// Initial Member Function Text //
	//------------------------------//
	_LIT8(KConstructText, "CMMFTestController Construction");
	iMemFunctionText.SetLength(64);
	iMemFunctionText = KConstructText;
	
	iMemFunctionText2.SetLength(64);
	iMemFunctionText2 = KNullDesC8;
	
	//-------------------------//
	// Set Default Object Node //
	//-------------------------//
	iControllerMode = EModeNormal;

	iDelayEvent->DelayOpen(TTimeIntervalMicroSeconds(KOpenDelay));


	//-----------------//
	// mime type setup //
	//-----------------//
	_LIT8(KVideoMimeType1, "x-video1");
	_LIT8(KVideoMimeType2, "x-video2");
	
	iSinkVideoMimeTypes = new (ELeave) CDesC8ArrayFlat(KBufMimeTypeGranularity);
	iSinkVideoMimeTypes->Reset();
	iSinkVideoMimeTypes->AppendL(KVideoMimeType1);
	iSinkVideoMimeTypes->AppendL(KVideoMimeType2);
	}

// The method below need to be filled in with the correct responses

void CMMFTestController::AddDataSourceL(MDataSource& aDataSource)
	{
	//if (aDataSource.DataSourceType() != Kxxx)
	//	User::Leave(KErrNotSupported);
	
	//if (iDataSource)
	//	User::Leave(KErrAlreadyExists);

	iDataSource = &aDataSource;
	iSourceAdded = ETrue;

	_LIT8(KFuncText, "AddDataSourceL Called");
	iMemFunctionText = KFuncText;

	if (aDataSource.DataSourceType()==KUidMmfAudioInput)
		iRecPlayMode = EModeRecord;
	
#ifdef SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT
	iSubtitleAvailable = EFalse;
	if (iDataSource->DataSourceType() == KUidMmfFileSource)
		{
		CMMFFile* file = static_cast<CMMFFile*>(iDataSource);
		TPtrC filename(file->FileName());
		_LIT(KClock2FileName, "clock2");	// file with subtitle support
		if (filename == KClock2FileName)
			{
			iSubtitleAvailable = ETrue;
			}
		}
#endif //SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT
	
//	if (iControllerMode == EModeFailure)
//		User::Leave(KAddDataSourceFailure);
//	else if (iControllerMode == EModeNotSupported)
//		User::Leave(KErrNotSupported);
	
	}

void CMMFTestController::AddDataSinkL(MDataSink& aDataSink)
	{
	//if (iDataSink)
	//	User::Leave(KErrAlreadyExists);

	iDataSink = &aDataSink;
	iSinkAdded = ETrue;

	_LIT8(KFuncText, "AddDataSinkL Called");
	iMemFunctionText = KFuncText;

	if (aDataSink.DataSinkType()==KUidMmfAudioOutput)
		iRecPlayMode = EModePlay;


//	if (iControllerMode == EModeFailure)
//		User::Leave(KAddDataSourceFailure);
//	else if (iControllerMode == EModeNotSupported)
//		User::Leave(KErrNotSupported);

	}

void CMMFTestController::RemoveDataSourceL(MDataSource& /*aDataSource*/)
	{
	_LIT8(KFuncText, "RemoveDataSourceL Called");
	iMemFunctionText = KFuncText;

	if (iControllerMode == EModeFailure)
		User::Leave(KAddDataSourceFailure);
	else if (iControllerMode == EModeNotSupported)
		User::Leave(KErrNotSupported);

	}

void CMMFTestController::RemoveDataSinkL(MDataSink& /*aDataSink*/)
	{
	_LIT8(KFuncText, "RemoveDataSinkL Called");
	iMemFunctionText = KFuncText;

	if (iControllerMode == EModeFailure)
		User::Leave(KAddDataSourceFailure);
	else if (iControllerMode == EModeNotSupported)
		User::Leave(KErrNotSupported);
	}

void CMMFTestController::RemoveSourceSinkL()
	{
	_LIT8(KFuncText, "RemoveSourceSinkL Called");
	iMemFunctionText = KFuncText;

	if (iControllerMode == EModeFailure)
		User::Leave(KAddDataSourceFailure);
	else if (iControllerMode == EModeNotSupported)
		User::Leave(KErrNotSupported);
	}

void CMMFTestController::ResetL() //XXX - replacement for RemoveSourceSink()
	{
	_LIT8(KFuncText, "ResetL Called");
	iMemFunctionText = KFuncText;

	if (iControllerMode == EModeFailure)
		User::Leave(KAddDataSourceFailure);
	else if (iControllerMode == EModeNotSupported)
		User::Leave(KErrNotSupported);
	}

void CMMFTestController::PrimeL()
	{
	_LIT8(KFuncText, "PrimeL Called");
	iMemFunctionText = KFuncText;

	if (iControllerMode == EModeFailure)
		User::Leave(KAddDataSourceFailure);
	else if (iControllerMode == EModeNotSupported)
		User::Leave(KErrNotSupported);
	}

void CMMFTestController::PlayL()
	{
	_LIT8(KFuncText, "PlayL Called");
	iMemFunctionText = KFuncText;
	
	if (iRecPlayMode == EModePlay && !iPlayPrepareComplete)
		User::Leave(KErrNotReady);

	if (iRecPlayMode == EModeRecord && !iRecordPrepareComplete)
		User::Leave(KErrNotReady);

	if (iState != EStateOpen)
		User::Leave(KErrNotReady);
	if (iDelayEvent->IsActive())
		{
		User::Leave(KErrInUse);
		}
	else 
		{
		iState = EStatePlaying;
		iDelayEvent->PlayFor( iFrameLength );
		}
	}

void CMMFTestController::PauseL()
	{
	_LIT8(KFuncText, "PauseL Called");
	iMemFunctionText = KFuncText;
	
	if (iRecPlayMode == EModePlay && !iPlayPrepareComplete)
		User::Leave(KErrNotReady);

	if (iRecPlayMode == EModeRecord && !iRecordPrepareComplete)
		User::Leave(KErrNotReady);


	if (iControllerMode == EModeFailure)
		User::Leave(KAddDataSourceFailure);
	else if (iControllerMode == EModeNotSupported)
		User::Leave(KErrNotSupported);
	}

void CMMFTestController::StopL()
	{
	_LIT8(KFuncText, "StopL Called");
	iMemFunctionText = KFuncText;
	
	if (iRecPlayMode == EModePlay && !iPlayPrepareComplete)
		User::Leave(KErrNotReady);

	if (iRecPlayMode == EModeRecord && !iRecordPrepareComplete)
		User::Leave(KErrNotReady);


	if (iControllerMode == EModeFailure)
		User::Leave(KAddDataSourceFailure);
	else if (iControllerMode == EModeNotSupported)
		User::Leave(KErrNotSupported);
	}

TTimeIntervalMicroSeconds CMMFTestController::PositionL() const
	{
	//_LIT8(KFuncText, "PositionL Called");
	//iMemFunctionText = KFuncText;
	
	if (iRecPlayMode == EModePlay && !iPlayPrepareComplete)
		User::Leave(KErrNotReady);

	if (iRecPlayMode == EModeRecord && !iRecordPrepareComplete)
		User::Leave(KErrNotReady);


	if (iControllerMode == EModeFailure)
		User::Leave(KAddDataSourceFailure);
	else if (iControllerMode == EModeNotSupported)
		User::Leave(KErrNotSupported);

	return iPosition;
	}

void CMMFTestController::SetPositionL(const TTimeIntervalMicroSeconds& aPosition)
	{
	_LIT8(KFuncText, "SetPositionL Called");
	iMemFunctionText = KFuncText;
	if (aPosition < TTimeIntervalMicroSeconds(0))
		iPosition = TTimeIntervalMicroSeconds(0);
	else if (aPosition > iDuration)
		iPosition = iDuration;
	else
		iPosition = aPosition;
	}

TTimeIntervalMicroSeconds CMMFTestController::DurationL() const
	{
	if (iRecPlayMode == EModePlay && !iPlayPrepareComplete)
		User::Leave(KErrNotReady);

	return iDuration;
	}

void CMMFTestController::CustomCommand(TMMFMessage& aMessage)
	{
	//_LIT8(KFuncText, "CustomCommand Called");
	//iMemFunctionText = KFuncText;

//	TInt a = 3;
	if( ( KUidInterfaceMMFVideoPlayExt == aMessage.Destination().InterfaceId()) ||
	    ( KUidInterfaceMMFVideoRecorderExt == aMessage.Destination().InterfaceId()) )
	  {
	  aMessage.Complete(KErrNotSupported);
	  return;
	  }
	//_LIT(KAText2, "Custom Commands");
	switch (aMessage.Function())
		{
	case KDummyFunc1:
		aMessage.Complete(KDummyFunc1Return);
		break;
	case KDummyFunc2:
		aMessage.Complete(KDummyFunc2Return);
		break;
	case KLogFunction:
		GetMemberFunctionCalled(aMessage);
		break;
	case KLogFunction2:
		GetMemberFunctionCalled2(aMessage);
		break;
	case KLogIntFunction:
		GetTmpInt1Passed(aMessage);
		break;
	case KLogInt2Function:
		GetTmpInt2Passed(aMessage);
		break;
	case KLogRealFunction:
		GetTmpReal32Passed(aMessage);
		break;
	case KLogTimeInterMicroFunction:
		GetTmpTimeIntervalMicroPassed(aMessage);
		break;
	case KLogTFourCCFunction:
		GetTmpCodecPassed(aMessage);
		break;
	case KModeFunction:
		SetControllerMode(aMessage);
		break;
	case KTestSetError:
		SetTestError(aMessage);
		break;
	default:
		aMessage.Complete(KErrNotSupported);
		}
	
	}

void CMMFTestController::SetPrioritySettings(const TMMFPrioritySettings& aPrioritySettings)
	{
	_LIT8(KFuncText, "SetPrioritySettings Called");
	iMemFunctionText = KFuncText;
	
	tmpInt1 = aPrioritySettings.iPriority;
	tmpInt2 = aPrioritySettings.iPref;

	if (iControllerMode == EModeFailure)
		Panic(EAddDataSourceFailure);
	else if (iControllerMode == EModeNotSupported)
		Panic(ENotSupported);
	}

void CMMFTestController::GetNumberOfMetaDataEntriesL(TInt& aNumberOfEntries)
	{
	_LIT8(KFuncText, "GetNumberOfMetaDataEntriesL Called");
	iMemFunctionText = KFuncText;
	
	if (iRecPlayMode == EModePlay && !iPlayPrepareComplete)
		User::Leave(KErrNotReady);

	aNumberOfEntries = tmpInt1;

	if (iControllerMode == EModeFailure)
		User::Leave(KAddDataSourceFailure);
	else if (iControllerMode == EModeNotSupported)
		User::Leave(KErrNotSupported);
	}

CMMFMetaDataEntry* CMMFTestController::GetMetaDataEntryL(TInt aIndex)
	{ 
	_LIT8(KFuncText, "GetMetaDataEntryL Called");
	iMemFunctionText = KFuncText;
	
	if (iRecPlayMode == EModePlay && !iPlayPrepareComplete)
		User::Leave(KErrNotReady);

	_LIT(KInit,"Text");
	TDesC metaData = KInit;
	TDesC metaValue = KInit;

	if (aIndex == 1)
		{
		_LIT(KMetaData, "AudioLevel");
		_LIT(KMetaValue, "HIGH");
		metaData = KMetaData;
		metaValue = KMetaValue;
		}
	else if (aIndex == 2)
		{
		_LIT(KMetaData, "AudioQuality");
		_LIT(KMetaValue, "LOW");
		metaData = KMetaData;
		metaValue = KMetaValue;
		}

	const TDesC metaDataConst = metaData;
	const TDesC metaValueConst = metaValue;
	CMMFMetaDataEntry* metaDataEntry = CMMFMetaDataEntry::NewL(metaDataConst, metaValueConst);
	

	if (iControllerMode == EModeFailure)
		User::Leave(KAddDataSourceFailure);
	else if (iControllerMode == EModeNotSupported)
		User::Leave(KErrNotSupported);

	return metaDataEntry;
	}


	//---------------------------------------------//
	// MMMFVideoControllerCustomCommandImplementor //
	//---------------------------------------------//

void CMMFTestController::MvcSetFrameRateL(TReal32 aFramesPerSecond)
	{
	_LIT8(KFuncText, "MvcSetFrameRateL Called");
	iMemFunctionText = KFuncText;

	if (iControllerMode == EModeFailure)
		User::Leave(KAddDataSourceFailure);
	else if (iControllerMode == EModeNotSupported)
		User::Leave(KErrNotSupported);

	tmpReal32 = aFramesPerSecond;
	}

void CMMFTestController::MvcGetFrameRateL(TReal32& aFramesPerSecond)
	{
	_LIT8(KFuncText, "MvcGetFrameRateL Called");
	iMemFunctionText = KFuncText;
	
	if (iControllerMode == EModeFailure)
		User::Leave(KAddDataSourceFailure);
	else if (iControllerMode == EModeNotSupported)
		User::Leave(KErrNotSupported);

	aFramesPerSecond = tmpReal32;
	}

void CMMFTestController::MvcGetVideoDimensionL(TRect& aVideoDimension)
	{
	_LIT8(KFuncText, "MvcGetVideoDimensionL Called");
	iMemFunctionText = KFuncText;

	if (iControllerMode == EModeFailure)
		User::Leave(KAddDataSourceFailure);
	else if (iControllerMode == EModeNotSupported)
		User::Leave(KErrNotSupported);

	aVideoDimension.SetWidth(tmpInt1);
	aVideoDimension.SetHeight(tmpInt2);
	}


void CMMFTestController::MvcGetAudioCodecL(TFourCC& aCodec)
	{
	_LIT8(KFuncText, "MvcGetAudioCodecL Called");
	iMemFunctionText = KFuncText;
	
	if (iRecPlayMode == EModePlay && !iPlayPrepareComplete)
		User::Leave(KErrNotReady);

	if (iControllerMode == EModeFailure)
		User::Leave(KAddDataSourceFailure);
	else if (iControllerMode == EModeNotSupported)
		User::Leave(KErrNotSupported);

	aCodec = tmpCodec;
	}

void CMMFTestController::MvcGetVideoBitRateL(TInt& aBitRate)
	{
	_LIT8(KFuncText, "MvcGetVideoBitRateL Called");
	iMemFunctionText = KFuncText;

	if (iControllerMode == EModeFailure)
		User::Leave(KAddDataSourceFailure);
	else if (iControllerMode == EModeNotSupported)
		User::Leave(KErrNotSupported);

	aBitRate = tmpInt1;
	}

void CMMFTestController::MvcGetAudioBitRateL(TInt& aBitRate)
	{
	_LIT8(KFuncText, "MvcGetAudioBitRateL Called");
	iMemFunctionText = KFuncText;

	if (iControllerMode == EModeFailure)
		User::Leave(KAddDataSourceFailure);
	else if (iControllerMode == EModeNotSupported)
		User::Leave(KErrNotSupported);

	aBitRate = tmpInt1;
	}

void CMMFTestController::MvcGetVideoFrameSizeL(TSize& aSize)
	{
	aSize = iFrameSize;
	}

void CMMFTestController::MvcGetVideoMimeTypeL(TDes8& aMimeType)
	{
	if (iVideoCodec)
		{
		TPtr8 temp = iVideoCodec->Des();
		aMimeType.Copy(temp);
		}
	}


//-----------------------------------//
// RMMFAudioPlayDeviceCustomCommands //
//-----------------------------------//

void CMMFTestController::MapdSetVolumeL(TInt aVolume)
	{
	_LIT8(KFuncText, "MapdSetVolumeL Called");
	iMemFunctionText = KFuncText;
	
	iVolume = aVolume;
	}


void CMMFTestController::MapdGetMaxVolumeL(TInt& aMaxVolume)
	{
	_LIT8(KFuncText, "MapdGetMaxVolumeL Called");
	iMemFunctionText = KFuncText;

	aMaxVolume = KMaxVolume;
	}

void CMMFTestController::MapdGetVolumeL(TInt& aVolume)
	{
	_LIT8(KFuncText, "MapdGetVolumeL Called");
	iMemFunctionText = KFuncText;

	aVolume = iVolume;
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

	iGain = aGain;

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

	aGain = iGain;
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




//---------------------------------------------//
// MMMFVideoPlayControllerCustomCommandImplementor //
//---------------------------------------------//

void CMMFTestController::MvpcUpdateDisplayRegionL(const TRegion& /*aRegion*/)
	{
	_LIT8(KFuncText, "UpdateDisplayRegionL called");
	iMemFunctionText2 = KFuncText;
	}
void CMMFTestController::MvpcGetFrameL(MMMFVideoFrameMessage& aMessage)
	{
	iFrameReadyError = KErrNone;
	if (iRecPlayMode == EModePlay && !iPlayPrepareComplete)
		iFrameReadyError = KErrNotReady;

	iVideoFrameMessage = &aMessage;
	iDelayEvent->DelayFrameReady(TTimeIntervalMicroSeconds(KFrameReadyDelay));
	
	}
void CMMFTestController::MvpcGetAudioEnabledL(TBool& aEnabled)
	{
	aEnabled = iAudioEnabled;
	}
void CMMFTestController::MvpcSetDisplayWindowL(const TRect& /*aWindowRect*/, const TRect& /*aClipRect*/)
	{
	_LIT8(KFuncText, "SetDisplayWindowL called");
	iMemFunctionText = KFuncText;
	}

void CMMFTestController::MvpcDirectScreenAccessEventL(const TMMFDSAEvent aDSAEvent)
	{
	if (aDSAEvent == EAbortDSA)
		{
		_LIT8(KFuncText, "DirectScreenAccessEventL EAbortDSA");
		iMemFunctionText = KFuncText;
		}
	else if (aDSAEvent == EResumeDSA)
		{
		_LIT8(KFuncText, "DirectScreenAccessEventL EResumeDSA");
		iMemFunctionText = KFuncText;
		}
	}
	
void CMMFTestController::MvpcPlayL(const TTimeIntervalMicroSeconds& aBegin,const TTimeIntervalMicroSeconds& aEnd)
	{
	_LIT8(KFuncText, "MvpcPlayL Called");
	iMemFunctionText = KFuncText;

	if (iState != EStateOpen)
		User::Leave(KErrNotReady);
	else 
		{
		iState = EStatePlaying;
		iDelayEvent->PlayFor( TTimeIntervalMicroSeconds(aEnd.Int64() - aBegin.Int64()) );
		}
	}

void CMMFTestController::MvpcRefreshFrameL()
	{
	}

void CMMFTestController::MvpcGetLoadingProgressL(TInt& aPercentage)
	{
	aPercentage = iDelayEvent->LoadingProgress();
	}
	
void CMMFTestController::MvpcPrepare()
	{
	iPlayPrepareComplete = ETrue;
	iDelayEvent->DelayPrepare(TTimeIntervalMicroSeconds(KPrepareDelay));
	}

	
void CMMFTestController::MvpcSetRotationL(TVideoRotation aRotation)
	{
	_LIT8(KFuncText, "MvpcSetRotationL Called");
	iMemFunctionText = KFuncText;
	
	iRotation = aRotation;
	}
void CMMFTestController::MvpcGetRotationL(TVideoRotation& aRotation)
	{
	_LIT8(KFuncText, "MvpcGetRotationL Called");
	iMemFunctionText = KFuncText;
	
	aRotation = iRotation;
	}
void CMMFTestController::MvpcSetScaleFactorL(TReal32 aWidthPercentage, TReal32 aHeightPercentage, TBool aAntiAliasFiltering)
	{
	_LIT8(KFuncText, "MvpcSetScaleFactorL Called");
	iMemFunctionText = KFuncText;
	
	iWidthPercentage = aWidthPercentage;
	iHeightPercentage = aHeightPercentage;
	iAntiAliasFiltering = aAntiAliasFiltering;
	}
void CMMFTestController::MvpcGetScaleFactorL(TReal32& aWidthPercentage, TReal32& aHeightPercentage, TBool& aAntiAliasFiltering)
	{
	_LIT8(KFuncText, "MvpcGetScaleFactorL Called");
	iMemFunctionText = KFuncText;
	
	aWidthPercentage = iWidthPercentage;
	aHeightPercentage = iHeightPercentage;
	aAntiAliasFiltering = iAntiAliasFiltering;
	}
void CMMFTestController::MvpcSetCropRegionL(const TRect& aCropRegion)
	{
	_LIT8(KFuncText, "MvpcSetCropRegionL Called");
	iMemFunctionText = KFuncText;
	
	iCropRegion = aCropRegion;
	}
void CMMFTestController::MvpcGetCropRegionL(TRect& aCropRegion)
	{
	_LIT8(KFuncText, "MvpcGetCropRegionL Called");
	iMemFunctionText = KFuncText;
	
	aCropRegion = iCropRegion;
	}


//---------------------------------------------//
// MMMFVideoRecordControllerCustomCommandImplementor //
//---------------------------------------------//

void CMMFTestController::MvrcSetVideoFormatL(TUid aVideoFormatUid)
	{
	iVideoFormatUid = aVideoFormatUid;
	}
void CMMFTestController::MvrcSetVideoCodecL(const TDesC8& aVideoCodec)
	{
	HBufC8* temp = aVideoCodec.AllocL();
	
	delete iVideoCodec;
	iVideoCodec = temp;
	}
void CMMFTestController::MvrcSetAudioCodecL(TFourCC aAudioCodec)
	{
	iAudioCodec = aAudioCodec;
	}
void CMMFTestController::MvrcSetVideoBitRateL(TInt& aBitRate)
	{
	iVideoBitRate = aBitRate;
	}
void CMMFTestController::MvrcSetAudioBitRateL(TInt& aBitRate)
	{
	iAudioBitRate = aBitRate;
	}
void CMMFTestController::MvrcAddMetaDataEntryL(const CMMFMetaDataEntry& /*aNewEntry*/)
	{
	User::Leave(KErrNotSupported);
	}
void CMMFTestController::MvrcRemoveMetaDataEntryL(TInt /*aIndex*/)
	{
	User::Leave(KErrNotSupported);
	}
void CMMFTestController::MvrcReplaceMetaDataEntryL(TInt /*aIndex*/, const CMMFMetaDataEntry& /*aNewEntry*/)
	{
	User::Leave(KErrNotSupported);
	}
void CMMFTestController::MvrcSetMaxFileSizeL(TInt aMaxFileSize)
	{
	iMaxFileSize = aMaxFileSize;
	}

void CMMFTestController::MvrcSetVideoFrameSizeL(TSize aFrameSize)
	{
	iFrameSize = aFrameSize;
	}

void CMMFTestController::MvrcSetAudioEnabledL(TBool aEnabled)
	{
	iAudioEnabled = aEnabled;
	}

void CMMFTestController::MvrcPrepareL()
	{
	User::LeaveIfError(iPrepareLeave);
	iRecordPrepareComplete = ETrue;
	iDelayEvent->DelayPrepare(TTimeIntervalMicroSeconds(KPrepareDelay));
	}

void CMMFTestController::MvrcSetCameraHandleL(TInt aCameraHandle)
	{
	iCameraHandle = aCameraHandle;
	}

void CMMFTestController::MvrcGetRecordTimeAvailableL(TTimeIntervalMicroSeconds& aTime)
	{
	aTime = iRecordTimeAvail;
	}

void CMMFTestController::MvrcGetSupportedSinkAudioTypesL(RArray<TFourCC>& /*aDataTypes*/)
	{
	}

void CMMFTestController::MvrcGetSupportedSinkVideoTypesL(CDesC8Array& aDataTypes)
	{
	aDataTypes.Reset();
	
	for (TInt i = 0; i < iSinkVideoMimeTypes->Count(); i++)
		{
		aDataTypes.AppendL((*iSinkVideoMimeTypes)[i]);
		}
	}

void CMMFTestController::MvrcGetAudioEnabledL(TBool& aEnabled)
	{
	aEnabled = iAudioEnabled;
	}

//---------------------------------------------//
// MMMFVideoDRMExtCustomCommandImplementor		//
//---------------------------------------------//
void CMMFTestController::MvdeGetFrameL(MMMFVideoFrameMessage& aMessage, TIntent aIntent)
	{
	iFrameReadyError = KErrNone;
	if (iRecPlayMode == EModePlay && !iPlayPrepareComplete)
		iFrameReadyError = KErrNotReady;

	if (iDataSource->DataSourceType() == KUidMmfFileSource)
		{
		CMMFFile* file = static_cast<CMMFFile*>(iDataSource);

		TInt err = file->ExecuteIntent(aIntent);
		if (err!=KErrNone) // file not open
			{
			if (err == KErrNotReady)
				{
				file->SourcePrimeL();
				iFrameReadyError = file->ExecuteIntent(aIntent);
				file->SourceStopL();
				}
			else 
				{
				iFrameReadyError = err;
				}
			}
		}

	iVideoFrameMessage = &aMessage;
	iDelayEvent->DelayFrameReady(TTimeIntervalMicroSeconds(KFrameReadyDelay));
	}

#ifdef SYMBIAN_BUILD_GCE

void CMMFTestController::CreateSurfaceL()
	{
	User::LeaveIfError(iSurfaceManager.Open());
	
	RSurfaceManager::TSurfaceCreationAttributesBuf bf;
	RSurfaceManager::TSurfaceCreationAttributes& b=bf();
	b.iSize.iWidth = 320;
	b.iSize.iHeight = 240;
	b.iBuffers = 1;
	b.iPixelFormat = EUidPixelFormatXRGB_8888;	
	b.iStride = 320 * 4;
	b.iOffsetToFirstBuffer = 0;
	b.iAlignment=4;
	b.iContiguous=ETrue;
	b.iMappable=ETrue;
	
	User::LeaveIfError(iSurfaceManager.CreateSurface(bf, iSurfaceId));
	}

void CMMFTestController::MvtccCustomCommandSimulateSurfaceCreatedL(TMMFMessage& aMessage)
	{
	// no failures, so indicate success
	iDelayEvent->DelaySurfaceCreated(TTimeIntervalMicroSeconds(KSurfaceEventDelay));
	aMessage.Complete(KErrNone);
	}

void CMMFTestController::MvtccCustomCommandSimulateParaChangedL(TMMFMessage& aMessage)
	{
	// no failures, so indicate success
	iDelayEvent->DelayParaChanged(TTimeIntervalMicroSeconds(KSurfaceEventDelay));
	aMessage.Complete(KErrNone);
	}

void CMMFTestController::MvtccCustomCommandSimulateRemoveSurfaceL(TMMFMessage& aMessage)
	{
	// no failures, so indicate success
	iDelayEvent->DelayRemoveSurface(TTimeIntervalMicroSeconds(KSurfaceEventDelay));
	aMessage.Complete(KErrNone);
	}
		
void CMMFTestController::SurfaceCreated()
	{
	if (!iSurfaceId.IsNull())
   		{
   		iSurfaceId2 = iSurfaceId;
   		}
	
   	TRAPD(err, CreateSurfaceL());
	
	if(err == KErrNone)
		{
		iSurfaceAvailable = ETrue;
		TMMFEvent created(KMMFEventCategoryVideoSurfaceCreated, KErrNone);
		SendEventToClient(created);
		}
	}

void CMMFTestController::ParaChanged()
	{
	TMMFEvent changed(KMMFEventCategoryVideoSurfaceParametersChanged, KErrNone);
	SendEventToClient(changed);
	}

void CMMFTestController::RemoveSurface()
	{
	TMMFEvent remove(KMMFEventCategoryVideoRemoveSurface, KErrNone);
	SendEventToClient(remove);
	}

//----------------------------------------------------------//
// MMMFVideoPlaySurfaceSupportCustomCommandImplementor		//
//----------------------------------------------------------//	
void CMMFTestController::MvpssUseSurfacesL()
	{
	_LIT8(KFuncText, "MvpssUseSurfacesL Called");
	iMemFunctionText = KFuncText;
	
	iSurfaceSupported = ETrue;
	}

void CMMFTestController::MvpssGetSurfaceParametersL(TSurfaceId& aSurfaceId, 
									TRect& aCropRect, TVideoAspectRatio& aPixelAspectRatio)
	{
	_LIT8(KFuncText, "MvpssGetSurfaceParametersL Called");
	iMemFunctionText2 = KFuncText;
	
	if (!iSurfaceSupported)
		{
		User::Leave(KErrNotSupported);
		}
		
	TRect cropRect(0, 0, 320, 240);
	TVideoAspectRatio par(1,2);
	aSurfaceId = iSurfaceId;
	aCropRect = cropRect;
	aPixelAspectRatio = par;
	}

void CMMFTestController::MvpssSurfaceRemovedL(const TSurfaceId& aSurfaceId)
	{
	_LIT8(KFuncText, "MvpssSurfaceRemovedL Called");
	iMemFunctionText = KFuncText;
	
	if (!iSurfaceSupported)
		{
		User::Leave(KErrNotSupported);
		}

	if(aSurfaceId == iSurfaceId2)
		{
		iSurfaceManager.CloseSurface(iSurfaceId2);
		iSurfaceId2 = TSurfaceId::CreateNullId();
		return;
		}
	
	if(aSurfaceId != iSurfaceId)
		{
		User::Leave(KErrNotReady);
		}
	
	iSurfaceManager.CloseSurface(iSurfaceId);
	iSurfaceId = TSurfaceId::CreateNullId();
	iSurfaceManager.Close();
	iSurfaceAvailable = EFalse;
	}
		
#endif // SYMBIAN_BUILD_GCE

#ifdef SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT
//--------------------------------------------------------//
// 	MMMFVideoPlaySubtitleSupportCustomCommandImplementor   //
//--------------------------------------------------------//
void CMMFTestController::MvpsusGetCrpParametersL(TInt aWindowId, TWsGraphicId& aId, TRect& aCrpRect)
	{
	const TInt dummyCrpId = 1234;

	++iGetCrpParamCallCount;

	if (iSubtitleEnabled)
		{
		TInt pos = iSubtitleWindows.Find(aWindowId, CMMFTestController::FindWindowByWindowId);
		User::LeaveIfError(pos);
		// set CRP rect to the window clip rect in test code
		aCrpRect = iSubtitleWindows[pos].iWindowClipRect;
		// use a dummy id for CRP
		aId.Set(dummyCrpId);
		}
	else
		{
		User::Leave(KErrNotReady);
		}
	}

void CMMFTestController::MvpsusAddSubtitleConfigL(const TMMFSubtitleWindowConfig& aConfig)
	{
	++iAddSubtitleConfigCallCount;

	if (iSubtitleEnabled)
		{
		// test case expect maximum of 2 windows are supported
		if (iSubtitleWindows.Count() >= 2)
			{
			User::LeaveIfError(KErrInUse);
			}

		TInt pos = iSubtitleWindows.Find(aConfig.iWindowId, CMMFTestController::FindWindowByWindowId);
		if (pos >= 0)
			{
			User::LeaveIfError(KErrAlreadyExists);
			}
		
		TInt expectedPos = iExpectedSubtitleWindows.Find(aConfig.iWindowId, CMMFTestController::FindWindowByWindowId);
		User::LeaveIfError(expectedPos);
		TMMFSubtitleWindowConfig expectedConfig = iExpectedSubtitleWindows[expectedPos];
		if (aConfig.iWindowId != expectedConfig.iWindowId ||
			aConfig.iWindowClipRect != expectedConfig.iWindowClipRect ||
			aConfig.iDisplayMode != expectedConfig.iDisplayMode ||
			aConfig.iRotation != expectedConfig.iRotation)
			{
			User::Panic(_L("TestController::MvpsusAddSubtitleConfigL"), KErrArgument);
			}
		
		iSubtitleWindows.AppendL(aConfig);
		CrpReady(aConfig.iWindowId);
		}
	else
		{
		User::Leave(KErrNotReady);
		}
	}

TBool CMMFTestController::FindWindowByWindowId(const TInt* aWindowId, const TMMFSubtitleWindowConfig& aData)
	{
	TBool ret = EFalse;
	if (aWindowId)
		{
		ret = (*aWindowId == aData.iWindowId);
		}
	return ret;
	}


void CMMFTestController::MvpsusRemoveSubtitleConfigL(TInt aWindowId)
	{
	++iRemoveSubtitleConfigCallCount;

	if (iSubtitleEnabled)
		{
		TInt pos = iSubtitleWindows.Find(aWindowId, CMMFTestController::FindWindowByWindowId);
		User::LeaveIfError(pos);
		iSubtitleWindows.Remove(pos);
		}
	else
		{
		User::Leave(KErrNotReady);
		}
	}

void CMMFTestController::MvpsusUpdateSubtitleConfigL(const TMMFSubtitleWindowConfig& aConfig)
	{
	++iUpdateSubtitleConfigCallCount;
	
	if (iSubtitleEnabled)
		{
		TInt pos = iSubtitleWindows.Find(aConfig.iWindowId, CMMFTestController::FindWindowByWindowId);
		User::LeaveIfError(pos);
		
		TInt expectedPos = iExpectedSubtitleWindows.Find(aConfig.iWindowId, CMMFTestController::FindWindowByWindowId);
		User::LeaveIfError(expectedPos);
		TMMFSubtitleWindowConfig expectedConfig = iExpectedSubtitleWindows[expectedPos];
		if (aConfig.iWindowId != expectedConfig.iWindowId ||
			aConfig.iWindowClipRect != expectedConfig.iWindowClipRect ||
			aConfig.iDisplayMode != expectedConfig.iDisplayMode ||
			aConfig.iRotation != expectedConfig.iRotation)
			{
			User::Panic(_L("TestController::MvpsusAddSubtitleConfigL"), KErrArgument);
			}
		
		// update subtitle config
		iSubtitleWindows.Remove(pos);
		iSubtitleWindows.AppendL(aConfig);
		CrpReady(aConfig.iWindowId);
		}
	else
		{
		User::Leave(KErrNotReady);
		}
	}

void CMMFTestController::MvpsusGetSubtitlesAvailableL(TBool& aAvailable)
	{
	aAvailable = iSubtitleAvailable;
	}

void CMMFTestController::MvpsusDisableSubtitlesL()
	{
	++iDisableSubtitleCallCount;

	iSubtitleEnabled = EFalse;
	iSubtitleWindows.Reset();
	}

void CMMFTestController::MvpsusEnableSubtitlesL()
	{
	++iEnableSubtitleCallCount;

	if (iSubtitleAvailable)
		{
		iSubtitleEnabled = ETrue;
		iSubtitleLanguage = KSupportedLang0;
		}
	else
		{
		User::Leave(KErrNotFound);
		}
	}

void CMMFTestController::MvpsusGetSubtitleLanguageL(TLanguage& aLanguage)
	{
	if (!iLangNotAvailable)
		{
		aLanguage = iSubtitleLanguage;
		}
	else
		{
		User::Leave(KErrNotSupported);
		}
	}

void CMMFTestController::MvpsusGetSupportedSubtitleLanguagesL(RArray<TLanguage>& aAvailable)
	{
	if (!iLangNotAvailable)
		{
		aAvailable.AppendL(KSupportedLang0);
		aAvailable.AppendL(KSupportedLang1);
		aAvailable.AppendL(KSupportedLang2);
		}
	else
		{
		User::Leave(KErrNotSupported);
		}
	}

void CMMFTestController::MvpsusSetSubtitleLanguageL(TLanguage aLanguage)
	{
	if (!iLangNotAvailable)
		{
		if (aLanguage == KSupportedLang0 || aLanguage == KSupportedLang1 || aLanguage == KSupportedLang2)
			{
			iSubtitleLanguage = aLanguage;
			}
		}
	else
		{
		User::Leave(KErrNotSupported);
		}
	}

void CMMFTestController::CrpReady(TInt aWindowId)
	{
	++iCrpReadyEventCount;
	
	TMMFEvent crpReady(KMMFEventCategoryVideoSubtitleCrpReady, aWindowId);
	SendEventToClient(crpReady);
	}

void CMMFTestController::MvtccCustomCommandSetSubtitleDisplayCheckL(TMMFMessage& aMessage)
	{
	TPckgBuf<TMMFSubtitleWindowConfig> pckg;
	TInt err = aMessage.ReadData1FromClient(pckg);

	if (KErrNone != err)
		{
		aMessage.Complete(err);
		return;
		}
	
	TMMFSubtitleWindowConfig config = pckg();

	// Add the expected window config, or replace if already exists
	TInt pos = iExpectedSubtitleWindows.Find(config.iWindowId, CMMFTestController::FindWindowByWindowId);
	if (pos >= 0)
		{
		iExpectedSubtitleWindows.Remove(pos);
		}
	iExpectedSubtitleWindows.AppendL(config);
	
	aMessage.Complete(KErrNone);
	}

// simulate CRP ready event with unexpected window id
void CMMFTestController::MvtccCustomCommandSimulateCrpReadyEvent(TMMFMessage& aMessage)
	{
	TPckgBuf<TInt> pckg;
	TInt err = aMessage.ReadData1FromClient(pckg);

	if (KErrNone != err)
		{
		aMessage.Complete(err);
		return;
		}
	
	CrpReady(pckg());
	aMessage.Complete(KErrNone);
	}

void CMMFTestController::MvtccCustomCommandSetLanguageNotAvail(TMMFMessage& aMessage)
	{
	iLangNotAvailable = ETrue;
	aMessage.Complete(KErrNone);
	}

void CMMFTestController::MvtccCustomCommandGetSubtitleCallCount(TMMFMessage& aMessage)
	{
	TPckgBuf<TMMFTestGetSubtitleCountType> pckg;
	TInt err = aMessage.ReadData1FromClient(pckg);

	if (KErrNone != err)
		{
		aMessage.Complete(err);
		return;
		}
	
	TInt count = 0;
	TMMFTestGetSubtitleCountType type = pckg();
	switch (type)
		{
	case EMMFTestEnableSubtitleCallCount:
		count = iEnableSubtitleCallCount;
		break;
	case EMMFTestDisableSubtitleCallCount:
		count = iDisableSubtitleCallCount;
		break;
	case EMMFTestAddSubtitleConfigCallCount:
		count = iAddSubtitleConfigCallCount;
		break;
	case EMMFTestRemoveSubtitleConfigCallCount:
		count = iRemoveSubtitleConfigCallCount;
		break;
	case EMMFTestGetCrpParamCallCount:
		count = iGetCrpParamCallCount;
		break;
	case EMMFTestCrpReadyEventCount:
		count = iCrpReadyEventCount;
		break;
	case EMMFTestUpdateSubtitleConfigCallCount:
		count = iUpdateSubtitleConfigCallCount;
		break;
	default:
		err = KErrNotSupported;
		}

	if (KErrNone == err)
		{
		TPckgBuf<TInt> retPckg(count);
		TRAP(err, aMessage.WriteDataToClientL(retPckg));
		}
	
	aMessage.Complete(err);
	}

#endif //SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT


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

//--------------------------------------------------------//
// Custom Command to Determine the Member Function called //
//--------------------------------------------------------//
TInt CMMFTestController::GetMemberFunctionCalled2(TMMFMessage& aMessage)
	{
	TPtrC8 ptr;
	ptr.Set(iMemFunctionText2);

	TRAPD(err, aMessage.WriteDataToClientL(ptr));
	if (err != KErrNone)
		aMessage.Complete(err);
	else
		aMessage.Complete(KFuncComplete);

	// reset text after retrieved by client. This is needed where the same function needs
	// to be tested in the same test step.
	iMemFunctionText2 = KNullDesC8;
	
	return 1;
	}

//--------------------------------------------------------//
// Custom Command to Determine the Pamaters passed		  //
//--------------------------------------------------------//
TInt CMMFTestController::GetTmpInt1Passed(TMMFMessage& aMessage)
	{
	TPckgBuf<TInt> pckg;
	pckg() = tmpInt1;

	TRAPD(err, aMessage.WriteDataToClientL(pckg));
	if (err != KErrNone)
		aMessage.Complete(err);
	else
		aMessage.Complete(KFuncComplete);

	return 1;
	}

//--------------------------------------------------------//
// Custom Command to Determine the Pamaters passed		  //
//--------------------------------------------------------//
TInt CMMFTestController::GetTmpInt2Passed(TMMFMessage& aMessage)
	{
	TPckgBuf<TInt> pckg;
	pckg() = tmpInt2;

	TRAPD(err, aMessage.WriteDataToClientL(pckg));
	if (err != KErrNone)
		aMessage.Complete(err);
	else
		aMessage.Complete(KFuncComplete);

	return 1;
	}

//--------------------------------------------------------//
// Custom Command to Determine the Pamaters passed		  //
//--------------------------------------------------------//
TInt CMMFTestController::GetTmpReal32Passed(TMMFMessage& aMessage)
	{
	TPckgBuf<TReal32> pckg;
	pckg() = tmpReal32;

	TRAPD(err, aMessage.WriteDataToClientL(pckg));
	if (err != KErrNone)
		aMessage.Complete(err);
	else
		aMessage.Complete(KFuncComplete);

	return 1;
	}

//--------------------------------------------------------//
// Custom Command to Determine the Pamaters passed		  //
//--------------------------------------------------------//
TInt CMMFTestController::GetTmpCodecPassed(TMMFMessage& aMessage)
	{
	TPckgBuf<TFourCC> pckg;
	pckg() = tmpCodec;
	
	TRAPD(err, aMessage.WriteDataToClientL(pckg));
	if (err != KErrNone)
		aMessage.Complete(err);
	else
		aMessage.Complete(KFuncComplete);

	return 1;
	}

//--------------------------------------------------------//
// Custom Command to Determine the Pamaters passed		  //
//--------------------------------------------------------//
TInt CMMFTestController::GetTmpTimeIntervalMicroPassed(TMMFMessage& aMessage)
	{
	TPckgBuf<TTimeIntervalMicroSeconds> pckg;
	pckg() = tmpTimeIntervalMicroSecs;
	
	TRAPD(err, aMessage.WriteDataToClientL(pckg));
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
		iControllerMode = EModeNormal;
	else if (value == 1)
		iControllerMode = EModeFailure;
	else if (value == 2)
		iControllerMode = EModeMemoryFailure;
	else if (value == 3)
		iControllerMode = EModeNotSupported;

	aMessage.Complete(KFuncComplete);

	}

//------------------------------------------------------------------//
// Custom Command to set a fake error value							//
//------------------------------------------------------------------//
void CMMFTestController::SetTestError(TMMFMessage& aMessage)
	{
	TPckgBuf<TInt> errorValPckg;
	TPckgBuf<TInt> errorTypePckg;
	TInt err = KErrNone;

	// Read error value
	TRAP(err, aMessage.ReadData1FromClientL(errorValPckg));
	if (err != KErrNone)
		{
		aMessage.Complete(err);
		return;
		}

	// Read error type
	TRAP(err, aMessage.ReadData2FromClientL(errorTypePckg));
	if (err != KErrNone)
		{
		aMessage.Complete(err);
		return;
		}

	const TInt errorVal = errorValPckg();
	TestControllerErrorType errorType = (TestControllerErrorType)errorTypePckg();
	switch (errorType)
		{
		case EMmfTC_OpenError:
			iOpenError = errorVal;
			break;
		case EMmfTC_PrepareError:
			iPrepareError = errorVal;
			break;
		case EMmfTC_PlayError:
			iPlayError = errorVal;
			break;
		case EMmfTC_PrepareLeave:
			iPrepareLeave = errorVal;
			break;
		default:
			aMessage.Complete(KErrNotFound);
			return;
		}

	aMessage.Complete(KErrNone);
	}


TInt CMMFTestController::SendEventToClient(const TMMFEvent& aEvent)
	{
	//send event to client...
	TInt result = DoSendEventToClient(aEvent);
	return result;
	}

void CMMFTestController::OpenComplete()
	{
	iState = EStateOpen;
	TMMFEvent opencomplete(KMMFEventCategoryVideoOpenComplete, iOpenError);
	SendEventToClient(opencomplete);
	}


void CMMFTestController::PlayComplete()
	{
	iPosition = TTimeIntervalMicroSeconds(iPosition.Int64() + iFrameLength.Int64());
	if (iPosition < iDuration)
		iDelayEvent->PlayFor(iFrameLength);
	else
		{
		iPosition = 0;
		TMMFEvent playcomplete(KMMFEventCategoryPlaybackComplete, iPlayError);
		SendEventToClient(playcomplete);
		}

	}

void CMMFTestController::PrepareComplete()
	{
	TMMFEvent prepareComplete(KMMFEventCategoryVideoPrepareComplete, iPrepareError);
	SendEventToClient(prepareComplete);
	}
	
void CMMFTestController::FrameReady()
	{
	if (iFrameReadyError != KErrNone)
		{
		iVideoFrameMessage->FrameReady(iFrameReadyError);
		}
	else
		{
		TInt err = iVideoFrameMessage->GetBitmap().Resize(iFrameSize);
		iVideoFrameMessage->FrameReady(err);
		}
	}
	
void CMMFTestController::StartReloading()
	{
	TMMFEvent reloading(KMMFEventCategoryVideoLoadingStarted, KErrNone);
	SendEventToClient(reloading);
	}

void CMMFTestController::CompleteReloading()
	{
	TMMFEvent reloading(KMMFEventCategoryVideoLoadingComplete, KErrNone);
	SendEventToClient(reloading);
	}

CMMFTestController::CControllerTimer* CMMFTestController::CControllerTimer::NewL(CMMFTestController* aController)
	{
	CControllerTimer* self = new (ELeave) CControllerTimer(aController);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CMMFTestController::CControllerTimer::CControllerTimer(CMMFTestController* aController)
	:CTimer(EPriorityStandard)
	{
	iController = aController;
	iPercentageComplete = 100;
	}

void CMMFTestController::CControllerTimer::DelayOpen(TTimeIntervalMicroSeconds aDelay)
	{
	ASSERT(iController);
	iType = ETmrOpenComplete;
	After(I64INT(aDelay.Int64()));
	}

void CMMFTestController::CControllerTimer::DelayPrepare(TTimeIntervalMicroSeconds aDelay)
	{
	ASSERT(iController);
	iType = ETmrPrepareComplete;
	After(I64INT(aDelay.Int64()));
	}
	
void CMMFTestController::CControllerTimer::DelayFrameReady(TTimeIntervalMicroSeconds aDelay)
	{
	ASSERT(iController);
	iType = ETmrFrameReady;
	After(I64INT(aDelay.Int64()));
	}



void CMMFTestController::CControllerTimer::PlayFor(TTimeIntervalMicroSeconds aDelay)
	{
	ASSERT(iController);
	iType = ETmrPlay;
	After(I64INT(aDelay.Int64()));
	}

void CMMFTestController::CControllerTimer::DelaySurfaceCreated(TTimeIntervalMicroSeconds aDelay)
	{
	ASSERT(iController);
	iType = ETmrSurfaceCreated;
	After(I64INT(aDelay.Int64()));
	}
	
void CMMFTestController::CControllerTimer::DelayParaChanged(TTimeIntervalMicroSeconds aDelay)
	{
	ASSERT(iController);
	iType = ETrmParaChanged;
	After(I64INT(aDelay.Int64()));
	}

void CMMFTestController::CControllerTimer::DelayRemoveSurface(TTimeIntervalMicroSeconds aDelay)
	{
	ASSERT(iController);
	iType = ETrmRemoveSurface;
	After(I64INT(aDelay.Int64()));
	}
		
void CMMFTestController::CControllerTimer::StartReloading(const TTimeIntervalMicroSeconds& aInterval,TInt aPercentageIncrement)
	{
	ASSERT(iController);
	iInterval = aInterval;
	iPercentageIncrement = aPercentageIncrement;
	iType = ETmrStartReloading;
	After(I64INT(aInterval.Int64()));
	}
	
TInt CMMFTestController::CControllerTimer::LoadingProgress()
	{
	return iPercentageComplete;
	}

void CMMFTestController::CControllerTimer::RunL()
	{
	ASSERT(iController);
	switch(iType) 
		{
		case ETmrOpenComplete :
			iController->OpenComplete();
			break;
		case ETmrPrepareComplete :
			iController->PrepareComplete();
			break;
		case ETmrPlay: 
			iController->PlayComplete();
			break;
		case ETmrFrameReady :
			iController->FrameReady();
			break;
		case ETmrStartReloading:
			iController->StartReloading();
			iPercentageComplete = 0;
			iType = ETmrReloading;
			After(I64INT(iInterval.Int64()));
			break;
		case ETmrReloading:
			iPercentageComplete+=iPercentageIncrement;
			if (iPercentageComplete>=100)
				iController->CompleteReloading();
			else
				After(I64INT(iInterval.Int64()));
			break;
#ifdef SYMBIAN_BUILD_GCE
		case ETmrSurfaceCreated :
			iController->SurfaceCreated();
			break;
		case ETrmParaChanged: 
			iController->ParaChanged();
			break;
		case ETrmRemoveSurface :
			iController->RemoveSurface();
			break;
#endif // SYMBIAN_BUILD_GCE
		default:
			break;
		}
	}
	
	
void CMMFTestController::MvtccCustomCommandSyncL(TMMFMessage& aMessage)
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
	
void CMMFTestController::MvtccCustomCommandSyncWithReturnL(TMMFMessage& aMessage)
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
	
void CMMFTestController::MvtccCustomCommandRequestGeneralEventL(TMMFMessage& aMessage)
	{
	// use the same event code for both player and recorder. It doesn't matter, as, to the client utility it is just an unrecognized event
	TMMFEvent event(KMMFEventCategoryVideoPlayerGeneralError,KTestGeneralError);
	DoSendEventToClient(event);
	aMessage.Complete(KErrNone);
	}

	
void CMMFTestController::MvtccCustomCommandAsyncL(TMMFMessage& aMessage)
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

	
void CMMFTestController::MvtccCustomCommandAsyncWithReturnL(TMMFMessage& aMessage)
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
	
	
void CMMFTestController::MvtccCustomCommandSimulateReloadingL(TMMFMessage& aMessage)
	{
	// no failures, so indicate success
	iDelayEvent->StartReloading(TTimeIntervalMicroSeconds(10),10);
	aMessage.Complete(KErrNone);
	}
	
void CMMFTestController::MvtccCustomCommandCheckUrlL(TMMFMessage& aMessage)
	{
	if (iDataSource->DataSourceType() == KUidMmfUrlSource) 
		{ // video playback
		CMMFUrlSource* urlSource = STATIC_CAST(CMMFUrlSource*, iDataSource);
		if ((urlSource->Url().Compare(KTestURL)==0) &&
			urlSource->IAPId() == KTestIap &&
			urlSource->UseIAPId())
			aMessage.Complete(KErrNone);
		else
			aMessage.Complete(KErrArgument);
		
		}
	else if (iDataSink->DataSinkType() == KUidMmfUrlSink)
		{
		CMMFUrlSink* urlSink = STATIC_CAST(CMMFUrlSink*, iDataSink);
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
	
TInt CMMFTestController::GetDurationInfo(TMMFDurationInfo& aDurationInfo)
	{
	aDurationInfo = EMMFDurationInfoInfinite;
	return KErrNone;
	}

// __________________________________________________________________________
// Exported proxy for instantiation method resolution
// Define the interface UIDs


const TImplementationProxy ImplementationTable[] =
	{
		IMPLEMENTATION_PROXY_ENTRY(KMmfVideoTestControllerUid,	CMMFTestController::NewL)
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

	return ImplementationTable;
	}
	

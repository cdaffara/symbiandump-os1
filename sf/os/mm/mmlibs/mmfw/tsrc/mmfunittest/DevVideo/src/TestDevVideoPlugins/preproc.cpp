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

#include "preproc.h"
#include "../TestDevVideoPlayTestData.h"

_LIT(KDevVideoPreProcPanicCategory, "DevVideoPreProcessor");
void DevVideoPreProcPanic(TInt aReason)
	{
	User::Panic(KDevVideoPreProcPanicCategory, aReason);
	}

CMMFVideoPreProcHwDevice* CMMFTestVideoPreProcHwDevice::NewL(TAny* /*aInitParams*/)
	{
	CMMFTestVideoPreProcHwDevice* s = new(ELeave) CMMFTestVideoPreProcHwDevice;
	return (STATIC_CAST(CMMFVideoPreProcHwDevice*, s));
	}

CMMFTestVideoPreProcHwDevice::CMMFTestVideoPreProcHwDevice()
	:iRecPosition(KTestRecordPosition)
	{
	}

CMMFTestVideoPreProcHwDevice::~CMMFTestVideoPreProcHwDevice()
	{
	iInputVidFormats.Reset();
	iInputVidFormats.Close();

	iOutputVidFormats.Reset();
	iOutputVidFormats.Close();

	iScaleFactors.Reset();
	iScaleFactors.Close();

	iCombinations.Reset();
	iCombinations.Close();
	}

TAny* CMMFTestVideoPreProcHwDevice::CustomInterface(TUid aInterface)
	{
	if (aInterface == KUidCustomInterfaceFour)
		{
		return this;//just want to return something non-null!
		}
	else
		{
		return NULL;
		}
	}

CPreProcessorInfo* CMMFTestVideoPreProcHwDevice::PreProcessorInfoLC()
	{
	// construct array of test types
	for (TUint i = 0; i < KTestPostProcInfoCount; i++)
		{
		// append the input video formats
		TUncompressedVideoFormat vid = KTestPreProcInfoInputFormatArray[i];
		User::LeaveIfError(iInputVidFormats.Append(vid));
		
		// append the output video formats
		vid = KTestPreProcInfoOutputFormatArray[i];
		User::LeaveIfError(iOutputVidFormats.Append(vid));

		// append the combinations
		TUint32 comb = KTestPostProcInfoCombsArray[i];
		User::LeaveIfError(iCombinations.Append(comb));

		// append the scale factors
		TScaleFactor scale = KTestPostProcInfoScaleFactorsArray[i];
		User::LeaveIfError(iScaleFactors.Append(scale));
		}
	
	// construct the video decoder info object
	CPreProcessorInfo* vInfo = CPreProcessorInfo::NewL(
		KUidDevVideoTestPreProcHwDevice,
		KTestPreProcInfoManufacturer,
		KTestPreProcInfoIdentifier,
		TVersion(KTestPreProcInfoVersionMaj, KTestPreProcInfoVersionMin, KTestPreProcInfoVersionBuild),
		ETrue,	// accelerated
		ETrue,	// supports direct capture
		iInputVidFormats.Array(),
		iOutputVidFormats.Array(),
		iCombinations.Array(),
		ETrue,	// scaling
		ETrue,	// anti-aliasing
		iScaleFactors.Array(),
		KTestPreProcInfoYuvToYuvCaps,
		KTestPreProcInfoRgbRanges,
		KTestPreProcInfoRotations,
		KTestPreProcInfoISInfo );
		
	CleanupStack::PushL(vInfo);
	return vInfo;
	}

void CMMFTestVideoPreProcHwDevice::SetInputFormatL(const TUncompressedVideoFormat& aFormat, const TSize& aPictureSize)
	{
	TSize testSize(KTestInputSize2X, KTestInputSize2Y);

	if (!(aFormat == KTestVidFormat2) || !(aPictureSize == testSize))
		User::Leave(KErrCorrupt);
	}

void CMMFTestVideoPreProcHwDevice::SetSourceCameraL(TInt aCameraHandle, TReal aPictureRate)
	{
	if (aCameraHandle == KTestCamHandleFatal)
		iProxy->MdvrpFatalError(this, KErrDied);
	else if ((KTestCamHandlePre != aCameraHandle) || (KTestPictureRate != aPictureRate))
		User::Leave(KErrCorrupt);
	}

void CMMFTestVideoPreProcHwDevice::SetSourceMemoryL(TReal aMaxPictureRate, TBool aConstantPictureRate, TBool aProcessRealtime)
	{
	if ((KTestPictureRate != aMaxPictureRate) || !aConstantPictureRate || aProcessRealtime)
		User::Leave(KErrCorrupt);
	}

void CMMFTestVideoPreProcHwDevice::SetClockSource(MMMFClockSource* aClock)
	{
	__ASSERT_ALWAYS(aClock, DevVideoPreProcPanic(EPreProcPanicClockSource)); 

	// call Time() to check that clock can be used
	TTimeIntervalMicroSeconds currTime(0); // done this way to remove compiler warning
	currTime = aClock->Time();
	}

void CMMFTestVideoPreProcHwDevice::SetPreProcessTypesL(TUint32 aPreProcessTypes)
	{
	if (!(aPreProcessTypes == KTestProcessType2))
		User::Leave(KErrCorrupt);
	}

void CMMFTestVideoPreProcHwDevice::SetRgbToYuvOptionsL(TRgbRange aRange, const TYuvFormat& aOutputFormat)
	{
	// check against test data
	if ((aRange != KTestRgbRange2) || !CompareYuvFormats(aOutputFormat, KTestYuvFormat2) )
		User::Leave(KErrCorrupt);
	}

void CMMFTestVideoPreProcHwDevice::SetYuvToYuvOptionsL(const TYuvFormat& aInputFormat, const TYuvFormat& aOutputFormat)
	{
	if (!CompareYuvFormats(aInputFormat, KTestYuvFormat2) || !CompareYuvFormats(aOutputFormat, KTestYuvFormat1) )
		User::Leave(KErrCorrupt);
	}

void CMMFTestVideoPreProcHwDevice::SetRotateOptionsL(TRotationType aRotationType)
	{
	if (!(aRotationType == KTestRotate2))
		User::Leave(KErrCorrupt);
	}

void CMMFTestVideoPreProcHwDevice::SetScaleOptionsL(const TSize& aTargetSize, TBool aAntiAliasFiltering)
	{
	TSize testScale(KTestScaleY, KTestScaleX);
	if (!(aTargetSize == testScale) || !aAntiAliasFiltering)
		User::Leave(KErrCorrupt);
	}

void CMMFTestVideoPreProcHwDevice::SetInputCropOptionsL(const TRect& aRect)
	{
	TRect testRect(KTestInputCropRectD, KTestInputCropRectC, KTestInputCropRectB, KTestInputCropRectA);
	if (!(aRect == testRect))
		User::Leave(KErrCorrupt);
	}

void CMMFTestVideoPreProcHwDevice::SetOutputCropOptionsL(const TRect& aRect)
	{
	TRect testRect(KTestOutputCropRectD, KTestOutputCropRectC, KTestOutputCropRectB, KTestOutputCropRectA);
	if (!(aRect == testRect))
		User::Leave(KErrCorrupt);
	}

void CMMFTestVideoPreProcHwDevice::SetOutputPadOptionsL(const TSize& aOutputSize, const TPoint& aPicturePos)
	{
	TSize testPad(KTestPadY, KTestPadX);
	TPoint testPoint(KTestPadPointY, KTestPadPointX);

	if (!(testPad == aOutputSize) || !(testPoint == aPicturePos))
		User::Leave(KErrCorrupt);
	}

void CMMFTestVideoPreProcHwDevice::SetColorEnhancementOptionsL(const TColorEnhancementOptions& aOptions)
	{
	if (!CompareColorEnhancements(aOptions, KTestColorEnhance2))
		User::Leave(KErrCorrupt);
	}

void CMMFTestVideoPreProcHwDevice::SetFrameStabilisationOptionsL(const TSize& aOutputSize, TBool aFrameStabilisation)
	{
	TSize testSize(KTestScaleY, KTestScaleX);
	if (!(aOutputSize == testSize) || !aFrameStabilisation)
		User::Leave(KErrCorrupt);
	}

void CMMFTestVideoPreProcHwDevice::SetCustomPreProcessOptionsL(const TDesC8& aOptions)
	{
	if (!(aOptions == KTestCustomPreProc2))
		User::Leave(KErrCorrupt);
	}

void CMMFTestVideoPreProcHwDevice::Initialize()
	{
	iProxy->MdvrpInitializeComplete(this, KErrNone);
	}

void CMMFTestVideoPreProcHwDevice::WritePictureL(TVideoPicture* aPicture)
	{
	iProxy->MdvrpReturnPicture(aPicture);
	}

void CMMFTestVideoPreProcHwDevice::InputEnd()
	{
	iProxy->MdvrpStreamEnd();
	}

void CMMFTestVideoPreProcHwDevice::Start()
	{
	iRecPosition = KTestRecTimeStartPre;
	iIsRecording = ETrue;
	}

void CMMFTestVideoPreProcHwDevice::Stop()
	{
	iRecPosition = KTestRecTimeStop;
	iIsRecording = EFalse;
	}

void CMMFTestVideoPreProcHwDevice::Pause()
	{
	iRecPosition = KTestRecTimePausePre;
	iIsRecording = EFalse;
	}

void CMMFTestVideoPreProcHwDevice::Resume()
	{
	iRecPosition = KTestRecTimeResumePre;
	iIsRecording = ETrue;
	}

void CMMFTestVideoPreProcHwDevice::Freeze()
	{
	}

void CMMFTestVideoPreProcHwDevice::ReleaseFreeze()
	{
	}

TTimeIntervalMicroSeconds CMMFTestVideoPreProcHwDevice::RecordingPosition()
	{
	return iRecPosition;
	}

void CMMFTestVideoPreProcHwDevice::GetPictureCounters(CMMFDevVideoRecord::TPictureCounters& aCounters)
	{
	aCounters = GetTestEncPictureCounters();
	}

void CMMFTestVideoPreProcHwDevice::GetFrameStabilisationOutput(TRect& aRect)
	{
	aRect = TRect(KTestFrameStableY1, KTestFrameStableX1, KTestFrameStableY2, KTestFrameStableX2);
	}

TUint CMMFTestVideoPreProcHwDevice::NumComplexityLevels()
	{
	return KTestNumComplexityLevels2;
	}

void CMMFTestVideoPreProcHwDevice::SetComplexityLevel(TUint aLevel)
	{
	__ASSERT_ALWAYS(aLevel == KTestComplexityLevel2, DevVideoPreProcPanic(EPreProcPanicComplexityLevel));
	}

void CMMFTestVideoPreProcHwDevice::SetOutputFormatL(const TUncompressedVideoFormat& aFormat)
	{
	if (!(aFormat == KTestVidFormat2))
		{
		User::Leave(KErrCorrupt);
		}
	}

void CMMFTestVideoPreProcHwDevice::SetOutputDevice(CMMFVideoEncodeHwDevice* /*aDevice*/)
	{
	}

void CMMFTestVideoPreProcHwDevice::ReturnPicture(TVideoPicture* /*aPicture*/)
	{
	}

void CMMFTestVideoPreProcHwDevice::CommitL()
	{
	}

void CMMFTestVideoPreProcHwDevice::Revert()
	{
	}

void CMMFTestVideoPreProcHwDevice::SetProxy(MMMFDevVideoRecordProxy& aProxy)
	{
	ASSERT(iProxy == NULL);
	iProxy = &aProxy;
	}


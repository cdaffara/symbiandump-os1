// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "ciupreproc.h"

_LIT(KDevVideoPreProcPanicCategory, "DevVideoPreProcessor");
void DevVideoPreProcPanic(TInt aReason)
	{
	User::Panic(KDevVideoPreProcPanicCategory, aReason);
	}

CMMFVideoPreProcHwDevice* CMMFTestCiuVideoPreProcHwDevice::NewL(TAny* /*aInitParams*/)
	{
	CMMFTestCiuVideoPreProcHwDevice* s = new(ELeave) CMMFTestCiuVideoPreProcHwDevice;
	return (STATIC_CAST(CMMFVideoPreProcHwDevice*, s));
	}

CMMFTestCiuVideoPreProcHwDevice::CMMFTestCiuVideoPreProcHwDevice()
	{
	}

CMMFTestCiuVideoPreProcHwDevice::~CMMFTestCiuVideoPreProcHwDevice()
	{
	}

TAny* CMMFTestCiuVideoPreProcHwDevice::CustomInterface(TUid aInterface)
	{
	if (aInterface == KUidMmfVideoInputSnapshot)
		{
		MMmfVideoInputSnapshot* ntrfc = this;
		return ntrfc;
		}
	else if (aInterface == KUidDevVideoCiuTestInterfaceInputSnapshot)
		{
		MDevVideoCiuTestInterfaceInputSnapshot* ntrfc = this;
		return ntrfc;
		}
	else
		{
		return NULL;
		}
	}

CPreProcessorInfo* CMMFTestCiuVideoPreProcHwDevice::PreProcessorInfoLC()
	{
	return NULL;
	}

void CMMFTestCiuVideoPreProcHwDevice::SetInputFormatL(const TUncompressedVideoFormat& /*aFormat*/, const TSize& /*aPictureSize*/)
	{
	}

void CMMFTestCiuVideoPreProcHwDevice::SetSourceCameraL(TInt /*aCameraHandle*/, TReal /*aPictureRate*/)
	{
	}

void CMMFTestCiuVideoPreProcHwDevice::SetSourceMemoryL(TReal /*aMaxPictureRate*/, TBool /*aConstantPictureRate*/, TBool /*aProcessRealtime*/)
	{
	}

void CMMFTestCiuVideoPreProcHwDevice::SetClockSource(MMMFClockSource* /*aClock*/)
	{
	}

void CMMFTestCiuVideoPreProcHwDevice::SetPreProcessTypesL(TUint32 /*aPreProcessTypes*/)
	{
	}

void CMMFTestCiuVideoPreProcHwDevice::SetRgbToYuvOptionsL(TRgbRange /*aRange*/, const TYuvFormat& /*aOutputFormat*/)
	{
	}

void CMMFTestCiuVideoPreProcHwDevice::SetYuvToYuvOptionsL(const TYuvFormat& /*aInputFormat*/, const TYuvFormat& /*aOutputFormat*/)
	{
	}

void CMMFTestCiuVideoPreProcHwDevice::SetRotateOptionsL(TRotationType /*aRotationType*/)
	{
	}

void CMMFTestCiuVideoPreProcHwDevice::SetScaleOptionsL(const TSize& /*aTargetSize*/, TBool /*aAntiAliasFiltering*/)
	{
	}

void CMMFTestCiuVideoPreProcHwDevice::SetInputCropOptionsL(const TRect& /*aRect*/)
	{
	}

void CMMFTestCiuVideoPreProcHwDevice::SetOutputCropOptionsL(const TRect& /*aRect*/)
	{
	}

void CMMFTestCiuVideoPreProcHwDevice::SetOutputPadOptionsL(const TSize& /*aOutputSize*/, const TPoint& /*aPicturePos*/)
	{
	}

void CMMFTestCiuVideoPreProcHwDevice::SetColorEnhancementOptionsL(const TColorEnhancementOptions& /*aOptions*/)
	{
	}

void CMMFTestCiuVideoPreProcHwDevice::SetFrameStabilisationOptionsL(const TSize& /*aOutputSize*/, TBool /*aFrameStabilisation*/)
	{
	}

void CMMFTestCiuVideoPreProcHwDevice::SetCustomPreProcessOptionsL(const TDesC8& /*aOptions*/)
	{
	}

void CMMFTestCiuVideoPreProcHwDevice::Initialize()
	{
	iProxy->MdvrpInitializeComplete(this, KErrNone);
	}

void CMMFTestCiuVideoPreProcHwDevice::WritePictureL(TVideoPicture* aPicture)
	{
	iProxy->MdvrpReturnPicture(aPicture);
	}

void CMMFTestCiuVideoPreProcHwDevice::InputEnd()
	{
	iProxy->MdvrpStreamEnd();
	}

void CMMFTestCiuVideoPreProcHwDevice::Start()
	{
	}

void CMMFTestCiuVideoPreProcHwDevice::Stop()
	{
	}

void CMMFTestCiuVideoPreProcHwDevice::Pause()
	{
	}

void CMMFTestCiuVideoPreProcHwDevice::Resume()
	{
	}

void CMMFTestCiuVideoPreProcHwDevice::Freeze()
	{
	}

void CMMFTestCiuVideoPreProcHwDevice::ReleaseFreeze()
	{
	}

TTimeIntervalMicroSeconds CMMFTestCiuVideoPreProcHwDevice::RecordingPosition()
	{
	return 0;
	}

void CMMFTestCiuVideoPreProcHwDevice::GetPictureCounters(CMMFDevVideoRecord::TPictureCounters& /*aCounters*/)
	{
	}

void CMMFTestCiuVideoPreProcHwDevice::GetFrameStabilisationOutput(TRect& /*aRect*/)
	{
	}

TUint CMMFTestCiuVideoPreProcHwDevice::NumComplexityLevels()
	{
	return 0;
	}

void CMMFTestCiuVideoPreProcHwDevice::SetComplexityLevel(TUint /*aLevel*/)
	{
	}

void CMMFTestCiuVideoPreProcHwDevice::SetOutputFormatL(const TUncompressedVideoFormat& /*aFormat*/)
	{
	}

void CMMFTestCiuVideoPreProcHwDevice::SetOutputDevice(CMMFVideoEncodeHwDevice* /*aDevice*/)
	{
	}

void CMMFTestCiuVideoPreProcHwDevice::ReturnPicture(TVideoPicture* /*aPicture*/)
	{
	}

void CMMFTestCiuVideoPreProcHwDevice::CommitL()
	{
	}

void CMMFTestCiuVideoPreProcHwDevice::Revert()
	{
	}

void CMMFTestCiuVideoPreProcHwDevice::SetProxy(MMMFDevVideoRecordProxy& aProxy)
	{
	ASSERT(iProxy == NULL);
	iProxy = &aProxy;
	}

// Implementation of MMmfVideoInputSnapshot
void CMMFTestCiuVideoPreProcHwDevice::MmvisSetObserver(MMmfVideoInputSnapshotObserver* aObserver)
	{
	iInputSnapshotObserver = aObserver;
	}
	
void CMMFTestCiuVideoPreProcHwDevice::MmvisGetSnapshotL(TPictureData& aPictureData,
														const TUncompressedVideoFormat& aFormat)
	{
	// the format should match the one set using the test interface
	if (iVideoFormat == aFormat)
		{
		aPictureData = iPictureData;
		}
	else
		{
		User::Leave(KErrArgument);
		}
	}
	
void CMMFTestCiuVideoPreProcHwDevice::MmvisCancelSnapshot()
	{
	iCancelSnapshot = ETrue;
	}

// Test interface implementation
MMmfVideoInputSnapshotObserver* CMMFTestCiuVideoPreProcHwDevice::MmvisSnapshotObserver()
	{
	return iInputSnapshotObserver;
	}

void CMMFTestCiuVideoPreProcHwDevice::MmvisSetSnapshot(const TPictureData& aPictureData,
														const TUncompressedVideoFormat& aFormat)
	{
	iPictureData = aPictureData;
	iVideoFormat = aFormat;
	}

void CMMFTestCiuVideoPreProcHwDevice::MmvisRequestSnapshotCompleteCallback()
	{
	ASSERT(iInputSnapshotObserver);
	iInputSnapshotObserver->SnapshotComplete(KErrNone);
	}
	
TBool CMMFTestCiuVideoPreProcHwDevice::MmvisIsCancelSnapshot()
	{
	return iCancelSnapshot;
	}

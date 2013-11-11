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

#include "ciupostproc.h"

_LIT(KDevVideoPostProcPanicCategory, "DevVideoCiuPostProcessor");
void DevVideoPostProcPanic(TInt aReason)
	{
	User::Panic(KDevVideoPostProcPanicCategory, aReason);
	}

CMMFVideoPostProcHwDevice* CMMFTestCiuVideoPostProcHwDevice::NewL(TAny* /*aInitParams*/)
	{
	CMMFTestCiuVideoPostProcHwDevice* s = new(ELeave) CMMFTestCiuVideoPostProcHwDevice;
	return static_cast<CMMFVideoPostProcHwDevice*>(s);
	}

CMMFTestCiuVideoPostProcHwDevice::CMMFTestCiuVideoPostProcHwDevice()
	{
	}

CMMFTestCiuVideoPostProcHwDevice::~CMMFTestCiuVideoPostProcHwDevice()
	{
	}

TAny* CMMFTestCiuVideoPostProcHwDevice::CustomInterface(TUid aInterface)
	{
	if (aInterface == KUidMmfVideoMediaProtected)
		{
		MMmfVideoMediaProtected* interface = this;
		return interface;
		}
	else if (aInterface == KUidDevVideoCiuTestInterfaceMediaProtected)
		{
		MDevVideoCiuTestInterfaceMediaProtected* interface = this;
		return interface;
		}
	else if (aInterface == KUidMmfVideoPlayHWDeviceColourInfoCustomHeader)
		{
		MMmfVideoPlayHWDeviceColourInfoCustomHeader* interface = this;
		return interface;
		}
	else if (aInterface == KUidDevVideoCiuTestInterfaceColourSpace)
		{
		MDevVideoCiuTestInterfaceColourSpace* interface = this;
		return interface;
		}
	else
		{
		return NULL;
		}
	}

CPostProcessorInfo* CMMFTestCiuVideoPostProcHwDevice::PostProcessorInfoLC()
	{
	return NULL;
	}

void CMMFTestCiuVideoPostProcHwDevice::GetOutputFormatListL(RArray<TUncompressedVideoFormat>& /*aFormats*/)
	{
	}

void CMMFTestCiuVideoPostProcHwDevice::SetOutputFormatL(const TUncompressedVideoFormat &/*aFormat*/)
	{
	}

void CMMFTestCiuVideoPostProcHwDevice::SetPostProcessTypesL(TUint32 /*aPostProcCombination*/)
	{
	}

void CMMFTestCiuVideoPostProcHwDevice::SetInputCropOptionsL(const TRect& /*aRect*/)
	{
	}

void CMMFTestCiuVideoPostProcHwDevice::SetYuvToRgbOptionsL(const TYuvToRgbOptions& /*aOptions*/, const TYuvFormat& /*aYuvFormat*/, TRgbFormat /*aRgbFormat*/)
	{
	}

void CMMFTestCiuVideoPostProcHwDevice::SetYuvToRgbOptionsL(const TYuvToRgbOptions& /*aOptions*/)
	{
	}

void CMMFTestCiuVideoPostProcHwDevice::SetRotateOptionsL(TRotationType /*aRotationType*/)
	{
	}

void CMMFTestCiuVideoPostProcHwDevice::SetScaleOptionsL(const TSize& /*aTargetSize*/, TBool /*aAntiAliasFiltering*/)
	{
	}

void CMMFTestCiuVideoPostProcHwDevice::SetOutputCropOptionsL(const TRect& /*aRect*/)
	{
	}

void CMMFTestCiuVideoPostProcHwDevice::SetPostProcSpecificOptionsL(const TDesC8& /*aOptions*/)
	{
	}

void CMMFTestCiuVideoPostProcHwDevice::SetClockSource(MMMFClockSource* /*aClock*/)
	{
	}

void CMMFTestCiuVideoPostProcHwDevice::SetVideoDestScreenL(TBool /*aScreen*/)
	{
	}

void CMMFTestCiuVideoPostProcHwDevice::Initialize()
	{
	iProxy->MdvppInitializeComplete(this, KErrNone);
	}

void CMMFTestCiuVideoPostProcHwDevice::StartDirectScreenAccessL(const TRect& /*aVideoRect*/, 
																CFbsScreenDevice& /*aScreenDevice*/, 
																const TRegion& /*aClipRegion*/)
	{
	}

void CMMFTestCiuVideoPostProcHwDevice::SetScreenClipRegion(const TRegion& /*aRegion*/)
	{
	}

void CMMFTestCiuVideoPostProcHwDevice::SetPauseOnClipFail(TBool /*aPause*/)
	{
	}

void CMMFTestCiuVideoPostProcHwDevice::AbortDirectScreenAccess()
	{
	}

TBool CMMFTestCiuVideoPostProcHwDevice::IsPlaying()
	{
	return ETrue;
	}

void CMMFTestCiuVideoPostProcHwDevice::Redraw()
	{
	}

void CMMFTestCiuVideoPostProcHwDevice::Start()
	{
	}

void CMMFTestCiuVideoPostProcHwDevice::Stop()
	{
	}

void CMMFTestCiuVideoPostProcHwDevice::Pause()
	{
	}

void CMMFTestCiuVideoPostProcHwDevice::Resume()
	{
	}

void CMMFTestCiuVideoPostProcHwDevice::SetPosition(const TTimeIntervalMicroSeconds& /*aPlaybackPosition*/)
	{
	}

void CMMFTestCiuVideoPostProcHwDevice::FreezePicture(const TTimeIntervalMicroSeconds& /*aTimestamp*/)
	{
	}

void CMMFTestCiuVideoPostProcHwDevice::ReleaseFreeze(const TTimeIntervalMicroSeconds& /*aTimestamp*/)
	{
	}

TTimeIntervalMicroSeconds CMMFTestCiuVideoPostProcHwDevice::PlaybackPosition()
	{
	return 0;
	}

TUint CMMFTestCiuVideoPostProcHwDevice::PictureBufferBytes()
	{
	return 0;
	}

void CMMFTestCiuVideoPostProcHwDevice::GetPictureCounters(CMMFDevVideoPlay::TPictureCounters& /*aCounters*/)
	{
	}

void CMMFTestCiuVideoPostProcHwDevice::SetComplexityLevel(TUint /*aLevel*/)
	{
	}

TUint CMMFTestCiuVideoPostProcHwDevice::NumComplexityLevels()
	{
	return 0;
	}

void CMMFTestCiuVideoPostProcHwDevice::GetComplexityLevelInfo(TUint /*aLevel*/, 
																CMMFDevVideoPlay::TComplexityLevelInfo& /*aInfo*/)
	{
	}

void CMMFTestCiuVideoPostProcHwDevice::ReturnPicture(TVideoPicture* /*aPicture*/)
	{
	}

TBool CMMFTestCiuVideoPostProcHwDevice::GetSnapshotL(TPictureData& /*aPictureData*/,
														const TUncompressedVideoFormat& /*aFormat*/)
	{
	return EFalse;
	}

void CMMFTestCiuVideoPostProcHwDevice::GetTimedSnapshotL(TPictureData* /*aPictureData*/,
														 const TUncompressedVideoFormat& /*aFormat*/,
														 const TTimeIntervalMicroSeconds& /*aPresentationTimestamp*/)
	{
	}

void CMMFTestCiuVideoPostProcHwDevice::GetTimedSnapshotL(TPictureData* /*aPictureData*/,
														 const TUncompressedVideoFormat& /*aFormat*/,
														 const TPictureId& /*aPictureId*/)
	{
	}

void CMMFTestCiuVideoPostProcHwDevice::CancelTimedSnapshot()
	{
	}

void CMMFTestCiuVideoPostProcHwDevice::GetSupportedSnapshotFormatsL(RArray<TUncompressedVideoFormat>& /*aFormats*/)
	{
	}

void CMMFTestCiuVideoPostProcHwDevice::InputEnd()
	{
	}

void CMMFTestCiuVideoPostProcHwDevice::SetInputFormatL(const TUncompressedVideoFormat& /*aFormat*/)
	{
	}

void CMMFTestCiuVideoPostProcHwDevice::SetInputDevice(CMMFVideoDecodeHwDevice* /*aDevice*/)
	{
	}

void CMMFTestCiuVideoPostProcHwDevice::WritePictureL(TVideoPicture* /*aPicture*/)
	{
	}

void CMMFTestCiuVideoPostProcHwDevice::CommitL()
	{
	}

void CMMFTestCiuVideoPostProcHwDevice::Revert()
	{
	}

void CMMFTestCiuVideoPostProcHwDevice::SetProxy(MMMFDevVideoPlayProxy& aProxy)
	{
	iProxy = &aProxy;
	}

void CMMFTestCiuVideoPostProcHwDevice::MmvsoMediaIsProtectedL(TBool aProtected)
	{
	iIsProtected = aProtected;
	User::Leave(KErrNotSupported);
	}
	
TBool CMMFTestCiuVideoPostProcHwDevice::MmvsoMediaIsProtected()
	{
	return iIsProtected;
	}

void CMMFTestCiuVideoPostProcHwDevice::MmvpciGetColourSpaceHeaderInfoL( TVideoDataUnitType& aDataUnitType,
																		TVideoDataUnitEncapsulation& aDataUnitEncapsulation, 
																		TVideoInputBuffer* aDataUnit,
																		TYuvFormat& aYuvFormat)
	{
	aDataUnitType = iDataUnitType;
	aDataUnitEncapsulation = iDataUnitEncapsulation;
	*aDataUnit = *iDataUnit;
	aYuvFormat = iYuvFormat;	
	}
	
void CMMFTestCiuVideoPostProcHwDevice::MmvpciSetColourSpaceHeaderInfoL(const TVideoDataUnitType aDataUnitType,
																		const TVideoDataUnitEncapsulation aDataUnitEncapsulation, 
																		TVideoInputBuffer* aDataUnit, 
																		const TYuvFormat& aYuvFormat)
	{
	iDataUnitType = aDataUnitType;
	iDataUnitEncapsulation = aDataUnitEncapsulation;
	iDataUnit = aDataUnit;
	iYuvFormat = aYuvFormat;
	}

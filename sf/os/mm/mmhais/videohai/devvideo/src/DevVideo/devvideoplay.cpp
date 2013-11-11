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

#include <ecom/ecom.h>
#ifdef SYMBIAN_MULTIMEDIA_CODEC_API	
#include <mdf/codecapiuids.hrh>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <mmf/devvideo/devvideohwdeviceadaptersetup.h>
#endif
#endif // SYMBIAN_MULTIMEDIA_CODEC_API

#include "devvideoplay.h"
#include "videoplayhwdevice.h"
#include "devvideointernal.h"

#include <mm/mmpluginutils.h>

const THwDeviceId KHwDeviceIdVideoDecoder = 1;
const THwDeviceId KHwDeviceIdVideoPostProcessor = 2;

//
// CMMFVideoDecodeHwDevice
//
EXPORT_C CMMFVideoDecodeHwDevice* CMMFVideoDecodeHwDevice::NewL(TUid aUid, MMMFDevVideoPlayProxy& aProxy)
	{
	CMMFVideoDecodeHwDevice* s = reinterpret_cast<CMMFVideoDecodeHwDevice*> 
		(REComSession::CreateImplementationL(aUid, _FOFF(CMMFVideoDecodeHwDevice,iDtor_ID_Key)));
	s->SetProxy(aProxy);
	return s;
	}

#ifdef SYMBIAN_MULTIMEDIA_CODEC_API	
const TInt KUidMDFVideoDecoderHwDeviceAdapter = 0x102737ED;
EXPORT_C CMMFVideoDecodeHwDevice* CMMFVideoDecodeHwDevice::NewPuAdapterL(const CImplementationInformation& aInfo, MMMFDevVideoPlayProxy& aProxy)
	{
	CMMFVideoDecodeHwDevice* hwdevice = NewL(TUid::Uid(KUidMDFVideoDecoderHwDeviceAdapter), aProxy);
	CleanupStack::PushL(hwdevice);

	MDevVideoHwDeviceAdapterSetup* setup = static_cast<MDevVideoHwDeviceAdapterSetup*>(
			hwdevice->CustomInterface(TUid::Uid(KUidDevVideoHwDeviceAdapterSetup)));

	if (setup!=NULL)
		{
		setup->LoadProcessingUnitL(aInfo);
		}
	CleanupStack::Pop(hwdevice);
	return hwdevice;
	}
#else
EXPORT_C CMMFVideoDecodeHwDevice* CMMFVideoDecodeHwDevice::NewPuAdapterL(const CImplementationInformation& /*aInfo*/, MMMFDevVideoPlayProxy& /*aProxy*/)
	{
	User::Leave(KErrNotSupported);
	return NULL;
	}
#endif // SYMBIAN_MULTIMEDIA_CODEC_API

EXPORT_C CMMFVideoDecodeHwDevice::~CMMFVideoDecodeHwDevice()
	{
	REComSession::DestroyedImplementation(iDtor_ID_Key);
	}

EXPORT_C CMMFVideoDecodeHwDevice::CMMFVideoDecodeHwDevice()
	{
	}


//
// CMMFVideoPostProcHwDevice
//
EXPORT_C CMMFVideoPostProcHwDevice* CMMFVideoPostProcHwDevice::NewL(TUid aUid, MMMFDevVideoPlayProxy& aProxy)
	{
	CMMFVideoPostProcHwDevice* s = reinterpret_cast<CMMFVideoPostProcHwDevice*>
		(REComSession::CreateImplementationL(aUid, _FOFF(CMMFVideoPostProcHwDevice,iDtor_ID_Key)));
	s->SetProxy(aProxy);
	return s;
	}

EXPORT_C CMMFVideoPostProcHwDevice::~CMMFVideoPostProcHwDevice()
	{
	REComSession::DestroyedImplementation(iDtor_ID_Key);
	}

EXPORT_C CMMFVideoPostProcHwDevice::CMMFVideoPostProcHwDevice()
	{
	}


EXPORT_C TVideoInputBuffer::TVideoInputBuffer() :
	iData(0,0,0),
	iOptions(0),
	iDecodingTimestamp(0),
	iPresentationTimestamp(0),
	iPreRoll(EFalse),
	iSequenceNumber(0),
	iError(EFalse),
	iUser(NULL)
	{
	}


//
// CPostProcessorInfo
//
EXPORT_C CPostProcessorInfo* CPostProcessorInfo::NewL(TUid aUid,
													 const TDesC& aManufacturer,
													 const TDesC& aIdentifier,
													 TVersion aVersion,
													 const TArray<TUncompressedVideoFormat>& aSupportedFormats,
													 const TArray<TUint32>& aSupportedCombinations,
													 TBool aAccelerated,
													 TBool aSupportsDirectDisplay,
													 const TYuvToRgbCapabilities& aYuvToRgbCapabilities,
													 TUint32 aSupportedRotations,
													 TBool aSupportArbitraryScaling,
													 const TArray<TScaleFactor>& aSupportedScaleFactors,
													 TBool aAntiAliasedScaling,
													 const TDesC8& aImplementationSpecificInfo)
	{
	CPostProcessorInfo* s = new(ELeave) CPostProcessorInfo(aUid, 
														   aVersion, 
														   aAccelerated,
														   aSupportsDirectDisplay, 
														   aYuvToRgbCapabilities, 
														   aSupportedRotations, 
														   aSupportArbitraryScaling,
														   aAntiAliasedScaling);

	CleanupStack::PushL(s);

	s->ConstructL(aManufacturer,
				  aIdentifier,
				  aSupportedFormats,
				  aSupportedCombinations,
				  aSupportedScaleFactors,
				  aImplementationSpecificInfo);
	
	CleanupStack::Pop(s);
	return s;
	}

CPostProcessorInfo::CPostProcessorInfo(TUid aUid,
									   TVersion aVersion,
									   TBool aAccelerated,
									   TBool aSupportDirectDisplay,
									   const TYuvToRgbCapabilities& aYuvToRgbCapabilities,
									   TUint32 aSupportedRotations,
									   TBool aSupportArbitraryScaling,
									   TBool aAntiAliasedScaling) :
	iUid(aUid),
	iVersion(aVersion),
	iAccelerated(aAccelerated),
	iSupportDirectDisplay(aSupportDirectDisplay),
	iYuvToRgbCapabilities(aYuvToRgbCapabilities),
	iSupportedRotations(aSupportedRotations),
	iSupportArbitraryScaling(aSupportArbitraryScaling),
	iAntiAliasedScaling(aAntiAliasedScaling),
	iSupportsContentProtected(EFalse)
	{
	iSupportedScreens.Reset();
	}

void CPostProcessorInfo::ConstructL(const TDesC& aManufacturer,
									const TDesC& aIdentifier,
									const TArray<TUncompressedVideoFormat>& aSupportedFormats,
									const TArray<TUint32>& aSupportedCombinations,
									const TArray<TScaleFactor>& aSupportedScaleFactors,
									const TDesC8& aImplementationSpecificInfo)
	{
	iManufacturer = aManufacturer.AllocL();
	iIdentifier = aIdentifier.AllocL();
	iImplementationSpecificInfo = aImplementationSpecificInfo.AllocL();

	TInt i=0;
	TInt supportedFormatsCount = aSupportedFormats.Count();
	for (i=0; i<supportedFormatsCount; i++)
		{
		User::LeaveIfError(iSupportedFormats.Append(aSupportedFormats[i]));
		}

	TInt supportedCombinationsCount = aSupportedCombinations.Count();
	for (i=0; i<supportedCombinationsCount; i++)
		{
		User::LeaveIfError(iSupportedCombinations.Append(aSupportedCombinations[i]));
		}

	TInt supportedScaleFactors = aSupportedScaleFactors.Count();
	for (i=0; i<supportedScaleFactors; i++)
		{
		User::LeaveIfError(iSupportedScaleFactors.Append(aSupportedScaleFactors[i]));
		}
	}
				
EXPORT_C CPostProcessorInfo::~CPostProcessorInfo()
	{
	delete iManufacturer;
	delete iIdentifier;
	delete iImplementationSpecificInfo;
	iSupportedFormats.Reset();
	iSupportedCombinations.Reset();
	iSupportedScaleFactors.Reset();
	iSupportedScreens.Close();
	}

EXPORT_C TUid CPostProcessorInfo::Uid() const
	{
	return iUid;
	}

EXPORT_C const TDesC& CPostProcessorInfo::Manufacturer() const
	{
	return *iManufacturer;
	}

EXPORT_C const TDesC& CPostProcessorInfo::Identifier() const
	{
	return *iIdentifier;
	}

EXPORT_C TVersion CPostProcessorInfo::Version() const
	{
	return iVersion;
	}

EXPORT_C TBool CPostProcessorInfo::SupportsFormat(const TUncompressedVideoFormat& aFormat) const
	{
	TBool found = EFalse;
	TInt count = iSupportedFormats.Count();
	for (TInt i=0; i<count; i++)
		{
		if (iSupportedFormats[i] == aFormat)
			{
			found = ETrue;
			break;
			}
		}
	return found;
	}

EXPORT_C const RArray<TUncompressedVideoFormat>& CPostProcessorInfo::SupportedFormats() const
	{
	return iSupportedFormats;
	}

EXPORT_C TBool CPostProcessorInfo::SupportsCombination(TUint32 aCombination) const
	{
	TBool found = EFalse;
	TInt count = iSupportedCombinations.Count();
	for (TInt i=0; i<count; i++)
		{
		if (iSupportedCombinations[i] == aCombination)
			{
			found = ETrue;
			break;
			}
		}
	return found;
	}

EXPORT_C const RArray<TUint32>& CPostProcessorInfo::SupportedCombinations() const
	{
	return iSupportedCombinations;
	}

EXPORT_C TBool CPostProcessorInfo::Accelerated() const
	{
	return iAccelerated;
	}

EXPORT_C TBool CPostProcessorInfo::SupportsDirectDisplay() const
	{
	return iSupportDirectDisplay;
	}

EXPORT_C const TYuvToRgbCapabilities& CPostProcessorInfo::YuvToRgbCapabilities() const
	{
	return iYuvToRgbCapabilities;
	}

EXPORT_C TUint32 CPostProcessorInfo::SupportedRotations() const
	{
	return iSupportedRotations;
	}

EXPORT_C TBool CPostProcessorInfo::SupportsArbitraryScaling() const
	{
	return iSupportArbitraryScaling;
	}

EXPORT_C const RArray<TScaleFactor>& CPostProcessorInfo::SupportedScaleFactors() const
	{
	return iSupportedScaleFactors;
	}

EXPORT_C TBool CPostProcessorInfo::AntiAliasedScaling() const
	{
	return iAntiAliasedScaling;
	}

EXPORT_C const TDesC8& CPostProcessorInfo::ImplementationSpecificInfo() const
	{
	return *iImplementationSpecificInfo;
	}

EXPORT_C void CPostProcessorInfo::AddSupportedScreenL(TInt aScreenNo)
	{
#ifdef SYMBIAN_ENABLE_MMF_MULTISCREEN_SUPPORT
	iSupportedScreens.AppendL(aScreenNo);
#else
	aScreenNo = aScreenNo; //Added to remove the warning
	User::Leave(KErrNotSupported);
#endif
	}
	
EXPORT_C void CPostProcessorInfo::GetSupportedScreensL(RArray<TInt>& aSupportedScreens) const
	{
#ifdef SYMBIAN_ENABLE_MMF_MULTISCREEN_SUPPORT
	aSupportedScreens.Reset();
	TInt screensCount = iSupportedScreens.Count();
	for (TInt i = 0; i < screensCount;i++)
		{
		aSupportedScreens.AppendL(iSupportedScreens[i]);
		}
#else
	aSupportedScreens = aSupportedScreens;//Added to remove the warning
	User::Leave(KErrNotSupported);
#endif
	}

EXPORT_C void CPostProcessorInfo::SetSupportsContentProtected(const TBool aSetting)
	{
	iSupportsContentProtected = aSetting;
	}

EXPORT_C TBool CPostProcessorInfo::SupportsContentProtected() const
	{
	return iSupportsContentProtected;
	}

//
// CVideoDecoderInfo
//
EXPORT_C CVideoDecoderInfo* CVideoDecoderInfo::NewL(TUid aUid,
													const TDesC& aManufacturer,
													const TDesC& aIdentifier,
													TVersion aVersion,
													const TArray<CCompressedVideoFormat*>& aSupportedFormats,
													TBool aAccelerated,
													TBool aSupportsDirectDisplay,
													const TSize& aMaxPictureSize,
													TUint aMaxBitrate,
													const TArray<TPictureRateAndSize>& aMaxPictureRates,
													TBool aSupportsPictureLoss,
													TBool aSupportsSliceLoss,
													const TDesC8& aCodingStandardSpecificInfo,
													const TDesC8& aImplementationSpecificInfo)
	{
	CVideoDecoderInfo* s = new(ELeave) CVideoDecoderInfo(aUid,
														 aVersion,
														 aAccelerated,
														 aSupportsDirectDisplay,
														 aMaxPictureSize,
														 aMaxBitrate,
														 aSupportsPictureLoss,
														 aSupportsSliceLoss);

	CleanupStack::PushL(s);
	s->ConstructL(aManufacturer, aIdentifier, aSupportedFormats, aMaxPictureRates, aCodingStandardSpecificInfo, aImplementationSpecificInfo);
	CleanupStack::Pop(s);
	return s;
	}

CVideoDecoderInfo::CVideoDecoderInfo(TUid aUid,
									 TVersion aVersion,
									 TBool aAccelerated,
									 TBool aSupportsDirectDisplay,
									 const TSize& aMaxPictureSize,
								     TUint aMaxBitrate,
								     TBool aSupportsPictureLoss,
								     TBool aSupportsSliceLoss) :
	iUid(aUid),
	iVersion(aVersion),
	iAccelerated(aAccelerated),
	iSupportsDirectDisplay(aSupportsDirectDisplay),
	iMaxPictureSize(aMaxPictureSize),
	iMaxBitrate(aMaxBitrate),
	iSupportsPictureLoss(aSupportsPictureLoss),
	iSupportsSliceLoss(aSupportsSliceLoss),
	iSupportsContentProtected(EFalse)
	{
	iSupportedScreens.Reset();
	}

void CVideoDecoderInfo::ConstructL(const TDesC& aManufacturer,
								   const TDesC& aIdentifier,
								   const TArray<CCompressedVideoFormat*>& aSupportedFormats,
								   const TArray<TPictureRateAndSize>& aMaxPictureRates,
								   const TDesC8& aCodingStandardSpecificInfo,
								   const TDesC8& aImplementationSpecificInfo)
	{
	iManufacturer = aManufacturer.AllocL();
	iIdentifier = aIdentifier.AllocL();
	iCodingStandardSpecificInfo = aCodingStandardSpecificInfo.AllocL();
	iImplementationSpecificInfo = aImplementationSpecificInfo.AllocL();

	TInt i=0;
	TInt count = aSupportedFormats.Count();
	for (i=0; i<count; i++)
		{
		CCompressedVideoFormat* f = CCompressedVideoFormat::NewL(*(aSupportedFormats[i]));
		CleanupStack::PushL(f);
		User::LeaveIfError(iSupportedFormats.Append(f));
		CleanupStack::Pop(f);
		}

	count = aMaxPictureRates.Count();
	for (i=0; i<count; i++)
		{
		User::LeaveIfError(iMaxPictureRates.Append(aMaxPictureRates[i]));
		}
	}

EXPORT_C CVideoDecoderInfo::~CVideoDecoderInfo()
	{
	delete iManufacturer;
	delete iIdentifier;
	delete iCodingStandardSpecificInfo;
	delete iImplementationSpecificInfo;
	iSupportedFormats.ResetAndDestroy();
	iSupportedFormats.Close();
	iMaxPictureRates.Reset();
	iMaxPictureRates.Close();
	iSupportedScreens.Close();
	}

EXPORT_C TBool CVideoDecoderInfo::SupportsFormat(const CCompressedVideoFormat& aFormat) const
	{
	TBool result = EFalse;
	TInt count = iSupportedFormats.Count();
	for (TInt i=0; i<count; i++)
		{
		if (*(iSupportedFormats[i]) == aFormat)
			{
			result = ETrue;
			break;
			}
		}
	return result;
	}

EXPORT_C const RPointerArray<CCompressedVideoFormat>& CVideoDecoderInfo::SupportedFormats() const
	{
	return iSupportedFormats;
	}

EXPORT_C const TDesC& CVideoDecoderInfo::Manufacturer() const
	{
	return *iManufacturer;
	}

EXPORT_C const TDesC& CVideoDecoderInfo::Identifier() const
	{
	return *iIdentifier;
	}

EXPORT_C TVersion CVideoDecoderInfo::Version() const
	{
	return iVersion;
	}

EXPORT_C TUid CVideoDecoderInfo::Uid() const
	{
	return iUid;
	}

EXPORT_C TBool CVideoDecoderInfo::Accelerated() const
	{
	return iAccelerated;
	}

EXPORT_C TBool CVideoDecoderInfo::SupportsDirectDisplay() const
	{
	return iSupportsDirectDisplay;
	}

EXPORT_C const TSize& CVideoDecoderInfo::MaxPictureSize() const
	{
	return iMaxPictureSize;
	}

EXPORT_C TUint CVideoDecoderInfo::MaxBitrate() const
	{
	return iMaxBitrate;
	}

EXPORT_C const RArray<TPictureRateAndSize>& CVideoDecoderInfo::MaxPictureRates() const
	{
	return iMaxPictureRates;
	}

EXPORT_C TBool CVideoDecoderInfo::SupportsPictureLoss() const
	{
	return iSupportsPictureLoss;
	}

EXPORT_C TBool CVideoDecoderInfo::SupportsSliceLoss() const
	{
	return iSupportsSliceLoss;
	}

EXPORT_C const TDesC8& CVideoDecoderInfo::CodingStandardSpecificInfo() const
	{
	return *iCodingStandardSpecificInfo;
	}

EXPORT_C const TDesC8& CVideoDecoderInfo::ImplementationSpecificInfo() const
	{
	return *iImplementationSpecificInfo;
	}

EXPORT_C void CVideoDecoderInfo::AddSupportedScreenL(TInt aScreenNo)
	{
#ifdef SYMBIAN_ENABLE_MMF_MULTISCREEN_SUPPORT
	iSupportedScreens.AppendL(aScreenNo);
#else
	aScreenNo = aScreenNo; //Added to remove the warning.
	User::Leave(KErrNotSupported);
#endif
	}
	
EXPORT_C void CVideoDecoderInfo::GetSupportedScreensL(RArray<TInt>& aSupportedScreens) const
	{
#ifdef SYMBIAN_ENABLE_MMF_MULTISCREEN_SUPPORT
	aSupportedScreens.Reset();
	TInt screensCount = iSupportedScreens.Count();
	for (TInt i = 0; i < screensCount;i++)
		{
		aSupportedScreens.AppendL(iSupportedScreens[i]);
		}
#else
	aSupportedScreens = aSupportedScreens;//Added to remove the warning
	User::Leave(KErrNotSupported);
#endif
	}

EXPORT_C void CVideoDecoderInfo::SetSupportsContentProtected(const TBool aSetting)
	{
	iSupportsContentProtected = aSetting;
	}

EXPORT_C TBool CVideoDecoderInfo::SupportsContentProtected() const
	{
	return iSupportsContentProtected;
	}

//
// CMMFDevVideoPlay
//
EXPORT_C CMMFDevVideoPlay* CMMFDevVideoPlay::NewL(MMMFDevVideoPlayObserver& aObserver)
	{
	return new(ELeave) CMMFDevVideoPlay(aObserver);
	}

CMMFDevVideoPlay::CMMFDevVideoPlay(MMMFDevVideoPlayObserver& aObserver) :
	iObserver(aObserver),
	iInitializationState(ENotInitialized),
	iVideoPictureQue(_FOFF(TVideoPicture,iLink)),
	iVideoPictureQueIter(iVideoPictureQue)
	{
	}

EXPORT_C CMMFDevVideoPlay::~CMMFDevVideoPlay()
	{
	delete iVideoDecodeHwDevice;
	delete iVideoPostProcHwDevice;
#ifdef SYMBIAN_MULTIMEDIA_CODEC_API
	iPuImplementations.ResetAndDestroy();
#endif
	}

EXPORT_C TBool CMMFDevVideoPlay::FindCommonFormat(const TArray<TUncompressedVideoFormat>& aFormats1, const TArray<TUncompressedVideoFormat>& aFormats2, TUncompressedVideoFormat& aCommonFormat)
	{
	// Find the least expensive format common to both arrays.
	// Expense is proportional to array position, so do search twice, using each array as a base

	TInt formats1Position;
	TInt formats2Position;

	const TUncompressedVideoFormat* firstTry = NULL;
	TInt firstTryCumulativePosition = -1;
	const TUncompressedVideoFormat* secondTry = NULL;
	TInt secondTryCumulativePosition = -1;

	TInt formats1Count = aFormats1.Count();
	TInt formats2Count = aFormats2.Count();

	for (formats1Position=0; formats1Position < formats1Count; formats1Position++)
		{
		const TUncompressedVideoFormat& format1 = aFormats1[formats1Position];

		for (formats2Position=0; formats2Position < formats2Count; formats2Position++)
			{
			const TUncompressedVideoFormat& format2 = aFormats2[formats2Position];
			if (format1 == format2)
				{
				firstTry = &format1;
				firstTryCumulativePosition = formats1Position+formats2Position;
				break;
				}
			}

		if (firstTry != NULL)
			break;
		}

	for (formats2Position=0; formats2Position < formats2Count; formats2Position++)
		{
		const TUncompressedVideoFormat& format2 = aFormats2[formats2Position];

		for (formats1Position=0; formats1Position < formats1Count; formats1Position++)
			{
			const TUncompressedVideoFormat& format1 = aFormats1[formats1Position];
			if (format1 == format2)
				{
				secondTry = &format1;
				secondTryCumulativePosition = formats1Position+formats2Position;
				break;
				}
			}

		if (secondTry != NULL)
			break;
		}

	TBool found = EFalse;

	if (firstTry!=NULL)
		{
		ASSERT(secondTry!=NULL);
		found = ETrue;
		// Work out which is cheaper
		if (firstTryCumulativePosition <= secondTryCumulativePosition)
			{
			aCommonFormat = *firstTry;
			}
		else
			{
			aCommonFormat = *secondTry;
			}
		}

	return found;
	}

EXPORT_C void CMMFDevVideoPlay::FindDecodersL(const TDesC8& aMimeType, TUint32 aPostProcType, RArray<TUid>& aDecoders, TBool aExactMatch)
	{
#ifdef SYMBIAN_MULTIMEDIA_CODEC_API
	// find PU based plugins
	RImplInfoPtrArray codecPlugins;
	CleanupResetAndDestroyPushL(codecPlugins);
	DevVideoUtilities::FindVideoDecoderPluginsL(aMimeType, codecPlugins);	

	aDecoders.Reset();
	for (TInt i = 0; i < codecPlugins.Count();i++)
		{
		aDecoders.AppendL(codecPlugins[i]->ImplementationUid());
		}
		
	CleanupStack::PopAndDestroy(&codecPlugins);
#endif // SYMBIAN_MULTIMEDIA_CODEC_API
	RImplInfoPtrArray plugins;
	CleanupResetAndDestroyPushL(plugins);

	MmPluginUtils::FindImplementationsL(KUidDevVideoDecoderHwDevice, plugins);

	DevVideoUtilities::SelectPluginBasedOnMatchType(aMimeType, plugins);
		
	DevVideoUtilities::MatchPrePostProcessorCapabilitiesL(plugins, aPostProcType, aDecoders);

	// Perform the extra processing required if an exact match was requested
	if (aExactMatch)
		{
		// We now need to go through each of the plugins returned (which may have been matched using
		// a wildcard), instantiate each one, and perform an exact mime-type match.
		// Any plugins that don't support the desired mime type exactly will be removed from aDecoders.
		CCompressedVideoFormat* format = CCompressedVideoFormat::NewL(aMimeType);
		CleanupStack::PushL(format);

		for (TInt i=aDecoders.Count()-1; i>=0; i--)
			{
			TUid pluginUid = aDecoders[i];
			CVideoDecoderInfo* info = NULL;
			TRAPD(err, info = VideoDecoderInfoLC(pluginUid); CleanupStack::Pop(info););
			if (err == KErrNone)
				{
				if (!info->SupportsFormat(*format))
					{
					// The decoder doesn't support the format, so remove it from the list
					aDecoders.Remove(i);
					}
				delete info;
				}
			else if (err==KErrNotFound)
				{
				// we have a bogus UID so remove it from the list
				aDecoders.Remove(i);
				}
			else
				{
				User::Leave(err);
				}
			}

		CleanupStack::PopAndDestroy(format);
		}

	// Leave if we didn't find anything.
	if (aDecoders.Count() == 0)
		{
		User::Leave(KErrNotFound);
		}

	CleanupStack::PopAndDestroy(&plugins);
	}

EXPORT_C void CMMFDevVideoPlay::FindPostProcessorsL(TUint32 aPostProcType, RArray<TUid>& aPostProcessors)
	{
	RImplInfoPtrArray plugins;
	CleanupResetAndDestroyPushL(plugins);

	MmPluginUtils::FindImplementationsL(KUidDevVideoPostProcessorHwDevice, plugins);

	DevVideoUtilities::MatchPrePostProcessorCapabilitiesL(plugins, aPostProcType, aPostProcessors);

	// Leave if we didn't find anything.
	if (aPostProcessors.Count() == 0)
		{
		User::Leave(KErrNotFound);
		}

	CleanupStack::PopAndDestroy(&plugins);
	}

EXPORT_C void CMMFDevVideoPlay::GetDecoderListL(RArray<TUid>& aDecoders)
	{
	RImplInfoPtrArray plugins;
	CleanupResetAndDestroyPushL(plugins);

	MmPluginUtils::FindImplementationsL(KUidDevVideoDecoderHwDevice, plugins);

	aDecoders.Reset();

	TInt count = plugins.Count();
	for (TInt i=0; i<count; i++)
		{
		User::LeaveIfError(aDecoders.Append(plugins[i]->ImplementationUid()));
		}

	CleanupStack::PopAndDestroy(&plugins);
	}

EXPORT_C void CMMFDevVideoPlay::GetPostProcessorListL(RArray<TUid>& aPostProcessors)
	{
	RImplInfoPtrArray plugins;
	CleanupResetAndDestroyPushL(plugins);

	MmPluginUtils::FindImplementationsL(KUidDevVideoPostProcessorHwDevice, plugins);

	aPostProcessors.Reset();

	TInt count = plugins.Count();
	for (TInt i=0; i<count; i++)
		{
		User::LeaveIfError(aPostProcessors.Append(plugins[i]->ImplementationUid()));
		}

	CleanupStack::PopAndDestroy(&plugins);
	}

EXPORT_C CVideoDecoderInfo* CMMFDevVideoPlay::VideoDecoderInfoLC(TUid aVideoDecoder)
	{
	CMMFVideoDecodeHwDevice* dev = CreateDecoderL(aVideoDecoder);
	CleanupStack::PushL(dev);
	CVideoDecoderInfo* info = dev->VideoDecoderInfoLC();
	CleanupStack::Pop(info);
	CleanupStack::PopAndDestroy(dev);
	CleanupStack::PushL(info);
	return info;
	}

EXPORT_C CPostProcessorInfo* CMMFDevVideoPlay::PostProcessorInfoLC(TUid aPostProcessor)
	{
	CMMFVideoPlayHwDevice* device = NULL;

	// Determine whether the aPostProcessor uid is for a decoder or a postprocessor hw device
	RArray<TUid> array;
	CleanupClosePushL(array);
	
	GetDecoderListL(array);
	TInt position = array.Find(aPostProcessor);
	if (position == KErrNotFound)
		{
		array.Reset();
		GetPostProcessorListL(array);
		position = array.Find(aPostProcessor);
		if (position != KErrNotFound)
			{
			// uid is a post processor
			device = static_cast<CMMFVideoPlayHwDevice*>(CMMFVideoPostProcHwDevice::NewL(aPostProcessor, *this));
			}
		}
	else
		{
		// uid is a decoder
		device = static_cast<CMMFVideoPlayHwDevice*>(CMMFVideoDecodeHwDevice::NewL(aPostProcessor, *this));
		}
	CleanupStack::PopAndDestroy(&array);

	CPostProcessorInfo* info = NULL;
	if (device)
		{
		CleanupStack::PushL(device);
		info = device->PostProcessorInfoLC();		
		CleanupStack::Pop(info);
		CleanupStack::PopAndDestroy(device);
		CleanupStack::PushL(info);
		}
	return info;
	}

EXPORT_C THwDeviceId CMMFDevVideoPlay::SelectDecoderL(TUid aDecoder)
	{
	// This method can only be called before InitializeL() has been called.
	CheckInitializationState(ENotInitialized);

	delete iVideoDecodeHwDevice;
	iVideoDecodeHwDevice = NULL;
#ifdef SYMBIAN_MULTIMEDIA_CODEC_API
	iVideoDecodeHwDevice = CreateDecoderL(aDecoder);
#else
	iVideoDecodeHwDevice = CMMFVideoDecodeHwDevice::NewL(aDecoder, *this);
#endif // SYMBIAN_MULTIMEDIA_CODEC_API
	return KHwDeviceIdVideoDecoder;
	}

EXPORT_C THwDeviceId CMMFDevVideoPlay::SelectPostProcessorL(TUid aPostProcessor)
	{
	// This method can only be called before InitializeL() has been called.
	CheckInitializationState(ENotInitialized);

	delete iVideoPostProcHwDevice;
	iVideoPostProcHwDevice = NULL;
	iVideoPostProcHwDevice = CMMFVideoPostProcHwDevice::NewL(aPostProcessor, *this);

	return KHwDeviceIdVideoPostProcessor;
	}

EXPORT_C TVideoPictureHeader* CMMFDevVideoPlay::GetHeaderInformationL(TVideoDataUnitType aDataUnitType, TVideoDataUnitEncapsulation aDataUnitEncapsulation, TVideoInputBuffer* aDataUnit)
	{
	return VideoDecodeHwDevice().GetHeaderInformationL(aDataUnitType, aDataUnitEncapsulation, aDataUnit);
	}

/**
Configures the Decoder using header information known by the client.
@param	"aVideoPictureHeader"	"Header information to configure the decoder with"
@leave	"The method will leave if an error occurs. Running out of data is not considered an error, 
		as described above.
@pre	"This method can only be called before the API has been initialized with Initialize()."
*/
EXPORT_C void CMMFDevVideoPlay::ConfigureDecoderL(const TVideoPictureHeader& aVideoPictureHeader)
	{
	CheckInitializationState(ENotInitialized);

	VideoDecodeHwDevice().ConfigureDecoderL(aVideoPictureHeader);
	}



EXPORT_C void CMMFDevVideoPlay::ReturnHeader(TVideoPictureHeader* aHeader)
	{
	// This method can only be called before InitializeL()
	CheckInitializationState(ENotInitialized);

	VideoDecodeHwDevice().ReturnHeader(aHeader);
	}

EXPORT_C void CMMFDevVideoPlay::SetInputFormatL(THwDeviceId aHwDevice, const TUncompressedVideoFormat& aFormat)
	{
	CheckInitializationState(ENotInitialized);

	VideoPostProcHwDevice(aHwDevice).SetInputFormatL(aFormat);
	}

EXPORT_C void CMMFDevVideoPlay::SetInputFormatL(THwDeviceId aHwDevice, const CCompressedVideoFormat& aFormat, TVideoDataUnitType aDataUnitType, TVideoDataUnitEncapsulation aEncapsulation, TBool aDataInOrder)
	{
	CheckInitializationState(ENotInitialized);

	VideoDecodeHwDevice(aHwDevice).SetInputFormatL(aFormat, aDataUnitType, aEncapsulation, aDataInOrder);
	}

EXPORT_C void CMMFDevVideoPlay::GetOutputFormatListL(THwDeviceId aHwDevice, RArray<TUncompressedVideoFormat>& aFormats)
	{
	CheckInitializationState(ENotInitialized);

	VideoPlayHwDevice(aHwDevice).GetOutputFormatListL(aFormats);
	}

EXPORT_C void CMMFDevVideoPlay::SetOutputFormatL(THwDeviceId aHwDevice, const TUncompressedVideoFormat &aFormat)
	{
	CheckInitializationState(ENotInitialized);

	VideoPlayHwDevice(aHwDevice).SetOutputFormatL(aFormat);
	}

EXPORT_C void CMMFDevVideoPlay::SetClockSource(MMMFClockSource* aClock)
	{
	CheckInitializationState(ENotInitialized);

	ASSERT(aClock);

	// Make sure we have at least one of the hwdevices set up
	__ASSERT_ALWAYS((iVideoDecodeHwDevice||iVideoPostProcHwDevice), DevVideoPanic(EDevVideoPanicPreConditionViolation));

	// Set the clock source on both hw devices.
	if (iVideoDecodeHwDevice)
		iVideoDecodeHwDevice->SetClockSource(aClock);
	if (iVideoPostProcHwDevice)
		iVideoPostProcHwDevice->SetClockSource(aClock);
	}

EXPORT_C void CMMFDevVideoPlay::SetVideoDestScreenL(TBool aScreen)
	{
	CheckInitializationState(ENotInitialized);

	// Pass on to the rendering hw device
	RenderingHwDevice().SetVideoDestScreenL(aScreen);
	}

EXPORT_C void CMMFDevVideoPlay::SynchronizeDecoding(TBool aSynchronize)
	{
	CheckInitializationState(ENotInitialized);

	VideoDecodeHwDevice().SynchronizeDecoding(aSynchronize);
	}

EXPORT_C void CMMFDevVideoPlay::SetBufferOptionsL(const TBufferOptions& aOptions)
	{
	CheckInitializationState(ENotInitialized);

	VideoDecodeHwDevice().SetBufferOptionsL(aOptions);
	}

EXPORT_C void CMMFDevVideoPlay::GetBufferOptions(TBufferOptions& aOptions)
	{
	CheckInitializationState(ENotInitialized);

	VideoDecodeHwDevice().GetBufferOptions(aOptions);
	}

EXPORT_C void CMMFDevVideoPlay::SetHrdVbvSpec(THrdVbvSpecification aHrdVbvSpec, const TDesC8& aHrdVbvParams)
	{
	CheckInitializationState(ENotInitialized);

	VideoDecodeHwDevice().SetHrdVbvSpec(aHrdVbvSpec, aHrdVbvParams);
	}

EXPORT_C void CMMFDevVideoPlay::SetPostProcessTypesL(THwDeviceId aHwDevice, TUint32 aPostProcCombination)
	{
	CheckInitializationState(ENotInitialized|EInitialized);
	
	VideoPlayHwDevice(aHwDevice).SetPostProcessTypesL(aPostProcCombination);
	}

EXPORT_C void CMMFDevVideoPlay::SetInputCropOptionsL(THwDeviceId aHwDevice, const TRect& aRect)
	{
	CheckInitializationState(ENotInitialized|EInitialized);

	VideoPlayHwDevice(aHwDevice).SetInputCropOptionsL(aRect);	
	}

EXPORT_C void CMMFDevVideoPlay::SetYuvToRgbOptionsL(THwDeviceId aHwDevice, const TYuvToRgbOptions& aOptions, const TYuvFormat& aYuvFormat, TRgbFormat aRgbFormat)
	{
	CheckInitializationState(ENotInitialized|EInitialized);

	VideoPlayHwDevice(aHwDevice).SetYuvToRgbOptionsL(aOptions, aYuvFormat, aRgbFormat);
	}

EXPORT_C void CMMFDevVideoPlay::SetYuvToRgbOptionsL(THwDeviceId aHwDevice, const TYuvToRgbOptions& aOptions)
	{
	CheckInitializationState(ENotInitialized|EInitialized);

	VideoPlayHwDevice(aHwDevice).SetYuvToRgbOptionsL(aOptions);
	}

EXPORT_C void CMMFDevVideoPlay::SetRotateOptionsL(THwDeviceId aHwDevice, TRotationType aRotationType)
	{
	CheckInitializationState(ENotInitialized|EInitialized);

	VideoPlayHwDevice(aHwDevice).SetRotateOptionsL(aRotationType);
	}

EXPORT_C void CMMFDevVideoPlay::SetScaleOptionsL(THwDeviceId aHwDevice, const TSize& aTargetSize, TBool aAntiAliasFiltering)
	{
	CheckInitializationState(ENotInitialized|EInitialized);

	VideoPlayHwDevice(aHwDevice).SetScaleOptionsL(aTargetSize, aAntiAliasFiltering);
	}

EXPORT_C void CMMFDevVideoPlay::SetOutputCropOptionsL(THwDeviceId aHwDevice, const TRect& aRect)
	{
	CheckInitializationState(ENotInitialized|EInitialized);

	VideoPlayHwDevice(aHwDevice).SetOutputCropOptionsL(aRect);
	}

EXPORT_C void CMMFDevVideoPlay::SetPostProcSpecificOptionsL(THwDeviceId aHwDevice, const TDesC8& aOptions)
	{
	CheckInitializationState(ENotInitialized|EInitialized);

	VideoPlayHwDevice(aHwDevice).SetPostProcSpecificOptionsL(aOptions);
	}

EXPORT_C void CMMFDevVideoPlay::Initialize()
	{
	CheckInitializationState(ENotInitialized);

	// First introduce the two plugins to each other
	ConnectPlugins();

	// Next initialize the first hwdevice in the chain
	if (iVideoDecodeHwDevice)
		{
		iInitializationState = EInitializing;
		iVideoDecodeHwDevice->Initialize();
		}
	else if (iVideoPostProcHwDevice)
		{
		iInitializationState = EInitializing;
		iVideoPostProcHwDevice->Initialize();
		}
	else
		{
		// No hwdevice to initialize so panic
		DevVideoPanic(EDevVideoPanicPreConditionViolation);
		}
	}

EXPORT_C void CMMFDevVideoPlay::StartDirectScreenAccessL(const TRect& aVideoRect, CFbsScreenDevice& aScreenDevice, const TRegion& aClipRegion)
	{
	CheckInitializationState(EInitialized);
	RenderingHwDevice().StartDirectScreenAccessL(aVideoRect, aScreenDevice, aClipRegion);
	}

EXPORT_C void CMMFDevVideoPlay::SetScreenClipRegion(const TRegion& aRegion)
	{
	CheckInitializationState(EInitialized);
	RenderingHwDevice().SetScreenClipRegion(aRegion);
	}

EXPORT_C void CMMFDevVideoPlay::SetPauseOnClipFail(TBool aPause)
	{
	CheckInitializationState(EInitialized);
	RenderingHwDevice().SetPauseOnClipFail(aPause);
	}

EXPORT_C void CMMFDevVideoPlay::AbortDirectScreenAccess()
	{
	CheckInitializationState(EInitialized);
	RenderingHwDevice().AbortDirectScreenAccess();
	}

EXPORT_C TBool CMMFDevVideoPlay::IsPlaying()
	{
	CheckInitializationState(EInitialized);
	return RenderingHwDevice().IsPlaying();
	}

EXPORT_C void CMMFDevVideoPlay::Redraw()
	{
	CheckInitializationState(EInitialized);
	RenderingHwDevice().Redraw();
	}

EXPORT_C void CMMFDevVideoPlay::Start()
	{
	CheckInitializationState(EInitialized);

	if (iVideoDecodeHwDevice)
		iVideoDecodeHwDevice->Start();
	if (iVideoPostProcHwDevice)
		iVideoPostProcHwDevice->Start();
	}

EXPORT_C void CMMFDevVideoPlay::Stop()
	{
	CheckInitializationState(EInitialized);

	if (iVideoDecodeHwDevice)
		iVideoDecodeHwDevice->Stop();
	if (iVideoPostProcHwDevice)
		iVideoPostProcHwDevice->Stop();
	}

EXPORT_C void CMMFDevVideoPlay::Pause()
	{
	CheckInitializationState(EInitialized);

	if (iVideoDecodeHwDevice)
		iVideoDecodeHwDevice->Pause();
	if (iVideoPostProcHwDevice)
		iVideoPostProcHwDevice->Pause();
	}

EXPORT_C void CMMFDevVideoPlay::Resume()
	{
	CheckInitializationState(EInitialized);

	if (iVideoDecodeHwDevice)
		iVideoDecodeHwDevice->Resume();
	if (iVideoPostProcHwDevice)
		iVideoPostProcHwDevice->Resume();
	}

EXPORT_C void CMMFDevVideoPlay::SetPosition(const TTimeIntervalMicroSeconds& aPlaybackPosition)
	{
	CheckInitializationState(EInitialized);

	if (iVideoDecodeHwDevice)
		iVideoDecodeHwDevice->SetPosition(aPlaybackPosition);
	if (iVideoPostProcHwDevice)
		iVideoPostProcHwDevice->SetPosition(aPlaybackPosition);
	}

EXPORT_C void CMMFDevVideoPlay::FreezePicture(const TTimeIntervalMicroSeconds& aPlaybackPosition)
	{
	CheckInitializationState(EInitialized);
	RenderingHwDevice().FreezePicture(aPlaybackPosition);
	}

EXPORT_C void CMMFDevVideoPlay::ReleaseFreeze(const TTimeIntervalMicroSeconds& aPlaybackPosition)
	{
	CheckInitializationState(EInitialized);
	RenderingHwDevice().ReleaseFreeze(aPlaybackPosition);
	}

EXPORT_C TTimeIntervalMicroSeconds CMMFDevVideoPlay::DecodingPosition()
	{
	CheckInitializationState(EInitialized);
	return VideoDecodeHwDevice().DecodingPosition();
	}

EXPORT_C TTimeIntervalMicroSeconds CMMFDevVideoPlay::PlaybackPosition()
	{
	CheckInitializationState(EInitialized);
	return RenderingHwDevice().PlaybackPosition();
	}

EXPORT_C TUint CMMFDevVideoPlay::PreDecoderBufferBytes()
	{
	CheckInitializationState(EInitialized);
	return VideoDecodeHwDevice().PreDecoderBufferBytes();
	}

EXPORT_C TUint CMMFDevVideoPlay::PictureBufferBytes()
	{
	CheckInitializationState(EInitialized);
	return RenderingHwDevice().PictureBufferBytes();
	}

EXPORT_C void CMMFDevVideoPlay::GetPictureCounters(TPictureCounters& aCounters)
	{
	CheckInitializationState(EInitialized);

	TPictureCounters decoderPictureCounters;
	TPictureCounters postProcPictureCounters;

	if (iVideoDecodeHwDevice)
		iVideoDecodeHwDevice->GetPictureCounters(decoderPictureCounters);

	if (iVideoPostProcHwDevice)
		iVideoPostProcHwDevice->GetPictureCounters(postProcPictureCounters);


	// Only the decoder (if any) knows the number of pictures decoded
	aCounters.iPicturesDecoded = decoderPictureCounters.iPicturesDecoded;

	// Pictures skipped is sum of pictures skipped from decoder and postproc
	aCounters.iPicturesSkipped = decoderPictureCounters.iPicturesSkipped
									+ postProcPictureCounters.iPicturesSkipped;

	// Ask last plugin in chain for number of pictures displayed
	if (iVideoPostProcHwDevice)
		aCounters.iPicturesDisplayed = postProcPictureCounters.iPicturesDisplayed;
	else
		aCounters.iPicturesDisplayed = decoderPictureCounters.iPicturesDisplayed;

	// Ask first plugin in chain for number of input pictures
	if (iVideoDecodeHwDevice)
		aCounters.iTotalPictures = decoderPictureCounters.iTotalPictures;
	else
		aCounters.iTotalPictures = postProcPictureCounters.iTotalPictures;
	}

EXPORT_C void CMMFDevVideoPlay::GetBitstreamCounters(TBitstreamCounters& aCounters)
	{
	CheckInitializationState(EInitialized);
	VideoDecodeHwDevice().GetBitstreamCounters(aCounters);
	}

EXPORT_C TUint CMMFDevVideoPlay::NumFreeBuffers()
	{
	CheckInitializationState(EInitialized);
	return VideoDecodeHwDevice().NumFreeBuffers();
	}

EXPORT_C void CMMFDevVideoPlay::SetComplexityLevel(THwDeviceId aHwDevice, TUint aLevel)
	{
	CheckInitializationState(EInitialized);
	VideoPlayHwDevice(aHwDevice).SetComplexityLevel(aLevel);
	}

EXPORT_C TUint CMMFDevVideoPlay::NumComplexityLevels(THwDeviceId aHwDevice)
	{
	CheckInitializationState(EInitialized);
	return VideoPlayHwDevice(aHwDevice).NumComplexityLevels();
	}

EXPORT_C void CMMFDevVideoPlay::GetComplexityLevelInfo(THwDeviceId aHwDevice, TUint aLevel, TComplexityLevelInfo& aInfo)
	{
	CheckInitializationState(EInitialized);
	VideoPlayHwDevice(aHwDevice).GetComplexityLevelInfo(aLevel, aInfo);
	}

EXPORT_C TVideoInputBuffer* CMMFDevVideoPlay::GetBufferL(TUint aBufferSize)
	{
	return VideoDecodeHwDevice().GetBufferL(aBufferSize);
	}

EXPORT_C void CMMFDevVideoPlay::WriteCodedDataL(TVideoInputBuffer* aBuffer)
	{
	CheckInitializationState(EInitialized);
	VideoDecodeHwDevice().WriteCodedDataL(aBuffer);
	}

EXPORT_C void CMMFDevVideoPlay::WritePictureL(TVideoPicture* aPicture)
	{
	CheckInitializationState(EInitialized);
	VideoPostProcHwDevice().WritePictureL(aPicture);
	}

EXPORT_C void CMMFDevVideoPlay::InputEnd()
	{
	CheckInitializationState(EInitialized);
	
	//If decoder and post-processor exist,InputEnd() of decoder is issued
	//and wait for reply from it before issuing PostProcessor InputEnd()
	//iIsDecodeComplete is EFalse, until decoder responds
	iIsDecodeComplete=EFalse;
	
	if (iVideoDecodeHwDevice)
		{
		iVideoDecodeHwDevice->InputEnd();
		return;
		}
	if (iVideoPostProcHwDevice)
		{
		iVideoPostProcHwDevice->InputEnd();	
		}
	}

EXPORT_C void CMMFDevVideoPlay::GetNewPictureInfo(TUint& aNumPictures, TTimeIntervalMicroSeconds& aEarliestTimestamp, TTimeIntervalMicroSeconds& aLatestTimestamp)
	{
	CheckInitializationState(EInitialized);

	aNumPictures = iNumberOfVideoPictures;
	if (iNumberOfVideoPictures > 0)
		{
		aEarliestTimestamp = iVideoPictureQue.First()->iTimestamp;
		aLatestTimestamp = iVideoPictureQue.Last()->iTimestamp;
		}
	}

EXPORT_C TVideoPicture* CMMFDevVideoPlay::NextPictureL()
	{

	CheckInitializationState(EInitialized);

	if (iNumberOfVideoPictures == 0)
		return NULL;

	TVideoPicture* picture = iVideoPictureQue.First();

	// Remove the picture from the list
	picture->iLink.Deque();
	iNumberOfVideoPictures--;

	return picture;
	}

EXPORT_C void CMMFDevVideoPlay::ReturnPicture(TVideoPicture* aPicture)
	{
	CheckInitializationState(EInitialized);

	// Return the picture to the hwdevice
	RenderingHwDevice().ReturnPicture(aPicture);
	}

EXPORT_C TBool CMMFDevVideoPlay::GetSnapshotL(TPictureData& aPictureData, const TUncompressedVideoFormat& aFormat)
	{
	CheckInitializationState(EInitialized);
	return RenderingHwDevice().GetSnapshotL(aPictureData, aFormat);
	}

EXPORT_C void CMMFDevVideoPlay::GetTimedSnapshotL(TPictureData* aPictureData, const TUncompressedVideoFormat& aFormat, const TTimeIntervalMicroSeconds& aPresentationTimestamp)
	{
	CheckInitializationState(EInitialized);
	RenderingHwDevice().GetTimedSnapshotL(aPictureData, aFormat, aPresentationTimestamp);
	}

EXPORT_C void CMMFDevVideoPlay::GetTimedSnapshotL(TPictureData* aPictureData, const TUncompressedVideoFormat& aFormat, const TPictureId& aPictureId)
	{
	CheckInitializationState(EInitialized);
	RenderingHwDevice().GetTimedSnapshotL(aPictureData, aFormat, aPictureId);
	}

EXPORT_C void CMMFDevVideoPlay::CancelTimedSnapshot()
	{
	CheckInitializationState(EInitialized);
	RenderingHwDevice().CancelTimedSnapshot();
	}

EXPORT_C void CMMFDevVideoPlay::GetSupportedSnapshotFormatsL(RArray<TUncompressedVideoFormat>& aFormats)
	{
	CheckInitializationState(EInitialized);
	RenderingHwDevice().GetSupportedSnapshotFormatsL(aFormats);
	}

EXPORT_C void CMMFDevVideoPlay::CommitL()
	{
	CheckInitializationState(EInitialized);
	if (iVideoDecodeHwDevice)
		iVideoDecodeHwDevice->CommitL();
	if (iVideoPostProcHwDevice)
		iVideoPostProcHwDevice->CommitL();
	}

EXPORT_C void CMMFDevVideoPlay::Revert()
	{
	CheckInitializationState(EInitialized);
	if (iVideoDecodeHwDevice)
		iVideoDecodeHwDevice->Revert();
	if (iVideoPostProcHwDevice)
		iVideoPostProcHwDevice->Revert();
	}

EXPORT_C TAny* CMMFDevVideoPlay::CustomInterface(THwDeviceId aHwDevice, TUid aInterface)
	{
	CheckInitializationState(ENotInitialized|EInitialized);
	return VideoHwDevice(aHwDevice).CustomInterface(aInterface);
	}

void CMMFDevVideoPlay::MdvppNewPicture(TVideoPicture* aPicture)
	{
	iVideoPictureQue.AddLast(*aPicture);
	iNumberOfVideoPictures++;

	iObserver.MdvpoNewPictures();
	}

void CMMFDevVideoPlay::MdvppNewBuffers()
	{
	iObserver.MdvpoNewBuffers();
	}

void CMMFDevVideoPlay::MdvppReturnPicture(TVideoPicture* aPicture)
	{
	iObserver.MdvpoReturnPicture(aPicture);
	}

void CMMFDevVideoPlay::MdvppSupplementalInformation(const TDesC8& aData, const TTimeIntervalMicroSeconds& aTimestamp, const TPictureId& aPictureId)
	{
	iObserver.MdvpoSupplementalInformation(aData, aTimestamp, aPictureId);
	}

void CMMFDevVideoPlay::MdvppPictureLoss()
	{
	iObserver.MdvpoPictureLoss();
	}

void CMMFDevVideoPlay::MdvppPictureLoss(const TArray<TPictureId>& aPictures)
	{
	iObserver.MdvpoPictureLoss(aPictures);
	}

void CMMFDevVideoPlay::MdvppSliceLoss(TUint aFirstMacroblock, TUint aNumMacroblocks, const TPictureId& aPicture)
	{
	iObserver.MdvpoSliceLoss(aFirstMacroblock, aNumMacroblocks, aPicture);
	}

void CMMFDevVideoPlay::MdvppReferencePictureSelection(const TDesC8& aSelectionData)
	{
	iObserver.MdvpoReferencePictureSelection(aSelectionData);
	}

void CMMFDevVideoPlay::MdvppTimedSnapshotComplete(TInt aError, TPictureData* aPictureData, const TTimeIntervalMicroSeconds& aPresentationTimestamp, const TPictureId& aPictureId)
	{
	iObserver.MdvpoTimedSnapshotComplete(aError, aPictureData, aPresentationTimestamp, aPictureId);
	}

void CMMFDevVideoPlay::MdvppFatalError(CMMFVideoHwDevice* aDevice, TInt aError)
	{
	// Make sure the other plugin gets stopped
	__ASSERT_DEBUG((aDevice!=NULL) && ((aDevice==iVideoDecodeHwDevice) || (aDevice==iVideoPostProcHwDevice)), DevVideoPanic(EDevVideoPanicPreConditionViolation));
	if ((aDevice == iVideoDecodeHwDevice) && (iVideoPostProcHwDevice))
		iVideoPostProcHwDevice->Stop();
	else if ((aDevice == iVideoPostProcHwDevice) && (iVideoDecodeHwDevice))
		iVideoDecodeHwDevice->Stop();

	// Place the object in the "initialization failed" state to prevent any more method calls
	// This state is purely internal so this is ok.
	iInitializationState = EInitializationFailed;

	iObserver.MdvpoFatalError(aError);
	}

void CMMFDevVideoPlay::MdvppInitializeComplete(CMMFVideoHwDevice* aDevice, TInt aError)
	{
	if (aDevice == iVideoDecodeHwDevice)
		{
		HandleDecodeInitializeComplete(aError);
		}
	else if (aDevice == iVideoPostProcHwDevice)
		{
		HandlePostProcInitializeComplete(aError);
		}
	else
		{// We've been given a random pointer back!
		DevVideoPanic(EDevVideoPanicPreConditionViolation);
		}
	}


void CMMFDevVideoPlay::HandleDecodeInitializeComplete(TInt aError)
	{
	// Make sure we are in the correct state
	__ASSERT_ALWAYS(iInitializationState==EInitializing, DevVideoPanic(EDevVideoPanicPreConditionViolation));

	if (aError == KErrNone)
		{
		// If we have a post processor, initialize that.  Otherwise, we're initialized.
		if (iVideoPostProcHwDevice)
			{
			iVideoPostProcHwDevice->Initialize();
			// Now wait for the next callback
			}
		else
			{
			iInitializationState = EInitialized;
			iObserver.MdvpoInitComplete(KErrNone);
			}
		}
	else
		{//aError
		iInitializationState = EInitializationFailed;
		iObserver.MdvpoInitComplete(aError);
		}
	}

void CMMFDevVideoPlay::HandlePostProcInitializeComplete(TInt aError)
	{
	// Make sure we are in the correct state
	__ASSERT_ALWAYS(iInitializationState==EInitializing, DevVideoPanic(EDevVideoPanicPreConditionViolation));

	if (aError == KErrNone)
		{
		// We're now fully initialized
		iInitializationState = EInitialized;
		iObserver.MdvpoInitComplete(KErrNone);
		}
	else
		{//aError
		iInitializationState = EInitializationFailed;
		iObserver.MdvpoInitComplete(aError);
		}
	}


void CMMFDevVideoPlay::MdvppStreamEnd()
	{
	// This method must be called the same number of times as the number of plugins we have
	// before we call iObserver.MdvpoStreamEnd().
	iNumberOfMdvppStreamEndCallbacks++;
	
	TUint numberOfPlugins = 0;
	if (iVideoDecodeHwDevice && !iIsDecodeComplete)
		{
		iIsDecodeComplete = ETrue;
		if(iVideoPostProcHwDevice)
			{
			iVideoPostProcHwDevice->InputEnd();
			return;
			}
		}
		
	ASSERT(!iVideoDecodeHwDevice||iIsDecodeComplete);
	if(iVideoDecodeHwDevice)
		{
		numberOfPlugins++;
		}
	
	if (iVideoPostProcHwDevice)
		{
		numberOfPlugins++;
		}

	if (iNumberOfMdvppStreamEndCallbacks == numberOfPlugins)
		{
		// We've received the correct number of callbacks so signal the client
		iNumberOfMdvppStreamEndCallbacks = 0;
		iObserver.MdvpoStreamEnd();
		}
	// else wait for the next plugin to call back
	}

CMMFVideoPlayHwDevice& CMMFDevVideoPlay::VideoPlayHwDevice(THwDeviceId aHwDevice) const
	{
	if (aHwDevice == KHwDeviceIdVideoDecoder)
		{
		__ASSERT_ALWAYS(iVideoDecodeHwDevice, DevVideoPanic(EDevVideoPanicPreConditionViolation));
		return *iVideoDecodeHwDevice;
		}
	else if (aHwDevice == KHwDeviceIdVideoPostProcessor)
		{
		__ASSERT_ALWAYS(iVideoPostProcHwDevice, DevVideoPanic(EDevVideoPanicPreConditionViolation));
		return *iVideoPostProcHwDevice;
		}
	else
		{
		DevVideoPanic(EDevVideoPanicInvalidHwDeviceId);
		// The following is unreachable
		CMMFVideoPlayHwDevice* ret = NULL;
		return *ret;
		}
	}

CMMFVideoPlayHwDevice& CMMFDevVideoPlay::RenderingHwDevice() const
	{
	// Return the last plugin in the chain
	//  - the postprocessor if available, otherwise the videodecoder
	if (iVideoPostProcHwDevice)
		return *iVideoPostProcHwDevice;
	else if (iVideoDecodeHwDevice)
		return *iVideoDecodeHwDevice;
	else
		{
		DevVideoPanic(EDevVideoPanicPreConditionViolation);
		// The following is unreachable
		CMMFVideoPlayHwDevice* ret = NULL;
		return *ret;
		}
	}


CMMFVideoDecodeHwDevice& CMMFDevVideoPlay::VideoDecodeHwDevice(THwDeviceId aHwDevice) const
	{
	if (aHwDevice == KHwDeviceIdVideoDecoder)
		{
		__ASSERT_ALWAYS(iVideoDecodeHwDevice, DevVideoPanic(EDevVideoPanicPreConditionViolation));
		return *iVideoDecodeHwDevice;
		}
	else
		{
		DevVideoPanic(EDevVideoPanicInvalidHwDeviceId);
		// The following is unreachable
		CMMFVideoDecodeHwDevice* ret = NULL;
		return *ret;
		}
	}

CMMFVideoPostProcHwDevice& CMMFDevVideoPlay::VideoPostProcHwDevice(THwDeviceId aHwDevice) const
	{
	if (aHwDevice == KHwDeviceIdVideoPostProcessor)
		{
		__ASSERT_ALWAYS(iVideoPostProcHwDevice, DevVideoPanic(EDevVideoPanicPreConditionViolation));
		return *iVideoPostProcHwDevice;
		}
	else
		{
		DevVideoPanic(EDevVideoPanicInvalidHwDeviceId);
		// The following is unreachable
		CMMFVideoPostProcHwDevice* ret = NULL;
		return *ret;
		}
	}

CMMFVideoDecodeHwDevice& CMMFDevVideoPlay::VideoDecodeHwDevice() const
	{
	return VideoDecodeHwDevice(KHwDeviceIdVideoDecoder);
	}

CMMFVideoPostProcHwDevice& CMMFDevVideoPlay::VideoPostProcHwDevice() const
	{
	return VideoPostProcHwDevice(KHwDeviceIdVideoPostProcessor);
	}

CMMFVideoHwDevice& CMMFDevVideoPlay::VideoHwDevice(THwDeviceId aHwDevice) const
	{
	if (aHwDevice == KHwDeviceIdVideoPostProcessor)
		{
		__ASSERT_ALWAYS(iVideoPostProcHwDevice, DevVideoPanic(EDevVideoPanicPreConditionViolation));
		return *iVideoPostProcHwDevice;
		}
	else if (aHwDevice == KHwDeviceIdVideoDecoder)
		{
		__ASSERT_ALWAYS(iVideoDecodeHwDevice, DevVideoPanic(EDevVideoPanicPreConditionViolation));
		return *iVideoDecodeHwDevice;
		}
	else
		{
		DevVideoPanic(EDevVideoPanicInvalidHwDeviceId);
		// The following is unreachable
		CMMFVideoHwDevice* ret = NULL;
		return *ret;
		}
	}

void CMMFDevVideoPlay::CheckInitializationState(TUint aExpected)
	{
	__ASSERT_ALWAYS(iInitializationState&aExpected, DevVideoPanic(EDevVideoPanicPreConditionViolation));
	}

void CMMFDevVideoPlay::ConnectPlugins()
	{
	__ASSERT_ALWAYS((iVideoDecodeHwDevice||iVideoPostProcHwDevice), DevVideoPanic(EDevVideoPanicPreConditionViolation));
	if (iVideoDecodeHwDevice && iVideoPostProcHwDevice)
		{
		iVideoDecodeHwDevice->SetOutputDevice(iVideoPostProcHwDevice);
		iVideoPostProcHwDevice->SetInputDevice(iVideoDecodeHwDevice);
		}
	}
	
CMMFVideoDecodeHwDevice* CMMFDevVideoPlay::CreateDecoderL(TUid aDecoder)
	{
#ifdef SYMBIAN_MULTIMEDIA_CODEC_API	
	// need to check whether we've  been given a PU
	if (!iPuListCreated)
		{
		DevVideoUtilities::CreatePuListL(iPuImplementations);
		iPuListCreated = ETrue;
		}
	const CImplementationInformation* info = DevVideoUtilities::FindPu(iPuImplementations, aDecoder);
		
	if (!info)
		{
		return CMMFVideoDecodeHwDevice::NewL(aDecoder, *this);
		}
	else
		{
		return CMMFVideoDecodeHwDevice::NewPuAdapterL(*info, *this);
		}
#else
	return CMMFVideoDecodeHwDevice::NewL(aDecoder, *this);
#endif // SYMBIAN_MULTIMEDIA_CODEC_API
	}

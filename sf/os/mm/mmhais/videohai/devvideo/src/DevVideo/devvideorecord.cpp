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
#include "devvideorecord.h"
#include "videorecordhwdevice.h"
#include "devvideointernal.h"
#include <mm/mmpluginutils.h>

const THwDeviceId KHwDeviceIdVideoEncoder = 1;
const THwDeviceId KHwDeviceIdVideoPreProcessor = 2;

//
// CMMFVideoEncodeHwDevice
//
EXPORT_C CMMFVideoEncodeHwDevice* CMMFVideoEncodeHwDevice::NewL(TUid aUid, MMMFDevVideoRecordProxy& aProxy)
	{
	CMMFVideoEncodeHwDevice* s = reinterpret_cast<CMMFVideoEncodeHwDevice*>
		(REComSession::CreateImplementationL(aUid, _FOFF(CMMFVideoEncodeHwDevice,iDtor_ID_Key)));
	s->SetProxy(aProxy);
	return s;
	}

#ifdef SYMBIAN_MULTIMEDIA_CODEC_API

const TInt KUidMDFVideoEncoderHwDeviceAdapter = 0x102737EB;

EXPORT_C CMMFVideoEncodeHwDevice* CMMFVideoEncodeHwDevice::NewPuAdapterL(const CImplementationInformation& aInfo, MMMFDevVideoRecordProxy& aProxy)
	{
	CMMFVideoEncodeHwDevice* s = NewL(TUid::Uid(KUidMDFVideoEncoderHwDeviceAdapter), aProxy);
	CleanupStack::PushL(s);

	MDevVideoHwDeviceAdapterSetup* setup = static_cast<MDevVideoHwDeviceAdapterSetup*>(
			s->CustomInterface(TUid::Uid(KUidDevVideoHwDeviceAdapterSetup)));

	if (setup!=NULL)
		{
		setup->LoadProcessingUnitL(aInfo);
		}
	CleanupStack::Pop(s);
	return s;
	}
#else
EXPORT_C CMMFVideoEncodeHwDevice* CMMFVideoEncodeHwDevice::NewPuAdapterL(const CImplementationInformation& /*aInfo*/, MMMFDevVideoRecordProxy& /*aProxy*/)
	{
	User::Leave(KErrNotSupported);
	return NULL;
	}
#endif // SYMBIAN_MULTIMEDIA_CODEC_API

EXPORT_C CMMFVideoEncodeHwDevice::~CMMFVideoEncodeHwDevice()
	{
	REComSession::DestroyedImplementation(iDtor_ID_Key);
	}

EXPORT_C CMMFVideoEncodeHwDevice::CMMFVideoEncodeHwDevice()
	{
	}

//
// CMMFVideoPreProcHwDevice
//
EXPORT_C CMMFVideoPreProcHwDevice* CMMFVideoPreProcHwDevice::NewL(TUid aUid, MMMFDevVideoRecordProxy& aProxy)
	{
	CMMFVideoPreProcHwDevice* s = reinterpret_cast<CMMFVideoPreProcHwDevice*>
		(REComSession::CreateImplementationL(aUid, _FOFF(CMMFVideoPreProcHwDevice,iDtor_ID_Key)));
	s->SetProxy(aProxy);
	return s;
	}

EXPORT_C CMMFVideoPreProcHwDevice::~CMMFVideoPreProcHwDevice()
	{
	REComSession::DestroyedImplementation(iDtor_ID_Key);
	}

EXPORT_C CMMFVideoPreProcHwDevice::CMMFVideoPreProcHwDevice()
	{
	}


//
// CVideoEncoderInfo
//

EXPORT_C CVideoEncoderInfo* CVideoEncoderInfo::NewL(TUid aUid,
													const TDesC& aManufacturer,
													const TDesC& aIdentifier,
													TVersion aVersion,
													TBool aAccelerated,
													TBool aSupportsDirectCapture,
													const TArray<TUncompressedVideoFormat>& aSupportedInputFormats,
													const TArray<CCompressedVideoFormat*>& aSupportedOutputFormats,
													const TSize& aMaxPictureSize,
													TUint32 aSupportedDataUnitTypes,
													TUint32 aSupportedDataUnitEncapsulations,
													TUint aMaxBitrateLayers,
													TBool aSupportsSupplementalEnhancementInfo,
													TUint aMaxUnequalErrorProtectionLevels,
													TUint aMaxBitRate,
													const TArray<TPictureRateAndSize>& aMaxPictureRates,
													TUint aMaxInLayerScalabilitySteps,
													TUint32 aSupportedPictureOptions,
													TBool aSupportsPictureLoss,
													TBool aSupportsSliceLoss,
													const TDesC8& aCodingStandardSpecificInfo,
													const TDesC8& aImplementationSpecificInfo)
	{
	CVideoEncoderInfo* s = new(ELeave) CVideoEncoderInfo(aUid,
														 aVersion,
														 aAccelerated,
														 aSupportsDirectCapture,
														 aMaxPictureSize,
														 aSupportedDataUnitTypes,
														 aSupportedDataUnitEncapsulations,
														 aMaxBitrateLayers,
														 aSupportsSupplementalEnhancementInfo,
														 aMaxUnequalErrorProtectionLevels,
														 aMaxBitRate,
														 aMaxInLayerScalabilitySteps,
														 aSupportedPictureOptions,
														 aSupportsPictureLoss,
														 aSupportsSliceLoss);

	CleanupStack::PushL(s);

	s->ConstructL(aManufacturer,
				  aIdentifier,
				  aSupportedInputFormats,
				  aSupportedOutputFormats,
				  aMaxPictureRates,
				  aCodingStandardSpecificInfo,
				  aImplementationSpecificInfo);

	CleanupStack::Pop(s);
	return s;
	}

CVideoEncoderInfo::CVideoEncoderInfo(TUid aUid,
									 TVersion aVersion,
									 TBool aAccelerated,
									 TBool aSupportsDirectCapture,
									 const TSize& aMaxPictureSize,
									 TUint32 aSupportedDataUnitTypes,
									 TUint32 aSupportedDataUnitEncapsulations,
									 TUint aMaxBitrateLayers,
									 TBool aSupportsSupplementalEnhancementInfo,
									 TUint aMaxUnequalErrorProtectionLevels,
									 TUint aMaxBitRate,
									 TUint aMaxInLayerScalabilitySteps,
									 TUint32 aSupportedPictureOptions,
									 TBool aSupportsPictureLoss,
									 TBool aSupportsSliceLoss) :
	iUid(aUid),
	iVersion(aVersion),
	iAccelerated(aAccelerated),
	iSupportsDirectCapture(aSupportsDirectCapture),
	iMaxPictureSize(aMaxPictureSize),
	iSupportedDataUnitTypes(aSupportedDataUnitTypes),
	iSupportedDataUnitEncapsulations(aSupportedDataUnitEncapsulations),
	iMaxBitrateLayers(aMaxBitrateLayers),
	iSupportsSupplementalEnhancementInfo(aSupportsSupplementalEnhancementInfo),
	iMaxUnequalErrorProtectionLevels(aMaxUnequalErrorProtectionLevels),
	iMaxBitRate(aMaxBitRate),
	iMaxInLayerScalabilitySteps(aMaxInLayerScalabilitySteps),
	iSupportedPictureOptions(aSupportedPictureOptions),
	iSupportsPictureLoss(aSupportsPictureLoss),
	iSupportsSliceLoss(aSupportsSliceLoss)
	{
	}

void CVideoEncoderInfo::ConstructL(const TDesC& aManufacturer,
								   const TDesC& aIdentifier,
								   const TArray<TUncompressedVideoFormat>& aSupportedInputFormats,
								   const TArray<CCompressedVideoFormat*>& aSupportedOutputFormats,
								   const TArray<TPictureRateAndSize>& aMaxPictureRates,
								   const TDesC8& aCodingStandardSpecificInfo,
								   const TDesC8& aImplementationSpecificInfo)
	{
	iManufacturer = aManufacturer.AllocL();
	iIdentifier = aIdentifier.AllocL();
	iCodingStandardSpecificInfo = aCodingStandardSpecificInfo.AllocL();
	iImplementationSpecificInfo = aImplementationSpecificInfo.AllocL();

	TInt i;
	TInt count = aSupportedInputFormats.Count();
	for (i=0; i<count; i++)
		{
		User::LeaveIfError(iSupportedInputFormats.Append(aSupportedInputFormats[i]));
		}

	count = aSupportedOutputFormats.Count();
	for (i=0; i<count; i++)
		{
		CCompressedVideoFormat* f = CCompressedVideoFormat::NewL(*(aSupportedOutputFormats[i]));
		CleanupStack::PushL(f);
		User::LeaveIfError(iSupportedOutputFormats.Append(f));
		CleanupStack::Pop(f);
		}

	count = aMaxPictureRates.Count();
	for (i=0; i<count; i++)
		{
		User::LeaveIfError(iMaxPictureRates.Append(aMaxPictureRates[i]));
		}
	}

EXPORT_C CVideoEncoderInfo::~CVideoEncoderInfo()
	{
	delete iManufacturer;
	delete iIdentifier;
	delete iCodingStandardSpecificInfo;
	delete iImplementationSpecificInfo;
	iSupportedInputFormats.Reset();
	iSupportedInputFormats.Close();
	iSupportedOutputFormats.ResetAndDestroy();
	iSupportedOutputFormats.Close();
	iMaxPictureRates.Reset();
	iMaxPictureRates.Close();
	}

EXPORT_C TUid CVideoEncoderInfo::Uid() const
	{
	return iUid;
	}

EXPORT_C TVersion CVideoEncoderInfo::Version() const
	{
	return iVersion;
	}

EXPORT_C const TDesC& CVideoEncoderInfo::Manufacturer() const
	{
	return *iManufacturer;
	}

EXPORT_C const TDesC& CVideoEncoderInfo::Identifier() const
	{
	return *iIdentifier;
	}

EXPORT_C TBool CVideoEncoderInfo::Accelerated() const
	{
	return iAccelerated;
	}

EXPORT_C TBool CVideoEncoderInfo::SupportsDirectCapture() const
	{
	return iSupportsDirectCapture;
	}

EXPORT_C TBool CVideoEncoderInfo::SupportsInputFormat(const TUncompressedVideoFormat& aFormat) const
	{
	TBool found = EFalse;
	TInt count = iSupportedInputFormats.Count();
	for (TInt i=0; i<count; i++)
		{
		if (iSupportedInputFormats[i] == aFormat)
			{
			found = ETrue;
			break;
			}
		}
	return found;
	}

EXPORT_C const RArray<TUncompressedVideoFormat>& CVideoEncoderInfo::SupportedInputFormats() const
	{
	return iSupportedInputFormats;
	}

EXPORT_C TBool CVideoEncoderInfo::SupportsOutputFormat(const CCompressedVideoFormat& aFormat) const
	{
	TBool found = EFalse;
	TInt count = iSupportedOutputFormats.Count();
	for (TInt i=0; i<count; i++)
		{
		if (*(iSupportedOutputFormats[i]) == aFormat)
			{
			found = ETrue;
			break;
			}
		}
	return found;
	}

EXPORT_C const RPointerArray<CCompressedVideoFormat>& CVideoEncoderInfo::SupportedOutputFormats() const
	{
	return iSupportedOutputFormats;
	}

EXPORT_C const TSize& CVideoEncoderInfo::MaxPictureSize() const
	{
	return iMaxPictureSize;
	}

EXPORT_C TUint32 CVideoEncoderInfo::SupportedDataUnitTypes() const
	{
	return iSupportedDataUnitTypes;
	}

EXPORT_C TUint32 CVideoEncoderInfo::SupportedDataUnitEncapsulations() const
	{
	return iSupportedDataUnitEncapsulations;
	}

EXPORT_C TUint CVideoEncoderInfo::MaxBitrateLayers() const
	{
	return iMaxBitrateLayers;
	}

EXPORT_C TBool CVideoEncoderInfo::SupportsSupplementalEnhancementInfo() const
	{
	return iSupportsSupplementalEnhancementInfo;
	}

EXPORT_C TUint CVideoEncoderInfo::MaxUnequalErrorProtectionLevels() const
	{
	return iMaxUnequalErrorProtectionLevels;
	}

EXPORT_C TUint CVideoEncoderInfo::MaxBitrate() const
	{
	return iMaxBitRate;
	}

EXPORT_C const RArray<TPictureRateAndSize>& CVideoEncoderInfo::MaxPictureRates() const
	{
	return iMaxPictureRates;
	}

EXPORT_C TUint CVideoEncoderInfo::MaxInLayerScalabilitySteps() const
	{
	return iMaxInLayerScalabilitySteps;
	}

EXPORT_C TUint32 CVideoEncoderInfo::SupportedPictureOptions() const
	{
	return iSupportedPictureOptions;
	}

EXPORT_C TBool CVideoEncoderInfo::SupportsPictureLoss() const
	{
	return iSupportsPictureLoss;
	}

EXPORT_C TBool CVideoEncoderInfo::SupportsSliceLoss() const
	{
	return iSupportsSliceLoss;
	}

EXPORT_C const TDesC8& CVideoEncoderInfo::CodingStandardSpecificInfo() const
	{
	return *iCodingStandardSpecificInfo;
	}

EXPORT_C const TDesC8& CVideoEncoderInfo::ImplementationSpecificInfo() const
	{
	return *iImplementationSpecificInfo;
	}


//
// CPreProcessorInfo
//
EXPORT_C CPreProcessorInfo* CPreProcessorInfo::NewL(TUid aUid,
													const TDesC& aManufacturer,
													const TDesC& aIdentifier,
													TVersion aVersion,
													TBool aAccelerated,
													TBool aSupportsDirectCapture,
													const TArray<TUncompressedVideoFormat>& aInputFormats,
													const TArray<TUncompressedVideoFormat>& aOutputFormats,
													const TArray<TUint32>& aSupportedCombinations,
													TBool aSupportsArbitraryScaling,
													TBool aSupportsAntiAliasedScaling,
													const TArray<TScaleFactor>& aSupportedScaleFactors,
													const TYuvToYuvCapabilities& aYuvToYuvCapabilities,
													TUint32 aSupportedRgbRanges,
													TUint32 aSupportedRotations,
													const TDesC8& aImplementationSpecificInfo)
	{
	CPreProcessorInfo* s = new(ELeave) CPreProcessorInfo(aUid,
														 aVersion,
														 aAccelerated,
														 aSupportsDirectCapture,
														 aSupportsArbitraryScaling,
														 aSupportsAntiAliasedScaling,
														 aYuvToYuvCapabilities,
														 aSupportedRgbRanges,
														 aSupportedRotations);
	CleanupStack::PushL(s);
	s->ConstructL(aManufacturer,
				  aIdentifier,
				  aInputFormats,
				  aOutputFormats,
				  aSupportedCombinations,
				  aSupportedScaleFactors,
				  aImplementationSpecificInfo);
	CleanupStack::Pop(s);
	return s;
	}

CPreProcessorInfo::CPreProcessorInfo(TUid aUid,
									 TVersion aVersion,
									 TBool aAccelerated,
									 TBool aSupportsDirectCapture,
									 TBool aSupportsArbitraryScaling,
									 TBool aSupportsAntiAliasedScaling,
									 const TYuvToYuvCapabilities& aYuvToYuvCapabilities,
									 TUint32 aSupportedRgbRanges,
									 TUint32 aSupportedRotations) :
	iUid(aUid),
	iVersion(aVersion),
	iAccelerated(aAccelerated),
	iSupportsDirectCapture(aSupportsDirectCapture),
	iSupportsArbitraryScaling(aSupportsArbitraryScaling),
	iSupportsAntiAliasedScaling(aSupportsAntiAliasedScaling),
	iYuvToYuvCapabilities(aYuvToYuvCapabilities),
	iSupportedRgbRanges(aSupportedRgbRanges),
	iSupportedRotations(aSupportedRotations)	
	{
	}

void CPreProcessorInfo::ConstructL(const TDesC& aManufacturer,
								   const TDesC& aIdentifier,
								   const TArray<TUncompressedVideoFormat>& aInputFormats,
								   const TArray<TUncompressedVideoFormat>& aOutputFormats,
								   const TArray<TUint32>& aSupportedCombinations,
								   const TArray<TScaleFactor>& aSupportedScaleFactors,
								   const TDesC8& aImplementationSpecificInfo)
	{
	iManufacturer = aManufacturer.AllocL();
	iIdentifier = aIdentifier.AllocL();
	iImplementationSpecificInfo = aImplementationSpecificInfo.AllocL();

	TInt i;
	TInt count;

	count = aInputFormats.Count();
	for (i=0; i<count; i++)
		{
		User::LeaveIfError(iInputFormats.Append(aInputFormats[i]));
		}

	count = aOutputFormats.Count();
	for (i=0; i<count; i++)
		{
		User::LeaveIfError(iOutputFormats.Append(aOutputFormats[i]));
		}

	count = aSupportedCombinations.Count();
	for (i=0; i<count; i++)
		{
		User::LeaveIfError(iSupportedCombinations.Append(aSupportedCombinations[i]));
		}

	count = aSupportedScaleFactors.Count();
	for (i=0; i<count; i++)
		{
		User::LeaveIfError(iSupportedScaleFactors.Append(aSupportedScaleFactors[i]));
		}
	}

EXPORT_C CPreProcessorInfo::~CPreProcessorInfo()
	{
	delete iManufacturer;
	delete iIdentifier;
	delete iImplementationSpecificInfo;

	iInputFormats.Reset();
	iInputFormats.Close();
	iOutputFormats.Reset();
	iOutputFormats.Close();
	iSupportedCombinations.Reset();
	iSupportedCombinations.Close();
	iSupportedScaleFactors.Reset();
	iSupportedScaleFactors.Close();
	}

EXPORT_C TUid CPreProcessorInfo::Uid() const
	{
	return iUid;
	}

EXPORT_C const TDesC& CPreProcessorInfo::Manufacturer() const
	{
	return *iManufacturer;
	}

EXPORT_C const TDesC& CPreProcessorInfo::Identifier() const
	{
	return *iIdentifier;
	}

EXPORT_C TVersion CPreProcessorInfo::Version() const
	{
	return iVersion;
	}

EXPORT_C TBool CPreProcessorInfo::Accelerated() const
	{
	return iAccelerated;
	}

EXPORT_C TBool CPreProcessorInfo::SupportsDirectCapture() const
	{
	return iSupportsDirectCapture;
	}

EXPORT_C TBool CPreProcessorInfo::SupportsInputFormat(const TUncompressedVideoFormat& aFormat) const
	{
	TBool found = EFalse;
	TInt count = iInputFormats.Count();
	for (TInt i=0; i<count; i++)
		{
		if (iInputFormats[i] == aFormat)
			{
			found = ETrue;
			break;
			}
		}
	return found;
	}

EXPORT_C const RArray<TUncompressedVideoFormat>& CPreProcessorInfo::SupportedInputFormats() const
	{
	return iInputFormats;
	}

EXPORT_C TBool CPreProcessorInfo::SupportsOutputFormat(const TUncompressedVideoFormat& aFormat) const
	{
	TBool found = EFalse;
	TInt count = iOutputFormats.Count();
	for (TInt i=0; i<count; i++)
		{
		if (iOutputFormats[i] == aFormat)
			{
			found = ETrue;
			break;
			}
		}
	return found;
	}

EXPORT_C const RArray<TUncompressedVideoFormat>& CPreProcessorInfo::SupportedOutputFormats() const
	{
	return iOutputFormats;
	}

EXPORT_C TBool CPreProcessorInfo::SupportsCombination(TUint32 aCombination) const
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

EXPORT_C const RArray<TUint32>& CPreProcessorInfo::SupportedCombinations() const
	{
	return iSupportedCombinations;
	}

EXPORT_C TBool CPreProcessorInfo::SupportsArbitraryScaling() const
	{
	return iSupportsArbitraryScaling;
	}

EXPORT_C TBool CPreProcessorInfo::SupportsAntiAliasedScaling() const
	{
	return iSupportsAntiAliasedScaling;
	}

EXPORT_C const RArray<TScaleFactor>& CPreProcessorInfo::SupportedScaleFactors() const
	{
	return iSupportedScaleFactors;
	}

EXPORT_C const TYuvToYuvCapabilities& CPreProcessorInfo::YuvToYuvCapabilities() const
	{
	return iYuvToYuvCapabilities;
	}

EXPORT_C TUint32 CPreProcessorInfo::SupportedRgbRanges() const
	{
	return iSupportedRgbRanges;
	}

EXPORT_C TUint32 CPreProcessorInfo::SupportedRotations() const
	{
	return iSupportedRotations;
	}

EXPORT_C const TDesC8& CPreProcessorInfo::ImplementationSpecificInfo() const
	{
	return *iImplementationSpecificInfo;
	}



//
// CMMFDevVideoRecord
//
EXPORT_C CMMFDevVideoRecord* CMMFDevVideoRecord::NewL(MMMFDevVideoRecordObserver& aObserver)
	{
	return new(ELeave) CMMFDevVideoRecord(aObserver);
	}

CMMFDevVideoRecord::CMMFDevVideoRecord(MMMFDevVideoRecordObserver& aObserver) :
	iObserver(aObserver),
	iInitializationState(ENotInitialized),
	iVideoOutputBufferQue(_FOFF(TVideoOutputBuffer,iLink)),
	iVideoOutputBufferQueIter(iVideoOutputBufferQue)
	{
	}

EXPORT_C CMMFDevVideoRecord::~CMMFDevVideoRecord()
	{
	delete iVideoEncodeHwDevice;
	delete iVideoPreProcHwDevice;
#ifdef SYMBIAN_MULTIMEDIA_CODEC_API
	iPuImplementations.ResetAndDestroy();
#endif
	}

EXPORT_C void CMMFDevVideoRecord::FindEncodersL(const TDesC8& aMimeType, TUint32 aPreProcType, RArray<TUid>& aEncoders, TBool aExactMatch)
	{
#ifdef SYMBIAN_MULTIMEDIA_CODEC_API	
		// find PU based plugins
	RImplInfoPtrArray codecPlugins;
	CleanupResetAndDestroyPushL(codecPlugins);
	DevVideoUtilities::FindVideoEncoderPluginsL(aMimeType, codecPlugins);	

	aEncoders.Reset();
	for (TInt i = 0; i < codecPlugins.Count();i++)
		{
		aEncoders.AppendL(codecPlugins[i]->ImplementationUid());
		}
		
	CleanupStack::PopAndDestroy(&codecPlugins);
#endif // SYMBIAN_MULTIMEDIA_CODEC_API

	RImplInfoPtrArray plugins;
	CleanupResetAndDestroyPushL(plugins);

	MmPluginUtils::FindImplementationsL(KUidDevVideoEncoderHwDevice, plugins);
	
	DevVideoUtilities::SelectPluginBasedOnMatchType(aMimeType, plugins);
	
	DevVideoUtilities::MatchPrePostProcessorCapabilitiesL(plugins, aPreProcType, aEncoders);

	// Perform the extra processing required if an exact match was requested
	if (aExactMatch)
		{
		// We now need to go through each of the plugins returned (which may have been matched using
		// a wildcard), instantiate each one, and perform an exact mime-type match.
		// Any plugins that don't support the desired mime type exactly will be removed from aEncoders.
		CCompressedVideoFormat* format = CCompressedVideoFormat::NewL(aMimeType);
		CleanupStack::PushL(format);

		for (TInt i=aEncoders.Count()-1; i>=0; i--)
			{
			TUid pluginUid = aEncoders[i];
			CVideoEncoderInfo* info = NULL;
			TRAPD(err, info = VideoEncoderInfoLC(pluginUid); CleanupStack::Pop(info););
			if (err == KErrNone)
				{
				if (!info->SupportsOutputFormat(*format))
					{
					// The encoder doesn't support the format, so remove it from the list
					aEncoders.Remove(i);
					}
				delete info;
				}
			else if (err==KErrNotFound)
				{
				// we have a bogus UID so remove it from the list
				aEncoders.Remove(i);
				}
			else
				{
				User::Leave(err);
				}
			}

		CleanupStack::PopAndDestroy(format);
		}

	// Leave if we didn't find anything.
	if (aEncoders.Count() == 0)
		{
		User::Leave(KErrNotFound);
		}

	CleanupStack::PopAndDestroy(&plugins);
	}

EXPORT_C void CMMFDevVideoRecord::FindPreProcessorsL(TUint32 aPreProcType, RArray<TUid>& aPreProcessors)
	{
	RImplInfoPtrArray plugins;
	CleanupResetAndDestroyPushL(plugins);

	MmPluginUtils::FindImplementationsL(KUidDevVideoPreProcessorHwDevice, plugins);

	DevVideoUtilities::MatchPrePostProcessorCapabilitiesL(plugins, aPreProcType, aPreProcessors);

	// Leave if we didn't find anything.
	if (aPreProcessors.Count() == 0)
		{
		User::Leave(KErrNotFound);
		}

	CleanupStack::PopAndDestroy(&plugins);
	}

EXPORT_C void CMMFDevVideoRecord::GetEncoderListL(RArray<TUid>& aEncoders)
	{
	RImplInfoPtrArray plugins;
	CleanupResetAndDestroyPushL(plugins);

	MmPluginUtils::FindImplementationsL(KUidDevVideoEncoderHwDevice, plugins);

	aEncoders.Reset();

	TInt count = plugins.Count();
	for (TInt i=0; i<count; i++)
		{
		User::LeaveIfError(aEncoders.Append(plugins[i]->ImplementationUid()));
		}

	CleanupStack::PopAndDestroy(&plugins);
	}

EXPORT_C void CMMFDevVideoRecord::GetPreProcessorListL(RArray<TUid>& aPreProcessors)
	{
	RImplInfoPtrArray plugins;
	CleanupResetAndDestroyPushL(plugins);

	MmPluginUtils::FindImplementationsL(KUidDevVideoPreProcessorHwDevice, plugins);

	aPreProcessors.Reset();

	TInt count = plugins.Count();
	for (TInt i=0; i<count; i++)
		{
		User::LeaveIfError(aPreProcessors.Append(plugins[i]->ImplementationUid()));
		}

	CleanupStack::PopAndDestroy(&plugins);
	}

EXPORT_C CVideoEncoderInfo* CMMFDevVideoRecord::VideoEncoderInfoLC(TUid aVideoEncoder)
	{
	CMMFVideoEncodeHwDevice* dev = CreateEncoderL(aVideoEncoder);
	CleanupStack::PushL(dev);
	CVideoEncoderInfo* info = dev->VideoEncoderInfoLC();
	CleanupStack::Pop(info);
	CleanupStack::PopAndDestroy(dev);
	CleanupStack::PushL(info);
	return info;
	}

EXPORT_C CPreProcessorInfo* CMMFDevVideoRecord::PreProcessorInfoLC(TUid aPreProcessor)
	{
	CMMFVideoPreProcHwDevice* dev = CMMFVideoPreProcHwDevice::NewL(aPreProcessor, *this);
	CleanupStack::PushL(dev);
	CPreProcessorInfo* info = dev->PreProcessorInfoLC();
	CleanupStack::Pop(info);
	CleanupStack::PopAndDestroy(dev);
	CleanupStack::PushL(info);
	return info;
	}

EXPORT_C THwDeviceId CMMFDevVideoRecord::SelectEncoderL(TUid aEncoder)
	{
	// This method can only be called before InitializeL() has been called.
	CheckInitializationState(ENotInitialized);

	delete iVideoEncodeHwDevice;
	iVideoEncodeHwDevice = NULL;	
	iVideoEncodeHwDevice = CreateEncoderL(aEncoder);

	return KHwDeviceIdVideoEncoder;
	}

EXPORT_C THwDeviceId CMMFDevVideoRecord::SelectPreProcessorL(TUid aPreProcessor)
	{
	// This method can only be called before InitializeL() has been called.
	CheckInitializationState(ENotInitialized);

	delete iVideoPreProcHwDevice;
	iVideoPreProcHwDevice = NULL;
	iVideoPreProcHwDevice = CMMFVideoPreProcHwDevice::NewL(aPreProcessor, *this);

	return KHwDeviceIdVideoPreProcessor;
	}

EXPORT_C void CMMFDevVideoRecord::SetInputFormatL(THwDeviceId aHwDevice, const TUncompressedVideoFormat& aFormat, const TSize& aPictureSize)
	{
	// This method can only be called before InitializeL() has been called.
	CheckInitializationState(ENotInitialized);
	VideoRecordHwDevice(aHwDevice).SetInputFormatL(aFormat, aPictureSize);
	}

EXPORT_C void CMMFDevVideoRecord::SetSourceCameraL(TInt aCameraHandle, TReal aPictureRate)
	{
	// This method can only be called before InitializeL() has been called.
	CheckInitializationState(ENotInitialized);
	CapturingHwDevice().SetSourceCameraL(aCameraHandle, aPictureRate);
	}

EXPORT_C void CMMFDevVideoRecord::SetSourceMemoryL(TReal aMaxPictureRate, TBool aConstantPictureRate, TBool aProcessRealtime)
	{
	// This method can only be called before InitializeL() has been called.
	CheckInitializationState(ENotInitialized);
	CapturingHwDevice().SetSourceMemoryL(aMaxPictureRate, aConstantPictureRate, aProcessRealtime);
	}

EXPORT_C void CMMFDevVideoRecord::SetOutputFormatL(THwDeviceId aHwDevice, const CCompressedVideoFormat& aFormat, TVideoDataUnitType aDataUnitType, TVideoDataUnitEncapsulation aDataEncapsulation, TBool aSegmentationAllowed)
	{
	// This method can only be called before InitializeL() has been called.
	CheckInitializationState(ENotInitialized);
	VideoEncodeHwDevice(aHwDevice).SetOutputFormatL(aFormat, aDataUnitType, aDataEncapsulation, aSegmentationAllowed);
	}

EXPORT_C void CMMFDevVideoRecord::SetOutputFormatL(THwDeviceId aHwDevice, const TUncompressedVideoFormat& aFormat)
	{
	// This method can only be called before InitializeL() has been called.
	CheckInitializationState(ENotInitialized);
	VideoPreProcHwDevice(aHwDevice).SetOutputFormatL(aFormat);
	}

EXPORT_C void CMMFDevVideoRecord::SetOutputRectL(THwDeviceId aHwDevice, const TRect& aRect)
	{
	CheckInitializationState(ENotInitialized|EInitialized);
	VideoEncodeHwDevice(aHwDevice).SetOutputRectL(aRect);
	}

EXPORT_C void CMMFDevVideoRecord::SetClockSource(MMMFClockSource* aClock)
	{
	CheckInitializationState(ENotInitialized);

	ASSERT(aClock);

	// Make sure we have at least one of the hwdevices set up
	__ASSERT_ALWAYS((iVideoEncodeHwDevice||iVideoPreProcHwDevice), DevVideoPanic(EDevVideoPanicPreConditionViolation));

	// Set the clock source on both hw devices.
	if (iVideoEncodeHwDevice)
		iVideoEncodeHwDevice->SetClockSource(aClock);
	if (iVideoPreProcHwDevice)
		iVideoPreProcHwDevice->SetClockSource(aClock);
	}

EXPORT_C void CMMFDevVideoRecord::SetPreProcessTypesL(THwDeviceId aHwDevice, TUint32 aPreProcessTypes)
	{
	CheckInitializationState(ENotInitialized|EInitialized);
	VideoRecordHwDevice(aHwDevice).SetPreProcessTypesL(aPreProcessTypes);
	}

EXPORT_C void CMMFDevVideoRecord::SetRgbToYuvOptionsL(THwDeviceId aHwDevice, TRgbRange aRgbRange, const TYuvFormat& aOutputFormat)
	{
	CheckInitializationState(ENotInitialized);
	VideoRecordHwDevice(aHwDevice).SetRgbToYuvOptionsL(aRgbRange, aOutputFormat);
	}

EXPORT_C void CMMFDevVideoRecord::SetYuvToYuvOptionsL(THwDeviceId aHwDevice, const TYuvFormat& aInputFormat, const TYuvFormat& aOutputFormat)
	{
	CheckInitializationState(ENotInitialized);
	VideoRecordHwDevice(aHwDevice).SetYuvToYuvOptionsL(aInputFormat, aOutputFormat);
	}

EXPORT_C void CMMFDevVideoRecord::SetRotateOptionsL(THwDeviceId aHwDevice, TRotationType aRotationType)
	{
	CheckInitializationState(ENotInitialized|EInitialized);
	VideoRecordHwDevice(aHwDevice).SetRotateOptionsL(aRotationType);
	}

EXPORT_C void CMMFDevVideoRecord::SetScaleOptionsL(THwDeviceId aHwDevice, const TSize& aTargetSize, TBool aAntiAliasFiltering)
	{
	CheckInitializationState(ENotInitialized|EInitialized);
	VideoRecordHwDevice(aHwDevice).SetScaleOptionsL(aTargetSize, aAntiAliasFiltering);
	}

EXPORT_C void CMMFDevVideoRecord::SetInputCropOptionsL(THwDeviceId aHwDevice, const TRect& aRect)
	{
	CheckInitializationState(ENotInitialized|EInitialized);
	VideoRecordHwDevice(aHwDevice).SetInputCropOptionsL(aRect);
	}

EXPORT_C void CMMFDevVideoRecord::SetOutputCropOptionsL(THwDeviceId aHwDevice, const TRect& aRect)
	{
	CheckInitializationState(ENotInitialized|EInitialized);
	VideoRecordHwDevice(aHwDevice).SetOutputCropOptionsL(aRect);
	}

EXPORT_C void CMMFDevVideoRecord::SetOutputPadOptionsL(THwDeviceId aHwDevice, const TSize& aOutputSize, const TPoint& aPicturePos)
	{
	CheckInitializationState(ENotInitialized|EInitialized);
	VideoRecordHwDevice(aHwDevice).SetOutputPadOptionsL(aOutputSize, aPicturePos);
	}

EXPORT_C void CMMFDevVideoRecord::SetColorEnhancementOptionsL(THwDeviceId aHwDevice, const TColorEnhancementOptions& aOptions)
	{
	CheckInitializationState(ENotInitialized|EInitialized);
	VideoRecordHwDevice(aHwDevice).SetColorEnhancementOptionsL(aOptions);
	}

EXPORT_C void CMMFDevVideoRecord::SetFrameStabilisationOptionsL(THwDeviceId aHwDevice, const TSize& aOutputSize, TBool aFrameStabilisation)
	{
	CheckInitializationState(ENotInitialized|EInitialized);
	VideoRecordHwDevice(aHwDevice).SetFrameStabilisationOptionsL(aOutputSize, aFrameStabilisation);
	}

EXPORT_C void CMMFDevVideoRecord::SetCustomPreProcessOptionsL(THwDeviceId aHwDevice, const TDesC8& aOptions)
	{
	CheckInitializationState(ENotInitialized|EInitialized);
	VideoRecordHwDevice(aHwDevice).SetCustomPreProcessOptionsL(aOptions);
	}

EXPORT_C void CMMFDevVideoRecord::SetErrorsExpected(TBool aBitErrors, TBool aPacketLosses)
	{
	CheckInitializationState(ENotInitialized|EInitialized);
	VideoEncodeHwDevice().SetErrorsExpected(aBitErrors, aPacketLosses);
	}

EXPORT_C void CMMFDevVideoRecord::SetMinRandomAccessRate(TReal aRate)
	{
	CheckInitializationState(ENotInitialized|EInitialized);
	VideoEncodeHwDevice().SetMinRandomAccessRate(aRate);
	}

EXPORT_C void CMMFDevVideoRecord::SetNumBitrateLayersL(TUint aNumLayers)
	{
	CheckInitializationState(ENotInitialized);
	VideoEncodeHwDevice().SetNumBitrateLayersL(aNumLayers);
	}

EXPORT_C void CMMFDevVideoRecord::SetScalabilityLayerTypeL(TUint aLayer, TScalabilityType aScalabilityType)
	{
	CheckInitializationState(ENotInitialized);
	VideoEncodeHwDevice().SetScalabilityLayerTypeL(aLayer, aScalabilityType);
	}

EXPORT_C void CMMFDevVideoRecord::SetGlobalReferenceOptions(TUint aMaxReferencePictures, TUint aMaxPictureOrderDelay)
	{
	CheckInitializationState(ENotInitialized);
	VideoEncodeHwDevice().SetGlobalReferenceOptions(aMaxReferencePictures, aMaxPictureOrderDelay);
	}

EXPORT_C void CMMFDevVideoRecord::SetLayerReferenceOptions(TUint aLayer, TUint aMaxReferencePictures, TUint aMaxPictureOrderDelay)
	{
	CheckInitializationState(ENotInitialized);
	VideoEncodeHwDevice().SetLayerReferenceOptions(aLayer, aMaxReferencePictures, aMaxPictureOrderDelay);
	}

EXPORT_C void CMMFDevVideoRecord::SetBufferOptionsL(const TEncoderBufferOptions& aOptions)
	{
	CheckInitializationState(ENotInitialized);
	VideoEncodeHwDevice().SetBufferOptionsL(aOptions);
	}

EXPORT_C void CMMFDevVideoRecord::SetCodingStandardSpecificOptionsL(const TDesC8& aOptions)
	{
	CheckInitializationState(ENotInitialized|EInitialized);
	VideoEncodeHwDevice().SetCodingStandardSpecificOptionsL(aOptions);
	}

EXPORT_C void CMMFDevVideoRecord::SetImplementationSpecificEncoderOptionsL(const TDesC8& aOptions)
	{
	CheckInitializationState(ENotInitialized|EInitialized);
	VideoEncodeHwDevice().SetImplementationSpecificEncoderOptionsL(aOptions);
	}

EXPORT_C void CMMFDevVideoRecord::Initialize()
	{
	CheckInitializationState(ENotInitialized);

	// First introduce the two plugins to each other
	ConnectPlugins();

	// Next initialize the first hwdevice in the chain
	if (iVideoPreProcHwDevice)
		{
		iInitializationState = EInitializing;
		iVideoPreProcHwDevice->Initialize();
		}
	else if (iVideoEncodeHwDevice)
		{
		iInitializationState = EInitializing;
		iVideoEncodeHwDevice->Initialize();
		}
	else
		{
		// No hwdevice to initialize so panic
		DevVideoPanic(EDevVideoPanicPreConditionViolation);
		}
	}

EXPORT_C HBufC8* CMMFDevVideoRecord::CodingStandardSpecificInitOutputLC()
	{
	CheckInitializationState(EInitialized);
	return VideoEncodeHwDevice().CodingStandardSpecificInitOutputLC();
	}

EXPORT_C HBufC8* CMMFDevVideoRecord::ImplementationSpecificInitOutputLC()
	{
	CheckInitializationState(EInitialized);
	return VideoEncodeHwDevice().ImplementationSpecificInitOutputLC();
	}

EXPORT_C void CMMFDevVideoRecord::SetErrorProtectionLevelsL(TUint aNumLevels, TBool aSeparateBuffers)
	{
	CheckInitializationState(EInitialized);
	VideoEncodeHwDevice().SetErrorProtectionLevelsL(aNumLevels, aSeparateBuffers);
	}

EXPORT_C void CMMFDevVideoRecord::SetErrorProtectionLevelL(TUint aLevel, TUint aBitrate, TUint aStrength)
	{
	CheckInitializationState(EInitialized);
	VideoEncodeHwDevice().SetErrorProtectionLevelL(aLevel, aBitrate, aStrength);
	}

EXPORT_C void CMMFDevVideoRecord::SetChannelPacketLossRate(TUint aLevel, TReal aLossRate, TTimeIntervalMicroSeconds32 aLossBurstLength)
	{
	CheckInitializationState(EInitialized);
	VideoEncodeHwDevice().SetChannelPacketLossRate(aLevel, aLossRate, aLossBurstLength);
	}

EXPORT_C void CMMFDevVideoRecord::SetChannelBitErrorRate(TUint aLevel, TReal aErrorRate, TReal aStdDeviation)
	{
	CheckInitializationState(EInitialized);
	VideoEncodeHwDevice().SetChannelBitErrorRate(aLevel, aErrorRate, aStdDeviation);
	}

EXPORT_C void CMMFDevVideoRecord::SetSegmentTargetSize(TUint aLayer, TUint aSizeBytes, TUint aSizeMacroblocks)
	{
	CheckInitializationState(EInitialized);
	VideoEncodeHwDevice().SetSegmentTargetSize(aLayer, aSizeBytes, aSizeMacroblocks);
	}

EXPORT_C void CMMFDevVideoRecord::SetRateControlOptions(TUint aLayer, const TRateControlOptions& aOptions)
	{
	CheckInitializationState(EInitialized);
	VideoEncodeHwDevice().SetRateControlOptions(aLayer, aOptions);
	}

EXPORT_C void CMMFDevVideoRecord::SetInLayerScalabilityL(TUint aLayer, TUint aNumSteps, TInLayerScalabilityType aScalabilityType, const TArray<TUint>& aBitrateShare, const TArray<TUint>& aPictureShare)
	{
	CheckInitializationState(EInitialized);
	VideoEncodeHwDevice().SetInLayerScalabilityL(aLayer, aNumSteps, aScalabilityType, aBitrateShare, aPictureShare);
	}

EXPORT_C void CMMFDevVideoRecord::SetLayerPromotionPointPeriod(TUint aLayer, TUint aPeriod)
	{
	CheckInitializationState(EInitialized);
	VideoEncodeHwDevice().SetLayerPromotionPointPeriod(aLayer, aPeriod);
	}

EXPORT_C HBufC8* CMMFDevVideoRecord::CodingStandardSpecificSettingsOutputLC()
	{
	CheckInitializationState(EInitialized);
	return VideoEncodeHwDevice().CodingStandardSpecificSettingsOutputLC();
	}

EXPORT_C HBufC8* CMMFDevVideoRecord::ImplementationSpecificSettingsOutputLC()
	{
	CheckInitializationState(EInitialized);
	return VideoEncodeHwDevice().ImplementationSpecificSettingsOutputLC();
	}

EXPORT_C void CMMFDevVideoRecord::WritePictureL(TVideoPicture* aPicture)
	{
	CheckInitializationState(EInitialized);
	CapturingHwDevice().WritePictureL(aPicture);
	}

EXPORT_C void CMMFDevVideoRecord::SendSupplementalInfoL(const TDesC8& aData)
	{
	CheckInitializationState(EInitialized);
	VideoEncodeHwDevice().SendSupplementalInfoL(aData);
	}

EXPORT_C void CMMFDevVideoRecord::SendSupplementalInfoL(const TDesC8& aData, const TTimeIntervalMicroSeconds& aTimestamp)
	{
	CheckInitializationState(EInitialized);
	VideoEncodeHwDevice().SendSupplementalInfoL(aData, aTimestamp);
	}

EXPORT_C void CMMFDevVideoRecord::CancelSupplementalInfo()
	{
	CheckInitializationState(EInitialized);
	VideoEncodeHwDevice().CancelSupplementalInfo();
	}

EXPORT_C void CMMFDevVideoRecord::InputEnd()
	{
	CheckInitializationState(EInitialized);
	
	//If pre-processor and encoder exist,InputEnd() of pre-processor is issued
	//and waited for reply from it before issuing Encoder InputEnd()
	//iIsPreProcComplete is EFalse, until pre-processor responds
	iIsPreProcComplete=EFalse;
	
	if (iVideoPreProcHwDevice)
		{
		iVideoPreProcHwDevice->InputEnd();
		return;
		}
	
	if (iVideoEncodeHwDevice)
		{
		iVideoEncodeHwDevice->InputEnd();
		}
	}

EXPORT_C void CMMFDevVideoRecord::Start()
	{
	CheckInitializationState(EInitialized);

	if (iVideoEncodeHwDevice)
		iVideoEncodeHwDevice->Start();
	if (iVideoPreProcHwDevice)
		iVideoPreProcHwDevice->Start();
	}

EXPORT_C void CMMFDevVideoRecord::Stop()
	{
	CheckInitializationState(EInitialized);

	if (iVideoEncodeHwDevice)
		iVideoEncodeHwDevice->Stop();
	if (iVideoPreProcHwDevice)
		iVideoPreProcHwDevice->Stop();
	}

EXPORT_C void CMMFDevVideoRecord::Pause()
	{
	CheckInitializationState(EInitialized);

	if (iVideoEncodeHwDevice)
		iVideoEncodeHwDevice->Pause();
	if (iVideoPreProcHwDevice)
		iVideoPreProcHwDevice->Pause();
	}

EXPORT_C void CMMFDevVideoRecord::Resume()
	{
	CheckInitializationState(EInitialized);

	if (iVideoEncodeHwDevice)
		iVideoEncodeHwDevice->Resume();
	if (iVideoPreProcHwDevice)
		iVideoPreProcHwDevice->Resume();
	}

EXPORT_C void CMMFDevVideoRecord::Freeze()
	{
	CheckInitializationState(EInitialized);

	if (iVideoEncodeHwDevice)
		iVideoEncodeHwDevice->Freeze();
	if (iVideoPreProcHwDevice)
		iVideoPreProcHwDevice->Freeze();
	}

EXPORT_C void CMMFDevVideoRecord::ReleaseFreeze()
	{
	CheckInitializationState(EInitialized);

	if (iVideoEncodeHwDevice)
		iVideoEncodeHwDevice->ReleaseFreeze();
	if (iVideoPreProcHwDevice)
		iVideoPreProcHwDevice->ReleaseFreeze();
	}

EXPORT_C TTimeIntervalMicroSeconds CMMFDevVideoRecord::RecordingPosition()
	{
	CheckInitializationState(EInitialized);
	return CapturingHwDevice().RecordingPosition();
	}

EXPORT_C void CMMFDevVideoRecord::GetOutputBufferStatus(TUint& aNumFreeBuffers, TUint& aTotalFreeBytes)
	{
	CheckInitializationState(EInitialized);
	VideoEncodeHwDevice().GetOutputBufferStatus(aNumFreeBuffers, aTotalFreeBytes);
	}

EXPORT_C void CMMFDevVideoRecord::GetPictureCounters(TPictureCounters& aCounters)
	{
	CheckInitializationState(EInitialized);

	TPictureCounters encoderPictureCounters;
	TPictureCounters preProcPictureCounters;

	if (iVideoEncodeHwDevice)
		iVideoEncodeHwDevice->GetPictureCounters(encoderPictureCounters);

	if (iVideoPreProcHwDevice)
		iVideoPreProcHwDevice->GetPictureCounters(preProcPictureCounters);

	// Total pictures skipped is sum of enc + preproc
	aCounters.iPicturesSkippedBufferOverflow = encoderPictureCounters.iPicturesSkippedBufferOverflow + 
											   preProcPictureCounters.iPicturesSkippedBufferOverflow;

	aCounters.iPicturesSkippedProcPower = encoderPictureCounters.iPicturesSkippedProcPower + 
										  preProcPictureCounters.iPicturesSkippedProcPower;

	aCounters.iPicturesSkippedRateControl = encoderPictureCounters.iPicturesSkippedRateControl + 
											preProcPictureCounters.iPicturesSkippedRateControl;

	// Get the number of pictures processed from the encoder(since it is last in the chain)
	aCounters.iPicturesProcessed = encoderPictureCounters.iPicturesProcessed;

	// Get the number of input pictures from the first hwdevice in the chain
	if (iVideoPreProcHwDevice)
		aCounters.iInputPictures = preProcPictureCounters.iInputPictures;
	else
		aCounters.iInputPictures = encoderPictureCounters.iInputPictures;
	}

EXPORT_C void CMMFDevVideoRecord::GetFrameStabilisationOutput(THwDeviceId aHwDevice, TRect& aRect)
	{
	CheckInitializationState(EInitialized);
	VideoRecordHwDevice(aHwDevice).GetFrameStabilisationOutput(aRect);
	}

EXPORT_C TUint CMMFDevVideoRecord::NumDataBuffers()
	{
	CheckInitializationState(EInitialized);
	return iNumberOfVideoOutputBuffers;
	}

EXPORT_C TVideoOutputBuffer* CMMFDevVideoRecord::NextBufferL()
	{
	CheckInitializationState(EInitialized);

	if (iNumberOfVideoOutputBuffers == 0)
		return NULL;

	TVideoOutputBuffer* buffer = iVideoOutputBufferQue.First();

	// Remove the picture from the list
	buffer->iLink.Deque();
	iNumberOfVideoOutputBuffers--;

	return buffer;
	}

EXPORT_C void CMMFDevVideoRecord::ReturnBuffer(TVideoOutputBuffer* aBuffer)
	{
	CheckInitializationState(EInitialized);

	// Return the picture to the hwdevice
	VideoEncodeHwDevice().ReturnBuffer(aBuffer);
	}

EXPORT_C void CMMFDevVideoRecord::PictureLoss()
	{
	CheckInitializationState(EInitialized);
	VideoEncodeHwDevice().PictureLoss();
	}

EXPORT_C void CMMFDevVideoRecord::PictureLoss(const TArray<TPictureId>& aPictures)
	{
	CheckInitializationState(EInitialized);
	VideoEncodeHwDevice().PictureLoss(aPictures);
	}

EXPORT_C void CMMFDevVideoRecord::SliceLoss(TUint aFirstMacroblock, TUint aNumMacroblocks, const TPictureId& aPicture)
	{
	CheckInitializationState(EInitialized);
	VideoEncodeHwDevice().SliceLoss(aFirstMacroblock, aNumMacroblocks, aPicture);
	}

EXPORT_C void CMMFDevVideoRecord::ReferencePictureSelection(const TDesC8& aSelectionData)
	{
	CheckInitializationState(EInitialized);
	VideoEncodeHwDevice().ReferencePictureSelection(aSelectionData);
	}

EXPORT_C TUint CMMFDevVideoRecord::NumComplexityLevels(THwDeviceId aHwDevice)
	{
	CheckInitializationState(EInitialized);
	return VideoRecordHwDevice(aHwDevice).NumComplexityLevels();
	}

EXPORT_C void CMMFDevVideoRecord::SetComplexityLevel(THwDeviceId aHwDevice, TUint aLevel) 
	{
	CheckInitializationState(EInitialized);
	VideoRecordHwDevice(aHwDevice).SetComplexityLevel(aLevel);
	}

EXPORT_C void CMMFDevVideoRecord::CommitL()
	{
	CheckInitializationState(EInitialized);
	if (iVideoEncodeHwDevice)
		iVideoEncodeHwDevice->CommitL();
	if (iVideoPreProcHwDevice)
		iVideoPreProcHwDevice->CommitL();
	}

EXPORT_C void CMMFDevVideoRecord::Revert()
	{
	CheckInitializationState(EInitialized);
	if (iVideoEncodeHwDevice)
		iVideoEncodeHwDevice->Revert();
	if (iVideoPreProcHwDevice)
		iVideoPreProcHwDevice->Revert();
	}

EXPORT_C TAny* CMMFDevVideoRecord::CustomInterface(THwDeviceId aHwDevice, TUid aInterface)
	{
	CheckInitializationState(ENotInitialized|EInitialized);
	return VideoHwDevice(aHwDevice).CustomInterface(aInterface);
	}


CMMFVideoRecordHwDevice& CMMFDevVideoRecord::VideoRecordHwDevice(THwDeviceId aHwDevice) const
	{
	if (aHwDevice == KHwDeviceIdVideoEncoder)
		{
		__ASSERT_ALWAYS(iVideoEncodeHwDevice, DevVideoPanic(EDevVideoPanicPreConditionViolation));
		return *iVideoEncodeHwDevice;
		}
	else if (aHwDevice == KHwDeviceIdVideoPreProcessor)
		{
		__ASSERT_ALWAYS(iVideoPreProcHwDevice, DevVideoPanic(EDevVideoPanicPreConditionViolation));
		return *iVideoPreProcHwDevice;
		}
	else
		{
		DevVideoPanic(EDevVideoPanicInvalidHwDeviceId);
		// The following is unreachable
		CMMFVideoRecordHwDevice* ret = NULL;
		return *ret;
		}
	}

CMMFVideoRecordHwDevice& CMMFDevVideoRecord::CapturingHwDevice() const
	{
	// Return the first plugin in the chain
	//  - the preprocessor if available, otherwise the videoencoder
	if (iVideoPreProcHwDevice)
		return *iVideoPreProcHwDevice;
	else if (iVideoEncodeHwDevice)
		return *iVideoEncodeHwDevice;
	else
		{
		DevVideoPanic(EDevVideoPanicPreConditionViolation);
		// The following is unreachable
		CMMFVideoRecordHwDevice* ret = NULL;
		return *ret;
		}
	}

CMMFVideoEncodeHwDevice& CMMFDevVideoRecord::VideoEncodeHwDevice(THwDeviceId aHwDevice) const
	{
	if (aHwDevice == KHwDeviceIdVideoEncoder)
		{
		__ASSERT_ALWAYS(iVideoEncodeHwDevice, DevVideoPanic(EDevVideoPanicPreConditionViolation));
		return *iVideoEncodeHwDevice;
		}
	else
		{
		DevVideoPanic(EDevVideoPanicInvalidHwDeviceId);
		// The following is unreachable
		CMMFVideoEncodeHwDevice* ret = NULL;
		return *ret;
		}
	}

CMMFVideoPreProcHwDevice& CMMFDevVideoRecord::VideoPreProcHwDevice(THwDeviceId aHwDevice) const
	{
	if (aHwDevice == KHwDeviceIdVideoPreProcessor)
		{
		__ASSERT_ALWAYS(iVideoPreProcHwDevice, DevVideoPanic(EDevVideoPanicPreConditionViolation));
		return *iVideoPreProcHwDevice;
		}
	else
		{
		DevVideoPanic(EDevVideoPanicInvalidHwDeviceId);
		// The following is unreachable
		CMMFVideoPreProcHwDevice* ret = NULL;
		return *ret;
		}
	}

CMMFVideoEncodeHwDevice& CMMFDevVideoRecord::VideoEncodeHwDevice() const
	{
	return VideoEncodeHwDevice(KHwDeviceIdVideoEncoder);
	}

CMMFVideoPreProcHwDevice& CMMFDevVideoRecord::VideoPreProcHwDevice() const
	{
	return VideoPreProcHwDevice(KHwDeviceIdVideoPreProcessor);
	}

CMMFVideoHwDevice& CMMFDevVideoRecord::VideoHwDevice(THwDeviceId aHwDevice) const
	{
	if (aHwDevice == KHwDeviceIdVideoPreProcessor)
		{
		__ASSERT_ALWAYS(iVideoPreProcHwDevice, DevVideoPanic(EDevVideoPanicPreConditionViolation));
		return *iVideoPreProcHwDevice;
		}
	else if (aHwDevice == KHwDeviceIdVideoEncoder)
		{
		__ASSERT_ALWAYS(iVideoEncodeHwDevice, DevVideoPanic(EDevVideoPanicPreConditionViolation));
		return *iVideoEncodeHwDevice;
		}
	else
		{
		DevVideoPanic(EDevVideoPanicInvalidHwDeviceId);
		// The following is unreachable
		CMMFVideoHwDevice* ret = NULL;
		return *ret;
		}
	}


void CMMFDevVideoRecord::ConnectPlugins()
	{
	__ASSERT_ALWAYS((iVideoEncodeHwDevice||iVideoPreProcHwDevice), DevVideoPanic(EDevVideoPanicPreConditionViolation));
	if (iVideoEncodeHwDevice && iVideoPreProcHwDevice)
		{
		iVideoEncodeHwDevice->SetInputDevice(iVideoPreProcHwDevice);
		iVideoPreProcHwDevice->SetOutputDevice(iVideoEncodeHwDevice);
		}
	}

void CMMFDevVideoRecord::CheckInitializationState(TUint aExpected)
	{
	__ASSERT_ALWAYS(iInitializationState&aExpected, DevVideoPanic(EDevVideoPanicPreConditionViolation));
	}

void CMMFDevVideoRecord::HandlePreProcInitializeComplete(TInt aError)
	{
	// Make sure we are in the correct state
	__ASSERT_ALWAYS(iInitializationState==EInitializing, DevVideoPanic(EDevVideoPanicPreConditionViolation));

	if (aError == KErrNone)
		{
		// If we have a encoder, initialize that.  Otherwise, we're initialized.
		if (iVideoEncodeHwDevice)
			{
			iVideoEncodeHwDevice->Initialize();
			// Now wait for the next callback
			}
		else
			{
			iInitializationState = EInitialized;
			iObserver.MdvroInitializeComplete(KErrNone);
			}
		}
	else
		{//aError
		iInitializationState = EInitializationFailed;
		iObserver.MdvroInitializeComplete(aError);
		}
	}

void CMMFDevVideoRecord::HandleEncoderInitializeComplete(TInt aError)
	{
	// Make sure we are in the correct state
	__ASSERT_ALWAYS(iInitializationState==EInitializing, DevVideoPanic(EDevVideoPanicPreConditionViolation));

	if (aError == KErrNone)
		{
		// We're now fully initialized
		iInitializationState = EInitialized;
		iObserver.MdvroInitializeComplete(KErrNone);
		}
	else
		{//aError
		iInitializationState = EInitializationFailed;
		iObserver.MdvroInitializeComplete(aError);
		}
	}

void CMMFDevVideoRecord::MdvrpNewBuffer(TVideoOutputBuffer* aBuffer)
	{
	iVideoOutputBufferQue.AddLast(*aBuffer);
	iNumberOfVideoOutputBuffers++;

	iObserver.MdvroNewBuffers();
	}

void CMMFDevVideoRecord::MdvrpReturnPicture(TVideoPicture* aPicture)
	{
	iObserver.MdvroReturnPicture(aPicture);
	}

void CMMFDevVideoRecord::MdvrpSupplementalInfoSent()
	{
	iObserver.MdvroSupplementalInfoSent();
	}

void CMMFDevVideoRecord::MdvrpFatalError(CMMFVideoHwDevice* aDevice, TInt aError)
	{
	// Make sure the other plugin gets stopped
	__ASSERT_DEBUG((aDevice!=NULL) && ((aDevice==iVideoEncodeHwDevice) || (aDevice==iVideoPreProcHwDevice)), DevVideoPanic(EDevVideoPanicPreConditionViolation));
	if ((aDevice == iVideoEncodeHwDevice) && (iVideoPreProcHwDevice))
		iVideoPreProcHwDevice->Stop();
	else if ((aDevice == iVideoPreProcHwDevice) && (iVideoEncodeHwDevice))
		iVideoEncodeHwDevice->Stop();

	// Place the object in the "initialization failed" state to prevent any more method calls
	// This state is purely internal so this is ok.
	iInitializationState = EInitializationFailed;

	iObserver.MdvroFatalError(aError);
	}

void CMMFDevVideoRecord::MdvrpInitializeComplete(CMMFVideoHwDevice* aDevice, TInt aError)
	{
	if (aDevice == iVideoEncodeHwDevice)
		{
		HandleEncoderInitializeComplete(aError);
		}
	else if (aDevice == iVideoPreProcHwDevice)
		{
		HandlePreProcInitializeComplete(aError);
		}
	else
		{// We've been given a random pointer back!
		DevVideoPanic(EDevVideoPanicPreConditionViolation);
		}
	}

void CMMFDevVideoRecord::MdvrpStreamEnd()
	{
	// This method must be called the same number of times as the number of plugins we have
	// before we call iObserver.MdvpoStreamEnd().
	iNumberOfMdvrpStreamEndCallbacks++;
	
	TUint numberOfPlugins = 0;
	if (iVideoPreProcHwDevice && !iIsPreProcComplete)
		{
		iIsPreProcComplete=ETrue;
		if(iVideoEncodeHwDevice)
			{
			iVideoEncodeHwDevice->InputEnd();
			return;
			}
		}

	ASSERT(!iVideoPreProcHwDevice||iIsPreProcComplete);
	if(iVideoPreProcHwDevice)
		{
		numberOfPlugins++;
		}
	
	if (iVideoEncodeHwDevice)
		{
		numberOfPlugins++;
		}
	
	if (iNumberOfMdvrpStreamEndCallbacks == numberOfPlugins)
		{
		// We've received the correct number of callbacks so signal the client
		iNumberOfMdvrpStreamEndCallbacks = 0;
		iObserver.MdvroStreamEnd();
		}
	// else wait for the next plugin to call back
	}


CMMFVideoEncodeHwDevice* CMMFDevVideoRecord::CreateEncoderL(TUid aEncoder)
	{
#ifdef SYMBIAN_MULTIMEDIA_CODEC_API
	// need to check whether we've  been given a PU
	if (!iPuListCreated)
		{
		DevVideoUtilities::CreatePuListL(iPuImplementations);
		iPuListCreated = ETrue;
		}
	const CImplementationInformation* info = DevVideoUtilities::FindPu(iPuImplementations, aEncoder);
		
	if (!info)
		{
		return CMMFVideoEncodeHwDevice::NewL(aEncoder, *this);
		}
	else
		{
		return CMMFVideoEncodeHwDevice::NewPuAdapterL(*info, *this);
		}
#else
	return CMMFVideoEncodeHwDevice::NewL(aEncoder, *this);
#endif // SYMBIAN_MULTIMEDIA_CODEC_API
	}


// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "tsu_mdf_audiocodecs_resolver.h"
#include <openmax/il/khronos/v1_x/OMX_Core.h>
#include <mmf/server/mmfhwdevicesetup.h>
#include <mdf/codecapiresolverdata.h>
#include <mdf/codecapiuids.hrh>
#include <mmf/server/mmfhwdevicesetup.h>
#include <mmf/common/mmffourcc.h>
#include <mdf/codecapiresolverutils.h>
#include <mdf/codecapivideoresolverutils.h>
#include <mdf/codecapiresolverutils.h>
#include <mmf/devvideo/devvideoplay.h>
#include <mm/mmcleanup.h>

const TInt  KPicSizeX  = 320;
const TInt  KPicSizeY  = 200;

const TReal KPicRate  = 100;
const TInt  KPicWidth = 20;
const TInt  KPicHeight = 4000;
_LIT8(KTextManufactor, "Manufacturer");
	
// *********************************************************
// Loads a PU using the FourCC and using the Custom Resolver 
// *********************************************************

RTestStepAudioCodecLoadFourCC::RTestStepAudioCodecLoadFourCC()
	{
	iTestStepName = _L("MM-MDF-AUDIOCODECS-0004");	
	}

TVerdict RTestStepAudioCodecLoadFourCC::DoTestStepL()
	{
	__UHEAP_MARK;
	TFourCC aSrcDataType, aDestDataType;
	INFO_PRINTF1(_L("Test loading two PU using 2 FourCCs"));
	// create the OMX Core
	CCodecApiResolverData* customMatchData = CCodecApiResolverData::NewLC();
	customMatchData->SetMatchType(EMatchInputAndOutputDataFormat);
	customMatchData->SetImplementationType(TUid::Uid(0x1027379e));
	// string value of the input source data	
	HBufC8* packageData = HBufC8::NewLC(KMaxDataTypeLength);
	TPtr8 sourceData = packageData->Des();
	TFourCC srcDataType = KMMFFourCCCodeTestPCM8;
	srcDataType.FourCC(&sourceData);
	customMatchData->SetInputDataL(sourceData);
	
	TFourCC destDataType = KMMFFourCCCodePCM16;
	destDataType.FourCC(&sourceData);
	customMatchData->SetOutputDataL(sourceData);
	
 	HBufC8* package  = customMatchData->NewPackLC();
	TPtr8 packageDes = package->Des();
	TEComResolverParams resolverParams; // Parameters on which to match
	resolverParams.SetDataType(packageDes);
	
	RImplInfoPtrArray ecomArray;
	CleanupResetAndDestroyPushL(ecomArray);

	TRAPD(error, REComSession::ListImplementationsL(TUid::Uid(KUidMdfProcessingUnit), 
		  resolverParams, TUid::Uid(0x1027379b), ecomArray));
	
	if (error != KErrNone)
		{
		CleanupStack::PopAndDestroy(3); // customMatchData, packageData, package	
		return EFail;
		}

	// verify if the resolver found any PUs 
	TInt noOfProcessingUnits = ecomArray.Count();
	if(noOfProcessingUnits == 0)
		{
		INFO_PRINTF1(_L("PU Resolver did not find any PU with these data"));
		return EFail;
		}
	
	// verify if one of the PUs found is what we are looking for
	for(TInt index = 0; index < noOfProcessingUnits; index++)
		{
		TUid uid = ecomArray[index]->ImplementationUid();
		CCodecApiOpaqueData* parse = NULL;
		const TDesC8* opaqueData = &(ecomArray[index]->OpaqueData());
		// if opaque data is empty, move to the next element in the list
		if (opaqueData->Length() == 0)
			{
			INFO_PRINTF1(_L("PU Resolver returned a PU with empty opaque data"));
			return EFail;
			}
		
		TRAPD(error, parse = CCodecApiOpaqueData::NewL(*opaqueData));
		if (error!=KErrNone)
			{
			if (error==KErrNotSupported)
				{
				// means that the resource entry was not valid
				INFO_PRINTF1(_L("PU Resolver returned a PU with invalid opaque data"));
				return EFail;
				}				
			}	
		CleanupStack::PushL(parse);	
		HBufC8* inputDataFormat 		= customMatchData->InputDataFormat().AllocLC();
		HBufC8* outputDataFormat		= customMatchData->OutputDataFormat().AllocLC();
		if(!parse->CompareInputDataType(*inputDataFormat) 
			&& !parse->CompareOutputDataType(*outputDataFormat))
			{	
			INFO_PRINTF1(_L("PU Resolver returned a PU with a different input and output data type"));
			return EFail;			
			}
		CleanupStack::PopAndDestroy(3);		
		}
		
	CleanupStack::PopAndDestroy(4); // customMatchData, packageData, package, ecomArray		  
	REComSession::FinalClose();
	__UHEAP_MARKEND;

	INFO_PRINTF1(_L("PU Resolver returned the correct PU"));
	return EPass;
	}
	
// ***************************************************************
// Finds a PU using the FourCC and Custom Resolver with P8 src 
// ***************************************************************

RTestStepAudioCodecFindP8InputFourCC::RTestStepAudioCodecFindP8InputFourCC()
	{
	iTestStepName = _L("MM-MDF-AUDIOCODECS-0005");	
	}

TVerdict RTestStepAudioCodecFindP8InputFourCC::DoTestStepL()
	{
	__UHEAP_MARK;
	TFourCC aSrcDataType;
	INFO_PRINTF1(_L("Test locates the PU using just the source data type (P8) and the version"));
	// create the OMX Core
	CCodecApiResolverData* customMatchData = CCodecApiResolverData::NewLC();
	customMatchData->SetMatchType(EMatchInputDataFormat);
	customMatchData->SetImplementationType(TUid::Uid(0x1027379e));
	// string value of the input source data	
	HBufC8* packageData = HBufC8::NewLC(KMaxDataTypeLength);
	TPtr8 sourceData = packageData->Des();
	TFourCC srcDataType = KMMFFourCCCodeTestPCM8;
	srcDataType.FourCC(&sourceData);
	customMatchData->SetInputDataL(sourceData);

	// Not setting destination type
 	HBufC8* package  = customMatchData->NewPackLC();
	TPtr8 packageDes = package->Des();
	TEComResolverParams resolverParams; // Parameters on which to match
	resolverParams.SetDataType(packageDes);
	
	RImplInfoPtrArray ecomArray;
	CleanupResetAndDestroyPushL(ecomArray);

	TRAPD(error, REComSession::ListImplementationsL(TUid::Uid(KUidMdfProcessingUnit), 
		  resolverParams, TUid::Uid(0x1027379b), ecomArray));
	
	if (error != KErrNone)
		{
		CleanupStack::PopAndDestroy(3); // customMatchData, packageData, package	
		return EFail;
		}

	// check if there are any 
	TInt noOfProcessingUnits = ecomArray.Count();
	if(noOfProcessingUnits == 0)
		{
		return EFail;
		}
		// verify if one of the PUs found is what we are looking for
	for(TInt index = 0; index < noOfProcessingUnits; index++)
		{
		TUid uid = ecomArray[index]->ImplementationUid();
		CCodecApiOpaqueData* parse = NULL;
		const TDesC8* opaqueData = &(ecomArray[index]->OpaqueData());
		// if opaque data is empty, move to the next element in the list
		if (opaqueData->Length() == 0)
			{
			INFO_PRINTF1(_L("PU Resolver returned a PU with empty opaque data"));
			return EFail;
			}
		
		TRAPD(error, parse = CCodecApiOpaqueData::NewL(*opaqueData));
		if (error!=KErrNone)
			{
			if (error==KErrNotSupported)
				{
				// means that the resource entry was not valid
				INFO_PRINTF1(_L("PU Resolver returned a PU with invalid opaque data"));
				return EFail;
				}				
			}	
		CleanupStack::PushL(parse);	
		HBufC8* inputDataFormat 		= customMatchData->InputDataFormat().AllocLC();
		if(!parse->CompareInputDataType(*inputDataFormat) )
			{	
			INFO_PRINTF1(_L("PU Resolver returned a PU with a different input data type"));
			return EFail;			
			}
		CleanupStack::PopAndDestroy(2);		
		}
	
	CleanupStack::PopAndDestroy(4); // customMatchData, packageData, package, ecomArray		  
	REComSession::FinalClose();
	__UHEAP_MARKEND;
	INFO_PRINTF1(_L("The resolver located correctly the PU using just the source data type (P8) and the version"));	
	return EPass;
	}	

// ********************************************************************************
// Finds a PU using the FourCC and using the Custom Resolver with PU8 destination 
// ********************************************************************************

RTestStepAudioCodecFindPU8OutputFourCC::RTestStepAudioCodecFindPU8OutputFourCC()
	{
	iTestStepName = _L("MM-MDF-AUDIOCODECS-0006");	
	}

TVerdict RTestStepAudioCodecFindPU8OutputFourCC::DoTestStepL()
	{	
	__UHEAP_MARK;
	TFourCC aDestDataType;
	INFO_PRINTF1(_L("Test locates the PU using just the destination data type (PU8) and the version"));
	// create the OMX Core
	CCodecApiResolverData* customMatchData = CCodecApiResolverData::NewLC();
	customMatchData->SetMatchType(EMatchOutputDataFormat);
	customMatchData->SetImplementationType(TUid::Uid(0x1027379e));
	// string value of the input source data	
	HBufC8* packageData = HBufC8::NewLC(KMaxDataTypeLength);
	TPtr8 destData = packageData->Des();
		
	// Not setting source type
	
	TFourCC destDataType = KMMFFourCCCodeTestPCMU8;
	destDataType.FourCC(&destData);
	customMatchData->SetOutputDataL(destData);

 	HBufC8* package  = customMatchData->NewPackLC();
	TPtr8 packageDes = package->Des();
	TEComResolverParams resolverParams; // Parameters on which to match
	resolverParams.SetDataType(packageDes);
	
	RImplInfoPtrArray ecomArray;
	CleanupResetAndDestroyPushL(ecomArray);

	TRAPD(error, REComSession::ListImplementationsL(TUid::Uid(KUidMdfProcessingUnit), 
		  resolverParams, TUid::Uid(0x1027379b), ecomArray));
	
	if (error != KErrNone)
		{
		CleanupStack::PopAndDestroy(3); // customMatchData, packageData, package	
		return EFail;
		}

	// check if there are any 
	TInt noOfProcessingUnits = ecomArray.Count();
	if(noOfProcessingUnits == 0)
		{
		return EFail;
		}
	
	// verify if one of the PUs found is what we are looking for
	for(TInt index = 0; index < noOfProcessingUnits; index++)
		{
		TUid uid = ecomArray[index]->ImplementationUid();
		CCodecApiOpaqueData* parse = NULL;
		const TDesC8* opaqueData = &(ecomArray[index]->OpaqueData());
		// if opaque data is empty, move to the next element in the list
		if (opaqueData->Length() == 0)
			{
			INFO_PRINTF1(_L("PU Resolver returned a PU with empty opaque data"));
			return EFail;
			}
		
		TRAPD(error, parse = CCodecApiOpaqueData::NewL(*opaqueData));
		if (error!=KErrNone)
			{
			if (error==KErrNotSupported)
				{
				// means that the resource entry was not valid
				INFO_PRINTF1(_L("PU Resolver returned a PU with invalid opaque data"));
				return EFail;
				}				
			}	
		CleanupStack::PushL(parse);	
		HBufC8* outputDataFormat = customMatchData->OutputDataFormat().AllocLC();
		if(!parse->CompareOutputDataType(*outputDataFormat))
			{	
			INFO_PRINTF1(_L("PU Resolver returned a PU with a different output data type"));
			return EFail;			
			}
		CleanupStack::PopAndDestroy(2);		
		}	
	
	
	CleanupStack::PopAndDestroy(4); // customMatchData, packageData, package, ecomArray		  
	REComSession::FinalClose();
	__UHEAP_MARKEND;
	INFO_PRINTF1(_L("The resolver located the PU using just the destination data type (PU8) and the version"));
	return EPass;
	}	
	
// **************************************************************************************************
// Attempts to find a PU using the FourCC and Custom Resolver with correct version but incorrect src
// **************************************************************************************************

RTestStepAudioCodecFindIncorrectInputFourCC::RTestStepAudioCodecFindIncorrectInputFourCC()
	{
	iTestStepName = _L("MM-MDF-AUDIOCODECS-0008");	
	}

TVerdict RTestStepAudioCodecFindIncorrectInputFourCC::DoTestStepL()
	{
	__UHEAP_MARK;
	TFourCC aSrcDataType;
	INFO_PRINTF1(_L("Test attempts to locate the PU using the correct version, but incorrect source data type"));
	// create the OMX Core
	CCodecApiResolverData* customMatchData = CCodecApiResolverData::NewLC();
	customMatchData->SetMatchType(EMatchInputDataFormat);
	customMatchData->SetImplementationType(TUid::Uid(0x1027379e));
	// string value of the input source data	
	HBufC8* packageData = HBufC8::NewLC(KMaxDataTypeLength);
	TPtr8 sourceData = packageData->Des();
	TFourCC srcDataType = KMMFFourCCCodeNULL;
	srcDataType.FourCC(&sourceData);
	customMatchData->SetInputDataL(sourceData);

	// Not setting destination type

 	HBufC8* package  = customMatchData->NewPackLC();
	TPtr8 packageDes = package->Des();
	TEComResolverParams resolverParams; // Parameters on which to match
	resolverParams.SetDataType(packageDes);
	
	RImplInfoPtrArray ecomArray;
	CleanupResetAndDestroyPushL(ecomArray);

	TRAPD(error, REComSession::ListImplementationsL(TUid::Uid(KUidMdfProcessingUnit), 
		  resolverParams, TUid::Uid(0x1027379b), ecomArray));
	
	if (error != KErrNone)
		{
		CleanupStack::PopAndDestroy(3); // customMatchData, packageData, package	
		return EFail;
		}

	// check if there are any 
	TInt noOfProcessingUnits = ecomArray.Count();
	CleanupStack::PopAndDestroy(4); // customMatchData, packageData, package, ecomArray		  
	REComSession::FinalClose();
	__UHEAP_MARK;
	if(noOfProcessingUnits == 0)
		{
		// Should be zero, since no implementations of type KCMdfProcessingUnit can deal with KMMFFourCCCodeNULL
		return EPass;
		}
	
	return EFail;
	}


// *******************************************************************************************
// Attempts to find a PU using the FourCC and Custom Resolver with the correct interface UID,
// but incorrect implementation UID.
// *******************************************************************************************

RTestStepAudioCodecFindIncorrectImpUid::RTestStepAudioCodecFindIncorrectImpUid()
	{
	iTestStepName = _L("MM-MDF-AUDIOCODECS-0009");	
	}

TVerdict RTestStepAudioCodecFindIncorrectImpUid::DoTestStepL()
	{
	__UHEAP_MARK;
	TFourCC aSrcDataType;
	INFO_PRINTF1(_L("Test attempts to locate the PU  with the correct interface UID, but incorrect implementation UID."));
	// create the OMX Core
	CCodecApiResolverData* customMatchData = CCodecApiResolverData::NewLC();
	customMatchData->SetMatchType(EMatchInputDataFormat);
	// Set implementation interface as incorrect - from KUidMediaTypeAudio
	customMatchData->SetImplementationType(TUid::Uid(0x101f5d07));
	// string value of the input source data	
	HBufC8* packageData = HBufC8::NewLC(KMaxDataTypeLength);
	TPtr8 sourceData = packageData->Des();
	TFourCC srcDataType = KMMFFourCCCodeTestPCM8;
	srcDataType.FourCC(&sourceData);
	customMatchData->SetInputDataL(sourceData);

	TFourCC destDataType = KMMFFourCCCodePCM16;
	destDataType.FourCC(&sourceData);
	customMatchData->SetOutputDataL(sourceData);

 	HBufC8* package  = customMatchData->NewPackLC();
	TPtr8 packageDes = package->Des();
	TEComResolverParams resolverParams; // Parameters on which to match
	resolverParams.SetDataType(packageDes);
	
	RImplInfoPtrArray ecomArray;
	CleanupResetAndDestroyPushL(ecomArray);

	// Interface Uid is correct
	TRAPD(error, REComSession::ListImplementationsL(TUid::Uid(KUidMdfProcessingUnit), 
		  resolverParams, TUid::Uid(0x1027379b), ecomArray));
	
	if (error != KErrNone)
		{
		CleanupStack::PopAndDestroy(3); // customMatchData, packageData, package	
		return EFail;
		}

	// check if there are any 
	TInt noOfProcessingUnits = ecomArray.Count();
	CleanupStack::PopAndDestroy(4); // customMatchData, packageData, package, ecomArray		  
	REComSession::FinalClose();
	__UHEAP_MARKEND;
	if(noOfProcessingUnits == 0)
		{
		// Should be zero, since no implementations
		return EPass;
		}
	
	return EFail;
	}
	
// *******************************************************************************************
// Attempts to find a PU using the FourCC and Custom Resolver with the correct implementation
// UID, but incorrect interface UID.
// *******************************************************************************************

RTestStepAudioCodecFindIncorrectInterfaceUid::RTestStepAudioCodecFindIncorrectInterfaceUid()
	{
	iTestStepName = _L("MM-MDF-AUDIOCODECS-0010");	
	}

TVerdict RTestStepAudioCodecFindIncorrectInterfaceUid::DoTestStepL()
	{
	__UHEAP_MARK;
	TFourCC aSrcDataType;
	INFO_PRINTF1(_L("Test attempts to locate the PU  with the correct interface UID, but incorrect implementation UID."));
	// create the OMX Core
	CCodecApiResolverData* customMatchData = CCodecApiResolverData::NewLC();
	customMatchData->SetMatchType(EMatchInputDataFormat);
	// Set implementation interface as correct
	customMatchData->SetImplementationType(TUid::Uid(0x1027379e));
	// string value of the input source data	
	HBufC8* packageData = HBufC8::NewLC(KMaxDataTypeLength);
	TPtr8 sourceData = packageData->Des();
	TFourCC srcDataType = KMMFFourCCCodeTestPCM8;
	srcDataType.FourCC(&sourceData);
	customMatchData->SetInputDataL(sourceData);

	TFourCC destDataType = KMMFFourCCCodePCM16;
	destDataType.FourCC(&sourceData);
	customMatchData->SetOutputDataL(sourceData);

 	HBufC8* package  = customMatchData->NewPackLC();
	TPtr8 packageDes = package->Des();
	TEComResolverParams resolverParams; // Parameters on which to match
	resolverParams.SetDataType(packageDes);
	
	RImplInfoPtrArray ecomArray;
	CleanupResetAndDestroyPushL(ecomArray);

	// Interface Uid is incorrect - from KMmfUidPluginInterfaceDataSource			
	TRAPD(error, REComSession::ListImplementationsL(TUid::Uid(0x101F53AF), 
		  resolverParams, TUid::Uid(0x1027379b), ecomArray));
	
	if (error != KErrNone)
		{
		CleanupStack::PopAndDestroy(3); // customMatchData, packageData, package	
		return EFail;
		}

	// check if there are any 
	TInt noOfProcessingUnits = ecomArray.Count();
	CleanupStack::PopAndDestroy(4); // customMatchData, packageData, package, ecomArray		  
	REComSession::FinalClose();
	__UHEAP_MARKEND;
	if(noOfProcessingUnits == 0)
		{
		// Should be zero, since no implementations
		return EPass;
		}
	
	return EFail;
	}	
	

RTestStepVideoResolverUtils::RTestStepVideoResolverUtils()
	{
	iTestStepName = _L("MM-MDF-AUDIOCODECS-0011");
	}
	
TVerdict RTestStepVideoResolverUtils::DoTestStepL()
	{
	INFO_PRINTF1(_L("Tests the video resolver utility "));
	_LIT8(KText, "<m>Manufacturer<p>320,200<l>100,20,4000");
	TBufC8<50> data(KText);
	TPtr8 ptrData = data.Des();
	
	CCodecApiVideoOpaqueData* codecApiVideoOpaqueData = CCodecApiVideoOpaqueData::NewLC(ptrData);
	
	// verify the picture size
	TSize picSize = codecApiVideoOpaqueData->MaxPictureSize();
	if (picSize.iWidth != KPicSizeX &&
		picSize.iHeight != KPicSizeY)
		{
		INFO_PRINTF1(_L("Incorrect picture size"));		
		return EFail;
		}
	
	RArray<TPictureRateAndSize> pictureRateAndSize = codecApiVideoOpaqueData->MaxPictureRates();
	TUint arraySize = pictureRateAndSize.Count();
	if (arraySize == 0)
		{
		INFO_PRINTF1(_L("No picture rate and size extracted from the opaque data"));		
		CleanupStack::PopAndDestroy(codecApiVideoOpaqueData);
		return EFail;
		}
	
	INFO_PRINTF2(_L("No picture rate and size extracted from the opaque data, %d"), pictureRateAndSize[0].iPictureRate);	
	if (pictureRateAndSize[0].iPictureRate != KPicRate)	
		{
		INFO_PRINTF1(_L("Incorrect picture rate"));		
		CleanupStack::PopAndDestroy(codecApiVideoOpaqueData);
		return EFail;
		}
		
	if (pictureRateAndSize[0].iPictureSize.iWidth != KPicWidth)	
		{
		INFO_PRINTF1(_L("Incorrect picture width"));		
		CleanupStack::PopAndDestroy(codecApiVideoOpaqueData);
		return EFail;
		}
			
	if (pictureRateAndSize[0].iPictureSize.iHeight != KPicHeight)	
		{
		INFO_PRINTF1(_L("Incorrect picture heigh"));		
		CleanupStack::PopAndDestroy(codecApiVideoOpaqueData);
		return EFail;
		}		
		
	TDesC8 manufacturer = codecApiVideoOpaqueData->Manufacturer();
	if(manufacturer.Compare(KTextManufactor) == 0)
		{
		INFO_PRINTF1(_L("Incorrect manufacturer"));		
		CleanupStack::PopAndDestroy(codecApiVideoOpaqueData);
		return EFail;
		}
		
	CleanupStack::PopAndDestroy(codecApiVideoOpaqueData);
	return EPass;
	}
	
	
// *******************************************************************************************
// Attempts to find a PU using the FourCC and Custom Resolver with the correct implementation
// UID, but incorrect interface UID.
// *******************************************************************************************
RTestStepResolverUtils::RTestStepResolverUtils()
	{
	iTestStepName = _L("MM-MDF-AUDIOCODECS-0012");
	}
	
TVerdict RTestStepResolverUtils::DoTestStepL()
	{
	INFO_PRINTF1(_L("Tests the video resolver utility "));
	_LIT8(KText, "<s>");
	TBufC8<50> data(KText);
	TPtr8 ptrData = data.Des();
	
	CCodecApiOpaqueData* codecApiOpaqueData = CCodecApiOpaqueData::NewLC(ptrData);
	const TDesC8 inputData = codecApiOpaqueData->InputDataType();
	if (inputData.CompareF(KNullDesC8) != 0)
		{
		INFO_PRINTF1(_L("Incorrect manufacturer"));		
		CleanupStack::PopAndDestroy(codecApiOpaqueData);
		return EFail;
		}
		
	const TDesC8 outputData = codecApiOpaqueData->OutputDataType();
	if (outputData.CompareF(KNullDesC8) != 0)
		{
		INFO_PRINTF1(_L("Incorrect manufacturer"));		
		CleanupStack::PopAndDestroy(codecApiOpaqueData);
		return EFail;
		}	
	
	CleanupStack::PopAndDestroy(codecApiOpaqueData);
	return EPass;
	}


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

#include "tsu_mdf_codecapi_resolver2.h"
#include <openmax/il/khronos/v1_x/OMX_Core.h>
#include <mmf/server/mmfhwdevicesetup.h>
#include <mdf/codecapiresolverdata.h>
#include <mdf/codecapiuids.hrh>
#include <mmf/server/mmfhwdevicesetup.h>
#include <mdf/codecapiresolverutils.h>
#include <mdf/codecapivideoresolverutils.h>
#include <mm/mmcleanup.h>

//
// RTestStepCodecApiLoadVideoEncoder
// Loads a video encoder PU using the MIME type and using the Custom Resolver 

/**
 * Constructor
 */
RTestStepCodecApiLoadVideoEncoder::RTestStepCodecApiLoadVideoEncoder()
	{
	iTestStepName = _L("MM-MDF-CODECAPI-U-0050-HP");	
	}

/**
 * Do the test step
 */
TVerdict RTestStepCodecApiLoadVideoEncoder::DoTestStepL()
	{
	__UHEAP_MARK;

	INFO_PRINTF1(_L("Test loading video encoder PU using mime type"));
	// create the OMX Core
	CCodecApiResolverData* customMatchData = CCodecApiResolverData::NewLC();
	customMatchData->SetMatchType(EMatchOutputDataFormat);
	
	// ask it for a video encoder
	customMatchData->SetImplementationType(TUid::Uid(0x1027379f));
	
	// NB source/dest data are mime types for video (not FourCC codes)
	customMatchData->SetOutputDataL(KMMFMimeTypeMp4v);
	
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
		CleanupStack::PopAndDestroy(2); // customMatchData, package	
		return EFail;
		}

	// verify if the resolver found any PUs 
	TVerdict verdict = EPass;
	TInt noOfProcessingUnits = ecomArray.Count();
	if(noOfProcessingUnits == 0)
		{
		INFO_PRINTF1(_L("PU Resolver did not find any PU with these data"));
		verdict = EFail;
		}
	else
		{
		
	
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
				// means that the resource entry was not valid
				INFO_PRINTF1(_L("PU Resolver returned a PU with invalid opaque data"));			
				}	
			else 
				{
				CleanupStack::PushL(parse);	
				HBufC8* inputDataFormat 		= customMatchData->InputDataFormat().AllocLC();
				HBufC8* outputDataFormat		= customMatchData->OutputDataFormat().AllocLC();
				if(!parse->CompareInputDataType(*inputDataFormat) 
					&& !parse->CompareOutputDataType(*outputDataFormat))
					{	
					INFO_PRINTF1(_L("PU Resolver returned a PU with a different input and output data type"));
					verdict = EFail;			
					}
				CleanupStack::PopAndDestroy(3);		
				}
			}
		}
		
	CleanupStack::PopAndDestroy(3); // customMatchData, package, ecomArray		  
	REComSession::FinalClose();
	__UHEAP_MARKEND;

	INFO_PRINTF1(_L("PU Resolver returned the correct PU"));
	return verdict;
	}
	
//
// RTestStepCodecApiLoadVideoDecoder
// Loads a video decoder PU using the MIME type and using the Custom Resolver 

/**
 * Constructor
 */
RTestStepCodecApiLoadVideoDecoder::RTestStepCodecApiLoadVideoDecoder()
	{
	iTestStepName = _L("MM-MDF-CODECAPI-U-0051-HP");	
	}

/**
 * Do the test step
 */
TVerdict RTestStepCodecApiLoadVideoDecoder::DoTestStepL()
	{
	__UHEAP_MARK;

	INFO_PRINTF1(_L("Test loading video decoder PU using mime type"));
	// create the OMX Core
	CCodecApiResolverData* customMatchData = CCodecApiResolverData::NewLC();
	customMatchData->SetMatchType(EMatchOutputDataFormat);
	
	// ask it for a video decoder
	customMatchData->SetImplementationType(TUid::Uid(0x102737a0));
	
	// NB source/dest data are mime types for video (not FourCC codes)
	customMatchData->SetInputDataL(KMMFMimeTypeMp4v);
	
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
		CleanupStack::PopAndDestroy(2); // customMatchData, package	
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
		
		//TRAPD(error,
		parse = CCodecApiVideoOpaqueData::NewL(*opaqueData);
		//);
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
		
	CleanupStack::PopAndDestroy(3); // customMatchData, package, ecomArray		  
	REComSession::FinalClose();
	__UHEAP_MARKEND;

	INFO_PRINTF1(_L("PU Resolver returned the correct PU"));
	return EPass;
	}

//
// RTestStepCodecApiLoadVideoOpaqueData
// Find a video PU using opaque data.

/**
 * Constructor
 */
RTestStepCodecApiLoadVideoOpaqueData::RTestStepCodecApiLoadVideoOpaqueData()
	{
	iTestStepName = _L("MM-MDF-CODECAPI-U-0040-HP");	
	}

/**
 * Do the test step
 */
TVerdict RTestStepCodecApiLoadVideoOpaqueData::DoTestStepL()
	{
	TVerdict verdict = EFail;
	
	__UHEAP_MARK;

	INFO_PRINTF1(_L("Test finding video PU using opaque data"));
	// create the OMX Core
	CCodecApiResolverData* customMatchData = CCodecApiResolverData::NewLC();
	customMatchData->SetMatchType(EMatchOutputDataFormat);
	
	// ask it for a video decoder
	customMatchData->SetImplementationType(TUid::Uid(0x102737a0));
	customMatchData->SetInputDataL(KMMFMimeTypeMp4v);
	
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
		ERR_PRINTF2(_L("REComSession::ListImplementationsL left with error %d"), error);
		CleanupStack::PopAndDestroy(2); // customMatchData, package	
		return verdict;
		}

	// verify if the resolver found any PUs 
	TInt noOfProcessingUnits = ecomArray.Count();
	if(noOfProcessingUnits == 0)
		{
		ERR_PRINTF1(_L("PU Resolver did not find any PU with these data"));
		return verdict;
		}
	
	// look at the opaque data of our PUs
	// we will find the VideoTestDecoder (uid 0x102737E6) which has
	// an <m> tag of "Symbian"
	
	for(TInt index = 0; index < noOfProcessingUnits; index++)
		{
		TUid uid = ecomArray[index]->ImplementationUid();
		if(uid == KUidVideoTestDecoderPu) 
			{
			INFO_PRINTF1(_L("Found the video test decoder"));
		
			CCodecApiVideoOpaqueData* parse = NULL;
			const TDesC8* opaqueData = &(ecomArray[index]->OpaqueData());
			// if opaque data is empty, move to the next element in the list
			if (opaqueData->Length() == 0)
				{
				ERR_PRINTF1(_L("PU has empty opaque data"));
				return verdict;
				}
		
			TRAPD(error, parse = CCodecApiVideoOpaqueData::NewL(*opaqueData));
			if (error!=KErrNone)
				{
				if (error==KErrNotSupported)
					{
					// means that the resource entry was not valid
					ERR_PRINTF1(_L("PU has invalid opaque data"));
					return verdict;
					}
				else 
					{
					// some other error
					ERR_PRINTF2(_L("CCodecApiVideoOpaqueData::NewL left with error %d"), error);
					return verdict;
					}
				}				
			CleanupStack::PushL(parse);
			const TDesC8& theManufacturer = parse->Manufacturer();	
			if(&theManufacturer == NULL || (theManufacturer != KPuManufacturerSymbian))
				{
				ERR_PRINTF1(_L("Error - Manufacturer string invalid"));
				}
			else
				{	
				INFO_PRINTF1(_L("Manufacturer string valid"));
				verdict = EPass;
				}
			CleanupStack::PopAndDestroy(parse);	
			break;
			}
		}
		
	CleanupStack::PopAndDestroy(3); // customMatchData, package, ecomArray		  
	REComSession::FinalClose();
	__UHEAP_MARKEND;

	INFO_PRINTF1(_L("PU Resolver finished"));
	return verdict;
	}
	
//
// RTestStepCodecApiLoadVideoOpaqueDataInvalid
// Try and find a video PU using invalid opaque data.

/**
 * Constructor
 */
RTestStepCodecApiLoadVideoOpaqueDataInvalid::RTestStepCodecApiLoadVideoOpaqueDataInvalid()
	{
	iTestStepName = _L("MM-MDF-CODECAPI-U-1040-HP");	
	}

/**
 * Do the test step
 */
TVerdict RTestStepCodecApiLoadVideoOpaqueDataInvalid::DoTestStepL()
	{
	TVerdict verdict = EFail;
	
	__UHEAP_MARK;

	INFO_PRINTF1(_L("Test finding video PU using invalid opaque data"));
	// create the OMX Core
	CCodecApiResolverData* customMatchData = CCodecApiResolverData::NewLC();
	customMatchData->SetMatchType(EMatchOutputDataFormat);
	
	// ask it for a video decoder
	customMatchData->SetImplementationType(TUid::Uid(0x102737a0));
	customMatchData->SetInputDataL(KMMFMimeTypeMp4v);
	
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
		ERR_PRINTF2(_L("REComSession::ListImplementationsL left with error %d"), error);
		CleanupStack::PopAndDestroy(2); // customMatchData, package	
		return verdict;
		}

	// verify if the resolver found any PUs 
	TInt noOfProcessingUnits = ecomArray.Count();
	if(noOfProcessingUnits == 0)
		{
		ERR_PRINTF1(_L("PU Resolver did not find any PU with these data"));
		return verdict;
		}
	
	// look at the opaque data of our PUs
	// we will try and find the VideoTestDecoder (uid 0x102737E6) which has
	// an <m> tag of "Symbian" but ask it for some other <m> value
	
	for(TInt index = 0; index < noOfProcessingUnits; index++)
		{
		TUid uid = ecomArray[index]->ImplementationUid();
		if(uid == KUidVideoTestDecoderPu) 
			{
			INFO_PRINTF1(_L("Found the video test decoder"));
		
			CCodecApiVideoOpaqueData* parse = NULL;
			const TDesC8* opaqueData = &(ecomArray[index]->OpaqueData());
			// if opaque data is empty, move to the next element in the list
			if (opaqueData->Length() == 0)
				{
				ERR_PRINTF1(_L("PU has empty opaque data"));
				return verdict;
				}
		
			TRAPD(error, parse = CCodecApiVideoOpaqueData::NewL(*opaqueData));
			if (error!=KErrNone)
				{
				if (error==KErrNotSupported)
					{
					// means that the resource entry was not valid
					ERR_PRINTF1(_L("PU has invalid opaque data"));
					return verdict;
					}
				else 
					{
					// some other error
					ERR_PRINTF2(_L("CCodecApiVideoOpaqueData::NewL left with error %d"), error);
					return verdict;
					}
				}	
			else 
				{
				
				CleanupStack::PushL(parse);	
				const TDesC8& theManufacturer = parse->Manufacturer();	
				if(&theManufacturer == NULL || (theManufacturer != KPuManufacturerInvalid)) 
					{	
					INFO_PRINTF1(_L("Manufacturer string invalid as expected"));
					verdict = EPass;
					}
				else 
					{
					ERR_PRINTF1(_L("Error - Manufacturer string valid"));
					}
				CleanupStack::PopAndDestroy(parse);	
				break;
				}
			
			}
		}
		
	CleanupStack::PopAndDestroy(3); // customMatchData, package, ecomArray		  
	REComSession::FinalClose();
	__UHEAP_MARKEND;

	INFO_PRINTF1(_L("PU Resolver finished"));
	return verdict;
	}
	
// end

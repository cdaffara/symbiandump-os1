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

/**
 @file
 @publishedPartner
 @prototype
*/

#include <mdf/mdfprocessingunit.h>
#include <mmf/common/mmfutilities.h>
#include "puloader.h"
#include <mdf/codecapiresolverdata.h>
#include <mdf/codecapiuids.hrh>
#include <mdf/codecapiresolver.hrh>
#include <mm/mmcleanup.h>
#include <ecom/ecom.h>

static const TInt KMaxDataTypeLength = 126;

CPuLoader* CPuLoader::NewL()
	{
	CPuLoader* self = new (ELeave) CPuLoader;
	return self;
	}


CMdfProcessingUnit* CPuLoader::LoadProcessingUnitL(const MMdfProcessingUnitObserver& aProcessingUnitObserver, 
		TUid aImplementationUid)
	{
	CMdfProcessingUnit* pu = NULL;
	pu = CMdfProcessingUnit::NewL(aImplementationUid);
	CleanupStack::PushL(pu);
	User::LeaveIfError(pu->Create(aProcessingUnitObserver));
	CleanupStack::Pop(pu);
	return pu;
	}
	
CMdfProcessingUnit* CPuLoader::LoadProcessingUnitL( const MMdfProcessingUnitObserver& aProcessingUnitObserver, 
		const TDesC8& aSrcDataType, const TDesC8& aDestDataType, const TUid& aImplementationType) 
	{
	CCodecApiResolverData* customMatchData = CCodecApiResolverData::NewLC();
	customMatchData->SetMatchType(EMatchInputAndOutputDataFormat);
	customMatchData->SetImplementationType(aImplementationType);
	// string value of the input source data	
	customMatchData->SetInputDataL(aSrcDataType);
	customMatchData->SetOutputDataL(aDestDataType);
	
 	HBufC8* package  = customMatchData->NewPackLC();

	TEComResolverParams resolverParams; // Parameters on which to match
	resolverParams.SetDataType(package->Des());
	
	RImplInfoPtrArray ecomArray;
	CleanupResetAndDestroyPushL(ecomArray);

	REComSession::ListImplementationsL(TUid::Uid(KUidMdfProcessingUnit), resolverParams, TUid::Uid(KUidCodecApiResolverImplementation), ecomArray);
	
	// check if there are any 
	TInt noOfProcessingUnits = ecomArray.Count();

	if(noOfProcessingUnits == 0)
		{
		User::Leave(KErrNotFound);
		}

	CMdfProcessingUnit *processingUnit = NULL;
	TInt index = 0;
	TInt error = KErrNone;
	do
		{
		// Iterate through the array until a plugin can be created without errors
		const CImplementationInformation& puInfo = *(ecomArray[index++]);
		
		TRAP(error, processingUnit = CMdfProcessingUnit::NewL(puInfo.ImplementationUid()));
		}
	while(index < noOfProcessingUnits && error != KErrNone);

	if(error)
		{
		ASSERT(!processingUnit && index <= noOfProcessingUnits);
		User::Leave(error);
		}

	CleanupStack::PopAndDestroy(3, customMatchData); // customMatchData, package, ecomArray	

		
	TInt err = processingUnit->Create(aProcessingUnitObserver);
	if (err != KErrNone)
		{
		// delete processing unit before leaving
		delete processingUnit;
		User::Leave(err);
		}
	return processingUnit;
	}
	
CMdfProcessingUnit* CPuLoader::LoadProcessingUnitL( const MMdfProcessingUnitObserver& aProcessingUnitObserver, 
		TFourCC aSrcDataType, TFourCC aDestDataType)
	{
	HBufC8* packageSrcDataType = HBufC8::NewLC(KMaxDataTypeLength);
	TPtr8 sourceDataType = packageSrcDataType->Des();
	aSrcDataType.FourCC(&sourceDataType);
			
	HBufC8* packageDestDataType = HBufC8::NewLC(KMaxDataTypeLength);
	TPtr8 destinationDataType = packageDestDataType->Des();
	aDestDataType.FourCC(&destinationDataType);
	
	CMdfProcessingUnit* processingUnit = LoadProcessingUnitL(aProcessingUnitObserver, sourceDataType, destinationDataType, TUid::Uid(KUidAudioCodec));
	CleanupStack::PopAndDestroy(2, packageSrcDataType);
	return processingUnit;
	}


TInt CPuLoader::TunnelSetup(MMdfOutputPort& aOutputPort, MMdfInputPort& aInputPort)
	{
	TTunnelFlags tunnelFlags = EBufferReadOnly;
	TSupplierType supplierType = EBufferSupplyUnspecified;
	TUint error = KErrNone;
	 
	if ((error = aOutputPort.MopTunnelRequest(aInputPort, tunnelFlags, supplierType)) != KErrNone)
		{
	 	return error;
	 	}
	return aInputPort.MipTunnelRequest(aOutputPort,tunnelFlags,supplierType);
	}
	

void CPuLoader::UnloadProcessingUnit(CMdfProcessingUnit*& aPu)
	{
	delete aPu;
	aPu = NULL;
	}
	
CPuLoader::CPuLoader()
	{
	}

CPuLoader::~CPuLoader()
	{
	}

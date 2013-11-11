// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32base.h>
#include <bluetooth/eirpublishermanufacturerspecificdata.h>

void Panic(TManufacturerSpecificDataPanics aPanic)
	{
	User::Panic(KManufacturerSpecificDataPanic, aPanic);
	}
	

/**
 Constructs an Manufacturer Specific Data publisher object.

It takes an M-class interface for notifying the client about request result

 @param aNotifier holds a reference of the client, which will be notified about request result 
 @return Pointer to the created CEirPublisherManufacturerSpecificData object
*/
EXPORT_C CEirPublisherManufacturerSpecificData* CEirPublisherManufacturerSpecificData::NewL(MManufacturerSpecificDataPublisherNotifier& aNotifier)
	{
	CEirPublisherManufacturerSpecificData* self = new (ELeave) CEirPublisherManufacturerSpecificData(aNotifier);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

/**
 Client can use this function to request the size of data it wants to publish
 @param aSize is the size of data intended to be published
 */
EXPORT_C void CEirPublisherManufacturerSpecificData::RequestPublishDataSize(TUint aSize)
	{
	iRequestedDataSize = aSize;
	iPublisher->PublishData(aSize);
	}

/**
 Client can use this function to publish data according to the given available bytes
 @param aManufacturerSpecificData is the data to be published
 */
EXPORT_C void CEirPublisherManufacturerSpecificData::SetManufacturerSpecificData(TDesC8& aManufacturerSpecificData)
	{
	__ASSERT_ALWAYS(aManufacturerSpecificData.Length() <=iBytesAvailable, Panic(EManufacturerSpecificDataOutOfBoundary));
	// aBytesAvailable is not 0 and Client gives the same amount of data as originally requested
	iPublisher->SetData(aManufacturerSpecificData, EEirDataComplete);
	}

CEirPublisherManufacturerSpecificData::CEirPublisherManufacturerSpecificData(MManufacturerSpecificDataPublisherNotifier& aNotifier)
	: CEirPublisherBase(EEirTagManufacturerSpecific)
	, iRequestedDataSize(0)
	, iNotifier(aNotifier)
	{
	}

// From MEirPublisherNotifier
void CEirPublisherManufacturerSpecificData::MepnSpaceAvailable(TUint aBytesAvailable)
	{
	iBytesAvailable = aBytesAvailable;
	iNotifier.MmsdpnNotifyAvailableDataSize(iBytesAvailable);
	}
	
void CEirPublisherManufacturerSpecificData::MepnSetDataError(TInt aResult)
	{
	iNotifier.MmsdpnNotifyError(aResult);
	}

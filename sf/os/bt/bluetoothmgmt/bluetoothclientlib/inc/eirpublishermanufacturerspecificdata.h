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

/**
 @file
 @publishedPartner
 @released
*/

#ifndef EIRPUBLISHERMANUFACTURERSPECIFICDATA_H
#define EIRPUBLISHERMANUFACTURERSPECIFICDATA_H

#include <e32base.h>
#include <bluetooth/eirpublisherbase.h>

_LIT(KManufacturerSpecificDataPanic, "ManufacturerSpecificDataPublisher");

enum TManufacturerSpecificDataPanics
	{
	EManufacturerSpecificDataOutOfBoundary,
	};


//**********************************
// MManufacturerSpecificDataPublisherNotifier
//**********************************
/**
Interface for callback notification when publishing manufacturer specific data
**/
class MManufacturerSpecificDataPublisherNotifier
	{
public:
	virtual void MmsdpnNotifyAvailableDataSize(TUint aAvailableDataSize) = 0;
	virtual void MmsdpnNotifyError(TInt aResult) = 0;
	};

//**********************************
// CEirPublisherManufacturerSpecificData
//**********************************
/**
Provides functionality to publish Manufacturer Specific Data to EIR.
**/
NONSHARABLE_CLASS(CEirPublisherManufacturerSpecificData) : public CEirPublisherBase
	{
public:
	IMPORT_C static CEirPublisherManufacturerSpecificData* NewL(MManufacturerSpecificDataPublisherNotifier& aNotifier);
	IMPORT_C void RequestPublishDataSize(TUint aSize);
	IMPORT_C void SetManufacturerSpecificData(TDesC8& aManufacturerSpecificData);
	
private:
	CEirPublisherManufacturerSpecificData(MManufacturerSpecificDataPublisherNotifier& aNotifier);

	// From MEirPublisherNotifier
	virtual void MepnSpaceAvailable(TUint aBytesAvailable);
	virtual void MepnSetDataError(TInt aResult);
private:
	TUint										iRequestedDataSize;
	TUint										iBytesAvailable;
	MManufacturerSpecificDataPublisherNotifier& iNotifier;
	};
	
#endif	// EIRPUBLISHERMANUFACTURERSPECIFICDATA_H

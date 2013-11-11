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

/**
 @file
 @internalTechnology
 
*/

#include "suplmessagecommon.h"
#include "supldevloggermacros.h" 

/**
 * Supl IPAddress container
 */
EXPORT_C CSuplIpAddress* CSuplIpAddress::NewL()
	{
	SUPLLOG(ELogP1, "CSuplIpAddress::NewL() Begin\n");
	CSuplIpAddress* self = CSuplIpAddress::NewLC();
	SUPLLOG(ELogP1, "CSuplIpAddress::NewL() End\n");
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CSuplIpAddress* CSuplIpAddress::NewLC()
	{
	SUPLLOG(ELogP1, "CSuplIpAddress::NewLC() Begin\n");
	CSuplIpAddress* self = new (ELeave) CSuplIpAddress();
	CleanupStack::PushL(self);
	self->ConstructL();
	SUPLLOG(ELogP1, "CSuplIpAddress::NewLC() End\n");
	return self;
	}

EXPORT_C CSuplIpAddress::~CSuplIpAddress()
	{
	SUPLLOG(ELogP1, "CSuplIpAddress::~CSuplIpAddress() Begin\n");
	SUPLLOG(ELogP1, "CSuplIpAddress::~CSuplIpAddress() End\n");
	}

EXPORT_C TBool CSuplIpAddress::operator==(const CSuplIpAddress& other) const
	{
	return (iIpAddressType==other.iIpAddressType && 
				iIpAddress==other.iIpAddress);
	}

CSuplIpAddress::CSuplIpAddress()
	{
	}

void CSuplIpAddress::ConstructL()
	{
	}


/**
 * Supl FQDN container
 */
EXPORT_C CSuplFqdn* CSuplFqdn::NewL()
	{
	SUPLLOG(ELogP1, "CSuplFqdn::NewL() Begin\n");
	CSuplFqdn* self = new (ELeave) CSuplFqdn();
	CleanupStack::PushL(self);
	self->ConstructL();
	SUPLLOG(ELogP1, "CSuplFqdn::NewL() End\n");
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CSuplFqdn::~CSuplFqdn()
	{
	SUPLLOG(ELogP1, "CSuplFqdn::~CSuplFqdn() Begin\n");
	SUPLLOG(ELogP1, "CSuplFqdn::~CSuplFqdn() End\n");
	}

EXPORT_C TBool CSuplFqdn::operator==(const CSuplFqdn& other) const
	{
	return (iFqdn==other.iFqdn);
	}
	
CSuplFqdn::CSuplFqdn()
	{
	}

void CSuplFqdn::ConstructL()
	{
	}

		
/**
 * Supl SLP Address container
 */
EXPORT_C CSuplSlpAddress* CSuplSlpAddress::NewL()
	{
	SUPLLOG(ELogP1, "CSuplSlpAddress::NewL() Begin\n");
	CSuplSlpAddress* self = new (ELeave) CSuplSlpAddress();
	CleanupStack::PushL(self);
	self->ConstructL();
	SUPLLOG(ELogP1, "CSuplSlpAddress::NewL() End\n");
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CSuplSlpAddress::~CSuplSlpAddress()
	{
	SUPLLOG(ELogP1, "CSuplSlpAddress::~CSuplSlpAddress() Begin\n");
	delete iIpAddress;
	delete iFqdn;
	SUPLLOG(ELogP1, "CSuplSlpAddress::~CSuplSlpAddress() End\n");
	}

EXPORT_C TBool CSuplSlpAddress::operator==(const CSuplSlpAddress& other) const
	{
	return (iSlpAddressType==other.iSlpAddressType &&
			*iIpAddress==*other.iIpAddress &&
			*iFqdn==*other.iFqdn);
	}
	
CSuplSlpAddress::CSuplSlpAddress()
	{
	}

void CSuplSlpAddress::ConstructL()
	{
	iIpAddress = CSuplIpAddress::NewL();
	iFqdn = CSuplFqdn::NewL();
	}

	
/**
 * Supl SLP Session ID container
 */
EXPORT_C CSuplSlpSessionId* CSuplSlpSessionId::NewL()
	{
	SUPLLOG(ELogP1, "CSuplSlpSessionId::NewL() Begin\n");
	CSuplSlpSessionId* self = new (ELeave) CSuplSlpSessionId();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	SUPLLOG(ELogP1, "CSuplSlpSessionId::NewL() End\n");
	return self;
	}

EXPORT_C CSuplSlpSessionId::~CSuplSlpSessionId()
	{
	SUPLLOG(ELogP1, "CSuplSlpSessionId::~CSuplSlpSessionId() Begin\n");
	delete iSlpAddress;
	SUPLLOG(ELogP1, "CSuplSlpSessionId::~CSuplSlpSessionId() End\n");
	}

EXPORT_C TBool CSuplSlpSessionId::operator==(const CSuplSlpSessionId& other) const
	{
	return (iSessionId==other.iSessionId && 
	        *iSlpAddress==*other.iSlpAddress);
	}

void CSuplSlpSessionId::ConstructL()
	{
	iSlpAddress = CSuplSlpAddress::NewL();
	}

	
/**
 * Supl SET ID container
 */
EXPORT_C CSuplSetId* CSuplSetId::NewL()
	{
	SUPLLOG(ELogP1, "CSuplSetId::NewL() Begin\n");
	CSuplSetId* self = new (ELeave) CSuplSetId();
	CleanupStack::PushL(self);
	self->ConstructL();
	SUPLLOG(ELogP1, "CSuplSetId::NewL() End\n");
	CleanupStack::Pop(self);
	return self;
	}
	
EXPORT_C CSuplSetId::~CSuplSetId()
	{
	SUPLLOG(ELogP1, "CSuplSetId::~CSuplSetId() Begin\n");
	delete iIpAddress;
	SUPLLOG(ELogP1, "CSuplSetId::~CSuplSetId() End\n");
	}

EXPORT_C TBool CSuplSetId::operator==(const CSuplSetId& other) const
	{
	return (iSetIdType==other.iSetIdType && 
	        *iIpAddress==*other.iIpAddress &&
	        iSetId == other.iSetId);
	}
	
CSuplSetId::CSuplSetId()
	{
	}

void CSuplSetId::ConstructL()
	{
	iIpAddress = CSuplIpAddress::NewL();
	}

	
/**
 * Supl SET Session ID container
 */	
EXPORT_C CSuplSetSessionId* CSuplSetSessionId::NewL()
	{
	SUPLLOG(ELogP1, "CSuplSetSessionId::NewL() Begin\n");
	CSuplSetSessionId* self = new (ELeave) CSuplSetSessionId();
	CleanupStack::PushL(self);
	self->ConstructL();
	SUPLLOG(ELogP1, "CSuplSetSessionId::NewL() End\n");
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CSuplSetSessionId::~CSuplSetSessionId()
	{
	SUPLLOG(ELogP1, "CSuplSetSessionId::~CSuplSetSessionId() Begin\n");
	delete iSetId;
	SUPLLOG(ELogP1, "CSuplSetSessionId::~CSuplSetSessionId() End\n");
	}

EXPORT_C TBool CSuplSetSessionId::operator==(const CSuplSetSessionId& other) const
	{
	return (iSessionId==other.iSessionId &&
	 		*iSetId==*other.iSetId);
	}

CSuplSetSessionId::CSuplSetSessionId()
	{
	}
	
void CSuplSetSessionId::ConstructL()
	{
	iSetId = CSuplSetId::NewL();
	}


/**
 * Supl Session ID container
 */
EXPORT_C CSuplSessionId* CSuplSessionId::NewL()
	{
	SUPLLOG(ELogP1, "CSuplSessionId::NewL() \n");
	CSuplSessionId* self = CSuplSessionId::NewLC();
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CSuplSessionId* CSuplSessionId::NewLC()
	{
	SUPLLOG(ELogP1, "CSuplSessionId::NewLC() Begin\n");
	CSuplSessionId* self = new (ELeave) CSuplSessionId();
	CleanupStack::PushL(self);
	self->ConstructL();
	SUPLLOG(ELogP1, "CSuplSessionId::NewLC() End\n");
	return self;
	}

EXPORT_C CSuplSessionId::~CSuplSessionId()
	{
	SUPLLOG(ELogP1, "CSuplSessionId::~CSuplSessionId() Begin\n");
	delete iSetSessionId;
	delete iSlpSessionId;
	SUPLLOG(ELogP1, "CSuplSessionId::~CSuplSessionId() End\n");
	}
	
EXPORT_C TBool CSuplSessionId::operator==(const CSuplSessionId& other) const
	{
	return (iSetSessionIdPresent==other.iSetSessionIdPresent && 
			iSlpSessionIdPresent==other.iSlpSessionIdPresent &&
			*iSetSessionId==*other.iSetSessionId && 
			*iSlpSessionId==*other.iSlpSessionId);
	}

CSuplSessionId::CSuplSessionId()
	{
	}

void CSuplSessionId::ConstructL()
	{
	iSetSessionId = CSuplSetSessionId::NewL();
	iSlpSessionId = CSuplSlpSessionId::NewL();
	}

/**
 * Supl Version container
 */
EXPORT_C CSuplVersion* CSuplVersion::NewL()
	{
	SUPLLOG(ELogP1, "CSuplVersion::NewL() Begin\n");
	CSuplVersion* self = new (ELeave) CSuplVersion();
	CleanupStack::PushL(self);
	self->ConstructL();
	SUPLLOG(ELogP1, "CSuplVersion::NewL() End\n");
	CleanupStack::Pop(self);
	return self;
	}

CSuplVersion::CSuplVersion()
	{
	}

EXPORT_C CSuplVersion::~CSuplVersion()
	{
	SUPLLOG(ELogP1, "CSuplVersion::~CSuplVersion() Begin\n");
	SUPLLOG(ELogP1, "CSuplVersion::~CSuplVersion() End\n");
	}

EXPORT_C TBool CSuplVersion::operator==(const CSuplVersion& other) const
	{
	return (iMaj==other.iMaj &&
			iMin==other.iMin &&
			iServind==other.iServind);
	}
	
void CSuplVersion::ConstructL()
	{
	}
	
/**
 * Supl GSM Cell Info container
 */
EXPORT_C CSuplGsmCellInfo* CSuplGsmCellInfo::NewL()
	{
	SUPLLOG(ELogP1, "CSuplGsmCellInfo::NewL() Begin\n");
	CSuplGsmCellInfo* self = new (ELeave) CSuplGsmCellInfo();
	CleanupStack::PushL(self);
	self->ConstructL();
	SUPLLOG(ELogP1, "CSuplGsmCellInfo::NewL() End\n");
	CleanupStack::Pop(self);
	return self;
	}
	
EXPORT_C CSuplGsmCellInfo::~CSuplGsmCellInfo()
	{
	SUPLLOG(ELogP1, "CSuplGsmCellInfo::~CSuplGsmCellInfo() Begin\n");
	SUPLLOG(ELogP1, "CSuplGsmCellInfo::~CSuplGsmCellInfo() End\n");
	}
	
CSuplGsmCellInfo::CSuplGsmCellInfo()
	{
	}

void CSuplGsmCellInfo::ConstructL()
	{
	}
		
/**
 * Supl CDMA Cell Info container
 */
EXPORT_C CSuplCdmaCellInfo* CSuplCdmaCellInfo::NewL()
	{
	SUPLLOG(ELogP1, "CSuplCdmaCellInfo::NewL() Begin\n");
	CSuplCdmaCellInfo* self = new (ELeave) CSuplCdmaCellInfo();
	CleanupStack::PushL(self);
	self->ConstructL();
	SUPLLOG(ELogP1, "CSuplCdmaCellInfo::NewL() End\n");
	CleanupStack::Pop(self);
	return self;
	}
	
EXPORT_C CSuplCdmaCellInfo::~CSuplCdmaCellInfo()
	{
	SUPLLOG(ELogP1, "CSuplCdmaCellInfo::~CSuplCdmaCellInfo() Begin\n");
	SUPLLOG(ELogP1, "CSuplCdmaCellInfo::~CSuplCdmaCellInfo() End\n");
	}

CSuplCdmaCellInfo::CSuplCdmaCellInfo()
	{
	}
	
void CSuplCdmaCellInfo::ConstructL()
	{
	}
		
/**
 * Supl WCDMA Cell Info container
 */
EXPORT_C CSuplWcdmaCellInfo* CSuplWcdmaCellInfo::NewL()
	{
	SUPLLOG(ELogP1, "CSuplWcdmaCellInfo::NewL() Begin\n");
	CSuplWcdmaCellInfo* self = new (ELeave) CSuplWcdmaCellInfo();
	CleanupStack::PushL(self);
	self->ConstructL();
	SUPLLOG(ELogP1, "CSuplWcdmaCellInfo::NewL() End\n");
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CSuplWcdmaCellInfo::~CSuplWcdmaCellInfo()
	{
	SUPLLOG(ELogP1, "CSuplWcdmaCellInfo::~CSuplWcdmaCellInfo() Begin\n");
	SUPLLOG(ELogP1, "CSuplWcdmaCellInfo::~CSuplWcdmaCellInfo() End\n");
	}

CSuplWcdmaCellInfo::CSuplWcdmaCellInfo()
	{
	}

void CSuplWcdmaCellInfo::ConstructL()
	{
	}
		
/**
 * Supl Location ID container
 */
EXPORT_C CSuplLocationId* CSuplLocationId::NewL(TSuplLocationType aType)
	{
	SUPLLOG(ELogP1, "CSuplLocationId::NewL() Begin\n");
	CSuplLocationId* self = new (ELeave) CSuplLocationId(aType);
	CleanupStack::PushL(self);
	self->ConstructL();
	SUPLLOG(ELogP1, "CSuplLocationId::NewL() End\n");
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CSuplLocationId::~CSuplLocationId()
	{
	SUPLLOG(ELogP1, "CSuplLocationId::~CSuplLocationId() Begin\n");
	delete iGsmCellInfo;
	delete iCdmaCellInfo;
	delete iWcdmaCellInfo;
	SUPLLOG(ELogP1, "CSuplLocationId::~CSuplLocationId() End\n");
	}
	
CSuplLocationId::CSuplLocationId(TSuplLocationType aType)
 : iType(aType)
	{
	}

void CSuplLocationId::ConstructL()
	{
	switch (iType)
		{
		case ESuplLocationTypeGsm:
			iGsmCellInfo = CSuplGsmCellInfo::NewL();
			break;

		case ESuplLocationTypeCdma:
			iCdmaCellInfo = CSuplCdmaCellInfo::NewL();
			break;

		case ESuplLocationTypeWcdma:
			iWcdmaCellInfo = CSuplWcdmaCellInfo::NewL();
			break;

		default:
			User::Leave(KErrNotSupported);
			break;
		}
	}
		




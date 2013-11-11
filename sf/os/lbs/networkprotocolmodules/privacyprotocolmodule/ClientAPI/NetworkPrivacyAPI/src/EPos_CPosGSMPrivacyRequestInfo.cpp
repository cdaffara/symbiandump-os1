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



#include <lbs/epos_cposgsmprivacyrequestinfo.h>
#include    "EPos_RequestInfo.h"
#include	"EPos_NetworkPrivacyPanicDeclaration.h"


// ---------------------------------------------------------
// CheckIdType Panics if invalid id type is passed.
// ---------------------------------------------------------
//
void CheckIdType(
    CPosGSMPrivacyRequestInfo::TIdType aIdType)
    {
    __ASSERT_ALWAYS(
        aIdType >= CPosGSMPrivacyRequestInfo::EIdTypeLogicalName &&
        aIdType <= CPosGSMPrivacyRequestInfo::EIdTypeIMSPublicIdentity,
        Panic(EPosNetworkPrivacyInvalidGSMIdType));
    }



// ---------------------------------------------------------------------------
/**
Default constructor.
*/
CPosGSMPrivacyRequestInfo::CPosGSMPrivacyRequestInfo()
: CPosNetworkPrivacyRequestInfo(EPosGSMPrivacyRequestInfo)
    {
    }


// ---------------------------------------------------------------------------
/**
2nd phase constructor.
*/
void CPosGSMPrivacyRequestInfo::ConstructL()
    {
    }


// ---------------------------------------------------------------------------
/**
Allocates and constructs a new CPosGSMPrivacyRequestInfo object and leaves it on the cleanup stack.
The object initially contains undefined request info data.
@return A new instance of this class.
@see SetLCSClientL()
@see SetRequestorL()
*/
EXPORT_C CPosGSMPrivacyRequestInfo* CPosGSMPrivacyRequestInfo::NewLC()
    {
    CPosGSMPrivacyRequestInfo* self = new(ELeave) CPosGSMPrivacyRequestInfo;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// ---------------------------------------------------------------------------
/**
Allocates and constructs a new CPosGSMPrivacyRequestInfo object.
The object initially contains undefined request info data.
@return A new instance of this class.
@see SetLCSClientL()
@see SetRequestorL()
*/
EXPORT_C CPosGSMPrivacyRequestInfo* CPosGSMPrivacyRequestInfo::NewL()
    {
    CPosGSMPrivacyRequestInfo* self = NewLC();
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
/**
Destructor.
Frees resources allocated for the LCS Client and Requester ID data.
*/
CPosGSMPrivacyRequestInfo::~CPosGSMPrivacyRequestInfo()
    {
    delete iLCSClientId;
    delete iRequestorId;
    }


// ---------------------------------------------------------------------------
/**
Sets the LCS Client. The LCS Client can be a URL, phone number, a name etc.

@param aLCSClient A descriptor containing data that identifies the LCS Client.
@param aIdType Indicates the format of the LCS Client data: URL, phone number etc.
@panic KPosNetworkPrivacyPanicCategory EPosNetworkPrivacyInvalidGSMIdType aIdType is not a value from CPosGSMPrivacyRequestInfo::TIdType
*/
EXPORT_C void CPosGSMPrivacyRequestInfo::SetLCSClientL(
    const TDesC& aLCSClient,
    TIdType aIdType)
    {
    CheckIdType(aIdType);
    HBufC* newLCSClient = aLCSClient.AllocL();
    delete iLCSClientId;
    iLCSClientId = newLCSClient;
    iLCSClientIdType = aIdType;
    }


// ---------------------------------------------------------------------------
/**
Sets the Requester ID. The Requester ID can be a URL, phone number, a name etc.

The Requester is the originating entity which has requested the location
of the target mobile device.

@param aRequestorId A descriptor containing data that identifies the Requester.
@param aIdType Indicates the format of the Requester ID: URL, phone number, etc.
@panic KPosNetworkPrivacyPanicCategory EPosNetworkPrivacyInvalidGSMIdType aIdType is not a value from CPosGSMPrivacyRequestInfo::TIdType
*/
EXPORT_C void CPosGSMPrivacyRequestInfo::SetRequestorL(
    const TDesC& aRequestorId,
    TIdType aIdType)
    {
    CheckIdType(aIdType);
    HBufC* newRequestorId = aRequestorId.AllocL();
    delete iRequestorId;
    iRequestorId = newRequestorId;
    iRequestorIdType = aIdType;
    }


// ---------------------------------------------------------------------------
/**
Gets the LCS Client. The LCS Client can be a URL, phone number, a name etc.

@param aLCSClientId Returns the string identifying the LCS Client.
@param aIdType Returns the format of the LCS Client data: URL, phone number etc.
               
@return KErrNone if successful. 
        KErrNotFound if the LCS client ID has not been set.
        
*/
EXPORT_C TInt CPosGSMPrivacyRequestInfo::GetLCSClient(
    TPtrC& aLCSClientId,
    TIdType& aIdType) const
    {
    if (!iLCSClientId)
        {
        return KErrNotFound;
        }

    aLCSClientId.Set(*iLCSClientId);
    aIdType = iLCSClientIdType;
    return KErrNone;
    }


// ---------------------------------------------------------------------------
/**
Gets the Requester ID.

The Requester is the originating entity which has requested the location
of the target mobile device.

@param aRequestorId Returns a string identifying the Requester.
@param aIdType Returns the format of the Requester ID: URL, phone number, etc.
       
@return KErrNone if successful. 
        KErrNotFound if the requestor ID has not been set.
        
*/
EXPORT_C TInt CPosGSMPrivacyRequestInfo::GetRequestor(
    TPtrC& aRequestorId,
    TIdType& aIdType) const
    {
    if (!iRequestorId)
        {
        return KErrNotFound;
        }

    aRequestorId.Set(*iRequestorId);
    aIdType = iRequestorIdType;
    
    return KErrNone;
    }

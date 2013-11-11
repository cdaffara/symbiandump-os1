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



// INCLUDE FILES
#include <lbs/epos_cpossuplprivacyrequestinfo.h>
#include    "EPos_RequestInfo.h"
#include	"EPos_NetworkPrivacyPanicDeclaration.h"



/*
* CheckIdType Panics if invalid id type is passed.
*/

void CheckIdType( CPosSUPLPrivacyRequestInfo::TIdType aIdType )
    {
	TInt32 type = static_cast<TInt32>(aIdType);
	__ASSERT_ALWAYS (type >= CPosSUPLPrivacyRequestInfo::EIdTypeLogicalName &&
					 type <= CPosSUPLPrivacyRequestInfo::EIdTypeMDN,
					 Panic(EPosNetworkPrivacyInvalidSUPLIdType));
    }


/**
 * Default constructor.
 */
CPosSUPLPrivacyRequestInfo::CPosSUPLPrivacyRequestInfo()
: CPosNetworkPrivacyRequestInfo( EPosSUPLPrivacyRequestInfo )
    {
    }

/**
 * Standard ConstructL
*/
void CPosSUPLPrivacyRequestInfo::ConstructL()
    {
    }


/**
 * Standard Symbian OS two-phase constructor.
 * @return A new instance of this class.
*/
EXPORT_C CPosSUPLPrivacyRequestInfo* CPosSUPLPrivacyRequestInfo::NewLC()
    {
    CPosSUPLPrivacyRequestInfo* self = new(ELeave) CPosSUPLPrivacyRequestInfo;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

/**
 * Standard Symbian OS two-phase constructor.
 * @return A new instance of this class.
*/
EXPORT_C CPosSUPLPrivacyRequestInfo* CPosSUPLPrivacyRequestInfo::NewL()
    {
    CPosSUPLPrivacyRequestInfo* self = NewLC();
    CleanupStack::Pop( self );
    return self;
    }

/**
 * Destructor.
*/
CPosSUPLPrivacyRequestInfo::~CPosSUPLPrivacyRequestInfo()
    {
    delete iLCSClientId;
    delete iRequestorId;
    }

/**
 * Sets the LCS client.
 * @param aLCSClient IN. A string identifying the LCS client.
 * @param aIdType IN. Indicates how the LCS Client is identified, 
               e.g. by MIN, MDN,URL, phone number, etc.
 * @panic KPosNetworkPrivacyPanicCategory EPosNetworkPrivacyInvalidSUPLIdType aIdType is not a value from CPosSUPLPrivacyRequestInfo::TIdType
*/

EXPORT_C void CPosSUPLPrivacyRequestInfo::SetLCSClientL(
    const TDesC& aLCSClient,
    TIdType aIdType)
    {
    CheckIdType( aIdType );
    HBufC* newLCSClient = aLCSClient.AllocL();
    delete iLCSClientId;
    iLCSClientId = newLCSClient;
    iLCSClientIdType = aIdType;
    }

/**
 * Sets the requestor ID.
 * Requestor is the originating entity which has requested the location
 * of the target mobile station from the LCS client.
 * A requestor is identified e.g. by MIN, MDN.
 * @param aRequestorId IN. A string identifying the requestor.
 *  @param aIdType IN. Indicates how the requestor is identified, 
               e.g. by MIN, MDN,URL, phone number, etc.
 * @panic KPosNetworkPrivacyPanicCategory EPosNetworkPrivacyInvalidSUPLIdType aIdType is not a value from CPosSUPLPrivacyRequestInfo::TIdType
*/

EXPORT_C void CPosSUPLPrivacyRequestInfo::SetRequestorL(
    const TDesC& aRequestorId,
    TIdType aIdType)
    {
    CheckIdType( aIdType );
    HBufC* newRequestorId = aRequestorId.AllocL();
    delete iRequestorId;
    iRequestorId = newRequestorId;
    iRequestorIdType = aIdType;
    }


/**
 * Retrieves the LCS client.
 * @param aLCSClientId OUT. Will return a string identifying the LCS client.
 * @param aIdType OUT. Indicates how the LCS client is identified, 
               e.g. by MIN, MDN,URL, phone number, etc.
               
 * @return KErrNone if successful. 
        KErrNotFound if the LCS client ID has not been set.
*/
EXPORT_C TInt CPosSUPLPrivacyRequestInfo::GetLCSClient(
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

/**
 * Returns the requestor ID.

 * Requestor is the originating entity which has requested the location
 * of the target mobile station from the LCS client.

 * @param aRequestorId OUT. Returns a string identifying the requestor.
 * @param aIdType OUT. Indicates how the requestor is identified, 
       e.g. by URL, MIN, MDN.
       
 * @return KErrNone if successful. 
        KErrNotFound if the requestor ID has not been set.
*/
EXPORT_C TInt CPosSUPLPrivacyRequestInfo::GetRequestor(
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
    

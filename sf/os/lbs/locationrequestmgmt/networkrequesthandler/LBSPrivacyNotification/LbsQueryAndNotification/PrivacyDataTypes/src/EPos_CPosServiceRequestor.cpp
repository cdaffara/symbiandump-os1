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



#include <lbs/epos_cposservicerequestor.h>


// ---------------------------------------------------------------------------
/**
Allocates and constructs a new CPosServiceRequestor object and leaves it on the cleanup stack.

@param aIdFormat The format of the requestor ID string, such a phone number, URL, email address, etc.
@param aIdString A string which identifies the requestor.
@return A new instance of this class.
*/
EXPORT_C CPosServiceRequestor* CPosServiceRequestor::NewLC(
    TRequestorIdFormat aIdFormat,
    const TDesC& aIdString)
    {
    return CPosServiceRequestor::NewLC(aIdFormat,
    								   aIdString,
    								   ENetworkTypeUnknown,
    								   ERequestTypeUnknown);
    }

// ---------------------------------------------------------------------------
/**
Allocates and constructs a new CPosServiceRequestor object.

@param aIdFormat The format of the requestor ID string, such as a phone number, URL, email address, etc.
@param aIdString A string which identifies the requestor.
@return A new instance of this class.
*/
EXPORT_C CPosServiceRequestor* CPosServiceRequestor::NewL(
    TRequestorIdFormat aIdFormat,
    const TDesC& aIdString)
    {
    return CPosServiceRequestor::NewL(aIdFormat,
    								  aIdString,
    								  ENetworkTypeUnknown,
    								  ERequestTypeUnknown);
    }

// ---------------------------------------------------------------------------
/**
Allocates and constructs a new CPosServiceRequestor object and leaves it on the cleanup stack.

@param aIdFormat The format of the requestor ID string, such a phone number, URL, email address, etc.
@param aIdString A string which identifies the requestor.
@param aNetworkType The type of network that sent this request.
@param aRequestType The type of request this privacy request belongs to.
@return A new instance of this class.
*/
EXPORT_C CPosServiceRequestor* CPosServiceRequestor::NewLC(
    TRequestorIdFormat aIdFormat,
    const TDesC& aIdString,
    TNetworkType aNetworkType,
    TRequestType aRequestType)
    {
    CPosServiceRequestor* self = CPosServiceRequestor::NewL(
        aIdFormat, aIdString, aNetworkType, aRequestType);
    CleanupStack::PushL(self);
    return self;
    }

// ---------------------------------------------------------------------------
/**
Allocates and constructs a new CPosServiceRequestor object.

@param aIdFormat The format of the requestor ID string, such as a phone number, URL, email address, etc.
@param aIdString A string which identifies the requestor.
@param aNetworkType The type of network that sent this request.
@param aRequestType The type of request this privacy request belongs to.
@return A new instance of this class.
*/
EXPORT_C CPosServiceRequestor* CPosServiceRequestor::NewL(
    TRequestorIdFormat aIdFormat,
    const TDesC& aIdString,
    TNetworkType aNetworkType,
    TRequestType aRequestType)
    {
    HBufC* idString = aIdString.AllocLC();
    CPosServiceRequestor* self = new (ELeave) CPosServiceRequestor(
        aIdFormat, idString, aNetworkType, aRequestType);
    CleanupStack::Pop(idString);
    return self;
    }

// ---------------------------------------------------------------------------
/**
Default constructor
*/
CPosServiceRequestor::CPosServiceRequestor(
    TRequestorIdFormat aIdFormat,
    HBufC* aIdString,
    TNetworkType aNetworkType,
    TRequestType aRequestType)
    : CPosRequestor(ERequestorService, aIdFormat, aIdString, aNetworkType, aRequestType)
    {
    }

// ---------------------------------------------------------------------------
/**
Default destructor for a CPosServiceRequestor.
CPosServiceRequestor owns no resources so has nothing to cleanup.
*/
EXPORT_C CPosServiceRequestor::~CPosServiceRequestor()
    {
    }

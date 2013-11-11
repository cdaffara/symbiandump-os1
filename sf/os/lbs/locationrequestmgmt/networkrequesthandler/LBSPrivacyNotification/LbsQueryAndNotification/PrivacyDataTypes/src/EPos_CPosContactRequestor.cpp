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



#include <lbs/epos_cposcontactrequestor.h>

/**
Allocates and constructs a new CPosContactRequestor object and leaves it on the cleanup stack.

@param aIdFormat The format of the requester ID string, such as a phone number, URL, email address, etc.
@param aIdString A string which identifies the requester.
@return A new CPosContactRequestor instance.
*/
EXPORT_C CPosContactRequestor* CPosContactRequestor::NewLC(
    TRequestorIdFormat aIdFormat,
    const TDesC& aIdString)
    {
    return CPosContactRequestor::NewLC(aIdFormat, 
    								   aIdString,
    								   ENetworkTypeUnknown,
    								   ERequestTypeUnknown);
    }


/**
Allocates and constructs a new CPosContactRequestor object.

@param aIdFormat The format of the requester ID string, such as a phone number, URL, email address, etc.
@param aIdString A string which identifies the requester.
@return A new instance of this class.
*/
EXPORT_C CPosContactRequestor* CPosContactRequestor::NewL(
    TRequestorIdFormat aIdFormat,
    const TDesC& aIdString)
    {
    return CPosContactRequestor::NewL(aIdFormat, 
    								  aIdString,
    								  ENetworkTypeUnknown,
    								  ERequestTypeUnknown);
    }

/**
Allocates and constructs a new CPosContactRequestor object and leaves it on the cleanup stack.

@param aIdFormat The format of the requester ID string, such as a phone number, URL, email address, etc.
@param aIdString A string which identifies the requester.
@param aNetworkType The type of network that sent this request.
@param aRequestType The type of request this privacy request belongs to.
@return A new CPosContactRequestor instance.
*/
EXPORT_C CPosContactRequestor* CPosContactRequestor::NewLC(
    TRequestorIdFormat aIdFormat,
    const TDesC& aIdString,
    TNetworkType aNetworkType,
    TRequestType aRequestType)
    {
    CPosContactRequestor* self = CPosContactRequestor::NewL(
        aIdFormat, aIdString, aNetworkType, aRequestType);
    CleanupStack::PushL(self);
    return self;
    }


/**
Allocates and constructs a new CPosContactRequestor object.

@param aIdFormat The format of the requester ID string, such as a phone number, URL, email address, etc.
@param aIdString A string which identifies the requester.
@param aNetworkType The type of network that sent this request.
@param aRequestType The type of request this privacy request belongs to.
@return A new instance of this class.
*/
EXPORT_C CPosContactRequestor* CPosContactRequestor::NewL(
    TRequestorIdFormat aIdFormat,
    const TDesC& aIdString,
    TNetworkType aNetworkType,
    TRequestType aRequestType)
    {
    HBufC* idString = aIdString.AllocLC();
    CPosContactRequestor* self = new (ELeave) CPosContactRequestor(
        aIdFormat, idString, aNetworkType, aRequestType);
    CleanupStack::Pop(idString);
    return self;
    }

/**
Default constructor
*/
CPosContactRequestor::CPosContactRequestor(
    TRequestorIdFormat aIdFormat,
    HBufC* aIdString,
    TNetworkType aNetworkType,
    TRequestType aRequestType)
    : CPosRequestor(ERequestorContact, aIdFormat, aIdString, aNetworkType, aRequestType)
    {
    }


/**
Default destructor for a CPosContactRequestor.
CPosContactRequestor owns no resources so has nothing to cleanup.
*/
EXPORT_C CPosContactRequestor::~CPosContactRequestor()
    {
    }

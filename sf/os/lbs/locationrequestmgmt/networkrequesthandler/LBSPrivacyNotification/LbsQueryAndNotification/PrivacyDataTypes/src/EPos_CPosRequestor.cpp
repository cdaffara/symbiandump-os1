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



#include <lbs/epos_cposrequestor.h>


// ---------------------------------------------------------------------------
/**
Default constructor
*/
CPosRequestor::CPosRequestor(
    TRequestorType aRequestorType,
    TRequestorIdFormat aIdFormat,
    HBufC* aIdString,
    TNetworkType aNetworkType,
    TRequestType aRequestType)
    : 
    iRequestorType(aRequestorType), 
    iIdFormat(aIdFormat),
    iIdString(aIdString),
    iNetworkType(aNetworkType),
    iRequestType(aRequestType)
    {
    }

// ---------------------------------------------------------------------------
/**
Default destructor
*/
EXPORT_C CPosRequestor::~CPosRequestor()
    {
    delete iIdString;
    }

// ---------------------------------------------------------------------------
/**
Sets the requester id.
The requester id is a string that identifies the person or service making the privacy request.

@param aIdFormat The format of the requester id string, such as a phone number, URL, email address, etc.
@param aIdString A string that identifies the requester.
*/
EXPORT_C void CPosRequestor::SetRequestorIdL(
    TRequestorIdFormat aIdFormat,
    const TDesC& aIdString)
    {
    HBufC* idString = aIdString.AllocL();
    delete iIdString;
    iIdString = idString;
    iIdFormat = aIdFormat;
    }

// ---------------------------------------------------------------------------
/**
Gets the requester type: a contact or a service.

If the requester is a contact, it can be cast to CPosContactRequestor.
If the requester is a service, it can be cast to CPosServiceRequestor.
@return The requester type.
@see CPosContactRequestor
@see CPosServiceRequestor
*/
EXPORT_C CPosRequestor::TRequestorType CPosRequestor::RequestorType() const
    {
    return iRequestorType;
    }

// ---------------------------------------------------------------------------
/**
Gets the format of the requester ID string, such as a phone number, email address, URL etc.
@return The format of the requester ID string.
*/
EXPORT_C CPosRequestor::TRequestorIdFormat
    CPosRequestor::RequestorIdFormat() const
    {
    return iIdFormat;
    }

// ---------------------------------------------------------------------------
/**
Gets the requester ID string which identifies the contact or service making the privacy request.
@return A string identifying the requester.
*/
EXPORT_C TPtrC CPosRequestor::RequestorIdString() const
    {
    return iIdString->Des();
    }

// ---------------------------------------------------------------------------
/**
Sets the request type (single shot, periodic or area event).
This value is not currently used.
@param aRequestType The type of the request
@publishedPartner
@released
*/
EXPORT_C void CPosRequestor::SetRequestType(const TRequestType aRequestType)
	{
	iRequestType = aRequestType;
	}

// ---------------------------------------------------------------------------
/**
Gets the request type (single shot, periodic or area event).
This value is not currently used.
@ return The type of the request
@publishedPartner
@released
*/
EXPORT_C CPosRequestor::TRequestType CPosRequestor::RequestType() const
	{
	return iRequestType;
	}

// ---------------------------------------------------------------------------
/**
Sets the network type (GSM, SUPL or WCDMA).
@param aNetworkType The network type.
@publishedPartner
@released
*/
EXPORT_C void CPosRequestor::SetNetworkType(const TNetworkType aNetworkType)
	{
	iNetworkType = aNetworkType;
	}

// ---------------------------------------------------------------------------
/**
Gets the network type (GSM, SUPL or WCDMA).
@return The network type
@publishedPartner
@released
*/
EXPORT_C CPosRequestor::TNetworkType CPosRequestor::NetworkType() const
	{
	return iNetworkType;
	}

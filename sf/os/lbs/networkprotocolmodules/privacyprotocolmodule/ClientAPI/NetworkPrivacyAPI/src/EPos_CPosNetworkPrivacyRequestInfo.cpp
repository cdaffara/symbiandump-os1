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



#include    "EPos_CPosNetworkPrivacyRequestInfo.h"
#include "EPos_NetworkPrivacyPanicDeclaration.h"


// ---------------------------------------------------------------------------
/**
Default constructor
@internalComponent
*/
CPosNetworkPrivacyRequestInfo::CPosNetworkPrivacyRequestInfo(
    TUint aRequestInfoType)
    : iRequestInfoType(aRequestInfoType),
      iRequestType(ERequestTypeUnknown)
    {
    }

// ---------------------------------------------------------------------------
/**
Default destructor
*/
CPosNetworkPrivacyRequestInfo::~CPosNetworkPrivacyRequestInfo()
    {
    }

// ---------------------------------------------------------------------------
/**
Returns information of which subclass the instance is.
This function is only used internally and is therefore not exported.

@return Subclass type.
@internalComponent
*/
TUint CPosNetworkPrivacyRequestInfo::RequestInfoType() const
    {
    return iRequestInfoType;
    }

// ---------------------------------------------------------------------------
/**
Sets the type of the request.

@param aRequestType The type of the request
@panic EPosNetworkPrivacyInvalidRequestType if aRequestType is not a value from CPosNetworkPrivacyRequestInfo::_TRequestType

@released
*/
EXPORT_C void CPosNetworkPrivacyRequestInfo::SetRequestType(const TRequestType aRequestType)
	{
	TInt32 request = static_cast<TInt32>(aRequestType);

    __ASSERT_ALWAYS(request >= CPosNetworkPrivacyRequestInfo::ERequestTypeUnknown &&
    	   request < CPosNetworkPrivacyRequestInfo::ERequestTypeMaxEnum, Panic(EPosNetworkPrivacyInvalidRequestType));

	iRequestType = aRequestType;
	}

// ---------------------------------------------------------------------------
/**
Gets the type of the request.

@return The type of the request.

@released
*/
EXPORT_C CPosNetworkPrivacyRequestInfo::TRequestType CPosNetworkPrivacyRequestInfo::RequestType() const
	{
	return iRequestType;
	}

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


#include <lbs/lbsprivacyextnotifiers.h>


/**
Get the requester information contained in this class

@param aRequesterInfo Reference to a TLbsExternalRequestInfo object
                      that the requester information is written to.
*/
EXPORT_C void TLbsPrivacyNotifierParams::GetRequesterInfo(TLbsExternalRequestInfo& aRequesterInfo) const
	{
	aRequesterInfo = iRequesterInfo;
	}

/**
Set the requester information for this class

@param aRequesterInfo Reference to a TLbsExternalRequestInfo object
                      to set the requester info from.
*/	
EXPORT_C void TLbsPrivacyNotifierParams::SetRequesterInfo(const TLbsExternalRequestInfo& aRequesterInfo)
	{
	iRequesterInfo = aRequesterInfo;
	}

/**
Default C++ constructor.
*/
EXPORT_C TLbsPrivacyNotifierParams::TLbsPrivacyNotifierParams()
	{
	}

/**
Get the response type contained in this class

@param aResponse Reference to a TLbsPrivacyNotifierResponse 
                 object that the response type is written to.
*/
EXPORT_C void TLbsPrivacyResponseParams::GetResponseType(TLbsPrivacyNotifierResponse& aResponse) const
	{
	aResponse = iResponse;
	}

/**
Set the response type for this class

@param aResponse Reference to a TLbsPrivacyNotifierResponse
                 object to set the response type from.
*/
EXPORT_C void TLbsPrivacyResponseParams::SetResponseType(const TLbsPrivacyNotifierResponse& aResponse)
	{
	iResponse = aResponse;
	}

/**
Default C++ constructor.
*/
EXPORT_C TLbsPrivacyResponseParams::TLbsPrivacyResponseParams()
	{
	}


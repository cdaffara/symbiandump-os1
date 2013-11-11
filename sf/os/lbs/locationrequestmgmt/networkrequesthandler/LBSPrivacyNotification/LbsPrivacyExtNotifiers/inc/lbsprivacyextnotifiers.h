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

/**
 @file
 @publishedPartner
 @released
*/

#ifndef LBSPRIVACYEXTNOTIFIERS_H
#define LBSPRIVACYEXTNOTIFIERS_H

#include <lbs/lbsloccommon.h>

/** 
Defines a TUint for the Uid of the dialog used to notify the user that an 
external location request has been received. The user has
no choice over whether to accept or reject the request.

@publishedPartner
@released
*/
const TUint32 KLbsExtLocationRequestNotifyUidValue = 0x1020E195;

/** 
The Uid of the dialog used to notify the user that an 
external location request has been received. The user has
no choice over whether to accept or reject the request.

@publishedPartner
@released
*/
const TUid KLbsExtLocationRequestNotifyUid = {KLbsExtLocationRequestNotifyUidValue};

/** 
Defines a TUint for the Uid of the dialog used to ask the user to verify whether 
they want to accept or reject an an external location. 

@publishedPartner
@released
*/
const TUint32 KLbsExtLocationRequestVerifyUidValue = 0x1020E196;

/** 
The Uid of the dialog used to ask the user to verify whether 
they want to accept or reject an an external location. 

@publishedPartner
@released
*/
const TUid KLbsExtLocationRequestVerifyUid = {KLbsExtLocationRequestVerifyUidValue};

/**
Container for the parameters that are sent to the Lbs
privacy dialogs. Both dialogs receive this information.

@publishedPartner
@released
*/
NONSHARABLE_CLASS(TLbsPrivacyNotifierParams)
	{
public:
	IMPORT_C TLbsPrivacyNotifierParams();

	IMPORT_C void GetRequesterInfo(TLbsExternalRequestInfo& aRequesterInfo) const;
	
	IMPORT_C void SetRequesterInfo(const TLbsExternalRequestInfo& aRequesterInfo);
	
private:
	/** Information that identifies the external client that
	    made the location request. */
	TLbsExternalRequestInfo iRequesterInfo;

	/** Reserved for future use to help maintain BC. */
	TUint8 iReserved[16];
	};

/**
A typedef to wrap a TLbsPrivacyNotifierParams in a TPckgBuf. 
Use this type to send the notifier parameters to a dialog 
via the RNotifier API.

Both the LBS privacy notification and verification dialogs 
require this information.

@publishedPartner
@released
*/
typedef TPckgBuf<TLbsPrivacyNotifierParams> TLbsPrivacyNotifierParamsPckg;	

/**
Contains the user response to the privacy verification dialog.

@publishedPartner
@released
*/
enum TLbsPrivacyNotifierResponse
	{
	/** Unknown/Unused response. */
	EResponseUnknown = 0,
	/** User accepted the external location request. */
   	EResponseAccepted,
   	/** User rejected the external location request. */
   	EResponseRejected,
   	/** The dialog timed out before the user responded. */
   	EResponseTimedOut
   	};

/**
Container for the parameters that are returned from the Lbs
privacy verification dialog.

@publishedPartner
@released
*/
NONSHARABLE_CLASS(TLbsPrivacyResponseParams)
	{
public:
	IMPORT_C TLbsPrivacyResponseParams();
	
	IMPORT_C void GetResponseType(TLbsPrivacyNotifierResponse& aResponse) const;
	
	IMPORT_C void SetResponseType(const TLbsPrivacyNotifierResponse& aResponse);
	
private:
	/** The users' response to the verification dialog. */
	TLbsPrivacyNotifierResponse iResponse;

	/** Reserved for future use to help maintain BC. */
	TUint8 iReserved[16];
	};

/**
A typedef to wrap a TLbsPrivacyResponseParams in a TPckgBuf. 
Use this type to retrieve the result paramaters from a dialog 
via the RNotifier API.

Only the LBS privacy verification dialog returns this information.

@publishedPartner
@released
*/
typedef TPckgBuf<TLbsPrivacyResponseParams> TLbsPrivacyResponseParamsPckg;


#endif // LBSPRIVACYEXTNOTIFIERS_H

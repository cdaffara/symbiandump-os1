//lbsprivacyrequestimpl.h

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

#ifndef LBS_PRIVACYREQUESTIMPL_H
#define LBS_PRIVACYREQUESTIMPL_H

// System 
#include <e32base.h>
#include <e32cmn.h>
#include "lbsprivacyclient.h"
#include "lbsprivacyrequest.h"

// FORWARD DECLARATIONS
class CPrivacyRequestResponseHandler;
class MLbsPrivacyRequestObserver;

NONSHARABLE_CLASS(CLbsPrivacyRequestImpl) :  public CBase
	{
           
public:
	static CLbsPrivacyRequestImpl* NewL(MLbsPrivacyRequestObserver& aObserver /*in*/);
	virtual ~CLbsPrivacyRequestImpl();

public:
	TInt NewPrivacyRequest(TLbsPrivacyRequestId& aRequestId, /*out*/
								 const TLbsPrivacyRequest& aPrivacy,/*in*/
								 const TLbsExternalRequestInfo& aRequestInfo/*in*/);
	
	TInt RepeatPrivacyRequest(const TLbsPrivacyRequestId aRequestId, /*in*/
								 const TLbsPrivacyRequest& aPrivacy,/*in*/
								 const TLbsExternalRequestInfo& aRequestInfo/*in*/);

	TInt CompleteRequest(TLbsPrivacyRequestId aRequestId/*in*/,
								  TInt aReason/*in*/);
	
public:
	void RemoveHandler(CPrivacyRequestResponseHandler* aHandler);
	
private:
	CLbsPrivacyRequestImpl(MLbsPrivacyRequestObserver& aObserver);
	void ConstructL();
	// By default, prohibit copy constructor
	CLbsPrivacyRequestImpl(const CLbsPrivacyRequestImpl&);
	// Prohibit assigment operator
	CLbsPrivacyRequestImpl& operator= (const CLbsPrivacyRequestImpl&);

	TInt ValidateNewPrivReqArguments(const TLbsPrivacyRequest& aPrivacy, const TLbsExternalRequestInfo& aRequestInfo);
	TInt ValidateRepeatPrivReqArguments(const TLbsPrivacyRequestId aRequestId, const TLbsPrivacyRequest& aPrivacy, const TLbsExternalRequestInfo& aRequestInfo);
		                        
private:
	MLbsPrivacyRequestObserver& iObserver;
	
	RPrivacyServer iServer;
	RPointerArray<CPrivacyRequestResponseHandler> iResponseHandler;
    };

#endif      // LBS_PRIVACYREQUESTIMPL_H

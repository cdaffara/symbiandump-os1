/**
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/



/**
 @file ctlbsprivfwverifyrequest.h
*/
#ifndef __CT_LBS_PRIVFW_VERIFY_REQUEST_H__
#define __CT_LBS_PRIVFW_VERIFY_REQUEST_H__

#include <lbs/test/lbsprivfwdefs.h>
#include "ctlbsprivfwbaserequest.h"
#include "ctlbsprivfwapiwrapper.h"

#include "epos_cposnetworkprivacy.h"
#include "epos_cposgsmprivacyrequestinfo.h"


class CT_LbsPrivFwVerifyRequest: public CT_LbsPrivFwBaseRequest
{
	public:
		enum TCurReqState
		{	
			EReqSubmitted = 0, // or something like this
			EReqResubmitted,
			EReqAccepted,
			EReqRejected,
			EReqValidated = EReqEndOfRange + 1,  // Terminal
			EReqCtrlFinal, // Terminal for Ctrl
			EReqFailed // Terminal for all
		};

    public:
    	static CT_LbsPrivFwVerifyRequest* NewL(CPrivFwProxy* aProxy, 
    										CPrivFwApiWrapper* aNetPrivacy, 
							    			CT_LbsPrivFwApiDataObserver* aPrivacyObserver, 
							    			TPrivacyHandlerType aPrivacyHandler, 
							    			CT_LbsPrivFwStepBase* aStep);
    	~CT_LbsPrivFwVerifyRequest();
    	virtual void Start();
    	void SetDecision(CPosNetworkPrivacy::TRequestDecision aDecision);
    	
    	virtual TBool IsRequestComplete();
    	
    	// Call backs from AOs
	    virtual void ProcessPrivApiData(TInt aRequestId, TInt aCompletionCode);
	    virtual void ProcessPrivNotifierData(TInt aRequestId, TLbsTestRequestInfo& aPrivNotifierData);
	    virtual void ProcessPrivRequestComplete(TInt aRequestId, TInt aCompletionCode);
	    
	    void SetDesiredResponse(TResponseType aResponseType);


    protected:
    	void VerifyLocationRequest(CPosNetworkPrivacy::TRequestDecision aDecision);
    	void ConstructL(CPrivFwProxy* aProxy, 
    					CPrivFwApiWrapper* aNetPrivacy, 
    					CT_LbsPrivFwApiDataObserver* aPrivacyObserver, 
    					TPrivacyHandlerType aPrivacyHandler, 
    					CT_LbsPrivFwStepBase* aStep);
    	 CT_LbsPrivFwVerifyRequest();
    	 
    protected:		
    	TCurReqState iState;				// Override base state
    	TResponseType iDesiredResponse;		// Renamed from iResponseType due to ambiguity
    	CPosNetworkPrivacy::TRequestDecision iDecision;
    	 
};

#endif // __CT_LBS_PRIVFW_VERIFY_REQUEST_H__

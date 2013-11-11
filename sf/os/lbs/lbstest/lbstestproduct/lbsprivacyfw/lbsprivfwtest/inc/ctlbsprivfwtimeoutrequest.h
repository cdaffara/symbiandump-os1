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
 @file ctlbsprivfwtimeoutrequest.h
*/
#ifndef __CT_LBS_PRIVFW_TIMEOUT_REQUEST_H__
#define __CT_LBS_PRIVFW_TIMEOUT_REQUEST_H__

#include <lbs/test/lbsprivfwdefs.h>
#include "ctlbsprivfwbaserequest.h"
#include "epos_cposnetworkprivacy.h"
#include "epos_cposgsmprivacyrequestinfo.h"


class CT_LbsPrivFwTimeoutRequest: public CT_LbsPrivFwBaseRequest
{
	public:
		enum TCurReqState
		{	
			EReqSubmitted = 0, // or something like this
			EReqResubmitted,
			EReqAccepted,
			EReqRejected,
			EReqTestFailed,
			EReqValidated = EReqEndOfRange + 1,  // Terminal
			EReqCtrlFinal // Terminal for Ctrl
		};


		enum TReqBehaviourType
		{
			EReqDefault = 0,
			EReqRespondBeforeComplete,
			EReqRespondAfterComplete,
			EReqInvalidTimeoutId,
			EReqSendTimeoutError
		};
		
    public:
    	static CT_LbsPrivFwTimeoutRequest* NewL(CPrivFwProxy* aProxy, 
    										CPrivFwApiWrapper* aNetPrivacy, 
							    			CT_LbsPrivFwApiDataObserver* aPrivacyObserver, 
							    			TPrivacyHandlerType aPrivacyHandler, 
							    			CT_LbsPrivFwStepBase* aStep);
    	~CT_LbsPrivFwTimeoutRequest();
    	virtual void Start();
    	void SetDecision(CPosNetworkPrivacy::TRequestDecision aDecision);
    	
    	virtual TBool IsRequestComplete();
    	
    	// Call backs from AOs
	    virtual void ProcessPrivApiData(TInt aRequestId, TInt aCompletionCode);
	    virtual void ProcessPrivNotifierData(TInt aRequestId, TLbsTestRequestInfo& aPrivNotifierData);
	    virtual void ProcessPrivRequestComplete(TInt aRequestId, TInt aCompletionCode);
	    
	    void SetDesiredResponse(TResponseType aResponseType);
		void SetNetworkTimeout(TUint aNetworkTimeout);
		void SetBehaviourType(TReqBehaviourType aBehaviourType);
	    void SetRequestId(TInt aRequestId); 
//		void SetCancelReason(CPosNetworkPrivacy::TCancelReason aCancelReason);


    protected:
    	void VerifyLocationRequest(CPosNetworkPrivacy::TRequestDecision aDecision);
    	void ConstructL(CPrivFwProxy* aProxy, 
    					CPrivFwApiWrapper* aNetPrivacy, 
    					CT_LbsPrivFwApiDataObserver* aPrivacyObserver, 
    					TPrivacyHandlerType aPrivacyHandler, 
    					CT_LbsPrivFwStepBase* aStep);
    	 CT_LbsPrivFwTimeoutRequest();
    	 
    protected:
    	CPosNetworkPrivacy::TCancelReason iCancelReason;
    	TCurReqState iState;				// Override base state
    	TReqBehaviourType	iBehaviourType;
    	TResponseType iDesiredResponse;		// Renamed from iResponseType due to ambiguity
    	CPosNetworkPrivacy::TRequestDecision iDecision;
    	TUint	iNetworkTimeout;
    	 
};

#endif // __CT_LBS_PRIVFW_CANCEL_REQUEST_H__

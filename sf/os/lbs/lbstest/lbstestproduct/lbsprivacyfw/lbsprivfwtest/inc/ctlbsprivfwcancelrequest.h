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
 @file ctlbsprivfwcancelrequest.h
*/
#ifndef __CT_LBS_PRIVFW_CANCEL_REQUEST_H__
#define __CT_LBS_PRIVFW_CANCEL_REQUEST_H__

#include <lbs/test/lbsprivfwdefs.h>
#include "ctlbsprivfwbaserequest.h"
#include "ctlbsprivfwapiwrapper.h"

#include "epos_cposnetworkprivacy.h"
#include "epos_cposgsmprivacyrequestinfo.h"


class CT_LbsPrivFwCancelRequest: public CT_LbsPrivFwBaseRequest
{
	public:
		enum TCurReqState
		{	
			EReqSubmitted = 0, // or something like this
			EReqResubmitted,
			EReqAccepted,
			EReqRejected,
			EReqCancelled,
			EReqValidated = EReqEndOfRange + 1,  // Terminal
			EReqCtrlFinal, // Terminal for Ctrl
			EReqFailed // Terminal for all
		};

		enum TReqCancelBehaviour
		{
			EReqCancelNone = 0,
			EReqCancelBeforeStart,
			EReqCancelBeforeDialog,
			EReqCancelAfterDialog,
			EReqCamcelAfterUserResponse,
			EReqCancelAfterComplete,
			EReqCancelInvalidRequestId,
			EReqCancelNotifyAfterDialog,
			EReqCancelDone
		};
		
    public:
    	static CT_LbsPrivFwCancelRequest* NewL(CPrivFwProxy* aProxy, 
    										CPrivFwApiWrapper* aNetPrivacy, 
							    			CT_LbsPrivFwApiDataObserver* aPrivacyObserver, 
							    			TPrivacyHandlerType aPrivacyHandler, 
							    			CT_LbsPrivFwStepBase* aStep);
    	~CT_LbsPrivFwCancelRequest();
    	virtual void Start();
    	
    	virtual TBool IsRequestComplete();
    	
    	// Call backs from AOs
	    virtual void ProcessPrivApiData(TInt aRequestId, TInt aCompletionCode);
	    virtual void ProcessPrivNotifierData(TInt aRequestId, TLbsTestRequestInfo& aPrivNotifierData);
	    virtual void ProcessPrivRequestComplete(TInt aRequestId, TInt aCompletionCode);
	    
    	void SetDecision(CPosNetworkPrivacy::TRequestDecision aDecision);
	    void SetDesiredResponse(TResponseType aResponseType);
		void SetRequestType(TRequestType aRequestType);
		void SetCancelBehaviour(TReqCancelBehaviour aCancelBehaviour);
		void SetCancelReason(CPosNetworkPrivacy::TCancelReason aCancelReason);


    protected:
    	void VerifyLocationRequest(CPosNetworkPrivacy::TRequestDecision aDecision);
    	void ConstructL(CPrivFwProxy* aProxy, 
    					CPrivFwApiWrapper* aNetPrivacy, 
    					CT_LbsPrivFwApiDataObserver* aPrivacyObserver, 
    					TPrivacyHandlerType aPrivacyHandler, 
    					CT_LbsPrivFwStepBase* aStep);
    	 CT_LbsPrivFwCancelRequest();
    	 
    private:
    	void SetNotifierResponse(TInt aRequestId, TUint aResponseDelay);
    	
    protected:		
    	TCurReqState iState;				// Override base state
    	TReqCancelBehaviour	iCancelBehaviour;
    	TResponseType iDesiredResponse;		// Renamed from iResponseType due to ambiguity
    	CPosNetworkPrivacy::TRequestDecision iDecision;
    	TUint	iNetworkTimeout;
    	CPosNetworkPrivacy::TCancelReason iCancelReason;
    	TRequestType	iRequestType;
    	
    	 
};

#endif // __CT_LBS_PRIVFW_CANCEL_REQUEST_H__

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
 @file ctlbsprivfwvnotrequestwithnonotifier.h
*/
#ifndef __CT_LBS_PRIVFW_NOT_REQUEST_WITH_NO_NOTIFIER_H__
#define __CT_LBS_PRIVFW_NOT_REQUEST_WITH_NO_NOTIFIER_H__

#include <lbs/test/lbsprivfwdefs.h>
#include "ctlbsprivfwbaserequest.h"
#include "ctlbsprivfwapiwrapper.h"

#include "epos_cposnetworkprivacy.h"
#include "epos_cposgsmprivacyrequestinfo.h"


class CT_LbsPrivFwNotRequestWithNoNotifier: public CT_LbsPrivFwBaseRequest
{

	public:
		enum TCurReqState
		{	
			EReqSubmitted = 0, //
			EReqReceived,
			EReqCompleted = EReqEndOfRange + 1, // Terminal
			EReqCtrlFinal, // Terminal for controller
			EReqFailed // Terminal for all
		};

    public:
    	static CT_LbsPrivFwNotRequestWithNoNotifier* NewL(CPrivFwProxy* aProxy, 
    										CPrivFwApiWrapper* aNetPrivacy, 
							    			CT_LbsPrivFwApiDataObserver* aPrivacyObserver, 
							    			TPrivacyHandlerType aPrivacyHandler, 
							    			CT_LbsPrivFwStepBase* aStep);
    	~CT_LbsPrivFwNotRequestWithNoNotifier();
    	virtual void Start();
    	    	
    	virtual TBool IsRequestComplete();
    	
    	// Call backs from AOs
	    virtual void ProcessPrivApiData(TInt aRequestId, TInt aCompletionCode);
	    virtual void ProcessPrivNotifierData(TInt aRequestId, TLbsTestRequestInfo& aPrivNotifierData);
	    virtual void ProcessPrivRequestComplete(TInt aRequestId, TInt aCompletionCode);


    protected:
    	void VerifyLocationRequest(CPosNetworkPrivacy::TRequestDecision aDecision);
    	void ConstructL(CPrivFwProxy* aProxy, 
    					CPrivFwApiWrapper* aNetPrivacy, 
    					CT_LbsPrivFwApiDataObserver* aPrivacyObserver, 
    					TPrivacyHandlerType aPrivacyHandler, 
    					CT_LbsPrivFwStepBase* aStep);
    	 CT_LbsPrivFwNotRequestWithNoNotifier();
    	 
    	 
    protected:		
    	TCurReqState iState;				// Override base state
  	 
};

#endif // __CT_LBS_PRIVFW_NOT_REQUEST_WITH_NO_NOTIFIER_H__

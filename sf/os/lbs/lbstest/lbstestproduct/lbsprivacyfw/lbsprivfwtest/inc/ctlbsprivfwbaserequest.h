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
 @file ctlbsprivfwrequestinfo.h
*/
#ifndef __CT_LBS_PRIVFW_REQUEST_INFO_H__
#define __CT_LBS_PRIVFW_REQUEST_INFO_H__

#include <test/testexecutestepbase.h>

#include "ctlbsprivfwapidataobserver.h"
#include <lbs/test/lbsprivfwdefs.h>
#include <lbs/test/lbsprivfwproxy.h>
#include "ctlbsprivfwapiwrapper.h"

#include <lbs/lbsloccommon.h>
#include "epos_cposnetworkprivacy.h"
#include "epos_cposgsmprivacyrequestinfo.h"
#include "epos_cpossuplprivacyrequestinfo.h"

#include <lbs/epos_momasuplobserver.h>
#include <lbs/epos_comasuplposhandlerbase.h>
#include <lbs/epos_comasuplpossessionbase.h>

class CTestStep;
class CT_LbsPrivFwStepBase;

class TRequestTestData
{
// Expected answer, cancel reason, etc
};


class CT_LbsPrivFwBaseRequest: public CBase, MOMASuplObserver
{
	public:
		 
		enum TCurReqState
			{	
			EReqInitial = 0,
			EReqEndOfRange = 1000
			};

	
    public:
	    ~CT_LbsPrivFwBaseRequest();
	    virtual void Start() = 0;
	    
	    //Setters
	    void SetRequestInfo(CPosNetworkPrivacyRequestInfo* aRequestInfo, TPosNetworkPrivacyRequestInfoType aPrivacyInfoType);
	    void SetRequestId(TInt aRequestId);
	    void SetResponseDelay(TTimeIntervalMicroSeconds aResponseDelay);
	    
	    // Getters
	    TInt RequestId();
	    CPosNetworkPrivacyRequestInfo& RequestInfo();
	    TPosNetworkPrivacyRequestInfoType RequestInfoType();
	    TRequestType RequestType();
	    TCurReqState State();
	    CTestExecuteLogger& Logger();

	    virtual TBool IsRequestComplete();

	    // Data processing functions
	    virtual void ProcessPrivApiData(TInt aRequestId, TInt aCompletionCode);

	    virtual void ProcessPrivNotifierData(TInt aRequestId, TLbsTestRequestInfo& aPrivNotifierData);
	    virtual void ProcessPrivRequestComplete(TInt aRequestId, TInt aCompletionCode);
		virtual void ProcessPrivNotiferCtrlCloseDown(); 
		
		virtual void SetResponseDelay(TUint aResponseDelay);
	
		// from MOMASuplObserver
		TReal SuplVersion();
		void TerminateSession();

    protected:
	    virtual void VerifyPrivNotifierData(CPosNetworkPrivacyRequestInfo* aNetPrivReqInfo, 
				const TLbsTestRequestInfo& aExtReqInfo);

    	bool CompareRequestInfoType(TInt aExternalType, TInt aCposType);
	    CT_LbsPrivFwBaseRequest();
	    void ConstructL(CPrivFwProxy* aProxy, CPrivFwApiWrapper* aNetPrivacy, 
	    				CT_LbsPrivFwApiDataObserver* aApiObserver, 
	    				TPrivacyHandlerType aPrivacyHandler, CT_LbsPrivFwStepBase* aStep);
	    void TerminateSessionL(TInt aRequestId);
	    
    protected:
	    TInt iRequestId;              	   	//
	    TCurReqState iState;        		// State of this Request, Replaces iCompletionCode
	    CPosNetworkPrivacyRequestInfo* iRequestInfo; // Can be a GSM Request or a SUPL request
	    TPosNetworkPrivacyRequestInfoType iRequestInfoType;		// Whether iRequestInfo is Gsm or Supl
	    TLbsTestRequestInfo iNotifierInfo;	// local copy of data sent to notifier
	    
	    CPrivFwApiWrapper* iNetPrivacy;	// Reference to API that made request NOT OWNED
	    CPrivFwProxy* iProxy; 				// Reference to proxy NOT OWNED
	    CT_LbsPrivFwApiDataObserver* iApiObserver; //iPrivacyObserver renamed to  iApiObserver Observer used when submitting requests to privacy API
	   

	    
    	TTimeIntervalMicroSeconds	iResponseDelay;
	    TPrivacyHandlerType iPrivacyHandler; // Type of handler used for this request: QnN, Notifier or Controller
		
	    CT_LbsPrivFwStepBase* iStep; 					// Allows us to use TEF Log and Test step failing

	    
	    
	    /* The Following Should be moved into a Cancel Request Object when one is made
	    TCancelType iCancel;				// Type of Cancel MOVE
	    TUint iCancelId;					// Request ID to cancel if TCancelType > 0
	    CPosNetworkPrivacy::TCancelReason iCancelReason; // Reason to give if E*CancelWithReason
	    CPosNetworkPrivacy::TRequestDecision iCancelDecision;	// Decision to make in NotifyVerificationTimeoutL
	    */
		
	    //		enum TCancelType
	    //		{
	    //			ENoCancel,
	    //			ENrhCancel,
	    //			ENrhCancelWithReason,
	    //			ENrhTimeoutCancel,
	    //			EPrivacyCancel,
	    //			EPrivacyCancelWithReason,
	    //			EPrivacyTimeoutCancel
	    //		};
	    /* This Should also be moved to a derived class
	    TUint iResponseDelay;				// How long proxy should wait before servicing request MOVE
	    */
};

#endif // __CT_LBS_PRIVFW_REQUEST_INFO_H__

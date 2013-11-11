/**
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file ctlbsprivfwreqsubmitter.h
*/
#ifndef __CT_LBS_PRIVFW_REQ_SUBMITTER_H__
#define __CT_LBS_PRIVFW_REQ_SUBMITTER_H__

#include <lbs/test/lbsprivfwdefs.h>

#include "ctlbsprivfwstepbase.h"
#include "ctlbsprivfwapiwrapper.h"

#include <lbs/lbsloccommon.h>
#include "epos_cposnetworkprivacy.h"
#include "epos_cposgsmprivacyrequestinfo.h"
#include "epos_cpossuplprivacyrequestinfo.h"


class CT_LbsPrivFwReqSubmitter: public CActive
{
    public:
    	// destuctor
	    ~CT_LbsPrivFwReqSubmitter();
	    
	    // constructor
		static CT_LbsPrivFwReqSubmitter* NewL(CPrivFwProxy* aProxy, CPrivFwApiWrapper* aNetPrivacy, 
				CT_LbsPrivFwApiDataObserver* aApiObserver, 
				TPrivacyHandlerType aPrivacyHandler, CT_LbsPrivFwStepBase* aStep,
				RPointerArray<CT_LbsPrivFwBaseRequest>& aReqArray);
		
		void StartL(TInt aRequestCount);

	    TTime StartTime();
	    CTestExecuteLogger& Logger();
	    
    protected:
		// Redeclares the base class CActive::DoCancel(). 
		virtual void DoCancel();
		// Redeclares the base class CActive::RunL(). 
		virtual void RunL();
		// Redeclares the base class CActive::RunError(). 
		virtual TInt RunError(TInt aError);
		
		CT_LbsPrivFwReqSubmitter(RPointerArray<CT_LbsPrivFwBaseRequest>& aReqArray);
	    void ConstructL(CPrivFwProxy* aProxy, CPrivFwApiWrapper* aNetPrivacy, 
	    				CT_LbsPrivFwApiDataObserver* aApiObserver, 
	    				TPrivacyHandlerType aPrivacyHandler, CT_LbsPrivFwStepBase* aStep);
	    void SelfComplete();
    
	    
    private:
    	TInt iRequestCount;	
    	TInt iCurrentRequest;
    	TTime	iStartTime;
	    
    	CPrivFwApiWrapper* iNetPrivacy;	// Reference to API that made request NOT OWNED
	    CPrivFwProxy* iProxy; 				// Reference to proxy NOT OWNED
	    CT_LbsPrivFwApiDataObserver* iApiObserver; // Reference to observer used when submitting requests to privacy API NOT OWNED
	    CT_LbsPrivFwStepBase* iStep; 		// Reference to parent step  NOT OWNED
	    TPrivacyHandlerType iPrivacyHandler;
	    RPointerArray<CT_LbsPrivFwBaseRequest>& iReqArray; // Reference to array of requests NOT OWNED
	   
	    RPointerArray<CPosGSMPrivacyRequestInfo> iReqInfoArray;	  
};

#endif // __CT_LBS_PRIVFW_REQ_SUBMITTER_H__

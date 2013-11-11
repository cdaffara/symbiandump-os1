//lbsnetworkprivacyimpl.h

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
 @internalTechnology
*/

#ifndef LBS_NETWORKPRIVACYIMPL_H
#define LBS_NETWORKPRIVACYIMPL_H

// System 
#include <e32base.h>
#include <e32cmn.h>

//Lbs 
#include "lbstimer.h"
#include <lbs/lbsloccommon.h>
#include <lbs/lbsnetcommon.h>

//Component
#include <lbs/epos_cposnetworkprivacy.h>
#include <lbs/epos_cposgsmprivacyrequestinfo.h>
#include <lbs/epos_cpossuplprivacyrequestinfo.h>
#include "lbsprivacyclient.h"
#include "EPos_RequestInfo.h" 
#include <lbs/epos_mposverificationobserver.h>

//Default connection timeout
const TInt KDefaultConnectionTimeout = 30000000; //30s

// FORWARD DECLARATIONS
class CPosNetworkPrivacyRequestInfo;
class CNetworkPrivacyVerifyHandler;


NONSHARABLE_CLASS(CPosNetworkPrivacyImpl) :  public CBase,
                                             public MLbsCallbackTimerObserver,
                                             public MPosVerificationObserver
	{
           
    public:
        static CPosNetworkPrivacyImpl* NewL();
        virtual ~CPosNetworkPrivacyImpl();
    
    public:	
    	// From MLbsCallbackTimerObserver
    	
    	/** Called when the timer expires */
	    virtual void OnTimerEventL(TInt aTimerId); 
	    /** Called if OnTimerEventL leaves */
	    virtual TInt OnTimerError(TInt aTimerId, TInt aError);
    
	    /** From MPosVerificationObserver
	    *Called when a call to VerifyLocationRequestL completes, 
	    */
	    virtual void HandleVerifyComplete(TInt aRequestId, TInt aCompletionCode);
       
    public:
        void SetConnectionTimeout(TTimeIntervalMicroSeconds32 aTimeout);
        
        TTimeIntervalMicroSeconds32 ConnectionTimeout() const;
        
        void VerifyLocationRequestL(
        				const CPosNetworkPrivacyRequestInfo& aRequestInfo,
        				TInt& aRequestId,
        				MPosVerificationObserver& aObserver,
        			CPosNetworkPrivacy::TRequestDecision aTimeoutStrategy);

        void CancelVerifyLocationRequest(TInt aRequestId,CPosNetworkPrivacy::TCancelReason aCancelReason);

        void NotifyLocationRequestL(
        				const CPosNetworkPrivacyRequestInfo& aRequestInfo,
        				TInt& aRequestId);

        void NotifyVerificationTimeoutL(
        				const CPosNetworkPrivacyRequestInfo& aRequestInfo,
        				TInt aRequestId,
        			CPosNetworkPrivacy::TRequestDecision aRequestDecision);

    private:
		CPosNetworkPrivacyImpl();
		void ConstructL();
		// By default, prohibit copy constructor
		CPosNetworkPrivacyImpl(const CPosNetworkPrivacyImpl&);
		// Prohibit assigment operator
		CPosNetworkPrivacyImpl& operator= (const CPosNetworkPrivacyImpl&);
		  
    private:            
                
		void CancelConnectionTimer();
		
		void StartConnectionTimer(); 
		    
		void ExtractRequestInfoL(
		    		          const CPosNetworkPrivacyRequestInfo& aRequestInfo,
		    	              TLbsExternalRequestInfo2& aExternalRequestInfo2);
		   
		
		void ExtractRequestTypeL(
		    		          const CPosNetworkPrivacyRequestInfo& aRequestInfo,
		    	              TLbsExternalRequestInfo2& aExternalRequestInfo2);
		
		void ExtractGSMRequestInfoL(
		    		          const CPosNetworkPrivacyRequestInfo& aRequestInfo,
		                      TLbsExternalRequestInfo2& aExternalRequestInfo2);
		        
		void ConvertGSMRequestInfoL(CPosNetworkPrivacy::TRequestorType aRequestorType, 
		        		      CPosGSMPrivacyRequestInfo::TIdType aIdType,
		                      const TDesC& aId,
		                      TLbsExternalRequestInfo2& aExternalRequestInfo2);
		    
		void ExtractSUPLRequestInfoL(
				          const CPosNetworkPrivacyRequestInfo& aRequestInfo,
		                TLbsExternalRequestInfo2& aExternalRequestInfo2);
		
		
		void ConvertSUPLRequestInfoL(CPosNetworkPrivacy::TRequestorType aRequestorType, 
		                CPosSUPLPrivacyRequestInfo::TIdType aIdType,
		                const TDesC& aId,
		                TLbsExternalRequestInfo2& aExternalRequestInfo2);
		                
		TLbsNetPosRequestPrivacy::TLbsRequestAction ConvertInternalRequestType(
				                                    CPosNetworkPrivacy::TRequestDecision aRequestDecision);
		
		void CheckOutstandingAndStartTimer();
		void ConnectToServerL();
		void GetDefaultConnectionTimeout();
		        
		                        
    private:
	TTimeIntervalMicroSeconds32 iConnectionTimeout;
	TBool iConnectionTimeoutIsSet;
	CLbsCallbackTimer*   iConnectionTimer;
	TLbsExternalRequestInfo2 iExternalRequestInfo2;
	RPrivacyServer iServer;        
	TBool iServerConnected;
	RPointerArray<CNetworkPrivacyVerifyHandler> iVerifyHandler;
    };

#endif      // lbsnetworkprivacyimpl.h

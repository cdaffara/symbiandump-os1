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
 @file verificationtimeoutstep.cpp
*/

#include "multipleverifylocationrequeststep.h"
#include "te_lbsnetworkprivacysuitedefs.h"
#include <lbs/lbsnetprotocolbase.h>
#include <lbs/lbsadmin.h>
#include <lbs/epos_cposgsmprivacyrequestinfo.h>
#include "lbsnrhngmsgs.h"

/**
 * Constructor
 */
CMultipleVerifyLocationRequestStep::CMultipleVerifyLocationRequestStep()
	{
	SetTestStepName(KMultipleVerifyLocationRequestStep);
	}

/**
 * Destructor
 */
CMultipleVerifyLocationRequestStep::~CMultipleVerifyLocationRequestStep()
	{
	
	}


TVerdict CMultipleVerifyLocationRequestStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	CLbsAdmin* adminApi = CLbsAdmin::NewL();
	CleanupStack::PushL(adminApi);
	
	// Set the maximum number of simultaneous privacy requests allowed
	const TUint KMaxExternalLocateRequests(2);
	TInt err = KErrNone;
	err = adminApi->Set(KLbsSettingMaximumExternalLocateRequests, KMaxExternalLocateRequests);
	User::LeaveIfError(err);

	CleanupStack::PopAndDestroy(adminApi);

	
	
	// Call base class method for pre test actions	
	CTe_LbsNetworkPrivacySuiteStepBase::doTestStepPreambleL();
			
	// Connect to the Channel
	iGatewayChannel = CGatewayChannel::NewL(*this, RLbsNetChannel::EChannelNRH2NG);
			
	return TestStepResult();
	}


/** Perform CNotifyLocationRequestStep test step.
 * This test verifies that the  Privacy Protocol Module correctly handles 
 * NotifyLocationRequest 

 * @return TVerdict test result code
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
*/
TVerdict CMultipleVerifyLocationRequestStep::doTestStepL()

	{
	  if (TestStepResult()==EPass)
		{
		// Dummy gateway message satus
	    iNGMessage = ENGMessageWaiting;
	    
	    CPosGSMPrivacyRequestInfo* reqInfo = CPosGSMPrivacyRequestInfo::NewL();
	    
	    CleanupStack::PushL(reqInfo);
	    	    
	    reqInfo->SetRequestorL(KTestRequestorName, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
         
        TPtrC requestorId;
        
        CPosGSMPrivacyRequestInfo::TIdType aIdType; 
               
        // Verify that the requestor id is what we expect
        User::LeaveIfError(reqInfo->GetRequestor(requestorId,aIdType));          
                
        if(requestorId != KTestRequestorName)
	        {
	        INFO_PRINTF1(_L("** Requestor Id is wrong **"));
	        SetTestStepResult(EFail);	
	        }
	             
        TInt reqId = KErrNone;
        //Send first verification request
		iPosNetworkPrivacy->VerifyLocationRequestL(*reqInfo,reqId,*this,CPosNetworkPrivacy::EDecisionAccepted);
				
		INFO_PRINTF1(_L("First VerifyLocationRequest Sent"));
		
	    //Send second verification request
		iPosNetworkPrivacy->VerifyLocationRequestL(*reqInfo,reqId,*this,CPosNetworkPrivacy::EDecisionAccepted);
				
		INFO_PRINTF1(_L(" Second VerifyLocationRequest Sent"));
			    	    
	    CActiveScheduler::Start();
		
		CleanupStack::PopAndDestroy();
				
	    }
	return TestStepResult();
	}


TVerdict CMultipleVerifyLocationRequestStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	// Call base class method for post test actions	
	CTe_LbsNetworkPrivacySuiteStepBase::doTestStepPostambleL();
	
	// disconnect to the channel
    delete iGatewayChannel;
    iGatewayChannel =NULL;
    		
	return TestStepResult();
	}

/*This function is called when verification requests completes 
*/
void CMultipleVerifyLocationRequestStep::HandleVerifyComplete(TInt /*aRequestId */,TInt aCompletionCode)
    {
	INFO_PRINTF1(_L("CVerifyLocationRequestStep::HandleVerifyComplete got"));
	if(aCompletionCode != KErrNone)
		{
		INFO_PRINTF2(_L("Unexpected Completion Code %d"),aCompletionCode);
		SetTestStepResult(EFail);	
	    }
    
    }

/*This function is called for each message that is received from dummy NG. 
*/ 
void CMultipleVerifyLocationRequestStep::ProcessNetChannelMessage(RLbsNetChannel::TLbsNetChannelId aChannelId, const TLbsNetInternalMsgBase& aMessage)
	{
	if(iNGMessage == ENGMessageWaiting)
	{
	iNGMessage =ENGMessageGotFirst;	
	}
	// Call base implementation.
	CTe_LbsNetworkPrivacySuiteStepBase::ProcessNetChannelMessage(aChannelId,aMessage);
	
	if(aMessage.Type() == TLbsNetLocationRequestMsg::EPrivacyRequest)
		{
		const TLbsNetMtLrRequestMsg& msg = static_cast<const TLbsNetMtLrRequestMsg&>(aMessage);
			
		INFO_PRINTF3(_L("SessionId (%d, %d)"), msg.SessionId().SessionOwner().iUid, 
										   msg.SessionId().SessionNum());
		
		const TLbsNetMtLrReponseMsg KPrivacyResp(TLbsNetSessionIdInt(msg.SessionId()), 
										   TLbsNetworkEnumInt::EPrivacyResponseAccepted,KErrNone);	
			
		iNetInternalMessage = &KPrivacyResp;
			    
		// Send response to NG
		iGatewayChannel->SendMessageAndWaitForResponse(KPrivacyResp);
									
		INFO_PRINTF1(_L("Privacy Response succesfully sent\n"));
		}
			
	if(aMessage.Type() == TLbsNetLocationRequestMsg::ESessionComplete)
		{
		const TLbsNetSessionCompleteMsg& msg = static_cast<const TLbsNetSessionCompleteMsg&>(aMessage);
		INFO_PRINTF3(_L("SessionId (%d, %d)"), msg.SessionId().SessionOwner().iUid, 
										   msg.SessionId().SessionNum());
		if (msg.Reason()!= KErrNone)
			{
			INFO_PRINTF2(_L("Unexpected SessionComplete Reason %d"),msg.Reason());
			SetTestStepResult(EFail);
			}
                
        if(iNGMessage == ENGMessageGotSecond)	
	        {
	        CActiveScheduler::Stop();	
	        }
		
		if(iNGMessage == ENGMessageGotFirst)
			{
			iNGMessage =ENGMessageGotSecond;	
		    }
				
		}
	}

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

#include "verificationtimeoutstep.h"
#include "te_lbsnetworkprivacysuitedefs.h"
#include <EPos_CPosGSMPrivacyRequestInfo.h>
#include "lbsnrhngmsgs.h"

/**
 * Constructor
 */
CNotifyVerificationTimeoutStep::CNotifyVerificationTimeoutStep()
	{
	SetTestStepName(KVerificationTimeoutStep);
	}

/**
 * Destructor
 */
CNotifyVerificationTimeoutStep::~CNotifyVerificationTimeoutStep()
	{
	
	}


TVerdict CNotifyVerificationTimeoutStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	// Call base class method for pre test actions	
	CTe_LbsNetworkPrivacySuiteStepBase::doTestStepPreambleL();
			
	
	//Create request info
	iReqInfo = CPosGSMPrivacyRequestInfo::NewL();
	
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
TVerdict CNotifyVerificationTimeoutStep::doTestStepL()

	{
	  if (TestStepResult()==EPass)
		{
		// Dummy gateway message satus
	    iNGMessage = ENGMessageWaiting;
	    
	    iReqInfo->SetRequestorL(KTestRequestorName, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
         
        TPtrC requestorId;
        
        CPosGSMPrivacyRequestInfo::TIdType aIdType; 
               
        // Verify that the requestor id is what we expect
        User::LeaveIfError(iReqInfo->GetRequestor(requestorId,aIdType));          
                
        if(requestorId != KTestRequestorName)
	        {
	        INFO_PRINTF1(_L("** Requestor Id is wrong **"));
	        SetTestStepResult(EFail);	
	        }
	             
        iReqId = KErrNone;
        //Issue verification request
		iPosNetworkPrivacy->VerifyLocationRequestL(*iReqInfo,iReqId,*this,CPosNetworkPrivacy::EDecisionAccepted);
				
		INFO_PRINTF1(_L("VerifyLocationRequest Sent"));
		
	    CActiveScheduler::Start();
				
	    }
	return TestStepResult();
	}


TVerdict CNotifyVerificationTimeoutStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	// Call base class method for post test actions	
	CTe_LbsNetworkPrivacySuiteStepBase::doTestStepPostambleL();
	
	// Delete request info
	delete iReqInfo;
    iReqInfo =NULL;
	
	// Disconnect the channel
    delete iGatewayChannel;
    iGatewayChannel =NULL;
    		
	return TestStepResult();
	}

/*This function is called when verification request completes 
*/
void CNotifyVerificationTimeoutStep::HandleVerifyComplete(TInt /*aRequestId */,TInt aCompletionCode)
    {
	INFO_PRINTF1(_L("CVerifyLocationRequestStep::HandleVerifyComplete got"));
	if(aCompletionCode != KErrNone)
		{
		INFO_PRINTF2(_L("Unexpected Completion Code %d"),aCompletionCode);
		SetTestStepResult(EFail);	
		}
    //Send verification timeout 
	iPosNetworkPrivacy->NotifyVerificationTimeoutL(*iReqInfo,iReqId,CPosNetworkPrivacy::EDecisionRejected);
    INFO_PRINTF1(_L("NotifyVerificationTimeout Sent"));
    }

/*This function is called for each message that is received from dummy NG. 
*/ 
void CNotifyVerificationTimeoutStep::ProcessNetChannelMessage(RLbsNetChannel::TLbsNetChannelId aChannelId, const TLbsNetInternalMsgBase& aMessage)
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
			
		if(iNGMessage == ENGMessageGotFirst)
			{
			const TLbsNetMtLrReponseMsg KPrivacyResp(TLbsNetSessionIdInt(msg.SessionId()), 
										   TLbsNetworkEnumInt::EPrivacyResponseAccepted,KErrNone);	
			
			iNetInternalMessage = &KPrivacyResp;
			    
			// Send response to NG
			iGatewayChannel->SendMessageAndWaitForResponse(KPrivacyResp);
			}
		else if(iNGMessage == ENGMessageGotSecond)		
			{
			const TLbsNetMtLrReponseMsg KPrivacyResp(TLbsNetSessionIdInt(msg.SessionId()), 
										   TLbsNetworkEnumInt::EPrivacyResponseRejected,KErrNone);	
			
			iNetInternalMessage = &KPrivacyResp;
			    
			// Send response to NG
			iGatewayChannel->SendMessageAndWaitForResponse(KPrivacyResp);
			}
							
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

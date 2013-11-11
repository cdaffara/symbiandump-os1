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
 @file suplnotifylocationrequeststep.cpp
*/

#include "suplnotifylocationrequeststep.h"
#include "te_lbsnetworkprivacysuitedefs.h"
#include <EPos_CPosSUPLPrivacyRequestInfo.h>
#include "lbsnrhngmsgs.h"

/**
 * Constructor
 */
CSuplNotifyLocationRequestStep::CSuplNotifyLocationRequestStep()
	{
	SetTestStepName(KSuplNotifyLocationRequestStep);
	}

/**
 * Destructor
 */
CSuplNotifyLocationRequestStep::~CSuplNotifyLocationRequestStep()
	{
	
	}


TVerdict CSuplNotifyLocationRequestStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
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
TVerdict CSuplNotifyLocationRequestStep::doTestStepL()

	{
	  if (TestStepResult()==EPass)
		{
	    
	    CPosSUPLPrivacyRequestInfo* reqInfo = CPosSUPLPrivacyRequestInfo::NewL();
	    
	    CleanupStack::PushL(reqInfo);

	    reqInfo->SetRequestorL(KTestRequestorName, CPosSUPLPrivacyRequestInfo::EIdTypeLogicalName);
                        
        reqInfo->SetLCSClientL(KTestClientName,CPosSUPLPrivacyRequestInfo::EIdTypeLogicalName);
                
        TPtrC requestorId, aLCSClientId;
        CPosSUPLPrivacyRequestInfo::TIdType aIdType; 
        
        // Verify that the requestor id is what we expect
        User::LeaveIfError(reqInfo->GetRequestor(requestorId,aIdType));          
                
        if(requestorId != KTestRequestorName)
	        {
	        INFO_PRINTF1(_L("** Requestor Id is wrong **"));
	        SetTestStepResult(EFail);	
	        }
	                        
        // Verify that the LCSclient id is what we expect
        User::LeaveIfError(reqInfo->GetLCSClient(aLCSClientId,aIdType));          
                
        if(aLCSClientId != KTestClientName)
	        {
	        INFO_PRINTF1(_L("** LCSClient Id is wrong **"));
	        SetTestStepResult(EFail);	
	        }
        
        //Issue a request
		iPosNetworkPrivacy ->NotifyLocationRequestL(*reqInfo,iRequestId);
		
		INFO_PRINTF1(_L(" NotifyLocationRequest Sent"));
		
		CActiveScheduler::Start();
		
		CleanupStack::PopAndDestroy();		  			
		}
	return TestStepResult();
	}


TVerdict CSuplNotifyLocationRequestStep::doTestStepPostambleL()
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


/*This function is called for each message that is received from dummy NG. 
*/ 
void CSuplNotifyLocationRequestStep::ProcessNetChannelMessage(RLbsNetChannel::TLbsNetChannelId aChannelId, const TLbsNetInternalMsgBase& aMessage)
	{
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
			
		// Start and finish a positioning session, so we get the session end
	    TRequestStatus status;	
		iPosSession->InitializeL(iRequestId, status);
	    User::WaitForRequest(status);
		iPosSession->SessionEnd();
		
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
		CActiveScheduler::Stop();	
		}
	}

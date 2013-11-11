// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file lbsnrhstatetestsinglestep.cpp
*/

#include "lbscommoninternaldatatypes.h"
#include "lbsnrhstatetestsinglestep.h"


// MaxFixAge used for the tests that check MaxFixAge behaviour
const TTimeIntervalMicroSeconds KMaxFixAge = TTimeIntervalMicroSeconds(10000000);


CLbsNrhStateTestSingleStep::CLbsNrhStateTestSingleStep()
	{
	SetTestStepName(KLbsNrhStateTestSingleStep);
	}

CLbsNrhStateTestSingleStep::~CLbsNrhStateTestSingleStep()
	{
	}

/**
 * @return - TVerdict code
 * Override of base class virtual
 */
TVerdict CLbsNrhStateTestSingleStep::doTestStepPreambleL()
	{
	// Common initialisation  
	CLbsNrhStateTestBaseStep::doTestStepPreambleL();
	
	SetTestStepResult(EPass);
	return TestStepResult();
	}

/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
TVerdict CLbsNrhStateTestSingleStep::doTestStepL()
	{
	if (TestStepResult() == EPass)
		{
	    ReadIniFileValues();
	      
		switch (GetTestCaseId())
			{
			case 0:	{ SingleMtLrL(); 						break; }
			case 1:	{ SingleMoLrL(); 						break; }
			case 2:	{ SingleMoLr_CellL(); 					break; }
			case 3:	{ SingleX3pL(); 						break; }
			case 4:	{ SingleSuplMtLrL(); 					break; }
			
			case 5:	{ SingleSuplMoLrL(); 					break; }
			case 6:	{ SingleMtLrMaxFixAgeL(); 				break; }
			case 7:	{ SingleMoLrMaxFixAgeL(); 				break; }
			case 8:	{ SingleMtLrRefAfterLocRequestL(); 		break; }
			case 9:	{ SingleEmergencyMtLrL(); 				break; }
			
			case 10:{ SingleEmergencyMtLrLAdviceNotify(); 	break; }
			case 11:{ SingleEmergencyNI(); 					break; }
			case 12:{ SingleEmergencyNIRefPosBeforeLocReq();break; }
			case 13:{ SingleMtLrL2(); 						break; }
	        case 14:{ GenericTestL();                        break; }
	        
	        case 15:{ SingleMtLrTimeoutPrivacyL();          break; } 
	        case 16:{ SingleMtLrTimeoutLocationL();          break; } 
	        case 17:{ NewClientMoLrL(); 					break; }
	        case 18:{ PreemptedX3pL(); 					break; }


			default:
				{
				WARN_PRINTF2(_L("CLbsNrhStateTestSingleStep::doTestStepL: Unknown test case Id: %d"), 
							 GetTestCaseId());
				break;
				}
			}
		}
		
	return TestStepResult();
	}

/**
 * @return - TVerdict code
 * Override of base class virtual
 */
TVerdict CLbsNrhStateTestSingleStep::doTestStepPostambleL()
	{	
	CLbsNrhStateTestBaseStep::doTestStepPostambleL();

	return TestStepResult();	
	}


void CLbsNrhStateTestSingleStep::SingleMtLrL()
	{
	// Standard MT-LR
	NetGateway_SendPrivacyRequestL(4);
	PrivacyController_WaitPrivacyRequestL(4);
	PrivacyController_SendPrivacyResponseL(4, CLbsPrivacyController::ERequestAccepted);
	NetGateway_WaitPrivacyResponseL(4, TLbsNetworkEnumInt::EPrivacyResponseAccepted);
	AgpsManager_WaitLocationRequestStatusL();

	// note, here we inject the reference position and location request
	// one after the other,. This is the most likely as the PM processes the incoming
	// measurement control to produce these two messages.
	NetGateway_SendNetworkReferencePositionL(4, TLbsNetworkEnumInt::EServiceMobileTerminated);
	NetGateway_SendLocationRequestL(4, TLbsNetworkEnumInt::EServiceMobileTerminated);

	PrivacyController_WaitPrivacyReferencePositionL(4);

	AgpsManager_WaitLocationRequestL();
	AgpsManager_SendGpsPositionUpdateAccurateL();
	NetGateway_WaitLocationResponseL(4, KErrNone);
	PrivacyController_WaitPrivacyGpsPositionL(4);		
	NetGateway_SendSessionCompleteL(4, KErrNone, TLbsNetworkEnumInt::EServiceMobileTerminated);
	PrivacyController_WaitPrivacyCompleteL(4, KErrNone);
	AgpsManager_WaitLocationRequestCancelL();	
	}

void CLbsNrhStateTestSingleStep::SingleMtLrL2()
	{
	// Similar to Standard MT-LR, SingleMtLrL - but waits for REF pos 
	// callback  before injecting location request
	
	NetGateway_SendPrivacyRequestL(4);
	PrivacyController_WaitPrivacyRequestL(4);
	PrivacyController_SendPrivacyResponseL(4, CLbsPrivacyController::ERequestAccepted);
	NetGateway_WaitPrivacyResponseL(4, TLbsNetworkEnumInt::EPrivacyResponseAccepted);
	AgpsManager_WaitLocationRequestStatusL();

	NetGateway_SendNetworkReferencePositionL(4, TLbsNetworkEnumInt::EServiceMobileTerminated);
	// note its immediately after injecting REF pos and before loc request!
	// as shown on most of our message sequence charts
	PrivacyController_WaitPrivacyReferencePositionL(4);

	
	NetGateway_SendLocationRequestL(4, TLbsNetworkEnumInt::EServiceMobileTerminated);

	
	AgpsManager_WaitLocationRequestL();
	AgpsManager_SendGpsPositionUpdateAccurateL();
	NetGateway_WaitLocationResponseL(4, KErrNone);
	PrivacyController_WaitPrivacyGpsPositionL(4);		
	NetGateway_SendSessionCompleteL(4, KErrNone, TLbsNetworkEnumInt::EServiceMobileTerminated);
	PrivacyController_WaitPrivacyCompleteL(4, KErrNone);
	AgpsManager_WaitLocationRequestCancelL();	
	}

void CLbsNrhStateTestSingleStep::SingleEmergencyMtLrL()
	{
	// Emergency MT-LR with AdviceVerify - but as this is an Emergency
	// its processsed the same as AdviceNotify and no reposnse is expected
	// LBS simulates the response	
	// Emergency MT-LR - ref postion injected before location request
	
	NetGateway_SendEmergencyPrivacyRequestL(iSequenceNumber);
	PrivacyController_WaitPrivacyRequestL(iSequenceNumber);
	
	// Note the absnse of need to send in a reponse here. Its an emergency!
	// so the test does NOT do this ... PrivacyController_SendPrivacyResponseL(iSequenceNumer, CLbsPrivacyController::ERequestAccepted);

	NetGateway_WaitPrivacyResponseL(iSequenceNumber, TLbsNetworkEnumInt::EPrivacyResponseAccepted);
	AgpsManager_WaitLocationRequestStatusL();

	NetGateway_SendNetworkReferencePositionL(iSequenceNumber, TLbsNetworkEnumInt::EServiceMobileTerminated);
	NetGateway_SendEmergencyLocationRequestL(iSequenceNumber, TLbsNetworkEnumInt::EServiceMobileTerminated);
	
	PrivacyController_WaitPrivacyReferencePositionL(iSequenceNumber);

	AgpsManager_WaitLocationRequestOrRequestAndStatusL();
	
	AgpsManager_SendGpsPositionUpdateAccurateL();
	
	NetGateway_WaitLocationResponseL(iSequenceNumber, KErrNone);
	PrivacyController_WaitPrivacyGpsPositionL(iSequenceNumber);		
	NetGateway_SendSessionCompleteL(iSequenceNumber, KErrNone, TLbsNetworkEnumInt::EServiceMobileTerminated);
	PrivacyController_WaitPrivacyCompleteL(iSequenceNumber, KErrNone);
	AgpsManager_WaitLocationRequestCancelL();	
	}
	


void CLbsNrhStateTestSingleStep::SingleEmergencyNI()
	{
	// Emergency Network Induced - ref postion injected after location request

	NetGateway_SendEmergencyLocationRequestL(6, TLbsNetworkEnumInt::EServiceNetworkInduced);

	AgpsManager_WaitLocationRequestOrRequestAndStatusL();

	PrivacyController_WaitPrivacyRequestL(6);

	NetGateway_SendNetworkReferencePositionL(6, TLbsNetworkEnumInt::EServiceMobileTerminated);
	PrivacyController_WaitPrivacyReferencePositionL(6);

	AgpsManager_SendGpsPositionUpdateAccurateL();
	
	NetGateway_WaitLocationResponseL(6, KErrNone);

	PrivacyController_WaitPrivacyGpsPositionL(6);		
	NetGateway_SendSessionCompleteL(6, KErrNone, TLbsNetworkEnumInt::EServiceMobileTerminated);
	PrivacyController_WaitPrivacyCompleteL(6, KErrNone);
	AgpsManager_WaitLocationRequestCancelL();	
	}

void CLbsNrhStateTestSingleStep::SingleEmergencyNIRefPosBeforeLocReq()
	{
	// Emergency Network Induced - ref postion injected before location request

	NetGateway_SendNetworkReferencePositionL(7, TLbsNetworkEnumInt::EServiceMobileTerminated);

	NetGateway_SendEmergencyLocationRequestL(7, TLbsNetworkEnumInt::EServiceNetworkInduced);

	AgpsManager_WaitLocationRequestOrRequestAndStatusL();
	
	PrivacyController_WaitPrivacyRequestL(7);

	PrivacyController_WaitPrivacyReferencePositionL(7);
	
	AgpsManager_SendGpsPositionUpdateAccurateL();
	
	NetGateway_WaitLocationResponseL(7, KErrNone);

	PrivacyController_WaitPrivacyGpsPositionL(7);		
	NetGateway_SendSessionCompleteL(7, KErrNone, TLbsNetworkEnumInt::EServiceMobileTerminated);
	PrivacyController_WaitPrivacyCompleteL(7, KErrNone);
	AgpsManager_WaitLocationRequestCancelL();	
	}
void CLbsNrhStateTestSingleStep::SingleEmergencyMtLrLAdviceNotify()
	{
	// Emergency MT-LR with AdviceNotify 
	//(user merely notfied of the request and no reposnse is expected)
	
	NetGateway_SendEmergencyPrivacyRequestAdviceNotifyL(8);
	PrivacyController_WaitPrivacyRequestL(8);
	// because it is an Emergency, reagradless of whether AdviceNotify or Advice Verify - no response needs to be  sent from Privacy Controller
	// so note absense of  PrivacyController_SendPrivacyResponseL(8, CLbsPrivacyController::ERequestAccepted);
	NetGateway_WaitPrivacyResponseL(8, TLbsNetworkEnumInt::EPrivacyResponseAccepted);
	AgpsManager_WaitLocationRequestStatusL();
	
	NetGateway_SendNetworkReferencePositionL(8, TLbsNetworkEnumInt::EServiceMobileTerminated);
	NetGateway_SendEmergencyLocationRequestL(8, TLbsNetworkEnumInt::EServiceMobileTerminated);

	PrivacyController_WaitPrivacyReferencePositionL(8);

	AgpsManager_WaitLocationRequestOrRequestAndStatusL();
	
	AgpsManager_SendGpsPositionUpdateAccurateL();

	NetGateway_WaitLocationResponseL(8, KErrNone);
	PrivacyController_WaitPrivacyGpsPositionL(8);		
	NetGateway_SendSessionCompleteL(8, KErrNone, TLbsNetworkEnumInt::EServiceMobileTerminated);
	PrivacyController_WaitPrivacyCompleteL(8, KErrNone);
	AgpsManager_WaitLocationRequestCancelL();	
	}

void CLbsNrhStateTestSingleStep::SingleMtLrRefAfterLocRequestL()
	{
	// similar to Standard MT-LR but here we send in the REF position AFTER the location request
	NetGateway_SendPrivacyRequestL(9);
	PrivacyController_WaitPrivacyRequestL(9);
	PrivacyController_SendPrivacyResponseL(9, CLbsPrivacyController::ERequestAccepted);
	NetGateway_WaitPrivacyResponseL(9, TLbsNetworkEnumInt::EPrivacyResponseAccepted);
	AgpsManager_WaitLocationRequestStatusL();
	NetGateway_SendLocationRequestL(9, TLbsNetworkEnumInt::EServiceMobileTerminated);
	AgpsManager_WaitLocationRequestL();
	
	// send in the reference position AFTER the location request
	
	NetGateway_SendNetworkReferencePositionL(9, TLbsNetworkEnumInt::EServiceMobileTerminated);
	PrivacyController_WaitPrivacyReferencePositionL(9);

	AgpsManager_SendGpsPositionUpdateAccurateL();
	NetGateway_WaitLocationResponseL(9, KErrNone);
	PrivacyController_WaitPrivacyGpsPositionL(9);		
	NetGateway_SendSessionCompleteL(9, KErrNone, TLbsNetworkEnumInt::EServiceMobileTerminated);
	PrivacyController_WaitPrivacyCompleteL(9, KErrNone);
	AgpsManager_WaitLocationRequestCancelL();	
	}
	
void CLbsNrhStateTestSingleStep::SingleMoLrL()
	{
	// Standard MO-LR
	NetGateway_SendNetworkReferencePositionL(2, TLbsNetworkEnumInt::EServiceSelfLocation);
	NetGateway_SendLocationRequestL(2, TLbsNetworkEnumInt::EServiceSelfLocation);
	AgpsManager_WaitLocationRequestL();
	AgpsManager_SendGpsPositionUpdateAccurateL();
	NetGateway_WaitLocationResponseL(2, KErrNone);
	NetGateway_SendSessionCompleteL(2, KErrNone, TLbsNetworkEnumInt::EServiceSelfLocation);
	AgpsManager_WaitLocationRequestCancelL();
	
	// Inaccurate MO-LR
	NetGateway_SendNetworkReferencePositionL(3, TLbsNetworkEnumInt::EServiceSelfLocation);
	NetGateway_SendLocationRequestL(3, TLbsNetworkEnumInt::EServiceSelfLocation);
	AgpsManager_WaitLocationRequestL();
	AgpsManager_SendGpsPositionUpdateInaccurateL();
	NetGateway_WaitLocationResponseL(3, KPositionQualityLoss);
	NetGateway_SendSessionCompleteL(3, KErrNone, TLbsNetworkEnumInt::EServiceSelfLocation);
	AgpsManager_WaitLocationRequestCancelL();	
	}
	
void CLbsNrhStateTestSingleStep::NewClientMoLrL()
	{
	// For this test  a second self locate molr premempts one
	// already ongoing. The first one is implicitly cancelled.
	NetGateway_SendNetworkReferencePositionL(2, TLbsNetworkEnumInt::EServiceSelfLocation);
	NetGateway_SendLocationRequestL(2, TLbsNetworkEnumInt::EServiceSelfLocation);
	AgpsManager_WaitLocationRequestL();
	
	// now a new client causes a second session to start
	// the original session is dropped and the NRH sends no further messages
	// relating to this session to the protocol module

	NetGateway_SendNetworkReferencePositionL(3, TLbsNetworkEnumInt::EServiceSelfLocation);
	NetGateway_SendLocationRequestL(3, TLbsNetworkEnumInt::EServiceSelfLocation);
	AgpsManager_WaitLocationRequestL();
	AgpsManager_SendGpsPositionUpdateAccurateL();
	NetGateway_WaitLocationResponseL(3, KErrNone);
	NetGateway_SendSessionCompleteL(3, KErrNone, TLbsNetworkEnumInt::EServiceSelfLocation);
	AgpsManager_WaitLocationRequestCancelL();
		
	}
	

void CLbsNrhStateTestSingleStep::SingleMtLrTimeoutPrivacyL()
    {
    // Standard MT-LR
    NetGateway_SendPrivacyRequestL(4);
    PrivacyController_WaitPrivacyRequestL(4);
    // We send no response here
    //PrivacyController_SendPrivacyResponseL(4, CLbsPrivacyController::ERequestAccepted);
    // After the timeout we get the default action (accept)
    NetGateway_WaitPrivacyResponseL(4, TLbsNetworkEnumInt::EPrivacyResponseAccepted);
    AgpsManager_WaitLocationRequestStatusL();

    // note, here we inject the reference position and location request
    // one after the other,. This is the most likely as the PM processes the incoming
    // measurement control to produce these two messages.
    NetGateway_SendNetworkReferencePositionL(4, TLbsNetworkEnumInt::EServiceMobileTerminated);
    NetGateway_SendLocationRequestL(4, TLbsNetworkEnumInt::EServiceMobileTerminated);

    PrivacyController_WaitPrivacyReferencePositionL(4);

    AgpsManager_WaitLocationRequestL();
    AgpsManager_SendGpsPositionUpdateAccurateL();
    NetGateway_WaitLocationResponseL(4, KErrNone);
    PrivacyController_WaitPrivacyGpsPositionL(4);       
    NetGateway_SendSessionCompleteL(4, KErrNone, TLbsNetworkEnumInt::EServiceMobileTerminated);
    PrivacyController_WaitPrivacyCompleteL(4, KErrNone);
    AgpsManager_WaitLocationRequestCancelL();    
    }

void CLbsNrhStateTestSingleStep::SingleMtLrTimeoutLocationL()
    {
    // Standard MT-LR
    NetGateway_SendPrivacyRequestL(4);
    PrivacyController_WaitPrivacyRequestL(4);
    PrivacyController_SendPrivacyResponseL(4, CLbsPrivacyController::ERequestAccepted);
    NetGateway_WaitPrivacyResponseL(4, TLbsNetworkEnumInt::EPrivacyResponseAccepted);
    AgpsManager_WaitLocationRequestStatusL();

    // note, here we inject the reference position and location request
    // one after the other,. This is the most likely as the PM processes the incoming
    // measurement control to produce these two messages.
    NetGateway_SendNetworkReferencePositionL(4, TLbsNetworkEnumInt::EServiceMobileTerminated);
    NetGateway_SendLocationRequestL(4, TLbsNetworkEnumInt::EServiceMobileTerminated);

    PrivacyController_WaitPrivacyReferencePositionL(4);

    AgpsManager_WaitLocationRequestL();
    //AgpsManager_SendGpsPositionUpdateAccurateL();
    const TInt KErrPositionNoGpsUpdate = -17951; 
    NetGateway_WaitLocationResponseL(4, KErrPositionNoGpsUpdate);
    //PrivacyController_WaitPrivacyGpsPositionL(4);       
    NetGateway_SendSessionCompleteL(4, KErrPositionNoGpsUpdate, TLbsNetworkEnumInt::EServiceMobileTerminated);
    PrivacyController_WaitPrivacyCompleteL(4, KErrPositionNoGpsUpdate);
    AgpsManager_WaitLocationRequestCancelL();    
    }


// This test step is driven using the values in the .ini file
void CLbsNrhStateTestSingleStep::GenericTestL()
     {
     TLbsNetworkEnumInt::TLbsNetProtocolServiceInt type;

     type = static_cast<TLbsNetworkEnumInt::TLbsNetProtocolServiceInt> (iSessionType);
     
     switch(iSessionType)
         {
         case 0:
             break;
         case TLbsNetworkEnumInt::EServiceNetworkLocation:
             // Standard MO-LR Cell-based

       		if ((iFirstRequestAgpsMode0 == 6)&& (iFirstRequestAgpsMode1 == KErrNotFound)) // tap 
        		 {
        		 // TA mode - testing GPS terminal Assisted Positioning using SUPL protocol
        		 // we shall send in a REF oostion and measurements
        		 NetGateway_SendNetworkReferencePositionL(iSequenceNumber, type);
        		 NetGateway_SendLocationRequestL(iSequenceNumber, type, iFirstRequestAgpsMode0,iFirstRequestAgpsMode1,iAccuracyFirstRequest, iMaxFixFirstReq );
        		 AgpsManager_SendGpsMeasurementUpdateL();
        		 NetGateway_WaitMeasurementResponseL(iSequenceNumber, KErrNone);
        		 NetGateway_SendSessionCompleteL(iSequenceNumber, KErrNone, type);
        		 }
        	 else
        		 {
        		 // PTB mode
        		 // we shall send a REF postion in and then check that the same REF position pops out
        		 NetGateway_SendNetworkReferencePositionL(iSequenceNumber, type);
        		 // note that the agps mode, quality and max fix time does not matter here!
        		 NetGateway_SendLocationRequestL(iSequenceNumber, type);
        		 NetGateway_WaitLocationResponseL(iSequenceNumber, KErrNone);
        		 NetGateway_SendSessionCompleteL(iSequenceNumber, KErrNone, type);
        		 }
             break;
         case TLbsNetworkEnumInt::EServiceSelfLocation:
             // Standard MO-LR Self Locate
             
             if (KRefBeforeLocationrequest == iRefPosBeforeLocationRequest)
                 {
                 // if on in .ini or default send REF first
                 NetGateway_SendNetworkReferencePositionL(2, type);
                 }
                
             NetGateway_SendLocationRequestL(2, type, iFirstRequestAgpsMode0,iFirstRequestAgpsMode1,iAccuracyFirstRequest, iMaxFixFirstReq );

             AgpsManager_WaitLocationRequestL();
             
             if (0 == iRefPosBeforeLocationRequest)
                 {
                 NetGateway_SendNetworkReferencePositionL(2, type);
                 }
             
             NetGateway_SendNetworkReferencePositionL(2, type);
  
             if ((iFirstAgpsModeToAgpsMan0 == 6)&& (iSecondAgpsModeToAgpsMan1 == KErrNotFound)) // tap 
            	 {
            	 // tap mode
   				 AgpsManager_SendGpsMeasurementUpdateL();
				 NetGateway_WaitMeasurementResponseL(2, iErrorCodeFromNrh);
            	 }
             else if ((iFirstAgpsModeToAgpsMan0 == 5) && (iFirstAgpsModeToAgpsMan1 == 6)) // hybrid
                 {
                 // hybrid
                 // TODO - send in some of thses first before pos AgpsManager_SendGpsMeasurementUpdateL();
                 AgpsManager_SendGpsPositionUpdateAccurateL();
                 NetGateway_WaitLocationResponseL(2, iErrorCodeFromNrh);
                 }
             else if ((iFirstAgpsModeToAgpsMan0 == 5) && (iFirstAgpsModeToAgpsMan1 == KErrNotFound))
              	 {
              	 // tb
            	 AgpsManager_SendGpsPositionUpdateAccurateL();
            	 NetGateway_WaitLocationResponseL(2, iErrorCodeFromNrh);
            	 }
	             
             if (KErrNotFound != iNetworkPosToNrhError)
            	 {
            	 // ProcessLocationUpdate(error, pos) -> NRH
           		 NetGateway_SendNetworkFinalPositionL(2,type);
            	 }
          
             NetGateway_SendSessionCompleteL(2, KErrNone, type);
             AgpsManager_WaitLocationRequestCancelL();
             break;
 
                 
         case TLbsNetworkEnumInt::EServiceMobileTerminated:
        	// Standard MT-LR
        
             if(1 == iEmergency)
                 {
                 NetGateway_SendEmergencyPrivacyRequestAdviceNotifyL(iSequenceNumber);
                  PrivacyController_WaitPrivacyRequestL(iSequenceNumber);
                  // because it is an Emergency, reagradless of whether AdviceNotify or Advice Verify - no response needs to be  sent from Privacy Controller
                  // so note absense of  PrivacyController_SendPrivacyResponseL();
                  NetGateway_WaitPrivacyResponseL(iSequenceNumber, TLbsNetworkEnumInt::EPrivacyResponseAccepted);
                 }
             else
                 {
                 // Get Privacy request settings, or use defaults
                 TLbsNetPosRequestPrivacyInt::TLbsRequestAdviceInt advice = static_cast<TLbsNetPosRequestPrivacyInt::TLbsRequestAdviceInt> (iPrivacyRequestAdvice);
                 TLbsNetPosRequestPrivacyInt::TLbsRequestActionInt action = static_cast<TLbsNetPosRequestPrivacyInt::TLbsRequestActionInt> (iPrivacyRequestDefaultAction);
                 NetGateway_SendPrivacyRequestL(iSequenceNumber, advice, action, iEmergency);
                 
                 PrivacyController_WaitPrivacyRequestL(iSequenceNumber);
                 
                 // Get Privacy Response or use defaults
                 CLbsPrivacyController::TRequestVerificationResult privacySendResponse = static_cast<CLbsPrivacyController::TRequestVerificationResult> (iPrivacySendResponseAction);
                 PrivacyController_SendPrivacyResponseL(iSequenceNumber, privacySendResponse);
                 
                 TLbsNetworkEnumInt::TLbsPrivacyResponseInt privacyRcvResponse = static_cast<TLbsNetworkEnumInt::TLbsPrivacyResponseInt> (iPrivacyRcvResponseAction);
                 NetGateway_WaitPrivacyResponseL(iSequenceNumber, privacyRcvResponse);
                 if(2 == iPrivacyRcvResponseAction)
                     {
                     // We rejected the privacy request - this ends here
                     NetGateway_SendSessionCompleteL(iSequenceNumber, KErrNone, type);
                     PrivacyController_WaitPrivacyCompleteL(iSequenceNumber, iPrivacyCompleteError);  
                     return;
                     }
                 }
             
        	AgpsManager_WaitLocationRequestStatusL();

        	// note, here we inject the reference position and location request
        	// one after the other,. This is the most likely as the PM processes the incoming
        	// measurement control to produce these two messages.

            if (1 == iRefPosBeforeLocationRequest)
            	{
               	NetGateway_SendNetworkReferencePositionL(iSequenceNumber, type);
            	}
    
            if(1 == iEmergency)
                {
                NetGateway_SendEmergencyLocationRequestL(iSequenceNumber, type, iFirstRequestAgpsMode0,iFirstRequestAgpsMode1);
                }
            else
                {
                NetGateway_SendLocationRequestL(iSequenceNumber, type, iFirstRequestAgpsMode0,iFirstRequestAgpsMode1);
                }
         	

            if (0 == iRefPosBeforeLocationRequest)
                {
               	NetGateway_SendNetworkReferencePositionL(iSequenceNumber, type);
                }
            
        	PrivacyController_WaitPrivacyReferencePositionL(iSequenceNumber);

        	AgpsManager_WaitLocationRequestOrRequestAndStatusL();
        	
            if ((iFirstAgpsModeToAgpsMan0 == 6)&& (iSecondAgpsModeToAgpsMan1 == KErrNotFound)) // tap 
                {
                // tap mode
                AgpsManager_SendGpsMeasurementUpdateL();
                NetGateway_WaitMeasurementResponseL(iSequenceNumber, iErrorCodeFromNrh);
                }
            else
                {
                AgpsManager_SendGpsPositionUpdateAccurateL();
                NetGateway_WaitLocationResponseL(iSequenceNumber, iErrorCodeFromNrh);
                PrivacyController_WaitPrivacyGpsPositionL(iSequenceNumber); 
                }
        	
        		
        	NetGateway_SendSessionCompleteL(iSequenceNumber, KErrNone, type);
        	PrivacyController_WaitPrivacyCompleteL(iSequenceNumber, iPrivacyCompleteError);
        	AgpsManager_WaitLocationRequestCancelL();	
            break; 
             
         case TLbsNetworkEnumInt::EServiceTransmitThirdParty:
        	// Standard X3P
        	// for power advice behaviour see Figure 10 NRH – Power Advice selection of Arch Description
        	//
        	// initially X3P Status is X3P Idle
        	X3pClient_ConnectClientL(1);
        	// now X3P Status X3P is Connected
        	// when X3P Connected and there are no combined location request from NRH, power advice is STANDBY
        	AgpsManager_WaitLocationRequestStatusL(TLbsPositionUpdateRequestBase::EPowerAdviceStandby);
        	X3pClient_SendX3pRequestL(1);
        	// now X3P Status X3P is Active
        	TUint x3pSessionId;
        	NetGateway_WaitX3pRequestL(x3pSessionId);
        	// when X3P Connected and there are combined location request from NRH is IDLE, power advice is ON
        	AgpsManager_WaitLocationRequestStatusL(TLbsPositionUpdateRequestBase::EPowerAdviceOn);
        	NetGateway_SendNetworkReferencePositionL(x3pSessionId, type);
        	if(iX3pTransmitPositionAgpsAndRefPos)
        	    {
        	    X3pClient_WaitX3pReferencePositionL(1);
        	    }
        	NetGateway_SendLocationRequestL(x3pSessionId, type, iFirstRequestAgpsMode0, iFirstRequestAgpsMode1);
        	AgpsManager_WaitLocationRequestL();
        	
            if ((iFirstAgpsModeToAgpsMan0 == 6)&& (iSecondAgpsModeToAgpsMan1 == KErrNotFound)) // tap 
                {
                // tap mode
                AgpsManager_SendGpsMeasurementUpdateL();
                NetGateway_WaitMeasurementResponseL(x3pSessionId, iErrorCodeFromNrh);
                }
            else
                {
                AgpsManager_SendGpsPositionUpdateAccurateL(iAgpsPosToNrhError);
                NetGateway_WaitLocationResponseL(x3pSessionId, iErrorCodeFromNrh);
                }        	
        	
        	NetGateway_SendNetworkFinalPositionL(x3pSessionId, type);

        	NetGateway_SendSessionCompleteL(x3pSessionId, KErrNone, type);
        	AgpsManager_WaitLocationRequestCancelL(KErrNotFound); // KErrNotFound is used because status could be Standby or On
        	
        	X3pClient_WaitX3pCompleteL(1, KErrNone);
        	
        	// If context switching occurs quick enough, we may not get EPowerAdviceOn in the previous call.
        	// If instead we got Standby first, then we must not wait for another power status, we must disconnect
        	// and wait for poweroff.
        	
        	if(iLastPowerAdvice != TLbsPositionUpdateRequestBase::EPowerAdviceStandby)
        	    {
                // change from ON to Standby
        	    AgpsManager_WaitLocationRequestStatusL(TLbsPositionUpdateRequestBase::EPowerAdviceStandby);
        	    }
        	
       		X3pClient_DisconnectClientL(1);
       		// now X3P Status X3P is Idle
       		// and now Off because no X3P sessions connected
       		AgpsManager_WaitLocationRequestStatusL(TLbsPositionUpdateRequestBase::EPowerAdviceOff);
       		break; 
  
         case TLbsNetworkEnumInt::EServiceNetworkInduced:
        	// Emergency Network Induced - ref postion injected after location request

            if(iEmergency)
                {
                NetGateway_SendEmergencyLocationRequestL(iSequenceNumber, type, iFirstRequestAgpsMode0,iFirstRequestAgpsMode1);
                
        		AgpsManager_WaitLocationRequestOrRequestAndStatusL();
 
        		PrivacyController_WaitPrivacyRequestL(iSequenceNumber);
                }
            else
                {
                NetGateway_SendLocationRequestL(iSequenceNumber, type, iFirstRequestAgpsMode0,iFirstRequestAgpsMode1);
                
                PrivacyController_WaitPrivacyRequestL(iSequenceNumber);
                
                // Get Privacy Response or use defaults
                CLbsPrivacyController::TRequestVerificationResult privacySendResponse = static_cast<CLbsPrivacyController::TRequestVerificationResult> (iPrivacySendResponseAction);
                PrivacyController_SendPrivacyResponseL(iSequenceNumber, privacySendResponse);

                AgpsManager_WaitLocationRequestOrRequestAndStatusL();
                
                }
        	

        	NetGateway_SendNetworkReferencePositionL(iSequenceNumber, type);
        	PrivacyController_WaitPrivacyReferencePositionL(iSequenceNumber);
        	
            if ((iFirstAgpsModeToAgpsMan0 == 6)&& (iSecondAgpsModeToAgpsMan1 == KErrNotFound)) // tap 
                {
                // tap mode
                AgpsManager_SendGpsMeasurementUpdateL();
                NetGateway_WaitMeasurementResponseL(iSequenceNumber, iErrorCodeFromNrh);
                }
            else
                {
                AgpsManager_SendGpsPositionUpdateAccurateL();
                NetGateway_WaitLocationResponseL(iSequenceNumber, KErrNone);
                PrivacyController_WaitPrivacyGpsPositionL(iSequenceNumber); 
                }

        	NetGateway_SendSessionCompleteL(iSequenceNumber, KErrNone, type);
        	PrivacyController_WaitPrivacyCompleteL(iSequenceNumber, KErrNone);
        	AgpsManager_WaitLocationRequestCancelL();
        	break;    
  
         }
     
     }
        
        
void CLbsNrhStateTestSingleStep::SingleMoLr_CellL()
	{
	// Standard MO-LR Cell-based
	NetGateway_SendNetworkReferencePositionL(10, TLbsNetworkEnumInt::EServiceNetworkLocation);
	NetGateway_SendLocationRequestL(10, TLbsNetworkEnumInt::EServiceNetworkLocation);
	NetGateway_WaitLocationResponseL(10, KErrNone);
	NetGateway_SendSessionCompleteL(10, KErrNone, TLbsNetworkEnumInt::EServiceNetworkLocation);	
	}
	

void CLbsNrhStateTestSingleStep::PreemptedX3pL()
	{
	// A low poriority X3P is preemepted by one with higher priority
	// The first nrh session one is implicitly cancelled and
	// the second complets. After completion of the higher priority x3p
	// request the nrh re-sends the original one and that is completed also.
	
	// initially X3P Status is X3P Idle
	X3pClient_ConnectClientL(1);
	X3pClient_ConnectClientL(2);
	
	X3pClient_SendX3pRequestL(1, TRUE /* low priority*/);

	TUint x3pSessionId;
	NetGateway_WaitX3pRequestL(x3pSessionId);

	NetGateway_SendNetworkReferencePositionL(x3pSessionId, TLbsNetworkEnumInt::EServiceTransmitThirdParty);
	X3pClient_WaitX3pReferencePositionL(1);
	NetGateway_SendLocationRequestL(x3pSessionId, TLbsNetworkEnumInt::EServiceTransmitThirdParty);
	
	// now perform a second, higher priority x3p request
	X3pClient_SendX3pRequestL(2 /* high priority*/);
	TUint x3pSessionIdSecond;
	NetGateway_WaitX3pRequestL(x3pSessionIdSecond);
	NetGateway_SendNetworkReferencePositionL(x3pSessionIdSecond, TLbsNetworkEnumInt::EServiceTransmitThirdParty);
	X3pClient_WaitX3pReferencePositionL(2);
	NetGateway_SendLocationRequestL(x3pSessionIdSecond, TLbsNetworkEnumInt::EServiceTransmitThirdParty);
	
	// for this test we are not interested in testing 
	// messages being sent to the agps manager.correct (other tests do this!)
	// so just wait for a second before sending in a agps position. 
	// This should satisfy the now lone x3p request. 
	// The first one has been explicity cancelled
	User::After(1 * 1000 * 1000);
	AgpsManager_SendGpsPositionUpdateAccurateL();

	// check that the agps position satifies the higher prioity request
	NetGateway_WaitLocationResponseL(x3pSessionIdSecond, KErrNone);
	NetGateway_SendNetworkFinalPositionL(x3pSessionIdSecond, TLbsNetworkEnumInt::EServiceTransmitThirdParty);

	NetGateway_SendSessionCompleteL(x3pSessionIdSecond, KErrNone, TLbsNetworkEnumInt::EServiceTransmitThirdParty);
	
	X3pClient_WaitX3pCompleteL(2, KErrNone);
	
  	X3pClient_DisconnectClientL(2);
  	
  	TUint x3pSessionIdRepeatFirst;
  	NetGateway_WaitX3pRequestL(x3pSessionIdRepeatFirst);
  	
  	NetGateway_SendNetworkReferencePositionL(x3pSessionIdRepeatFirst, TLbsNetworkEnumInt::EServiceTransmitThirdParty);
  	
  	// note a REF position has already been returned
	// X3pClient_WaitX3pReferencePositionL(1);
	NetGateway_SendLocationRequestL(x3pSessionIdRepeatFirst, TLbsNetworkEnumInt::EServiceTransmitThirdParty);
	
	// wait for a second before sending in a agps position. 

	User::After(1 * 1000 * 1000);
	AgpsManager_SendGpsPositionUpdateAccurateL();

	// check that the agps position satifies the higher prioity request
	NetGateway_WaitLocationResponseL(x3pSessionIdRepeatFirst, KErrNone);
	NetGateway_SendNetworkFinalPositionL(x3pSessionIdRepeatFirst, TLbsNetworkEnumInt::EServiceTransmitThirdParty);

	NetGateway_SendSessionCompleteL(x3pSessionIdRepeatFirst, KErrNone, TLbsNetworkEnumInt::EServiceTransmitThirdParty);

	X3pClient_WaitX3pCompleteL(1, KErrNone);
		
  	X3pClient_DisconnectClientL(1);
  	
  		
	}
void CLbsNrhStateTestSingleStep::SingleX3pL()
	{
	// Standard X3P
	// for power advice behaviour see Figure 10 NRH – Power Advice selection of Arch Description
	//
	// initially X3P Status is X3P Idle
	X3pClient_ConnectClientL(1);
	// now X3P Status X3P is Connected
	// when X3P Connected and there are no combined location request from NRH, power advice is STANDBY
	AgpsManager_WaitLocationRequestStatusL(TLbsPositionUpdateRequestBase::EPowerAdviceStandby);
	X3pClient_SendX3pRequestL(1);
	// now X3P Status X3P is Active
	TUint x3pSessionId;
	NetGateway_WaitX3pRequestL(x3pSessionId);
	// when X3P Connected and there are combined location request from NRH is IDLE, power advice is ON
	AgpsManager_WaitLocationRequestStatusL(TLbsPositionUpdateRequestBase::EPowerAdviceOn);
	NetGateway_SendNetworkReferencePositionL(x3pSessionId, TLbsNetworkEnumInt::EServiceTransmitThirdParty);
	X3pClient_WaitX3pReferencePositionL(1);
	NetGateway_SendLocationRequestL(x3pSessionId, TLbsNetworkEnumInt::EServiceTransmitThirdParty);
	AgpsManager_WaitLocationRequestL();
	AgpsManager_SendGpsPositionUpdateAccurateL();
	NetGateway_WaitLocationResponseL(x3pSessionId, KErrNone);
	NetGateway_SendNetworkFinalPositionL(x3pSessionId, TLbsNetworkEnumInt::EServiceTransmitThirdParty);

	NetGateway_SendSessionCompleteL(x3pSessionId, KErrNone, TLbsNetworkEnumInt::EServiceTransmitThirdParty);
	AgpsManager_WaitLocationRequestCancelL(KErrNotFound);
	
	X3pClient_WaitX3pCompleteL(1, KErrNone);
	
    if(iLastPowerAdvice != TLbsPositionUpdateRequestBase::EPowerAdviceStandby)
         {
         // change from ON to Standby
         AgpsManager_WaitLocationRequestStatusL(TLbsPositionUpdateRequestBase::EPowerAdviceStandby);
         }
         
	X3pClient_DisconnectClientL(1);
	// now X3P Status X3P is Idle
	// and now Off because no X3P sessions connected
	AgpsManager_WaitLocationRequestStatusL(TLbsPositionUpdateRequestBase::EPowerAdviceOff);
	}

void CLbsNrhStateTestSingleStep::SingleSuplMtLrL()
	{
	// Standard SUPL MT-LR
	// TODO: Not yet updated properly
	NetGateway_SendPrivacyRequestL(12);
	PrivacyController_WaitPrivacyRequestL(12);
	PrivacyController_SendPrivacyResponseL(12, CLbsPrivacyController::ERequestAccepted);
	NetGateway_WaitPrivacyResponseL(12, TLbsNetworkEnumInt::EPrivacyResponseAccepted);
	AgpsManager_WaitLocationRequestStatusL();

	NetGateway_SendNetworkReferencePositionL(12, TLbsNetworkEnumInt::EServiceMobileTerminated);
	NetGateway_SendLocationRequestL(12, TLbsNetworkEnumInt::EServiceMobileTerminated);
	
	PrivacyController_WaitPrivacyReferencePositionL(12);
	AgpsManager_WaitLocationRequestL();
	AgpsManager_SendGpsPositionUpdateAccurateL();
	NetGateway_WaitLocationResponseL(12, KErrNone);
	PrivacyController_WaitPrivacyGpsPositionL(12);		
	NetGateway_SendSessionCompleteL(12, KErrNone, TLbsNetworkEnumInt::EServiceMobileTerminated);
	PrivacyController_WaitPrivacyCompleteL(12, KErrNone);
	AgpsManager_WaitLocationRequestCancelL();	
	}

void CLbsNrhStateTestSingleStep::SingleSuplMoLrL()
	{
	// Standard SUPL MO-LR
	NetGateway_SendLocationRequestL(13, TLbsNetworkEnumInt::EServiceSelfLocation);
	AgpsManager_WaitLocationRequestL();
	NetGateway_SendNetworkReferencePositionL(13, TLbsNetworkEnumInt::EServiceSelfLocation);
	NetGateway_SendLocationRequestL(13, TLbsNetworkEnumInt::EServiceSelfLocation);
	AgpsManager_WaitLocationRequestL();
	AgpsManager_SendGpsPositionUpdateAccurateL();
	NetGateway_WaitLocationResponseL(13, KErrNone);
	NetGateway_SendSessionCompleteL(13, KErrNone, TLbsNetworkEnumInt::EServiceSelfLocation);
	AgpsManager_WaitLocationRequestCancelL();	
	}

void CLbsNrhStateTestSingleStep::SingleMtLrMaxFixAgeL()
	{
	AgpsManager_SendGpsPositionUpdateAccurateL();
	NetGateway_SendPrivacyRequestL(14);
	PrivacyController_WaitPrivacyRequestL(14);
	PrivacyController_SendPrivacyResponseL(14, CLbsPrivacyController::ERequestAccepted);
	NetGateway_WaitPrivacyResponseL(14, TLbsNetworkEnumInt::EPrivacyResponseAccepted);
	AgpsManager_WaitLocationRequestStatusL();

	NetGateway_SendNetworkReferencePositionL(14, TLbsNetworkEnumInt::EServiceMobileTerminated);
	NetGateway_SendLocationRequestL(14, 
									TLbsNetworkEnumInt::EServiceMobileTerminated,
									KMaxFixAge);
	PrivacyController_WaitPrivacyReferencePositionL(14);

	NetGateway_WaitLocationResponseL(14, KErrNone);
	PrivacyController_WaitPrivacyGpsPositionL(14);
	NetGateway_SendSessionCompleteL(14, KErrNone, TLbsNetworkEnumInt::EServiceMobileTerminated);
	PrivacyController_WaitPrivacyCompleteL(14, KErrNone);
	AgpsManager_WaitLocationRequestStatusL();
	}

void CLbsNrhStateTestSingleStep::SingleMoLrMaxFixAgeL()
	{
	AgpsManager_SendGpsPositionUpdateAccurateL();
	NetGateway_SendNetworkReferencePositionL(15, TLbsNetworkEnumInt::EServiceSelfLocation);
	NetGateway_SendLocationRequestL(15, 
									TLbsNetworkEnumInt::EServiceSelfLocation, 
									KMaxFixAge);
	NetGateway_WaitLocationResponseL(15, KErrNone);
	NetGateway_SendSessionCompleteL(15, 
									KErrNone, 
									TLbsNetworkEnumInt::EServiceSelfLocation);
	}


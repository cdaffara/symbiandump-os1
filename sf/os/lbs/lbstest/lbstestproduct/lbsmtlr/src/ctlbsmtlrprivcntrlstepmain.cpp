// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// @file ctlbsmtlrprivcntrlstepmain.cpp
// This is the class implementation for the MTLR Privacy controller Tests
// 
//

// EPOC includes.
#include <e32property.h>


// LBS test includes.
#include "ctlbsmtlrprivcntrlstepmain.h"
#include <lbs/test/tlbsutils.h>

// Incorrect aRequestId
const TInt KBadRequestId = 696969;

const TTimeIntervalMicroSeconds KTimeToStallTestModule	= 120 * 1000000;	// 2 mins

_LIT8(KTestRequesterId, "MT-LR test requester id");
_LIT8(KTestClientName, "MT-LR test client name");
_LIT8(KTestClientExternalId, "MT-LR test client external id");

//profile_id in ini file
_LIT(KLbsProfileId, "profile_id");


TTimeIntervalMicroSeconds32 KExtendedTimerInterval = 60*1000000;	// 1 minute

// by default, a test case will abort after this time if all callbacks not completed:
// Note that this must be less than the TEF timeout in the script to be of any value
TTimeIntervalMicroSeconds32 KDefaultAbortTimerPeriod = 90*1000000;	// 1.5 mins 

// by default, a test case running with real gps hardware will abort after this time 
// Note that this must be less than the TEF timeout in the script to be of any value
TTimeIntervalMicroSeconds32 KDefaultAbortTimerPeriod_real = 300*1000000;	// 5 mins

/**
Static Constructor
*/
CT_LbsMTLRPrivCntrlStep_Main* CT_LbsMTLRPrivCntrlStep_Main::New(CT_LbsMTLRServer& aParent)
	{
	// Note the lack of ELeave.
	// This means that having insufficient memory will return NULL;
	CT_LbsMTLRPrivCntrlStep_Main* testStep = new CT_LbsMTLRPrivCntrlStep_Main(aParent);
	if (testStep)
		{
		TInt err = KErrNone;

		TRAP(err, testStep->ConstructL());
		if (err)
			{
			delete testStep;
			testStep = NULL;
			}
		}

	return testStep;
	}


/**
 * Constructor
 */
CT_LbsMTLRPrivCntrlStep_Main::CT_LbsMTLRPrivCntrlStep_Main(CT_LbsMTLRServer& aParent) : CT_LbsMTLRStep(aParent)
	{
	SetTestStepName(KLbsMTLRPrivCntrlStep_Main);
	}


void CT_LbsMTLRPrivCntrlStep_Main::ConstructL()
	{
	// Create the base class objects.
	CT_LbsMTLRStep::ConstructL();
	
	// Create object for changing admin settings:		
	iLbsAdmin = CLbsAdmin::NewL(*this, KLbsSettingHomeExternalLocate | KLbsSettingRoamingExternalLocate | KLbsSettingHomeEmergencyLocate | KLbsSettingRoamingEmergencyLocate);
	// Create object for privacy controller api:
	iLbsPrivacyController = CLbsPrivacyController::NewL(*this);
		
	}


/**
 * Destructor
 */
CT_LbsMTLRPrivCntrlStep_Main::~CT_LbsMTLRPrivCntrlStep_Main()
	{
	delete iLbsAdmin;
	iLbsAdmin = NULL;
	delete iLbsPrivacyController;
	iLbsPrivacyController = NULL;
	}


/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
TVerdict CT_LbsMTLRPrivCntrlStep_Main::doTestStepL()
	{
	// Generic test step used to test the LBS Client Notify position update API.
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsMTLRPrivCntrlStep_Main::doTestStepL()"));

	if (TestStepResult() == EPass)
		{		
		// Carry out common test actions (such as connecting to a server).
		T_LbsUtils utils;
			
		// set default values									
		// Expected callbacks flags, most test steps require MT-LR and privacy controller.
		iFlagsToHaltOn =
							KLbsCallback_NetSim_Got_Connect | 		// Net Sim callback events.
							KLbsCallback_PrivCntrl |
							KLbsCallback_MtLr;						
				
		iExpectedControlFailureReason = KErrNone;									// we usually don't expect a control failure to happen
		iConfigureEmergencyRequest = EFalse;											// kick off non-emergency request
		iPrivResult = CLbsPrivacyController::ERequestAccepted;						// accept the privacy request
		iExpectedNetworkResult = CLbsNetworkProtocolBase::EPrivacyResponseAccepted;	// default is to expect to abide with priv controller's response						
		iConfiguredRequestAdvice = TLbsNetPosRequestPrivacy::ERequestAdviceNotify;	// want netsim to do privacy request of type 'accept and notify'
		iConfiguredRequestAction = TLbsNetPosRequestPrivacy::ERequestActionAllow;	// as above
		iConfiguredExtLocateService = CLbsAdmin::EExternalLocateOn;					// almost all TCs want external locate turned on
		iRespondToPrivacyRequest = ETrue;
		iExpectedNetPosUpdCount	= 2;												// most TCs expect to get 2 position updates								
		iStartLocationRequest = ETrue;
		
		iUseBadRequestId = EFalse;
		iEarlyCancelWithBadId = EFalse;
		iRespondAndCancelWithBadId = EFalse;
		iLateCancelWithBadId = EFalse;
		iTestModuleStallTime = 0;
		iExtendedTimerTime = 0;
		iConfigureRoaming = EFalse;
		iConfigureProfile = ETrue;
		iProfileId = 3;
		TTimeIntervalMicroSeconds32 abortInterval;
		if(iParent.iSharedData->iTestModuleInUse)
			{
			abortInterval = KDefaultAbortTimerPeriod;
			}
		else
			{	
			abortInterval = KDefaultAbortTimerPeriod_real;
			}
	
		// configure unique test actions:
		if (GetIntFromConfig(ConfigSection(), KTestCaseId, iTestCaseId))
			{
			// TO DO: we could read all this from config file instead...?:
			switch (iTestCaseId)
				{
				case 1:	// Test case LBS-MTLR-Priv-Cntr-0001
					// use defaults
					break;
					
				case 2:	// Test case LBS-MTLR-Priv-Cntr-0002
					{
					iConfiguredRequestAdvice = TLbsNetPosRequestPrivacy::ERequestAdviceVerify;	// want netsim to do privacy request of type 'verify'
					}
					break;
					
				case 3:	// Test case LBS-MTLR-Priv-Cntr-0003
					{
					// since we reject the privacy request here, we do not expect these callbacks:
					iFlagsToHaltOn &= ~(KLbsCallback_Got_PrivCntrlNetworkPositionUpdate | 
										KLbsCallback_NetSim_Got_NotifyMeasurementControlLocation | 
										KLbsCallback_NetSim_Got_NotifyMeasurementReportLocation
										);
					iExpectedNetPosUpdCount = 0;
					iPrivResult = CLbsPrivacyController::ERequestRejected;
					iConfiguredRequestAdvice = TLbsNetPosRequestPrivacy::ERequestAdviceVerify;
					iConfiguredRequestAction = TLbsNetPosRequestPrivacy::ERequestActionAllow;
					iExpectedNetworkResult = CLbsNetworkProtocolBase::EPrivacyResponseRejected;
					iStartLocationRequest = EFalse;	// don't kick off a location request
					}
					break;
					
				case 4:	// Test case LBS-MTLR-Priv-Cntr-0004
					{
					ASSERT(FALSE);	// ERequestIgnore is unsupported currently (behaviour not defined)
					iPrivResult = CLbsPrivacyController::ERequestIgnore;
					}
					break;
					
				case 6:	// Test case LBS-MTLR-Priv-Cntr-0006
					{
					iRespondToPrivacyRequest = EFalse;
					}
					break;
					
				case 7:	// Test case LBS-MTLR-Priv-Cntr-0007
					{	
					iTestModuleStallTime = KTimeToStallTestModule;
					
					// Cancel request in first ProcessNetworkPositionUpdate()
					iCancelBeforeFinalPosUpd = ETrue;
					
					iExpectedNetPosUpdCount	= 1;
					
					iExtendedTimerTime = KExtendedTimerInterval;

					iFlagsToHaltOn &= ~(KLbsCallback_NetSim_Got_NotifyMeasurementReportLocation);
					
 					iFlagsToHaltOn |= KLbsCallback_NetSim_Got_NotifyMeasurementReportControlFailure;
 					iExpectedControlFailureReason = KErrCancel;										
					}
					break;
					
					
					
				case 8:	// Test case LBS-MTLR-Priv-Cntr-0008
					{
					// Negative test on RespondNetworkLocationRequest with incorrect parameter.
					// Pass bad request value to perform negative test
					iUseBadRequestId = ETrue; 			// Use bad Id to...
					iRespondWithBadId = ETrue;
					}
					break;
				
				case 9:	// Test case LBS-MTLR-Priv-Cntr-0009
					{
					// Negative test on CancelNetworkLocationRequest with incorrect parameter.
					// Pass bad request value to perform negative test			
			
					//Pass bad request value to perform negative test
					iUseBadRequestId = ETrue; 			// Use bad Id to...
					iEarlyCancelWithBadId = ETrue; 		// respond or cancel
					}
					break;
					
				case 10:// Test case LBS-MTLR-Priv-Cntr-00010
					{
					// Call RespondNetworkLocationRequest passing ERequestAccepted as a parameter.
					// Call CancelNetworkLocationRequest passing an invalid aRequestId parameter straight after. 
					iRespondAndCancelWithBadId = ETrue;
					
					//Pass bad request value to perform negative test
					iUseBadRequestId = ETrue; 		// Use bad Id to...
					iRespondAndCancelWithBadId = ETrue;	// or both					
					}
					break;
				
				case 11:// Test case LBS-MTLR-Priv-Cntr-00011
					{
					//Pass bad request value to perform negative test
					iUseBadRequestId = ETrue; 			// Use bad Id to...
					iLateCancelWithBadId = ETrue;			// or cancel in the second callback
					}
					break;
					
				case 12:// Test case LBS-MTLR-Priv-Cntr-0012					
					{
					iConfiguredRequestAdvice = TLbsNetPosRequestPrivacy::ERequestAdviceVerify;	// want netsim to do privacy request of type 'verify'
					iPrivResult = CLbsPrivacyController::ERequestRejected;
					iExpectedNetworkResult = CLbsNetworkProtocolBase::EPrivacyResponseRejected;
					
					// since we reject the privacy request here, we do not expect these callbacks:
					iFlagsToHaltOn &= ~(KLbsCallback_Got_PrivCntrlNetworkPositionUpdate | 
										KLbsCallback_NetSim_Got_NotifyMeasurementReportLocation
										);
										
					iFlagsToHaltOn |= KLbsCallback_NetSim_Got_NotifyMeasurementReportControlFailure;
					iExpectedControlFailureReason = KErrPermissionDenied;					
					iExpectedNetPosUpdCount = 0;
					}
					break;
					
				case 13:// Test case LBS-MTLR-Priv-Cntr-0013
					{
					iConfiguredRequestAdvice = TLbsNetPosRequestPrivacy::ERequestAdviceSilent;	// kick-off a 'silent' request
					}
					break;
				
				case 14:	// Test case LBS-MTLR-Priv-Cntr-0014
					{					
					iConfiguredExtLocateService = CLbsAdmin::EExternalLocateOff;
					iFlagsToHaltOn &= ~(KLbsCallback_PrivCntrl | 
										KLbsCallback_NetSim_Got_NotifyMeasurementControlLocation | 
										KLbsCallback_NetSim_Got_NotifyMeasurementReportLocation
										);
					
					iExpectedNetPosUpdCount	= 0;
					iStartLocationRequest = EFalse;	// do not kick off a location request
					iExpectedNetworkResult = CLbsNetworkProtocolBase::EPrivacyResponseRejected;	// lbs should reject															
					}
					break;
					
				case 15:	// Test case LBS-MTLR-Priv-Cntr-0015
					{
					iConfiguredRequestAdvice = TLbsNetPosRequestPrivacy::ERequestAdviceVerify;	// want netsim to do privacy request of type 'verify, reject if no response'
					iConfiguredRequestAction = TLbsNetPosRequestPrivacy::ERequestActionReject;	// as above
			 		
					iRespondToPrivacyRequest = EFalse;
					iStartLocationRequest = EFalse;
					
					// since we reject the privacy request here, we do not expect these callbacks:
					iFlagsToHaltOn &= ~(KLbsCallback_Got_PrivCntrlNetworkPositionUpdate | 
										KLbsCallback_NetSim_Got_NotifyMeasurementControlLocation | 
										KLbsCallback_NetSim_Got_NotifyMeasurementReportLocation
										);
										
					iExpectedNetPosUpdCount = 0;	
					iExpectedNetworkResult = CLbsNetworkProtocolBase::EPrivacyResponseRejected;	
					}
					break;
					
				case 16:	// Test case LBS-MTLR-Priv-Cntr-0016
					{
					iConfiguredExtLocateService = CLbsAdmin::EExternalLocateOffButNotify;
					iConfiguredRequestAction = TLbsNetPosRequestPrivacy::ERequestActionReject;
					iConfiguredRequestAdvice = TLbsNetPosRequestPrivacy::ERequestAdviceNotify;

					iRespondToPrivacyRequest = EFalse;
					iStartLocationRequest = EFalse;
					
					// Since the NRH should reject the privacy request, we don't expect these callbacks.
					iFlagsToHaltOn &= ~(KLbsCallback_Got_PrivCntrlNetworkPositionUpdate | 
										KLbsCallback_NetSim_Got_NotifyMeasurementControlLocation | 
										KLbsCallback_NetSim_Got_NotifyMeasurementReportLocation
										);
					iExpectedNetPosUpdCount = 0;
					
					iExpectedNetworkResult = CLbsNetworkProtocolBase::EPrivacyResponseRejected;
					}
					break;
					
				case 17:	// Test case LBS-MTLR-Priv-Cntr-0017
					{
					iConfiguredExtLocateService = CLbsAdmin::EExternalLocateOnButAlwaysVerify;
					iConfiguredRequestAction = TLbsNetPosRequestPrivacy::ERequestActionAllow;
					iConfiguredRequestAdvice = TLbsNetPosRequestPrivacy::ERequestAdviceVerify;
					}
					break;	
				
				case 18:	// Test case LBS-MTLR-Priv-Cntr-0018
					{
					iPrivResult = CLbsPrivacyController::ERequestUnknown;	// using this to indicate we should cancel request
					// since we cancel the privacy request here, we do not expect these callbacks:
					iFlagsToHaltOn &= ~(KLbsCallback_Got_PrivCntrlNetworkPositionUpdate | 
										KLbsCallback_NetSim_Got_NotifyMeasurementControlLocation |		// location request
										KLbsCallback_NetSim_Got_NotifyMeasurementReportLocation
										);
					iExpectedNetPosUpdCount = 0;
					iConfiguredRequestAdvice = TLbsNetPosRequestPrivacy::ERequestAdviceVerify;
					iConfiguredRequestAction = TLbsNetPosRequestPrivacy::ERequestActionAllow;
					iExpectedNetworkResult = CLbsNetworkProtocolBase::EPrivacyResponseRejected;
					iStartLocationRequest = EFalse;
					}
					break;
					
				case 19:	// Test case LBS-MTLR-Priv-Cntr-0019
					{			
					iUseBadRequestId = ETrue; 			
					iEarlyCancelWithBadId = ETrue; 	
					iRespondToPrivacyRequest = EFalse;										
					}
					break;
				case 20:	// Test case LBS-MTLR-Priv-Cntr-0020
					{
					// Call RespondNetworkLocationRequest passing ERequestAccepted as a parameter.
					// Call CancelNetworkLocationRequest passing an valid aRequestId parameter straight after. 
					iRespondToPrivacyRequest = ETrue;
					iCancelAfterResponding = ETrue;
					iStartLocationRequest = ETrue;
					iExpectedControlFailureReason = KErrCancel;
					iExpectedNetPosUpdCount = 0;
					
					iFlagsToHaltOn = (KLbsCallback_NetSim_Got_Connect |
									  KLbsCallback_NetSim_Got_NotifyMeasurementControlLocation |
									  KLbsCallback_NetSim_Got_NotifyReleaseLcsLocationNotification |
									  KLbsCallback_NetSim_Got_NotifyMeasurementReportControlFailure |
									  KLbsCallback_Got_PrivCntrlNetworkLocationRequest |
									  KLbsCallback_Got_PrivCntrlRequestComplete);
					}
					break;
					
				case 21:	// Test case LBS-MTLR-Priv-Cntr-Emergency-0001
					{
					iConfigureEmergencyRequest = ETrue;	
					}
					break;
				case 22:	// Test case LBS-MTLR-Priv-Cntr-Emergency-0002
					{
					iConfigureEmergencyRequest = ETrue;
					iPrivResult = CLbsPrivacyController::ERequestRejected;	// reject the privacy request
					}
					break;
					
				case 23:	// Test case LBS-MTLR-Priv-Cntr-Emergency-0003
					{
					iConfigureEmergencyRequest = ETrue;
					iConfiguredRequestAdvice = TLbsNetPosRequestPrivacy::ERequestAdviceVerify;	// kick-off a 'verify/reject' request
					iConfiguredRequestAction = TLbsNetPosRequestPrivacy::ERequestActionReject;	// as above
					iPrivResult = CLbsPrivacyController::ERequestRejected;	
					}
					break;
					
				case 24:	// Test case LBS-MTLR-Priv-Cntr-Emergency-0004
					{
					iConfigureEmergencyRequest = ETrue;
					iPrivResult = CLbsPrivacyController::ERequestUnknown;	// using this to indicate we should cancel request
					}
					break;

				case 25:	// Test case LBS-MTLR-Priv-Cntr-Emergency-0005
					{
					iConfigureEmergencyRequest = ETrue;
					iConfiguredRequestAdvice = TLbsNetPosRequestPrivacy::ERequestAdviceVerify;	// kick-off a 'verify/reject' request
					iRespondToPrivacyRequest = EFalse;											// don't respond to privacy request
					}
					break;
					
				case 26:	// Test case LBS-MTLR-Priv-Cntr-Emergency-0006
					{
					iConfigureEmergencyRequest = ETrue;
					iConfiguredRequestAdvice = TLbsNetPosRequestPrivacy::ERequestAdviceSilent;	// kick-off a 'silent' request					
					}
					break;					
					
				case 27:	// Test case LBS-MTLR-Priv-Cntr-Emergency-0007
					{					
					iExtendedTimerTime = KExtendedTimerInterval;
					iConfigureEmergencyRequest = ETrue;
					iConfiguredExtLocateService = CLbsAdmin::EExternalLocateOff;
					iFlagsToHaltOn &= ~(KLbsCallback_PrivCntrl | 
										KLbsCallback_NetSim_Got_NotifyMeasurementControlLocation | 
										KLbsCallback_NetSim_Got_NotifyMeasurementReportLocation
										);
					
					iExpectedNetPosUpdCount	= 0;
					iStartLocationRequest = EFalse;	// do not kick off a location request
					iExpectedNetworkResult = CLbsNetworkProtocolBase::EPrivacyResponseRejected;	// lbs should reject					
					}
					break;
					
				case 28:	// Test case LBS-MTLR-Priv-Cntr-Emergency-0008
					{
					iConfiguredExtLocateService = CLbsAdmin::EExternalLocateOnButAlwaysVerify;					
					iConfigureEmergencyRequest = ETrue;
					iConfiguredRequestAdvice = TLbsNetPosRequestPrivacy::ERequestAdviceVerify;	// kick-off a 'verify/reject' request
					iConfiguredRequestAction = TLbsNetPosRequestPrivacy::ERequestActionReject;	// as above
					iPrivResult = CLbsPrivacyController::ERequestRejected;						// reject the privacy request						
					}
					break;
							   
			   			   			   
			   case 1001:	// Test case LBS-MTLR-Priv-Cntr-Roaming-0001	Roaming. Request while ExternalLocateOn
					{
					iConfigureRoaming = ETrue;
					}
					break;
					
			   case 1002:	// Test case LBS-MTLR-Priv-Cntr-Roaming-0002	Roaming. Request while ExternalLocateOn
					{
					iConfigureRoaming = ETrue;
					iConfiguredExtLocateService = CLbsAdmin::EExternalLocateOff;
					iFlagsToHaltOn &= ~(KLbsCallback_PrivCntrl | 
										KLbsCallback_NetSim_Got_NotifyMeasurementControlLocation | 
										KLbsCallback_NetSim_Got_NotifyMeasurementReportLocation
										);
					
					iExpectedNetPosUpdCount	= 0;
					iStartLocationRequest = EFalse;	// do not kick off a location request
					iExpectedNetworkResult = CLbsNetworkProtocolBase::EPrivacyResponseRejected;	// lbs should reject
					
					}
					break;			
					
				case 2001:	// Test case LBS-MTLR-Priv-Cntr-Emergency-Roaming-0001	Roaming. Emergency request while EmergencyLocateOn
					{
					iConfigureRoaming = ETrue;
					iConfigureEmergencyRequest = ETrue;
					}
					break;	

				case 2002:	// Test case LBS-MTLR-Priv-Cntr-Emergency-Roaming-0002	Roaming. Emergency request while EmergencyLocateOff
					{
					iConfigureRoaming = ETrue;
					iConfigureEmergencyRequest = ETrue;
					iConfiguredExtLocateService = CLbsAdmin::EExternalLocateOff;
					iFlagsToHaltOn &= ~(KLbsCallback_PrivCntrl | 
										KLbsCallback_NetSim_Got_NotifyMeasurementControlLocation | 
										KLbsCallback_NetSim_Got_NotifyMeasurementReportLocation
										);
					
					iExpectedNetPosUpdCount	= 0;
					iStartLocationRequest = EFalse;	// do not kick off a location request
					iExpectedNetworkResult = CLbsNetworkProtocolBase::EPrivacyResponseRejected;	// lbs should reject					
					}
					break;
							    			   				   	
			   case 3001:// LBS-MTLR-Quality-Profile-0001 
					{
					// Read the profile ID from test config and fail if not found there
					TBool profileid_exists;
			        profileid_exists = GetIntFromConfig(ConfigSection(), KLbsProfileId, iProfileId);
		            if(!profileid_exists)
						{
						INFO_PRINTF1(_L("Could not get profileid from section"));
						SetTestStepResult(EFail);
						} 
					iConfigureProfile = ETrue;
					}
				    break;

			   case 3005:	// LBS-MTLR-Quality-Profile-0005
			   case 3006:	// LBS-MTLR-Quality-Profile-0006
				   	{
			   		iConfigureProfile = EFalse;	// taken from admin cenrep
				   	}
			   		break;
			   		
			   case 3007:	// LBS-MTLR-Quality-Profile-0007
					{
					iConfigureProfile = EFalse;	// taken from admin cenrep
					// the privacy request is accepted, but location request fails due to lack of quality params:
 					iFlagsToHaltOn &= ~(KLbsCallback_NetSim_Got_NotifyMeasurementReportLocation	);
 					iFlagsToHaltOn |= KLbsCallback_NetSim_Got_NotifyMeasurementReportControlFailure;
 					iExpectedControlFailureReason = KErrAccessDenied;	//
 					
					iExpectedNetPosUpdCount = 1;			// we get the ref pos, but no FNP
					}
					break;			
					
			   case 3003:	// LBS-MTLR-Timeout-0001
					{
					iFlagsToHaltOn &= ~(KLbsCallback_NetSim_Got_NotifyMeasurementReportLocation);
 					iFlagsToHaltOn |= KLbsCallback_NetSim_Got_NotifyMeasurementReportControlFailure;
					iExpectedControlFailureReason = KPositionQualityLoss;
					}
				    break;

				case 4001:// Test case LBS-MTLR-Priv-Cntr-SilentReject-0001
					{
					// kick-off a 'silent/reject' request
					iConfiguredRequestAdvice = TLbsNetPosRequestPrivacy::ERequestAdviceSilent;	
					iConfiguredRequestAction = TLbsNetPosRequestPrivacy::ERequestActionReject;	
					
					iRespondToPrivacyRequest = EFalse;	// don't need to respond as LBS will do that automatically for 'silent'
					// note that even tho' the action is reject here, we expect it to be accepted for 'custom1' behaviour!
					}
					break;
					
				case 4002:// Test case LBS-MTLR-Priv-Cntr-SilentReject-0002
					{
					// kick-off a 'silent/reject' request
					iConfiguredRequestAdvice = TLbsNetPosRequestPrivacy::ERequestAdviceSilent;
					iConfiguredRequestAction = TLbsNetPosRequestPrivacy::ERequestActionReject;	
					
					iRespondToPrivacyRequest = EFalse;	// don't need to respond as LBS will do that automatically for 'silent'					
					iStartLocationRequest = EFalse;
					
					// since privacy request is rejected, we do not expect these callbacks:
					iFlagsToHaltOn &= ~(KLbsCallback_Got_PrivCntrlNetworkPositionUpdate | 
										KLbsCallback_NetSim_Got_NotifyMeasurementControlLocation | 
										KLbsCallback_NetSim_Got_NotifyMeasurementReportLocation
										);
										
					iExpectedNetPosUpdCount = 0;	
					iExpectedNetworkResult = CLbsNetworkProtocolBase::EPrivacyResponseRejected;						
					}
					break;			
				
				case 5001:	// LBS-MTLR-Priv-Cntr-AlwaysVerify-Timeout-0001  (behaviourmode = 'custom1')
					{
					iConfiguredExtLocateService = CLbsAdmin::EExternalLocateOnButAlwaysVerify;
					iConfiguredRequestAction = TLbsNetPosRequestPrivacy::ERequestActionReject;
					iConfiguredRequestAdvice = TLbsNetPosRequestPrivacy::ERequestAdviceNotify;	
					
					iRespondToPrivacyRequest = EFalse;	// want lbs to timeout
					iStartLocationRequest = EFalse;
					
					// privacy request is rejected on timeout, we do not expect these callbacks:
					iFlagsToHaltOn &= ~(KLbsCallback_Got_PrivCntrlNetworkPositionUpdate | 
										KLbsCallback_NetSim_Got_NotifyMeasurementControlLocation | 
										KLbsCallback_NetSim_Got_NotifyMeasurementReportLocation
										);
										
					iExpectedNetPosUpdCount = 0;	
					iExpectedNetworkResult = CLbsNetworkProtocolBase::EPrivacyResponseRejected;					
					}
					break;
					
				case 5002:	// LBS-MTLR-Priv-Cntr-AlwaysVerify-Timeout-0002	(behaviourmode = 'vanilla')
					{
					iConfiguredExtLocateService = CLbsAdmin::EExternalLocateOnButAlwaysVerify;
					iConfiguredRequestAction = TLbsNetPosRequestPrivacy::ERequestActionAllow;
					iConfiguredRequestAdvice = TLbsNetPosRequestPrivacy::ERequestAdviceNotify;	
					
					iRespondToPrivacyRequest = EFalse;	// want lbs to timeout
					
					// privacy request should be accepted on timeout
					}
					break;
					
				case 5003:	// LBS-MTLR-Priv-Cntr-AlwaysVerify-Timeout-0003 (behaviourmode = 'vanilla')
					{
					iConfiguredExtLocateService = CLbsAdmin::EExternalLocateOnButAlwaysVerify;
					iConfiguredRequestAction = TLbsNetPosRequestPrivacy::ERequestActionAllow;
					iConfiguredRequestAdvice = TLbsNetPosRequestPrivacy::ERequestAdviceNotify;	
					
					iRespondToPrivacyRequest = EFalse;	// want lbs to timeout
					iStartLocationRequest = EFalse;
					
					// privacy request is rejected on timeout, we do not expect these callbacks:
					iFlagsToHaltOn &= ~(KLbsCallback_Got_PrivCntrlNetworkPositionUpdate | 
										KLbsCallback_NetSim_Got_NotifyMeasurementControlLocation | 
										KLbsCallback_NetSim_Got_NotifyMeasurementReportLocation
										);
										
					iExpectedNetPosUpdCount = 0;	
					iExpectedNetworkResult = CLbsNetworkProtocolBase::EPrivacyResponseRejected;						
					}
					break;
					
			   	default:
					{
					INFO_PRINTF2(_L("Unrecognised test case id %d"), iTestCaseId);
					ASSERT(FALSE);	
					}
					break;
				}
			}
		else
			{
			INFO_PRINTF1(_L("Unrecognised test case"));
			ASSERT(FALSE);
			}
		
		// Carry out the test.
		if((iTestModuleStallTime > 0) && (iParent.iSharedData->iTestModuleInUse))
			{
			// Configure test agps module to stall on next location request:
			TPckgBuf<TModuleDataIn> modDataInBuf;
			TModuleDataIn& modDataIn = modDataInBuf();
			
			modDataIn.iRequestType = TModuleDataIn::EModuleRequestTimeOut; 
			modDataIn.iTimeOut =  iTestModuleStallTime;
			
			User::LeaveIfError(RProperty::Set(KUidSystemCategory, ELbsTestAGpsModuleIn, modDataInBuf));
			}
			
		if(iExtendedTimerTime.Int() > 0)
			{
			// Set an extended timer interval to make sure we don't receive unexpected callbacks:
			// This will cause the test to continue running for this period after all expected flags have been set
			SetExtendedTimerInterval(iExtendedTimerTime);
			}
			
		// Connect to net sim.
		iNetSim.ConnectL(this);

		// Create reference location:
		// Location to use.
		RPointerArray<TAny>& srcPosInfoArr = iParent.iSharedData->iVerifyPosInfoArr;
		TPositionInfo* srcPosInfo = reinterpret_cast<TPositionInfo*>(srcPosInfoArr[0]);
		TPosition srcPos;
		srcPosInfo->GetPosition(srcPos);
	
		if (!iNetSim.SetReferenceLocation(srcPos))
			{
			INFO_PRINTF1(_L("Failed test, can't set NetSim's reference location."));
			SetTestStepResult(EFail);
			iNetSim.Close();
			return TestStepResult();
			}	
	
		TLbsNetPosRequestQuality netPosQuality;
		// LBS-MTLR-Quality-Profile-0001, -0002, -0004, -0007:  
		if(iTestCaseId==3001 || iTestCaseId==3002 || iTestCaseId==3005 || iTestCaseId==3007)
			{
			// Set all parameters invalid (0 for max fix time; NaN for
			// accuracy) then network should use quality profile with the 
			// ID set
			TTimeIntervalMicroSeconds maxFixTime(0);	
			netPosQuality.SetMaxFixTime(maxFixTime);
    		TRealX nan;
    		nan.SetNaN();
			netPosQuality.SetMinHorizontalAccuracy(nan);
	    	netPosQuality.SetMinVerticalAccuracy(nan);    		
			}
			
		else if (iTestCaseId==3003) //LBS-MTLR-Timeout-0001
			{
			TTimeIntervalMicroSeconds maxFixTime(5000000);
			netPosQuality.SetMaxFixTime(maxFixTime);
			netPosQuality.SetMinHorizontalAccuracy(1);
	    	netPosQuality.SetMinVerticalAccuracy(1);
			}
		
		else
			{
			//Set the position quality required by the network.
			TTimeIntervalMicroSeconds maxFixTime(600 * 1000000);
			netPosQuality.SetMaxFixTime(maxFixTime);
			netPosQuality.SetMinHorizontalAccuracy(srcPos.HorizontalAccuracy());
	    	netPosQuality.SetMinVerticalAccuracy(srcPos.VerticalAccuracy());
			}
		if (!iNetSim.SetQuality(netPosQuality))
			{
			INFO_PRINTF1(_L("Failed test, can't set NetSim's quality."));
			SetTestStepResult(EFail);
			iNetSim.Close();
			return TestStepResult();
			}

		// Set plugin to use.
		TUid pluginUid;
		if(iParent.iSharedData->iTestModuleInUse)
			{
			pluginUid = TUid::Uid(KSimpleAssistanceDataProviderPluginUidValue);
			}
		else
			{
			pluginUid = TUid::Uid(KSuplAssistanceDataProviderPluginUidValue);
			}
			
		if (!iNetSim.SetAssistanceDataProvider(pluginUid))
			{
			INFO_PRINTF1(_L("Failed test, can't set NetSim's assistance data plugin uid."));
			SetTestStepResult(EFail);
			iNetSim.Close();
			return TestStepResult();
			}
	
		// Kick off the keep alive timer
		TTimeIntervalMicroSeconds32 interval(KLbsKeepAlivePeriod);

		iKeepAliveTimer->SetTimer(interval);
		
		iAbortTimer->SetTimer(abortInterval);

		// Kick off test
		CActiveScheduler::Start();
		
		if(EDone != iState)
			{
			INFO_PRINTF2(_L("Failed test, finished in state %d"), iState);
			INFO_PRINTF3(_L("Callback flags currently set = 0x%x. Expecting 0x%x"), iCallbackFlags, iFlagsToHaltOn);
			SetTestStepResult(EFail);
			}
		else
			{
			INFO_PRINTF2(_L("Got all expected callbacks (0x%x)"), iCallbackFlags);			
			}

		if(iExpectedNetPosUpdCount != iProcessNetPosUpdCount)
			{
			INFO_PRINTF3(_L("<FONT><B>  Network Position Update Count mismatch>>  Expected : Received, %d : %d </B></FONT>"),
							iExpectedNetPosUpdCount,iProcessNetPosUpdCount);
			SetTestStepResult(EFail);
			}
		
		//Reset 'sticky' settings:
		if((iTestModuleStallTime > 0) && (iParent.iSharedData->iTestModuleInUse))
			{
			// Configure test agps module to stall on next location request:
			TPckgBuf<TModuleDataIn> modDataInBuf;
			TModuleDataIn& modDataIn = modDataInBuf();
			
			modDataIn.iRequestType = TModuleDataIn::EModuleRequestTimeOut; 
			modDataIn.iTimeOut =  0;
			
			User::LeaveIfError(RProperty::Set(KUidSystemCategory, ELbsTestAGpsModuleIn, modDataInBuf));
			}
			
		if(iExtendedTimerTime.Int() > 0)
			{
			// Set an extended timer interval to make sure we don't receive unexpected callbacks:
			// This will cause the test to continue running for this period after all expected flags have been set
			SetExtendedTimerInterval(0);
			}
			
		// Clean up.		
		iNetSim.ClearAssistanceDataFilters();
		iNetSim.Close();
		}

	INFO_PRINTF1(_L("&lt;&lt;CT_LbsMTLRPrivCntrlStep_Main::doTestStepL()"));

	return TestStepResult();
	}


/** NetSim callbacks given for a MoLr, which we invoke as a result of the notify position update.
*/
void CT_LbsMTLRPrivCntrlStep_Main::Connected()
	{
	// Call base implementation.
	CT_LbsNetSimStep::Connected();
	TInt err;
	
	if(iConfigureEmergencyRequest)	// emergency
		{
		if(iConfigureRoaming)	// roaming
			{
			// Set external locate service for home network and wait for notification:
			err = iLbsAdmin->Set(KLbsSettingRoamingEmergencyLocate, iConfiguredExtLocateService);
			}
		else	// home
			{
			// Set external locate service for roaming network and wait for notification:
			err = iLbsAdmin->Set(KLbsSettingHomeEmergencyLocate,iConfiguredExtLocateService);
			}
		}
	else	// non-emergency
		{
		
		if(iConfigureProfile)	// quality profile
			{
			//Set ExternalLocate quality profile & id then wait for notification
			err = iLbsAdmin->Set(KLbsSettingQualityProfileExternalLocate, TLbsQualityProfileId(iProfileId));
			}
		
		if(iConfigureRoaming)	// roaming
			{
			// Set external locate service on home network and wait for notification:
			err = iLbsAdmin->Set(KLbsSettingRoamingExternalLocate, iConfiguredExtLocateService);
			}
		else	// home
			{
			// Set external locate service on roaming network and wait for notification:
			err = iLbsAdmin->Set(KLbsSettingHomeExternalLocate, iConfiguredExtLocateService);
			}
		}
		
	iState = EAdminWait;
	INFO_PRINTF1(_L("iState -> EAdminWait"));
		
	// TO DO - not ideal, but necessary since we can't leave:
	if(KErrNone != err)
		{
		INFO_PRINTF2(_L("CT_LbsMTLRPrivCntrlStep_Main::Connected() Failed - unable to change admin setting. Err = %d"), err);
		SetTestStepResult(EFail);
		}
	}


/** NetSim has disconnected
*/
void CT_LbsMTLRPrivCntrlStep_Main::Disconnected()
	{
	// Call base implementation.
	CT_LbsNetSimStep::Disconnected();
	INFO_PRINTF1(_L("Got - NetSim Notify Disconnected - Callback Event."));
	}



/** Network has requested location
*/
void CT_LbsMTLRPrivCntrlStep_Main::NotifyMeasurementControlLocation(const TPositionInfo& aPosition, 
															  const RLbsAssistanceDataBuilderSet& aData, 
															  const TLbsNetPosRequestQuality& aQuality)
	{
	// Call base implementation.
	CT_LbsNetSimStep::NotifyMeasurementControlLocation(aPosition, aData, aQuality);

	(void)aQuality;
		
	}

/** Response to privacy request has been sent to network
*/
void CT_LbsMTLRPrivCntrlStep_Main::NotifyReleaseLcsLocationNotification(const CLbsNetworkProtocolBase::TLbsPrivacyResponse& aResult)
	{
	// Call base implementation.
	CT_LbsNetSimStep::NotifyReleaseLcsLocationNotification(aResult);
	if (iCancelAfterResponding)
		{
		iLbsPrivacyController->CancelNetworkLocationRequest(iSessionId);
		}

	// We should get this callback only if kicked off a privacy request.
	ASSERT(iState == EStartMTLRPrivReq);				
			
	if(aResult != iExpectedNetworkResult)	
		{
		INFO_PRINTF3(_L("CT_LbsMTLRPrivCntrlStep_Main::NotifyReleaseLcsLocationNotification() Failed: Expected result %d, got %d"), iExpectedNetworkResult, aResult);
		SetTestStepResult(EFail);		
		}
	// Tell NetSim to do location request?
	else if(iStartLocationRequest) 
		{
		iNetSim.StartNetworkLocationRequest();		
		iState = EStartMTLRLocReq;
		INFO_PRINTF1(_L("iState -> EStartMTLRLocReq"));
		}
	else
		{
		iState = EWaiting;
		INFO_PRINTF1(_L("iState -> EWaiting"));
		}
	}


/** Location has been sent to network
*/
void CT_LbsMTLRPrivCntrlStep_Main::NotifyMeasurementReportLocation(const TPositionInfo& aPosition)
	{	
	// Call base implementation.
	CT_LbsNetSimStep::NotifyMeasurementReportLocation(aPosition);
	
	// MT-LR complete.
	if(iState != ENetPosUpdCountWait)
		{
		ASSERT((EStartMTLRLocReq == iState) || (EWaiting == iState));
		iState = EWaiting;	
		INFO_PRINTF1(_L("iState -> EWaiting"));
		}

	
	// Verify the position given to the network:
	T_LbsUtils utils;
	RPointerArray<TAny>& verifyPosInfoArr = iParent.iSharedData->iVerifyPosInfoArr;
	TPositionInfo* verifyPosInfo = reinterpret_cast<TPositionInfo*>(verifyPosInfoArr[0]);
	
	TBool res;
	if(iParent.iSharedData->iTestModuleInUse)
		{
		res = utils.Compare_PosInfo(*verifyPosInfo, aPosition, T_LbsUtils::EExactAccuracy);
		}
	else
		{
		res = utils.Compare_PosInfo(*verifyPosInfo, aPosition, T_LbsUtils::ERoughAccuracy);
		}
	if(!res)
		{
		INFO_PRINTF1(_L("Failed test, position incorrect."));
		SetTestStepResult(EFail);
		}
	}


/** Location failure sent to network
*/
void CT_LbsMTLRPrivCntrlStep_Main::NotifyMeasurementReportControlFailure(TInt aReason)
	{
	// Call base implementation.
	CT_LbsNetSimStep::NotifyMeasurementReportControlFailure(aReason);

	if(KErrNone == iExpectedControlFailureReason)
		{
		INFO_PRINTF2(_L("Failed test, got unexpected control failure, with reason code %d"), aReason);
		SetTestStepResult(EFail);
		}
	else if(iExpectedControlFailureReason != aReason)
		{
		INFO_PRINTF3(_L("Failed test, got expected control failure, but wrong reason code = expected %d, got %d"), iExpectedControlFailureReason, aReason);
		SetTestStepResult(EFail);
		}
	else
		{
		iState = EWaiting;
		INFO_PRINTF1(_L("iState -> EWaiting"));		
		}
	}


void CT_LbsMTLRPrivCntrlStep_Main::NotifyMeasurementReportRequestMoreAssistanceData(const TLbsAssistanceDataGroup& aFilter)
	{
	(void)aFilter;

	INFO_PRINTF1(_L("Got - Net Sim Notify Measurement Report Request More Assistance Data - Callback Event."));
	
	}



/** External Locate has been set
*/
void CT_LbsMTLRPrivCntrlStep_Main::OnSettingUpdateEvent(TInt aError, const TLbsAdminSetting& aSetting)
	{
	// We've only registered to be told about TExternalLocateService, so this is it
	if (aError == KErrNone)
		{
		TInt err;
		// LBS-MTLR-Quality-Profile-0001 or LBS-MTLR-Quality-Profile-0002 
		if (iTestCaseId==3001 || iTestCaseId==3002)
		    {
		    TLbsQualityProfileId profileId;
		  
		    // Verify that the profileid setting exists
		    err = iLbsAdmin->Get(KLbsSettingQualityProfileExternalLocate, profileId);
		    		    
		    if(KErrNone != err)
				{
				INFO_PRINTF2(_L("CT_LbsMTLRPrivCntrlStep_Main::OnSettingUpdateEvent() Failed - unable to get admin setting. Er = %d"), err);
				SetTestStepResult(EFail);
				}	
		    // Check profile Id is as we expect
		    if(profileId != TLbsQualityProfileId(iProfileId))
				{
				SetTestStepResult(EFail);
				}
		    }
		    
		else	
			{
			// Verify that the setting is what we expect
			CLbsAdmin::TExternalLocateService extLocService;
			err = iLbsAdmin->Get(aSetting, extLocService);
			
			// TO DO - not ideal, but necessary since we can't leave:
			if(KErrNone != err)
				{
				INFO_PRINTF2(_L("CT_LbsMTLRPrivCntrlStep_Main::OnSettingUpdateEvent() Failed - unable to get admin setting. Er = %d"), err);
				SetTestStepResult(EFail);
				}		
			
	        if(iConfiguredExtLocateService != extLocService) 
		        {
				INFO_PRINTF2(_L("CT_LbsMTLRPrivCntrlStep_Main::OnSettingUpdateEvent TExternalLocateService  not set successfully to %d"), extLocService);		
				SetTestStepResult(EFail);
	    		}
			}
				
		if((iConfiguredExtLocateService)  && (iState == EAdminWait))	
			{
			// start the MT-LR request (privacy first)
			TLbsNetPosRequestPrivacy reqType;
			TLbsExternalRequestInfo reqInfo; 	
			reqInfo.SetRequesterId(KTestRequesterId);	
			reqInfo.SetClientName(KTestClientName);
			reqInfo.SetClientExternalId(KTestClientExternalId);			
		
			reqType.SetRequestAction(iConfiguredRequestAction); 
			reqType.SetRequestAdvice(iConfiguredRequestAdvice);
			iNetSim.SetEmergenyStatus(iConfigureEmergencyRequest);
			iNetSim.SetRoamingStatus(iConfigureRoaming);
						
			// kick off the privacy request:
			iNetSim.StartNetworkPrivacyRequest(reqType, reqInfo);
			iState = EStartMTLRPrivReq;
			INFO_PRINTF1(_L("iState -> EStartMTLRPrivReq"));
			}
		}
	else
		{
		INFO_PRINTF2(_L("CT_LbsMTLRPrivCntrlStep_Main::OnSettingUpdateEvent Failed: Admin setting failed with err = %d."), aError);
		SetTestStepResult(EFail);
		}
	}
	

/** Verifies that RequestInfo passed matches what we set and prints supplied requestor details to TEF log
*/
void CT_LbsMTLRPrivCntrlStep_Main::VerifyRequestorDetails(const TLbsExternalRequestInfo& aRequestInfo)
	{
	TLbsRequesterId requesterId;
	TLbsClientName clientName;
	TLbsClientExternalId clientExternalId;
	
	aRequestInfo.GetRequesterId(requesterId);
	aRequestInfo.GetClientName(clientName);
	aRequestInfo.GetClientExternalId(clientExternalId);
	
	if(!((requesterId == KTestRequesterId)&& (clientName == KTestClientName) && (clientExternalId == KTestClientExternalId)))
		{
		INFO_PRINTF1(_L("CT_LbsMTLRPrivCntrlStep_Main::ProcessNetworkLocationRequest: Failed. Requester info not as expected"));
		SetTestStepResult(EFail);
		}
	}


/**
Called when a network privacy request is received by the LBS system.
The implementer of this function must decide whether the location request 
is allowed and give the result to the LBS system. The result is passed 
by calling CLbsPrivacyController::RespondNetworkLocationRequest().

@param aRequestId Identifies the request within the LBS system.
@param aRequestInfo Information about the external client making the request.
@param aNotificationType The value set by the network for the user notification.
*/
void CT_LbsMTLRPrivCntrlStep_Main::ProcessNetworkLocationRequest(TUint aRequestId, const TLbsExternalRequestInfo& aRequestInfo, const TNotificationType& aNotificationType)
	{
	INFO_PRINTF2(_L("CT_LbsMTLRPrivCntrlStep_Main::ProcessNetworkLocationRequest.RequestID = 0x%x"), aRequestId);	
	
	SetCallbackFlag(KLbsCallback_Got_PrivCntrlNetworkLocationRequest);
	
	// verify that requester id is what we set:
	VerifyRequestorDetails(aRequestInfo);
		
	iExpectedCompleteRequestId = aRequestId;
	
	if(iConfigureEmergencyRequest)
		{
		if((ENotifyLocationAccepted != aNotificationType))
			{
			INFO_PRINTF2(_L("CT_LbsMTLRPrivCntrlStep_Main::ProcessNetworkLocationRequest() Failed: expected notification type ENotifyLocationAccepted during emergency request. Got %d"), aNotificationType);
			SetTestStepResult(EFail);
			}
		}
	else	// non-emergency
		{	
		// Verify that the notification type is as we expected (configured in NetSim via TLbsNetPosRequestPrivacy below)
		switch(aNotificationType)
			{
			case ENotifyLocationAccepted:
				{
				// Note: the following assertion no longer holds true - in 'Custom1' behaviour we accept 'silent reject' requests!
				//ASSERT((TLbsNetPosRequestPrivacy::ERequestActionAllow == iConfiguredRequestAction) && ((TLbsNetPosRequestPrivacy::ERequestAdviceNotify == iConfiguredRequestAdvice) || (TLbsNetPosRequestPrivacy::ERequestAdviceSilent == iConfiguredRequestAdvice)));
				}
				break;
				
			case ENotifyAndVerifyLocationAcceptedIfNoReponse:
				{
				// This assertion isn't necessarily true - LBS will convert notify->verify if admin says "AlwaysVerify"
				// ASSERT((TLbsNetPosRequestPrivacy::ERequestActionAllow == iConfiguredRequestAction) && (TLbsNetPosRequestPrivacy::ERequestAdviceVerify == iConfiguredRequestAdvice));
				ASSERT(TLbsNetPosRequestPrivacy::ERequestActionAllow == iConfiguredRequestAction);
				}
				break;
				
			case ENotifyAndVerifyLocationRejectedIfNoReponse:
				{
				// The following isn't necessarily true. LBS will convert notify->verify if admin says "AlwaysVerify"
				// ASSERT((TLbsNetPosRequestPrivacy::ERequestActionReject == iConfiguredRequestAction) && (TLbsNetPosRequestPrivacy::ERequestAdviceVerify == iConfiguredRequestAdvice));
				ASSERT(TLbsNetPosRequestPrivacy::ERequestActionReject == iConfiguredRequestAction);
				}
				break;
				
			case ENotifyLocationRejected:
				{
				// This assertion does not hold true for 'custom1' behaviour where 'Silent + Reject' requests are accepted!
				//ASSERT((TLbsNetPosRequestPrivacy::ERequestActionReject == iConfiguredRequestAction) && (TLbsNetPosRequestPrivacy::ERequestAdviceNotify == iConfiguredRequestAdvice));
				}
				break;
				
			default:
				
				break;	
			}	
		}

	if(iRespondToPrivacyRequest)
		{
	
		if(CLbsPrivacyController::ERequestUnknown != iPrivResult)
			{		
			// respond to the privacy request:	
			iLbsPrivacyController->RespondNetworkLocationRequest(aRequestId, iPrivResult);
			}
		else	// we're using unknown to indicate that we should cancel
			{
			iLbsPrivacyController->CancelNetworkLocationRequest(aRequestId);
			}
		}
	if (iCancelAfterResponding)
		{
		iSessionId = aRequestId;
		}

	if(iUseBadRequestId && !iLateCancelWithBadId)
	{
	if(iRespondWithBadId)
		{
		// LBS-MTLR-Priv-Cntr-0008
		iLbsPrivacyController->RespondNetworkLocationRequest(KBadRequestId, iPrivResult);
		}
	if(iEarlyCancelWithBadId)
		{
		// LBS-MTLR-Priv-Cntr-0009
		iLbsPrivacyController->CancelNetworkLocationRequest(KBadRequestId);
		}
	if(iRespondAndCancelWithBadId)
		{		
		// LBS-MTLR-Priv-Cntr-0010
		iLbsPrivacyController->RespondNetworkLocationRequest(KBadRequestId, iPrivResult);
		iLbsPrivacyController->CancelNetworkLocationRequest(KBadRequestId);
		}
	}
	
	if(EExtendedWaiting == iState)
		{
		SetTestStepResult(EFail);
		INFO_PRINTF1(_L("CT_LbsMTLRPrivCntrlStep_Main::ProcessNetworkLocationRequest(): Failed. Called during extended waiting state."));
		}		
	}

/**
Called whenever a position update is ready while processing a network 
location request. Typically, it will be called twice:
1) As soon as the network reference position is available.
2) As soon as the final calculated position is available.

@param aRequestId Identifies the request within the LBS system.
@param aPosInfo The latest position data.
*/
void CT_LbsMTLRPrivCntrlStep_Main::ProcessNetworkPositionUpdate(TUint aRequestId, const TPositionInfo& aPosInfo)
	{
	INFO_PRINTF2(_L("CT_LbsMTLRPrivCntrlStep_Main::ProcessNetworkPositionUpdate.RequestID = 0x%x"), aRequestId);	
	
	SetCallbackFlag(KLbsCallback_Got_PrivCntrlNetworkPositionUpdate);
	
	iProcessNetPosUpdCount++;	
	
	if((iProcessNetPosUpdCount) && (iProcessNetPosUpdCount < iExpectedNetPosUpdCount))
		{
		iState = ENetPosUpdCountWait;
		INFO_PRINTF1(_L("iState -> ENetPosUpdCountWait"));
		}
	else if(iState == ENetPosUpdCountWait)
		{
		iState = EWaiting;
		INFO_PRINTF1(_L("iState -> EWaiting"));
		}
		
	TPosition pos;
	aPosInfo.GetPosition(pos);
	// Note: not verifying position here, since we do that later
	if(1 == iProcessNetPosUpdCount)
		{
		INFO_PRINTF4(_L("ProcessNetworkPositionUpdate called - network reference position available (lat, long, alt:) %LF, %LF, %f"), pos.Latitude(), pos.Longitude(), pos.Altitude());
		if(iCancelBeforeFinalPosUpd)
			{
			iLbsPrivacyController->CancelNetworkLocationRequest(aRequestId);
			iState = EWaiting;
			}
				
		if(iLateCancelWithBadId)
			{
			// LBS-MTLR-Priv-Cntr-0011
			iLbsPrivacyController->CancelNetworkLocationRequest(KBadRequestId);
			}
		}
	else
		{
		ASSERT(2 == iProcessNetPosUpdCount);
		INFO_PRINTF4(_L("ProcessNetworkPositionUpdate called - final calculated position available (lat, long, alt:) %LF, %LF, %f"), pos.Latitude(), pos.Longitude(), pos.Altitude());
		}

	if(EExtendedWaiting == iState)
		{
		SetTestStepResult(EFail);
		INFO_PRINTF1(_L("CT_LbsMTLRPrivCntrlStep_Main::ProcessNetworkPositionUpdate(): Failed. Called during extended waiting state."));
		}		
	}
	
/**
Called when a network location request (e.g. MT-LR) has been completed.

@param aRequestId Identifies the request within the LBS system.
@param aReason Contains the reason for the completion of the request.
               Usually KErrNone, or a standard Symbian error code.
               KErrNotFound if aRequestId is not a currently 
               active external location request.
               KErrCancel if the request was cancelled by a 
               call to CLbsPrivacyController::CancelNetworkLocationRequest().
*/
void CT_LbsMTLRPrivCntrlStep_Main::ProcessRequestComplete(TUint aRequestId, TInt aReason)
	{
	INFO_PRINTF3(_L("CT_LbsMTLRPrivCntrlStep_Main::ProcessRequestComplete.RequestID = 0x%x, aReason = %d"), aRequestId, aReason);	
	
	SetCallbackFlag(KLbsCallback_Got_PrivCntrlRequestComplete);
	
	if(iExpectedCompleteRequestId != aRequestId)
			    {
				SetTestStepResult(EFail);
				INFO_PRINTF3(_L("CT_LbsMTLRPrivCntrlStep_Main::ProcessRequestComplete() Failed: Expected RequestId %d and received %d"), iExpectedCompleteRequestId, aRequestId);		
			    }
	
	switch (iTestCaseId)
		{
		case 3002:	// LBS-MTLR-Quality-Profile-0002  Negative Test
		{
		if(KErrAccessDenied!= aReason)
			{
			SetTestStepResult(EFail);
			INFO_PRINTF2(_L("CT_LbsMTLRPrivCntrlStep_Main::ProcessRequestComplete() Failed: Expected KErrAccessDenied and received %d"), aReason);
			}	
		}
		break;
		
		case 3003:	// LBS-MTLR-Timeout-0001
		{
		if(aReason != KPositionQualityLoss)
			{
			SetTestStepResult(EFail);
			INFO_PRINTF2(_L("CT_LbsMTLRPrivCntrlStep_Main::ProcessRequestComplete() Failed: Expected KPositionQualityLoss and received %d"), aReason);
			}	
		}
		break;

		case 3007:
			{
			if(aReason != KErrAccessDenied)
				{
				SetTestStepResult(EFail);
				INFO_PRINTF2(_L("CT_LbsMTLRPrivCntrlStep_Main::ProcessRequestComplete() Failed: Expected KErrAccessDenied and received %d"), aReason);
				}			
			}
		break;
		default:
			{
			if(KErrNone!= aReason)
				{
				SetTestStepResult(EFail);
				INFO_PRINTF2(_L("CT_LbsMTLRPrivCntrlStep_Main::ProcessRequestComplete() Failed: Expected KErrNone and received %d"), aReason);
				}
			 	    
		    if(EExtendedWaiting == iState)
			    {
			    SetTestStepResult(EFail);
			    INFO_PRINTF1(_L("CT_LbsMTLRPrivCntrlStep_Main::ProcessRequestComplete(): Failed. Called during extended waiting state."));
			    }
		
		    }
		    break;
		    }
	
	}

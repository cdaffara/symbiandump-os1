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

// LBS includes. 
#include <lbs/test/lbsnetprotocolproxy.h>
#include <lbs/lbsnetprotocolbase.h>
#include <lbs/lbsassistancedatabuilderset.h>
#include <e32base.h>
// LBS test includes.
#include "ctlbsoom.h"
#include <lbs/test/tlbsutils.h>
#include "argutils.h"
#include <lbs/test/activeyield.h>


/**
Static Constructor
*/
CT_LbsOom* CT_LbsOom::New(CT_LbsHybridMTLRServer& aParent)
	{
	// Note that we do not use ELeave.
	// This means that having insufficient memory will return NULL;
	CT_LbsOom* testStep = new CT_LbsOom(aParent);
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
CT_LbsOom::CT_LbsOom(CT_LbsHybridMTLRServer& aParent) : CT_LbsHybridMTLRStep(aParent)
	{
	SetTestStepName(KLbsOom);

	iState = EInitializing;
	iSessionId.SetSessionOwner(KRequestUid);
	iSessionId.SetSessionNum(0x0005);
	
	iRefPosInfo = ArgUtils::ReferencePositionInfo();
	}


void CT_LbsOom::ConstructL()
	{
	// Create the base class objects.
	CT_LbsHybridMTLRStep::ConstructL();

	iController = CLbsPrivacyController::NewL(*this);
	iProxy = CNetProtocolProxy::NewL();	
	
	//Open the RProperty object for OOM
	iOomRequest.OpenL();
	}


/**
 * Destructor
 */
CT_LbsOom::~CT_LbsOom()
	{
	delete iController;
	delete iProxy;
	//Close the RProperty object of the module
	iOomRequest.Close();
	}


TVerdict CT_LbsOom::doTestStepPreambleL()
	{
	_LIT(KProcessprivReq, "processprivreq");
	_LIT(KService, "service");
	_LIT(KEmergency, "emergency");
	_LIT(KAdmin, "admin");
	_LIT(KResponse, "response");
	_LIT(KProcessPrivReqOpt, "Process Priv Req = %d");
	_LIT(KEmergencyOpt, "Emergency = %d.");
	_LIT(KAdminOpt, "Admin = %d.");
	_LIT(KResponseOpt, "Response = %d.");
	_LIT(KServiceOpt, "Service = %d.");
     
	TInt value;
			
	// Read and setup test values for external locate admin and emergency. Also set the session id via timestamp to ensure
	// a unique id.
	
	// Set process privacy request flag for test. If EFalse Emergency NetworkInduced will be carried out, ETrue for
	// Emergency MTLR requests.
	if (GetIntFromConfig(ConfigSection(), KProcessprivReq, value))
		{
		INFO_PRINTF2(KProcessPrivReqOpt, value);
		iProcessPrivReq = EFalse;

		if (value)
			{
			iProcessPrivReq = ETrue;
			}
	
		// Set emergency for test.
		if (GetIntFromConfig(ConfigSection(), KEmergency, value))
			{
			INFO_PRINTF2(KEmergencyOpt, value);
			iEmergency = EFalse;

			if (value)
				{
				iEmergency = ETrue;		
				}
			// Set the request service type for the test.
			if (GetIntFromConfig(ConfigSection(), KService, value))
				{
				INFO_PRINTF2(KServiceOpt, value);
				iService = static_cast<MLbsNetworkProtocolObserver::TLbsNetProtocolService>(value);
		
				// Set the external locate admin for test.
				if (GetIntFromConfig(ConfigSection(), KAdmin, value))
					{
					INFO_PRINTF2(KAdminOpt, value);
					iExternalLocateValue = static_cast<CLbsAdmin::TExternalLocateService>(value);
										
					CLbsAdmin* adminAPI = CLbsAdmin::NewL();
					CleanupStack::PushL(adminAPI);

					// MTLR admin setting.
					if (iService == MLbsNetworkProtocolObserver::EServiceMobileTerminated)
						{
						User::LeaveIfError(adminAPI->Set(KLbsSettingHomeExternalLocate, iExternalLocateValue));						
						}

					// Network Induced location request admin setting.
					else
						{
						User::LeaveIfError(adminAPI->Set(KLbsSettingHomeNetworkInducedLocate, iExternalLocateValue));		
						}
					
					CleanupStack::PopAndDestroy(adminAPI);
			
					// Set the expected response for test.
					if (GetIntFromConfig(ConfigSection(), KResponse, value))
						{
						INFO_PRINTF2(KResponseOpt, value);
						iExpectedResponse = static_cast<CLbsPrivacyController::TRequestVerificationResult>(value);
					
						// Determine the session id here using a time stamp.
						TTime timeStamp;
						timeStamp.HomeTime();
						iSessionId.SetSessionNum(iSessionId.SessionNum() + I64LOW(timeStamp.Int64()));

						INFO_PRINTF2(_L("Session Id = 0x%x."), iSessionId.SessionNum());
						}
					
					else
						{
						ERR_PRINTF1(_L("<font><b>FAILED: Couldn't read 'response' ini setting.</font></b>\n"));
						SetTestStepResult(EFail);
						}
					}
					
				else
					{
					ERR_PRINTF1(_L("<font><b>FAILED: Couldn't read 'admin' ini setting.</font></b>\n"));
					SetTestStepResult(EFail);
					}
				}
				
			else
				{
				ERR_PRINTF1(_L("<font><b>FAILED: Couldn't read 'service' ini setting.</font></b>\n"));
				SetTestStepResult(EFail);
				}
			}
			
		else
			{
			ERR_PRINTF1(_L("<font><b>FAILED: Couldn't read 'emergency' ini setting.</font></b>\n"));
			SetTestStepResult(EFail);
			}
		}
		
	else
		{
		ERR_PRINTF1(_L("<font><b>FAILED: Couldn't read 'process priv req' ini setting.</font></b>\n"));
		SetTestStepResult(EFail);
		}
	
	return TestStepResult();
	}

TVerdict CT_LbsOom::doTestStepL()
	{
	INFO_PRINTF1(_L("CT_LbsOom::doTestStepL()"));	

	// Stop the test if the preamble failed
	TESTL(TestStepResult() == EPass);
    //Can't run in urel build as oom macros works only in debug build.
    #if defined(_DEBUG)
        //Set oom property ETrue 
        iOomRequest.OomTestUpdates(ETrue);
        
        // Carry out the measurement control, to start the MT-LR or Network Induced request.
        PerformMeasurementControl();
        //Carry out privacy notify and accept
        ProcessNotifyAndAccept();
        
        // Perform session complete.
        PerformSessionComplete();
       
        //Set oom property EFalse
        iOomRequest.OomTestUpdates(EFalse);
    #else
        SetTestStepResult(EPass);
	#endif 
	return TestStepResult();
	}

void CT_LbsOom::PerformMeasurementControl()
	{
	const TInt KAdviceSystemStatusTimeout = 40*1000*1000;

	// As we are running many tests, the AdviceSystemStatus will only appear the first time round after LBS start.
	TNetProtocolResponseType response = iProxy->WaitForResponse(KAdviceSystemStatusTimeout);
	
	// >> AdviceSystemStatus(0)
	if (response == ENetMsgGetCurrentCapabilitiesResponse)
		{
		CLbsNetworkProtocolBase::TLbsSystemStatus status;
		iProxy->GetArgsLC(ENetMsgGetCurrentCapabilitiesResponse, &status); 
		TESTL(status == CLbsNetworkProtocolBase::ESystemStatusNone);
		}

	// Timeout
	else
		{
		TESTL(response == ENetMsgTimeoutExpired);
		}
	
	// << ProcessStatusUpdate()
	MLbsNetworkProtocolObserver::TLbsNetProtocolServiceMask activeServiceMask = MLbsNetworkProtocolObserver::EServiceMobileTerminated;
	iProxy->CallL(ENetMsgProcessStatusUpdate, &activeServiceMask);

	// Perform successful privacy request response.
	// For standard MTLRs with a prior privacy request, the test harness only supports accepted responses.
	if (iProcessPrivReq)
		{
		// << ProcessPrivacyRequest()
		const TBool emergency = ETrue;
		TLbsNetPosRequestPrivacy privacy    = ArgUtils::Privacy();
		TLbsExternalRequestInfo requestInfo = ArgUtils::RequestInfo();
		iProxy->CallL(ENetMsgProcessPrivacyRequest, &iSessionId, &emergency, &privacy, &requestInfo);
		const TInt KTimeOut = 60*1000*1000;
        // >> Respond Privacy Request
        TNetProtocolResponseType response = iProxy->WaitForResponse(KTimeOut);
        if (response == ENetMsgRespondPrivacyRequest)
            {
            TLbsNetSessionId* getSessionId = NULL;
            CLbsNetworkProtocolBase::TLbsPrivacyResponse getPrivacy;
            TInt cleanupCnt = iProxy->GetArgsLC(ENetMsgRespondPrivacyRequest, &getSessionId, &getPrivacy);
            TESTL(getSessionId->SessionNum()==iSessionId.SessionNum());
            TESTL(getPrivacy==CLbsNetworkProtocolBase::EPrivacyResponseAccepted);
            }
		}		
	// << ProcessLocationUpdate()
	iProxy->CallL(ENetMsgProcessLocationUpdate, &iSessionId, &iRefPosInfo);
	// << ProcessAssistanceData()
	TLbsAsistanceDataGroup dataRequestMask = EAssistanceDataReferenceTime;
	RLbsAssistanceDataBuilderSet assistanceData;
	ArgUtils::PopulateLC(assistanceData);
	TInt reason = KErrNone;
	iProxy->CallL(ENetMsgProcessAssistanceData, &dataRequestMask, &assistanceData, &reason);
	CleanupStack::PopAndDestroy(); //assistanceData
	// << ProcessLocationRequest()
	TLbsNetPosRequestQuality quality = ArgUtils::Quality(); 
	TLbsNetPosRequestMethod method   = ArgUtils::RequestMethod();
	iProxy->CallL(ENetMsgProcessLocationRequest, &iSessionId, &iEmergency, &iService, &quality, &method);
	}

void CT_LbsOom::ProcessNotifyAndAccept()
	{
	const TInt KTimeOut = 60*1000*1000;
	// >> RequestAssistanceData(0)
	TNetProtocolResponseType response = iProxy->WaitForResponse(KTimeOut);
	if (response == ENetMsgRequestAssistanceData)
	    {     
        //TESTL(iProxy->WaitForResponse(KTimeOut) == ENetMsgRequestAssistanceData); 
        TLbsAsistanceDataGroup dataGroup;
        iProxy->GetArgsLC(ENetMsgRequestAssistanceData, &dataGroup);
        TESTL(dataGroup == EAssistanceDataNone);
	    }
	// >> RespondLocationRequest()
	TESTL(iProxy->WaitForResponse(KTimeOut) == ENetMsgRespondLocationRequest); 
	TLbsNetSessionId* getSessionId = NULL;
	TInt getReason = KErrNone;
	TPositionSatelliteInfo* getPositionInfo = NULL;
	TPosition pos;
	iProxy->GetArgsLC(ENetMsgRespondLocationRequest, &getSessionId, &getReason, &getPositionInfo);
	getPositionInfo->GetPosition(pos);
	TESTL(getSessionId->SessionNum() == iSessionId.SessionNum());
	TESTL(getReason == KErrNone);
	// Validate the GPS position, test for $update,1,2,51.5015,-0.105,50,2,3* taken from the ini file section [NetworkInduced].
	if( pos.Latitude() == 51.5015 &&
	    pos.Longitude() == -0.105 &&
	    pos.Altitude() == 50 &&
	    pos.HorizontalAccuracy() == 2 &&
	    pos.VerticalAccuracy() == 3)
	   {
	   INFO_PRINTF1(_L("&gt;&gt;RespondLocationRequest()"));
	   iState = EGpsLocReceived;   
	   }
	
	}

void CT_LbsOom::PerformSessionComplete()
	{
	// << ProcessSessionComplete()
	TInt completeReason = KErrNone;
	iProxy->CallL(ENetMsgProcessSessionComplete, &iSessionId, &completeReason);
	// << ProcessStatusUpdate()
	MLbsNetworkProtocolObserver::TLbsNetProtocolServiceMask activeServiceMask = MLbsNetworkProtocolObserver::EServiceNone;	
	iProxy->CallL(ENetMsgProcessStatusUpdate, &activeServiceMask);
	TESTL(iState == EGpsLocReceived);
	}
//In OOM won't expect any callbacks 	
void CT_LbsOom::ProcessNetworkLocationRequest(TUint /*aRequestId*/, const TLbsExternalRequestInfo& /*aRequestInfo*/, const TNotificationType& /*aNotificationType*/)
	{
	INFO_PRINTF1(_L("&gt;&gt;Test Failed due to unexpected call back :: ProcessNetworkLocationRequest()"));
	SetTestStepResult(EFail);
	}
  
void CT_LbsOom::ProcessNetworkPositionUpdate(TUint /*aRequestId*/, const TPositionInfo& /*aPosInfo*/)
	{
	INFO_PRINTF1(_L("&gt;&gt;Test Failed due to unexpected call back ::ProcessNetworkPositionUpdate()"));
	SetTestStepResult(EFail);
	}    	
	
void CT_LbsOom::ProcessRequestComplete(TUint /*aRequestId*/, TInt /*aReason*/)
	{
	INFO_PRINTF1(_L("&gt;&gt;Test Failed due to unexpected call back::ProcessRequestComplete()"));
	SetTestStepResult(EFail);
	}


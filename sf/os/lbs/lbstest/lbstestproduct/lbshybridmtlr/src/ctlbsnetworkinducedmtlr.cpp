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
#include <lbs/lbsextendedsatellite.h>
// LBS test includes.
#include "ctlbsnetworkinducedmtlr.h"
#include <lbs/test/tlbsutils.h>
#include "argutils.h"
#include <lbs/test/activeyield.h>


/**
Static Constructor
*/
CT_LbsNetworkInducedMTLR* CT_LbsNetworkInducedMTLR::New(CT_LbsHybridMTLRServer& aParent)
	{
	// Note that we do not use ELeave.
	// This means that having insufficient memory will return NULL;
	CT_LbsNetworkInducedMTLR* testStep = new CT_LbsNetworkInducedMTLR(aParent);
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
CT_LbsNetworkInducedMTLR::CT_LbsNetworkInducedMTLR(CT_LbsHybridMTLRServer& aParent) : CT_LbsHybridMTLRStep(aParent)
	{
	SetTestStepName(KLbsNetworkInducedMTLR);

	iState = EInitializing;
	iSessionId.SetSessionOwner(KRequestUid);
	iSessionId.SetSessionNum(0x0005);
	
	iRefPosInfo = ArgUtils::ReferencePositionInfo();
	}


void CT_LbsNetworkInducedMTLR::ConstructL()
	{
	// Create the base class objects.
	CT_LbsHybridMTLRStep::ConstructL();

	iController = CLbsPrivacyController::NewL(*this);
	iProxy = CNetProtocolProxy::NewL();	
	}


/**
 * Destructor
 */
CT_LbsNetworkInducedMTLR::~CT_LbsNetworkInducedMTLR()
	{
	delete iController;
	delete iProxy;
	}


TVerdict CT_LbsNetworkInducedMTLR::doTestStepPreambleL()
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
	
	// Set process privacy request flag for test. If EFalse a MTLR will be carried out without a prior privacy request, ETrue for
	// normal MTLR requests.
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


TVerdict CT_LbsNetworkInducedMTLR::doTestStepL()
	{
	INFO_PRINTF1(_L("CT_LbsNetworkInducedMTLR::doTestStepL()"));	

	// Stop the test if the preamble failed
	TESTL(TestStepResult() == EPass);

	
	// Carry out the measurement control, to start the MT-LR or Network Induced request.
	PerformMeasurementControl();


	// Verify location request outcome based on admin and settings.
	
	// MTLR without a prior privacy request, test emergency and non-emergency.
	if ((!iProcessPrivReq) && (iService == MLbsNetworkProtocolObserver::EServiceMobileTerminated))
		{
		ProcessReject();		
		}
	
	// Emergency On, Admin any - Accept.
	else if (iEmergency)
		{
		ProcessNotifyAndAccept();
		}

	// Admin On - Notify and accept.
	else if ((iExternalLocateValue == CLbsAdmin::EExternalLocateOn) && (iExpectedResponse == CLbsPrivacyController::ERequestAccepted))
		{
		ProcessNotifyAndAccept();
		}
	
	// Admin OnButAlwaysVerify - Verify and accept.
	else if ((iExternalLocateValue == CLbsAdmin::EExternalLocateOnButAlwaysVerify) && (iExpectedResponse == CLbsPrivacyController::ERequestAccepted))
		{
		ProcessVerifyAndAccept();
		}
	
	// Admin OnButAlwaysVerify - Verify and reject.
	else if ((iExternalLocateValue == CLbsAdmin::EExternalLocateOnButAlwaysVerify) && (iExpectedResponse == CLbsPrivacyController::ERequestRejected))
		{
		ProcessVerifyAndReject();
		}
	
	// Admin OnButAlwaysVerify - Verfiy and timeout.
	else if ((iExternalLocateValue == CLbsAdmin::EExternalLocateOnButAlwaysVerify) && (iExpectedResponse == CLbsPrivacyController::ERequestUnknown))
		{
		ProcessVerifyAndTimeout();
		}
	
	// Admin Off - Reject.
	else if ((iExternalLocateValue == CLbsAdmin::EExternalLocateOff) && (iExpectedResponse == CLbsPrivacyController::ERequestRejected))
		{
		ProcessReject();
		}
	
	// Admin OffButNotify - Notify and reject.
	else if ((iExternalLocateValue == CLbsAdmin::EExternalLocateOffButNotify) && (iExpectedResponse == CLbsPrivacyController::ERequestRejected))
		{
		ProcessNotifyAndReject();
		}


	// Perform session complete.
	PerformSessionComplete();


	return TestStepResult();
	}


void CT_LbsNetworkInducedMTLR::PerformMeasurementControl()
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
		
		// >> Callback from RespondNetworkLocationRequest(iExpectedResponse)
		const TInt KTimeOut = 60*1000*1000;
		CheckForObserverEventTestsL(KTimeOut, *this);

		// >> Respond Privacy Request
		TESTL(iProxy->WaitForResponse(KTimeOut) == ENetMsgRespondPrivacyRequest);
		TLbsNetSessionId* getSessionId = NULL;
		CLbsNetworkProtocolBase::TLbsPrivacyResponse getPrivacy;
		TInt cleanupCnt = iProxy->GetArgsLC(ENetMsgRespondPrivacyRequest, &getSessionId, &getPrivacy);
		TESTL(getSessionId->SessionNum()==iSessionId.SessionNum());
		TESTL(getPrivacy==CLbsNetworkProtocolBase::EPrivacyResponseAccepted);
		CleanupStack::PopAndDestroy(cleanupCnt);//getSessionId
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


void CT_LbsNetworkInducedMTLR::ProcessVerifyAndAccept()
	{
	const TInt KTimeOut = 60*1000*1000;

	// >> Callback from RespondNetworkLocationRequest(iExpectedResponse)
	if (!iProcessPrivReq)
		{
		CheckForObserverEventTestsL(KTimeOut, *this);	
		}

	// >> Callback from ProcessNetworkPostionUpdate(refPosition)
	CheckForObserverEventTestsL(KTimeOut, *this);
	
	// >> RequestAssistanceData(0)
	TESTL(iProxy->WaitForResponse(KTimeOut) == ENetMsgRequestAssistanceData); 
	TLbsAsistanceDataGroup dataGroup;
	iProxy->GetArgsLC(ENetMsgRequestAssistanceData, &dataGroup);
	TESTL(dataGroup == EAssistanceDataNone);

	// >> RespondLocationRequest()
	TESTL(iProxy->WaitForResponse(KTimeOut) == ENetMsgRespondLocationRequest); 
	TLbsNetSessionId* getSessionId = NULL;
	TInt getReason = KErrNone;
	TPositionExtendedSatelliteInfo* getPositionInfo = NULL;
	iProxy->GetArgsLC(ENetMsgRespondLocationRequest, &getSessionId, &getReason, &getPositionInfo);
	// check it is a position
    TESTL(getPositionInfo->PositionClassType() == (EPositionInfoClass|EPositionCourseInfoClass|EPositionSatelliteInfoClass|EPositionExtendedSatelliteInfoClass));
    TESTL(getSessionId->SessionNum() == iSessionId.SessionNum());
    TESTL(getReason == KErrNone);   
    /* Check whether ExtendedSatelliteInfo data received by protocol module is the same as data fed to the GPS module or not. 
    Latitude(51.5015),Longitude(-0.105),Altitude(50),HorizontalAccuracy(2),VerticalAccuracy(3),
    speed(26.0),vertical speed(20.0),heading(25.0),course(30.0),SPEED_ACCURACY(2.0),VERTICAL_SPEED_ACCURACY(3.0),
    HEADING_ACCURACY(10.0),COURSE_ACCURACY(4.0),SD_OF_LONG_ERROR(5.0),SD_OF_LAT_ERROR(6.0),SD_OF_ALT_ERROR(7.0),
    SD_OF_SEMI_MAJOR_AXIS_ERROR(8.0),SD_OF_SEMI_MINOR_AXIS_ERROR(9.0),ORIEN_OF_SEMI_MAJOR_AXIS_ERROR(10.0),
    RMS_VAL_OF_SD_OF_RANGE(11.0),GEOIDAL_SEPARATION(12.0),MAGNETIC_VARIATION(13.0),COURSE_OVER_GROUND_MAGNETIC(14.0)
    GPS_TIMING_OF_CELL_MsPart(16383),GPS_TIMING_OF_CELL_LsPart(4294967295),REFERENCE_IDENTITY(511)
    SFN(4095)*/  
    
    TPosition gpsPos;
    getPositionInfo->GetPosition(gpsPos);
    TESTL(gpsPos.Latitude()==51.5015 && gpsPos.Longitude()== -0.105 && gpsPos.Altitude()==50 
          && gpsPos.HorizontalAccuracy()==2 && gpsPos.VerticalAccuracy()==3);             
    TCourse course;
    getPositionInfo->GetCourse(course);
         
    TESTL(course.Speed()==26.0 && course.VerticalSpeed()==20.0 && course.Heading()==25.0 && course.Course()==30.0 && 
          course.SpeedAccuracy()==2.0 && course.VerticalSpeedAccuracy()==3.0 && course.HeadingAccuracy()==10.0 &&
          course.CourseAccuracy()==4.0);           
    
    TDetailedErrorReport detErrRep;
    getPositionInfo->GetDetailedErrorReport(detErrRep);
    TESTL(detErrRep.StanDeviOfLongitudeError()==5.0 && detErrRep.StanDeviOfLatiitudeError()==6.0 && 
          detErrRep.StanDeviOfAltitudeError()==7.0 && detErrRep.StanDeviOfSemiMajorAxisError()==8.0 && 
          detErrRep.StanDeviOfSemiMinorAxisError()==9.0 && detErrRep.OrientationOfSemiMajorAxisError()==10.0 && 
          detErrRep.RmsValOfStanDeviOfRange()==11.0);  
    
    TGpsTimingMeasurementData gpsTimingData;
    getPositionInfo->GetGpsTimingData(gpsTimingData);
     
    TESTL(gpsTimingData.DataType()== TGpsTimingMeasurementData::EGpsTimingDataTypeUtran && 
          gpsTimingData.NetworkMode()== TGpsTimingMeasurementData::ENetworkModeFdd && 
          gpsTimingData.GPSTimingOfCellMsPart()==16383 && gpsTimingData.GPSTimingOfCellLsPart()==4294967295UL && 
          gpsTimingData.ReferenceIdentity()==511 && gpsTimingData.Sfn()==4095);  
                   
    
    TESTL(getPositionInfo->GeoidalSeparation()==12.0 && getPositionInfo->MagneticVariation()==13.0 && 
          getPositionInfo->CourseOverGroundMagnetic()==14.0);  
    
        
    // >> Callback from ProcessNetworkPostionUpdate(GPS Location)
	CheckForObserverEventTestsL(KTimeOut, *this);
	}


void CT_LbsNetworkInducedMTLR::ProcessVerifyAndReject()
	{
	const TInt KTimeOut = 60*1000*1000;
	
	// >> Callback from RespondNetworkLocationRequest(iExpectedResponse)
	CheckForObserverEventTestsL(KTimeOut, *this);

	// >> RespondLocationRequest()
	ProcessReject();
	}


void CT_LbsNetworkInducedMTLR::ProcessVerifyAndTimeout()
	{
	ProcessVerifyAndReject();
	}


void CT_LbsNetworkInducedMTLR::ProcessNotifyAndAccept()
	{
	ProcessVerifyAndAccept();	
	}


void CT_LbsNetworkInducedMTLR::ProcessNotifyAndReject()
	{
	ProcessVerifyAndReject();
	}


void CT_LbsNetworkInducedMTLR::ProcessReject()
	{
	const TInt KTimeOut = 60*1000*1000;

	// >> RespondLocationRequest()
	TESTL(iProxy->WaitForResponse(KTimeOut) == ENetMsgRespondLocationRequest); 
	TLbsNetSessionId* getSessionId = NULL;
	TInt getReason = KErrNone;
	TPositionSatelliteInfo* getPositionInfo = NULL;
	iProxy->GetArgsLC(ENetMsgRespondLocationRequest, &getSessionId, &getReason, &getPositionInfo);
	TESTL(getSessionId->SessionNum() == iSessionId.SessionNum());
	
	if ((!iProcessPrivReq) && (iService == MLbsNetworkProtocolObserver::EServiceMobileTerminated))
		{
		TESTL(getReason == KErrNotSupported);
		}
		
	else
		{
		TESTL(getReason == KErrAccessDenied);	
		}
		
	}


void CT_LbsNetworkInducedMTLR::PerformSessionComplete()
	{
	const TInt KTimeOut = 60*1000*1000;
	
	// << ProcessSessionComplete()
	TInt completeReason = KErrNone;
	iProxy->CallL(ENetMsgProcessSessionComplete, &iSessionId, &completeReason);

	// << ProcessStatusUpdate()
	MLbsNetworkProtocolObserver::TLbsNetProtocolServiceMask activeServiceMask = MLbsNetworkProtocolObserver::EServiceNone;	
	iProxy->CallL(ENetMsgProcessStatusUpdate, &activeServiceMask);

	// >> Callback from ProcessRequestComplete().
	
	// The user/privacy controller is not notified, for these cases:
	if (((iExternalLocateValue == CLbsAdmin::EExternalLocateOff) && (!iEmergency)) ||					// Admin setting is off and is not an emergency.
		((!iProcessPrivReq) && (iService == MLbsNetworkProtocolObserver::EServiceMobileTerminated)))	// Invalid MTLR, without a prior privacy request.
		{
		// Verify final expected state.
		TESTL(iState == EInitializing);
		}

	// The user/privacy controller is notified for all other cases.
	else
		{
		CheckForObserverEventTestsL(KTimeOut, *this);

		// Verify that the last callback was to ProcessSessionComplete()
		TESTL(iState == ERequestComplete);
		}		
	}

	
void CT_LbsNetworkInducedMTLR::ProcessNetworkLocationRequest(TUint aRequestId, const TLbsExternalRequestInfo& aRequestInfo, const TNotificationType& aNotificationType)
	{
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsNetworkInducedMTLR::ProcessNetworkLocationRequest()"));
	TEST(iState == EInitializing);

	// Validate request info string.
	TLbsRequesterId requesterId;
	TLbsClientName clientName;
	TLbsClientExternalId clientExternalId;	

	aRequestInfo.GetRequesterId(requesterId);
	aRequestInfo.GetClientName(clientName);
	aRequestInfo.GetClientExternalId(clientExternalId);

	// Normal MTLR.
	if (iProcessPrivReq)
		{
		TEST(requesterId == _L8("+441234567891"));	// Values taken from RequestInfo() in argutils.h.
		TEST(clientName == _L8("THybridNWClient"));
		TEST(clientExternalId ==_L8("+441234567891"));	
		}

	// Network Induced MTLR.
	else
		{
		TEST(requesterId == _L8(""));
		TEST(clientName == _L8(""));
		TEST(clientExternalId == _L8(""));
		}


	// Validate the notification type.
	
	// The following table allows us to determine the expected notification types.

	// Admin setting												TLbsNetPosRequestPrivacy	TNotificationType (held in MLbsPrivacyObserver)
	// -------------												------------------------	-----------------------------------------------
	// NetworkInduced_EmergencyOn_AdminAny							ERequestAdviceNotify		ENotifyLocationAccepted
	// NoPrivMTLR_EmergencyOn_AdminAny								ERequestActionAllow
																
	// NetworkInduced_EmergencyOff_AdminOn							ERequestAdviceNotify		ENotifyLocationAccepted
	// NoPrivMTLR_EmergencyOff_AdminOn								ERequestActionAllow
																			
	// NetworkInduced_EmergencyOn_AdminOnButAlwaysVerify_Accept	ERequestAdviceVerify		ENotifyAndVerifyLocationRejectedIfNoReponse
	// NetworkInduced_EmergencyOn_AdminOnButAlwaysVerify_Reject	ERequestActionReject
	// NoPrivMTLR_EmergencyOff_AdminOnButAlwaysVerify_Accept
	// NoPrivMTLR_EmergencyOff_AdminOnButAlwaysVerify_Reject

	// NetworkInduced_EmergencyOff_AdminOff						N/A - just reject
	// NoPrivMTLR_EmergencyOff_AdminOff

	// NetworkInduced_EmergencyOff_AdminOffButNotify				ERequestAdviceNotfy			ENotifyLocationRejected
	// NoPrivMTLR_EmergencyOff_AdminOffButNotify					ERequestActionReject

	TNotificationType expectedNotification;

	if ((iEmergency) || (iExternalLocateValue == CLbsAdmin::EExternalLocateOn))
		{
		expectedNotification = MLbsPrivacyObserver::ENotifyLocationAccepted;
		}

	else if (iExternalLocateValue == CLbsAdmin::EExternalLocateOnButAlwaysVerify)
		{
		expectedNotification = MLbsPrivacyObserver::ENotifyAndVerifyLocationRejectedIfNoReponse;
		}

	else
		{
		expectedNotification = MLbsPrivacyObserver::ENotifyLocationRejected;
		}

	TEST(aNotificationType == expectedNotification);
	
		
	// The privacy controller requires a response for verifys.
	if ((aNotificationType == MLbsPrivacyObserver::ENotifyAndVerifyLocationRejectedIfNoReponse) && (iExpectedResponse != CLbsPrivacyController::ERequestUnknown))	// Note: ERequestUnknown == timeout response.
		{
		iController->RespondNetworkLocationRequest(aRequestId, iExpectedResponse);
		}

	iState = EPrivacyCheckOk;
	
	ReturnToTestStep();
	}

   
void CT_LbsNetworkInducedMTLR::ProcessNetworkPositionUpdate(TUint aRequestId, const TPositionInfo& aPosInfo)
	{
	TPosition pos;
	
	aPosInfo.GetPosition(pos);

	// Validate session request id.
	TEST(aRequestId == iSessionId.SessionNum());

	// and position data.	
	if(iState == EPrivacyCheckOk)
		{
		INFO_PRINTF1(_L("&gt;&gt;CT_LbsNetworkInducedMTLR::ProcessNetworkPositionUpdate(RefPosition)"));

		// Validate the reference position.
		TPosition refPos;
		
		iRefPosInfo.GetPosition(refPos);
		if (pos.Latitude() == refPos.Latitude() &&
			pos.Longitude() == refPos.Longitude() &&
			pos.Altitude() == refPos.Altitude() &&
			pos.HorizontalAccuracy() == refPos.HorizontalAccuracy() &&
			pos.VerticalAccuracy() == refPos.VerticalAccuracy())
			{
			iState = ERefLocReceived;		
			}
		}

	else if(iState == ERefLocReceived)
		{
		INFO_PRINTF1(_L("&gt;&gt;CT_LbsNetworkInducedMTLR::ProcessNetworkPositionUpdate(GpsPosition)"));

		// Validate the GPS position, test for $update,1,2,51.5015,-0.105,50,2,3* taken from the ini file section [NetworkInduced].
		if(	pos.Latitude() == 51.5015 &&
			pos.Longitude() == -0.105 &&
			pos.Altitude() == 50 &&
			pos.HorizontalAccuracy() == 2 &&
			pos.VerticalAccuracy() == 3)
			{
			iState = EGpsLocReceived;	
			}
		}

	ReturnToTestStep();
	}


void CT_LbsNetworkInducedMTLR::ProcessRequestComplete(TUint aRequestId, TInt aReason)
	{
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsNetworkInducedMTLR::ProcessRequestComplete()"));

	// Valid request complete parameters and state.
	TEST(aRequestId == iSessionId.SessionNum());
	TEST(aReason == KErrNone);

	if ((iExpectedResponse == CLbsPrivacyController::ERequestRejected) || (iExpectedResponse == CLbsPrivacyController::ERequestUnknown))	// Note: ERequestUnknown == timeout response.
		{
		TEST(iState == EPrivacyCheckOk); // For rejected requests no updates are received.
		}
	
	else
		{
		TEST(iState == EGpsLocReceived);
		}
	
	iState = ERequestComplete; 
	ReturnToTestStep();
	}


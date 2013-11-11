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
#include <lbs/lbsnetprotocolbase.h>
#include <lbs/lbsassistancedatabuilderset.h>
#include <lbs/lbsloccommon.h>

// LBS test includes.
#include "ctlbsuebasedmtlr.h"
#include <lbs/test/tlbsutils.h>
#include "argutils.h"
#include <lbs/test/activeyield.h>

/**
Static Constructor
*/
CT_LbsUEBasedMTLR* CT_LbsUEBasedMTLR::New(CT_LbsHybridMTLRServer& aParent)
	{
	// Note the lack of ELeave.
	// This means that having insufficient memory will return NULL;
	CT_LbsUEBasedMTLR* testStep = new CT_LbsUEBasedMTLR(aParent);
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
CT_LbsUEBasedMTLR::CT_LbsUEBasedMTLR(CT_LbsHybridMTLRServer& aParent) : CT_LbsHybridMTLRStep(aParent)
	{
	SetTestStepName(KLbsUEBasedMTLR);
	iSessionId.SetSessionOwner(KRequestUid);
	iSessionId.SetSessionNum(0x0001);
	iPositionUpdateRef = EFalse;
	iState = EInitializing;
	}


void CT_LbsUEBasedMTLR::ConstructL()
	{
	// Create the base class objects.
	CT_LbsHybridMTLRStep::ConstructL();
	iController = CLbsPrivacyController::NewL(*this);
	iProxy = CNetProtocolProxy::NewL();
	}


/**
 * Destructor
 */
CT_LbsUEBasedMTLR::~CT_LbsUEBasedMTLR()
	{
	delete iController;
	delete iProxy;
	}


TVerdict CT_LbsUEBasedMTLR::doTestStepL()
	{
	INFO_PRINTF1(_L("CT_LbsUEBasedMTLR::doTestStepL()"));	
	// Stop the test if the preamble failed
	TESTL(TestStepResult() == EPass);

	const TInt KTimeOut = 60*1000*1000;
	const TInt KAdviceSystemStatusTimeout = 40*1000*1000;	

	// >> AdviceSystemStatus(0)
	TESTL(iProxy->WaitForResponse(KAdviceSystemStatusTimeout) == ENetMsgGetCurrentCapabilitiesResponse);
	CLbsNetworkProtocolBase::TLbsSystemStatus status;
	CleanupStack::PopAndDestroy(iProxy->GetArgsLC(ENetMsgGetCurrentCapabilitiesResponse, &status));
	TESTL(status == CLbsNetworkProtocolBase::ESystemStatusNone);
	
	// Initiate MTLR Start

	// << ProcessStatusUpdate()
	MLbsNetworkProtocolObserver::TLbsNetProtocolServiceMask activeServiceMask = MLbsNetworkProtocolObserver::EServiceMobileTerminated;
	iProxy->CallL(ENetMsgProcessStatusUpdate, &activeServiceMask);

	// << ProcessPrivacyRequest()
	const TBool emergency = ETrue;
	TLbsNetPosRequestPrivacy privacy    = ArgUtils::Privacy();
	TLbsExternalRequestInfo requestInfo = ArgUtils::RequestInfo();
	iProxy->CallL(ENetMsgProcessPrivacyRequest, &iSessionId, &emergency, &privacy, &requestInfo);

	// >> Callback from RespondNetworkLocationRequest(ERequestAccepted)
	CheckForObserverEventTestsL(KTimeOut, *this);
	
	// >> Respond Privacy Request
	TESTL(iProxy->WaitForResponse(KTimeOut) == ENetMsgRespondPrivacyRequest);

	TLbsNetSessionId* getSessionId = NULL;
	CLbsNetworkProtocolBase::TLbsPrivacyResponse getPrivacy;
	TInt cleanupCnt;
	cleanupCnt = iProxy->GetArgsLC(ENetMsgRespondPrivacyRequest, &getSessionId, &getPrivacy);
	TESTL(getSessionId->SessionNum()==iSessionId.SessionNum());
	TESTL(getPrivacy==CLbsNetworkProtocolBase::EPrivacyResponseAccepted);
	CleanupStack::PopAndDestroy(cleanupCnt);

	// Initiate MTLR End

	// MTLR Reference Position Notification Start

	// << ProcessLocationUpdate()
	TPositionInfo positionInfo = ArgUtils::ReferencePositionInfo();
	iProxy->CallL(ENetMsgProcessLocationUpdate, &iSessionId, &positionInfo);

	// MTLR Reference Position Notification Stop

	// MTLR Assistance Data Notification Start

	// << ProcessAssistanceData()
	TLbsAsistanceDataGroup dataRequestMask = EAssistanceDataReferenceTime;
	RLbsAssistanceDataBuilderSet assistanceData;
	ArgUtils::PopulateLC(assistanceData);
	TInt reason = KErrNone;
	iProxy->CallL(ENetMsgProcessAssistanceData, &dataRequestMask, &assistanceData, &reason);
	CleanupStack::PopAndDestroy();

	// MTLR Assistance Data Notification Stop

	// MTLR Network Location Request Start

	// << ProcessLocationRequest()
	MLbsNetworkProtocolObserver::TLbsNetProtocolService service = MLbsNetworkProtocolObserver::EServiceMobileTerminated;
	TLbsNetPosRequestQuality quality = ArgUtils::QualityAlpha2();
	TLbsNetPosRequestMethod method   = ArgUtils::RequestHybridMethod();
	iProxy->CallL(ENetMsgProcessLocationRequest, &iSessionId, &emergency, &service, &quality, &method);

	// MTLR Network Location Request Stop

	// >> Callback from ProcessNetworkPostionUpdate(refPosition)
	CheckForObserverEventTestsL(KTimeOut, *this);

	// >> Callback from ProcessNetworkPostionUpdate(GPS Location)
	CheckForObserverEventTestsL(KTimeOut, *this);
	
	// >> RequestAssistanceData(0)
	TESTL(iProxy->WaitForResponse(KTimeOut) == ENetMsgRequestAssistanceData); 
	TLbsAsistanceDataGroup dataGroup;
	CleanupStack::PopAndDestroy(iProxy->GetArgsLC(ENetMsgRequestAssistanceData, &dataGroup));
	TESTL(dataGroup == EAssistanceDataNone);

	// >> RespondLocationRequest()
	TESTL(iProxy->WaitForResponse(KTimeOut) == ENetMsgRespondLocationRequest); 
	getSessionId = NULL;
	TInt getReason = KErrNone;
	TPositionExtendedSatelliteInfo* getPositionInfo = NULL;
	cleanupCnt = iProxy->GetArgsLC(ENetMsgRespondLocationRequest, &getSessionId, &getReason, &getPositionInfo);
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
    			
	CleanupStack::PopAndDestroy(cleanupCnt);
	
	// MTLR Session Completion Start
	// << ProcessSessionComplete()
	reason = KErrNone;
	iProxy->CallL(ENetMsgProcessSessionComplete, &iSessionId, &reason);

	// << ProcessStatusUpdate()
	MLbsNetworkProtocolObserver::TLbsNetProtocolServiceMask activeServiceMask2 = MLbsNetworkProtocolObserver::EServiceNone;	
	iProxy->CallL(ENetMsgProcessStatusUpdate, &activeServiceMask2);

	// >> Callback from ProcessRequestComplete()
	CheckForObserverEventTestsL(KTimeOut, *this);
	// MTLR Session Completion Stop

	// Verify that the last callback was to ProcessSessionComplete()
	TESTL(iState==ERequestComplete);
	
	return TestStepResult();
	}

void CT_LbsUEBasedMTLR::ProcessNetworkLocationRequest(TUint aRequestId, const TLbsExternalRequestInfo& /*aRequestInfo*/, const TNotificationType& /*aNotificationType*/)
{
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsUEBasedMTLR::ProcessNetworkLocationRequest()"));
	TEST(iState==EInitializing);
	iController->RespondNetworkLocationRequest(aRequestId, CLbsPrivacyController::ERequestAccepted);
	iState = EPrivacyCheckOk;
	ReturnToTestStep();
}

void CT_LbsUEBasedMTLR::ProcessNetworkPositionUpdate(TUint /*aRequestId*/, const TPositionInfo& /*aPosInfo*/)
{
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsUEBasedMTLR::ProcessNetworkPositionUpdate()"));
	if(!iPositionUpdateRef)
		{
		TEST(iState==EPrivacyCheckOk);
		iState=ERefLocReceived;	
		iPositionUpdateRef = ETrue;	
		}
	else
		{
		//This function is called back twice. First time when it returns the ref loc and second time when it returns a Gps loc
		//We need to test iState for different state
		TEST(iState==ERefLocReceived);
		iState=EGpsLocReceived;
		}
	ReturnToTestStep();
}

void CT_LbsUEBasedMTLR::ProcessRequestComplete(TUint /*aRequestId*/, TInt /*aReason*/)
{
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsUEBasedMTLR::ProcessRequestComplete()"));
	TEST(iState==EGpsLocReceived);
	iState=ERequestComplete; 
	ReturnToTestStep();
}





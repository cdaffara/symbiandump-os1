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
// This is the class implementation for the MO-LR - Accurate GPS Result scenario Tests
// 
//

// LBS includes. 
#include <lbs/test/lbsnetprotocolproxy.h>
#include <lbs/lbsnetprotocolbase.h>
#include <lbs/lbsassistancedatabuilderset.h>

// LBS test includes.
#include "ctlbshybridueassistednpudposstatus.h"
#include <lbs/test/tlbsutils.h>
#include "argutils.h"
#include <lbs/test/activeyield.h>


const TInt KN(2); // Number of times to send the measurement from GPS to NW

/**
Static Constructor
*/
CT_LbsHybridUEAssistedNpudPosStatus* CT_LbsHybridUEAssistedNpudPosStatus::New(CT_LbsHybridMOLRServer& aParent)
	{
	// Note that we do not use ELeave.
	// This means that having insufficient memory will return NULL;
	CT_LbsHybridUEAssistedNpudPosStatus* testStep = new CT_LbsHybridUEAssistedNpudPosStatus(aParent);
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
CT_LbsHybridUEAssistedNpudPosStatus::CT_LbsHybridUEAssistedNpudPosStatus(CT_LbsHybridMOLRServer& aParent) : CT_LbsHybridMOLRStep(aParent)
	{
	SetTestStepName(KLbsHybridUEAssistedNpudPosStatus);
	iSessionId.SetSessionOwner(KRequestUid);
	iSessionId.SetSessionNum(0x0005);
	}


void CT_LbsHybridUEAssistedNpudPosStatus::ConstructL()
	{
	// Create the base class objects.
	CT_LbsHybridMOLRStep::ConstructL();
	iLbsPositioningStatus = CLbsPositioningStatus::NewL(*this);
	iPosStatusCount = 0;
	}


/**
 * Destructor
 */
CT_LbsHybridUEAssistedNpudPosStatus::~CT_LbsHybridUEAssistedNpudPosStatus()
	{
    delete iLbsPositioningStatus;
	}

// Hybrid - UE Assisted MO-LR GPS ok

TVerdict CT_LbsHybridUEAssistedNpudPosStatus::doTestStepL()
	{
	INFO_PRINTF1(_L("CT_LbsHybridUEAssistedNpudPosStatus::doTestStepL()"));	

    // Stop the test if the preable failed
	TESTL(TestStepResult() == EPass);

	const TInt KTimeOut = 60*1000*1000;

	// Create Network Protocol Proxy
	CNetProtocolProxy* proxy = CNetProtocolProxy::NewL();
	CleanupStack::PushL(proxy);

	// >> AdviceSystemStatus(0) - GetCurrentCapabilitiesResponse
	TESTL(proxy->WaitForResponse(KTimeOut) == ENetMsgGetCurrentCapabilitiesResponse);
	CLbsNetworkProtocolBase::TLbsSystemStatus status;
	TInt cleanupCnt;
	cleanupCnt = proxy->GetArgsLC(ENetMsgGetCurrentCapabilitiesResponse, &status);
	TESTL(status == CLbsNetworkProtocolBase::ESystemStatusNone);
	CleanupStack::PopAndDestroy(cleanupCnt);

	// Start Test Step
	RPositionServer server;
	TESTL(KErrNone == server.Connect());
	CleanupClosePushL(server);	

	RPositioner pos;
	TESTL(KErrNone == pos.Open(server));
	
	CPosServerWatcher *pWatch = NULL;
	TInt reason = KErrNone;
			
    CleanupClosePushL(pos);
    // Set the max fix time for the client request to ensure the location server does not complete the request too soon during the test.
    TPositionUpdateOptions posOpts(TTimeIntervalMicroSeconds(0), TTimeIntervalMicroSeconds(KMOLRFixTime));
    pos.SetUpdateOptions(posOpts);
    
    pWatch = CPosServerWatcher::NewLC(pos, *this);

    // Request a self-locate MoLr.
    pWatch->IssueNotifyPositionUpdate();
    CheckForObserverEventTestsL(KTimeOut, *this);
    //We should get Positioning Indicator flag on and may be off after that
    TESTL(iPosStatusCount==1);
    TESTL(iPositioningStatus == CLbsPositioningStatus::ELbsPositioningStatusActive);

    // >> RequestSelfLocation()
    TESTL(proxy->WaitForResponse(KTimeOut) == ENetMsgRequestSelfLocation);

    // Process the response.
    TLbsNetSessionId* 					sessionId = NULL;
    TLbsNetPosRequestOptionsAssistance*	opts = NULL;
    
    cleanupCnt = proxy->GetArgsLC(ENetMsgRequestSelfLocation, &sessionId, &opts);

    TBool qualitycheck = 	ArgUtils::CompareQuality(	opts, 
                                                        ETrue, 
                                                        KMinHorizontalAcc, 
                                                        KMinVerticalAcc, 
                                                        KMOLRFixTime,
                                                        0, 
                                                        EAssistanceDataReferenceTime, 
                                                        (TPositionModuleInfo::ETechnologyTerminal 
                                                        | TPositionModuleInfo::ETechnologyAssisted)
                                                    );
    
    TESTL(qualitycheck);

    iSessionId.SetSessionNum(sessionId->SessionNum());
    iSessionId.SetSessionOwner(sessionId->SessionOwner());

    CleanupStack::PopAndDestroy(cleanupCnt);
    sessionId = NULL;
    opts = NULL;	
    
    // << ProcessStatusUpdate(EServiceSelfLocation)
    MLbsNetworkProtocolObserver::TLbsNetProtocolServiceMask activeServiceMask = MLbsNetworkProtocolObserver::EServiceSelfLocation;
    proxy->CallL(ENetMsgProcessStatusUpdate, &activeServiceMask);

    // << ProcessLocationUpdate(SessionId, RefPosition)
    TPositionInfo refPosInfo = ArgUtils::MolrReferencePositionInfo();
    proxy->CallL(ENetMsgProcessLocationUpdate, &iSessionId, &refPosInfo);

    // TEST: Get the ref pos app side.
    CheckForObserverEventTestsL(KTimeOut, *this);
    //Check for Position Status update - This event could be after reference position
    //NOTE: Ref position and positioning indicator callback can be in any order
    CheckForObserverEventTestsL(KTimeOut, *this);
    TESTL(iPosStatusCount==2);
    TESTL(iPositioningStatus == CLbsPositioningStatus::ELbsPositioningStatusNotActive);
    
    // << ProcessAssistanceData()
    TLbsAsistanceDataGroup dataMask = EAssistanceDataReferenceTime;
    RLbsAssistanceDataBuilderSet assistanceData;
    ArgUtils::PopulateLC(assistanceData);
    reason = KErrNone;
    proxy->CallL(ENetMsgProcessAssistanceData, &dataMask, &assistanceData, &reason);
    CleanupStack::PopAndDestroy(); // assistanceData

    // << ProcessLocationRequest(SessionId, HybridMode, alpha2)
    TBool emergency = EFalse;
    MLbsNetworkProtocolObserver::TLbsNetProtocolService service = MLbsNetworkProtocolObserver::EServiceSelfLocation;
    TLbsNetPosRequestQuality quality = ArgUtils::Quality(); 
    quality.SetMaxFixTime(ArgUtils::Alpha2());
    TLbsNetPosRequestMethod method   = ArgUtils::RequestHybridMethod();
    proxy->CallL(ENetMsgProcessLocationRequest, &iSessionId, &emergency, &service, &quality, &method);

    // Now that the hybrid/alpha2 has been requested, record current time to verify alpha2 timer expires correctly.
    TTime startTime;
    startTime.HomeTime();

    // >> RequestAssistanceData(0)
    TESTL(proxy->WaitForResponse(KTimeOut) == ENetMsgRequestAssistanceData); 
    cleanupCnt = proxy->GetArgsLC(ENetMsgRequestAssistanceData, &dataMask);
    TESTL(dataMask == EAssistanceDataNone);
    CleanupStack::PopAndDestroy(cleanupCnt);

    // << NotifyPositionUpdate()
    pWatch->IssueNotifyPositionUpdate();

    CheckForObserverEventTestsL(KTimeOut, *this);
    //We should get Positioning Indicator flag on
    TESTL(iPosStatusCount==3);
    TESTL(iPositioningStatus == CLbsPositioningStatus::ELbsPositioningStatusActive);

    // >> RequestAssistanceData(0)
    TESTL(proxy->WaitForResponse(KTimeOut) == ENetMsgRequestAssistanceData); 
    
    
    cleanupCnt = proxy->GetArgsLC(ENetMsgRequestAssistanceData, &dataMask);
    TESTL(dataMask == EAssistanceDataNone);
    CleanupStack::PopAndDestroy(cleanupCnt);
    // Determine the value to take off the alpha2 value. This is required because we had to wait for the assistance data response.
    TTimeIntervalMicroSeconds microseconds;
    TTime stopTime;
    stopTime.HomeTime();
    microseconds = stopTime.MicroSecondsFrom(startTime); 
    TInt64 timeElapsed = microseconds.Int64();
    TInt delta = 2 * 1000 * 1000; // 2 secs.

    // >> RespondLocationRequest()
    TESTL(proxy->WaitForResponse(ArgUtils::Alpha2() - timeElapsed - delta) == ENetMsgTimeoutExpired);

    // Wait for and process the response.
    TESTL(proxy->WaitForResponse(2 * delta) == ENetMsgRespondLocationRequest);		// DONT get because the measurement data bus has not been created...

    sessionId = NULL;
    TPositionGpsMeasurementInfo* measurementInfo = NULL;
    cleanupCnt = proxy->GetArgsLC(ENetMsgRespondLocationRequest, &sessionId, &reason, &measurementInfo);
    TESTL(sessionId->SessionNum() == iSessionId.SessionNum());
    TESTL(reason == KErrNone);
    CleanupStack::PopAndDestroy(cleanupCnt);//sessionId, measurementInfo

    // Recv -> RequestAssistanceData - we get an extra msg as the result of the A-GPS manager re-issueing a location request when it's
    //									max fix time timer expries.
    TESTL(proxy->WaitForResponse(KTimeOut) == ENetMsgRequestAssistanceData); 
    cleanupCnt = proxy->GetArgsLC(ENetMsgRequestAssistanceData, &dataMask);
    TESTL(dataMask == EAssistanceDataNone);
    CleanupStack::PopAndDestroy(cleanupCnt);
    const TInt t = 8 * 1000 * 1000; // 8 secs.
    quality.SetMaxFixTime(t);

    TPositionExtendedSatelliteInfo* positionInfo = NULL;
    //TPositionInfo* positionInfo = NULL;
    
    for (TInt i = 0; i < KN; i++)
        {
        // << ProcessLocationRequest(SessionId, HybridMode, t)
        proxy->CallL(ENetMsgProcessLocationRequest, &iSessionId, &emergency, &service, &quality, &method);

        // >> RequestAssistanceData(0)
        TESTL(proxy->WaitForResponse(KTimeOut) == ENetMsgRequestAssistanceData); 
        cleanupCnt = proxy->GetArgsLC(ENetMsgRequestAssistanceData, &dataMask);
        TESTL(dataMask == EAssistanceDataNone);
        CleanupStack::PopAndDestroy(cleanupCnt);

        // >> RespondLocationRequest() - first measurement, second position.
        TESTL(proxy->WaitForResponse(t + delta) == ENetMsgRespondLocationRequest);

        sessionId = NULL;
        
        // Expect measurement first time.
        if (i < (KN-1))
            {
            measurementInfo = NULL;
            cleanupCnt = proxy->GetArgsLC(ENetMsgRespondLocationRequest, &sessionId, &reason, &measurementInfo);			

            // Check it is measurement
            TESTL(measurementInfo->PositionClassType() == EPositionGpsMeasurementInfoClass);

            // >> RequestAssistanceData - we get an extra msg as the result of the A-GPS manager re-issueing a location request when it's
            //									max fix time timer expries.
            TESTL(proxy->WaitForResponse(KTimeOut) == ENetMsgRequestAssistanceData);

            cleanupCnt += proxy->GetArgsLC(ENetMsgRequestAssistanceData, &dataMask);

            TESTL(dataMask == EAssistanceDataNone);
            TESTL(sessionId->SessionNum() == iSessionId.SessionNum());
            TESTL(reason == KErrNone);
            }
        
        // Expect position second time.
        else
            {
            cleanupCnt = proxy->GetArgsLC(ENetMsgRespondLocationRequest, &sessionId, &reason, &positionInfo);
            
            // check it is a position
            TESTL(positionInfo->PositionClassType() == (EPositionInfoClass|EPositionCourseInfoClass|EPositionSatelliteInfoClass|EPositionExtendedSatelliteInfoClass));
            TESTL(sessionId->SessionNum() == iSessionId.SessionNum());
            TESTL(reason == KErrNone);
            /* Check whether ExtendedSatelliteInfo data received by protocol module is the same as data fed to the GPS module or not. 
            Latitude(50.2454),Longitude(0.1668),Altitude(1.0),HorizontalAccuracy(10),VerticalAccuracy(10), speed(26.0),vertical speed(20.0),heading(25.0),course(30.0), 
            SPEED_ACCURACY(2.0),VERTICAL_SPEED_ACCURACY(3.0),HEADING_ACCURACY(10.0),COURSE_ACCURACY(4.0),
            SD_OF_LONG_ERROR(5.0),SD_OF_LAT_ERROR(6.0),SD_OF_ALT_ERROR(7.0),SD_OF_SEMI_MAJOR_AXIS_ERROR(8.0),
            SD_OF_SEMI_MINOR_AXIS_ERROR(9.0),ORIEN_OF_SEMI_MAJOR_AXIS_ERROR(10.0),RMS_VAL_OF_SD_OF_RANGE(11.0),
            GEOIDAL_SEPARATION(12.0),MAGNETIC_VARIATION(13.0),COURSE_OVER_GROUND_MAGNETIC(14.0)
            GPS_TIMING_OF_CELL_MsPart(16383),GPS_TIMING_OF_CELL_LsPart(4294967295),REFERENCE_IDENTITY(511)
            SFN(4095)*/  
            
            TPosition gpsPos;
            positionInfo->GetPosition(gpsPos);
            TESTL(gpsPos.Latitude()==50.2454 && gpsPos.Longitude()== 0.1668 && gpsPos.Altitude()==1.0 
                  && gpsPos.HorizontalAccuracy()==10 && gpsPos.VerticalAccuracy()==10);             
            TCourse course;
            positionInfo->GetCourse(course);
                 
            TESTL(course.Speed()==26.0 && course.VerticalSpeed()==20.0 && course.Heading()==25.0 && course.Course()==30.0 && 
                  course.SpeedAccuracy()==2.0 && course.VerticalSpeedAccuracy()==3.0 && course.HeadingAccuracy()==10.0 &&
                  course.CourseAccuracy()==4.0);           
            
            TDetailedErrorReport detErrRep;
            positionInfo->GetDetailedErrorReport(detErrRep);
            TESTL(detErrRep.StanDeviOfLongitudeError()==5.0 && detErrRep.StanDeviOfLatiitudeError()==6.0 && 
                  detErrRep.StanDeviOfAltitudeError()==7.0 && detErrRep.StanDeviOfSemiMajorAxisError()==8.0 && 
                  detErrRep.StanDeviOfSemiMinorAxisError()==9.0 && detErrRep.OrientationOfSemiMajorAxisError()==10.0 && 
                  detErrRep.RmsValOfStanDeviOfRange()==11.0);  
            
            TGpsTimingMeasurementData gpsTimingData;
            positionInfo->GetGpsTimingData(gpsTimingData);
             
            TESTL(gpsTimingData.DataType()== TGpsTimingMeasurementData::EGpsTimingDataTypeUtran && 
                  gpsTimingData.NetworkMode()== TGpsTimingMeasurementData::ENetworkModeFdd &&
                  gpsTimingData.GPSTimingOfCellMsPart()==16383 && gpsTimingData.GPSTimingOfCellLsPart()==4294967295LL && 				  
                  gpsTimingData.ReferenceIdentity()==511 && gpsTimingData.Sfn()==4095);  
                           
            
            TESTL(positionInfo->GeoidalSeparation()==12.0 && positionInfo->MagneticVariation()==13.0 && 
                    positionInfo->CourseOverGroundMagnetic()==14.0);  
                            
            
            // Client recv - the gps position determined by the gps module.
            CheckForObserverEventTestsL(KTimeOut, *this);
            // Check Positioning Status Indicator callback - 
            //NOTE: GPS position and positioning indicator callback can be in any order
            CheckForObserverEventTestsL(KTimeOut, *this);
            //We should get Positioning Indicator flag off
            TESTL(iPosStatusCount==4);    
            TESTL(iPositioningStatus == CLbsPositioningStatus::ELbsPositioningStatusNotActive);

            TESTL(iState == EGpsLocReceived);
            
            // << ProcessLocationUpdate(SessionId, FinalNetworkPosition)
            // Return modules' position as FinalNetworkPosition
            proxy->CallL(ENetMsgProcessLocationUpdate, &iSessionId, positionInfo);
            }
        CleanupStack::PopAndDestroy(cleanupCnt);// sessionId, measurementInfo/positionInfo
        }

    // << ProcessSessionComplete(SessionId, KErrNone)	
    reason = KErrNone;
    proxy->CallL(ENetMsgProcessSessionComplete, &iSessionId, &reason);

    // << ENetMsgProcessStatusUpdate()
    MLbsNetworkProtocolObserver::TLbsNetProtocolServiceMask serviceMask = MLbsNetworkProtocolObserver::EServiceNone;
    proxy->CallL(ENetMsgProcessStatusUpdate, &serviceMask);

    // Wait for 10 seconds to ensure no additional responses turn up.
    delta = 10 * 1000 * 1000;
    TNetProtocolResponseType mType = proxy->WaitForResponse(delta);
    TESTL(mType == ENetMsgTimeoutExpired);

    // Done. Now cleanup...
    CleanupStack::PopAndDestroy(pWatch);
    CleanupStack::PopAndDestroy(); // pos
    CleanupStack::PopAndDestroy(); // server	
    CleanupStack::PopAndDestroy(proxy);
    
    return TestStepResult();
	}


// MPosServerObserver
void CT_LbsHybridUEAssistedNpudPosStatus::OnGetLastKnownPosition(TInt32 /*aErr*/, const TPositionInfoBase& /*aPosInfo*/)
	{
	TEST(EFalse); // Shouldn't see this...
	ReturnToTestStep();
	}

void CT_LbsHybridUEAssistedNpudPosStatus::OnNotifyPositionUpdate(TInt32 aErr, const TPositionInfoBase& aPosInfo)
	{
	// Verify error.
	if (aErr == KErrCancel)
		{
		iState = ERefLocReceived;
		}
	else
		{
	TEST(aErr == KErrNone);

	// Verify position.
	TEST(aPosInfo.PositionClassType() == EPositionInfoClass);

	// Expecting ref pos.
	if (iState == EInitializing)
		{
		iState = ERefLocReceived;

		TEST(aPosInfo.PositionMode() == TPositionModuleInfo::ETechnologyNetwork);

		// check for refpos details
		const TPositionInfo posInfo = static_cast<const TPositionInfo&>(aPosInfo);
		TESTL(ArgUtils::ComparePositionInfoToMolrRefPos(posInfo));
		}
		
	// Expecting network pos.
	else if (iState == ERefLocReceived)
		{
		iState = EGpsLocReceived;

		TEST(aPosInfo.PositionMode() == (TPositionModuleInfo::ETechnologyTerminal | TPositionModuleInfo::ETechnologyAssisted));
		}
	
	// Not expecting anything else.
	else
		{
		TEST(EFalse);
		}
		}
	ReturnToTestStep();
	}

void CT_LbsHybridUEAssistedNpudPosStatus::OnPositioningStatusUpdate(const CLbsPositioningStatus::TLbsPositioningStatus& aPositioningStatus)
    {
    iPosStatusCount++;
    iPositioningStatus = aPositioningStatus;
    ReturnToTestStep();
    }

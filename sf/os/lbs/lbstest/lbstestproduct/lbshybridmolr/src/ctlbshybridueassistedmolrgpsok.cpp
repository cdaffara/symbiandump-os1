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
#include "ctlbshybridueassistedmolrgpsok.h"
#include <lbs/test/tlbsutils.h>
#include "argutils.h"
#include <lbs/test/activeyield.h>


const TInt KN(2); // Number of times to send the measurement from GPS to NW

/**
Static Constructor
*/
CT_LbsHybridUEAssistedMOLRGPSOk* CT_LbsHybridUEAssistedMOLRGPSOk::New(CT_LbsHybridMOLRServer& aParent)
	{
	// Note that we do not use ELeave.
	// This means that having insufficient memory will return NULL;
	CT_LbsHybridUEAssistedMOLRGPSOk* testStep = new CT_LbsHybridUEAssistedMOLRGPSOk(aParent);
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
CT_LbsHybridUEAssistedMOLRGPSOk::CT_LbsHybridUEAssistedMOLRGPSOk(CT_LbsHybridMOLRServer& aParent) : CT_LbsHybridMOLRStep(aParent)
	{
	SetTestStepName(KLbsHybridUEAssistedMOLRGPSOk);
	iSessionId.SetSessionOwner(KRequestUid);
	iSessionId.SetSessionNum(0x0005);
	}


void CT_LbsHybridUEAssistedMOLRGPSOk::ConstructL()
	{
	// Create the base class objects.
	CT_LbsHybridMOLRStep::ConstructL();
	}


/**
 * Destructor
 */
CT_LbsHybridUEAssistedMOLRGPSOk::~CT_LbsHybridUEAssistedMOLRGPSOk()
	{
	}

// Hybrid - UE Assisted MO-LR GPS ok

TVerdict CT_LbsHybridUEAssistedMOLRGPSOk::doTestStepL()
	{
	INFO_PRINTF1(_L("CT_LbsHybridUEAssistedMOLRGPSOk::doTestStepL()"));	
	
	TInt result;
	
	// Stop the test if the preable failed
	TESTL(TestStepResult() == EPass);

	TInt testCaseId;
	if (GetIntFromConfig(ConfigSection(), KTestCaseId, testCaseId))
		{
		 INFO_PRINTF2(_L("test ID= %d"),testCaseId);
		}
		
	const TInt KTimeOut = 60*1000*1000;

	// Create Network Protocol Proxy
	CNetProtocolProxy* proxy = CNetProtocolProxy::NewL();
	CleanupStack::PushL(proxy);

	// >> AdviceSystemStatus(0) - GetCurrentCapabilitiesResponse
	result = proxy->WaitForResponse(KTimeOut);
    INFO_PRINTF3(_L("expecting ENetMsgGetCurrentCapabilitiesResponse(%d) got %d"),ENetMsgGetCurrentCapabilitiesResponse, result ); 
	TESTL(result == ENetMsgGetCurrentCapabilitiesResponse);
	
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
			
	if (testCaseId == 9)
		{
		 // see INC121701
		// this tests that a cancel on the P&S bus fron the locserver to the AGPS 
		// manager does not get overwritten by a status when the sub-session is closed
		
		// Note that here we do not put pos on cleanup stack i.e CleanupClosePushL(pos);
		// it emans that if the test fails by leaving between now and when we close the 
		// subsession then there will be a leak!
		// This is accepable beacuse if we did push it and then
		// CleanupStack::PopAndDestroy(); then the timings change
		// and the test is unable to create the back to back  cancel/status scenario
		// Set the max fix time for the client request to ensure the location server does not complete the request too soon during the test.
		TPositionUpdateOptions posOpts(TTimeIntervalMicroSeconds(0), TTimeIntervalMicroSeconds(KMOLRFixTime));
		pos.SetUpdateOptions(posOpts);
	
		pWatch = CPosServerWatcher::NewLC(pos, *this);
		// Request a self-locate MoLr.
		pWatch->IssueNotifyPositionUpdate();

		// >> RequestSelfLocation()
		//TESTL(proxy->WaitForResponse(KTimeOut) == ENetMsgRequestSelfLocation);
		result = proxy->WaitForResponse(KTimeOut);
	   INFO_PRINTF3(_L("expecting ENetMsgRequestSelfLocation(%d) got %d"),ENetMsgRequestSelfLocation, result ); 

       TESTL(result == ENetMsgRequestSelfLocation);

	   
		// Process the response.
		TLbsNetSessionId* 					sessionId = NULL;
		TLbsNetPosRequestOptionsAssistance*	opts = NULL;
		TInt								cleanupCnt;
	
		cleanupCnt = proxy->GetArgsLC(ENetMsgRequestSelfLocation, &sessionId, &opts);
		iSessionId.SetSessionNum(sessionId->SessionNum());
		iSessionId.SetSessionOwner(sessionId->SessionOwner());

		CleanupStack::PopAndDestroy(cleanupCnt);
		sessionId = NULL;
		opts = NULL;	
	
		// << ProcessStatusUpdate(EServiceSelfLocation)
		MLbsNetworkProtocolObserver::TLbsNetProtocolServiceMask activeServiceMask = MLbsNetworkProtocolObserver::EServiceSelfLocation;
		proxy->CallL(ENetMsgProcessStatusUpdate, &activeServiceMask);

		// do NOT send to LBS a location reques
		// thus ensuring that the cancel should arive fron LBS without 
		// and assistance data requests 
	
		pos.CancelRequest(EPositionerNotifyPositionUpdate);
		CheckForObserverEventTestsL(KTimeOut, *this);
		pos.Close();
	
		// now check that one and ONLY one cancel arrives
		// Wait for and process the response.
		TInt err = proxy->WaitForResponse( 1 * 1000 * 1000) ;
		// if a status overwrites the cancel on the P&S bus then no cancel will arrive and te test will fail!
		if (err==ENetMsgCancelSelfLocation)
			{
			INFO_PRINTF1(_L("Received a Cancel, as expected"));	
			}
		else
			{
	      INFO_PRINTF3(_L("Expected a Cancel()%d, but got %d"),ENetMsgCancelSelfLocation , err); 

			TESTL(err == ENetMsgCancelSelfLocation);
			}	
		
		// << ProcessSessionComplete(SessionId, KErrCancel)	
		TInt reason = KErrCancel;
		proxy->CallL(ENetMsgProcessSessionComplete, &iSessionId, &reason);
		
		const TInt KTimeOutOneSecond = 1*1000*1000;
			
		// now no more messages should arrive and definately NOT another cancel!
		// in order to demonstrate that this test actually works ..
		// in location server's CRequestQ::IssueStatus
		// add dleay before sending the cancel:
		// For example:
		// User::After(2* 1000 *1000);
		// TRAP_IGNORE(iLocReqBus->PublishRequestL(cancel));
		// this ensures that TWO cancels are sent from the location server to the agps manager
		// and exercises the code in the manager that prevents TWO cancels being sent to 
		// the protocol module
		// in order to see the test failing when a second cancel arrives also need to 
		// comment out the line	"if (!iSessionStartedStateAndCancelling)"
		// in agps manager's lbsmanagermainlogic.cpp
		
		err = proxy->WaitForResponse( KTimeOutOneSecond) ;
		if (err==ENetMsgCancelSelfLocation)
			{
			INFO_PRINTF1(_L("Test failed: received a SECOND cancel"));	
			}
		TESTL(err == ENetMsgTimeoutExpired); 


		// Done. Now cleanup...
		CleanupStack::PopAndDestroy(pWatch);
		CleanupStack::PopAndDestroy(); // server	
		CleanupStack::PopAndDestroy(proxy);
	
		return TestStepResult();
		}
	else
		{
		CleanupClosePushL(pos);
		// Set the max fix time for the client request to ensure the location server does not complete the request too soon during the test.
		TPositionUpdateOptions posOpts(TTimeIntervalMicroSeconds(0), TTimeIntervalMicroSeconds(KMOLRFixTime));
		pos.SetUpdateOptions(posOpts);
		
		pWatch = CPosServerWatcher::NewLC(pos, *this);

		// Request a self-locate MoLr.
		pWatch->IssueNotifyPositionUpdate();

		// >> RequestSelfLocation()
	
		result = proxy->WaitForResponse(KTimeOut);
        INFO_PRINTF3(_L("expecting ENetMsgRequestSelfLocation(%d) got %d"),ENetMsgRequestSelfLocation, result ); 
  		TESTL(result == ENetMsgRequestSelfLocation);

		// Process the response.
		TLbsNetSessionId* 					sessionId = NULL;
		TLbsNetPosRequestOptionsAssistance*	opts = NULL;
		TInt cleanupCnt;
		
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

		// << NotifyPositionUpdate()
		pWatch->IssueNotifyPositionUpdate();

		// Determine the value to take off the alpha2 value. This is required because we had to wait for the assistance data response.
		TTimeIntervalMicroSeconds microseconds;
		TTime stopTime;
		stopTime.HomeTime();
		microseconds = stopTime.MicroSecondsFrom(startTime); 
		TInt64 timeElapsed = microseconds.Int64();
		TInt delta = 2 * 1000 * 1000; // 2 secs.

		// >> RespondLocationRequest()
		result = proxy->WaitForResponse(ArgUtils::Alpha2() - timeElapsed - delta,ENetMsgRequestAssistanceData);
		INFO_PRINTF3(_L("expecting ENetMsgTimeoutExpired(%d) got %d"),ENetMsgTimeoutExpired, result ); 
		TESTL(result == ENetMsgTimeoutExpired);

		// Wait for and process the response.
		result = proxy->WaitForResponse(2 * delta,ENetMsgRequestAssistanceData);
		INFO_PRINTF3(_L("expecting ENetMsgRespondLocationRequest(%d) got %d"),ENetMsgRespondLocationRequest, result ); 
		TESTL(result == ENetMsgRespondLocationRequest);		// DONT get because the measurement data bus has not been created...

		sessionId = NULL;
		TPositionGpsMeasurementInfo* measurementInfo = NULL;
		cleanupCnt = proxy->GetArgsLC(ENetMsgRespondLocationRequest, &sessionId, &reason, &measurementInfo);
		TESTL(sessionId->SessionNum() == iSessionId.SessionNum());
		TESTL(reason == KErrNone);
		CleanupStack::PopAndDestroy(cleanupCnt);//sessionId, measurementInfo

		const TInt t = 8 * 1000 * 1000; // 8 secs.
		quality.SetMaxFixTime(t);

		TPositionExtendedSatelliteInfo* positionInfo = NULL;
		//TPositionInfo* positionInfo = NULL;
		
		for (TInt i = 0; i < KN; i++)
			{
			// << ProcessLocationRequest(SessionId, HybridMode, t)
			proxy->CallL(ENetMsgProcessLocationRequest, &iSessionId, &emergency, &service, &quality, &method);

			// >> RespondLocationRequest() - first measurement, second position.
			result = proxy->WaitForResponse(t + delta,ENetMsgRequestAssistanceData);
			INFO_PRINTF3(_L("expecting ENetMsgRespondLocationRequest(%d) got %d"),ENetMsgRespondLocationRequest, result ); 
			TESTL(result == ENetMsgRespondLocationRequest);

			sessionId = NULL;
			
			// Expect measurement first time.
			if (i < (KN-1))
				{
				measurementInfo = NULL;
				cleanupCnt = proxy->GetArgsLC(ENetMsgRespondLocationRequest, &sessionId, &reason, &measurementInfo);			

				// Check it is measurement
				TESTL(measurementInfo->PositionClassType() == EPositionGpsMeasurementInfoClass);
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
                      gpsTimingData.GPSTimingOfCellMsPart()==16383 && gpsTimingData.GPSTimingOfCellLsPart()==4294967295UL && 
                      gpsTimingData.ReferenceIdentity()==511 && gpsTimingData.Sfn()==4095);  
                               
                
                TESTL(positionInfo->GeoidalSeparation()==12.0 && positionInfo->MagneticVariation()==13.0 && 
                        positionInfo->CourseOverGroundMagnetic()==14.0);  
                                
				
				// Client recv - the gps position determined by the gps module.
				CheckForObserverEventTestsL(KTimeOut, *this);
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
	}


// MPosServerObserver
void CT_LbsHybridUEAssistedMOLRGPSOk::OnGetLastKnownPosition(TInt32 /*aErr*/, const TPositionInfoBase& /*aPosInfo*/)
	{
	TEST(EFalse); // Shouldn't see this...
	ReturnToTestStep();
	}

void CT_LbsHybridUEAssistedMOLRGPSOk::OnNotifyPositionUpdate(TInt32 aErr, const TPositionInfoBase& aPosInfo)
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

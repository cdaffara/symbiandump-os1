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
// This is the class implementation for the Module Information Tests
// EPOC includes.
// 
//

// LBS includes. 
#include <lbs/lbsextendedsatellite.h>
#include <lbs/lbsnetprotocolbase.h>
#include <lbs/lbsassistancedatabuilderset.h>
#include <e32math.h>
#include <lbs/lbsloccommon.h>
#include <lbs/lbsx3p.h>
#include <lbs/lbsnetprotocolbase.h>

// LBS test includes.
#include "ctlbsuebasedmolrself.h"
#include <lbs/test/tlbsutils.h>
#include "argutils.h"

#include <lbs/test/activeyield.h>


/**
Static Constructor
*/
CT_LbsUEBasedMOLRSelf* CT_LbsUEBasedMOLRSelf::New(CT_LbsHybridMOLRServer& aParent)
	{
	// Note the lack of ELeave.
	// This means that having insufficient memory will return NULL;
	CT_LbsUEBasedMOLRSelf* testStep = new CT_LbsUEBasedMOLRSelf(aParent);
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
CT_LbsUEBasedMOLRSelf::CT_LbsUEBasedMOLRSelf(CT_LbsHybridMOLRServer& aParent) : CT_LbsHybridMOLRStep(aParent)
	{
	SetTestStepName(KLbsUEBasedMOLRSelf);
	iSessionId.SetSessionOwner(KRequestUid);
	iSessionId.SetSessionNum(0x0001);
	}


void CT_LbsUEBasedMOLRSelf::ConstructL()
	{
	// Create the base class objects.
	CT_LbsHybridMOLRStep::ConstructL();
	}

/**
 * Destructor
 */
CT_LbsUEBasedMOLRSelf::~CT_LbsUEBasedMOLRSelf()
	{
	}


TVerdict CT_LbsUEBasedMOLRSelf::doTestStepL()
	{
	INFO_PRINTF1(_L("CT_LbsUEBasedMOLRSelf::doTestStepL()"));	
	// Stop the test if the preable failed
	TESTL(TestStepResult() == EPass);

	const TInt KTimeOut = 60*1000*1000;

	// Create Network Protocol Proxy
	CNetProtocolProxy* proxy = CNetProtocolProxy::NewL();
	CleanupStack::PushL(proxy);

	TESTL(proxy->WaitForResponse(KTimeOut) == ENetMsgGetCurrentCapabilitiesResponse);
	CLbsNetworkProtocolBase::TLbsSystemStatus status;
	TInt cleanupCnt;
	cleanupCnt = proxy->GetArgsLC(ENetMsgGetCurrentCapabilitiesResponse, &status);
	TESTL(status == CLbsNetworkProtocolBase::ESystemStatusNone);
	CleanupStack::PopAndDestroy(cleanupCnt);

// Do the /actual/ test step!
	RPositionServer server;
	TESTL(KErrNone == server.Connect());
	CleanupClosePushL(server);	

	RPositioner pos;
	TESTL(KErrNone == pos.Open(server));
	CleanupClosePushL(pos);
	
	CPosServerWatcher *pWatch = CPosServerWatcher::NewLC(pos, *this);

	// <<(App) NotifyPositionUpdate().
	pWatch->IssueNotifyPositionUpdate();

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
														KDefaultMaxFixTime,
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
	MLbsNetworkProtocolObserver::TLbsNetProtocolServiceMask activeServiceMask1 = MLbsNetworkProtocolObserver::EServiceSelfLocation;
	proxy->CallL(ENetMsgProcessStatusUpdate, &activeServiceMask1);

	// << ProcessLocationUpdate(SessionId, RefPosition)
	TPositionInfo refPosInfo = ArgUtils::MolrReferencePositionInfo();
	proxy->CallL(ENetMsgProcessLocationUpdate, &iSessionId, &refPosInfo);

	// TEST: Get NotifyPositionUpdate app side - get the ref pos.
	CheckForObserverEventTestsL(KTimeOut, *this);

	pWatch->IssueNotifyPositionUpdate();


	// >> RequestAssistanceData - as a result of the second client request.
	TLbsAsistanceDataGroup dataMask;	
	TESTL(proxy->WaitForResponse(KTimeOut) == ENetMsgRequestAssistanceData); 
	cleanupCnt = proxy->GetArgsLC(ENetMsgRequestAssistanceData, &dataMask);
	TESTL(dataMask == EAssistanceDataNone);
	CleanupStack::PopAndDestroy(cleanupCnt);

	// << ProcessAssistanceData()
	dataMask = EAssistanceDataReferenceTime;
	RLbsAssistanceDataBuilderSet assistanceData;
	ArgUtils::PopulateLC(assistanceData);
	TInt reason = KErrNone;
	proxy->CallL(ENetMsgProcessAssistanceData, &dataMask, &assistanceData, &reason);
	CleanupStack::PopAndDestroy(); // assistanceData

	// << ProcessLocationRequest() - this will cancel the client req.
	TBool emergency = EFalse;
	MLbsNetworkProtocolObserver::TLbsNetProtocolService	service = MLbsNetworkProtocolObserver::EServiceSelfLocation;
	TLbsNetPosRequestQuality quality = ArgUtils::Quality();
	TLbsNetPosRequestMethod method   = ArgUtils::RequestHybridMethod();
	proxy->CallL(ENetMsgProcessLocationRequest, &iSessionId, &emergency, &service, &quality, &method);

	// >> RequestAssistanceData - as a result of the NRH request.
	TESTL(proxy->WaitForResponse(KTimeOut) == ENetMsgRequestAssistanceData); 
	cleanupCnt = proxy->GetArgsLC(ENetMsgRequestAssistanceData, &dataMask);
	TESTL(dataMask == EAssistanceDataNone);
	CleanupStack::PopAndDestroy(cleanupCnt);

	// >> RespondLocationRequest()
	TESTL(proxy->WaitForResponse(KTimeOut) == ENetMsgRespondLocationRequest);
	sessionId = NULL;
	reason = KErrNone;
	TPositionExtendedSatelliteInfo* getPositionInfo = NULL;
	cleanupCnt = proxy->GetArgsLC(ENetMsgRespondLocationRequest, &sessionId, &reason, &getPositionInfo);
	TESTL(sessionId->SessionNum() == iSessionId.SessionNum());
	TESTL(reason == KErrNone);
	/* Check whether ExtendedSatelliteInfo data received by protocol module is the same as data fed to the GPS module or not. 
    Latitude(49.2),Longitude(3.5),Altitude(50),HorizontalAccuracy(2),VerticalAccuracy(3), speed(26.0),vertical speed(20.0),heading(25.0),course(30.0), 
    SPEED_ACCURACY(2.0),VERTICAL_SPEED_ACCURACY(3.0),HEADING_ACCURACY(10.0),COURSE_ACCURACY(4.0),
    SD_OF_LONG_ERROR(5.0),SD_OF_LAT_ERROR(6.0),SD_OF_ALT_ERROR(7.0),SD_OF_SEMI_MAJOR_AXIS_ERROR(8.0),
    SD_OF_SEMI_MINOR_AXIS_ERROR(9.0),ORIEN_OF_SEMI_MAJOR_AXIS_ERROR(10.0),RMS_VAL_OF_SD_OF_RANGE(11.0),
    GEOIDAL_SEPARATION(12.0),MAGNETIC_VARIATION(13.0),COURSE_OVER_GROUND_MAGNETIC(14.0)
    GPS_TIMING_OF_CELL_MsPart(16383),GPS_TIMING_OF_CELL_LsPart(4294967295),REFERENCE_IDENTITY(511)
    SFN(4095)*/  
	    
    TPosition gpsPos;
    getPositionInfo->GetPosition(gpsPos);
    TESTL(gpsPos.Latitude()==49.2 && gpsPos.Longitude()== 3.5 && gpsPos.Altitude()==50 
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
	
	
 	// << ProcessLocationUpdate(SessionId, FinalNetworkPosition)
	proxy->CallL(ENetMsgProcessLocationUpdate, &iSessionId, getPositionInfo);

	CleanupStack::PopAndDestroy(cleanupCnt);
	
	// Client recv - the gps position determined by the gps module.
	CheckForObserverEventTestsL(KTimeOut, *this);
	TESTL(iState == EGpsLocReceived);

	// << ProcessSessionComplete()
	reason = KErrNone;
	proxy->CallL(ENetMsgProcessSessionComplete, &iSessionId, &reason);

	// << ProcessStatusUpdate()
	MLbsNetworkProtocolObserver::TLbsNetProtocolServiceMask activeServiceMask2 = MLbsNetworkProtocolObserver::EServiceNone;	
	proxy->CallL(ENetMsgProcessStatusUpdate, &activeServiceMask2);

	// Wait for 10 seconds to ensure no additional responses turn up.
	TInt delta = 10 * 1000 * 1000;
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
void CT_LbsUEBasedMOLRSelf::OnGetLastKnownPosition(TInt32 /*aErr*/, const TPositionInfoBase& /*aPosInfo*/)
	{
	TEST(EFalse); // Shouldn't see this...
	ReturnToTestStep();
	}


void CT_LbsUEBasedMOLRSelf::OnNotifyPositionUpdate(TInt32 aErr, const TPositionInfoBase& aPosInfo)
	{
	// Verify error.
	TEST(aErr == KErrNone);

	// Verify position.

	// Expecting ref pos.
	if (iState == EInitializing)
		{
		iState = ERefLocReceived;
		
		TEST(aPosInfo.PositionClassType() == EPositionInfoClass);
		TEST(aPosInfo.PositionMode() == TPositionModuleInfo::ETechnologyNetwork);

		// check for refpos details
		const TPositionInfo posInfo = static_cast<const TPositionInfo&>(aPosInfo);
		TEST(ArgUtils::ComparePositionInfoToMolrRefPos(posInfo));
		}
		
	// Expecting gps pos.
	else if (iState == ERefLocReceived)
		{
		iState = EGpsLocReceived;

		TEST(aPosInfo.PositionClassType() == EPositionInfoClass);
		TEST(aPosInfo.PositionMode() == (TPositionModuleInfo::ETechnologyTerminal | TPositionModuleInfo::ETechnologyAssisted));
		}
	
	// Not expecting anything else.
	else
		{
		TEST(EFalse);
		}

	ReturnToTestStep();
	}


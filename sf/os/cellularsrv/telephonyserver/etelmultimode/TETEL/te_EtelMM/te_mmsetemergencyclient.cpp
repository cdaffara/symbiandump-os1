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

#include <e32test.h>

#include "TE_EtelMMTestStepBase.h"
#include "te_mmsetemergencyclient.h"
#include "testdef.h"

CTestMMSetEmgClientLcsReqPreallocHeap::CTestMMSetEmgClientLcsReqPreallocHeap()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestMMSetEmgClientLcsReqPreallocHeap"));
	}

enum TVerdict CTestMMSetEmgClientLcsReqPreallocHeap::doTestStepL()
//
// To check that SetEmergencyClient API preallocate the heap memory for handling Emergency Mobile
// Terminated Location service request.
//
	{
	INFO_PRINTF1(_L("Test connecting SetEmergencyClient API preallocate the heap memory for handling Emergency Mobile Terminated Location service request."));
	// OOM testing only works with debug builds
#ifdef _DEBUG
	TInt setemergency;
	setemergency=iPhone.SetEmergencyClient(RPhone::EEmergencyLCSRequest);
	TEST(setemergency==KErrNone);
    INFO_PRINTF2(_L("Test %d - iPhone.SetEmergencyClient(RPhone::EEmergencyLCSRequest); "), setemergency);
    OOMTest_LBS();
#else
	INFO_PRINTF1(_L("Test disabled on release builds."));	
#endif // _DEBUG
    return TestStepResult();
	}
	
CTestMMSetEmgClientLcsReqAndSetPrClient::CTestMMSetEmgClientLcsReqAndSetPrClient()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestMMSetEmgClientLcsReqAndSetPrClient"));
	}

enum TVerdict CTestMMSetEmgClientLcsReqAndSetPrClient::doTestStepL()
//
// To test a combination of SetPriorityClient API and SetEmergencyClient(LCSRequest) API.
//
	{
	INFO_PRINTF1(_L("To test a combination of SetPriorityClient API and SetEmergencyClient(LCSRequest) API."));
	// OOM testing only works with debug builds
#ifdef _DEBUG
	TInt setemergency;
	setemergency=iPhone.SetEmergencyClient(RPhone::EEmergencyLCSRequest);
    TEST(setemergency==KErrNone);
    INFO_PRINTF2(_L("Test %d - iPhone.SetEmergencyClient(RPhone::EEmergencyLCSRequest); "), setemergency);
  	TInt ret=iTelServer.SetPriorityClient();
	TEST(ret==KErrAlreadyExists);
 	INFO_PRINTF1(_L("Test 	iTelServer.SetPriorityClient "));
 	OOMTest_LBS();
#else
	INFO_PRINTF1(_L("Test disabled on release builds."));	
#endif // _DEBUG	
    return TestStepResult();
	}

CTestMMSetEmgClientVoiceReqAndSetPrClient::CTestMMSetEmgClientVoiceReqAndSetPrClient()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestMMSetEmgClientVoiceReqAndSetPrClient"));
	}
	
enum TVerdict CTestMMSetEmgClientVoiceReqAndSetPrClient::doTestStepL()
//
// To test a combination of SetEmergencyClient(VoiceCallRequest) API and SetPriorityClient API.
//
	{
	INFO_PRINTF1(_L("To test a combination of SetEmergencyClient(VoiceCallRequest) API and SetPriorityClient API."));
#ifdef _DEBUG
	TInt setemergency;
	setemergency=iPhone.SetEmergencyClient(RPhone::EEmergencyCSVoiceCallRequest);
    TEST(setemergency==KErrNone);
    INFO_PRINTF2(_L("Test %d - iPhone.SetEmergencyClient(RPhone::EEmergencyCSVoiceCallRequest); "), setemergency);
  	TInt ret=iTelServer.SetPriorityClient();
	TEST(ret==KErrAlreadyExists);
 	INFO_PRINTF1(_L("Test 	iTelServer.SetPriorityClient "));
 	OOMTest_DialEmergencyCall();
#else
	INFO_PRINTF1(_L("Test disabled on release builds."));	
#endif // _DEBUG 	
   	return TestStepResult();
	}

CTestMMSetEmgClientHeapSuffForLBS::CTestMMSetEmgClientHeapSuffForLBS()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestMMSetEmgClientHeapSuffForLBS"));
	}
			
enum TVerdict CTestMMSetEmgClientHeapSuffForLBS::doTestStepL()
//
// To check preallocated heap is sufficient to handle all Location Based Services.
//
	{
	INFO_PRINTF1(_L("To check preallocated heap is sufficient to handle all Location Based Services."));
	TInt setemergency;
	setemergency=iPhone.SetEmergencyClient(RPhone::EEmergencyLCSRequest);
    TEST(setemergency==KErrNone);
    INFO_PRINTF2(_L("Test %d - iPhone.SetEmergencyClient(RPhone::EEmergencyLCSRequest); "), setemergency);
 	TRequestStatus mtlrStatus,mcontrolStatus,mtlrStatus2,sendmtlrresponsereqStatus,mcontrolStatus1;
	TRequestStatus sendmcreportreqStatus,sendmtlrresponse2,sendmcreport2reqStatus;
    RMobileLocationServices::TNotifyMtlrV7   iNotifyMtlr;
	iNotifyMtlr.iLocationInfo.iLcsClientName.iLcsDataCodingScheme = DMMTSY_DATA_CODING_SCHEME1;
	RMobileLocationServices::TMtlrResponse  iMtlrResponse;
	iMeasurementControl.iMeasurementIdentity=DMMTSY_PHONE_LCS_MEASUREMENTID;
	RMobileLocationServices::TMeasurementControlV7Pckg  measurementControlPckg(iMeasurementControl);
	RMobileLocationServices::TMeasurementReportV7Pckg  measurementReportPckg(iMeasurementReport);
	RMobileLocationServices::TNotifyMtlrV7Pckg  notifyMtlrPckg(iNotifyMtlr);

	iLocationSvc.NotifyMtlr(mtlrStatus,notifyMtlrPckg);
	iLocationSvc.NotifyMeasurementControl(mcontrolStatus,measurementControlPckg);
   	User::WaitForRequest(mtlrStatus);
	INFO_PRINTF2(_L("Notify MTLR %d "),mtlrStatus.Int()); 
    TEST(mtlrStatus == KErrNone);
    TEST(iNotifyMtlr.iNotificationType == DMMTSY_NOTIFICATION_TYPE1);
	INFO_PRINTF2(_L("Test %d - RMobileLocationServices::NotifyMtlr "), mtlrStatus.Int());
    iLocationSvc.NotifyMtlr( mtlrStatus2,notifyMtlrPckg);
   // send Mtlr response as Permission granted
    iMtlrResponse=DMMTSY_MTLR_RESPONSE1;   
    iLocationSvc.SendMtlrResponse(sendmtlrresponsereqStatus,iMtlrResponse);
    User::WaitForRequest(sendmtlrresponsereqStatus);
    TEST(sendmtlrresponsereqStatus == KErrNone);
 	INFO_PRINTF2(_L("Test %d - RMobileLocationServices::SendMtlrResponse "), sendmtlrresponsereqStatus.Int());
   
	// wait for measurement control request to complete
    User::WaitForRequest(mcontrolStatus);
    TEST(mcontrolStatus==KErrNone);
	TEST(iMeasurementControl.iMeasurementIdentity==DMMTSY_PHONE_LCS_MEASUREMENTID);
	INFO_PRINTF2(_L("Test %d - RMobileLocationServices::NotifyMeasurementControl "), mcontrolStatus.Int());
	INFO_PRINTF2(_L("Measurement Identity=%d"),iMeasurementControl.iMeasurementIdentity);

	TEST(iMeasurementControl.iVelocityRequested == DMMTSY_PHONE_LCS_MC_VELOCITY_REQUESTED);
	TEST(iMeasurementControl.iMeasReportTransferMode == DMMTSY_PHONE_LCS_MC_RPTTRANSFERMODE);
	
	//Reference Time Data populated and status is false
	TEST(iMeasurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iReferencTime.iGpsWeek == DMMTSY_PHONE_LCS_MC_GPS_REFTIME_GPSWK);		
	TEST(iMeasurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iReferencTime.iGpsTowOneMsec == DMMTSY_PHONE_LCS_MC_GPS_REFTIME_GPSTOWONEMSCE );
	TEST(iMeasurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iGpsAddlDataStatus.iReferenceTimeRequest == EFalse);	
		
	//	Acquisition Assistance Data populated and status is false
	TEST(iMeasurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iReferencTime.iUtranGpsRefTime.iLsPart == DMMTSY_PHONE_LCS_MC_GPS_UTRANREFTIME_LSPART);
	TEST(iMeasurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iReferencTime.iUtranGpsRefTime.iMsPart == DMMTSY_PHONE_LCS_MC_GPS_UTRANREFTIME_MSPART);
	TEST(iMeasurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iReferencTime.iUtranGpsRefTime.iSfn == DMMTSY_PHONE_LCS_MC_GPS_UTRANREFTIME_SFN);
	TEST(iMeasurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iGpsAddlDataStatus.iAcquisitionAssistanceReq == EFalse);

	// Reference Location data populated and status is false
	TEST(iMeasurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iReferenceLocation.iLatitudeSign == DMMTSY_PHONE_LCS_MC_GPS_REFLOC_LATSIGN1);
	TEST(iMeasurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iReferenceLocation.iLatitude == DMMTSY_PHONE_LCS_MC_GPS_REFLOC_LAT);
	TEST(iMeasurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iReferenceLocation.iLongitude == DMMTSY_PHONE_LCS_MC_GPS_REFLOC_LONG) ;
	TEST(iMeasurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iGpsAddlDataStatus.iReferenceLocationRequest == EFalse);
	
	//	NavigationModel data populated and status is false	
	TEST(iMeasurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iNavigationModel.iEphemerisParameter.iCodeOnL2 == DMMTSY_PHONE_LCS_MC_GPS_NAVIGATION_EPHEMERIS);
	TEST(iMeasurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iNavigationModel.iSatID == DMMTSY_PHONE_LCS_MC_GPS_NAVIGATION_SATID);
	TEST(iMeasurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iNavigationModel.iSatelliteStatus == DMMTSY_PHONE_LCS_MC_GPS_NAVIGATION_SATSTATUS1);
	TEST(iMeasurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iGpsAddlDataStatus.iNavigationModelRequest == EFalse);
	
	//DGPS corrections data populated and status is false		
	TEST(iMeasurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iDgpsCorrections.iGpsTow == DMMTSY_PHONE_LCS_MC_GPS_CORRECTIONS_TOW);	
	TEST(iMeasurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iGpsAddlDataStatus.iDgpsCorrectionsRequest == EFalse);	
	
	//	Ionospheric data populated and status is false	
	TEST(iMeasurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iIonosphericModel.iAlfa1 == DMMTSY_PHONE_LCS_MC_GPS_IONOSPHERIC_MODEL1);
	TEST(iMeasurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iGpsAddlDataStatus.iIonosphericModelRequest == EFalse);	

	//UTC data populated and status is false		
	TEST(iMeasurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iUtcModel.iA1 == DMMTSY_PHONE_LCS_MC_GPS_UTC_MODEL1);
	TEST(iMeasurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iGpsAddlDataStatus.iUtcModelRequest == EFalse);
			
	//Almanac data populated and status is false
	TEST(iMeasurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iAlmanac.iWnA == DMMTSY_PHONE_LCS_MC_GPS_ALMANAC);
	TEST(iMeasurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iGpsAddlDataStatus.iAlmanacRequest	== EFalse);
	
	//RealTime integrity data populated and status is false
	TEST(iMeasurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iBadSatList[0] == DMMTSY_PHONE_LCS_MC_GPS_BADSATLIST);
	TEST(iMeasurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iGpsAddlDataStatus.iRealTimeIntegrityRequest == EFalse);
	
	TEST(iMeasurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iAcquisitionAssistance.iGpsReferenceTime == DMMTSY_PHONE_LCS_MC_GPS_ACQASSISTANCE_TIME);
	TEST(iMeasurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iAcquisitionAssistance.iUtranGpsReferenceTime.iPrimaryScramblingCode == DMMTSY_PHONE_LCS_MC_GPS_MODE_PRIMARYCODE);
	TEST(iMeasurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iAcquisitionAssistance.iUtranGpsReferenceTime.iCellParametersID == DMMTSY_PHONE_LCS_MC_GPS_MODE_CELLPARAMID);
	TEST(iMeasurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iReferencTime.iUtranGpsRefTime.iModeSpecificInfoStatus == DMMTSY_PHONE_LCS_MC_GPS_UTRANREFTIME_MODESTATUS_REQUESTED);
	TEST(iMeasurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iReferencTime.iUtranGpsRefTime.iModeSpecificInfo.iPrimaryScramblingCode ==DMMTSY_PHONE_LCS_MC_GPS_MODE_CELLPARAMID);
	TEST(iMeasurementControl.iMeasurementCommand.iSetup.iUePosReportingQuantity.iHorzAccuracy == DMMTSY_PHONE_LCS_MC_HORZACCURACY);
	TEST(iMeasurementControl.iMeasurementCommand.iSetup.iUePosReportingQuantity.iVertAccuracy == DMMTSY_PHONE_LCS_MC_VERTACCURACY);
	//Additional Assistance Data is not required
	TEST(iMeasurementControl.iMeasurementCommand.iSetup.iUePosReportingQuantity.iAddlAssistanceDataReq == EFalse);
	
    // Send Measurement Report containing complete data
    iMeasurementReport.iMeasuredResults.iTUePosMeasuredResults.iPosErrorStatus = DMMTSY_PHONE_LCS_MCR_POSERROR_STATUS_FALSE;
    iMeasurementReport.iMeasurementIdentity=DMMTSY_PHONE_LCS_MEASUREMENTID;
    
	//velocity estimate status is EHorzVelPresent, send report with Horiontal velocity
    iMeasurementReport.iVelEstimateStatus = DMMTSY_PHONE_LCS_MC_GPS_VELOCITYEST_STATUS1;
    iMeasurementReport.iVelocityEstimate.iHorzVelocity.iHorzSpeed = DMMTSY_PHONE_LCS_MC_GPS_VELOCITYEST_HORZ_SPEED;
    iMeasurementReport.iVelocityEstimate.iHorzVelocity.iBearing = DMMTSY_PHONE_LCS_MC_GPS_VELOCITYEST_HORZ_BEARING;
	
	// status is EUtranGpsRefTimeResultPresent, send report containing Reference time result
	iMeasurementReport.iMeasuredResults.iTUePosMeasuredResults.iUePosPositionEstimateInfo.iPosEstimateStatus = DMMTSY_PHONE_LCS_MC_GPS_POSESTINFO1;    
	iMeasurementReport.iMeasuredResults.iTUePosMeasuredResults.iUePosPositionEstimateInfo.iPosEstimateRefTime.iUtranGpsRefTimeResult.iLsPart = DMMTSY_PHONE_LCS_MC_GPS_UTRANREFTIME_LSPART;
	iMeasurementReport.iMeasuredResults.iTUePosMeasuredResults.iUePosPositionEstimateInfo.iPosEstimateRefTime.iUtranGpsRefTimeResult.iMsPart = DMMTSY_PHONE_LCS_MC_GPS_UTRANREFTIME_MSPART;	
	iMeasurementReport.iMeasuredResults.iTUePosMeasuredResults.iUePosPositionEstimateInfo.iPosEstimateRefTime.iUtranGpsRefTimeResult.iSfn =	DMMTSY_PHONE_LCS_MC_GPS_UTRANREFTIME_SFN;
	
	//Measurement mode status is false so send report with iTddRefID
	iMeasurementReport.iMeasuredResults.iTUePosMeasuredResults.iUePosPositionEstimateInfo.iPosEstimateRefTime.iUtranGpsRefTimeResult.iGpsMeasModeStatus = EFalse;
	iMeasurementReport.iMeasuredResults.iTUePosMeasuredResults.iUePosPositionEstimateInfo.iPosEstimateRefTime.iUtranGpsRefTimeResult.iGpsMeasModeSpecificInfo.iTddRefID=DMMTSY_PHONE_LCS_MC_GPS_UTRANMODE_MODE_TDDID;

	//Position estimate status is EEllipsoidPointPresent, send report containing position estimate for Ellipsoid point
	iMeasurementReport.iMeasuredResults.iTUePosMeasuredResults.iUePosPositionEstimateInfo.iPosEstimateStatus = DMMTSY_PHONE_LCS_MC_GPS_POSESTINFO5;
	iMeasurementReport.iMeasuredResults.iTUePosMeasuredResults.iUePosPositionEstimateInfo.iPostionEstimate.iEllipsoidPoint.iLatitude = 	DMMTSY_PHONE_LCS_MC_GPS_POSEST_ELIPSPT_LAT;
	iMeasurementReport.iMeasuredResults.iTUePosMeasuredResults.iUePosPositionEstimateInfo.iPostionEstimate.iEllipsoidPoint.iLongitude = DMMTSY_PHONE_LCS_MC_GPS_POSEST_ELIPSPT_LON;
	iMeasurementReport.iMeasuredResults.iTUePosMeasuredResults.iUePosPositionEstimateInfo.iPostionEstimate.iEllipsoidPoint.iLatitudeSign = DMMTSY_PHONE_LCS_MC_GPS_POSEST_ELIPSOIDPT_LATSIGN1;
	
 	iMeasurementReport.iMeasuredResults.iTUePosMeasuredResults.iUePosGpsMeasurement.iGpsMeasurementParamList[0].iSatID = DMMTSY_PHONE_LCS_MC_GPS_PARAMLIST_SATID;
 	iMeasurementReport.iMeasuredResults.iTUePosMeasuredResults.iUePosGpsMeasurement.iGpsMeasurementParamList[0].iFractionalGpsChips = DMMTSY_PHONE_LCS_MC_GPS_PARAMLIST_GPSCHIPS; 
 	iMeasurementReport.iMeasuredResults.iTUePosMeasuredResults.iUePosGpsMeasurement.iGpsMeasurementParamList[0].iMultipathIndicator = DMMTSY_PHONE_LCS_MC_GPS_PARAMLIST_PATHINDICATOR1 ;
	
 	iMeasurementReport.iMeasuredResults.iTUePosMeasuredResults.iUePosGpsMeasurement.iGpsMeasurementParamList[0].iSatID = DMMTSY_PHONE_LCS_MC_GPS_PARAMLIST_SATID;
 	iMeasurementReport.iMeasuredResults.iTUePosMeasuredResults.iUePosGpsMeasurement.iGpsMeasurementParamList[0].iFractionalGpsChips = DMMTSY_PHONE_LCS_MC_GPS_PARAMLIST_GPSCHIPS; 
 	
 	iLocationSvc.NotifyMeasurementControl(mcontrolStatus1,measurementControlPckg);  

	User::WaitForRequest(mtlrStatus2);
    TEST(mtlrStatus2 == KErrNone);
    TEST(iNotifyMtlr.iNotificationType == DMMTSY_NOTIFICATION_TYPE1);
	INFO_PRINTF2(_L("Test %d - RMobileLocationServices::NotifyMtlr "), mtlrStatus2.Int());

 	iLocationSvc.SendMeasurementReport(sendmcreportreqStatus,measurementReportPckg);
    User::WaitForRequest(sendmcreportreqStatus);
    TEST(sendmcreportreqStatus==KErrNone);
    INFO_PRINTF2(_L("Test %d - RMobileLocationServices::SendMeasurementReport "), sendmcreportreqStatus.Int());

 	iMtlrResponse=DMMTSY_MTLR_RESPONSE1;    
    iLocationSvc.SendMtlrResponse(sendmtlrresponse2,iMtlrResponse);
    User::WaitForRequest(sendmtlrresponse2);
    TEST(sendmtlrresponse2 == KErrNone);
 	INFO_PRINTF2(_L("Test %d - RMobileLocationServices::SendMtlrResponse "), sendmtlrresponse2.Int());

	// wait for measurement control request to complete
    User::WaitForRequest(mcontrolStatus1);
    TEST(mcontrolStatus1==KErrNone);
	TEST(iMeasurementControl.iMeasurementIdentity==DMMTSY_PHONE_LCS_MEASUREMENTID);
	INFO_PRINTF2(_L("Test %d - RMobileLocationServices::NotifyMeasurementControl "), mcontrolStatus1.Int());
	INFO_PRINTF2(_L("Measurement Identity=%d"),iMeasurementControl.iMeasurementIdentity);

	TEST(iMeasurementControl.iVelocityRequested == DMMTSY_PHONE_LCS_MC_VELOCITY_REQUESTED);
	TEST(iMeasurementControl.iMeasReportTransferMode == DMMTSY_PHONE_LCS_MC_RPTTRANSFERMODE);
	
	//Reference Time Data populated and status is false
	TEST(iMeasurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iReferencTime.iGpsWeek == DMMTSY_PHONE_LCS_MC_GPS_REFTIME_GPSWK);		
	TEST(iMeasurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iReferencTime.iGpsTowOneMsec == DMMTSY_PHONE_LCS_MC_GPS_REFTIME_GPSTOWONEMSCE );
	TEST(iMeasurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iGpsAddlDataStatus.iReferenceTimeRequest == EFalse);	
		
	//	Acquisition Assistance Data populated and status is false
	TEST(iMeasurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iReferencTime.iUtranGpsRefTime.iLsPart == DMMTSY_PHONE_LCS_MC_GPS_UTRANREFTIME_LSPART);
	TEST(iMeasurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iReferencTime.iUtranGpsRefTime.iMsPart == DMMTSY_PHONE_LCS_MC_GPS_UTRANREFTIME_MSPART);
	TEST(iMeasurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iReferencTime.iUtranGpsRefTime.iSfn == DMMTSY_PHONE_LCS_MC_GPS_UTRANREFTIME_SFN);
	TEST(iMeasurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iGpsAddlDataStatus.iAcquisitionAssistanceReq == EFalse);

	// Reference Location data populated and status is false
	TEST(iMeasurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iReferenceLocation.iLatitudeSign == DMMTSY_PHONE_LCS_MC_GPS_REFLOC_LATSIGN1);
	TEST(iMeasurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iReferenceLocation.iLatitude == DMMTSY_PHONE_LCS_MC_GPS_REFLOC_LAT);
	TEST(iMeasurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iReferenceLocation.iLongitude == DMMTSY_PHONE_LCS_MC_GPS_REFLOC_LONG) ;
	TEST(iMeasurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iGpsAddlDataStatus.iReferenceLocationRequest == EFalse);
	
	//	NavigationModel data populated and status is false	
	TEST(iMeasurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iNavigationModel.iEphemerisParameter.iCodeOnL2 == DMMTSY_PHONE_LCS_MC_GPS_NAVIGATION_EPHEMERIS);
	TEST(iMeasurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iNavigationModel.iSatID == DMMTSY_PHONE_LCS_MC_GPS_NAVIGATION_SATID);
	TEST(iMeasurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iNavigationModel.iSatelliteStatus == DMMTSY_PHONE_LCS_MC_GPS_NAVIGATION_SATSTATUS1);
	TEST(iMeasurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iGpsAddlDataStatus.iNavigationModelRequest == EFalse);
	
	//DGPS corrections data populated and status is false		
	TEST(iMeasurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iDgpsCorrections.iGpsTow == DMMTSY_PHONE_LCS_MC_GPS_CORRECTIONS_TOW);	
	TEST(iMeasurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iGpsAddlDataStatus.iDgpsCorrectionsRequest == EFalse);	
	
	//	Ionospheric data populated and status is false	
	TEST(iMeasurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iIonosphericModel.iAlfa1 == DMMTSY_PHONE_LCS_MC_GPS_IONOSPHERIC_MODEL1);
	TEST(iMeasurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iGpsAddlDataStatus.iIonosphericModelRequest == EFalse);	

	//UTC data populated and status is false		
	TEST(iMeasurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iUtcModel.iA1 == DMMTSY_PHONE_LCS_MC_GPS_UTC_MODEL1);
	TEST(iMeasurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iGpsAddlDataStatus.iUtcModelRequest == EFalse);
			
	//Almanac data populated and status is false
	TEST(iMeasurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iAlmanac.iWnA == DMMTSY_PHONE_LCS_MC_GPS_ALMANAC);
	TEST(iMeasurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iGpsAddlDataStatus.iAlmanacRequest	== EFalse);
	
	//RealTime integrity data populated and status is false
	TEST(iMeasurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iBadSatList[0] == DMMTSY_PHONE_LCS_MC_GPS_BADSATLIST);
	TEST(iMeasurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iGpsAddlDataStatus.iRealTimeIntegrityRequest == EFalse);
	
	TEST(iMeasurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iAcquisitionAssistance.iGpsReferenceTime == DMMTSY_PHONE_LCS_MC_GPS_ACQASSISTANCE_TIME);
	TEST(iMeasurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iAcquisitionAssistance.iUtranGpsReferenceTime.iPrimaryScramblingCode == DMMTSY_PHONE_LCS_MC_GPS_MODE_PRIMARYCODE);
	TEST(iMeasurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iAcquisitionAssistance.iUtranGpsReferenceTime.iCellParametersID == DMMTSY_PHONE_LCS_MC_GPS_MODE_CELLPARAMID);
	TEST(iMeasurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iReferencTime.iUtranGpsRefTime.iModeSpecificInfoStatus == DMMTSY_PHONE_LCS_MC_GPS_UTRANREFTIME_MODESTATUS_REQUESTED);
	TEST(iMeasurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iReferencTime.iUtranGpsRefTime.iModeSpecificInfo.iPrimaryScramblingCode ==DMMTSY_PHONE_LCS_MC_GPS_MODE_CELLPARAMID);
	TEST(iMeasurementControl.iMeasurementCommand.iSetup.iUePosReportingQuantity.iHorzAccuracy == DMMTSY_PHONE_LCS_MC_HORZACCURACY);
	TEST(iMeasurementControl.iMeasurementCommand.iSetup.iUePosReportingQuantity.iVertAccuracy == DMMTSY_PHONE_LCS_MC_VERTACCURACY);
	//Additional Assistance Data is not required
	TEST(iMeasurementControl.iMeasurementCommand.iSetup.iUePosReportingQuantity.iAddlAssistanceDataReq == EFalse);
	
    // Send Measurement Report containing complete data
    iMeasurementReport.iMeasuredResults.iTUePosMeasuredResults.iPosErrorStatus = DMMTSY_PHONE_LCS_MCR_POSERROR_STATUS_FALSE;
    iMeasurementReport.iMeasurementIdentity=DMMTSY_PHONE_LCS_MEASUREMENTID;
    
	//velocity estimate status is EHorzVelPresent, send report with Horiontal velocity
    iMeasurementReport.iVelEstimateStatus = DMMTSY_PHONE_LCS_MC_GPS_VELOCITYEST_STATUS1;
    iMeasurementReport.iVelocityEstimate.iHorzVelocity.iHorzSpeed = DMMTSY_PHONE_LCS_MC_GPS_VELOCITYEST_HORZ_SPEED;
    iMeasurementReport.iVelocityEstimate.iHorzVelocity.iBearing = DMMTSY_PHONE_LCS_MC_GPS_VELOCITYEST_HORZ_BEARING;
	
	// status is EUtranGpsRefTimeResultPresent, send report containing Reference time result
	iMeasurementReport.iMeasuredResults.iTUePosMeasuredResults.iUePosPositionEstimateInfo.iPosEstimateStatus = DMMTSY_PHONE_LCS_MC_GPS_POSESTINFO1;    
	iMeasurementReport.iMeasuredResults.iTUePosMeasuredResults.iUePosPositionEstimateInfo.iPosEstimateRefTime.iUtranGpsRefTimeResult.iLsPart = DMMTSY_PHONE_LCS_MC_GPS_UTRANREFTIME_LSPART;
	iMeasurementReport.iMeasuredResults.iTUePosMeasuredResults.iUePosPositionEstimateInfo.iPosEstimateRefTime.iUtranGpsRefTimeResult.iMsPart = DMMTSY_PHONE_LCS_MC_GPS_UTRANREFTIME_MSPART;	
	iMeasurementReport.iMeasuredResults.iTUePosMeasuredResults.iUePosPositionEstimateInfo.iPosEstimateRefTime.iUtranGpsRefTimeResult.iSfn =	DMMTSY_PHONE_LCS_MC_GPS_UTRANREFTIME_SFN;
	
	//Measurement mode status is false so send report with iTddRefID
	iMeasurementReport.iMeasuredResults.iTUePosMeasuredResults.iUePosPositionEstimateInfo.iPosEstimateRefTime.iUtranGpsRefTimeResult.iGpsMeasModeStatus = EFalse;
	iMeasurementReport.iMeasuredResults.iTUePosMeasuredResults.iUePosPositionEstimateInfo.iPosEstimateRefTime.iUtranGpsRefTimeResult.iGpsMeasModeSpecificInfo.iTddRefID=DMMTSY_PHONE_LCS_MC_GPS_UTRANMODE_MODE_TDDID;

	//Position estimate status is EEllipsoidPointPresent, send report containing position estimate for Ellipsoid point
	iMeasurementReport.iMeasuredResults.iTUePosMeasuredResults.iUePosPositionEstimateInfo.iPosEstimateStatus = DMMTSY_PHONE_LCS_MC_GPS_POSESTINFO5;
	iMeasurementReport.iMeasuredResults.iTUePosMeasuredResults.iUePosPositionEstimateInfo.iPostionEstimate.iEllipsoidPoint.iLatitude = 	DMMTSY_PHONE_LCS_MC_GPS_POSEST_ELIPSPT_LAT;
	iMeasurementReport.iMeasuredResults.iTUePosMeasuredResults.iUePosPositionEstimateInfo.iPostionEstimate.iEllipsoidPoint.iLongitude = DMMTSY_PHONE_LCS_MC_GPS_POSEST_ELIPSPT_LON;
	iMeasurementReport.iMeasuredResults.iTUePosMeasuredResults.iUePosPositionEstimateInfo.iPostionEstimate.iEllipsoidPoint.iLatitudeSign = DMMTSY_PHONE_LCS_MC_GPS_POSEST_ELIPSOIDPT_LATSIGN1;
	
 	iMeasurementReport.iMeasuredResults.iTUePosMeasuredResults.iUePosGpsMeasurement.iGpsMeasurementParamList[0].iSatID = DMMTSY_PHONE_LCS_MC_GPS_PARAMLIST_SATID;
 	iMeasurementReport.iMeasuredResults.iTUePosMeasuredResults.iUePosGpsMeasurement.iGpsMeasurementParamList[0].iFractionalGpsChips = DMMTSY_PHONE_LCS_MC_GPS_PARAMLIST_GPSCHIPS; 
 	iMeasurementReport.iMeasuredResults.iTUePosMeasuredResults.iUePosGpsMeasurement.iGpsMeasurementParamList[0].iMultipathIndicator = DMMTSY_PHONE_LCS_MC_GPS_PARAMLIST_PATHINDICATOR1 ;
	
 	iMeasurementReport.iMeasuredResults.iTUePosMeasuredResults.iUePosGpsMeasurement.iGpsMeasurementParamList[0].iSatID = DMMTSY_PHONE_LCS_MC_GPS_PARAMLIST_SATID;
 	iMeasurementReport.iMeasuredResults.iTUePosMeasuredResults.iUePosGpsMeasurement.iGpsMeasurementParamList[0].iFractionalGpsChips = DMMTSY_PHONE_LCS_MC_GPS_PARAMLIST_GPSCHIPS; 
 	
    iLocationSvc.SendMeasurementReport(sendmcreport2reqStatus,measurementReportPckg);
    User::WaitForRequest(sendmcreport2reqStatus);
    TEST(sendmcreport2reqStatus==KErrNone);
    INFO_PRINTF2(_L("Test %d - RMobileLocationServices::SendMeasurementReport"), sendmcreport2reqStatus.Int());

	return TestStepResult();
	}

CTestMMSetEmgClientVoiceReqPreallocHeap::CTestMMSetEmgClientVoiceReqPreallocHeap()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestMMSetEmgClientVoiceReqPreallocHeap"));
	}
		
enum TVerdict CTestMMSetEmgClientVoiceReqPreallocHeap::doTestStepL()
//
// Ensure SetEmergencyClient API(With Voice request) preallocates heap.
//
	{
	INFO_PRINTF1(_L("Ensure SetEmergencyClient API(With Voice request) preallocates heap."));
#ifdef _DEBUG
	TInt setemergency;
	setemergency=iPhone.SetEmergencyClient(RPhone::EEmergencyCSVoiceCallRequest);
    TEST(setemergency==KErrNone);
    INFO_PRINTF2(_L("Test %d - iPhone.SetEmergencyClient(RPhone::EEmergencyCSVoiceCallRequest); "), setemergency);
 	OOMTest_DialEmergencyCall();
 	
#else
	INFO_PRINTF1(_L("Test disabled on release builds."));	
#endif // _DEBUG 	
    return TestStepResult();
	}

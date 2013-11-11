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
//

/**
 @file
*/

#include "TE_EtelMMTestStepBase.h"
#include "testdef.h"


/**
	Preamble for all CEtelMMLbsTestStepBase
*/
TVerdict CEtelMMLbsTestStepBase::doTestStepPreambleL()
	{
	//base class preamble
	CEtelMMTestStepBase::doTestStepPreambleL();
	TInt ret=iPhone.Open(iTelServer,DMMTSY_PHONE_NAME);
	TEST(ret==KErrNone);
	TInt lbsOpenFlag ;
	TBool existsLbsOpenFlag = GetIntFromConfig(ConfigSection(), _L("LbsOpenFlag"), lbsOpenFlag);
	if(!existsLbsOpenFlag)
		{
		ret=iLocationSvc.Open(iPhone);
		TEST(ret == KErrNone);	
		INFO_PRINTF2(_L("Test %d - RMobileLocationServices::Open request"), ret);
		}
	return TestStepResult() ;
	}

/**
	Postamble for all CEtelMMLbsTestStepBase
*/
TVerdict CEtelMMLbsTestStepBase::doTestStepPostambleL()
	{
	TInt lbsCloseFlag ;
	TBool existsLbsCloseFlag = GetIntFromConfig(ConfigSection(), _L("LbsCloseFlag"), lbsCloseFlag);
	if(!existsLbsCloseFlag)
		{
		iLocationSvc.Close();
		INFO_PRINTF1(_L("Test- RMobileLocationServices::Close request"));
		}
	iPhone.Close();	
	
	//base class postamble
	CEtelMMTestStepBase::doTestStepPostambleL();
 	return TestStepResult() ;
	}

void CEtelMMLbsTestStepBase::ValidateMCRefTimeParams()
{
	TInt expectedGpsWeek ;
	GetIntFromConfig(ConfigSection(), _L("expectedGpsWeek"), expectedGpsWeek );

	TInt expectedGpsTowOneMsce ;
	GetIntFromConfig(ConfigSection(), _L("expectedGpsTowOneMsce"), expectedGpsTowOneMsce );

	TBool expectedReftimeRequest;
	GetBoolFromConfig(ConfigSection(), _L("expectedReftimeRequest"), expectedReftimeRequest);

	TInt expectedRefTimeLsPart  ;
	GetIntFromConfig(ConfigSection(), _L("expectedRefTimeLsPart"), expectedRefTimeLsPart  );

	TInt expectedRefTimeMsPart  ;
	GetIntFromConfig(ConfigSection(), _L("expectedRefTimeMsPart"), expectedRefTimeMsPart  );

	TInt expectedRefTimeSfn  ;
	GetIntFromConfig(ConfigSection(), _L("expectedRefTimeSfn"), expectedRefTimeSfn  );

	TBool expectedAcqAsstRequest;
	GetBoolFromConfig(ConfigSection(), _L("expectedAcqAsstRequest"), expectedAcqAsstRequest);

	TBool expectedIntegrityRequest;
	GetBoolFromConfig(ConfigSection(), _L("expectedIntegrityRequest"), expectedIntegrityRequest);

	TInt expectedAcqAsstTime ;
	GetIntFromConfig(ConfigSection(), _L("expectedAcqAsstTime"), expectedAcqAsstTime );

	TInt expectedModePrimaryCode ;
	GetIntFromConfig(ConfigSection(), _L("expectedModePrimaryCode"), expectedModePrimaryCode );

	TInt expectedModeCellId ;
	GetIntFromConfig(ConfigSection(), _L("expectedModeCellId"), expectedModeCellId );

	TBool expectedModeStatusRequest;
	GetBoolFromConfig(ConfigSection(), _L("expectedModeStatusRequest"), expectedModeStatusRequest);

	//Reference Time Data populated and status is false
	TEST(iMeasurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iReferencTime.iGpsWeek == expectedGpsWeek);		
	TEST(iMeasurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iReferencTime.iGpsTowOneMsec == expectedGpsTowOneMsce );
	TEST(iMeasurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iGpsAddlDataStatus.iReferenceTimeRequest == expectedReftimeRequest);	
		
	//	Acquisition Assistance Data populated and status is false
	TEST(iMeasurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iReferencTime.iUtranGpsRefTime.iLsPart == expectedRefTimeLsPart);
	TEST(iMeasurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iReferencTime.iUtranGpsRefTime.iMsPart == expectedRefTimeMsPart);
	TEST(iMeasurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iReferencTime.iUtranGpsRefTime.iSfn == expectedRefTimeSfn);
	TEST(iMeasurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iGpsAddlDataStatus.iAcquisitionAssistanceReq == expectedAcqAsstRequest);
	
	TEST(iMeasurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iGpsAddlDataStatus.iRealTimeIntegrityRequest == expectedIntegrityRequest);
	TEST(iMeasurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iAcquisitionAssistance.iGpsReferenceTime == expectedAcqAsstTime	);
	TEST(iMeasurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iAcquisitionAssistance.iUtranGpsReferenceTime.iPrimaryScramblingCode == expectedModePrimaryCode);
	TEST(iMeasurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iAcquisitionAssistance.iUtranGpsReferenceTime.iCellParametersID == expectedModeCellId);
	TEST(iMeasurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iReferencTime.iUtranGpsRefTime.iModeSpecificInfoStatus == expectedModeStatusRequest);
	TEST(iMeasurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iReferencTime.iUtranGpsRefTime.iModeSpecificInfo.iPrimaryScramblingCode == expectedModeCellId);
}


void CEtelMMLbsTestStepBase::ValidateMCRefLocParams()
	{
	TInt expectedRefLocLatSign ;
	GetIntFromConfig(ConfigSection(), _L("expectedRefLocLatSign"), expectedRefLocLatSign );

	TInt expectedRefLocLongSign ;
	GetIntFromConfig(ConfigSection(), _L("expectedRefLocLongSign"), expectedRefLocLongSign );

	TBool expectedRefLocRequest;
	GetBoolFromConfig(ConfigSection(), _L("expectedRefLocRequest"), expectedRefLocRequest);
	
	// Reference Location data populated and status is false
	TEST(iMeasurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iReferenceLocation.iLatitudeSign == DMMTSY_PHONE_LCS_MC_GPS_REFLOC_LATSIGN1);
	TEST(iMeasurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iReferenceLocation.iLatitude == expectedRefLocLatSign);
	TEST(iMeasurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iReferenceLocation.iLongitude == expectedRefLocLongSign) ;
	TEST(iMeasurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iGpsAddlDataStatus.iReferenceLocationRequest == expectedRefLocRequest);
	}

	
void CEtelMMLbsTestStepBase::ValidateMCModelParams()
	{
	TInt expectedNavEphemeris ;
	GetIntFromConfig(ConfigSection(), _L("expectedNavEphemeris"), expectedNavEphemeris );

	TInt expectedNavSatId ;
	GetIntFromConfig(ConfigSection(), _L("expectedNavSatId"), expectedNavSatId );

	TBool expectedNavModelRequest;
	GetBoolFromConfig(ConfigSection(), _L("expectedNavModelRequest"), expectedNavModelRequest);

	//	NavigationModel data populated and status is false	
	TEST(iMeasurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iNavigationModel.iEphemerisParameter.iCodeOnL2 == expectedNavEphemeris);
	TEST(iMeasurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iNavigationModel.iSatID == expectedNavSatId);
	TEST(iMeasurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iNavigationModel.iSatelliteStatus == DMMTSY_PHONE_LCS_MC_GPS_NAVIGATION_SATSTATUS1);
	TEST(iMeasurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iGpsAddlDataStatus.iNavigationModelRequest == expectedNavModelRequest);

	TInt expectedIonModel ;
	GetIntFromConfig(ConfigSection(), _L("expectedIonModel"), expectedIonModel );

	TBool expectedIonModelRequest;
	GetBoolFromConfig(ConfigSection(), _L("expectedIonModelRequest"), expectedIonModelRequest);
	
	//	Ionospheric data populated and status is false	
	TEST(iMeasurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iIonosphericModel.iAlfa1 == expectedIonModel);
	TEST(iMeasurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iGpsAddlDataStatus.iIonosphericModelRequest == expectedIonModelRequest);	

	TInt expectedUtcModel1 ;
	GetIntFromConfig(ConfigSection(), _L("expectedUtcModel1"), expectedUtcModel1 );

	TBool expectedUtcModelRequest;
	GetBoolFromConfig(ConfigSection(), _L("expectedUtcModelRequest"), expectedUtcModelRequest);

	//UTC data populated and status is false		
	TEST(iMeasurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iUtcModel.iA1 == expectedUtcModel1);
	TEST(iMeasurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iGpsAddlDataStatus.iUtcModelRequest == expectedUtcModelRequest);
	}


void CEtelMMLbsTestStepBase::ValidateMCParams()
	{

	TBool isVelocityRequested;
	GetBoolFromConfig(ConfigSection(), _L("velocityRequested"), isVelocityRequested);

	TInt expectedGpsTow ;
	GetIntFromConfig(ConfigSection(), _L("expectedGpsTow"), expectedGpsTow );

	TBool expectedCorrectionRequest;
	GetBoolFromConfig(ConfigSection(), _L("expectedCorrectionRequest"), expectedCorrectionRequest);

	TInt expectedGpsAlmanac ;
	GetIntFromConfig(ConfigSection(), _L("expectedGpsAlmanac"), expectedGpsAlmanac );

	TBool expectedGpsAlmanacRequest;
	GetBoolFromConfig(ConfigSection(), _L("expectedGpsAlmanacRequest"), expectedGpsAlmanacRequest);

	TInt expectedBadSatList ;
	GetIntFromConfig(ConfigSection(), _L("expectedBadSatList"), expectedBadSatList );

	TInt expectedHorAccuracy ;
	GetIntFromConfig(ConfigSection(), _L("expectedHorAccuracy"), expectedHorAccuracy );

	TInt expectedVertAccuracy ;
	GetIntFromConfig(ConfigSection(), _L("expectedVertAccuracy"), expectedVertAccuracy );

	TBool expectedAddlAsstDataRequest;
	GetBoolFromConfig(ConfigSection(), _L("expectedAddlAsstDataRequest"), expectedAddlAsstDataRequest);

	TEST(iMeasurementControl.iVelocityRequested == isVelocityRequested);
	TEST(iMeasurementControl.iMeasReportTransferMode == DMMTSY_PHONE_LCS_MC_RPTTRANSFERMODE);

	//DGPS corrections data populated and status is false		
	TEST(iMeasurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iDgpsCorrections.iGpsTow == expectedGpsTow);	
	TEST(iMeasurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iGpsAddlDataStatus.iDgpsCorrectionsRequest == expectedCorrectionRequest);	
			
	//Almanac data populated and status is false
	TEST(iMeasurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iAlmanac.iWnA == expectedGpsAlmanac);
	TEST(iMeasurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iGpsAddlDataStatus.iAlmanacRequest	== expectedGpsAlmanacRequest);
	
	//RealTime integrity data populated and status is false
	TEST(iMeasurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iBadSatList[0] == expectedBadSatList);
	TEST(iMeasurementControl.iMeasurementCommand.iSetup.iUePosReportingQuantity.iHorzAccuracy == expectedHorAccuracy);
	TEST(iMeasurementControl.iMeasurementCommand.iSetup.iUePosReportingQuantity.iVertAccuracy == expectedVertAccuracy);
	//Additional Assistance Data is not required
	TEST(iMeasurementControl.iMeasurementCommand.iSetup.iUePosReportingQuantity.iAddlAssistanceDataReq == expectedAddlAsstDataRequest);
	}

void CEtelMMLbsTestStepBase::SendCompleteMeasurementReport()
{
	TBool errorStatus;
	GetBoolFromConfig(ConfigSection(), _L("ErrorStatus"), errorStatus);

	TInt positionEstLatSign;
	GetIntFromConfig(ConfigSection(), _L("PositionEstLatSign"), positionEstLatSign);

	TInt velocityEstStatus;
	GetIntFromConfig(ConfigSection(), _L("VelocityEstStatus"), velocityEstStatus);

	TInt velocityEstHorzSpeed;
	GetIntFromConfig(ConfigSection(), _L("VelocityEstHorzSpeed"), velocityEstHorzSpeed);

	TInt velocityEstHorzBearing ;
	GetIntFromConfig(ConfigSection(), _L("VelocityEstHorzBearing "), velocityEstHorzBearing );

	TInt positionEstInfo1;
	GetIntFromConfig(ConfigSection(), _L("PositionEstInfo1"), positionEstInfo1);

	TInt refTimeLsPart;
	GetIntFromConfig(ConfigSection(), _L("RefTimeLsPart"), refTimeLsPart);

	TInt refTimeMsPart;
	GetIntFromConfig(ConfigSection(), _L("RefTimeMsPart"), refTimeMsPart);

	TInt refTimeSfn;
	GetIntFromConfig(ConfigSection(), _L("RefTimeSfn"), refTimeSfn);

	TBool modeStatusRequest;
	GetBoolFromConfig(ConfigSection(), _L("ModeStatusRequest"), modeStatusRequest);

	TInt modeTDDId;
	GetIntFromConfig(ConfigSection(), _L("ModeTDDId"), modeTDDId);

	TInt positionEstInfo5;
	GetIntFromConfig(ConfigSection(), _L("PositionEstInfo5"), positionEstInfo5);

	TInt positionEstElipseLat;
	GetIntFromConfig(ConfigSection(), _L("PositionEstElipseLat"), positionEstElipseLat);

	TInt positionEstElipseLong;
	GetIntFromConfig(ConfigSection(), _L("PositionEstElipseLong"), positionEstElipseLong);

	TInt paramListSatId;
	GetIntFromConfig(ConfigSection(), _L("ParamListSatId"), paramListSatId);

	TInt paramListGpsChips;
	GetIntFromConfig(ConfigSection(), _L("ParamListGpsChips"), paramListGpsChips);

	TInt paramListPathInd;
	GetIntFromConfig(ConfigSection(), _L("ParamListPathInd"), paramListPathInd);

	if(positionEstInfo1 == 1)
		iMeasurementReport.iMeasuredResults.iTUePosMeasuredResults.iUePosPositionEstimateInfo.iPosEstimateStatus = DMMTSY_PHONE_LCS_MC_GPS_POSESTINFO1;
	if(positionEstInfo5 == 5)
		iMeasurementReport.iMeasuredResults.iTUePosMeasuredResults.iUePosPositionEstimateInfo.iPosEstimateStatus = DMMTSY_PHONE_LCS_MC_GPS_POSESTINFO5;
	if(positionEstLatSign == 1)
		iMeasurementReport.iMeasuredResults.iTUePosMeasuredResults.iUePosPositionEstimateInfo.iPostionEstimate.iEllipsoidPoint.iLatitudeSign = DMMTSY_PHONE_LCS_MC_GPS_POSEST_ELIPSOIDPT_LATSIGN1;
    iMeasurementReport.iMeasuredResults.iTUePosMeasuredResults.iPosErrorStatus = errorStatus;
    
	//velocity estimate status is EHorzVelPresent, send report with Horiontal velocity
	if(velocityEstStatus == 1)
    	iMeasurementReport.iVelEstimateStatus = DMMTSY_PHONE_LCS_MC_GPS_VELOCITYEST_STATUS1;
    iMeasurementReport.iVelocityEstimate.iHorzVelocity.iHorzSpeed = velocityEstHorzSpeed;
    iMeasurementReport.iVelocityEstimate.iHorzVelocity.iBearing = velocityEstHorzBearing;
	
	iMeasurementReport.iMeasuredResults.iTUePosMeasuredResults.iUePosPositionEstimateInfo.iPosEstimateRefTime.iUtranGpsRefTimeResult.iLsPart = refTimeLsPart;
	iMeasurementReport.iMeasuredResults.iTUePosMeasuredResults.iUePosPositionEstimateInfo.iPosEstimateRefTime.iUtranGpsRefTimeResult.iMsPart = refTimeMsPart;	
	iMeasurementReport.iMeasuredResults.iTUePosMeasuredResults.iUePosPositionEstimateInfo.iPosEstimateRefTime.iUtranGpsRefTimeResult.iSfn =	refTimeSfn;
	
	//Measurement mode status is false so send report with iTddRefID
	iMeasurementReport.iMeasuredResults.iTUePosMeasuredResults.iUePosPositionEstimateInfo.iPosEstimateRefTime.iUtranGpsRefTimeResult.iGpsMeasModeStatus = modeStatusRequest;
	iMeasurementReport.iMeasuredResults.iTUePosMeasuredResults.iUePosPositionEstimateInfo.iPosEstimateRefTime.iUtranGpsRefTimeResult.iGpsMeasModeSpecificInfo.iTddRefID=modeTDDId;

	//Position estimate status is EEllipsoidPointPresent, send report containing position estimate for Ellipsoid point
	iMeasurementReport.iMeasuredResults.iTUePosMeasuredResults.iUePosPositionEstimateInfo.iPostionEstimate.iEllipsoidPoint.iLatitude = 	positionEstElipseLat;
	iMeasurementReport.iMeasuredResults.iTUePosMeasuredResults.iUePosPositionEstimateInfo.iPostionEstimate.iEllipsoidPoint.iLongitude = positionEstElipseLong;
	
 	iMeasurementReport.iMeasuredResults.iTUePosMeasuredResults.iUePosGpsMeasurement.iGpsMeasurementParamList[0].iSatID = paramListSatId;
 	iMeasurementReport.iMeasuredResults.iTUePosMeasuredResults.iUePosGpsMeasurement.iGpsMeasurementParamList[0].iFractionalGpsChips = paramListGpsChips; 
 	if(paramListPathInd ==1)
 		iMeasurementReport.iMeasuredResults.iTUePosMeasuredResults.iUePosGpsMeasurement.iGpsMeasurementParamList[0].iMultipathIndicator = DMMTSY_PHONE_LCS_MC_GPS_PARAMLIST_PATHINDICATOR1 ;
}

void CEtelMMLbsTestStepBase::SetMolrParams()
{
	TInt domain;
	GetIntFromConfig(ConfigSection(), _L("Domain"), domain);
	
	TInt molrType;
	GetIntFromConfig(ConfigSection(), _L("Molrtype"), molrType);

	TInt locationestimationtype;
	GetIntFromConfig(ConfigSection(), _L("locationestimationtype"), locationestimationtype);

	TInt locationMethod;
	GetIntFromConfig(ConfigSection(), _L("LocationMethods"), locationMethod);

	TInt horizontalAccuracy;
	GetIntFromConfig(ConfigSection(), _L("HorizontalAccuracy"), horizontalAccuracy);

	TInt verticalAccuracy;
	GetIntFromConfig(ConfigSection(), _L("VerticalAccuracy"), verticalAccuracy);

	TInt responseTime;
	GetIntFromConfig(ConfigSection(), _L("ResponseTime"), responseTime);

	TPtrC gpsAssistanceData;
	GetStringFromConfig(ConfigSection(), _L("GPSAssistanceData"), gpsAssistanceData );

	TPtrC expectedLocationEstimate ;
	TBool exists1 = GetStringFromConfig(ConfigSection(), _L("expectedLocationEstimate"), expectedLocationEstimate );

	TPtrC expectedVelocityEstimate;
	GetStringFromConfig(ConfigSection(), _L("expectedVelocityEstimate"), expectedVelocityEstimate );

	TPtrC expectedMolrError;
	GetStringFromConfig(ConfigSection(), _L("expectedMolrError"), expectedMolrError );

	TPtrC invalidClientIdFlag;
	TBool existsInvalidClientIdFlag = GetStringFromConfig(ConfigSection(), _L("InvalidClientIdFlag"), invalidClientIdFlag);
	TInt ret;	
	//set domain to Packet Switched
	if (domain == 1)
		{
  		ret = iLocationSvc.SetLcsDomain(DMMTSY_SETLCS_DOMAIN1);		
		}
	else
		{
  		ret = iLocationSvc.SetLcsDomain(DMMTSY_SETLCS_DOMAIN2);		
		}
	
  	TEST(ret == KErrNone);
 

	if(molrType == 1)
		iStartMolr.iMolrType=DMMTSY_MOLR_TYPE1;
	if(locationestimationtype == 1)
		iStartMolr.iMolrLocationEstimateType = DMMTSY_MOLR_LOCATION_ESTIMATION_TYPE1;
	if(locationMethod == 3)
		iStartMolr.iLocationMethod=DMMTSY_MOLR_LOCATION_METHOD3;
	if(responseTime == 1)	
		iStartMolr.iQos.iResponseTime = DMMTSY_MOLR_RESPONSE_TIME1;
	
	iStartMolr.iQos.iHorizontalAccuracy = horizontalAccuracy;
	iStartMolr.iQos.iVerticalAccuracy = verticalAccuracy;
	iStartMolr.iGpsAssistanceData.Copy(gpsAssistanceData);
	if(existsInvalidClientIdFlag)
			iStartMolr.iLcsClientExternalID=DMMTSY_INVALID_EXTERNAL_CLIENT_ID;	
}


 void CEtelMMLbsTestStepBase::OOMTest_LBS()
 {
 	const TInt KMaxTests = 1000;	
 	TBool oomAvailable = EFalse; 
	TInt failure=1;
 	TRequestStatus mtlrStatus;
	TRequestStatus reqStatus;
    RMobileLocationServices::TNotifyMtlrV7   iNotifyMtlr;
	iNotifyMtlr.iLocationInfo.iLcsClientName.iLcsDataCodingScheme = DMMTSY_DATA_CODING_SCHEME1;
	RMobileLocationServices::TNotifyMtlrV7Pckg  notifyMtlrPckg(iNotifyMtlr);
	while (!oomAvailable  && failure < KMaxTests)
		{
	  	iTelServer.__DbgFailNext(failure);
      	iLocationSvc.NotifyMtlr(mtlrStatus,notifyMtlrPckg);
   		User::WaitForRequest(mtlrStatus);
   		INFO_PRINTF2(_L("Notify MTLR %d "),mtlrStatus.Int()); 
		TEST(mtlrStatus==KErrNone);
		if(mtlrStatus!=KErrNone)	
			{
			INFO_PRINTF2(_L("Failing after %d Allocs"),failure); 
			} 
		else 
			{ 
			INFO_PRINTF2(_L("Passing after %d Allocs"),failure); 
			} 
		iTelServer.__DbgFailNextAvailable(oomAvailable); 
		failure++; 
		}
		
	// KMaxTests is used to stop the above while statement looping
	// indefinitely. If KMaxTests is reached then we can guess that something
	// has gone wrong and the test should be failed.
	TESTCHECKCONDITION(failure < KMaxTests);

 }
 
void CEtelMMLbsTestStepBase::OOMTest_DialEmergencyCall()
 {
 	const TInt KMaxTests = 1000;	
 	RMobilePhone mmPhone;
	TInt ret=mmPhone.Open(iTelServer,DMMTSY_PHONE_NAME);
	TEST(ret==KErrNone);
	RMobileLine mmLine;
	ret=mmLine.Open(mmPhone,DMMTSY_LINE_VOICE_NAME);
    TEST(ret==KErrNone);
	RMobileCall mmCall;
	TName callName;
	ret=mmCall.OpenNewCall(mmLine,callName);
    TEST(ret==KErrNone);
	TRequestStatus reqStatus;
	TBuf<RMobileENStore::KEmergencyNumberSize> aNumber(DMMTSY_EN_STORE_AMBULANCE_NUM);

	TBool oomAvailable = EFalse; 
	TInt failure=1;
	while (!oomAvailable  && failure < KMaxTests)
		{
	  	iTelServer.__DbgFailNext(failure);
     	mmCall.DialEmergencyCall(reqStatus, aNumber);
   		User::WaitForRequest(reqStatus);
		INFO_PRINTF2(_L("mmCall.DialEmergencyCall(reqStatus, aNumber);%d "),reqStatus.Int());
		TEST(reqStatus==KErrNone); 
		if(reqStatus!=KErrNone)	
			{
			INFO_PRINTF2(_L("Failing after %d Allocs"),failure); 
			} 
		else 
			{ 
			INFO_PRINTF2(_L("Passing after %d Allocs"),failure); 
			} 

		iTelServer.__DbgFailNextAvailable(oomAvailable); 
		failure++; 
		}
		
	// KMaxTests is used to stop the above while statement looping
	// indefinitely. If KMaxTests is reached then we can guess that something
	// has gone wrong and the test should be failed.
	TESTCHECKCONDITION(failure < KMaxTests);
	
	mmCall.Close();
	mmLine.Close();
	mmPhone.Close();
 }

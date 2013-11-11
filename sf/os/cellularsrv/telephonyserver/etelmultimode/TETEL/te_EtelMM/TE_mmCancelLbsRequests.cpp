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

#include <e32test.h>

#include "TE_EtelMMTestStepBase.h"
#include "TE_mmCancelLbsRequests.h"
#include "testdef.h"


/**
 *  Cancel all Location Services requests
 */
enum TVerdict CTestCancelLbsRequests::doTestStepL()
	{
 	INFO_PRINTF1(_L("Cancel a MT-LR request"));
   	TRequestStatus mtlrStatus;
	TRequestStatus mcontrolStatus;
	TRequestStatus reqStatus;
	RMobileLocationServices::TNotifyMtlrV7Pckg  notifyMtlrPckg(iNotifyMtlr);
	RMobileLocationServices::TMeasurementControlV7Pckg  measurementControlPckg(iMeasurementControl);	
	RMobileLocationServices::TMeasurementReportV7Pckg  measurementReportPckg(iMeasurementReport);
	RMobileLocationServices::TStartMolrV7Pckg  startMolrPckg(iStartMolr);
	RMobileLocationServices::TMolrReturnResultV7Pckg  molrReturnResultPckg(iMolrReturnResult);

    // Notify Mtlr Cancel 
   	TInt measurementId;
	GetIntFromConfig(ConfigSection(), _L("MeasurementId"), measurementId);

   	TInt dataCodingScheme ;
	GetIntFromConfig(ConfigSection(), _L("DataCodingScheme"), dataCodingScheme);

	TBool errorStatus;
	GetBoolFromConfig(ConfigSection(), _L("ErrorStatus"), errorStatus);

  	iMeasurementControl.iMeasurementIdentity=measurementId;
	iNotifyMtlr.iLocationInfo.iLcsClientName.iLcsDataCodingScheme = dataCodingScheme;     
    iLocationSvc.NotifyMtlr(mtlrStatus,notifyMtlrPckg);
	iLocationSvc.NotifyMeasurementControl(mcontrolStatus,measurementControlPckg);    
	iLocationSvc.CancelAsyncRequest(EMobileLocationServicesNotifyMtlr);	
    User::WaitForRequest(mtlrStatus);
	TEST(mtlrStatus == KErrCancel);
	INFO_PRINTF2(_L("Test %d - Cancel NotifyMTLRl passed"), iTestCount++);
  
	// Measurement Control Cancel
	iLocationSvc.CancelAsyncRequest(EMobileLocationServicesNotifyMeasurementControl);
    User::WaitForRequest(mcontrolStatus);
    TEST(mcontrolStatus==KErrCancel);
 	INFO_PRINTF2(_L("Test %d - Cancel NotifyMeasurementControl passed"), iTestCount++);
 	 	
  	// Cancel SendMtlrResponse 
  	iMtlrResponse=DMMTSY_MTLR_RESPONSE2;
    iLocationSvc.SendMtlrResponse(reqStatus,iMtlrResponse);
   	iLocationSvc.CancelAsyncRequest(EMobileLocationServicesSendMtlrResponse);
    User::WaitForRequest(reqStatus);
    TEST(reqStatus == KErrCancel);    
	INFO_PRINTF2(_L("Test %d - Cancel SendMtlrResponse passed"), iTestCount++);
	
	// Cancel SendMeasurementReport request
	iMeasurementReport.iMeasuredResults.iTUePosMeasuredResults.iPosErrorStatus = errorStatus;
    iMeasurementReport.iMeasurementIdentity=measurementId;
    iLocationSvc.SendMeasurementReport(reqStatus,measurementReportPckg);
   	iLocationSvc.CancelAsyncRequest(EMobileLocationServicesSendMeasurementReport);
    User::WaitForRequest(reqStatus);
    TEST(reqStatus == KErrCancel);    
	INFO_PRINTF2(_L("Test %d - Cancel SendMeasurementReport passed"), iTestCount++);

	// cancel MOLR request
	TInt molrType;
	GetIntFromConfig(ConfigSection(), _L("Molrtype"), molrType);
	if(molrType == 1)
		iStartMolr.iMolrType=DMMTSY_MOLR_TYPE1;
	iLocationSvc.SendMolr(reqStatus,startMolrPckg,molrReturnResultPckg);
	iLocationSvc.CancelAsyncRequest(EMobileLocationServicesSendMolr);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus == KErrCancel);
	INFO_PRINTF2(_L("Test %d - RMobileLocationServices::SendMolr Cancel request passed"), iTestCount++);	   	
		
	
    // cancel a NotifyResetUePositioningInformation request.
    RMobileLocationServices::TUePosTechnology uePositionTechnology;
    iLocationSvc.NotifyResetUePositioningInformation(reqStatus, uePositionTechnology);
    iLocationSvc.CancelAsyncRequest(EMobileLocationServicesNotifyResetUePositioningInformation);
    User::WaitForRequest(reqStatus);
    TEST(reqStatus == KErrCancel);
	INFO_PRINTF2(_L("Test %d - RMobileLocationServices::NotifyResetUePositioningInformation Cancel request passed"), iTestCount++);	   	

	return TestStepResult();
	}


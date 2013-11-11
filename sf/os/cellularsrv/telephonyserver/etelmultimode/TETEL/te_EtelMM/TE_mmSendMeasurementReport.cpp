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
#include "TE_mmSendMeasurementReport.h"
#include "testdef.h"

enum TVerdict CTestSendMeasurementReport::doTestStepL()
	{
	INFO_PRINTF1(_L("Sending Measurement Report..."));

	TRequestStatus reqStatus;

   	RMobileLocationServices::TMeasurementReportV7Pckg  measurementReportPckg(iMeasurementReport);
	
	TInt measurementId;
	GetIntFromConfig(ConfigSection(), _L("MeasurementId"), measurementId);

	TBool errorStatus;
	GetBoolFromConfig(ConfigSection(), _L("ErrorStatus"), errorStatus);
	
	TInt incompleteReportFlag;
	TBool existsIncompleteReportFlag = GetIntFromConfig(ConfigSection(), _L("IncompleteReportFlag"), incompleteReportFlag);

	TInt measurementReportFailureFlag;
	TBool existsMRFailureFlag = GetIntFromConfig(ConfigSection(), _L("MeasurementReportFailureFlag"), measurementReportFailureFlag);

	TInt measurementReportFailureCause;
	GetIntFromConfig(ConfigSection(), _L("MeasurementReportFailureCause"), measurementReportFailureCause);

	TInt userDeniedFlag;
	TBool existsUserDeniedFlag = GetIntFromConfig(ConfigSection(), _L("UserDeniedFlag"), userDeniedFlag);

	TInt errorCause;
	GetIntFromConfig(ConfigSection(), _L("ErrorCause"), errorCause);

    iMeasurementReport.iMeasurementIdentity=measurementId;
    
	if (existsMRFailureFlag)
		{
		// measurementcontrol failure
		if(measurementReportFailureCause  == 2)
	   		iMeasurementControlFailure = DMMTSY_PHONE_LCS_MCTRLFAILURE2;
		iLocationSvc.SendMeasurementReport(reqStatus,iMeasurementControlFailure);
		User::WaitForRequest(reqStatus);
		TEST(reqStatus==KErrNone);		
		}
	else
		{
		//incomplete measurement report
		if(existsIncompleteReportFlag)
			{
			TBool refLocRequest;
			GetBoolFromConfig(ConfigSection(), _L("RefLocRequest"), refLocRequest);

			TBool additionalDataRequest;
			GetBoolFromConfig(ConfigSection(), _L("AdditionalDataRequest"), additionalDataRequest);
	   		iMeasurementReport.iMeasuredResults.iTUePosMeasuredResults.iPosErrorStatus = errorStatus;
		    if(errorCause == 2)
			    iMeasurementReport.iMeasuredResults.iTUePosMeasuredResults.iUePosError.iPosErrorCause = DMMTSY_PHONE_LCS_MC_GPS_ERRORCAUSE2;
			iMeasurementReport.iMeasuredResults.iTUePosMeasuredResults.iUePosError.iGpsAddlAssistDataReq.iReferenceLocationRequest = refLocRequest;
			iMeasurementReport.iMeasuredResults.iTUePosMeasuredResults.iUePosError.iGpsAddlAssistDataReq.iUtcModelRequest = additionalDataRequest;	
			}
		else if(existsUserDeniedFlag)
			{
			// error in measurement report
		    if(errorCause == 5)
			    iMeasurementReport.iMeasuredResults.iTUePosMeasuredResults.iUePosError.iPosErrorCause = DMMTSY_PHONE_LCS_MC_GPS_ERRORCAUSE5;
   	   		iMeasurementReport.iMeasuredResults.iTUePosMeasuredResults.iPosErrorStatus = errorStatus;
		    			
			}
		else	
			//complete measurement report
			{
		    SendCompleteMeasurementReport();
			}

			iLocationSvc.SendMeasurementReport(reqStatus,measurementReportPckg);
		    User::WaitForRequest(reqStatus);
		    TEST(reqStatus==KErrNone);
		}

		INFO_PRINTF2(_L("Test %d - RMobileLocationServices::SendMeasurementReport passed"), iTestCount++);		
	return TestStepResult();
	}
	

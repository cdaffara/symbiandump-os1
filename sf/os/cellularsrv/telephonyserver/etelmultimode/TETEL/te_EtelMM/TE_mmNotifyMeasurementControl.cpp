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
// TE_MMLOCATIONSERVICES.CPP
// 
//

#include <e32test.h>

#include "TE_EtelMMTestStepBase.h"
#include "TE_mmNotifyMeasurementControl.h"
#include "testdef.h"
#include "mmtsy.h"

enum TVerdict CTestNotifyMeasurementControl::doTestStepL()
	{
	INFO_PRINTF1(_L("Measurement Control request"));

	TRequestStatus mcontrolStatus;

	RMobileLocationServices::TMeasurementControlV7Pckg  measurementControlPckg(iMeasurementControl);	

	TInt incompleteMCRequestFlag;
	TBool existsIncompleteMCFlag = GetIntFromConfig(ConfigSection(), _L("IncompleteMCRequestFlag"), incompleteMCRequestFlag );

	TInt measurementId;
	GetIntFromConfig(ConfigSection(), _L("MeasurementId"), measurementId);

	TInt invalidMeasurementIdFlag ;
	TBool existsInvalidMeasurementIdFlag = GetIntFromConfig(ConfigSection(), _L("InvalidMeasurementIdFlag"), invalidMeasurementIdFlag );

	iMeasurementControl.iMeasurementIdentity = measurementId;

	iLocationSvc.NotifyMeasurementControl(mcontrolStatus,measurementControlPckg);
    User::WaitForRequest(mcontrolStatus);
    TEST(mcontrolStatus==KErrNone);
  
  	if(existsInvalidMeasurementIdFlag)  
	  	{
	  	//invalid data in measurementControl
		TEST(iMeasurementControl.iMeasurementIdentity == measurementId);  		
  		}
	else if(existsIncompleteMCFlag)
		{
		// Incomplete MeasurementControl Request
		TBool expectedAddlAsstDataRequest;
		GetBoolFromConfig(ConfigSection(), _L("expectedAddlAsstDataRequest"), expectedAddlAsstDataRequest);

		TEST(iMeasurementControl.iMeasurementCommand.iSetup.iUePosReportingQuantity.iAddlAssistanceDataReq == expectedAddlAsstDataRequest);
		}
	else 
		{
		//complete measurement control request
		TEST(iMeasurementControl.iMeasurementIdentity == measurementId);
		INFO_PRINTF2(_L("Measurement Identity=%d"),iMeasurementControl.iMeasurementIdentity);
		ValidateMCParams();
		ValidateMCRefTimeParams();
		ValidateMCRefLocParams();
		ValidateMCModelParams();
		}

	INFO_PRINTF2(_L("Test %d - RMobileLocationServices::NotifyMeasurementControl passed"), iTestCount++);		
	return TestStepResult();
	}

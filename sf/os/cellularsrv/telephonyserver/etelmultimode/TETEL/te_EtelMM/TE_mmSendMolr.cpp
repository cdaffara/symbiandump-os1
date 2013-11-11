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
#include "TE_mmSendMolr.h"
#include "testdef.h"

/**
 * Client makes its own location request to the network and it waits for the notification of incoming A-GPS assistance data from the TSY. 
 * Location information is computed using the A-GPS assistance data received from the network 
 * and sends the location information report to the network/TSY
 * On completion of the MOLR request incoming location estimate and velocity estimate is sent to the client
 */

enum TVerdict CTestSendMolr::doTestStepL()
	{

	INFO_PRINTF1(_L("Successful MO-LR"));

	TRequestStatus molrStatus;
	TRequestStatus mcontrolStatus;
	TRequestStatus reqStatus;
	
	RMobileLocationServices::TStartMolrV7Pckg  startMolrPckg(iStartMolr);
	RMobileLocationServices::TMolrReturnResultV7Pckg  molrReturnResultPckg(iMolrReturnResult);

	TInt domain;
	GetIntFromConfig(ConfigSection(), _L("Domain"), domain);

	TPtrC invalidClientIdFlag;
	TBool existsInvalidClientIdFlag = GetStringFromConfig(ConfigSection(), _L("InvalidClientIdFlag"), invalidClientIdFlag);
	// Set Molr parameters before MOLR request
	SetMolrParams();
	iLocationSvc.SendMolr(molrStatus,startMolrPckg,molrReturnResultPckg);
 	User::WaitForRequest(molrStatus);
	TEST(molrStatus == KErrNone);

	if(domain == 2)
		TEST(iStartMolr.iLcsClientExternalID == DMMTSY_EXTERNAL_CLIENT_ID);
	if(existsInvalidClientIdFlag)
		{
		TEST(iStartMolr.iMolrType==DMMTSY_MOLR_TYPE1 && iMolrReturnResult.iMolrErrors==DMMTSY_MOLR_ERROR_TYPE2 );
		}
	else
		{
		TEST(iMolrReturnResult.iLocationEstimate == DMMTSY_MOLR_LOCATION_ESTIMATE);
		TEST(iMolrReturnResult.iVelocityEstimate == DMMTSY_MOLR_VELOCITY_ESTIMATE);
		}
	
	INFO_PRINTF2(_L("Test %d - RMobileLocationServices::SendMolr request passed"), iTestCount++);
	
	return TestStepResult();
	}


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
// TE_MMLNOTIFYMTLR.CPP
// 
//

#include <e32test.h>

#include "TE_EtelMMTestStepBase.h"
#include "TE_mmNotifyMtlr.h"
#include "testdef.h"

enum TVerdict CTestNotifyMtlr::doTestStepL()
	{
	INFO_PRINTF1(_L("Successful MT-LR request"));

 	TRequestStatus mtlrStatus;
	TRequestStatus mcontrolStatus;
	TRequestStatus reqStatus;
    TInt notificationTypeReceived = 0;
    
	TInt expectedNotificationType;
	GetIntFromConfig(ConfigSection(), _L("expectedNotificationType"), expectedNotificationType);

	TInt dataCodingScheme ;
	GetIntFromConfig(ConfigSection(), _L("DataCodingScheme"), dataCodingScheme);

	TInt invalidCliendIdFlag ;
	TBool existsInvalidCliendIdFlag = GetIntFromConfig(ConfigSection(), _L("InvalidCliendIdFlag"), invalidCliendIdFlag);

	iNotifyMtlr.iLocationInfo.iLcsClientName.iLcsDataCodingScheme = dataCodingScheme;
	
    RMobileLocationServices::TNotifyMtlrV7Pckg  notifyMtlrPckg(iNotifyMtlr);
 
     //  Notify Mtlr request
    iLocationSvc.NotifyMtlr(mtlrStatus,notifyMtlrPckg);
    User::WaitForRequest(mtlrStatus);
    TEST(mtlrStatus == KErrNone);
     
	//notificationtype ENotifyLocationAllowed
	if(iNotifyMtlr.iNotificationType == DMMTSY_NOTIFICATION_TYPE1 )
		{
			notificationTypeReceived = 1; 
		}
	// notificationtype ENotifyAndVerify_LocationAllowedIfNoResponse		
	if(iNotifyMtlr.iNotificationType == DMMTSY_NOTIFICATION_TYPE2 )
		{
			notificationTypeReceived = 2; 
		}
	// notificationtype ENotifyAndVerify_LocationNotAllowedIfNoResponse				
	if(iNotifyMtlr.iNotificationType == DMMTSY_NOTIFICATION_TYPE3 )
		{
			notificationTypeReceived = 3; 
		}
	// notificationtype ELocationPrivacyOverride				
	if(iNotifyMtlr.iNotificationType == DMMTSY_NOTIFICATION_TYPE4 )
		{
			notificationTypeReceived = 4; 
		}
	// notificationtype ELocationNotAllowed				
	if(iNotifyMtlr.iNotificationType == DMMTSY_NOTIFICATION_TYPE5 )
		{
			notificationTypeReceived = 5; 
		}

	if(existsInvalidCliendIdFlag)
		{
		TEST(iNotifyMtlr.iLocationInfo.iLcsClientID == DMMTSY_INVALID_EXTERNAL_CLIENT_ID);
		}
	else
		{
	    TEST(expectedNotificationType == notificationTypeReceived);
		}
		
	INFO_PRINTF2(_L("Test %d - RMobileLocationServices::NotifyMtlr passed"), iTestCount++);

	return TestStepResult();
	}

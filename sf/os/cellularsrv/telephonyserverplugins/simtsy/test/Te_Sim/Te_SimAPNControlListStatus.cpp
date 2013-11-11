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

#include "Te_SimAPNControlListStatus.h"

//APN Control List
#define SIMTSY_O2_APN_ENUMERATE								0
#define SIMTSY_ORANGE_APN_ENUMERATE							1
#define SIMTSY_VODAPHONE_APN_ENUMERATE						2

#define SIMTSY_CONFIG_ENTRIES								2

#define SIMTSY_O2_APN_ENTRY_TO_APPEND						_L8("wap.o2.co.uk")
#define SIMTSY_ORANGE_APN_ENTRY_TO_APPEND					_L8("orangewap")
#define SIMTSY_VODAPHONE_APN_ENTRY_TO_APPEND				_L8("wap.vodafone.co.uk")

CSimAPNControlListStatus::CSimAPNControlListStatus()
	{
	
	}

TVerdict CSimAPNControlListStatus::doTestStepL()
/**
Tests SimTsy API APN control list status functions.

*/
	{
	TInt ret = KErrNone;
	TRAP(ret, CreateConfigFileL(_L("c:\\config3.txt")););
	if (ret!=KErrNone)
		{
		INFO_PRINTF2(_L("CreateConfigFileL returned %d, expected KErrNone=0"), ret);
		SetTestStepResult(EFail);		
		return TestStepResult();
		}
		
 	TRAP(ret, SetTestNumberL(37);); 	
	if (ret!=KErrNone)
		{
		INFO_PRINTF2(_L("SetTestNumberL returned %d, expected KErrNone=0"), ret);
		SetTestStepResult(EFail);		
		return TestStepResult();
		}
		
	ret = iPhone.Open(iTelServer,KPhoneName); 
	TESTL(ret == KErrNone); 
	INFO_PRINTF2(_L("Opened the RMobilePhone with %d"),ret); 
	
	TRequestStatus reqStatus;		
	
	TBuf16<200> stringBuf;

	RMobilePhone::TAPNEntryV3 appended;
	RMobilePhone::TAPNEntryV3Pckg appendedPckg(appended);
	
	RMobilePhone::TAPNEntryV3 retrieved;
	RMobilePhone::TAPNEntryV3Pckg retrievedPckg(retrieved);		

	// Switch on the enabled services table to empty the APN List
	iPhone.SetAPNControlListServiceStatus(reqStatus,RMobilePhone::EAPNControlListServiceEnabled);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int() == KErrNone);		
	INFO_PRINTF2(_L("Set APN control list service status to enabled with result %d, expected KErrNone=0"), reqStatus.Int());
	
	// Add an O2 server address to the APN List
	appended.iApn.Zero();
	appended.iApn = SIMTSY_O2_APN_ENTRY_TO_APPEND;	
	stringBuf.Copy(appended.iApn);
	iPhone.AppendAPNName(reqStatus, appendedPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int() == KErrNone);
	INFO_PRINTF3(_L("Adding %S to the USIM resulted in %d, expected KErrNone=0"), &stringBuf, reqStatus.Int());		
	stringBuf.Zero();
	
	// Switch on the enabled services table 
	iPhone.SetAPNControlListServiceStatus(reqStatus,RMobilePhone::EAPNControlListServiceDisabled);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int() == KErrNone);		
	INFO_PRINTF2(_L("Set APN control list service status to enabled with result %d, expected KErrNone=0"), reqStatus.Int());
	
	// Make the server return the extended error codes
	ret = iTelServer.SetExtendedErrorGranularity(RTelServer::EErrorExtended);
	TEST(ret == KErrNone);
	INFO_PRINTF2(_L("Setting error returns to extended for extra granularity returned %d, expected KErrNone=0"), ret);	
		
	// Add a vodaphone server address to the APN List
	appended.iApn.Zero();
	appended.iApn = SIMTSY_VODAPHONE_APN_ENTRY_TO_APPEND;	
	stringBuf.Copy(appended.iApn);
	iPhone.AppendAPNName(reqStatus, appendedPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int() == KErrMMEtelAPNEnabledServicesTableNotFound);
	INFO_PRINTF3(_L("Adding %S to the USIM resulted in %d, expected KErrNone=-3236"), &stringBuf, reqStatus.Int());		
	stringBuf.Zero();	
	
	// Retrieve the last server address just added
	retrieved.iApn.Zero();
	iPhone.GetAPNname(reqStatus, SIMTSY_O2_APN_ENUMERATE + SIMTSY_CONFIG_ENTRIES, retrievedPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int() == KErrMMEtelAPNEnabledServicesTableNotFound);
	stringBuf.Copy(retrievedPckg().iApn);
	INFO_PRINTF3(_L("Retrieved %S from the USIM resulted in %d, expected KErrNone=-3236"), &stringBuf, reqStatus.Int());		
	stringBuf.Zero();	
	
	// Test the retrieval of the status flag
	RMobilePhone::TAPNControlListServiceStatus currentStatus;
	iPhone.GetAPNControlListServiceStatus(reqStatus,currentStatus);
	User::WaitForRequest(reqStatus);
	INFO_PRINTF3(_L("Retrieved status of %d from the USIM resulted in %d, expected KErrNone=0"), currentStatus, reqStatus.Int());			
	TEST(reqStatus.Int() == KErrNone);
	TEST(currentStatus == RMobilePhone::EAPNControlListServiceDisabled);
	
	// Wait for notification about status changes
	TRequestStatus notifytStatus;
	RMobilePhone::TAPNControlListServiceStatus changedTo;
	iPhone.NotifyAPNControlListServiceStatusChange(notifytStatus,changedTo);
	
	// Switch on the enabled services table 
	iPhone.SetAPNControlListServiceStatus(reqStatus,RMobilePhone::EAPNControlListServiceEnabled);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int() == KErrNone);		
	INFO_PRINTF2(_L("Set APN control list service status to enabled with result %d, expected KErrNone=0"), reqStatus.Int());
	
	// Notification should come back about the change in state
	User::WaitForRequest(notifytStatus);
	TEST(notifytStatus.Int() == KErrNone);	
	TEST(changedTo == RMobilePhone::EAPNControlListServiceEnabled);
	INFO_PRINTF1(_L("Set APN control list service status notification success"));
	
	// Test the cancelling of the notify status
	iPhone.NotifyAPNControlListServiceStatusChange(notifytStatus,changedTo);	
	iPhone.CancelAsyncRequest(EMobilePhoneNotifyAPNControlListServiceStatusChange);
	User::WaitForRequest(notifytStatus);
	TEST(notifytStatus.Int() == KErrCancel);
	
	// Test retrieving the service tables
	RMobilePhone::TMobilePhoneServiceTableV1 asyncSstFile;
	RMobilePhone::TMobilePhoneServiceTableV1Pckg asyncSstFilePckg(asyncSstFile);
	iPhone.GetServiceTable(reqStatus,RMobilePhone::ESIMServiceTable, asyncSstFilePckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int() == KErrNone);
	TEST(asyncSstFilePckg().iServices9To16 == RMobilePhone::KUstSMS); // As per the config file

	// Testing complete
	INFO_PRINTF2(_L("Tested notification conditions in APN from the USIM with %d (Success = -3)"),reqStatus.Int()); 

	iPhone.Close();	
	return TestStepResult();	
	}

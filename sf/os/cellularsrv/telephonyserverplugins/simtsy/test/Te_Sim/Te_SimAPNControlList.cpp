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

#include "Te_SimAPNControlList.h"

//APN Control List
#define SIMTSY_O2_APN_ENUMERATE								0
#define SIMTSY_ORANGE_APN_ENUMERATE							1
#define SIMTSY_VODAPHONE_APN_ENUMERATE						2

#define SIMTSY_CONFIG_ENTRIES								2

#define SIMTSY_O2_APN_ENTRY_TO_APPEND						_L8("wap.o2.co.uk")
#define SIMTSY_ORANGE_APN_ENTRY_TO_APPEND					_L8("orangewap")
#define SIMTSY_VODAPHONE_APN_ENTRY_TO_APPEND				_L8("wap.vodafone.co.uk")

CSimAPNControlList::CSimAPNControlList()
	{
	
	}

TVerdict CSimAPNControlList::doTestStepL()
/**
Tests SimTsy API APN control list functions.

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
	
	TUint32 amount = 0;	

	RMobilePhone::TAPNEntryV3 appended;
	RMobilePhone::TAPNEntryV3Pckg appendedPckg(appended);
	
	RMobilePhone::TAPNEntryV3 retrieved;
	RMobilePhone::TAPNEntryV3Pckg retrievedPckg(retrieved);		
	
// ** -------------------------------------------------------------------
// * Test One: Add an APN to the USIM and retrieve it
// ** -------------------------------------------------------------------	

	// Add an O2 server address to the APN List
	appended.iApn.Zero();
	appended.iApn = SIMTSY_O2_APN_ENTRY_TO_APPEND;	
	stringBuf.Copy(appended.iApn);
	iPhone.AppendAPNName(reqStatus, appendedPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int() == KErrNone);
	INFO_PRINTF3(_L("Adding %S to the USIM resulted in %d, expected KErrNone=0"), &stringBuf, reqStatus.Int());		
	stringBuf.Zero();
	
	// Retrieve the last server address just added
	retrieved.iApn.Zero();
	iPhone.GetAPNname(reqStatus, SIMTSY_O2_APN_ENUMERATE + SIMTSY_CONFIG_ENTRIES, retrievedPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int() == KErrNone);
	stringBuf.Copy(retrievedPckg().iApn);
	INFO_PRINTF3(_L("Retrieved %S from the USIM resulted in %d, expected KErrNone=0"), &stringBuf, reqStatus.Int());		
	stringBuf.Zero();

	RMobilePhone::TAPNEntryV3 output = retrievedPckg();		
	TEST(output.iApn == SIMTSY_O2_APN_ENTRY_TO_APPEND);
	
	INFO_PRINTF2(_L("Added and retrieved an APN name from the USIM with %d"),TestStepResult()); 
	INFO_PRINTF1(_L("Part a. Complete\n")); 
	
// ** -------------------------------------------------------------------
// * Test Two: Add an APN to the USIM, enumerate and retrieve it
// ** -------------------------------------------------------------------
	
	// Add an orange server address to the APN List
	appended.iApn.Zero();
	appended.iApn = SIMTSY_ORANGE_APN_ENTRY_TO_APPEND;	
	stringBuf.Copy(appended.iApn);
	iPhone.AppendAPNName(reqStatus, appendedPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int() == KErrNone);
	INFO_PRINTF3(_L("Adding %S to the USIM resulted in %d, expected KErrNone=0"), &stringBuf, reqStatus.Int());		
	stringBuf.Zero();
	
	// Get the current number of entries in the APN List
	amount = 0;
	iPhone.EnumerateAPNEntries(reqStatus, amount);
	User::WaitForRequest(reqStatus);	
	TEST(reqStatus.Int() == KErrNone);
	TEST(amount == 4); // 2 from the config file and 2 added so far
	INFO_PRINTF3(_L("Counted %d entries from the USIM with a %d result, expected KErrNone=0"), amount, reqStatus.Int()); 
	
	// Retrieve the last server address just added
	retrieved.iApn.Zero();
	iPhone.GetAPNname(reqStatus, SIMTSY_ORANGE_APN_ENUMERATE + SIMTSY_CONFIG_ENTRIES, retrievedPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int() == KErrNone);
	stringBuf.Copy(retrievedPckg().iApn);
	INFO_PRINTF3(_L("Retrieved %S from the USIM resulted in %d, expected KErrNone=0"), &stringBuf, reqStatus.Int());		
	stringBuf.Zero();	
	
	// Test the output to make sure it's the same as we added
	output = retrievedPckg();	
	TEST(output.iApn == SIMTSY_ORANGE_APN_ENTRY_TO_APPEND); 

	INFO_PRINTF2(_L("Added, enumerated and retrieved an APN name from the USIM with %d"),reqStatus.Int()); 
	INFO_PRINTF1(_L("Part b. Complete\n")); 
	
// ** -------------------------------------------------------------------
// * Test Three: Perform testing on the out of bounds conditions for the APN's
// ** -------------------------------------------------------------------		

	// Add a vodaphone server address to the APN List
	appended.iApn.Zero();
	appended.iApn = SIMTSY_VODAPHONE_APN_ENTRY_TO_APPEND;	
	stringBuf.Copy(appended.iApn);
	iPhone.AppendAPNName(reqStatus, appendedPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int() == KErrNone);
	INFO_PRINTF3(_L("Adding %S to the USIM resulted in %d, expected KErrNone=0"), &stringBuf, reqStatus.Int());		
	stringBuf.Zero();

	// Get the current number of entries in the APN List
	amount = 0;
	iPhone.EnumerateAPNEntries(reqStatus, amount);
	User::WaitForRequest(reqStatus);	
	TEST(reqStatus.Int() == KErrNone);
	TEST(amount == 5); // 2 from the config file and 3 added so far
	INFO_PRINTF3(_L("Counted %d entries from the USIM with a %d result, expected KErrNone=0"), amount, reqStatus.Int()); 
	
	// Delete the last added entry in the APN list
	iPhone.DeleteAPNName(reqStatus, SIMTSY_VODAPHONE_APN_ENUMERATE + SIMTSY_CONFIG_ENTRIES);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int() == KErrNone);
	
	// Retrieve the last server address just added
	retrieved.iApn.Zero();
	iPhone.GetAPNname(reqStatus, SIMTSY_VODAPHONE_APN_ENUMERATE + SIMTSY_CONFIG_ENTRIES, retrievedPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int() == KErrNotFound);
	stringBuf.Copy(retrievedPckg().iApn);
	INFO_PRINTF3(_L("Retrieved %S from the USIM resulted in %d, expected KErrNone=-1"), &stringBuf, reqStatus.Int());		
	stringBuf.Zero();		
	
	// Delete the last added entry in the APN list and allow it to fail
	iPhone.DeleteAPNName(reqStatus, SIMTSY_VODAPHONE_APN_ENUMERATE + SIMTSY_CONFIG_ENTRIES);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int() == KErrNotFound);
	INFO_PRINTF2(_L("Deleting non-existant entry returned with result %d, expected=-1"), reqStatus.Int());	
	
	// Make the server return the extended error codes
	ret = iTelServer.SetExtendedErrorGranularity(RTelServer::EErrorExtended);
	TEST(ret == KErrNone);
	INFO_PRINTF2(_L("Setting error returns to extended for extra granularity returned %d, expected KErrNone=0"), ret);	
	
	// Delete the last added entry in the APN list and allow it to fail again to test for extended 
	// error codes
	iPhone.DeleteAPNName(reqStatus, SIMTSY_VODAPHONE_APN_ENUMERATE + SIMTSY_CONFIG_ENTRIES);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int() == KErrMMEtelAPNNameInvalidIndex);	
	INFO_PRINTF2(_L("Deleting non-existant entry with entended errors returned with result %d, expected=-3235"), reqStatus.Int());
	
	// Switch off the enabled services table to test extended code for not enabled
	iPhone.SetAPNControlListServiceStatus(reqStatus,RMobilePhone::EAPNControlListServiceDisabled);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int() == KErrNone);		
	INFO_PRINTF2(_L("Set APN control list service status to disabled with result %d, expected KErrNone=0"), reqStatus.Int());
	
	// Add a vodaphone server address to the APN List
	appended.iApn.Zero();
	appended.iApn = SIMTSY_VODAPHONE_APN_ENTRY_TO_APPEND;	
	stringBuf.Copy(appended.iApn);
	iPhone.AppendAPNName(reqStatus, appendedPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int() == KErrMMEtelAPNEnabledServicesTableNotFound);
	INFO_PRINTF3(_L("Adding %S to the USIM resulted in %d, expected KErrNone=-3236"), &stringBuf, reqStatus.Int());		
	stringBuf.Zero();
	
	// Switch on the enabled services table to empty the APN List
	iPhone.SetAPNControlListServiceStatus(reqStatus,RMobilePhone::EAPNControlListServiceEnabled);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int() == KErrNone);		
	INFO_PRINTF2(_L("Set APN control list service status to enabled with result %d, expected KErrNone=0"), reqStatus.Int());
	
	// Make the server return the extended error codes
	ret = iTelServer.SetExtendedErrorGranularity(RTelServer::EErrorBasic);
	TEST(ret == KErrNone);
	INFO_PRINTF2(_L("Setting error returns to basic granularity returned %d, expected KErrNone=0"), ret);	
	
	// Delete all the entries in the APN List
	while (reqStatus.Int() == KErrNone)
		{
		TEST(reqStatus.Int() == KErrNone);
		
		// Delete the last added entry in the APN list and allow it to fail
		iPhone.DeleteAPNName(reqStatus, 0);
		User::WaitForRequest(reqStatus);
		INFO_PRINTF2(_L("Deleting entry returned with result %d, expected=-1 or 0"), reqStatus.Int());	
		}
	TEST(reqStatus.Int() == KErrNotFound);	
	
	// Get the current number of entries in the APN List
	amount = 0;
	iPhone.EnumerateAPNEntries(reqStatus, amount);
	User::WaitForRequest(reqStatus);	
	TEST(reqStatus.Int() == KErrNone);
	TEST(amount == 0); // list should be empty
	INFO_PRINTF3(_L("Counted %d entries from the USIM with a %d result, expected KErrNone=0"), amount, reqStatus.Int()); 		
		
	INFO_PRINTF2(_L("Tested out of bounds conditions in APN from the USIM with %d"),reqStatus.Int()); 
	INFO_PRINTF1(_L("Part c. Complete\n")); 

// ** -------------------------------------------------------------------
// * Test Four: Test the notification functionality on APN changes
// ** -------------------------------------------------------------------	

	TRequestStatus notifyReqStatus;
	
	// Asynchronous
	iPhone.NotifyAPNListChanged(notifyReqStatus);
	
	iPhone.AppendAPNName(reqStatus, appendedPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int() == KErrNone);
	
	User::WaitForRequest(notifyReqStatus);
	TEST(notifyReqStatus.Int() == KErrNone);

	iPhone.NotifyAPNListChanged(reqStatus);
	iPhone.CancelAsyncRequest(EMobilePhoneNotifyAPNListChanged);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int() == KErrCancel);
	INFO_PRINTF2(_L("Tested notification conditions in APN from the USIM with %d (Success = -3)"),reqStatus.Int()); 
	
	INFO_PRINTF1(_L("Part d. Complete\n")); 	
	
// ** -------------------------------------------------------------------
// * Test Five: Test the notification functionality on APN changes with 
// * 			extended error codes
// ** -------------------------------------------------------------------		

	ret = iTelServer.SetExtendedErrorGranularity(RTelServer::EErrorExtended);
	TEST(ret == KErrNone);

	iPhone.DeleteAPNName(reqStatus, 100);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int() == KErrMMEtelAPNNameInvalidIndex);
	
	INFO_PRINTF2(_L("Testing extended error conditions complete"),reqStatus.Int()); 
	INFO_PRINTF1(_L("Part e. Complete\n")); 	

	iPhone.Close();	
	return TestStepResult();	
	}

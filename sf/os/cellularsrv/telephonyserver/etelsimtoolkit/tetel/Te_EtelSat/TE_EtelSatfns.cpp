// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "Te_EtelSatTestStepBase.h"
#include "TE_EtelSatfns.h"

CTestSatFns::CTestSatFns()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestSatFns"));
	}

enum TVerdict CTestSatFns::doTestStepL()
    {
	INFO_PRINTF1(_L("************************"));
	INFO_PRINTF1(_L("Test RSat Functionality"));
	INFO_PRINTF1(_L("************************"));

	RPhone phone;
	TInt ret=phone.Open(iTelServer,DSATTSY_PHONE_NAME);

	TEST(ret==KErrNone);

	RSat sat;
	ret=sat.Open(phone);
    TEST(ret==KErrNone);
	
	// local variables used throughout the sat tests
	TRequestStatus reqStatus;
	iTestCount=1;

	//Test delegation of capability policy checking
	RTestSat testSat;
	ret=testSat.Open(phone);
	TEST(ret==KErrNone);

	INFO_PRINTF2(_L("Test %d - Test delegation of capability policy checking"),iTestCount++);
 
	//Test calling CustomIPC1. Should return KErrNone
    ret = testSat.TestCustomIPC1();  
    TEST(ret==KErrNone);
	testSat.Close();

	// Session Start
	sat.NotifyProactiveSimSessionStart(reqStatus);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);
	INFO_PRINTF2(_L("Test %d - RSat::NotifyProactiveSimSessionStart passed"),iTestCount++);

	sat.NotifyProactiveSimSessionStart(reqStatus);
	sat.CancelAsyncRequest(ESatNotifyProactiveSimSessionStart);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);
	INFO_PRINTF2(_L("Test %d - RSat::NotifyProactiveSimSessionStartCancel passed"),iTestCount++);

	// Session End
	sat.NotifyProactiveSimSessionEnd(reqStatus);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);
	INFO_PRINTF2(_L("Test %d - RSat::NotifyProactiveSimSessionEnd passed"),iTestCount++);

	sat.NotifyProactiveSimSessionEnd(reqStatus);
	sat.CancelAsyncRequest(ESatNotifyProactiveSimSessionEnd);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);
	INFO_PRINTF2(_L("Test %d - RSat::NotifyProactiveSimSessionEndCancel passed"),iTestCount++);

	// Get Icon
	RSat::TIcon icon;
	sat.GetIcon(reqStatus,1,icon); 
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);
	TEST(icon==DSATTSY_ICON_EF1);
	INFO_PRINTF2(_L("Test %d - RSat::GetIcon passed"),iTestCount++);

	sat.GetIcon(reqStatus,2,icon); 
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);
	TEST(icon==DSATTSY_ICON_EF2);
	INFO_PRINTF2(_L("Test %d - RSat::GetIcon passed"),iTestCount++);

	sat.GetIcon(reqStatus,1,icon);
	sat.CancelAsyncRequest(ESatGetIcon);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);
	INFO_PRINTF2(_L("Test %d - RSat::GetIconCancel passed"),iTestCount++);

	// Get Image Instance
	// The Client should have read the icon EF before reading the image instance.
	// The contents of the icon EF indicate the size of the image instance,
	// the Client should use this information to allocate a buffer size that it requires
	// and pass a reference to is as a TDes8&

	TBuf8<12> instance;
	RSat::TInstanceInfoV3 instanceInfo;

	// first test
	instanceInfo.iInstanceNumber=DSATTSY_INSTANCE_NUMBER1;
	instanceInfo.iLength=DSATTSY_INSTANCE_LENGTH1;
	instanceInfo.iOffset=DSATTSY_INSTANCE_OFFSET1;
	instanceInfo.iCoding=DSATTSY_INSTANCE_CODING_BASIC;
	instanceInfo.iImgWidth=DSATTSY_INSTANCE_WIDTH_1;
	instanceInfo.iImgHeight=DSATTSY_INSTANCE_HEIGHT_1;
	instanceInfo.iFileId=DSATTSY_INSTANCE_FILE_ID_1;
		
	RSat::TInstanceInfoV3Pckg instanceInfoPackage1(instanceInfo);
	
	sat.GetImageInstance(reqStatus, instanceInfoPackage1, instance);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);
	TEST(instance==DSATTSY_IMG_INSTANCE1);
	INFO_PRINTF2(_L("Test %d - RSat::GetImageInstance passed"),iTestCount++);

	// second test 
	instanceInfo.iInstanceNumber=DSATTSY_INSTANCE_NUMBER2;
	instanceInfo.iLength=DSATTSY_INSTANCE_LENGTH2;
	instanceInfo.iOffset=DSATTSY_INSTANCE_OFFSET2;
	instanceInfo.iCoding=DSATTSY_INSTANCE_CODING_COLOUR;
	instanceInfo.iImgWidth=DSATTSY_INSTANCE_WIDTH_2;
	instanceInfo.iImgHeight=DSATTSY_INSTANCE_HEIGHT_2;
	instanceInfo.iFileId=DSATTSY_INSTANCE_FILE_ID_2;
	
	RSat::TInstanceInfoV3Pckg instanceInfoPackage2(instanceInfo);

	sat.GetImageInstance(reqStatus,instanceInfoPackage2,instance);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);
	TEST(instance==DSATTSY_IMG_INSTANCE2);
	INFO_PRINTF2(_L("Test %d - RSat::GetImageInstance passed"),iTestCount++);

	// cancel test
	sat.GetImageInstance(reqStatus,instanceInfoPackage1,instance);
	sat.CancelAsyncRequest(ESatGetImageInstance);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);
	INFO_PRINTF2(_L("Test %d - RSat::GetImageInstanceCancel passed"),iTestCount++);

	// GetClut
	// The size of the clut should be known at this point (it is defined in the image instance)
	// The Client allocates a buffer of the required size.
	TBuf8<5> clut;
	sat.GetClut(reqStatus, DSATTSY_INSTANCE_NUMBER1, DSATTSY_CLUT_OFFSET, clut);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);
	TEST(clut==DSATTSY_CLUT1);
	INFO_PRINTF2(_L("Test %d - RSat::GetClut passed"),iTestCount++);

	sat.GetClut(reqStatus,DSATTSY_INSTANCE_NUMBER1, DSATTSY_CLUT_OFFSET, clut);
	sat.CancelAsyncRequest(ESatGetClut);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);
	INFO_PRINTF2(_L("Test %d - RSat::GetClutCancel passed"),iTestCount++);

	//Client Ready Indication
	TEST(sat.UsatClientReadyIndication()==KErrNone);
	
	//Client notification of TSy completing handling of a PCmd 
	RSat::TPCmd pcmd;
	sat.NotifyTsyStateUpdated(reqStatus, pcmd);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus==KErrNone);
	TEST(pcmd==DSATTSY_SENDSS);
	
	//test cancel
	sat.NotifyTsyStateUpdated(reqStatus, pcmd);
	sat.CancelAsyncRequest(ESatNotifyTsyStateUpdated);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus==KErrCancel);
	INFO_PRINTF2(_L("Test %d - RSat::NotifyTsyStateUpdated passed"),iTestCount++);	

	// End of RSat tests.
	sat.Close();
	phone.Close();

	return TestStepResult();

	}

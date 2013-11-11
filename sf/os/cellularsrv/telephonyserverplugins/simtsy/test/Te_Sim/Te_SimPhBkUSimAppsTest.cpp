// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "Te_SimPhBkUSimAppsTest.h"

CSimPhBkUSimAppsTest::CSimPhBkUSimAppsTest() 
	{ 
	SetTestStepName(_L("USimAppsTest"));
	}

TVerdict CSimPhBkUSimAppsTest::doTestStepL()
	{
	INFO_PRINTF1(_L("BeginPhBkUSimAppsTest"));

 	CreateConfigFileL(_L("c:\\config3.txt"));
 	SetTestNumberL(5);

	RMobilePhone::TAID activeAID;
	TInt count;
	TRequestStatus aReqStatus,stat0;

	TInt ret = iPhone.Open(iTelServer,KPhoneName); 
	INFO_PRINTF2(_L("Result: %d"),ret);
	TESTL(ret == KErrNone); 
	INFO_PRINTF1(_L("Opened phone object"));

	INFO_PRINTF1(_L("EnumerateUSimApplications test"));

	iPhone.EnumerateUSimApplications(aReqStatus, count, activeAID);
	User::WaitForRequest(aReqStatus);
	TESTL(activeAID.Compare(_L8("App2"))==0);
	TESTL(count == 3);
	CHECKPOINTL(aReqStatus, KErrNone, CHP_APPS_CASE("G.1"));

	//Test GetUSimApplicationInfo
	RMobilePhone::TUSimApplicationInfoV2 usimAppInfo;
	RMobilePhone::TUSimApplicationInfoV2Pckg usimAppInfoPckg(usimAppInfo);

	TInt index=0;

	INFO_PRINTF1(_L("GetUSimApplicationInfo test"));

	iPhone.GetUSimApplicationInfo(aReqStatus, index, usimAppInfoPckg);
	User::WaitForRequest(aReqStatus);
	CHECKPOINTL(aReqStatus, KErrNone, CHP_APPS_CASE("G.2"));
	TESTL(usimAppInfo.iAID.Compare(_L8("App2")) == KErrNone);
	TESTL(usimAppInfo.iLabel.Compare(_L8("PhoneApp2")) == KErrNone);

	index=5;
	iPhone.GetUSimApplicationInfo(aReqStatus, index, usimAppInfoPckg);
	User::WaitForRequest(aReqStatus);
	CHECKPOINT_EXL(aReqStatus==KErrNotFound, CHP_APPS_CASE("G.3"));
	TESTL(usimAppInfo.iAID.Compare(_L8("App2")) == KErrNone);
	TESTL(usimAppInfo.iLabel.Compare(_L8("PhoneApp2")) == KErrNone);

	INFO_PRINTF1(_L("NotifyUSimApplicationsInfoChange test"));
	count = 0;	// Set count to some value different to that of the number of Apps.
	iPhone.NotifyUSimApplicationsInfoChange(aReqStatus, count, activeAID);
	User::WaitForRequest(aReqStatus);
	CHECKPOINTL(aReqStatus, KErrNone, CHP_APPS_CASE("G.4"));
	TESTL(activeAID.Compare(_L8("App1")) == 0);
	TESTL(count == 3);

	iPhone.EnumerateUSimApplications(aReqStatus, count, activeAID);
	User::WaitForRequest(aReqStatus);
	TESTL(aReqStatus.Int() == KErrNone);
	TESTL(activeAID.Compare(_L8("App1"))==0);
	TESTL(count == 3);

	RMobilePhone::TUSimSelectionMode selectionMode;
	INFO_PRINTF1(_L("GetUSimAppsSelectionMode test"));
	ret = iPhone.GetUSimAppsSelectionMode(selectionMode); 
	CHECKPOINTL(ret, KErrNone, CHP_APPS_CASE("G.8"));

	TESTL(selectionMode == RMobilePhone::EUSimSelectionAutomatic);
	
	INFO_PRINTF1(_L("NotifyUSimAppsSelectionModeChange test"));
	iPhone.NotifyUSimAppsSelectionModeChange(aReqStatus, selectionMode);
	INFO_PRINTF1(_L("SetUSimAppsSelectionMode test"));
	ret=iPhone.SetUSimAppsSelectionMode(RMobilePhone::EUSimSelectionManual);
	CHECKPOINTL(ret, KErrNone, CHP_APPS_CASE("G.7"));
	User::WaitForRequest(aReqStatus);
	CHECKPOINTL(aReqStatus, KErrNone, CHP_APPS_CASE("G.11"));
	TESTL(selectionMode == RMobilePhone::EUSimSelectionManual);

	iPhone.SetUSimApplicationStatus(aReqStatus, _L8("App2"), RMobilePhone::EUSimActivate);
	User::WaitForRequest(aReqStatus);
	
	iPhone.NotifyUSimApplicationsInfoChange(aReqStatus, count, activeAID);
	iPhone.SetUSimApplicationStatus(stat0, _L8("App2"), RMobilePhone::EUSimTerminate);
	User::WaitForRequest(aReqStatus);
	User::WaitForRequest(stat0);
	CHECKPOINTL(stat0, KErrNone, CHP_APPS_CASE("G.5"));
	CHECKPOINTL(aReqStatus, KErrNone, CHP_APPS_CASE("G.4"));

	iPhone.SetUSimApplicationStatus(stat0, _L8("App5"), RMobilePhone::EUSimTerminate);
	User::WaitForRequest(stat0);
	CHECKPOINTL(stat0, KErrNotFound, CHP_APPS_CASE("G.6"));
	
	iPhone.Close();
	ASSERT(RThread().RequestCount()==0);

	return TestStepResult();
	}


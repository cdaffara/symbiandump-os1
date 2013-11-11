// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "Te_SimCellInfoTest.h"

CSimCellInfoTest::CSimCellInfoTest() 
	{ 
	SetTestStepName(_L("CellInfoTest"));
	}

TVerdict CSimCellInfoTest::doTestStepL()
	{
	INFO_PRINTF1(_L("BeginCellInfoTest"));

 	CreateConfigFileL(_L("c:\\config3.txt"));
 	SetTestNumberL(0);
 	
 	const RMobilePhone::TCellMode KMode0 = RMobilePhone::ECellModeGsm;
 	const TUint KCellId0  = 21;
 	const RMobilePhone::TGsmTimingAdvance KTimingAdvance0 = 111;
 	const RMobilePhone::TCellInfoStatus KStatus0 = RMobilePhone::ECellInfoStatusNotCurrent;

 	const RMobilePhone::TCellMode KMode1 = RMobilePhone::ECellModeWcdma;
 	const TUint KCellId1  = 514;
 	const RMobilePhone::TGsmTimingAdvance KTimingAdvance1 = 73;
 	const RMobilePhone::TCellInfoStatus KStatus1 = RMobilePhone::ECellInfoStatusCurrent;

	TInt ret = iPhone.Open(iTelServer,KPhoneName); 
	INFO_PRINTF2(_L("Result: %d"),ret); 
	TESTL(ret == KErrNone); 
	INFO_PRINTF1(_L("Opened phone object"));

	TESTL(iPhone.Initialise()==KErrNone);
	INFO_PRINTF1(_L("Opened phone object"));

	INFO_PRINTF1(_L("Test Retrieve current Cell Info information"));
	TRequestStatus stat0;
	RMobilePhone::TMobilePhoneCellInfoV9 cellInfo;
	RMobilePhone::TMobilePhoneCellInfoV9Pckg cellInfoPckg(cellInfo);
	
	iPhone.GetCellInfo(stat0,cellInfoPckg);
	User::WaitForRequest(stat0);
	TESTL(stat0==KErrNone);
	TESTL(cellInfo.iMode==KMode0);
	TESTL(cellInfo.iCellId==KCellId0);
	TESTL(cellInfo.iTimingAdvance==KTimingAdvance0);
	TESTL(cellInfo.iStatus==KStatus0);

	INFO_PRINTF1(_L("Test Notification of Cell information changing"));
	iPhone.NotifyCellInfoChange(stat0,cellInfoPckg);
	User::WaitForRequest(stat0);
	TESTL(stat0==KErrNone);
	TESTL(cellInfo.iMode==KMode1);
	TESTL(cellInfo.iCellId==KCellId1);
	TESTL(cellInfo.iTimingAdvance==KTimingAdvance1);
	TESTL(cellInfo.iStatus==KStatus1);

	INFO_PRINTF1(_L("Test Retrieve current Cell Info and cancellation"));
	iPhone.GetCellInfo(stat0,cellInfoPckg);
	iPhone.CancelAsyncRequest(EMobilePhoneGetCellInfo);
	User::WaitForRequest(stat0);
	TESTL((stat0==KErrNone) || (stat0==KErrCancel) );

	INFO_PRINTF1(_L("Test Notification of Cell information changing and cancllation"));
	iPhone.NotifyCellInfoChange(stat0,cellInfoPckg);
	iPhone.CancelAsyncRequest(EMobilePhoneNotifyCellInfoChange);
	User::WaitForRequest(stat0);
	TESTL((stat0==KErrNone) || (stat0==KErrCancel) );
	
	iPhone.Close();
	ASSERT(RThread().RequestCount()==0);

	return TestStepResult();
	}


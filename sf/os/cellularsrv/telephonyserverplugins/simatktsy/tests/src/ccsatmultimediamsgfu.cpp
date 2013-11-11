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



/**
 @file The TEFUnit test suite for MultimediaMsg in the SAT.
*/

#include "ccsatmultimediamsgfu.h"
#include <etel.h>
#include <etelmm.h>
#include <test/tmockltsydata.h>

CTestSuite* CCSatMultimediaMsgFU::CreateSuiteL(const TDesC& aName)
	{
	SUB_SUITE;

	ADD_TEST_STEP_ISO_CPP(CCSatMultimediaMsgFU, TestNotifyRetrieveMultimediaMsgPCmd0001L);
	ADD_TEST_STEP_ISO_CPP(CCSatMultimediaMsgFU, TestNotifySubmitMultimediaMsgPCmd0001L);
	ADD_TEST_STEP_ISO_CPP(CCSatMultimediaMsgFU, TestNotifyDisplayMultimediaMsgPCmd0001L);

	END_SUITE;
	}


//
// Actual test cases
//


/**
@SYMTestCaseID BA-CSAT-MMM-RMMMPC-0001
@SYMPREQ 1780
@SYMComponent  telephony_csat
@SYMTestCaseDesc Test support in CSAT for RSat::NotifyRetrieveMultimediaMsgPCmd
@SYMTestPriority High
@SYMTestActions Invokes RSat::NotifyRetrieveMultimediaMsgPCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCSatMultimediaMsgFU::TestNotifyRetrieveMultimediaMsgPCmd0001L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();	
	
	RSat::TRetrieveMultimediaMessageV6 msg;
	RSat::TRetrieveMultimediaMessageV6Pckg msgPck(msg);
	
	TRequestStatus notifyStatus;

	// RetrieveMultimediaMessage proactive command is not supported in current version of Sat	
	iSat.NotifyRetrieveMultimediaMsgPCmd(notifyStatus, msgPck);

	User::WaitForRequest(notifyStatus);
	ASSERT_EQUALS(KErrNotSupported, notifyStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(1, this); // this
	}


/**
@SYMTestCaseID BA-CSAT-MMM-SMMMPC-0001
@SYMPREQ 1780
@SYMComponent  telephony_csat
@SYMTestCaseDesc Test support in CSAT for RSat::NotifySubmitMultimediaMsgPCmd
@SYMTestPriority High
@SYMTestActions Invokes RSat::NotifySubmitMultimediaMsgPCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCSatMultimediaMsgFU::TestNotifySubmitMultimediaMsgPCmd0001L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();	
	
	RSat::TSubmitMultimediaMessageV6 msg;
	RSat::TSubmitMultimediaMessageV6Pckg msgPck(msg);
	
	TRequestStatus notifyStatus;

	// SubmitMultimediaMessage proactive command is not supported in current version of Sat	
	iSat.NotifySubmitMultimediaMsgPCmd(notifyStatus, msgPck);

	User::WaitForRequest(notifyStatus);
	ASSERT_EQUALS(KErrNotSupported, notifyStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(1, this); // this
	}


/**
@SYMTestCaseID BA-CSAT-MMM-DMMMPC-0001
@SYMPREQ 1780
@SYMComponent  telephony_csat
@SYMTestCaseDesc Test support in CSAT for RSat::NotifyDisplayMultimediaMsgPCmd
@SYMTestPriority High
@SYMTestActions Invokes RSat::NotifyDisplayMultimediaMsgPCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCSatMultimediaMsgFU::TestNotifyDisplayMultimediaMsgPCmd0001L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();	
	
	RSat::TDisplayMultimediaMessageV6 msg;
	RSat::TDisplayMultimediaMessageV6Pckg msgPck(msg);
	
	TRequestStatus notifyStatus;

	// DisplayMultimediaMessage proactive command is not supported in current version of Sat	
	iSat.NotifyDisplayMultimediaMsgPCmd(notifyStatus, msgPck);

	User::WaitForRequest(notifyStatus);
	ASSERT_EQUALS(KErrNotSupported, notifyStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(1, this); // this	
	}




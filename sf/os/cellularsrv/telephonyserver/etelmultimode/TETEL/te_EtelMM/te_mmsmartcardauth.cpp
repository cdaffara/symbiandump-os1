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
// Tests ETelMM's SmartCardAuthenticate API.  This uses
// the dummy TSY (dmmtsy.tsy) to run basic tests on the
// RMobileSmartCardEap.
// 
//

/**
 @file
 @internalComponent
*/


#include <e32test.h>

#include "TE_EtelMMTestStepBase.h"
#include "te_mmsmartcardauth.h"
#include "testdef.h"

/**
Each test step initialises it's own name
*/
CTestSmartCardAuthenticate::CTestSmartCardAuthenticate()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestMMSmartCardAuthenticate"));
	}

/**
Test RMobilePhone::SmartCardAuthenticate() for default (U)SIM and for a
specific AID.
*/
enum TVerdict CTestSmartCardAuthenticate::doTestStepL()
	{
	INFO_PRINTF1(_L("Test SmartCard Application based EAP Functionality"));

	RMobilePhone mmPhone;
	TInt ret = mmPhone.Open(iTelServer, DMMTSY_PHONE_NAME);
	TEST(ret == KErrNone);

	//Test SmartCardAuthenticate (Default AID)
	RMobilePhone::TSmartCardAuthenticateDataV6 authenticateData;
	authenticateData.iAUTN = DMMTSY_PHONE_SC_AUTN_IN;
	authenticateData.iRAND = DMMTSY_PHONE_SC_RAND_IN;

	// asynchronous
	INFO_PRINTF1(_L("Test - RMobilePhone::SmartCardAuthenticate async."));
	TRequestStatus reqStatus;

	RMobilePhone::TSmartCardAuthenticateDataV6Pckg authenticateDataPckgd(authenticateData);
	mmPhone.SmartCardAuthenticate(reqStatus, authenticateDataPckgd);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus == KErrNone);
	TEST(authenticateData.iAUTN == DMMTSY_PHONE_SC_AUTN_OUT);
	TEST(authenticateData.iRAND == DMMTSY_PHONE_SC_RAND_OUT);    
	TEST(authenticateData.iRES == DMMTSY_PHONE_SC_RES_OUT);
	TEST(authenticateData.iIK == DMMTSY_PHONE_SC_IK_OUT);        
	TEST(authenticateData.iCK == DMMTSY_PHONE_SC_CK_OUT);
	TEST(authenticateData.iAUTS == DMMTSY_PHONE_SC_AUTS_OUT);
	TEST(authenticateData.iKc == DMMTSY_PHONE_SC_KC_OUT);

	// asynchronous & cancel
	authenticateData.iAUTN = DMMTSY_PHONE_SC_AUTN_IN;
	authenticateData.iRAND = DMMTSY_PHONE_SC_RAND_IN;

	mmPhone.SmartCardAuthenticate(reqStatus, authenticateDataPckgd);
	mmPhone.CancelAsyncRequest(EMobilePhoneSmartCardAuthenticate);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus == KErrCancel);

	//Test SmartCardAuthenticate with specific AID
	RMobilePhone::TSmartCardAuthenticateDataV6 authenticateData2;
	authenticateData2.iAUTN = DMMTSY_PHONE_SC_AUTN_IN;
	authenticateData2.iRAND = DMMTSY_PHONE_SC_RAND_IN;

	RMobilePhone::TAID scAppAID = DMMTSY_PHONE_EAPAPP_AID();

	// asynchronous
	INFO_PRINTF1(_L("Test - RMobilePhone::SmartCardAuthenticate async with AID."));

	RMobilePhone::TSmartCardAuthenticateDataV6Pckg authenticateDataPckgd2(authenticateData2);
	mmPhone.SmartCardAuthenticate(reqStatus, scAppAID, authenticateDataPckgd2);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus == KErrNone);
	TEST(authenticateData2.iAUTN == DMMTSY_PHONE_SC_AUTN_OUT);
	TEST(authenticateData2.iRAND == DMMTSY_PHONE_SC_RAND_OUT);    
	TEST(authenticateData2.iRES == DMMTSY_PHONE_SC_RES_OUT);
	TEST(authenticateData2.iIK == DMMTSY_PHONE_SC_IK_OUT);        
	TEST(authenticateData2.iCK == DMMTSY_PHONE_SC_CK_OUT);
	TEST(authenticateData2.iAUTS == DMMTSY_PHONE_SC_AUTS_OUT);
	TEST(authenticateData2.iKc == DMMTSY_PHONE_SC_KC_OUT);

	// asynchronous & cancel
	authenticateData2.iAUTN = DMMTSY_PHONE_SC_AUTN_IN;
	authenticateData2.iRAND = DMMTSY_PHONE_SC_RAND_IN;

	mmPhone.SmartCardAuthenticate(reqStatus, scAppAID, authenticateDataPckgd2);
	mmPhone.CancelAsyncRequest(EMobilePhoneSmartCardAuthenticate);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus == KErrCancel);

	mmPhone.Close(); 
	return TestStepResult();
	}

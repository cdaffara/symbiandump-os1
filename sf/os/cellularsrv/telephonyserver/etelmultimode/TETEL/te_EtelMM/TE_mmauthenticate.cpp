// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This is the cpp file for MM Authenticate
// 
//

/**
 @file
*/

#include <e32base.h>

#include "ETELMM.H"

#include "testdef.h"

#include "TE_mmauthenticate.h"


CEtelMMTestAuthenticate::CEtelMMTestAuthenticate()
{
	// store the name of this test case
	SetTestStepName(_L("TestAuthenticate"));
}

// destructor
CEtelMMTestAuthenticate::~CEtelMMTestAuthenticate()
{
}

enum TVerdict CEtelMMTestAuthenticate::doTestStepL( void )
/**
 * Test step Authenticate
 *
 */
{
   __UHEAP_MARK;
	TInt ret;
	RMobilePhone phone;
	ret=phone.Open(iTelServer,DMMTSY_PHONE_NAMEAUTH1);
	TEST(ret==KErrNone);
	// start sanity test for
	// 2.4	REQ3877 Enable retrieval of the IMS authentication parameters

  	TRequestStatus reqStatus;
  	RMobilePhone::TImsAuthenticateDataV5 authenticateData;
  	authenticateData.iAUTN=DMMTSY_PHONE_AUTN_IN;
	authenticateData.iRAND=DMMTSY_PHONE_RAND_IN;
	authenticateData.iRES=DMMTSY_PHONE_RES_IN;
	authenticateData.iIK=DMMTSY_PHONE_IK_IN;
	authenticateData.iCK=DMMTSY_PHONE_CK_IN;
	authenticateData.iAUTS=DMMTSY_PHONE_AUTS_IN;


  	RMobilePhone::TImsAuthenticateDataV5Pckg authenticateDataPckgd(authenticateData);
  	phone.ImsAuthenticate (reqStatus,authenticateDataPckgd);
  	User::WaitForRequest(reqStatus);
  	TEST(reqStatus==KErrNone);
  	TEST(authenticateData.iAUTN==DMMTSY_PHONE_AUTN_OUT);
	TEST(authenticateData.iRAND==DMMTSY_PHONE_RAND_OUT);	
	TEST(authenticateData.iRES==DMMTSY_PHONE_RES_OUT);
	TEST(authenticateData.iIK==DMMTSY_PHONE_IK_OUT);		
	TEST(authenticateData.iCK==DMMTSY_PHONE_CK_OUT);
	TEST(authenticateData.iAUTS==DMMTSY_PHONE_AUTS_OUT);	
	
	
	authenticateData.iAUTN=DMMTSY_PHONE_AUTN_IN;
	authenticateData.iRAND=DMMTSY_PHONE_RAND_IN;
	
	phone.ImsAuthenticate(reqStatus,authenticateDataPckgd);
	phone.CancelAsyncRequest(EMobilePhoneIMSAuthenticate);
  	User::WaitForRequest(reqStatus);
  	TEST(reqStatus==KErrCancel);
	
			
  	phone.Close();
	// end sanity test for
	// 2.4	REQ3877 Enable retrieval of the IMS authentication parameters
	__UHEAP_MARKEND;

	return TestStepResult();
}



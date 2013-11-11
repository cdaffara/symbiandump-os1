// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file TestIAPWArningDialog.cpp
*/

//Symbian OS Headers
#include <c32comm.h>
#include <agentdialog.h>

//Test Framework Header
#include <test/testexecutelog.h>

//Test Step Header
#include "TestIAPWArningDialog.h"


CTestIAPWArningDialog::~CTestIAPWArningDialog()
/**
 * Destructor
 */
	{
	}

CTestIAPWArningDialog::CTestIAPWArningDialog()
/**
 * Constructor
 */
	{
	SetTestStepName(KTestIAPWArningDialog);
	}

TVerdict CTestIAPWArningDialog::doTestStepL()
	{
	
	TConnectionPrefs prefs;
	_LIT(KIapName,"New IAP");

 	// When bootstrapping C32 we have to avoid the PhBkSyncServer being started, since
 	// it needs a different CommDB
 	_LIT(KPhbkSyncCMI, "phbsync.cmi");
    TInt ret = StartC32WithCMISuppressions(KPhbkSyncCMI);
	if ((ret != KErrNone) && (ret != KErrAlreadyExists))
		INFO_PRINTF2(_L("error is : %d \n"),ret);
	else	
		INFO_PRINTF1(_L("Started C32\n"));

	RGenConAgentDialogServer dlgSv;
	TRAP_IGNORE(dlgSv.Connect());
	INFO_PRINTF1(_L("Connected Dialog Server\n"));

	TRequestStatus status;
	
	INFO_PRINTF1(_L("\nTesting the IAP Warning Dialog\n")); 
	TBuf<16> iapName(KIapName);
	TBool response = EFalse;
	dlgSv.WarnNewIapConnection(prefs,KErrBadName,iapName,response,status);
	INFO_PRINTF1(_L("Requested IAP warning\n"));
	User::WaitForRequest(status);
	ret=status.Int();
	INFO_PRINTF2(_L("Request IAP warning completed with code=%d\n"),ret);
	checkError(dlgSv,ret);
	if (response)
		INFO_PRINTF1(_L("Attempt new connection\n"));
	else
		INFO_PRINTF1(_L("Do not attempt new connection\n"));

	
	return TestStepResult();
	}


void CTestIAPWArningDialog::checkError(RGenConAgentDialogServer& aDlgSvr, TInt aError)
	{
	if (aError!=KErrNone && aError!=KErrCancel)
		aDlgSvr.Close();

	if ((aError!=KErrNone) && (aError!=KErrCancel))
		INFO_PRINTF2(_L("Error is : %d \n"), aError);
	}

void CTestIAPWArningDialog::checkPctError(RGenConAgentDialogServer& aDlgSvr, TInt aError)
	{
	if (aError!=KErrNone)
		{
		aDlgSvr.ClosePct();
		TRequestStatus destroyStatus;
		User::WaitForRequest(destroyStatus);
		aDlgSvr.Close();
		}

	TEST(aError==KErrNone);
	}	



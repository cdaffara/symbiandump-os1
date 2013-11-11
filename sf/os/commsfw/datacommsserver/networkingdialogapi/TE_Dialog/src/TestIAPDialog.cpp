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
 @file TestIAPDialog.cpp
*/

//Symbian OS Headers
#include <c32comm.h>
#include <agentdialog.h>

//Test Framework Header
#include <test/testexecutelog.h>

//Test Step Header
#include "TestIAPDialog.h"


//

CTestIAPDialog::~CTestIAPDialog()
/**
 * Destructor
 */
	{
	}

CTestIAPDialog::CTestIAPDialog()
/**
 * Constructor
 */
	{
	SetTestStepName(KTestIAPDialog);
	}

TVerdict CTestIAPDialog::doTestStepL()
	{


 	// When bootstrapping C32 we have to avoid the PhBkSyncServer being started, since
 	// it needs a different CommDB
 	_LIT(KPhbkSyncCMI, "phbsync.cmi");
    TInt ret = StartC32WithCMISuppressions(KPhbkSyncCMI);
	if ((ret != KErrNone) && (ret != KErrAlreadyExists))
		INFO_PRINTF2(_L("error is : %d \n"), ret);
	else	
		INFO_PRINTF1(_L("Started C32\n"));

	RGenConAgentDialogServer dlgSv;
	TRAP_IGNORE(dlgSv.Connect());
	INFO_PRINTF1(_L("Connected Dialog Server\n"));

	TRequestStatus status;

	INFO_PRINTF1(_L("\nTesting the IAP Selection Dialog\n")); 
	TUint32 iap=0;
	TConnectionPrefs prefs;
	prefs.iRank = 1;
	prefs.iDirection = ECommDbConnectionDirectionOutgoing;
	prefs.iBearerSet = ECommDbBearerCSD;
	dlgSv.IapConnection(iap,prefs,status);
	INFO_PRINTF1(_L("Requested IAP selection\n"));
	User::WaitForRequest(status);
	ret=status.Int();
	INFO_PRINTF2(_L("Request IAP selection completed with code=%d\n"),ret);
	checkError(dlgSv,ret);
	INFO_PRINTF2(_L("IAP id = %d\n"),iap);

	return TestStepResult();
	}


void CTestIAPDialog::checkError(RGenConAgentDialogServer& aDlgSvr, TInt aError)
	{
	if (aError!=KErrNone && aError!=KErrCancel)
		aDlgSvr.Close();

	if ((aError!=KErrNone) && (aError!=KErrCancel))
		INFO_PRINTF2(_L("Error is : %d \n"), aError);
	}

void CTestIAPDialog::checkPctError(RGenConAgentDialogServer& aDlgSvr, TInt aError)
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


//

CTestAccessPointDialog::~CTestAccessPointDialog()
/**
 * Destructor
 */
	{
	}

CTestAccessPointDialog::CTestAccessPointDialog()
/**
 * Constructor
 */
	{
	SetTestStepName(KTestAccessPointDialog);
	}

TVerdict CTestAccessPointDialog::doTestStepL()
	{
 	// When bootstrapping C32 we have to avoid the PhBkSyncServer being started, since
 	// it needs a different CommDB
 	_LIT(KPhbkSyncCMI, "phbsync.cmi");
    TInt ret = StartC32WithCMISuppressions(KPhbkSyncCMI);
	if ((ret != KErrNone) && (ret != KErrAlreadyExists))
		INFO_PRINTF2(_L("error is : %d \n"), ret);
	else	
		INFO_PRINTF1(_L("Started C32\n"));

	RGenConAgentDialogServer dlgSv;
	TRAP_IGNORE(dlgSv.Connect());
	INFO_PRINTF1(_L("Connected Dialog Server\n"));

	TRequestStatus status;

	INFO_PRINTF1(_L("\nTesting the Access Point Selection Dialog\n")); 
	TUint32 ap=0;
	dlgSv.AccessPointConnection(ap,1,status);
	INFO_PRINTF1(_L("Requested Access Point selection\n"));
	User::WaitForRequest(status);
	ret=status.Int();
	INFO_PRINTF2(_L("Request Access Point selection completed with code=%d\n"),ret);
	checkError(dlgSv,ret);
	INFO_PRINTF2(_L("Access Point id = %d\n"),ap);

	return TestStepResult();
	}


void CTestAccessPointDialog::checkError(RGenConAgentDialogServer& aDlgSvr, TInt aError)
	{
	if (aError!=KErrNone && aError!=KErrCancel)
		aDlgSvr.Close();

	if ((aError!=KErrNone) && (aError!=KErrCancel))
		INFO_PRINTF2(_L("Error is : %d \n"), aError);
	}

void CTestAccessPointDialog::checkPctError(RGenConAgentDialogServer& aDlgSvr, TInt aError)
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




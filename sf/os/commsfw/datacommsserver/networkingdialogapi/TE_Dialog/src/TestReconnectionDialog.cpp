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
 @file TestReconnectionDialog.cpp
*/

//Symbian OS Headers
#include <c32comm.h>
#include <agentdialog.h>
#include <e32cons.h>

//Test Framework Header
#include <test/testexecutelog.h>

//Test Step Header
#include "TestReconnectionDialog.h"



CTestReconnectionDialog::~CTestReconnectionDialog()
/**
 * Destructor
 */
	{
	}

CTestReconnectionDialog::CTestReconnectionDialog()
/**
 * Constructor
 */
	{

	SetTestStepName(KTestReconnectionDialog);
	}


TVerdict CTestReconnectionDialog::doTestStepL()
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

	TConnectionPrefs prefs;
	_LIT(KIapName,"New IAP");
	
	TBuf<16> iapName(KIapName);
	TBool response = EFalse;
	dlgSv.WarnNewIapConnection(prefs,KErrBadName,iapName,response,status);
	User::WaitForRequest(status);

	INFO_PRINTF1(_L("\nTesting the Reconnection Dialog\n")); 
	TBool b=ETrue;
	dlgSv.Reconnect(b,status);
	User::WaitForRequest(status);
	ret=status.Int();
	checkError(dlgSv,ret);
	if (b)
		INFO_PRINTF1(_L("Reconnect Required\n"));
	else
		INFO_PRINTF1(_L("Reconnect Not Required\n"));
	
	return TestStepResult();
	}

void CTestReconnectionDialog::checkError(RGenConAgentDialogServer& aDlgSvr, TInt aError)
	{
	if (aError!=KErrNone && aError!=KErrCancel)
		aDlgSvr.Close();

	if ((aError!=KErrNone) && (aError!=KErrCancel))
		INFO_PRINTF2(_L("Error is : %d \n"), aError);
	
	}

void CTestReconnectionDialog::checkPctError(RGenConAgentDialogServer& aDlgSvr, TInt aError)
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



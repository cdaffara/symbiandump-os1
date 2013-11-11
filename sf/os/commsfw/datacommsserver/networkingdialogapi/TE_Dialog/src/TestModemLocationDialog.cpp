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
 @file TestModemLocationDialog.cpp
*/

//Symbian OS Headers
#include <c32comm.h>
#include <agentdialog.h>

//Test Framework Header
#include <test/testexecutelog.h>

//Test Step Header
#include "TestModemLocationDialog.h"


CTestModemLocationDialog::~CTestModemLocationDialog()
/**
 * Destructor
 */
	{
	}

CTestModemLocationDialog::CTestModemLocationDialog()
/**
 * Constructor
 */
	{
	SetTestStepName(KTestModemLocationDialog);
	}


TVerdict CTestModemLocationDialog::doTestStepL()
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
	
	INFO_PRINTF1(_L("\nTesting the Modem And Location Selection Dialog\n")); 
	TUint32 modemId=0;
	TUint32 locationId=0;
	dlgSv.ModemAndLocationSelection(modemId,locationId,status);
	INFO_PRINTF1(_L("Requested Modem And Location selection\n"));
	User::WaitForRequest(status);
	ret=status.Int();
	INFO_PRINTF2(_L("Request Location selection completed with code=%d\n"),ret);
	checkError(dlgSv,ret);
	INFO_PRINTF2(_L("Modem id = %d\n"),modemId);
	INFO_PRINTF2(_L("Location id = %d\n"),locationId);
		
	return TestStepResult();
	}


void CTestModemLocationDialog::checkError(RGenConAgentDialogServer& aDlgSvr, TInt aError)
	{
	if (aError!=KErrNone && aError!=KErrCancel)
		aDlgSvr.Close();

	if ((aError!=KErrNone) && (aError!=KErrCancel))
		INFO_PRINTF2(_L("Error is : %d \n"), aError);
	}

void CTestModemLocationDialog::checkPctError(RGenConAgentDialogServer& aDlgSvr, TInt aError)
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




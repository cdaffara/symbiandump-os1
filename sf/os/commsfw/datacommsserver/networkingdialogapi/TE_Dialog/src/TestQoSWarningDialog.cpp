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
 @file TestQoSWarningDialog.cpp
*/

//Symbian OS Headers
#include <c32comm.h>
#include <agentdialog.h>

//Test Framework Header
#include <test/testexecutelog.h>

//Test Step Header
#include "TestQoSWarningDialog.h"


CTestQoSWarningDialog::~CTestQoSWarningDialog()
/**
 * Destructor
 */
	{
	}

CTestQoSWarningDialog::CTestQoSWarningDialog()
/**
 * Constructor
 */
	{
	SetTestStepName(KTestQoSWarningDialog);
	}

TVerdict CTestQoSWarningDialog::doTestStepL()
	{

	TInt ret=StartC32(); 
	if ((ret != KErrNone) && (ret != KErrAlreadyExists))
		INFO_PRINTF2(_L("error is : %d \n"),ret);
	else	
		INFO_PRINTF1(_L("Started C32\n"));

	RGenConAgentDialogServer dlgSv;
	TRAP_IGNORE(dlgSv.Connect());
	INFO_PRINTF1(_L("Connected Dialog Server\n"));

	TRequestStatus status;
	
	INFO_PRINTF1(_L("\nTesting the QoS Warning Dialog\n")); 
	TBool response = ETrue;
	dlgSv.QoSWarning(response, status);
	INFO_PRINTF1(_L("Requested QoS warning\n"));
	User::WaitForRequest(status);
	ret=status.Int();
	INFO_PRINTF2(_L("Request QoS warning completed with code=%d\n"),ret);
	checkError(dlgSv,ret);
	if (response)
		INFO_PRINTF1(_L("Disconnect\n"));
	else
		INFO_PRINTF1(_L("Do not disconnect\n"));

	
	return TestStepResult();
	}


void CTestQoSWarningDialog::checkError(RGenConAgentDialogServer& aDlgSvr, TInt aError)
	{
	if (aError!=KErrNone && aError!=KErrCancel)
		aDlgSvr.Close();

	if ((aError!=KErrNone) && (aError!=KErrCancel))
		INFO_PRINTF2(_L("Error is : %d \n"), aError);
	}

void CTestQoSWarningDialog::checkPctError(RGenConAgentDialogServer& aDlgSvr, TInt aError)
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



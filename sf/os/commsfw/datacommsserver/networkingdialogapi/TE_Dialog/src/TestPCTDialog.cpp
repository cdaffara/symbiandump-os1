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
 @file TestPCTDialog.cpp
*/

//Symbian OS Headers
#include <c32comm.h>
#include <agentdialog.h>

//Test Framework Header
#include <test/testexecutelog.h>

//Test Step Header
#include "TestPCTDialog.h"


CTestPCTDialog::~CTestPCTDialog()
/**
 * Destructor
 */
	{
	}

CTestPCTDialog::CTestPCTDialog()
/**
 * Constructor
 */
	{
	SetTestStepName(KTestPCTDialog);
	}


TVerdict CTestPCTDialog::doTestStepL()
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

	INFO_PRINTF1(_L("Testing the PCT\n")); 
	dlgSv.OpenPct();
	TRequestStatus destroyStatus;
	dlgSv.DestroyPctNotification(destroyStatus);
	INFO_PRINTF1(_L("Requested destroy notification\n"));	
	TBuf<256> writeBuffer=_L("Welcome to Ann's ISP");
	dlgSv.WritePct(writeBuffer);
	writeBuffer=_L("Login:");
	dlgSv.WritePct(writeBuffer);
	TBuf<256> readBuffer=_L("UserName");
	dlgSv.ReadPct(readBuffer,status);
	User::WaitForRequest(status);
	ret=status.Int();
	INFO_PRINTF2(_L("Read from PCT completed with code = %d\n"),ret);
	checkPctError(dlgSv,ret);
	INFO_PRINTF2(_L("Read from PCT = %S\n"),&readBuffer);
	writeBuffer=_L("Password:");
	dlgSv.WritePct(writeBuffer);
	readBuffer=_L("PassWord");
	dlgSv.ReadPct(readBuffer,status);
	User::WaitForRequest(status);
	ret=status.Int();
	INFO_PRINTF2(_L("Read from PCT completed with code = %d\n"),ret);
	checkPctError(dlgSv,ret);
	INFO_PRINTF2(_L("Read from PCT = %S\n"),&readBuffer);
	writeBuffer=_L("Login correct!");
	dlgSv.WritePct(writeBuffer);
	User::After(2*1000000);	
	dlgSv.CancelDestroyPctNotification();
	User::WaitForRequest(destroyStatus);
	
	if ((destroyStatus.Int()==KErrCancel))
		INFO_PRINTF2(_L("error is : %d \n"), destroyStatus.Int());

	dlgSv.ClosePct();
	dlgSv.Close();

	return TestStepResult();
	}


void CTestPCTDialog::checkError(RGenConAgentDialogServer& aDlgSvr, TInt aError)
	{
	if (aError!=KErrNone && aError!=KErrCancel)
		aDlgSvr.Close();

	if ((aError!=KErrNone) && (aError!=KErrCancel))
		INFO_PRINTF2(_L("Error is : %d \n"), aError);
	}

void CTestPCTDialog::checkPctError(RGenConAgentDialogServer& aDlgSvr, TInt aError)
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




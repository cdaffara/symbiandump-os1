// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file
 @test
 @internalComponent - Internal Symbian test code 
*/

#include "tcmncustomcmd_step_rendezvouswithestart.h"
#include "ssmcmncustomcmdfactory.h"

// class under test
#include "cmdrendezvouswithestart.h"

CCustomCmdTestRendezvousWithEStart::~CCustomCmdTestRendezvousWithEStart()
	{
	}

CCustomCmdTestRendezvousWithEStart::CCustomCmdTestRendezvousWithEStart()
	{
	SetTestStepName(KTCCustomCmdTestRendezvousWithEStart);
	}


TVerdict CCustomCmdTestRendezvousWithEStart::doTestStepL()
	{
	INFO_PRINTF1(_L("Entering test for rendezvous with estart custom command"));
	__UHEAP_MARK;
	
	TRAPD(err, doTestCreateExecuteAndDestroyL());
	TEST(err == KErrNone);
		
	__UHEAP_MARKEND;
	INFO_PRINTF1(_L("Leaving test for rendezvous with estart custom command"));
	return TestStepResult();
	}

/**
Old Test CaseID 		APPFWK-CUSTCMD-0025
New Test CaseID 		DEVSRVS-SSPLUGINS-CUSTCMD-0025
 */

void CCustomCmdTestRendezvousWithEStart::doTestCreateExecuteAndDestroyL()
	{
	INFO_PRINTF1(_L("Entering test for factory create, execute cancel and destroy"));
	__UHEAP_MARK;
	
	INFO_PRINTF1(_L("Testing NewL for rendezvous with estart"));
	MSsmCustomCommand* customCommand = SsmCmnCustomCmdFactory::CmdRendezvousWithEStartL();
	CleanupStack::PushL(customCommand);
	
	INFO_PRINTF1(_L("Testing Initialize for rendezvous with estart"));
	TInt err = customCommand->Initialize(NULL);
	TEST(err == KErrNone);
	
	INFO_PRINTF1(_L("Testing Execute for rendezvous with estart"));
	TBuf8<1> nullBuf;
	TRequestStatus status;
	// Subscribe to rendezvous requests
	TRequestStatus rendezvousStatus;
	RProcess thisProcess;
	thisProcess.Rendezvous(rendezvousStatus);
	TEST(rendezvousStatus == KRequestPending);
	// Issue execute
	customCommand->Execute(nullBuf, status);
	TEST(status == KErrNone);
	INFO_PRINTF1(_L("Testing rendezvous occured"));
	TEST(rendezvousStatus == KErrNone);

	INFO_PRINTF1(_L("Testing ExecuteCancel for rendezvous with estart"));
	customCommand->ExecuteCancel();
	TEST(status == KErrNone);
	
	INFO_PRINTF1(_L("Cleaning up custom command"));
	customCommand->Close();
	customCommand->Release();
	CleanupStack::Pop(customCommand);
	customCommand = NULL;
	
	__UHEAP_MARKEND;
	INFO_PRINTF1(_L("Leaving test for factor create, execute cancel and destroy"));
	}


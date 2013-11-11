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


#include "ssmdebug.h"
#include "ssmpanic.h"

#include "tcle_step_rclesessionfail.h"

CCleRCleSessionFailTest::~CCleRCleSessionFailTest()
	{
	}

CCleRCleSessionFailTest::CCleRCleSessionFailTest()
	{
	SetTestStepName(KTCleRCleSessionFailStep);
	}

TVerdict CCleRCleSessionFailTest::doTestStepPreambleL()
	{
	return CTestStep::doTestStepPreambleL();
	}

TVerdict CCleRCleSessionFailTest::doTestStepPostambleL()
	{
	return CTestStep::doTestStepPostambleL();
	}
/**
Old Test CaseID 		APPFWK-CLE-0010
New Test CaseID 		DEVSRVS-SSMA-CLE-0010
 */
TVerdict CCleRCleSessionFailTest::doTestStepL()
	{
	INFO_PRINTF1(_L("CCleRCleSessionFailTest started....."));
	__UHEAP_MARK;
	
	TRAPD(err, iCleCli.ConnectL(KCleSrver));
	TEST(err == KErrPermissionDenied);
	INFO_PRINTF2(_L("Connecting client to server completed with code '%d'."), err);

	iCleCli.Close();
	__UHEAP_MARKEND;
	INFO_PRINTF1(_L("....CCleRCleSessionFailTest finished!!!"));
	
	return TestStepResult();
	}
	

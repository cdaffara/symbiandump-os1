// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
@internalComponent
*/

#include "t_wdptest.h"

namespace t_wservwdptest
	{
	_LIT(KT_WDPTestId,"testid");
	_LIT(KT_TestPanicTxt, "t_wservwdptest");
	}

using namespace t_wservwdptest;

CT_WDPTest::CT_WDPTest()
	{
	SetTestStepName(KT_WDPTestStep);
	}


CT_WDPTest::~CT_WDPTest()
	{
	}


enum TVerdict CT_WDPTest::doTestStepPreambleL()
	{
	TVerdict ret=CTestStep::doTestStepPreambleL();

	if ( !GetIntFromConfig( ConfigSection(), KT_WDPTestId, iTestId ) )
		{
		User::Leave(KErrNotFound);
		}
	return ret;
	}

enum TVerdict CT_WDPTest::doTestStepPostambleL()
	{
	return TestStepResult();
	}


/**
Starts test step
@internalComponent
@return TVerdict pass / fail
@pre N/A
@post N/A
*/
enum TVerdict CT_WDPTest::doTestStepL()
	{
	switch( iTestId )
		{
		case 1:
			TEST(!WServDefaultDataPagedL());
			break;
		default:
			User::Panic(KT_TestPanicTxt, KErrNotFound);  //Unexpected value!
			break;
		}
	return TestStepResult();
	}

TBool CT_WDPTest::WServDefaultDataPagedL()
	{
	RProcess proc = RProcess();
	_LIT(KPattern, "*EwSrv*");
	TFindProcess findProc(KPattern);
	TFullName fullName;
	TInt err=KErrNone;
		
	INFO_PRINTF2(_L("TFindProcess: Find a process whose name match the pattern %S "), &KPattern());
	if( (err=findProc.Next(fullName))!= KErrNone )
		{
		INFO_PRINTF1(_L("Error: Process whose name match the above pattern not found"));
		User::LeaveIfError(err);
		}
	else
		{
		INFO_PRINTF2(_L("TFindProcess.Next() found process --> %S "), &fullName );
		}
	
	if (findProc.Next(fullName)!= KErrNotFound)
		{
		INFO_PRINTF2(_L("Error: TFindProcess found more than 1 process matching the pattern --> %S "), &fullName );
		User::Leave(KErrGeneral);		
		}
	
	if( (err=proc.Open(fullName)) != KErrNone)
		{
		INFO_PRINTF2(_L("Error: RProcess.Open() returned --> %d"), err);
		User::LeaveIfError(err);
		}
	
	TBool ret = proc.DefaultDataPaged();
	INFO_PRINTF2(_L("wserv.DefaultDataPaged() returned --> %d"), ret);
	proc.Close();	 
	return ret;	
	}

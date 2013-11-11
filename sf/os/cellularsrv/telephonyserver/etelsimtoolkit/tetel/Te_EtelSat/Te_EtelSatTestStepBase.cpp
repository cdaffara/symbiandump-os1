// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Te_EtelSatSuiteStepBase.cpp
// 
//

#include "Te_EtelSatTestStepBase.h"

TVerdict CRSatTestStep::doTestStepPreambleL()
	{
	__UHEAP_MARK;
	TInt ret = iTelServer.Connect();
	if (ret!=KErrNone)
		{
		INFO_PRINTF1(_L("Failed connect phone"));
		User::Leave(ret);
		}
   	ret=iTelServer.LoadPhoneModule(DSATTSY_MODULE_NAME);
	if (ret!=KErrNone)
		{
		INFO_PRINTF1(_L("Failed load phone module"));
		iTelServer.Close();
		User::Leave(ret);
		}
	
	iTestScheduler=new (ELeave) CActiveScheduler;
	CActiveScheduler::Install(iTestScheduler);

	return TestStepResult();
	}

TVerdict CRSatTestStep::doTestStepPostambleL()
	{
	delete iTestScheduler;
	iTelServer.UnloadPhoneModule(DSATTSY_MODULE_NAME);
	iTelServer.Close();

	__UHEAP_MARKEND;
	return TestStepResult();
	}

RTestSat::RTestSat()
	{
	}

//Requests service of IPC 1000055 - a standard custom IPC which may or may not be supported
//by the TSY
TInt RTestSat::TestCustomIPC1()
	{
	return Blank(ETestCustomIPC1);
	}

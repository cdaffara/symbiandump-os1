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

#include "teststepbase.h"

CBluetoothUtilsTestStepBase::~CBluetoothUtilsTestStepBase()
	{
	}

CBluetoothUtilsTestStepBase::CBluetoothUtilsTestStepBase(CBluetoothUtilsTestServer& aParent) 
: iParent(aParent)
	{
	}

/**
@pre 	N.A.
@return Test verdict
@post	An active scheduler is installed if none is presented
*/
TVerdict CBluetoothUtilsTestStepBase::doTestStepPreambleL()
	{
	
	INFO_PRINTF1(_L("&gt;&gt;CBluetoothUtilsTestStepBase::doTestStepPreambleL()"));

	if( !CActiveScheduler::Current() )
		{
		INFO_PRINTF1(_L("Install Active Scheduler into TEF Test Step Thread"));
		CActiveScheduler::Install( iParent.iScheduler );
		}
	
	SetTestStepResult(EPass);
	
	INFO_PRINTF1(_L("&lt;&lt;CBluetoothUtilsTestStepBase::doTestStepPreambleL()"));
	return TestStepResult();

	}

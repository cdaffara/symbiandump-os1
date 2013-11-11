// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// @file ctlbsstep_installscheduler.cpp
// This is the class implementation for the Install Active Scheduler Test Step
// 
//

/**
 @file
 @internalTechnology
 @test
*/

#include "ctlbsstepinstallscheduler.h"

CT_LbsStep_InstallScheduler::CT_LbsStep_InstallScheduler(CT_LbsServer& aParent) : CT_LbsStep(aParent)
	{
	SetTestStepName(KLbsStep_InstallScheduler);
	}

/**
Static Constructor
*/
CT_LbsStep_InstallScheduler* CT_LbsStep_InstallScheduler::New(CT_LbsServer& aParent)
	{
	return new CT_LbsStep_InstallScheduler(aParent);
	// Note the lack of ELeave.
	// This means that having insufficient memory will return NULL;
	}

/**
@pre 	No Active Scheduler installed in the worker test thread
@return Test verdict
@post	Test server's Active Scheduler member installed in worker thread
*/
TVerdict CT_LbsStep_InstallScheduler::doTestStepL()
	{
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsStep_InstallScheduler::doTestStepL()"));

	if( CActiveScheduler::Current() )
		{
		INFO_PRINTF1(_L("Test Warning: Scheduler already installed in worker thread"));

		return TestStepResult();
		}
	CActiveScheduler::Install( iParent.iScheduler );
	
	INFO_PRINTF1(_L("&lt;&lt;CT_LbsStep_InstallScheduler::doTestStepL()"));

	return TestStepResult();
	}


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
// @file ctlbsstepresetdefaultadmin.cpp
// This is the class implementation for the Reset Default Admin Test Step
// 
//

/**
 @file
 @internalTechnology
 @test
*/

#include <centralrepository.h>		  
#include "ctlbsstepresetdefaultadmin.h"
#include <lbs/lbsadmin.h>

CT_LbsStep_ResetDefaultAdmin::CT_LbsStep_ResetDefaultAdmin(CT_LbsServer& aParent) : CT_LbsStep(aParent)
	{
	SetTestStepName(KLbsStep_ResetDefaultAdmin);
	}

/**
Static Constructor
*/
CT_LbsStep_ResetDefaultAdmin* CT_LbsStep_ResetDefaultAdmin::New(CT_LbsServer& aParent)
	{
	return new CT_LbsStep_ResetDefaultAdmin(aParent);
	// Note the lack of ELeave.
	// This means that having insufficient memory will return NULL;
	}


/**
@pre 	
@return Test verdict
@post	All admin settings reset to default
*/
TVerdict CT_LbsStep_ResetDefaultAdmin::doTestStepL()
	{
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsStep_ResetDefaultAdmin::doTestStepL()"));

	CLbsAdmin* adminApi = CLbsAdmin::NewL();
	CleanupStack::PushL(adminApi);
	
	TInt err = adminApi->ResetToDefault();	

	if(!KErrNone == err)
		{
		SetTestStepResult(EFail);
		}
		
	// Reset the LbsRoot cenrep also.
	const TUid KLbsRootRepositoryUid = { 0x10282266 };
	CRepository* rootRep = CRepository::NewLC(KLbsRootRepositoryUid);
	err = rootRep->Reset();

	if(!KErrNone == err)
		{
		SetTestStepResult(EFail);
		}
	
	INFO_PRINTF1(_L("&lt;&lt;CT_LbsStep_ResetDefaultAdmin::doTestStepL()"));

	CleanupStack::PopAndDestroy(2, adminApi); 	

	// Alow the change to be propagated
	User::After(4000000);

	return TestStepResult();
	}


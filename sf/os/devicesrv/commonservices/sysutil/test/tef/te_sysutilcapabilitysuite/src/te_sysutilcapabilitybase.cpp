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
// file te_sysutilcapabilitybase.h
// 
//

#include <bafl/sysutil.h>
#include "te_sysutilcapabilitybase.h"
#include "sysutilfunctions.h"

CTeSysUtilSuiteStepBase::~CTeSysUtilSuiteStepBase()
	{
	}

CTeSysUtilSuiteStepBase::CTeSysUtilSuiteStepBase()
	{
	}

TVerdict CTeSysUtilSuiteStepBase::doTestStepPreambleL()
	{
	TInt err = DeletePSProperties();
	if(err == KErrNone)
		{
		INFO_PRINTF1(_L("P&S properties successfully deleted."));
		SetTestStepResult(EPass);
		}
	else
		{
		INFO_PRINTF2(_L("P&S properties could not be deleted. Error = %d"), err);
		SetTestStepResult(EAbort);
		}
	return TestStepResult();
	}

TVerdict CTeSysUtilSuiteStepBase::doTestStepPostambleL()
	{
	return TestStepResult();
	}

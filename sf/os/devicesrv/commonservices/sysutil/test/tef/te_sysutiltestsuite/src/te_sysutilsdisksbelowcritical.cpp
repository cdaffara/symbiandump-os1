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
// Examine the space left on the c: drive
// 
//

/**
 @file 
 @internalTechnology
*/

#include "te_sysutilsdisksbelowcritical.h"

/**
Destructor
 */
CSysUtilsDisksBelowCriticalStep::~CSysUtilsDisksBelowCriticalStep()
	{
	}

/**
Constructor
 */
CSysUtilsDisksBelowCriticalStep::CSysUtilsDisksBelowCriticalStep()
	{
	SetTestStepName(KSysUtilsDisksBelowCriticalStep);
	}

/**
@SYMTestCaseID		SYSLIB-BAFL-CT-4007    
@SYMREQ	    		REQ1872 			
@SYMTestCaseDesc    	DisksBelowCritical
@SYMTestPriority    	HIGH
@SYMTestStatus     	Implemented
@SYMTestActions    	Trap any errors that may occur when the space remaining on drive c: 
@SYMTestActions    	is checked using SysUtil::DiskSpaceBelowCriticalLevelL and print out if the
@SYMTestActions    	space remaining is above or below the critical limit 
@SYMTestActions    	If no errors occur repeat with drive z: 
@SYMTestExpectedResults The method returns whether the space remaining  
@SYMTestExpectedResults	is above and below the critical level without error
*/

TVerdict CSysUtilsDisksBelowCriticalStep::doTestStepL()
	{
	TInt err=KErrNone;
	TBool status = EFalse;
	
	INFO_PRINTF1(_L("Checking drive C"));
	TRAP(err, status = SysUtil::DiskSpaceBelowCriticalLevelL( NULL, 0, EDriveC ) );
	ProcessResult(status, err);
	
	INFO_PRINTF1(_L("Checking drive Z"));
	TRAP(err, status = SysUtil::DiskSpaceBelowCriticalLevelL( NULL, 0, EDriveZ) );
	ProcessResult(status, err);
	return TestStepResult();
	}

void CSysUtilsDisksBelowCriticalStep::ProcessResult(TBool aResult, TInt aErr)
	{
	 if ( aErr == KErrNone )
		{
		if ( aResult )
			{
			INFO_PRINTF1(_L("DiskSpaceBelowCriticalLevel = TRUE"));
			}
		else
			{
			INFO_PRINTF1(_L("DiskSpaceBelowCriticalLevel = FALSE"));
			}
		}
	else if ( aErr == KErrNotReady )
		{
		ERR_PRINTF1(_L("Drive contains no media"));
		SetTestStepResult(EFail);
		}
	else if ( aErr == KErrNotSupported )
		{
		ERR_PRINTF1(_L("System does not have this drive"));
		SetTestStepResult(EFail);
		}
	else
		{
		ERR_PRINTF2(_L("Other error %d"), aErr);
		SetTestStepResult(EFail);
		}
	}
	

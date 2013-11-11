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
// Examine the space left on the FFS
// 
//

/**
 @file 
 @internalTechnology
*/

#include "te_sysutilsffsspacebelowcriticalrfs.h"

/**
Destructor
 */
CSysUtilsFFSSpaceBelowCriticalRfsStep::~CSysUtilsFFSSpaceBelowCriticalRfsStep()
	{
	}

/**
Constructor
 */
CSysUtilsFFSSpaceBelowCriticalRfsStep::CSysUtilsFFSSpaceBelowCriticalRfsStep()
	{
	SetTestStepName(KSysUtilsFFSSpaceBelowCriticalRfsStep);
	}

/**
@SYMTestCaseID	    SYSLIB-BAFL-CT-4018
@SYMREQ		    	REQ1802  			
@SYMTestCaseDesc    FFSSpaceBelowCriticalRfs
@SYMTestPriority    LOW
@SYMTestStatus      Implemented
@SYMTestActions     Examine if the space in the flash file system is below the critical 
@SYMTestActions     level or not using the SysUtil::FFSSpaceBelowCriticalLevelL method. 
@SYMTestActions     The file server session is not provided as a parameter to the method.
@SYMTestActions     If no error is caught from the first call then call the SysUtil::FFSSpaceBelowCriticalLevelL
@SYMTestActions     method with an unconnected file server session as a parameter.
@SYMTestExpectedResults The first call to the method returns whether the space is below the critical 
@SYMTestExpectedResults level or not without error. The second call causes a panic with kern exec 0
*/

TVerdict CSysUtilsFFSSpaceBelowCriticalRfsStep::doTestStepL()
	{
	TInt err=KErrNone;
	TBool status = EFalse;
	
	TRAP(err, status = SysUtil::FFSSpaceBelowCriticalLevelL( NULL, 0 ) );

	if ( err == KErrNotSupported )
		{
		ERR_PRINTF1(_L("System does not have this drive"));
		SetTestStepResult(EFail);
		}
	else if ( err == KErrNotReady )
		{
		ERR_PRINTF1(_L("Drive contains no media"));
		SetTestStepResult(EFail);
		}
	else if (err != KErrNone)
		{
		ERR_PRINTF2(_L("Err getting critical level %d"),err);
		SetTestStepResult(EFail);
		}
	else 
		{
		if ( status )
			{
			INFO_PRINTF1(_L("FFSSpaceBelowCriticalLevel = TRUE"));
			}
		else
			{
			INFO_PRINTF1(_L("FFSSpaceBelowCriticalLevel = FALSE"));
			}
		}
	
	//This should cause a panic therefore the error can not be checked. 
	TRAP(err, status = SysUtil::FFSSpaceBelowCriticalLevelL(&iFs, 0 ) );
	
	//Test should not get this far if it does it has failed. 
	return EFail;
	}

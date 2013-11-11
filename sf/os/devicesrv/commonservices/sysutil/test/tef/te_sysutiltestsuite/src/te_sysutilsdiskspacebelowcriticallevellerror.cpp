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

#include "te_sysutilsdiskspacebelowcriticallevellerror.h"

/**
Destructor
 */
CSysUtilsDiskSpaceBelowCriticalLevelLErrorStep::~CSysUtilsDiskSpaceBelowCriticalLevelLErrorStep()
	{
	}

/**
Constructor
 */
CSysUtilsDiskSpaceBelowCriticalLevelLErrorStep::CSysUtilsDiskSpaceBelowCriticalLevelLErrorStep()
	{
	SetTestStepName(KSysUtilsDiskSpaceBelowCriticalLevelLErrorStep);
	}

/**
@SYMTestCaseID	  	SYSLIB-BAFL-CT-4020
@SYMREQ  	    	REQ1872  			
@SYMTestCaseDesc    DiskSpaceBelowCriticalLevelLError
@SYMTestPriority    High
@SYMTestStatus      Implemented
@SYMTestActions     Call the SysUtil::DiskSpaceBelowCriticalLevelL method but provide a 
@SYMTestActions		non-existent drive as the aDrive parameter
@SYMTestActions		If KErrBadName is returned call SysUtil::DiskSpaceBelowCriticalLevelL with drive c: 
@SYMTestActions		as a parameter and an unconnected RFs session as a parameter
@SYMTestExpectedResults The method should fail with the KErrBadName error code  The second call causes a panic with kern exec 0
*/

TVerdict CSysUtilsDiskSpaceBelowCriticalLevelLErrorStep::doTestStepL()
	{
	TInt err=KErrNone;
	TInt noDrive = -1;
	
	TRAP(err, SysUtil::DiskSpaceBelowCriticalLevelL( NULL, 0, noDrive) );

	if ( err == KErrBadName )
		{
		INFO_PRINTF1(_L("Error KErrBadName returned this is expected"));			
		}
	else 
		{
		ERR_PRINTF2(_L("Incorrect error code DiskSpaceBelowCriticalLevelL returned = %d"), err );
		SetTestStepResult(EFail);
		}
	
	//This should cause a panic therefore the error can not be checked. 
	TRAP(err, SysUtil::DiskSpaceBelowCriticalLevelL( &iFs, 0, EDriveC) );

	
	//Test should not get this far if it does it has failed. 
	return EFail;
	}

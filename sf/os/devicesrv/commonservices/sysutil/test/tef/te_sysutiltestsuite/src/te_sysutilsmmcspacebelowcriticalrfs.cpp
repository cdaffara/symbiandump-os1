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

#include "te_sysutilsmmcspacebelowcriticalrfs.h"

/**
Destructor
 */
CSysUtilsMMCSpaceBelowCriticalRfsStep::~CSysUtilsMMCSpaceBelowCriticalRfsStep()
	{
	iFs.Close();
	}

/**
Constructor
 */
CSysUtilsMMCSpaceBelowCriticalRfsStep::CSysUtilsMMCSpaceBelowCriticalRfsStep()
	{
	SetTestStepName(KSysUtilsMMCSpaceBelowCriticalRfsStep);
	}

/**
@SYMTestCaseID		SYSLIB-BAFL-CT-4019 
@SYMREQ 	    	REQ1802  			
@SYMTestCaseDesc    MMCSpaceBelowCriticalRfs
@SYMTestPriority    LOW
@SYMTestStatus      Implemented
@SYMTestActions     Examine if the space on the multi media card is below the critical level or 
@SYMTestActions		not using the SysUtil::MMCSpaceBelowCriticalLevelL method. The file server session
@SYMTestActions		is connected and provided as a parameter to the method. 
@SYMTestActions		The second call to the SysUtil::MMCSpaceBelowCriticalLevelL method is with an 
@SYMTestActions		unconnected file server session as a parameter.
@SYMTestExpectedResults The first call to the method returns whether the space is below the critical level or not without error. 
@SYMTestExpectedResults	The second call causes a panic with kern exec 0
*/

TVerdict CSysUtilsMMCSpaceBelowCriticalRfsStep::doTestStepL()
	{
	TInt err=KErrNone;
	TBool status = EFalse;

	TInt errCon = iFs.Connect();
	if (errCon == KErrNone)
		{

		TRAP(err, status = SysUtil::MMCSpaceBelowCriticalLevelL(&iFs, 0 ) );
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
				INFO_PRINTF1(_L("MMCSpaceBelowCriticalLevel = TRUE"));
				}
			else
				{
				INFO_PRINTF1(_L("MMCSpaceBelowCriticalLevel = FALSE"));
				}
			}	
		iFs.Close();
		}
	else
		{
		ERR_PRINTF2(_L("Err connecting the rfs session %d"),err);
		SetTestStepResult(EFail);
		}
	
	//This should cause a panic therefore the error can not be checked. 
	TRAP(err, status = SysUtil::MMCSpaceBelowCriticalLevelL(&iFs, 0 ) );
	
	//Test should not get this far if it does it has failed. 
	return EFail;
	}

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
// Examine the space left on the given disk
// 
//

/**
 @file 
 @internalTechnology
*/

#include "te_syutilmmcdrivelocation.h"

/**
Destructor
*/
CSysUtilsMMCDriveLocationStep::~CSysUtilsMMCDriveLocationStep()
	{
	}

/**
Constructor
 */
CSysUtilsMMCDriveLocationStep::CSysUtilsMMCDriveLocationStep()
	{
	SetTestStepName(KSysUtilsMMCDriveLocationStep);
	}

/**
@SYMTestCaseID		SYSLIB-BAFL-CT-4036   
@SYMREQ  	    	REQ1872  			
@SYMTestCaseDesc    MMCDriveLocation
@SYMTestPriority    HIGH
@SYMTestStatus      Implemented
@SYMTestActions    	The SysUtil::MMCSpaceBelowCriticalLevelL method is called once the following 
@SYMTestActions    	changes have been made to the product code and the product code has been rebuilt. 
@SYMTestActions    	The changes to the product are to be done for each step and the code is then rebuilt 
@SYMTestActions    	and the test is run again the output is checked against the expected results.
@SYMTestActions    	The changes to the drive letter should be changed in the sysutil.iby file.
@SYMTestActions    	1.	KSysUtilDefaultMMCDrive is set to -1.
@SYMTestActions    	2.	KSysUtilDefaultMMCDrive is set to -1 and the test is run without the card inserted in the board
@SYMTestActions    	3.	KSysUtilDefaultMMCDrive is set to EDriveE (4)
@SYMTestActions    	4.	KSysUtilDefaultMMCDrive is set to EDriveC (2) Note that whilst this is nominally the FFS rather 
@SYMTestActions    	than the MMC, SysUtil should simply accept that the given value (EDriveC) is in fact the MMC.  
@SYMTestActions    	It will then make MMC specific calls on the FFS
@SYMTestExpectedResults The following result should occur due to changes to the product code:
@SYMTestExpectedResults 1.	The MMC drive should be found and no error should occur
@SYMTestExpectedResults 2.	The code should leave with KErrNotFound  The test will be displayed as failing and the error 
@SYMTestExpectedResults code will be printed out, this error must be KErrNotFound for the test to be considered as passed.
@SYMTestExpectedResults 3.	The MMC drive should be found and no error should occur
@SYMTestExpectedResults 4.	The MMC drive should be found and no error should occur. The sysutil method assumes that is a valid drive.
*/
TVerdict CSysUtilsMMCDriveLocationStep::doTestStepL()
	{
	TInt err=KErrNone;
	TBool status = EFalse;
	
	TRAP(err, status = SysUtil::MMCSpaceBelowCriticalLevelL( NULL, 0 ) );

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
	else if ( err != KErrNone )
		{
		ERR_PRINTF1(_L("KErrNone expected"));
		ERR_PRINTF2(_L("Error code %d  caught"), err);
		SetTestStepResult(EFail);
		}
	else
		{
		if ( status )
			{
			INFO_PRINTF2(_L("MMCSpaceBelowCriticalLevel = TRUE  %d"), err);
			}
		else
			{
			INFO_PRINTF2(_L("MMCSpaceBelowCriticalLevel = FALSE %d"), err);
			}
		}

	return TestStepResult();
	}

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
// Examine the space left on the z: drive
// 
//

/**
 @file 
 @internalTechnology
*/
#include "te_sysutilsdiskspacenotfoundbelowcritical.h"

/**
Destructor
 */
CSysUtilsDiskSpaceNotFoundBelowCriticalStep::~CSysUtilsDiskSpaceNotFoundBelowCriticalStep()
	{
	iFs.Close();
	}

/**
Constructor
 */
CSysUtilsDiskSpaceNotFoundBelowCriticalStep::CSysUtilsDiskSpaceNotFoundBelowCriticalStep()
	{
	SetTestStepName(KSysUtilsDiskSpaceNotFoundBelowCriticalStep);
	}

/**
@SYMTestCaseID		SYSLIB-BAFL-CT-4017    
@SYMREQ  	    	REQ1872 			
@SYMTestCaseDesc	To ensure that checking for space remaining on a drive that is not ready returns an error
@SYMTestPriority    HIGH
@SYMTestStatus      Implemented
@SYMTestActions  	The test should check that an error is expected when a drive is found that is not available 
@SYMTestActions  	and is checked using the SysUtil::DiskSpaceBelowCriticalLevelL. 
@SYMTestExpectedResults  KErrNotReady is returned because the drive does not exist
*/

TVerdict CSysUtilsDiskSpaceNotFoundBelowCriticalStep::doTestStepL()
	{

	TInt err = iFs.Connect();
	if (err == KErrNone)
		{
		TDriveList driveList;
		err = iFs.DriveList(driveList);
		if (err == KErrNone)
			{
			TInt driveID = 0;
			while ((driveList[driveID] != 0) && (driveID < KMaxDrives))
				{
				++driveID;
				}
	
			if (driveID != KMaxDrives)
				{
				TChar driveChar;
				TBuf16<1> driveBuf;
				iFs.DriveToChar(driveID, driveChar);
				
				driveBuf.Append(driveChar);
				INFO_PRINTF1(_L("Trying drive "));
				INFO_PRINTF1(driveBuf);
				TRAP(err, SysUtil::DiskSpaceBelowCriticalLevelL( NULL, 0, driveID ) );
			
				if ( err == KErrNotReady )
					{
					INFO_PRINTF1(_L("KErrNotReady occured this is expected"));
					}
				else 
					{
					ERR_PRINTF2(_L("Other error caught error code %d"), err);
					SetTestStepResult(EFail);
					}
				}
			else
				{
				ERR_PRINTF1(_L("all drives ready"));
				SetTestStepResult(EFail);
				}
			}

		iFs.Close();
		}
	else
		{
		ERR_PRINTF2(_L("other error connecting rfs %d"), err);
		SetTestStepResult(EFail);
		}
	return TestStepResult();
	}


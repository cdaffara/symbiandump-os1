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

#include "te_sysutilsdiskspacebelowcritical.h"
#include "sysutil.h"
#include "sysutildomaincrkeys.h"    // for disc space

#include "sysutilinternalpskeys.h"  // for disc space

#include "sysutilpatchdata.h"       // patchable data defining the MMC drive integer identifer

/**
 Destructor
 */
CSysUtilsDiskSpaceBelowCriticalStep::~CSysUtilsDiskSpaceBelowCriticalStep()
	{
	iFs.Close();
	}

/**
 Constructor
 */
CSysUtilsDiskSpaceBelowCriticalStep::CSysUtilsDiskSpaceBelowCriticalStep()
	{
	SetTestStepName (KSysUtilsDiskSpaceBelowCriticalStep);
	}



/**
@SYMTestCaseID		SYSLIB-BAFL-CT-4006    
@SYMREQ  	    	REQ1872 			
@SYMTestCaseDesc    DiskSpaceBelowCritical
@SYMTestPriority    HIGH
@SYMTestStatus      Implemented
@SYMTestActions    	The first call to SysUtil::DiskSpaceBelowCriticalLevelL method is made 
@SYMTestActions    	with a request amount parameter of 0. If the first call returns true 
@SYMTestActions    	then a second call should be made to the method with a request amount 
@SYMTestActions    	parameter of a large enough negative value so that the space remaining 
@SYMTestActions    	is above the critical level. If the first call returns false then the 
@SYMTestActions    	second call should be made with a large enough value so that the space 
@SYMTestActions    	remaining is below the critical level. The file server session is 
@SYMTestActions    	connected and provided as a parameter to the method.					 
@SYMTestExpectedResults The method returns whether the space remaining plus the request amount 
@SYMTestExpectedResults	is above and below the critical level without error. The second value should be the opposite 
@SYMTestExpectedResults of the first if not the test fails.
*/
TVerdict CSysUtilsDiskSpaceBelowCriticalStep::doTestStepL()
	{
	TInt err=KErrNone;

	err = iFs.Connect();
	
#ifndef __WINSCW__
	TInt driveToUse = EDriveC;
#else
	TInt driveToUse = EDriveT;
#endif
	if ( err == KErrNone)
		{
		TVolumeInfo vinfo;
		err = iFs.Volume( vinfo, driveToUse);
		if (err == KErrNone)
			{
			TInt threshold = -1;
			if ( vinfo.iDrive.iType == EMediaRam) // RAM drives have different critical level than others
				{
				TRAP(err, threshold = FindValueL (KRamDiskCriticalThreshold, KRamDiskCriticalLevel, KSysUtilRamDiskCriticalThreshold));
				}
			else // Some other media type
				{
				TRAP(err, threshold = FindValueL (KOtherDiskCriticalThreshold, KDiskCriticalThreshold, KSysUtilOtherDiskCriticalThreshold));
				}
			if (err == KErrNone)
				{
				TInt64 freeSpace = vinfo.iFree;
		
				TBool status = EFalse;
				TRAP(err, status = SysUtil::DiskSpaceBelowCriticalLevelL (&iFs, 0, driveToUse));
				if ( err == KErrNotSupported)
					{
					ERR_PRINTF1(_L("System does not have this drive"));
					SetTestStepResult (EFail);
					}
				else if ( err == KErrNotReady)
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
					const TInt below = -1;
					const TInt above = +1;
					TInt toWrite = 0;
					
					if (status)
						{
						toWrite = (freeSpace - threshold) + below;				
						INFO_PRINTF1(_L("DiskSpaceBelowCriticalLevel = TRUE"));
						}
					else
						{
						toWrite = (freeSpace - threshold) + above;
						INFO_PRINTF1(_L("DiskSpaceBelowCriticalLevel = FALSE"));
						}
					TBool status2 = false;
					TRAP(err, status2 = SysUtil::DiskSpaceBelowCriticalLevelL (&iFs, toWrite, driveToUse));
					if ( err == KErrNotSupported)
						{
						ERR_PRINTF1(_L("System does not have this drive"));
						SetTestStepResult (EFail);
						}
					else if ( err == KErrNotReady)
						{
						ERR_PRINTF1(_L("Drive contains no media"));
						SetTestStepResult (EFail);
						}
					else if (err != KErrNone)
						{
						ERR_PRINTF2(_L("Error getting critical level %d"),err);
						SetTestStepResult(EFail);
						}
					else 
						{
						if (status ==  status2)
							{
							ERR_PRINTF1(_L("Error the level has not changed"));
							SetTestStepResult(EFail);
							}
						else
							{
							if ( status2)
								{
								INFO_PRINTF1(_L("DiskSpaceBelowCriticalLevel = TRUE"));
								}
							else
								{
								INFO_PRINTF1(_L("DiskSpaceBelowCriticalLevel = FALSE"));
								}
							}
						}
					}
				}
			else
				{
				ERR_PRINTF2(_L("Error getting treshold value %d"), err);
				SetTestStepResult(EFail);
				}
			}
		else
			{
			ERR_PRINTF2(_L("2nd call to DiskSpaceBelowCriticalLevelL returned %d"), err);
			SetTestStepResult(EFail);
			}
		iFs.Close();
		}
	else
		{
		ERR_PRINTF2(_L("1st call to DiskSpaceBelowCriticalLevelL returned %d"), err);
		SetTestStepResult(EFail);
		}

	return TestStepResult ();

	}


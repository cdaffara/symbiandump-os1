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
// Examine the space left on the flash file system
// 
//

/**
 @file 
 @internalTechnology
*/

#include "te_sysutilsffsspacebelowcritical.h"
#include "sysutil.h"
#include "sysutildomaincrkeys.h"    // for disc space
#include "sysutilinternalpskeys.h"  // for disc space
#include "sysutilpatchdata.h"       // patchable data defining the MMC drive integer identifer

/**
 Destructor
 */
CSysUtilsFFSSpaceBelowCriticalStep::~CSysUtilsFFSSpaceBelowCriticalStep()
	{
	iFs.Close();
	}

/**
 Constructor
 */
CSysUtilsFFSSpaceBelowCriticalStep::CSysUtilsFFSSpaceBelowCriticalStep()
	{
	SetTestStepName (KSysUtilsFFSSpaceBelowCriticalStep);
	}

/**
@SYMTestCaseID		SYSLIB-BAFL-CT-4004    
@SYMREQ  	    	REQ1872 			
@SYMTestCaseDesc    FFSSpaceBelowCritical
@SYMTestPriority    HIGH
@SYMTestStatus      Implemented
@SYMTestActions    	The first call to SysUtil::FFSSpaceBelowCriticalLevelL method is made 
@SYMTestActions    	with a request amount parameter of 0. If the first call returns true 
@SYMTestActions    	then a second call should be made to the method with a request amount 
@SYMTestActions    	parameter of a large enough negative value so that the space remaining 
@SYMTestActions    	is above the critical level. If the first call returns false then the 
@SYMTestActions    	second call should be made with a large enough value so that the space 
@SYMTestActions    	remaining is below the critical level. The file server session is 
@SYMTestActions    	connected and provided as a parameter to the method.					 
@SYMTestExpectedResults The method returns whether the space remaining plus the request amount 
@SYMTestExpectedResults	is above and below the critical level without error, The second value should be the opposite 
@SYMTestExpectedResults of the first if not the test fails.
*/

TVerdict CSysUtilsFFSSpaceBelowCriticalStep::doTestStepL()
	{
#ifndef __WINSCW__
	
	TInt err = iFs.Connect();
	if (err == KErrNone)
		{
		TInt ffsDrive = KSysUtilDefaultFFSDrive;
		if (!(ffsDrive >= EDriveA && ffsDrive <= EDriveZ))
			{
			ffsDrive = iFs.GetSystemDrive();
			}
		if ((ffsDrive >= EDriveA && ffsDrive <= EDriveZ))
			{
			TVolumeInfo vinfo;
			err= iFs.Volume( vinfo, ffsDrive);
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
					
					TRAP(err, status = SysUtil::FFSSpaceBelowCriticalLevelL( &iFs, 0 ));
					if (err == KErrNone)
						{
						
						TInt64 toWrite = 0;
						
						const TInt below = -1;
						const TInt above = +1;
						
						if ( status )
							{
							toWrite = (freeSpace - threshold) + below;
							INFO_PRINTF1(_L("FFSSpaceBelowCriticalLevel = TRUE"));
							}
						else
							{
							toWrite = (freeSpace - threshold) + above;
							INFO_PRINTF1(_L("FFSSpaceBelowCriticalLevel = FALSE"));
							}
		
						TBool status2 = EFalse; 
						TRAP(err, status2 = SysUtil::FFSSpaceBelowCriticalLevelL( &iFs, TInt(toWrite) ));
						
						if (err == KErrNone)
							{
							if (status ==  status2)
								{
								ERR_PRINTF1(_L("Error the level has not changed"));
								SetTestStepResult(EFail);
								}
							else
								{
								if ( status2 )
									{
									INFO_PRINTF1(_L("FFSpaceBelowCriticalLevel = TRUE"));
									}
								else
									{
									INFO_PRINTF1(_L("FFSpaceBelowCriticalLevel = FALSE"));
									}
								}
							}
						else
							{
							ERR_PRINTF2(_L("2nd call to FFSSpaceBelowCriticalLevelL returned %d"), err);
							SetTestStepResult(EFail);
							}
						}
					else
						{
						ERR_PRINTF2(_L("1st call to FFSSpaceBelowCriticalLevelL returned %d"), err);
						SetTestStepResult(EFail);
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
				ERR_PRINTF2(_L("Error getting volume information %d"), err);
				SetTestStepResult(EFail);
				}
			}
		else
			{
			ERR_PRINTF1(_L("error finding the drive "));
			SetTestStepResult(EFail);
			}
		iFs.Close();
		}
	else
		{
		ERR_PRINTF2(_L ("error connecting rfs err code %d"), err);
		SetTestStepResult (EFail);
		}
#else
	INFO_PRINTF1(_L("FFSpaceBelowCriticalLevel test not running on the emulator"));
#endif
	
	return TestStepResult();
	}



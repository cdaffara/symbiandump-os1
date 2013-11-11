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
// Examine the space left on the MMC Card
// 
//

/**
 @file 
 @internalTechnology
*/

#include "te_sysutilsmmcspacebelowcritical.h"
#include <bsul/ccacheddriveinfo.h>
#include "sysutildomaincrkeys.h"    // for disc space

#include "sysutilinternalpskeys.h"  // for disc space

#include "sysutilpatchdata.h"       // patchable data defining the MMC drive integer identifer


/**
 Destructor
 */
CSysUtilsMMCSpaceBelowCriticalStep::~CSysUtilsMMCSpaceBelowCriticalStep()
	{
	}

/**
 Constructor
 */
CSysUtilsMMCSpaceBelowCriticalStep::CSysUtilsMMCSpaceBelowCriticalStep()
	{
	SetTestStepName (KSysUtilsMMCSpaceBelowCriticalStep);
	}

/**
@SYMTestCaseID		SYSLIB-BAFL-CT-4005    
@SYMREQ	    		REQ1872 			
@SYMTestCaseDesc    MMCSpaceBelowCritical
@SYMTestPriority    HIGH
@SYMTestStatus      Implemented
@SYMTestActions    	The first call to SysUtil::MMCSpaceBelowCriticalLevelL method is made 
@SYMTestActions    	with a request amount parameter of 0. If the first call returns true 
@SYMTestActions    	then a second call should be made to the method with a request amount 
@SYMTestActions    	parameter of a large enough negative value so that the space remaining 
@SYMTestActions    	is above the critical level. If the first call returns false then the 
@SYMTestActions    	second call should be made with a large enough value so that the space 
@SYMTestActions    	remaining is below the critical level. The file server session is 
@SYMTestActions    	connected and provided as a parameter to the method.					 
@SYMTestExpectedResults The method returns whether the space remaining plus the request amount 
@SYMTestExpectedResults is above and below the critical level without error
*/
TVerdict CSysUtilsMMCSpaceBelowCriticalStep::doTestStepL()
	{
	TInt err = iFs.Connect ();
	if ( err == KErrNone)
		{
		TInt mmcDrive = KSysUtilDefaultMMCDrive;
		// If a valid mmcDrive is not currently found in patchable data, then search for one.        
		if ( !(mmcDrive >= EDriveA && mmcDrive <= EDriveZ))
			{
			// If mmcDrive is not set in patchable data, then fetch the MMC drive from BSUL.
			BSUL::CCachedDriveInfo *cachedDriveInfo = BSUL::CCachedDriveInfo::NewLC (iFs);
			// This will find valid MMC drive, leave if none available
			TRAP(err, mmcDrive = cachedDriveInfo->GetDefaultRemovableMemoryCardDriveL ());
			CleanupStack::PopAndDestroy (cachedDriveInfo);
			}
		
		if ((mmcDrive >= EDriveA && mmcDrive <= EDriveZ) && (err == KErrNone ))
			{
		
			TVolumeInfo vinfo;
			// This may leave e.g. KErrNotReady if no drive
			err = iFs.Volume ( vinfo, mmcDrive);
			if ( err == KErrNone)
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
					TRAP(err, status = SysUtil::MMCSpaceBelowCriticalLevelL( NULL, 0 ) );
					TInt toWrite = 0;
					if (err == KErrNone)
						{
						const TInt below = -1;
						const TInt above = +1;
						
						if ( status)
							{
							toWrite = (freeSpace - threshold) + below;
							INFO_PRINTF1 (_L("MMCSpaceBelowCriticalLevel = TRUE"));
							}
						else
							{
							toWrite = (freeSpace - threshold) + above;
							INFO_PRINTF1 (_L("MMCSpaceBelowCriticalLevel = FALSE"));
							}
						TBool status2 = EFalse;
	
						TRAP(err, status2 = SysUtil::MMCSpaceBelowCriticalLevelL( NULL, toWrite ) );
	
						if (err == KErrNone)
							{
							
							if (status ==  status2)
								{
								ERR_PRINTF1(_L("Error the level has not changed"));
								SetTestStepResult (EFail);
								}
							else
								{
								if ( status2)
									{
									INFO_PRINTF1 (_L("MMCSpaceBelowCriticalLevel = TRUE"));
									}
								else
									{
									INFO_PRINTF1 (_L("MMCSpaceBelowCriticalLevel = FALSE"));
									}
								}
							}
						else
							{
							ERR_PRINTF2(_L("2nd call to MMCSpaceBelowCriticalLevelL returned %d"), err);
							SetTestStepResult (EFail);
							}
						}
					else
						{
						ERR_PRINTF2(_L("1st call to MMCSpaceBelowCriticalLevelL returned %d"), err);
						SetTestStepResult (EFail);

						}
					}
				else
					{
					ERR_PRINTF2(_L("error finding the threshold level %d"), err);
					SetTestStepResult (EFail);
					}				
				}
			else
				{
				ERR_PRINTF1(_L("Could not get the volume information"));
				SetTestStepResult(EFail);				
				}
			}
		else
			{
			ERR_PRINTF1(_L("Could not find the mmc drive"));
			SetTestStepResult(EFail);
			
			}
		iFs.Close ();
		}
	else
		{
		ERR_PRINTF2(_L ("error connecting rfs err code %d"), err);
		SetTestStepResult (EFail);
		}

	return TestStepResult ();

	}

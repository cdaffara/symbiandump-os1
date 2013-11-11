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
//

/**
 @file 
 @internalTechnology
*/
#include "sysutildomaincrkeys.h"    // for disc space

#include "sysutilinternalpskeys.h"  // for disc space

#include "sysutilpatchdata.h"       // patchable data defining the MMC drive integer identifer

#include "te_sysutilsfssspacebelowcriticalquery.h"

/**
Destructor
 */
CSysUtilsFssSpaceBelowCriticalQueryStep::~CSysUtilsFssSpaceBelowCriticalQueryStep()
	{
	}

/**
Constructor
 */
CSysUtilsFssSpaceBelowCriticalQueryStep::CSysUtilsFssSpaceBelowCriticalQueryStep()
	{
	SetTestStepName(KSysUtilsFssSpaceBelowCriticalQueryStep);
	}

/**
@SYMTestCaseID		BASESRVCS-BAFL-CT-4081  
@SYMREQ  	    	INC130430 			
@SYMTestCaseDesc	To ckeck the disk space,SysUtil API functions 
					DiskSpaceBelowCriticalLevelL() and MMCSpaceBelowCriticalLevelL()
 					is used, which take argument TInt aBytesToWrite and tell if the disk
 				 	space will become below critical level after writing 
 				 	aBytesToWrite data to disk.
					Since the data type of the argument is Tint, the maximum positive
					value that it can accommodate is 2147483648. But the size of 
					download is 3302636520 that is bigger than what TInt can have.
					So the value becomes -992330776 when assigned to aBytesToWrite. 
					Since this value is smaller than available disk space, the SysUtil 
					functions do not complain. Thus, the evaluation is incorrect.
					Similar scenario is foreseen for FssSpaceBelowCriticalLevelL
@SYMTestPriority    HIGH
@SYMTestStatus      Implemented
@SYMTestActions  	checks SysUtil::FssSpaceBelowCriticalLevelL with extreamly large aByteToWrite values
*/

TVerdict CSysUtilsFssSpaceBelowCriticalQueryStep::doTestStepL()
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
					// one extreamly large number
					//0xFFFFFFFFFFFF = 281474976710655 = 2^48-1 = ~ 281 Tera Byte
					TInt64 bytesToWrite = 0xFFFFFFFFFFFF;
					// bytesToWrite = Max(bytesToWrite,freeSpace);
					bytesToWrite = (bytesToWrite > freeSpace) ? bytesToWrite : freeSpace;
					TBool status = EFalse;
					TRAP(err, status = SysUtil::FFSSpaceBelowCriticalLevelL (&iFs, bytesToWrite));
					if (err != KErrNone)
						{
						ERR_PRINTF2(_L("Err getting critical level %d"),err);
						SetTestStepResult(EFail);
						}
					else 
						{
						// calculate the actual disk space criticality
						// this is supposed to be done by the 
						// API FFSSpaceBelowCriticalLevelL
						// if the calculated values are diffirent the API has 
						// acted inconsistently (since the calculation done here 
						// cannot go wrong )
						if(((freeSpace - bytesToWrite ) <= threshold)^status)
							{
							// ETrue // values mismatch
							ERR_PRINTF2(_L("Err querying critical level, inconsistent return value! %d"),err);
							SetTestStepResult(EFail);
							}
						else
							{
							// EFalse // values match
							INFO_PRINTF1(_L("FFSSpaceBelowCriticalLevelL = PASS"));
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

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

#include "te_sysutilsdiskspacebelowcriticalquery.h"

/**
Destructor
 */
CSysUtilsDiskSpaceBelowCriticalQueryStep::~CSysUtilsDiskSpaceBelowCriticalQueryStep()
	{
	}

/**
Constructor
 */
CSysUtilsDiskSpaceBelowCriticalQueryStep::CSysUtilsDiskSpaceBelowCriticalQueryStep()
	{
	SetTestStepName(KSysUtilsDiskSpaceBelowCriticalQueryStep);
	}

/**
@SYMTestCaseID		BASESRVCS-BAFL-CT-4079   
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
					functions do not complain. Thus, the APIs DiskSpaceBelowCriticalLevelL,
					MMCSpaceBelowCriticalLevelL and FSSSpaceBelowCriticalLevelL, return inconsistent
					criticality values
@SYMTestPriority    HIGH
@SYMTestStatus      Implemented
@SYMTestActions  	checks SysUtil::DiskSpaceBelowCriticalLevelL with extreamly large aByteToWrite values
*/


TVerdict CSysUtilsDiskSpaceBelowCriticalQueryStep::doTestStepL()
	{
	
#ifndef __WINSCW__
	TInt driveToUse = EDriveC;
#else
	TInt driveToUse = EDriveT;
#endif
	TInt err = iFs.Connect();
	if (err == KErrNone)
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
				// one extreamly large number
				//0xFFFFFFFFFFFF = 281474976710655 = 2^48-1 = ~ 281 Tera Byte
				TInt64 bytesToWrite = 0xFFFFFFFFFFFF;
				// bytesToWrite = Max(bytesToWrite,freeSpace);
				bytesToWrite = (bytesToWrite > freeSpace) ? bytesToWrite : freeSpace;
				TBool status = EFalse;
				TRAP(err, status = SysUtil::DiskSpaceBelowCriticalLevelL (&iFs, bytesToWrite, driveToUse));
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
					// calculate the actual disk space criticality
					// this is supposed to be done by the 
					// API DiskSpaceBelowCriticalLevel
					// if the calculated values are different, the API has 
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
						INFO_PRINTF1(_L("DiskSpaceBelowCriticalLevel = PASS"));
						}
					}
				}
			else
				{
				ERR_PRINTF2(_L("Error getting treshold value %d"), err);
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

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

#include <f32file.h>
#include <bsul/ccacheddriveinfo.h>
#include "sysutildomaincrkeys.h"    // for disc space

#include "sysutilinternalpskeys.h"  // for disc space
#include "sysutilpatchdata.h"
#include "te_syutilfindcriticalthresholdvalues.h"

/**
 Destructor
 */
CSysUtilsFindCriticalThresholdValuesStep::~CSysUtilsFindCriticalThresholdValuesStep()
	{
	iFs.Close();
	}

/**
 Constructor
 */
CSysUtilsFindCriticalThresholdValuesStep::CSysUtilsFindCriticalThresholdValuesStep()
	{
	SetTestStepName (KSysUtilsFindCriticalThresholdValuesStep);
	}

/**
 @SYMTestCaseID		SYSLIB-BAFL-CIT-4037    
 @SYMREQ  	    	REQ1873			
 @SYMTestCaseDesc   FindCriticalThresholdValues
 @SYMTestPriority   HIGH
 @SYMTestStatus     Implemented
 @SYMTestActions    The test needs to be run on a NAND rom for it to pass. 
 @SYMTestActions    The SysUtil::DiskSpaceBelowCriticalLevelL method and the SysUtil::FFSSpaceBelowCriticalLevelL method are called.  
 @SYMTestActions    The rom needs to be built with te_sysutiltestcenrep.iby file (to ensure that the capability exists to set a 
 @SYMTestActions    central repository value for the critical threshold) and not the sysutil.iby (so that there is no ROM build time 
 @SYMTestActions    patching of the critical threshold constant in the built binary). The central repository and the P and S values 
 @SYMTestActions    that define the critical threshold are set within the code as the test cycles through the three cases listed below.
 @SYMTestActions	1.	KSysUtilRamDriveCriticalThreshold is set to -1, KSysUtilOtherDiskCriticalThreshold is set to -1, do not 
 @SYMTestActions    set P & S Keys or repository entries for RAM drive and other disk critical thresholds.  
 @SYMTestActions    2.	KSysUtilRamDriveCriticalThreshold is set to -1, KSysUtilOtherDiskCriticalThreshold is set to -1,  
 @SYMTestActions    a P & S Key for RAM drive critical threshold is set to 51000, a P & S Key for
 @SYMTestActions    other disk critical threshold is set to 131000 and valid repository entries are not set for RAM drive
 @SYMTestActions    and other disk critical thresholds.  
 @SYMTestActions    3.	KSysUtilRamDriveCriticalThreshold is set to -1,  KSysUtilOtherDiskCriticalThreshold is set to -1,  
 @SYMTestActions    do not set P & S Keys for RAM drive and other disk critical thresholds and valid repository entries are 
 @SYMTestActions    set for RAM drive critical threshold  of 52000 and other disk critical threshold of 132000.
 @SYMTestExpectedResults The following result should occur due to changes to the product code and environment
 @SYMTestExpectedResults There is no way to return what critical threshold values are being used within SysUtil.  Therefore in order to 
 @SYMTestExpectedResults detect that the threshold level has been set correctly the SysUtil::DiskSpaceBelowCriticalLevelL and 
 @SYMTestExpectedResults SysUtil::FFSSpaceBelowCriticalLevelL methods will be called twice with a value offset of one byte above and below 
 @SYMTestExpectedResults from the known critical threshold level value as defined for each case.  The call with the value set above the known 
 @SYMTestExpectedResults critical threshold level must return false and then call with the value set below the critical threshold level must 
 @SYMTestExpectedResults return true.  In this way we can be certain that the correct critical threshold values are being used
 @SYMTestExpectedResults 1.	SysUtil::DiskSpaceBelowCriticalLevelL method uses the 51200 threshold and the SysUtil::FFSSpaceBelowCriticalLevelL 
 @SYMTestExpectedResults  method uses the 131072 threshold.
 @SYMTestExpectedResults 2.	SysUtil::DiskSpaceBelowCriticalLevelL method uses the 51000 threshold and the SysUtil::FFSSpaceBelowCriticalLevelL 
 @SYMTestExpectedResults method uses the 131000 threshold.
 @SYMTestExpectedResults 3.	SysUtil::DiskSpaceBelowCriticalLevelL method uses the 52000 threshold and the SysUtil::FFSSpaceBelowCriticalLevelL 
 @SYMTestExpectedResults method uses the 132000 threshold.
 @SYMTestExpectedResults 4.	SysUtil::DiskSpaceBelowCriticalLevelL method and SysUtil::FFSSpaceBelowCriticalLevelL method both leave with 
 @SYMTestExpectedResults KErrNotFound

 */
TVerdict CSysUtilsFindCriticalThresholdValuesStep::doTestStepL()
	{
	TInt err = iFs.Connect();
	if (err == KErrNone)
		{
		BSUL::CCachedDriveInfo *cachedDriveInfo  = NULL;	
		TRAP(err, cachedDriveInfo = BSUL::CCachedDriveInfo::NewL(iFs));
		if (err == KErrNone)
			{
			TInt driveID = 0;
			TInt driveToUse = 0; 
		
			while((driveID < KMaxDrives) && !(cachedDriveInfo->MediaTypeL(driveID, EMediaRam)))
				{
				++driveID;
				}
			
			if (cachedDriveInfo->MediaTypeL(driveID, EMediaRam))
				{
				driveToUse = driveID;
				driveID = KMaxDrives;
				
	    	   err = testStep1(driveToUse);
	    	   if (err == KErrNotFound)
	    		   {
	    		   err = testStep2(driveToUse);
		    	   if (err == KErrNone)
		    		   {
		    		   err = testStep3(driveToUse);
		    		   }
	    		   }
	    	   
				}
			else
				{
				ERR_PRINTF1(_L("Disk of type ram not found"));
				SetTestStepResult(EFail);
				}
			}
		else
			{
			ERR_PRINTF2(_L("Error creating CCachedDriveInfo %d"), err);
			SetTestStepResult(EFail);
			}
		Delete(cachedDriveInfo);
		iFs.Close();
		}
	else
		{
		ERR_PRINTF2(_L("Could not connect the RFs Session %d"), err);
		SetTestStepResult(EFail);
		}
	return TestStepResult ();
	}

void CSysUtilsFindCriticalThresholdValuesStep::processResult(int aErr, TBool aStatus)
	{
	if ( aErr == KErrNotSupported )
		{
		ERR_PRINTF1(_L("System does not have this drive"));
		SetTestStepResult(EFail);
		}
	else if ( aErr == KErrNotReady )
		{
		ERR_PRINTF1(_L("Drive contains no media"));
		SetTestStepResult(EFail);
		}
	else if ( aErr != KErrNone )
		{
		ERR_PRINTF2(_L("Other error %d"), aErr);
		SetTestStepResult(EFail);
		}
	else
		{
		if ( aStatus )
			{
			INFO_PRINTF1(_L("DiskSpaceBelowCriticalLevel = TRUE"));
			}
		else
			{
			INFO_PRINTF1(_L("DiskSpaceBelowCriticalLevel = FALSE"));
			}
		}
	}

TInt CSysUtilsFindCriticalThresholdValuesStep::testStep1(TInt aDrive)
	{
	TInt err = 0;
	TRAP(err, SysUtil::DiskSpaceBelowCriticalLevelL( NULL, 0, aDrive ) );

	if (err == KErrNotFound)
		{		
		INFO_PRINTF1(_L("Ram Values not found this is correct  "));
		TRAP(err, SysUtil::FFSSpaceBelowCriticalLevelL( NULL, 0 ) );
		if (err == KErrNotFound)
			{		
			INFO_PRINTF1(_L("Other Values not found this is correct  "));
			}
		else
			{
			ERR_PRINTF2(_L("KErrNotFound expected got error %d"), err);
			SetTestStepResult(EFail);
			}
		}
	else
		{
		ERR_PRINTF2(_L("KErrNotFound expected got error %d"), err);
		SetTestStepResult(EFail);
		}
	return err;
	}

TInt CSysUtilsFindCriticalThresholdValuesStep::testStep2(TInt aDrive)
	{
	TInt err = 0;
	CRepository* repository = NULL;
	TRAP(err, repository = CRepository::NewL (KCRUidDiskLevel));
	if (err == KErrNone)
		{
		err = CreateRepositoryEntries(repository, aDrive);
		Delete(repository);
		}
	else
		{		
		ERR_PRINTF2(_L("Could not create the repository object %d"), err);
		SetTestStepResult(EFail);
		}
	return err;
	}

TInt CSysUtilsFindCriticalThresholdValuesStep::CreateRepositoryValues(CRepository *aRepository, const TUint32 aKey, TInt aValue )
	{
	TInt err = aRepository->Create(aKey, aValue);
	if ((err == KErrNone) || (err == KErrAlreadyExists))
		{			
		if (err == KErrAlreadyExists)
			{
			err = aRepository->Set(aKey, aValue);
			}
			
		if (err != KErrNone)
			{
			ERR_PRINTF2(_L("Could not create the repository value %d"), err);
			SetTestStepResult(EFail);
			}
		}
	else
		{
		ERR_PRINTF2(_L("Could not create the repository value %d"), err);
		SetTestStepResult(EFail);
		}
	return err;
	}

TInt CSysUtilsFindCriticalThresholdValuesStep::CreateRepositoryEntries(CRepository *aRepository, TInt aDrive)
	{
	TInt valueRam = 52000;
	TInt valueOther = 132000;

	TInt err = aRepository->Create(KRamDiskCriticalLevel, valueRam);
	if (err == KErrNone)
		{				
		err = aRepository->Create(KDiskCriticalThreshold, valueOther);
		if (err == KErrNone)
			{
			err = CreateValuesToWrite(aDrive, valueRam, valueOther);
			if (err != KErrNone)
				{
				ERR_PRINTF2(_L("Error occured %d"), err);
				SetTestStepResult(EFail);
				}
			}
		else
			{
			ERR_PRINTF2(_L("Could not create the repository value %d"), err);
			SetTestStepResult(EFail);
			}
		}
	else
		{
		ERR_PRINTF2(_L("Could not create the repository value %d"), err);
		SetTestStepResult(EFail);
		}
	
	return err; 
	}

TInt CSysUtilsFindCriticalThresholdValuesStep::CreateValuesToWrite(TInt aDrive, TInt aRamThreshold, TInt aOtherThreshold)
	{
	
	TInt ffsDrive = KSysUtilDefaultFFSDrive;
	if (!(ffsDrive >= EDriveA && ffsDrive <= EDriveZ))
		{
	    ffsDrive =  iFs.GetSystemDrive();
	    } 
	
	TVolumeInfo vInfoFFS;
	TVolumeInfo vInfoDisk;
	TInt err = iFs.Volume( vInfoFFS, ffsDrive);
	if (err == KErrNone)
		{
		err = iFs.Volume( vInfoDisk, aDrive);
		if (err==KErrNone)
			{
			TInt64 freeDiskSpace = vInfoDisk.iFree;
			TInt64 freeFFSSpace = vInfoFFS.iFree;
			
			const TInt KOne = 1;
			TInt toWriteDiskAbove = 0;
			TInt toWriteDiskBelow = 0;
			TInt toWriteFFSAbove = 0;
			TInt toWriteFFSBelow = 0;
			
			if (aRamThreshold <= freeDiskSpace) 
				{
				freeDiskSpace = vInfoDisk.iFree;
				toWriteDiskAbove = (freeDiskSpace - aRamThreshold) - KOne;
				freeDiskSpace = vInfoDisk.iFree;
				toWriteDiskBelow = (freeDiskSpace - aRamThreshold) + KOne;
				}
			else
				{
				freeDiskSpace = vInfoDisk.iFree;
				toWriteDiskAbove = (aRamThreshold - freeDiskSpace) - KOne;
				freeDiskSpace = vInfoDisk.iFree;
				toWriteDiskBelow = (aRamThreshold - freeDiskSpace) + KOne;	
				}
			INFO_PRINTF1(_L("checking disk space" ));
			err = CallDiskThresholdCheck(aDrive, toWriteDiskAbove, toWriteDiskBelow);	
			if (err == KErrNone)
				{
				if (aOtherThreshold <= freeFFSSpace ) 
					{
					freeFFSSpace = vInfoFFS.iFree;
					toWriteFFSAbove = (freeFFSSpace - aOtherThreshold) - KOne;
					freeFFSSpace = vInfoFFS.iFree;
					toWriteFFSBelow = (freeFFSSpace - aOtherThreshold) + KOne;
					}
				else
					{
					freeFFSSpace = vInfoFFS.iFree;
					toWriteFFSAbove = (aOtherThreshold - freeFFSSpace) - KOne;
					freeFFSSpace = vInfoFFS.iFree;
					toWriteFFSBelow = (aOtherThreshold - freeFFSSpace) + KOne;	
					}

				INFO_PRINTF1(_L("checking ffs space" ));
				err = CallFFSThresholdCheck(toWriteFFSAbove, toWriteFFSBelow);	
				}
			}			
		}

	return err;	
	}

TInt CSysUtilsFindCriticalThresholdValuesStep::CallDiskThresholdCheck(TInt aDrive, TInt aDiskAbove, TInt aDiskBelow)
	{
	
	TInt err;
	TBool firstStatus = EFalse;
	TRAP(err, firstStatus = SysUtil::DiskSpaceBelowCriticalLevelL( NULL, aDiskAbove, aDrive ) );
	processResult(err, firstStatus);
	if ( err == KErrNone)
		{
		TBool secondStatus = EFalse;
		TRAP(err, secondStatus = SysUtil::DiskSpaceBelowCriticalLevelL( NULL, aDiskBelow, aDrive ) );
		processResult(err, secondStatus );
		if (err == KErrNone)
			{
			if (firstStatus != secondStatus)
				{
				INFO_PRINTF1(_L("The values are different therefore the values appear to be set correctly"));
				}
			else
				{
				ERR_PRINTF1(_L("Err the status has not changed"));
				SetTestStepResult(EFail);
				err = KErrGeneral;
				}
			}
		else
			{
			ERR_PRINTF2(_L("Error finding the disk status call 2 %d"), err);
			SetTestStepResult(EFail);
			}
		}
	else
		{
		ERR_PRINTF2(_L("Error finding the disk status call 1 %d"), err);
		SetTestStepResult(EFail);
		}
	
	return err;
	
	}

TInt CSysUtilsFindCriticalThresholdValuesStep::CallFFSThresholdCheck(TInt aFFSAbove, TInt aFFSBelow)
	{
	
	TInt err;
	TBool firstStatus = EFalse;
	TRAP(err, firstStatus = SysUtil::FFSSpaceBelowCriticalLevelL( NULL, aFFSAbove) );
	processResult(err, firstStatus);
	if ( err == KErrNone)
		{
		TBool secondStatus = EFalse;
		TRAP(err, secondStatus = SysUtil::FFSSpaceBelowCriticalLevelL( NULL, aFFSBelow ) );
		processResult(err, secondStatus );
		if (err==KErrNone)
			{
			if (firstStatus != secondStatus)
				{
				INFO_PRINTF1(_L("The values are different therefore the values appear to be set correctly"));
				}
			else
				{
				ERR_PRINTF1(_L("Error the status has not changed"));
				SetTestStepResult(EFail);
				err = KErrGeneral;
				}
			}
		else
			{
			ERR_PRINTF2(_L("Error finding the ffs status  call 2 %d"), err);
			SetTestStepResult(EFail);
			}
		}
	else
		{
		ERR_PRINTF2(_L("Error finding the ffs status  call 1 %d"), err);
		SetTestStepResult(EFail);
		}
	
	return err;	
	
	}


TInt CSysUtilsFindCriticalThresholdValuesStep::testStep3(TInt aDrive)
	{
	TInt valueRam = 51000;
	TInt valueOther = 131000;
	
	TInt err = CreatePSValues(KPSUidDiskLevel, KRamDiskCriticalThreshold, valueRam);
	if (err == KErrNone)
		{
		err = CreatePSValues(KPSUidDiskLevel, KOtherDiskCriticalThreshold, valueOther);
		if (err == KErrNone)
			{
			err = CreateValuesToWrite(aDrive, valueRam, valueOther);
			}
		}
	return err;
	}

TInt CSysUtilsFindCriticalThresholdValuesStep::CreatePSValues(const TUid aUid, const TUint32 aKey, TInt aValue)
	{
	TInt err = RProperty::Define(aUid, aKey, RProperty::EInt);
	
	if ((err == KErrNone) || (err == KErrAlreadyExists))
		{
		err = RProperty::Set ( aUid, aKey, aValue);
		if (err != KErrNone)
			{
			ERR_PRINTF2(_L("Unable to set the threshold values %d"), err);
			SetTestStepResult(EFail);
			}
		}
	else
		{
		ERR_PRINTF2(_L("Could not define the p and s %d"), err);
		SetTestStepResult(EFail);
		}
	return err;
	}


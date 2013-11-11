// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file backuprestoreStep.cpp
*/

#include "SocketServer.TestSteps.h"

CBackupRestoreStep::CBackupRestoreStep(CCEsockTestBase*& aEsockTest)
:   CTe_EsockStepBase(aEsockTest)
	{
	SetTestStepName(KBackupRestoreStep);
	}

TVerdict CBackupRestoreStep::doTestStepPreambleL()
	{
	//default implementation of this function checks for iEsockTest
	return EPass;
	}

TInt CBackupRestoreStep::ConfigureFromIni()
	{
	// Read in appropriate fields
	TPtrC type;
	if((GetStringFromConfig(iSection, KTe_Type, type) != 1)
		|| (type.Length() == 0))
		{
		INFO_PRINTF1(_L("Couldn't find appropriate field in config file"));
		return KErrNotFound;
		}

    if (type.Compare(KTe_BackupRestoreTypeBackup)==0)
	    {
	    iIsBackup = ETrue;
	    }
    else if (type.Compare(KTe_BackupRestoreTypeRestore)==0)
	    {
	    iIsBackup = EFalse;
	    }
    else
        {
        INFO_PRINTF2(_L("Backup type (%S) not recognised."),&type);
        return KErrNotFound;
        }

    if (GetStringFromConfig(iSection,KTe_OriginalFileName,iOriginalFileName)!=1)
        {
        INFO_PRINTF1(_L("Original file name missing."));
        return KErrNotFound;
        }
    if (GetStringFromConfig(iSection,KTe_BackupFileName,iBackupFileName)!=1)
        {
        INFO_PRINTF1(_L("Backup file name missing."));
        return KErrNotFound;
        }
    if (GetStringFromConfig(iSection,KTe_NewFileName,iNewFileName)!=1)
        {
        INFO_PRINTF1(_L("New file name missing."));
        return KErrNotFound;
        }
	
    // All ok if we got this far
    return KErrNone;
	}

TVerdict CBackupRestoreStep::doSingleTestStep()
	{
    SetTestStepResult(EFail);

	if (iIsBackup)
    	{
    	// delete the old backup file (wont Leave if its not found)
        DeleteFileL(iBackupFileName);
        INFO_PRINTF1(_L("Deleted old backup file"));

    	// backup the original (target) file
    	TRAPD(error, CopyFileL(iOriginalFileName, iBackupFileName));
    	if (error==KErrNotFound)
    	    {
    	    INFO_PRINTF1(_L("Could not find the original file to backup. Ignoring."));
    	    error=KErrNone; //we do not need to back up if does not exist
    	    }
    	else if (error==KErrPathNotFound)
    	    {
    	    INFO_PRINTF1(_L("Could not find the original file\'s path. Ignoring."));
    	    error=KErrNone; //we do not need to back up if does not exist
    	    }
    	    
	    User::LeaveIfError(error);

        // Copy in our new replacement file
        CopyFileL(iNewFileName, iOriginalFileName);
        INFO_PRINTF1(_L("New file copied to target location"));
    	}
    else
        {
	    // delete the target file
	    TRAPD(error, DeleteFileL(iOriginalFileName));
	    if (error != KErrNone)
	        {
            WARN_PRINTF3(_L("Could not remove the original file (%S) failed with %d"),&iBackupFileName, error);
            if (error == KErrNotFound)
                {
                // This shouldn't happen, but if its not there then its
                // not going to be in the way of our restore
                error = KErrNone;
                }
            User::Leave(error);
	        }
        
        // restore the previously backed up file
        TRAP(error, CopyFileL(iBackupFileName, iOriginalFileName));
        if (error == KErrNotFound)
            {
    	    WARN_PRINTF1(_L("No backup found to restore. Ignoring."));
    	    error = KErrNone;
            }
        else if (error == KErrNone)
	        {
            INFO_PRINTF1(_L("File restored"));
	        }
    	
	    // delete the backup file
	    TRAPD(err2, DeleteFileL(iBackupFileName));
        if (err2 == KErrNone) 
            {
            INFO_PRINTF1(_L("Deleted backup file"));
            }
        else
            {
            INFO_PRINTF2(_L("Delete backup file failed with %d. Ignoring."), err2);
            }

        // Leave with the original error, because its the restore operation
        // that we're interested in
    	User::LeaveIfError(error);
        }

    SetTestStepResult(EPass);
	return TestStepResult();
	}



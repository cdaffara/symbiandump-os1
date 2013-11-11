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
 @file Te_EsockStepBase.cpp
*/

#include "Te_EsockStepBase.h"

TVerdict CTe_EsockStepBase::doTestStepPreambleL()
/**
 * This is a default implementation of doTestStepPreambleL()
 * for all test steps. It checks whether the instance of esock
 * test has been initialised.
 */
	{
	SetTestStepResult(EPass);
	if (iEsockTest==NULL)
        {
        INFO_PRINTF1(_L("Esock test not instantiated!!. Aborting!"));
        SetTestStepResult(EFail);
        }
    
	return TestStepResult();
	}

TVerdict CTe_EsockStepBase::doTestStepL()
/**
 * Runs the test step (in the derived class), firstly by configuring from the given ini file
 */
	{
	SetTestStepResult(EPass);

	// Configure the test step according to the ini file section
	iSection.Copy(ConfigSection());
	TInt configuredOk = ConfigureFromIni();
	
	// If we configured ok then run the test step otherwise we failed
	if (configuredOk == KErrNone)
		{
		TVerdict result = doSingleTestStep();
		SetTestStepResult(result);
		}
	else
		{
		INFO_PRINTF1(_L("Test step does not have a valid configuration."));
		SetTestStepResult(EFail);
		}
	
	return TestStepResult();
	}


TInt CTe_EsockStepBase::GetCurrentIdx()
/**
 * Returns index of the current single step
 */
    {
    return iIdx;
    }

CTe_EsockStepBase::~CTe_EsockStepBase()
/**
 * Destructor
 */
	{
	}

CTe_EsockStepBase::CTe_EsockStepBase(CCEsockTestBase*& aEsockTest)
:   iEsockTest(aEsockTest), iFoundUniqueSection(EFalse)
/**
 * Constructor
 */
	{
	}


TBool CTe_EsockStepBase::GetIpAddressFromConfig(const TDesC &aSectName,const TDesC &aKeyName,TInetAddr &anAddr)
/* Get the IP address from the configuration file
 *
 * get string from config file
 * convert the string to a TInetAddr
 */
	{
	TPtrC result;

	TBool bRet = GetStringFromConfig(aSectName, aKeyName, result);

	// if True
	if (bRet)
		{
		TInt nRet = anAddr.Input(result);
		// Invalid IP Address
		if (nRet!=KErrNone)
			{
			INFO_PRINTF3(_L("Invalid IP address, section:%S key:%S "),&aSectName, &aKeyName);
			bRet = EFalse;
			}
		}

	return bRet;
	}

// Method to Copy a file to another file. 
void CTe_EsockStepBase::CopyFileL (const TDesC& aOld,const TDesC& aNew) 
	{
	// create a fileserver
	RFs fileSystem;

	// connect to file server
	TInt returnCode = fileSystem.Connect();
	User::LeaveIfError(returnCode);
	CleanupClosePushL(fileSystem);

	// create a file manager
	CFileMan * fileMan = CFileMan::NewL(fileSystem);
	CleanupStack::PushL(fileMan);

	// parse the filenames
	TParse source;
	returnCode = source.Set(aOld, NULL, NULL);
	User::LeaveIfError(returnCode);
 
	// parse the filenames
	TParse target;
	returnCode = target.Set(aNew, NULL, NULL);
	User::LeaveIfError(returnCode);

    TPtrC path(target.DriveAndPath());
    INFO_PRINTF2(_L("Ensuring path %S exists"), &path);
	returnCode = fileSystem.MkDirAll(target.DriveAndPath());
	if (returnCode == KErrAlreadyExists)
	    {
	    returnCode = KErrNone;	    
	    }
	User::LeaveIfError(returnCode);
	
	// do the copy
	returnCode = fileMan->Attribs(target.FullName(), 0, KEntryAttReadOnly, 0);
	INFO_PRINTF2(_L("Attribs returned with %d"), returnCode);

	returnCode = fileMan->Copy(source.FullName(), 
		target.FullName(), CFileMan::EOverWrite);

	if (returnCode != KErrNone)
		{
        INFO_PRINTF2(_L("Copy failed with %d"), returnCode);
		User::Leave(returnCode);
		}
	

	CleanupStack::PopAndDestroy(fileMan);
	CleanupStack::PopAndDestroy();
	}

void CTe_EsockStepBase::DeleteFileL (const TDesC& aFileName)
	{
	// create a fileserver
	RFs fileSystem;

	// connect to file server
	TInt returnCode = fileSystem.Connect();
	User::LeaveIfError(returnCode);
	CleanupClosePushL(fileSystem);

	// create a file manager
	CFileMan * fileMan = CFileMan::NewL(fileSystem);
	CleanupStack::PushL(fileMan);

	// parse the filenames
	TParse source;
	returnCode = source.Set(aFileName, NULL, NULL);
	User::LeaveIfError(returnCode);
 
	// do the delete
	returnCode = fileMan->Attribs(source.FullName(), 0, KEntryAttReadOnly, 0);
	INFO_PRINTF2(_L("Attribs returned with %d"), returnCode);
	
	returnCode = fileMan->Delete(source.FullName()); 
	if (returnCode != KErrNone && returnCode != KErrNotFound)
		{
        INFO_PRINTF2(_L("Delete file %S failed"), &source.FullName());
		User::Leave(returnCode);
		}
	if (returnCode == KErrNotFound)
	    {
        INFO_PRINTF1(_L("File to delete not present"));
	    }

	CleanupStack::PopAndDestroy(fileMan);
	CleanupStack::PopAndDestroy();
	}



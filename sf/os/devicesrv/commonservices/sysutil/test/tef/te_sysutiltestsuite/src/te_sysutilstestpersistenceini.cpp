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
// file te_sysutilstestpersistenceini.cpp
// 
//

#include <e32debug.h>
#include <e32property.h>
#include "te_sysutilstestpersistenceini.h"

CSysUtilsTestPersistenceIniStep::~CSysUtilsTestPersistenceIniStep()
	{
	}

CSysUtilsTestPersistenceIniStep::CSysUtilsTestPersistenceIniStep()
	{
	}

/**
@SYMTestCaseID			SYSLIB-BAFL-CT-4058
@SYMTestCaseDesc		To ensure the INI file isn’t read twice.
						
						Note: The INI file should be copied in to 
						C:\versions\deviceattributes.ini. In addition the P&S properties 
						should be deleted so new ones can be created.
@SYMTestPriority		Medium
@SYMTestActions			1.	Create an instance of CDeviceTypeInformation by calling 
							SysUtil::GetDeviceTypeInfo (this makes sure the device 
							attributes P&S property gets set).
						2.	Delete the instance of CDeviceTypeInformation.
						3.	Replace the INI file in C:\versions\ with one which contains 
							different attributes values.
						4.	Create a new instance of CDeviceTypeInformation by calling 
							SysUtil::GetDeviceTypeInfo.
						5.	Call each of CDeviceTypeInformation member functions checking 
							the retrieved values and return codes.
@SYMTestExpectedResults	The retrieved values should be the ones in the first INI file and 
						the corresponding return codes should be returned.
@SYMDEF					CR1328
*/
TVerdict CSysUtilsTestPersistenceIniStep::doTestStepL()
	{
	TInt err = SetTestPath(ETrue);
	if( err != KErrNone )
		{
		ERR_PRINTF2(_L("Could not turn test path on. Error = %d"), err);
		SetTestStepResult(EAbort);
		return TestStepResult();
		}
	INFO_PRINTF1(_L("Test path turned on."));
	
	err = DeletePSProperties();
	if( err != KErrNone )
		{
		ERR_PRINTF2(_L("Could not delete P&S properties. Error = %d"), err);
		SetTestStepResult(EAbort);
		return TestStepResult();
		}
	INFO_PRINTF1(_L("Deleted P&S properties successfully."));
	
	CDeviceTypeInformation* attribs = SysUtil::GetDeviceTypeInfoL();
	if(TestNamedAttributeFunctions(*attribs) == EFalse || TestUnnamedAttributeFunctions(*attribs) == EFalse)
		{
		SetTestStepResult(EFail);
		}
	
	delete attribs;
	attribs = NULL;
	
	// Copy a different deviceattributes.ini over the original one.
	RFs fs;
	fs.Connect();
	CleanupClosePushL(fs);

	CFileMan* fileMan = CFileMan::NewL(fs);
	CleanupStack::PushL(fileMan);
	
	TDriveUnit drive(static_cast<TUint>(RFs::GetSystemDrive()));
	TParse deviceAttribsPathParse;
	deviceAttribsPathParse.Set(drive.Name(), &KDeviceAttributesIniTestPath, NULL);
	
	TParse newFileParse;
	newFileParse.Set(KDeviceAttributesIniPath, NULL, NULL);
	TFileName newFile(newFileParse.DriveAndPath());
	newFile.Append(_L("deviceattributes_persistence_ini_unread.ini"));
	
	User::LeaveIfError(fileMan->Copy(newFile, deviceAttribsPathParse.FullName()));
	CleanupStack::PopAndDestroy(2);
	
	// Check the file is where we expect it
	TFileName deviceAttribsPath(deviceAttribsPathParse.FullName());
	err = CheckIsFileExist(deviceAttribsPath);
	if(KErrNone != err)
		{
		INFO_PRINTF2(_L("File doesnt exist: %S"), &deviceAttribsPath);
		}
	
	attribs = SysUtil::GetDeviceTypeInfoL();
	if(TestNamedAttributeFunctions(*attribs) == EFalse || TestUnnamedAttributeFunctions(*attribs) == EFalse)
		{
		SetTestStepResult(EFail);
		}
	
	delete attribs;
	
	return TestStepResult();
	}

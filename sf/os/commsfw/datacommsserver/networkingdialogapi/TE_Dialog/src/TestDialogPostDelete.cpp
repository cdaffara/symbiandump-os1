// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// POSTDELETE.CPP
// @file
// @internalComponent
// Ini file Deletion
// 
//

//Symbian OS Headers
#include <c32comm.h>
//#include <agentdialog.h>
#include <f32file.h>
#include "TestDialogPostDelete.h"
//Test Framework Header
#include <test/testexecutelog.h>

//
//
// Test Delete
/* This New Class is added to remove deletefile command from auto_Ts_genconn.script file
   This test does a delete operation for agentdialog.ini file	
*/
CTestPostDeleteIni::CTestPostDeleteIni()
	{
	// store the name of this test case
	SetTestStepName(KPostDeleteDialog);
	}

CTestPostDeleteIni::~CTestPostDeleteIni()
	{
	}

// Delete Ini Test Main Code
enum TVerdict CTestPostDeleteIni::doTestStepL( void )
	{
	deleteFileL(KFilePath);
	return EPass;
	}

void CTestPostDeleteIni::deleteFileL(const TDesC& aFileName) 
	{
	// create a fileserver
	RFs  fileSystem;
	CTestExecuteLogger log = Logger();

	User::LeaveIfError(fileSystem.Connect());
	CleanupClosePushL(fileSystem);

	// Remove read only flag
	TInt ret = fileSystem.SetAtt(aFileName, 0, KEntryAttReadOnly);
	if (ret == KErrNotFound)
		{
		// If file already removed then no need to delete it
		log.Write(_L("File not found"));
		CleanupStack::PopAndDestroy(&fileSystem);
		return;
		}
	User::LeaveIfError(ret);

	log.Write(_L("Set file to read only"));

	// Delete file
	User::LeaveIfError(fileSystem.Delete(aFileName));
	log.Write(_L("deleted file"));

	// clean up
	CleanupStack::PopAndDestroy(&fileSystem);
	}


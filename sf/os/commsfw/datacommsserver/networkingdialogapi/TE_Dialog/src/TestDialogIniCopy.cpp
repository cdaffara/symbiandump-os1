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
// INICOPY.CPP
// @file
// @internalComponent
// Ini file preparation
// 
//

//Symbian OS Headers
#include <c32comm.h>
//#include <agentdialog.h>
#include <f32file.h>
#include "TestDialogIniCopy.h"
//Test Framework Header
#include <test/testexecutelog.h>

//
//
// Test Copy Test
/* This New Class is added to remove copyfile command from auto_Ts_genconn.script file
   This test does a copy operation for agentdialog.ini file	
*/
CTestPreCopyIni::CTestPreCopyIni()
	{
	// store the name of this test case
	SetTestStepName(KPreCopyDialog);
	}

CTestPreCopyIni::~CTestPreCopyIni()
	{
	}

// Copy Test Main Code
enum TVerdict CTestPreCopyIni::doTestStepL( void )
	{
	copyFileL(KSrcPath, KDestPath);
	return EPass;
	}

void CTestPreCopyIni::copyFileL(const TDesC& anOld,const TDesC& aNew) 
	{
	// create a fileserver
	RFs fileSystem;

	// connect to file server
	User::LeaveIfError(fileSystem.Connect());
	CleanupClosePushL(fileSystem);

	// create a file manager
	CFileMan* fileMan = CFileMan::NewL(fileSystem);
	CleanupStack::PushL(fileMan);

	// parse the filenames
	TParse source;
	User::LeaveIfError(source.Set(anOld, NULL, NULL));
 
	// parse the filenames
	TParse target;
	User::LeaveIfError(target.Set(aNew, NULL, NULL));

	// do the copy
	User::LeaveIfError(fileMan->Copy(source.FullName(), target.FullName(), CFileMan::EOverWrite));

	CleanupStack::PopAndDestroy(fileMan);
	CleanupStack::PopAndDestroy(&fileSystem);
	}


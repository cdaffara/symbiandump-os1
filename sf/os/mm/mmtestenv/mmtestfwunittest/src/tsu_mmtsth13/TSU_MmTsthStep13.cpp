// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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

// EPOC includes
#include <e32base.h>

// Test system includes
#include <testframework.h>
#include "TSU_MmTsthStep13.h"
#include "TSU_MmTsthSuite13.h"

// preamble
TVerdict RTSUMmTsthStep13::OpenL()
	{
	// initialise iTestUtils for tests. if this fails, tests are
	// inconclusive (will not run)
	CLog* theLog = iSuite->LogSystem();	// use the current log
	CTestUtils* theTestUtils = NULL;
	TRAPD(err, theTestUtils = CTestUtils::NewL(theLog));
	if(err != KErrNone)
		{
		ERR_PRINTF2(_L("CTestUtils::NewL() failed in preamble, error code %d"), err);
		return iTestStepResult = EInconclusive;
		}
	iTestUtils = theTestUtils;
	return iTestStepResult = EPass;
	}

// postamble
void RTSUMmTsthStep13::Close()
	{
	delete iTestUtils;
	}

// Cleanup file system after tests.
// NB - any file locks or read-only flags MUST be cleared 
// by the test itself before calling this method.
// We do not intend to check return codes from this function, as
// not all the files/directories we intend to remove will 
// necessarily exist.
void RTSUMmTsthStep13::CleanupFileSystem()
	{
	RFs theFs;
	CFileMan* theFm = NULL;
	theFs.Connect();
	TRAPD(err, theFm = CFileMan::NewL(theFs));
	if(err != KErrNone)
		{
		ERR_PRINTF2(_L("Cannot create CFileMan, error code %d"), err);
		theFs.Close();
		}

	theFm->RmDir(_L("c:\\TFData1\\"));
	theFm->RmDir(_L("c:\\TFData2\\"));
	theFs.Close();
	delete theFm;
}

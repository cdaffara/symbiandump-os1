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
// PRECOPY.CPP
// @file
// @internalComponent
// Pre Copy file preparation
// 
//

// EPOC includes
#include <e32base.h>
#include <f32file.h>

// Qos Test system includes
#include "TE_RConnectionConfigDaemonTestSteps.h"


//
//
// Test Post Delete
/* This New Class is added to move deletefile command from script file
   This test does a delete operation 	
*/

TE_RConnectionPostDelete::~TE_RConnectionPostDelete()
	{
	}

// Delete Test Main Code
enum TVerdict TE_RConnectionPostDelete::doTestStepL( void )
	{
	copyFileL(KTCPIPBackPath, KTCPIPPath);
	deleteFileL(KTCPIPBackPath);
	return EPass;
	}


// Method to Copy a file to another file. 
void TE_RConnectionPostDelete::copyFileL (const TDesC& anOld,const TDesC& aNew) 
	{

	// create a fileserver
	RFs  FileSystem;

	// connect to file server
	TInt returnCode=FileSystem.Connect();

	// create a file manager
	CFileMan * fileMan = CFileMan::NewL( FileSystem );

	CleanupStack::PushL(fileMan);

	if (returnCode != KErrNone )
		{
		User::Leave(returnCode);
		}

	// parse the filenames
	TParse Source;
	returnCode = Source.Set( anOld, NULL, NULL );
	if ( returnCode != KErrNone )
		{
		User::Leave(returnCode);
		}
 
	// parse the filenames
	TParse Target;
	returnCode = Target.Set( aNew, NULL, NULL );
	if ( returnCode != KErrNone )
		{
		User::Leave(returnCode);
		}

	// do the copy
	returnCode=fileMan->Copy(Source.FullName(), 
		Target.FullName(), CFileMan::EOverWrite);

	if ( returnCode != KErrNone )
		{
			User::Leave(returnCode);
		}

	CleanupStack::PopAndDestroy(fileMan);
	// close the file system
	FileSystem.Close();
	}

void TE_RConnectionPostDelete::deleteFileL (const TDesC& aFileName)
	{
	// create a fileserver
	RFs  fileSystem;

	User::LeaveIfError(fileSystem.Connect());
	CleanupClosePushL(fileSystem);

	// Remove read only flag
	TInt ret = fileSystem.SetAtt(aFileName, 0, KEntryAttReadOnly);
	if (ret == KErrNotFound)
		{
		// If file already removed then no need to delete it
		INFO_PRINTF1(_L("File not found"));
		CleanupStack::PopAndDestroy(&fileSystem);
		return;
		}
	User::LeaveIfError(ret);

	INFO_PRINTF1(_L("Set file to read only"));

	// Delete file
	User::LeaveIfError(fileSystem.Delete(aFileName));
	INFO_PRINTF1(_L("deleted file"));

	// clean up
	CleanupStack::PopAndDestroy(&fileSystem);
	}


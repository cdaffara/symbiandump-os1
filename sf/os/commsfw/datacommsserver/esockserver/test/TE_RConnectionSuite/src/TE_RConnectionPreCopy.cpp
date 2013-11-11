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

#include "TE_RConnectionConfigDaemonTestSteps.h"
 

//
//
// Test Pre Copy
/* This New Class is added to move copyfile command from script file
   This test does a copy operation 	
*/
TE_RConnectionPreCopy::~TE_RConnectionPreCopy()
	{
	}

// Copy Test Main Code
enum TVerdict TE_RConnectionPreCopy::doTestStepL( void )
	{
	copyFileL(KTCPIPPath, KTCPIPBackPath);
	copyFileL(KQosTCPIPPath, KTCPIPPath);
	return EPass;
	}


// Method to Copy a file to another file. 
void TE_RConnectionPreCopy::copyFileL (const TDesC& anOld,const TDesC& aNew) 
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


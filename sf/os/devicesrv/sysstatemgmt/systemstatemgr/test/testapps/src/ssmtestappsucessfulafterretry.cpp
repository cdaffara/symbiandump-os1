// Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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
 @test
 @internalComponent - Internal Symbian test code
*/

#include <s32file.h>
#include <f32file.h>
#include "ssmtestapps.h"

// This exe fails to start for a specified number of times given in failCount, 
// decrements the failCount each time and starts successfully once the fail count is exhausted
static void StartProcessL()
{
	RFs fs;
	RFileReadStream readStream;
	RFileWriteStream writeStream;
					
	CleanupClosePushL(fs);
	CleanupClosePushL(readStream);
	CleanupClosePushL(writeStream);

	User::LeaveIfError(fs.Connect());
	User::LeaveIfError(readStream.Open(fs, KFileForFAndF, EFileRead));
	
	TInt retryCount = readStream.ReadInt8L();
	TInt failCount = readStream.ReadInt8L();
	TInt delayRequired = readStream.ReadInt8L();
	readStream.Close();	
	
	if(0 == failCount)
		{
		RProcess::Rendezvous(KErrNone);
		}
	else
		{
		User::LeaveIfError(writeStream.Open(fs, KFileForFAndF, EFileWrite));

		writeStream.WriteInt8L(retryCount+1);
		writeStream.WriteInt8L(--failCount);
		writeStream.CommitL();
		writeStream.Close();

		// This delay is required for testing the absolute timeout of startsafe, this is just 
		// random time delay been used.
		if(delayRequired)
			User::After(500000);
									

		// StartSafe will restart the process
		RProcess::Rendezvous(KErrGeneral);
		}
	CleanupStack::PopAndDestroy(3, &fs);
}

/**
Process entry point function.
@return KErrNone if successful; KErrNoMemory if there is insufficient memory; otherwise, one of the other system-wide error codes.
*/
TInt E32Main()
	{
    CTrapCleanup* cleanup=CTrapCleanup::New();
	
	TInt error = KErrNoMemory;
	if (cleanup)
		{
		TRAP(error, StartProcessL());
		delete cleanup;
		}

	if (KErrNone != error)
		{
		User::Panic(_L("SUCCESSFULRETRY"),error);
		}
	User::WaitForAnyRequest();
	return error;
	}



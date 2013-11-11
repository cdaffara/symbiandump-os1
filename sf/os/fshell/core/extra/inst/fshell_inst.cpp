// fshell_inst.cpp
// 
// Copyright (c) 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//
#include <e32std.h>
#include <f32file.h>
#include <fshell/ioutils.h>

using namespace IoUtils;

void RunL()
	{
	RProcess process;
	TFileName2 sourceFileName(process.FileName());
	RArray<TFileName2> splitFileName;
	CleanupClosePushL(splitFileName);
	sourceFileName.SplitL(splitFileName);
	splitFileName.Remove(splitFileName.Count() - 1);
	splitFileName.Append(_L("iocons.dll"));
	sourceFileName.UnsplitL(splitFileName);

	TFileName2 destinationFileName;
	splitFileName.Remove(splitFileName.Count() - 1);
	splitFileName.Append(_L("econs.dll"));
	destinationFileName.UnsplitL(splitFileName);
	
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	CFileMan* fileMan = CFileMan::NewL(fs, NULL);
	CleanupStack::PushL(fileMan);

	User::LeaveIfError(fileMan->Copy(sourceFileName, destinationFileName));

	CleanupStack::PopAndDestroy(3, &splitFileName);
	}

GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	TInt err = KErrNoMemory;
	CActiveScheduler* sheduler = new CActiveScheduler;
	if (sheduler)
		{
		CActiveScheduler::Install(sheduler);
		CTrapCleanup* cleanup = CTrapCleanup::New();
		if (cleanup)
			{
			TRAP(err, RunL());
			delete cleanup;
			}
		delete sheduler;
		}
	__UHEAP_MARKEND;
	return err;
	}


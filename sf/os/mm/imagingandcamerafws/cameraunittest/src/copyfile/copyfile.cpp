// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32base.h>
#include <e32std.h>
#include <f32file.h>

_LIT(KSeparator, "|"); // Invalid filepath char used to separate filenames


// Copy the files specified.
void CopyFileL(const TDesC& anOld, const TDesC& aNew)
	{	
	RFs fs;
	fs.Connect();	
	CleanupClosePushL(fs);

	CFileMan* fileMan = CFileMan::NewL(fs);
	CleanupStack::PushL(fileMan); 

	// Check if the destination directory exists, if so then delete files if they are present
	TInt err = fs.MkDirAll(aNew);	
	if (err == KErrAlreadyExists)
		{
		err = fileMan->Attribs(aNew, 0, KEntryAttReadOnly, TTime(0), 0);
		if ((err != KErrNone) && (err != KErrNotFound))
			{
			User::Leave(err);
			}
		else if(err == KErrNone)
			{
			User::LeaveIfError(fs.Delete(aNew));
			}
		}
	else if (err != KErrNone)
		{
		User::Leave(err);
		}

	// Make the destination file writeable 
    err = fileMan->Attribs(aNew, 0, KEntryAttReadOnly, TTime(0), 0);
	if ((err != KErrNone) && (err != KErrNotFound))
		{
		User::Leave(err);
		}

	// Do the file copy	
    User::LeaveIfError(fileMan->Copy(anOld, aNew));

	CleanupStack::PopAndDestroy(2); 
	}


// Format of aFileNames is [srcFile]|[dstFile]
GLDEF_C TInt E32Main()
	{
	CTrapCleanup* cleanup =	CTrapCleanup::New(); 

	TBuf<KMaxFileName*2> names;
	User::CommandLine(names);
	TInt pos = names.Find(KSeparator);
	TFileName srcFile(names.Mid(0, pos));
	TFileName dstFile(names.Mid(pos + 1, names.Length() - (pos + 1)));

	TInt err = KErrNone;
	TRAP(err, CopyFileL(srcFile, dstFile));
	if (err == KErrNone)
		{
   		RDebug::Print(_L("CFileMan Copy file '%S' to '%S' succeeded.\n"), &srcFile, &dstFile);
		}
	else
    	{
    	RDebug::Print(_L("CFileMan Copy file '%S' to '%S' failed with error = %d.\n"), &srcFile, &dstFile, err);
    	}

	delete cleanup;
	return err;
	}

// sbapp.cpp
// 
// Copyright (c) 2007 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

// Description:
// Launches the executable named in a "shebang" line (e.g "#!fshell") at the beginning of a script.

#include <e32std.h>
#include <e32base.h>
#include <f32file.h>
#include <apacmdln.h>

void RunL()
	{
	CApaCommandLine* commandLine = NULL;
	User::LeaveIfError(CApaCommandLine::GetCommandLineFromProcessEnvironment(commandLine));
	CleanupStack::PushL(commandLine);
	TFileName scriptFileName(commandLine->DocumentName());
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	RFile file;
	commandLine->GetFileByHandleL(file);
	if (!file.SubSessionHandle())
		{
		User::LeaveIfError(file.Open(fs, scriptFileName, EFileRead));
		}
	RThread().Rendezvous(KErrNone);
	CleanupClosePushL(file);
	TBuf8<64> buf;
	User::LeaveIfError(file.Read(buf));
	CleanupStack::PopAndDestroy(3, commandLine);
	if (buf.Left(2) == _L8("#!"))
		{
		TLex8 lex(buf.Mid(2));
		lex.Mark();
		while (!lex.Eos())
			{
			TChar ch(lex.Get());
			if ((ch == TChar('\r')) || (ch == TChar('\n')))
				{
				lex.UnGet();
				break;
				}
			}
		TPtrC8 narrowExeName(lex.MarkedToken());
		if (narrowExeName.Length() > 0)
			{
			TBuf<64> exeName;
			exeName.Copy(narrowExeName);
			_LIT(KSingleQuote, "'");
			scriptFileName.Insert(0, KSingleQuote);
			scriptFileName.Append(KSingleQuote);
			RProcess process;
			User::LeaveIfError(process.Create(exeName, scriptFileName));
			TRequestStatus status;
			process.Logon(status);
			process.Resume();
			User::WaitForRequest(status);
			User::LeaveIfError(status.Int());
			process.Close();
			}
		}
	}

GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	TInt err = KErrNoMemory;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if (cleanup)
		{
		TRAP(err, RunL());
		delete cleanup;
		}
	__UHEAP_MARKEND;
	return err;
	}


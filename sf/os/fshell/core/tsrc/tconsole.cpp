// tconsole.cpp
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

#include <e32cons.h>

void MainL()
	{
	CActiveScheduler* scheduler = new(ELeave) CActiveScheduler;
	CleanupStack::PushL(scheduler);
	CActiveScheduler::Install(scheduler);
	
	CConsoleBase* console = Console::NewL(_L("tconsole"), TSize(KConsFullScreen, KConsFullScreen));
	CleanupStack::PushL(console);

	if (User::CommandLineLength() == 0)
		{
		console->Printf(_L("Output from direct fshell descendant\r\n"));
		RProcess child;
		User::LeaveIfError(child.Create(_L("tconsole.exe"), _L("child")));
		TRequestStatus stat;
		child.Logon(stat);
		child.Resume();
		User::WaitForRequest(stat);
		child.Close();
		}
	else
		{
		console->Printf(_L("Output from tconsole once removed\r\n"));
		// If iocons is doing its thing, these lines should both end up on the fshell console if "tconsole" is launched from fshell
		}
	CleanupStack::PopAndDestroy(2, scheduler);
	}

GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	TInt err = KErrNoMemory;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if (cleanup)
		{
		TRAP(err, MainL());
		delete cleanup;
		}
	__UHEAP_MARKEND;
	return err;
	}

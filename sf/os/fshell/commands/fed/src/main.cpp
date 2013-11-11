// main.cpp
// 
// Copyright (c) 2009 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#include <e32base.h>
#include <e32std.h>
#include <e32cons.h>
#include <BADESCA.H>
#include <BACLINE.H>

#include "fed.h"

//  Constants
_LIT(KTextConsoleTitle, "Fed editor");
_LIT(KTextFailed, "\n failed, leave code = %d\n press any key to exit");

//  Global Variables
//CColorConsoleBase* console = NULL;  // write all messages to this
CConsoleBase* console = NULL;
CFed* gFed = NULL;

LOCAL_C void DoStartL()
	{
	// Create active scheduler (to run active objects)
	CActiveScheduler* scheduler = new (ELeave) CActiveScheduler();
	CleanupStack::PushL(scheduler);
	CActiveScheduler::Install(scheduler);

	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	CFed* fed = CFed::NewL(*console, fs);
	CleanupStack::PushL(fed);
	CDesC16ArrayFlat* argsBuf = new(ELeave) CDesC16ArrayFlat(4);
	CleanupStack::PushL(argsBuf);
	CCommandLineArguments* args = CCommandLineArguments::NewLC();
	for (TInt i = 1; i < args->Count(); i++) // Start from 1 as arg zero is the exe name
		{
		// No attempt to be clever with arguments - this code is only used when building a raw non-fshell version - mainfshell has the nicer equivalent
		TPtrC arg = args->Arg(i);
		argsBuf->AppendL(arg);
		}
	CleanupStack::PopAndDestroy(args);
	CleanupStack::Pop(argsBuf);
	gFed = fed;
	fed->StartL(argsBuf);
	CActiveScheduler::Start();

	// Delete active scheduler
	CleanupStack::PopAndDestroy(3, scheduler); // fed, fs, scheduler
	}

//  Global Functions
GLDEF_C TInt E32Main()
	{
	// Create cleanup stack
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();

	// Create output console
	//TRAPD(createError, console = dynamic_cast<CColorConsoleBase*>(Console::NewL(KTextConsoleTitle, TSize(KConsFullScreen,KConsFullScreen))));
	TRAPD(createError, console = Console::NewL(KTextConsoleTitle, TSize(KConsFullScreen,KConsFullScreen)));
	if (createError)
		return createError;

	// Run application code inside TRAP harness, wait keypress when terminated
	TRAPD(mainError, DoStartL());
	if (mainError)
		{
		console->Printf(KTextFailed, mainError);
		console->Getch();
		}
	
	delete console;
	delete cleanup;
	__UHEAP_MARKEND;
	return KErrNone;
	}

class TSilentOverflow : public TDes8Overflow
	{
	void Overflow(TDes8& /*aDes*/) { /* Silently truncate */ }
	};

void AssertionFail(const char* aAssertion, const char* aFile, TInt aLine)
	{
	__DEBUGGER();
	TBuf8<256> buf;
	TSilentOverflow o;
	buf.AppendFormat(_L8("Assertion failed: \"%s\" in %s:%i\n"), &o, aAssertion, aFile, aLine);
	if (buf.Length()*2>buf.MaxLength()) buf.SetLength(buf.MaxLength()/2);
	console->Write(buf.Expand());
	User::Panic(_L("Fed"), aLine);
	}

extern TInt KConsoleWidthCorrection = -2;
extern TInt KConsoleHeightCorrection = -2;

TInt TabWidth()
	{
	return 4;
	}

_LIT(KHelpText, "\
Fed Help\n\
========\n\
Fed is a basic console text editor. Its interface is similar to pico/nano. It can handle ASCII, UTF-8 and UTF-16 format files, different line endings, very large files, copy and paste, and opening multiple files at once.\n\
\n\
The supported commands are given below. The control key is represented as a caret '^', so ^G means Control-G.\n\
\n\
Commands\n\
========\n\
^L, F1  Get help\n\
^N      New document\n\
^O      Open document\n\
^S      Save document\n\
^A      Save as\n\
\n\
^U      Page up (alternative to PgUp key)\n\
^D      Page down (alternative to PgDn key)\n\
^T      Go to top of document\n\
^B      Go to bottom of document\n\
\n\
^F      Find text\n\
^K      Delete current line\n\
^G      Go to line\n\
^R, F5  Redraw the screen\n\
^X      Cut (see below)\n\
^C      Copy (see below)\n\
^V      Paste\n\
\n\
^P, F2  Switch to Previous view\n\
^], F3  Switch to Next view\n\
^W, Esc Close current doc\n\
^Q      Quit and close all docs\n\
\n\
Press ^W to close this window, or use ^P etc to cycle between the help and other documents.\n\
\n\
Clipboard support\n\
=================\n\
The standard Symbian clipboard is supported. You can use ^V to paste into the frontmost document. Cut and copy are two-stage operations, because of the limitations of the console:\n\
\n\
To begin a cut/copy operation, press ^X or ^C. Then move the cursor to the other end of the selection you want to make, and press ^X/^C again to confirm the cut/copy operation.\n\
");

void ShowHelpL()
	{
	gFed->ShowHelpL(KHelpText().AllocL());
	}


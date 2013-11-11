// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "TelephonyISVExamples.h"
#include "CMainMenu.h"

/**
Real main function.
*/
void MainL(CConsoleBase& aConsole)
	{
	CMainMenu* menu = CMainMenu::NewLC(aConsole);
	menu->Start();
	
	// Start the active scheduler
	CActiveScheduler::Start();
	CleanupStack::PopAndDestroy(menu);
	}

/**
Console harness.
*/
void ConsoleMainL()
	{
	// Get a console
	CConsoleBase* aConsole = Console::NewL(_L("Telephony ISV API Example"),
	                                       TSize(KConsFullScreen,
	                                       KConsFullScreen));
	CleanupStack::PushL(aConsole);
	CActiveScheduler* sched = new(ELeave) CActiveScheduler();
	
	// Push the active scheduler object onto the cleanup stack.
	CleanupStack::PushL(sched);
	
	// Install active scheduler
	CActiveScheduler::Install(sched);

	// Call function
	MainL(*aConsole);

	aConsole->Printf(_L("[ Press any key to end ]"));
	aConsole->Getch();
	
	// Finished with console
	CleanupStack::PopAndDestroy(sched);
	CleanupStack::PopAndDestroy(aConsole);
	}

/**
Cleanup stack harness.
*/
GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanupStack = CTrapCleanup::New();
	TRAPD(error, ConsoleMainL());
	__ASSERT_ALWAYS(!error, User::Panic(_L("Console main error\n"), error));
	delete cleanupStack;
	__UHEAP_MARKEND;
	return KErrNone;
	}


// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// The process to close all the panic window.
// 
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/

#include <e32base.h>
#include <e32cons.h>
#include <e32test.h>
#include <e32std.h>
#include <e32debug.h>
#include <w32debug.h>
#include <w32std.h>

// Simulate 50 escape-keys down to close all the panic window
static TInt CloseAllPanicWindows(RWsSession& aWs) 
	{
	TInt idFocus = aWs.GetFocusWindowGroup();
	TWsEvent event;
	event.SetType(EEventKey); 
	TKeyEvent* keyEvent = event.Key();
	keyEvent->iCode = EKeyEscape;
	keyEvent->iScanCode = EStdKeyEscape;
	keyEvent->iModifiers = 0;
	TInt theLimit = 50;
	TInt err = KErrNone;
	while(idFocus != NULL && (theLimit-- > 0))
		{
		err = aWs.SendEventToAllWindowGroups(event);
		if (err != KErrNone && err != KErrNoMemory)
			{
			return err;
			}
		User::After(1000);	//give it time to process
		idFocus = aWs.GetFocusWindowGroup();
		}
	return KErrNone;
	}


// Real main function
void MainL()
	{
	// Open wserv session
	RWsSession ws;
	TInt err = ws.Connect();
	if (err != KErrNone)
		{
		User::Leave(err);
		}	

	CleanupClosePushL(ws);
	
	User::LeaveIfError(CloseAllPanicWindows(ws));
			
	CleanupStack::PopAndDestroy(&ws);
	}

// Cleanup stack harness
GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanupStack = CTrapCleanup::New();
	TRAPD(error, MainL());
	_LIT(KTCloseAllPanicWindowPanic,"tcloseallpanicwindow");
	// Panic the current process
	__ASSERT_ALWAYS(!error, User::Panic(KTCloseAllPanicWindowPanic, error));
	delete cleanupStack;
	__UHEAP_MARKEND;
	return KErrNone;
	}

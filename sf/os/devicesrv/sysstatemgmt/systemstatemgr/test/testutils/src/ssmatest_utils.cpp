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
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code 
*/



#include "ssmatest_utils.h"



void CloseAllPanicWindowsL()
	{
	RWsSession	ws;
	User::LeaveIfError(ws.Connect());

	TInt wgFocus = ws.GetFocusWindowGroup();

	const TUint32 ENullWsHandle = 0xFFFFFFFF;	// Events delivered to this handle are thrown away
	RWindowGroup wg = RWindowGroup(ws);

	wg.Construct(ENullWsHandle);
	TInt wgId = wg.Identifier();

	TWsEvent event;
	event.SetType(EEventKey);
	TKeyEvent *keyEvent = event.Key();
	keyEvent->iCode = EKeyEscape;
	keyEvent->iScanCode = EStdKeyEscape;
	keyEvent->iModifiers = 0;

	TInt limit = 0;
	for(limit = 0; wgFocus != wgId && (limit < 50); limit++)
		{
		ws.SendEventToAllWindowGroups(event);
		wgFocus = ws.GetFocusWindowGroup();
		RDebug::Print(_L("CloseAllPanicWindowsL() - EKeyEscape sent to Windows Group"));
		}

	// close everything
	wg.Close();
	ws.Close();
	}



// keyinject.cpp
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
#include <fshell/ltkutils.h>
#include <e32event.h>
#include <e32keys.h>
#include <e32svr.h>

EXPORT_C TInt LtkUtils::InjectRawKeyEvent(TInt aScanCode, TInt aModifiers, TInt /*aRepeatCount*/)
	{
	TInt err = KErrNone;
	TRawEvent ev;

	// Check for modifiers
	if (!err && (aModifiers & EModifierCtrl))
		{
		ev.Set(TRawEvent::EKeyDown, EStdKeyLeftCtrl);
		err = UserSvr::AddEvent(ev);
		}
	if (!err && (aModifiers & EModifierShift))
		{
		ev.Set(TRawEvent::EKeyDown, EStdKeyLeftShift);
		err = UserSvr::AddEvent(ev);
		}

	// send the key down and up
	if (!err)
		{
		ev.Set(TRawEvent::EKeyDown, aScanCode);
		err = UserSvr::AddEvent(ev);
		}
	if (!err)
		{
		ev.Set(TRawEvent::EKeyUp, aScanCode);
		err = UserSvr::AddEvent(ev);			
		}

	// generate repeats if necessary
	/*
	for (TInt i = 0; i < aRepeatCount; ii++)
		{
		ev.Set(TRawEvent::EKeyDown, aScanCode);
		error = UserSvr::AddEvent(ev);
		if (error == KErrNone)
			{
			ev.Set(TRawEvent::EKeyUp, sc);
			error = UserSvr::AddEvent(ev);			
			}
		if (error != KErrNone)
			break;			
		}
	*/

	// Reset modifiers
	if (!err && (aModifiers & EModifierShift))
		{
		ev.Set(TRawEvent::EKeyUp, EStdKeyLeftShift);
		err = UserSvr::AddEvent(ev);
		}
	if (!err && (aModifiers & EModifierCtrl))
		{
		ev.Set(TRawEvent::EKeyUp, EStdKeyLeftCtrl);
		err = UserSvr::AddEvent(ev);
		}

	return err;
	}

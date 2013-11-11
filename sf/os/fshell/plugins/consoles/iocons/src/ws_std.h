// Copyright (c) 1995-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the License "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Cut down version of M:\sf\os\kernelhwsrv\kernel\eka\include\ws_std.h for iocons.
// 
//

/**
 @file
 @internalTechnology
*/

#ifndef __WS_STD_H__
#define __WS_STD_H__

#include <e32base.h>
#include <e32cons.h>
#include <e32twin.h>
#include <e32ver.h>
#include <e32svr.h>
//
//
const TInt KW32MajorVersionNumber=1;
const TInt KW32MinorVersionNumber=0;
const TInt KMessageSlots=3;
//
enum TConsolePanic
	{
	EConsServerFailed,
	EConsNotSupportedYet
	};
//
enum TWsPanic
	{
	EPrematureOperation,
	ETooManyWindowsOpen,
	EWindowTooWide,
	EWindowTooThin,
	EWindowTooHigh,
	EWindowTooShort,
	EWindowOutOfMemory,
	EDoubleReadRequest,
	};
//
enum TWsFault
	{
	ECreateScheduler,
	ECreateServer,
	EStartServer,
	ECreateEvent,
	ECreateShell,
	ESchedulerError,
	EWindowsInitialisation,
	ENoKeyboardTranslator,
	ECreateNotifierSemaphore,
	ECreateNotifierThread,
	ELocaleDll,
	EChangeLocale,
	};
//
class CWsSession : public CSession2
	{
public:
	enum
		{
		EConsoleCreate,
		EConsoleSet,
		EConsoleClearScreen,
		EConsoleClearToEndOfLine,
		EConsoleSetWindowPosAbs,
		EConsoleSetCursorHeight,
		EConsoleSetCursorPosAbs,
		EConsoleSetCursorPosRel,
		EConsoleCursorPos,
		EConsoleControl,
		EConsoleWrite,
		EConsoleRead,
		EConsoleReadCancel,
		EConsoleDestroy,
		EConsoleSetTitle,
		EConsoleSetSize,
		EConsoleSize,
		EConsoleScreenSize,
		EConsoleSetMode,
		EConsoleSetPaletteEntry,
		EConsoleGetPaletteEntry,
		EConsoleSetTextColors,
		EConsoleSetUIColors,
		EConsoleSetTextAttribute
		};
	};
//
GLREF_C TInt WindowServerThread(TAny *anArg);
GLREF_C void Panic(TWsPanic aPanic);
GLREF_C void Fault(TWsFault aFault);
//
IMPORT_C void Panic(TConsolePanic aPanic);

#endif	// __WS_STD_H__

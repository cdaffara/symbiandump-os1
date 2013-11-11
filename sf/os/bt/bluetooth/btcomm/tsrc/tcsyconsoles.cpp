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

#include "tcsy.h"

// ----------------------------------------------------------------------------
//							CActiveConsoleBase
// ----------------------------------------------------------------------------


CActiveConsoleBase::CActiveConsoleBase(CConsoleBase& aConsole)
: CActive(EPriorityStandard), iConsole(aConsole)
	{
	CActiveScheduler::Add(this);
	}

CActiveConsoleBase::~CActiveConsoleBase()
	{
	Cancel();
	}

void CActiveConsoleBase::DoCancel()
	{
	iConsole.ReadCancel();
	}

void CActiveConsoleBase::RequestKey()
	{
	DrawCursor();
	iConsole.Read(iStatus);
	SetActive();
	}

void CActiveConsoleBase::DrawCursor()
	{
	iConsole.Printf(_L(">>"));
	}


TInt CActiveConsoleBase::RunError(TInt /*aError*/)
	{
	__DEBUGGER();
	return KErrNone;
	}

// ----------------------------------------------------------------------------
//							CActiveRootConsole
// ----------------------------------------------------------------------------

void CActiveRootConsole::DrawMenu()
	{
	iConsole.Printf(_L("Choose action...\n\n"));
	iConsole.Printf(_L("n.\t\tStart RComm\n"));
	iConsole.Printf(_L("x.\t\tDelete all RComm\n"));
	iConsole.Printf(_L("a.\t\tAuto\n"));
	iConsole.Printf(_L("e.\t\tStart exclusive session\n"));
	iConsole.Printf(_L("f.\t\tStop exclusive session\n"));
	iConsole.Printf(_L("u.\t\tMark C32 User Heap\n"));
	iConsole.Printf(_L("v.\t\tMark-end C32 User Heap\n"));
	iConsole.Printf(_L("0.\t\tAccess Violation\n"));
	iConsole.Printf(_L("P.\t\tPanic\n"));
	iConsole.Printf(_L("T.\t\tHTTP Get\n"));
	iConsole.Printf(_L("Esc.\tStop\n"));
	RequestKey();
	}


CActiveRootConsole::CActiveRootConsole(CConsoleBase& aConsole, CCSYApp& aApp)
: CActiveConsoleBase(aConsole), iApp(aApp)
	{
	}

CActiveRootConsole* CActiveRootConsole::NewL(CConsoleBase& aConsole, CCSYApp& aApp)
	{
	CActiveRootConsole* console = new (ELeave) CActiveRootConsole(aConsole, aApp);
//	CleanupStack::PushL(console);
//	console->ConstructL();
//	CleanupStack::Pop();
	console->DrawMenu();
	return console;
	}


void CActiveRootConsole::RunL()
	{
	// key has been pressed

	TChar ch = iConsole.KeyCode();
	switch (ch)
		{
	case 'n':
		TimePrint();
		iConsole.Printf(_L("Starting RComm...\n"));
		iApp.NewCSYL();
		break;

	case 'x':
		TimePrint();
		iConsole.Printf(_L("Deleting all RComm...\n"));
		iApp.DeleteAllConnections();
		break;

	case 'a':
		TimePrint();
		iConsole.Printf(_L("Auto mode...\n"));
		iApp.AutoMode();
		break;

	case 'e':
		{
		TimePrint();
		iConsole.Printf(_L("Starting an exclusive session\n"));
		iApp.StartExclusiveSessionL();
		break;
		}

	case 'f':
		{
		TimePrint();
		iConsole.Printf(_L("Stopping the exclusive session\n"));
		iApp.StopExclusiveSession();
		break;
		}

	case 'u':
		{
		TimePrint();
		iConsole.Printf(_L("Mark C32 User Heap\n"));
		iApp.C32UHeapMark();
		break;
		}

	case 'v':
		{
		TimePrint();
		iConsole.Printf(_L("Mark-end C32 User Heap\n"));
		iApp.C32UHeapMarkEnd();
		break;
		}

	case '0':
		{
		iConsole.Printf(_L("Deref-ing NULL...\n"));
		TInt* boom = NULL;
		TInt detritus = *boom;
		(void)(detritus != NULL); // keep the compiler happy by taking detritus as an r-value
		break;
		}

	case 'P':
		{
		iConsole.Printf(_L("Panic!\n"));
		User::Panic(_L("PANIC!!"), 0x1234);
		}

	case 'T':
		{
		iConsole.Printf(_L("Doing a HTTP Get...\n"));
		iApp.FetchWebPageL();
		}

	case EKeyEscape:
		{
		iApp.Stop();
		goto end;
		}


	default:
		iConsole.Printf(_L("Unknown command\r\n"));
		}
	// schedule another key press
	RequestKey();

end:
	return;
	}




// ----------------------------------------------------------------------------
//							CActiveCommConsole
// ----------------------------------------------------------------------------

CActiveCommConsole::CActiveCommConsole(CConsoleBase& aConsole, CCSY& aCSY)
: CActiveConsoleBase(aConsole), iComm(aCSY)
	{
	}


CActiveCommConsole* CActiveCommConsole::NewL(CConsoleBase& aConsole, CCSY& aCSY)
	{
	CActiveCommConsole* console = new (ELeave) CActiveCommConsole(aConsole, aCSY);
//	CleanupStack::PushL(console);
//	console->ConstructL();
//	CleanupStack::Pop();
	console->DrawMenu();
	return console;
	}

void CActiveCommConsole::RunL()
	{
	// key has been pressed

	TChar ch = iConsole.KeyCode();
	TBool reque = ETrue;

	switch (ch)
		{
	case 'c':
		iConsole.Printf(_L("Cancel RComm...\n"));
//		iComm.CancelCSY();
		break;

	case 'r':
		iConsole.Printf(_L("RComm::Read...\n"));
		iComm.Read();
		break;

	case 'w':
		iConsole.Printf(_L("RComm::Write...\n"));
		break;

	case 'x':
		iComm.Destroy();
		reque = EFalse;
		break;


	default:
		iConsole.Printf(_L("Unknown command\r\n"));
		}
	if (reque)
		RequestKey();	// schedule another key press
	return;
	}

void CActiveCommConsole::DrawMenu()
	{
	iConsole.Printf(_L("c.\t\tCancel RComm \n"));
	iConsole.Printf(_L("r.\t\tRComm::Read\n"));
	iConsole.Printf(_L("w.\t\tRComm::Write\n"));
	iConsole.Printf(_L("q.\t\tRComm::Close\n"));

	iConsole.Printf(_L("x.\t\tDelete RComm\n"));
	RequestKey();
	}


void CActiveConsoleBase::TimePrint()
	{
	TBuf<16> time;

	iTime.HomeTime();

	TRAPD(err, iTime.FormatL(time, KDateFormatter));
	__ASSERT_ALWAYS(err==KErrNone, User::Panic(_L("Test thread"), 0x666));

	iConsole.Printf(_L("%S: "), &time);
	}

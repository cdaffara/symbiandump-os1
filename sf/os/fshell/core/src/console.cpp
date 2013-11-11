// console.cpp
// 
// Copyright (c) 2006 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#include <e32cons.h>
#include <fshell/ioutils.h>
#include "console.h"


//
// CConsoleReader.
//

CConsoleReader* CConsoleReader::NewL(RIoConsoleReadHandle& aStdin, MConsoleObserver& aObserver)
	{
	CConsoleReader* self = new(ELeave) CConsoleReader(aStdin, aObserver);
	self->Queue();
	return self;
	}

CConsoleReader::~CConsoleReader()
	{
	Cancel();
	}

CConsoleReader::CConsoleReader(RIoConsoleReadHandle& aStdin, MConsoleObserver& aObserver)
	: CActive(CActive::EPriorityStandard), iStdin(aStdin), iObserver(aObserver)
	{
	CActiveScheduler::Add(this);
	}

void CConsoleReader::Queue()
	{
	TInt result = iStdin.AttachedToConsole();
	if (result > 0)
		{
		iRawRead = ETrue;
		iStdin.Read(iRawReadBuf, iStatus);
		}
	else if (result == 0)
		{
		iRawRead = EFalse;
		iStdin.WaitForKey(iStatus);
		}
	else
		{
		// This seems to happen if the console failed to construct
		// [I've seen a E32USER-CBase 45 panic as a result of this call so possibly it's not always the right thing to do (I added this in the first place). -TomS]
		iObserver.CoHandleError(result);
		return;
		}
	SetActive();
	}

void CConsoleReader::RunL()
	{
	TInt err = iStatus.Int();
	if (err)
		{
		iObserver.CoHandleError(err);
		}
	else
		{
		TUint keyCode = iRawRead ? iRawReadBuf[0] : iStdin.KeyCode();
		TUint modifiers = iRawRead ? 0 : iStdin.KeyModifiers();
		iObserver.CoHandleKey(keyCode, modifiers);
		}
	Queue();
	}

void CConsoleReader::DoCancel()
	{
	if (iRawRead)
		{
		iStdin.ReadCancel();
		}
	else
		{
		iStdin.WaitForKeyCancel();
		}
	}

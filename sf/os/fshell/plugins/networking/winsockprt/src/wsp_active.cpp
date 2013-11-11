// wsp_active.cpp
// 
// Copyright (c) 2002 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//


#include "_windows.h"
#include "wsp_active.h"
#include "wsp_scheduler.h"
#include "wsp_panic.h"


CWin32ActiveObject::CWin32ActiveObject(CWin32Scheduler& aScheduler)
	: iScheduler(aScheduler)
	{
	}

CWin32ActiveObject::~CWin32ActiveObject()
	{
	if (iScheduler.IsPresent(*this))
		{
		ReleaseEventHandle();
		}
	}

void CWin32ActiveObject::ConstructL()
	{
	iEvent = CreateEvent(NULL, EFalse, EFalse, NULL);
	if (iEvent == NULL)
		{
		User::Leave(KErrBadHandle);
		}
	iScheduler.Add(*this);
	}

void CWin32ActiveObject::ReleaseEventHandle()
	{
	if (!CloseHandle(iEvent))
		{
		__ASSERT_DEBUG(EFalse, Panic(EWinSockPrtUnexpectedCWin32ActiveObjectCloseHandleError));
		}
	iScheduler.Remove(*this);
	}

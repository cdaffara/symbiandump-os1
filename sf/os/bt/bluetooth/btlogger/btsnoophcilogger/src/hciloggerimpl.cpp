// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "hciloggerbtsnoop.h"

EXPORT_C MHCIFrameLogger* CHCILoggerBtSnoop::GetInstanceL()
	{
	CHCILoggerBtSnoop* self = static_cast<CHCILoggerBtSnoop*>(Dll::Tls());
	if (!self)
		{
		self = NewLC();
		User::LeaveIfError(Dll::SetTls(self));
		CleanupStack::Pop(self);
		}
	++(self->iRefCount);
	return self;		
	}

void CHCILoggerBtSnoop::SetLibrary(RLibrary& aLib)
	{
	if(iRefCount > 1)
		{
		__ASSERT_DEBUG(iMyLib.Handle(), User::Panic(_L("BtSnoopLogger"), EBtSnoopLoggerPanicNoMyLibHandle));
		return;
		}
	__ASSERT_DEBUG(!iMyLib.Handle(), User::Panic(_L("BtSnoopLogger"), EBtSnoopLoggerPanicHandleAlreadyExists));
	iMyLib.SetHandle(aLib.Handle());
	aLib.SetHandleNC(iMyLib.Handle());
	}

void CHCILoggerBtSnoop::GetLibrary(RLibrary& aLib)
	{
	if(iRefCount > 1)
		{
		aLib.SetHandleNC(iMyLib.Handle());
		}
	else // Give final client full control of the handle!
		{
		aLib.SetHandle(iMyLib.Handle());
		}
	}

void CHCILoggerBtSnoop::Release()
	{
	__ASSERT_DEBUG((iRefCount > 0), User::Panic(_L("BtSnoopLogger"), EBtSnoopLoggerPanicRefCountTooLow));
	--iRefCount;
	if(iRefCount == 0)
		{
		delete this;
		Dll::SetTls(0);
		}
	}

void CHCILoggerBtSnoop::Initialise(TInt aType)
	{
	if (iRefCount > 1)
		{
		__ASSERT_DEBUG(iInitialised, User::Panic(_L("BtSnoopLogger"), EBtSnoopLoggerPanicHasntBeenInitialised));
		return;
		}
	__ASSERT_DEBUG(!iInitialised, User::Panic(_L("BtSnoopLogger"), EBtSnoopLoggerPanicShouldNotBeInitialised));
	DoInitialise(aType);
	iInitialised = ETrue;	// this flag implies the DoInitialise() function has
							// been called, but not that it was necessarily
							// successful.
	}


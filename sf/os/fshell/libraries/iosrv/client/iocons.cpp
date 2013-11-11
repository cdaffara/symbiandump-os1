// iocons.cpp
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

#include <fshell/iocli.h>
#include <fshell/consoleextensions.h>
#include "iocons.h"
#include <fshell/ioutils.h>
#include <fshell/common.mmh>
#ifdef FSHELL_MEMORY_ACCESS_SUPPORT
#include <fshell/memoryaccess.h>
#endif

// This constant is used to find the PIPS Stdio Server CServer2 object in the situation
// where iocons in running in the stdioserver.exe process. Normally, when iocons calls
// RIo(Read|Write)Handle::Open(RIoSession&), iosrv looks to see if it hand any read or
// write objects that belong to the calling thread. Note, when fshell creates a new process
// it creates a set of read and write objects inside iosrv, and assigns them as being owned
// by the main thread of the new process. However, in the case of PIPS, the main thread
// of the process that fshell creates isn't the one that opens the console. This is done
// from the context of the sdioserver.exe process. To cater for this, iocons has a special
// behaviour when its running in the context of sdioserver.exe. This involves finding the
// server's CServer2 sub-class object, and using this to retrieve the current RMessage2
// object. From this, the client's thread id can be found. This is passed into a different
// RIo(Read|Write)Handle::Open overload that attempt to open a handle using the specified
// thread id (not that which iocons is currently running in). If fshell created the process
// this should return without error, in which case the PIPS process will be able to share
// fshell's console (or take part in a pipe-line that it created). If not, then the old
// behaviour of creating a new console will proceed.
const TInt KPipsStdioServerAllocPos = 5;


CIoConsole::CIoConsole()
	{
	}

CIoConsole::CIoConsole(RIoConsoleReadHandle& aReadHandle, RIoConsoleWriteHandle& aWriteHandle)
	{
	iReadHandle = aReadHandle;
	iWriteHandle = aWriteHandle;
	}

CIoConsole::~CIoConsole()
	{
	if (iIoSession.Handle())
		{
		// If the handle isn't set it means we don't own iReadHandle/iWriteHandle (or they haven't been constructed and we needn't close them anyway)
		iReadHandle.Close();
		iWriteHandle.Close();
		iConsole.Close();
		iIoSession.Close();
		}
	}

TInt CIoConsole::Create(const TDesC& aTitle, TSize aSize)
	{
	// This should only be called via iocons, ie when the zero arguments constructor was used.
	// When the 2-arg constructor is used, the console is fully constructed and Create should not be called
	TInt err = iIoSession.Connect();
	if (err == KErrNone)
		{
		// See if iosrv has any handles ready for us
		err = iReadHandle.Open(iIoSession);
		if (err)
			{
			// If not, check if we're being opened from the stdioserver on behalf of the client PIPS exe
			TThreadId clientThreadId;
			if (FindClientThreadId(clientThreadId) == KErrNone)
				{
				err = iReadHandle.Open(iIoSession, clientThreadId);
				if (err == KErrNone)
					{
					err = iWriteHandle.Open(iIoSession, clientThreadId);
					if (err != KErrNone)
						{
						iReadHandle.Close();
						}
					}
				}

#ifdef FSHELL_MEMORY_ACCESS_SUPPORT
			if (err)
				{
				// Try and see if the thread that created us has some handles we can use, and if not, the thread that created it etc until we find one or lose track
				RMemoryAccess::LoadDriver();
				RMemoryAccess memAccess;
				err = memAccess.Open();
				if (!err) err = iReadHandle.Create(iIoSession);
				if (!err) iWriteHandle.Create(iIoSession);
				if (!err)
					{
					TUint creator = RThread().Id();
					while (creator != 0)
						{
						creator = memAccess.GetThreadCreatorId(creator);
						err = iReadHandle.DuplicateHandleFromThread(TThreadId(creator));
						if (err == KErrNone)
							{
							// Found one - open the writer too...
							err = iWriteHandle.DuplicateHandleFromThread(TThreadId(creator));
							break; // Stop looking if we found one
							}
						}
					memAccess.Close();
					}
				if (err)
					{
					iReadHandle.Close();
					iWriteHandle.Close();
					}
				}
#endif

			if (err)
				{
				// If all else fails, create a new console
				err = CreateNewConsole(aTitle, aSize);
				}
			}
		else
			{
			err = iWriteHandle.Open(iIoSession);
			}
		}
	if (err)
		{
		iReadHandle.Close();
		iWriteHandle.Close();
		iConsole.Close();
		iIoSession.Close();
		}
	return err;
	}

TInt CIoConsole::CreateNewConsole(const TDesC& aTitle, TSize aSize)
	{
	TInt err = iConsole.Create(iIoSession, aTitle, aSize);
	if (err) return err;
	err = iReadHandle.Create(iIoSession);
	if (err) return err;
	err = iWriteHandle.Create(iIoSession);
	if (err) return err;
	err = iConsole.Attach(iReadHandle, RIoEndPoint::EForeground);
	if (err) return err;
	err = iConsole.Attach(iWriteHandle);
	if (err) return err;

	err = iReadHandle.SetOwner(RThread().Id());
	if (err) return err;
	err = iWriteHandle.SetOwner(RThread().Id());
	
	return err;
	}

TBool CIoConsole::HeapWalk(TAny* aSelf, RAllocatorHelper::TCellType aCellType, TLinAddr aCellPtr, TInt)
	{
	CIoConsole* self = (CIoConsole*)aSelf;
	if (aCellType == RAllocatorHelper::EAllocation)
		{
		if (++(self->iHeapCellCount) == KPipsStdioServerAllocPos)
			{
			self->iServerAddress = (TAny*)aCellPtr;
			return EFalse; // To indicate we're finished
			}
		}
	return ETrue; // keep going
	}

class CHackedServer : public CServer2
	{
public:
	inline CHackedServer() : CServer2(CActive::EPriorityStandard) {} // needed to shut up gcce
	inline const RMessage2& Msg() const { return Message(); }
	};

TInt CIoConsole::FindClientThreadId(TThreadId& aThreadId)
	{
	TInt err = KErrNotFound;
	_LIT(KPipsStdioServer, "stdioserver.exe*");
	TName processName(RProcess().Name());
	if (processName.MatchF(KPipsStdioServer) == 0)
		{
		LtkUtils::RAllocatorHelper allocHelper;
		TInt err = allocHelper.Open(&User::Allocator());
		if (!err) err = allocHelper.Walk(&HeapWalk, this);
		allocHelper.Close();
		if (err == KErrNone && iServerAddress)
			{
			// This means that we found the server's address (possibly).
			CHackedServer* server = (CHackedServer*)iServerAddress;
			RThread client;
			if (server->Msg().Client(client) == KErrNone)
				{
				err = KErrNone;
				aThreadId = client.Id();
				client.Close();
				}
			}
		}
	return err;
	}

void CIoConsole::Read(TRequestStatus& aStatus)
	{
	iReadHandle.WaitForKey(aStatus);
	}

void CIoConsole::ReadCancel()
	{
	iReadHandle.WaitForKeyCancel();
	}

void CIoConsole::Write(const TDesC& aDes)
	{
	iWriteHandle.Write(aDes);
	}

TPoint CIoConsole::CursorPos() const
	{
	TPoint pos;
	iWriteHandle.GetCursorPos(pos);
	return pos;
	}

void CIoConsole::SetCursorPosAbs(const TPoint& aPoint)
	{
	iWriteHandle.SetCursorPosAbs(aPoint);
	}

void CIoConsole::SetCursorPosRel(const TPoint& aPoint)
	{
	iWriteHandle.SetCursorPosRel(aPoint);
	}

void CIoConsole::SetCursorHeight(TInt aPercentage)
	{
	iWriteHandle.SetCursorHeight(aPercentage);
	}

void CIoConsole::SetTitle(const TDesC& aTitle)
	{
	iWriteHandle.SetTitle(aTitle);
	}

void CIoConsole::ClearScreen()
	{
	iWriteHandle.ClearScreen();
	}

void CIoConsole::ClearToEndOfLine()
	{
	iWriteHandle.ClearToEndOfLine();
	}

TSize CIoConsole::ScreenSize() const
	{
	TSize size;
	iWriteHandle.GetScreenSize(size);
	return size;
	}

TKeyCode CIoConsole::KeyCode() const
	{
	return (TKeyCode)iReadHandle.KeyCode();
	}

TUint CIoConsole::KeyModifiers() const
	{
	return iReadHandle.KeyModifiers();
	}

TInt CIoConsole::Extension_(TUint aExtensionId, TAny*& a0, TAny* a1)
	{
	return CConsoleBase::Extension_(aExtensionId, a0, a1);
	}

EXPORT_C CConsoleBase* IoUtils::NewConsole()
	{
	return new CIoConsole;
	}

// This is horrible, but only way to satisfy WINSCW compiler. If we define it always, we end up re-exporting typeinfo for CColorConsoleBase in armv5 which we don't want to be doing
#ifdef __WINS__
void CColorConsoleBase::SetTextAttribute(TTextAttribute)
	{
	}
#endif

void CIoConsole::SetTextAttribute(TTextAttribute aAttribute)
	{
	TUint attrib = 0;
	switch (aAttribute)
		{
	case ETextAttributeNormal:
		attrib = ConsoleAttributes::ENone;
		break;
	case ETextAttributeBold:
		attrib = ConsoleAttributes::EBold;
		break;
	case ETextAttributeInverse:
		attrib = ConsoleAttributes::EInverse;
		break;
	case ETextAttributeHighlight:
		// Should highlight map to inverse? Seems slightly better than bold
		attrib = ConsoleAttributes::EInverse;
		break;
	default:
		break;
		}
	iWriteHandle.SetAttributes(attrib); // Can't do anything with the error
	}

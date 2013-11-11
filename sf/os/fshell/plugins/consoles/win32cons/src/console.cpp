// console.cpp
// 
// Copyright (c) 2008 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#include <e32debug.h>
#include "console.h"
#include <fshell/consoleextensions.h>

_LIT(KWin32ConsPanic, "Win32Console");
#define PANIC() User::Panic(KWin32ConsPanic, 0);

//_____________________________________________________________________________
//					CWin32Console
CWin32Console::CWin32Console()
	{
	}

CWin32Console::~CWin32Console()
	{
	iWin32.FreeConsole();
	// when call FreeConsole(), it should cause the reader thread to exit as the
	// console read handle will become invalid, and windows will complete the
	// ReadRequest with an error
	if (iReaderThread.Handle())
		{
		TRequestStatus stat;
		iReaderThread.Logon(stat);
		User::WaitForRequest(stat);
		}
	iReaderThread.Close();
	iKeyQueue.Close();
	iThreadParams.iOwner.Close();
	}

TInt CWin32Console::Create(const TDesC& aTitle, TSize /*aSize*/)
	{
	int r = iWin32.AttachConsole();
	if (r==0)
		{
		return KErrCouldNotConnect; // suitable error code??
		}
	SetTitle(aTitle);
	
	_LIT(KReadThreadName, "Win32ConsReader");
	
	TInt err = iKeyQueue.CreateLocal(4);
	if (err!=KErrNone) return err;
	
	err = iThreadParams.iOwner.Open(RThread().Id());
	if (err!=KErrNone) return err;
	iThreadParams.iWin32 = &iWin32;
	iThreadParams.iKeyQueue = iKeyQueue;
	err = iReaderThread.Create(KReadThreadName, ReaderThread, 0x800, KMinHeapSize, KMinHeapSize*4, &iThreadParams);
	if (err!=KErrNone) return err;
	iReaderThread.Resume();	
	
	return KErrNone;
	}

void CWin32Console::Read(TRequestStatus& aStatus)
	{
	iReadKp = EFalse;
	iKeyQueue.NotifyDataAvailable(aStatus);
	}

void CWin32Console::ReadCancel()
	{
	iKeyQueue.CancelDataAvailable();
	}

void CWin32Console::Write(const TDesC& aDes)
	{
	iWin32.Write(aDes.Ptr(), aDes.Length());
	}
	
TInt CWin32Console::WriteStdErr(const TDesC& aDes)
	{
	iWin32.WriteStdErr(aDes.Ptr(), aDes.Length());
	return KErrNone;
	}

TPoint CWin32Console::CursorPos() const
	{
	TPoint pos;
	iWin32.GetCursorPos(pos.iX, pos.iY);
	return pos;
	}

void CWin32Console::SetCursorPosAbs(const TPoint& aPoint)
	{
	iWin32.SetCursorPos(aPoint.iX, aPoint.iY);
	}

void CWin32Console::SetCursorPosRel(const TPoint& aPoint)
	{
	iWin32.SetCursorPosRel(aPoint.iX, aPoint.iY);
	}

void CWin32Console::SetCursorHeight(TInt aPercentage)
	{
	iWin32.SetCursorHeight(aPercentage);
	}

void CWin32Console::SetTitle(const TDesC& aTitle)
	{
	RBuf8 title;
	TInt err = title.Create(aTitle.Length()+1);
	if (err==KErrNone)
		{
		title.Copy(aTitle);
		iWin32.SetTitle((const char*)title.PtrZ());
		title.Close();
		}
	}

void CWin32Console::ClearScreen()
	{
	iWin32.ClearScreen();
	}

void CWin32Console::ClearToEndOfLine()
	{
	iWin32.ClearToEndOfLine();
	}

TSize CWin32Console::ScreenSize() const
	{
	TSize size;
	iWin32.GetScreenSize(size.iWidth, size.iHeight);
	return size;
	}

TKeyCode CWin32Console::KeyCode() const
	{
	if (!iReadKp)
		{
		iKeyQueue.Receive(iKp);
		iReadKp = ETrue;
		}
	return iKp.iCode;
	}

TUint CWin32Console::KeyModifiers() const
	{
	if (!iReadKp)
		{
		iKeyQueue.Receive(iKp);
		iReadKp = ETrue;
		}
	return iKp.iModifiers;
	}

TInt CWin32Console::Extension_(TUint aExtensionId, TAny*& a0, TAny* a1)
	{
	if (aExtensionId == ConsoleAttributes::KSetConsoleAttributesExtension)
		{
		ConsoleAttributes::TAttributes* attributes = (ConsoleAttributes::TAttributes*)a1;
		int res = iWin32.SetAttributes(attributes->iAttributes, (TWin32Console::TColor)attributes->iForegroundColor, (TWin32Console::TColor)attributes->iBackgroundColor);
		return res ? KErrNone : KErrGeneral; // Yay for KErrGeneral!
		}
	else
		{
		TInt ret = MIosrvConsoleHelper_Extension(aExtensionId, a0, a1);
		if (ret == KErrExtensionNotSupported)
			{
			ret = CConsoleBase::Extension_(aExtensionId, a0, a1);
			}
		return ret;
		}
	
	}

//_____________________________________________________________________________
//					Reader thread

static int ReaderThread(TAny* aParams)
	{
	TReaderThreadParams params = *(TReaderThreadParams*)aParams;
	
	RMsgQueue<TKeyPress> keyQueue = params.iKeyQueue;
	TInt err = keyQueue.Duplicate(params.iOwner);
	
	while (err==KErrNone)
		{
		TKeyPress kp;
		err = params.iWin32->ReadKey(kp.iCode, kp.iModifiers);
		
		if (err==KErrNone)
			{
			keyQueue.SendBlocking(kp);
			}

		};
	
	keyQueue.Close();
	
	return err;
	}

//_____________________________________________________________________________
//					Debugging stuff
EXPORT_C TAny* NewConsole()
	{
	return new CWin32Console;
	}

class TOverflowTruncate8 : public TDes8Overflow
	{
public:
	virtual void Overflow(TDes8&) {}
	};

class TOverflowTruncate16 : public TDes16Overflow
	{
public:
	virtual void Overflow(TDes16&) {}
	};

void DebugMsg(const char* aMsg, ...)
	{
	VA_LIST list;
	VA_START(list, aMsg);
	TBuf8<0x100> buf;
	TOverflowTruncate8 overflow;
	buf.AppendFormatList(_L8(aMsg), list, &overflow);

	RDebug::Printf((const char*)buf.PtrZ());
	}

void DebugMsg(const TUint16* aMsg, ...)
	{
	VA_LIST list;
	VA_START(list, aMsg);
	TBuf16<0x100> buf;
	TOverflowTruncate16 overflow;
	buf.AppendFormatList(TPtrC((const TText16*)aMsg), list, &overflow);

	RDebug::Print(buf);
	}


#ifndef EKA2
GLDEF_C TInt E32Dll(TDllReason)
	{
	return(KErrNone);
	}
#endif

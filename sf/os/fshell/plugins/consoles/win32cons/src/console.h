// console.h
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
#include <e32msgqueue.h>
#include <fshell/consoleextensions.h>
#include "win32cons.h"


class TKeyPress
	{
public:
	TKeyCode iCode;
	TUint iModifiers;
	};

class TReaderThreadParams
	{
public:
	TWin32Console* iWin32;
	RMsgQueue<TKeyPress> iKeyQueue;
	RThread iOwner;
	};
	
	
/**
Symbian CConsoleBase implementation that uses a Win32 console.

It supports all of the CConsoleBase API. It also supports piping of test written
to the console into a Win32 file or process, if the emulator is launched from a
script in an appropriate manner.

More specifally:
- Redirection to a file: i.e. if the emulator is launched the emulator STDOUT
  piped to a file, data written to this console will end up in that file
- Attaching to the console of the parent process: i.e. when the emulator is
  launched from a windows Command Prompt box, output will appear there
- Creating a new console, if the emulator is lauched from a process with no
  console. This will happen if the emulator is launched from an IDE.
  
Piping into the emulator process via this console is as yet untested.
*/
NONSHARABLE_CLASS(CWin32Console) : public CConsoleBase, public MIosrvConsoleHelper
	{
public:
	CWin32Console();
public: // From CConsoleBase.
	virtual ~CWin32Console();
	virtual TInt Create(const TDesC& aTitle, TSize aSize);
	virtual void Read(TRequestStatus& aStatus);
	virtual void ReadCancel();
	virtual void Write(const TDesC& aDes);
	virtual TPoint CursorPos() const;
	virtual void SetCursorPosAbs(const TPoint& aPoint);
	virtual void SetCursorPosRel(const TPoint& aPoint);
	virtual void SetCursorHeight(TInt aPercentage);
	virtual void SetTitle(const TDesC& aTitle);
	virtual void ClearScreen();
	virtual void ClearToEndOfLine();
	virtual TSize ScreenSize() const;
	virtual TKeyCode KeyCode() const;
	virtual TUint KeyModifiers() const;
	virtual TInt Extension_(TUint aExtensionId, TAny*& a0, TAny* a1);
	
private: // From MIosrvConsoleHelper
	TInt WriteStdErr(const TDesC& aDes);

private:
	TInt CreateNewConsole(const TDesC& aTitle, TSize aSize);
	TInt FindClientThreadId(TThreadId& aThreadId);
	static void HeapWalk(TAny* aSelf, RHeap::TCellType aCellType, TAny* aCellPtr, TInt aLength);
private:
	TWin32Console iWin32;
	mutable RMsgQueue<TKeyPress> iKeyQueue;
	RThread iReaderThread;
	TReaderThreadParams iThreadParams;
	
	mutable TBool iReadKp;
	mutable TKeyPress iKp;
	};
	
static int ReaderThread(TAny* aParams);


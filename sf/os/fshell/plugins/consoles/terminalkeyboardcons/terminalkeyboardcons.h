// terminalkeyboardcons.h
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
#ifndef TERMINALKEYBOARDCONS_H
#define TERMINALKEYBOARDCONS_H

#include <e32std.h>
#include <e32cons.h>
#include <TcDriverIf.h>
#include <fshell/consoleextensions.h>
#include "vtc_cursor_tracker.h"
class CMessageWatcher;

//#define SHOW_TEXTSHELL_BORDERS

NONSHARABLE_CLASS(CTerminalKeyboardCons) : public CConsoleBase, public MIosrvConsoleHelper, public MConsoleScrollHandler
	{
public:
	CTerminalKeyboardCons();
	virtual ~CTerminalKeyboardCons();

public: // From CConsoleBase
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

private: // From MConsoleScrollHandler
	void ConsoleScrolled(TInt aNumberOfLines);

private:
	virtual void ConstructL(const TDesC& aTitle, const TSize& aSize);
	void Update();
	void DoUpdate();
	void MessageReceived(TInt aError);
	void Transmit(const TDesC& aBuf, TInt aWidth, TInt aHeight);
	static TInt UpdateCallback(TAny* aSelf);

protected:
	TRequestStatus* iClientStatus;
	TBool iGotKey;
	TKeyCode iKeyCode;
	TUint iKeyModifiers;
	RTcDriver iDriver;

	RBuf iTextBuffer;
	RBuf iMungedTextBuffer; // With the borders
	TCursorTracker iTracker;
	CMessageWatcher* iWatcher;
	friend class CMessageWatcher;
	TBool iBacktickModifierDown;
	CPeriodic* iIdleUpdateTimer;
	};

#endif // TERMINALKEYBOARDCONS_H

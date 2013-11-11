// terminalkeyboardcons.cpp
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
#include "terminalkeyboardcons.h"
#include <fshell/common.mmh>
#include <e32debug.h>
#include <fshell/memoryaccess.h>

const TInt KTkbdMessageId = 0xD6;
// These are the defaults unless overridden by --console-size
const TInt KWidth = 80;
const TInt KHeight = 24;

class CMessageWatcher : public CActive
	{
public:
	CMessageWatcher(CTerminalKeyboardCons& aConsole)
		: CActive(CActive::EPriorityStandard), iConsole(aConsole)
		{
		CActiveScheduler::Add(this);
		iConsole.iDriver.RequestMessage(iStatus);
		SetActive();
		}

	void DoCancel()
		{
		iConsole.iDriver.CancelMessage();
		}

	void RunL()
		{
		iConsole.MessageReceived(iStatus.Int());
		if (iStatus.Int() == KErrNone)
			{
			iConsole.iDriver.RequestMessage(iStatus);
			SetActive();
			}
		}

	~CMessageWatcher()
		{
		Cancel();
		}

private:
	CTerminalKeyboardCons& iConsole;
	};


CTerminalKeyboardCons::CTerminalKeyboardCons()
	: iTracker(TSize(KWidth, KHeight), this)
	{
	}

CTerminalKeyboardCons::~CTerminalKeyboardCons()
	{
	delete iIdleUpdateTimer;
	CleanupUnderlyingConsole();
	delete iWatcher;
	iDriver.Close();
	iTextBuffer.Close();
	iMungedTextBuffer.Close();
	}

TInt CTerminalKeyboardCons::Create(const TDesC& aTitle, TSize aSize)
	{
	TRAPD(err, ConstructL(aTitle, aSize));
	HandleConsoleCreationError(_L("TerminalKeyboard"), err);
	return err;
	}

void CTerminalKeyboardCons::ConstructL(const TDesC& aTitle, const TSize& aSize)
	{
	if (aTitle == _L("debug")) SetDebug(ETrue);
	if (aSize.iWidth > 10 && aSize.iHeight > 10)
		{
		// Override size now we know it, checking that it's sane-ish - be sure to do this before anything that uses the console size!
		new (&iTracker) TCursorTracker(aSize, this);
		}

	iTextBuffer.CreateMaxL(ScreenSize().iWidth * ScreenSize().iHeight);
#ifdef SHOW_TEXTSHELL_BORDERS
	iMungedTextBuffer.CreateL((ScreenSize().iWidth + 2) * (ScreenSize().iHeight + 2));
#endif

	iIdleUpdateTimer = CPeriodic::NewL(CActive::EPriorityLow); // Lower priority than the message watcher

	TInt err = User::LoadLogicalDevice(KTcLddDriverName);
	if (err && err != KErrAlreadyExists)
		{
		Message(EError, _L("Couldn't load LDD %S: %d"), &KTcLddDriverName, err);
		User::Leave(err);
		}

	// Annoying driver only accepts connections from processes with nokia vid - like that will stop us
	TUint originalVid = RProcess().VendorId();
	RMemoryAccess memAccess;
	User::LeaveIfError(memAccess.Open());
	TProcessProperties props;
	props.iVid = 0x101FB657;
	RProcess me;
	User::LeaveIfError(me.Open(RProcess().Id()));
	memAccess.SetProcessProperties(me, props);

	err = iDriver.Open();

	props.iVid = originalVid;
	memAccess.SetProcessProperties(me, props);
	me.Close();
	memAccess.Close();

	if (err)
		{
		Message(EError, _L("Couldn't open RTcDriver: %d"), err);
		User::Leave(err);
		}

	err = iDriver.Subscribe(KTkbdMessageId);
	if (err)
		{
		Message(EError, _L("Couldn't subscribe: %d"), err);
		User::Leave(err);
		}

	iWatcher = new(ELeave) CMessageWatcher(*this);
	CleanupUnderlyingConsole();

	ClearScreen();
	}

	
TInt CTerminalKeyboardCons::Extension_(TUint aExtensionId, TAny*& a0, TAny* a1)
	{
	/*if (aExtensionId == ConsoleMode::KSetConsoleModeExtension)
		{
		ConsoleMode::TMode mode = (ConsoleMode::TMode)(TInt)a1;
		iInputController->SetMode(mode);
		iOutputController->SetMode(mode);
		return KErrNone;
		}
	else*/
	/*else if (aExtensionId == ConsoleAttributes::KSetConsoleAttributesExtension)
		{
		ConsoleAttributes::TAttributes* attributes = (ConsoleAttributes::TAttributes*)a1;
		return iOutputController->SetAttributes(attributes->iAttributes, attributes->iForegroundColor, attributes->iBackgroundColor);
		}*/
	
	TInt ret = MIosrvConsoleHelper_Extension(aExtensionId, a0, a1);
	if (ret == KErrExtensionNotSupported) ret = CConsoleBase::Extension_(aExtensionId, a0, a1);
	return ret;
	}

void CTerminalKeyboardCons::MessageReceived(TInt aError)
	{
	Message(EDebug, _L("MessageReceived err=%d"), aError);
	TInt err = aError;
	if (err == KErrNone)
		{
		TPtrC8 data;
		err = iDriver.GetMessageData(data);
		if (!err && data.Length() < 1) err = KErrCorrupt;
		if (!err && !iGotKey)
			{
			TUint16 rawkey = data[0];
			Message(EDebug, _L("CTerminalKeyboardCons got key %d"), (TInt)rawkey);

			// Terminal keyboard doesn't support control keys or cursor keys so we use our own two-stage sticky modifier (like unix meta key I think?)
			if (iBacktickModifierDown)
				{
				// Terminal keyboard sends 2,4,6,8 for cursors, so we translate backtick-2 (ie backtick-leftarrow) as meaning left arrow
				if (rawkey == '2') rawkey = (TUint16)EKeyUpArrow;
				else if (rawkey == '4') rawkey = (TUint16)EKeyLeftArrow;
				else if (rawkey == '6') rawkey = (TUint16)EKeyRightArrow;
				else if (rawkey == '8') rawkey = (TUint16)EKeyDownArrow;
				else if (rawkey >= 'a' && rawkey <= 'z')
					{
					// backtick-c means CTRL-C
					rawkey = rawkey - 'a'+1;
					}
				else if (rawkey == ' ') rawkey = '`'; // backtick-space is how you do a backtick
				else if (rawkey == '`') rawkey = (TUint16)EKeyEscape; // backtick-backtick is 'escape'
				else if (rawkey == '1') rawkey = (TUint16)EKeyTab; // backtick-1 is 'tab'
				iBacktickModifierDown = EFalse;
				Message(EDebug, _L("Backtick escape converted to %d"), (TInt)rawkey);
				}
			else if (rawkey == '`')
				{
				iBacktickModifierDown = ETrue;
				return;
				}

			iKeyCode = (TKeyCode)rawkey; // Close enough for now!
			iGotKey = ETrue;
			}
		}
	if (iClientStatus)
		{
		User::RequestComplete(iClientStatus, err);
		if (err == KErrNone) iGotKey = EFalse;
		}
	}

void CTerminalKeyboardCons::Read(TRequestStatus& aStatus)
	{
	/*if (iClientStatus)
		{
		TRequestStatus* stat = &aStatus;
		User::RequestComplete(stat, KErrInUse);
		}
	else*/
		{
		aStatus = KRequestPending;
		iClientStatus = &aStatus;
		if (iGotKey)
			{
			iGotKey = EFalse;
			User::RequestComplete(iClientStatus, KErrNone);
			}
		}
	}

void CTerminalKeyboardCons::ReadCancel()
	{
	if (iClientStatus)
		{
		User::RequestComplete(iClientStatus, KErrCancel);
		}
	}

void CTerminalKeyboardCons::Write(const TDesC& aDes)
	{
	for (TInt i = 0; i < aDes.Length(); i++)
		{
		TInt textBufPos = CursorPos().iY * ScreenSize().iWidth + CursorPos().iX;
		TChar ch(aDes[i]);
		iTracker.WriteChar(ch);
		if (ch.IsPrint()) iTextBuffer[textBufPos] = aDes[i];
		}

	Update();
	}
	
TPoint CTerminalKeyboardCons::CursorPos() const
	{
	return iTracker.CursorPos();
	}

void CTerminalKeyboardCons::SetCursorPosAbs(const TPoint& aPoint)
	{
	iTracker.SetCursorPosAbs(aPoint);
	}

void CTerminalKeyboardCons::SetCursorPosRel(const TPoint& aPoint)
	{
	iTracker.SetCursorPosRel(aPoint);
	}

void CTerminalKeyboardCons::SetCursorHeight(TInt /*aPercentage*/)
	{
	//iOutputController->SetCursorHeight(aPercentage);
	}

void CTerminalKeyboardCons::SetTitle(const TDesC& /*aDes*/)
	{
	//iOutputController->SetTitle(aDes);
	}

void CTerminalKeyboardCons::ClearScreen()
	{
	SetCursorPosAbs(TPoint(0,0));
	iTextBuffer.Fill(' ');
	Update();
	}

void CTerminalKeyboardCons::ClearToEndOfLine()
	{
	iTextBuffer.MidTPtr(CursorPos().iY * ScreenSize().iWidth + CursorPos().iX, ScreenSize().iWidth - CursorPos().iX).Fill(' ');
	Update();
	}

TSize CTerminalKeyboardCons::ScreenSize() const
	{
	return iTracker.ConsoleSize();
	}

TKeyCode CTerminalKeyboardCons::KeyCode() const
	{
	return iKeyCode;
	}

TUint CTerminalKeyboardCons::KeyModifiers() const
	{
	return iKeyModifiers;
	}

// All these magic vals are as per the DOS 437 codepage http://en.wikipedia.org/wiki/CP437 and not Windows-1252 like the ws_win.cpp source incorrectly states
// See also http://en.wikipedia.org/wiki/Box_drawing_characters
enum
	{
	EBoxHorizontalBar = 0xCD,
	EBoxVerticalBar = 0xBA,
	EBoxTopLeft = 0xC9,
	EBoxTopRight = 0xBB,
	EBoxBottomLeft = 0xC8,
	EBoxBottomRight = 0xBC,
	};

void CTerminalKeyboardCons::Update()
	{
	// Delay updates to the screen, to improve performance
	static const TInt KDelay = 100000; // 0.1s
	if (!iIdleUpdateTimer->IsActive())
		{
		// Don't reset timer if we're already running - otherwise constant typing will never show up until you stop pressing keys
		iIdleUpdateTimer->Start(KDelay, KDelay, TCallBack(&UpdateCallback, this));
		}
	}

TInt CTerminalKeyboardCons::UpdateCallback(TAny* aSelf)
	{
	static_cast<CTerminalKeyboardCons*>(aSelf)->DoUpdate();
	return 0;
	}

void CTerminalKeyboardCons::DoUpdate()
	{
	iIdleUpdateTimer->Cancel(); // Stop any further updates

#ifdef SHOW_TEXTSHELL_BORDERS
	// Update munged buffer
	const TInt contentWidth = ScreenSize().iWidth;
	const TInt width = contentWidth + 2;
	iMungedTextBuffer.SetLength(width);
	iMungedTextBuffer.Fill(TChar(EBoxHorizontalBar));
	iMungedTextBuffer[0] = EBoxTopLeft;
	iMungedTextBuffer[width-1] = EBoxTopRight;

	for (TInt i = 0; i < ScreenSize().iHeight; i++)
		{
		TPtrC line(iTextBuffer.Mid(i*contentWidth, contentWidth));
		iMungedTextBuffer.AppendFormat(_L("%c%S%c"), EBoxVerticalBar, &line, EBoxVerticalBar);
		}
	
	iMungedTextBuffer.Append(EBoxBottomLeft);
	iMungedTextBuffer.SetLength(iMungedTextBuffer.Length() + contentWidth);
	iMungedTextBuffer.RightTPtr(contentWidth).Fill(EBoxHorizontalBar);
	iMungedTextBuffer.Append(EBoxBottomRight);
	
	// And send the munged buffer
	Transmit(iMungedTextBuffer, width, ScreenSize().iHeight + 2);
#else
	// Just send it straight
	Transmit(iTextBuffer, ScreenSize().iWidth, ScreenSize().iHeight);
#endif
	}

void CTerminalKeyboardCons::Transmit(const TDesC& aBuf, TInt aWidth, TInt aHeight)
	{
	// This is how terminal keyboard does it - pretty horrible really

	static const TInt KMaxLen = 200; // This is what terminal keyboard uses - technically you could go as far as a total of 256
	TInt numLinesPerPrint = KMaxLen / aWidth;

	TBuf<256> line;
	TInt startLine = 0;
	while (startLine < aHeight)
		{
		if (startLine == 0)
			{
			// First line has extra info
			line.Format(_L("#$STIConsole#$%02d%02X%02X"), startLine, aWidth, aHeight);
			}
		else
			{
			line.Format(_L("#$STIConsole#$%02d"), startLine);
			}

		for (TInt i = 0; i < numLinesPerPrint; i++)
			{
			TPtrC theContents(aBuf.Mid((startLine+i)*aWidth, aWidth));
			//RDebug::Printf("line len=%d theContents len=%d", line.Length(), theContents.Length());
			line.Append(theContents);
			}

		RDebug::Print(line);
		startLine += numLinesPerPrint;
		}
	}

void CTerminalKeyboardCons::ConsoleScrolled(TInt aNumberOfLines)
	{
	TInt numChars = Abs(aNumberOfLines) * ScreenSize().iWidth;
	if (aNumberOfLines > 0)
		{
		iTextBuffer.Delete(0, numChars);
		iTextBuffer.AppendFill(' ', numChars);
		}
	else if (aNumberOfLines < 0)
		{
		iTextBuffer.SetLength(iTextBuffer.Length() - numChars);
		while (numChars--) iTextBuffer.Insert(0, _L(" "));
		}
	}

extern "C" EXPORT_C TAny* NewConsole()
	{
	return new CTerminalKeyboardCons;
	}

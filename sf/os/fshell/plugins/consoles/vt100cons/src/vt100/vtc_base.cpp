// vtc_base.cpp
// 
// Copyright (c) 2009 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#include "vtc_base.h"
#include <fshell/common.mmh>

_LIT(KIniFileName, "\\system\\console\\vt100.ini");
_LIT(KIniDesciptionFile, "\\resource\\vt100.idf");
_LIT(KAttConsoleSizeDetect, "console_size_detect");

EXPORT_C CVtcConsoleBase::CVtcConsoleBase()
	{
	}

EXPORT_C CVtcConsoleBase::~CVtcConsoleBase()
	{
	delete iIniFile;
	delete iInputController;
	delete iOutputController;
	CleanupUnderlyingConsole();
	}

EXPORT_C TInt CVtcConsoleBase::Create(const TDesC& aTitle, TSize /*aSize*/)
	{
	TRAPD(err, ConstructL(aTitle));
	HandleConsoleCreationError(_L("VT100"), err);
	return err;
	}

EXPORT_C void CVtcConsoleBase::ConstructL(const TDesC& aTitle)
	{
	ConstructL(aTitle, ETrue);
	}

EXPORT_C void CVtcConsoleBase::ConstructL(const TDesC&, TBool aConsoleSupportsSizeDetect)
	{
	iIniFile = LtkUtils::CIniFile::NewL(KIniFileName, KIniDesciptionFile);

	TSize screenSize(80, 24); // If sizeDetect is not specified, we default to (and only support) 80x24
	if (aConsoleSupportsSizeDetect && iIniFile->GetBool(KAttConsoleSizeDetect))
		{
		DetectScreenSizeL(screenSize);
		}
	iOutputController = CVtConsoleOutputController::NewL(*this, *iIniFile, screenSize);
	iInputController = CVtConsoleInputController::NewL(*this, *iIniFile);
	ClearScreen();
	CleanupUnderlyingConsole();
	}
	
EXPORT_C TInt CVtcConsoleBase::Extension_(TUint aExtensionId, TAny*& a0, TAny* a1)
	{
	if (aExtensionId == ConsoleMode::KSetConsoleModeExtension)
		{
		ConsoleMode::TMode mode = (ConsoleMode::TMode)(TInt)a1;
		iInputController->SetMode(mode);
		iOutputController->SetMode(mode);
		return KErrNone;
		}
	else if (aExtensionId == ConsoleAttributes::KSetConsoleAttributesExtension)
		{
		ConsoleAttributes::TAttributes* attributes = (ConsoleAttributes::TAttributes*)a1;
		return iOutputController->SetAttributes(attributes->iAttributes, attributes->iForegroundColor, attributes->iBackgroundColor);
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
		
EXPORT_C void CVtcConsoleBase::Read(TRequestStatus& aStatus)
	{
	iInputController->GetKeyPress(iKeyPress, aStatus);
	}

EXPORT_C void CVtcConsoleBase::ReadCancel()
	{
	iInputController->CancelGetKeyPress();
	}

EXPORT_C void CVtcConsoleBase::Write(const TDesC& aDes)
	{
	iOutputController->Write(aDes);
	}
	
EXPORT_C TPoint CVtcConsoleBase::CursorPos() const
	{
	TPoint pos;
	iOutputController->GetCursorPos(pos);
	return pos;
	}

EXPORT_C void CVtcConsoleBase::SetCursorPosAbs(const TPoint& aPoint)
	{
	iOutputController->SetCursorPosAbs(aPoint);
	}

EXPORT_C void CVtcConsoleBase::SetCursorPosRel(const TPoint& aPoint)
	{
	iOutputController->SetCursorPosRel(aPoint);
	}

EXPORT_C void CVtcConsoleBase::SetCursorHeight(TInt aPercentage)
	{
	iOutputController->SetCursorHeight(aPercentage);
	}

EXPORT_C void CVtcConsoleBase::SetTitle(const TDesC& aDes)
	{
	iOutputController->SetTitle(aDes);
	}

EXPORT_C void CVtcConsoleBase::ClearScreen()
	{
	iOutputController->ClearScreen();
	}

EXPORT_C void CVtcConsoleBase::ClearToEndOfLine()
	{
	iOutputController->ClearToEndOfLine();
	}

EXPORT_C TSize CVtcConsoleBase::ScreenSize() const
	{
	TSize size;
	iOutputController->GetScreenSize(size);
	return size;
	}

EXPORT_C TKeyCode CVtcConsoleBase::KeyCode() const
	{
	return iKeyPress.iCode;
	}

EXPORT_C TUint CVtcConsoleBase::KeyModifiers() const
	{
	return iKeyPress.iModifiers;
	}

void CVtcConsoleBase::DetectScreenSizeL(TSize& aSize)
	{
	TInt err;
	do
		{
		// If we get a KErrCorrupt error during the detect, we restart it from the top
		// This is because KErrCorrupt is (hopefully) only ever returned as a result of the DSR in ReadCursorPos failing, in which case it is worth retrying.
		// If it's any other error, the console is actually dead so we should bail
		err = DoDetectScreenSize(aSize);
		Message(EDebug, _L("DoDetectScreenSize returned %d"), err);
		}
#ifdef FSHELL_PLATFORM_OPP // Temporary OPP specific change - the drivers for the mid-sized prototype currently complete requests with KErrAbort just before power management sends the device to sleep.
	while ((err == KErrCorrupt) || (err == KErrAbort));
#else
	while (err == KErrCorrupt);
#endif
	User::LeaveIfError(err);
	}

TInt CVtcConsoleBase::DoDetectScreenSize(TSize& aSize)
	{
	Message(EDebug, _L("Beginning VT100 console size detect"));
	_LIT8(KSpace, " ");
	_LIT8(KNewLine, "\r\n");
	_LIT8(KResetCursorPosAbs, "\x1b[1;1H");
	TInt err = Output(KResetCursorPosAbs);
	if (err) return err;

#ifdef FSHELL_VT100_WORK_AROUND_TERATERM_CURSOR_BUG
	// Note, at the point where the cursor has just wrap to the next line, TeraTerm reports
	// the same cursor position has just before is wrapped. When the cursor is moved to the
	// right again, TeraTerm corrects its reckoning of the cursor position. The following
	// code works around this bug by keeping track of the previous cursor position and noticing
	// if it doesn't increment. This is treated as though the cursor had wrapped.

	TInt previousCursorPosX = -1;
	for (TInt x = 0; ; ++x)
		{
		err = Output(KSpace);
		if (err) return err;
		TPoint pos;
		TInt err = ReadCursorPos(pos);
		if (err) return err;
		TInt cursorPosX = pos.iX;
		if ((cursorPosX == 0) || (cursorPosX == previousCursorPosX))
			{
			aSize.iWidth = x + 1;
			break;
			}
		previousCursorPosX = cursorPosX;
		}
#else
	for (TInt x = 0; ; ++x)
		{
		err = Output(KSpace);
		if (err) return err;
		TPoint pos;
		err = ReadCursorPos(pos);
		if (err) return err;
		if (pos.iX == 0)
			{
			aSize.iWidth = x + 1;
			break;
			}
		}
#endif
	err = Output(KResetCursorPosAbs);
	if (err) return err;
	TInt prevYPos = 0;
	for (TInt y = 0; ; ++y)
		{
		err = Output(KNewLine);
		if (err) return err;
		TPoint pos;
		TInt err = ReadCursorPos(pos);
		if (err) return err;
		if (pos.iY == prevYPos)
			{
			aSize.iHeight = y;
			break;
			}
		else
			{
			prevYPos = y;
			}
		}
	err = Output(KResetCursorPosAbs);
	Message(EDebug, _L("Completed VT100 console size detect = %dx%d"), aSize.iWidth, aSize.iHeight);
	return err;
	}

TInt CVtcConsoleBase::ReadCursorPos(TPoint& aPosition)
	{
	// Ideally this functionality should be moved into the input and output controllers, but currently
	// this is problematic because:
	//
	// 1) TeraTerm contains bugs in the way it reports cursor position that need to be worked around.
	//    See FSHELL_VT100_WORK_AROUND_TERATERM_CURSOR_BUG.
	//
	// 2) The CVtConsoleInputController state machine would need some fairly heavy changes to be able
	//    to cope with ANSI Device Status Report escape sequences.
	//
	// 3) The fact that CVtConsoleInputController and CVtConsoleOutputController run in the same thread
	//    means that implementing CVtcConsoleBase::CursorPos (which is synchronous) would be tricky.
	//    Possible solutions are a) using CActiveSchedulerWait (which is almost always a bad idea) and
	//    b) putting CVtConsoleInputController in a separate thread (which would be a fair bit of work,
	//       and we've been there before...).
	//
	// For the time being then, vt100.dll has its own console size detection code (a duplication of
	// the version in iosrv) and uses this function only during construction to initialize the size of
	// the output controller's cursor tracker. From that point on, the cursor position is always retrieved
	// via the cursor tracker (the horrible thing that it is).

	// Note, this function can only safely be used before the input and output controllers are created
	// because in uses the MConsoleInput and MConsoleOutput interfaces directly.
	ASSERT((iOutputController == NULL) && (iInputController == NULL));

	_LIT8(KDeviceStatusReport, "\x1b[6n");
	//Message(EDebug, _L("Sending VT100 DSR"));
	TInt err = Output(KDeviceStatusReport);
	if (err < 0)
		{
		Message(EDebug, _L("Error %d sending DSR"), err);
		return err;
		}
	
	TPoint pos;
	TBuf8<32> buf;
	TPtr8 ptr(const_cast<TUint8*>(buf.Ptr()), 0, buf.MaxLength());
	FOREVER
		{
		TRequestStatus status;
		//Message(EDebug, _L("Waiting for input"));
		Input(ptr, status);
		User::WaitForRequest(status);
		err = status.Int();
		if (err < 0)
			{
			Message(EDebug, _L("Error getting input %d"), err);
			return err;
			}

		buf.SetLength(buf.Length() + ptr.Length());
		ptr.Set(const_cast<TUint8*>(buf.Ptr()) + buf.Length(), 0, buf.MaxLength() - buf.Length());
		if (ptr.MaxLength() == 0) return KErrOverflow; // Not sure how this could happen, maybe if the terminal returned really really long but otherwise valid numbers for the x and y pos

		// The sequence we're looking for is \x1b[nnn;nnnR
		TLex8 lex(buf);
		if (lex.Eos()) continue;
		if (lex.Get() != 0x1b) return KErrCorrupt;
		if (lex.Eos()) continue;
		if (lex.Get() != '[') return KErrCorrupt;
		if (lex.Eos()) continue;
		err = lex.Val(pos.iY);
		if (err) return err;
		if (lex.Eos()) continue;
		if (lex.Get() != ';') return KErrCorrupt;
		if (lex.Eos()) continue;
		err = lex.Val(pos.iX);
		if (err) return err;
		if (lex.Eos()) continue;
		if (lex.Get() != 'R') return KErrCorrupt;
		// If we reach here, we've successfully read the whole sequence

		// I assume we subtract one here because the VT100 indexes are 1-based and we use zero-based? -TomS
		pos.iX--;
		pos.iY--;
		aPosition = pos;
		return KErrNone;
		}
	}

EXPORT_C TInt CVtcConsoleBase::ReadKeywordValuePair(TLex& aLex, TPtrC& aKeyword, TPtrC& aValue)
	{
	TLexMark mark;
	aLex.SkipSpaceAndMark(mark);
	while (!aLex.Eos() && !aLex.Peek().IsSpace() && (aLex.Peek() != '='))
		{
		aLex.Get();
		}
	aKeyword.Set(aLex.MarkedToken(mark));
	aLex.SkipSpace();
	if (aLex.Get() != '=')
		{
		return KErrArgument;
		}
	aLex.SkipSpaceAndMark(mark);
	while (!aLex.Eos() && (aLex.Peek() != ','))
		{
		aLex.Get();
		}
	aValue.Set(aLex.MarkedToken(mark));
	if (aLex.Peek() == ',')
		{
		aLex.Get();
		}
	return KErrNone;
	}


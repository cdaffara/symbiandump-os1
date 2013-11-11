// vtc_controller.cpp
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

#include <e32base.h>
#include <fshell/common.mmh>
#include "vtc_controller.h"
#include "vtc_cursor_tracker.h"


//
// Constants.
//

_LIT(KAttSupportAttributes, "support_attributes");
_LIT(KAttForegroundColor, "foreground_color");
_LIT(KAttBackgroundColor, "background_color");

class TEscapeMapping
	{
public:
	TUint8 iEscapeChar;
	TKeyCode iKeyCode;
	};

class TLongerEscapeMapping
	{
public:
	TUint8 iEscapeChar1;
	TUint8 iEscapeChar2;
	TKeyCode iKeyCode;
	};


const TEscapeMapping KEscapeMappings[] = 
	{
		{ 'A', EKeyUpArrow },
		{ 'B', EKeyDownArrow },
		{ 'C', EKeyRightArrow },
		{ 'D', EKeyLeftArrow },
		{ 'H', EKeyHome },
		{ 'K', EKeyEnd },
	};
const TInt KNumEscapeMappings(sizeof(KEscapeMappings) / sizeof(TEscapeMapping));

const TEscapeMapping KFunctionKeyMappings[] =
	{
		{ 'P', EKeyF1 },
		{ 'Q', EKeyF2 },
		{ 'R', EKeyF3 },
		{ 'S', EKeyF4 },
	};
const TInt KNumEscapeFunctionMappings(sizeof(KFunctionKeyMappings) / sizeof(TEscapeMapping));

// The following are for VT220 support, which is needed to get function keys understood from teraterm. They use ESC [xx~ where xx is given by the below table
// See http://aperiodic.net/phil/archives/Geekery/term-function-keys.html for the reference I used
const TLongerEscapeMapping KExtendedEscapeMappings[] = 
	{
		{ '1', '1', EKeyF1 },
		{ '1', '2', EKeyF2 },
		{ '1', '3', EKeyF3 },
		{ '1', '4', EKeyF4 },
		{ '1', '5', EKeyF5 },
		// '1' '6' isn't used
		{ '1', '7', EKeyF6 },
		{ '1', '8', EKeyF7 },
		{ '1', '9', EKeyF8 },
		{ '2', '0', EKeyF9 },
		{ '2', '1', EKeyF10 },
		// '2' '2' isn't used
		{ '2', '3', EKeyF11 },
		{ '2', '4', EKeyF12 },

		// ESC [1~ is Home key according to http://www.zaik.uni-koeln.de/~ftp/elfi/etc/telnet.key
		{ '1', 0, EKeyHome },
		{ '4', 0, EKeyEnd },
		{ '5', 0, EKeyPageUp },
		{ '6', 0, EKeyPageDown },
		{ '2', 0, EKeyInsert },
		{ '3', 0, EKeyDelete },
	};
const TInt KNumExtendedEscapeMappings = sizeof(KExtendedEscapeMappings) / sizeof(TLongerEscapeMapping);

static const TUint8 KEscapeChar1 = 0x1b;
static const TUint8 KEscapeChar2 = '[';
static const TUint8 KEscapeChar2Func = 'O';

static const TInt KEscapeTimeoutMicros = 200000; // 1/5th second


//______________________________________________________________________________
//						TKeyPress
EXPORT_C TKeyPress::TKeyPress()
	: iCode(EKeyNull), iModifiers(0)
	{
	}

EXPORT_C TKeyPress::TKeyPress(TKeyCode aCode, TUint aModifiers)
	: iCode(aCode), iModifiers(aModifiers)
	{
	}

//______________________________________________________________________________
//						CVtConsoleOutputController
EXPORT_C CVtConsoleOutputController* CVtConsoleOutputController::NewL(MConsoleOutput& aOutput, LtkUtils::CIniFile& aIniFile, const TSize& aScreenSize)
	{
	CVtConsoleOutputController* self = new(ELeave)CVtConsoleOutputController(aOutput, aIniFile);
	CleanupStack::PushL(self);
	User::LeaveIfError(self->Construct(aScreenSize));
	CleanupStack::Pop(self);
	return self;
	}
	
EXPORT_C CVtConsoleOutputController* CVtConsoleOutputController::New(MConsoleOutput& aOutput, LtkUtils::CIniFile& aIniFile, const TSize& aScreenSize)
	{
	CVtConsoleOutputController* self = new CVtConsoleOutputController(aOutput, aIniFile);
	TInt err = self->Construct(aScreenSize);
	if (err!=KErrNone)
		{
		delete self;
		self = NULL;
		}
	return self;
	}

class TAttributeBuf : public TBuf8<64>
	{
public:
	enum TColorType
		{
		EForeground,
		EBackground
		};
public:
	TAttributeBuf(LtkUtils::CIniFile& aIniFile);
	void Add(TUint aAttributes);
	void Add(ConsoleAttributes::TColor aColor, TColorType aType);
	void Finalize();
	TBool SomethingAdded() const;
private:
	void Add(const TDesC8& aAttribute);
private:
	LtkUtils::CIniFile& iIniFile;
	TBool iSomethingAdded;
	};

_LIT8(KPrefix, "\x1b[");
TAttributeBuf::TAttributeBuf(LtkUtils::CIniFile& aIniFile)
	: TBuf8<64>(KPrefix), iIniFile(aIniFile), iSomethingAdded(EFalse)
	{
	}

void TAttributeBuf::Add(const TDesC8& aAttribute)
	{
	if (iSomethingAdded)
		{
		Append(';');
		}
	Append(aAttribute);
	iSomethingAdded = ETrue;
	}

void TAttributeBuf::Finalize()
	{
	ASSERT(iSomethingAdded);
	Append('m');
	}

TBool TAttributeBuf::SomethingAdded() const
	{
	return iSomethingAdded;
	}

void TAttributeBuf::Add(TUint aAttributes)
	{
	_LIT8(KAttReset,		"0");
	_LIT8(KAttBold,			"1");
	_LIT8(KAttUnderscore,	"4");
	_LIT8(KAttBlink,		"5");
	_LIT8(KAttInverse,		"7");
	_LIT8(KAttConceal,		"8");

	if (aAttributes & ConsoleAttributes::ENone)
		{
		Add(KAttReset);
		Add((ConsoleAttributes::TColor)iIniFile.GetInt(KAttForegroundColor), TAttributeBuf::EForeground);
		Add((ConsoleAttributes::TColor)iIniFile.GetInt(KAttBackgroundColor), TAttributeBuf::EBackground);
		}
	if (aAttributes & ConsoleAttributes::EBold)
		{
		Add(KAttBold);
		}
	if (aAttributes & ConsoleAttributes::EUnderscore)
		{
		Add(KAttUnderscore);
		}
	if (aAttributes & ConsoleAttributes::EBlink)
		{
		Add(KAttBlink);
		}
	if (aAttributes & ConsoleAttributes::EInverse)
		{
		Add(KAttInverse);
		}
	if (aAttributes & ConsoleAttributes::EConceal)
		{
		Add(KAttConceal);
		}
	}

void TAttributeBuf::Add(ConsoleAttributes::TColor aColor, TColorType aType)
	{
	_LIT8(KFgBlack,			"30");
	_LIT8(KFgRed,			"31");
	_LIT8(KFgGreen,			"32");
	_LIT8(KFgYellow,		"33");
	_LIT8(KFgBlue,			"34");
	_LIT8(KFgMagenta,		"35");
	_LIT8(KFgCyan,			"36");
	_LIT8(KFgWhite,			"37");
	_LIT8(KFgReset,			"39");
	_LIT8(KBgBlack,			"40");
	_LIT8(KBgRed,			"41");
	_LIT8(KBgGreen,			"42");
	_LIT8(KBgYellow,		"43");
	_LIT8(KBgBlue,			"44");
	_LIT8(KBgMagenta,		"45");
	_LIT8(KBgCyan,			"46");
	_LIT8(KBgWhite,			"47");
	_LIT8(KBgReset,			"49");

	if (aType == EForeground)
		{
		switch (aColor)
			{
			default:
			case ConsoleAttributes::EUnchanged:
				{
				// Do nothing.
				break;
				}
			case ConsoleAttributes::EBlack:
				{
				Add(KFgBlack);
				break;
				}
			case ConsoleAttributes::ERed:
				{
				Add(KFgRed);
				break;
				}
			case ConsoleAttributes::EGreen:
				{
				Add(KFgGreen);
				break;
				}
			case ConsoleAttributes::EYellow:
				{
				Add(KFgYellow);
				break;
				}
			case ConsoleAttributes::EBlue:
				{
				Add(KFgBlue);
				break;
				}
			case ConsoleAttributes::EMagenta:
				{
				Add(KFgMagenta);
				break;
				}
			case ConsoleAttributes::ECyan:
				{
				Add(KFgCyan);
				break;
				}
			case ConsoleAttributes::EWhite:
				{
				Add(KFgWhite);
				break;
				}
			case ConsoleAttributes::EReset:
				{
				Add(KFgReset);
				break;
				}
			}
		}
	else
		{
		switch (aColor)
			{
			default:
			case ConsoleAttributes::EUnchanged:
				{
				// Do nothing.
				break;
				}
			case ConsoleAttributes::EBlack:
				{
				Add(KBgBlack);
				break;
				}
			case ConsoleAttributes::ERed:
				{
				Add(KBgRed);
				break;
				}
			case ConsoleAttributes::EGreen:
				{
				Add(KBgGreen);
				break;
				}
			case ConsoleAttributes::EYellow:
				{
				Add(KBgYellow);
				break;
				}
			case ConsoleAttributes::EBlue:
				{
				Add(KBgBlue);
				break;
				}
			case ConsoleAttributes::EMagenta:
				{
				Add(KBgMagenta);
				break;
				}
			case ConsoleAttributes::ECyan:
				{
				Add(KBgCyan);
				break;
				}
			case ConsoleAttributes::EWhite:
				{
				Add(KBgWhite);
				break;
				}
			case ConsoleAttributes::EReset:
				{
				Add(KBgReset);
				break;
				}
			}
		}
	}

EXPORT_C TInt CVtConsoleOutputController::ResetAttributes()
	{
	if (iIniFile.GetBool(KAttSupportAttributes))
		{
		TAttributeBuf buf(iIniFile);
		buf.Add(ConsoleAttributes::ENone);
		buf.Finalize();
		return iOutput.Output(buf);
		}

	return KErrNone;
	}

EXPORT_C TInt CVtConsoleOutputController::SetAttributes(TUint aAttributes, ConsoleAttributes::TColor aForegroundColor, ConsoleAttributes::TColor aBackgroundColor)
	{
	if (iIniFile.GetBool(KAttSupportAttributes))
		{
		TAttributeBuf buf(iIniFile);
		buf.Add(aAttributes);
		buf.Add(aForegroundColor, TAttributeBuf::EForeground);
		buf.Add(aBackgroundColor, TAttributeBuf::EBackground);

		if (buf.SomethingAdded())
			{
			buf.Finalize();
			return iOutput.Output(buf);
			}

		return KErrNone;
		}

	return KErrNotSupported;
	}
	
CVtConsoleOutputController::CVtConsoleOutputController(MConsoleOutput& aOutput, LtkUtils::CIniFile& aIniFile)
	: iOutput(aOutput), iIniFile(aIniFile), iMode(ConsoleMode::EText)
	{
	}

TInt CVtConsoleOutputController::Construct(const TSize& aScreenSize)
	{
	iCursorTracker = new TCursorTracker(aScreenSize);
	if (!iCursorTracker) return KErrNoMemory;
	return KErrNone;
	}

EXPORT_C CVtConsoleOutputController::~CVtConsoleOutputController()
	{
	delete iCursorTracker;
	iOutputBuf.Close();
	}

TInt CVtConsoleOutputController::GetCursorPos(TPoint& aPos) const
	{
	aPos = iCursorTracker->CursorPos();
	return KErrNone;
	}


TInt CVtConsoleOutputController::SetCursorPosAbs(const TPoint& aPos)
	{
	TInt err = KErrInUse;
	if (iMode == ConsoleMode::EText)
		{
		_LIT8(KEscapeSetCursorPosAbs, "\x1b[%u;%uH");
		TBuf8<32> buf;
		buf.Format(KEscapeSetCursorPosAbs, aPos.iY + 1, aPos.iX + 1);
		err = iOutput.Output(buf);
		if (err == KErrNone)
			{
			iCursorTracker->SetCursorPosAbs(aPos);
			}
		}
	return err;
	}

TInt CVtConsoleOutputController::SetCursorPosRel(const TPoint& aPoint)
	{
	TInt err = KErrInUse;
	if (iMode == ConsoleMode::EText)
		{
		TBuf8<32> buf;

		if (aPoint.iY == 0)
			{
			// Do nothing.
			}
		else if (aPoint.iY < 0)
			{
			// Move cursor up.
			_LIT8(KEscapeCursorUp, "\x1b[%uA");
			buf.AppendFormat(KEscapeCursorUp, -aPoint.iY);
			}
		else if (aPoint.iY > 0)
			{
			// Move cursor down.
			_LIT8(KEscapeCursorDown, "\x1b[%uB");
			buf.AppendFormat(KEscapeCursorDown, aPoint.iY);
			}

		if (aPoint.iX == 0)
			{
			// Do nothing.
			}
		else if (aPoint.iX < 0)
			{
			// Move cursor left.
			_LIT8(KEscapeCursorLeft, "\x1b[%uD");
			buf.AppendFormat(KEscapeCursorLeft, -aPoint.iY);
			}
		else if (aPoint.iX > 0)
			{
			// Move cursor right.
			_LIT8(KEscapeCursorRight, "\x1b[%uC");
			buf.AppendFormat(KEscapeCursorRight, aPoint.iY);
			}
		
		err = KErrNone;
		if (buf.Length() > 0)
			{
			err = iOutput.Output(buf);
			if (err == KErrNone)
				{
				iCursorTracker->SetCursorPosRel(aPoint);
				}
			}
		}
	return err;
	}

TInt CVtConsoleOutputController::SetCursorHeight(TInt aPercentage)
	{
	TInt err = KErrInUse;
	if (iMode == ConsoleMode::EText)
		{
		_LIT8(KEscapeHideCursor, "\x1b[?25l");
		_LIT8(KEscapeSeeCursor, "\x1b[?25h");
		if (aPercentage == 0)
			{
			err = iOutput.Output(KEscapeHideCursor);
			}
		else
			{
			err = iOutput.Output(KEscapeSeeCursor);
			}
		}
	return err;
	}

TInt CVtConsoleOutputController::SetTitle(const TDesC&)
	{
	return KErrNone;
	}

TInt CVtConsoleOutputController::ClearScreen()
	{
	TInt err = KErrInUse;
	if (iMode == ConsoleMode::EText)
		{
		_LIT8(KResetTerminal, "\x1b" "c" "\x1b" "[?7h"); // reset console, then enable line wrapping
		err = iOutput.Output(KResetTerminal);
		if (err == KErrNone)
			{
			User::After(100000); // It seems that TeraTerm doesn't like receiving attribute changes too soon after a terminal reset (tends to ignore the attributes).
			err = ResetAttributes();
			if (err == KErrNone)
				{
				_LIT8(KEscapeClearScreen, "\x1b[2J\x1b[01;01H");
				err = iOutput.Output(KEscapeClearScreen);
				if (err == KErrNone)
					{
					iCursorTracker->Reset();
					}
				}
			}
		}
	return err;
	}

TInt CVtConsoleOutputController::ClearToEndOfLine()
	{
	TInt err = KErrInUse;
	if (iMode == ConsoleMode::EText)
		{
		_LIT8(KEscapeClearToEndOfLine, "\x1b[K");
		err = iOutput.Output(KEscapeClearToEndOfLine);
		}
	return err;
	}

TInt CVtConsoleOutputController::GetScreenSize(TSize& aSize) const
	{
	aSize = iCursorTracker->ConsoleSize();
	return KErrNone;
	}

TInt CVtConsoleOutputController::Write(const TDesC& aDes)
	{
	TInt err = KErrNone;
	if (iMode == ConsoleMode::EBinary)
		{
		// staight collapse to 8 bit, no cleverness
		TBuf8<256> buf;
		TInt offset = 0;
		while ((offset < aDes.Length()) && (err == KErrNone))
			{
			buf.Copy(aDes.Mid(offset, Min(aDes.Length() - offset, buf.MaxLength())));
			offset += buf.Length();
			err = iOutput.Output(buf);
			}
		}
	else
		{
		// In text mode we do a UTF-16 -> UTF-8 conversion
		TRAP(err, iOutputBuf.CopyAsUtf8L(aDes));
		if (err == KErrNone)
			{
			err = iOutput.Output(iOutputBuf);
			if (err == KErrNone)
				{
				iCursorTracker->Write(aDes);
				}
			}
		}
	return err;
	}
	
TInt CVtConsoleOutputController::Write(const TDesC8& aDes)
	{
	TInt err = iOutput.Output(aDes);
	if ((err == KErrNone) && (iMode == ConsoleMode::EText))
		{
		iCursorTracker->Write(aDes);
		}
	return err;
	}

void CVtConsoleOutputController::SetMode(ConsoleMode::TMode aMode)
	{
	iMode = aMode;
	}

//______________________________________________________________________________
//						CVtConsoleInputController

EXPORT_C CVtConsoleInputController* CVtConsoleInputController::New(MConsoleInput& aConsoleInput, LtkUtils::CIniFile& aIniFile)
	{
	CVtConsoleInputController* self = NULL;
	TRAPD(err, self = NewL(aConsoleInput, aIniFile));
	if (err == KErrNone)
		{
		return self;
		}
	return NULL;
	}

EXPORT_C  CVtConsoleInputController* CVtConsoleInputController::NewL(MConsoleInput& aConsoleInput, LtkUtils::CIniFile& aIniFile)
	{
	CVtConsoleInputController* self = NewLC(aConsoleInput, aIniFile);
	CleanupStack::Pop(self);
	return self;
	}
	
EXPORT_C  CVtConsoleInputController* CVtConsoleInputController::NewLC(MConsoleInput& aConsoleInput, LtkUtils::CIniFile& aIniFile)
	{
	CVtConsoleInputController* self = new(ELeave)CVtConsoleInputController(aConsoleInput, aIniFile);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CVtConsoleInputController::CVtConsoleInputController(MConsoleInput& aConsoleInput, LtkUtils::CIniFile& aIniFile)
	: CActive(CActive::EPriorityStandard), iConsoleInput(aConsoleInput), iIniFile(aIniFile), iMode(ConsoleMode::EText)
	{
	CActiveScheduler::Add(this);
	}
	
void CVtConsoleInputController::ConstructL()
	{
	iEscapeTimer = CPeriodic::NewL(CActive::EPriorityLow);
	}
	
CVtConsoleInputController::~CVtConsoleInputController()
	{
	Reset();
	delete iEscapeTimer;
	}

EXPORT_C void CVtConsoleInputController::GetKeyPress(TKeyPress& aKeyPress, TRequestStatus& aStatus)
	{
	ASSERT((iClientKeyPress == NULL) && (iClientRequestStatus == NULL));
	iClientKeyPress = &aKeyPress;
	iClientRequestStatus = &aStatus;
	*iClientRequestStatus = KRequestPending;
	if (iKeyCodePending)
		{
		iKeyCodePending = EFalse;
		CompleteKeyPressRequest(iPendingKeyCode);
		}
	else if (iInputError)
		{
		CompleteKeyPressRequest(iInputError);
		iInputError = KErrNone;
		}
	else
		{
		ReadKeyPress();
		}
	}

EXPORT_C void CVtConsoleInputController::CancelGetKeyPress()
	{
	Cancel();
	if (iClientRequestStatus)
		{
		CompleteKeyPressRequest(KErrCancel);
		}
	}

EXPORT_C void CVtConsoleInputController::SetMode(ConsoleMode::TMode aMode)
	{
	Reset();
	iMode = aMode;
	}

void CVtConsoleInputController::ReadKeyPress()
	{
	iEscapeTimer->Cancel();

	while (iClientRequestStatus && (iBufPos < iBuf.Length()))
		{
		TUint8 c = iBuf[iBufPos++];
		if (iMode == ConsoleMode::EBinary)
			{
			CompleteKeyPressRequest((TKeyCode)c);
			}
		else
			{
			switch (iState)
				{
			case ENormal:
				if (c == KEscapeChar1)
					{
					iState = EWaitingForEscapeChar2;
					if (iBufPos == iBuf.Length())
						{
						iEscapeTimer->Start(KEscapeTimeoutMicros, KEscapeTimeoutMicros, TCallBack(EscapeTimeoutS, this));
						}
					}
				else
					{
					CompleteKeyPressRequest((TKeyCode)c);
					}
				break;
			case EWaitingForEscapeChar2:
				if (c == KEscapeChar2)
					{
					iState = EWaitingForEscapeChar3;
					}
				else if (c == KEscapeChar2Func)
					{
					iState = EWaitingForEscapeChar3Func;
					}
				else
					{
					CompleteKeyPressRequest(EKeyEscape, (TKeyCode)c);
					iState = ENormal;
					}
				break;
			case EWaitingForEscapeChar3:
				if (c >= '0' && c <= '9')
					{
					iState = EWaitingForExtendedFunc;
					iExtendedEscapeBuf.Zero();
					iExtendedEscapeBuf.Append(c);
					}
				else
					{
					DoEscapeKeyL(c, KEscapeMappings, KNumEscapeMappings);
					}
				break;
			case EWaitingForEscapeChar3Func:
				DoEscapeKeyL(c, KFunctionKeyMappings, KNumEscapeFunctionMappings);
				break;
			case EWaitingForExtendedFunc:
				if (iExtendedEscapeBuf.Length() < iExtendedEscapeBuf.MaxLength() && ((c >= '0' && c <= '9') || c == ';'))
					{
					iExtendedEscapeBuf.Append(c);
					// Stay in this state until you see a '~'
					}
				else if (c == '~')
					{
					DoExtendedEscapeKey();
					}
				else
					{
					// Gone off the rails, bail
					iState = ENormal;
					CompleteKeyPressRequest((TKeyCode)c);
					}
				break;
				}
			}
		}

	ReadInput();
	}

void CVtConsoleInputController::CompleteKeyPressRequest(TInt aError)
	{
	ASSERT(iClientKeyPress && iClientRequestStatus);
	iClientKeyPress = NULL;
	User::RequestComplete(iClientRequestStatus, aError);
	}

void CVtConsoleInputController::CompleteKeyPressRequest(TKeyCode aKeyCode)
	{
	ASSERT(iClientKeyPress && iClientRequestStatus);
	iClientKeyPress->iCode = (TKeyCode)aKeyCode;
	iClientKeyPress->iModifiers = 0;
	iClientKeyPress = NULL;
	User::RequestComplete(iClientRequestStatus, KErrNone);
	}

void CVtConsoleInputController::CompleteKeyPressRequest(TKeyCode aKeyCode1, TKeyCode aKeyCode2)
	{
	ASSERT(!iKeyCodePending);
	// Store the second key-code in a member variable to be used the next time GetKeyPress is called.
	iKeyCodePending = ETrue;
	iPendingKeyCode = aKeyCode2;
	CompleteKeyPressRequest(aKeyCode1);
	}

void CVtConsoleInputController::Reset()
	{
	Cancel();
	iEscapeTimer->Cancel();
	iState = ENormal;
	iKeyCodePending = EFalse;
	}

void CVtConsoleInputController::ReadInput()
	{
	if (iClientRequestStatus && !IsActive()) // Note, if the escape timer expired we could already be active.
		{
		ASSERT(iBufPos == iBuf.Length());
		iBufPos = 0;
		iBuf.Zero();
		iConsoleInput.Input(iBuf, iStatus);
		SetActive();
		}
	}

void CVtConsoleInputController::RunL()
	{
	TInt err = iStatus.Int();
#ifdef FSHELL_PLATFORM_OPP
	if (err == KErrAbort)
		{
		ReadInput();
		return;
		}
#endif
	if (err == KErrNone)
		{
		ReadKeyPress();
		}
	else if (iClientRequestStatus)
		{
		CompleteKeyPressRequest(err);
		}
	else
		{
		// Report the error next time the client requests a key.
		iInputError = err;
		}
	}

void CVtConsoleInputController::DoCancel()
	{
	iConsoleInput.CancelInput(iStatus);
	}

void CVtConsoleInputController::DoEscapeKeyL(TUint8 aChar, const TEscapeMapping* aMappings, TInt aMappingCount)
	{
	iState = ENormal;
	for (TInt j = 0; j < aMappingCount; ++j)
		{
		if (aChar == aMappings[j].iEscapeChar)
			{
			CompleteKeyPressRequest(aMappings[j].iKeyCode);
			return;
			}
		}
	}
	
TInt CVtConsoleInputController::EscapeTimeout()
	{
	ASSERT(iState == EWaitingForEscapeChar2);
	iState = ENormal;
	CompleteKeyPressRequest(EKeyEscape);
	return KErrNone;
	}

TInt CVtConsoleInputController::EscapeTimeoutS(TAny* aSelf)
	{
	return ((CVtConsoleInputController*)aSelf)->EscapeTimeout();
	}

	
void CVtConsoleInputController::DoExtendedEscapeKey()
	{
	iState = ENormal;
	TUint8 escape1 = 0;
	TUint8 escape2 = 0;
	if (iExtendedEscapeBuf.Length()) escape1 = iExtendedEscapeBuf[0];
	if (iExtendedEscapeBuf.Length() > 1) escape2 = iExtendedEscapeBuf[1];

	for (TInt j = 0; j < KNumExtendedEscapeMappings; ++j)
		{
		const TLongerEscapeMapping& mapping = KExtendedEscapeMappings[j];
		if (escape1 == mapping.iEscapeChar1 && escape2 == mapping.iEscapeChar2)
			{
			CompleteKeyPressRequest(mapping.iKeyCode);
			return;
			}
		}
	}

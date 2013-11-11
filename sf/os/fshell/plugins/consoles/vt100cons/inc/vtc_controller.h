// vtc_controller.h
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

#ifndef __VTC_CONTROLLER_H__
#define __VTC_CONTROLLER_H__

#include <e32keys.h>
#include <fshell/settings.h>
#include <fshell/consoleextensions.h>
#include <fshell/abstract_console_writer.h>
#include <fshell/common.mmh>
#include <fshell/descriptorutils.h>

class TCursorTracker;
class TEscapeMapping;

class MConsoleOutput
	{
public:
	virtual TInt Output(const TDesC8& aDes) = 0;
	};

NONSHARABLE_CLASS(CVtConsoleOutputController) : public CBase, public MAbstractConsoleWriter
	{
public:
	IMPORT_C static CVtConsoleOutputController* NewL(MConsoleOutput& aOutput, LtkUtils::CIniFile& aIniFile, const TSize& aScreenSize);
	IMPORT_C static CVtConsoleOutputController* New(MConsoleOutput& aOutput, LtkUtils::CIniFile& aIniFile, const TSize& aScreenSize);
	IMPORT_C ~CVtConsoleOutputController();
	IMPORT_C TInt ResetAttributes();
	IMPORT_C TInt SetAttributes(TUint aAttributes, ConsoleAttributes::TColor aForegroundColor = ConsoleAttributes::EUnchanged, ConsoleAttributes::TColor aBackgroundColor = ConsoleAttributes::EUnchanged);
	void SetMode(ConsoleMode::TMode aMode);

public: // From MAbstractConsoleWriter.
	virtual TInt GetCursorPos(TPoint& aPos) const;
	virtual TInt SetCursorPosAbs(const TPoint& aPos);
	virtual TInt SetCursorPosRel(const TPoint& aPos);
	virtual TInt SetCursorHeight(TInt aPercentage);
	virtual TInt SetTitle(const TDesC& aTitle);
	virtual TInt ClearScreen();
	virtual TInt ClearToEndOfLine();
	virtual TInt GetScreenSize(TSize& aSize) const;
	virtual TInt Write(const TDesC& aDes);
	virtual TInt Write(const TDesC8& aDes);
private:
	CVtConsoleOutputController(MConsoleOutput& aOutput, LtkUtils::CIniFile& aIniFile);
	TInt Construct(const TSize& aScreenSize);
private:
	MConsoleOutput& iOutput;
	LtkUtils::CIniFile& iIniFile;
	TCursorTracker* iCursorTracker;
	LtkUtils::RLtkBuf8 iOutputBuf;
	ConsoleMode::TMode iMode;
	};
	
class TKeyPress
	{
public:
	IMPORT_C TKeyPress();
	IMPORT_C TKeyPress(TKeyCode aCode, TUint aModifiers);
public:
	TKeyCode iCode;
	TUint iModifiers;
	};

class MConsoleInput
	{
public:
	virtual void Input(TDes8& aDes, TRequestStatus& aStatus) = 0;
	virtual void CancelInput(TRequestStatus& aStatus) = 0;
	};

NONSHARABLE_CLASS(CVtConsoleInputController) : public CActive
	{
public:
	IMPORT_C static CVtConsoleInputController* New(MConsoleInput& aConsoleInput, LtkUtils::CIniFile& aIniFile);
	IMPORT_C static CVtConsoleInputController* NewL(MConsoleInput& aConsoleInput, LtkUtils::CIniFile& aIniFile);
	IMPORT_C static CVtConsoleInputController* NewLC(MConsoleInput& aConsoleInput, LtkUtils::CIniFile& aIniFile);
	IMPORT_C void GetKeyPress(TKeyPress& aKeyPress, TRequestStatus& aStatus);
	IMPORT_C void CancelGetKeyPress();
	IMPORT_C void SetMode(ConsoleMode::TMode aMode);
public:
	virtual ~CVtConsoleInputController();
private:
	void ConstructL();
	CVtConsoleInputController(MConsoleInput& aConsoleInput, LtkUtils::CIniFile& aIniFile);
	void DoEscapeKeyL(TUint8 aChar, const TEscapeMapping* iMappings, TInt aMappingCount);
	void DoExtendedEscapeKey();
	static TInt EscapeTimeoutS(TAny* aSelf);
	TInt EscapeTimeout();
	void ReadKeyPress();
	void CompleteKeyPressRequest(TInt aError);
	void CompleteKeyPressRequest(TKeyCode aKeyCode);
	void CompleteKeyPressRequest(TKeyCode aKeyCode1, TKeyCode aKeyCode2);
	void Reset();
	void ReadInput();
private: // From CActive.
	virtual void RunL();
	virtual void DoCancel();
private:
	enum TState
		{
		ENormal,
		EWaitingForEscapeChar2,
		EWaitingForEscapeChar3,
		EWaitingForEscapeChar3Func,
		EWaitingForExtendedFunc, // Chars 4 and later
		};
private:
	MConsoleInput& iConsoleInput;
	LtkUtils::CIniFile& iIniFile;
	TState iState;
	CPeriodic* iEscapeTimer;
	ConsoleMode::TMode iMode;
	TBuf8<1024> iBuf;
	TInt iBufPos;
	TKeyPress* iClientKeyPress;
	TRequestStatus* iClientRequestStatus;
	TBool iKeyCodePending;
	TKeyCode iPendingKeyCode;
	TInt iInputError;
	TBuf8<4> iExtendedEscapeBuf;
	};
	
#endif //__VTC_CONTROLLER_H__

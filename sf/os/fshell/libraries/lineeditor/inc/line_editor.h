// line_editor.h
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

#ifndef __LINE_EDITOR_H__
#define __LINE_EDITOR_H__

#include <e32std.h>
#include <e32base.h>
#include <e32keys.h>
#include <fshell/line_editor_observer.h>
#include <fshell/abstract_console_writer.h>
#include <fshell/ioutils.h>

const TInt KMaxLineLength = 256;
#define CTRL(x) ((x)-'a'+1)

class CLineHistory;

//
// MConsoleScrollObserver.
//

class MConsoleScrollObserver
	{
public:
	virtual void CsoHandleConsoleScrolled() = 0;
	};
	
//
// RConsole.
//

class RConsole
	{
public:
	enum TCursorMode
		{
		EInsert,
		EOverwrite
		};
public:
	RConsole(MAbstractConsoleWriter& aStdout);
	void Close();
	void Start();
	void Refresh();
	void Write(const TDesC& aDes);
	void SetCursorPosAbs(const TPoint& aPoint);
	void SetCursorPosRel(const TPoint& aPoint);
	void MoveCursorLeft();
	void MoveCursorRight();
	void NewLine();
	void SetCursorMode(TCursorMode aMode);
	void SetCursorVisible(TBool aVisible);
	void ClearToEndOfLine();
	void Clear();
	TPoint PosFrom(const TPoint& aStartPos, TInt aLength) const;
	TPoint CursorPos() const;
	TInt AddObserver(MConsoleScrollObserver& aObserver);
	void RemoveObserver(MConsoleScrollObserver& aObserver);
	TSize Size() const;
private:
	void CursorLeft();
	void CursorRight();
	void LineFeed();
	void CarriageReturn();
	void NotifyScrollObservers();
private:
	TSize iSize;
	MAbstractConsoleWriter& iStdout;
	RPointerArray<MConsoleScrollObserver> iScrollObservers;
	TPoint iCursorPos;
	TCursorMode iCursorMode;
	TBool iCursorVisible;
	};


//
// TConsoleLine.
//

class TConsoleLine : public MConsoleScrollObserver
	{
public:
	IMPORT_C TConsoleLine(RConsole& aConsole);
	IMPORT_C void Start(const TDesC& aPrompt);
	IMPORT_C void Start(const TDesC& aPrompt, const TDesC& aInitialInput);
	IMPORT_C void Replace(const TDesC& aDes);
	IMPORT_C void Replace(TInt aFrom, const TDesC& aDes);
	IMPORT_C void Insert(TChar aChar);
	IMPORT_C void Overwrite(TChar aChar);
	IMPORT_C void Redraw();
	IMPORT_C void End();
	IMPORT_C void DeleteLeft();
	IMPORT_C void DeleteRight();
	IMPORT_C void CursorLeft();
	IMPORT_C void CursorRight();
	IMPORT_C void CursorPreviousWord();
	IMPORT_C void CursorNextWord();
	IMPORT_C void CursorBeginning();
	IMPORT_C void CursorEnd();
	IMPORT_C void PrintCompletionPossibilitiesL(const TDesC& aPossibilities);
	IMPORT_C TPtrC Contents() const;
	IMPORT_C TPtrC ContentsToCursor() const;
public:
	void Hide();
	void Show();
	void SetCursorPosition(TInt aPosition);
	void Abort();
private:
	void Restart(const TDesC& aPrompt);
private: // From MConsoleScrollObserver.
	virtual void CsoHandleConsoleScrolled();
private:
	RConsole& iConsole;
	TBuf<KMaxLineLength> iBuf;
	TInt iPromptLength;
	TInt iBufPos;
	TPoint iStartPos;
	TBool iStarted;
	TBool iHidden;
	};


//
// CLineEditor.
//

class CLineEditor : public CBase
	{
public:
	IMPORT_C static CLineEditor* NewL(	RFs& aFs, 
										MAbstractConsoleWriter& aStdout, 
										MLineEditorObserver& aObserver, 
										MLineCompleter& aCompleter,
										const TDesC& aConsoleHistoryFile);
	IMPORT_C ~CLineEditor();
	IMPORT_C void HandleKey(TUint aKeyCode, TUint aModifiers);
	IMPORT_C void Start(const TDesC& aPrompt);
	IMPORT_C void Start(const TDesC& aPrompt, const TDesC& aInitialInput);
	IMPORT_C void Abort();
	
	IMPORT_C void Redraw();
	IMPORT_C void RemovePromptAndUserInput();
	IMPORT_C void ReinstatePromptAndUserInput();
private:
	CLineEditor(MAbstractConsoleWriter& aStdout, MLineEditorObserver& aObserver, MLineCompleter& aCompleter);
	void ConstructL(RFs& aFs, const TDesC& aConsoleHistoryFile);
	void InsertChar(TChar aChar);
	void HandleEnter();
	void HandleBackspace();
	void HandleTab();
	void HandleTabL();
	void HandleDelete();
	void HandleLeftArrow();
	void HandleRightArrow();
	void HandleUpArrow();
	void HandleDownArrow();
	void HandlePreviousWord();
	void HandleNextWord();
	void HandleHome();
	void HandleEnd();
	void HandlePageUp();
	void HandlePageDown();
	void HandleInsert();
	void HandleEscape();
	void HandleF8Completion(); // DOS-style F8 line completion
	void ReplaceLine(const TDesC& aNewLine);
private:
	enum TState
		{
		EIdle,
		EEditing,
		ERecallingHistory
		};
	enum TMode
		{
		EInsert,
		EOverwrite
		};
private:
	void SetState(TState aState);
private:
	MLineEditorObserver& iObserver;
	MLineCompleter& iCompleter;
	RConsole iConsole;
	TConsoleLine iLine;
	TBuf<KMaxLineLength> iLineBackup;
	CLineHistory* iHistory;
	TState iState;
	TMode iMode;
	};


#endif // __LINE_EDITOR_H__

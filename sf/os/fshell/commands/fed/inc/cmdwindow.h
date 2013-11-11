// cmdwindow.h
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
#ifndef CMDWINDOW_H_
#define CMDWINDOW_H_

#include <e32base.h>
#include "common.h"
#include <fshell/cconsolebase_writer.h>
#include <fshell/line_editor.h>

class CColorConsoleBase;
class TPoint;
class RFs;
class CLineEditor;
class CDesC16Array;

/*
 The purpose of this class is to handle so-called command window - a piece of of screen where different views can write their
 problems, query the user, etc. In general the command window is only one-line in height, but if it is necessary then it is
 possible to make the command window bigger. One can imagine that the user wants to open a new file and the editor allows
 to browse the directory and use auto-completion for specifying which file should be opened. That could be much better done
 if the command window was bigger than one line in height. To resize the command window CScreenManager::ResizeCommandWindowL
 should be called, which will first ensure that all open views are resized as well.
 
 The reason why there is a special class and views don't access the command window directly, is that at different times
 different classes would like to show something on the screen. For example when user wants to close an unsaved file then
 only a particular buffer (CFedBufferBase) would know it and should be able to query an user. Similarly when User enters
 an invalid char, or a combination of chars which make a complicated command, a particular View should be able to show a
 message or a text explaining the complicated command (or even the just entered command keys, like in emacs). Similarly
 when User is opening a new file and we don't know what is its type then we don't know which View should be used,
 and in that case DCommandWindow could have a function to allow the user browsing and selecting the required file.

 DCommandWindow is not responsible for controlling which class has access to the command window, it is only responsible
 for writing chars on the command window and holds some common function which use the command window (i.e. browsing for
 a new file). The access, which class should be allowed to access the command window, is controlled by CFed and
 CRequestHandler (which receives requests to access the command window from particular classes inheriting the
 MCommandWindowClient interface).
*/
class CCommandWindow : public CBase, public MConsoleProvider, public MLineEditorObserver, public MLineCompleter
	{
public:
	static CCommandWindow* NewL(RFs& aFs, CColorConsoleBase& aConsole);
	static CCommandWindow* NewL(RFs& aFs, CConsoleBase& aConsole);
	~CCommandWindow();

	//Sets the window which the command window can use
	void SetWindow(const TWindow& aWindow) {iWindow = aWindow;}

	CConsoleBase& Console();
	CColorConsoleBase* ColorConsole();
	void WriteStatus(const TDesC& aNameToTruncate, TRefByValue<const TDesC> aFmt, ...);
	void InfoPrint(TRefByValue<const TDesC> aFmt, ...);
	TKeyCode Query(const TDesC& aPrompt, const TDesC& aValidKeys);
	TBool QueryFilename(const TDesC& aPrompt, TFileName& aFileName);
	TBool QueryText(const TDesC& aPrompt, TDes& aText);
	RFs& Fs();

private:
	CCommandWindow(RFs& aFs, CConsoleBase& aConsole);
	CCommandWindow(RFs& aFs, CColorConsoleBase& aConsole);
	void ConstructL();
	void DoWriteLine(const TDesC& aLine, TInt aHighlightStart=0, TInt aHighlightLength=0);
	static TInt DismissInfoPrint(TAny* aSelf);
	TBool DoQueryText(const TDesC& aPrompt, TDes& aText);

private: // Line editor stuff
	void LeoHandleLine(const TDesC& aLine);
	void LcCompleteLineL(TConsoleLine& aLine, const TChar& aEscapeChar);

	void SuggestDrivesL(CDesC16Array* aSuggestions);
	void CompleteL(TConsoleLine& aLine, const CDesC16Array& aPossibilities, const TDesC* aPrefix, const TDesC* aSuffix);


private:
	RFs& iFs;
	CConsoleBase& iConsole;
	CColorConsoleBase* iColorConsole;
	TWindow iWindow;
	TBuf<256> iLastStatus;
	TBuf<256> iQuery;
	CPeriodic* iInfoPrintDismisser;
	CLineEditor* iGeneralLineEditor;
	CLineEditor* iFileNameLineEditor; // Have different editors because they have different histories
	CLineEditor* iLineEditor; // The current line editor (one of the above). Not owned.
	TConsoleBaseAdaptor iConsoleAdapter; // Needed by line editor
	TBool iFinished;
	TDes* iResultForQuery;
	};

#endif /*CMDWINDOW_H_*/

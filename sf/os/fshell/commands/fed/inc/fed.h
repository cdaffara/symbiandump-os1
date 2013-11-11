// fed.h
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
#ifndef FED_H_
#define FED_H_

#include <e32base.h>
#include <f32file.h>

#include "screenmngr.h"
#include "cmdwindow.h"

class CColorConsoleBase;
class CFedBufferBase;
class CFedViewBase;
class CDesC16Array;
class CCnvCharacterSetConverter;

class CFed : public CActive
{
public:
	static CFed* NewL(CColorConsoleBase& aConsole, RFs& aFs);
	static CFed* NewL(CConsoleBase& aConsole, RFs& aFs);
	~CFed();

	//Main entrance to the editor
	void StartL(CDesC16Array* aFiles); // Takes ownership of aFiles immediately

	void RedrawEverythingL();
	void ShowHelpL(HBufC* aHelpText);

public:
	//CActive
	void DoCancel();
	void RunL();
	TInt RunError(TInt aError);

private:
	CFed(CColorConsoleBase& aConsole, RFs& aFs);
	CFed(CConsoleBase& aConsole, RFs& aFs);
	void ConstructL();
	//Construct all necessary views as specified in the command line
	void CreateViewsL(CDesC16Array* aFiles);
	//Some standard commands which need to be handled in the CFed class
	enum TCmdType
		{
		ECmdNone,
		ECmdHelp,
		ECmdPrevView,
		ECmdNextView,
		ECmdNewDocument,
		ECmdCloseCurrent,
		ECmdCloseAll,
		ECmdOpenDocument,
		ECmdRefresh,
		};
	//Main function processing keys which should catch all commands related to the whole editor, not to any particular view
	TCmdType ProcessKeyL(const TKeyCode& aCode);
	//Execute commands returned by ProcessKeyL
	TBool ExecuteCommandL(TCmdType aCmd);

	//Closes the view which is currently active
	TBool CloseCurrentViewL();
	//Closes all views, will cause the program to close only if special char has been pressed (EKeyEscape)
	void CloseAllViewsL();
	//Removes the view from the array of views
	void DeleteView();
	void NewViewL(const TDesC& aFileName, TBool aPromptOnNonexistantName=EFalse);
	void OpenDocumentL();

private:
	RFs& iFs;
	CConsoleBase& iConsole;
	CColorConsoleBase* iColorConsole;
	CScreenManager iScreenMngr;
	CCommandWindow* iCmdWindow;

	RArray<CFedViewBase*> iViewArray;
	TInt iViewIdx;
	CCnvCharacterSetConverter* iCharconv; // Shared between CFileBuffers, for doing UTF-8 conversion. May be null if charconv isn't available
};

#endif /*FED_H_*/

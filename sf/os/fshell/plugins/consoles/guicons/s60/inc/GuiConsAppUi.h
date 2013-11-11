// GuiConsAppUi.h
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

#ifndef __GUICONSAPPUI_H__
#define __GUICONSAPPUI_H__

#include <aknappui.h>
#include <aknnavide.h>
#include <eikedwin.h>
#include <akntabobserver.h>

#include <fshell/guicons.h>
#include <fshell/settings.h>
#include <fshell/qr3dll.h>

class CGuiConsAppView;
class CAknTabGroup;
class CConsoleState;
class CCommandText;

class CGuiConsAppUi : public CAknAppUi
					, public MConsoleUi
					, public MAknTabObserver
					, public MCaptureInterface
	{
public:
	void ConstructL();
	CGuiConsAppUi();
	virtual ~CGuiConsAppUi();
	
	void SetNaviPaneTextL(const TDesC& aText);
	// from CCoeAppUi:
	virtual TKeyResponse HandleKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);
	// from MConsoleUi
	virtual void HandleNewConsoleL(CConsoleControl* aConsole);
	virtual void HandleConsoleClosed(CConsoleControl* aConsole);
	virtual void ConsoleTitleChangedL(const CConsoleControl*, const TDesC&);
	virtual TInt GetConsoleBufferSize();
	virtual const TDesC& GetConsoleFont();
private:
	enum TState
		{
		ENoConsole,
		EConsoleActive,
		EEdwinActive,
		EConsoleClosed,
		};
private:
	void HandleCommandL(TInt aCommand);
	void HandleStatusPaneSizeChange();
	void SetMenuItemDimmed(TInt aCommandId, CEikMenuPane* aMenuPane);
	void DynInitMenuPaneL(TInt aMenuId, CEikMenuPane* aMenuPane);
	TBool CommandValidInCurrentState(TInt aCommandId);
	
	void SetStateL(TState aNewState);
	void SetSoftkeysL();
	
	void NewFshellProcessL(const TDesC& aArgs);
	CConsoleState* StateForConsole(const CConsoleControl* aControl);
	void SetStateToPreferredL();
	void DeleteCurrentConsoleL();
	void SwitchTabL(TInt aTabIdx);
	void SetCapturingLongPressesL(TBool aEnable);
	void InputTextL(const TDesC& aText);
	void LoadCommandsL();
	
	void DisplayMessageL(TRefByValue<const TDesC> aFmt, ...);

private: // From MAknTabObserver
	void TabChangedL(TInt aIndex);
private: // From MCaptureInterface
	void CapturedKeyPressedL(TInt aIdentifier);

private:
	TState iState;
	TBool iEdwinWasLastRunningState;
	CGuiConsAppView* iAppView;
	CAknNavigationDecorator* iNaviDecorator;
	CAknNavigationControlContainer* iNaviPane;
	CAknTabGroup* iTabGroup;
	RPointerArray<CConsoleState> iTabs;
	TBool iShouldDisplayOnscreenNav;
	
	CServer2* iConsoleServer;

	CConsoleControl* iConsole; // The currently displayed console. Not owned
	CConsoleState* iTempState; // Used in case we get a call to SetTitle before HandleNewConsole
	RPointerArray<CCommandText> iCommands;
	
	LtkUtils::CIniFile* iIniFile;
	};
	
class CBlankControl : public CCoeControl
	{
public:
	virtual void Draw(const TRect& aRect) const;
	};

class CConsoleState : public CBase
	{
public:
	CConsoleState(CConsoleControl* aControl);
	~CConsoleState();

	CConsoleControl* iControl;
	TBool iConsoleClosed;
	TBuf<32> iTitle;
	};

class CCommandText : public CBase
	{
public:
	CCommandText(TInt aOrder);
	~CCommandText();
	static TInt Order(const CCommandText& aLeft, const CCommandText& aRight);
public:
	TInt iOrder;
	TBuf<16> iName;
	HBufC* iValue;
	};
	
#endif // __GUICONSAPPUI_H__

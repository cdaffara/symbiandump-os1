// GuiConsAppUi.cpp
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

#include <avkon.hrh>
#include <aknnotewrappers.h>
#include <stringloader.h>
#include <GuiCons.rsg>
#include <f32file.h>
#include <s32file.h>
#include <aknnavi.h>
#include <aknnavilabel.h>
#include <akntabgrp.h>
#include <akntitle.h>
#include <hal.h>
#include <caknmemoryselectiondialog.h>
#include <caknfileselectiondialog.h>
#include <fshell/ltkutils.h>

#include "GuiConsAppUi.h"
#include "GuiConsAppView.h"
#include "GuiCons.hrh"

#define EXPAND(L, X) L(X)
#define PANIC() User::Panic(EXPAND(_L, __FILE__), __LINE__)
#define GUICONS_ASSERT(x) __ASSERT_ALWAYS(x, PANIC())

_LIT(KServerName, "guicons");
_LIT(KNoConsole, "no console connected");
_LIT(KConsoleClosed, "console closed");
_LIT(KNewProcName, "fshell.exe");
_LIT(KNewProcArgs, "--console guicons");

_LIT(KIniFile, "\\system\\console\\guicons.ini");
_LIT(KIdfFile, "\\resource\\guicons.idf");
_LIT(KIniLongPress, "longpress");
_LIT(KIniShowNav, "shownav");
_LIT(KIniBufferSize, "buffersize");
_LIT(KIniFont, "font");

_LIT(KInvalidFontMsgFmt, "The file '%S' is not a valid font (error %d).\r\n\r\nA font file is a bitmap containing an 8x12 grid "
			L"of characters with ASCII codes 32 to 127. The width of the bitmap must be a multiple of 8, and the "
			L"height must be a multiple of 12.");
_LIT(KFontPreview, "New font:\r\nThe quick brown fox jumps over the lazy dog.");

void CGuiConsAppUi::ConstructL()
	{
	BaseConstructL(CAknAppUi::EAknEnableSkin);
	iTempState = new(ELeave) CConsoleState(NULL);
	iAppView = CGuiConsAppView::NewL( ClientRect() );
	
	iIniFile = LtkUtils::CIniFile::NewL(KIniFile, KIdfFile);
	
	iAppView->Layout();
	iAppView->ActivateL();

	CEikStatusPane* sp = StatusPane();
	iNaviPane = (CAknNavigationControlContainer*)sp->ControlL(TUid::Uid(EEikStatusPaneUidNavi));
	//iNaviDecorator = iNaviPane->CreateNavigationLabelL(_L("guicons"));
	iNaviDecorator = iNaviPane->CreateTabGroupL(this);
	iTabGroup = static_cast<CAknTabGroup*>(iNaviDecorator->DecoratedControl());
	iTabGroup->SetTabFixedWidthL(KTabWidthWithFourTabs);
	iNaviPane->PushL(*iNaviDecorator);

	SetStateL(ENoConsole);
	ConsoleTitleChangedL(NULL, KNullDesC);

	SetCapturingLongPressesL(iIniFile->GetBool(KIniLongPress));
	
	LtkUtils::CSetting* showNavSetting = iIniFile->GetSetting(KIniShowNav);
	if (!showNavSetting->IsSet())
		{
		TBool hasTouchScreen = EFalse;
		TInt err = HAL::Get(HAL::EPen, hasTouchScreen);
		if (!err && hasTouchScreen)
			{
			iShouldDisplayOnscreenNav = ETrue;
			}
		}
	else
		{
		iShouldDisplayOnscreenNav = showNavSetting->AsBool();
		}
	
	iConsoleServer = StartGuiConsServerL(KServerName, *this);	

	TRAP_IGNORE(LoadCommandsL());
	}

CGuiConsAppUi::CGuiConsAppUi()
	{
	}
	
void CGuiConsAppUi::SetNaviPaneTextL(const TDesC& aText)
	{
	if (iNaviDecorator->ControlType() == CAknNavigationDecorator::ENaviLabel)
		{
		CAknNaviLabel* naviLabel = (CAknNaviLabel*)iNaviDecorator->DecoratedControl();
		naviLabel->SetTextL(aText);
		iNaviPane->DrawNow();
		naviLabel->DrawNow();
		}
	}
	
TKeyResponse CGuiConsAppUi::HandleKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType)
	{
	if (aType == EEventKey)
		{
		switch (iState)
			{
		case EEdwinActive:
			GUICONS_ASSERT(iConsole);
			// fallthrough
		case EConsoleClosed:
			switch (aKeyEvent.iCode)
				{
			case EKeyEnter:
			case EKeyDevice3:
				if (iState == EEdwinActive) 
					{
					HBufC* text = iAppView->GetEditorTextL();
					if (text)
						{
							{
							CleanupStack::PushL(text);
							iConsole->InjectKeysL(*text);
							CleanupStack::PopAndDestroy();
							}
						iConsole->InjectKeysL(_L("\r"));
						iAppView->SetEditorTextL(NULL);
						//iAppView->Editor()->DrawNow();
						}
					SetStateL(iState);
					return EKeyWasConsumed;
					}
			case EKeyUpArrow:
				iConsole->ViewScrollUp();
				return EKeyWasConsumed;
			case EKeyDownArrow:
				iConsole->ViewScrollDown();
				return EKeyWasConsumed;
			case EKeyPageDown:
				iConsole->ViewPageDown();
				return EKeyWasConsumed;
			case EKeyPageUp:
				iConsole->ViewPageUp();
				return EKeyWasConsumed;
			case EKeyHome:
				iConsole->ViewHome();
				return EKeyWasConsumed;
			case EKeyEnd:
				iConsole->ViewEnd();
				return EKeyWasConsumed;
			default:
				break;
				}
		case EConsoleActive:
		case ENoConsole:
		default:
			return EKeyWasNotConsumed;
			}
		}
	return EKeyWasNotConsumed;
	}
	
	
void CGuiConsAppUi::SetSoftkeysL()
	{
	CEikButtonGroupContainer* cba = CEikButtonGroupContainer::Current();
	
	TInt softkeysResourceId;
	switch (iState)
		{
	case EConsoleActive:
		softkeysResourceId = R_GUICONS_INTERACTIVE_CBA;
		break;
	case EEdwinActive:
		softkeysResourceId = R_GUICONS_LINE_CBA;
		break;
	case EConsoleClosed:
		softkeysResourceId = R_GUICONS_CLOSED_CBA;
		break;
	case ENoConsole:
	default:
		softkeysResourceId = R_AVKON_SOFTKEYS_OPTIONS_EXIT;
		break;
		}
	cba->SetCommandSetL(softkeysResourceId);
	cba->DrawNow();
	}
	
void CGuiConsAppUi::SetStateL(TState aNewState)
	{
	switch (aNewState)
		{
	case ENoConsole:
		GUICONS_ASSERT(!iConsole);
		CCoeEnv::Static()->RootWin().CancelTextCursor();
		iAppView->ShowLabelL(KNoConsole);
		break;
		
	case EConsoleActive:
		GUICONS_ASSERT(iConsole);
		iAppView->FocusEditor(EFalse);
		iAppView->RemoveEditorFromStack(this);

		RemoveFromStack(iConsole);
		AddToStackL(iConsole);
		iConsole->SetFocusing(ETrue);
		iConsole->SetFocus(ETrue);
		iAppView->ShowEditor();
		iEdwinWasLastRunningState = EFalse;
		break;
		
	case EEdwinActive:
		GUICONS_ASSERT(iConsole);
		iConsole->SetFocus(EFalse);
		iConsole->SetFocusing(EFalse);
		RemoveFromStack(iConsole);
		iAppView->ShowEditor();
		iAppView->FocusEditor(ETrue);
		
		iAppView->RemoveEditorFromStack(this);
		iAppView->AddEditorToStackL(this);

		iAppView->ShowEditor();
		iEdwinWasLastRunningState = ETrue;
		break;
		
	case EConsoleClosed:
		GUICONS_ASSERT(iConsole);
		iAppView->FocusEditor(EFalse);
		iAppView->RemoveEditorFromStack(this);
		RemoveFromStack(iConsole);
		iAppView->ShowLabelL(KConsoleClosed);
		CCoeEnv::Static()->RootWin().CancelTextCursor();
		break;
		}
	iState = aNewState;
	SetSoftkeysL();
	}
	
void CGuiConsAppUi::HandleNewConsoleL(CConsoleControl* aConsole)
	{
	CConsoleState* newState = new(ELeave) CConsoleState(aConsole);
	iTabs.AppendL(newState);
	if (aConsole == iTempState->iControl)
		{
		newState->iTitle.Copy(iTempState->iTitle);
		iTempState->iControl = NULL;
		}
	TBuf<2> text;
	text.Num(iTabs.Count());
	iTabGroup->AddTabL(reinterpret_cast<TInt>(newState), text);
	SwitchTabL(iTabs.Count()-1);
	SetStateToPreferredL();
	aConsole->ActivateL();
	}
	
void CGuiConsAppUi::HandleConsoleClosed(CConsoleControl* aConsole)
	{
	CConsoleState* state = StateForConsole(aConsole);
	if (state)
		{
		state->iConsoleClosed = ETrue;
		}

	if (aConsole && (aConsole == iConsole))
		{
		TRAP_IGNORE(SetStateL(EConsoleClosed));
		}
	}
	
void CGuiConsAppUi::ConsoleTitleChangedL(const CConsoleControl* aConsole, const TDesC& aTitle)
	{
	//SetNaviPaneTextL(aTitle);

	// Put it in the title so the navi bar is free for tabs
	if (aConsole == iConsole)
		{
		// If aConsole isn't current, we don't want to update anything
		CEikStatusPane* sp = StatusPane();
		CAknTitlePane* titlePane = NULL;
		if (sp) titlePane = (CAknTitlePane*)sp->ControlL(TUid::Uid(EEikStatusPaneUidTitle));
		if (titlePane)
			{
			if (aTitle.Length())
				{
				titlePane->SetTextL(aTitle);
				}
			else
				{
				titlePane->SetTextL(_L("GuiCons"));
				}
			}
		}

	// And update the state
	CConsoleState* state = StateForConsole(aConsole);
	if (state)
		{
		state->iTitle = aTitle.Left(state->iTitle.MaxLength());
		}
	else
		{
		iTempState->iControl = const_cast<CConsoleControl*>(aConsole);
		iTempState->iTitle = aTitle.Left(iTempState->iTitle.MaxLength());
		}
	}
	
TInt CGuiConsAppUi::GetConsoleBufferSize()
	{
	return iIniFile->GetInt(KIniBufferSize);
	}
	
const TDesC& CGuiConsAppUi::GetConsoleFont()
	{
	return iIniFile->GetString(KIniFont);
	}

CGuiConsAppUi::~CGuiConsAppUi()
	{
	CKeyCapturer::DeleteCapturer();

	iAppView->RemoveEditorFromStack(this);
	delete iConsoleServer;
	iTabs.ResetAndDestroy();
	delete iNaviDecorator;
	delete iAppView;
	delete iTempState;
	iCommands.ResetAndDestroy();
	delete iIniFile;
	}

void CGuiConsAppUi::HandleCommandL(TInt aCommand)
	{
	if (!CommandValidInCurrentState(aCommand))
		{
		_LIT(KNotAvailableNow, "Not available now");
		iEikonEnv->InfoMsg(KNotAvailableNow);
		return;
		}
	
	switch(aCommand)
		{
	case EEikCmdExit:
	case EAknSoftkeyExit:
		Exit();
		break;

	case EGuiConsFocusConsole:
		SetStateL(EConsoleActive);
		break;
	case EGuiConsFocusEdwin:
		SetStateL(EEdwinActive);
		break;
	case EGuiConsPageUp:
		iConsole->ViewPageUp();
		break;
	case EGuiConsPageDown:
		iConsole->ViewPageDown();
		break;
	case EGuiConsHome:
		iConsole->ViewHome();
		break;
	case EGuiConsEnd:
		iConsole->ViewEnd();
		break;
	case EGuiConsCloseConsole:
		DeleteCurrentConsoleL();
		break;
	case EGuiConsNewConsole:
		NewFshellProcessL(KNullDesC);
		break;
	case EGuiConsCtrlC:
		{
		TKeyEvent key;
		key.iCode = CTRL('c');
		key.iScanCode = 0;
		key.iRepeats = 0;
		key.iModifiers = EModifierLeftCtrl | EModifierCtrl;
		iConsole->OfferKeyEventL(key, EEventKey);
		break;
		}
	case EGuiConsShow5Way:
		{
		iShouldDisplayOnscreenNav = !iShouldDisplayOnscreenNav;
		if (iConsole) iConsole->SetDisplayFivewayNav(iShouldDisplayOnscreenNav);
		TRAP_IGNORE(iIniFile->SetL(KIniShowNav, iShouldDisplayOnscreenNav); iIniFile->WriteL());
		break;
		}
	case EGuiConsLongPressSwitchesTabs:
		{
		TBool capturing = CKeyCapturer::GetCapturerL()->IsCaptured(EKeyLeftArrow);
		SetCapturingLongPressesL(!capturing);
		TRAP_IGNORE(iIniFile->SetL(KIniLongPress, !capturing); iIniFile->WriteL());
		break;
		}
	case EGuiConsChooseFont:
		{
		// Note, we have to use the memory and file selection dialogs seperately
		// so that we can choose things from C: outside of C:\Data
		
		// choose drive
		// TODO see if we can do something better here, so we can also choose files from Z: and
		// other drives
		CAknMemorySelectionDialog::TMemory memory = CAknMemorySelectionDialog::EPhoneMemory;
		if (!CAknMemorySelectionDialog::RunDlgLD(memory))
			{
			break;
			}
		TChar drive = (memory == CAknMemorySelectionDialog::EPhoneMemory) ? 'C' : 'E';
		
		TFileName* fn = new(ELeave)TFileName;
		CleanupStack::PushL(fn);
		fn->Append(drive);
		fn->Append(_L(":\\"));
		CAknFileSelectionDialog* fileSel = CAknFileSelectionDialog::NewL(ECFDDialogTypeSelect);
		CleanupStack::PushL(fileSel);
		fileSel->SetDefaultFolderL(_L("\\"));
		if (fileSel->ExecuteL(*fn))
			{
			
			// verify that the file chosen is a valid font
			CConsoleFont* font = NULL;
			TRAPD(err, font = CConsoleFont::NewL(*fn));
			if (err!=KErrNone)
				{
				DisplayMessageL(KInvalidFontMsgFmt, fn, err);
				}
			else
				{
				TRAP_IGNORE(iIniFile->SetL(KIniFont, *fn); iIniFile->WriteL());
				DisplayMessageL(KFontPreview);
				}
			delete font;
			}
		CleanupStack::PopAndDestroy(2, fn); // fn, fileSel
		break;
		}
	
	case EGuiConsResetFont:
		iIniFile->RemoveValueL(KIniFont);
		iIniFile->WriteL();
		break;
		
	default:		
		break;
		}

	if (aCommand >= ETabStart)
		{
		TInt tabIdx = aCommand - ETabStart;
		SwitchTabL(tabIdx);
		}
	else if (aCommand >= ECommandStart)
		{
		TInt cmdIdx = aCommand - ECommandStart;
		InputTextL(*iCommands[cmdIdx]->iValue);
		}
	}
	
void CGuiConsAppUi::NewFshellProcessL(const TDesC& aArgs)
	{
	RBuf args;
	args.Create(aArgs.Length()+1+KNewProcArgs().Length());
	CleanupClosePushL(args);
	args.Append(KNewProcArgs);
	args.Append(' ');
	args.Append(aArgs);
	
	RProcess proc;
	User::LeaveIfError(proc.Create(KNewProcName, args));
	proc.Resume();
	proc.Close();
	CleanupStack::PopAndDestroy(&args);
	}

void CGuiConsAppUi::HandleStatusPaneSizeChange()
	{
	iAppView->SetRect( ClientRect() );
	}
	
void CGuiConsAppUi::SetMenuItemDimmed(TInt aCommandId, CEikMenuPane* aMenuPane)
	{
	aMenuPane->SetItemDimmed(aCommandId, !CommandValidInCurrentState(aCommandId));
	}
	
void CGuiConsAppUi::DynInitMenuPaneL(TInt aMenuId, CEikMenuPane* aMenuPane)
	{
	if (aMenuId == R_GUICONS_MENU)
		{
		SetMenuItemDimmed(EAknSoftkeyExit, aMenuPane);
		for (TInt i = EGuiConsCommandFirst; i<EGuiConsCommandLast; ++i)
			{
			SetMenuItemDimmed(i, aMenuPane);
			}
		}
	else if (aMenuId == R_GUICONS_TABS_SUBMENU)
		{
		for (TInt i = 0; i < iTabs.Count(); i++)
			{
			CConsoleState* state = iTabs[i];
			CEikMenuPaneItem::SData menu;
			menu.iCommandId = ETabStart + i;
			menu.iCascadeId = 0;
			menu.iFlags = 0;
			menu.iText = state->iTitle;
			aMenuPane->AddMenuItemL(menu);
			}
		}
	else if (aMenuId == R_GUICONS_SETTINGS_SUBMENU)
		{
		TBool capturing = CKeyCapturer::GetCapturerL()->IsCaptured(EKeyLeftArrow);
		aMenuPane->SetItemButtonState(EGuiConsLongPressSwitchesTabs, capturing ? EEikMenuItemSymbolOn : 0);
		aMenuPane->SetItemButtonState(EGuiConsShow5Way, iShouldDisplayOnscreenNav ? EEikMenuItemSymbolOn : 0);
		SetMenuItemDimmed(EGuiConsResetFont, aMenuPane);
		}
	else if (aMenuId == R_GUICONS_COMMANDS_SUBMENU)
		{
		for (TInt i = 0; i < iCommands.Count(); i++)
			{
			CCommandText* command = iCommands[i];
			CEikMenuPaneItem::SData menu;
			menu.iCommandId = ECommandStart + i;
			menu.iCascadeId = 0;
			menu.iFlags = 0;
			menu.iText = command->iName;
			aMenuPane->AddMenuItemL(menu);
			}
		}
	}
	
TBool CGuiConsAppUi::CommandValidInCurrentState(TInt aCommandId)
	{
	switch (aCommandId)
		{
	case EEikCmdExit:
	case EAknSoftkeyExit:
		return (iState == ENoConsole || iState == EConsoleClosed);
	case EGuiConsFocusConsole:
		return (iState == EEdwinActive);
	case EGuiConsFocusEdwin:
		return (iState == EConsoleActive);
	case EGuiConsPageUp:
	case EGuiConsPageDown:
	case EGuiConsHome:
	case EGuiConsEnd:
		return (iState == EEdwinActive) || (iState == EConsoleActive) || (iState == EConsoleClosed);
	case EGuiConsCloseConsole:
		return (iState == EConsoleClosed);
	case EGuiConsNewConsole:
		return ETrue; //(iState == ENoConsole);
	case EGuiConsSwitchTab:
		return iTabs.Count() > 1;
	case EGuiConsCtrlC:
		return (iState == EEdwinActive) || (iState == EConsoleActive);
	case EGuiConsNavigationSubMenu:
		return CommandValidInCurrentState(EGuiConsHome) || CommandValidInCurrentState(EGuiConsEnd) || CommandValidInCurrentState(EGuiConsPageUp) || CommandValidInCurrentState(EGuiConsPageDown);
	case EGuiConsCommandsSubMenu:
		return (iState == EEdwinActive) || (iState == EConsoleActive);
	case EGuiConsResetFont:
		return iIniFile->GetSetting(KIniFont)->IsSet();
	default:
		return ETrue;
		}
	}


void CBlankControl::Draw(const TRect& aRect) const
	{
	CWindowGc& gc = SystemGc();
	gc.SetPenStyle(CGraphicsContext::ENullPen);
	gc.SetBrushStyle(CGraphicsContext::ESolidBrush);
	gc.SetBrushColor(TRgb(0x80, 0x80, 0x80));
	gc.DrawRect(aRect);
	}

void CGuiConsAppUi::TabChangedL(TInt aIndex)
	{
	//__DEBUGGER();
	CConsoleState* newTabState = (CConsoleState*)(TAny*)iTabGroup->TabIdFromIndex(aIndex);
	CConsoleControl* newTab = newTabState->iControl;
	CConsoleControl* oldTab = iConsole;
	iConsole = newTab;

	// Update what's on screen, and what has keyboard focus
	if (oldTab)
		{
		// Could be null, maybe
		RemoveFromStack(oldTab);
		oldTab->MakeVisible(EFalse);
		}
	if (newTab)
		{
		// Could be null if we haven't implemented this tab yet
		//AppUi()->AddToStackL(*this, newTab, ECoeStackPriorityDefault-1); // One less than the tab bar to ensure we don't unintentionally consume left and right arrows
		AddToStackL(newTab);
		newTab->MakeVisible(ETrue);
		newTab->SetDisplayFivewayNav(iShouldDisplayOnscreenNav); // Make sure this is sync'd
		}
	iAppView->ClearLargeControl();
	iAppView->SetLargeControlL(iConsole);
	if (newTabState->iConsoleClosed)
		{
		SetStateL(EConsoleClosed);
		}
	else
		{
		SetStateToPreferredL();
		}
	ConsoleTitleChangedL(iConsole, newTabState->iTitle);
	}

CConsoleState* CGuiConsAppUi::StateForConsole(const CConsoleControl* aControl)
	{
	for (TInt i = 0; i < iTabs.Count(); i++)
		{
		if (iTabs[i]->iControl == aControl) return iTabs[i];
		}
	return NULL;
	}

void CGuiConsAppUi::SetStateToPreferredL()
	{
	if (iEdwinWasLastRunningState)
		{
		SetStateL(EEdwinActive);
		}
	else
		{
		SetStateL(EConsoleActive);
		}
	}

void CGuiConsAppUi::DeleteCurrentConsoleL()
	{
	TInt found = KErrNotFound;
	for (TInt i = 0; i < iTabs.Count(); i++)
		{
		if (iTabs[i]->iControl == iConsole)
			{
			found = i;
			break;
			}
		}
	GUICONS_ASSERT(found >= 0);

	CConsoleState* state = iTabs[found];
	iTabs.Remove(found);
	CleanupStack::PushL(state); // Keep the state around until we've finished switching, just in case
	iTabGroup->DeleteTabL((TInt)state);
	if (found >= iTabs.Count()) found = iTabs.Count()-1;
	if (found >= 0)
		{
		SwitchTabL(found);
		}
	else
		{
		iConsole = NULL;
		iAppView->ClearLargeControl();
		SetStateL(ENoConsole);
		ConsoleTitleChangedL(NULL, KNullDesC);
		}
	CleanupStack::PopAndDestroy(state);
	}

void CGuiConsAppUi::CapturedKeyPressedL(TInt aIdentifier)
	{
	TKeyEvent key;
	key.iCode = aIdentifier;
	key.iRepeats = 0;
	key.iModifiers = 0;
	switch (aIdentifier)
		{
		case EKeyLeftArrow:
			key.iScanCode = EStdKeyLeftArrow;
			iTabGroup->OfferKeyEventL(key, EEventKey);
			break;
		case EKeyRightArrow:
			key.iScanCode = EStdKeyRightArrow;
			iTabGroup->OfferKeyEventL(key, EEventKey);
			break;
		default:
			break;
		}
	}

void CGuiConsAppUi::SwitchTabL(TInt aTabIdx)
	{
	iTabGroup->SetActiveTabByIndex(aTabIdx);
	TabChangedL(aTabIdx);
	}

void CGuiConsAppUi::SetCapturingLongPressesL(TBool aEnable)
	{
	CKeyCapturer::GetCapturerL()->RemoveCapture(this, EKeyLeftArrow);
	CKeyCapturer::GetCapturerL()->RemoveCapture(this, EKeyRightArrow);
	if (aEnable)
		{
		CKeyCapturer::GetCapturerL()->AddLongCaptureL(this, EKeyLeftArrow, EKeyLeftArrow, EStdKeyLeftArrow);
		CKeyCapturer::GetCapturerL()->AddLongCaptureL(this, EKeyRightArrow, EKeyRightArrow, EStdKeyRightArrow);
		}
	}

void CGuiConsAppUi::InputTextL(const TDesC& aText)
	{
	if (iState == EEdwinActive)
		{
		iAppView->SetEditorTextL(&aText);
		}
	else
		{
		iConsole->InjectKeysL(aText);
		}
	}

void CGuiConsAppUi::LoadCommandsL()
	{
	_LIT(KCommandsDir, "\\system\\console\\commands\\");
	_LIT(KCommandNameMatch, "*.txt");
	TFindFile findfile(iEikonEnv->FsSession());
	CDir* list = NULL;
	TInt found = findfile.FindWildByDir(KCommandsDir, KCommandNameMatch, list);
	TFileName filename;
	while (found == KErrNone)
		{
		filename = TParsePtrC(findfile.File()).DriveAndPath();
		TInt dirLen = filename.Length();
		CleanupStack::PushL(list);
		for (TInt i = 0; i < list->Count(); i++)
			{
			const TEntry& entry = (*list)[i];
			const TDesC& name = entry.iName;
			filename.SetLength(dirLen);
			filename.Append(name);
			TLex lex(name);
			TInt order = 0;
			lex.Val(order);
			if (lex.Peek() == '-') lex.Inc();
			CCommandText* cmd = new(ELeave) CCommandText(order);
			CleanupStack::PushL(cmd);
			cmd->iName = TParsePtrC(lex.Remainder()).Name().Left(cmd->iName.MaxLength());
			LtkUtils::ReplaceText(cmd->iName, _L("_"), _L(" "));
			cmd->iValue = HBufC::NewMaxL(entry.iSize);
			TPtr8 narrow((TUint8*)cmd->iValue->Ptr(), 0, entry.iSize*2);
			User::LeaveIfError(iEikonEnv->FsSession().ReadFileSection(filename, 0, narrow, entry.iSize));
			narrow.Expand();
			iCommands.InsertInOrderAllowRepeatsL(cmd, TLinearOrder<CCommandText>(&CCommandText::Order));
			CleanupStack::Pop(cmd);
			}
		CleanupStack::PopAndDestroy(list);
		found = findfile.FindWild(list);
		}
	}
	
_LIT(KShellEchoArgsStart, "--exec \"echo --wrap ^\"");
_LIT(KShellEchoArgsEnd, "^\"\"");
_LIT(KEsc, "^^");

TBool NeedsEscape(TChar aChar)
	{
	return (aChar == '"');
	}

void CGuiConsAppUi::DisplayMessageL(TRefByValue<const TDesC> aFmt, ...)
	{
	VA_LIST list;
	VA_START(list, aFmt);

	IoUtils::CTextBuffer* msgBuf = IoUtils::CTextBuffer::NewLC(0x20);
	msgBuf->AppendFormatListL(aFmt, list);	
	
	IoUtils::CTextBuffer* buf = IoUtils::CTextBuffer::NewLC(0x20);
	
	buf->AppendL(KShellEchoArgsStart);
	const TDesC& msg(msgBuf->Descriptor());
	
	for (TInt i=0; i<msg.Length(); ++i)
		{
		if (msg[i] == '\n')
			{
			buf->AppendL(KEsc);
			buf->AppendL('n');
			}
		else if (msg[i] == '\r')
			{
			buf->AppendL(KEsc);
			buf->AppendL('r');
			}
		else if (NeedsEscape(msg[i]))
			{
			buf->AppendL(KEsc);
			buf->AppendL(msg[i]);
			}
		else
			{
			buf->AppendL(msg[i]);
			}
		}
	buf->AppendL(KShellEchoArgsEnd);
	
	NewFshellProcessL(buf->Descriptor());
	CleanupStack::PopAndDestroy(2, msgBuf);
	}

// CConsoleState

CConsoleState::CConsoleState(CConsoleControl* aControl)
: iControl(aControl), iConsoleClosed(EFalse)
	{
	}

CConsoleState::~CConsoleState()
	{
	delete iControl;
	}

// CCommandText

CCommandText::CCommandText(TInt aOrder)
	: iOrder(aOrder)
	{}

TInt CCommandText::Order(const CCommandText& aLeft, const CCommandText& aRight)
	{
	return aLeft.iOrder - aRight.iOrder;
	}

CCommandText::~CCommandText()
	{
	delete iValue;
	}

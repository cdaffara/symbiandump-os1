// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//



// INCLUDE FILES
#include <e32cons.h>
#include <bacline.h>
#include "epos_cpospsyconsolemanager.h"
#include "epos_cpospsyconsoleview.h"
#include "epos_cpospsystartview.h"
#include "epos_cpospsysettingsview.h"
#include "epos_cpospsylistview.h"
#include "epos_cpospsycmdlineview.h"

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CPosPSYConsoleManager::CPosPSYConsoleManager() 
	: CActive(CActive::EPriorityUserInput), iTestScope(-1) 
    {
    CActiveScheduler::Add(this);
    }

// EPOC default constructor can leave.
void CPosPSYConsoleManager::ConstructL(
	CPosPSYTestHandler* aPSYTestHandler)
    {
    iTestHandler = aPSYTestHandler;
	
	_LIT(KPSYTester, "PSY Tester");
    iConsole = Console::NewL(KPSYTester, TSize(KConsFullScreen, KConsFullScreen));

    _LIT(KRow1, "Loading positioning\n");
    _LIT(KRow2, "plug-in modules... \n");
    TInt xPos = (iConsole->ScreenSize().iWidth - KRow1().Length()) / 2;
    TInt yPos = 5;
    iConsole->SetPos(xPos, yPos++);
    iConsole->Printf(KRow1);
    iConsole->SetPos(xPos, yPos);
    iConsole->Printf(KRow2);

    CPosPSYStartView* startView = CPosPSYStartView::NewL(iConsole);
    CleanupStack::PushL(startView);
    startView->SetViewManager(this);
    User::LeaveIfError(iViewList.Append(startView));
    CleanupStack::Pop(startView);

	CPosPSYListView* listView = CPosPSYListView::NewL(iConsole, iTestHandler);
    CleanupStack::PushL(listView);
    listView->SetViewManager(this);
    User::LeaveIfError(iViewList.Append(listView));
    CleanupStack::Pop(listView);

    CPosPSYSettingsView* settingsView = CPosPSYSettingsView::NewL(iConsole, iTestHandler);
    CleanupStack::PushL(settingsView);
    settingsView->SetViewManager(this);
    User::LeaveIfError(iViewList.Append(settingsView));
    CleanupStack::Pop(settingsView);

    CPosPSYCmdlineView* cmdLineView = CPosPSYCmdlineView::NewL(iConsole);
    CleanupStack::PushL(cmdLineView);
    cmdLineView->SetViewManager(this);
    User::LeaveIfError(iViewList.Append(cmdLineView));
    CleanupStack::Pop(cmdLineView);
    
    iActiveView = startView;
    }

// Two-phased constructor.
CPosPSYConsoleManager* CPosPSYConsoleManager::NewL(
	CPosPSYTestHandler* aPSYTestHandler)
    {
    CPosPSYConsoleManager* self = new (ELeave) CPosPSYConsoleManager;
    CleanupStack::PushL(self);
    self->ConstructL(aPSYTestHandler);
    CleanupStack::Pop(self);
    return self;
    }

// Destructor
CPosPSYConsoleManager::~CPosPSYConsoleManager()
    {
	Cancel();
	delete iConsole;
    iViewList.ResetAndDestroy();
    }

// ---------------------------------------------------------
// CPosPSYConsoleManager::StartL
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYConsoleManager::StartL()
    {
    _LIT(KAllPsys, "-a");
	_LIT(KOnePsy, "-p");
	CCommandLineArguments* args = CCommandLineArguments::NewL();
	for (int i=1; i<args->Count() && iTestScope == -1; i++) 
		{
		TPtrC arg = args->Arg(i);
		if (arg.Find(KAllPsys) >= 0) 
			{
			// Run All PSYs, PSY = 0, mean testing all PSYS
			iTestScope = 0;
			}
		else if (arg.Find(KOnePsy) >= 0) 
			{
			// Run One PSY
			TPtrC uidstr = arg.Right(8);
			TLex lex = TLex(uidstr);
			TUint uidint;
			lex.Val(uidint, EHex);
			
			TInt psys = iTestHandler->InfoList().Count();
			for (int j=0; j<psys; j++) 
				{
				if ((iTestHandler->InfoList()[j]).Uid().iUid == uidint) 
					{
					iTestScope = j+1;
					break;
					}
				}
			}
		}
	delete args;

	if (iTestScope != -1) 
		{
		iTestHandler->StartPSYTesterL(iTestScope);
	    // SwitchViewL(KPSYListView);        
	    SwitchViewL(KCmdLineView);        
		}
	else {	
	    SwitchViewL(KStartView);
	    RequestCharacter();
		}
    }

// ---------------------------------------------------------
// CPosPSYConsoleManager::SetTestState
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYConsoleManager::SetTestState(
	TInt aTestState)
	{	
	// If tests has completed and command line arguments has been supplied, 
	// PSY Tester should close
	if ((aTestState == ETestsCompletedCorrect || 
		aTestState == ETestsCompletedWithError) && iTestScope != -1) 
		{
	    CActiveScheduler::Stop();		
		}
	else 
		{
		iActiveView->HandleTestEvent(aTestState);
		}
	}

// ---------------------------------------------------------
// CPosPSYConsoleManager::RunL
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYConsoleManager::RunL()
   {
   if (iStatus == KErrNone)
        {
        ProcessKeyPressL(iConsole->KeyCode());
        }    
   }   

// ---------------------------------------------------------
// CPosPSYConsoleManager::DoCancelL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYConsoleManager::DoCancel()
   {
   iConsole->ReadCancel();
   }

// ---------------------------------------------------------
// CPosPSYConsoleManager::RunError
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CPosPSYConsoleManager::RunError(
    TInt aError)
	{
	iConsole->ReadCancel();
    CActiveScheduler::Stop();
    return aError;
	}

// ---------------------------------------------------------
// CPosPSYConsoleManager::SwitchViewL
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYConsoleManager::SwitchViewL(
	const TUid& aViewId)
    {
    iActiveView->DeActivate();

    if (aViewId == KPreviousView)
        {
        CPosPSYConsoleView* tempView = iActiveView;
        iActiveView = iOldView;
        iOldView = tempView;
        }
    else
        {
        iOldView = iActiveView;
        iActiveView = iViewList[aViewId.iUid];
        }

    iActiveView->ActivateL();
    }

// ---------------------------------------------------------
// CPosPSYConsoleManager::ProcessKeyPressL
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYConsoleManager::ProcessKeyPressL(
    TKeyCode aKey)
    {
    if (aKey == EKeyRightArrow || // Switch view
        aKey == EKeyLeftArrow ||  // Switch view
        aKey == EKeyUpArrow ||    // Scroll up
        aKey == EKeyDownArrow ||  // Scroll down
        aKey == EKeyDevice3 ||    // Stick down (S60)
        aKey == EKeyEnter ||      // Select (corresponding to stick down for target
        aKey == EKeyDevice0 ||	  // Left softkey(S60)
		((aKey >= '0') && (aKey <= '9')) || // 1-9  
		((aKey == 'R') || (aKey == 'r')) || // Run All
		((aKey == 'C') || (aKey == 'c')) || // Cancel
		((aKey == 'L') || (aKey == 'l')) || // List form beginning
		((aKey == 'H') || (aKey == 'h')) || // Help
		((aKey == 'N') || (aKey == 'n')) || // List Next
		((aKey == 'H') || (aKey == 'h')) || // List Former
		((aKey == 'U' )|| (aKey == 'u')))   // Undo 
        {
        // let the views handle these events
        iActiveView->HandleKeyEventL(aKey);
        }
	else if (aKey == 'X' || aKey == 'x'|| aKey == EKeyDevice1) // Right softkey, Exit
        {
        CActiveScheduler::Stop();
        }
    
	RequestCharacter();
    }

// ---------------------------------------------------------
// CPosPSYConsoleManager::RequestCharacter
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYConsoleManager::RequestCharacter()
   {
   iConsole->Read(iStatus);
   SetActive();
   }

//  End of File

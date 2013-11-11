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
#include "epos_cpospsystartview.h"
#include "epos_mpospsyviewmanager.h"

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CPosPSYStartView::CPosPSYStartView(
    CConsoleBase* aConsole)
	: CPosPSYConsoleView(aConsole)
    {
    }

// EPOC default constructor can leave.
void CPosPSYStartView::ConstructL()
    {
    }

// Two-phased constructor.
CPosPSYStartView* CPosPSYStartView::NewL(
    CConsoleBase* aConsole)
    {
    CPosPSYStartView* self = new (ELeave) CPosPSYStartView(
		aConsole);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// Destructor
CPosPSYStartView::~CPosPSYStartView()
    {
    }

// ---------------------------------------------------------
// CPosPSYStartView::ActivateL
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYStartView::ActivateL()
    {
    ReDraw();
    }

// ---------------------------------------------------------
// CPosPSYStartView::HandleKeyEventL
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYStartView::HandleKeyEventL(
    TKeyCode aKey)
    {
    if (aKey == EKeyDevice3 || 
		aKey == EKeySpace || 
		aKey == EKeyRightArrow || // joy-stick right, space
		aKey == EKeyDevice1) // right soft-key
        {
        iViewManager->SwitchViewL(KPSYListView);
        }
    else if (IsHelpRequested(aKey))
        {
        PrintHelp();
        }
    }

// ---------------------------------------------------------
// CPosPSYStartView::HandleTestEvent
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//	
void CPosPSYStartView::HandleTestEvent(TInt /*aEvent*/)
	{
	}

// ---------------------------------------------------------
// CPosPSYStartView::PrintHelp
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYStartView::PrintHelp()
    {
   	_LIT(KNumber, "1-N: Select PSY\n");
    _LIT(KRunAll, "R: Test all PSYs\n");
    _LIT(KHelp, "H: Help list\n");
    _LIT(KCancel, "C: Cancel tests\n");
    _LIT(KUndo, "U: Undo selection\n");
	_LIT(KList, "L: List PSYs\n");
    _LIT(KNext, "Joystick down:\n");
	_LIT(KNext2," List next\n");
    _LIT(KForward, "Joystick right:\n"); 
	_LIT(KForward2, " Shift view\n");
	_LIT(KBack, "Joystick left:\n"); 
	_LIT(KBack2, " Shift view\n");
	_LIT(KExit, "X: Exit PSY Tester\n");

    iConsole->ClearScreen();
    iConsole->Printf(KNumber);
    iConsole->Printf(KRunAll);
    iConsole->Printf(KHelp);
    iConsole->Printf(KCancel);
	iConsole->Printf(KUndo);
    iConsole->Printf(KList);
    iConsole->Printf(KNext);
	iConsole->Printf(KNext2);
	iConsole->Printf(KForward);
	iConsole->Printf(KForward2);
	iConsole->Printf(KBack);
	iConsole->Printf(KBack2);
    iConsole->Printf(KExit);
    }

// ---------------------------------------------------------
// CPosPSYStartView::ReDraw
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYStartView::ReDraw()
    {
    //_LIT(KRow1, "Welcome to\n"); 
	//_LIT(KRow2,	"PSY Tester!\n");
    //_LIT(KRow3, "Press left soft\n");
    //_LIT(KRow4, "key or H for help.\n");
    
    _LIT(KRow1, "Welcome to \n"); 
	_LIT(KRow2,	"PSY Tester!\n");
	_LIT(KRow3,	"Please Note :");
	_LIT(KRow4,	"Location Server");
	_LIT(KRow5,	"should not be used");
	_LIT(KRow6,	"during test");
	_LIT(KRow7,	"execution");
    _LIT(KRow8, "Press left soft\n");
    _LIT(KRow9, "key or H for help.\n");

    iConsole->ClearScreen();
    TInt width = iConsole->ScreenSize().iWidth;
    TInt xPos = (width - KRow4().Length()) / 2;
    TInt yPos = KDefaultYPos;

    iConsole->SetPos(xPos ,yPos++);
    iConsole->Printf(KRow1);
    iConsole->SetPos(xPos, yPos++);
    iConsole->Printf(KRow2);
    yPos ++;
    iConsole->SetPos(xPos, yPos++);
    iConsole->Printf(KRow3);
	iConsole->SetPos(xPos, yPos++);
    iConsole->Printf(KRow4);
    iConsole->SetPos(xPos, yPos++);
    iConsole->Printf(KRow5);
    iConsole->SetPos(xPos, yPos++);
    iConsole->Printf(KRow6);
    iConsole->SetPos(xPos, yPos++);
    iConsole->Printf(KRow7);
    yPos++;
    iConsole->SetPos(xPos, yPos++);
    iConsole->Printf(KRow8);
    iConsole->SetPos(xPos, yPos);
    iConsole->Printf(KRow9);
    }

//  End of File

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
#include "epos_cpospsycmdlineview.h"
#include "epos_mpospsyviewmanager.h"

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CPosPSYCmdlineView::CPosPSYCmdlineView(
    CConsoleBase* aConsole)
	: CPosPSYConsoleView(aConsole)
    {
    }

// EPOC default constructor can leave.
void CPosPSYCmdlineView::ConstructL()
    {
    }

// Two-phased constructor.
CPosPSYCmdlineView* CPosPSYCmdlineView::NewL(
    CConsoleBase* aConsole)
    {
    CPosPSYCmdlineView* self = new (ELeave) CPosPSYCmdlineView(
		aConsole);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// Destructor
CPosPSYCmdlineView::~CPosPSYCmdlineView()
    {
    }

// ---------------------------------------------------------
// CPosPSYCmdlineView::ActivateL
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYCmdlineView::ActivateL()
    {
    ReDraw();
    }

// ---------------------------------------------------------
// CPosPSYCmdlineView::HandleKeyEventL
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYCmdlineView::HandleKeyEventL(
    TKeyCode /*aKey*/)
    {
    }

// ---------------------------------------------------------
// CPosPSYCmdlineView::HandleTestEvent
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//	
void CPosPSYCmdlineView::HandleTestEvent(TInt /*aEvent*/)
	{
	}

// ---------------------------------------------------------
// CPosPSYCmdlineView::PrintHelp
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYCmdlineView::PrintHelp()
    {
    }

// ---------------------------------------------------------
// CPosPSYCmdlineView::ReDraw
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYCmdlineView::ReDraw()
    {
    _LIT(KRow1, "Running tests\n"); 
	_LIT(KRow2,	"according to\n");
	_LIT(KRow3,	"command line\n");
	_LIT(KRow4,	"arguments.\n");
	_LIT(KRow5,	"\n");
	_LIT(KRow6,	"Please wait...\n");

    iConsole->ClearScreen();
    TInt width = iConsole->ScreenSize().iWidth;
    TInt xPos = (width - KRow4().Length()) / 2;
    TInt yPos = KDefaultYPos;

    iConsole->SetPos(xPos ,yPos++);
    iConsole->Printf(KRow1);
    iConsole->SetPos(xPos, yPos++);
    iConsole->Printf(KRow2);
    iConsole->SetPos(xPos, yPos++);
    iConsole->Printf(KRow3);
	iConsole->SetPos(xPos, yPos++);
    iConsole->Printf(KRow4);
    iConsole->SetPos(xPos, yPos++);
    iConsole->Printf(KRow5);
    iConsole->SetPos(xPos, yPos++);
    iConsole->Printf(KRow6);
    }

//  End of File

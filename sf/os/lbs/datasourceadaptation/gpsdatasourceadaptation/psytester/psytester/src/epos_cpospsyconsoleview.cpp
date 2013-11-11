// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// epos_cpospsycmdlineview.cpp
// Class that handles command line for psy tester
// 
//

// INCLUDE FILES
#include "epos_cpospsyconsoleview.h"
#include <e32cons.h>

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CPosPSYConsoleView::CPosPSYConsoleView(
    CConsoleBase* aConsole)
	: iConsole(aConsole)
    {
    }

// Destructor
CPosPSYConsoleView::~CPosPSYConsoleView()
    {
    }

// ---------------------------------------------------------
// CPosPSYConsoleView::ActivateL
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYConsoleView::ActivateL()
    {
    ReDraw();
    }

// ---------------------------------------------------------
// CPosPSYConsoleView::DeActivate
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYConsoleView::DeActivate()
    {
    }

// ---------------------------------------------------------
// CPosPSYConsoleView::SetViewManager
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYConsoleView::SetViewManager(
	MPosPSYViewManager* aViewManager)
    {
    iViewManager = aViewManager;
    }

// ---------------------------------------------------------
// CPosPSYConsoleView::IsHelpRequested
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool CPosPSYConsoleView::IsHelpRequested(
	TKeyCode aKey)
	{
	if (aKey == EKeyDevice0 || //Left softkey
		aKey == 'H' || 
		aKey == 'h')
		{
		return ETrue;
		}
	else
		{
		return EFalse;
		}
	}

// ---------------------------------------------------------
// CPosPSYConsoleView::IsNumeric
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool CPosPSYConsoleView::IsNumeric(
	TKeyCode aKey)
	{
	if ((aKey >= '0') && (aKey <= '9'))
		{
		return ETrue;
		}
	else
		{
		return EFalse;
		}
	}

//  End of File

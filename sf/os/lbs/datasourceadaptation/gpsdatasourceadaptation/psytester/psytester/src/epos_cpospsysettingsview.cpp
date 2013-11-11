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
#include <e32math.h>
#include "epos_cpospsysettingsview.h"
#include "epos_mpospsyviewmanager.h"

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CPosPSYSettingsView::CPosPSYSettingsView(
    CConsoleBase* aConsole) 
: CPosPSYConsoleView(aConsole)
    {
    TSize size = iConsole->ScreenSize();
    iMaxWidth = size.iWidth - 2;
    }

// EPOC default constructor can leave.
void CPosPSYSettingsView::ConstructL(CPosPSYTestHandler* aPSYTestHandler)
    {
    iTestHandler = aPSYTestHandler;
	}

// Two-phased constructor.
CPosPSYSettingsView* CPosPSYSettingsView::NewL(
    CConsoleBase* aConsole,
	CPosPSYTestHandler* aPSYTestHandler)
    {
    CPosPSYSettingsView* self = new (ELeave) CPosPSYSettingsView(aConsole);
    CleanupStack::PushL(self);
    self->ConstructL(aPSYTestHandler);
    CleanupStack::Pop(self);
    return self;
    }
    
// Destructor
CPosPSYSettingsView::~CPosPSYSettingsView()
    {
	iReadBuf.Reset();
    iReadBuf.Close();
    }

// ---------------------------------------------------------
// CPosPSYSettingsView::ActivateL
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYSettingsView::ActivateL()
    {
    ReDraw();
    }

// ---------------------------------------------------------
// CPosPSYSettingsView::PrintHelp
//
// (other items were commented in a header).
// ---------------------------------------------------------
// 
void CPosPSYSettingsView::PrintHelp()
    {
    _LIT(KText1, "Select number of\n");
    _LIT(KText2, "location requests\n");
	_LIT(KText3, "for memory tests = 0\n");
    _LIT(KText4, "to skip memory tests\n");
    _LIT(KText5, "Default value = 1000\n");
	
	iConsole->ClearScreen();
    TInt width = iConsole->ScreenSize().iWidth;
    iXPos = (width - KText3().Length()) / 2 - 1;
    iYPos = KDefaultYPos - 1;
    iConsole->SetPos(iXPos ,iYPos++);
	iConsole->Printf(KText1);
    iConsole->SetPos(iXPos ,iYPos++);
	iConsole->Printf(KText2);
    iConsole->SetPos(iXPos ,iYPos++);
	iConsole->Printf(KText3);
    iConsole->SetPos(iXPos ,iYPos++);
	iConsole->Printf(KText4);
	iConsole->SetPos(iXPos ,iYPos++);
	iConsole->Printf(KText5);
    }

// ---------------------------------------------------------
// CPosPSYSettingsView::HandleKeyEventL
//
// (other items were commented in a header).
// ---------------------------------------------------------
// 
void CPosPSYSettingsView::HandleKeyEventL(
    TKeyCode aKey)
    {
    if (aKey == EKeyLeftArrow)
        {
        iViewManager->SwitchViewL(KPreviousView);
        }
    else if (IsHelpRequested(aKey))
        {
        PrintHelp();
        }
    else if (aKey == EKeySpace)
        {
        CActiveScheduler::Stop();
        }

	if (iReadBuf.Count() < KMemoryMaxBufSize)
        {
        if (IsNumeric(aKey))
   		    {
			if (iReadBuf.Count() == 0) 
				{
				_LIT(KMsg, " : ");
				iConsole->SetPos(iXPos ,iYPos);
				iConsole->Printf(KMsg);
				}
			_LIT(KKey, "%c");
			TBuf<10> buf;
			buf.Format(KKey, aKey);
			iConsole->Printf(buf);
            iReadBuf.Append((static_cast<TInt> (aKey)) - '0');
   		    }
        
		else if (aKey == EKeyEnter ||	// enter
				 aKey == EKeyDevice3)	// stick down (S60)			
            {
			TInt number = ParseBufferL();
			iReadBuf.Reset();

			TPosPSYTestParameters testParameters;
			iTestHandler->GetTestParameters(testParameters);
			testParameters.SetNumberOfMemoryRequests(number);
			iTestHandler->SetTestParameters(testParameters);
			ReDraw();
            }
		else if (aKey == 'U' || aKey == 'u')
            {
            iReadBuf.Reset();
            ReDraw();
			}
        else
            {
            //Ignore character
            }    
        }
    else
        {
        iReadBuf.Reset();
		ReDraw();
        }
    }

// ---------------------------------------------------------
// CPosPSYSettingsView::HandleTestEventL
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//	
void CPosPSYSettingsView::HandleTestEvent(TInt /*aEvent*/)
	{
	}

// ---------------------------------------------------------
// CPosPSYSettingsView::ReDraw
//
// (other items were commented in a header).
// ---------------------------------------------------------
// 
void CPosPSYSettingsView::ReDraw()
    {
    _LIT(KMemoryTests1,"Enter number of");
	_LIT(KMemoryTests2,"location requests");
	_LIT(KMemoryTests3,"for memory tests");
	_LIT(KMemoryTests4,"and press joystick");
	_LIT(KMemoryTests5, "Current value:");
	
	iConsole->ClearScreen();
	iReadBuf.Reset();
	
	TPosPSYTestParameters testParameters;
	iTestHandler->GetTestParameters(testParameters);
	TBuf<KMemoryMaxBufSize> nrBuf;
	nrBuf.AppendNum(testParameters.NumberOfMemoryRequests());
	
	TInt width = iConsole->ScreenSize().iWidth;
    iXPos = (width - KMemoryTests1().Length()) / 2 - 2;
    iYPos = KDefaultYPos;

    iConsole->SetPos(iXPos ,iYPos++);
    iConsole->Printf(KMemoryTests1);
	iConsole->SetPos(iXPos, iYPos++);
    iConsole->Printf(KMemoryTests2);
    iConsole->SetPos(iXPos, iYPos++);
    iConsole->Printf(KMemoryTests3);
	iConsole->SetPos(iXPos, iYPos++);
    iConsole->Printf(KMemoryTests4);
	iConsole->SetPos(iXPos, iYPos++);
    iConsole->Printf(KMemoryTests5);
	iConsole->SetPos(iXPos, iYPos++);
    iConsole->Printf(nrBuf);
    }


// ---------------------------------------------------------
// CPosPSYSettingsView::ParseBufferL
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CPosPSYSettingsView::ParseBufferL()
    {
    TInt number = 0;
    for (TInt i=0; i<iReadBuf.Count(); i++)
        {
        TReal a;
        User::LeaveIfError(Math::Pow(a,10.0, static_cast<TReal> (iReadBuf.Count()-(i+1))) );
        number += static_cast<TInt> (a) * iReadBuf[i];
        }
    return number;
    }

//  End of File  

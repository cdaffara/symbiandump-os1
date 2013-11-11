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
#include "epos_cpospsylistview.h"
#include "epos_mpospsyviewmanager.h"

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CPosPSYListView::CPosPSYListView(
    CConsoleBase* aConsole)
	: CPosPSYConsoleView(aConsole)
    {
    }

// EPOC default constructor can leave.
void CPosPSYListView::ConstructL(CPosPSYTestHandler* aPSYTestHandler)
    {
    iTestHandler = aPSYTestHandler;
	}

// Two-phased constructor.
CPosPSYListView* CPosPSYListView::NewL(
    CConsoleBase* aConsole,
	CPosPSYTestHandler* aPSYTestHandler)
    {
    CPosPSYListView* self = new (ELeave) CPosPSYListView(
		aConsole);
    CleanupStack::PushL(self);
    self->ConstructL(aPSYTestHandler);
    CleanupStack::Pop(self);
    return self;
    }

// Destructor
CPosPSYListView::~CPosPSYListView()
    {
    iReadBuf.Reset();
    iReadBuf.Close();
	}

// ---------------------------------------------------------
// CPosPSYListView::ActivateL
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYListView::ActivateL()
    {
    ReDraw();
    }

// ---------------------------------------------------------
// CPosPSYListView::HandleKeyEventL
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYListView::HandleKeyEventL(
    TKeyCode aKey)
    {
    _LIT(KAlreadyRunning, "\r\nAlready running.");
	_LIT(KNoTestsRunning, "\r\nNo test running.");

	if (aKey == EKeyRightArrow)
        {
        iViewManager->SwitchViewL(KPSYSettingsView);
        }
	else if (IsHelpRequested(aKey))
        {
        PrintHelp();
        }

    if (iReadBuf.Count() < KMaxBufSize)
        {
        if (IsNumeric(aKey))
   		    {
			if (iReadBuf.Count() == 0) 
				{
				_LIT(KMsg, "\r\nPSY: ");
				iConsole->Printf(KMsg);
				}
			_LIT(KKey, "%c");
			TBuf<10> buf;
			buf.Format(KKey, aKey);
			iConsole->Printf(buf);
            iReadBuf.Append((static_cast<TInt> (aKey)) - '0');
   		    }
        else if ((aKey == 'R') || (aKey == 'r'))
            {
			if (iTestHandler->TestsRunning())
				{
				iConsole->Printf(KAlreadyRunning);				
				}
			else
				{
				iTestHandler->StartPSYTesterL(KTestAllPSYs);
				}
			}
		else if ((aKey == 'C') || (aKey == 'c'))
   		    {
			if (iTestHandler->TestsRunning())
				{
				iTestHandler->Cancel();
				_LIT(KMsg, "\r\nTests canceled.");
				iConsole->Printf(KMsg);
				}
			else 
				{
				iConsole->Printf(KNoTestsRunning);				
				}
			}
        else if (aKey == 'L' ||			// "L"
				 aKey == 'l')			// "l"
            {
            iCurrentIndex = 0;
            iTestHandler->UpdatePSYListL();
            iConsole->ClearScreen();
            ListPSYs(KListStart);
            }
        else if (aKey == 'X' ||	
				 aKey == 'x' ||
				 aKey == EKeyDevice1)		// right softkey (S60)
            {
            CActiveScheduler::Stop();
			}
        else if (aKey == 'F' ||       // "F"
				 aKey == 'f' ||		  // "f"
				 aKey == EKeyUpArrow) // joy-stick up (S60)
            {
            iConsole->ClearScreen();
            ListPSYs(KListPrevious);
			}
        else if (aKey == 'N' ||			// "N"
				 aKey == 'n' ||			// "n"
				 aKey == EKeyDownArrow) // joy-stick down (S60)
            {
            iConsole->ClearScreen();
            ListPSYs(KListNext);
			}
        else if (aKey == 'U' ||			// "U"
				 aKey == 'u')			// "u"
            {
            iReadBuf.Reset();
            iCurrentIndex = 0;
            iTestHandler->UpdatePSYListL();
            iConsole->ClearScreen();
            ListPSYs(KListStart);
			}
		else if (aKey == EKeyEnter || // enter
				 aKey == EKeyDevice3) // stick down (S60)
            {
			if (iTestHandler->TestsRunning())
				{
				iConsole->Printf(KAlreadyRunning);
				}
			else 
				{
				TInt number = ParseBufferL();
				iReadBuf.Reset();

				if (number > 0) //Because readbuffer can't be flushed
					{
					iTestHandler->StartPSYTesterL(number);
					}
				}
            }
        else
            {
            //Ignore character
            }    
        }
    else
        {
        iReadBuf.Reset();
        }
    }

// ---------------------------------------------------------
// CPosPSYListView::HandleTestEvent
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//	
void CPosPSYListView::HandleTestEvent(TInt aEvent)
	{
	if (aEvent == ERunAll)
		{
		_LIT(KRunAll, "\r\nTesting all PSYs.");
		iConsole->Printf(KRunAll);
		}
	else if (aEvent == ETestsRunning)
		{
		_LIT(KRun, "\r\nTests running.");	
		iConsole->Printf(KRun);
		}
	else if (aEvent == EIndexError)
		{
		_LIT(KMessage, "\r\nChoose number between 1 and %d");
		TBuf<KMessageBufSize> buf;
		buf.Format(KMessage, iTestHandler->InfoList().Count());
        iConsole->Printf(buf);	
		}
	else if (aEvent == ETestsCompletedCorrect)
		{
		_LIT(KEnd,"\r\nTests done.");
		iConsole->Printf(KEnd);
		
		}
	else if (aEvent == ETestsCompletedWithError)
		{
		_LIT(KError,"\r\nTests completed with errors!");
        iConsole->Printf(KError);
		}

	}

// ---------------------------------------------------------
// CPosPSYListView::PrintHelp
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYListView::PrintHelp()
    {
	_LIT(KNumber, "1-N: Select PSY\n");
    _LIT(KRunAll, "R: Test all PSYs\n");
    _LIT(KHelp, "H: Help list\n");
    _LIT(KCancel, "C: Cancel tests\n");
    _LIT(KUndo, "U: Undo selection\n");
	_LIT(KList, "L: List PSYs\n");
    _LIT(KPrev, "Joystick up:\n");
	_LIT(KPrev2," List previous\n");
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
    iConsole->Printf(KPrev);
	iConsole->Printf(KPrev2);
    iConsole->Printf(KNext);
	iConsole->Printf(KNext2);
	iConsole->Printf(KForward);
	iConsole->Printf(KForward2);
	iConsole->Printf(KBack);
	iConsole->Printf(KBack2);
    iConsole->Printf(KExit);
    }

// ---------------------------------------------------------
// CPosPSYListView::ReDraw
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYListView::ReDraw()
    {
    iConsole->ClearScreen();
	ListPSYs(KListCurrent);
    }

// ---------------------------------------------------------
// CPosPSYListView::ListPSYs
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYListView::ListPSYs(TInt aListDir)
    {
	TInt psyPerView = 4;
    TInt lowerIndex;

	if (aListDir == KListNext)
		{
		lowerIndex = iCurrentIndex;
		// End of list
		if (iCurrentIndex >= iTestHandler->InfoList().Count())
			{
			iCurrentIndex = 0;
			lowerIndex = 0;
			}
		// Start/Middle of list
		if ((iCurrentIndex + psyPerView) < iTestHandler->InfoList().Count())
			{
			iCurrentIndex += psyPerView;
			}
		// Near end of list
		else
			{
			iCurrentIndex = iTestHandler->InfoList().Count();
			}
		}
	else if (aListDir == KListPrevious)
		{
		// End of list
		if (iCurrentIndex >= iTestHandler->InfoList().Count())
			{
			lowerIndex = 0;
			if (iCurrentIndex > psyPerView)
				{
				TInt rest = iCurrentIndex % psyPerView;
				if (rest == 0)
					{
					rest = psyPerView;
					}
				iCurrentIndex = iCurrentIndex - rest;
				lowerIndex = iCurrentIndex - psyPerView;
				}
			}
		// Start of list
		else if (iCurrentIndex - psyPerView <= 0)
			{
			iCurrentIndex = iTestHandler->InfoList().Count();
			TInt rest = iCurrentIndex % psyPerView;
			if (rest == 0)
				{
				rest = psyPerView;
				}
			lowerIndex = iCurrentIndex - rest;
			}
		// Middle of list
		else
			{
			lowerIndex = iCurrentIndex - psyPerView * 2; 
			iCurrentIndex -= psyPerView; 
			}
		}
	else if (aListDir == KListCurrent)
		{
		lowerIndex = iCurrentIndex;
		// End of list
		if (iCurrentIndex >= iTestHandler->InfoList().Count())
			{
			TInt rest = iCurrentIndex % psyPerView;
			if (rest == 0 && iTestHandler->InfoList().Count() != 0)
				{
				rest = psyPerView;
				}
			lowerIndex = iCurrentIndex - rest;
			}
		// Start/Middle of list
		else if (iCurrentIndex >= psyPerView)
			{
			lowerIndex = iCurrentIndex - psyPerView;
			}
		// Start of application and start of list
		else
			{
			if (iTestHandler->InfoList().Count() >= psyPerView)
				{
				iCurrentIndex = psyPerView;
				}
			else 
				{
				iCurrentIndex = iTestHandler->InfoList().Count();
				}
			}
		}
	else if (aListDir == KListStart)
		{
		lowerIndex = 0;
		iCurrentIndex = psyPerView;
		}
	else 
		{
		return;
		}

    for (TInt i = lowerIndex; i < iCurrentIndex; i++)
        {
        TPosPSYBaseInfo info = iTestHandler->InfoList()[i];
        _LIT(KNr, "%d.");
        iConsole->Printf(KNr, i+1);

		TSize size = iConsole->ScreenSize();

        TInt nameLength = info.Name().Length();
        TPtrC name(info.Name().Ptr(), Min(nameLength, (size.iWidth - 6)));
        
        iConsole->Printf(name);
        _LIT(KVersionInfo, "\n  UID: [%x] \n  Vers: %d\n");
        iConsole->Printf(KVersionInfo, info.Uid().iUid, info.Version());
        }
	
	if (lowerIndex == iCurrentIndex)
		{
		PrintNoFound();
		}
	
	iReadBuf.Reset();
    }

// ---------------------------------------------------------
// CPosPSYListView::PrintNoFound
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYListView::PrintNoFound()
	{
	_LIT(KNoFound, "No PSYs Installed.");
	TInt width = iConsole->ScreenSize().iWidth;
    TInt xPos = (width - KNoFound().Length()) / 2;
    TInt yPos = KDefaultYPos;
	
	iConsole->ClearScreen();
    iConsole->SetPos(xPos ,yPos++);
	iConsole->Printf(KNoFound);
	}

// ---------------------------------------------------------
// CPosPSYListView::ParseBufferL
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CPosPSYListView::ParseBufferL()
    {
    TInt number = 0;
    for (TInt i=0; i<iReadBuf.Count(); i++)
        {
        TReal a;
        User::LeaveIfError(Math::Pow(a,10.0, static_cast<TReal> (iReadBuf.Count()-(i+1))));
        number += static_cast<TInt> (a) * iReadBuf[i];
        }
    return number;
    }

//  End of File

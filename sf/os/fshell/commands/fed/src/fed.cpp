// fed.cpp
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

#include <e32cons.h>
#include <BADESCA.H>
#include <charconv.h>

#include "fed.h"
#include "viewbase.h"
#include "bufferbase.h"
#include "screenmngr.h"
#include "cmdwindow.h"

#include "filebuffer.h"
#include "lrtextview.h"

const TInt KDefScreenPosX = 0;
const TInt KDefScreenPosY = 0;

CFed* CFed::NewL(CColorConsoleBase& aConsole, RFs& aFs)
	{
	CFed* fed = new (ELeave) CFed(aConsole, aFs);
	CleanupStack::PushL(fed);
	fed->ConstructL();
	CleanupStack::Pop(fed);
	return fed;
	}

CFed* CFed::NewL(CConsoleBase& aConsole, RFs& aFs)
	{
	CFed* fed = new (ELeave) CFed(aConsole, aFs);
	CleanupStack::PushL(fed);
	fed->ConstructL();
	CleanupStack::Pop(fed);
	return fed;
	}

CFed::CFed(CColorConsoleBase& aConsole, RFs& aFs)
	: CActive(CActive::EPriorityStandard), iFs(aFs), iConsole(aConsole), iColorConsole(&aConsole), iViewIdx(-1)
	{
	CActiveScheduler::Add(this);
	}

CFed::CFed(CConsoleBase& aConsole, RFs& aFs)
	: CActive(CActive::EPriorityStandard), iFs(aFs), iConsole(aConsole), iViewIdx(-1)
	{
	CActiveScheduler::Add(this);
	}

CFed::~CFed()
	{
	Cancel();
	for (TInt i = iViewArray.Count()-1; i >= 0; i--)
		delete iViewArray[i];
	iViewArray.Reset();
	delete iCmdWindow;
	delete iCharconv;
	}

void CFed::ConstructL()
	{
	iFs.CreatePrivatePath(EDriveC);
	TSize size = iConsole.ScreenSize();
	iScreenMngr.ResizeScreenL(TWindow(KDefScreenPosX, KDefScreenPosY, size.iWidth, size.iHeight));
	if (iColorConsole)
		{
		iCmdWindow = CCommandWindow::NewL(iFs, *iColorConsole);
		}
	else
		{
		iCmdWindow = CCommandWindow::NewL(iFs, iConsole);
		}
	iCmdWindow->SetWindow(iScreenMngr.GetCommandWindow());
	
	TRAPD(err, iCharconv = CCnvCharacterSetConverter::NewL());
	if (!err)
		{
		// This can fail if charconv isn't properly configured in the ROM
		ASSERT(iCharconv->PrepareToConvertToOrFromL(KCharacterSetIdentifierUtf8, iFs) == CCnvCharacterSetConverter::EAvailable);
		}
	}

void CFed::StartL(CDesC16Array* aFiles)
	{
	CleanupStack::PushL(aFiles);
	CreateViewsL(aFiles);
	CleanupStack::PopAndDestroy(aFiles);

	iConsole.Read(iStatus);
	SetActive();
	}

//CActive
void CFed::DoCancel()
	{
	iConsole.ReadCancel();
	}

void CFed::RunL()
	{
	if(iStatus.Int() < 0)
		{
		CActiveScheduler::Stop();
		return;
		}

	TKeyCode code = iConsole.KeyCode();
	TCmdType cmd = ProcessKeyL(code);

	if(!ExecuteCommandL(cmd) && iViewIdx >= 0)
		iViewArray[iViewIdx]->ConsumeKeyL(code);
	if(iViewArray.Count() == 0)
		{
		// No more views, time to exit.
		// Set the cursor to the bottom of the screen before we do - easier for the user to see that we've exited
		TSize screenSize = iConsole.ScreenSize();
		iConsole.SetPos(screenSize.iWidth-1, screenSize.iHeight-1); // Bottom right corner
		iConsole.Write(_L(" ")); // This should cause the window to scroll and the cursor to be on the bottom line
		CActiveScheduler::Stop();
		return;
		}

	iConsole.Read(iStatus);
	SetActive();
	}

TInt CFed::RunError(TInt aError)
	{
	if (iCmdWindow)
		{
		iCmdWindow->InfoPrint(_L("Leave occurred: %d"), aError);

		// And rerequest further keys
		iConsole.Read(iStatus);
		SetActive();

		return KErrNone;
		}
	return aError;
	}

void CFed::CreateViewsL(CDesC16Array* aFiles)
	{
	TInt n = aFiles->Count();
	for (TInt i = 0; i < n; i++)
		{
		NewViewL((*aFiles)[i], ETrue);
		}

	if (n == 0)
		{
		// Create a new untitled view instead
		NewViewL(KNullDesC);
		}
	if (iCharconv)
		{
		iCmdWindow->InfoPrint(_L("Welcome to fed. Press Ctrl-L for help."));
		}
	else
		{
		iCmdWindow->InfoPrint(_L("Warning: couldn't load charconv. UTF-8 not available."));
		}
	}

CFed::TCmdType CFed::ProcessKeyL(const TKeyCode& aCode)
	{
	switch(aCode)
		{
	case CTRL('q'):
		return ECmdCloseAll;
	case EKeyF1:
	case CTRL('l'):
		return ECmdHelp;
	case EKeyF2:
	case CTRL('p'):
		return ECmdPrevView;
	case EKeyF3:
	case CTRL('}'):
		return ECmdNextView;
	case EKeyEscape:
	case CTRL('w'):
		return ECmdCloseCurrent;
	case CTRL('n'):
		return ECmdNewDocument;
	case CTRL('o'):
		return ECmdOpenDocument;
	case CTRL('r'):
	case EKeyF5:
		return ECmdRefresh;
	default:
		break;
		}
	return ECmdNone;
	}

TBool CFed::ExecuteCommandL(TCmdType aCmd)
{
	switch(aCmd)
		{
		case ECmdHelp:
			::ShowHelpL();
			break;
		case ECmdPrevView:
			iViewIdx--;
			if (iViewIdx < 0) iViewIdx = iViewArray.Count()-1;
			iScreenMngr.AttachViewL(*iViewArray[iViewIdx]);
			break;
		case ECmdNextView:
			iViewIdx++;
			if (iViewIdx >= iViewArray.Count()) iViewIdx = 0;
			iScreenMngr.AttachViewL(*iViewArray[iViewIdx]);
			break;
		case ECmdCloseCurrent:
			CloseCurrentViewL();
			break;
		case ECmdCloseAll:
			CloseAllViewsL();
			break;
		case ECmdNewDocument:
			NewViewL(KNullDesC);
			break;
		case ECmdOpenDocument:
			OpenDocumentL();
			break;
		case ECmdRefresh:
			iScreenMngr.RefreshScreenL();
			break;
		default:
			return EFalse;
		}
	return ETrue;
}

TBool CFed::CloseCurrentViewL()
{
	if(iViewIdx < 0)
		{
		//Show a message to the user that there is nothing to close
		return EFalse;
		}

	CFedViewBase* view = iViewArray[iViewIdx];
	TBool okToClose = ETrue; 
	if (view->Buffer().RefCount() == 1)
		{
		// We're the only view on this buffer - better ask about saving
		okToClose = view->TryCloseL();
		}

	if (okToClose)
		{
		DeleteView();
		}
	return okToClose;
	}

void CFed::CloseAllViewsL()
{
	TBool ret = ETrue;
	while(iViewIdx >=0 && ret)
		ret = CloseCurrentViewL();
}

void CFed::DeleteView()
	{
	delete iViewArray[iViewIdx];
	iViewArray.Remove(iViewIdx);
	iScreenMngr.UnsetCurrentView();
	if (iViewIdx >= iViewArray.Count())
		{
		iViewIdx = iViewArray.Count()-1;
		}
	if (iViewIdx >= 0)
		{
		// Attach the new frontmost view
		iScreenMngr.AttachViewL(*(iViewArray[iViewIdx]));
		}
	}

void CFed::NewViewL(const TDesC& aFileName, TBool aPromptOnNonexistantName)
	{
	CFileBuffer* buffer = NULL;
	TRAPD(err, buffer = CFileBuffer::NewL(iFs, iCharconv, aFileName, EFalse));
	if ((err == KErrNotFound || err == KErrPathNotFound) && aPromptOnNonexistantName)
		{
		if (iCmdWindow->Query(_L("File not found. Create? (yes/no) "), _L("yn")) == 'y')
			{
			buffer = CFileBuffer::NewL(iFs, iCharconv, aFileName, ETrue);
			err = KErrNone;
			}
		}
	User::LeaveIfError(err);

	buffer->PushL();
	CLRTextView* view = CLRTextView::NewL(*iCmdWindow, *buffer);
	CleanupStack::Pop(buffer);
	buffer->DecRef(); // view now has a ref to it
	CleanupStack::PushL(view);
	iViewArray.AppendL(view);
	CleanupStack::Pop(view);
	iViewIdx = iViewArray.Count()-1;
	iScreenMngr.AddViewL(*view);
	}

void CFed::OpenDocumentL()
	{
	TFileName name;
	TBool go = iCmdWindow->QueryFilename(_L("Open file: "), name);
	if (go)
		{
		TInt prevIdx = iViewIdx;
		TRAPD(err, NewViewL(name));
		if (err)
			{
			switch (err)
				{
				case KErrNotFound:
					iCmdWindow->InfoPrint(_L("File '%S' not found"), &name);
					break;
				case KErrPathNotFound:
					iCmdWindow->InfoPrint(_L("KErrPathNotFound. Directory doesn't exist?"));
					break;
				case KErrBadName:
					iCmdWindow->InfoPrint(_L("Bad name"));
					break;
				default:
					iCmdWindow->InfoPrint(_L("Unable to open file: %d."), err);
					break;
				}
			}
		else if (!err && prevIdx >= 0)
			{
			CFedViewBase* prevView = iViewArray[prevIdx];
			if (prevView->Buffer().Editable() && !prevView->Buffer().Modified() && prevView->Buffer().Title().Length() == 0)
				{
				// Then previous doc was an empty, untitled window, which this open should replace
				delete prevView;
				iViewArray.Remove(prevIdx);
				iViewIdx = iViewArray.Count()-1;
				}
			}
		}
	}

// Not usable: ^H = Bksp, ^M = CR, ^I = Tab, ^J = LF, ^Z = suspend, ^[ = Esc
// Currently free: ^E, ^Y
void CFed::ShowHelpL(HBufC* aHelpText)
	{
	_LIT(KHelpTitle, "Fed Help");
	CleanupStack::PushL(aHelpText);
	CConstDataBuffer* helpBuf = new(ELeave) CConstDataBuffer(KHelpTitle, aHelpText);
	CleanupStack::Pop(aHelpText);
	helpBuf->PushL();
	CLRTextView* view = CLRTextView::NewL(*iCmdWindow, *helpBuf);
	CleanupStack::Pop(helpBuf);
	helpBuf->DecRef(); // view now has a ref to it
	CleanupStack::PushL(view);
	iViewArray.AppendL(view);
	CleanupStack::Pop(view);
	iViewIdx = iViewArray.Count()-1;
	iScreenMngr.AddViewL(*view);
	}

void CFed::RedrawEverythingL()
	{
	ExecuteCommandL(ECmdRefresh);
	}

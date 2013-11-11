// ConsoleWindow.cpp
// 
// Copyright (c) 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//
#include "Misc.h"
#include <windowsx.h>
#include "stdafx.h"
#include "ConsoleWindow.h"
#include "TextBuffer.h"
#include "TextView.h"
#include "resource.h"

const int KMaxFormatBuf = 256;


CConsoleWindow* CConsoleWindow::New(HINSTANCE aAppHandle, LPCTSTR aWindowClass, LPCTSTR aTitle, int aPosX, int aPosY, int aWidth, int aHeight, int aNumOverflowLines, MWindowObserver* aWindowObserver, MConsoleCharHandler* aCharHandler, bool aNoSelection)
	{
	std::auto_ptr<CConsoleWindow> self(new(EThrow) CConsoleWindow(aNoSelection));
	self->Construct(aAppHandle, aWindowClass, aTitle, aPosX, aPosY, aWidth, aHeight, aNumOverflowLines, aWindowObserver, aCharHandler);
	return self.release();
	}

CConsoleWindow::~CConsoleWindow()
	{
	delete iView;
	delete iBuffer;
	}

void CConsoleWindow::Write(LPCTSTR aString)
	{
	Write(aString, wcslen(aString));
	}

void CConsoleWindow::Write(LPCTSTR aString, int aLength)
	{
	iView->BeginUpdate();
	iBuffer->Write(aString, aLength);
	iView->EndUpdate();
	}

void CConsoleWindow::WriteFormat(LPCTSTR aFormat, ...)
	{
	va_list args;
	va_start(args, aFormat);
	TCHAR buf[KMaxFormatBuf];
	int length = _vsnwprintf(buf, KMaxFormatBuf, aFormat, args);
	Write(buf, length);
	}

void CConsoleWindow::GetCursorPos(int& aX, int& aY)
	{
	iBuffer->GetCursorPos(aX, aY);
	}

void CConsoleWindow::SetAbsCursorPos(int aX, int aY)
	{
	iView->BeginUpdate();
	iBuffer->SetAbsCursorPos(aX, aY);
	iView->EndUpdate();
	}

void CConsoleWindow::SetRelCursorPos(int aX, int aY)
	{
	iView->BeginUpdate();
	iBuffer->SetRelCursorPos(aX, aY);
	iView->EndUpdate();
	}

void CConsoleWindow::SetCursorHeight(int aHeight)
	{
	(aHeight > 0) ? iView->EnableCursor() : iView->DisableCursor();
	}

void CConsoleWindow::GetConsoleSize(int& aWidth, int& aHeight)
	{
	iBuffer->GetSize(aWidth, aHeight);
	}

void CConsoleWindow::ClearScreen()
	{
	iView->BeginUpdate();
	iBuffer->Clear();
	iView->EndUpdate();
	}

void CConsoleWindow::ClearToEndOfLine()
	{
	iView->BeginUpdate();
	iBuffer->ClearToEndOfLine();
	iView->EndUpdate();
	}

void CConsoleWindow::CaptureToFile(LPCTSTR aFileName)
	{
	iBuffer->CaptureToFile(aFileName);
	}

void CConsoleWindow::StopCaptureToFile()
	{
	iBuffer->StopCaptureToFile();
	}

bool CConsoleWindow::IsCapturingToFile() const
	{
	return iBuffer->IsCapturingToFile();
	}

void CConsoleWindow::PasteFromClipboard()
	{
	if (iCharHandler && OpenClipboard(Handle())) 
		{
		HGLOBAL hglb = GetClipboardData(CF_TEXT);
		if (hglb)
			{
			char* narrowBuf = (char*)GlobalLock(hglb);
			if (narrowBuf)
				{
				LPTSTR wideBuf;
				int length = strlen(narrowBuf);
				ConvertNarrowToWideAlloc(narrowBuf, length, wideBuf);
				if (wideBuf)
					{
					iCharHandler->HandleConsoleString(wideBuf, length);
					delete wideBuf;
					}
				GlobalUnlock(hglb);
				}
			}
		CloseClipboard(); 
		}
	}

void CConsoleWindow::CopyToClipboard() const
	{
    if (OpenClipboard(Handle())) 
		{
	    EmptyClipboard();
		LPTSTR wideSelection = iView->Selection();
		if (wideSelection)
			{
			int length = wcslen(wideSelection);
			HGLOBAL hglbCopy = GlobalAlloc(GMEM_MOVEABLE, (length + 1) * sizeof(TCHAR));
			if (hglbCopy)
				{
				char* narrowBuf = (char*)GlobalLock(hglbCopy);
				ConvertWideToNarrow(wideSelection, length, narrowBuf);
				GlobalUnlock(hglbCopy);
				SetClipboardData(CF_TEXT, hglbCopy);
				iView->ClearSelection();
				}
			delete wideSelection;
			}
		CloseClipboard();
		}
	}

void CConsoleWindow::SetDimmed(bool aDimmed)
	{
	iView->SetDimmed(aDimmed);
	}

CConsoleWindow::CConsoleWindow(bool aNoSelection) : iCharHandler(NULL), iBuffer(NULL), iView(NULL), iNoSelection(aNoSelection)
	{
	}

void CConsoleWindow::Construct(HINSTANCE aAppHandle, LPCTSTR aWindowClass, LPCTSTR aTitle, int aPosX, int aPosY, int aCharWidth, int aCharHeight, int aNumOverflowLines, MWindowObserver* aWindowObserver, MConsoleCharHandler* aCharHandler)
	{
	iCharHandler = aCharHandler;

	int numHorizChars = (aCharWidth == -1) ? 80 : aCharWidth;
	int numVertChars = (aCharHeight == -1) ? 40 : aCharHeight;

	iBuffer = CTextBuffer::New(numHorizChars, numVertChars, aNumOverflowLines);
	iView = CTextView::New(*this, *iBuffer, aCharWidth, aCharHeight);
	if (!iCharHandler)
		{
		iView->DisableCursor();
		}

	int pixelWidth;
	int pixelHeight;
	iView->GetSize(pixelWidth, pixelHeight);

	CWindow::Construct(aAppHandle, aWindowClass, aTitle, aPosX, aPosY, pixelWidth, pixelHeight, aWindowObserver);
	}

void CConsoleWindow::Draw() const
	{
	iView->Draw();
	}

LRESULT CConsoleWindow::HandleChar(TCHAR aChar, UINT aModifiers)
	{
	if (iCharHandler)
		{
		iView->ScrollToEndIfNeeded();
		iCharHandler->HandleConsoleChar(aChar, aModifiers);
		}
	return 0;
	}

LRESULT CConsoleWindow::HandleFocusChange(bool aFocused)
	{
	aFocused ? iView->HandleFocusGained() : iView->HandleFocusLost();
	return 0;
	}

void CConsoleWindow::HandleScrollEvent(UINT aMessage, WPARAM aWParam)
	{
	int scrollDir = (aMessage == WM_HSCROLL) ? SB_HORZ : SB_VERT;
	SCROLLINFO scrollInfo;
	scrollInfo.cbSize = sizeof(scrollInfo);
	scrollInfo.fMask = SIF_ALL;
	GetScrollInfo(Handle(), scrollDir, &scrollInfo);
	int pos = scrollInfo.nPos;
	switch (LOWORD(aWParam))
		{
		case SB_LINEUP:		// Also SB_LINELEFT.
			scrollInfo.nPos -= (aMessage == WM_HSCROLL) ? iView->CharWidth() : iView->CharHeight();
			break;
		case SB_LINEDOWN:	// Also SB_LINERIGHT.
			scrollInfo.nPos += (aMessage == WM_HSCROLL) ? iView->CharWidth() : iView->CharHeight();
			break;
		case SB_PAGEUP:		// Also SB_PAGELEFT.
			scrollInfo.nPos -= scrollInfo.nPage;
			break;
		case SB_PAGEDOWN:	// Also SB_PAGERIGHT.
			scrollInfo.nPos += scrollInfo.nPage;
			break;
		case SB_THUMBTRACK: 
			scrollInfo.nPos = scrollInfo.nTrackPos;
			break;
		default:
			break;
		}
	scrollInfo.fMask = SIF_POS;
	SetScrollInfo (Handle(), scrollDir, &scrollInfo, TRUE);
	GetScrollInfo (Handle(), scrollDir, &scrollInfo);
	if (scrollInfo.nPos != pos)
		{
		if (aMessage == WM_HSCROLL)
			{
			iView->SetHorzScrollPosition(scrollInfo.nPos);
			}
		else
			{
			iView->SetVertScrollPosition(scrollInfo.nPos);
			}
		}
	}

void CConsoleWindow::HandleSizeChanged(int aWidth, int aHeight)
	{
	iView->SetSize(aWidth, aHeight);
	}

void CConsoleWindow::HandleMouseEvent(UINT aMessage, WPARAM aWParam, LPARAM aLParam)
	{
	if (!iNoSelection)
		{
		switch(aMessage)
			{
			case WM_LBUTTONDOWN:
				{
				SetCapture(Handle());
				iView->StartSelection(GET_X_LPARAM(aLParam), GET_Y_LPARAM(aLParam));
				break;
				}
			case WM_LBUTTONDBLCLK:
				{
				iView->SelectWord(GET_X_LPARAM(aLParam), GET_Y_LPARAM(aLParam));
				break;
				}
			case WM_MOUSEMOVE:
				{
				RECT clientRect;
				GetClientRect(Handle(), &clientRect);
				POINT mousePos = { GET_X_LPARAM(aLParam), GET_Y_LPARAM(aLParam) };
				if ((mousePos.x > clientRect.right) || (mousePos.x < clientRect.left))
					{
					SCROLLINFO scrollInfo;
					scrollInfo.cbSize = sizeof(scrollInfo);
					scrollInfo.fMask = SIF_POS;
					GetScrollInfo(Handle(), SB_HORZ, &scrollInfo);
					if (mousePos.x > clientRect.right)
						{
						scrollInfo.nPos += iView->CharWidth();
						}
					else
						{
						scrollInfo.nPos -= iView->CharWidth();
						}
					SetScrollInfo (Handle(), SB_HORZ, &scrollInfo, TRUE);
					GetScrollInfo (Handle(), SB_HORZ, &scrollInfo);
					iView->SetHorzScrollPosition(scrollInfo.nPos);
					}
				if ((mousePos.y < clientRect.top) || (mousePos.y > clientRect.bottom))
					{
					SCROLLINFO scrollInfo;
					scrollInfo.cbSize = sizeof(scrollInfo);
					scrollInfo.fMask = SIF_POS;
					GetScrollInfo(Handle(), SB_VERT, &scrollInfo);
					if (mousePos.y < clientRect.top)
						{
						scrollInfo.nPos -= iView->CharHeight();
						}
					else
						{
						scrollInfo.nPos += iView->CharHeight();
						}
					SetScrollInfo (Handle(), SB_VERT, &scrollInfo, TRUE);
					GetScrollInfo (Handle(), SB_VERT, &scrollInfo);
					iView->SetVertScrollPosition(scrollInfo.nPos);
					}

				iView->AdjustSelection(GET_X_LPARAM(aLParam), GET_Y_LPARAM(aLParam));
				break;
				}
			case WM_LBUTTONUP:
				{
				ReleaseCapture();
				iView->EndSelection(GET_X_LPARAM(aLParam), GET_Y_LPARAM(aLParam));
				break;
				}
			default:
				{
				break;
				}
			}
		}
	}

void CConsoleWindow::HandleMenuPopUp(HMENU aMenu)
	{
	int numMenuItems = GetMenuItemCount(aMenu);
	for (int i = 0; i < numMenuItems; ++i)
		{
		switch (GetMenuItemID(aMenu, i))
			{
			case ID_EDIT_COPY:
				{
				EnableMenuItem(aMenu, ID_EDIT_COPY, iView->SelectionAvailable() ? MF_BYCOMMAND | MF_ENABLED : MF_BYCOMMAND | MF_GRAYED);
				break;
				}
			case ID_EDIT_PASTE:
				{
				EnableMenuItem(aMenu, ID_EDIT_PASTE, iCharHandler && IsClipboardFormatAvailable(CF_TEXT) ? MF_BYCOMMAND | MF_ENABLED : MF_BYCOMMAND | MF_GRAYED);
				break;
				}
			default:
				{
				break;
				}
			}
		}
	}

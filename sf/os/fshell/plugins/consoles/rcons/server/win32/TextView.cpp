// TextView.cpp
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
#include "TextView.h"
#include "Window.h"


const int KCursorHeight = 2;


CTextView* CTextView::New(CWindow& aWindow, CTextBuffer& aTextBuffer, int aWidthInChars, int aHeightInChars)
	{
	std::auto_ptr<CTextView> self(new(EThrow) CTextView(aWindow, aTextBuffer));
	self->Construct(aWidthInChars, aHeightInChars);
	return self.release();
	}

CTextView::~CTextView()
	{
	}

CTextView::CTextView(CWindow& aWindow, CTextBuffer& aTextBuffer)
	: iWindow(aWindow), iTextBuffer(aTextBuffer), iViewWidth(0), iViewHeight(0), iNumOverflowLines(aTextBuffer.NumOverflowLines()), iCursorEnabled(TRUE), iUpdating(FALSE), iCursorHidden(FALSE), iFocused(FALSE), iSelectionX1(-1), iSelectionY1(-1), iSelectionX2(-1), iSelectionY2(-1), iSelecting(FALSE), iDimmed(FALSE)
	{
	}

void CTextView::Construct(int aWidthInChars, int aHeightInChars)
	{
	iFont = (HFONT)GetStockObject(ANSI_FIXED_FONT);
	LOGFONT logFont;
	if (GetObject(iFont, sizeof(LOGFONT), &logFont) == 0)
		{
		throw KExceptionConsoleWindowConstructFailed;
		}

	// ToDo - Figure out why logFont appears to be out by 1.
	iCharWidth = 8;//logFont.lfWidth;
	iCharHeight = 13;//logFont.lfHeight;
	iViewWidth = aWidthInChars * iCharWidth;
	iViewHeight = aHeightInChars * iCharHeight;

	ResetBufferPos();
	ConfigureScrollBars(TRUE);
	iTextBuffer.SetObserver(this);
	}

void CTextView::EnableCursor()
	{
	if (!iCursorEnabled)
		{
		iCursorEnabled = TRUE;
		SetCursorVisibility();
		}
	}

void CTextView::DisableCursor()
	{
	if (iCursorEnabled)
		{
		iCursorEnabled = FALSE;
		SetCursorVisibility();
		}
	}

void CTextView::BeginUpdate()
	{
	ASSERT(!iUpdating);
	iUpdating = TRUE;
	SetCursorVisibility();
	}

void CTextView::EndUpdate()
	{
	ASSERT(iUpdating);
	iUpdating = FALSE;
	SetCursorVisibility();
	}

void CTextView::Draw() const
	{
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(iWindow.Handle(), &ps);
	HRGN undrawnRegion = CreateRectRgn(ps.rcPaint.left, ps.rcPaint.top, ps.rcPaint.right, ps.rcPaint.bottom);
	--ps.rcPaint.right;
	--ps.rcPaint.bottom;
	SetMapMode(hdc, MM_TEXT);
	HFONT oldFont = (HFONT)SelectObject(hdc, iFont);
	int bufferWidth;
	int bufferHeight;
	iTextBuffer.GetSize(bufferWidth, bufferHeight);
	int firstLineToDraw;
	int lastLineToDraw;
	GetVisibleLines(ps.rcPaint, firstLineToDraw, lastLineToDraw);
	RECT selectionRect;
	GetSelectionRect(selectionRect);

	for (int i = firstLineToDraw; i <= lastLineToDraw; ++i)
		{
		RECT lineRect;
		GetViewRect(0, i, bufferWidth, lineRect);
		RECT selectionIntersection;
		IntersectRect(&selectionIntersection, &selectionRect, &lineRect);
		if (IsRectEmpty(&selectionIntersection))
			{
			COLORREF origTextColor;
			if (iDimmed)
				{
				origTextColor = SetTextColor(hdc, GetSysColor(COLOR_GRAYTEXT));
				}
			TextOut(hdc, lineRect.left, lineRect.top, iTextBuffer.GetLine(i), bufferWidth);
			if (iDimmed)
				{
				SetTextColor(hdc, origTextColor);
				}
			}
		else
			{
			LPCTSTR lineText = iTextBuffer.GetLine(i);
			RECT drawRect;
			drawRect.left = lineRect.left;
			drawRect.top = lineRect.top;
			drawRect.right = selectionIntersection.left;
			drawRect.bottom = selectionIntersection.bottom;
			COLORREF origTextColor;
			if (iDimmed)
				{
				origTextColor = SetTextColor(hdc, GetSysColor(COLOR_GRAYTEXT));
				}
			ExtTextOut(hdc, lineRect.left, lineRect.top, ETO_CLIPPED, &drawRect, lineText, bufferWidth, NULL);
			if (iDimmed)
				{
				SetTextColor(hdc, origTextColor);
				}
			drawRect.left = drawRect.right;
			drawRect.right = selectionIntersection.right;
			origTextColor = SetTextColor(hdc, GetSysColor(COLOR_HIGHLIGHTTEXT));
			COLORREF origBackgroundColor = SetBkColor(hdc, GetSysColor(COLOR_HIGHLIGHT));
			ExtTextOut(hdc, lineRect.left, lineRect.top, ETO_CLIPPED, &drawRect, lineText, bufferWidth, NULL);
			drawRect.left = drawRect.right;
			drawRect.right = lineRect.right;
			SetTextColor(hdc, origTextColor);
			SetBkColor(hdc, origBackgroundColor);
			if (iDimmed)
				{
				origTextColor = SetTextColor(hdc, GetSysColor(COLOR_GRAYTEXT));
				}
			ExtTextOut(hdc, lineRect.left, lineRect.top, ETO_CLIPPED, &drawRect, lineText, bufferWidth, NULL);
			if (iDimmed)
				{
				SetTextColor(hdc, origTextColor);
				}
			}
		HRGN lineRegion = CreateRectRgn(lineRect.left, lineRect.top, lineRect.right, lineRect.bottom);
		CombineRgn(undrawnRegion, undrawnRegion, lineRegion, RGN_DIFF);
		DeleteObject(lineRegion);
		}

	HBRUSH brush = CreateSolidBrush(GetSysColor(COLOR_WINDOW));
	FillRgn(hdc, undrawnRegion, brush);
	DeleteObject(brush);
	DeleteObject(undrawnRegion);
	SelectObject(hdc, oldFont);
	EndPaint(iWindow.Handle(), &ps);
	}

void CTextView::HandleFocusGained()
	{
	iFocused = TRUE;
	CreateCaret(iWindow.Handle(), (HBITMAP) NULL, iCharWidth, 2);
	iCursorHidden = TRUE;
	SetCursorPos();
	SetCursorVisibility();
	}

void CTextView::HandleFocusLost()
	{
	iFocused = FALSE;
	DestroyCaret();
	}

void CTextView::SetSize(int aWidth, int aHeight)
	{
	iViewWidth = aWidth;
	iViewHeight = aHeight;
	ResetBufferPos();
	SetCursorPos();
	ConfigureScrollBars(TRUE);
	ReDraw();
	}

void CTextView::GetSize(int& aWidth, int& aHeight) const
	{
	aWidth = iViewWidth;
	aHeight = iViewHeight;
	}

void CTextView::SetHorzScrollPosition(int aPosX)
	{
	int newBufferPosX;
	int newBufferPosY;
	MapScrollPosToBufPix(aPosX, 0, newBufferPosX, newBufferPosY);
	int dX = newBufferPosX - iBufferPosX;
	SetBufferPos(newBufferPosX, iBufferPosY);
	SetCursorPos();
	ScrollWindowEx(iWindow.Handle(), dX, 0, NULL, NULL, NULL, NULL, SW_INVALIDATE);
	UpdateWindow(iWindow.Handle());
	}

void CTextView::SetVertScrollPosition(int aPosY)
	{
	int newBufferPosX;
	int newBufferPosY;
	MapScrollPosToBufPix(0, aPosY, newBufferPosX, newBufferPosY);
	int dY = newBufferPosY - iBufferPosY;
	SetBufferPos(iBufferPosX, newBufferPosY);
	SetCursorPos();
	ScrollWindowEx(iWindow.Handle(), 0, dY, NULL, NULL, NULL, NULL, SW_INVALIDATE);
	UpdateWindow(iWindow.Handle());
	}

void CTextView::StartSelection(int aPosX, int aPosY)
	{
	ASSERT(!iSelecting);
	iSelecting = TRUE;
	RECT invalidRect;
	GetSelectionRect(invalidRect);
	if (!IsRectEmpty(&invalidRect))
		{
		InvalidateRect(iWindow.Handle(), &invalidRect, FALSE);
		}
	MapViewPixToBufPix(aPosX, aPosY, aPosX, aPosY);
	MapBufPixToBufChar(aPosX, aPosY, iSelectionX1, iSelectionY1);
	iSelectionX2 = iSelectionX1;
	iSelectionY2 = iSelectionY1;
	GetSelectionRect(invalidRect);
	InvalidateRect(iWindow.Handle(), &invalidRect, FALSE);
	UpdateWindow(iWindow.Handle());
	}

void CTextView::AdjustSelection(int aPosX, int aPosY)
	{
	if (iSelecting)
		{
		RECT previousSelectionRect;
		GetSelectionRect(previousSelectionRect);
		MapViewPixToBufPix(aPosX, aPosY, aPosX, aPosY);
		MapBufPixToBufChar(aPosX, aPosY, iSelectionX2, iSelectionY2);
		int bufferWidth;
		int bufferHeight;
		iTextBuffer.GetSize(bufferWidth, bufferHeight);
		if (iSelectionX2 < 0)
			{
			iSelectionX2 = 0;
			}
		if (iSelectionX2 >= bufferWidth)
			{
			iSelectionX2 = bufferWidth - 1;
			}
		if (iSelectionY2 < -iNumOverflowLines)
			{
			iSelectionY2 = -iNumOverflowLines;
			}
		if (iSelectionY2 >= bufferHeight)
			{
			iSelectionY2 = bufferHeight - 1;
			}
		RECT newSelectionRect;
		GetSelectionRect(newSelectionRect);

		HRGN previousSelectionRegion = CreateRectRgn(previousSelectionRect.left, previousSelectionRect.top, previousSelectionRect.right, previousSelectionRect.bottom);
		HRGN region = CreateRectRgn(newSelectionRect.left, newSelectionRect.top, newSelectionRect.right, newSelectionRect.bottom);
		CombineRgn(region, region, previousSelectionRegion, RGN_XOR);
		DeleteObject(previousSelectionRegion);

		RECT regionBox;
		if (GetRgnBox(region, &regionBox) != NULLREGION)
			{
			InvalidateRgn(iWindow.Handle(), region, FALSE);
			UpdateWindow(iWindow.Handle());
			}
		DeleteObject(region);
		}
	}

void CTextView::EndSelection(int aPosX, int aPosY)
	{
	if (iSelecting)
		{
		AdjustSelection(aPosX, aPosY);
		}
	iSelecting = FALSE;
	}

void CTextView::SelectWord(int aPosX, int aPosY)
	{
	iSelecting = FALSE;
	RECT invalidRect;
	GetSelectionRect(invalidRect);
	if (!IsRectEmpty(&invalidRect))
		{
		InvalidateRect(iWindow.Handle(), &invalidRect, FALSE);
		}

	MapViewPixToBufPix(aPosX, aPosY, aPosX, aPosY);
	MapBufPixToBufChar(aPosX, aPosY, iSelectionX1, iSelectionY1);
	iSelectionX2 = iSelectionX1;
	iSelectionY2 = iSelectionY1;
	LPCTSTR linePtr = iTextBuffer.GetLine(iSelectionY1);

	if (*(linePtr + iSelectionX1) != TCHAR(' '))
		{
		// Find the beginning of the word.
		while ((*(linePtr + iSelectionX1) != TCHAR(' ')) && (iSelectionX1 > 0))
			{
			--iSelectionX1;
			}
		if (*(linePtr + iSelectionX1) == TCHAR(' '))
			{
			++iSelectionX1;
			}
		int bufferWidth;
		int bufferHeight;
		iTextBuffer.GetSize(bufferWidth, bufferHeight);

		// Find the end of the word.
		while ((*(linePtr + iSelectionX2) != TCHAR(' ')) && (iSelectionX2 < bufferWidth))
			{
			++iSelectionX2;
			}
		if (*(linePtr + iSelectionX2) == TCHAR(' '))
			{
			--iSelectionX2;
			}
		}

	GetSelectionRect(invalidRect);
	InvalidateRect(iWindow.Handle(), &invalidRect, FALSE);
	}

int CTextView::CharHeight() const
	{
	return iCharHeight;
	}

int CTextView::CharWidth() const
	{
	return iCharWidth;
	}

bool CTextView::SelectionAvailable() const
	{
	if (iSelectionX1 == -1)
		{
		ASSERT(iSelectionY1 == -1);
		ASSERT(iSelectionX2 == -1);
		ASSERT(iSelectionY2 == -1);
		return FALSE;
		}
	return TRUE;
	}

LPTSTR CTextView::Selection() const
	{
	ASSERT(iSelectionX1 != -1);
	ASSERT(iSelectionY1 != -1);
	ASSERT(iSelectionX2 != -1);
	ASSERT(iSelectionY2 != -1);
	int tlX = min(iSelectionX1, iSelectionX2);
	int tlY = min(iSelectionY1, iSelectionY2);
	int brX = max(iSelectionX1, iSelectionX2);
	int brY = max(iSelectionY1, iSelectionY2);
	TCHAR* buf = new TCHAR[((brX - tlX + 3) * (brY - tlY + 1)) + 1];
	if (buf)
		{
		TCHAR* bufPtr = buf;
		for (int y = tlY; y <= brY; ++y)
			{
			LPCTSTR lineText = iTextBuffer.GetLine(y);
			int x;
			for (x = tlX; x <= brX; ++x)
				{
				*bufPtr++ = lineText[x];
				}
			while ((*(bufPtr - 1) == TCHAR(' ')) && (x > tlX))
				{	// Remove trailing spaces from line.
				--bufPtr;
				--x;
				}
			if (y < brY)
				{
				*bufPtr++ = TCHAR('\r');
				*bufPtr++ = TCHAR('\n');
				}
			}
		*bufPtr = TCHAR('\0');
		}
	return buf;
	}

void CTextView::ClearSelection()
	{
	RECT invalidRect;
	GetSelectionRect(invalidRect);
	iSelectionX1 = iSelectionY1 = iSelectionX2 = iSelectionY2 = -1;
	if (!IsRectEmpty(&invalidRect))
		{
		InvalidateRect(iWindow.Handle(), &invalidRect, FALSE);
		UpdateWindow(iWindow.Handle());
		}
	}

void CTextView::SetDimmed(bool aDimmed)
	{
	if (!aDimmed != !iDimmed)
		{
		iDimmed = aDimmed;
		if (iDimmed)
			{
			DisableCursor();
			}
		else
			{
			EnableCursor();
			}
		iWindow.Redraw();
		}
	}

void CTextView::HandleTextBufferChange(int aPosX, int aPosY, PTCHAR aPtr, int aLength)
	{
	RECT viewRect;
	GetViewRect(aPosX, aPosY, aLength, viewRect);
	RECT clientRect = {0, 0, iViewWidth, iViewHeight};
	RECT intersection;
	IntersectRect(&intersection, &clientRect, &viewRect);
	if (!IsRectEmpty(&intersection))
		{
		InvalidateRect(iWindow.Handle(), &intersection, FALSE);
		UpdateWindow(iWindow.Handle());
		}
	}

void CTextView::HandleTextBufferScroll()
	{
	bool doScroll(FALSE);
	ASSERT((iNumOverflowLines == iTextBuffer.NumOverflowLines()) || (iNumOverflowLines + 1 == iTextBuffer.NumOverflowLines()));
	bool newOverflowLine(iNumOverflowLines != iTextBuffer.NumOverflowLines());
	int moveBufferBy(0);

	if (BottomLineVisible())
		{
		if (newOverflowLine)
			{
			moveBufferBy = -iCharHeight;
			}
		doScroll = TRUE;
		}
	else if (TopLineVisible() && !newOverflowLine)
		{
		doScroll = TRUE;
		}
	else if (!newOverflowLine)
		{
		moveBufferBy = iCharHeight;
		}

	if (newOverflowLine)
		{
		++iNumOverflowLines;
		}

	ScrollSelection();

	if (moveBufferBy)
		{
		MoveBuffer(moveBufferBy);
		}

	if (doScroll)
		{
		ScrollWindowEx(iWindow.Handle(), 0, -iCharHeight, NULL, NULL, NULL, NULL, SW_INVALIDATE);
		UpdateWindow(iWindow.Handle());
		}

	SetCursorPos();
	ConfigureScrollBars(FALSE);
	}

void CTextView::HandleTextBufferCursorChange()
	{
	SetCursorPos();
	}

void CTextView::HandleTextBufferCleared()
	{
	iNumOverflowLines = 0;
	ResetBufferPos();
	ConfigureScrollBars(TRUE);
	ReDraw();
	}

void CTextView::SetCursorVisibility()
	{
	if (iCursorEnabled && !iUpdating && CursorInView())
		{
		if (iCursorHidden)
			{
			ShowCaret(iWindow.Handle());
			iCursorHidden = FALSE;
			}
		}
	else if (!iCursorHidden)
		{
		HideCaret(iWindow.Handle());
		iCursorHidden = TRUE;
		}
	}

bool CTextView::CursorInView() const
	{
	int cursorX;
	int cursorY;
	GetCursorPixelPos(cursorX, cursorY);
	RECT cursorRect = {cursorX, cursorY, cursorX + iCharWidth, cursorY + KCursorHeight};
	return RectVisible(cursorRect);
	}

void CTextView::GetCursorPixelPos(int& aX, int& aY) const
	{
	iTextBuffer.GetCursorPos(aX, aY);
	MapBufCharToBufPix(aX, aY, aX, aY);
	aY += (iCharHeight - KCursorHeight);
	MapBufPixToViewPix(aX, aY, aX, aY);
	}

void CTextView::SetCursorPos()
	{
	if (iFocused)
		{
		int cursorX;
		int cursorY;
		GetCursorPixelPos(cursorX, cursorY);
		SetCaretPos(cursorX, cursorY);
		}
	}

void CTextView::ResetBufferPos()
	{
	SetBufferPos(0, MinBufferPosY());
	}

void CTextView::SetBufferPos(int aPosX, int aPosY)
	{
	iBufferPosX = aPosX;
	iBufferPosY = aPosY;
	SetCursorVisibility();
	}

void CTextView::MoveBuffer(int aDiffY)
	{
	iBufferPosY += aDiffY;
	ASSERT(iBufferPosY <= MaxBufferPosY());
	int min = MinBufferPosY();
	if (iBufferPosY < min)
		{
		iBufferPosY = min;
		}
	}

void CTextView::ConfigureScrollBars(bool aReset)
	{
	int bufferWidth;
	int bufferHeight;
	iTextBuffer.GetSize(bufferWidth, bufferHeight);
	bufferHeight += iNumOverflowLines;

	int vertMax = (bufferHeight * iCharHeight) - 1;
	int horzMax = (bufferWidth * iCharWidth) - 1;

	SCROLLINFO scrollInfo;
	scrollInfo.cbSize = sizeof(scrollInfo);
	scrollInfo.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;
	scrollInfo.nMin = 0;
	scrollInfo.nMax = vertMax;
	scrollInfo.nPage = iViewHeight;
	if (aReset)
		{
		scrollInfo.nPos = vertMax;
		}
	else
		{
		scrollInfo.nPos = -iBufferPosY;
		}
	SetScrollInfo(iWindow.Handle(), SB_VERT, &scrollInfo, TRUE);

	scrollInfo.fMask = aReset ? SIF_RANGE | SIF_PAGE | SIF_POS : SIF_RANGE | SIF_PAGE;
	scrollInfo.nMax = horzMax;
	scrollInfo.nPage = iViewWidth;
	if (aReset)
		{
		scrollInfo.nPos = 0;
		}
	SetScrollInfo(iWindow.Handle(), SB_HORZ, &scrollInfo, TRUE);
	}

void CTextView::ReDraw() const
	{
	InvalidateRect(iWindow.Handle(), NULL, FALSE);
	}

void CTextView::GetViewRect(int aCharPosX, int aCharPosY, int aNumChars, RECT& aRect) const
	{
	int x;
	int y;
	MapBufCharToBufPix(aCharPosX, aCharPosY, x, y);
	MapBufPixToViewPix(x, y, x, y);
	aRect.left = x;
	aRect.top = y;
	aRect.right = aRect.left + (aNumChars * iCharWidth);
	aRect.bottom = aRect.top + iCharHeight;
	}

void CTextView::GetViewRect(int aCharPosX1, int aCharPosY1, int aCharPosX2, int aCharPosY2, RECT& aRect) const
	{
	int tlX = min(aCharPosX1, aCharPosX2);
	int tlY = min(aCharPosY1, aCharPosY2);
	int brX = max(aCharPosX1, aCharPosX2);
	int brY = max(aCharPosY1, aCharPosY2);
	MapBufCharToBufPix(tlX, tlY, tlX, tlY);
	MapBufPixToViewPix(tlX, tlY, tlX, tlY);
	MapBufCharToBufPix(brX, brY, brX, brY);
	MapBufPixToViewPix(brX, brY, brX, brY);
	brX += iCharWidth;
	brY += iCharHeight;
	aRect.left = tlX;
	aRect.top = tlY;
	aRect.right = brX;
	aRect.bottom = brY;
	}

bool CTextView::RectVisible(const RECT& aRect) const
	{
	RECT clientRect = {0, 0, iViewWidth, iViewHeight};
	RECT intersection;
	IntersectRect(&intersection, &clientRect, &aRect);
	return !IsRectEmpty(&intersection);
	}

bool CTextView::LineVisible(int aLinePos) const
	{
	int bufWidth;
	int bufHeight;
	iTextBuffer.GetSize(bufWidth, bufHeight);
	RECT rect;
	GetViewRect(0, aLinePos, bufWidth, rect);
	return RectVisible(rect);
	}

bool CTextView::TopLineVisible() const
	{
	return LineVisible(-iNumOverflowLines);
	}

bool CTextView::BottomLineVisible() const
	{
	int bufWidth;
	int bufHeight;
	iTextBuffer.GetSize(bufWidth, bufHeight);
	return LineVisible(bufHeight - 1);
	}

int CTextView::TotalBufferHeight() const
	{
	int bufferWidth;
	int bufferHeight;
	iTextBuffer.GetSize(bufferWidth, bufferHeight);
	return (bufferHeight + iNumOverflowLines) * iCharHeight;
	}

int CTextView::MinBufferPosY() const
	{
	return iViewHeight - TotalBufferHeight();
	}

int CTextView::MaxBufferPosY() const
	{
	int minBufferPosY = MinBufferPosY();
	if (minBufferPosY <= 0)
		{
		return 0;
		}
	return minBufferPosY;
	}

void CTextView::GetVisibleLines(const RECT& aRect, int& aFrom, int& aTo) const
	{
	int x;
	int y;
	MapViewPixToBufPix(aRect.left, aRect.top, x, y);
	MapBufPixToBufChar(x, y, x, aFrom);
	MapViewPixToBufPix(aRect.right, aRect.bottom, x, y);
	MapBufPixToBufChar(x, y, x, aTo);
	int bufWidth;
	int bufHeight;
	iTextBuffer.GetSize(bufWidth, bufHeight);
	if (aFrom < -iNumOverflowLines)
		{
		aFrom = -iNumOverflowLines;
		}
	if (aFrom >= bufHeight)
		{
		aFrom = bufHeight - 1;
		}
	if (aTo >= bufHeight)
		{
		aTo = bufHeight - 1;
		}
	}

void CTextView::MapBufCharToBufPix(int aBufCharPosX, int aBufCharPosY, int& aBufPixPosX, int& aBufPixPosY) const
	{
	aBufPixPosX = aBufCharPosX * iCharWidth;
	aBufPixPosY = (iNumOverflowLines + aBufCharPosY) * iCharHeight;
	}

void CTextView::MapBufPixToViewPix(int aBufPixPosX, int aBufPixPosY, int& aViewPixPosX, int& aViewPixPosY) const
	{
	aViewPixPosX = aBufPixPosX + iBufferPosX;
	aViewPixPosY = aBufPixPosY + iBufferPosY;
	}

void CTextView::MapBufPixToBufChar(int aBufPixPosX, int aBufPixPosY, int& aBufCharPosX, int& aBufCharPosY) const
	{
	aBufCharPosX = aBufPixPosX / iCharWidth;
	aBufCharPosY = (aBufPixPosY / iCharHeight) - iNumOverflowLines;
	}

void CTextView::MapViewPixToBufPix(int aViewPixPosX, int aViewPixPosY, int& aBufPixPosX, int& aBufPixPosY) const
	{
	aBufPixPosX = aViewPixPosX - iBufferPosX;
	aBufPixPosY = aViewPixPosY - iBufferPosY;
	}

void CTextView::MapScrollPosToBufPix(int aScrollPosX, int aScrollPosY, int& aBufPixPosX, int& aBufPixPosY) const
	{
	aBufPixPosX = -aScrollPosX;
	aBufPixPosY = -aScrollPosY;
	}

void CTextView::GetSelectionRect(RECT& aRect) const
	{
	if (iSelectionX1 == -1)
		{
		ASSERT(iSelectionY1 == -1);
		ASSERT(iSelectionX2 == -1);
		ASSERT(iSelectionY2 == -1);
		aRect.bottom = aRect.left = aRect.right = aRect.top = 0;
		}
	else
		{
		GetViewRect(iSelectionX1, iSelectionY1, iSelectionX2, iSelectionY2, aRect);
		}
	}

void CTextView::ScrollSelection()
	{
	if (iSelectionX1 == -1)
		{
		ASSERT(iSelectionY1 == -1);
		ASSERT(iSelectionX2 == -1);
		ASSERT(iSelectionY2 == -1);
		}
	else
		{
		if ((iSelectionY1 == -iNumOverflowLines) && (iSelectionY2 == -iNumOverflowLines))
			{
			iSelectionX1 = iSelectionY1 = iSelectionX2 = iSelectionY2 = -1;
			}
		else
			{
			--iSelectionY1;
			--iSelectionY2;
			if (iSelectionY1 < -iNumOverflowLines)
				{
				iSelectionY1 = -iNumOverflowLines;
				}
			if (iSelectionY2 < -iNumOverflowLines)
				{
				iSelectionY2 = -iNumOverflowLines;
				}
			}
		}
	}

void CTextView::ScrollToEndIfNeeded()
	{
	if (!BottomLineVisible())
		{
		SetVertScrollPosition(TotalBufferHeight()-iViewHeight);
		ConfigureScrollBars(FALSE);
		}
	}

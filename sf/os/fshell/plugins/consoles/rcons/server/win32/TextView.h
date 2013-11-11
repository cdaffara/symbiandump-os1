// TextView.h
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

#ifndef __TEXTVIEW_H__
#define __TEXTVIEW_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "base.h"
#include "TextBuffer.h"

class CWindow;


class CTextView : public CBase, public MTextBufferObserver
	{
public:
	static CTextView* New(CWindow& aWindow, CTextBuffer& aTextBuffer, int aWidthInChars, int aHeightInChars);
	virtual ~CTextView();
	void EnableCursor();
	void DisableCursor();
	void BeginUpdate();
	void EndUpdate();
	void Draw() const;
	void HandleFocusGained();
	void HandleFocusLost();
	void SetSize(int aWidth, int aHeight);
	void GetSize(int& aWidth, int& aHeight) const;
	void SetHorzScrollPosition(int aPosX);
	void SetVertScrollPosition(int aPosY);
	void StartSelection(int aPosX, int aPosY);
	void AdjustSelection(int aPosX, int aPosY);
	void EndSelection(int aPosX, int aPosY);
	void SelectWord(int aPosX, int aPosY);
	int CharHeight() const;
	int CharWidth() const;
	bool SelectionAvailable() const;
	LPTSTR Selection() const;
	void ClearSelection();
	void SetDimmed(bool aDimmed);
	void ScrollToEndIfNeeded();
private:
	CTextView(CWindow& aWindow, CTextBuffer& aTextBuffer);
	void Construct(int aWidthInChars, int aHeightInChars);
private: // From MTextBufferObserver.
	virtual void HandleTextBufferChange(int aPosX, int aPosY, PTCHAR aPtr, int aLength);
	virtual void HandleTextBufferScroll();
	virtual void HandleTextBufferCursorChange();
	virtual void HandleTextBufferCleared();
private:
	void SetCursorVisibility();
	bool CursorInView() const;
	void GetCursorPixelPos(int& aX, int& aY) const;
	void SetCursorPos();
	void ResetBufferPos();
	void SetBufferPos(int aPosX, int aPosY);
	void MoveBuffer(int aDiffY);
	void ConfigureScrollBars(bool aReset);
	void ReDraw() const;
	void GetViewRect(int aCharPosX, int aCharPosY, int aNumChars, RECT& aRect) const;
	void GetViewRect(int aCharPosX1, int aCharPosY1, int aCharPosX2, int aCharPosY2, RECT& aRect) const;
	bool RectVisible(const RECT& aRect) const;
	bool LineVisible(int aLinePos) const;
	bool TopLineVisible() const;
	bool BottomLineVisible() const;
	int TotalBufferHeight() const;
	int MinBufferPosY() const;
	int MaxBufferPosY() const;
	void GetVisibleLines(const RECT& aRect, int& aFrom, int& aTo) const;
	void MapBufCharToBufPix(int aBufCharPosX, int aBufCharPosY, int& aBufPixPosX, int& aBufPixPosY) const;
	void MapBufPixToViewPix(int aBufPixPosX, int aBufPixPosY, int& aViewPixPosX, int& aViewPixPosY) const;
	void MapBufPixToBufChar(int aBufPixPosX, int aBufPixPosY, int& aBufCharPosX, int& aBufCharPosY) const;
	void MapViewPixToBufPix(int aViewPixPosX, int aViewPixPosY, int& aBufPixPosX, int& aBufPixPosY) const;
	void MapScrollPosToBufPix(int aScrollPosX, int aScrollPosY, int& aBufPixPosX, int& aBufPixPosY) const;
	void GetSelectionRect(RECT& aRect) const;
	void ScrollSelection();
private:
	CWindow& iWindow;
	CTextBuffer& iTextBuffer;
	HFONT iFont;
	int iCharWidth;			///< Character width in pixels.
	int iCharHeight;		///< Character height in pixels.
	int iViewWidth;
	int iViewHeight;
	int iBufferPosX;
	int iBufferPosY;
	int iNumOverflowLines;
	bool iCursorEnabled;
	bool iUpdating;
	bool iCursorHidden;
	bool iFocused;
	int iSelectionX1;		///< In buffer character coordinates.
	int iSelectionY1;		///<        "             "
	int iSelectionX2;		///<        "             "
	int iSelectionY2;		///<        "             "
	bool iSelecting;
	bool iDimmed;
	};



/*
        /-- (iBufferPosX, iBufferPosY) in pixels relative to view.
       /
      |----------------------------------------------------------------|
      |                                                                |
      |                                                                |
      |                                                                |
      |                                                                |
      |                                                                |
      |                                                                |
      |        /-- (0, 0) in Windows pixel coordinates.                |
      |       /                                                        |
      |   i  |-------------------------------------------------|       |
      |   V  |                                               |-|       |
      |   i  |                                               | |       |
      |   e  |                                               | |       |
      |   w  |                                               | |       |
      |   H  |                                               | |       |
      |   e  |                                               | |       |
      |   i  |                                               | |       |
      |   g  |                                               | |       |
      |   h  |-|---------------------------------------------|-|       |
      |   t  |-------------------------------------------------|       |
      |                                                                |
      |      <----------- iViewWidth (in pixels) ----------->          |
      |                                                                |
      |                                                                |
      |                                                                |
      |                                                                |
      |                                                                |
      |                                                                |
      |----------------------------------------------------------------|
*/

#endif // __TEXTVIEW_H__

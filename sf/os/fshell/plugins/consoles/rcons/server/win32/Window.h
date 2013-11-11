// Window.h
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

#if !defined(AFX_WINDOW_H__079A244F_DED8_49A3_AB8E_EEE262119929__INCLUDED_)
#define AFX_WINDOW_H__079A244F_DED8_49A3_AB8E_EEE262119929__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "Base.h"

const unsigned int KSocketMessage = WM_USER;


class CWindow;


class MWindowMessageHandler
	{
public:
	MWindowMessageHandler();
	virtual LRESULT HandleWindowMessage(UINT aMessage, WPARAM aWParam, LPARAM aLParam) = 0;
private:
	friend class CWindow;
	MWindowMessageHandler* iNext;
	MWindowMessageHandler* iPrevious;
	};


class MWindowObserver
	{
public:
	virtual void HandleWindowClosure(CWindow& aWindow) = 0;
	virtual LRESULT HandleWindowCommand(UINT aMessage, WPARAM aWParam, LPARAM aLParam) = 0;
	};


class CWindow : public CBase
	{
public:
	virtual ~CWindow();
	void AddMessageHandler(MWindowMessageHandler& aHandler);
	void RemoveMessageHandler(MWindowMessageHandler& aHandler);
	HWND Handle() const;
	void SetTitle(LPCTSTR aTitle);
	void GetTitle(LPTSTR aTitle, int aMaxLength) const;
	void Redraw();
protected:
	CWindow();
	void Construct(HINSTANCE aAppHandle, LPCTSTR aWindowClass, LPCTSTR aTitle, int aPoxX, int aPosY, int aWidth, int aHeight, MWindowObserver* aObserver);
private:
	virtual void Draw() const;
	virtual LRESULT HandleChar(TCHAR aChar, UINT aModifiers);
	virtual LRESULT HandleCommand(UINT aMessage, WPARAM aWParam, LPARAM aLParam);
	virtual LRESULT HandleFocusChange(bool aFocused);
	virtual void HandleScrollEvent(UINT aMessage, WPARAM aWParam);
	virtual void HandleSizeChanged(int aWidth, int aHeight);
	virtual void HandleMouseEvent(UINT aMessage, WPARAM aWParam, LPARAM aLParam);
	virtual void HandleMenuPopUp(HMENU aMenu);
public:
	static LRESULT CALLBACK HandleMessage(HWND aHandle, UINT aMessage, WPARAM aWParam, LPARAM aLParam);
	static CWindow* Instance(HWND aHandle);
private:
	static CWindow* sFirst;
	static CWindow* sLast;
	static MWindowMessageHandler* sFirstMessageHandler;
	static MWindowMessageHandler* sLastMessageHandler;
	CWindow* iNext;
	CWindow* iPrevious;
	HINSTANCE iAppHandle;
	HWND iHandle;
	MWindowObserver* iObserver;
	};

#endif // !defined(AFX_WINDOW_H__079A244F_DED8_49A3_AB8E_EEE262119929__INCLUDED_)

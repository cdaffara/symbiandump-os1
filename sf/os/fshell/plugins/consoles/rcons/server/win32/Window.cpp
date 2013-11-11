// Window.cpp
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
#include "stdafx.h"
#include "console_host.h"
#include "Window.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif

const DWORD KWindowStyle = (WS_OVERLAPPEDWINDOW | WS_HSCROLL | WS_VSCROLL);// & ~WS_THICKFRAME;
CWindow* CWindow::sFirst = NULL;
CWindow* CWindow::sLast = NULL;
MWindowMessageHandler* CWindow::sFirstMessageHandler = NULL;
MWindowMessageHandler* CWindow::sLastMessageHandler = NULL;


// From e32keys.h
enum TKeyCode
	{
	EKeyNull=0x0000,
	EKeyBell=0x0007,
	EKeyBackspace=0x0008,
	EKeyTab=0x0009,
	EKeyLineFeed=0x000a,
	EKeyVerticalTab=0x000b,
	EKeyFormFeed=0x000c,
	EKeyEnter=0x000d,
	EKeyEscape=0x001b,
	EKeySpace=0x0020,
	EKeyDelete=0x007f,
	EKeyPrintScreen=0xf800,
	EKeyPause,
	EKeyHome,
	EKeyEnd,
	EKeyPageUp,
	EKeyPageDown,
	EKeyInsert,
	EKeyLeftArrow,
	EKeyRightArrow,
	EKeyUpArrow,
	EKeyDownArrow,
	EKeyLeftShift,
	EKeyRightShift,
	EKeyLeftAlt,
	EKeyRightAlt,
	EKeyLeftCtrl,
	EKeyRightCtrl,
	EKeyLeftFunc,
	EKeyRightFunc,
	EKeyCapsLock,
	EKeyNumLock,
	EKeyScrollLock,
	EKeyF1,
	EKeyF2,
	EKeyF3,
	EKeyF4,
	EKeyF5,
	EKeyF6,
	EKeyF7,
	EKeyF8,
	EKeyF9,
	EKeyF10,
	EKeyF11,
	EKeyF12,
	EKeyF13,
	EKeyF14,
	EKeyF15,
	EKeyF16,
	EKeyF17,
	EKeyF18,
	EKeyF19,
	EKeyF20,
	EKeyF21,
	EKeyF22,
	EKeyF23,
	EKeyF24,
    EKeyOff,
    EKeyIncContrast,
    EKeyDecContrast,
    EKeyBacklightOn,
    EKeyBacklightOff,
    EKeyBacklightToggle,
    EKeySliderDown,
    EKeySliderUp,
    EKeyMenu,
    EKeyDictaphonePlay,
    EKeyDictaphoneStop,
    EKeyDictaphoneRecord,
    EKeyHelp,
    EKeyDial,
	EKeyScreenDimension0,
	EKeyScreenDimension1,
	EKeyScreenDimension2,
	EKeyScreenDimension3,
	EKeyIncVolume,
	EKeyDecVolume,
	EKeyDevice0,
	EKeyDevice1,
	EKeyDevice2,
	EKeyDevice3,
	EKeyDevice4,
	EKeyDevice5,
	EKeyDevice6,
	EKeyDevice7,
	EKeyDevice8,
	EKeyDevice9,
	EKeyDeviceA,
	EKeyDeviceB,
	EKeyDeviceC,
	EKeyDeviceD,
	EKeyDeviceE,
	EKeyDeviceF,
	EKeyApplication0,
	EKeyApplication1,
	EKeyApplication2,
	EKeyApplication3,
	EKeyApplication4,
	EKeyApplication5,
	EKeyApplication6,
	EKeyApplication7,
	EKeyApplication8,
	EKeyApplication9,
	EKeyApplicationA,
	EKeyApplicationB,
	EKeyApplicationC,
	EKeyApplicationD,
	EKeyApplicationE,
	EKeyApplicationF,
	EKeyYes,
	EKeyNo,
	EKeyIncBrightness,
	EKeyDecBrightness
	};

enum TEventModifier
	{
	EModifierAutorepeatable=0x00000001, /**< Key event can auto-repeat.*/
	EModifierKeypad=0x00000002,         /**< The key that generated the event was on the numeric keypad, on the emulator.*/
	EModifierLeftAlt=0x00000004,        /**< Left Alt key.*/
	EModifierRightAlt=0x00000008,       /**< Right Alt key.*/
	EModifierAlt=0x00000010,            /**< Single Alt key.*/
	EModifierLeftCtrl=0x00000020,       /**< Left Control (Ctrl) key.*/
	EModifierRightCtrl=0x00000040,      /**< Right Control (Ctrl) key.*/
	EModifierCtrl=0x00000080,           /**< Single Control (Ctrl) key.*/
	EModifierLeftShift=0x00000100,      /**< Left Shift key.*/
	EModifierRightShift=0x00000200,     /**< Right Shift key.*/
	EModifierShift=0x00000400,          /**< Single Shift key.*/
	EModifierLeftFunc=0x00000800,       /**< Left Fn key.*/
	EModifierRightFunc=0x00001000,      /**< Right Fn key.*/
	EModifierFunc=0x00002000,           /**< Single Fn key.*/
	EModifierCapsLock=0x00004000,       /**< Caps lock key.*/
	EModifierNumLock=0x00008000,        /**< Num lock key.*/
	EModifierScrollLock=0x00010000,     /**< Scroll lock key.*/
	EModifierKeyUp=0x00020000,          /**< Key up event.*/
	EModifierSpecial=0x00040000,        /**< The keycode is a non-standard keyboard character that has been generated in a special keyboard mode, for example accented vowels.*/
	EModifierDoubleClick=0x00080000,    /**< Double click.*/
	EModifierPureKeycode=0x00100000,    /**< The key code in the key event is not changed. E.g.an alphabetic key is not changed by the Caps Lock or Shift key being pressed.*/
	EModifierKeyboardExtend=0x00200000,	/**< The "Keyboard extend" generated modifier. */
	EModifierCancelRotation=0x00000000, /**< No Keyboard rotation is in effect. */
	EModifierRotateBy90=0x00400000,		/**< Keyboard rotation through 90 degrees clockwise is in effect. */
	EModifierRotateBy180=0x00800000,	/**< Keyboard rotation through 180 degrees clockwise is in effect. */
	EModifierRotateBy270=0x01000000,	/**< Keyboard rotation through 270 degrees clockwise is in effect. */
	EModifierPointer3DButton1=0x02000000,/**< 3D pointer device specific modifier (button 1). */
	EModifierPointer3DButton2=0x04000000,/**< 3D pointer device specific modifier (button 2). */
	EModifierPointer3DButton3=0x08000000,/**< 3D pointer device specific modifier (button 3). */
	EAllModifiers=0x0fffffff            /**< A combination of all event modifiers.*/
	};


MWindowMessageHandler::MWindowMessageHandler() : iNext(NULL), iPrevious(NULL)
	{
	}

CWindow::~CWindow()
	{
	if (sLast == this)
		{
		ASSERT(iNext == NULL);
		sLast = iPrevious;
		}
	if (sFirst == this)
		{
		sFirst = iNext;
		}
	if (iPrevious)
		{
		iPrevious->iNext = iNext;
		}
	if (iNext)
		{
		iNext->iPrevious = iPrevious;
		}
	}

void CWindow::AddMessageHandler(MWindowMessageHandler& aHandler)
	{
	if (sFirstMessageHandler == NULL)
		{
		ASSERT(sLastMessageHandler == NULL);
		sFirstMessageHandler = &aHandler;
		sLastMessageHandler = &aHandler;
		}
	else
		{
		sLastMessageHandler->iNext = &aHandler;
		aHandler.iPrevious = sLastMessageHandler;
		sLastMessageHandler = &aHandler;
		}
	}

void CWindow::RemoveMessageHandler(MWindowMessageHandler& aHandler)
	{
	if (sLastMessageHandler == &aHandler)
		{
		ASSERT(aHandler.iNext == NULL);
		sLastMessageHandler = aHandler.iPrevious;
		}
	if (sFirstMessageHandler == &aHandler)
		{
		sFirstMessageHandler = aHandler.iNext;
		}
	if (aHandler.iPrevious)
		{
		aHandler.iPrevious->iNext = aHandler.iNext;
		}
	if (aHandler.iNext)
		{
		aHandler.iNext->iPrevious = aHandler.iPrevious;
		}
	}

HWND CWindow::Handle() const
	{
	return iHandle;
	}

void CWindow::SetTitle(LPCTSTR aTitle)
	{
	SetWindowText(iHandle, aTitle);
	}

void CWindow::GetTitle(LPTSTR aTitle, int aMaxLength) const
	{
	GetWindowText(iHandle, aTitle, aMaxLength);
	}

CWindow::CWindow() : iNext(NULL), iPrevious(NULL), iObserver(NULL)
	{
	if (sFirst == NULL)
		{
		ASSERT(sLast == NULL);
		sFirst = this;
		sLast = this;
		}
	else
		{
		sLast->iNext = this;
		iPrevious = sLast;
		sLast = this;
		}
	}

void CWindow::Construct(HINSTANCE aAppHandle, LPCTSTR aWindowClass, LPCTSTR aTitle, int aPoxX, int aPosY, int aWidth, int aHeight, MWindowObserver* aObserver)
	{
	iAppHandle = aAppHandle;
	iObserver = aObserver;

	RECT windowRect = {0, 0, aWidth, aHeight};
	AdjustWindowRect(&windowRect, KWindowStyle, TRUE);
	windowRect.right += GetSystemMetrics(SM_CYVSCROLL);
	iHandle = CreateWindow(aWindowClass, aTitle, KWindowStyle, aPoxX, aPosY, windowRect.right-windowRect.left, windowRect.bottom-windowRect.top, NULL, NULL, aAppHandle, NULL);
	if (iHandle == 0)
		{
		throw KExceptionWindowConstructFailed;
		}
	ShowWindow(iHandle, SW_SHOWNORMAL);
	if (UpdateWindow(iHandle) == 0)
		{
		throw KExceptionWindowConstructFailed;
		}
	}

void CWindow::Redraw()
	{
	InvalidateRect(iHandle, NULL, TRUE);
	}

void CWindow::Draw() const
	{
	}

LRESULT CWindow::HandleChar(TCHAR, UINT)
	{
	return 0;
	}

LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
	{
	switch (message)
		{
		case WM_INITDIALOG:
				return TRUE;
		case WM_COMMAND:
			if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) 
				{
				EndDialog(hDlg, LOWORD(wParam));
				return TRUE;
				}
			break;
		}
    return FALSE;
	}

LRESULT CWindow::HandleCommand(UINT aMessage, WPARAM aWParam, LPARAM aLParam)
	{
	int wmId = LOWORD(aWParam); 
	int wmEvent = HIWORD(aWParam); 
	switch (wmId)
		{
		case ID_ABOUT:
			DialogBox(iAppHandle, (LPCTSTR)IDD_ABOUTBOX, iHandle, (DLGPROC)About);
			break;
		case IDM_EXIT:
			DestroyWindow(iHandle);
			break;
		default:
			if (iObserver)
				{
				return iObserver->HandleWindowCommand(aMessage, aWParam, aLParam);
				}
			return DefWindowProc(iHandle, aMessage, aWParam, aLParam);
		}
	return 0;
	}

LRESULT CWindow::HandleFocusChange(bool aFocused)
	{
	return 0;
	}

void CWindow::HandleScrollEvent(UINT aMessage, WPARAM aWParam)
	{
	}

void CWindow::HandleSizeChanged(int aWidth, int aHeight)
	{
	}

void CWindow::HandleMouseEvent(UINT aMessage, WPARAM aWParam, LPARAM aLParam)
	{
	}

void CWindow::HandleMenuPopUp(HMENU aMenu)
	{
	}

LRESULT CALLBACK CWindow::HandleMessage(HWND aHandle, UINT aMessage, WPARAM aWParam, LPARAM aLParam)
	{
	CWindow* window = Instance(aHandle);

	if (window)
		{
		MWindowMessageHandler* messageHandler = sFirstMessageHandler;
		while (messageHandler)
			{
			if (messageHandler->HandleWindowMessage(aMessage, aWParam, aLParam) == 0)
				{
				return 0;
				}
			messageHandler = messageHandler->iNext;
			}
		}

	switch (aMessage) 
		{
		case WM_SETFOCUS:
			if (window)
				{
				window->HandleFocusChange(TRUE);
				}
			break;
		case WM_KILLFOCUS:
			if (window)
				{
				window->HandleFocusChange(FALSE);
				}
			break;
		case WM_COMMAND:
			if (window)
				{
				return window->HandleCommand(aMessage, aWParam, aLParam);
				}
			break;
		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
			if (window)
				{
				UINT modifiers = EModifierAutorepeatable;
				if (aMessage == WM_SYSKEYDOWN)
					{
					modifiers |= (EModifierLeftAlt | EModifierAlt | EModifierLeftFunc | EModifierFunc);
					}
				switch (aWParam) 
					{ 
					case VK_LEFT:		return window->HandleChar(EKeyLeftArrow, modifiers);
					case VK_UP:			return window->HandleChar(EKeyUpArrow, modifiers);
					case VK_RIGHT:		return window->HandleChar(EKeyRightArrow, modifiers);
					case VK_DOWN:		return window->HandleChar(EKeyDownArrow, modifiers);
					case VK_HOME:		return window->HandleChar(EKeyHome, modifiers);
					case VK_END:		return window->HandleChar(EKeyEnd, modifiers);
					case VK_PRIOR:		return window->HandleChar(EKeyPageUp, modifiers);
					case VK_NEXT:		return window->HandleChar(EKeyPageDown, modifiers);
					case VK_PAUSE:		return window->HandleChar(EKeyPause, modifiers);
					case VK_PRINT:		return window->HandleChar(EKeyPrintScreen, modifiers);
					case VK_INSERT:		return window->HandleChar(EKeyInsert, modifiers);
					case VK_DELETE:		return window->HandleChar(EKeyDelete, modifiers);
					case VK_CAPITAL:	return window->HandleChar(EKeyCapsLock, modifiers);
					case VK_NUMLOCK:	return window->HandleChar(EKeyNumLock, modifiers);
					case VK_SCROLL:		return window->HandleChar(EKeyScrollLock, modifiers);
					case VK_F1:			return window->HandleChar(EKeyF1, modifiers);
					case VK_F2:			return window->HandleChar(EKeyF2, modifiers);
					case VK_F3:			return window->HandleChar(EKeyF3, modifiers);
					case VK_F4:			return window->HandleChar(EKeyF4, modifiers);
					case VK_F5:			return window->HandleChar(EKeyF5, modifiers);
					case VK_F6:			return window->HandleChar(EKeyF6, modifiers);
					case VK_F7:			return window->HandleChar(EKeyF7, modifiers);
					case VK_F8:			return window->HandleChar(EKeyF8, modifiers);
					case VK_F9:			return window->HandleChar(EKeyF9, modifiers);
					case VK_F10:		return window->HandleChar(EKeyF10, modifiers);
					case VK_F11:		return window->HandleChar(EKeyF11, modifiers);
					case VK_F12:		return window->HandleChar(EKeyF12, modifiers);
					case VK_F13:		return window->HandleChar(EKeyF13, modifiers);
					case VK_F14:		return window->HandleChar(EKeyF14, modifiers);
					case VK_F15:		return window->HandleChar(EKeyF15, modifiers);
					case VK_F16:		return window->HandleChar(EKeyF16, modifiers);
					case VK_F17:		return window->HandleChar(EKeyF17, modifiers);
					case VK_F18:		return window->HandleChar(EKeyF18, modifiers);
					case VK_F19:		return window->HandleChar(EKeyF19, modifiers);
					case VK_F20:		return window->HandleChar(EKeyF20, modifiers);
					case VK_F21:		return window->HandleChar(EKeyF21, modifiers);
					case VK_F22:		return window->HandleChar(EKeyF22, modifiers);
					case VK_F23:		return window->HandleChar(EKeyF23, modifiers);
					case VK_F24:		return window->HandleChar(EKeyF24, modifiers);
					default:			return 0;
					}
				}
			break;
		case WM_CHAR:
		case WM_SYSCHAR:
			if (window)
				{
				UINT modifiers = EModifierAutorepeatable;
				if (aMessage == WM_SYSCHAR)
					{
					modifiers |= (EModifierLeftAlt | EModifierAlt | EModifierLeftFunc | EModifierFunc);
					}
				return window->HandleChar(TCHAR(aWParam), modifiers);
				}
			break;
		case WM_SIZE:
			if (window)
				{
				window->HandleSizeChanged(LOWORD(aLParam), HIWORD(aLParam));
				}
			break;
		case WM_HSCROLL:
		case WM_VSCROLL:
			if (window)
				{
				window->HandleScrollEvent(aMessage, aWParam);
				}
			break;
		case 0x020A: // WM_MOUSEWHEEL
			if (window)
				{
				window->HandleScrollEvent(WM_VSCROLL, (short)HIWORD(aWParam) > 0 ? SB_LINEUP : SB_LINEDOWN);
				}
			break;
		case WM_PAINT:
			if (window)
				{
				window->Draw();
				}
			break;
		case WM_LBUTTONDOWN:
		case WM_LBUTTONDBLCLK:
		case WM_MOUSEMOVE:
		case WM_LBUTTONUP:
			if (window)
				{
				window->HandleMouseEvent(aMessage, aWParam, aLParam);
				}
			break;
		case WM_INITMENUPOPUP:
			if (window)
				{
				window->HandleMenuPopUp((HMENU)aWParam);
				}
			break;
		case WM_DESTROY:
			if (window && window->iObserver)
				{
				window->iObserver->HandleWindowClosure(*window);
				}
			break;
		default:
			return DefWindowProc(aHandle, aMessage, aWParam, aLParam);
		}
	return 0;
	}

CWindow* CWindow::Instance(HWND aHandle)
	{
	CWindow* window = sFirst;
	bool found(0);
	while (window && !found)
		{
		if (window->iHandle == aHandle)
			{
			found = 1;
			}
		else
			{
			window = window->iNext;
			}
		}

	if (found)
		{
		return window;
		}
	return NULL;
	}

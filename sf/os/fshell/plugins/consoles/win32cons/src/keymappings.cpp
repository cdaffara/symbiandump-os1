// keymappings.cpp
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


#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <e32keys.h>
#include <e32def.h>
#include "keymappings.h"


static const TKeyMapping KKeyMapping[] =
	{
	{ VK_ESCAPE, EKeyEscape },
	{ VK_LEFT, EKeyLeftArrow },
	{ VK_UP, EKeyUpArrow },
	{ VK_RIGHT, EKeyRightArrow },
	{ VK_DOWN, EKeyDownArrow },
	{ VK_HOME, EKeyHome },
	{ VK_END, EKeyEnd },
	{ VK_PRIOR, EKeyPageUp },
	{ VK_NEXT, EKeyPageDown },
	{ VK_PAUSE, EKeyPause },
	{ VK_PRINT, EKeyPrintScreen },
	{ VK_INSERT, EKeyInsert },
	{ VK_DELETE, EKeyDelete },
	{ VK_CAPITAL, EKeyCapsLock },
	{ VK_NUMLOCK, EKeyNumLock },
	{ VK_SCROLL, EKeyScrollLock },
	{ VK_F1, EKeyF1 },
	{ VK_F2, EKeyF2 },
	{ VK_F3, EKeyF3 },
	{ VK_F4, EKeyF4 },
	{ VK_F5, EKeyF5 },
	{ VK_F6, EKeyF6 },
	{ VK_F7, EKeyF7 },
	{ VK_F8, EKeyF8 },
	{ VK_F9, EKeyF9 },
	{ VK_F10, EKeyF10 },
	{ VK_F11, EKeyF11 },
	{ VK_F12, EKeyF12 },
	{ VK_F13, EKeyF13 },
	{ VK_F14, EKeyF14 },
	{ VK_F15, EKeyF15 },
	{ VK_F16, EKeyF16 },
	{ VK_F17, EKeyF17 },
	{ VK_F18, EKeyF18 },
	{ VK_F19, EKeyF19 },
	{ VK_F20, EKeyF20 },
	{ VK_F21, EKeyF21 },
	{ VK_F22, EKeyF22 },
	{ VK_F23, EKeyF23 },
	{ VK_F24, EKeyF24 },
	};
	
TKeyCode GetSymbianKeyCode(int aWindowsKeyCode)
	{
	for (int i=0; i<sizeof(KKeyMapping) / sizeof(TKeyMapping); ++i)
		{
		if (KKeyMapping[i].iWindowsKey == aWindowsKeyCode) return KKeyMapping[i].iSymbianKey;
		}
	return EKeyNull;
	}
	
TUint GetSymbianModifiers(TUint aWindowsModifiers)
	{
	TUint32 symMod = 0;
	if (aWindowsModifiers & CAPSLOCK_ON) symMod |= EModifierCapsLock;
	if (aWindowsModifiers & LEFT_ALT_PRESSED) symMod |= EModifierLeftAlt | EModifierAlt;
	if (aWindowsModifiers & LEFT_CTRL_PRESSED) symMod |= EModifierLeftCtrl | EModifierCtrl;
	if (aWindowsModifiers & NUMLOCK_ON) symMod |= EModifierNumLock;
	if (aWindowsModifiers & RIGHT_ALT_PRESSED) symMod |= EModifierRightAlt | EModifierAlt;
	if (aWindowsModifiers & RIGHT_CTRL_PRESSED) symMod |= EModifierLeftAlt | EModifierCtrl;
	if (aWindowsModifiers & SCROLLLOCK_ON) symMod |= EModifierScrollLock;
	if (aWindowsModifiers & SHIFT_PRESSED) symMod |= EModifierShift;
	return symMod;	
	}
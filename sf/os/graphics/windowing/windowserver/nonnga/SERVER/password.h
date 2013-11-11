// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// CWsPassword Class definitions
// 
//

#ifndef __PASSWORD_H__
#define __PASSWORD_H__

#include "server.h"

class CWsPassword
	{
private:
	enum {EPasswordMidnightOffset=4};	// Hours past midnight to trigger next days password
public:
	static void WindowDestroyed(CWsClientWindow *aWindow);
	static void SetPasswordWindowL(CWsClientWindow *aWindow, TPasswordMode aPasswordMode);
	static inline CWsClientWindow *PasswordWindow();
	static inline TBool PasswordModeActive();
	static void SwitchOn();
	static void PasswordEntered(CWsClient *aClient);
private:
	static void CancelPasswordWindow();
	static TInt Day();
private:
	static TBool iPasswordModeActive;
	static TPasswordMode iPasswordMode;
	static CWsClientWindow *iPasswordWindow;
	static TInt iPrevPasswordDay;
	};

inline TBool CWsPassword::PasswordModeActive()
	{
	return iPasswordModeActive;
	}
	
inline CWsClientWindow *CWsPassword::PasswordWindow()
	{
	return iPasswordWindow;
	}

#endif

// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#if !defined(__STARTUPSPLASHSCREEN_H__)
#define __STARTUPSPLASHSCREEN_H__

#include <e32base.h>
#include "StartupCommand.h"

/** Class to start and stop the splash screen. This command is designed to work
with the techview splash screen, which uses a global chunk to signal that it
should terminate.
@internalComponent
@deprecated
*/
class CStartupSplashScreen : public CBase, public MStartupCommand
	{
public:
	static CStartupSplashScreen* NewL(TBool aStart, HBufC* aPath = 0);
	static CStartupSplashScreen* NewLC(TBool aStart, HBufC* aPath = 0);
	~CStartupSplashScreen();
	
 	// from MStartupCommand
	void Execute(TRequestStatus& aStatus);
	void Release();
	
private:
	CStartupSplashScreen(TBool aStart, HBufC* aPath = 0);
	
	TInt StartSplashScreen();
	TInt KillSplashScreen();
	
private:
	TBool iStartSplash;
	HBufC* iSplashPath;
	};

#endif

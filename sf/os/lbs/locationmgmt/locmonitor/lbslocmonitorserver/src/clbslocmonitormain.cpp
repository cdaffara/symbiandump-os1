/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/

	
	
#include <e32base.h>
#include "csecureasbase.h"
#include "tserverstartparams.h"
#include "lbsdevloggermacros.h"

// E32Main of the location monitor
// as required by LBS's ServerFramework
TInt E32Main()
	{
	LBSLOG_BEGIN();
	LBSLOG(ELogP3, "Location Monitor Start()\n");
	TServerStartParams params;
	TPtr ptr(reinterpret_cast<TText*>(&params), 0, sizeof(TServerStartParams)/sizeof(TText16));
	User::CommandLine(ptr);
	TInt err = CSecureASBase::ServerMain(params);
	LBSLOG(ELogP3, "Location Monitor Stop()\n");
	return err;
	}

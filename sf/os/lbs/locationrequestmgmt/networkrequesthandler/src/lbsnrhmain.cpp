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
// Global entry point for the LBS Network Request Handler
// 
//

/**
 @file
 @internalTechnology
 @released
*/
#include <e32base.h>
#include <e32test.h>
// jcm #include "lbsnetworkrequesthandler.h"
#include "lbsdevloggermacros.h"
#include "csecureasbase.h"
#include "tserverstartparams.h"

TInt E32Main()
	{
	LBSLOG_BEGIN();
	TServerStartParams params;
	TPtr ptr(reinterpret_cast<TText*>(&params), 0, sizeof(TServerStartParams)/sizeof(TText16));
	User::CommandLine(ptr);
	TInt err = CSecureASBase::ServerMain(params);
	return err;
	}

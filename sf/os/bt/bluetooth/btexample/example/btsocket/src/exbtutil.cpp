// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name     : exbtutil.cpp
// Part of  : ex_btsocket
// Created  : 17/11/2004 by Shane Kearns
// Helper functions
// Version  :
// 
//

#include <flogger.h>
#include "exbtutil.h"

_LIT(KExBtPanicCat, "EX_BTSOCKET");
#ifdef _DEBUG
_LIT(KLogDir, "BT");
_LIT(KLogFile, "EX_BTSOCKET.txt");
#endif // _DEBUG
void ExBtSocket::Panic(TExBtPanic aReason)
{
	User::Panic(KExBtPanicCat, aReason);
}

#ifdef _DEBUG
void ExBtSocket::Log(const TRefByValue<const TDesC> aFmt, ...)
	{
	VA_LIST list;
	VA_START(list, aFmt);
	RFileLogger::WriteFormat(KLogDir, KLogFile, EFileLoggingModeAppend, aFmt, list);
	}
#endif


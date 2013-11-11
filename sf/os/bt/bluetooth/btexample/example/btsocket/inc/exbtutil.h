/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name     : exbtutil.h
* Part of  : ex_btsocket
* Created  : 17/11/2004 by Shane Kearns
* Helper functions
* Version  :
* 
*
*/



#ifndef EXBTUTIL_H
#define EXBTUTIL_H

#include <e32std.h>


/**
A static utility class for debugging purposes
*/
class ExBtSocket
	{
	public:
	enum TExBtPanic
		{
		EBadCallback,
		EReEntrant,
		EUnsupportedProtocol
		};
	static void Panic(TExBtPanic aPanic);
	static void Log(const TRefByValue<const TDesC> aFormat, ...);
	};
	
#ifdef _DEBUG
#define ENABLE_LOG_INFO
#define ENABLE_LOG_FN_TRACE
#define ENABLE_LOG_ERROR
#define USE_FLOGGER
#endif

#ifdef ENABLE_LOG_INFO
#ifdef USE_FLOGGER
#define LOG_INFO(X) ExBtSocket::Log X
#else
#define LOG_INFO(X) RDebug::Print X
#endif
#else
#define LOG_INFO(X)
#endif

#ifdef ENABLE_LOG_ERROR
#ifdef USE_FLOGGER
#define LOG_ERROR(X) ExBtSocket::Log X
#else
#define LOG_ERROR(X) RDebug::Print X
#endif
#else
#define LOG_ERROR(X)
#endif

#ifdef ENABLE_LOG_FN_TRACE
#ifdef USE_FLOGGER
#define LOG_FN_TRACE(X) ExBtSocket::Log X
#else
#define LOG_FN_TRACE(X) RDebug::Print X
#endif
#else
#define LOG_FN_TRACE(X)
#endif

#endif // EXBTUTIL_H

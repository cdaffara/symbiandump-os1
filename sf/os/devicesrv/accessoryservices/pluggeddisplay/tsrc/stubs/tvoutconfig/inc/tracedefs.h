/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description: Definitions for tracing. 2nd generation.
 *
 */

#ifndef TRACEDEFS_H
#define TRACEDEFS_H


//-----------------------------------------------------------------------------
// Constants
//-----------------------------------------------------------------------------
//

/**
* Trace prefixes for macros with component name.
*/
#define _PREFIX_TRACE( a ) TPtrC( (const TText*) L"[PluggedDisplayAsyTest]: " L##a )
#define _PREFIX_TRACE_2( a, b ) TPtrC( (const TText*) L"[PluggedDisplayAsyTest]: " L##a L##b )
#define _PREFIX_TRACE8( a ) (const char*)( "[PluggedDisplayAsyTest] " ##a )

/**
* Files for traces and event log
*/
#define TRACE_FILE "testframework\\trace\\pluggeddisplayasytest_trace.txt"
#define LOG_FILE "testframework\\log\\pluggeddisplayasytest_log.txt"

#endif // TRACEDEFS_H

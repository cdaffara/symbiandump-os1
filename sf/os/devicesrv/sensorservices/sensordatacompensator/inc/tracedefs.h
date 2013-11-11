/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Definitions for tracing.
*
*/


#ifndef TRACEDEFS_H
#define TRACEDEFS_H

//-----------------------------------------------------------------------------
// Constants
//-----------------------------------------------------------------------------
//

/**
* Prefix trace macro to complete tracing with component name.
* Returns TDesC which can be used directly with RDebug or RFileLogger.
*/
#define _PREFIX_TRACE( aMsg ) TPtrC( (const TText*)L"[SDCOMP]: " L##aMsg )
#define _PREFIX_TRACE_2( aMsg1, aMsg2 ) TPtrC( (const TText*)L"[SDCOMP]: " L##aMsg1 L##aMsg2 )

/**
* Prefix macro for strings
*/
#define _PREFIX_CHAR( aMsg ) (const char*)"[SDCOMP]: " ##aMsg

#endif // TRACEDEFS_H

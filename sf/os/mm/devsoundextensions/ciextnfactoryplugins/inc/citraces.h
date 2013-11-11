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
* Description:   Trace macro definitions for CI framework plugins.
*
*/



#ifndef CITRACES_H
#define CITRACES_H

#include <e32debug.h>

#ifdef _DEBUG
    #define DEB_TRACE0(str)             RDebug::Print(str)
    #define DEB_TRACE1(str, val1)       RDebug::Print(str, val1)
#else
    #define DEB_TRACE0(str)
    #define DEB_TRACE1(str, val1)
#endif // _DEBUG

#endif // CITRACES_H

// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @internalComponent
*/
   
#ifndef __RDEBUGMACROS_H__
#define __RDEBUGMACROS_H__

#include <e32debug.h>


//logging macros//
#ifdef USE_RDEBUGLOGGING

#define RDEBUGPRINTLOGGER1(_t)   (RDebug::Print(_t))     
#define RDEBUGPRINTLOGGER2(_t, _a)   (RDebug::Print(_t, _a))     
#define RDEBUGPRINTLOGGER3(_t, _a, _b)   (RDebug::Print(_t, _a, _b))     
				
#else 
//do nothing
#define RDEBUGPRINTLOGGER1(_t)
#define RDEBUGPRINTLOGGER2(_t, _a)
#define RDEBUGPRINTLOGGER3(_t, _a, _b)

#endif  // USE_RDEBUGLOGGING

#endif // __RDEBUGMACROS_H__

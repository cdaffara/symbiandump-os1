// btdebug.h
// 
// Copyright (c) 2008 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#ifndef __BTDEBUG_H__
#define __BTDEBUG_H__

//#define _DEBUG_BT

#define EXPAND(L, X) L(X)
#define PANIC() User::Panic(EXPAND(_L, __FILE__), __LINE__)
#define CHECK(fn) {TInt _err = (fn); if (_err!=KErrNone) return _err; }

#ifdef _DEBUG_BT
	#include <e32debug.h>
	#define TRACE1(x) RDebug::Printf(x)
	#define TRACE2(x, y) RDebug::Printf(x, y)
	#define TRACE3(x, y, z) RDebug::Printf(x, y, z)
	#define _BTDEB(x) x
#else
	#define TRACE1(x)
	#define TRACE2(x, y)
	#define TRACE3(x, y, z)
	#define _BTDEB(x)
#endif

#endif //__BTDEBUG_H__

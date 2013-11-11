// Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
//


#ifndef MEDIACLIENTVIDEOTRACE_H_
#define MEDIACLIENTVIDEOTRACE_H_

#ifdef _DEBUG

#include <e32debug.h>

#define DEBUG_PRINTF(a) RDebug::Printf(a)
#define DEBUG_PRINTF2(a,b) RDebug::Printf(a,b)
#define DEBUG_PRINTF3(a,b,c) RDebug::Printf(a,b,c)
#define DEBUG_PRINTF4(a,b,c,d) RDebug::Printf(a,b,c,d)
#define DEBUG_PRINTF5(a, b, c, d, e) RDebug::Printf(a,b,c,d,e)

#else // _DEBUG

#define DEBUG_PRINTF(a)
#define DEBUG_PRINTF2(a, b)
#define DEBUG_PRINTF3(a, b, c)
#define DEBUG_PRINTF4(a, b, c, d)
#define DEBUG_PRINTF5(a, b, c, d, e)

#endif // _DEBUG

#endif // MEDIACLIENTVIDEOTRACE_H_

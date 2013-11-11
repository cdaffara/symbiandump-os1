// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef	__UTILS_H__
#define	__UTILS_H__

#include <gdi.h>
#include <pixelformats.h>

GLDEF_C TDisplayMode ParseDisplayMode(const TDesC& aModeName);

// Macros to highlight usage of deprecated render stage APIs.
// The description (X) should be small because panic descriptions are limited to 16 chars 

#ifdef RS_DEPRECATED_PANIC
#ifndef RS_DEPRECATED_LOG
#define RS_DEPRECATED_LOG
#endif
#endif

#ifdef RS_DEPRECATED_PANIC
#define RS_REPORT_DEPRECATED(X)		RDebug::Printf("Deprecated render stage API:" X), User::Panic(_L("RS:" X), __LINE__)
#define RS_REPORT_DEPRECATED_SET(X)	RDebug::Printf("Deprecated render stage API - SET:" X), User::Panic(_L("RS:" X), __LINE__)
#define RS_REPORT_DEPRECATED_GET(X)	RDebug::Printf("Deprecated render stage API - GET:" X), User::Panic(_L("RS:" X), __LINE__)
#else	/* RS_DEPRECATED_PANIC */
#ifdef RS_DEPRECATED_LOG
#define RS_REPORT_DEPRECATED(X)		RDebug::Printf("Deprecated render stage API:" X)
#define RS_REPORT_DEPRECATED_SET(X)	RDebug::Printf("Deprecated render stage API - SET:" X)
#define RS_REPORT_DEPRECATED_GET(X)	RDebug::Printf("Deprecated render stage API - GET:" X)
#else	/* RS_DEPRECATED_LOG */
#define RS_REPORT_DEPRECATED(X)
#define RS_REPORT_DEPRECATED_SET(X)
#define RS_REPORT_DEPRECATED_GET(X)
#endif	/* RS_DEPRECATED_LOG */
#endif	/* RS_DEPRECATED_PANIC */

#endif //__UTILS_H__

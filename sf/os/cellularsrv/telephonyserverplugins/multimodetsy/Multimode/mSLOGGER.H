// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Event Logger for MultiMode TSY
// 
//

/**
 @file
 @internalAll 
*/

#include <e32std.h>	// for basic Symbian data types

#ifndef __MSLOGGER_H__
#define __MSLOGGER_H__

#ifdef _DEBUG
#define __LOGDEB__
#else
#define __LOGREL__
#endif

#ifdef	__LOGDEB__
#define __LOGREL__
#endif

#ifdef __LOGDEB__
#define LOGTEXT(AAA)				TSYLogger::Write(AAA)
#else
#define LOGTEXT(AAA)
#endif

#ifdef __LOGDEB__
#define LOGTEXT2(AAA,BBB)			TSYLogger::WriteFormat(AAA,BBB)
#else
#define LOGTEXT2(AAA,BBB)
#endif

#ifdef __LOGDEB__
#define LOGTEXT3(AAA,BBB,CCC)		TSYLogger::WriteFormat(AAA,BBB,CCC)
#else
#define LOGTEXT3(AAA,BBB,CCC)
#endif

#ifdef	__LOGREL__
#define LOGTEXTREL(AAA)				TSYLogger::Write(AAA)
#else
#define LOGTEXTREL(AAA)				
#endif

#ifdef	__LOGREL__
#define LOGTEXTREL2(AAA,BBB)		TSYLogger::WriteFormat(AAA,BBB)
#else
#define LOGTEXTREL2(AAA,BBB)		
#endif

#ifdef __LOGREL__
#define LOGTEXTREL3(AAA,BBB,CCC)	TSYLogger::WriteFormat(AAA,BBB,CCC)
#else
#define LOGTEXTREL3(AAA,BBB,CCC)	
#endif

// TSYLogger
// Implements logging.  All static calls.

class TSYLogger
	{
public:
	static void Write(const TDesC8& aText);
	static void WriteFormat(TRefByValue<const TDesC8> aFmt,...);
	};

#endif	// __MSLOGGER_H__

// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __ES_FLOG_H__
#define __ES_FLOG_H__


#include <comms-infras/commsdebugutility.h>

#if defined (__FLOG_ACTIVE)

#define LOG(a) a

#if defined(DETAILED_LOG)

	#define LOG_DETAILED(a) a
#else
	#define LOG_DETAILED(a)
#endif

//_LIT(KLogDir, "esock"); //< KLogDir holds name of Log directory
//_LIT(KLogFile, "esock.txt"); //< KLogFile holds the name of Log file.

class ESockLogExternal
/**
@internalComponent
*/
	{
public:
	static void Printf(const TDesC8& aComponent, const TDesC8& aSubTag, TRefByValue<const TDesC8> aFmt, VA_LIST& aList);
	};

class ESockLog : public ESockLogExternal
/**
@internalComponent
*/
	{
public:
	static void Printf(TRefByValue<const TDesC8> aFmt, ...);
	static void Printf(TRefByValue<const TDesC> aFmt, ...);
	};

#else

#define LOG(a)
#define LOG_DETAILED(a)

#endif // __FLOG_ACTIVE

#endif // __ES_FLOG_H__


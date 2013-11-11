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
// eintsocklog.h
// 
//

/**
 @file
 @internalComponent 
*/


#ifndef __EINTSOCKLOG_H__
#define __EINTSOCKLOG_H__

#include <comms-infras/commsdebugutility.h>

#ifdef __FLOG_ACTIVE // define everything we need for logging

#define LOG(x) x

class Log
	{
public:
	static void Printf(TRefByValue<const TDesC> aFmt, ...);
	};
	
#else

	#define LOG(x)
	
#endif // __FLOG_ACTIVE

#endif // __EINTSOCKLOG_H__


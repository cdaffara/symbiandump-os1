// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// eintsocklog.cpp
// 
//

/**
 @file
 @internalComponent 
*/

#include "ss_eintsocklog.h"

#ifdef __FLOG_ACTIVE

__FLOG_STMT(_LIT(KEIntSockLogFolder, "esock"));
__FLOG_STMT(_LIT(KEIntSockLogFile, "eintsock"));

void Log::Printf(TRefByValue<const TDesC> aFmt,...)
/**
Write a multiple argument list to the log, trapping and ignoring any leave
*/
	{
	VA_LIST list;
	VA_START(list,aFmt);
	RFileLogger::WriteFormat(KEIntSockLogFolder(),KEIntSockLogFile(),EFileLoggingModeAppend,aFmt,list);
	}
	
#endif // __FLOG_ACTIVE


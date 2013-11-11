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
//

/**
 @file
 @internalComponent
*/


#include <e32std.h>
#include "es_flog.h"
#include <comms-infras/commsdebugutility.h>

#if defined (__FLOG_ACTIVE)

/**
@internalComponent
*/
_LIT(KESockLogFolder, "esock");

/**
@internalComponent
*/
_LIT(KESockLogFile, "esock.txt");

void ESockLogExternal::Printf(const TDesC8& /*aComponent*/, const TDesC8& /*aSubTag*/, TRefByValue<const TDesC8> aFmt, VA_LIST& aList)
/**
Write a multiple argument list to the log, trapping and ignoring any leave

*/
	{
	
	// Format the text.
	TBuf8<200> buf8;
	buf8.FormatList(aFmt, aList);
	
	// Convert it to Unicode.
	TBuf16<200> buf16;
	buf16.Copy(buf8);
	
	RFileLogger::WriteFormat(KESockLogFolder, KESockLogFile, EFileLoggingModeAppend, buf16);
	}

void ESockLog::Printf(TRefByValue<const TDesC> aFmt,...)
/**
Write a multiple argument list to the log, trapping and ignoring any leave

*/
	{

	VA_LIST list;
	VA_START(list,aFmt);
	RFileLogger::WriteFormat(KESockLogFolder(), KESockLogFile(), EFileLoggingModeAppend, aFmt, list);
	}

void ESockLog::Printf(TRefByValue<const TDesC8> aFmt,...)
/**
Write a multiple argument list to the log, trapping and ignoring any leave

*/
	{

	VA_LIST list;
	VA_START(list,aFmt);
	RFileLogger::WriteFormat(KESockLogFolder(), KESockLogFile(), EFileLoggingModeAppend, aFmt, list);
	}

#endif

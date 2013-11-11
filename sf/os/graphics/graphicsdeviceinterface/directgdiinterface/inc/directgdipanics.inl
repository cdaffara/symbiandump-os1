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
// General inline utility and logging functions for use with DirectGDI.
// 
//

/**
 @file
 @publishedPartner
 @prototype
*/

#ifndef DIRECTGDIPANICS_INL
#define DIRECTGDIPANICS_INL

#include <e32std.h>
#include <e32debug.h>

_LIT(KDGDILogFormat, "File %S +%i: %S");
_LIT(KDGDINonConditionalPanicFormat, "Panic: %S %i.");
_LIT(KDGDIConditionalPanicFormat, "Panic: %S %i, assert \"%S\" failed.");

namespace DirectGdi
	{

/**
Helper function used by the GRAPHICS_LOG_ALWAYS and GRAPHICS_LOGD_ALWAYS macros.
 */
inline void Log(const TDesC& aFileName, TInt aLine, const TDesC& aLogMessage)
	{
	TBuf<256> buf;
	buf.Format(KDGDILogFormat, &aFileName, aLine, &aLogMessage);
	RDebug::Print(buf);
	}

/**
Helper function used by GRAPHICS_ASSERT_ALWAYS and GRAPHICS_PANIC_ALWAYS macros
which allows a panic category and code to be logged.
 */
inline void PanicWithDebugLog (
		const TDesC& aPanicCategory, 
		TInt aPanicCode,
		const TDesC& aFileName,
		TInt aLine,
		const TDesC* aCondition)
	{
	TBuf<256> message;

	if (aCondition)
		{
		message.Format(KDGDIConditionalPanicFormat, &aPanicCategory, aPanicCode, aCondition);
		}
	else
		{
		message.Format(KDGDINonConditionalPanicFormat, &aPanicCategory, aPanicCode);
		}
	
	Log(aFileName, aLine, message);

	User::Panic(aPanicCategory, aPanicCode);
	}
	}

#endif // DIRECTGDIPANICS_INL



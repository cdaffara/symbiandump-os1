// Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Logging macros for Tsy. These use the Comms Debug / Flogger V2
// Framework.
//



/**
 @file
 @internalComponent
*/

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "ltsyloggerTraces.h"
#endif

#include <ctsy/ltsy/ltsylogger.h>
//#include <e32debug.h>

#if (OST_TRACE_CATEGORY & OST_TRACE_CATEGORY_DEBUG)

const TInt KLineLength = 255;

EXPORT_C TLogEntryExit::TLogEntryExit(const TDesC8& aFnName, const TDesC8& aLayer)
/**
Constructor: log the function name entry
*/
	:iFnName(aFnName), iLayer(aLayer), iErr(KErrNone)
	{
    OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, TLOGENTRYEXIT_TLOGENTRYEXIT_1, "%s", iFnName);
	};

EXPORT_C TLogEntryExit::TLogEntryExit(const TDesC8& aFnName, const TDesC8& aLayer, TRefByValue<const TDesC8> aFmt, ...)
/**
Write the function name entry plus plus 8 bits formated list
*/
	:iFnName(aFnName), iLayer(aLayer), iErr(KErrNone)
	{
	/* coverity[var_decl] */ VA_LIST list;
	VA_START(list,aFmt);
	TBuf8<KLineLength> line;
	line.Append(_L8(">>"));
	line.Append(iFnName);
	line.Append(' ');
	line.Append(aFmt);
	TBuf8<KLineLength> evaluatedLine;
	/* coverity[uninit_use_in_call] */ evaluatedLine.FormatList(line,list);
	OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, TLOGENTRYEXIT_TLOGENTRYEXIT1_1, "%s", evaluatedLine);
	};

EXPORT_C TLogEntryExit::TLogEntryExit(const TDesC8& aFnName, const TDesC8& aLayer, TRefByValue<const TDesC16> aFmt, ...)
/**
Write the function name entry plus 16 bits formated list
*/
	:iFnName(aFnName), iLayer(aLayer), iErr(KErrNone)
	{
	/* coverity[var_decl] */ VA_LIST list;
	VA_START(list,aFmt);

	TBuf<KLineLength> line;
	line.Copy(iFnName);
	line.Insert(0,_L(">>"));
	line.Append(' ');
	line.Append(aFmt);
	TBuf<KLineLength> evaluatedLine;
	/* coverity[uninit_use_in_call] */ evaluatedLine.FormatList(line,list);
	OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, TLOGENTRYEXIT_TLOGENTRYEXIT2_1, "%S", evaluatedLine);
	};
			
EXPORT_C TLogEntryExit::~TLogEntryExit()
/**
Write the function name exit
*/
	{
	if (iErr == KErrNone)
		{
        OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, TLOGENTRYEXIT_TLOGENTRYEXIT_DTOR_1, "%s",iFnName);
		}
	else
		{
        OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, TLOGENTRYEXIT_TLOGENTRYEXIT_DTOR_2, "%s [err=%d]",iFnName, iErr);
		}
	};

#else // #ifdef _DEBUG

// stub export for urel builds
EXPORT_C TLogEntryExit::TLogEntryExit(const TDesC8& /*aFnName*/, const TDesC8& /*aLayer*/){};
EXPORT_C TLogEntryExit::TLogEntryExit(const TDesC8& /*aFnName*/, const TDesC8& /*aLayer*/, TRefByValue<const TDesC8> /*aFmt*/, ...){};
EXPORT_C TLogEntryExit::TLogEntryExit(const TDesC8& /*aFnName*/, const TDesC8& /*aLayer*/, TRefByValue<const TDesC16> /*aFmt*/, ...){};
EXPORT_C TLogEntryExit::~TLogEntryExit() {};

#endif // #ifdef OST_TRACE_COMPILER_IN_USE

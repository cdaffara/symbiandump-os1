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

#include "TE_TelWatchersUnitWatcherLog.h"

/**
 *  @file
 *  Telephony Watchers Unit Test Watcher logging.
 */

/* 
   Class used for logging in telephony watcher unit tests - normally watchers will link
   with watcher.lib and use the watcher implementation of CWatcherLog, but under unit
   test conditions, we want the extra functionality provided by this CWatcherLog, so link
   to this file
*/

//
// Switch this define on for extra output!
//
//#define WATCHERS_VERBOSE_OUTPUT 

CWatcherLog::CWatcherLog(CTestExecuteLogger& aTestExecuteLogger)
  : iTestExecuteLogger(aTestExecuteLogger)
	{
	// NOP
	} // CWatcherLog::CWatcherLog


/**
 *  Print to the Test Execute log...
 *
 *  @param aFmt  A format string.
 *  @param ...   An optional list of parameters.
 */
void CWatcherLog::PrintToTestExecuteLogL(TRefByValue<const TDesC> aFmt, VA_LIST aParamList)
	{
	//
	// Convert the text to a single string.  We do this regardless of whether
	// we need to, so that the EABI warning about unused parameters does not
	// occur.
	//
	HBufC*  buffer = HBufC::NewLC(KMaxTestExecuteLogLineLength);
	TPtr  ptr(buffer->Des());

	ptr.AppendFormatList(aFmt, aParamList);

#ifdef WATCHERS_VERBOSE_OUTPUT
	//
	// If this string is of the form "<Class> : <Some Text>" then use the Test
	// Execute format, otherwise write it all...
	//
	_LIT(KWatcherLogSplitText, " : ");
	TInt  offset = ptr.Find(KWatcherLogSplitText);

	if (offset >= 0)
		{
		HBufC8*  className = HBufC8::NewLC(buffer->Size());
		HBufC*  textString = HBufC::NewLC(buffer->Size());

		_LIT8(KNullChar,"\000");
		className->Des().Append(ptr.Left(offset));
		className->Des().Append(KNullChar);

		textString->Des().Append(ptr.Mid(offset + TDesC(KWatcherLogSplitText).Length()));

		iTestExecuteLogger.LogExtra(&((*className)[0]), 0, ESevrInfo, 
									  _L("%S"), textString);
		
		CleanupStack::PopAndDestroy(textString);
		CleanupStack::PopAndDestroy(className);
		}
	else
		{
		iTestExecuteLogger.LogExtra(((TText8*) "CWatcherLog"), 0, ESevrInfo,
									  _L("%S"), buffer);
		}
#endif

	CleanupStack::PopAndDestroy(buffer);
	} // CWatcherLog::PrintToTestExecuteLogL


/**
 *  Convert the log request into a Test Execute logging request.
 *
 *  @param aFmt  A format string.
 *  @param ...   An optional list of parameters.
 */
void CWatcherLog::Printf(TRefByValue<const TDesC> aFmt, ...)
	{
	//
	// Call the PrintToTestExecuteLogL() procedure which may leave.  There's
	// not much that can be done if the function leaves.
	//
	VA_LIST  list;
	VA_START(list, aFmt);
	TRAP_IGNORE(PrintToTestExecuteLogL(aFmt, list));
	VA_END(list);
	} // CWatcherLog::Printf

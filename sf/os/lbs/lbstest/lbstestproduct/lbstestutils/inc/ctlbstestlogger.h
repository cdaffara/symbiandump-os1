// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef LBS_TEST_LOGGER_H
#define LBS_TEST_LOGGER_H

//************************************************************************************************************
#include <e32base.h>

//-------------------------------------------------------------------------------	
/** Enum for log txt priority
Priorities 0 - 2 are for logging component internal events.
Priority 3 is for logging major component events that may have a major influence on other components.
Priorities 3 - 4 are for logging lbs system level events.
@internalTechnology
@released
*/
enum TLogPriority
	{
	/** The lowest priority. Use for hex dump and other low priority information. */
	ELogP0 = 0,
	/** Use for method enter/exit logging. */
	ELogP1 = 1,
	/** Use for low priority, component internal event and data logging. */
	ELogP2 = 2,
	/** Use for important component events like timeouts, warnings and errors. */
	ELogP3 = 3,
	/** Use for IPC only. */
	ELogP4 = 4,
	/** The highest priority. Use only for really major user, network or hardware events.*/
	ELogP5 = 5
	};
	
/** Log entry type.
@internalTechnology
@released
*/
enum TLogType
	{
	ELogNormal  = 0,
	ELogWarning = 1,
	ELogError 	= 2
	};
	
//-------------------------------------------------------------------------------	
/** Logger for development and debug purpose
The log format will be

Time			File name	Process Id	Log Priority			Log text
[08:51:00.997500] [File Name] 	[Process Id] 	[Log Text Priority] 	Any Log text

note the maximum buffer size limitation KLogBufferSize=150

[File Name] see RProcess::FileName()
[Process Id]see RProcess::Id() 
[Log Text Priority]  - [P0]- [P5]

@internalTechnology
@released
*/
class LbsTestLogger
	{
public:
	
	static void Write(TLogType aType, TLogPriority aPrior, TRefByValue<const TDesC16> aFmt, ...);
	static void Write(TLogType aType, TLogPriority aPrior, TRefByValue<const TDesC8> aFmt, ...);
	
	static void HexDump(TLogPriority aPrior, const TUint8 *aPtr, TInt aLen);
	
private:
	static void CreateLogDir();
	static void CreateLogTxt(TLogType aType, TLogPriority aPrior, TDes16& aBuf);
	static void CreateLogTxt(TLogType aType, TLogPriority aPrior, TDes8& aBuf);
	}; 

#endif //LBS_TEST_LOGGER_H

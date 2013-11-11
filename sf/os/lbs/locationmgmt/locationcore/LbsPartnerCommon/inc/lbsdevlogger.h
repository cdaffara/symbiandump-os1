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

/**
@file
@internalTechnology
*/


#ifndef LBS_DEV_LOGGER_H
#define LBS_DEV_LOGGER_H

//************************************************************************************************************
#include <e32base.h>

// Literals used
_LIT(KLbsDevLogger, "LbsDevLogger");

//-------------------------------------------------------------------------------	
/** Enum for log txt priority
Priorities 0 - 2 are for logging component internal events.
Priority 3 is for logging major component events that may have a major influence on other components.
Priorities 4 - 5 are currently unassigned (TBD)
Priorities 6 - 7 are for logging internal API
Priorities 8 - 9 are for logging public API
@internalTechnology
@released
*/
enum TLbsLogPriority
	{
	/** The lowest priority. Use for hex dump and other low priority information. */
	ELogP0 = 0,
	/** Use for method enter/exit logging. */
	ELogP1 = 1,
	/** Use for low priority, component internal event and data logging. */
	ELogP2 = 2,
	/** Use for important component events like timeouts, warnings and errors. */
	ELogP3 = 3,
	/** TBD (May temporarily contain old values that should be P7)*/
	ELogP4 = 4,
	/** TBD (May temporarily contain old entries that should be P9)*/
	ELogP5 = 5,
	/** Use for Internal API */
	ELogP6 = 6,	//Minor Values
	ELogP7 = 7,	//Major Values
	/** The highest priority. Use only for really major user, network or hardware events and Published API
	values. For example:
	   - MTLR/X3P/Self locate request/response/cancel
	   - location update from GPS 
	   - change in configuration
	   - privacy response from user
	   - Sequence Diagram log entries*/
	ELogP8 = 8,	//Minor Values
	ELogP9 = 9	//Major Values
	};
	
/** Log entry type.
@internalTechnology
@released
*/
enum TLbsLogType
	{
	ELogNormal  = 0,
	ELogWarning = 1,
	ELogError 	= 2
	};
	
//-------------------------------------------------------------------------------	
/** Logger for development and debug purpose
The log format will be

Time | ProcessName | Process Id | Log Priority | StackPointer | Log text
08:51:00.997500 | File Name | Process Id | Log Text Priority | StackPointer | Any Log text

note the maximum buffer size limitation KLogBufferSize=150

[File Name] see RProcess::FileName()
[Process Id]see RProcess::Id() 
[Log Text Priority]  - [P0]- [P9]

@internalTechnology
@released
*/
class LbsDevLogger
	{
public:
	IMPORT_C static void OverWrite(const TDesC8& aFmt);
	IMPORT_C static void OverWrite(const TDesC16& aFmt);
	
	IMPORT_C static void Write(TLbsLogType aType, TLbsLogPriority aPrior, TRefByValue<const TDesC16> aFmt, ...);
	IMPORT_C static void Write(TLbsLogType aType, TLbsLogPriority aPrior, TRefByValue<const TDesC8> aFmt, ...);

	IMPORT_C static void HexDump(TLbsLogPriority aPrior, const TUint8 *aPtr, TInt aLen);
	
	IMPORT_C static void GetMessageProcessName(const RMessage2& aMessage, TFileName& aName);
private:
	static void CreateLogTxt(TLbsLogType aType, TLbsLogPriority aPrior, TInt* aStackPtr, TDes16& aBuf);
	static void CreateLogTxt(TLbsLogType aType, TLbsLogPriority aPrior, TInt* aStackPtr, TDes8& aBuf);

	static void OverWriteL(const TDesC8& aFmt);
	static void OverWriteL(const TDesC16& aFmt);
		
	static void WriteL(TLbsLogType aType, TLbsLogPriority aPrior, TRefByValue<const TDesC16> aFmt, ...);
	static void WriteL(TLbsLogType aType, TLbsLogPriority aPrior, TRefByValue<const TDesC8> aFmt, ...);
	
	static void WriteL(TLbsLogType aType, TLbsLogPriority aPrior, TRefByValue<const TDesC16> aFmt,VA_LIST list);
	static void WriteL(TLbsLogType aType, TLbsLogPriority aPrior, TRefByValue<const TDesC8> aFmt, VA_LIST list);
	static void WriteListL(TLbsLogType aType, TLbsLogPriority aPrior, TRefByValue<const TDesC8> aFmt, VA_LIST list);

	static void HexDumpL(TLbsLogPriority aPrior, const TUint8 *aPtr, TInt aLen);
	}; 

#endif //LBS_DEV_LOGGER_H

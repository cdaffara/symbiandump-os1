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
 @test
*/
#ifndef LBS_TEST_LOGGER_H
#define LBS_TEST_LOGGER_H

//************************************************************************************************************
#include <e32base.h>

// Literals used
_LIT(KLbsTestLogger, "LbsTestLogger");

//-------------------------------------------------------------------------------	
/** Enum for log txt priority
Priorities 0 - 2 are for logging component internal events.
Priority 3 is for logging major component events that may have a major influence on other components.
Priorities 3 - 4 are for logging lbs system level events.
@internalTechnology
@test
*/
enum TLbsTestLogPriority
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
	/** The highest priority. Use only for really major user, network or hardware events.
	For example:
	   - MTLR/X3P/Self locate request/response/cancel
	   - location update from GPS 
	   - change in configuration
	   - privacy response from user*/
	ELogP5 = 5
	};
	
/** Log entry type.
@internalTechnology
@test
*/
enum TLbsTestLogType
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
@test
*/
class LbsTestLogger
	{
public:
	IMPORT_C static void Initialize();
	IMPORT_C static void OverWrite(const TDesC8& aFmt);
	IMPORT_C static void OverWrite(const TDesC16& aFmt);
	
	IMPORT_C static void Write(TLbsTestLogType aType, TLbsTestLogPriority aPrior, TRefByValue<const TDesC16> aFmt, ...);
	IMPORT_C static void Write(TLbsTestLogType aType, TLbsTestLogPriority aPrior, TRefByValue<const TDesC8> aFmt, ...);
	
	IMPORT_C static void HexDump(TLbsTestLogPriority aPrior, const TUint8 *aPtr, TInt aLen);
	
private:
	static void CreateLogTxt(TLbsTestLogType aType, TLbsTestLogPriority aPrior, TInt* aStackPtr, TDes16& aBuf);
	static void CreateLogTxt(TLbsTestLogType aType, TLbsTestLogPriority aPrior, TInt* aStackPtr, TDes8& aBuf);
	}; 



#endif //LBS_DEV_LOGGER_H

// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef SYMBIAN_DEN_LOG_H
#define SYMBIAN_DEN_LOG_H

// Enable logging for debug builds or builds with the comms flogger enabled - must be defined before including e32utrace.h
#if (defined(_DEBUG) || defined(__FLOG_ACTIVE)) && !defined(SYMBIAN_TRACE_ENABLE)
#define SYMBIAN_TRACE_ENABLE
#endif

#include <e32utrace.h>
#include <e32des8.h>

namespace Den
{

const TInt KMaxLogTextLength = 250;

class TLogTextBuf : public TBuf8<KMaxLogTextLength>
/**
@internalComponent
*/
    {
public:
    class TLogIgnoreOverflow8 : public TDes8Overflow
        {
        public:
            void Overflow(TDes8& /*aDes*/) { }
        };

    void AppendFormatIgnoreOverflow(TRefByValue<const TDesC8> aFmt, ...)
        {
    	TLogIgnoreOverflow8 overflowHandler;
    	VA_LIST list;
    	VA_START(list, aFmt);
    	AppendFormatList(aFmt, list, &overflowHandler);
    	VA_END(list);
        }

    void AppendFormatIgnoreOverflow(TRefByValue<const TDesC8> aFmt, VA_LIST& aList)
        {
    	TLogIgnoreOverflow8 overflowHandler;
    	AppendFormatList(aFmt, aList, &overflowHandler);
        }
    };

class Logging
/**
@internalComponent
*/
    {
public:
    // These are used in the btrace/utrace log output as the SubCategory/Schema
    enum TLogEntryType
        {
        ELogBinary = KBinary,   // Binary dump = 0
        ELogInfo = KText,       // Text output = 1
        ELogBlockStart,         // = 2
        ELogBlockEnd           // = 3
        };

private:
    enum
        {
        KPrimaryFilter = 194
        };

public:
    // General logging statememts
	static IMPORT_C void Printf(TInt aWorkerId, const TDesC8& aSubTag, TRefByValue<const TDesC8> aFmt, ...);
	static IMPORT_C void Printf(TInt aWorkerId, const TDesC8& aSubTag, TLogEntryType aType, TRefByValue<const TDesC8> aFmt, ...);
	static IMPORT_C void Printf(TInt aWorkerId, const TDesC8& aSubTag, TLogEntryType aType, TRefByValue<const TDesC8> aFmt, VA_LIST& aList);
	static const TText8* IPCMessName(TInt aMess);
	static void IPCMessName(TInt aMessNum, TDes8& aMessBuf);
	};

#ifdef SYMBIAN_TRACE_ENABLE

/**
@internalComponent
*/

// Minor levels of log tagging
_LIT8(KECommonClientTag, "Client");			// the common client-side case
_LIT8(KECommonServerTag, "Server");			// the common server-side case
_LIT8(KECommonBootingTag, "Booting");		// grim & verbose details of booting & shutting down
_LIT8(KECommonSessDetailTag, "Detail");		// detailed session handling & MT logs
_LIT8(KECommonLockTag, "Lock");
_LIT8(KECommonErrorTag, "Error");			// exceptional circumstances (like panic'ing a client).


#define COMMONLOG(logArgs) Den::Logging::Printf logArgs ;

#if defined(COMMONDETAILED_LOG)
	#define COMMONLOG_DETAILED(logArgs) Den::Logging::Printf logArgs ;
#else
	#define COMMONLOG_DETAILED(logArgs)
#endif

#define COMMONLOG_START_BLOCK(subTag, blockTag)  { Den::Logging::Printf(subTag, Den::Logging::ELogBlockStart, blockTag);
#define COMMONLOG_END_BLOCK(subTag, blockTag) Den::Logging::Printf(subTag, Den::Logging::ELogBlockEnd, blockTag); }

#else
//! SYMBIAN_TRACE_ENABLE

#define COMMONLOG(logArgs)
#define COMMONLOG_DETAILED(logArgs)

#define COMMONLOG_START_BLOCK(subTag, blockTag)
#define COMMONLOG_END_BLOCK(subTag, blockTag)

#endif
//SYMBIAN_TRACE_ENABLE


} // namespace Den

#endif
//SYMBIAN_DEN_LOG_H






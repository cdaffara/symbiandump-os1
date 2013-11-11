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
//

/**
 @file
 @internalTechnology
*/


#ifndef SYMBIAN_FACTORY_LOG_H
#define SYMBIAN_FACTORY_LOG_H

// Enable logging for debug builds or builds with the comms flogger enabled - must be defined before including e32utrace.h
#if (defined(_DEBUG) || defined(__FLOG_ACTIVE)) && !defined(SYMBIAN_TRACE_ENABLE)
#define SYMBIAN_TRACE_ENABLE
#endif

#include <e32utrace.h>


namespace Factories
{
class Logging
    {
public:
    enum TLogEntryType
        {
        ELogNodeCreate,
        ELogNodeDestroy,
        ELogInfo
        };

private:
    enum
        {
        KPrimaryFilter = 196
        };

    class TLogIgnoreOverflow8 : public TDes8Overflow
        {
        public:
            void Overflow(TDes8& /*aDes*/) { }
        };

public:
    // General logging statememts
	static void Printf(const TDesC8& aSubTag, TRefByValue<const TDesC8> aFmt, ...);
	static void Printf(const TDesC8& aSubTag, TRefByValue<const TDesC8> aFmt, VA_LIST& aList);
	static void Printf(const TDesC8& aSubTag, TLogEntryType aType, TRefByValue<const TDesC8> aFmt, ...);
	static void Printf(const TDesC8& aSubTag, TLogEntryType aType, TRefByValue<const TDesC8> aFmt, VA_LIST& aList);
	};
}


#ifdef SYMBIAN_TRACE_ENABLE

_LIT8(KFactoryTag, "factory");

// Local helper macros - don't use these directly
#define FACTORY_LOG_NODE_CREATE_ARG0(typeName)           _L8(#typeName " %08x:\tcreated, factory id %08x" ), this, Uid().iUid
#define FACTORY_LOG_NODE_DESTROY_ARG0(typeName)          _L8(#typeName " %08x:\t~" ), this

// Node general logging
#define FACTORY_LOG(logArgs) Factories::Logging::Printf logArgs ;

// Node creation logging macros
#define FACTORY_LOG_NODE_CREATE(subTag, typeName) \
    FACTORY_LOG((subTag, Factories::Logging::ELogNodeCreate, FACTORY_LOG_NODE_CREATE_ARG0(typeName)))

// Node destruction logging macros
#define FACTORY_LOG_NODE_DESTROY(subTag, typeName) \
    FACTORY_LOG((subTag, Factories::Logging::ELogNodeDestroy, FACTORY_LOG_NODE_DESTROY_ARG0(typeName)))

#else
//! SYMBIAN_TRACE_ENABLE

#define FACTORY_LOG_NODE_CREATE_ARG0(typeName)
#define FACTORY_LOG_NODE_DESTROY_ARG0(typeName)

#define FACTORY_LOG(logArgs)

#define FACTORY_LOG_NODE_CREATE(subTag, typeName)
#define FACTORY_LOG_NODE_DESTROY(subTag, typeName)

#endif
// SYMBIAN_TRACE_ENABLE


#endif
// SYMBIAN_FACTORY_LOG_H


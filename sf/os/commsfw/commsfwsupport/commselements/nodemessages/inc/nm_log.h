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
// node message logging
// 
//

/**
 @file
 @internalTechnology
*/


#ifndef SYMBIAN_NM_LOG_H
#define SYMBIAN_NM_LOG_H

// Enable logging for debug builds or builds with the comms flogger enabled - must be defined before including e32utrace.h
#if (defined(_DEBUG) || defined(__FLOG_ACTIVE)) && !defined(SYMBIAN_TRACE_ENABLE)
#define SYMBIAN_TRACE_ENABLE
#endif

#include <e32utrace.h>
#include <e32des8.h>

namespace Messages
{

class TSignalBase;
class TRuntimeCtxId;
class ANode;

const TInt KMaxLogTextLength = 250;

class TLogTextBuf : public TBuf8<KMaxLogTextLength>
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
    {
public:
    // These are used in the btrace/utrace log output as the SubCategory/Schema
    enum TLogEntryType
        {
        ELogBinary = KBinary,   // Binary dump = 0
        ELogInfo = KText,       // Text output = 1
        
        ELogBlockStart,         // = 2
        ELogBlockEnd,           // = 3
        ELogNodeCreate,         // = 4 
        ELogNodeDestroy,        // = 5
        ELogAddress,            // = 6
        ELogMessage,            // = 7
        ELogNode,               // = 8
        };

private:
    enum
        {
        KPrimaryFilter = 194
        };

public:
    // General logging statememts
	IMPORT_C static void Printf(const TDesC8& aSubTag, TRefByValue<const TDesC8> aFmt, ...);
	IMPORT_C static void Printf(const TDesC8& aSubTag, TRefByValue<const TDesC8> aFmt, VA_LIST& aList);
	IMPORT_C static void Printf(const TDesC8& aSubTag, TLogEntryType aType, TRefByValue<const TDesC8> aFmt, ...);
	IMPORT_C static void Printf(const TDesC8& aSubTag, TLogEntryType aType, TRefByValue<const TDesC8> aFmt, VA_LIST& aList);

	IMPORT_C static void LogAddress(const TDesC8& aSubTag, const TRuntimeCtxId& aAddress, const TDesC8& aText);
	IMPORT_C static void LogAddress(const TDesC8& aSubTag, const TRuntimeCtxId& aAddress);
	IMPORT_C static void LogMessage(const TDesC8& aSubTag, const TDesC8& aMessage, const TDesC8& aText);
	IMPORT_C static void LogMessage(const TDesC8& aSubTag, const TDesC8& aMessage);
	IMPORT_C static void LogMessage(const TDesC8& aSubTag, const TSignalBase& aMessage, const TDesC8& aText);
    IMPORT_C static void LogMessage(const TDesC8& aSubTag, const TSignalBase& aMessage);
	IMPORT_C static void LogNode(const TDesC8& aSubTag, const ANode& aNode, const TDesC8& aText);
	IMPORT_C static void LogNode(const TDesC8& aSubTag, const ANode& aNode);
	};

} // namespace Messages


#ifdef SYMBIAN_TRACE_ENABLE

_LIT8(KNodeMessagesSubTag, "nodemsg");

// Local helper macros - don't use these directly
#define NM_LOG_NODE_CREATE_ARG0(typeName)                               _L8(#typeName " %08x:\tcreated [ANode=0x%08x] [%d]" ), this, static_cast<Messages::ANodeId*>(this)->Id().Ptr(), sizeof(typeName)
#define NM_LOG_NODE_CREATE_ARG1(typeName, formatText, a1)               _L8(#typeName " %08x:\tcreated [ANode=0x%08x] [%d] " formatText), this, static_cast<Messages::ANodeId*>(this)->Id().Ptr(), sizeof(typeName), a1
#define NM_LOG_NODE_CREATE_ARG2(typeName, formatText, a1, a2)           _L8(#typeName " %08x:\tcreated [ANode=0x%08x] [%d] " formatText), this, static_cast<Messages::ANodeId*>(this)->Id().Ptr(), sizeof(typeName), a1, a2
#define NM_LOG_NODE_CREATE_ARG3(typeName, formatText, a1, a2, a3)       _L8(#typeName " %08x:\tcreated [ANode=0x%08x] [%d] " formatText), this, static_cast<Messages::ANodeId*>(this)->Id().Ptr(), sizeof(typeName), a1, a2, a3

#define NM_LOG_NODE_DESTROY_ARG0(typeName)                              _L8(#typeName " %08x:\t~" #typeName " [ANode=0x%08x]" ), this, static_cast<Messages::ANodeId*>(this)->Id().Ptr()
#define NM_LOG_NODE_DESTROY_ARG1(typeName, formatText, a1)              _L8(#typeName " %08x:\t~" #typeName " [ANode=0x%08x]" formatText), this, static_cast<Messages::ANodeId*>(this)->Id().Ptr(), a1
#define NM_LOG_NODE_DESTROY_ARG2(typeName, formatText, a1, a2)          _L8(#typeName " %08x:\t~" #typeName " [ANode=0x%08x]" formatText), this, static_cast<Messages::ANodeId*>(this)->Id().Ptr(), a1, a2
#define NM_LOG_NODE_DESTROY_ARG3(typeName, formatText, a1, a2, a3)      _L8(#typeName " %08x:\t~" #typeName " [ANode=0x%08x]" formatText), this, static_cast<Messages::ANodeId*>(this)->Id().Ptr(), a1, a2, a3


// Node Messages general logging
#define NM_LOG(logArgs) Messages::Logging::Printf logArgs ;
#define NM_LOG_STMT(statement) statement ;


// Node creation logging macros
#define NM_LOG_NODE_CREATE(subTag, typeName) \
    NM_LOG((subTag, Messages::Logging::ELogNodeCreate, NM_LOG_NODE_CREATE_ARG0(typeName)))

#define NM_LOG_NODE_CREATE1(subTag, typeName, formatText, arg1) \
    NM_LOG((subTag, Messages::Logging::ELogNodeCreate, NM_LOG_NODE_CREATE_ARG1(typeName, formatText, arg1)))

#define NM_LOG_NODE_CREATE2(subTag, typeName, formatText, arg1, arg2) \
    NM_LOG((subTag, Messages::Logging::ELogNodeCreate, NM_LOG_NODE_CREATE_ARG2(typeName, formatText, arg1, arg2)))

#define NM_LOG_NODE_CREATE3(subTag, typeName, formatText, arg1, arg2, arg3) \
    NM_LOG((subTag, Messages::Logging::ELogNodeCreate, NM_LOG_NODE_CREATE_ARG3(typeName, formatText, arg1, arg2, arg3)))


// Node destruction logging macros
#define NM_LOG_NODE_DESTROY(subTag, typeName) \
    NM_LOG((subTag, Messages::Logging::ELogNodeDestroy, NM_LOG_NODE_DESTROY_ARG0(typeName)))

#define NM_LOG_NODE_DESTROY1(subTag, typeName, formatText, arg1) \
    NM_LOG((subTag, Messages::Logging::ELogNodeDestroy, NM_LOG_NODE_DESTROY_ARG1(typeName, formatText, arg1)))

#define NM_LOG_NODE_DESTROY2(subTag, typeName, formatText, arg1, arg2) \
    NM_LOG((subTag, Messages::Logging::ELogNodeDestroy, NM_LOG_NODE_DESTROY_ARG2(typeName, formatText, arg1, arg2)))

#define NM_LOG_NODE_DESTROY3(subTag, typeName, formatText, arg1, arg2, arg3) \
    NM_LOG((subTag, Messages::Logging::ELogNodeDestroy, NM_LOG_NODE_DESTROY_ARG3(typeName, formatText, arg1, arg2, arg3)))


#define NM_LOG_START_BLOCK(subTag, blockTag)  { Messages::Logging::Printf(subTag, Messages::Logging::ELogBlockStart, blockTag);
#define NM_LOG_END_BLOCK(subTag, blockTag) Messages::Logging::Printf(subTag, Messages::Logging::ELogBlockEnd, blockTag); }



#define NM_LOG_ADDRESS(subTag, address) \
    Messages::Logging::LogAddress(subTag, address, _L8(#address ": "))

#define NM_LOG_ADDRESS_EXT(subTag, address, logTxt) \
    Messages::Logging::LogAddress(subTag, address, logTxt)



#define NM_LOG_MESSAGE(subTag, message) \
    Messages::Logging::LogMessage(subTag, message, _L8(#message ": "))

#define NM_LOG_MESSAGE_EXT(subTag, message, logTxt) \
    Messages::Logging::LogMessage(subTag, message, logTxt)



#define NM_LOG_NODE(subTag, node) \
    Messages::Logging::LogNode(subTag, node, _L8(#node ": "))

#define NM_LOG_NODE_EXT(subTag, node, logTxt) \
    Messages::Logging::LogNode(subTag, node, logTxt)

    
#else
//! SYMBIAN_TRACE_ENABLE


#define NM_LOG_NODE_CREATE_ARG0(typeName)
#define NM_LOG_NODE_CREATE_ARG1(typeName, formatText, a1)
#define NM_LOG_NODE_CREATE_ARG2(typeName, formatText, a1, a2)
#define NM_LOG_NODE_CREATE_ARG3(typeName, formatText, a1, a2, a3)

#define NM_LOG_NODE_DESTROY_ARG0(typeName)
#define NM_LOG_NODE_DESTROY_ARG1(typeName, formatText, a1)
#define NM_LOG_NODE_DESTROY_ARG2(typeName, formatText, a1, a2)
#define NM_LOG_NODE_DESTROY_ARG3(typeName, formatText, a1, a2, a3)

#define NM_LOG(logArgs)
#define NM_LOG_STMT(statement)

#define NM_LOG_NODE_CREATE(subTag, typeName)
#define NM_LOG_NODE_CREATE1(subTag, typeName, formatText, arg1)
#define NM_LOG_NODE_CREATE2(subTag, typeName, formatText, arg1, arg2)
#define NM_LOG_NODE_CREATE3(subTag, typeName, formatText, arg1, arg2, arg3)

#define NM_LOG_NODE_DESTROY(subTag, typeName)
#define NM_LOG_NODE_DESTROY1(subTag, typeName, formatText, arg1)
#define NM_LOG_NODE_DESTROY2(subTag, typeName, formatText, arg1, arg2)
#define NM_LOG_NODE_DESTROY3(subTag, typeName, formatText, arg1, arg2, arg3)

#define NM_LOG_START_BLOCK(subTag, blockTag)
#define NM_LOG_END_BLOCK(subTag, blockTag)

#define NM_LOG_ADDRESS(subTag, address)
#define NM_LOG_ADDRESS_EXT(subTag, address, logTxt)
#define NM_LOG_MESSAGE(subTag, message)
#define NM_LOG_MESSAGE_EXT(subTag, message, logTxt)
#define NM_LOG_NODE(subTag, node)
#define NM_LOG_NODE_EXT(subTag, node, logTxt)

#endif
// SYMBIAN_TRACE_ENABLE


#endif
// SYMBIAN_NM_LOG_H


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
// mesh machine logging
// 
//

/**
 @file
 @internalTechnology
*/


#ifndef SYMBIAN_MM_LOG_H
#define SYMBIAN_MM_LOG_H

#include <elements/nm_log.h>

namespace MeshMachine
{
class TNodeContextBase;
class CNodeActivityBase;

class Logging
    {
public:
    // These are used in the btrace/utrace log output as the SubCategory/Schema
    enum TLogEntryType
        {
        ELogBinary = KBinary,   // Binary dump = 0
        ELogInfo = KText,       // Text output = 1
        
        ELogContext,            // = 2
        ELogActivity,           // = 3
        };

private:
    enum
        {
        KPrimaryFilter = 195
        };

public:
	IMPORT_C static void Printf(const TDesC8& aSubTag, TRefByValue<const TDesC8> aFmt, ...);
	IMPORT_C static void Printf(const TDesC8& aSubTag, TRefByValue<const TDesC8> aFmt, VA_LIST& aList);

	IMPORT_C static void LogContext(const TDesC8& aSubTag, const TNodeContextBase& aContext);
	IMPORT_C static void LogContext(const TDesC8& aSubTag, const TNodeContextBase& aContext, const TDesC8& aText);
	IMPORT_C static void LogActivity(const TDesC8& aSubTag, const CNodeActivityBase* aActivity, const TNodeContextBase* aContext);
	IMPORT_C static void LogActivity(const TDesC8& aSubTag, const CNodeActivityBase* aActivity, const TNodeContextBase* aContext, const TDesC8& aText);
	};
	
	
} // namespace MeshMachine


#ifdef SYMBIAN_TRACE_ENABLE

_LIT8(KMeshMachineSubTag, "meshmachine");

// Node Messages general logging
#define MESH_LOG(logArgs) MeshMachine::Logging::Printf logArgs ;


#define MESH_LOG_ACTIVITY(subTag, activity, context) \
    MeshMachine::Logging::LogActivity(subTag, activity, context)

#define MESH_LOG_ACTIVITY_EXT(subTag, activity, context, logTxt) \
    { \
    Messages::TLogTextBuf _logTxtBuf; \
    _logTxtBuf.AppendFormatIgnoreOverflow logTxt ; \
    MeshMachine::Logging::LogActivity(subTag, activity, context, _logTxtBuf); \
    }

    
// context = TNodeContextBase
#define MESH_LOG_CONTEXT(subTag, context) \
    MeshMachine::Logging::LogContext(subTag, context, _logTxtBuf)

#define MESH_LOG_CONTEXT_EXT(subTag, context, logTxt) \
    { \
    Messages::TLogTextBuf _logTxtBuf; \
    _logTxtBuf.AppendFormatIgnoreOverflow logTxt ; \
    MeshMachine::Logging::LogContext(subTag, context, _logTxtBuf); \
    }
	


#else
// !SYMBIAN_TRACE_ENABLE

#define MESH_LOG(logArgs)

#define MESH_LOG_ACTIVITY(subTag, activity, context)
#define MESH_LOG_ACTIVITY_EXT(subTag, activity, context, logTxt)
#define MESH_LOG_CONTEXT(subTag, context)
#define MESH_LOG_CONTEXT_EXT(subTag, context, logTxt)

#endif
// SYMBIAN_TRACE_ENABLE


#endif
// SYMBIAN_MM_LOG_H


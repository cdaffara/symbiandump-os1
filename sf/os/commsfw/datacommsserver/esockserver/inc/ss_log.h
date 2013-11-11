// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __SS_LOG_H__
#define __SS_LOG_H__

#include <cflog.h>
#include <comms-infras/cfmacro.h>
#include <comms-infras/ss_logext.h>
#include <comms-infras/sockmes.h>
#include <comms-infras/es_connectionservermessages.h>



// Minor levels of log tagging
_LIT8(KESockClientTag, "Client");			// the common client-side case
_LIT8(KESockServerTag, "Server");			// the common server-side case
_LIT8(KESockBootingTag, "Booting");			// grim & verbose details of booting & shutting down
_LIT8(KESockProvChoresTag, "ProvChores");	// housekeeping activities for providers & protocols (eg [re]loading PRT)
_LIT8(KESockSessDetailTag, "Detail");		// detailed session handling & MT logs
_LIT8(KESockCoreProviderTag, "CoreProv");
_LIT8(KESockLockTag, "Lock");
_LIT8(KESockErrorTag, "Error");			// exceptional circumstances (like panic'ing a client).
_LIT8(KFactoryTag, "Factory");          // log for Comms factory components

#if defined (__CFLOG_ACTIVE) || defined (SYMBIAN_TRACE_ENABLE)


/**
@internalComponent
*/
const TInt KHexDumpWidth = 16;

/**
@internalComponent
*/
#ifndef LOG
	#define ESOCK_LOGGING_ACTIVE
	#define LOG(x) x
	#if defined(DETAILED_LOG)
		#define LOG_DETAILED(a) a
	#else
		#define LOG_DETAILED(a)
	#endif
#endif

#ifndef LOG_STMT
	#define LOG_STMT(a) a
#endif

class ESockLog : public ESockLogExternal
/**
@internalComponent
*/
	{
public:
	static void Printf(TRefByValue<const TDesC> aFmt, ...);
	static void Printf(const TDesC8& aSubTag, TRefByValue<const TDesC> aFmt, ...);
	static void Printf(const TDesC8& aSubTag, TRefByValue<const TDesC> aFmt, VA_LIST& aList);
	static void Printf(const TDesC8& aMajorTag, const TDesC8& aSubTag, TRefByValue<const TDesC> aFmt, VA_LIST& aList);
	static void Printf(TRefByValue<const TDesC8> aFmt, ...);
	static void Printf(const TDesC8& aSubTag, TRefByValue<const TDesC8> aFmt, ...);
	static void Printf(const TDesC8& aSubTag, TRefByValue<const TDesC8> aFmt, VA_LIST& aList);
	static void ConnectionInfoPrintf(const TDesC8& aConnectionInfo, TRefByValue<const TDesC> aFmt, ...);
	static const TText8* IPCMessName(TSockMess aMess);
	static void IPCMessName(TSockMess aMessNum, TDes8& aMessBuf);
	static const TText8* ConnServIPCMessName(TConnectionServerMessage aMess);
	static void ConnServIPCMessName(TConnectionServerMessage aMessNum, TDes8& aMessBuf);
	};

class TLogIgnoreOverflow8 : public TDes8Overflow
	{
public:
	void Overflow(TDes8& /*aDes*/) { }
	};

class TLogIgnoreOverflow16 : public TDes16Overflow
	{
public:
	void Overflow(TDes16& /*aDes*/) { }
	};

#else
#define LOG(x)
#define LOG_DETAILED(a)
#define LOG_STMT(a)
#endif



#endif
// __SS_LOG_H__


// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Central place for nice debug-type macros & functions
// 
//

#ifndef BT_DEBUG_H
#define BT_DEBUG_H

#ifdef __USING_POLICING_ESOCK__
#undef __USING_POLICING_ESOCK__
#endif
//#define __USING_POLICING_ESOCK__

#include <e32std.h>
#include <f32file.h>
#include <comms-infras/commsdebugutility.h>

#ifdef __FLOG_ACTIVE	// = commsdbg logging on
//#define BLOGGING__    //removed for MCL submission
#define __FLOGGING__
#endif // __FLOG_ACTIVE

// Borrowed from comms-framework - experimental ASSERT variants that
// breakpt at the scene of the crime.
#ifdef _DEBUG
#if defined (__WINS__)
#undef __ASSERT_DEBUG
inline void About_To_Panic_Thread() { __DEBUGGER() }
#define __ASSERT_DEBUG(c,p) (void)((c)||(About_To_Panic_Thread(),p,0))
#undef __ASSERT_ALWAYS
#define __ASSERT_ALWAYS(c,p) (void)((c)||(About_To_Panic_Thread(),p,0))
#endif
#endif


class CBlogger;
class CDeltaTimer;
class CDebugControlInterface;
class CAvdtpLogEngine;
class CAvctpLogEngine;

struct SBtTls
	{
public:
	CDeltaTimer*			iTimer;	// always needed: not owned
	TUint					iPort;	// the serial port we are on - for logging only
	CBlogger*				iBlogger;	// for logging only	
	CDebugControlInterface* iDebugCtrlIF;
	CAvdtpLogEngine*		iAvdtpLogEngine;
	CAvctpLogEngine*		iAvctpLogEngine;	
	};

#define BTTLS static_cast<SBtTls*>(Dll::Tls())

#ifdef __USING_POLICING_ESOCK__
#else
enum TCapabilityRequestType 
 	{
	//Socket Capability
 	ESetOptCheck,
 	EGetOptCheck,
 	EIoctlCheck,
 	EBindCheck,
	EListenCheck,
 	EConnectionCheck,
	EWriteCheck,
 	EReceiveCheck,
	EGetLocalNameCheck,
	EGetRemoteNameCheck,

	//HostResolver
	EHRGetByNameCheck,
	EHRGetByAddressCheck,
	EHRSetHostNameCheck,
	EHRGetHostNameCheck,

	// NetDataBase
	ENDQueryCheck,
	ENDAddCheck,
	ENDRemoveCheck,

	// ServiceResolver
	ESRGetByNameCheck,
	ESRGetByNumberCheck,
	ESRRegisterServiceCheck,
	ESRRemoveServiceCheck,

	//Cconnection
 	EReadCommdbCheck,
	EControlCheck
 	};

#endif


static const TInt KL2CAPDebugOptionMask = 0xFFFF; //< SetOpt for debugging modes in L2CAP
static const TUint KL2CAPDebugSendRequestsTwice = 1; //< Send L2CAP requests twice
static const TUint KL2CAPDebugInvalidL2CAPLengthInConnect= 3; //< Set the L2CAP length wrong
static const TUint KL2CAPDebugInvalidL2CAPLengthInData=4;//< Set the L2CAP length wrong

#ifndef LOGGER_H

_LIT(KLogFile,"btlog0.txt");
const TInt KLogIndexOffset=5;
_LIT(KLogDirFullName,"c:\\logs\\bt\\");
_LIT(KLogDir,"BT");

//#define COMMENT SLASH(/)
#define COMMENT
//#define SLASH(s) /##s
#ifndef BLOGGING__
// define empty blogging macro stubs
#define BLOGGER                    COMMENT
#define BLOG(layer,verbosity,type,blog_call,cbloggette_type) COMMENT
//------- Blogging for BT Qual ------------
#define LLQBLOG(type,blog_call)     COMMENT
#define L2QBLOG(type,blog_call)     COMMENT
#define RFQBLOG(type,blog_call)     COMMENT
//------- Loud Blogging Calls -------------
#define LLLBLOG(type,blog_call)     COMMENT
#define L2LBLOG(type,blog_call)     COMMENT
#define RFLBLOG(type,blog_call)     COMMENT
//------- Generic Blogging calls ----------
#define LLBLOG(verbosity,type,blog_call)      COMMENT
#define L2BLOG(verbosity,type,blog_call)      COMMENT
#define RFBLOG(verbosity,type,blog_call)      COMMENT
#else   // define blogging calls
#define BLOGGER BTTLS->iBlogger
#define BLOG(layer,verbosity,type,blog_call,cbloggette_type) \
	{ \
	CBlogger* pBlog = BLOGGER;  \
	CBloggette* pBloggette;\
	if((pBlog->LoggingMask(layer,verbosity))&(type))  \
		{  \
		pBloggette=(pBlog->GetLayerBloggette(layer)); \
		(STATIC_CAST(cbloggette_type*,pBloggette))->blog_call;  \
		}  \
	}
// the blog_call should look like Log(KMyFormatingLit,var1,var2,varbla);
// or L2BLOG(KVerbose,Log(KL2ConnLit, connH, addr));
// or L2BLOG(KVerbose,ConnLog(connH,addr));
//------- Blogging for BT Qual ------------
#define LLQBLOG(type,blog_call) BLOG(KBlogLayerLinkLayer,KBlogVerbosityBTQualLevel,type,blog_call, CLLBloggette)
#define L2QBLOG(type,blog_call) BLOG(KBlogLayerL2CAP,KBlogVerbosityBTQualLevel,type,blog_call, CL2Bloggette)
#define RFQBLOG(type,blog_call) BLOG(KBlogLayerRFComm,KBlogVerbosityBTQualLevel,type,blog_call, CRFBloggette)
//------- Loud Blogging Calls -------------
#define LLLBLOG(type,blog_call) BLOG(KBlogLayerLinkLayer,KBlogVerbosityLoud,type,blog_call,CLLBloggette)
#define L2LBLOG(type,blog_call) BLOG(KBlogLayerL2CAP,KBlogVerbosityLoud,type,blog_call, CL2Bloggette)
#define RFLBLOG(type,blog_call) BLOG(KBlogLayerRFComm,KBlogVerbosityLoud,type,blog_call, CRFBloggette)
//------- Generic Blogging calls ----------
#define LLBLOG(verbosity,type,blog_call) BLOG(KBlogLayerLinkLayer,verbosity,type,blog_call,CLLBloggette)
#define L2BLOG(verbosity,type,blog_call) BLOG(KBlogLayerL2CAP,verbosity,type,blog_call,CL2Bloggette)
#define RFBLOG(verbosity,type,blog_call) BLOG(KBlogLayerRFComm,verbosity,type,blog_call,CRFBloggette)
#endif
  
inline void __BTDEBUGGER()
// needed to call __DEBUGGER inside an __ASSERT
	{
	__DEBUGGER()
	}

#ifdef __FLOGGING__
#define FLOG(a) {FPrint(a);}
#define FTRACE(a) {a;}

inline void FPrint(const TRefByValue<const TDesC> aFmt, ...)
	{
	VA_LIST list;
	VA_START(list,aFmt);
	TBuf<32> logFile(KLogFile);
#ifdef BLOGGING__
	// the Blogger holds the flogging file index since it is pointed by the TLS
	CBlogger* pBlog = BLOGGER;
	logFile[KLogIndexOffset]=TText('0'+(TInt)pBlog->GetFloggerInfo());	
#else
	logFile[KLogIndexOffset]=TText('0'+BTTLS->iPort);
#endif
	RFileLogger::WriteFormat(KLogDir, logFile, EFileLoggingModeAppend, aFmt, list);
#ifdef MEMTRACE
	TInt size;
	User::Heap().AllocSize(size);
	
	RFileLogger::WriteFormat(KLogDir, logFile, EFileLoggingModeAppend, _L("Mem usage: %d high: %d"), size, User::Heap().Size());
#endif
	}

inline void FHex(const TUint8* aPtr, TInt aLen)
	{
	TBuf<32> logFile(KLogFile);
	logFile[KLogIndexOffset]=TText('0'+BTTLS->iPort);
	RFileLogger::HexDump(KLogDir, logFile, EFileLoggingModeAppend, 0, 0, aPtr, aLen);
	}

inline void FHex(const TDesC8& aDes)
	{
	FHex(aDes.Ptr(), aDes.Length());
	}


#define SCO_LOG(a) FPrint(a);
#define SCO_LOG_2(a, b) FPrint(a, b);
#define SCO_LOG_3(a, b, c) FPrint(a, b, c);
#define SCO_LOG_4(a, b, c, d) FPrint(a, b, c, d);
#define SCO_LOG_5(a, b, c, d, e) FPrint(a, b, c, d, e);
#define SCO_LOG_6(a, b, c, d, e, f) FPrint(a, b, c, d, e, f);
#define SCO_LOG_ADDR(string, addr) { TBuf<20> addrbuf; addrbuf.Format(_L("%02x:%02x:%02x:%02x:%02x:%02x"), addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]); SCO_LOG_2(string, &addrbuf); }

#else // i.e. __FLOGGING__ undefined
#define FLOG(a)
#define FTRACE(a)

#define SCO_LOG(a)
#define SCO_LOG_2(a, b)
#define SCO_LOG_3(a, b, c)
#define SCO_LOG_4(a, b, c, d)
#define SCO_LOG_5(a, b, c, d, e)
#define SCO_LOG_6(a, b, c, d, e, f)
#define SCO_LOG_ADDR(string, addr)


inline void FPrint(const TRefByValue<const TDesC> /*aFmt*/,...) 
	{
#ifdef _DEBUG // to quell linker warnings
	RFileLogger dummy;
#endif
	}

inline void FHex(const TUint8* /*aPtr*/, TInt /*aLen*/)
	{
	}

inline void FHex(const TDesC8& /*aDes*/)
	{
	}

#endif //	__FLOGGING__

#endif // #ifndef LOGGER_H

#define __PANIC_UNEXPECTED_CALL __ASSERT_DEBUG(NULL, User::Invariant());

#endif // __BT_DEBUG_H__

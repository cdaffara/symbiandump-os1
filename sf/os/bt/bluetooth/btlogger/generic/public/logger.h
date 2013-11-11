// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef LOGGER_H
#define LOGGER_H

// Enable __UREL_RDEBUG_PRINT to allow method entry / exit logging to
// be output for UREL builds through epocwind.out (for emulator) and
// the debugport for hardware.
//#define __UREL_RDEBUG_PRINT

#include <e32base.h>
#include <comms-infras/commsdebugutility.h>
#include <bluetooth/logger/components.h> // standard component string definitions

// Control function entry and exit logging using a compile-time switch.
#ifndef __FLOG_NO_FUNC // Declare this macro in an mmp file to disable function logging
#define __LOG_FUNCTIONS__
#endif

class TFunctionLogger;

#ifdef __FLOG_ACTIVE
#define IF_FLOGGING(a) a
#else
#define IF_FLOGGING(a)
#endif

// Undefine if defined by comms-infras
#ifdef LOG
#undef LOG
#endif

// Undefine if defined by comms-infras
#ifdef FLOG
#undef FLOG
#endif

_LIT8(KDefaultLogFile, "BT");

#ifdef __FLOG_ACTIVE
#define CONNECT_LOGGER					static_cast<void>(CBtLog::Connect());
#define CLOSE_LOGGER					CBtLog::Close();
#define LEAVEIFERRORL(a)				VerboseLeaveIfErrorL(KLogComponent, __FILE__, __LINE__, a)
#define LEAVEL(a)						VerboseLeaveL(KLogComponent, __FILE__, __LINE__, a)
#define PANIC(CAT, CODE) 				VerbosePanic(KLogComponent, __FILE__, __LINE__, CODE, (TText8*)#CODE, CAT)
#define PANIC_MSG(msg, cat, code)		VerboseMsgPanic(KLogComponent, __FILE__, __LINE__, msg, cat, code);
#define FLOG(a)							CBtLog::Write(KDefaultLogFile, a);

#define FTRACE(a)						{a;}
#define LOG(text)						CBtLog::Write(KLogComponent, text);
#define LOG1(text, a)					CBtLog::WriteFormat(KLogComponent, text, a);
#define LOG2(text, a, b)				CBtLog::WriteFormat(KLogComponent, text, a, b);
#define LOG3(text, a, b, c)				CBtLog::WriteFormat(KLogComponent, text, a, b, c);
#define LOG4(text, a, b, c, d)			CBtLog::WriteFormat(KLogComponent, text, a, b, c, d);
#define LOG5(text, a, b, c, d, e)		CBtLog::WriteFormat(KLogComponent, text, a, b, c, d, e);
#define LOG6(text, a, b, c, d, e, f)	CBtLog::WriteFormat(KLogComponent, text, a, b, c, d, e, f);
#define LOGHEXDESC(desc)				CBtLog::HexDump(KLogComponent, 0, 0, desc.Ptr() , desc.Length());
#define LOGHEXRAW(data, len)			CBtLog::HexDump(KLogComponent, 0, 0, data, len);
#define FLOGIFERR(var,text) 			{ if (KErrNone!=var) {CBtLog::Write(KLogComponent, text);}}
#define DUMPFRAME(text)					{LOG(text);TInt _lctr = this->Length();if(_lctr){for(TInt _ictr = 0; _ictr < _lctr; ++_ictr){LOG2(_L8("....[%d] = %02x"),_ictr,this->First()->Ptr()[_ictr]);}}}
#define LOG_BT(aa, bb)					{TBuf8<20> addr1; addr1.Format(_L8("%02x:%02x:%02x:%02x:%02x:%02x"), bb[0], bb[1], bb[2], bb[3], bb[4], bb[5]); LOG1(aa, &addr1); }
#else
#define CONNECT_LOGGER
#define CLOSE_LOGGER
#define LEAVEIFERRORL(a)				static_cast<void>(User::LeaveIfError(a))
#define LEAVEL(a)						User::Leave(a)
#define PANIC(CAT, CODE) 				User::Panic(CAT, CODE)
#define PANIC_MSG(msg, cat, code)		msg.Panic(cat, code);
#define FLOG(a)
#define FTRACE(a)
#define LOG(text)
#define LOG1(text, a)
#define LOG2(text, a, b)
#define LOG3(text, a, b, c)
#define LOG4(text, a, b, c, d)
#define LOG5(text, a, b, c, d, e)
#define LOG6(text, a, b, c, d, e, f)
#define LOGHEXDESC(desc)
#define LOGHEXRAW(data, len)
#define FLOGIFERR(var,text) {var=var;}
#define DUMPFRAME(text)
#define LOG_BT(aa, bb)
#endif // __FLOG_ACTIVE

#define FORCED_LOG_FUNC					TFunctionLogger __instrument(KLogComponent, TPtrC8((TUint8*)__PRETTY_FUNCTION__), (TAny*)this);
#define FORCED_LOG_STATIC_FUNC			TStaticFunctionLogger __instrument(KLogComponent, TPtrC8((TUint8*)__PRETTY_FUNCTION__));

#if ( defined __FLOG_ACTIVE && defined __LOG_FUNCTIONS__ )
#define LOG_LINE						CBtLog::Write(KLogComponent, KNullDesC8());
#define LOG_FUNC						FORCED_LOG_FUNC
#define LOG_STATIC_FUNC					FORCED_LOG_STATIC_FUNC
#else
#if ( defined __UREL_RDEBUG_PRINT && !defined _DEBUG )
#include <e32debug.h>
_LIT8(KLogComponent, "RDebugOutput");
#define LOG_LINE						RDebug::Printf("%S", &KLogComponent());
#define LOG_FUNC						FORCED_LOG_FUNC
#define LOG_STATIC_FUNC					FORCED_LOG_STATIC_FUNC
#else
#define LOG_LINE
#define LOG_FUNC
#define LOG_STATIC_FUNC
#endif
#endif

inline void __BTDEBUGGER()
// needed to call __DEBUGGER inside an __ASSERT
	{
	__DEBUGGER()
	}

// Logs the TBTDevAddr x.
#define LOGBTDEVADDR(x) 				LOG6(_L8("\t" #x " = 0x%02x%02x%02x%02x%02x%02x"),x[0],x[1],x[2],x[3],x[4],x[5])


// For internal programming errors we want a macro which uses a file-specific 
// panic category and __LINE__ as the panic code. 
// To make this non-standard pattern as helpful to users as possible, we 
// append ' line#' to the category. That means we first have to check that the 
// category is 10 characters long or less, so that the whole thing is legible 
// when it appears to users (panic categories are truncated to 16 characters).

// We want a 10-character string (but allow for the NULL terminator).
#define PANICCATEGORY(aaa) __ASSERT_COMPILE(sizeof(L##aaa)/2 <= 11); _LIT(KMungedPanicCat, aaa) 

// A handy panic-self macro- the category is whatever has been declared with 
// BTPANICCATEGORY, with " line#" appended. The panic code is the line number. 
#define PANIC_LINENUM \
	{ \
	_LIT(KLnNo, " line#"); \
	TBuf<KMaxExitCategoryName> cat = KMungedPanicCat(); \
	cat.Append(KLnNo()); \
	PANIC(cat, __LINE__); \
	}

// A handy assertion macro that panics with a locally-defined panic category 
// and the line number.
#define ASSERT_ALWAYS(a) \
	{ \
	if ( !(a) ) \
		{ \
		PANIC_LINENUM; \
		} \
	}

#ifdef _DEBUG
#define ASSERT_DEBUG(a) ASSERT_ALWAYS(a)
#define DEBUG_PANIC_LINENUM PANIC_LINENUM
#else
#define ASSERT_DEBUG(a)
#define DEBUG_PANIC_LINENUM
#endif // _DEBUG

// Undefine the e32def.h-defined ASSERT macro to make sure no-one uses it. 
// Use the more informative ASSERT_DEBUG (defined above) or the vanilla 
// __ASSERT_DEBUG instead!
// #undef ASSERT


NONSHARABLE_CLASS(CBtLog) : public CBase
	{
public:
	IMPORT_C static TInt Connect();
	IMPORT_C static void Close();
	
	IMPORT_C static void Write(const TDesC8& aCmpt, const TDesC8& aText);
	IMPORT_C static void WriteFormat(const TDesC8& aCmpt, TRefByValue<const TDesC8> aFmt, ...);
	IMPORT_C static void WriteFormat(const TDesC8& aCmpt, TRefByValue<const TDesC8> aFmt, VA_LIST& aList);
	IMPORT_C static void Write(const TDesC8& aCmpt, const TDesC16& aText);
	IMPORT_C static void WriteFormat(const TDesC8& aCmpt, TRefByValue<const TDesC16> aFmt, ...);
	IMPORT_C static void WriteFormat(const TDesC8& aCmpt, TRefByValue<const TDesC16> aFmt, VA_LIST& aList);
	IMPORT_C static void HexDump(const TDesC8& aCmpt, const TText* aHeader, const TText* aMargin, const TUint8* aPtr, TInt aLen);
	};


#ifndef NO_FPRINT
inline void FPrint(const TRefByValue<const TDesC> IF_FLOGGING(aFmt), ...)
	{
#ifdef __FLOG_ACTIVE
	VA_LIST list;
	VA_START(list,aFmt);
	CBtLog::WriteFormat(KDefaultLogFile, aFmt, list);
#endif
	}
#endif


#ifndef NO_FHEX_PTR
inline void FHex(const TUint8* IF_FLOGGING(aPtr), TInt IF_FLOGGING(aLen))
	{
#ifdef __FLOG_ACTIVE
	CBtLog::HexDump(KDefaultLogFile, 0, 0, aPtr, aLen);
#endif
	}
#endif


#ifndef NO_FHEX_DESC
inline void FHex(const TDesC8& IF_FLOGGING(aDes))
	{
#ifdef __FLOG_ACTIVE
	FHex(aDes.Ptr(), aDes.Length());
#endif
	}
#endif


IMPORT_C void VerboseLeaveIfErrorL(const TDesC8& aCpt, 
						  char* aFile, 
						  TInt aLine, 
						  TInt aReason);

IMPORT_C void VerboseLeaveL(const TDesC8& aCpt, 
					char* aFile, 
					TInt aLine, 
					TInt aReason);

IMPORT_C void VerboseMsgPanic(const TDesC8& aCpt,
								char* aFile,
								TInt  aLine,
								const RMessage2& aMsg,
								const TDesC& aCat,
								TInt  aPanicCode);

						  
IMPORT_C void VerbosePanic(const TDesC8& aCpt, 
				  char* aFile, 
				  TInt aLine, 
				  TInt aPanicCode, 
				  TText8* aPanicName,
				  const TDesC& aPanicCategory);

static const TInt KMaxIndent = 60;

NONSHARABLE_CLASS(TLoggerBase)
	{
protected:
	TUint PreIncrementIndent();
	TUint PostDecrementIndent();
	TUint Indent();

protected:
	TBuf8<KMaxIndent> iIndentBuf;
	TPtrC8 iCpt;
	TPtrC8 iString;
	};

NONSHARABLE_CLASS(TFunctionLogger) : public TLoggerBase
	{
public:
	IMPORT_C TFunctionLogger(const TDesC8& aCpt, const TDesC8& aString, TAny* aThis);
	IMPORT_C ~TFunctionLogger();
	};

NONSHARABLE_CLASS(TStaticFunctionLogger) : public TLoggerBase
	{
public:
	IMPORT_C TStaticFunctionLogger(const TDesC8& aCpt, const TDesC8& aString);
	IMPORT_C ~TStaticFunctionLogger();
	};

#endif	// LOGGER_H


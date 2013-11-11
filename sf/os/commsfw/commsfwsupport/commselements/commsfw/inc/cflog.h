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

#ifndef __CFLOG_H__
#define __CFLOG_H__

#include <e32base.h>

// This is defined by including <cflog.mmh>
#ifdef __CFLOG_INCLUDED

#include <comms-infras/commsdebugutility.h>
#include <cfshared.h>
#if defined(__FLOG_ACTIVE)
#define __CFLOG_ACTIVE
#endif

#endif	// __CFLOG_INCLUDED

// Enable logging for debug builds or builds with the comms flogger enabled - must be defined before including e32utrace.h
#if (defined(_DEBUG) || defined(__FLOG_ACTIVE)) && !defined(SYMBIAN_TRACE_ENABLE)
#define SYMBIAN_TRACE_ENABLE
#endif


/** Panic codes. 
*/
enum TCFLogPanic
	{
	/** No flogger present.
    */
	ECFNoLogger,
	/** Trying to use cflog without having connected first.
    */
	ECFUserCountInvalid
	};

class CCFLog;
NONSHARABLE_CLASS(CCFLogIf) : public CBase
/** Interface class defining the CFlogger interface.
@internalComponent 
*/
	{
private:
	CCFLogIf();
	void ConstructL();
	CCFLog* CFLog();

public:
	~CCFLogIf();
 	IMPORT_C static CCFLogIf* NewL();
	IMPORT_C static TInt Delete(CCFLogIf* aCFLogIf);
	
	IMPORT_C static void Panic(TCFLogPanic aPanic);

	IMPORT_C static CCFLogIf* Context(); // Panics if no context!
	IMPORT_C void SetContext();

	IMPORT_C static void Connect();
	IMPORT_C static void Close();

	IMPORT_C static void Write(const TDesC8& aSubsystem, const TDesC8& aComponent, const TDesC16& aText);
	IMPORT_C static void WriteFormat(const TDesC8& aSubsystem, const TDesC8& aComponent, TRefByValue<const TDesC16> aFmt,...);
	IMPORT_C static void WriteFormat(const TDesC8& aSubsystem, const TDesC8& aComponent, TRefByValue<const TDesC16> aFmt, VA_LIST& aList);
	IMPORT_C static void Write(const TDesC8& aSubsystem, const TDesC8& aComponent, const TDesC8& aText);
	IMPORT_C static void WriteFormat(const TDesC8& aSubsystem, const TDesC8& aComponent, TRefByValue<const TDesC8> aFmt,...);
	IMPORT_C static void WriteFormat(const TDesC8& aSubsystem, const TDesC8& aComponent, TRefByValue<const TDesC8> aFmt, VA_LIST& aList);
	IMPORT_C static void WriteBinary(const TDesC8& aSubsystem, const TDesC8& aComponent, const TDesC8& aData);
	IMPORT_C static void HexDump(const TDesC8& aSubsystem, const TDesC8& aComponent, const TText* aHeader, const TText* aMargin, const TUint8* aPtr, TInt aLen);

private:
	CCFLog* iCFLog;
	};

#define __CFLOG_DECLARATION_MEMBER_SIZE   TInt32

/** check that cflog member size doesn't change. 
*/
const TInt KCheckCFLogIsCorrectSize = 1/(sizeof(CCFLogIf*)==sizeof(__CFLOG_DECLARATION_MEMBER_SIZE));


#if defined(__CFLOG_ACTIVE)

#define __CFLOG_DECLARATION_VARIABLE	CCFLogIf* __cflog__
#define __CFLOG_DECLARATION_MEMBER		CCFLogIf* __cflog__
#define __CFLOG_CREATEL					__cflog__ = CCFLogIf::NewL()
#define __CFLOG_DELETE					CCFLogIf::Delete(__cflog__)
#define __CFLOG_OPEN					CCFLogIf::Connect()
#define __CFLOG_CLOSE					CCFLogIf::Close()
#define __CFLOG_CLEAR					CCFLogIf::ClearLog()
#define __CFLOG_STMT(text) text

#define __CFLOG_0(subsys,compnt,text)					CCFLogIf::Write(subsys,compnt,text)
#define __CFLOG_1(subsys,compnt,text,a)					CCFLogIf::WriteFormat(subsys,compnt,text,a)
#define __CFLOG_2(subsys,compnt,text,a,b)				CCFLogIf::WriteFormat(subsys,compnt,text,a,b)
#define __CFLOG_3(subsys,compnt,text,a,b,c)				CCFLogIf::WriteFormat(subsys,compnt,text,a,b,c)
#define __CFLOG_4(subsys,compnt,text,a,b,c,d)			CCFLogIf::WriteFormat(subsys,compnt,text,a,b,c,d)
#define __CFLOG_5(subsys,compnt,text,a,b,c,d,e)			CCFLogIf::WriteFormat(subsys,compnt,text,a,b,c,d,e)
#define __CFLOG_6(subsys,compnt,text,a,b,c,d,e,f)		CCFLogIf::WriteFormat(subsys,compnt,text,a,b,c,d,e,f)
#define __CFLOG_7(subsys,compnt,text,a,b,c,d,e,f,g)		CCFLogIf::WriteFormat(subsys,compnt,text,a,b,c,d,e,f,g)
#define __CFLOG_8(subsys,compnt,text,a,b,c,d,e,f,g,h)	CCFLogIf::WriteFormat(subsys,compnt,text,a,b,c,d,e,f,g,h)
#define __CFLOG_9(subsys,compnt,text,a,b,c,d,e,f,g,h,i)	CCFLogIf::WriteFormat(subsys,compnt,text,a,b,c,d,e,f,g,h,i)
#define __CFLOG(subsys,compnt,text)						CCFLogIf::WriteFormat(subsys,compnt,text)
#define __CFLOG_VA(subsys,compnt,fmt,va_list)			CCFLogIf::WriteFormat(subsys,compnt,fmt,va_list)
#define __CFLOG_VAR(args)								CCFLogIf::WriteFormat args
#define __CFLOG_HEXDUMP(subsys,compnt,data,length)		CCFLogIf::HexDump(subsys,compnt,0,0,data,length)
#define __CFLOG_BINARY(subsys,compnt,data)				CCFLogIf::WriteBinary(subsys,compnt,data)

#define __CFLOG_SMADDR(list)	{ TCFFullModuleName __FullModName; CCFLogIf::WriteFormat list; }
#ifdef SYMBIAN_NETWORKING_CFTRANSPORT
#define __CFLOG_SMADDR2(list)	{ TCFFullModuleName __FullModName1,__FullModName2; CCFLogIf::WriteFormat list; }
#else
#define __CFLOG_SMADDR2(list)	{ TCFFullModuleName __FullModName1; TCFFullModuleName __FullModName2; CCFLogIf::WriteFormat list; }
#endif


typedef TBuf8<40> TCFLogStr;

#define __CFLOG_OBJ(list)	{ TCFLogStr _b1; list; }
#define __CFLOG_OBJ2(list)	{ TCFLogStr _b1,_b2; list; }
#define __CFLOG_OBJ3(list)	{ TCFLogStr _b1,_b2,_b3; list; }

#else // defined(__CFLOG_ACTIVE)

#define __CFLOG_DECLARATION_VARIABLE
#define __CFLOG_DECLARATION_MEMBER		__CFLOG_DECLARATION_MEMBER_SIZE __noCFLog__; \
											inline void useNoCFLog() { __noCFLog__ = 0; }
#define __CFLOG_CREATEL
#define __CFLOG_DELETE					//(0)
#define __CFLOG_OPEN
#define __CFLOG_CLOSE
#define __CFLOG_CLEAR
#define __CFLOG_STMT(text)

#define __CFLOG_0(subsys,compnt,text)
#define __CFLOG_1(subsys,compnt,text,a)
#define __CFLOG_2(subsys,compnt,text,a,b)
#define __CFLOG_3(subsys,compnt,text,a,b,c)
#define __CFLOG_4(subsys,compnt,text,a,b,c,d)
#define __CFLOG_5(subsys,compnt,text,a,b,c,d,e)
#define __CFLOG_6(subsys,compnt,text,a,b,c,d,e,f)
#define __CFLOG_7(subsys,compnt,text,a,b,c,d,e,f,g)
#define __CFLOG_8(subsys,compnt,text,a,b,c,d,e,f,g,h)
#define __CFLOG_9(subsys,compnt,text,a,b,c,d,e,f,g,h,i)
#define __CFLOG(subsys,compnt,text)
#define __CFLOG_VA(subsys,compnt,fmt,va_list)
#define __CFLOG_VAR(args)
#define __CFLOG_HEXDUMP(subsys,compnt,data,length)
#define __CFLOG_BINARY(subsys,compnt,data)

#define __CFLOG_SMADDR(list)	{ }
#define __CFLOG_SMADDR2(list)	{ }

#define __CFLOG_OBJ(list)	{ }
#define __CFLOG_OBJ2(list)	{ }
#define __CFLOG_OBJ3(list)	{ }

#endif // defined(__CFLOG_ACTIVE)

/** Standard component tag for the Comms Framework. 
*/
__CFLOG_STMT(_LIT8(KLogCommsFw,"CommsFW");)

/** Standard component tag for the Comms Framework Messages. 
*/
__CFLOG_STMT(_LIT8(KLogMsg,"Message");)

/** Standard logging tag for Comms source code, for fine grained logging.
*/
__CFLOG_STMT(_LIT8(KLogCode,"Code");)

/** Standard component tag for Comms events, for course grained logging. 
*/
__CFLOG_STMT(_LIT8(KLogEvent,"Event");)

/** Standard component tag for Comms warnings. 
*/
__CFLOG_STMT(_LIT8(KLogWarning,"Warning");)

/** Special tag used when tag memory exhausted. 
*/
__CFLOG_STMT(_LIT8(KCFLogOom, "WARNING-CFLogOom");)

#endif // __CFLOG_H__


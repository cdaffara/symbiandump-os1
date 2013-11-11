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
// Logging macros for C32. These use the Comms Debug (Flogger V2) Utility.
// 
//

/**
 @file
 @internalComponent
*/


#ifndef C32LOG_H
#define C32LOG_H

#include <comms-infras/commsdebugutility.h>

#if (defined __FLOG_ACTIVE)

#include <c32comm.h>
#include <cs_port.h>
#include "COMMIPC.H"

class TC32Log
/**
@internalComponent
*/
	{
public:
 	IMPORT_C static void Printf(TBool aStatic, const TDesC8& aSubTag, TRefByValue<const TDesC> aFmt, ...);
 	IMPORT_C static void Printf(TBool aStatic, const TDesC8& aSubTag, TRefByValue<const TDesC8> aFmt, ...);
   	static void Printf(TBool aStatic, const TDesC8& aSubTag, TRefByValue<const TDesC> aFmt, VA_LIST& aList);
   	static void Printf(TBool aStatic, const TDesC8& aSubTag, TRefByValue<const TDesC8> aFmt, VA_LIST& aList);
	static const TDesC8& CommRoleStr(TCommRole aRole);
	static const TDesC8& CommAccessStr(const TCommAccess& aMode);
	static const TDesC& CommRoleStr16(TCommRole aRole);
	static const TDesC& CommAccessStr16(const TCommAccess& aMode);
	static const TDesC8& InternalCommAccessStr(TInternalCommAccess aMode);
	static const TDesC8& C32RequestStr(TInt aIPC);
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
	
	

/** Rootserver module tag, used for leak output logging
*/
_LIT8(KRSModule, "rsmodule");

/** Rootserver module leak logging tag
*/
_LIT8(KRSModuleLeak, "leak");


_LIT8(KC32SubSystem,   "C32SerComms");
_LIT8(KC32Bootup,      "Bootup");
_LIT8(KC32Shutdown,    "Shutdown");
_LIT8(KC32Warning,     "Warning");
_LIT8(KC32Client,      "Client");
_LIT8(KC32Dealer,	   "Dealer");
_LIT8(KC32Player,      "Player");
_LIT8(KC32CommsTrspMsg,"Transport");
_LIT8(KC32Detail,      "Detail");


#define C32LOG(Level,A)                 		TC32Log::Printf(EFalse, Level,A)
#define C32_STATIC_LOG(Level,A)                 TC32Log::Printf(ETrue, Level,A)


#define C32LOG1(Level,A)                		TC32Log::Printf(EFalse, Level,A)
#define C32LOG2(Level,A,B)              		TC32Log::Printf(EFalse, Level,A,B)
#define C32LOG3(Level,A,B,C)            		TC32Log::Printf(EFalse, Level,A,B,C)
#define C32LOG4(Level,A,B,C,D)          		TC32Log::Printf(EFalse, Level,A,B,C,D)
#define C32LOG5(Level,A,B,C,D,E)        		TC32Log::Printf(EFalse, Level,A,B,C,D,E)
#define C32LOG6(Level,A,B,C,D,E,F)      		TC32Log::Printf(EFalse, Level,A,B,C,D,E,F)
#define C32LOG7(Level,A,B,C,D,E,F,G)    		TC32Log::Printf(EFalse, Level,A,B,C,D,E,F,G)
#define C32LOG8(Level,A,B,C,D,E,F,G,H)  		TC32Log::Printf(EFalse, Level,A,B,C,D,E,F,G,H)

#define C32_STATIC_LOG1(Level,A)                TC32Log::Printf(ETrue, Level,A)
#define C32_STATIC_LOG2(Level,A,B)              TC32Log::Printf(ETrue, Level,A,B)
#define C32_STATIC_LOG3(Level,A,B,C)            TC32Log::Printf(ETrue, Level,A,B,C)
#define C32_STATIC_LOG4(Level,A,B,C,D)          TC32Log::Printf(ETrue, Level,A,B,C,D)
#define C32_STATIC_LOG5(Level,A,B,C,D,E)        TC32Log::Printf(ETrue, Level,A,B,C,D,E)
#define C32_STATIC_LOG6(Level,A,B,C,D,E,F)      TC32Log::Printf(ETrue, Level,A,B,C,D,E,F)
#define C32_STATIC_LOG7(Level,A,B,C,D,E,F,G)    TC32Log::Printf(ETrue, Level,A,B,C,D,E,F,G)
#define C32_STATIC_LOG8(Level,A,B,C,D,E,F,G,H)  TC32Log::Printf(ETrue, Level,A,B,C,D,E,F,G,H)

// C32LOGVA allows multi-param logging without having to specify number; useful if num params changed
// often during coding. To use, enclose the whole thing in a second set of brackets, eg:
// C32LOGVA((_L8("Params: %d %d %d"),param1,param2,param3));
#define C32LOGVA(args)                   args





#else  // __FLOG_ACTIVE

#define KC32Warning KNullDesC8
#define KC32Detail  KNullDesC8
#define KC32Bootup  KNullDesC8

#define C32LOG(Level,A)
#define C32LOG1(Level,A)               
#define C32LOG2(Level,A,B)             
#define C32LOG3(Level,A,B,C)           
#define C32LOG4(Level,A,B,C,D)         
#define C32LOG5(Level,A,B,C,D,E)       
#define C32LOG6(Level,A,B,C,D,E,F)     
#define C32LOG7(Level,A,B,C,D,E,F,G)   
#define C32LOG8(Level,A,B,C,D,E,F,G,H) 
#define C32LOGVA(args)                 

#define C32_STATIC_LOG(Level,A) 
#define C32_STATIC_LOG1(Level,A)                
#define C32_STATIC_LOG2(Level,A,B)              
#define C32_STATIC_LOG3(Level,A,B,C)            
#define C32_STATIC_LOG4(Level,A,B,C,D)          
#define C32_STATIC_LOG5(Level,A,B,C,D,E)        
#define C32_STATIC_LOG6(Level,A,B,C,D,E,F)      
#define C32_STATIC_LOG7(Level,A,B,C,D,E,F,G)    
#define C32_STATIC_LOG8(Level,A,B,C,D,E,F,G,H)  

class TC32Log
/**
@internalComponent
*/
 	{
public:
 	IMPORT_C static void Printf(TBool /* aStatic */, const TDesC8& /* aSubTag */, TRefByValue<const TDesC> /* aFmt */, ...);
 	IMPORT_C static void Printf(TBool /* aStatic */, const TDesC8& /* aSubTag */, TRefByValue<const TDesC8> /* aFmt */, ...);
 	};

#endif	//__FLOG_ACTIVE

#endif  // C32LOG_H

/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/




/**
 @file
 @internalTechnology 
*/

#ifndef HWRMTRACE_H
#define HWRMTRACE_H


#include <e32svr.h>
#include <comms-infras/commsdebugutility.h>

//define this macro to force all logging off.  Will require a rebuild after change.
//#define FORCE_TRACE_OFF 

//define the macro to enable RDebug traces.
//#define ENABLE_RDEBUG_TRACE

#ifdef _DEBUG 
	#ifndef FORCE_TRACE_OFF
		#ifndef DISABLE_COMPONENT_TRACE
			#define COMPONENT_TRACE_FLAG
		#endif //DISABLE_COMPONENT_TRACE
	#endif //FORCE_TRACE_OFF
#endif //_DEBUG


#ifdef COMPONENT_TRACE_FLAG

	_LIT8(KHwrmSubSystem, "HWRM");

	#if defined HWRM_SERVER
		_LIT8(KHwrmCompnt, "Server");
	#elif defined HWRM_CLIENT
		_LIT8(KHwrmCompnt, "Client");
	#elif defined HWRM_COMMON
		_LIT8(KHwrmCompnt, "Common");
	#elif defined HWRM_LIGHT
		_LIT8(KHwrmCompnt, "Light");
	#elif defined HWRM_EXTENDED_LIGHT
		_LIT8(KHwrmCompnt, "ExtendedLight");
	#elif defined HWRM_VIBRA
		_LIT8(KHwrmCompnt, "Vibra");
	#elif defined HWRM_POWER
		_LIT8(KHwrmCompnt, "Power");
	#elif defined HWRM_FMTX
		_LIT8(KHwrmCompnt, "FmTx");
	#elif HWRM_TARGET_MOD_PLUGIN
		_LIT8(KHwrmCompnt, "TargetModPlugin");
	#else
		_LIT8(KHwrmCompnt, "Unknown");
	#endif    

    
    #ifndef ENABLE_RDEBUG_TRACE
    //LOG Using CommsDebugUtility
    #define COMPONENT_TRACE1(A)                 __FLOG_STATIC0(KHwrmSubSystem,KHwrmCompnt,(A))
	#define COMPONENT_TRACE2(A,B)               __FLOG_STATIC1(KHwrmSubSystem,KHwrmCompnt,(A),(B))
	#define COMPONENT_TRACE3(A,B,C)             __FLOG_STATIC2(KHwrmSubSystem,KHwrmCompnt,(A),(B),(C))
	#define COMPONENT_TRACE4(A,B,C,D)           __FLOG_STATIC3(KHwrmSubSystem,KHwrmCompnt,(A),(B),(C),(D))
	#define COMPONENT_TRACE5(A,B,C,D,E)         __FLOG_STATIC4(KHwrmSubSystem,KHwrmCompnt,(A),(B),(C),(D),(E))
	#define COMPONENT_TRACE6(A,B,C,D,E,F)       __FLOG_STATIC5(KHwrmSubSystem,KHwrmCompnt,(A),(B),(C),(D),(E),(F))
	#define COMPONENT_TRACE7(A,B,C,D,E,F,G)     __FLOG_STATIC6(KHwrmSubSystem,KHwrmCompnt,(A),(B),(C),(D),(E),(F),(G))
	#define COMPONENT_TRACE8(A,B,C,D,E,F,G,H)   __FLOG_STATIC7(KHwrmSubSystem,KHwrmCompnt,(A),(B),(C),(D),(E),(F),(G),(H))    
	#define COMPONENT_TRACE9(A,B,C,D,E,F,G,H,I)   __FLOG_STATIC8(KHwrmSubSystem,KHwrmCompnt,(A),(B),(C),(D),(E),(F),(G),(H), (I))    

    #else 
    //LOG Using RDebug
    #define COMPONENT_TRACE1(A)                 RDebug::Print((A))
    #define COMPONENT_TRACE2(A,B)               RDebug::Print((A),(B))
    #define COMPONENT_TRACE3(A,B,C)             RDebug::Print((A),(B),(C))
    #define COMPONENT_TRACE4(A,B,C,D)           RDebug::Print((A),(B),(C),(D))
    #define COMPONENT_TRACE5(A,B,C,D,E)         RDebug::Print((A),(B),(C),(D),(E))
    #define COMPONENT_TRACE6(A,B,C,D,E,F)       RDebug::Print((A),(B),(C),(D),(E),(F))
    #define COMPONENT_TRACE7(A,B,C,D,E,F,G)     RDebug::Print((A),(B),(C),(D),(E),(F),(G))
    #define COMPONENT_TRACE8(A,B,C,D,E,F,G,H)   RDebug::Print((A),(B),(C),(D),(E),(F),(G),(H))    
    #define COMPONENT_TRACE9(A,B,C,D,E,F,G,H,I) RDebug::Print((A),(B),(C),(D),(E),(F),(G),(H), (I))            

    #endif //ENABLE_RDEBUG_TRACE
    

#else // _DEBUG || DISABLE_COMPONENT_TRACE

    #define COMPONENT_TRACE1(A)                 
	#define COMPONENT_TRACE2(A,B)               
	#define COMPONENT_TRACE3(A,B,C)             
	#define COMPONENT_TRACE4(A,B,C,D)           
	#define COMPONENT_TRACE5(A,B,C,D,E)         
	#define COMPONENT_TRACE6(A,B,C,D,E,F)       
	#define COMPONENT_TRACE7(A,B,C,D,E,F,G)     
	#define COMPONENT_TRACE8(A,B,C,D,E,F,G,H) 
	#define COMPONENT_TRACE9(A,B,C,D,E,F,G,H,I)  


#endif //COMPONENT_TRACE_FLAG

#endif // HWRMTRACE_H


//  End of File

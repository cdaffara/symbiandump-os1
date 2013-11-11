// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef _RPSSFLOG_H_
#define _RPSSFLOG_H_

/**
@file

Logging macros for RPS Slave. These use the Comms Debug / Flogger V2
Framework.

@internalComponent
 */

#include <comms-infras/commsdebugutility.h>
#include <e32debug.h>
#include <e32cons.h>

GLREF_D CConsoleBase* console; // write all your messages to this

_LIT(KNewLine, "\n");

#define LOGLINE1(A)      							\
	{												\
	CDULOGLINE1(A);									\
	RDEBUGPRINT1(A);								\
	console->Printf(A);							\
	console->Printf(KNewLine);						\
	}

#define LOGLINE2(A, B)      						\
	{												\
	CDULOGLINE2(A, B);								\
	RDEBUGPRINT2(A, B);								\
	console->Printf(A, B);							\
	console->Printf(KNewLine);						\
	}
	
#define LOGLINE3(A, B, C)      						\
	{												\
	CDULOGLINE3(A, B, C);							\
	RDEBUGPRINT3(A, B, C);							\
	console->Printf(A, B, C);						\
	console->Printf(KNewLine);						\
	}

#ifdef USE_RDEBUGLOGGING

#define RDEBUGPRINT1(A)			RDebug::Print((A))
#define RDEBUGPRINT2(A,B)		RDebug::Print((A), (B))
#define RDEBUGPRINT3(A,B,C)		RDebug::Print((A), (B), (C))

#else
		
#define RDEBUGPRINT1(A)			
#define RDEBUGPRINT2(A,B)		
#define RDEBUGPRINT3(A,B,C) 

#endif


#ifdef _DEBUG

_LIT8(KRpsSubSystem, "Rps");

_LIT8(KRpsSession, "Slave");


#define CDULOGLINE1(A)	               __FLOG_STATIC0(KRpsSubSystem,KRpsSession,(A))
#define CDULOGLINE2(A,B)               __FLOG_STATIC1(KRpsSubSystem,KRpsSession,(A),(B))
#define CDULOGLINE3(A,B,C)             __FLOG_STATIC2(KRpsSubSystem,KRpsSession,(A),(B),(C))
#define CDULOGLINE4(A,B,C,D)           __FLOG_STATIC3(KRpsSubSystem,KRpsSession,(A),(B),(C),(D))
#define CDULOGLINE5(A,B,C,D,E)         __FLOG_STATIC4(KRpsSubSystem,KRpsSession,(A),(B),(C),(D),(E))
#define CDULOGLINE6(A,B,C,D,E,F)       __FLOG_STATIC5(KRpsSubSystem,KRpsSession,(A),(B),(C),(D),(E),(F))
#define CDULOGLINE7(A,B,C,D,E,F,G)     __FLOG_STATIC6(KRpsSubSystem,KRpsSession,(A),(B),(C),(D),(E),(F),(G))
#define CDULOGLINE8(A,B,C,D,E,F,G,H)   __FLOG_STATIC7(KRpsSubSystem,KRpsSession,(A),(B),(C),(D),(E),(F),(G),(H))

                              
#else  // _DEBUG

#define CDULOGLINE1(A)
#define CDULOGLINE2(A,B)
#define CDULOGLINE3(A,B,C)
#define CDULOGLINE4(A,B,C,D)
#define CDULOGLINE5(A,B,C,D,E)
#define CDULOGLINE6(A,B,C,D,E,F)
#define CDULOGLINE7(A,B,C,D,E,F,G)
#define CDULOGLINE8(A,B,C,D,E,F)

                                 
#endif	//_DEBUG

//console logging

#endif  // _RPSSFLOG_H_

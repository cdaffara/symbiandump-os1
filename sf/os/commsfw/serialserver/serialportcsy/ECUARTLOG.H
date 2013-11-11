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
// Logging macros for ECUART. These use the Comms Debug / Flogger V2 Utility.
// PLEASE NOTE: This file is new for C32 V2 but is included in both new and old C32 builds.
// It is needed for the old builds to implement a cunning ruse so that all the new logging in ecuart.cpp
// resolves to nothing for old builds and thus save having to configure the logging lines individually.
// 
//

/**
 @file
 @internalComponent
*/
#ifndef ECUARTLOG_H
#define ECUARTLOG_H

// has to be included to get correct "__FLOG_ACTIVE"
#include <comms-infras/commsdebugutility.h>

// logging code in ECUART is not configured, so configure it here!
#if defined __FLOG_ACTIVE 

#include "ECUART.H"
#include "C32LOG.H"

_LIT8(KC32ECUART,  "ECUART");

class TECUARTLOG
	{
public:
	static const TDesC8& CommFaultStr(const TCommFault& aFault);
	
	
	};

#define LOGECUART(A)                  C32LOG1(KC32ECUART,(A))
#define LOGECUART1(A)                 C32LOG1(KC32ECUART,(A))
#define LOGECUART2(A,B)               C32LOG2(KC32ECUART,(A),(B))
#define LOGECUART3(A,B,C)             C32LOG3(KC32ECUART,(A),(B),(C))
#define LOGECUART4(A,B,C,D)           C32LOG4(KC32ECUART,(A),(B),(C),(D))
#define LOGECUART5(A,B,C,D,E)         C32LOG5(KC32ECUART,(A),(B),(C),(D),(E))
#define LOGECUART6(A,B,C,D,E,F)       C32LOG6(KC32ECUART,(A),(B),(C),(D),(E),(F))
#define LOGECUART7(A,B,C,D,E,F,G)     C32LOG7(KC32ECUART,(A),(B),(C),(D),(E),(F),(G))
#define LOGECUART8(A,B,C,D,E,F,G,H)   C32LOG8(KC32ECUART,(A),(B),(C),(D),(E),(F),(G),(H))

#define LOGECUART_LOGGING_ACTIVE
#else  // __FLOG_ACTIVE && SYMBIAN_C32_SERCOMMS_V2

#define LOGECUART(A)
#define LOGECUART1(A)
#define LOGECUART2(A,B)
#define LOGECUART3(A,B,C)
#define LOGECUART4(A,B,C,D)
#define LOGECUART5(A,B,C,D,E)
#define LOGECUART6(A,B,C,D,E,F)
#define LOGECUART7(A,B,C,D,E,F,G)
#define LOGECUART8(A,B,C,D,E,F,G,H)

#define LOGECUART_STMT(S)

#endif	//__FLOG_ACTIVE && SYMBIAN_C32_SERCOMMS_V2

#endif  // ECUARTLOG_H

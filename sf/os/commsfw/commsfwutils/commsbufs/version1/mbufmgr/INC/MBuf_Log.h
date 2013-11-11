// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Declares MBuf logging
// 
//

#ifndef MBUF_LOG
#define MBUF_LOG

#include "comms-infras/commsdebugutility.h" // for __FLOG*
#include "cflog.h"
__FLOG_STMT(_LIT8(KSubsysMBufMgr, "MBufMgr");) // subsystem name
__FLOG_STMT(_LIT8(KComponentPerformance, "performance");)	// component name - used for any sub-optimal performance behaviuor

// CDU and flogger have multiple conflicts (eg. KLogBufferSize), consequently theirs headers can not both be included at the same time
// - eg. tcpip6 uses flogger & mbufs
// - solutions/workarounds;
//   1. __FLOG_STATIC* macros --> open & close the CDU for every logging statement
//	    +ve - does not use __FLOG_DECLARATION_MEMBER in the class decleration (ie. typically public)
//      -ve - very slow
//   2. __CFLOG* macros --> uses __FLOG_DECLARATION_MEMBER stored within commsfw (requires a TLS lookup for every logging statement)
//	    +ve - does not use __FLOG_DECLARATION_MEMBER in the class decleration (ie. typically public)
//      -ve - slow, but not as slow as static
//   3. __CFLOG* macros --> uses __FLOG_DECLARATION_MEMBER stored within class
//	    +ve - fast, no TLS or open/close of CDU
//      -ve - clashes arise when users of the class include the header file if they use flogger instead of CDU
//   4. As per 3, except that __FLOG_DECLARATION_MEMBER is not used; TInt iFlogger added to classes & __logger__ defined (refer below)
//	    +ve - no clashes for users that use flogger
//      -ve - requires a redecleration of __logger__ --> ideally this could/should be made available by a CDU header file (but alas is not)
#define __logger__ (reinterpret_cast<RFileLogger&>(iLogger))

#endif	// MBUF_LOG

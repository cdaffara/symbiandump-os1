//
// * Copyright 2004 Neusoft America Inc.
// * All rights reserved.
// * This component and the accompanying materials are made available
// * under the terms of the Eclipse Public License v1.0
// * which accompanies this distribution, and is available
// * at the URL "http://www.eclipse.org/legal/epl-v10.html".
// *
// * Contributors:
// * Keith Collins (Neusoft America Inc.)  original software development and additional code and modifications.
// * Thomas Gahagen (Neusoft America Inc.)  additional code and modifications.
// * Zhen Yuan (Neusoft America Inc.)  additional code and modifications.
// *
// * Description: This file defines the CCsyDebugLogger class for the CSY and the macros
// *              to use for logging debug messages to either a serial port or a log file.
//

// CsyDebugLogger.h

/** @file CsyDebugLogger.h
 *
 */

#ifndef __CSYDEBUGLOGGER_H__
#define __CSYDEBUGLOGGER_H__

#ifdef _DEBUG

// To reduce debugging - comment out one or more of these defines

#define __DEBUGLEVELCRITICAL__  // Critical logging (erroneous conditions)
#define __DEBUGLEVELMAJOR__     // Major (important data used in the Csy)
#define __DEBUGLEVELMINOR__     // Minor (important conditions)
#define __DEBUGLEVELDETAILED__  // Detailed (comments & func >> << logging)

// Route for logging - select one of these
#define __DEBUGLOGFILE__
//#define __DEBUGSERIALPORT__

#define LOGHEADER()							CCsyDebugLogger::WriteHeader()
#else
#define LOGHEADER()
#endif

#ifdef __DEBUGLEVELCRITICAL__
#define _LOG_L1C1(AAA)						CCsyDebugLogger::Write(_L8(AAA))
#define _LOG_L1C2(AAA, BBB)					CCsyDebugLogger::WriteFormat(_L8(AAA),BBB)
#define _LOG_L1C3(AAA, BBB, CCC)			CCsyDebugLogger::WriteFormat(_L8(AAA),BBB,CCC)
#else
#define _LOG_L1C1(AAA)
#define _LOG_L1C2(AAA, BBB)					(void) BBB;
#define _LOG_L1C3(AAA, BBB, CCC)			(void) BBB; (void) CCC;
#endif

#ifdef __DEBUGLEVELMAJOR__
#define _LOG_L2C1(AAA)						CCsyDebugLogger::Write(_L8(AAA))
#define _LOG_L2C2(AAA, BBB)					CCsyDebugLogger::WriteFormat(_L8(AAA),BBB)
#define _LOG_L2C3(AAA, BBB, CCC)			CCsyDebugLogger::WriteFormat(_L8(AAA),BBB,CCC)
#else
#define _LOG_L2C1(AAA)
#define _LOG_L2C2(AAA, BBB)					(void) BBB;
#define _LOG_L2C3(AAA, BBB, CCC)			(void) BBB; (void) CCC;
#endif

#ifdef __DEBUGLEVELMINOR__
#define _LOG_L3C1(AAA)						CCsyDebugLogger::Write(_L8(AAA))
#define _LOG_L3C2(AAA, BBB)					CCsyDebugLogger::WriteFormat(_L8(AAA),BBB)
#define _LOG_L3C3(AAA, BBB, CCC)			CCsyDebugLogger::WriteFormat(_L8(AAA),BBB,CCC)
#else
#define _LOG_L3C1(AAA)
#define _LOG_L3C2(AAA, BBB)					(void) BBB;
#define _LOG_L3C3(AAA, BBB, CCC)			(void) BBB; (void) CCC;
#endif

#ifdef __DEBUGLEVELDETAILED__
#define _LOG_L4C1(AAA)						CCsyDebugLogger::Write(_L8(AAA))
#define _LOG_L4C2(AAA, BBB)					CCsyDebugLogger::WriteFormat(_L8(AAA),BBB)
#define _LOG_L4C3(AAA, BBB, CCC)			CCsyDebugLogger::WriteFormat(_L8(AAA),BBB,CCC)
#else
#define _LOG_L4C1(AAA)
#define _LOG_L4C2(AAA, BBB)					(void) BBB;
#define _LOG_L4C3(AAA, BBB, CCC)			(void) BBB; (void) CCC;
#endif



// The following logging macros are the same as those above with the following exception:
// The arguments are removed from the code when the logging is removed.
// This is needed to remove warnings where otherwise unused variables are logged.

#ifdef __DEBUGLEVELCRITICAL__
#define _LOG_L1C1E(AAA)						CCsyDebugLogger::Write(_L8(AAA))
#define _LOG_L1C2E(AAA, BBB)				CCsyDebugLogger::WriteFormat(_L8(AAA),BBB)
#define _LOG_L1C3E(AAA, BBB, CCC)			CCsyDebugLogger::WriteFormat(_L8(AAA),BBB,CCC)
#else
#define _LOG_L1C1E(AAA)
#define _LOG_L1C2E(AAA, BBB)
#define _LOG_L1C3E(AAA, BBB, CCC)
#endif

#ifdef __DEBUGLEVELMAJOR__
#define _LOG_L2C1E(AAA)						CCsyDebugLogger::Write(_L8(AAA))
#define _LOG_L2C2E(AAA, BBB)				CCsyDebugLogger::WriteFormat(_L8(AAA),BBB)
#define _LOG_L2C3E(AAA, BBB, CCC)			CCsyDebugLogger::WriteFormat(_L8(AAA),BBB,CCC)
#else
#define _LOG_L2C1E(AAA)
#define _LOG_L2C2E(AAA, BBB)
#define _LOG_L2C3E(AAA, BBB, CCC)
#endif

#ifdef __DEBUGLEVELMINOR__
#define _LOG_L3C1E(AAA)						CCsyDebugLogger::Write(_L8(AAA))
#define _LOG_L3C2E(AAA, BBB)				CCsyDebugLogger::WriteFormat(_L8(AAA),BBB)
#define _LOG_L3C3E(AAA, BBB, CCC)			CCsyDebugLogger::WriteFormat(_L8(AAA),BBB,CCC)
#else
#define _LOG_L3C1E(AAA)
#define _LOG_L3C2E(AAA, BBB)
#define _LOG_L3C3E(AAA, BBB, CCC)
#endif

#ifdef __DEBUGLEVELDETAILED__
#define _LOG_L4C1E(AAA)						CCsyDebugLogger::Write(_L8(AAA))
#define _LOG_L4C2E(AAA, BBB)				CCsyDebugLogger::WriteFormat(_L8(AAA),BBB)
#define _LOG_L4C3E(AAA, BBB, CCC)			CCsyDebugLogger::WriteFormat(_L8(AAA),BBB,CCC)
#else
#define _LOG_L4C1E(AAA)
#define _LOG_L4C2E(AAA, BBB)
#define _LOG_L4C3E(AAA, BBB, CCC)
#endif

#include <flogger.h>

class CCsyDebugLogger : public CBase
	{
public:
	static void WriteHeader();
	static void Write(const TDesC8& aText);
	static void WriteFormat(TRefByValue<const TDesC8> aFmt, ...);
	};

#endif	// __CSYDEBUGLOGGER_H__
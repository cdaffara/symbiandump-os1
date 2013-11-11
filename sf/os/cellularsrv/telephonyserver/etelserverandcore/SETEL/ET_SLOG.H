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
// Logging macros for ETel.  These use the Comms Debug Utility
// framework (aka Flogger V2).
// To enable logging create a file called "commsdbg.ini" in the Symbian OS
// "C:\Logs\" directory which contains the following lines:
// LOG ETel *
// NB: When using hardware, as oposed to emulator, the user also has the
// option of placing this ini file in the device's "z:\resource" folder,
// which can enable logging from the early stages of system start-up.
// (This is simply because you can't build a ROM with c:\logs\..., unless
// you are using a persistent c:\ drive.)
// 
//

/**
 @file
 @internalAll
*/

#ifndef __ET_SLOG_H__
#define __ET_SLOG_H__

#include <comms-infras/commsdebugutility.h>

#ifdef __FLOG_ACTIVE
/**
Make sure ETELLOGGER is defined if UREL debugging is required.
@see LOGTEXTREL
*/
#define ETELLOGGER	
#endif // __FLOG_ACTIVE

#ifdef ETELLOGGER
_LIT8(KETelSubSystem, "ETel");
#define CMNFLOG1(N,A)                 { \
                                      _LIT8(KLogString, A); \
                                      __FLOG_STATIC0(KETelSubSystem,(N),KLogString); \
                                      }
#define CMNFLOG2(N,A,B)               { \
                                      _LIT8(KLogString, A); \
                                      __FLOG_STATIC1(KETelSubSystem,(N),KLogString,(B)); \
                                      }
#define CMNFLOG3(N,A,B,C)             { \
                                      _LIT8(KLogString, A); \
                                      __FLOG_STATIC2(KETelSubSystem,(N),KLogString,(B),(C)); \
                                      }
#define CMNFLOG4(N,A,B,C,D)           { \
                                      _LIT8(KLogString, A); \
                                      __FLOG_STATIC3(KETelSubSystem,(N),KLogString,(B),(C),(D)); \
                                      }

#endif // ETELLOGGER


#ifdef __FLOG_ACTIVE

_LIT8(KETelCommon,  "udeb");

#ifdef  ETELLOGGER
#define LOGTEXT(AAA)	          CMNFLOG1(KETelCommon, AAA)
#define LOGTEXT2(AAA,BBB)	      CMNFLOG2(KETelCommon, AAA, BBB)
#define LOGTEXT3(AAA,BBB,CCC)	  CMNFLOG3(KETelCommon, AAA, BBB, CCC)
#define LOGTEXT4(AAA,BBB,CCC,DDD) CMNFLOG4(KETelCommon, AAA, BBB, CCC, DDD)
#else
#define LOGTEXT(AAA)
#define LOGTEXT2(AAA,BBB)
#define LOGTEXT3(AAA,BBB,CCC)
#define LOGTEXT4(AAA,BBB,CCC,DDD)
#endif // ETELLOGGER

#define LOGTEXTREL(AAA)

#else // release version and therefore no logging unless ETel is built with ETELLOGGER defined

_LIT8(KETelURelCommon,  "urel");

#define LOGTEXT(AAA)
#define LOGTEXT2(AAA,BBB)
#define LOGTEXT3(AAA,BBB,CCC)
#define LOGTEXT4(AAA,BBB,CCC,DDD)

#ifdef  ETELLOGGER
#define LOGTEXTREL(AAA)	          CMNFLOG1(KETelURelCommon, AAA)
#else
#define LOGTEXTREL(AAA)
#endif // ETELLOGGER

#endif // __FLOG_ACTIVE

#endif // __ET_SLOG_H__

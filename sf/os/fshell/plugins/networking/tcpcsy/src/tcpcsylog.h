// tcpcsylog.h
// 
// Copyright (c) 2008 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#ifndef __TCPCSYLOG_H__
#define __TCPCSYLOG_H__

#include <e32std.h>
#include <comms-infras/commsdebugutility.h>

// The following 4 macros define the level of logging in udeb.
// The macros are (un)defined in the tcpcsy.mmp file.

#ifdef __FLOG_ACTIVE

_LIT8(KDebugSubSystem, "fshell");

#ifdef LOG_CSY_EVENTS
_LIT8(KDebugCategoryEvents, "tcpcsy_event");
_LIT8(KDebugCategoryEventsStatic, "tcpcsy_static");
#define LOG(A,ARGS...)         {_LIT8(KA,A);this->iFactory.iEventLogger.WriteFormat(KA(),##ARGS);}
#define LOG_STATIC(A,ARGS...)  {_LIT8(KA,A);__FLOG_STATIC_VA((KDebugSubSystem(),KDebugCategoryEventsStatic(),KA(),##ARGS));}
#else
#define LOG(A,ARGS...)
#define LOG_STATIC(A,ARGS...)
#endif // LOG_CSY_EVENTS

#ifdef LOG_CSY_TX
_LIT8(KDebugCategoryTx, "tcpcsy_tx");
#define LOGTx(A,ARGS...)       {_LIT8(KA,A);this->iFactory.iTxLogger.WriteFormat(KA(),##ARGS);}
#else
#define LOGTx(A,ARGS...)
#endif // LOG_CSY_TX

#ifdef LOG_CSY_RX
_LIT8(KDebugCategoryRx, "tcpcsy_rx");
#define LOGRx(A,ARGS...)       {_LIT8(KA,A);this->iFactory.iRxLogger.WriteFormat(KA(),##ARGS);}
#else
#define LOGRx(A,ARGS...)
#endif // LOG_CSY_RX

#else

#define LOG(A,ARGS...)
#define LOGTx(A,ARGS...)
#define LOGRx(A,ARGS...)
#define LOG_STATIC(A,ARGS...)

#endif // __FLOG_ACTIVE


#endif // __TCPCSYLOG_H__

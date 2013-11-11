// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Definitions of performance tracing macros
// 
//

#ifndef __WS_TRACES_H__
#define __WS_TRACES_H__

#include <e32btrace.h>

#ifdef USE_TRACECORE_CATEGORY
enum TWsPerformanceTraceCategories 
{
	EWsPerformanceTraceProcessRawEvent = 0x10,
	EWsPerformanceTraceQueueEvent,
	EWsPerformanceTraceGetEvent,
	EWsPerformanceTraceEventReady,

	EWsPerformanceTraceTriggerRedraw = 0x20,
	EWsPerformanceTraceGetRedraw,
	EWsPerformanceTraceRedrawReady,
	
	EWsPerformanceTraceStartRedraw = 0x30,
	EWsPerformanceTraceStopRedraw
};
#else
enum TWsPerformanceTraceCategories 
{
	EWsPerformanceTraceProcessRawEvent = 130,
	EWsPerformanceTraceQueueEvent,
	EWsPerformanceTraceGetEvent,
	EWsPerformanceTraceEventReady,

	EWsPerformanceTraceTriggerRedraw = 140,
	EWsPerformanceTraceGetRedraw,
	EWsPerformanceTraceRedrawReady,
	
	EWsPerformanceTraceStartRedraw = 150,
	EWsPerformanceTraceStopRedraw
};
#endif

#ifndef USE_WS_PERFORMANCE_TRACING
	#define WS_TRACE0(category) 
	#define WS_TRACE_CONTEXT0(category)
#else
	// TraceCore BTrace Handler
	#ifdef USE_TRACECORE_CATEGORY
		#define WS_TRACE0(category) BTrace4(0xA0, 0, category)
		#define WS_TRACE_CONTEXT0(category) BTraceContext4(0xA0, 0, category)
	#else
		#define WS_TRACE0(category) BTrace0(category, 0)
		#define WS_TRACE_CONTEXT0(category) BTraceContext0(category, 0)
	#endif // USE_TRACECORE_CATEGORY
#endif // USE_WS_PERFORMANCE_TRACING

#define WS_TRACE_SERVER_PROCESSRAWEVENT() WS_TRACE0(EWsPerformanceTraceProcessRawEvent)
#define WS_TRACE_SERVER_QUEUEEVENT() WS_TRACE0(EWsPerformanceTraceQueueEvent)
#define WS_TRACE_CLIENT_GETEVENT() WS_TRACE_CONTEXT0(EWsPerformanceTraceGetEvent)
#define WS_TRACE_CLIENT_EVENTREADY() WS_TRACE_CONTEXT0(EWsPerformanceTraceEventReady)

#define WS_TRACE_SERVER_TRIGGERREDRAW() WS_TRACE0(EWsPerformanceTraceTriggerRedraw)
#define WS_TRACE_CLIENT_GETREDRAW() WS_TRACE_CONTEXT0(EWsPerformanceTraceGetRedraw)
#define WS_TRACE_CLIENT_REDRAWREADY() WS_TRACE_CONTEXT0(EWsPerformanceTraceRedrawReady)

#define WS_TRACE_SERVER_STARTDRAW() WS_TRACE0(EWsPerformanceTraceStartRedraw)
#define WS_TRACE_SERVER_STOPDRAW() WS_TRACE0(EWsPerformanceTraceStopRedraw)

#endif

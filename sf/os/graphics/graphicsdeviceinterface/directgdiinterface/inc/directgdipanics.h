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
// Panic and logging functions common to both the DirectGDI adaptation layer and
// associated modules/interfaces.
// 
//

#ifndef DIRECTGDIPANICS_H
#define DIRECTGDIPANICS_H

#include <graphics/directgdipanics.inl>
#include <e32cmn.h>


/**
@publishedPartner
@prototype
*/

#ifndef SYMBIAN_PANIC_CATEGORY
#error SYMBIAN_PANIC_CATEGORY not defined.
#endif

/** Helper macros for the assert and panic macros.
 */
#define GRAPHICS_SOURCE_FILENAME(sourceFileName)	_LIT(KSourceFileName, sourceFileName);
#define GRAPHICS_ASSERT_CONDITION(condition)		_LIT(KPanicCondition, condition);
#define GRAPHICS_LOG_MESSAGE(literalMessage)		_LIT(KLogMessage, # literalMessage);

#define GRAPHICS_ASSERT_ALWAYS(condition, panicCode) \
			if (!(condition)) \
				{ \
				GRAPHICS_SOURCE_FILENAME(__FILE__); \
				GRAPHICS_ASSERT_CONDITION(#condition); \
				DirectGdi::PanicWithDebugLog(SYMBIAN_PANIC_CATEGORY, panicCode, KSourceFileName, __LINE__, &KPanicCondition); \
				}

#define GRAPHICS_PANIC_ALWAYS(panicCode) \
		{ \
		GRAPHICS_SOURCE_FILENAME(__FILE__); \
		DirectGdi::PanicWithDebugLog(SYMBIAN_PANIC_CATEGORY, panicCode, KSourceFileName, __LINE__, NULL); \
		}

#define GRAPHICS_LOG_ALWAYS(message) \
		{ \
		GRAPHICS_SOURCE_FILENAME(__FILE__);\
		GRAPHICS_LOG_MESSAGE(message); \
		DirectGdi::Log(KSourceFileName, __LINE__, KLogMessage); \
		}

#define GRAPHICS_LOGD_ALWAYS(message) \
	{ \
	GRAPHICS_SOURCE_FILENAME(__FILE__);\
	DirectGdi::Log(KSourceFileName, __LINE__, message); \
	} 

/**
Define "debug only" macros ONLY when this is a debug build.
*/
#ifdef _DEBUG
	#define GRAPHICS_LOG_DEBUG(literalMessage)			GRAPHICS_LOG_ALWAYS(literalMessage)	 // Paste parameter literally to log
	#define GRAPHICS_LOGD_DEBUG(TDesCRefMessage)			GRAPHICS_LOGD_ALWAYS(TDesCRefMessage) // Treat message as const TDesC&
	#define GRAPHICS_ASSERT_DEBUG(condition, panicCode) 	GRAPHICS_ASSERT_ALWAYS(condition, panicCode)
	#define GRAPHICS_PANIC_DEBUG(panicCode)				GRAPHICS_PANIC_ALWAYS(panicCode)
#else
	#define GRAPHICS_LOG_DEBUG(literalMessage)
	#define GRAPHICS_LOGD_DEBUG(TDesCRefMessage)
	#define GRAPHICS_ASSERT_DEBUG(condition, panicCode)
    #define GRAPHICS_PANIC_DEBUG(panicCode)
#endif // _DEBUG


// Cannot enable DirectGDI calltrace unless in debug mode.
#ifndef _DEBUG
#undef ENABLE_DIRECTGDI_CALLTRACE
#endif

#ifdef ENABLE_DIRECTGDI_CALLTRACE
	#define GRAPHICS_TRACE(functionName)				RDebug::Print(_L(functionName));
	#define GRAPHICS_TRACE1(functionName, p)		    RDebug::Printf(functionName, p);
	#define GRAPHICS_TRACE2(functionName, p1, p2)		RDebug::Printf(functionName, p1, p2);
#else
	#define GRAPHICS_TRACE(functionName)
	#define GRAPHICS_TRACE1(functionName, p)
	#define GRAPHICS_TRACE2(functionName, p1, p2)
#endif 

#endif // DIRECTGDIPANICS_H

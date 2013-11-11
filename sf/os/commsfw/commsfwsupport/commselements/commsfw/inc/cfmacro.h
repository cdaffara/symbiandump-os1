// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @publishedPartner
 @released
*/

#ifndef SYMBIAN_NETWORKING_CFMACRO_H__
#define SYMBIAN_NETWORKING_CFMACRO_H__

#include <e32debug.h>
#include <e32def.h>
#include <e32std.h>


// Enable logging for debug builds or builds with the comms flogger enabled - must be defined before including e32utrace.h
#if (defined(_DEBUG) || defined(__FLOG_ACTIVE)) && !defined(SYMBIAN_TRACE_ENABLE)
#define SYMBIAN_TRACE_ENABLE
#endif
#include <e32utrace.h>


#if !defined(VERIFY_RESULT)
/**
In UDEB builds ASSERT()s that c==q. In UREL builds simply executes c without any test.
This contrasts to the use of ASSERT(), where the condition is not executed at all in UREL.
*/
#if defined(_DEBUG)
// The compiler may warn about discarded side-effects of VERIFY() in UREL builds (seen with CW when
// comparing to a non-NULL pointer). If so try using VERIFY_RESULT() instead,
// eg "VERIFY(foo() == NULL)" => "VERIFY_RESULT(foo(), NULL)"
#define VERIFY(c) ASSERT(c)
#define VERIFY_RESULT(c,d) ASSERT(c==d)
#else
#define VERIFY(c) ((void)(c))
#define VERIFY_RESULT(c,d) ((void)(c))
#endif

#endif

// We use our own debugger macro which checks to see if a debugger is present and does not trigger
// a breakpoint if one is not.  This prevents a needless crash due to an unhandled structured
// exception.
#if defined _DEBUG && defined (__WINS__)
	// Win32 function to check to see if a debugger has been attached to the emulator process
	// introduced in Windows 2000.
	extern "C" __declspec(dllimport) int __stdcall IsDebuggerPresent(void);

	#define __CF_BREAK_IF_DEBUGGER() \
		{ \
		if(::IsDebuggerPresent() && User::JustInTime()) \
			{ \
			__BREAKPOINT(); \
			} \
		}
#else
	#define __CF_BREAK_IF_DEBUGGER()
#endif

// Define this to enable assertion logging (e.g., assertion expression, file, line number and function information).
// in debug builds.  RDebug and UTrace can be enabled as log sinks independently.  We current only enable assertion
// logging for the emulator and the x86gcc SMP reference platform to avoid bloating debug binaries with file and
// function name strings causing us to exceed the ROM budget.
//
// Enabling/disabling assertion logging requires the rebuild of all binaries which use the assertion macros in order
// for the change to take effect.
//
// Assertion logging cannot be enabled in release builds regardless of whether or not
// __COMMSFW_ENABLE_DEBUG_ASSERTION_LOGGING is defined.
#if defined __WINS__ || defined __X86GCC__
	#define __COMMSFW_ENABLE_DEBUG_ASSERTION_LOGGING
#endif

// Define this to output assertion logs to the debugport (does not do anything if assertion logging is not active).
#define __COMMSFW_ENABLE_ASSERTION_LOGSINK_DEBUGPORT

// Define this to output assertion logs to UTrace (does not do anything if assertion logging is not active).
#ifdef SYMBIAN_TRACE_ENABLE
	#define __COMMSFW_ENABLE_ASSERTION_LOGSINK_UTRACE
#endif

#ifdef _DEBUG

#ifdef __COMMSFW_ENABLE_DEBUG_ASSERTION_LOGGING

#define __COMMSFW_DEBUG_ASSERTION_LOGGING_ACTIVE

#undef __ASSERT_DEBUG
#undef __ASSERT_ALWAYS

// ASSERT macros that log information about an assertion after it fires.
// They also trigger a breakpoint if the emulator is running under a debugger.
#define __ASSERT_DEBUG(c,p) (void)((c)||(::CFAbout_To_Panic_Thread(#c, __FILE__, __LINE__, __PRETTY_FUNCTION__),p,0))
#define __ASSERT_ALWAYS(c,p) (void)((c)||(::CFAbout_To_Panic_Thread(#c, __FILE__, __LINE__, __PRETTY_FUNCTION__),p,0))

inline void CFAbout_To_Panic_Thread(const char* aExpression, const char* aFile, TInt aLine, const char* aFunc)
	{
	_LIT8(KAssertionLogEntry, "ERROR! Assertion failed: \"%S\" in file %S on line %d in function %S");
	TPtrC8 expressionPtr(reinterpret_cast<const TUint8*>(aExpression));
	TPtrC8 filePtr(reinterpret_cast<const TUint8*>(aFile));
	TPtrC8 funcPtr(reinterpret_cast<const TUint8*>(aFunc));

	// Log to the debugport.
	#ifdef __COMMSFW_ENABLE_ASSERTION_LOGSINK_DEBUGPORT
		RDebug::Printf(reinterpret_cast<const char*>(TPtrC8(KAssertionLogEntry).Ptr()), &expressionPtr, &filePtr, aLine, &funcPtr);
	#endif

	// Log to UTrace.
	#ifdef __COMMSFW_ENABLE_ASSERTION_LOGSINK_UTRACE
	    enum
	        {
	        KPrimaryFilter = 194, // server den
	        KMaxLogTextLength = 250
	        };

	    class TLogIgnoreOverflow8 : public TDes8Overflow
	        {
	        public:
	            void Overflow(TDes8& /*aDes*/) { }
	        };

	    TBuf8<KMaxLogTextLength> buf;
    	TLogIgnoreOverflow8 overflowHandler;
	    buf.Format(KAssertionLogEntry, &expressionPtr, &filePtr, aLine, &funcPtr, &overflowHandler);

		UTracePfAny(KPrimaryFilter, KText, ETrue, EFalse, buf.Length(), buf.Ptr(), buf.Length());
	#endif

	// Break into the debugger if one is present and just in time debugging is enabled.
	__CF_BREAK_IF_DEBUGGER();
	}

// For compatability with the non-logging emulator versions of the macro if binaries are mismatched.
#if defined (__WINS__)
	inline void CFAbout_To_Panic_Thread()
		{
		// Break into the debugger if one is present and just in time debugging is enabled.
		__CF_BREAK_IF_DEBUGGER();
		}
#endif // __WINS__

#elif defined __WINS__ // no point in redefining the assertion macros if assertion logging is disabled and the emulator is not the build target

#undef __ASSERT_DEBUG
#undef __ASSERT_ALWAYS

// Assertion macros which trigger a breakpoint if the emulator is running under a debugger.
// (non-logging versions).
#define __ASSERT_DEBUG(c,p) (void)((c)||(CFAbout_To_Panic_Thread(),p,0))
#define __ASSERT_ALWAYS(c,p) (void)((c)||(CFAbout_To_Panic_Thread(),p,0))

// Break into the debugger if one is present and just in time debugging is enabled.
inline void CFAbout_To_Panic_Thread()
	{
	__CF_BREAK_IF_DEBUGGER();
	}

#endif // __COMMSFW_ENABLE_DEBUG_ASSERTION_LOGGING
#endif // _DEBUG

/** Mark-up macro, shows that we explicitly rely upon the statement enclosed to be atomic without the use of locks. An example
is the write of a (32 bit) pointer or counter. Architectural changes might invalidate this behaviour, eg SMP, in which case
we can find such code through this macro
*/
#define NETWORKING_ATOMIC(s) s


#endif	// SYMBIAN_NETWORKING_CFMACRO_H__


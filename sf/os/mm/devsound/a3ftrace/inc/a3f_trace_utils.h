/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef _A3F_TRACE_UTILS_H_
#define _A3F_TRACE_UTILS_H_

//  INCLUDES
#include <a3f/a3f_trace_types.h>

#define DP_ASSERT(_assertion)  __DP_ASSERT_DBG(_assertion)


#ifdef _DEBUG

    #ifndef DISABLE_SYNTAX_CHECK
        #define _MARK_ENTRY() _dc.inOk=ETrue
        #define _DOINCHK() _dc.DoInChk()
        #define _CHK_MULTIIN() _dc.ChkMultiIn()
        #define _CHK_MULTIOUT() _dc.ChkMultiOut()
        #define _MARK_EXIT() _dc.outOk=ETrue
    #else
        #define _MARK_ENTRY()
        #define _DOINCHK()
        #define _CHK_MULTIIN()
        #define _CHK_MULTIOUT()
        #define _MARK_EXIT()
    #endif // DISABLE_SYNTAX_CHECK

    #ifndef DISABLE_GROUP_CHECKS
        #include <a3f/a3f_trace_heap.h>
        #define _CHK_GRP() if ( (!TraceHeap::IsMaskOn(TRACENFO[_dc.iId].iGrpId)) || ((!_dc.iApi) && TraceHeap::IsApiOnly()) ) { break; }
        #define _CREATE_MASK() TRAP_IGNORE(TraceHeap::CreateL(CtxDefaultGroups))
        #define _CHK_LEVEL(level) if ( TraceHeap::IsBelowLevel(level) ) { break; }
    #else
        #define _CHK_GRP()
        #define _CREATE_MASK()
        #define _CHK_LEVEL(level)
    #endif // DISABLE_GROUP_CHECKS

	#ifdef SYMBIAN_MULTIMEDIA_A3F_ENABLE_LOGGING

		#ifndef __KERNEL_MODE__
			#define DP_CONTEXT(_fn, _id, _vis) _TTraceCtx _dc((TText*)L ## #_fn, _id, _vis, (TUint)this)
			#define DP_STATIC_CONTEXT(_fn, _id, _vis) _CREATE_MASK(); _TTraceCtx _dc((TText*)L ## #_fn, _id, _vis, 0)

			#define DP_IN() do { _CHK_GRP(); _CHK_MULTIIN(); _DPPRINTER(_T(L ## "%s%d[%x:%x]>%s "), _dc.iVis, _dc.iCategory, _dc.iId, _dc.iAddr, _dc.iFn); _MARK_ENTRY(); } while(0)
			#define DP0_IN(string) do { _CHK_GRP(); _CHK_MULTIIN(); _DPPRINTER(_T(L ## "%s%d[%x:%x]>%s " L ## string), _dc.iVis, _dc.iCategory, _dc.iId, _dc.iAddr, _dc.iFn); _MARK_ENTRY(); } while(0)
			#define DP1_IN(string, p1) do { _CHK_GRP(); _CHK_MULTIIN(); _DPPRINTER(_T(L ## "%s%d[%x:%x]>%s " L ## string), _dc.iVis, _dc.iCategory, _dc.iId, _dc.iAddr, _dc.iFn, p1); _MARK_ENTRY(); } while(0)
			#define DP2_IN(string, p1, p2) do { _CHK_GRP(); _CHK_MULTIIN(); _DPPRINTER(_T(L ## "%s%d[%x:%x]>%s " L ## string), _dc.iVis, _dc.iCategory, _dc.iId, _dc.iAddr, _dc.iFn, p1, p2); _MARK_ENTRY(); } while(0)
			#define DP3_IN(string, p1, p2, p3) do { _CHK_GRP(); _CHK_MULTIIN(); _DPPRINTER(_T(L ## "%s%d[%x:%x]>%s " L ## string), _dc.iVis, _dc.iCategory, _dc.iId, _dc.iAddr, _dc.iFn, p1, p2, p3); _MARK_ENTRY(); } while(0)
			#define DP4_IN(string, p1, p2, p3, p4) do { _CHK_GRP(); _CHK_MULTIIN(); _DPPRINTER(_T(L ## "%s%d[%x:%x]>%s " L ## string), _dc.iVis, _dc.iCategory, _dc.iId, _dc.iAddr, _dc.iFn, p1, p2, p3, p4); _MARK_ENTRY(); } while(0)
			#define DP5_IN(string, p1, p2, p3, p4, p5) do { _CHK_GRP(); _CHK_MULTIIN(); _DPPRINTER(_T(L ## "%s%d[%x:%x]>%s " L ## string), _dc.iVis, _dc.iCategory, _dc.iId, _dc.iAddr, _dc.iFn, p1, p2, p3, p4, p5); _MARK_ENTRY(); } while(0)

			#define DP_OUT() do { _CHK_GRP(); _DOINCHK(); _CHK_MULTIOUT(); _DPPRINTER(_T(L ## "%s%d[%x:%x]<%s "), _dc.iVis, _dc.iCategory, _dc.iId, _dc.iAddr, _dc.iFn); _MARK_EXIT(); } while(0)
			#define DP0_OUT(string) do { _CHK_GRP(); _DOINCHK(); _CHK_MULTIOUT(); _DPPRINTER(_T(L ## "%s%d[%x:%x]<%s " L ## string), _dc.iVis, _dc.iCategory, _dc.iId, _dc.iAddr, _dc.iFn); _MARK_EXIT(); } while(0)
			#define DP1_OUT(string, p1) do { _CHK_GRP(); _DOINCHK(); _CHK_MULTIOUT(); _DPPRINTER(_T(L ## "%s%d[%x:%x]<%s " L ## string), _dc.iVis, _dc.iCategory, _dc.iId, _dc.iAddr, _dc.iFn, p1); _MARK_EXIT(); } while(0)
			#define DP2_OUT(string, p1, p2) do { _CHK_GRP(); _DOINCHK(); _CHK_MULTIOUT(); _DPPRINTER(_T(L ## "%s%d[%x:%x]<%s " L ## string), _dc.iVis, _dc.iCategory, _dc.iId, _dc.iAddr, _dc.iFn, p1, p2); _MARK_EXIT(); } while(0)
			#define DP3_OUT(string, p1, p2, p3) do { _CHK_GRP(); _DOINCHK(); _CHK_MULTIOUT(); _DPPRINTER(_T(L ## "%s%d[%x:%x]<%s " L ## string), _dc.iVis, _dc.iCategory, _dc.iId, _dc.iAddr, _dc.iFn, p1, p2, p3); _MARK_EXIT(); } while(0)
			#define DP4_OUT(string, p1, p2, p3, p4) do { _CHK_GRP(); _DOINCHK(); _CHK_MULTIOUT(); _DPPRINTER(_T(L ## "%s%d[%x:%x]<%s " L ## string), _dc.iVis, _dc.iCategory, _dc.iId, _dc.iAddr, _dc.iFn, p1, p2, p3, p4); _MARK_EXIT(); } while(0)
			#define DP5_OUT(string, p1, p2, p3, p4, p5) do { _CHK_GRP(); _DOINCHK(); _CHK_MULTIOUT(); _DPPRINTER(_T(L ## "%s%d[%x:%x]<%s " L ## string), _dc.iVis, _dc.iCategory, _dc.iId, _dc.iAddr, _dc.iFn, p1, p2, p3, p4, p5); _MARK_EXIT(); } while(0)

			#define DP0_RET(val, fmtstr) do { do { _CHK_GRP(); _DOINCHK(); _CHK_MULTIOUT(); _DPPRINTER(_T(L ## "%s%d[%x:%x]<%s " L ## fmtstr), _dc.iVis, _dc.iCategory, _dc.iId, _dc.iAddr, _dc.iFn, val); _MARK_EXIT(); } while(0); return val;} while(0)
			#define DP1_RET(val, fmtstr, p1) do { do { _CHK_GRP(); _DOINCHK(); _CHK_MULTIOUT(); _DPPRINTER(_T(L ## "%s%d[%x:%x]<%s " L ## fmtstr), _dc.iVis, _dc.iCategory, _dc.iId, _dc.iAddr, _dc.iFn, val, p1); _MARK_EXIT(); } while(0); return val;} while(0)
			#define DP2_RET(val, fmtstr, p1, p2) do { do { _CHK_GRP(); _DOINCHK(); _CHK_MULTIOUT(); _DPPRINTER(_T(L ## "%s%d[%x:%x]<%s " L ## fmtstr), _dc.iVis, _dc.iCategory, _dc.iId, _dc.iAddr, _dc.iFn, val, p1, p2); _MARK_EXIT(); } while(0); return val;} while(0)
			#define DP3_RET(val, fmtstr, p1, p2, p3) do { do { _CHK_GRP(); _DOINCHK(); _CHK_MULTIOUT(); _DPPRINTER(_T(L ## "%s%d[%x:%x]<%s " L ## fmtstr), _dc.iVis, _dc.iCategory, _dc.iId, _dc.iAddr, _dc.iFn, val, p1, p2, p3); _MARK_EXIT(); } while(0); return val;} while(0)
			#define DP4_RET(val, fmtstr, p1, p2, p3, p4) do { do { _CHK_GRP(); _DOINCHK(); _CHK_MULTIOUT(); _DPPRINTER(_T(L ## "%s%d[%x:%x]<%s " L ## fmtstr), _dc.iVis, _dc.iCategory, _dc.iId, _dc.iAddr, _dc.iFn, val, p1, p2, p3, p4); _MARK_EXIT(); } while(0); return val;} while(0)
			#define DP5_RET(val, fmtstr, p1, p2, p3, p4, p5) do { do { _CHK_GRP(); _DOINCHK(); _CHK_MULTIOUT(); _DPPRINTER(_T(L ## "%s%d[%x:%x]<%s " L ## fmtstr), _dc.iVis, _dc.iCategory, _dc.iId, _dc.iAddr, _dc.iFn, val, p1, p2, p3, p4, p5); _MARK_EXIT(); } while(0); return val;} while(0)

			#define DP0(level, string) do { _CHK_GRP(); _CHK_LEVEL(level); _DOINCHK(); _DPPRINTER(_T(L ## "%s%d[%x:%x]%s " L ## string), _dc.iVis, level, _dc.iId, _dc.iAddr, _dc.iFn); } while(0)
			#define DP1(level, string, p1) do { _CHK_GRP(); _CHK_LEVEL(level); _DOINCHK(); _DPPRINTER(_T(L ## "%s%d[%x:%x]%s " L ## string), _dc.iVis, level, _dc.iId, _dc.iAddr, _dc.iFn, p1); } while(0)
			#define DP2(level, string, p1, p2) do { _CHK_GRP(); _CHK_LEVEL(level); _DOINCHK(); _DPPRINTER(_T(L ## "%s%d[%x:%x]%s " L ## string), _dc.iVis, level, _dc.iId, _dc.iAddr, _dc.iFn, p1, p2); } while(0)
			#define DP3(level, string, p1, p2, p3) do { _CHK_GRP(); _CHK_LEVEL(level); _DOINCHK(); _DPPRINTER(_T(L ## "%s%d[%x:%x]%s " L ## string), _dc.iVis, level, _dc.iId, _dc.iAddr, _dc.iFn, p1, p2, p3); } while(0)
			#define DP4(level, string, p1, p2, p3, p4) do { _CHK_GRP(); _CHK_LEVEL(level); _DOINCHK(); _DPPRINTER(_T(L ## "%s%d[%x:%x]%s " L ## string), _dc.iVis, level, _dc.iId, _dc.iAddr, _dc.iFn, p1, p2, p3, p4); } while(0)
			#define DP5(level, string, p1, p2, p3, p4, p5) do { _CHK_GRP(); _CHK_LEVEL(level); _DOINCHK(); _DPPRINTER(_T(L ## "%s%d[%x:%x]%s " L ## string), _dc.iVis, level, _dc.iId, _dc.iAddr, _dc.iFn, p1, p2, p3, p4, p5); } while(0)

			#define __DP_ASSERT_DBG( _assertion ) do { if( _assertion ) { break; } TFileName file; file.Copy( _L8( __FILE__ ) );   _DPPRINTER(_T(L ## "%s%d[%x:%x]<%s Assert:%S:%d:" L ## #_assertion) , _dc.iVis, _dc.iCategory, _dc.iId, _dc.iAddr, _dc.iFn, &file, __LINE__ ); User::Invariant(); } while( 0 )

			#define __TRACE_ASSERT_DBG(_assertion, _textToPrint, _panicCode)  do { if (_assertion) { break; } _DPPRINTER(_T(L ## "%s%d[%x:%x]<%s ASSERTION FAILED!!! %s file: %s, line: %s"), _dc.iVis, _dc.iCategory, _dc.iId, _dc.iAddr, _dc.iFn, _textToPrint, __FILE__, __LINE__); User::Panic(_L("AssertionFailed"), _panicCode} while(0)

		#else // __KERNEL_MODE__
			#define DP_CONTEXT(_fn, _id, _vis) _TTraceCtx _dc((TText*) #_fn, _id, _vis, (TUint)this )
			#define DP_STATIC_CONTEXT(_fn, _id, _vis) _TTraceCtx _dc((TText*) #_fn, _id, _vis, 0)

			// NOTE: no trace mask checks in kernel code (no access to shared heap)
			#define DP_IN() do { _CHK_MULTIIN(); _DPPRINTER(_T("%s%d[%x:%x]>%s "), _dc.iVis, _dc.iCategory, _dc.iId, _dc.iAddr, _dc.iFn); _MARK_ENTRY(); } while(0)
			#define DP0_IN(string) do { _CHK_MULTIIN(); _DPPRINTER(_T("%s%d[%x:%x]>%s " string), _dc.iVis, _dc.iCategory, _dc.iId, _dc.iAddr, _dc.iFn); _MARK_ENTRY(); } while(0)
			#define DP1_IN(string, p1) do { _CHK_MULTIIN(); _DPPRINTER(_T("%s%d[%x:%x]>%s " string), _dc.iVis, _dc.iCategory, _dc.iId, _dc.iAddr, _dc.iFn, p1); _MARK_ENTRY(); } while(0)
			#define DP2_IN(string, p1, p2) do { _CHK_MULTIIN(); _DPPRINTER(_T("%s%d[%x:%x]>%s " string), _dc.iVis, _dc.iCategory, _dc.iId, _dc.iAddr, _dc.iFn, p1, p2); _MARK_ENTRY(); } while(0)
			#define DP3_IN(string, p1, p2, p3) do { _CHK_MULTIIN(); _DPPRINTER(_T("%s%d[%x:%x]>%s " string), _dc.iVis, _dc.iCategory, _dc.iId, _dc.iAddr, _dc.iFn, p1, p2, p3); _MARK_ENTRY(); } while(0)
			#define DP4_IN(string, p1, p2, p3, p4) do { _CHK_MULTIIN(); _DPPRINTER(_T("%s%d[%x:%x]>%s " string), _dc.iVis, _dc.iCategory, _dc.iId, _dc.iAddr, _dc.iFn, p1, p2, p3, p4); _MARK_ENTRY(); } while(0)
			#define DP5_IN(string, p1, p2, p3, p4, p5) do { _CHK_MULTIIN(); _DPPRINTER(_T("%s%d[%x:%x]>%s " string), _dc.iVis, _dc.iCategory, _dc.iId, _dc.iAddr, _dc.iFn, p1, p2, p3, p4, p5); _MARK_ENTRY(); } while(0)

			#define DP_OUT() do { _DOINCHK(); _CHK_MULTIOUT(); _DPPRINTER(_T("%s%d[%x:%x]<%s "), _dc.iVis, _dc.iCategory, _dc.iId, _dc.iAddr, _dc.iFn); _MARK_EXIT(); } while(0)
			#define DP0_OUT(string) do { _DOINCHK(); _CHK_MULTIOUT(); _DPPRINTER(_T("%s%d[%x:%x]<%s " string), _dc.iVis, _dc.iCategory, _dc.iId, _dc.iAddr, _dc.iFn); _MARK_EXIT(); } while(0)
			#define DP1_OUT(string, p1) do { _DOINCHK(); _CHK_MULTIOUT(); _DPPRINTER(_T("%s%d[%x:%x]<%s " string), _dc.iVis, _dc.iCategory, _dc.iId, _dc.iAddr, _dc.iFn, p1); _MARK_EXIT(); } while(0)
			#define DP2_OUT(string, p1, p2) do { _DOINCHK(); _CHK_MULTIOUT(); _DPPRINTER(_T("%s%d[%x:%x]<%s " string), _dc.iVis, _dc.iCategory, _dc.iId, _dc.iAddr, _dc.iFn, p1, p2); _MARK_EXIT(); } while(0)
			#define DP3_OUT(string, p1, p2, p3) do { _DOINCHK(); _CHK_MULTIOUT(); _DPPRINTER(_T("%s%d[%x:%x]<%s " string), _dc.iVis, _dc.iCategory, _dc.iId, _dc.iAddr, _dc.iFn, p1, p2, p3); _MARK_EXIT(); } while(0)
			#define DP4_OUT(string, p1, p2, p3, p4) do { _DOINCHK(); _CHK_MULTIOUT(); _DPPRINTER(_T("%s%d[%x:%x]<%s " string), _dc.iVis, _dc.iCategory, _dc.iId, _dc.iAddr, _dc.iFn, p1, p2, p3, p4); _MARK_EXIT(); } while(0)
			#define DP5_OUT(string, p1, p2, p3, p4, p5) do { _DOINCHK(); _CHK_MULTIOUT(); _DPPRINTER(_T("%s%d[%x:%x]<%s " string), _dc.iVis, _dc.iCategory, _dc.iId, _dc.iAddr, _dc.iFn, p1, p2, p3, p4, p5); _MARK_EXIT(); } while(0)

			#define DP0_RET(val, fmtstr) do { _DOINCHK(); _CHK_MULTIOUT(); _DPPRINTER(_T("%s%d[%x:%x]<%s " fmtstr), _dc.iVis, _dc.iCategory, _dc.iId, _dc.iAddr, _dc.iFn, val); _MARK_EXIT(); return val; } while(0)
			#define DP1_RET(val, fmtstr, p1) do { _DOINCHK(); _CHK_MULTIOUT(); _DPPRINTER(_T("%s%d[%x:%x]<%s " fmtstr), _dc.iVis, _dc.iCategory, _dc.iId, _dc.iAddr, _dc.iFn, val, p1); _MARK_EXIT(); return val; } while(0)
			#define DP2_RET(val, fmtstr, p1, p2) do { _DOINCHK(); _CHK_MULTIOUT(); _DPPRINTER(_T("%s%d[%x:%x]<%s " fmtstr), _dc.iVis, _dc.iCategory, _dc.iId, _dc.iAddr, _dc.iFn, val, p1, p2); _MARK_EXIT(); return val; } while(0)
			#define DP3_RET(val, fmtstr, p1, p2, p3) do { _DOINCHK(); _CHK_MULTIOUT(); _DPPRINTER(_T("%s%d[%x:%x]<%s " fmtstr), _dc.iVis, _dc.iCategory, _dc.iId, _dc.iAddr, _dc.iFn, val, p1, p2, p3); _MARK_EXIT(); return val; } while(0)
			#define DP4_RET(val, fmtstr, p1, p2, p3, p4) do { _DOINCHK(); _CHK_MULTIOUT(); _DPPRINTER(_T("%s%d[%x:%x]<%s " fmtstr), _dc.iVis, _dc.iCategory, _dc.iId, _dc.iAddr, _dc.iFn, val, p1, p2, p3, p4); _MARK_EXIT(); return val; } while(0)
			#define DP5_RET(val, fmtstr, p1, p2, p3, p4, p5) do { _DOINCHK(); _CHK_MULTIOUT(); _DPPRINTER(_T("%s%d[%x:%x]<%s " fmtstr), _dc.iVis, _dc.iCategory, _dc.iId, _dc.iAddr, _dc.iFn, val, p1, p2, p3, p4, p5); _MARK_EXIT(); return val; } while(0)

			#define DP0(level, string) do { _DOINCHK(); _DPPRINTER(_T("%s%d[%x:%x]%s " string), _dc.iVis, level, _dc.iId, _dc.iAddr, _dc.iFn); } while(0)
			#define DP1(level, string, p1) do { _DOINCHK(); _DPPRINTER(_T("%s%d[%x:%x]%s " string), _dc.iVis, level, _dc.iId, _dc.iAddr, _dc.iFn, p1); } while(0)
			#define DP2(level, string, p1, p2) do { _DOINCHK(); _DPPRINTER(_T("%s%d[%x:%x]%s " string), _dc.iVis, level, _dc.iId, _dc.iAddr, _dc.iFn, p1, p2); } while(0)
			#define DP3(level, string, p1, p2, p3) do { _DOINCHK(); _DPPRINTER(_T("%s%d[%x:%x]%s " string), _dc.iVis, level, _dc.iId, _dc.iAddr, _dc.iFn, p1, p2, p3); } while(0)
			#define DP4(level, string, p1, p2, p3, p4) do { _DOINCHK(); _DPPRINTER(_T("%s%d[%x:%x]%s " string), _dc.iVis, level, _dc.iId, _dc.iAddr, _dc.iFn, p1, p2, p3, p4); } while(0)
			#define DP5(level, string, p1, p2, p3, p4, p5) do { _DOINCHK(); _DPPRINTER(_T("%s%d[%x:%x]%s " string), _dc.iVis, level, _dc.iId, _dc.iAddr, _dc.iFn, p1, p2, p3, p4, p5); } while(0)

			#define __DP_ASSERT_DBG( _assertion ) do { if( _assertion ) { break; } TFileName file; file.Copy( _L8( __FILE__ ) );   _DPPRINTER(_T(L ## "%s%d[%x:%x]<%s Assert:%S:%d:" L ## #_assertion) , _dc.iVis, _dc.iCategory, _dc.iId, _dc.iAddr, _dc.iFn, &file, __LINE__ ); User::Invariant(); } while( 0 )

			#define __TRACE_ASSERT_DBG(_assertion, _textToPrint, _panicCode)  do { if (_assertion) { break; } _DPPRINTER(_T("%s[%x:%x:%d]<%s ASSERTION FAILED!!! %s file: %s, line: %s"), _dc.iVis, _dc.iId, _dc.iAddr, _dc.iFn, _textToPrint, __FILE__, __LINE__); User::Panic(_L("AssertionFailed"), _panicCode} while(0)

		#endif // __KERNEL_MODE__

		#define BIND_TRACE_TRAPHANDLER() _TTraceTrapHandler _traceTrapHandler; _traceTrapHandler.oldHandler = User::SetTrapHandler(&_traceTrapHandler)
		#define TRACE_CREATE() _CREATE_MASK();
		#define TRACE_FAST_CREATE(_thdId) _CREATE_MASK();

	#else // SYMBIAN_MULTIMEDIA_A3F_ENABLE_LOGGING

		#define DP_CONTEXT(_fn, _id, _vis)
		#define DP_STATIC_CONTEXT(_fn, _id, _vis)

		#define DP_IN()
		#define DP0_IN(string)
		#define DP1_IN(string, p1)
		#define DP2_IN(string, p1, p2)
		#define DP3_IN(string, p1, p2, p3)
		#define DP4_IN(string, p1, p2, p3, p4)
		#define DP5_IN(string, p1, p2, p3, p4, p5)

		#define DP_OUT()
		#define DP0_OUT(string)
		#define DP1_OUT(string, p1)
		#define DP2_OUT(string, p1, p2)
		#define DP3_OUT(string, p1, p2, p3)
		#define DP4_OUT(string, p1, p2, p3, p4)
		#define DP5_OUT(string, p1, p2, p3, p4, p5)

		#define DP0_RET(val, fmtstr) return val
		#define DP1_RET(val, fmtstr, p1) return val
		#define DP2_RET(val, fmtstr, p1, p2) return val
		#define DP3_RET(val, fmtstr, p1, p2, p3) return val
		#define DP4_RET(val, fmtstr, p1, p2, p3, p4) return val
		#define DP5_RET(val, fmtstr, p1, p2, p3, p4, p5) return val

		#define DP0(level, string)
		#define DP1(level, string, p1)
		#define DP2(level, string, p1, p2)
		#define DP3(level, string, p1, p2, p3)
		#define DP4(level, string, p1, p2, p3, p4)
		#define DP5(level, string, p1, p2, p3, p4, p5)

		#define __DP_ASSERT_DBG( _assertion )
		#define __TRACE_ASSERT_DBG(_assertion, _textToPrint, _panicCode)

		#define BIND_TRACE_TRAPHANDLER()
		#define TRACE_CREATE()
		#define TRACE_FAST_CREATE(_thdId) _thdId++;

	#endif // SYMBIAN_MULTIMEDIA_A3F_ENABLE_LOGGING

#else // _DEBUG

    /**
    * Creates a trace context for traceable function. This variant is used with instance functions (methods).
    * Parameter @a _fn defines the name of the function (method) to trace. @_id defines the group where this trace entry belongs.
    * For full list of available trace contexts, see file a3f_trace_ctxt.h. Parameter @a _vis defines the visibility this context is having.
    * Methods with IMPORT_C / EXPORT_C definitions are marked as DPAPI, others have signature DPLOCAL. \n\n
    * Usage example:
    * @code
    * DP_CONTEXT(CFoo::Bar, CtxTest, DPLOCAL);
    * @endcode
    */
    #define DP_CONTEXT(_fn, _id, _vis)

    /**
    * Creates a trace context for traceable function. This variant is used with plain functions and static methods.
    * Parameter @a _fn defines the name of the function (method) to trace. @_id defines the group where this trace entry belongs.
    * For full list of available trace contexts, see file a3f_trace_ctxt.h. Parameter @a _vis defines the visibility this context is having.
    * Methods / functions with IMPORT_C / EXPORT_C definitions are marked as DPAPI, others have signature DPLOCAL.
    * Parameter @a _thdId is a name for a variable to create when thread id is queried from kernel. Later in the code you can use this
    * variable e.g. to instantiate other trace enabled classes.\n\n
    * Usage example:
    * @code
    * DP_CONTEXT(FooBar, CtxTest, DPAPI, thdId);
    * ...
    * TRACE_FAST_CREATE(thdId);
    * @endcode
    */
    #define DP_STATIC_CONTEXT(_fn, _id, _vis)

    /**
    * Using the defined trace context, outputs method entry information into traces.
    * <b> NOTE: Trace context must be defined before this method can be used.</b>
    * Trace context can be defined using either DP_CONTEXT or DP_STATIC_CONTEXT
    */
    #define DP_IN()
    #define DP0_IN(string)
    #define DP1_IN(string, p1)
    #define DP2_IN(string, p1, p2)
    #define DP3_IN(string, p1, p2, p3)
    #define DP4_IN(string, p1, p2, p3, p4)
    #define DP5_IN(string, p1, p2, p3, p4, p5)

    #define DP_OUT()
    #define DP0_OUT(string)
    #define DP1_OUT(string, p1)
    #define DP2_OUT(string, p1, p2)
    #define DP3_OUT(string, p1, p2, p3)
    #define DP4_OUT(string, p1, p2, p3, p4)
    #define DP5_OUT(string, p1, p2, p3, p4, p5)

    #define DP0_RET(val, fmtstr) return val
    #define DP1_RET(val, fmtstr, p1) return val
    #define DP2_RET(val, fmtstr, p1, p2) return val
    #define DP3_RET(val, fmtstr, p1, p2, p3) return val
    #define DP4_RET(val, fmtstr, p1, p2, p3, p4) return val
    #define DP5_RET(val, fmtstr, p1, p2, p3, p4, p5) return val

    #define DP0(level, string)
    #define DP1(level, string, p1)
    #define DP2(level, string, p1, p2)
    #define DP3(level, string, p1, p2, p3)
    #define DP4(level, string, p1, p2, p3, p4)
    #define DP5(level, string, p1, p2, p3, p4, p5)

    #define BIND_TRACE_TRAPHANDLER()
    #define TRACE_FAST_CREATE(_thdId) _thdId++;
    #define TRACE_CREATE()

    #define __DP_ASSERT_DBG(_assertion)
    #define __TRACE_ASSERT_DBG(_assertion, _message, _panicCode )

#endif // _DEBUG

#define PERF_IN_USE

//performance trace categories
#define PRF_NONE 				0x0000
#define PRF_TIME 				0x0001
#define PRF_LOAD   			0x0002
#define PRF_MEM   			0x0004

//performance trace actions
#define PRF_START 	1
#define PRF_STOP  	0

#define PRF_ID 			1
#define PRF_NO_ID 	0

//active categories
#define PRF_CATEGORIES ( PRF_TIME | PRF_LOAD | PRF_MEM )

#ifdef SYMBIAN_MULTIMEDIA_A3F_ENABLE_LOGGING

	#ifndef __KERNEL_MODE__
		#ifdef ASW_PERF_TRACES_ENABLED
			#define _T(a) (TPtrC((const TText *)(a)))
			#define PRF_PRINT RDebug::Print

			#define PRF(id, action, category, event, string); \
				do { \
					if (category == (TUint16)(PRF_CATEGORIES & category))\
						{\
						if (PRF_NO_ID == id) \
							{\
							PRF_PRINT(_L("e_[]%s %d"), (TText*)L ## # event, action); \
							}\
						else \
							{\
							RThread thdIdGeneral; \
							if ("" == string) \
								{\
								PRF_PRINT(_L("e_[%x:%x]%s %d"), \
									category, (TInt)thdIdGeneral.Id(), this, (TText*)L ## # event, action); \
								}\
							else \
								{\
								PRF_PRINT(_L("e_[%x:%x]%s %d, %s"), \
									category, (TInt)thdIdGeneral.Id(), this, (TText*)L ## # event, action, \
									(TText*)L ## # string); \
								}\
							}\
						}\
				   } while(0)

			#define PRF1(id, action, category, event, string, p1); \
				do { \
					if (category == (TUint16)(PRF_CATEGORIES & category))\
						{\
						RThread thdId; \
						PRF_PRINT( _T(L ## "e_[%x:%x]%s %d, " L ## string), \
							category, (TInt)thdId.Id(), this, L ## # event, action, p1); \
						}\
				   } while(0)

			#define PRF2(id, action, category, event, string, p1, p2); \
				do { \
					if (category == (TUint16)(PRF_CATEGORIES & category))\
						{\
						RThread thdId; \
						PRF_PRINT( _T(L ## "e_[%x:%x]%s %d, " L ## string), \
							category, (TInt)thdId.Id(), this, L ## # event, action, p1, p2); \
						}\
				   } while(0)

			#define PRF3(id, action, category, event, string, p1, p2, p3); \
				do { \
					if (category == (TUint16)(PRF_CATEGORIES & category))\
						{\
						RThread thdId; \
						PRF_PRINT( _T(L ## "e_[%x:%x]%s %d, " L ## string), \
							category, (TInt)thdId.Id(), this, L ## # event, action, p1, p2, p3); \
						}\
				   } while(0)

			#define PRF4(id, action, category, event, string, p1, p2, p3, p4); \
				do { \
					if (category == (TUint16)(PRF_CATEGORIES & category))\
						{\
						RThread thdId; \
						PRF_PRINT( _T(L ## "e_[%x:%x]%s %d, " L ## string), \
							category, (TInt)thdId.Id(), this, L ## # event, action, p1, p2, p3, p4); \
						}\
				   } while(0)

			#define PRF5(id, action, category, event, string, p1, p2, p3, p4, p5); \
				do { \
					if (category == (TUint16)(PRF_CATEGORIES & category))\
						{\
						RThread thdId; \
						PRF_PRINT( _T(L ## "e_[%x:%x]%s %d, " L ## string), \
							category, (TInt)thdId.Id(), this, L ## # event, action, p1, p2, p3, p4, p5); \
						}\
				   } while(0)

			#define PRF_TEST(event, string, param); \
				PRF_PRINT(_L("string: %s, param %d, " ## string), \
					(TText*)L ## # event, param); \

			#define PRF_HEAP_STATUS(category, component); \
				do { \
					if(category == (TUint16)(PRF_CATEGORIES & category )) \
						{ \
						TInt cellsFree(0); \
						TInt cellsAllocated = User::CountAllocCells( cellsFree ); \
						TInt bytesAllocated(0); \
						TInt cellsAllocatedInHeap = User::AllocSize( bytesAllocated ); \
						TInt bytesInLargestFreeBlock(0); \
						TInt bytesAvailable = User::Available(bytesInLargestFreeBlock); \
						PRF_PRINT(_L("[%x:%x]%s cellsFree=%d, cellsAllocated=%d, bytesAllocated=%d, cellsAllocatedInHeap=%d, bytesInLargestFreeBlock=%d, bytesAvailable=%d"), \
							category, this, (TText*)L ## # component, \
							cellsFree, cellsAllocated, bytesAllocated, cellsAllocatedInHeap, \
							bytesInLargestFreeBlock, bytesAvailable); \
						} \
				   } while(0)
		#else
			#define PRF(id, action, category, event, string);
			#define PRF1(id, action, category, event, string, p1);
			#define PRF2(id, action, category, event, string, p1, p2);
			#define PRF3(id, action, category, event, string, p1, p2, p3);
			#define PRF4(id, action, category, event, string, p1, p2, p3, p4);
			#define PRF5(id, action, category, event, string, p1, p2, p3, p4, p5);
			#define PRF_HEAP_STATUS(category, component);

		#endif  //ASW_PERF_TRACES_ENABLED
	#endif //__KERNEL_MODE__

#else // SYMBIAN_MULTIMEDIA_A3F_ENABLE_LOGGING

	#define PRF_PRINT

	#define PRF(id, action, category, event, string);
	#define PRF1(id, action, category, event, string, p1);
	#define PRF2(id, action, category, event, string, p1, p2);
	#define PRF3(id, action, category, event, string, p1, p2, p3);
	#define PRF4(id, action, category, event, string, p1, p2, p3, p4);
	#define PRF5(id, action, category, event, string, p1, p2, p3, p4, p5);
	#define PRF_HEAP_STATUS(category, component);

#endif	// SYMBIAN_MULTIMEDIA_A3F_ENABLE_LOGGING

// DATA TYPES
//enum ?declaration
//typedef ?declaration
//extern ?data_type;

// FUNCTION PROTOTYPES
//?type ?function_name(?arg_list);

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;

// CLASS DECLARATION



#endif      // _A3F_TRACE_UTILS_H_

// End of File

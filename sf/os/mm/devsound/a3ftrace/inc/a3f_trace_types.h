/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef _A3F_TRACE_TYPES_H_
#define _A3F_TRACE_TYPES_H_

//  INCLUDES
#include <a3f/a3f_trace_ctxt.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <a3f/a3f_trace_ctxt_def.h>
#endif

#ifndef __KERNEL_MODE__
#include <e32std.h>
#include <e32svr.h>
#endif // __KERNEL_MODE__

#ifdef _DEBUG

    #include <a3f/a3f_trace_heap.h>
    
    // CONSTANTS
    static const TUint DLINFO  = 0;  /**< Used to create an info message    */
    static const TUint DLWARN  = 1;  /**< Used to create a warning message */
    static const TUint DLERR   = 2;  /**< Used to create an error message */
    static const TUint DLFATAL = 3;  /**< Used to create a fatal message */
    
    static const TUint DPAPI   = 0;  /**< Used to create an API context. Context is used to define function's trace context for following trace macro definitions. */
    static const TUint DPLOCAL = 1;  /**< Used to create an local context. Context is used to define function's trace context for following trace macro definitions. */
    


    #ifdef __KERNEL_MODE__
        #define _T(a) a
        #define _DPPRINTER Kern::Printf
            
        #define _UNCONTROLLED_INPUT_MSG  "%s%d[%x:%x]>%s UNCONTROLLED ENTRY!"
        #define _MULTIPLE_ENTRY_MSG      "%s%d[%x:%x]%s  ADDITIONAL ENTRY!"
        #define _UNCONTROLLER_EXIT_MSG   "%s%d[%x:%x]<%s UNCONTROLLED EXIT!"
        #define _MULTIPLE_EXIT_MSG       "%s%d[%x:%x]%s  ADDITIONAL EXIT!"
        #define _TRAP_HARNESS_ENTRY      "_D%d[%x:%x]TraceFramework: Entering trap harness"
        #define _TRAP_HARNESS_EXIT       "_D%d[%x:%x]TraceFramework: Exiting trap harness"
        #define _TRAP_HARNESS_LEAVE      "_D%d[%x:%x]TraceFramework: ---------- LEAVE OCCURRED !!! ---------- "
        #define _API_PREFIX              "_A"
        #define _LOCAL_PREFIX            "_L"
    #else
        #define _T(a) (TPtrC((const TText *)(a)))
        #define _DPPRINTER RDebug::Print
            
        #define _UNCONTROLLED_INPUT_MSG  L"%s%d[%x:%x]>%s UNCONTROLLED ENTRY!"
        #define _MULTIPLE_ENTRY_MSG      L"%s%d[%x:%x]%s  ADDITIONAL ENTRY!"
        #define _UNCONTROLLER_EXIT_MSG   L"%s%d[%x:%x]<%s UNCONTROLLED EXIT!"
        #define _MULTIPLE_EXIT_MSG       L"%s%d[%x:%x]%s  ADDITIONAL EXIT!"
        #define _TRAP_HARNESS_ENTRY      L"_D%d[%x:%x]TraceFramework: Entering trap harness"
        #define _TRAP_HARNESS_EXIT       L"_D%d[%x:%x]TraceFramework: Exiting trap harness"
        #define _TRAP_HARNESS_LEAVE      L"_D%d[%x:%x]TraceFramework: ---------- LEAVE OCCURRED !!! ---------- "
        #define _API_PREFIX              L"_A"
        #define _LOCAL_PREFIX            L"_L"
    #endif
    
    class _TTraceCtx
        {
        public:
            _TTraceCtx(const TText* _fn, const TUint _id, const TUint _vis, const TUint _addr, const TUint _category=0): 
                iFn(_fn), iId(TRACENFO[_id].iGrpId), iApi((TBool)_vis), iAddr(_addr), iVis((_vis == DPAPI ? (TText*)_API_PREFIX : (TText*)_LOCAL_PREFIX)),
                iCategory(_category)
            #ifndef DISABLE_SYNTAX_CHECK
                ,outOk(EFalse), inOk(EFalse)
            #endif
                {
                }
            ~_TTraceCtx() 
                {
                #ifndef DISABLE_SYNTAX_CHECK
                    do
                        {
                        #ifndef DISABLE_GROUP_CHECKS
                                if ( (!TraceHeap::IsMaskOn(TRACENFO[iId].iGrpId)) || ((!iApi) && TraceHeap::IsApiOnly()) ) 
                                    { 
                                    break; 
                                    }
                        #endif
                        
                        DoInChk();
                        if (!outOk)
                            {
                            _DPPRINTER(_T(_UNCONTROLLER_EXIT_MSG), iVis, iCategory, iId, iAddr, iFn);
                            }
                        } while (0);
                #endif // DISABLE_SYNTAX_CHECK
                }
    
            const TText* iFn;
            const TUint iId;
            const TText* iVis;
            const TUint iAddr;
            const TBool iApi;
            const TUint iCategory;
            
            #ifndef DISABLE_SYNTAX_CHECK
                inline void DoInChk()
                    {
                    if (!inOk)
                        {
                        _DPPRINTER(_T(_UNCONTROLLED_INPUT_MSG), iVis, iCategory, iId, iAddr, iFn);
                        inOk = ETrue;
                        }
                    }
                            
                inline void ChkMultiIn()
                    {
                    if (inOk)
                        {
                        _DPPRINTER(_T(_MULTIPLE_ENTRY_MSG), iVis, iCategory, iId, iAddr, iFn);                
                        }
                    }
        
                inline void ChkMultiOut()
                    {
                    if (outOk)
                        {
                        _DPPRINTER(_T(_MULTIPLE_EXIT_MSG), iVis, iCategory, iId, iAddr, iFn);                
                        }
                    }
    
                TBool inOk;
                TBool outOk;
            #endif // DISABLE_SYNTAX_CHECK
        };
    
    #ifndef __KERNEL_MODE__
        class _TTraceTrapHandler: public TTrapHandler
            {
            public:
                _TTraceTrapHandler()
                    {

                    }
            	void Trap() 
            	    {
            	    _DPPRINTER(_T(_TRAP_HARNESS_ENTRY), 0, DLERR, this);
            	    oldHandler->Trap();
            	    } 
            	void UnTrap()
            	    {
            	    _DPPRINTER(_T(_TRAP_HARNESS_EXIT), 0, DLERR, this);
            	    oldHandler->UnTrap();
            	    }	
            	void Leave(TInt aValue)
            	    {
            	    _DPPRINTER(_T(_TRAP_HARNESS_LEAVE), 0, DLERR, this);
            	    oldHandler->Leave(aValue);
            	    }	
            	TTrapHandler* oldHandler;
            private:
            };
    #endif

#endif // _DEBUG

#endif      // _A3F_TRACE_TYPES_H_
            
// End of File

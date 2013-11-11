/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implementation of tracing functions for SysLangUtil module.
*
*/


#ifdef _DEBUG
    #include <e32debug.h>
    #include "syslangutiltrace.h"
#endif // _DEBUG

#ifdef _DEBUG

    _LIT( KModuleName, "SYSLANG" );
    _LIT( KOverflowStr, "..." );
    _LIT( KTraceFuncFmt, "%S # %S" );
    _LIT( KTraceSeparator, " # " );
    _LIT( KAssertionFmt, "%S # Assertion failed: file=%S, line=%d" );
    _LIT( KFatalMemAllocFailure, "SYSLANG # Memory allocation failure during tracing" );

    const TInt KTraceBufLen = 128;
    typedef TBuf<KTraceBufLen> TTraceBuf;

    const TInt KFileNameBufLen = 64;
    typedef TBuf<KFileNameBufLen> TFileNameBuf;

#endif // _DEBUG

#if defined _DEBUG && defined __SYSLANG_TRACE__
    _LIT( KTraceFuncExitFmt, "%S # %S finished" );
    _LIT( KTraceFuncExitRetFmt, "%S # %S finished with %d" );
    _LIT( KTraceCallerFmt, " (caller thread: %S)" );

    const TInt KFuncNameBufLen = 64;
    typedef TBuf<KFuncNameBufLen> TFuncNameBuf;
#endif // _DEBUG && __SYSLANG_TRACE__

#ifdef _DEBUG

NONSHARABLE_CLASS( TTraceOverflowHandler ) : public TDes16Overflow
    {
    public: // Functions from base classes

        /**
        * Handle descriptor ovewflow.
        */
        virtual void Overflow( TDes16 &aDes )
            {
            aDes.RightTPtr( KOverflowStr().Length() ).Copy( KOverflowStr );
            }
    };

#endif // _DEBUG

// ============================= LOCAL FUNCTIONS ===============================

#ifdef _DEBUG

// -----------------------------------------------------------------------------
// AssertTraceFunc
//
// -----------------------------------------------------------------------------
//
void AssertTraceFunc( const TDesC8& aFile, TInt aLine )
    {
    TTraceOverflowHandler overflowHandler;

    TFileNameBuf name;
    name.Copy( aFile.Left( KFileNameBufLen ) ); // Change it to 16-bit descriptor

    TTraceBuf buf;
    buf.AppendFormat( KAssertionFmt, &overflowHandler,
                      &KModuleName, &name, aLine );
    RDebug::Print( buf );
    }


// -----------------------------------------------------------------------------
// TraceFunc
//
// -----------------------------------------------------------------------------
//
void TraceFunc( TRefByValue<const TDesC16> aFmt, ... )
    {
    HBufC* fmtString = HBufC::New( KTraceFuncFmt().Length() +
                                   KModuleName().Length() +
                                   static_cast<const TDesC16&>( aFmt ).Length() );

    if ( fmtString )
        {
        TPtr ptr( fmtString->Des() );
        ptr.Append( KModuleName );
        ptr.Append( KTraceSeparator );
        ptr.Append( aFmt );

        VA_LIST list;
        VA_START( list, aFmt );

        TTraceOverflowHandler overflowHandler;
        TTraceBuf buf;
        buf.AppendFormatList( *fmtString, list, &overflowHandler );
        RDebug::Print( buf );

        VA_END( list );

        delete fmtString;
        }
    else
        {
        RDebug::Print( KFatalMemAllocFailure );
        }
    }


#endif // _DEBUG

#if defined _DEBUG && defined __SYSLANG_TRACE__

// -----------------------------------------------------------------------------
// FuncEntryTrace
//
// -----------------------------------------------------------------------------
//
void FuncEntryTrace( const TDesC8& aFunction, TBool aPrintCaller )
    {
    TTraceOverflowHandler overflowHandler;

    TFuncNameBuf name;
    name.Copy( aFunction.Left( KFuncNameBufLen ) ); // Change it to 16-bit descriptor

    TTraceBuf buf;
    buf.AppendFormat( KTraceFuncFmt, &overflowHandler, &KModuleName, &name );

    if ( aPrintCaller )
        {
        const TName caller = RThread().Name();
        buf.AppendFormat( KTraceCallerFmt, &overflowHandler, &caller );
        }

    RDebug::Print( buf );
    }


// -----------------------------------------------------------------------------
// FuncExitTrace
//
// -----------------------------------------------------------------------------
//
void FuncExitTrace( const TDesC8& aFunction )
    {
    TTraceOverflowHandler overflowHandler;

    TFuncNameBuf name;
    name.Copy( aFunction.Left( KFuncNameBufLen ) ); // Change it to 16-bit descriptor

    TTraceBuf buf;
    buf.AppendFormat( KTraceFuncExitFmt, &overflowHandler, &KModuleName, &name );
    RDebug::Print( buf );
    }


// -----------------------------------------------------------------------------
// FuncExitTrace
//
// -----------------------------------------------------------------------------
//
void FuncExitTrace( const TDesC8& aFunction, const TInt aReturnValue )
    {
    TTraceOverflowHandler overflowHandler;

    TFuncNameBuf name;
    name.Copy( aFunction.Left( KFuncNameBufLen ) ); // Change it to 16-bit descriptor

    TTraceBuf buf;
    buf.AppendFormat( KTraceFuncExitRetFmt, &overflowHandler,
                      &KModuleName, &name, aReturnValue );
    RDebug::Print( buf );
    }


#endif // _DEBUG && __SYSLANG_TRACE__

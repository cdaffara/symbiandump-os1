/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef GLOBAL_H
#define GLOBAL_H

//  INCLUDES
#include <e32std.h>

#ifdef _DEBUG
#include <e32debug.h>
#endif

// FORWARD DECLARATIONS
class TPositionClassTypeBase;

// CLASS DECLARATION

/**
*  Global functions for the server
*  
*/
class Global
    {
    public: // New functions
        static TInt Read(const RMessage2& aMessage, const TInt aParam, TDes8& aDes, TInt aOffset=0);
        static TInt Read(const RMessage2& aMessage, const TInt aParam, TDes16& aDes, TInt aOffset=0);
        static TInt Write(const RMessage2& aMessage, const TInt aParam, const TDesC8& aDes, TInt aOffset=0);
        static HBufC8* CopyClientBuffer8LC(const RMessage2& aMessage, const TInt aClientBufferParam);
        static HBufC* CopyClientBufferLC(const RMessage2& aMessage, const TInt aClientBufferParam);
        static void ValidatePositionClassBufferL(const TPositionClassTypeBase& aClass, const TDes8& aBuffer);
        static void ValidatePositionClassTypeL(const TPositionClassTypeBase& aClass, const TInt aTypeExpected, const TInt aResult = KErrNotSupported);

#ifdef _DEBUG
        static void SetupTraceL();
        static void TearDownTrace();
        static void Trace(const TDesC& aDescription, const TDesC& aFilename, const TInt aLineNumber);
#endif

    private:
        Global( const Global& );
        Global& operator= ( const Global& );
    };

#ifdef _DEBUG
    #define DEBUG_TRACE(aTracePoint, aLineNum)                  \
        { _LIT(KTracePoint, aTracePoint);                         \
        Global::Trace(KTracePoint, KTraceFileName, aLineNum); }
#else
    #define DEBUG_TRACE(aTracePoint, aLineNum)
#endif

#endif      // GLOBAL_H  
            
// End of File

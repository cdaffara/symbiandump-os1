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



#ifndef CPOSPSYCRLOG_H
#define CPOSPSYCRLOG_H

//  INCLUDES
#include <e32base.h>
#include <f32file.h>

// S60 3.0

// CONSTANTS
_LIT(KDelim, "********************************************************************\r\n");

_LIT(KInfoMessage, "Info: ");
_LIT(KWarningMessage, "Warning: ");
_LIT(KErrorMessage, "Error: ");

_LIT(KNewLine, "\r\n");
const TInt KSummaryBufSize = 120;
const TInt KMaxNumberSize = 10;
    
// CLASS DECLARATION

/**
* Class that handles logging of test results to file.
*/
class CPosPSYCRLog : public CBase
    {
    public:  // Constructors and destructor

        /**
        * C++ default constructor.
        */
        CPosPSYCRLog();

        /**
        * Two-phased constructor.
        */
        static CPosPSYCRLog* NewL();

        /**
        * Destructor.
        */
        virtual ~CPosPSYCRLog();

    public: // New functions

        void OpenLogL(TUid aUid);
        
        void CloseLog();
        
        void AppendInfoMsgL(TDesC &aMsg);
        void AppendWarningMsgL(TDesC &aMsg);
        void AppendErrorMsgL(TDesC &aMsg);
                
          
    private:
    
        /**
        * By default EPOC constructor is private.
        */
        void ConstructL();
        
        void AppendLineL(
        /* IN */ const TDesC& aLine
        );

        void AppendLineL(
        /* IN */ const TInt aNumber
        );

        // By default, prohibit copy constructor
        CPosPSYCRLog( const CPosPSYCRLog& );
        
        // Prohibit assigment operator
        CPosPSYCRLog& operator= ( const CPosPSYCRLog& );

    private:     // Data
        
        RFile iCurrentLogFile;
        RFs   iFileSession;
        TInt  iCurrentPosition;
        TInt  iInfo;
        TInt  iWarning;
        TInt  iError;

    };

#endif      // CPOSPSYLOG_H

// End of File

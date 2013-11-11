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



#ifndef CPOSPSYRESULT_H
#define CPOSPSYRESULT_H

//  INCLUDES
#include <e32base.h>

// DATA TYPES
enum _TResultType
	{
	EInfoMessage,
	EWarningMessage,
	EErrorMessage,
    EDisplayMessage
	};

// CLASS DECLARATION
/**
*  Info class containing information about a single test in a test case
*/
class CPosPSYResult : public CBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CPosPSYResult* NewL(
        /* IN */ const TDesC& aMessage, 
        /* IN */ const TInt aResultType
        );

        /**
        * Two-phased constructor.
        */
        static CPosPSYResult* NewLC(
        /* IN */ const TDesC& aMessage,
        /* IN */ const TInt aResultType
        );

        /**
        * Destructor.
        */
        virtual ~CPosPSYResult();

    public: // New functions

        /**
        * Message
        * @return HBufC*, pointer to message buf
        */
        HBufC* Message() const;

        /**
        * ResultType
        * @return TInt, result message type  
        */
        TInt ResultType() const;
    
    private:

        /**
        * C++ default constructor.
        */
        CPosPSYResult();    
    
        /**
        * By default EPOC constructor is private.
        */
        void ConstructL(
        /* IN */ const TDesC& aMessage, 
        /* IN */ const TInt aResultType
        );

        // By default, prohibit copy constructor
        CPosPSYResult( const CPosPSYResult& );
        
        // Prohibit assigment operator
        CPosPSYResult& operator= ( const CPosPSYResult& );

    private:    // Data
        
        TInt iResultType;
        HBufC* iMessage;

    };

#endif      // CPOSPSYRESULT_H

// End of File

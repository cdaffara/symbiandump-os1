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



#ifndef CPOSPSYMEDIACAPABILITYTEST_H
#define CPOSPSYMEDIACAPABILITYTEST_H

//  INCLUDES
#include <lbspositioninfo.h>
#include "epos_cpospsycapabilitytestbase.h"

// CONSTANTS
const TInt KMediaLinkSize = 500;

// CLASS DECLARATION

/**
*  Class for testing Media capability
*/
class CPosPSYMediaCapabilityTest : public CPosPSYCapabilityTestBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CPosPSYMediaCapabilityTest* NewLC(
        /* IN/OUT */ CPosPSYResults& aResults,
        /* IN */     const RPointerArray<TPositionInfo>& aPositionArray,
        /* IN */     const TPosPSYInfo& aPSYInfo
        );

        /**
        * Destructor.
        */
        virtual ~CPosPSYMediaCapabilityTest();

    public:  // Functions from base classes

        /**
        * Function for testing media capability, if set
        */
        void TestCapabilityL();

    private:

        /**
        * C++ default constructor.
        */
        CPosPSYMediaCapabilityTest(
        /* IN/OUT */ CPosPSYResults& aResults,
        /* IN */     const RPointerArray<TPositionInfo>& aPositionArray,
        /* IN */     const TPosPSYInfo& aPSYInfo
        );

        /**
        * Function for checking result of capability test
        */
        void CheckResultsL();

        // By default, prohibit copy constructor
        CPosPSYMediaCapabilityTest( const CPosPSYMediaCapabilityTest& );

        // Prohibit assigment operator
        CPosPSYMediaCapabilityTest& operator= ( const CPosPSYMediaCapabilityTest& );

    private: // Data
    
        TInt iNrOfMediaNotSet;
		TInt iNrOfIncompleteMedia;
		TInt iNrOfMediaStartNotSet;
        TInt iNrOfEmptyMediaSentences;

    };

#endif      // CPOSPSYMEDIACAPABILITYTEST_H

// End of File

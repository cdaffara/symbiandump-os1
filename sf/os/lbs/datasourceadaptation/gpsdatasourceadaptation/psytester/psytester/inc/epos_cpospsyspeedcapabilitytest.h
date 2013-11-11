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



#ifndef CPOSPSYSPEEDCAPABILITYTEST_H
#define CPOSPSYSPEEDCAPABILITYTEST_H

//  INCLUDES
#include "epos_cpospsycapabilitytestbase.h"

// CLASS DECLARATION

/**
*  Class for testing Speed capability
*/
class CPosPSYSpeedCapabilityTest : public CPosPSYCapabilityTestBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CPosPSYSpeedCapabilityTest* NewLC(
        /* IN/OUT */ CPosPSYResults& aResults,
        /* IN */     const RPointerArray<TPositionInfo>& aPositionArray,
        /* IN */     const TPosPSYInfo& aPSYInfo
        );

        /**
        * Destructor.
        */
        virtual ~CPosPSYSpeedCapabilityTest();

    public:  // Functions from base classes

        /**
        * TestCapabilityL. Function for testing the Speed capability, if supported 
        */
        void TestCapabilityL();
        
    private:

        /**
        * C++ constructor.
        */
        CPosPSYSpeedCapabilityTest(
        /* IN/OUT */ CPosPSYResults& aResults,
        /* IN */     const RPointerArray<TPositionInfo>& aPositionArray,
        /* IN */     const TPosPSYInfo& aPSYInfo
        );

        /**
        * Function for checking result of capability test
        */
        void CheckResultsL();

        void ResetResults();

        void TestCapabilityForCourseInfoL();

        void TestCapabilityForGenericInfoL();

        // By default, prohibit copy constructor
        CPosPSYSpeedCapabilityTest( const CPosPSYSpeedCapabilityTest& );

        // Prohibit assigment operator
        CPosPSYSpeedCapabilityTest& operator= ( const CPosPSYSpeedCapabilityTest& );

    private:    // Data
        
        TInt iHorizontalSpeedNotSet;
        TInt iHorizontalSpeedErrorNotSet;
        TInt iVerticalSpeedNotSet;
        TInt iVerticalSpeedErrorNotSet;
        TInt iVSpeedLessThanZero;
        TInt iVSpeedErrorLessThanZero;
        TInt iHSpeedLessThanZero;
        TInt iHSpeedErrorLessThanZero;
        TInt iHorizontalCombinationError;
        TInt iVerticalCombinationError;
        TBool iTestingPerformed;

    };

#endif      // CPOSPSYSPEEDCAPABILITYTEST_H

// End of File

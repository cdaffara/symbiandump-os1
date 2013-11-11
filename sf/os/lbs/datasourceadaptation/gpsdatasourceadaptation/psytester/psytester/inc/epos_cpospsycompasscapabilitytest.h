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



#ifndef CPOSPSYCOMPASSCAPABILITYTEST_H
#define CPOSPSYCOMPASSCAPABILITYTEST_H

//  INCLUDES
#include "epos_cpospsycapabilitytestbase.h"

// CLASS DECLARATION

/**
*  Class for testing Compass capability
*/
class CPosPSYCompassCapabilityTest : public CPosPSYCapabilityTestBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CPosPSYCompassCapabilityTest* NewLC(
        /* IN/OUT */ CPosPSYResults& aResults,
        /* IN */     const RPointerArray<TPositionInfo>& aPositionArray,
        /* IN */     const TPosPSYInfo& aPSYInfo
        );

        /**
        * Destructor.
        */
        virtual ~CPosPSYCompassCapabilityTest();

    public:  // Functions from base classes

        /**
        * Function for testing compass capability, if set
        */
        void TestCapabilityL();

    private:

        /**
        * C++ constructor.
        */
        CPosPSYCompassCapabilityTest(
        /* IN/OUT */ CPosPSYResults& aResults,
        /* IN */     const RPointerArray<TPositionInfo>& aPositionArray,
        /* IN */     const TPosPSYInfo& aPSYInfo
        );

        /**
        * Function for checking result of capability test
        */
        void CheckResultsL();

        // By default, prohibit copy constructor
        CPosPSYCompassCapabilityTest( const CPosPSYCompassCapabilityTest& );
        
        // Prohibit assigment operator
        CPosPSYCompassCapabilityTest& operator= ( const CPosPSYCompassCapabilityTest& );
    
    private: // Data
        
        TBool iNoneSet;
        TInt iNoneSetError;
        TInt iTrueCourseNotSet;
        TInt iTrueCourseErrorNotSet;
        TInt iMagneticCourseNotSet;
        TInt iMagneticCourseVariationNotSet;
        TInt iOrientationNotSet;
        TInt iOrientationErrorNotSet;
        TInt iTrueCourseCombinationError;
        TInt iMagneticCourseCombinationError;
        TInt iOrientationCombinationError;

    };

#endif      // CPOSPSYCOMPASSCAPABILITYTEST_H

// End of File
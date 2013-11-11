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



#ifndef CPOSPSYADDRESSCAPABILITYTEST_H
#define CPOSPSYADDRESSCAPABILITYTEST_H

//  INCLUDES
#include "epos_cpospsycapabilitytestbase.h"

// CONSTANTS
const TInt KAddressResultBufSize = 300;

// CLASS DECLARATION

/**
*  Class for testing Address capability
*/
class CPosPSYAddressCapabilityTest : public CPosPSYCapabilityTestBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CPosPSYAddressCapabilityTest* NewLC(
        /* IN/OUT */ CPosPSYResults& aResults,
        /* IN */     const RPointerArray<TPositionInfo>& aPositionArray,
        /* IN */     const TPosPSYInfo& aPSYInfo
        );

        /**
        * Destructor.
        */
        virtual ~CPosPSYAddressCapabilityTest();

    public: // Functions from base classes
        
        /**
        * Function for testing address capability, if set
        */
        void TestCapabilityL();
        
    private:

        /**
        * C++ constructor.
        */
        CPosPSYAddressCapabilityTest(
        /* IN/OUT */ CPosPSYResults& aResults,
        /* IN */     const RPointerArray<TPositionInfo>& aPositionArray,
        /* IN */     const TPosPSYInfo& aPSYInfo
        );

        /**
        * Function for checking result of capability test
        */
        void CheckResultsL();

        // By default, prohibit copy constructor
        CPosPSYAddressCapabilityTest( const CPosPSYAddressCapabilityTest& );
        
        // Prohibit assigment operator
        CPosPSYAddressCapabilityTest& operator= ( const CPosPSYAddressCapabilityTest& );

    private: // Data
        
        TBool iNoneSet;
        TInt iNoneSetError;
        TInt iCountryNotSet;
        TInt iCountryCodeNotSet;
        TInt iStateNotSet;
        TInt iCityNotSet;
        TInt iDistrictNotSet;
        TInt iStreetNotSet;
        TInt iStreetExtensionNotSet;
        TInt iLocationNameNotSet;
        TInt iPostalCodeNotSet;
        TInt iLocalityNotSet;
        TInt iCrossing1NotSet;
        TInt iCrossing2NotSet;
    
    };

#endif      // CPOSPSYADDRESSCAPABILITYTEST_H

// End of File

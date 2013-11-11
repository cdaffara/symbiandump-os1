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



#ifndef CPOSPSYHORIZONTALCAPABILITYTEST_H
#define CPOSPSYHORIZONTALCAPABILITYTEST_H

//  INCLUDES
#include "epos_cpospsycapabilitytestbase.h"
#include "epos_tpospsytestparameters.h"

// CLASS DECLARATION
/**
*  Class for testing Horizontal capability
*/
class CPosPSYHorizontalCapabilityTest : public CPosPSYCapabilityTestBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CPosPSYHorizontalCapabilityTest* NewLC(
        /* IN/OUT */ CPosPSYResults& aResults,
        /* IN */     const RPointerArray<TPositionInfo>& aPositionArray,
        /* IN */     const TPosPSYInfo& aPSYInfo,
		/* IN */	 const TPosPSYTestParameters& aTestParamters
        );

        /**
        * Destructor.
        */
        virtual ~CPosPSYHorizontalCapabilityTest();

    public:  // Functions from base classes

        /**
        * Function for testing horizontal capability, if set
        */
        void TestCapabilityL();

    private:

        /**
        * C++ constructor.
        */
        CPosPSYHorizontalCapabilityTest(
        /* IN/OUT */ CPosPSYResults& aResults,
        /* IN */     const RPointerArray<TPositionInfo>& aPositionArray,
        /* IN */     const TPosPSYInfo& aPSYInfo,
		/* IN */	 const TPosPSYTestParameters& aTestParamters
        );
               
        /**
        * Function for checking result of capability test
        */
		void CheckResultsL();
        
        /**
		* TestCapabilityForPositionInfoL
		*/
		void TestCapabilityForPositionInfoL();

        // By default, prohibit copy constructor
        CPosPSYHorizontalCapabilityTest( const CPosPSYHorizontalCapabilityTest& );
        
        // Prohibit assigment operator
        CPosPSYHorizontalCapabilityTest& operator= ( const CPosPSYHorizontalCapabilityTest& );

    private: // Data

        TInt iAccuracyError;
		TInt iHorizontalAccuracyThreshold;
    };

#endif      // CPOSPSYHORIZONTALCAPABILITYTEST_H

// End of File

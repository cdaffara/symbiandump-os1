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



#ifndef CPOSPSYVERTICALCAPABILITYTEST_H
#define CPOSPSYVERTICALCAPABILITYTEST_H

//  INCLUDES
#include "epos_cpospsycapabilitytestbase.h"
#include "epos_tpospsytestparameters.h"

// CLASS DECLARATION
/**
*  Class for testing Vertical capability
*/
class CPosPSYVerticalCapabilityTest : public CPosPSYCapabilityTestBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CPosPSYVerticalCapabilityTest* NewLC(
        /* IN/OUT */ CPosPSYResults& aResults,
        /* IN */     const RPointerArray<TPositionInfo>& aPositionArray,
        /* IN */     const TPosPSYInfo& aPSYInfo,
		/* IN */	 const TPosPSYTestParameters& aTestParameters
        );

        /**
        * Destructor.
        */
        virtual ~CPosPSYVerticalCapabilityTest();

    public:  // Functions from base classes

        /**
        * Function for testing vertical capability, if set
        */
        void TestCapabilityL();

    private:

        /**
        * C++ constructor.
        */
        CPosPSYVerticalCapabilityTest(
        /* IN/OUT */ CPosPSYResults& aResults,
        /* IN */	 const RPointerArray<TPositionInfo>& aPositionArray,
        /* IN */     const TPosPSYInfo& aPSYInfo,
		/* IN */	 const TPosPSYTestParameters& aTestParameters
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
        CPosPSYVerticalCapabilityTest( const CPosPSYVerticalCapabilityTest& );
        
        // Prohibit assigment operator
        CPosPSYVerticalCapabilityTest& operator= ( const CPosPSYVerticalCapabilityTest& );

    private:    // Data

        TInt iAccuracyError;
		TPosPSYTestParameters iTestParameters;

    };

#endif      // CPOSPSYVERTICALCAPABILITYTEST_H

// End of File

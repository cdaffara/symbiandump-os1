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



#ifndef CPOSPSYDIRECTIONCAPABILITYTEST_H
#define CPOSPSYDIRECTIONCAPABILITYTEST_H

//  INCLUDES
#include "epos_cpospsycapabilitytestbase.h"

// CLASS DECLARATION

/**
*  Class for testing Direction capability
*/
class CPosPSYDirectionCapabilityTest : public CPosPSYCapabilityTestBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */    
        static CPosPSYDirectionCapabilityTest* NewLC(
        /* IN/OUT */ CPosPSYResults& aResults,
        /* IN */     const RPointerArray<TPositionInfo>& aPositionArray,
        /* IN */     const TPosPSYInfo& aPSYInfo
        );

        /**
        * Destructor.
        */
        virtual ~CPosPSYDirectionCapabilityTest();

    public: // Functions from base classes

        /**
        * Function for testing compass capability, if set
        */
        void TestCapabilityL();

    private:

        /**
        * C++ constructor.
        */
        CPosPSYDirectionCapabilityTest(
        /* IN/OUT */ CPosPSYResults& aResults,
        /* IN */     const RPointerArray<TPositionInfo>& aPositionArray,
        /* IN */     const TPosPSYInfo& aPSYInfo
        );

        /**
        * Function for checking result of capability test
        */
        void CheckResultsL();

        /**
		* ResetResults
		*/
		void ResetResults();

		/**
		* TestCapabilityForCourseInfoL
		*/
		void TestCapabilityForCourseInfoL();

        /**
		* TestCapabilityForGenericInfoL
		*/
		void TestCapabilityForGenericInfoL();
        
        // By default, prohibit copy constructor
        CPosPSYDirectionCapabilityTest( const CPosPSYDirectionCapabilityTest& );
        
        // Prohibit assigment operator
        CPosPSYDirectionCapabilityTest& operator= ( const CPosPSYDirectionCapabilityTest& );

    private: // Data

        TInt iHeadingNotSet;
        TInt iHeadingErrorNotSet;
        TInt iHeadingCombinationError;
        TBool iTestingPerformed;
        
    };

#endif      // CPOSPSYDIRECTIONCAPABILITYTEST_H

// End of File

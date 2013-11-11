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



#ifndef CPOSPSYCRCAPABILITYTEST_H
#define CPOSPSYCRCAPABILITYTEST_H

//  INCLUDES
#include "epos_cpospsytestbase.h"
#include "epos_cpospsyrequester.h"

// CLASS DECLARATION
/**
*  Class for PSY Central Repository Capability validation tests
*/
class CPosPSYCRCapabilityTest : public CPosPSYTestBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CPosPSYCRCapabilityTest* NewL();

        /**
        * Destructor.
        */
        ~CPosPSYCRCapabilityTest();

    public: // Functions from base classes

        /**
        * From CPosPSYTestBase GetName
        * @param aName The name of the test
        */
        void GetName(
        /* OUT */    TDes& aName
        ) const;

        /**
        * From CPosPSYTestBase StartTest function for starting execution of the test case
        * @param aPSYInfo the information about the psy to be tested
        * @param aStatus status flag from calling object
		* @param aTestParameters test parameters specifying number 
		*		 of location requests to be done etc.
        */
        void StartTest(
        /* IN */        const TPosPSYInfo& aPSYInfo, 
        /* IN/OUT */    TRequestStatus& aStatus,
		/* IN */		const TPosPSYTestParameters& aTestParameters
        );

		void CancelTest();

    private:

        /**
        * C++ default constructor.
        */
        CPosPSYCRCapabilityTest();
    
        /**
        * By default EPOC constructor is private.
        */
        void ConstructL();

        /**
        * Executes all tests of the test case
        */ 
        void ExecuteTestL();
                
        // By default, prohibit copy constructor
        CPosPSYCRCapabilityTest( const CPosPSYCRCapabilityTest& );
        
        // Prohibit assigment operator
        CPosPSYCRCapabilityTest& operator= ( const CPosPSYCRCapabilityTest& );

    };

#endif      // CPOSPSYPARAMETERTEST_H

// End of File

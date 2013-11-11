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



#ifndef CPOSPSYTESTER_H
#define CPOSPSYTESTER_H

//  INCLUDES
#include "epos_tpospsyinfo.h"
#include "epos_cpospsylog.h"
#include "epos_tpospsytestparameters.h"

// FORWARD DECLARATION
class CPosPSYTestBase;

// CONSTANTS
const TInt KTestNameLength = 100;

// CLASS DECLARATION
/**
*  Driver for performing testing on a PSY.
*/
class CPosPSYTester : public CActive
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CPosPSYTester* NewL();

        /**
        * Destructor.
        */
        virtual ~CPosPSYTester();

    public: // New functions
        
        /**
        * StartTests.
        * @param TPosPSYInfo& psyInfo Complete information 
        *  about a PSY that the user has selected to test 
        * @param aStatus status flag from calling object
		* @param aTestParameters test parameters specifying number 
		*	of location requests to be done etc.
        */
       void StartTestsL(
       /* IN */     const TPosPSYInfo& psyInfo, 
       /* IN/OUT */ TRequestStatus& aStatus,
	   /* IN */		const TPosPSYTestParameters& aTestParameters
       );
        
    public: // Functions from base classes

        /**
        * From CActive RunL.
        */
        void RunL();

        /**
        * From CActive DoCancel.
        */
        void DoCancel();

        /**
        * From CActive RunError.
        */
        TInt RunError(TInt aErr);
        
    private:

        /**
        * C++ default constructor.
        */
        CPosPSYTester();
    
        /**
        * By default EPOC constructor is private.
        */
        void ConstructL();

		void AddTestL(
            CPosPSYTestBase* aTest
        );

        // By default, prohibit copy constructor
        CPosPSYTester( const CPosPSYTester& );
        
        // Prohibit assigment operator
        CPosPSYTester& operator= ( const CPosPSYTester& );

    private:    // Data
        
        TInt							iTestIndex;
        TPosPSYInfo						iPSYInfo;
		TPosPSYTestParameters			iPSYTestParameters;
        TRequestStatus*					iHandlerStatus;
        RPointerArray<CPosPSYTestBase>	iTestArray;
        CPosPSYLog*						iLog;

    };

#endif      // CPOSPSYTESTER_H

// End of File

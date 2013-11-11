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



#ifndef CPOSPSYCAPABILITYTESTBASE_H
#define CPOSPSYCAPABILITYTESTBASE_H

//  INCLUDES
#include <e32base.h>
#include "epos_tpospsyinfo.h"
#include "epos_cpospsyresults.h"

class TPositionInfo;

// CONSTANTS
const TInt KCapabilityBufSize = 150;

// CLASS DECLARATION

/**
*  Class for testing Address capability
*/
class CPosPSYCapabilityTestBase : public CBase
    {
    public:  // Constructors and destructor

        /**
        * Base constructor for all capability tests initiates 
        * class variables 
        * @param aResults The result array to add the results
        *        of the capability tests to
        * @param aPositionArray The array containing the position 
        *        data from requests that has been made in caller class
        * @param aPSYInfo Information about the current PSY that is tested
        */
        void BaseConstruct(
        /* IN/OUT */ CPosPSYResults& aResults,
        /* IN */     const RPointerArray<TPositionInfo>& aPositionArray,
        /* IN */     const TPosPSYInfo& aPSYInfo
        );

        /**
        * Destructor.
        */
        virtual ~CPosPSYCapabilityTestBase();

    public: // New functions
        
        /**
        * Function for testing capability, if set
        */
        virtual void TestCapabilityL() = 0;

    protected:

        /**
        * C++ default constructor.
        */
        CPosPSYCapabilityTestBase();
    
        /**
        * Function adding a result from capability tests to result array
        * @param aMessage the message string of the result
        * @param aResultType the type of result to be added
        */
        void AddResultL(
        /* IN */ const TDesC& aMessage, 
        /* IN */ const TInt aResultType
        );

        /**
        * Function for checking result of capability test
        */
        virtual void CheckResultsL() = 0;

    private:

        // By default, prohibit copy constructor
        CPosPSYCapabilityTestBase( const CPosPSYCapabilityTestBase& );
        
        // Prohibit assigment operator
        CPosPSYCapabilityTestBase& operator= ( const CPosPSYCapabilityTestBase& );

    protected:    // Data
        
        TPosPSYInfo iPSYInfo;
        CPosPSYResults* iTestResults;
        const RPointerArray<TPositionInfo>* iPositionArray;

    };

#endif      // CPOSPSYCAPABILITYTESTBASE_H

// End of File

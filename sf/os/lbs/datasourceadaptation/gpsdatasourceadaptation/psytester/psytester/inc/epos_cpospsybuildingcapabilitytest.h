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



#ifndef CPOSPSYBUILDINGCAPABILITYTEST_H
#define CPOSPSYBUILDINGCAPABILITYTEST_H

//  INCLUDES
#include <lbspositioninfo.h>
#include "epos_cpospsycapabilitytestbase.h"

// CONSTANTS
const TInt KBuildingResultBufSize = 300;

// CLASS DECLARATION

/**
*  Class for testing Building capability
*/
class CPosPSYBuildingCapabilityTest : public CPosPSYCapabilityTestBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CPosPSYBuildingCapabilityTest* NewLC(
        /* IN/OUT */ CPosPSYResults& aResults,
        /* IN */     const RPointerArray<TPositionInfo>& aPositionArray,
        /* IN */     const TPosPSYInfo& aPSYInfo
        );

        /**
        * Destructor.
        */
        virtual ~CPosPSYBuildingCapabilityTest();

    public:  // Functions from base classes

        /**
        * Function for testing building capability, if set
        */
        void TestCapabilityL();

    private:

        /**
        * C++ constructor.
        */
        CPosPSYBuildingCapabilityTest(
        /* IN/OUT */ CPosPSYResults& aResults,
        /* IN */     const RPointerArray<TPositionInfo>& aPositionArray,
        /* IN */     const TPosPSYInfo& aPSYInfo
        );

        /**
        * Function for checking result of capability test
        */
        void CheckResultsL();

        // By default, prohibit copy constructor
        CPosPSYBuildingCapabilityTest( const CPosPSYBuildingCapabilityTest& );
        
        // Prohibit assigment operator
        CPosPSYBuildingCapabilityTest& operator= ( const CPosPSYBuildingCapabilityTest& );

    private: // Data
        
        TBool iNoneSet;
        TInt iNrOfNoneSet;
        TInt iNrOfBuildingNameNotSet;
        TInt iNrOfBuildingFloorNotSet;
        TInt iNrOfBuildingRoomNotSet;
        TInt iNrOfBuildingZoneNotSet;
        TInt iNrOfBuildingTelephoneNotSet;

    };

#endif      // CPOSPSYBUILDINGCAPABILITYTEST_H

// End of File
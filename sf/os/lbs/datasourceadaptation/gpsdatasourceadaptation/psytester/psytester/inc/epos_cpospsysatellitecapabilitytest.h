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



#ifndef CPOSPSYSATELLITECAPABILITYTEST_H
#define CPOSPSYSATELLITECAPABILITYTEST_H

//  INCLUDES
#include "epos_cpospsycapabilitytestbase.h"

// CLASS DECLARATION
/**
*  Class for testing Satellite capability
*/
class CPosPSYSatelliteCapabilityTest : public CPosPSYCapabilityTestBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CPosPSYSatelliteCapabilityTest* NewLC(
        /* IN/OUT */ CPosPSYResults& aResults,
        /* IN */     const RPointerArray<TPositionInfo>& aPositionArray,
        /* IN */     const TPosPSYInfo& aPSYInfo
        );

        /**
        * Destructor.
        */
        virtual ~CPosPSYSatelliteCapabilityTest();

    public:  // Functions from base classes

        /**
        * TestCapabilityL. Function for testing the Satellite capability, if supported 
        */
        void TestCapabilityL();
        
    private:

        /**
        * C++ constructor.
        */
        CPosPSYSatelliteCapabilityTest(
        /* IN/OUT */ CPosPSYResults& aResults,
        /* IN */     const RPointerArray<TPositionInfo>& aPositionArray,
        /* IN */     const TPosPSYInfo& aPSYInfo
        );

        /**
        * Function for checking result of capability test
        */
        void CheckResultsL();

        /**
		* Function for checking satellite data for satellite info
		* @param satelliteInfo, satellite information object containing information
		*		 from a location request made with TPositionSatelliteInfo
		* @param nrOfSatellites, the total number of satellites that satellite data 
		*		 should be check for		 
		*/
		void CheckSatelliteDataL(
		/* IN */	const TPositionSatelliteInfo& satelliteInfo,
		/* IN */	TInt nrOfSatellites
		);

		void ResetResults();

        void TestCapabilityForGenericInfoL();

        void TestCapabilityForSatelliteInfoL();
       
        // By default, prohibit copy constructor
        CPosPSYSatelliteCapabilityTest( const CPosPSYSatelliteCapabilityTest& );
        
        // Prohibit assigment operator
        CPosPSYSatelliteCapabilityTest& operator= ( const CPosPSYSatelliteCapabilityTest& );

    private: // Data
        
        TInt iNumInViewNotSet;
        TInt iNumInViewLessThanZero;
        TInt iNumInUseNotSet;
        TInt iNumInUseLessThanZero;
        TInt iNumInUseGreaterThanNumInView;
        TInt iTimeNotSet;
        TInt iTimeLessThanZero;
        TInt iHorizontalDoPNotSet;
        TInt iVerticalDoPNotSet;
        TInt iTimeDoPNotSet;
        TInt iPositionDoPNotSet;
        TInt iWgs84AltitudeNotSet;
        TInt iGeoidalSeparationNotSet;
		TInt iNrOfUsedSatellitesMissmatch;
		TInt iNumOfSatelliteIdNotSet;
		TInt iNumOfAzimuthNotSet;
		TInt iNumOfElevationNotSet;
		TInt iNumOfSignalStrengthNotSet;
        TBool iTestingPerformed;

    };

#endif      // CPOSPSYSATELLITECAPABILITYTEST_H

// End of File

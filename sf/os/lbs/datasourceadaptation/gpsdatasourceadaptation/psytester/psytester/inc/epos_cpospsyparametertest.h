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



#ifndef CPOSPSYPARAMETERTEST_H
#define CPOSPSYPARAMETERTEST_H

//  INCLUDES
#include "epos_cpospsytestbase.h"
#include "epos_cpospsyrequester.h"

// CONSTANTS
const TInt KParameterResultBufSize = 175;

// CLASS DECLARATION
/**
*  Class for PSY parameter validation tests
*/
class CPosPSYParameterTest : public CPosPSYTestBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CPosPSYParameterTest* NewL();

        /**
        * Destructor.
        */
        ~CPosPSYParameterTest();

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
        CPosPSYParameterTest();
    
        /**
        * By default EPOC constructor is private.
        */
        void ConstructL();

        /**
        * Executes all tests of the test case
        */ 
        void ExecuteTestL();
        
        /**
        * If interface version is negative generates an error
        */
        void CheckInterfaceVersionL();
        
        /**
        * If priority is anything other than auto, first, last,
        * ganerate an error message
        */
        
        void CheckPriorityL();
        
        /**
        * If availability status is anything other than available, 
        * not available, ganerate an error message
        */
        void CheckStatusL();
        
        /**
        * If time to first fix is more than twelve minutes 
        * or less than zero minutes generate an error message
        */
        void CheckTimeToFirstFixL();
        
        /**
        * If time to first fix is more than time to first fix
        * or less than zero minutes generate an error message
        */
        void CheckTimeToFixL();
        
        /**
        * CheckVerticalAccuracy
        */
        void CheckVerticalAccuracyL();
        
        /**
        * CheckHorizontalAccuracy
        */
        void CheckHorizontalAccuracyL();
        
        /**
        * CheckCostIndicator
        */
        void CheckCostIndicatorL();
        
        /**
        * CheckPowerConsumption
        */
        void CheckPowerConsumptionL();
        
        /**
        * CheckTechnologyType
        */
        void CheckTechnologyTypeL();
        
        /**
        * CheckDeviceLocation
        */
        void CheckDeviceLocationL();
        
        /**
        * CheckCapabilities
        */
        void CheckCapabilitiesL();
        
        /**
        * CheckClassesSupported
        */
        void CheckClassesSupportedL();
        
        /**
        * CheckVisibility
        */
        void CheckVisibilityL();    

        // By default, prohibit copy constructor
        CPosPSYParameterTest( const CPosPSYParameterTest& );
        
        // Prohibit assigment operator
        CPosPSYParameterTest& operator= ( const CPosPSYParameterTest& );

    private:    // Data
		TInt iMaxTimeToFirstFix;
		TInt iMaxVerticalAccuracy;
		TInt iReasonableVerticalAccuracy;
		TInt iMaxHorizontalAccuracy;
		TInt iReasonableHorizontalAccuracy;
      
    };

#endif      // CPOSPSYPARAMETERTEST_H

// End of File

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



#ifndef CPOSPSYTESTHANDLER_H
#define CPOSPSYTESTHANDLER_H

//  INCLUDES
#include "epos_cpospsyecomhandler.h"
#include "epos_cpospsytester.h"
#include "epos_tpospsytestparameters.h"
#include "epos_cpospsyconsolemanager.h"

// FORWARD DECLARATION
class CPosPSYConsole;

// CONSTANTS
const TInt KMessageBufSize = 60;

// CLASS DECLARATION
/**
*  Driver for testing one or several PSYs
*/
class CPosPSYTestHandler : public CActive
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CPosPSYTestHandler* NewL();

        /**
        * Destructor.
        */
        virtual ~CPosPSYTestHandler();
        

    public: // New functions
		
		/**
        * Starts the PSY tester to test 
        * with the PSY at the specified index
        *
		* @param TInt aIndex, The index for the PSY to be tested
        */
        void StartPSYTesterL(
        /* IN */ TInt aIndex
        );

        /**
        * Returns the array with implementation information about all installed PSYs
        *
		* @return RArray<TPosPSYBaseInfo>, The array with the implementation information
        */
        RArray<TPosPSYBaseInfo> InfoList();
    
        /**
        * Retrieves information about all installed PSYs by calling CPosPSYEComHandler  
        */
        void UpdatePSYListL();

		/**
		* Checks if test is running or not.
		* @return ETrue if test is running, otherwise EFalse.
		*/
		TBool TestsRunning();

		/**
		* Function for accessing the test parameters 
		* read from resource file
		* @param aTestParameters, The parameters for the tests
		*/
		void GetTestParameters(
		/* OUT */ TPosPSYTestParameters& aTestParameters
		);
		
		/**
		* Function for overriding the test parameters
		* read from resource file
		* @param, The new test parameters for the test
		*/
		void SetTestParameters(
		/* IN */ TPosPSYTestParameters aTestParameters
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
        TInt RunError(
		/* IN */ TInt aErr
		);

    private:

	
	
		/**
		* Reads the test parameters for PSY Tester from resource file
		* @param object to store values in
		*/
		
		void ReadResourceValuesL( 
		/* IN/OUT */  TPosPSYTestParameters& aPSYParameters
		); 

	     /**
        * C++ default constructor.
        */
        CPosPSYTestHandler();
    
        /**
        * By default EPOC constructor is private.
        */
        void ConstructL();

        // By default, prohibit copy constructor
        CPosPSYTestHandler( const CPosPSYTestHandler& );
       
        // Prohibit assigment operator
        CPosPSYTestHandler& operator= ( const CPosPSYTestHandler& );
        
 		// A new method to exclude the list of PSYs specified
		// in the resource file 
		/**
        * Removes the list of PSYs to be excluded from iInfoArray
        */
		void ExcludePSYsL();

    private:    // Data
		
        CPosPSYConsoleManager*  iConsoleManager;
        CPosPSYTester*			iPSYTester;
           	
        CPosPSYEComHandler*		iPSYEComHandler;
        RArray<TPosPSYBaseInfo> iInfoArray;
        TPosPSYTestParameters	iTestParameters;
		TBool					iTestAll;
        TInt					iPSYIndex;

    };

#endif      // CPOSPSYTESTHANDLER_H

// End of File

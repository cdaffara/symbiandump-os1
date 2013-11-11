/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: STIF testclass declaration
*
*/

#ifndef LOCINFOCONVERSIONUNITTEST_H
#define LOCINFOCONVERSIONUNITTEST_H

//  INCLUDES
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>
#include <TestclassAssert.h>
#include <e32base.h>
#include "LocInfoConversionHandler.h"
#include <lbs/lbslocationclasstypes.h>

// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def
#define TEST_CLASS_VERSION_MAJOR 0
#define TEST_CLASS_VERSION_MINOR 0
#define TEST_CLASS_VERSION_BUILD 0

// Logging path
_LIT( KLocInfoConversionUnitTestLogPath, "\\logs\\testframework\\LocInfoConversionUnitTest\\" ); 
// Log file
_LIT( KLocInfoConversionUnitTestLogFile, "LocInfoConversionUnitTest.txt" ); 
_LIT( KLocInfoConversionUnitTestLogFileWithTitle, "LocInfoConversionUnitTest_[%S].txt" );

// FUNCTION PROTOTYPES
//?type ?function_name(?arg_list);

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;
class CLocInfoConversionUnitTest;
class CLbsPositionInfo;
class CLbsGsmCellInfo;
class CLbsWcdmaCellInfo;
class CLbsWlanInfo;
class CLbsLocationInfo;

enum TTestType
    {
    EPositive,
    
    ENegative
    };

// DATA TYPES
//enum ?declaration
//typedef ?declaration
//extern ?data_type;

// CLASS DECLARATION

/**
*  CLocInfoConversionUnitTest test class for STIF Test Framework TestScripter.
*  ?other_description_lines
*
*  @lib ?library
*  @since ?Series60_version
*/
NONSHARABLE_CLASS(CLocInfoConversionUnitTest) : public CScriptBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CLocInfoConversionUnitTest* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~CLocInfoConversionUnitTest();

    public: // New functions

        /**
        * ?member_description.
        * @since ?Series60_version
        * @param ?arg1 ?description
        * @return ?description
        */
        //?type ?member_function( ?type ?arg1 );

    public: // Functions from base classes

        /**
        * From CScriptBase Runs a script line.
        * @since ?Series60_version
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code
        */
        virtual TInt RunMethodL( CStifItemParser& aItem );
        
        protected:  // New functions

        /**
        * ?member_description.
        * @since ?Series60_version
        * @param ?arg1 ?description
        * @return ?description
        */
        //?type ?member_function( ?type ?arg1 );

    protected:  // Functions from base classes

        /**
        * From ?base_class ?member_description
        */
        //?type ?member_function();

    private:

        /**
        * C++ default constructor.
        */
        CLocInfoConversionUnitTest( CTestModuleIf& aTestModuleIf );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        // Prohibit copy constructor if not deriving from CBase.
        // ?classname( const ?classname& );
        // Prohibit assigment operator if not deriving from CBase.
        // ?classname& operator=( const ?classname& );

        /**
        * Frees all resources allocated from test methods.
        * @since ?Series60_version
        */
        void Delete();

        /**
        * Test methods are listed below. 
        */

        /**
        * Example test method.
        * @since ?Series60_version
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        TInt RLbsLocInfoConverter_Connect1L(CStifItemParser& aItem );
        TInt RLbsLocInfoConverter_Connect2L(CStifItemParser& aItem );
        TInt RLbsLocInfoConverter_ConvertLocationInfo1L(CStifItemParser& aItem );
        TInt RLbsLocInfoConverter_ConvertLocationInfo2L(CStifItemParser& aItem );
        TInt RLbsLocInfoConverter_ConvertLocationInfo3L(CStifItemParser& aItem );
        TInt RLbsLocInfoConverter_ConvertLocationInfo4L(CStifItemParser& aItem );
        TInt RLbsLocInfoConverter_ConvertLocationInfo5L(CStifItemParser& aItem );
        TInt RLbsLocInfoConverter_ConvertLocationInfo6L(CStifItemParser& aItem );
        TInt RLbsLocInfoConverter_ConvertLocationInfo7L(CStifItemParser& aItem );
        TInt RLbsLocInfoConverter_ConvertLocationInfo8L(CStifItemParser& aItem );
        TInt RLbsLocInfoConverter_ConvertLocationInfo9L(CStifItemParser& aItem );
        TInt RLbsLocInfoConverter_ConvertLocationInfo10L(CStifItemParser& aItem );
        TInt RLbsLocInfoConverter_Cancel1L(CStifItemParser& aItem );
        TInt RLbsLocInfoConverter_Cancel2L(CStifItemParser& aItem );
        TInt RLbsLocInfoConverter_Cancel3L(CStifItemParser& aItem );
        TInt RLbsLocInfoConverter_Cancel4L(CStifItemParser& aItem );
        
        /**
         * Method used to log version of test class
         */
        void SendTestClassVersion();
        
        /**
         * Helper methods.
         */
        CLbsPositionInfo* CreatePositionInfoObjectLC();
        
        CLbsGsmCellInfo* CreateGsmCellInfoObjectLC();
        
        CLbsWcdmaCellInfo* CreateWcdmaCellInfoObjectLC();
        
        CLbsWlanInfo* CreateWlanInfoObjectLC();
        
        CLbsLocationInfo* CreateLocationInfoObjectLC( TLbsAreaInfoClassType aAreaType );
        
        void AppendAreaInfoObjectToLocationInfoL( CLbsLocationInfo* aLocationInfo,                    
                                                    TLbsAreaInfoClassType aAreaType );
        
        void CheckPositionConvertedInfoL( CLbsLocationInfo* aLocationInfo,
                                                    TTestType aTestType = EPositive );
        
        void CheckGsmCellConvertedInfoL( CLbsLocationInfo* aLocationInfo,
                                                    TTestType aTestType = EPositive  );
        
        void CheckWcdmaCellConvertedInfoL( CLbsLocationInfo* aLocationInfo,
                                                    TTestType aTestType = EPositive  );
        
        void CheckWlanConvertedInfoL( CLbsLocationInfo* aLocationInfo,
                                                    TTestType aTestType = EPositive  );
    public:     // Data
        // ?one_line_short_description_of_data
        //?data_declaration;

    protected:  // Data
        // ?one_line_short_description_of_data
        //?data_declaration;

    private:    // Data
    CLocInfoConversionHanlder* iConversionHandler; 
    
    CLocInfoConversionHanlder* iConversionHandler1; 
    public:     // Friend classes
        //?friend_class_declaration;
    protected:  // Friend classes
        //?friend_class_declaration;
    private:    // Friend classes
        //?friend_class_declaration;

    };

#endif      // LOCINFOCONVERSIONUNITTEST_H

// End of File

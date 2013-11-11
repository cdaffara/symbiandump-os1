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

#ifndef TE_LOCATIONINFOCONVERTER_H
#define TE_LOCATIONINFOCONVERTER_H

//  INCLUDES


#include <lbs/lbslocationinfoconverter.h>
#include <lbs/lbslocationinfoconvertercommon.h>
#include <lbs/lbslocationclasstypes.h>
#include <lbs/lbslocationcommon.h>
#include <lbs/lbslocationposinfo.h>
#include <lbs/lbslocationgsminfo.h>
#include <lbs/lbslocationgsmneighbourinfo.h>
#include <lbs/lbslocationinfo.h>
#include <lbs/lbslocationwcdmainfo.h>
#include <lbs/lbslocationwcdmaneighbourinfo.h>
#include <lbs/lbslocationwlaninfo.h>
#include <LbsPosition.h> 


#include "Te_locationinfoconversionobserver.h"

// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def
#define TEST_CLASS_VERSION_MAJOR 0
#define TEST_CLASS_VERSION_MINOR 0
#define TEST_CLASS_VERSION_BUILD 0

// Logging path
_LIT( KTe_locationinfoconverterLogPath, "c:\\logs\\testframework\\" ); 
// Log file
_LIT( KTe_locationinfoconverterLogFile, "Te_locationinfoconverter.txt" ); 
_LIT( KTe_locationinfoconverterLogFileWithTitle, "Te_locationinfoconverter_[%S].txt" );

// FUNCTION PROTOTYPES
//?type ?function_name(?arg_list);

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;
class CTe_locationinfoconverter;

// DATA TYPES
//enum ?declaration
//typedef ?declaration
//extern ?data_type;

// CLASS DECLARATION

/**
*  CTe_locationinfoconverter test class for STIF Test Framework TestScripter.
*  ?other_description_lines
*
*  @lib ?library
*  @since ?Series60_version
*/
NONSHARABLE_CLASS(CTe_locationinfoconverter) : public CScriptBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CTe_locationinfoconverter* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~CTe_locationinfoconverter();

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
        CTe_locationinfoconverter( CTestModuleIf& aTestModuleIf );

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
        virtual TInt ConvertLocationInfoL( CStifItemParser& aItem );
        virtual TInt CreateLocationInfoConverterL( CStifItemParser& aItem );
        virtual TInt SetCoordinateInfoL( CStifItemParser& aItem );
        virtual TInt SetGSMCellInfoL( CStifItemParser& aItem );
        virtual TInt SetWCDMACellInfoL( CStifItemParser& aItem );
        virtual TInt SetWLanInfoL( CStifItemParser& aItem );
        virtual TInt AddLocationInfoL( CStifItemParser& aItem );
        virtual TInt ValidateLocationInfoL( CStifItemParser& aItem );
        virtual TInt MiscL( CStifItemParser& aItem );
        virtual TInt Cleanup( CStifItemParser& aItem );
        



    private:    // Data
        
        // ?one_line_short_description_of_data
        //?data_declaration;
        CLbsLocationInfoConverter* ilbslocationinfoconverter;
        CLbsLocationInfo* ilbslocationinfo;
        CTelocationinfoconversionobserver* iObserver;
        RLbsLocationInfoBaseArray ilbslocationinfoarray;
        RLbsLocationInfoBaseArray ilbslocationinfoarrayTemp;
        


    };

#endif      // TE_LOCATIONINFOCONVERTER_H

// End of File

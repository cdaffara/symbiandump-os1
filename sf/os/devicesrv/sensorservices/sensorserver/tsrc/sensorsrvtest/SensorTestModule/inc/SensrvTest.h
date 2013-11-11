/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  ?Description
*
*/



#ifndef SENSRVTEST_H
#define SENSRVTEST_H

//  INCLUDES
#include <e32property.h>
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>
#include "sensrvtypes.h"
#include <sensrvdatalistener.h>
#include "sensrvtestcases.h"


// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def
// Function pointer related internal definitions


// Logging path
_LIT( KSensrvTestLogPath, "\\logs\\testframework\\SensrvTest\\" ); 
// Log file
_LIT( KSensrvTestLogFile, "SensrvTest.txt" ); 


// FUNCTION PROTOTYPES
//?type ?function_name(?arg_list);

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;
class CSensrvSensorChannelFinder;
class CSensrvSensorChannel;
class CSensrvTestNotifier;
class CSensrvChannelConditionSetImpl;

// DATA TYPES
//enum ?declaration
//typedef ?declaration
//extern ?data_type;



// CLASS DECLARATION

/**
*  CSensrvTest test class for STIF Test Framework TestScripter.
*  ?other_description_lines
*
*  @lib ?library
*  @since ?Series60_version
*/
NONSHARABLE_CLASS(CSensrvTest) : public CScriptBase//, public MSensrvDataListener
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CSensrvTest* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~CSensrvTest();

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
        CSensrvTest( CTestModuleIf& aTestModuleIf );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
        
        
        void DefinePSKeyL( TUint32 aKey, TInt aAttr );
        
        TInt CheckTest();
        
        TInt DoRunAndWaitTest( CSensrvTestCases::TSensrvTestCase aTestCase, 
                                        CStifItemParser& aItem );
        
        TInt CreateNewThread();
        
        static TInt StartThread( TAny* aParameter );
        
        void DoStartThreadL();
        

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
        * Set error code.
        * Parameters: SRKey form SensrvInternalPSKeys.h(TUint32), Error code(TInt)
        * @since S60 5.0
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        virtual TInt SetPSValue( CStifItemParser& aItem );
        
        /**
        * Set PS value for process purpose
        */
        virtual TInt SetProcessPSValue( CStifItemParser& aItem );
        
        /**
        * Set completed key form SensrvInternalPSKeys.h
        * Parameters: SRKey form SensrvInternalPSKeys.h(TUint32)
        * @since S60 5.0
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        virtual TInt SetCompletedKeyL( CStifItemParser& aItem );
        
        /**
        * Set GetData delay
        * Parameters: Delay in millisecond
        * @since S60 5.0
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        virtual TInt SetGetDataDelay( CStifItemParser& aItem );
        
        /**
        * check test return value
        * Parameters: 
        * @since S60 5.0
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        virtual TInt WaitCompletedKey( CStifItemParser& aItem );
        
        /**
        * Shutdown sensor server
        * @since S60 5.0
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        virtual TInt ShutdownServer( CStifItemParser& aItem );
        /**
        * Start sensor server
        * @since S60 5.0
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        virtual TInt StartServer( CStifItemParser& aItem );
        
        /**
        * Create and destroy channel finder.
        * @since S60 5.0
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        virtual TInt CreateAndDestroyChannelFinderL( CStifItemParser& aItem );
        
        /**
        * Find channels.
        * Parameters: ChannelId(TUint32), ContextType(TInt), Quantity(TInt), 
        *              ChannelType(TUint32), dataSize(TInt), Location(TDesC8), VendorId(TDesC8)(optional)
        * @since S60 5.0
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        virtual TInt FindChannelsL( CStifItemParser& aItem );
        
        /**
        * Open the channel 
        * Parameters: ChannelId(TUint32), ContextType(TInt), Quantity(TInt), 
        *              ChannelType(TUint32), dataSize(TInt), Location(TDesC8), VendorId(TDesC8)(optional)
        * @since S60 5.0
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */    
        virtual TInt OpenChannelL( CStifItemParser& aItem );
        
        /**
        * Close the channel 
        *
        * @since S60 5.0
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */    
        virtual TInt CloseChannelL( CStifItemParser& aItem );
        
        /**
        * Open close performance test. 
        * Parameters: Open-Close Count(TInt)
        * @since S60 5.0
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */    
        virtual TInt OpenClosePerformanceL( CStifItemParser& aItem );
        
        /**
        * Start channel data listening 
        * Parameters: dataCount, timeout, repeatCount
        * @since S60 5.0
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */    
        virtual TInt StartDataListeningL( CStifItemParser& aItem );
        
        /**
        * Stop channel data listening 
        *
        * @since S60 5.0
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */    
        virtual TInt StopDataListeningL( CStifItemParser& aItem );
        
        /**
        * Start channel data listening when channel is not open
        * Parameters: ChannelId(TUint32), ContextType(TInt), Quantity(TInt), 
        *              ChannelType(TUint32), dataSize(TInt), Location(TDesC8), VendorId(TDesC8)(optional)
        * @since S60 5.0
        * @param aItem Script line containing parameters.
        * @return void Description
        */
        virtual TInt StartListeningWhenChannelIsNotOpenL( CStifItemParser& aItem );
        
        /**
        * Panic client
        *
        * @since S60 5.0
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */    
        virtual TInt PanicClient( CStifItemParser& aItem );
        
        /**
        * Wait SSY unloading
        *
        * @since S60 5.0
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */    
        virtual TInt WaitSSYUnloading( CStifItemParser& aItem );

        /**
        * Start/Stop channel property listening 
        * @since S60 5.0
        * @param aItem Script line containing parameters.
        * @return void Description
        */    
        virtual TInt StartPropertyListeningL( CStifItemParser& aItem );
        virtual TInt StopPropertyListeningL( CStifItemParser& aItem );
        
        virtual TInt GetPropertyL( CStifItemParser& aItem );
        virtual TInt SetPropertyL( CStifItemParser& aItem );
        virtual TInt GetAllPropertiesL( CStifItemParser& aItem );
        virtual TInt CheckPropertyChangedCount( CStifItemParser& aItem );
        virtual TInt GetAllPropertiesWithPropertyIdL( CStifItemParser& aItem );
        virtual TInt GetAllPropertiesWithArrayIndexL( CStifItemParser& aItem );
        
        virtual TInt CreateNewProcess( CStifItemParser& aItem );
        virtual TInt KillProcess( CStifItemParser& aItem );
        
        virtual TInt ConditionUnitTestL( CStifItemParser& aItem );
        virtual TInt ConditionSetUnitTestL( CStifItemParser& aItem );
        
        virtual void CompareConditionSetsL(CSensrvChannelConditionSetImpl* aSet1, 
                                           CSensrvChannelConditionSetImpl* aSet2);

        virtual TInt AddConditionL( CStifItemParser& aItem );
        virtual TInt AddConditionSetL( CStifItemParser& aItem );
        virtual TInt RemoveConditionsL( CStifItemParser& aItem );

        /**
        * Start channel condition listening 
        * Parameters: dataCount, timeout, repeatCount
        * @since S60 5.0
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */    
        virtual TInt StartConditionListeningL( CStifItemParser& aItem );
        
        /**
        * Stop channel condition listening 
        *
        * @since S60 5.0
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */    
        virtual TInt StopConditionListeningL( CStifItemParser& aItem );

        virtual TInt CheckConditionMetCount( CStifItemParser& aItem );
        
        virtual TInt CheckFoundChannelCount( CStifItemParser& aItem );

        virtual TInt StartChannelChangeListeningL( CStifItemParser& aItem );
        virtual TInt StopChannelChangeListeningL( CStifItemParser& aItem );
        virtual TInt AdjustChannelsL( CStifItemParser& aItem );
        virtual TInt GetPsResult( CStifItemParser& aItem );
        virtual TInt StoreFoundChannelsL( CStifItemParser& aItem );
        virtual TInt GetPSValue( CStifItemParser& aItem );
        
        virtual TInt CheckListenedSetPropertySuccessIndicator( CStifItemParser& aItem );
        virtual TInt CheckServerAlive ( CStifItemParser& aItem );

        virtual TInt SetDefaultValueL ( CStifItemParser& aItem );
        
        //Regression Tests
        virtual TInt TestOpenClose ( CStifItemParser& aItem );
        virtual TInt TestOpenCloseSensorL ( CStifItemParser& aItem );
        virtual TInt TestOpenCloseSensorsL ( CStifItemParser& aItem );
        virtual TInt TestOpenCloseCombinationSensorsL ( CStifItemParser& aItem );
        virtual TInt TestChannelsL ( CStifItemParser& aItem );
        virtual TInt TestChannelFinderL ( CStifItemParser& aItem );
        
    public:     // Data
        // ?one_line_short_description_of_data
        //?data_declaration;

    protected:  // Data
        // ?one_line_short_description_of_data
        //?data_declaration;

    private:    // Data
    
        RThread iThread;
        
        CSensrvTestCases* iTestCases;
        
        
        TInt iCompletedKey;
        


    };



#endif      // SENSRVTEST_H

// End of File

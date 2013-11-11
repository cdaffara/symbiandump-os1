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



#ifndef PSMTEST_H
#define PSMTEST_H

//  INCLUDES
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>
#include <psmclientobserver.h>

// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def
// Logging path
_LIT( KPSMTestLogPath, "\\logs\\testframework\\PSMTest\\" ); 
// Log file
_LIT( KPSMTestLogFile, "PSMTest.txt" ); 

// FUNCTION PROTOTYPES
//?type ?function_name(?arg_list);

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;
class CPSMTest;
class CPsmClient;
class CPsmNotifier;
class CRepository;

// DATA TYPES
//enum ?declaration
//typedef ?declaration
//extern ?data_type;

/**
 * Observer class for PSM change notifications

NONSHARABLE_CLASS(CPSMTestObserver) : public CBase,
                                      public MPsmClientObserver
    {
    public:
    /**
     * Constructor.
    CPSMTestObserver();

    /**
     * Destructor.
   /* virtual ~CPSMTestObserver();

    public:
    // From MPsmChangeCompleteObserver
    virtual void PowerSaveModeChanged( const TPsmsrvMode aMode );

    // From MPsmChangeCompleteObserver
    virtual void PowerSaveModeChangeError( const TInt aError );

    public: // data

    TInt iNewMode;

    };
*/

/**
 * Active object observer class for PSM change notifications
*/
/*NONSHARABLE_CLASS(CPSMTestActiveObject) : public CActive
    {
    public:
    /**
     * Constructor.
     CPSMTestActiveObject();

    /**
     * Destructor.
     virtual ~CPSMTestActiveObject();

    public:

    TRequestStatus& RequestStatus();

    protected:

    public: // data

    TInt iNewMode;

    };

*/

// CLASS DECLARATION

/**
*  CSSYTest test class for STIF Test Framework TestScripter.
*  ?other_description_lines
*
*  @lib ?library
*  @since ?Series60_version
*/
NONSHARABLE_CLASS(CPSMTest) : public CScriptBase,
                              public MPsmClientObserver
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CPSMTest* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~CPSMTest();

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
        CPSMTest( CTestModuleIf& aTestModuleIf );

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
        virtual TInt InitializeTest( CStifItemParser& aItem );

        virtual TInt FullPowerSaveModeChange( CStifItemParser& aItem );

        virtual TInt NotifyPowerSaveModeChangeInit( CStifItemParser& aItem );

        virtual TInt PsmWaitComplete( CStifItemParser& aItem );

        virtual TInt NotifyPowerSaveModeChangeEnd( CStifItemParser& aItem );

        virtual TInt CancelPowerSaveModeChange( CStifItemParser& aItem );

        virtual TInt CheckPsmMode( CStifItemParser& aItem );

        virtual TInt GetCurrentSettings( CStifItemParser& aItem );

        virtual TInt BackupSettings( CStifItemParser& aItem );

        virtual TInt PsmNotificationInit( CStifItemParser& aItem );

        virtual TInt PsmNotificationCancel( CStifItemParser& aItem );

        virtual TInt PsmNotificationEnd( CStifItemParser& aItem );

        virtual TInt LoadAndUnload( CStifItemParser& aItem );

        virtual TInt CenRepChange( CStifItemParser& aItem );

        virtual TInt VerifyCenRepChangeL( CStifItemParser& aItem );

        virtual TInt ChangeSettingsInit( CStifItemParser& aItem );

        virtual TInt ChangeSettings( CStifItemParser& aItem );

        virtual TInt ChangeSettingsEnd( CStifItemParser& aItem );

        virtual TInt OpenClosePerformanceInit( CStifItemParser& aItem );
        virtual TInt OpenClosePerformanceRequest( CStifItemParser& aItem );
        virtual TInt OpenClosePerformanceCancel( CStifItemParser& aItem );
        virtual TInt OpenClosePerformanceChange( CStifItemParser& aItem );
        virtual TInt OpenClosePerformanceEnd( CStifItemParser& aItem );

        virtual TInt ChangePsmPerformance( CStifItemParser& aItem );

        virtual TInt ChangeSettingsPerformance( CStifItemParser& aItem );

        // Fills config array with correct values to mode
        virtual void GenerateConfigArray( RConfigInfoArray& aPsmConfigArray, TInt aMode, TInt aIncrement );

        // Compare two TPsmConfigInfos 
        virtual TBool Compare( TPsmsrvConfigInfo aTarget, TPsmsrvConfigInfo aSource );







        // From MPsmChangeCompleteObserver
        virtual void PowerSaveModeChanged( const TPsmsrvMode aMode );

        // From MPsmChangeCompleteObserver
        virtual void PowerSaveModeChangeError( const TInt aError );

#ifdef _DEBUG
        TInt OOMNotifyPowerSaveModeChangeInitL( CStifItemParser& aItem );
        TInt OOMCancelPowerSaveModeChangeL( CStifItemParser& aItem );
        TInt OOMPsmBackupAndGetSettingsL( CStifItemParser& /*aItem*/ );
        TInt OOMPsmGetSettingsL( CStifItemParser& /*aItem*/ );
        TInt OOMPsmBackupSettingsL( CStifItemParser& /*aItem*/ );      
#endif //_DEBUG     

    public:     // Data
        // ?one_line_short_description_of_data
        //?data_declaration;

    protected:  // Data
        // ?one_line_short_description_of_data
        //?data_declaration;

        CActiveSchedulerWait iSchedulerWait;

        CPsmClient* iPsmClient;

        //CPsmNotifier* iPsmNotifier;

        TInt iErrorCode;

        TInt iChangedMode;

        //CPSMTestObserver* iChangeObserver;

        TBool iPsmComplete;

        // For Settings API tests
        RConfigInfoArray iPsmConfigArray;

        // For performance tests
        //RArray<CPsmNotifier*> iNotifierArray;
        //RArray<CPSMTestObserver*> iObserverArray;

        // For noitifications
        CRepository* iRepository;

    private:    // Data


    };

#endif      // PSMTEST_H

// End of File

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
* Description:  MediatorTest test module.
*
*/



#ifndef MEDIATORTEST_H
#define MEDIATORTEST_H

// INCLUDES
#include "StifTestModule.h"
#include <StifLogger.h>
#include "MediatorCommandInitiator.h"
#include "MediatorCommandResponder.h"
#include "MediatorEventProvider.h"
#include "MediatorEventConsumer.h"
#include "MediatorNotifications.h"
#include "MediatorServiceDefs.h"
#include "MediatorTestHelper.h"
#include "TestScheduler.h"

// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def

// Logging path
_LIT( KMediatorTestLogPath, "\\logs\\testframework\\MediatorTest\\" ); 
// Log file
_LIT( KMediatorTestLogFile, "MediatorTest.txt" ); 

// Function pointer related internal definitions
#ifndef __GCC32__
#define GETPTR
#else
#define GETPTR &
#endif  
#define ENTRY(str,func) {_S(str), GETPTR func,0,0,0}
#define FUNCENTRY(func) {_S(#func), GETPTR func,0,0,0}
#define OOM_ENTRY(str,func,a,b,c) {_S(str), GETPTR func,a,b,c}
#define OOM_FUNCENTRY(func,a,b,c) {_S(#func), GETPTR func,a,b,c}

// FUNCTION PROTOTYPES
//?type ?function_name(?arg_list);

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;
class CMediatorTest;

// DATA TYPES
//enum ?declaration
//typedef ?declaration
//extern ?data_type;
// A typedef for function that does the actual testing,
// function is a type 
// TInt CMediatorTest::<NameOfFunction> ( TTestResult& aResult )
typedef TInt (CMediatorTest::* TestFunction)(TTestResult&);

// CLASS DECLARATION

/**
*  An internal structure containing a test case name and
*  the pointer to function doing the test
*
*  @lib ?library
*  @since ?Series60_version
*/
class TCaseInfoInternal
    {
    public:
        const TText*    iCaseName;
        TestFunction    iMethod;
        TBool           iIsOOMTest;
        TInt            iFirstMemoryAllocation;
        TInt            iLastMemoryAllocation;
    };

// CLASS DECLARATION

/**
*  A structure containing a test case name and
*  the pointer to function doing the test
*
*  @lib ?library
*  @since ?Series60_version
*/
class TCaseInfo
    {
    public:
        TPtrC iCaseName;
        TestFunction iMethod;
        TBool           iIsOOMTest;
        TInt            iFirstMemoryAllocation;
        TInt            iLastMemoryAllocation;

    TCaseInfo( const TText* a ) : iCaseName( (TText*) a )
        {
        };

    };


// CLASS DECLARATION

/**
*  This a MediatorTest class.
*  ?other_description_lines
*
*  @lib ?library
*  @since ?Series60_version
*/
NONSHARABLE_CLASS(CMediatorTest) : public CTestModuleBase
    {
    public:  // Constructors and destructor


        /**
        * Two-phased constructor.
        */
        static CMediatorTest* NewL();

        /**
        * Destructor.
        */
        virtual ~CMediatorTest();

    public: // New functions

        /**
        * ?member_description.
        * @since ?Series60_version
        * @param ?arg1 ?description
        * @return ?description
        */
        //?type ?member_function( ?type ?arg1 );

    public: // Functions from base classes
        TInt CleanRegisterOneEvent(TUid aDomain, TUid aCategory, TInt aEventId);
        TInt CleanRegisterMultipleEvents( TUid aDomain, TUid aCategory, REventList& aEventList );
        TInt CleanRegisterOver500Events(TUid aDomain, TUid aCategory, REventList aEventList);
        TInt PreconditionRegisterOneEvent(TUid aDomain, TUid aCategory, TInt aEventId, TVersion aVersion, TCapabilitySet aCaps);
        TInt PreconditionRegisterAListOfEvents( TUid aDomain, TUid aCategory, REventList aEventList);
        TInt CleanSubscribeToEvent( TUid aDomain, TUid aCategory, TInt aEventId);
        TInt PreconditionRegisterEvents( TUid aDomain, TUid aCategory, REventList aEvents);
        TInt CleanSubscribeToListOfEvents( TUid aDomain, TUid aCategory, REventList aEvents);
        TInt CleanRegisterCommand( TUid aDomain, TUid aCategory, TInt aCommand );       
        TInt CleanRegisterCommands( TUid aDomain, TUid aCategory, RCommandList aCommands );        
        TInt PreconditionRegisterCommand( TUid aDomain, 
                                          TUid aCategory, 
                                          TInt aCommandId, 
                                          TVersion aVersion,
                                          TCapabilitySet aCaps, 
                                          TInt aTimeOut );
                                                      
        TInt PreconditionRegisterCommands( TUid aDomain,
                                           TUid aCategory, 
                                           RCommandList aCommands );                                          
        
        /**
        * From CTestModuleBase InitL is used to initialize the 
        *       MediatorTest. It is called once for every instance of 
        *       TestModuleMediatorTest after its creation.
        * @since ?Series60_version
        * @param aIniFile Initialization file for the test module (optional)
        * @param aFirstTime Flag is true when InitL is executed for first 
        *               created instance of MediatorTest.
        * @return Symbian OS error code
        */
        TInt InitL( TFileName& aIniFile, TBool aFirstTime );

        /**
        * From CTestModuleBase GetTestCasesL is used to inquiry test cases 
        *   from MediatorTest. 
        * @since ?Series60_version
        * @param aTestCaseFile Test case file (optional)
        * @param aTestCases  Array of TestCases returned to test framework
        * @return Symbian OS error code
        */
        TInt GetTestCasesL( const TFileName& aTestCaseFile, 
                            RPointerArray<TTestCaseInfo>& aTestCases );

        /**
        * From CTestModuleBase RunTestCaseL is used to run an individual 
        *   test case. 
        * @since ?Series60_version
        * @param aCaseNumber Test case number
        * @param aTestCaseFile Test case file (optional)
        * @param aResult Test case result returned to test framework (PASS/FAIL)
        * @return Symbian OS error code (test case execution error, which is 
        *           not reported in aResult parameter as test case failure).
        */   
        TInt RunTestCaseL( const TInt aCaseNumber, 
                           const TFileName& aTestCaseFile,
                           TTestResult& aResult );

        /**
        * From CTestModuleBase; OOMTestQueryL is used to specify is particular
        * test case going to be executed using OOM conditions
        * @param aTestCaseFile Test case file (optional)
        * @param aCaseNumber Test case number (optional)
        * @param aFailureType OOM failure type (optional)
        * @param aFirstMemFailure The first heap memory allocation failure value (optional)
        * @param aLastMemFailure The last heap memory allocation failure value (optional)
        * @return TBool
        */
        virtual TBool OOMTestQueryL( const TFileName& /* aTestCaseFile */, 
                                     const TInt /* aCaseNumber */, 
                                     TOOMFailureType& aFailureType, 
                                     TInt& /* aFirstMemFailure */, 
                                     TInt& /* aLastMemFailure */ );

        /**
        * From CTestModuleBase; OOMTestInitializeL may be used to initialize OOM
        * test environment
        * @param aTestCaseFile Test case file (optional)
        * @param aCaseNumber Test case number (optional)
        * @return None
        */
        virtual void OOMTestInitializeL( const TFileName& /* aTestCaseFile */, 
                                    const TInt /* aCaseNumber */ ); 

        /**
        * From CTestModuleBase; OOMHandleWarningL
        * @param aTestCaseFile Test case file (optional)
        * @param aCaseNumber Test case number (optional)
        * @param aFailNextValue FailNextValue for OOM test execution (optional)
        * @return None
        *
        * User may add implementation for OOM test warning handling. Usually no
        * implementation is required.           
        */
        virtual void OOMHandleWarningL( const TFileName& /* aTestCaseFile */,
                                        const TInt /* aCaseNumber */, 
                                        TInt& /* aFailNextValue */); 

        /**
        * From CTestModuleBase; OOMTestFinalizeL may be used to finalize OOM
        * test environment
        * @param aTestCaseFile Test case file (optional)
        * @param aCaseNumber Test case number (optional)
        * @return None
        *
        */
        virtual void OOMTestFinalizeL( const TFileName& /* aTestCaseFile */, 
                                       const TInt /* aCaseNumber */ );

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
        CMediatorTest();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        // Prohibit copy constructor if not deriving from CBase.
        // ?classname( const ?classname& );
        // Prohibit assigment operator if not deriving from CBase.
        // ?classname& operator=( const ?classname& );

        /**
        * Function returning test case name and pointer to test case function.
        * @since ?Series60_version
        * @param aCaseNumber test case number
        * @return TCaseInfo 
        */
        const TCaseInfo Case ( const TInt aCaseNumber ) const;

        /**
        * Actual Hardcoded test case functions are listed below.
        */

        /**
        * Printing test case.
        * @since ?Series60_version
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt PrintTest( TTestResult& aResult );

        /**
        * Printing loop test case.
        * @since ?Series60_version
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
		    TInt LoopTest( TTestResult& aResult );
        
        //Mediator tests ->        
        TInt RegisterOneEvent( TTestResult& aResult );
        
        TInt RegisterMultipleEventsL( TTestResult& aResult );
        
        TInt RegisterMultipleCommandsWithMultipleUIDsL( TTestResult& aResult );
        TInt RegisterMultipleEventsWithMultipleUIDsL( TTestResult& aResult );

        TInt RegisterOver500EventsL( TTestResult& aResult );
        
        TInt UnregisterNonExistingEventDom( TTestResult& aResult );
        TInt UnregisterNonExistingEventCat( TTestResult& aResult );
        TInt UnregisterNonExistingEventEve( TTestResult& aResult );
        
        TInt UnregisterExistingEvent( TTestResult& aResult );
		    
        TInt UnregisterRxistingEventDifferentSID( TTestResult& aResult );
        
        TInt UnregisterAListOfEventsL( TTestResult& aResult );    
        TInt UnregisterAListOfEventsNotRegisteredL( TTestResult& aResult );
        
        TInt SubscribeToNonExistingEventDom( TTestResult& aResult );
        
        TInt SubscribeToNonExistingEventCat( TTestResult& aResult );
        
        TInt SubscribeToNonExistingEventEve( TTestResult& aResult );
        
        TInt SubscribeToWrongEventVersionMajor( TTestResult& aResult );
        
        TInt SubscribeToEventNonExistCapabilities( TTestResult& aResult );
        
        TInt SubscribeToEventCorrectParam( TTestResult& aResult );
        
        
        TInt SubscribeToListOfEventsCorrectParamL( TTestResult& aResult );
                                                          
        TInt SubscribeToListOfEventsOneInfoIncorrectL( TTestResult& aResult );
        
        TInt RaiseNonExistingEventWrongDomainL( TTestResult& aResult );
        TInt RaiseNonExistingEventWrongCatL( TTestResult& aResult );
        TInt RaiseNonExistingEventEveL( TTestResult& aResult );
        TInt RaiseEventWithWrongEventVerMajorL( TTestResult& aResult );
        TInt RaiseEventRequiresNotExistingCapsInPubL( TTestResult& aResult );
        TInt RaiseExistingEventL( TTestResult& aResult );
        TInt SubscribeExistingEventRaiseFromPublisherSideL( TTestResult& aResult );
        TInt SubscribeExistingEventRaiseFromPublisherSideDataOver1kBL( TTestResult& aResult );
        TInt SubscribeToEventDeleteConsumerInstanceRaiseEventFromPublisherL( TTestResult& aResult );
        TInt Add2ndSubscriberToEventRaiseEventFromPublisherL( TTestResult& aResult );
        TInt UnsubscribeFrom1stSubscriberL( TTestResult& aResult );
        TInt SubscribeToEventRaise500TimesInLoopL( TTestResult& aResult );
        
        TInt RegisterOneCommandToMediator( TTestResult& aResult );                                        
        TInt RegisterMultipleCommandsToMediatorL( TTestResult& aResult ); 
        TInt RegisterMultipleCommandsToMediatorWithDifferentUIDs( TTestResult& aResult ) ;
        
        TInt RegisterOver500CommandsWithinOneCatL( TTestResult& aResult );
        TInt RegisterCommandWithDomainUID0( TTestResult& aResult );
        TInt RegisterCommandWithCategoryUID0( TTestResult& aResult );
        TInt RegisterCommandWithCommandID0( TTestResult& aResult );
        TInt RegisterCommandWithTimeoutValue0( TTestResult& aResult );
        TInt IssueNonExistingCommandWrongDomainL( TTestResult& aResult );
        TInt IssueNonExistingCommandWrongCatL( TTestResult& aResult );
        TInt IssueNonExistingCommandIdL( TTestResult& aResult );
        
        TInt IssueCommandWrongCommandVerMajorL( TTestResult& aResult );
                                                
                                                      
        TInt IssueCommandL( TTestResult& aResult );
        TInt IssueCommand2L( TTestResult& aResult );
        TInt IssueExistingCommandSameInitiatorWhileProcessingL( TTestResult& aResult );
        TInt IssueCommandWaitWithoutRespondingToCommandL( TTestResult& aResult );
        TInt IssueCommandCancelFromInitiatorBeforeTimeoutL( TTestResult& aResult );
        TInt IssueCommandRespondCorrectlyFromResponderL( TTestResult& aResult );
        TInt IssueCommandRespondIncorrectlyFromResponderL( TTestResult& aResult );
        TInt IssueCommandALotOfParameterDataL( TTestResult& aResult );
        TInt IssueCommandRespondALotOfResponseDataL( TTestResult& aResult );
        TInt GetListOfDomainsNoDomainsL( TTestResult& aResult );
        TInt GetListOfDomains1DomainL( TTestResult& aResult );
        TInt GetListOfDomainsMultipleDomainsL( TTestResult& aResult );
        TInt GetListOfDomainsOver500DomainsL( TTestResult& aResult );
        TInt GetListOfCategoriesNoCategoriesL( TTestResult& aResult );
        TInt GetListOfCategoriesIncorrectDomainUIDL( TTestResult& aResult );
        TInt GetListOfCategoriesMultipleCategoriesL( TTestResult& aResult );
        TInt GetListOfCommandsL( TTestResult& aResult );
        TInt GetListOfCommandsIncorrectDomainUIDL( TTestResult& aResult );
        TInt GetListOfCommandsIncorrectCategoryUIDL( TTestResult& aResult );
        TInt GetListOfCommands1CommandL( TTestResult& aResult );
        TInt GetListOfCommandsMultipleCommandsL( TTestResult& aResult );
        TInt GetListOfCommandsOver500CommandsL( TTestResult& aResult );
        TInt GetListOfEventsNoEventsInCategoryL( TTestResult& aResult );
        TInt GetListOfEventsWithIncorrectDomainUIDL( TTestResult& aResult );
        TInt GetListOfEventsWithIncorrectCategoryUIDL( TTestResult& aResult );
        
        TInt GetListOfEvents1EventInCategoryL( TTestResult& aResult );
        
        TInt GetListOfEventsMultipleEventsInCategoryL( TTestResult& aResult );
        TInt GetListOfEventsOver500EventsInCategoryL( TTestResult& aResult );
        TInt SubscribeNotificationReceiver( TTestResult& aResult );
        TInt ReSubscribeNotificationReceiver( TTestResult& aResult );
        TInt UnsubscribeNotificationReceiver( TTestResult& aResult );
        TInt ReUnsubscribeNotificationReceiver( TTestResult& aResult );
        TInt SubscribeNotificationReceiverRegisterCommandsL( TTestResult& aResult );
        TInt SubscribeNotificationReceiverUnregCommandsL( TTestResult& aResult );
        TInt SubscribeNotificationReceiverUnregCommandsSoThatCatIsEmptyL( TTestResult& aResult );
        TInt SubscribeNotificationReceiverRegisterEventsL( TTestResult& aResult );
        TInt SubscribeNotificationReceiverUnregisterEventsL( TTestResult& aResult );
        TInt SubscribeNotificationReceiverUnregisterEventsSoThatCatIsEmptyL( TTestResult& aResult );
        TInt SubscribeNotificationReceiverRegisterCommandsSoThatErrorInRegistrationL( TTestResult& aResult );
        TInt SubscribeNotificationReceiverUnegisterCommandsSoThatErrorInRegistrationL( TTestResult& aResult );
        TInt SubscribeNotificationReceiverRegisterEventsSoThatErrorInRegistration( TTestResult& aResult );
        TInt SubscribeNotificationReceiverUnregisterEventsSoThatErrorInRegistration( TTestResult& aResult );
        TInt GetListOfDomainsInitL( TTestResult& aResult );  
        TInt IssueCommandCancelAndReissueL( TTestResult& aResult );
        TInt IssueCommandCancelAndIssueNewCommandL( TTestResult& aResult );
        TInt InstantiateAndDeleteMediatorPluginL( TTestResult& aResult );
        TInt IssueCommandNoDataL( TTestResult& aResult );
        TInt RaiseEventNoDataL( TTestResult& aResult );
        TInt OOMForEventProviderTestL( TTestResult& aResult );
        TInt EventProviderTestL();
        TInt OOMForEventConsumerTestL(TTestResult& aResult);
        TInt EventConsumerTestL();
        TInt OOMForEventProviderwithListTestL(TTestResult& aResult);
        TInt EventProviderListEventTestL();
        TInt OOMForEventConsumerWithEventTestL(TTestResult& aResult);
        TInt EventConsumerWithEventTestL();
        TInt OOMForCommandInitiatorTestL(TTestResult& aResult);
        TInt CommandInitiatorTestL();
        TInt OOMForCommandInitiatorCommandListTestL(TTestResult& aResult);
        TInt CommandInitiatorCommandListTestL();
        TInt OOMForMediatorNotificationsTestL(TTestResult& aResult);
        TInt CMediatorNotificationsTestL();
    public:     // Data
        // ?one_line_short_description_of_data
        //?data_declaration;
        
        CTestCasePropertyWatch* iTestHelper;
        CTestCasePropertyWatch* iAnotherTestHelper;
        CMediatorEventProvider* iEventProvider;
        CMediatorEventConsumer* iEventConsumer;
        CMediatorNotifications* iNotifications;
        CMediatorCommandResponder* iCommandResponder;
        CMediatorCommandInitiator* iCommandInitiator;
        CActiveScheduler* iMyActiveScheduler;
        TInt initCountOfDomain;
        
    protected:  // Data
        // ?one_line_short_description_of_data
        //?data_declaration;

    private:    // Data
        // Pointer to test (function) to be executed
        TestFunction iMethod;

        // Pointer to logger
        CStifLogger * iLog; 

        // ?one_line_short_description_of_data
        //?data_declaration;

        // Reserved pointer for future extension
        //TAny* iReserved;
        
    public:     // Friend classes
        //?friend_class_declaration;
        TVersion iDefaultVersion;
    protected:  // Friend classes
        //?friend_class_declaration;
    private:    // Friend classes
        //?friend_class_declaration;

    };

#endif      // MEDIATORTEST_H

// End of File

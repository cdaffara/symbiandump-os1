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



// INCLUDE FILES
#include <e32math.h>
#include <mediatorpluginbase.h>
#include "MediatorTest.h"
#include "mediatortestdata.h"

using namespace MediatorService;

// EXTERNAL DATA STRUCTURES
//extern  ?external_data;

// EXTERNAL FUNCTION PROTOTYPES  
//extern ?external_function( ?arg_type,?arg_type );

// CONSTANTS
//const ?type ?constant_var = ?constant;
const TInt KTestUid = 0x101FB3E7;
const TInt KTestEventId = 1000;
const TInt KNonExistingDomainId = 999;
const TInt KReallyNonExistingDomainId = 1234567;
const TInt KNonExistingCategoryId = 99;
const TInt KNonExistingEventId = 99999;
const TInt KExistingEventId = 50;
const TInt KOtherUid = 0x00000124;           // static domain defined in StaticEvents.rss
const TInt KOtherSIDCategory = 0x00006767;   // static category defined in StaticEvents.rss
const TInt KOtherSIDEventId = 12;            // static event defined in StaticEvents.rss
const TInt KNonExistingEventCapaId = 5;
const TInt KMinListId = 1;
const TInt KMaxListId = 99;
//const TInt KNonExistingCapsEventId = 889;
const TInt KDefaultCommandId = 1000;
const TInt KDefaultTimeOut = 3000; // 3 seconds
const TInt KInvalidCommandId = 99999;
const TInt KDataSize = 2000;
const TInt KSecondaryCommandId = 700;
const TInt KMinDomainID = 200100;
const TInt KMidDomainID = 200150;
const TInt KMaxDomainID = 200660;

// MACROS
//#define ?macro ?macro_def

// LOCAL CONSTANTS AND MACROS
//const ?type ?constant_var = ?constant;
//#define ?macro_name ?macro_def

// MODULE DATA STRUCTURES
//enum ?declaration
//typedef ?declaration

// LOCAL FUNCTION PROTOTYPES
//?type ?function_name( ?arg_type, ?arg_type );

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// ?function_name ?description.
// ?description
// Returns: ?value_1: ?description
//          ?value_n: ?description_line1
//                    ?description_line2
// -----------------------------------------------------------------------------
//
/*
?type ?function_name(
    ?arg_type arg,  // ?description
    ?arg_type arg)  // ?description
    {

    ?code  // ?comment

    // ?comment
    ?code
    }
*/

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMediatorTest::Case
// Returns a test case by number.
//
// This function contains an array of all available test cases 
// i.e pair of case name and test function. If case specified by parameter
// aCaseNumber is found from array, then that item is returned.
// 
// The reason for this rather complicated function is to specify all the
// test cases only in one place. It is not necessary to understand how
// function pointers to class member functions works when adding new test
// cases. See function body for instructions how to add new test case.
// -----------------------------------------------------------------------------
//
const TCaseInfo CMediatorTest::Case ( 
    const TInt aCaseNumber ) const 
     {

    /**
    * To add new test cases, implement new test case function and add new 
    * line to KCases array specify the name of the case and the function 
    * doing the test case
    * In practice, do following
    * 1) Make copy of existing test case function and change its name
    *    and functionality. Note that the function must be added to 
    *    MediatorTest.cpp file and to MediatorTest.h 
    *    header file.
    *
    * 2) Add entry to following KCases array either by using:
    *
    * 2.1: FUNCENTRY or ENTRY macro
    * ENTRY macro takes two parameters: test case name and test case 
    * function name.
    *
    * FUNCENTRY macro takes only test case function name as a parameter and
    * uses that as a test case name and test case function name.
    *
    * Or
    *
    * 2.2: OOM_FUNCENTRY or OOM_ENTRY macro. Note that these macros are used
    * only with OOM (Out-Of-Memory) testing!
    *
    * OOM_ENTRY macro takes five parameters: test case name, test case 
    * function name, TBool which specifies is method supposed to be run using
    * OOM conditions, TInt value for first heap memory allocation failure and 
    * TInt value for last heap memory allocation failure.
    * 
    * OOM_FUNCENTRY macro takes test case function name as a parameter and uses
    * that as a test case name, TBool which specifies is method supposed to be
    * run using OOM conditions, TInt value for first heap memory allocation 
    * failure and TInt value for last heap memory allocation failure. 
    */ 
    
    static TCaseInfoInternal const KCases[] =
        {
        // To add new test cases, add new items to this array
        ENTRY( "[CMediatorTest] RegisterOneEvent", &CMediatorTest::RegisterOneEvent ),
        ENTRY( "[CMediatorTest] GetListOfDomainsInitL", &CMediatorTest::GetListOfDomainsInitL ),
        ENTRY( "[CMediatorTest] GetListOfDomains1DomainL", &CMediatorTest::GetListOfDomains1DomainL ),
        ENTRY( "[CMediatorTest] GetListOfEvents1EventInCategoryL", &CMediatorTest::GetListOfEvents1EventInCategoryL ),
        ENTRY( "[CMediatorTest] RegisterMultipleEventsL", &CMediatorTest::RegisterMultipleEventsL ),
        ENTRY( "[CMediatorTest] RegisterOver500EventsL", &CMediatorTest::RegisterOver500EventsL ),
        ENTRY( "[CMediatorTest] UnregisterNonExistingEventDom", &CMediatorTest::UnregisterNonExistingEventDom ),
        ENTRY( "[CMediatorTest] GetListOfDomainsMultipleDomainsL", &CMediatorTest::GetListOfDomainsMultipleDomainsL ),
        ENTRY( "[CMediatorTest] UnregisterNonExistingEventCat", &CMediatorTest::UnregisterNonExistingEventCat ),
        ENTRY( "[CMediatorTest] UnregisterNonExistingEventEve", &CMediatorTest::UnregisterNonExistingEventEve ),
        ENTRY( "[CMediatorTest] UnregisterExistingEvent", &CMediatorTest::UnregisterExistingEvent ),
        ENTRY( "[CMediatorTest] UnregisterRxistingEventDifferentSID", &CMediatorTest::UnregisterRxistingEventDifferentSID ),
        ENTRY( "[CMediatorTest] UnregisterAListOfEventsL", &CMediatorTest::UnregisterAListOfEventsL ),
        ENTRY( "[CMediatorTest] UnregisterAListOfEventsNotRegisteredL", &CMediatorTest::UnregisterAListOfEventsNotRegisteredL ),
        ENTRY( "[CMediatorTest] SubscribeToNonExistingEventDom", &CMediatorTest::SubscribeToNonExistingEventDom ),
        ENTRY( "[CMediatorTest] SubscribeToNonExistingEventCat", &CMediatorTest::SubscribeToNonExistingEventCat ),
        ENTRY( "[CMediatorTest] SubscribeToNonExistingEventEve", &CMediatorTest::SubscribeToNonExistingEventEve ),
        ENTRY( "[CMediatorTest] SubscribeToWrongEventVersionMajor", &CMediatorTest::SubscribeToWrongEventVersionMajor ),
        ENTRY( "[CMediatorTest] SubscribeToEventNonExistCapabilities", &CMediatorTest::SubscribeToEventNonExistCapabilities ),
        ENTRY( "[CMediatorTest] SubscribeToEventCorrectParam", &CMediatorTest::SubscribeToEventCorrectParam ),
        ENTRY( "[CMediatorTest] SubscribeToListOfEventsCorrectParamL", &CMediatorTest::SubscribeToListOfEventsCorrectParamL ),
        ENTRY( "[CMediatorTest] SubscribeToListOfEventsOneInfoIncorrectL", &CMediatorTest::SubscribeToListOfEventsOneInfoIncorrectL ),
        ENTRY( "[CMediatorTest] RaiseNonExistingEventWrongDomainL", &CMediatorTest::RaiseNonExistingEventWrongDomainL ),
        ENTRY( "[CMediatorTest] RaiseNonExistingEventWrongCatL", &CMediatorTest::RaiseNonExistingEventWrongCatL ),
        ENTRY( "[CMediatorTest] RaiseNonExistingEventEveL", &CMediatorTest::RaiseNonExistingEventEveL ),
        ENTRY( "[CMediatorTest] RaiseEventWithWrongEventVerMajorL", &CMediatorTest::RaiseEventWithWrongEventVerMajorL ),
        ENTRY( "[CMediatorTest] RaiseEventRequiresNotExistingCapsInPubL", &CMediatorTest::RaiseEventRequiresNotExistingCapsInPubL ),
        ENTRY( "[CMediatorTest] RaiseExistingEventL", &CMediatorTest::RaiseExistingEventL ),
        ENTRY( "[CMediatorTest] SubscribeExistingEventRaiseFromPublisherSideL", &CMediatorTest::SubscribeExistingEventRaiseFromPublisherSideL ),
        ENTRY( "[CMediatorTest] SubscribeExistingEventRaiseFromPublisherSideDataOver1kBL", &CMediatorTest::SubscribeExistingEventRaiseFromPublisherSideDataOver1kBL ),
        ENTRY( "[CMediatorTest] SubscribeToEventDeleteConsumerInstanceRaiseEventFromPublisherL", &CMediatorTest::SubscribeToEventDeleteConsumerInstanceRaiseEventFromPublisherL ),
        ENTRY( "[CMediatorTest] Add2ndSubscriberToEventRaiseEventFromPublisherL", &CMediatorTest::Add2ndSubscriberToEventRaiseEventFromPublisherL ),
        ENTRY( "[CMediatorTest] UnsubscribeFrom1stSubscriberL", &CMediatorTest::UnsubscribeFrom1stSubscriberL ),
        ENTRY( "[CMediatorTest] SubscribeToEventRaise500TimesInLoopL", &CMediatorTest::SubscribeToEventRaise500TimesInLoopL ),
        ENTRY( "[CMediatorTest] RegisterOneCommandToMediator", &CMediatorTest::RegisterOneCommandToMediator ),
        ENTRY( "[CMediatorTest] RegisterMultipleCommandsToMediatorL", &CMediatorTest::RegisterMultipleCommandsToMediatorL ),
        ENTRY( "[CMediatorTest] RegisterOver500CommandsWithinOneCatL", &CMediatorTest::RegisterOver500CommandsWithinOneCatL ),
        ENTRY( "[CMediatorTest] RegisterCommandWithDomainUID0", &CMediatorTest::RegisterCommandWithDomainUID0 ),
        ENTRY( "[CMediatorTest] RegisterCommandWithCategoryUID0", &CMediatorTest::RegisterCommandWithCategoryUID0 ),
        ENTRY( "[CMediatorTest] RegisterCommandWithCommandID0", &CMediatorTest::RegisterCommandWithCommandID0 ),
        ENTRY( "[CMediatorTest] RegisterCommandWithTimeoutValue0", &CMediatorTest::RegisterCommandWithTimeoutValue0 ),
        ENTRY( "[CMediatorTest] IssueNonExistingCommandWrongDomainL", &CMediatorTest::IssueNonExistingCommandWrongDomainL ),
        ENTRY( "[CMediatorTest] IssueNonExistingCommandWrongCatL", &CMediatorTest::IssueNonExistingCommandWrongCatL ),
        ENTRY( "[CMediatorTest] IssueNonExistingCommandIdL", &CMediatorTest::IssueNonExistingCommandIdL ),
        ENTRY( "[CMediatorTest] IssueCommandWrongCommandVerMajorL", &CMediatorTest::IssueCommandWrongCommandVerMajorL ),
        ENTRY( "[CMediatorTest] IssueCommandL", &CMediatorTest::IssueCommandL ),
        ENTRY( "[CMediatorTest] IssueCommand2L", &CMediatorTest::IssueCommand2L ),
        ENTRY( "[CMediatorTest] IssueExistingCommandSameInitiatorWhileProcessingL", &CMediatorTest::IssueExistingCommandSameInitiatorWhileProcessingL ),
        ENTRY( "[CMediatorTest] IssueCommandWaitWithoutRespondingToCommandL", &CMediatorTest::IssueCommandWaitWithoutRespondingToCommandL ),
        ENTRY( "[CMediatorTest] IssueCommandCancelFromInitiatorBeforeTimeoutL", &CMediatorTest::IssueCommandCancelFromInitiatorBeforeTimeoutL ),
        ENTRY( "[CMediatorTest] IssueCommandRespondCorrectlyFromResponderL", &CMediatorTest::IssueCommandRespondCorrectlyFromResponderL ),
        ENTRY( "[CMediatorTest] IssueCommandALotOfParameterDataL", &CMediatorTest::IssueCommandALotOfParameterDataL ),
        ENTRY( "[CMediatorTest] IssueCommandRespondALotOfResponseDataL", &CMediatorTest::IssueCommandRespondALotOfResponseDataL ),
        //ENTRY( "[CMediatorTest] GetListOfDomainsOver500Domains", &CMediatorTest::GetListOfDomainsOver500Domains ),
        ENTRY( "[CMediatorTest] GetListOfCategoriesNoCategoriesL", &CMediatorTest::GetListOfCategoriesNoCategoriesL ),
        ENTRY( "[CMediatorTest] GetListOfCategoriesIncorrectDomainUIDL", &CMediatorTest::GetListOfCategoriesIncorrectDomainUIDL ),
        ENTRY( "[CMediatorTest] GetListOfCategoriesMultipleCategoriesL", &CMediatorTest::GetListOfCategoriesMultipleCategoriesL ),
        ENTRY( "[CMediatorTest] GetListOfCommandsL", &CMediatorTest::GetListOfCommandsL ),
        ENTRY( "[CMediatorTest] GetListOfCommandsIncorrectDomainUIDL", &CMediatorTest::GetListOfCommandsIncorrectDomainUIDL ),
        ENTRY( "[CMediatorTest] GetListOfCommandsIncorrectCategoryUIDL", &CMediatorTest::GetListOfCommandsIncorrectCategoryUIDL ),
        ENTRY( "[CMediatorTest] GetListOfCommands1CommandL", &CMediatorTest::GetListOfCommands1CommandL ),
        ENTRY( "[CMediatorTest] GetListOfCommandsMultipleCommandsL", &CMediatorTest::GetListOfCommandsMultipleCommandsL ),
        ENTRY( "[CMediatorTest] GetListOfCommandsOver500CommandsL", &CMediatorTest::GetListOfCommandsOver500CommandsL ),
        ENTRY( "[CMediatorTest] GetListOfEventsNoEventsInCategoryL", &CMediatorTest::GetListOfEventsNoEventsInCategoryL ),
        ENTRY( "[CMediatorTest] GetListOfEventsWithIncorrectDomainUIDL", &CMediatorTest::GetListOfEventsWithIncorrectDomainUIDL ),
        ENTRY( "[CMediatorTest] GetListOfEventsWithIncorrectCategoryUIDL", &CMediatorTest::GetListOfEventsWithIncorrectCategoryUIDL ),
        ENTRY( "[CMediatorTest] GetListOfEventsMultipleEventsInCategoryL", &CMediatorTest::GetListOfEventsMultipleEventsInCategoryL ),
        ENTRY( "[CMediatorTest] GetListOfEventsOver500EventsInCategoryL", &CMediatorTest::GetListOfEventsOver500EventsInCategoryL ),
        ENTRY( "[CMediatorTest] SubscribeNotificationReceiver", &CMediatorTest::SubscribeNotificationReceiver ),
        ENTRY( "[CMediatorTest] ReSubscribeNotificationReceiver", &CMediatorTest::ReSubscribeNotificationReceiver ),
        ENTRY( "[CMediatorTest] ReUnsubscribeNotificationReceiver", &CMediatorTest::ReUnsubscribeNotificationReceiver ),
        ENTRY( "[CMediatorTest] SubscribeNotificationReceiverRegisterCommandsL", &CMediatorTest::SubscribeNotificationReceiverRegisterCommandsL ),
        ENTRY( "[CMediatorTest] SubscribeNotificationReceiverUnregCommandsSoThatCatIsEmptyL", &CMediatorTest::SubscribeNotificationReceiverUnregCommandsSoThatCatIsEmptyL ),
        ENTRY( "[CMediatorTest] SubscribeNotificationReceiverRegisterEventsL", &CMediatorTest::SubscribeNotificationReceiverRegisterEventsL ),
        ENTRY( "[CMediatorTest] SubscribeNotificationReceiverUnregisterEventsL", &CMediatorTest::SubscribeNotificationReceiverUnregisterEventsL ),
        ENTRY( "[CMediatorTest] SubscribeNotificationReceiverUnregisterEventsSoThatCatIsEmptyL", &CMediatorTest::SubscribeNotificationReceiverUnregisterEventsSoThatCatIsEmptyL ),
        ENTRY( "[CMediatorTest] SubscribeNotificationReceiverRegisterCommandsSoThatErrorInRegistrationL", &CMediatorTest::SubscribeNotificationReceiverRegisterCommandsSoThatErrorInRegistrationL ),
        ENTRY( "[CMediatorTest] SubscribeNotificationReceiverUnegisterCommandsSoThatErrorInRegistrationL", &CMediatorTest::SubscribeNotificationReceiverUnegisterCommandsSoThatErrorInRegistrationL ),
        ENTRY( "[CMediatorTest] SubscribeNotificationReceiverRegisterEventsSoThatErrorInRegistration", &CMediatorTest::SubscribeNotificationReceiverRegisterEventsSoThatErrorInRegistration ),
        ENTRY( "[CMediatorTest] SubscribeNotificationReceiverUnregisterEventsSoThatErrorInRegistration", &CMediatorTest::SubscribeNotificationReceiverUnregisterEventsSoThatErrorInRegistration ),
        ENTRY( "[CMediatorTest] IssueCommandCancelAndReissueL", &CMediatorTest::IssueCommandCancelAndReissueL ),
        ENTRY( "[CMediatorTest] IssueCommandCancelAndIssueNewCommandL", &CMediatorTest::IssueCommandCancelAndIssueNewCommandL ),
        ENTRY( "[CMediatorTest] InstantiateAndDeleteMediatorPluginL", &CMediatorTest::InstantiateAndDeleteMediatorPluginL ),
        ENTRY( "[CMediatorTest] IssueCommandNoDataL", &CMediatorTest::IssueCommandNoDataL ),
        ENTRY( "[CMediatorTest] RaiseEventNoDataL", &CMediatorTest::RaiseEventNoDataL ),
        ENTRY( "[CMediatorTest] RegisterMultipleEventsWithMultipleUIDsL", &CMediatorTest::RegisterMultipleEventsWithMultipleUIDsL ),
        ENTRY( "[CMediatorTest] RegisterMultipleCommandsWithMultipleUIDsL", &CMediatorTest::RegisterMultipleCommandsWithMultipleUIDsL ),
        ENTRY( "[CMediatorTest] IssueCommandRespondIncorrectlyFromResponderL", &CMediatorTest::IssueCommandRespondIncorrectlyFromResponderL ),
        ENTRY( "[CMediatorTest] OOMForEventProviderTestL", &CMediatorTest::OOMForEventProviderTestL ),
        ENTRY( "[CMediatorTest] OOMForEventConsumerTestL", &CMediatorTest::OOMForEventConsumerTestL ),
        ENTRY( "[CMediatorTest] OOMForEventProviderwithListTestL", &CMediatorTest::OOMForEventProviderwithListTestL ),
        ENTRY( "[CMediatorTest] OOMForEventConsumerWithEventTestL", &CMediatorTest::OOMForEventConsumerWithEventTestL ),
        ENTRY( "[CMediatorTest] OOMForCommandInitiatorTestL", &CMediatorTest::OOMForCommandInitiatorTestL ),
        ENTRY( "[CMediatorTest] OOMForCommandInitiatorCommandListTestL", &CMediatorTest::OOMForCommandInitiatorCommandListTestL ),
        ENTRY( "[CMediatorTest] OOMForMediatorNotificationsTestL", &CMediatorTest::OOMForMediatorNotificationsTestL )
        // Example how to use OOM functionality
        //OOM_ENTRY( "Loop test with OOM", LoopTest, ETrue, 2, 3 ),
        //OOM_FUNCENTRY( PrintTest, ETrue, 1, 3 ),
        };

    // Verify that case number is valid
    if( (TUint) aCaseNumber >= sizeof( KCases ) / 
                               sizeof( TCaseInfoInternal ) )
        {
        // Invalid case, construct empty object
        TCaseInfo null( (const TText*) L"" );
        null.iMethod = NULL;
        null.iIsOOMTest = EFalse;
        null.iFirstMemoryAllocation = 0;
        null.iLastMemoryAllocation = 0;
        return null;
        } 

    // Construct TCaseInfo object and return it
    TCaseInfo tmp ( KCases[ aCaseNumber ].iCaseName );
    tmp.iMethod = KCases[ aCaseNumber ].iMethod;
    tmp.iIsOOMTest = KCases[ aCaseNumber ].iIsOOMTest;
    tmp.iFirstMemoryAllocation = KCases[ aCaseNumber ].iFirstMemoryAllocation;
    tmp.iLastMemoryAllocation = KCases[ aCaseNumber ].iLastMemoryAllocation;
    return tmp;
    }

// -----------------------------------------------------------------------------
// CMediatorTest::PrintTest
// Simple printing to UI test.
// -----------------------------------------------------------------------------
//
TInt CMediatorTest::PrintTest( 
    TTestResult& aResult )
    {
     /* Simple print test */
    _LIT( KPrintTest, "PrintTest" );
    _LIT( KEnter, "Enter" );
    _LIT( KOnGoing, "On-going" );
    _LIT( KExit, "Exit" );

    TestModuleIf().Printf( 0, KPrintTest, KEnter );
           
    TestModuleIf().Printf( 1, KPrintTest, KOnGoing );
    
    TestModuleIf().Printf( 0, KPrintTest, KExit );

    // Test case passed

    // Sets test case result and description(Maximum size is KStifMaxResultDes)
    _LIT( KDescription, "PrintTest passed" );
    aResult.SetResult( KErrNone, KDescription );

    // Case was executed
    return KErrNone;

    }

// -----------------------------------------------------------------------------
// CMediatorTest::LoopTest
// Another printing to UI test.
// -----------------------------------------------------------------------------
//
TInt CMediatorTest::LoopTest( TTestResult& aResult )
    {

    /* Simple print and wait loop */
    _LIT( KState, "State" );
    _LIT( KLooping, "Looping" );

    TestModuleIf().Printf( 0, KState, KLooping );

    _LIT( KRunning, "Running" );
    _LIT( KLoop, "%d" );
    for( TInt i=0; i<10; i++)
        {
        TestModuleIf().Printf( 1, KRunning, KLoop, i);
        User::After( 1000000 );
        }

    _LIT( KFinished, "Finished" );
    TestModuleIf().Printf( 0, KState, KFinished );

    // Test case passed

    // Sets test case result and description(Maximum size is KStifMaxResultDes)
    _LIT( KDescription, "LoopTest passed" );
    aResult.SetResult( KErrNone, KDescription );

    // Case was executed
    return KErrNone;

    }

    
////////////////////////////////////////////////////////////////////////////////
//
// M E D I A T O R      T E S T S ->
//
//
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// CMediatorTest::RegisterOneEvent
//
// Register one event in Mediator
// Expected precondition: No event created previously
// Expected behaviour: Registration succeed
// Cleaning: Unregister event
//
// Domain: KTestUid
// Category: KTestUid
// Event ID: KTestEventId
// 
// -----------------------------------------------------------------------------
//

TInt CMediatorTest::RegisterOneEvent( TTestResult& aResult )
    {    
    TInt status( KErrNone );
    TUid myUid = TUid::Uid( KTestUid );
    TCapabilitySet caps;
    caps.SetEmpty();
    
    status = iEventProvider->RegisterEvent( myUid, 
                                            myUid,
                                            KTestEventId,
                                            iDefaultVersion,
                                            caps ) ;        

    _LIT( KDescription, "RegisterEvent returns with" );
    aResult.SetResult( status, KDescription ); // set status as test result
    
    if( KErrNone == status  )
        {
        /* Registration succeeded -> Clean Event */
        TInt cleanStatus = CleanRegisterOneEvent( myUid, myUid, KTestEventId ); 
        if( KErrNone != cleanStatus )
            {
            _LIT( KDescription, "Test passed but cleaning failed" );
            aResult.SetResult( cleanStatus, KDescription );     
            }
        }
    return KErrNone;
    }


TInt CMediatorTest::CleanRegisterOneEvent( TUid aDomain, TUid aCategory, TInt aEventId )
    {
    return iEventProvider->UnregisterEvent( aDomain, 
                                            aCategory,
                                            aEventId );  
    }


// -----------------------------------------------------------------------------
// CMediatorTest::RegisterMultipleEvents
//
// Register 50 events in Mediator
// Expected precondition: No events created previously
// Expected behaviour: Registration succeed
// Cleaning: Unregister events
//
// Domain: KTestUid
// Category: KTestUid
// Event IDs: 50 - 99
// -----------------------------------------------------------------------------
//
TInt CMediatorTest::RegisterMultipleEventsL( TTestResult& aResult )
    {
    TInt status( KErrNone );
    TUid myUid = TUid::Uid( KTestUid );
    TCapabilitySet caps;
    caps.SetEmpty();    
    REventList eventList;
    CleanupClosePushL( eventList );
    
    for( TInt i=50; i<100; i++ ) // i = Event ID
        {
        TEvent newEvent;
        newEvent.iEventId = i;
        newEvent.iVersion = iDefaultVersion;
        newEvent.iCaps = caps;  
        eventList.AppendL( newEvent );
        }
    
    status = iEventProvider->RegisterEvent( myUid, 
                                            myUid,
                                            eventList);


    _LIT( KDescription, "RegisterEvent returns with" );
    aResult.SetResult( status, KDescription );
 
    if( KErrNone == status  )
        {
        /* Registration succeeded -> Clean Event */
        TInt cleanStatus = CleanRegisterMultipleEvents( myUid, myUid, eventList ); 
        if( KErrNone != cleanStatus )
            {
            _LIT( KDescription, "Test passed but cleaning failed" );
            aResult.SetResult( cleanStatus, KDescription );     
            }
        }
    CleanupStack::PopAndDestroy( &eventList );
    return KErrNone;
    }

TInt CMediatorTest::CleanRegisterMultipleEvents( TUid aDomain, TUid aCategory, REventList& aEventList )
    {
    return iEventProvider->UnregisterEvent( aDomain, 
                                            aCategory,
                                            aEventList );
    }

// -----------------------------------------------------------------------------
// CMediatorTest::RegisterOver500Events
//
// Register over 500 events
// Expected precondition: No events created previously
// Expected behaviour: Registration succeed
// Cleaning: Unregister events
//
// Domain: KTestUid
// Category: KTestUid
// Event IDs: 100 - 699
// -----------------------------------------------------------------------------
//       
TInt CMediatorTest::RegisterOver500EventsL( TTestResult& aResult )
    {
    TInt status( KErrNone );
    TUid myUid = TUid::Uid( KTestUid );
    TCapabilitySet caps;
    caps.SetEmpty();
    REventList eventList;
    CleanupClosePushL( eventList );
    
    for( TInt i=100; i<700; i++ ) // i = event ID
        {
        TEvent newEvent;
        newEvent.iEventId = i;
        newEvent.iVersion = iDefaultVersion;
        newEvent.iCaps = caps;    
        eventList.AppendL( newEvent );
        }
    
    status = iEventProvider->RegisterEvent( myUid, 
                                            myUid,
                                            eventList);

    _LIT( KDescription, "RegisterEvent returns with" );
    aResult.SetResult( status, KDescription );  // status is returned as test result

    if( KErrNone == status  )
        {
        /* Registration succeeded -> Clean Events */
        TInt cleanStatus = CleanRegisterOver500Events(myUid, myUid, eventList);  
        if( KErrNone != cleanStatus)
            {
            _LIT( KDescription, "Test passed but cleaning failed" );
            aResult.SetResult( cleanStatus, KDescription );     
            }
        }
    CleanupStack::PopAndDestroy( &eventList );
    return KErrNone;
    }

TInt CMediatorTest::CleanRegisterOver500Events( TUid aDomain, TUid aCategory, REventList aEventList )
    {
    TInt status = iEventProvider->UnregisterEvent( aDomain, 
                                                   aCategory,
                                                   aEventList );    
    CTestScheduler::StartAndStopL( 1000 ); // wait 1s 
    
    return status;
    }

 // -----------------------------------------------------------------------------
// CMediatorTest::UnregisterNonExistingEventDom
//
// Expected preconditions: Requested event domain is not existing
// Expected behaviour: Unregistration fail (KErrMediatorDomainNotFound)
// Cleaning: None
//
// Try to unregister event from domain that does not exist
// Domain: KNonExistingDomainId
// Category: KTestUid
// Event: KTestEventId
// -----------------------------------------------------------------------------
//      
TInt CMediatorTest::UnregisterNonExistingEventDom( TTestResult& aResult )
    {   
    TInt status( KErrNone );
    TUid myUid = TUid::Uid( KTestUid );
    TUid invalidUid = TUid::Uid( KNonExistingDomainId );
    
    status = iEventProvider->UnregisterEvent( invalidUid, 
                                              myUid,
                                              KTestEventId ) ;       
    //expected status is KErrMediatorDomainNotFound 
    if( KErrMediatorDomainNotFound == status )
        {
        _LIT( KDescription, "UnregisterEvent returned with expected KErrMediatorDomainNotFound" );
        aResult.SetResult( KErrNone, KDescription );      
        }
    else
        {
        _LIT( KErrorDescription, "Wrong error status" );
        aResult.SetResult( KErrGeneral, KErrorDescription );
        }
    return KErrNone;
    }   

// -----------------------------------------------------------------------------
// CMediatorTest::UnregisterNonExistingEventCat
//
// Expected preconditions: Requested event category is not existing
// Expected behaviour: Unregistration fail (KErrMediatorCategoryNotFound)
// Cleaning: None
//
// Try to unregister event from category that does not exist 
// Domain: KTestUid
// Category: KNonExistingCategoryId
// Event ID: KTestEventId
// -----------------------------------------------------------------------------
//      
TInt CMediatorTest::UnregisterNonExistingEventCat( TTestResult& aResult )
    {
    TInt status( KErrNone );
    TUid myUid = TUid::Uid( KTestUid );
    TUid invalidUid = TUid::Uid( KNonExistingCategoryId );
    status = iEventProvider->UnregisterEvent( myUid, 
                                              invalidUid,
                                              KTestEventId) ;
    //expected status is KErrMediatorCategoryNotFound                                        
    if( KErrMediatorCategoryNotFound == status )
        {
        _LIT( KDescription, "UnregisterEvent returned with expected KErrMediatorCategoryNotFound" );
        aResult.SetResult( KErrNone, KDescription );        
        }
    else
        {
        _LIT( KErrorDescription, "Wrong error status" );
        aResult.SetResult( KErrGeneral, KErrorDescription );
        }
       
    return KErrNone;
    }



// -----------------------------------------------------------------------------
// CMediatorTest::UnregisterNonExistingEventEve
//
// Expected preconditions: Event is not existing but domain and category is
// Expected behaviour: Unregistration fail (KErrMediatorEventNotFound)
// Cleaning: Unregister existing events
//
// Try to unregister nonexisting event from valid Domain and Category
// Domain: KTestUid
// Category: KTestUid
// Event ID: KNonExistingEventId
// -----------------------------------------------------------------------------
//

TInt CMediatorTest::UnregisterNonExistingEventEve( TTestResult& aResult )
    {
    TInt status( KErrNone );
    TUid myUid = TUid::Uid( KTestUid );
    TCapabilitySet caps;
    caps.SetEmpty();
    
    TInt preStatus = PreconditionRegisterOneEvent( myUid, myUid, KExistingEventId, iDefaultVersion, caps );
    
    _LIT( KDescription, "PreconditionRegisterOneEvent returns with" );
    aResult.SetResult( preStatus, KDescription );    
    
    if( KErrNone == preStatus )
        {    
        //unregister non existing event
        status = iEventProvider->UnregisterEvent( myUid, 
                                                  myUid,
                                                  KNonExistingEventId);          
        //expected status is KErrMediatorEventNotFound
        if( KErrMediatorEventNotFound == status )
            {
            _LIT( KDescription, "UnregisterEvent returned with expected KErrMediatorEventNotFound" );
            aResult.SetResult( KErrNone, KDescription );
            
            //clean existing event
            status = CleanRegisterOneEvent( myUid, myUid, KExistingEventId );
            if( KErrNone != status )
                {
                _LIT( KErrorDescription, "Test code passed but cleaning event failed" );
                aResult.SetResult( status, KErrorDescription );
                } 
            }
        else
            {
            _LIT( KErrorDescription, "Wrong error status" );
            aResult.SetResult( KErrGeneral, KErrorDescription );
            }
        }   
    return KErrNone;
    }


// -----------------------------------------------------------------------------
// CMediatorTest::UnregisterExistingEvent
// 
// Expected preconditions: Event is existing
// Expected behaviour: Unregistration succeed (KErrNone)
// Cleaning: Event is unregistered
//
// Unregister event
// Domain: KTestUid
// Category: KTestUid
// Event: KExistingEventId
// -----------------------------------------------------------------------------
//

TInt CMediatorTest::UnregisterExistingEvent( TTestResult& aResult )
    {
    TInt status( KErrNone );
    TUid myUid = TUid::Uid( KTestUid );
    TCapabilitySet caps;
    caps.SetEmpty();
    
    TInt preStatus = PreconditionRegisterOneEvent( myUid, myUid, KExistingEventId, iDefaultVersion, caps );
    
    _LIT( KDescription, "PreconditionRegisterOneEvent returns with" );
    aResult.SetResult( preStatus, KDescription );    
    
    if( KErrNone == preStatus )
        {
        status = iEventProvider->UnregisterEvent( myUid, 
                                                  myUid,
                                                  KExistingEventId );
        _LIT( KDescription, "UnregisterEvent returns with" );
        aResult.SetResult( status, KDescription );
        }
    return KErrNone;
    }

TInt CMediatorTest::PreconditionRegisterOneEvent( TUid aDomain,
                                                  TUid aCategory, 
                                                  TInt aEventId, 
                                                  TVersion aVersion,
                                                  TCapabilitySet aCaps )
    {    
    TInt status( KErrNone );      
    status = iEventProvider->RegisterEvent( aDomain,
                                            aCategory, 
                                            aEventId, 
                                            aVersion,
                                            aCaps ) ;
    CTestScheduler::StartAndStopL(1000); // wait 1s
    return status;
    }



// -----------------------------------------------------------------------------
// CMediatorTest::UnregisterRxistingEventDifferentSID
//
// Expected preconditions: Event is existing
// Expected behaviour: Unregistration fail (KErrMediatorSecureIdMismatch)
// Cleaning: Not needed
//
// Try to unregister event that is registered by different SID (static event)
// Domain: KOtherUid
// Category: KOtherSIDCategory
// Event: KOtherSIDEventId
// -----------------------------------------------------------------------------
//

TInt CMediatorTest::UnregisterRxistingEventDifferentSID( TTestResult& aResult )
    {
    TInt status( KErrNone );
    TUid myUid = TUid::Uid( KOtherUid );
    TUid myCat = TUid::Uid( KOtherSIDCategory );
    
    // Precondition!!! Event is static, defined in resource file
        
    status = iEventProvider->UnregisterEvent( myUid, 
                                              myCat,
                                              KOtherSIDEventId) ;       
    //expected status is KErrMediatorSecureIdMismatch
    if( KErrMediatorSecureIdMismatch == status ) // Event is registered by different SID
        {
        _LIT( KDescription, "UnregisterEvent returned with expected KErrMediatorSecureIdMismatch" );
        aResult.SetResult( KErrNone, KDescription );
        }
    else
        {
        _LIT( KErrorDescription, "Wrong error status" );
        aResult.SetResult( KErrGeneral, KErrorDescription );
        }
       
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CMediatorTest::UnregisterAListOfEvents
//
// Expected preconditions: Event is existing
// Expected behaviour: Unregistration succeed (KErrNone)
// Cleaning: Not needed
//
// Unregister list of events
// Domain: KTestUid
// Category: KTestUid
// Event ID(s): 51 - 75 
// -----------------------------------------------------------------------------
//

TInt CMediatorTest::UnregisterAListOfEventsL( TTestResult& aResult )
    {
    TInt status( KErrNone );
    TUid myUid = TUid::Uid( KTestUid );
    TCapabilitySet caps;
    caps.SetEmpty();    
    REventList eventList;
    CleanupClosePushL( eventList );              
    for( TInt i=51; i<76; i++ ) // Create list of events (i = event ID)
        {
        TEvent newEvent;
        newEvent.iEventId = i;
        newEvent.iVersion = iDefaultVersion;
        newEvent.iCaps = caps;    
        eventList.AppendL( newEvent );
        }
    
    TInt preStatus = PreconditionRegisterAListOfEvents( myUid, myUid, eventList );
    _LIT( KDescription, "PreconditionRegisterAListOfEvents returns with" );
    aResult.SetResult( preStatus, KDescription );
          
    if( KErrNone == preStatus )
        {
        status = iEventProvider->UnregisterEvent( myUid, 
                                                  myUid,
                                                  eventList );
      
        _LIT( KDescription, "UnregisterEvent returns with" );
        aResult.SetResult( status, KDescription );
        }
    
    CleanupStack::PopAndDestroy( &eventList );
    return KErrNone;
    }

TInt CMediatorTest::PreconditionRegisterAListOfEvents( TUid aDomain,
                                                       TUid aCategory, 
                                                       REventList aEventList )
    {    
    return iEventProvider->RegisterEvent( aDomain,
                                            aCategory, 
                                            aEventList );       
    }

// -----------------------------------------------------------------------------
// CMediatorTest::UnregisterAListOfEventsNotRegistered
//
// Expected preconditions: Event does not exist
// Expected behaviour: Unregistration fail (KErrMediatorEventNotFound)
// Cleaning: Not needed
//
// Unregister list of event (some of events are not registered)
// Domain: KTestUid
// Category: KTestUid
// Event ID(s): 73 - 103 
// -----------------------------------------------------------------------------
//

TInt CMediatorTest::UnregisterAListOfEventsNotRegisteredL( TTestResult& aResult )
    {
    TInt status( KErrNone );
    TUid myUid = TUid::Uid( KTestUid );
    TCapabilitySet caps;
    caps.SetEmpty();

    TInt preStatus = PreconditionRegisterOneEvent( myUid, myUid, KExistingEventId, iDefaultVersion, caps );
    
    _LIT( KDescription, "PreconditionRegisterOneEvent returns with" );
    aResult.SetResult( preStatus, KDescription );    
    
    if( KErrNone == preStatus )
        {    
        REventList eventList;
        CleanupClosePushL( eventList );
                        
        for(TInt i=51; i<100; i++) 
            {
            TEvent newEvent;
            newEvent.iEventId = i;
            newEvent.iVersion = iDefaultVersion;
            newEvent.iCaps = caps;    
            eventList.AppendL( newEvent );
            }
        
        //unregister a list of events that is not registered 
        status = iEventProvider->UnregisterEvent( myUid, 
                                                myUid,
                                                eventList);
        //expected status is KErrMediatorEventNotFound
        if( KErrMediatorEventNotFound == status )
            {
            _LIT( KDescription, "UnregisterEvent returned with expected KErrMediatorEventNotFound" );
            aResult.SetResult( KErrNone, KDescription );
            //clean the registered event
            status = CleanRegisterOneEvent( myUid, myUid, KExistingEventId );
            if( KErrNone != status )
                {
                _LIT( KErrorDescription, "Test code passed but cleaning event failed" );
                aResult.SetResult( status, KErrorDescription );
                }
            }
        else
            {
            _LIT( KErrorDescription, "Wrong error status" );
            aResult.SetResult( KErrGeneral, KErrorDescription );
            }
        CleanupStack::PopAndDestroy( &eventList );
        } 
    return KErrNone;
    }


// -----------------------------------------------------------------------------
// CMediatorTest::SubscribeToNonExistingEventDom
//
// Expected preconditions: Event does not exist
// Expected behaviour: Subscribe fail (KErrMediatorDomainNotFound)
// Cleaning: Not needed
//
// Try to subscribe to event (domain does not exist)
// Domain: KNonExistingDomainId
// Category: KTestUid
// Event: KTestEventId
// -----------------------------------------------------------------------------
//

TInt CMediatorTest::SubscribeToNonExistingEventDom( TTestResult& aResult )
    {
    TInt status( KErrNone );
    TUid myUid = TUid::Uid( KTestUid );
    TUid invalidUid = TUid::Uid( KNonExistingDomainId );
    
    status = iEventConsumer->SubscribeEvent( invalidUid, 
                                             myUid,
                                             KTestEventId,
                                             iDefaultVersion) ;      
    //expected status is KErrMediatorDomainNotFound
    if( KErrMediatorDomainNotFound == status )
        {
        _LIT( KDescription, "SubscribeEvent returned with expected KErrMediatorDomainNotFound" );
        aResult.SetResult( KErrNone, KDescription );        
        }
    else
        {
        _LIT( KErrorDescription, "Wrong error status" );
        aResult.SetResult( KErrGeneral, KErrorDescription );
        }
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CMediatorTest::SubscribeToNonExistingEventCat
//
// Expected preconditions: Event does not exist
// Expected behaviour: Subscribe fail (KErrMediatorCategoryNotFound)
// Cleaning: Not needed
//
// Try to subscribe event (category does not exist)
// Domain: KTestUid
// Category: KNonExistingCategoryId
// Event: KTestEventId
// -----------------------------------------------------------------------------
//

TInt CMediatorTest::SubscribeToNonExistingEventCat( TTestResult& aResult )
    {
    TInt status( KErrNone );
    TUid myUid = TUid::Uid( KTestUid );
    TUid invalidUid = TUid::Uid( KNonExistingCategoryId );

    status = iEventConsumer->SubscribeEvent( myUid, 
                                             invalidUid,
                                             KTestEventId,
                                             iDefaultVersion ) ;  
    //expected status is KErrMediatorCategoryNotFound                                        
    if( KErrMediatorCategoryNotFound == status )
        {
        _LIT( KDescription, "SubscribeEvent returned with expected KErrMediatorCategoryNotFound" );
        aResult.SetResult( KErrNone, KDescription );        
        }
    else
        {
        _LIT( KErrorDescription, "Wrong error status" );
        aResult.SetResult( KErrGeneral, KErrorDescription );
        }
    return KErrNone;
    }


// -----------------------------------------------------------------------------
// CMediatorTest::SubscribeToNonExistingEventEve
//
// Expected preconditions: Event does not exist
// Expected behaviour: Subscribe fail (KErrMediatorEventNotFound)
// Cleaning: Not needed
//
// Try to subscribe event (event does not exist)
// Domain: KTestUid
// Category: KTestUid
// Event: KNonExistingEventId
// -----------------------------------------------------------------------------
//

TInt CMediatorTest::SubscribeToNonExistingEventEve( TTestResult& aResult )
    {
    TInt status( KErrNone );
    TUid myUid = TUid::Uid( KTestUid );
    TCapabilitySet caps;
    caps.SetEmpty();

    TInt preStatus = PreconditionRegisterOneEvent( myUid, myUid, KExistingEventId, iDefaultVersion, caps );
    
    _LIT( KDescription, "PreconditionRegisterOneEvent returns with" );
    aResult.SetResult( preStatus, KDescription );    
    
    if( KErrNone == preStatus )
        {    
        status = iEventConsumer->SubscribeEvent( myUid, 
                                                 myUid,
                                                 KNonExistingEventId,
                                                 iDefaultVersion );    
        //expected status is KErrMediatorEventNotFound                                        
        if( KErrMediatorEventNotFound == status )
            {
            _LIT( KDescription, "SubscribeEvent returned with expected KErrMediatorEventNotFound" );
            aResult.SetResult( KErrNone, KDescription ); 
            //unregister the registered event
            status = CleanRegisterOneEvent( myUid, myUid, KExistingEventId );
            if(KErrNone != status)
                {
                _LIT( KErrorDescription, "Test code passed but cleaning event failed" );
                aResult.SetResult( status, KErrorDescription );
                }
            }
        else
            {
            _LIT( KErrorDescription, "Wrong error status" );
            aResult.SetResult( KErrGeneral, KErrorDescription );
            }
        }   
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CMediatorTest::SubscribeToWrongEventVersionMajor
//
// Expected preconditions: Event does not exist
// Expected behaviour: Subscribe fail (KErrMediatorVersionMismatch)
// Cleaning: Not needed
//
// Try to subscribe event (event exist but version is wrong) 
// Domain: KTestUid
// Category: KTestUid
// Event: KTestEventId
// Version: 6.5.5
// -----------------------------------------------------------------------------
//

TInt CMediatorTest::SubscribeToWrongEventVersionMajor( TTestResult& aResult )
    {
    TInt status( KErrNone );
    TUid myUid = TUid::Uid( KTestUid );
    TVersion version( 777, 5, 5 );
    TCapabilitySet caps;
    caps.SetEmpty();
    
    TInt preStatus = PreconditionRegisterOneEvent( myUid, myUid, KTestEventId, iDefaultVersion, caps );
    
    _LIT( KDescription, "PreconditionRegisterOneEvent returns with" );
    aResult.SetResult( preStatus, KDescription );    
    
    if( KErrNone == preStatus )
        {    
        status = iEventConsumer->SubscribeEvent( myUid, 
                                                 myUid,
                                                 KTestEventId,
                                                 version );   
        //expected status is KErrMediatorVersionMismatch                                      
        if( KErrMediatorVersionMismatch == status )
            {
            _LIT( KDescription, "SubscribeEvent returned with expected KErrMediatorVersionMismatch" );
            aResult.SetResult( KErrNone, KDescription );
            status = CleanRegisterOneEvent(myUid, myUid, KTestEventId);
            if( KErrNone != status )
                {
                _LIT( KErrorDescription, "Test code passed but cleaning event failed" );
                aResult.SetResult( status, KErrorDescription );
                } 
            }
        else
            {
            _LIT( KErrorDescription, "Wrong error code" );
            aResult.SetResult( KErrGeneral, KErrorDescription );
            }
        }   
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CMediatorTest::SubscribeToEventNonExistCapabilities
//
// Expected preconditions: Event is existing
// Expected behaviour: Subscribe fail (KErrPermissionDenied)
// Cleaning: Not needed
//
// Try to subscribe event that require too much capabilities 
// Domain: KOtherUid
// Category: KOtherSIDCategory
// Event: KNonExistingEventCapaId
// -----------------------------------------------------------------------------
//

TInt CMediatorTest::SubscribeToEventNonExistCapabilities( TTestResult& aResult )
    {
    TInt status( KErrNone );
    TUid myUid = TUid::Uid( KOtherUid );
    TUid catUid = TUid::Uid( KOtherSIDCategory );
    TVersion version( 1,0,0 );
    
    status = iEventConsumer->SubscribeEvent( myUid, 
                                             catUid,
                                             KNonExistingEventCapaId,
                                             version);   
    //expected status is KErrPermissionDenied                                           
    if( KErrPermissionDenied == status )
        {
        _LIT( KDescription, "SubscribeEvent returned with expected KErrPermissionDenied" );
        aResult.SetResult( KErrNone, KDescription );        
        }
    else
        {
        _LIT( KErrorDescription, "Wrong error status" );
        aResult.SetResult( KErrGeneral, KErrorDescription );
        }
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CMediatorTest::SubscribeToEventCorrectParam
//
// Expected preconditions: Event is existing
// Expected behaviour: Subscribe succeed (KErrNone)
// Cleaning: Unsubscribe event
//
// Subscribe event
// Domain: KTestUid
// Category: KTestUid
// Event: KTestEventId
// -----------------------------------------------------------------------------
//

TInt CMediatorTest::SubscribeToEventCorrectParam( TTestResult& aResult )
    {
    TInt status( KErrNone );
    TUid myUid = TUid::Uid( KTestUid );
    TCapabilitySet caps;
    caps.SetEmpty();
    
    TInt preStatus = PreconditionRegisterOneEvent( myUid, myUid, KTestEventId, iDefaultVersion, caps );
    _LIT( KErrorDescription, "PreconditionRegisterOneEvent returns with" );
    aResult.SetResult( preStatus, KErrorDescription );
    
    if( KErrNone == preStatus )
        {                                 
        status = iEventConsumer->SubscribeEvent( myUid, 
                                                myUid,
                                                KTestEventId,
                                                iDefaultVersion );    
        
        _LIT( KErrorDescription, "SubscribeEvent returns with" );
        aResult.SetResult( status, KErrorDescription );
        if( KErrNone == status  )
            {
            status = CleanSubscribeToEvent( myUid, myUid, KTestEventId );
            if( KErrNone != status )
                {
                _LIT( KErrorDescription, "Test (SubscribeEvent) passed but unsubscribe failed" );
                aResult.SetResult( status, KErrorDescription );           
                }
            }
        //unregister the registered event
        status = CleanRegisterOneEvent( myUid, myUid, KTestEventId );
        if( KErrNone != status )
            {
            _LIT( KErrorDescription, "Test code passed but cleaning event failed" );
            aResult.SetResult( status, KErrorDescription );
            }                                         
        }
    
    return KErrNone;
    }

TInt CMediatorTest::CleanSubscribeToEvent( TUid aDomain,
                                           TUid aCategory, 
                                           TInt aEventId )
    {
    return iEventConsumer->UnsubscribeEvent( aDomain, 
                                             aCategory,
                                             aEventId ) ;
    }

// -----------------------------------------------------------------------------
// CMediatorTest::SubscribeToListOfEventsCorrectParam
//
// Expected preconditions: Event is existing
// Expected behaviour: Subscribe succeed (KErrNone)
// Cleaning: Unsubscribe event
//
// Subscribe list of events
// Domain: KTestUid
// Category: KTestUid
// Events: 80 - 90
// -----------------------------------------------------------------------------
//

TInt CMediatorTest::SubscribeToListOfEventsCorrectParamL( TTestResult& aResult )
    {
    TInt status( KErrNone );
    TUid myUid = TUid::Uid( KTestUid );
    TCapabilitySet caps;
    caps.SetEmpty();
    REventList eventList;
    CleanupClosePushL( eventList );                  
    for( TInt i=80; i<91; i++ ) // Create list of events (i = event ID)
        {
        TEvent newEvent;
        newEvent.iEventId = i;
        newEvent.iVersion = iDefaultVersion;
        newEvent.iCaps = caps;    
        eventList.AppendL( newEvent );
        }
    TInt preStatus = PreconditionRegisterEvents( myUid, myUid, eventList );
    _LIT( KDescription, "PreconditionRegisterEvents returns with" );
    aResult.SetResult( preStatus, KDescription );
    
    if( KErrNone == preStatus )
        {
        status = iEventConsumer->SubscribeEvent( myUid, 
                                                 myUid,
                                                 eventList );
        
        _LIT( KDescription, "SubscribeEvent returns with" );
        aResult.SetResult( status, KDescription );
        
        if( KErrNone == status  )
            {
            status = CleanSubscribeToListOfEvents( myUid, myUid, eventList );
            if( KErrNone != status )
                {
                _LIT( KDescription, "Test(SubscribeEvent) passed but unsbscribe failed" );
                aResult.SetResult( status, KDescription );
                }
            }
        //clean registered events
        status = CleanRegisterMultipleEvents( myUid, myUid, eventList );
        if( KErrNone != status )
            {
            _LIT( KErrorDescription, "Test passed but cleaning events failed" );
            aResult.SetResult( status, KErrorDescription );
            }             
        }
    CleanupStack::PopAndDestroy( &eventList );
    return KErrNone;
    }

TInt CMediatorTest::PreconditionRegisterEvents( TUid aDomain, 
                                                TUid aCategory, 
                                                REventList aEvents )
    {   
    return iEventProvider->RegisterEvent( aDomain, aCategory, aEvents);
    }

TInt CMediatorTest::CleanSubscribeToListOfEvents( TUid aDomain,
                                                  TUid aCategory, 
                                                  REventList aEvents )
    {
    return iEventConsumer->UnsubscribeEvent( aDomain, 
                                                    aCategory,
                                                    aEvents ) ;
    }


// -----------------------------------------------------------------------------
// CMediatorTest::SubscribeToListOfEventsOneInfoIncorrect
//
// Expected preconditions: Event is existing
// Expected behaviour: Subscribe fail (KErrMediatorVersionMismatch)
// Cleaning: Unsubscribe event
//
// Subscribe to list of events (one item in list not valid)
// Domain: KTestUid
// Category; KTestUid
// Events: 91 - KMaxListId (One event version is incorrect)
// -----------------------------------------------------------------------------
//

TInt CMediatorTest::SubscribeToListOfEventsOneInfoIncorrectL( TTestResult& aResult )
    {
    TInt status( KErrNone );
    TUid myUid = TUid::Uid( KTestUid );
    TCapabilitySet caps;
    caps.SetEmpty();
    TVersion invalidVersion( 6, 5, 5 );
    
    REventList eventList;
    CleanupClosePushL( eventList );
    
    for( TInt i=91; i<101; i++ ) // Create list of events (i = event ID)
        {
        TEvent newEvent;
        newEvent.iEventId = i;
        newEvent.iVersion = iDefaultVersion;
        newEvent.iCaps = caps;    
        eventList.AppendL( newEvent );
        }
      
    REventList myList = eventList;
            
    TInt preStatus = PreconditionRegisterEvents( myUid, myUid, myList );
    _LIT( KErrorDescription, "PreconditionRegisterEvents returns with" );
    aResult.SetResult( preStatus, KErrorDescription );
            
    if( KErrNone == preStatus )
        {
        TEvent newEvent;
        newEvent.iEventId = 100;
        newEvent.iVersion = invalidVersion;//wrong version 
        newEvent.iCaps = caps;    
        eventList.AppendL( newEvent );
      
        status = iEventConsumer->SubscribeEvent( myUid, 
                                                 myUid,
                                                 eventList );
        //expected status is KErrMediatorVersionMismatch  
        if( KErrMediatorVersionMismatch == status )
            {
            _LIT( KDescription, "SubscribeEvent returned with expected KErrMediatorVersionMismatch" );
            aResult.SetResult( KErrNone, KDescription );  
            //clean the registered events
            status = CleanRegisterMultipleEvents( myUid, myUid, myList );
            if( KErrNone != status )
                {
                _LIT( KErrorDescription, "Test passed but cleaning failed" );
                aResult.SetResult( status, KErrorDescription );
                }
            }
        else
            {
            _LIT( KErrorDescription, "Wrong error status" );
            aResult.SetResult( KErrGeneral, KErrorDescription );
            }
        }
    CleanupStack::PopAndDestroy( &eventList );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CMediatorTest::RaiseNonExistingEventWrongDomain
//
// Expected preconditions: There are domains but requested domain does not exist
// Expected behaviour: Raise fail (KErrMediatorDomainNotFound)
// Cleaning: Not needed
//
// Try to raise event (event not registered) 
// Domain: KNonExistingDomainId
// Category: KTestUid   
// Event: KTestEventId
// -----------------------------------------------------------------------------
//

TInt CMediatorTest::RaiseNonExistingEventWrongDomainL( TTestResult& aResult )
    {
    TInt status( KErrNone );
    TUid invalidUid = TUid::Uid( KNonExistingDomainId );
    TUid myUid = TUid::Uid( KTestUid );
    TCapabilitySet caps;
    caps.SetEmpty();

    _LIT8( KTest, "TestTestTest" );
    HBufC8* data = KTest().AllocLC();
    status = iEventProvider->RaiseEvent( invalidUid,
                                         myUid, 
                                         KTestEventId,
                                         iDefaultVersion, 
                                         *data );
    
    CleanupStack::PopAndDestroy( data );
    //expected status is KErrMediatorDomainNotFound 
    if( KErrMediatorDomainNotFound == status )
        {
        _LIT( KDescription, "RaiseEvent returned with expected KErrMediatorDomainNotFound" );
        aResult.SetResult( KErrNone, KDescription );        
        }
    else
        {
        _LIT( KErrorDescription, "Wrong error status" );
        aResult.SetResult( KErrGeneral, KErrorDescription );
        }
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CMediatorTest::RaiseNonExistingEventWrongCat
//
// Expected preconditions: There are domains but requested category does not exist
// Expected behaviour: Raise fail (KErrMediatorCategoryNotFound)
// Cleaning: Not needed
//
// Try to raise event (category does not exist)
// Domain: KOtherUid
// Category: KNonExistingCategoryId
// Event: KTestEventId
// -----------------------------------------------------------------------------
//

TInt CMediatorTest::RaiseNonExistingEventWrongCatL( TTestResult& aResult )
    {
    TInt status( KErrNone );
    TUid invalidUid = TUid::Uid( KNonExistingCategoryId );
    TUid myUid = TUid::Uid(KOtherUid);
    TCapabilitySet caps;
    caps.SetEmpty();

    _LIT8( KTest, "TestTestTest" );
    HBufC8* data = KTest().AllocLC();

    status = iEventProvider->RaiseEvent( myUid,
                                         invalidUid, 
                                         KTestEventId,
                                         iDefaultVersion, 
                                         *data );
    
    CleanupStack::PopAndDestroy( data );
    //expected status is KErrMediatorCategoryNotFound
    if( KErrMediatorCategoryNotFound == status )
        {
        _LIT( KDescription, "RaiseEvent returned with expected KErrMediatorCategoryNotFound" );
        aResult.SetResult( KErrNone, KDescription );        
        }
    else
        {
        _LIT( KErrorDescription, "Wrong error status" );
        aResult.SetResult( KErrGeneral, KErrorDescription );
        }
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CMediatorTest::RaiseNonExistingEventEve
//
// Expected preconditions: There is domain and category but requested event does not exist
// Expected behaviour: Raise fail (KErrMediatorEventNotFound)
// Cleaning: Unregister event
//
// Raise event (event does not exist)
// Domain: KTestUid  
// Category: KTestUid
// Event: KNonExistingEventId
// -----------------------------------------------------------------------------
//

TInt CMediatorTest::RaiseNonExistingEventEveL( TTestResult& aResult )
    {
    TInt status( KErrNone );
    TUid myUid = TUid::Uid( KTestUid );
    TCapabilitySet caps;
    caps.SetEmpty();

    _LIT8( KTest, "TestTestTest" );
    HBufC8* data = KTest().AllocLC();
    TInt preStatus = PreconditionRegisterOneEvent( myUid, myUid, KTestEventId, iDefaultVersion, caps );
    if( KErrNone == status  )
        {
        status = iEventProvider->RaiseEvent( myUid,
                                             myUid, 
                                             KNonExistingEventId,
                                             iDefaultVersion, 
                                             *data );
        CleanupStack::PopAndDestroy( data );
        //expected status is KErrMediatorEventNotFound
        if( KErrMediatorEventNotFound == status )
            {
            _LIT( KDescription, "RaiseEvent returned with expected KErrMediatorEventNotFound" );
            aResult.SetResult( KErrNone, KDescription );
            status = CleanRegisterOneEvent( myUid, myUid, KTestEventId );
            if( KErrNone != status )
                    {
                    _LIT( KErrorDescription, "Test passed but cleaning failed" );
                    aResult.SetResult( status, KErrorDescription );
                    }
            }
        else
            {
            _LIT( KErrorDescription, "Wrong error status" );
            aResult.SetResult( KErrGeneral, KErrorDescription );
            }
        }
    return KErrNone;
    }


// -----------------------------------------------------------------------------
// CMediatorTest::RaiseEventWithWrongEventVerMajor
//
// Expected preconditions: There is domain and category but requested event version does not exist
// Expected behaviour: Raise fail (KErrMediatorVersionMismatch)
// Cleaning: Not needed
//
// Try to raise event (wrong version)
// Domain: KTestUid
// Category: KTestUid
// Event: KTestEventId 
// -----------------------------------------------------------------------------
//

TInt CMediatorTest::RaiseEventWithWrongEventVerMajorL( TTestResult& aResult )
    {
    TInt status( KErrNone );
    TUid myUid = TUid::Uid( KTestUid );
    TCapabilitySet caps;
    caps.SetEmpty();
    TVersion version( 888, 5, 5 );
    
    TInt preStatus = PreconditionRegisterOneEvent( myUid, myUid, KTestEventId, iDefaultVersion, caps );
    _LIT( KErrorDescription, "PreconditionRegisterOneEvent returns with" );
    aResult.SetResult( preStatus, KErrorDescription );
    
    if( KErrNone == preStatus )
        {
        _LIT8( KTest, "TestTestTest" );
        HBufC8* data = KTest().AllocLC();
        status = iEventProvider->RaiseEvent( myUid,
                                             myUid, 
                                             KTestEventId,
                                             version, 
                                             *data );
        CleanupStack::PopAndDestroy( data );
        //expected status is KErrMediatorVersionMismatch
        if( KErrMediatorVersionMismatch == status )
            {
            _LIT( KDescription, "RaiseEvent returned with expected KErrMediatorVersionMismatch" );
            aResult.SetResult( KErrNone, KDescription );
            status = CleanRegisterOneEvent( myUid, myUid, KTestEventId );
            if( KErrNone != status )
                {
                _LIT( KErrorDescription, "Test passed but cleaning failed" );
                aResult.SetResult( status, KErrorDescription );
                } 
            }
        else
            {
            _LIT( KErrorDescription, "Wrong error status" );
            aResult.SetResult( KErrGeneral, KErrorDescription );
            }
        }
    return KErrNone;
    }


// -----------------------------------------------------------------------------
// CMediatorTest::RaiseEventRequiresNotExistingCapsInPub
//
// Expected preconditions: There is domain and category but requested event capabilites does not exist
// Expected behaviour: Raise fail (KErrMediatorDomainNotFound)
// Cleaning: Not needed
//
// Raise event that requires capabilities not existing in publisher
// Domain: KTestUid
// Category: KTestUid
// Event: KNonExistingEventCapaId
// -----------------------------------------------------------------------------
//

TInt CMediatorTest::RaiseEventRequiresNotExistingCapsInPubL( TTestResult& aResult )
    {
    TInt status( KErrNone );
    TUid myUid = TUid::Uid( KOtherUid );
    TUid catUid = TUid::Uid( KOtherSIDCategory );
    TVersion version( 1, 0 ,0 );

    _LIT8(KTest, "TestTestTest");
    HBufC8* data = KTest().AllocLC();

    status = iEventProvider->RaiseEvent( myUid,
                                         catUid, 
                                         KNonExistingEventCapaId, // static event
                                         version, 
                                         *data );
    CleanupStack::PopAndDestroy( data );

    _LIT( KErrorDescription, "" );
    aResult.SetResult( status, KErrorDescription );

    return KErrNone;
}

// -----------------------------------------------------------------------------
// CMediatorTest::RaiseExistingEvent
//
// Expected preconditions: Event exist
// Expected behaviour: Raise succeed
// Cleaning: Event unregistered
//
// Raise event
// Domain: KTestUid
// Category: KTestUid
// Event ID: KTestEventId
// -----------------------------------------------------------------------------
//

TInt CMediatorTest::RaiseExistingEventL( TTestResult& aResult )
    {
    TInt status( KErrNone );
    TUid myUid = TUid::Uid( KTestUid );
    TCapabilitySet caps;
    caps.SetEmpty();
    
    iTestHelper->ClearAllNotifyData();
    
    TInt preStatus = PreconditionRegisterOneEvent( myUid, myUid, KTestEventId, iDefaultVersion, caps );
    _LIT( KErrorDescription, "Preconditions failed" );
    aResult.SetResult( preStatus, KErrorDescription );
    
    if( KErrNone == preStatus )
        { 
        CMediatorEventProvider* eventProvider = CMediatorEventProvider::NewL();
        CleanupStack::PushL( eventProvider );
        
        _LIT8(KTest, "TestTestTest");
        HBufC8* data = KTest().AllocLC();
       
        status = eventProvider->RaiseEvent( myUid,
                                            myUid, 
                                            KTestEventId,
                                            iDefaultVersion, 
                                            *data );        
        // wait 3s to make sure that all asyncronous events are completed
        CTestScheduler::StartAndStopL( 3000 ); 
                                                     
        CleanupStack::PopAndDestroy( data );
        CleanupStack::PopAndDestroy( eventProvider );
        _LIT( KDescription, "RaiseEvent returns with" );
        aResult.SetResult( status, KDescription );      
        status = CleanRegisterOneEvent(myUid, myUid, KTestEventId);
        if( KErrNone != status )
            {
            _LIT( KErrorDescription, "Test Passed but cleaning event failed" );
            aResult.SetResult( status, KErrorDescription );
            }      
        }
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CMediatorTest::SubscribeExistingEventRaiseFromPublisherSide
//
// Expected preconditions: Event exist
// Expected behaviour: Raise succeed
// Cleaning: Event unregistered
//
// Raise event
// Domain: KTestUid
// Category: KTestUid
// Event ID: KTestEventId
// -----------------------------------------------------------------------------
//

TInt CMediatorTest::SubscribeExistingEventRaiseFromPublisherSideL( TTestResult& aResult )
    {
    TInt status( KErrNone );
    TUid myUid = TUid::Uid( KTestUid );
    TCapabilitySet caps;
    caps.SetEmpty();

    CMediatorEventProvider* eventProvider = CMediatorEventProvider::NewL();
    CleanupStack::PushL( eventProvider );
    _LIT8(KTest, "TestTestTest");
    HBufC8* data = KTest().AllocLC();

    // Clear all notify information from helper
    iTestHelper->ClearAllNotifyData();

    TInt preStatus = PreconditionRegisterOneEvent( myUid, myUid, KTestEventId, iDefaultVersion, caps );
    _LIT( KErrorDescription, "PreconditionRegisterOneEvent returns with" );
    aResult.SetResult( preStatus, KErrorDescription );
    
    if( KErrNone == preStatus )
        {
        status = iEventConsumer->SubscribeEvent( myUid, 
                                                 myUid,
                                                 KTestEventId,
                                                 iDefaultVersion) ;  
        CTestScheduler::StartAndStopL( 1000 ); // 1s delay 
        
        if( KErrNone == status  )
            {
            status = eventProvider->RaiseEvent( myUid,
                                                myUid, 
                                                KTestEventId,
                                                iDefaultVersion, 
                                                *data );
            // wait 2s to make sure that all asyncronous events are completed
            CTestScheduler::StartAndStopL( 2000 ); 
            _LIT( KDescription, "RaiseEvent returns with" );
            aResult.SetResult( status, KDescription );
            /* Check that notify was received, only when RaiseEvent returns KErrNone*/
            if ( ( KErrNone == status  )
                &&( ( iTestHelper->iNotificationReceived == 0 ) 
                        || ( iTestHelper->iDomain != myUid )
                        || ( iTestHelper->iCategory != myUid )
                        || ( iTestHelper->iEventId != KTestEventId ) ) )
                {
                status = KErrNotFound; 
                _LIT( KDescription, "iTestHelper checks failed" );
                aResult.SetResult( status, KDescription );  
                }
            else if ( KErrNone == status  )
                {
                status = CleanRegisterOneEvent( myUid, myUid, KTestEventId ) ;
                if( KErrNone != status )
                    {
                    _LIT( KErrorDescription, "Test passed but cleaning event failed" );
                    aResult.SetResult( status, KErrorDescription );
                    }   
                }
            }
        }
    CleanupStack::PopAndDestroy( data );
    CleanupStack::PopAndDestroy( eventProvider );
    return KErrNone;
    }


// -----------------------------------------------------------------------------
// CMediatorTest::SubscribeExistingEventRaiseFromPublisherSideDataOver1kB
//
// Expected preconditions: Event exist
// Expected behaviour: Raise succeed
// Cleaning: Event unregistered
// 
// -----------------------------------------------------------------------------
//

TInt CMediatorTest::SubscribeExistingEventRaiseFromPublisherSideDataOver1kBL( TTestResult& aResult )
    {
    TInt status( KErrNone );
    TUid myUid = TUid::Uid( KTestUid );
    TCapabilitySet caps;
    caps.SetEmpty();

    CMediatorEventProvider* eventProvider = CMediatorEventProvider::NewL();
    CleanupStack::PushL( eventProvider );

    // Clear all notify information from helper
    iTestHelper->ClearAllNotifyData();

    TInt preStatus = PreconditionRegisterOneEvent( myUid, myUid, KTestEventId, iDefaultVersion, caps );
    _LIT( KErrorDescription, "PreconditionRegisterOneEvent returns with" );
    aResult.SetResult( preStatus, KErrorDescription );
    
    if( KErrNone == preStatus )
        {
        status = iEventConsumer->SubscribeEvent( myUid, 
                                                 myUid,
                                                 KTestEventId,
                                                 iDefaultVersion ) ;
                
        HBufC8* data = HBufC8::NewLC( KDataSize );
        TPtr8 dataPtr = data->Des();
        for( TInt i = 0; i < KDataSize; i++ )
            {
            TChar tmpChar(64);
            dataPtr.Append( tmpChar );
            }
        _LIT( KDescription, "SubscribeEvent returns with" );
        aResult.SetResult( status, KDescription );
        if( KErrNone == status  )
            {
            status = eventProvider->RaiseEvent( myUid,
                                                myUid, 
                                                KTestEventId,
                                                iDefaultVersion, 
                                                *data );
            _LIT( KDescription, "RaiseEvent returns with" );
            aResult.SetResult( status, KDescription );
            
            // wait 2s to make sure that all asyncronous events are completed
            CTestScheduler::StartAndStopL(2000); 
            
            /* Check that notify was received, only when RaiseEvent returns KErrNone*/
            if( ( KErrNone == status  )
                &&( ( iTestHelper->iNotificationReceived == 0 ) 
                        || ( iTestHelper->iDomain != myUid )
                        || ( iTestHelper->iCategory != myUid )
                        || ( iTestHelper->iEventId != KTestEventId ) ) )
                {
                status = KErrNotFound; 
                _LIT( KDescription, "iTestHelper checks failed" );
                aResult.SetResult( status, KDescription );  
                }
            else if ( KErrNone == status  )
                {
                status = CleanRegisterOneEvent( myUid, myUid, KTestEventId ) ;
                if( KErrNone != status )
                    {
                    _LIT( KErrorDescription, "Test passed but cleaning event failed" );
                    aResult.SetResult( status, KErrorDescription );
                    }   
                }
            }
        CleanupStack::PopAndDestroy( data );  
        CleanupStack::PopAndDestroy( eventProvider );
        }
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CMediatorTest::SubscribeToEventDeleteConsumerInstanceRaiseEventFromPublisher
// 
// Expected preconditions: Event exist
// Expected behaviour: Raise succeed
// Cleaning: Event unregistered
// 
// -----------------------------------------------------------------------------
//

TInt CMediatorTest::SubscribeToEventDeleteConsumerInstanceRaiseEventFromPublisherL( TTestResult& aResult )
    {
    TInt status( KErrNone );
    TUid myUid = TUid::Uid( KTestUid );
    TCapabilitySet caps;
    caps.SetEmpty();

    // Clear all notify information from helper
    iTestHelper->ClearAllNotifyData();
    
    TInt preStatus = PreconditionRegisterOneEvent( myUid, myUid, KTestEventId, iDefaultVersion, caps);
    _LIT( KErrorDescription, "PreconditionRegisterOneEvent returns with" );
    aResult.SetResult( preStatus, KErrorDescription );
    
    CMediatorEventConsumer* eventConsumer = CMediatorEventConsumer::NewL( iTestHelper );
    CleanupStack::PushL( eventConsumer );
    if( KErrNone == preStatus )
        {
        status = eventConsumer->SubscribeEvent( myUid, 
                                                myUid,
                                                KTestEventId,
                                                iDefaultVersion ) ;  
        
        _LIT8(KTest, "TestTestTest");
        HBufC8* data = KTest().AllocLC();
        
        _LIT( KDescription, "SubscribeEvent returns with" );
        aResult.SetResult( status, KDescription );
        if( KErrNone == status  )
            {
            CMediatorEventProvider* eventProvider = CMediatorEventProvider::NewL();
            CleanupStack::PushL( eventProvider );
            status = eventProvider->RaiseEvent( myUid,
                                                myUid, 
                                                KTestEventId,
                                                iDefaultVersion, 
                                                *data );
            
            _LIT( KDescription, "RaiseEvent returns with" );
            aResult.SetResult( status, KDescription );
            // wait 2s to make sure that all asyncronous events are completed
            CTestScheduler::StartAndStopL(2000);
            /* Check that notify was received, only when RaiseEvent returns KErrNone*/
            if( ( KErrNone == status  )
                &&( ( iTestHelper->iNotificationReceived == 0 ) 
                        || ( iTestHelper->iDomain != myUid )
                        || ( iTestHelper->iCategory != myUid )
                        || ( iTestHelper->iEventId != KTestEventId ) ) )
                {
                status = KErrNotFound; 
                _LIT( KDescription, "iTestHelper checks failed" );
                aResult.SetResult( status, KDescription );  
                }
            //clean the registered event
            else if ( KErrNone == status  )
                {
                status = CleanRegisterOneEvent( myUid, myUid, KTestEventId ) ;
                if( KErrNone != status )
                    {
                    _LIT( KErrorDescription, "Test passed but cleaning event failed" );
                    aResult.SetResult( status, KErrorDescription );
                    }   
                }
            CleanupStack::PopAndDestroy( eventProvider );
            }
        CleanupStack::PopAndDestroy( data );
        }
    CleanupStack::PopAndDestroy(eventConsumer);
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CMediatorTest::Add2ndSubscriberToEventRaiseEventFromPublisher
// 
// Expected preconditions: Event exist
// Expected behaviour: Raise succeed
// Cleaning: Event unregistered
// 
// -----------------------------------------------------------------------------
//

TInt CMediatorTest::Add2ndSubscriberToEventRaiseEventFromPublisherL( TTestResult& aResult )
    {
    TInt status( KErrNone );
    TUid myUid = TUid::Uid( KTestUid );
    TCapabilitySet caps;
    caps.SetEmpty();

    CMediatorEventConsumer* eventConsumer = CMediatorEventConsumer::NewL( iTestHelper );
    CleanupStack::PushL( eventConsumer );
    CMediatorEventConsumer* otherEventConsumer = CMediatorEventConsumer::NewL( iAnotherTestHelper );
    CleanupStack::PushL( otherEventConsumer );
    iTestHelper->ClearAllNotifyData();
    iAnotherTestHelper->ClearAllNotifyData();

    TInt preStatus = PreconditionRegisterOneEvent( myUid, myUid, KTestEventId, iDefaultVersion, caps );
    _LIT( KErrorDescription, "PreconditionRegisterOneEvent returns with" );
    aResult.SetResult( preStatus, KErrorDescription );

    if( KErrNone == preStatus )
        {  
        status = eventConsumer->SubscribeEvent( myUid, 
                                                myUid,
                                                KTestEventId,
                                                iDefaultVersion );    
        TInt status2( KErrNone );                                        
        status2 = otherEventConsumer->SubscribeEvent( myUid, 
                                                      myUid,
                                                      KTestEventId,
                                                      iDefaultVersion);
              
        _LIT8(KTest, "TestTestTest");
        HBufC8* data = KTest().AllocLC();
        
        _LIT( KDescription, "SubscribeEvent returns with" );
        aResult.SetResult( status, KDescription );
        
        if( ( KErrNone == status2 ) && ( KErrNone ==status ) )
            {
            CMediatorEventProvider* eventProvider = CMediatorEventProvider::NewL();
            CleanupStack::PushL( eventProvider );           
            status = eventProvider->RaiseEvent( myUid,
                                                myUid, 
                                                KTestEventId,
                                                iDefaultVersion, 
                                                *data );
            _LIT( KDescription, "RaiseEvent returns with" );
            aResult.SetResult( status, KDescription );
            // wait 2s to make sure that all asyncronous events are completed
            CTestScheduler::StartAndStopL(2000);
            /* Check that notify was received for both notification, only when RaiseEvent returns KErrNone*/
            if( ( KErrNone == status  )
                  &&( ( iTestHelper->iNotificationReceived == 0 ) 
                          || ( iTestHelper->iDomain != myUid )
                          || ( iTestHelper->iCategory != myUid )
                          || ( iTestHelper->iEventId != KTestEventId ) )
                          
                  &&( ( iAnotherTestHelper->iNotificationReceived == 0 ) 
                          || ( iAnotherTestHelper->iDomain != myUid )
                          || ( iAnotherTestHelper->iCategory != myUid )
                          || ( iAnotherTestHelper->iEventId != KTestEventId ) ) )
                  {
                  status = KErrNotFound; 
                  _LIT( KDescription, "iTestHelper checks failed" );
                  aResult.SetResult( status, KDescription );  
                  }
            else if ( KErrNone == status  )
                {
                status = CleanRegisterOneEvent( myUid, myUid, KTestEventId ) ;
                if( KErrNone != status )
                    {
                    _LIT( KErrorDescription, "Test passed but cleaning event failed" );
                    aResult.SetResult( status, KErrorDescription );
                    }   
                }
            CleanupStack::PopAndDestroy( eventProvider );
            }
        CleanupStack::PopAndDestroy( data );
        }
    CleanupStack::PopAndDestroy( otherEventConsumer );
    CleanupStack::PopAndDestroy( eventConsumer );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CMediatorTest::UnsubscribeFrom1stSubscriber
// 
// Expected preconditions: Event exist
// Expected behaviour: Raise succeed
// Cleaning: Event unregistered
// 
// -----------------------------------------------------------------------------
//

TInt CMediatorTest::UnsubscribeFrom1stSubscriberL( TTestResult& aResult )
    {
    TInt status( KErrNone );
    TUid myUid = TUid::Uid( KTestUid );
    TCapabilitySet caps;
    caps.SetEmpty();

    CMediatorEventConsumer* eventConsumer = CMediatorEventConsumer::NewL( iTestHelper );
    CleanupStack::PushL( eventConsumer );
    CMediatorEventConsumer* otherEventConsumer = CMediatorEventConsumer::NewL( iAnotherTestHelper );
    CleanupStack::PushL( otherEventConsumer );
    iTestHelper->ClearAllNotifyData();
    iAnotherTestHelper->ClearAllNotifyData();

    TInt preStatus = PreconditionRegisterOneEvent( myUid, myUid, KTestEventId, iDefaultVersion, caps );
    _LIT( KErrorDescription, "PreconditionRegisterOneEvent returns with" );
    aResult.SetResult( preStatus, KErrorDescription );

    if( KErrNone == preStatus )
        {  
        status = eventConsumer->SubscribeEvent( myUid, 
                                                myUid,
                                                KTestEventId,
                                                iDefaultVersion );    
        TInt status2( KErrNone );                                        
        status2 = otherEventConsumer->SubscribeEvent( myUid, 
                                                      myUid,
                                                      KTestEventId,
                                                      iDefaultVersion);
              
        _LIT8(KTest, "TestTestTest");
        HBufC8* data = KTest().AllocLC();
        
        _LIT( KDescription, "SubscribeEvent returns with" );
        aResult.SetResult( status, KDescription );
        
        if( ( KErrNone == status2 ) && ( KErrNone ==status ) )
            {
            CMediatorEventProvider* eventProvider = CMediatorEventProvider::NewL();
            CleanupStack::PushL( eventProvider ); 
            status = otherEventConsumer->UnsubscribeEvent( myUid, 
                                                           myUid,
                                                           KTestEventId);
            _LIT( KDescription, "UnsubscribeEvent returns with" );
            aResult.SetResult( status, KDescription );
            if( KErrNone == status  )
                {
                status = eventProvider->RaiseEvent( myUid,
                                                    myUid, 
                                                    KTestEventId,
                                                    iDefaultVersion, 
                                                    *data );
                
                _LIT( KDescription, "RaiseEvent returns with" );
                aResult.SetResult( status, KDescription );
                
                // wait 2s to make sure that all asyncronous events are completed
                CTestScheduler::StartAndStopL(2000);
                
                /* Check that notify was received for only one notification, only when RaiseEvent returns KErrNone*/
                if( ( KErrNone == status  )
                    &&( ( iTestHelper->iNotificationReceived == 0 ) 
                          || ( iTestHelper->iDomain != myUid )
                          || ( iTestHelper->iCategory != myUid )
                          || ( iTestHelper->iEventId != KTestEventId ) )
                          
                    &&( ( iAnotherTestHelper->iNotificationReceived != 0 ) 
                          || ( iAnotherTestHelper->iDomain == myUid )
                          || ( iAnotherTestHelper->iCategory == myUid )
                          || ( iAnotherTestHelper->iEventId == KTestEventId ) ) )
                    {
                    status = KErrNotFound; 
                    _LIT( KDescription, "notifications check failed" );
                    aResult.SetResult( status, KDescription );  
                    }
                else if ( KErrNone == status  )
                    {
                    status = CleanRegisterOneEvent( myUid, myUid, KTestEventId ) ;
                    if( KErrNone != status )
                        {
                        _LIT( KErrorDescription, "Test passed but cleaning event failed" );
                        aResult.SetResult( status, KErrorDescription );
                        }   
                    }
                }
            CleanupStack::PopAndDestroy( eventProvider );
            }
        CleanupStack::PopAndDestroy( data );
        }
    CleanupStack::PopAndDestroy( otherEventConsumer );
    CleanupStack::PopAndDestroy( eventConsumer );
    return KErrNone;
    }
  
// -----------------------------------------------------------------------------
// CMediatorTest::SubscribeToEventRaise500TimesInLoop
// 
// Expected preconditions: Event exist
// Expected behaviour: Subscribe and raise succeed
// Cleaning: Event unregistered
//  
// -----------------------------------------------------------------------------
//

TInt CMediatorTest::SubscribeToEventRaise500TimesInLoopL( TTestResult& aResult )
    {
    TInt status( KErrNone );
    TUid myUid = TUid::Uid( KTestUid );
    TCapabilitySet caps;
    caps.SetEmpty();

    iTestHelper->ClearAllNotifyData();

    TInt preStatus = PreconditionRegisterOneEvent( myUid, myUid, KTestEventId, iDefaultVersion, caps );
    _LIT( KErrorDescription, "PreconditionRegisterOneEvent returns with" );
    aResult.SetResult( preStatus, KErrorDescription );

    if( KErrNone == preStatus )
        {  
        status = iEventConsumer->SubscribeEvent( myUid, 
                                                 myUid,
                                                 KTestEventId,
                                                 iDefaultVersion );  
        _LIT( KDescription, "SubscribeEvent returns with" );
        aResult.SetResult( status, KDescription );
        if( KErrNone == status  )
            {
            CMediatorEventProvider* eventProvider = CMediatorEventProvider::NewL();
            CleanupStack::PushL( eventProvider );
            _LIT8(KTest, "TestTestTest");
            HBufC8* data = KTest().AllocLC();
          
            for(TInt i=0; i<550; i++)
                {     
                status = eventProvider->RaiseEvent( myUid,
                                                    myUid, 
                                                    KTestEventId,
                                                    iDefaultVersion, 
                                                    *data );
                if( KErrNone != status)
                    {
                    break;
                    }
                }
            _LIT( KDescription, "RaiseEvent returns with" );
            aResult.SetResult( status, KDescription );
            // wait 2s to make sure that all asyncronous events are completed
            CTestScheduler::StartAndStopL(2000);
            
            /* Check that notify was received, only when RaiseEvent returns KErrNone*/
            if( ( KErrNone == status  )
                &&( ( iTestHelper->iNotificationReceived == 0 ) 
                        || ( iTestHelper->iDomain != myUid )
                        || ( iTestHelper->iCategory != myUid )
                        || ( iTestHelper->iEventId != KTestEventId ) ) )
                {
                status = KErrNotFound; 
                _LIT( KDescription, "iTestHelper checks failed" );
                aResult.SetResult( status, KDescription );  
                }
            //clean the registered event
            else if ( KErrNone == status  )
                {
                status = CleanRegisterOneEvent( myUid, myUid, KTestEventId ) ;
                if( KErrNone != status )
                    {
                    _LIT( KErrorDescription, "Test passed but cleaning event failed" );
                    aResult.SetResult( status, KErrorDescription );
                    }   
                }
            CleanupStack::PopAndDestroy( data );
            CleanupStack::PopAndDestroy( eventProvider );
            }
        }
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CMediatorTest::RegisterOneCommandToMediator
// 
// Expected preconditions: Command does not exist
// Expected behaviour: Register command succeed
// Cleaning: Command unregistered
//  
// -----------------------------------------------------------------------------
//

TInt CMediatorTest::RegisterOneCommandToMediator( TTestResult& aResult )
    {
    TInt status( KErrNone );
    TUid myUid = TUid::Uid( KTestUid );
    TCapabilitySet caps;
    caps.SetEmpty();
    
    status = iCommandResponder->RegisterCommand( myUid, 
                                                 myUid, 
                                                 KDefaultCommandId, 
                                                 iDefaultVersion,
                                                 caps, 
                                                 KDefaultTimeOut );
    
    _LIT( KDescription, "RegisterCommand returns with" );
    aResult.SetResult( status, KDescription );
       
    if( KErrNone == status  )
        {
        status = CleanRegisterCommand( myUid, 
                                       myUid, 
                                       KDefaultCommandId );
        if( KErrNone != status )
            {
            _LIT( KDescription, "Test passed but cleaning failed" );
            aResult.SetResult( status, KDescription );     
            }
        }
    return KErrNone;    
    }

TInt CMediatorTest::CleanRegisterCommand( TUid aDomain, 
                                         TUid aCategory, 
                                         TInt aCommandId )
    {
    TInt status = iCommandResponder->UnregisterCommand( aDomain, 
                                                       aCategory, 
                                                       aCommandId );  
    CTestScheduler::StartAndStopL( 1000 ); // wait 1s
    return status;
    }
                                          
// -----------------------------------------------------------------------------
// CMediatorTest::RegisterMultipleCommandsToMediator
// 
// Expected preconditions: Command does not exist
// Expected behaviour: Register command succeed
// Cleaning: Command unregistered
//  
// Register multiple commands
// Command ID(s): 1 -  KMaxListId
// -----------------------------------------------------------------------------
//

TInt CMediatorTest::RegisterMultipleCommandsToMediatorL( TTestResult& aResult )
    {
    TInt status( KErrNone );
    TUid myUid = TUid::Uid( KTestUid );
    TCapabilitySet caps;
    caps.SetEmpty();
   
    RCommandList  commands;
    CleanupClosePushL(commands);
    for ( TInt index = KMinListId; index <KMaxListId; index++ )
        {
         MediatorService::TCommand newCommand;
         newCommand.iCommandId = index;
         newCommand.iVersion = iDefaultVersion;
         newCommand.iCaps = caps;
         newCommand.iTimeout = KDefaultTimeOut;
         commands.AppendL( newCommand );
        }
    
    status = iCommandResponder->RegisterCommand( myUid,
                                                 myUid, 
                                                 commands );
    
    _LIT( KDescription, "RegisterCommand returns with" );
    aResult.SetResult( status, KDescription );
    
    if( KErrNone == status  )
        {
        status = CleanRegisterCommands( myUid, 
                                       myUid, 
                                       commands );
        if( KErrNone != status )
            {
            _LIT( KDescription, "Test passed but cleaning failed" );
            aResult.SetResult( status, KDescription );     
            }
        }
    CleanupStack::PopAndDestroy( &commands );
    return KErrNone;
    }

TInt CMediatorTest::CleanRegisterCommands( TUid aDomain, 
                                           TUid aCategory, 
                                           RCommandList aCommands )
    {
   
    TInt status = iCommandResponder->UnregisterCommand( aDomain, 
                                                        aCategory, 
                                                        aCommands );
    CTestScheduler::StartAndStopL( 2000 ); // wait 1s                                                   
    return status;
    }

// -----------------------------------------------------------------------------
// CMediatorTest::RegisterOver500CommandsWithinOneCat
//
// Expected preconditions: Command does not exist
// Expected behaviour: Register command succeed
// Cleaning: Command unregistered
//
// IDs: KMaxListId - (KMaxListId + 540)
// -----------------------------------------------------------------------------
//

TInt CMediatorTest::RegisterOver500CommandsWithinOneCatL( TTestResult& aResult )
    {
    TInt status( KErrNone );
    TUid myUid = TUid::Uid( KTestUid );
    TCapabilitySet caps;
    caps.SetEmpty();

    RCommandList  commands;
    CleanupClosePushL(commands);
    
    for ( TInt index = KMaxListId; index <KMaxListId+540; index++ )
        {
        MediatorService::TCommand newCommand;
        newCommand.iCommandId = index;
        newCommand.iVersion = iDefaultVersion;
        newCommand.iCaps = caps;
        newCommand.iTimeout = KDefaultTimeOut;
        User::LeaveIfError( commands.Append( newCommand ) );
        }
    
    status = iCommandResponder->RegisterCommand( myUid,
                                                 myUid, 
                                                 commands );

    _LIT( KDescription, "RegisterCommand returns with" );
    aResult.SetResult( status, KDescription );
    
    if( KErrNone == status  )
       {
       status = CleanRegisterCommands( myUid, 
                                      myUid, 
                                      commands );
       if( KErrNone != status )
           {
           _LIT( KDescription, "Test passed but cleaning failed" );
           aResult.SetResult( status, KDescription );     
           }
       }
    CleanupStack::PopAndDestroy( &commands );
    return KErrNone;
    }


// -----------------------------------------------------------------------------
// CMediatorTest::RegisterCommandWithDomainUID0
// 
// Expected preconditions: Command does not exist
// Expected behaviour: Register command succeed
// Cleaning: Command unregistered
//  
// Register command
// Domain: 0 
// -----------------------------------------------------------------------------
//

TInt CMediatorTest::RegisterCommandWithDomainUID0( TTestResult& aResult )
    {
    TInt status( KErrNone );
    TUid invalidUid = TUid::Uid(0);
    TUid myUid = TUid::Uid( KTestUid );
    TCapabilitySet caps;
    caps.SetEmpty();

    status = iCommandResponder->RegisterCommand( myUid, 
                                                 invalidUid, 
                                                 KDefaultCommandId, 
                                                 iDefaultVersion,
                                                 caps, 
                                                 KDefaultTimeOut );
    
    _LIT( KErrorDescription, "RegisterCommand returns with" );
    aResult.SetResult( status, KErrorDescription );

    if( KErrNone == status  )
        {
        status = CleanRegisterCommand( myUid, invalidUid, KDefaultCommandId );
        if( KErrNone != status )
            {
            _LIT( KDescription, "Test passed but cleaning failed" );
            aResult.SetResult( status, KDescription );
            }
        } 
    return KErrNone;    
    }


// -----------------------------------------------------------------------------
// CMediatorTest::RegisterCommandWithCategoryUID0
// 
// Expected preconditions: Command does not exist
// Expected behaviour: Register command fail
// Cleaning: Not needed
//  
// Register command with category 0
// -----------------------------------------------------------------------------
//

TInt CMediatorTest::RegisterCommandWithCategoryUID0( TTestResult& aResult )
    {
    TInt status( KErrNone );
    TUid myUid = TUid::Uid( KTestUid );
    TUid invalidUid = TUid::Uid(0);
    TCapabilitySet caps;
    caps.SetEmpty();
    
    status = iCommandResponder->RegisterCommand( myUid, 
                                                 invalidUid, 
                                                 KDefaultCommandId, 
                                                 iDefaultVersion,
                                                 caps, 
                                                 KDefaultTimeOut );
    
    _LIT( KErrorDescription, "RegisterCommand returns with" );
    aResult.SetResult( status, KErrorDescription );

    if( KErrNone == status  )
        {
        status = CleanRegisterCommand( myUid, invalidUid, KDefaultCommandId );
        if( KErrNone != status )
            {
            _LIT( KDescription, "Test passed but cleaning failed" );
            aResult.SetResult( status, KDescription );
            }
        }
    else
        {
        _LIT( KErrorDescription, "Wrong error status" );
        aResult.SetResult( status, KErrorDescription );
        }
    return KErrNone;    
    }


// -----------------------------------------------------------------------------
// CMediatorTest::RegisterCommandWithCommandID0
// 
// Expected preconditions: Command does not exist
// Expected behaviour: Register command succeed
// Cleaning: Unregister command
//  
// REgister command with command ID 0
// -----------------------------------------------------------------------------
//

TInt CMediatorTest::RegisterCommandWithCommandID0( TTestResult& aResult )
    {
    TInt status( KErrNone );
    TUid myUid = TUid::Uid( KTestUid );
    TCapabilitySet caps;
    caps.SetEmpty();

    status = iCommandResponder->RegisterCommand( myUid, 
                                                 myUid, 
                                                 0, 
                                                 iDefaultVersion,
                                                 caps, 
                                                 KDefaultTimeOut );
                                                 
    _LIT( KErrorDescription, "RegisterCommand returns with" );
    aResult.SetResult( status, KErrorDescription );

    if( KErrNone == status  )
        {
        status = CleanRegisterCommand( myUid, myUid, 0 );
        if( KErrNone != status )
            {
            _LIT( KDescription, "Test passed but cleaning failed" );
            aResult.SetResult( status, KDescription );
            }
        }
    return KErrNone;    
    }


// -----------------------------------------------------------------------------
// CMediatorTest::RegisterCommandWithTimeoutValue
// 
// Expected preconditions: Command does not exist
// Expected behaviour: Register command succeed
// Cleaning: Unregister command
//  
// Register command with time out 0
// -----------------------------------------------------------------------------
//

TInt CMediatorTest::RegisterCommandWithTimeoutValue0( TTestResult& aResult )
    {
    TInt status( KErrNone );
    TUid myUid = TUid::Uid( KTestUid );
    TCapabilitySet caps;
    caps.SetEmpty();

    status = iCommandResponder->RegisterCommand( myUid, 
                                                 myUid, 
                                                 KSecondaryCommandId, 
                                                 iDefaultVersion,
                                                 caps, 
                                                 0 ); // time out
    _LIT( KErrorDescription, "RegisterCommand returns with" );
    aResult.SetResult( status, KErrorDescription );

    if( KErrNone == status  )
        {
        status = CleanRegisterCommand( myUid, myUid, KSecondaryCommandId );
        if( KErrNone != status )
            {
            _LIT( KDescription, "Test passed but cleaning failed" );
            aResult.SetResult( status, KDescription );
            }
        }
    return KErrNone;    
    }


// -----------------------------------------------------------------------------
// CMediatorTest::IssueNonExistingCommandWrongDomain
// 
// Expected preconditions: Command domain does not exist
// Expected behaviour: Issue command fail
// Cleaning: Not needed
//  
// Try to issue command
// Domain not valid
// -----------------------------------------------------------------------------
//

TInt CMediatorTest::IssueNonExistingCommandWrongDomainL( TTestResult& aResult )
    {
    TInt status( KErrNone );
    TUid myUid = TUid::Uid( KTestUid );
    TUid invalidUid = TUid::Uid( KNonExistingDomainId );

    _LIT8(KTest, "TestTestTest");
    HBufC8* data = KTest().AllocLC();
    
    status = iCommandInitiator->IssueCommand( invalidUid, 
                                              myUid, 
                                              KDefaultCommandId,
                                              iDefaultVersion, 
                                              *data);
    CleanupStack::PopAndDestroy( data );
    //expected status is KErrMediatorDomainNotFound
    if( KErrMediatorDomainNotFound == status )
        {
          _LIT( KDescription, "IssueCommand returns with expected KErrMediatorDomainNotFound" );
          aResult.SetResult( KErrNone, KDescription );        
        }
    else
        {
        _LIT( KErrorDescription, "Wrong error status" );
        aResult.SetResult( KErrGeneral, KErrorDescription );
        }
    CTestScheduler::StartAndStopL( 5000 ); // wait 5s                                                                                    
    return KErrNone;
    }


// -----------------------------------------------------------------------------
// CMediatorTest::IssueNonExistingCommandWrongCat
// 
// Expected preconditions: Command category does not exist
// Expected behaviour: Issue command fail
// Cleaning: Not needed
//  
// Try to issue command
// Category not valid
// -----------------------------------------------------------------------------
//

TInt CMediatorTest::IssueNonExistingCommandWrongCatL( TTestResult& aResult )
    {
    TInt status( KErrNone );
    TUid myUid = TUid::Uid( KTestUid );
    TUid invalidUid = TUid::Uid( KNonExistingCategoryId );

    _LIT8(KTest, "TestTestTest");
    HBufC8* data = KTest().AllocLC();
    
    status = iCommandInitiator->IssueCommand( myUid, 
                                              invalidUid, 
                                              KDefaultCommandId,
                                              iDefaultVersion, 
                                              *data);
    CleanupStack::PopAndDestroy( data );
    //expected status is KErrMediatorCategoryNotFound
    if( KErrMediatorCategoryNotFound == status )
    {
      _LIT( KDescription, "IssueCommand returns with expected KErrMediatorCategoryNotFound" );
        aResult.SetResult( KErrNone, KDescription );        
    }
    else
    {
        _LIT( KErrorDescription, "Wrong error status" );
        aResult.SetResult( KErrGeneral, KErrorDescription );
    }
    CTestScheduler::StartAndStopL( 2000 ); // wait 2s                                                                                    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CMediatorTest::IssueNonExistingCommandId
// 
// Expected preconditions: Register a command and issue a command with invalid command id
// Expected behaviour: Issue command fail
// Cleaning: Unregister command
//  
// Try to issue command
// Command ID not valid
// -----------------------------------------------------------------------------
//

TInt CMediatorTest::IssueNonExistingCommandIdL( TTestResult& aResult )
    {
    TInt status( KErrNone );
    TUid myUid = TUid::Uid( KTestUid );
    TCapabilitySet caps;
    caps.SetEmpty();
    
    _LIT8(KTest, "TestTestTest");
    HBufC8* data = KTest().AllocLC();
    
    status = PreconditionRegisterCommand( myUid, 
                                          myUid, 
                                          KDefaultCommandId,
                                          iDefaultVersion,
                                          caps,
                                          KDefaultTimeOut );
        
    _LIT( KErrorDescription, "PreconditionRegisterCommand returns with" );
    aResult.SetResult( status, KErrorDescription );   
    if( KErrNone == status  )
        {
        status = iCommandInitiator->IssueCommand( myUid, 
                                                  myUid, 
                                                  KInvalidCommandId,//command does not exist
                                                  iDefaultVersion, 
                                                  *data );
        
        //expected status is KErrMediatorCommandNotFound                                        
        if( KErrMediatorCommandNotFound == status )
            {
            _LIT( KDescription, "IssueCommand returns with expected KErrMediatorCommandNotFound" );
            aResult.SetResult( KErrNone, KDescription );
            status = CleanRegisterCommand( myUid, 
                                           myUid, 
                                           KDefaultCommandId );
            if( KErrNone != status )
                {
                _LIT( KDescription, "Test Passed but cleaning failed" );
                aResult.SetResult( status, KDescription );
                }
            }
        else
            {
            _LIT( KErrorDescription, "Wrong error status" );
            aResult.SetResult( status, KErrorDescription );
            }
        }
    CleanupStack::PopAndDestroy( data );
    CTestScheduler::StartAndStopL( 2000 ); // wait 3s 
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CMediatorTest::IssueCommandWrongCommandVerMajor
// 
// Expected preconditions: Command version does not exist
// Expected behaviour: Issue command fail
// Cleaning: Unregister command
//  
// Try to issue command
// Wrong version
// -----------------------------------------------------------------------------
//

TInt CMediatorTest::IssueCommandWrongCommandVerMajorL( TTestResult& aResult )
    {
    TInt status( KErrNone );
    TUid myUid = TUid::Uid( KTestUid );
    TVersion version( 666,5,5);
    TCapabilitySet caps;
    caps.SetEmpty();
        
    _LIT8(KTest, "TestTestTest");
    HBufC8* data = KTest().AllocLC();
    
    TInt preStatus = PreconditionRegisterCommand( myUid, 
                                                  myUid, 
                                                  KDefaultCommandId,
                                                  iDefaultVersion,
                                                  caps,
                                                  1000 );
    _LIT( KErrorDescription, "PreconditionRegisterCommand returns with" );
    aResult.SetResult( preStatus, KErrorDescription );    
    if( KErrNone == preStatus )
        {
        status = iCommandInitiator->IssueCommand( myUid, 
                                                  myUid, 
                                                  KDefaultCommandId,
                                                  version, 
                                                  *data );
      
        CTestScheduler::StartAndStopL(2000); // wait 2s
        //expected status is KErrMediatorVersionMismatch
        if( KErrMediatorVersionMismatch == status )
            {
            _LIT( KDescription, "IssueCommand returns with expected KErrMediatorVersionMismatch" );
            aResult.SetResult( KErrNone, KDescription );
            status = CleanRegisterCommand( myUid, 
                                           myUid, 
                                           KDefaultCommandId );
            if( KErrNone != status )
                {
                _LIT( KDescription, "Test Passed but cleaning failed" );
                aResult.SetResult( status, KDescription );        
                }
            }
        else
            {
            _LIT( KErrorDescription, "Wrong error status" );
            aResult.SetResult( KErrGeneral, KErrorDescription );
            }
        }
    CleanupStack::PopAndDestroy( data ); 
    return KErrNone;
    }

TInt CMediatorTest::PreconditionRegisterCommand( TUid aDomain, 
                                                 TUid aCategory, 
                                                 TInt aCommandId, 
                                                 TVersion aVersion,
                                                 TCapabilitySet aCaps, 
                                                 TInt aTimeOut )
    {
    TInt status = iCommandResponder->RegisterCommand( aDomain, 
                                                      aCategory, 
                                                      aCommandId, 
                                                      aVersion,
                                                      aCaps, 
                                                      aTimeOut );
    CTestScheduler::StartAndStopL(1000); // wait 1s  
    return status;  
    }

TInt CMediatorTest::PreconditionRegisterCommands( TUid aDomain,
                                                  TUid aCategory, 
                                                  RCommandList aCommands )
    {
    TInt status = iCommandResponder->RegisterCommand( aDomain, 
                                                      aCategory, 
                                                      aCommands );
    CTestScheduler::StartAndStopL(1000); // wait 1s
    return status;  
    }


// -----------------------------------------------------------------------------
// CMediatorTest::IssueCommand
//
// Issue command
// -----------------------------------------------------------------------------
//

TInt CMediatorTest::IssueCommandL( TTestResult& aResult )
    {
    TInt status( KErrNone );
    TUid myUid = TUid::Uid( KTestUid );
    TCapabilitySet caps;
    caps.SetEmpty();
        
    _LIT8(KTest, "TestTestTest");
    HBufC8* data = KTest().AllocLC();
    
    iTestHelper->ClearAllNotifyData( );

    TInt preStatus = PreconditionRegisterCommand( myUid, 
                                                  myUid, 
                                                  KDefaultCommandId,
                                                  iDefaultVersion,
                                                  caps,
                                                  KDefaultTimeOut );
    
    _LIT( KErrorDescription, "PreconditionRegisterCommand returns with" );
    aResult.SetResult( preStatus, KErrorDescription );    
    
    if( KErrNone == preStatus  )
        {
        status = iCommandInitiator->IssueCommand( myUid, 
                                                  myUid, 
                                                  KDefaultCommandId,
                                                  iDefaultVersion, 
                                                  *data );
        CTestScheduler::StartAndStopL(2000); // wait 2s
         
        
        _LIT( KDescription, "IssueCommand returns with" );
        aResult.SetResult( status, KDescription );    
        
        if( KErrNone == status  )
            {
            _LIT8(KTestR, "Command response here");
            HBufC8* replyData = KTestR().AllocLC();
            status = iCommandResponder->IssueResponse( iTestHelper->iDomain, 
                                                       iTestHelper->iCategory, 
                                                       iTestHelper->iCommandId, 
                                                       KErrNone,
                                                       *replyData );                                      
            if( KErrNone == status  )
                {
                status = CleanRegisterCommand( myUid, 
                                               myUid, 
                                               KDefaultCommandId );
                if( KErrNone != status )
                    {
                    _LIT( KDescription, "Test Passed but cleaning failed" );
                    aResult.SetResult( status, KDescription );        
                    }
                }
            _LIT( KErrorDescription, "IssueResponse returns with" );
            aResult.SetResult( status, KErrorDescription );
            CleanupStack::PopAndDestroy( replyData );
            }
        }
    CleanupStack::PopAndDestroy( data );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CMediatorTest::IssueCommand2
//
// Issue command from here. 
// Command receiver implemented in plugin. Reply is expected
// -----------------------------------------------------------------------------
//
TInt CMediatorTest::IssueCommand2L( TTestResult& aResult )
    {
    TInt status( KErrNone );
    TUid myUid = TUid::Uid(0x999101);
    TVersion version( 1,0,0);
    
    _LIT8(KTest, "TestTestTest");
    HBufC8* data = KTest().AllocLC();
    
    CMediatorCommandInitiator *commandInitiator = CMediatorCommandInitiator::NewL( iTestHelper );
    CleanupStack::PushL( commandInitiator );
    TInt invalidCmd = 1;
    commandInitiator->CancelCommand( myUid,
                                     myUid, 
                                     invalidCmd );
    
    status = commandInitiator->IssueCommand( myUid, 
                                            myUid, 
                                            invalidCmd,
                                            version, 
                                            *data );
    
    CTestScheduler::StartAndStopL(2000); // wait 2s
        
    // Check that reply was received
    if( ( iTestHelper->iNotificationReceived == ECommandResponse ) && ( KErrNone == status  ) )
        {
        _LIT( KDescription, "IssueCommand gets notification" );
        aResult.SetResult( status, KDescription );      
        }
    else
        {
        _LIT( KErrorDescription, "Error" );
        aResult.SetResult( KErrGeneral, KErrorDescription );
        }
    iTestHelper->ClearAllNotifyData( );
    commandInitiator->CancelCommand( myUid,
                                     myUid, 
                                     invalidCmd );
    
    CleanupStack::PopAndDestroy( commandInitiator );
    CleanupStack::PopAndDestroy( data );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CMediatorTest::IssueExistingCommandSameInitiatorWhileProcessing
// Issue command twice with same command initiator
// Expected preconditions: None
// Expected behaviour: KErrInUse
// Cleaning: Unregister command
// -----------------------------------------------------------------------------


TInt CMediatorTest::IssueExistingCommandSameInitiatorWhileProcessingL( TTestResult& aResult )
    {
    TInt status( KErrNone );
    TUid myUid = TUid::Uid( KTestUid );
    TCapabilitySet caps;
    caps.SetEmpty();
        
    iTestHelper->ClearAllNotifyData();
    
    _LIT8(KTest, "TestTestTest");
    HBufC8* data = KTest().AllocLC();
 
    TInt preStatus = PreconditionRegisterCommand( myUid, 
                                                  myUid, 
                                                  KMinListId,
                                                  iDefaultVersion,
                                                  caps,
                                                  5000 );
    _LIT( KErrorDescription, "PreconditionRegisterCommand returns with" );
    aResult.SetResult( preStatus, KErrorDescription );    
    if( KErrNone == preStatus )
        {    
        status = iCommandInitiator->IssueCommand( myUid, 
                                                  myUid, 
                                                  KMinListId,
                                                  iDefaultVersion, 
                                                  *data );
        CTestScheduler::StartAndStopL( 2000 ); // wait 2s
        if( KErrNone == status  )
            {
            status = iCommandInitiator->IssueCommand( myUid, 
                                                      myUid, 
                                                      KMinListId,
                                                      iDefaultVersion, 
                                                      *data );
       
            if ( KErrInUse == status )
                {
                _LIT( KErrorDescription, "IssueCommand returns with expected KErrInUse " );
                aResult.SetResult( KErrNone, KErrorDescription );
                status = CleanRegisterCommand( myUid, 
                                               myUid, 
                                               KMinListId );
                CTestScheduler::StartAndStopL( 2000 );
                if( KErrNone != status )
                    {
                    _LIT( KDescription, "Test code passed but cleaning failed" );
                    aResult.SetResult( status, KDescription );        
                    }       
                }   
            else
                {
                _LIT( KErrorDescription, "IssueCommand returns with" );
                aResult.SetResult( status, KErrorDescription );
                }
            }
        }
    CleanupStack::PopAndDestroy( data );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CMediatorTest::IssueCommandWaitWithoutRespondingToCommand
// Issue command and wait for some time and check the notification
// Expected preconditions: None
// Expected behaviour: 
// Cleaning: Unregister command
// -----------------------------------------------------------------------------
//

TInt CMediatorTest::IssueCommandWaitWithoutRespondingToCommandL( TTestResult& aResult )
    {
    TInt status( KErrNone );
    TUid myUid = TUid::Uid( KTestUid );
    
    _LIT8(KTest, "TestTestTest");
    HBufC8* data = KTest().AllocLC();
    TCapabilitySet caps;
    caps.SetEmpty();
        
    iTestHelper->ClearAllNotifyData();
    
    TInt preStatus = PreconditionRegisterCommand( myUid, 
                                                  myUid, 
                                                  KMinListId+1,
                                                  iDefaultVersion,
                                                  caps,
                                                  5000 );
    _LIT( KErrorDescription, "PreconditionRegisterCommand returns with" );
    aResult.SetResult( preStatus, KErrorDescription );    
    if( KErrNone == preStatus )
        {    
        status = iCommandInitiator->IssueCommand( myUid, 
                                                  myUid, 
                                                  KMinListId+1,
                                                  iDefaultVersion, 
                                                  *data );
           
        CTestScheduler::StartAndStopL(2000); // wait 2s
        _LIT( KErrorDescription, "IssueCommand returns with" );
        aResult.SetResult( status, KErrorDescription );
        /* Check that notify was received, only when RaiseEvent returns KErrNone*/
        if( ( KErrNone == status  )
            &&( ( iTestHelper->iNotificationReceived == 0 ) 
                    || ( iTestHelper->iDomain != myUid )
                    || ( iTestHelper->iCategory != myUid )
                    || ( iTestHelper->iCommandId != KMinListId+1 ) ) )
            {
            status = KErrNotFound; 
            _LIT( KDescription, "iTestHelper checks failed" );
            aResult.SetResult( status, KDescription );  
            }
        //clean the registered command
        else if ( KErrNone == status  )
            {
            status = CleanRegisterCommand( myUid, 
                                           myUid, 
                                           KMinListId+1 );
            if( KErrNone != status )
                {
                _LIT( KErrorDescription, "Test passed but cleaning command failed" );
                aResult.SetResult( status, KErrorDescription );
                }   
            }   
        }
    CleanupStack::PopAndDestroy( data );
    return KErrNone;
}


// -----------------------------------------------------------------------------
// CMediatorTest::IssueCommandCancelFromInitiatorBeforeTimeout
// Issue command and cancel it before timeout.
// Expected preconditions: register command with specific time
// Cleaning: Unregister command
// -----------------------------------------------------------------------------
//

TInt CMediatorTest::IssueCommandCancelFromInitiatorBeforeTimeoutL( TTestResult& aResult )
    {
    TInt status( KErrNone );
    TUid myUid = TUid::Uid( KTestUid );
    iTestHelper->ClearAllNotifyData();
    _LIT8(KTest, "TestTestTest");
    HBufC8* data = KTest().AllocLC();
    TCapabilitySet caps;
    caps.SetEmpty();
    
    TInt preStatus = PreconditionRegisterCommand( myUid, 
                                                  myUid, 
                                                  KMinListId+2,
                                                  iDefaultVersion,
                                                  caps,
                                                  5000 );
    _LIT( KErrorDescription, "PreconditionRegisterCommand returns with" );
    aResult.SetResult( preStatus, KErrorDescription );    
    if( KErrNone == preStatus )
        {    
        status = iCommandInitiator->IssueCommand( myUid, 
                                                  myUid, 
                                                  KMinListId+2,
                                                  iDefaultVersion, 
                                                  *data );
        _LIT( KErrorDescription, "IssueCommand returns with" );
        aResult.SetResult( status, KErrorDescription );
        if( KErrNone == status  )
            {
            iCommandInitiator->CancelCommand( myUid, 
                                              myUid, 
                                              KMinListId+2 );
        
            CTestScheduler::StartAndStopL( 2000 ); // wait 2s
        
            status = CleanRegisterCommand(myUid, 
                                    myUid, 
                                    KMinListId+2);
      
            if( KErrNone != status )
                {
                _LIT( KDescription, "Test passed but cleaning failed" );
                aResult.SetResult( status, KDescription );        
                }
            }
        }
    CleanupStack::PopAndDestroy( data );
    return KErrNone;
    }


// -----------------------------------------------------------------------------
// CMediatorTest::IssueCommandRespondCorrectlyFromResponder
// Issue command, get the correct reposonse from command responder
// Cleaning: Unregister command
// -----------------------------------------------------------------------------
//

TInt CMediatorTest::IssueCommandRespondCorrectlyFromResponderL( TTestResult& aResult )
    {
    TInt status( KErrNone );
    TUid myUid = TUid::Uid( KTestUid );
    iTestHelper->ClearAllNotifyData();
    _LIT8(KTest, "TestTestTest");
    HBufC8* data = KTest().AllocLC();
    TCapabilitySet caps;
    caps.SetEmpty();
    
    TInt preStatus = PreconditionRegisterCommand( myUid, 
                                                  myUid, 
                                                  KDefaultCommandId,
                                                  iDefaultVersion,
                                                  caps,
                                                  5000 );
                                                  
    _LIT( KErrorDescription, "PreconditionRegisterCommand returns with" );
    aResult.SetResult( preStatus, KErrorDescription );
        
    if( KErrNone == preStatus )
        {    
    
        status = iCommandInitiator->IssueCommand( myUid, 
                                                  myUid, 
                                                  KDefaultCommandId,
                                                  iDefaultVersion, 
                                                  *data );
        
        CTestScheduler::StartAndStopL(1000);
        _LIT( KDescription, "IssueCommand returns with" );
        aResult.SetResult( status, KDescription );
        if( KErrNone == status  )
            {
            _LIT8(KTestR, "Command response here");
            HBufC8* replyData = KTestR().AllocLC();
            status = iCommandResponder->IssueResponse( iTestHelper->iDomain, 
                                                       iTestHelper->iCategory, 
                                                       iTestHelper->iCommandId, 
                                                       KErrNone,
                                                       *replyData );                                      
            CleanupStack::PopAndDestroy( replyData );      
        
            _LIT( KErrorDescription, "IssueResponse returns with" );
            aResult.SetResult( status, KErrorDescription );
        
            status = CleanRegisterCommand( myUid, 
                                           myUid, 
                                           KDefaultCommandId );
            CTestScheduler::StartAndStopL(1000); // wait 1s
            if( KErrNone != status )
                {
                _LIT( KDescription, "Test passed but cleaning failed" );
                aResult.SetResult( status, KDescription );        
                }                   
            }
        }
    CleanupStack::PopAndDestroy( data );
    return KErrNone;
    }


// -----------------------------------------------------------------------------
// CMediatorTest::IssueCommandRespondIncorrectlyFromResponderL
// Issue command and get the wrong reposonse from command responder
// Expected preconditions: IssueResponse with wrong category id
// Expected behaviour: KErrMediatorCommandNotFound
// Cleaning: Unregister command
// -----------------------------------------------------------------------------


TInt CMediatorTest::IssueCommandRespondIncorrectlyFromResponderL( TTestResult& aResult )
    {
    TInt status( KErrNone );
    TUid myUid = TUid::Uid( KTestUid );
    TUid invalidUid = TUid::Uid(KNonExistingCategoryId);

    _LIT8(KTest, "TestTestTest");
    HBufC8* data = KTest().AllocLC();
    TCapabilitySet caps;
    caps.SetEmpty();
    
    TInt preStatus = PreconditionRegisterCommand( myUid, 
                                                  myUid, 
                                                  KDefaultCommandId,
                                                  iDefaultVersion,
                                                  caps,
                                                  5000 );
                                                  
    _LIT( KErrorDescription, "PreconditionRegisterCommand returns with" );
    aResult.SetResult( preStatus, KErrorDescription );
        
    if( KErrNone == preStatus )
        {    
        status = iCommandInitiator->IssueCommand( myUid, 
                                                  myUid, 
                                                  KDefaultCommandId,
                                                  iDefaultVersion, 
                                                  *data );
                
        CTestScheduler::StartAndStopL(1000); // wait 1s
        _LIT( KDescription, "IssueCommand returns with" );
        aResult.SetResult( status, KDescription );
        if( KErrNone == status  )
            {
            _LIT8(KTestR, "Command response here");
            HBufC8* replyData = KTestR().AllocLC();
            status = iCommandResponder->IssueResponse( iTestHelper->iDomain, 
                                                       invalidUid,  // This is is incorrect value in reply
                                                       iTestHelper->iCommandId, 
                                                       KErrNone,
                                                       *replyData );                                      
            CleanupStack::PopAndDestroy( replyData );      
                              
            if( KErrMediatorCommandNotFound != status  )
                {
                _LIT( KErrorDescription, "Wrong error code" );
                aResult.SetResult( KErrGeneral, KErrorDescription );        
                }
            else
                {
                _LIT( KErrorDescription, "IssueResponse returns with" );
                aResult.SetResult( KErrNone, KDescription );   
                status = CleanRegisterCommand( myUid, 
                                               myUid, 
                                               KDefaultCommandId );
                CTestScheduler::StartAndStopL(1000); // wait 1s
                if( KErrNone != status )
                    {
                    _LIT( KDescription, "Test passed but cleaning failed" );
                    aResult.SetResult( status, KDescription );        
                    }
                }
            }
        }
    CleanupStack::PopAndDestroy( data );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CMediatorTest::IssueCommandALotOfParameterData
// Issue command with big amount of data
// Expected preconditions: IssueCommand with big data buffer
// Expected behaviour: IssueCommand returns with KErrNone
// Cleaning: Unregister command
// -----------------------------------------------------------------------------
//

TInt CMediatorTest::IssueCommandALotOfParameterDataL( TTestResult& aResult )
    {
    TInt status( KErrNone );
    TUid myUid = TUid::Uid( KTestUid );
    
    HBufC8* data = HBufC8::NewLC( KDataSize );
    TPtr8 dataPtr = data->Des();
    for( TInt i = 0; i < KDataSize; i++ )
        {
        TChar tmpChar( 64 );
        dataPtr.Append( tmpChar );
        }

    TCapabilitySet caps;
    caps.SetEmpty();
    
    TInt preStatus = PreconditionRegisterCommand( myUid, 
                                                  myUid, 
                                                  KDefaultCommandId,
                                                  iDefaultVersion,
                                                  caps,
                                                  5000 );
                                                  
    _LIT( KErrorDescription, "PreconditionRegisterCommand returns with" );
    aResult.SetResult( preStatus, KErrorDescription );
        
    if( KErrNone == preStatus )
        {    
    
        status = iCommandInitiator->IssueCommand( myUid, 
                                                  myUid, 
                                                  KDefaultCommandId,
                                                  iDefaultVersion, 
                                                  *data );
        
        CTestScheduler::StartAndStopL(2000); // wait 2s
        _LIT( KErrorDescription, "IssueCommand returns with" );
        aResult.SetResult( status, KErrorDescription );
        /* Check that notify was received, */
        if( ( KErrNone == status  )
            &&( ( iTestHelper->iNotificationReceived == 0 ) 
                    || ( iTestHelper->iDomain != myUid )
                    || ( iTestHelper->iCategory != myUid )
                    || ( iTestHelper->iCommandId != KDefaultCommandId ) ) )
            {
            status = KErrNotFound; 
            _LIT( KDescription, "iTestHelper checks failed" );
            aResult.SetResult( status, KDescription );  
            }
        //clean the registered command
        else if ( KErrNone == status  )
            {
            status = CleanRegisterCommand( myUid, 
                                           myUid, 
                                           KDefaultCommandId );
            if( KErrNone != status )
                {
                _LIT( KErrorDescription, "Test passed but cleaning command failed" );
                aResult.SetResult( status, KErrorDescription );
                }   
            } 
        }
    CleanupStack::PopAndDestroy( data  );
    return KErrNone;
    }


// -----------------------------------------------------------------------------
// CMediatorTest::IssueCommandRespondALotOfResponseData
// IssueResponse with big amount of data
// Expected preconditions: IssueResponse with big data buffer
// Expected behaviour: IssueResponse returns with KErrNone
// Cleaning: Unregister command
// -----------------------------------------------------------------------------
//

TInt CMediatorTest::IssueCommandRespondALotOfResponseDataL( TTestResult& aResult )
{
    TInt status( KErrNone );
    TUid myUid = TUid::Uid( KTestUid );

    _LIT8(KTest, "Replywithalotofdata");
    HBufC8* data = KTest().AllocLC();
    TCapabilitySet caps;
    caps.SetEmpty();
    
    iTestHelper->ClearAllNotifyData();
    TInt preStatus = PreconditionRegisterCommand( myUid, 
                                                  myUid, 
                                                  KDefaultCommandId,
                                                  iDefaultVersion,
                                                  caps,
                                                  5000 );
                                                  
    _LIT( KErrorDescription, "PreconditionRegisterCommand returns with" );
    aResult.SetResult( preStatus, KErrorDescription );
        
    if( KErrNone == preStatus )
        {        
        status = iCommandInitiator->IssueCommand( myUid, 
                                                  myUid, 
                                                  KDefaultCommandId,
                                                  iDefaultVersion, 
                                                  *data );
        CTestScheduler::StartAndStopL( 2000 );
        _LIT( KDescription, "IssueCommand returns with" );
        aResult.SetResult( status, KDescription );
        HBufC8* rData = HBufC8::NewLC( KDataSize );
        TPtr8 dataPtr = rData->Des();
        for( TInt i = 0; i < KDataSize; i++ )
            {
            TChar tmpChar(64);
            dataPtr.Append( tmpChar );
            }
        
        status = iCommandResponder->IssueResponse( iTestHelper->iDomain, 
                                                   iTestHelper->iCategory, 
                                                   iTestHelper->iCommandId, 
                                                   KErrNone,
                                                   *rData );    
        CleanupStack::PopAndDestroy( rData );      
                      
        if( KErrNone != status )
            {
            _LIT( KErrorDescription, "Wrong error code" );
            aResult.SetResult( KErrGeneral, KErrorDescription );        
            }
        else
            {
            _LIT( KErrorDescription, "IssueResponse returns with" );
            aResult.SetResult( KErrNone, KDescription );   
            status = CleanRegisterCommand( myUid, 
                                           myUid, 
                                           KDefaultCommandId );
            CTestScheduler::StartAndStopL(1000); // wait 1s
            if( KErrNone != status )
                {
                _LIT( KDescription, "Test passed but cleaning failed" );
                aResult.SetResult( status, KDescription );        
                }
            }
        }
    CleanupStack::PopAndDestroy( data );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CMediatorTest::GetListOfDomainsNoDomains
//
// Get list of domains
// Expected: 0 domains
// -----------------------------------------------------------------------------
//

TInt CMediatorTest::GetListOfDomainsNoDomainsL( TTestResult& aResult )
    {
    // There shall not be any domains when executing this test
    RDomainList domainList;
    CleanupClosePushL( domainList );
    TInt status = iNotifications->GetDomains( domainList );
    
    if( KErrNone == status  )
        {
        if( domainList.Count() == 0 )
            {
            _LIT( KErrorDescription, "GetDomains returns with" );
            aResult.SetResult( status, KErrorDescription );           
            }
        else
            {
            _LIT( KErrorDescription, "Registered domains found" );
            aResult.SetResult( KErrGeneral, KErrorDescription );
            }
        }
    else
        {
        _LIT( KErrorDescription, "GetDomains returns with" );
        aResult.SetResult( status, KErrorDescription );        
        }
    CleanupStack::PopAndDestroy( &domainList );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CMediatorTest::GetListOfDomainsInit
//
// Get list of domains
// Expected: Unknown count of domains
// -----------------------------------------------------------------------------
//

TInt CMediatorTest::GetListOfDomainsInitL( TTestResult& aResult )
    {
    RDomainList domainList;
    CleanupClosePushL( domainList );
    TInt status = iNotifications->GetDomains( domainList );
    
    if( KErrNone == status  )
        {
        initCountOfDomain = domainList.Count();
        _LIT( KErrorDescription, "GetDomains returns with" );
        aResult.SetResult( status, KErrorDescription );
        }
    else
        {
        _LIT( KErrorDescription, "GetDomains returns with" );
        aResult.SetResult( status, KErrorDescription );        
        }
    CleanupStack::PopAndDestroy( &domainList );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CMediatorTest::GetListOfDomains1Domain
//
// Get list of domains
// Expected 1 domain (+already existing)
// -----------------------------------------------------------------------------
//

TInt CMediatorTest::GetListOfDomains1DomainL( TTestResult& aResult )
    {
    // There shall be one domain when executing this test
    RDomainList domainList;
    CleanupClosePushL( domainList );
    TInt status = iNotifications->GetDomains( domainList );
    
    if( KErrNone == status  )
        {
        if( domainList.Count() == initCountOfDomain )
            {
            _LIT( KErrorDescription, "GetDomains returns with" );
            aResult.SetResult( status, KErrorDescription );           
            }
        else
            {
            _LIT( KErrorDescription, "Wrong amount of domains found" );
            aResult.SetResult( KErrGeneral, KErrorDescription );
            }
        }
    else
        {
        _LIT( KErrorDescription, "GetDomains returns with" );
        aResult.SetResult( status, KErrorDescription );        
        }
    CleanupStack::PopAndDestroy( &domainList );
    return KErrNone;
    }


// -----------------------------------------------------------------------------
// CMediatorTest::GetListOfDomainsMultipleDomains
//
// Get list of domains
// 
// -----------------------------------------------------------------------------
//

TInt CMediatorTest::GetListOfDomainsMultipleDomainsL( TTestResult& aResult )
    {
    TInt status( KErrNone );
    TUid myUid = TUid::Uid( KTestUid );
    TCapabilitySet caps;
    caps.SetEmpty();
    TUid domain;   
    
    REventList eventList;
    CleanupClosePushL(eventList);
    
    for(TInt i=50; i<100; i++) // i = Event ID
        {
        TEvent newEvent;
        newEvent.iEventId = i;
        newEvent.iVersion = iDefaultVersion;
        newEvent.iCaps = caps;  
        eventList.AppendL(newEvent);
        }
    
    for( TInt j=KMinDomainID; j<KMidDomainID; j++ ) // j = domain
        {
        domain = TUid::Uid(j);
        status = iEventProvider->RegisterEvent( domain, 
                                                myUid,
                                                eventList );
        if( KErrNone != status )
            {
            _LIT( KErrorDescription, "Register event failure" );
            aResult.SetResult( status, KErrorDescription );       
            break;
            }
        }        
    // There shall be multiple domain when executing this test
    if ( KErrNone == status  )
        {
        RDomainList domainList;
        CleanupClosePushL(domainList);
        status = iNotifications->GetDomains( domainList );
        
        _LIT( KErrorDescription, "GetDomains returns with" );
        if( KErrNone == status  )
            {
            if(domainList.Count() > 1 )
                {
               aResult.SetResult( status, KErrorDescription );           
                }
            else
                {
                _LIT( KErrorDescription, "Wrong amount of domains found" );
                aResult.SetResult( KErrGeneral, KErrorDescription );
                }
            }
        else
            {
            aResult.SetResult( status, KErrorDescription );        
            }
        CleanupStack::PopAndDestroy( &domainList );
        }
    status = CleanRegisterMultipleEvents(domain, myUid, eventList);
    if( KErrNone != status )
        {
        _LIT( KErrorDescription, "Cleaning failed" );
        aResult.SetResult( status, KErrorDescription );
        }
    CleanupStack::PopAndDestroy( &eventList );
    return KErrNone;
    }


// -----------------------------------------------------------------------------
// CMediatorTest::GetListOfDomainsOver500Domains
// 
// Get list of domains
// -----------------------------------------------------------------------------
//

TInt CMediatorTest::GetListOfDomainsOver500DomainsL( TTestResult& aResult )
    {
    TInt status( KErrNone );
    TUid myUid = TUid::Uid( KTestUid );
    TCapabilitySet caps;
    caps.SetEmpty();
    TUid domain;
    REventList eventList;
    CleanupClosePushL( eventList );                    
    for( TInt i=50; i<100; i++ ) // i = Event ID
        {
        TEvent newEvent;
        newEvent.iEventId = i;
        newEvent.iVersion = iDefaultVersion;
        newEvent.iCaps = caps;  
        eventList.AppendL(newEvent);
        }

    for( TInt j=KMidDomainID; j<KMaxDomainID; j++ ) // j = domain
        {
        domain = TUid::Uid(j);
        status = iEventProvider->RegisterEvent( domain, 
                                                myUid,
                                                eventList );
        if( KErrNone != status )
            {
            _LIT( KErrorDescription, "Register event failure" );
            aResult.SetResult( status, KErrorDescription );       
            break;
            }
        }         
    if( KErrNone == status  )
        {
 
        // There shall be over 500 domain when executing this test
        RDomainList domainList;
        CleanupClosePushL( domainList );
        status = iNotifications->GetDomains( domainList );
      
        if(KErrNone == status )
          {
          if(domainList.Count() > 500 )
              {
              _LIT( KErrorDescription, "GetDomains returns with" );
              aResult.SetResult( status, KErrorDescription );           
              }
          else
              {
              _LIT( KErrorDescription, "Wrong amount of domains found" );
              aResult.SetResult( KErrGeneral, KErrorDescription );
              }
          }
        else
          {
          _LIT( KErrorDescription, "GetDomains returns with" );
          aResult.SetResult( status, KErrorDescription );        
          }

        for( TInt j=KMidDomainID; j<KMaxDomainID; j++ ) // j = domain
            {      
            domain = TUid::Uid(j); 
            status = CleanRegisterMultipleEvents(domain, myUid, eventList);
            if(KErrNone != status)
                {
                _LIT( KErrorDescription, "Test code passed but cleaning failed" );
                aResult.SetResult( status, KErrorDescription );
                }
            }
        CleanupStack::PopAndDestroy( &domainList );
        }
    CleanupStack::PopAndDestroy( &eventList );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CMediatorTest::GetListOfCategoriesNoCategories
// 
// Get list of categories
// -----------------------------------------------------------------------------
//
TInt CMediatorTest::GetListOfCategoriesNoCategoriesL( TTestResult& aResult )
    {
    // There shall not be categories in domain
    RCategoryList categoryList;
    CleanupClosePushL( categoryList );
    TUid myUid = TUid::Uid( KTestUid );
    iTestHelper->ClearAllNotifyData();
    TInt status = iNotifications->GetCategories( myUid, categoryList );
    
    if( KErrNone == status  )
        {
        if( categoryList.Count() == 0 )
            {
            _LIT( KErrorDescription, "GetCategories returns with" );
            aResult.SetResult( KErrNone, KErrorDescription );           
            }
        else
            {
            _LIT( KErrorDescription, "Wrong number of categories found" );
            aResult.SetResult( KErrGeneral, KErrorDescription );
            }
        }
    else
        {
        _LIT( KErrorDescription, "GetCategories returns with" );
        aResult.SetResult( status, KErrorDescription );        
        }
    CleanupStack::PopAndDestroy( &categoryList );
    return KErrNone;
    }


// -----------------------------------------------------------------------------
// CMediatorTest::GetListOfCategoriesIncorrectDomainUI
// 
// Get list of categories
// Domain ID not valid
// -----------------------------------------------------------------------------
//

TInt CMediatorTest::GetListOfCategoriesIncorrectDomainUIDL( TTestResult& aResult )
    {
    // There shall not be categories in domain
    RCategoryList categoryList;
    CleanupClosePushL( categoryList );
    TUid myUid = TUid::Uid(KNonExistingDomainId);
    
    TInt status = iNotifications->GetCategories( myUid, categoryList );
    
    if( KErrMediatorDomainNotFound == status )
        {
       _LIT( KErrorDescription, "GetCategories returns with" );
       aResult.SetResult( KErrNone, KErrorDescription );           
        }
    else
        {
       _LIT( KErrorDescription, "Wrong error status received" );
       aResult.SetResult( KErrGeneral, KErrorDescription );        
        }
    CleanupStack::PopAndDestroy( &categoryList );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CMediatorTest::GetListOfCategoriesMultipleCategories
// 
// Get list of categories
// -----------------------------------------------------------------------------
//

TInt CMediatorTest::GetListOfCategoriesMultipleCategoriesL( TTestResult& aResult )
{
    // There shall be more than one category in domain
    RCategoryList categoryList;
    CleanupClosePushL( categoryList );
    TUid myUid = TUid::Uid( KTestUid );
    TUid my2ndUid = TUid::Uid(KTestUid+1);
    TUid my3rdUid = TUid::Uid(KTestUid+2);
    
    TCapabilitySet caps;
    caps.SetEmpty();
        
    TInt preStatus = PreconditionRegisterOneEvent( myUid, myUid, KTestEventId, iDefaultVersion, caps);
    preStatus = PreconditionRegisterOneEvent( myUid, my2ndUid, KTestEventId, iDefaultVersion, caps);
    preStatus = PreconditionRegisterOneEvent( myUid, my3rdUid, KTestEventId, iDefaultVersion, caps);
    
    _LIT( KErrorDescription, "PreconditionRegisterOneEvent returns with" );
    aResult.SetResult( preStatus, KErrorDescription );  
    
    if( KErrNone == preStatus )
        {
        TInt status = iNotifications->GetCategories( myUid, categoryList );
        if( KErrNone == status  )
            {
            if( categoryList.Count() > 1 )
                {
                _LIT( KErrorDescription, "GetCategories returns with" );
                aResult.SetResult( status, KErrorDescription );           
                }
            else
                {
                _LIT( KErrorDescription, "Wrong amount of categories found" );
                aResult.SetResult( KErrGeneral, KErrorDescription );
                }
            }
        else
            {
            _LIT( KErrorDescription, "GetCategories returns with" );
            aResult.SetResult( status, KErrorDescription );        
            }
    
        status = CleanRegisterOneEvent( myUid, myUid, KTestEventId);
        status = CleanRegisterOneEvent( myUid, my2ndUid, KTestEventId);
        status = CleanRegisterOneEvent( myUid, my3rdUid, KTestEventId);
        if( KErrNone != status )
            {
            _LIT( KErrorDescription, "Test code passed but cleaning failed" );
            aResult.SetResult( status, KErrorDescription );
            }
        }
    CleanupStack::PopAndDestroy( &categoryList );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CMediatorTest::GetListOfCommands
// 
// Get list of commands
// -----------------------------------------------------------------------------
//

TInt CMediatorTest::GetListOfCommandsL( TTestResult& aResult )
    {
    //There may not be any commands in this category
    RCommandList commandList;
    CleanupClosePushL( commandList );
    TUid myUid = TUid::Uid( KTestUid );
    TUid otherUid = TUid::Uid(KOtherUid);
    TCapabilitySet caps;
    caps.SetEmpty();
        
    TInt preStatus = PreconditionRegisterOneEvent( myUid, otherUid, KTestEventId, iDefaultVersion, caps);
    _LIT( KErrorDescription, "PreconditionRegisterOneEvent returns with" );
    aResult.SetResult( preStatus, KErrorDescription );
    
    if( KErrNone == preStatus )
        {    
        TInt status = iNotifications->GetCommands( myUid, otherUid, commandList  );
        if( KErrNone == status  )
            {
            if( commandList.Count() == 0 )
                {
                _LIT( KErrorDescription, "GetCommands returns with" );
                aResult.SetResult( status, KErrorDescription );           
                }
            else
                {
                _LIT( KErrorDescription, "Wrong amount of categories found" );
                aResult.SetResult( KErrGeneral, KErrorDescription );
                }
            }
        else
            {
            _LIT( KErrorDescription, "GetCommands returns with" );
            aResult.SetResult( status, KErrorDescription );        
            }
        
        status = CleanRegisterOneEvent( myUid, otherUid, KTestEventId );
        if(KErrNone != status)
            {
            _LIT( KErrorDescription, "Cleaning event failed" );
            aResult.SetResult( status, KErrorDescription );
            }           
        }
    CleanupStack::PopAndDestroy( &commandList );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CMediatorTest::GetListOfCommandsIncorrectDomainUID
// 
// Get list of commands
// -----------------------------------------------------------------------------
//

TInt CMediatorTest::GetListOfCommandsIncorrectDomainUIDL( TTestResult& aResult )
    {
    //There shall not be this domain   
    RCommandList commandList;
    CleanupClosePushL( commandList );
    TUid myUid = TUid::Uid(KReallyNonExistingDomainId);
    
    TInt status = iNotifications->GetCommands( myUid, myUid, commandList );
    //expected status is KErrMediatorDomainNotFound
    if( KErrMediatorDomainNotFound == status )
        {
        _LIT( KErrorDescription, "GetCommands returns with expected KErrMediatorDomainNotFound" );
        aResult.SetResult( KErrNone, KErrorDescription );           
        }
    else
        {
        _LIT( KErrorDescription, "Wrong error code" );
        aResult.SetResult( KErrGeneral, KErrorDescription );        
        }
    CleanupStack::PopAndDestroy( &commandList );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CMediatorTest::GetListOfCommandsIncorrectCategoryUID
// 
// Get list of commands
// -----------------------------------------------------------------------------
//

TInt CMediatorTest::GetListOfCommandsIncorrectCategoryUIDL( TTestResult& aResult )
    {
    //There shall not be this category   
    RCommandList commandList;
    CleanupClosePushL( commandList );
    TUid myUid = TUid::Uid( KTestUid );
    TUid invalidUid = TUid::Uid(KReallyNonExistingDomainId);
    
    TInt status = iNotifications->GetCommands( myUid, invalidUid, commandList  );
    //expected status is KErrMediatorCategoryNotFound
    if( KErrMediatorCategoryNotFound == status )
        {
        _LIT( KErrorDescription, "GetCommands returns with expected KErrMediatorCategoryNotFound" );
        aResult.SetResult( KErrNone, KErrorDescription );           
        }
    else
        {
        _LIT( KErrorDescription, "Wrong error code" );
        aResult.SetResult( KErrGeneral, KErrorDescription );        
        }
    CleanupStack::PopAndDestroy( &commandList );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CMediatorTest::GetListOfCommands1Command
// 
// Get list of commands
// -----------------------------------------------------------------------------
//

TInt CMediatorTest::GetListOfCommands1CommandL( TTestResult& aResult )
    {
    RCommandList commandList;
    CleanupClosePushL( commandList );
    TUid myUid = TUid::Uid(KMinDomainID);
    TInt status( KErrNone );
    TCapabilitySet caps;
    caps.SetEmpty();

    status = iCommandResponder->RegisterCommand( myUid, 
                                                 myUid, 
                                                 KDefaultCommandId, 
                                                 iDefaultVersion,
                                                 caps, 
                                                 KDefaultTimeOut );
    _LIT( KErrorDescription, "RegisterCommand returns with" );
    aResult.SetResult( status, KErrorDescription );
    if( KErrNone == status  )
        {
        status = iNotifications->GetCommands( myUid, myUid, commandList  );
        if(KErrNone == status )
            {
            if(commandList.Count() == 1 )
                {
                _LIT( KErrorDescription, "GetCommands returns with" );
                aResult.SetResult( status, KErrorDescription );           
                }
            else
                {
                _LIT( KErrorDescription, "Wrong amount of commands found" );
                aResult.SetResult( KErrGeneral, KErrorDescription );
                }
            }
        else
            {
            _LIT( KErrorDescription, "GetCommands returns with" );
            aResult.SetResult( status, KErrorDescription );        
            }
        }
    CleanupStack::PopAndDestroy( &commandList );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CMediatorTest::GetListOfCommandsMultipleCommands
// 
// Get list of commands
// -----------------------------------------------------------------------------
//

TInt CMediatorTest::GetListOfCommandsMultipleCommandsL( TTestResult& aResult )
    {
    RCommandList commandList;
    CleanupClosePushL(commandList);
    TUid myUid = TUid::Uid( KTestUid );

    TCapabilitySet caps;
    caps.SetEmpty();
   
    RCommandList  commands;
    CleanupClosePushL(commands);
    for ( TInt index = KMinListId; index <KMaxListId; index++ )
        {
        MediatorService::TCommand newCommand;
        newCommand.iCommandId = index;
        newCommand.iVersion = iDefaultVersion;
        newCommand.iCaps = caps;
        newCommand.iTimeout = KDefaultTimeOut;
        User::LeaveIfError( commands.Append(newCommand) );
        }
    
    TInt preStatus = PreconditionRegisterCommands( myUid,
                                                   myUid, 
                                                   commands );
    _LIT( KDescription, "PreconditionRegisterCommands returns with" );
    aResult.SetResult( preStatus, KDescription );

    if( KErrNone == preStatus )
        {
        TInt status = iNotifications->GetCommands( myUid, myUid, commandList );
        if( KErrNone == status  )
            {
            if( commandList.Count() > 1 )
                {
                _LIT( KErrorDescription, "GetCommands returns with" );
                aResult.SetResult( status, KErrorDescription );           
                }
            else
                {
                _LIT( KErrorDescription, "Wrong amount of commands found" );
                aResult.SetResult( KErrGeneral, KErrorDescription );
                }
            }
      else
          {
          _LIT( KErrorDescription, "GetCommands returns with" );
          aResult.SetResult( status, KErrorDescription );        
          }
      status = CleanRegisterCommands( myUid, myUid, commands );
      if( KErrNone != status )
          {
          _LIT( KDescription, "Cleaning failed" );
          aResult.SetResult( status, KDescription );
          }      
        }
    CleanupStack::PopAndDestroy(&commands);
    CleanupStack::PopAndDestroy(&commandList);
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CMediatorTest::GetListOfCommandsOver500Commands
// 
// Get list of commands
// -----------------------------------------------------------------------------
//

TInt CMediatorTest::GetListOfCommandsOver500CommandsL( TTestResult& aResult )
    {
    RCommandList commandList;
    CleanupClosePushL( commandList );
    TUid myUid = TUid::Uid( KTestUid );
    TCapabilitySet caps;
    caps.SetEmpty();
   
    RCommandList  commands;
    CleanupClosePushL( commands );
    for ( TInt index = KMinListId; index <KMinListId+525; index++ )
        {
        MediatorService::TCommand newCommand;
        newCommand.iCommandId = index;
        newCommand.iVersion = iDefaultVersion;
        newCommand.iCaps = caps;
        newCommand.iTimeout = KDefaultTimeOut;
        User::LeaveIfError( commandList.Append(newCommand) );
        }
    
    TInt preStatus = PreconditionRegisterCommands( myUid,
                                                   myUid, 
                                                   commandList );
    _LIT( KDescription, "PreconditionRegisterCommands returns with" );
    aResult.SetResult( preStatus, KDescription );

    if( KErrNone == preStatus )
        {
        TInt status = iNotifications->GetCommands( myUid, myUid, commands );
        if( KErrNone == status  )
            {
            if( commands.Count() > 500 )
                {
                _LIT( KErrorDescription, "GetCommands returns with" );
                aResult.SetResult( status, KErrorDescription );           
                }
            else
                {
                _LIT( KErrorDescription, "Wrong amount of commands found" );
                aResult.SetResult( KErrGeneral, KErrorDescription );
                }
            }
        else
            {
            _LIT( KErrorDescription, "GetCommands returns with" );
            aResult.SetResult( status, KErrorDescription );        
            }
        status = CleanRegisterCommands( myUid, myUid, commandList );
        if( KErrNone != status )
            {
            _LIT( KDescription, "Cleaning failed" );
            aResult.SetResult( status, KDescription );
            }
        }
    CleanupStack::PopAndDestroy( &commands );
    CleanupStack::PopAndDestroy( &commandList );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CMediatorTest::GetListOfEventsNoEventsInCategory
// 
// Get list of events
// -----------------------------------------------------------------------------
//

TInt CMediatorTest::GetListOfEventsNoEventsInCategoryL( TTestResult& aResult )
    {
    REventList eventList;
    REventList eventListB;
    CleanupClosePushL(eventList);
    CleanupClosePushL(eventListB);
    
    TUid myUid = TUid::Uid( KTestUid );

    TCapabilitySet caps;
    caps.SetEmpty();
   
    RCommandList  commands;
    CleanupClosePushL(commands);
    
    TInt status = iNotifications->GetEvents( myUid, myUid, eventListB ); // get events
    status = CleanRegisterMultipleEvents(myUid, myUid, eventListB); // clean those events
    
    for ( TInt index = KMinListId; index <KMinListId+10; index++ )
        {
        MediatorService::TCommand newCommand;
        newCommand.iCommandId = index;
        newCommand.iVersion = iDefaultVersion;
        newCommand.iCaps = caps;
        newCommand.iTimeout = KDefaultTimeOut;
        User::LeaveIfError( commands.Append(newCommand) );
        }
    
    TInt preStatus = PreconditionRegisterCommands( myUid,
                                                   myUid, 
                                                   commands ); // register some commands
    _LIT( KDescription, "PreconditionRegisterCommands returns with" );
    aResult.SetResult( preStatus, KDescription );

    if( KErrNone == preStatus )
        {
        TInt status = iNotifications->GetEvents( myUid, myUid, eventList );
      
        if( KErrNone == status  )
            {
            if( eventList.Count() == 0 )
                {
                _LIT( KErrorDescription, "GetEvents returns with" );
                aResult.SetResult( status, KErrorDescription );
                status = CleanRegisterCommands( myUid, myUid, commands );
                if(KErrNone != status)
                    {
                    _LIT( KDescription, "Test code passed but cleaning failed" );
                    aResult.SetResult( status, KDescription );
                    } 
                }
            else
                {
                _LIT( KErrorDescription, "Wrong amount of commands found" );
                aResult.SetResult( KErrGeneral, KErrorDescription );
                }
            }
        else
            {
            _LIT( KErrorDescription, "GetEvents returns with" );
            aResult.SetResult( status, KErrorDescription );        
            }
        status = CleanRegisterMultipleEvents( myUid, myUid, eventListB ); // clean those events
        if( KErrNone != status )
            {
            _LIT( KDescription, "Cleaning failed" );
            aResult.SetResult( status, KDescription );
            }      
        }      
    CleanupStack::PopAndDestroy( &commands );
    CleanupStack::PopAndDestroy( &eventListB );
    CleanupStack::PopAndDestroy( &eventList );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CMediatorTest::GetListOfEventsWithIncorrectDomainUID
//
// Get list of events
// 
// -----------------------------------------------------------------------------
//

TInt CMediatorTest::GetListOfEventsWithIncorrectDomainUIDL( TTestResult& aResult )
    {
    REventList eventList;
    CleanupClosePushL( eventList );
    TUid invalidUid = TUid::Uid(KNonExistingDomainId);
    TUid myUid = TUid::Uid( KTestUid );
    
    TInt status = iNotifications->GetEvents( invalidUid, myUid, eventList );
    //expected status is KErrMediatorDomainNotFound
    if( KErrMediatorDomainNotFound == status )
        {
        _LIT( KErrorDescription, "GetEvents returns with expected KErrMediatorDomainNotFound" );
        aResult.SetResult( KErrNone, KErrorDescription );           
        }
    else
        {
        _LIT( KErrorDescription, "Wrong error code" );
        aResult.SetResult( KErrGeneral, KErrorDescription );        
        }    
    CleanupStack::PopAndDestroy( &eventList );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CMediatorTest::GetListOfEventsWithIncorrectCategoryUID
//
// Get list of events
// 
// -----------------------------------------------------------------------------
//

TInt CMediatorTest::GetListOfEventsWithIncorrectCategoryUIDL( TTestResult& aResult )
    {
    REventList eventList;
    CleanupClosePushL( eventList );
    TUid invalidUid = TUid::Uid(KNonExistingDomainId);
    TUid myUid = TUid::Uid( KTestUid );
    
    TInt status = iNotifications->GetEvents( myUid, invalidUid, eventList );
    //expected status is KErrMediatorCategoryNotFound
    if( KErrMediatorCategoryNotFound == status )
        {
        _LIT( KErrorDescription, "GetEvents returns with expected KErrMediatorCategoryNotFound" );
        aResult.SetResult( KErrNone, KErrorDescription );           
        }
    else
        {
        _LIT( KErrorDescription, "Wrong error code" );
        aResult.SetResult( KErrGeneral, KErrorDescription );        
        }    
    CleanupStack::PopAndDestroy( &eventList );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CMediatorTest::GetListOfEvents1EventInCategory
// 
// Get list of events
// -----------------------------------------------------------------------------
//

TInt CMediatorTest::GetListOfEvents1EventInCategoryL( TTestResult& aResult )
    {
    REventList eventList;
    CleanupClosePushL( eventList );
    TUid myUid = TUid::Uid(KTestUid+6);
    TCapabilitySet caps;
    caps.SetEmpty();
        
    TInt preStatus = PreconditionRegisterOneEvent( myUid, myUid, KTestEventId, iDefaultVersion, caps);
    _LIT( KErrorDescription, "PreconditionRegisterOneEvent returns with" );
    aResult.SetResult( preStatus, KErrorDescription );
    
    if( KErrNone == preStatus )    
        {
        TInt status = iNotifications->GetEvents( myUid, myUid, eventList );
        _LIT( KErrorDescription, "GetEvents returns with" );
        if( KErrNone == status  )
            {
            if( eventList.Count() == 1 )
                {
                aResult.SetResult( status, KErrorDescription );           
                }
            else
                {
                _LIT( KErrorDescription, "Wrong amount of commands found" );
                aResult.SetResult( KErrGeneral, KErrorDescription );
                }
            }
        else
            {
            aResult.SetResult( status, KErrorDescription );        
            }
      
        TInt cleanStatus = CleanRegisterOneEvent(myUid, myUid, KTestEventId); 
        if( KErrNone != cleanStatus)
            {
            _LIT( KDescription, "Cleaning failed" );
            aResult.SetResult( cleanStatus, KDescription );     
            }
        else
            {
            aResult.SetResult( cleanStatus, KErrorDescription );
            }
        }
    CleanupStack::PopAndDestroy( &eventList );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CMediatorTest::GetListOfEventsMultipleEventsInCategory
//
// Get list of events
// 
// -----------------------------------------------------------------------------
//

TInt CMediatorTest::GetListOfEventsMultipleEventsInCategoryL( TTestResult& aResult )
    {
    REventList eventList;
    CleanupClosePushL(eventList);
    TUid myUid = TUid::Uid( KTestUid );

    TCapabilitySet caps;
    caps.SetEmpty();
    REventList eventList2;
    CleanupClosePushL(eventList2);
    
    for( TInt i=80; i<91; i++ ) // Create list of events (i = event ID)
        {
        TEvent newEvent;
        newEvent.iEventId = i;
        newEvent.iVersion = iDefaultVersion;
        newEvent.iCaps = caps;    
        eventList.AppendL(newEvent);
        }
    
    TInt preStatus = PreconditionRegisterEvents(myUid, myUid, eventList);
    _LIT( KDescription, "PreconditionRegisterEvents returns with" );
    aResult.SetResult( preStatus, KDescription );
    
    if( KErrNone == preStatus )
        {
        TInt status = iNotifications->GetEvents( myUid, myUid, eventList2 );
      
        if( KErrNone == status )
          {
          if( eventList2.Count() > 1 )
              {
              _LIT( KErrorDescription, "GetEvents returns with" );
              aResult.SetResult( status, KErrorDescription );           
              }
          else
              {
              _LIT( KErrorDescription, "Wrong amount of events found" );
              aResult.SetResult( KErrGeneral, KErrorDescription );
              }
          }
        else
          {
          _LIT( KErrorDescription, "GetEvents returns with" );
          aResult.SetResult( status, KErrorDescription );        
          }
        status = CleanRegisterMultipleEvents(myUid, myUid, eventList); // clean those events
        if( KErrNone != status )
          {
          _LIT( KDescription, "Test passed but cleaning failed" );
          aResult.SetResult( status, KDescription );
          }
        }
    CleanupStack::PopAndDestroy(&eventList2);
    CleanupStack::PopAndDestroy(&eventList);
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CMediatorTest::GetListOfEventsOver500EventsInCategory
//
// Get list of events
// 
// -----------------------------------------------------------------------------
//
TInt CMediatorTest::GetListOfEventsOver500EventsInCategoryL( TTestResult& aResult )
    {
    REventList eventList;
    CleanupClosePushL( eventList );
    TUid myUid = TUid::Uid( KTestUid+5 );
    TCapabilitySet caps;
    caps.SetEmpty();
    REventList eventList2;
    CleanupClosePushL( eventList2 );
                        
    for( TInt i=80; i<600; i++) // Create list of events (i = event ID)
        {
        TEvent newEvent;
        newEvent.iEventId = i;
        newEvent.iVersion = iDefaultVersion;
        newEvent.iCaps = caps;    
        eventList.AppendL( newEvent );
        }
    
    TInt preStatus = PreconditionRegisterEvents(myUid, myUid, eventList);
    _LIT( KDescription, "PreconditionRegisterEvents returns with" );
    aResult.SetResult( preStatus, KDescription );
    
    if( KErrNone == preStatus )
        {
        TInt status = iNotifications->GetEvents( myUid, myUid, eventList2 );      
        if( KErrNone == status  )
            {
            if( eventList2.Count() ==  eventList.Count() )
                {
                _LIT( KErrorDescription, "GetEvents returns with" );
                aResult.SetResult( status, KErrorDescription );
                status = CleanRegisterMultipleEvents( myUid, myUid, eventList);
                if( KErrNone != status )
                    {
                    _LIT( KDescription, "Cleaning failed" );
                    aResult.SetResult( status, KDescription );
                    }
                }
            else
                {
                 _LIT( KErrorDescription, "Wrong amount of commands found" );
                 aResult.SetResult( KErrGeneral, KErrorDescription );
                }
            }
        else
            {
            _LIT( KErrorDescription, "GetEvents returns with" );
            aResult.SetResult( status, KErrorDescription );        
            }    
        }
        CleanupStack::PopAndDestroy( &eventList2 );
        CleanupStack::PopAndDestroy( &eventList );
        return KErrNone;
    }

// -----------------------------------------------------------------------------
// CMediatorTest::SubscribeNotificationReceiver
//
// Subscribe notification receiver
// KESKEN
// -----------------------------------------------------------------------------
//

TInt CMediatorTest::SubscribeNotificationReceiver( TTestResult& aResult )
    {
    TInt status( KErrNone );
    iTestHelper->ClearAllNotifyData();
    status = iNotifications->RegisterNotificationObserver( iTestHelper );
    CTestScheduler::StartAndStopL( 2000 );
    _LIT( KErrorDescription, "RegisterNotificationObserver returns with" );
    aResult.SetResult( status, KErrorDescription );  
    iTestHelper->ClearAllNotifyData();
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CMediatorTest::ReSubscribeNotificationReceiver
// ReSubscribe notification receiver
// KESKEN
// -----------------------------------------------------------------------------
//

TInt CMediatorTest::ReSubscribeNotificationReceiver( TTestResult& aResult )
    {
    TInt status( KErrNone );
    status = iNotifications->RegisterNotificationObserver( iTestHelper );
    
    //expected status is KErrAlreadyExists
    if( KErrAlreadyExists == status )
        {
        _LIT( KErrorDescription, "RegisterNotificationObserver returns with expected KErrAlreadyExists" );
        aResult.SetResult( KErrNone, KErrorDescription );        
        }
    else
        {
        _LIT( KErrorDescription, "Wrong error code" );
        aResult.SetResult( KErrGeneral, KErrorDescription );
        }        
    return KErrNone;
    }
// -----------------------------------------------------------------------------
// CMediatorTest::UnsubscribeNotificationReceiver
// unsubscribe notification receiver
// KESKEN
// -----------------------------------------------------------------------------
//

TInt CMediatorTest::UnsubscribeNotificationReceiver( TTestResult& aResult )
    {
    TInt status( KErrNone );
    status = iNotifications->UnregisterNotificationObserver();
    _LIT( KErrorDescription, "UnsubscribeNotificationReceiver returns with" );
    aResult.SetResult( status, KErrorDescription );        
    return KErrNone;
    }
// -----------------------------------------------------------------------------
// CMediatorTest::ReUnsubscribeNotificationReceiver
// ReUnsubscribe notification receiver
// KESKEN
// -----------------------------------------------------------------------------
//
TInt CMediatorTest::ReUnsubscribeNotificationReceiver( TTestResult& aResult )
    {
    TInt status( KErrNone );
    status = iNotifications->UnregisterNotificationObserver();
    
    //expected return is KErrNotFound
    if( KErrNotFound == status )
        {
        status = KErrNone;
        }
    _LIT( KErrorDescription, "UnregisterNotificationObserver returns with" );
    aResult.SetResult( status, KErrorDescription );
    CTestScheduler::StartAndStopL( 2000 );
    return KErrNone;
    }
// -----------------------------------------------------------------------------
// CMediatorTest::SubscribeNotificationReceiverRegisterCommands
// SubscribeNotificationReceiver and Register Commands
// KESKEN
// -----------------------------------------------------------------------------
//
TInt CMediatorTest::SubscribeNotificationReceiverRegisterCommandsL( TTestResult& aResult )
    {
    TInt status( KErrNone );
    TUid myUid = TUid::Uid( KTestUid ); 
    TCapabilitySet caps;
    caps.SetEmpty();

    iTestHelper->ClearAllNotifyData();
    status = iNotifications->RegisterNotificationObserver(iTestHelper);
    CTestScheduler::StartAndStopL( 2000 );
    _LIT( KDescription, "RegisterNotificationObserver returns with" );
    aResult.SetResult( status, KDescription );
    if( KErrNone == status  )
        {
        status = iCommandResponder->RegisterCommand( myUid, 
                                                 myUid, 
                                                 KMaxListId+1000,
                                                 iDefaultVersion,
                                                 caps, 
                                                 KDefaultTimeOut );
    
        CTestScheduler::StartAndStopL( 2000 );
        _LIT( KDescription, "RegisterCommand returns with" );
        aResult.SetResult( status, KDescription );
        if ( iTestHelper->iNotificationReceived == EClear )
            {
            status = KErrNotFound;
            _LIT( KDescription, "iTestHelper check failed" );
            aResult.SetResult( status, KDescription );
            }
        }
    iNotifications->UnregisterNotificationObserver();
    // let request for canceling notications to complete
    CTestScheduler::StartAndStopL( 2000 );
    return KErrNone;    
    }
// -----------------------------------------------------------------------------
// CMediatorTest::SubscribeNotificationReceiverUnregCommands
// SubscribeNotificationReceiver and unegister Commands
// KESKEN
// -----------------------------------------------------------------------------
//
TInt CMediatorTest::SubscribeNotificationReceiverUnregCommandsL( TTestResult& aResult )
    {
    TInt status( KErrNone );
    TUid myUid = TUid::Uid( KTestUid ); 
    TCapabilitySet caps;
    caps.SetEmpty();

    RCommandList  commands;
    CleanupClosePushL(commands);
    
    for ( TInt index = 1; index <KMaxListId; index++ )
        {
        MediatorService::TCommand newCommand;
        newCommand.iCommandId = index;
        newCommand.iVersion = iDefaultVersion;
        newCommand.iCaps = caps;
        newCommand.iTimeout = KDefaultTimeOut;
        User::LeaveIfError( commands.Append(newCommand) );
        }

    status = iCommandResponder->UnregisterCommand( myUid, 
                                                   myUid,
                                                   commands );
    
     _LIT( KDescription, "UnregisterCommand returns with" );
    aResult.SetResult( status, KDescription );
    CleanupStack::PopAndDestroy( &commands );
    return KErrNone;    
    }
// -----------------------------------------------------------------------------
// CMediatorTest::SubscribeNotificationReceiverUnregCommandsSoThatCatIsEmpty
// SubscribeNotificationReceiver and unegister commands 
// KESKEN
// -----------------------------------------------------------------------------
//
TInt CMediatorTest::SubscribeNotificationReceiverUnregCommandsSoThatCatIsEmptyL( TTestResult& aResult )
    {
    TInt status( KErrNone );
    TUid myUid = TUid::Uid( KTestUid ); 
    TCapabilitySet caps;
    caps.SetEmpty();
    RCommandList  commandList;
    CleanupClosePushL( commandList );
    
    // First clean commands if any exist
    status = iNotifications->GetCommands( myUid, myUid, commandList );
    status = CleanRegisterCommands( myUid, myUid, commandList );
    
    iTestHelper->ClearAllNotifyData();
    status = iNotifications->RegisterNotificationObserver( iTestHelper );
    _LIT( KDescription, "RegisterNotificationObserver returns with" );
    aResult.SetResult( status, KDescription );
    CTestScheduler::StartAndStopL( 2000 );
    if( KErrNone == status  )
        {
        RCommandList  commands;
        CleanupClosePushL(commands);
        
        for ( TInt index = 1; index <KMaxListId; index++ )
            {
            MediatorService::TCommand newCommand;
            newCommand.iCommandId = index;
            newCommand.iVersion = iDefaultVersion;
            newCommand.iCaps = caps;
            newCommand.iTimeout = KDefaultTimeOut;
            User::LeaveIfError( commands.Append(newCommand) );
            }
    
        TInt preStatus = PreconditionRegisterCommands( myUid,
                                                       myUid, 
                                                       commands );
        _LIT( KDescription, "PreconditionRegisterCommands register with" );
        aResult.SetResult( preStatus, KDescription );
    
        if( KErrNone == preStatus )
            {
            status = iCommandResponder->UnregisterCommand( myUid, 
                                                           myUid,
                                                           commands );
            CTestScheduler::StartAndStopL( 2000 );
            _LIT( KDescription, "UnregisterCommand returns with" );
            aResult.SetResult( status, KDescription );
            } 
        iNotifications->UnregisterNotificationObserver();
        // let request for canceling notications to complete
        CTestScheduler::StartAndStopL(2000);
        CleanupStack::PopAndDestroy(&commands);    
        }
    CleanupStack::PopAndDestroy(&commandList);
    return KErrNone;    
    }

// -----------------------------------------------------------------------------
// CMediatorTest::SubscribeNotificationReceiverRegisterEvents
// SubscribeNotificationReceiver and Register Events
// KESKEN
// -----------------------------------------------------------------------------
//
TInt CMediatorTest::SubscribeNotificationReceiverRegisterEventsL( TTestResult& aResult )
    {
    TInt status( KErrNone );
    TUid myUid = TUid::Uid( KTestUid );
    TCapabilitySet caps;
    caps.SetEmpty();

    status = iEventProvider->RegisterEvent( myUid, 
                                            myUid,
                                            KTestEventId+10002,
                                            iDefaultVersion,
                                            caps ) ;        
    CTestScheduler::StartAndStopL(2000);
    _LIT( KDescription, "RegisterEvent returns with" );
    aResult.SetResult( status, KDescription ); 
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CMediatorTest::SubscribeNotificationReceiverUnregisterEvents
// SubscribeNotificationReceiver and unegister Events
// KESKEN
// -----------------------------------------------------------------------------
//
TInt CMediatorTest::SubscribeNotificationReceiverUnregisterEventsL( TTestResult& aResult )
    {
    TInt status( KErrNone );
    TUid myUid = TUid::Uid( KTestUid );
    TCapabilitySet caps;
    caps.SetEmpty();
    
    iTestHelper->ClearAllNotifyData();
    status = iNotifications->RegisterNotificationObserver(iTestHelper);
    CTestScheduler::StartAndStopL( 2000 );
    _LIT( KDescription, "RegisterNotificationObserver returns with" );
    aResult.SetResult( status, KDescription );
    if( KErrNone == status  )
        {
        REventList eventList;
        CleanupClosePushL( eventList );
        REventList eventList2;
        CleanupClosePushL( eventList2 );
        //clean the allready registered event
        TInt preStatus = iNotifications->GetEvents( myUid, myUid, eventList );  
        if( KErrNone == preStatus )
            {
            preStatus = CleanRegisterMultipleEvents(myUid, myUid, eventList);
            }
          
        for( TInt i=76; i<85; i++ ) // Create list of events (i = event ID)
            {
            TEvent newEvent;
            newEvent.iEventId = i;
            newEvent.iVersion = iDefaultVersion;
            newEvent.iCaps = caps;    
            eventList2.AppendL( newEvent );
            }
        preStatus = PreconditionRegisterEvents(myUid, myUid, eventList2);
        _LIT( KDescription, "PreconditionRegisterEvents returns with" );
        aResult.SetResult( preStatus, KDescription );
    
        if( KErrNone == preStatus )
            {
            status = iEventProvider->UnregisterEvent( myUid, 
                                                      myUid,
                                                      eventList2 );
            CTestScheduler::StartAndStopL( 3000 );
            _LIT( KDescription, "UnregisterEvent returns with" );
            aResult.SetResult( status, KDescription );
            
            if( iTestHelper->iNotificationReceived == 0 )
                {
                status = KErrNotFound;
                _LIT( KDescription, "iTestHelper check failed" );
                aResult.SetResult( status, KDescription );
                }
            }
        status = iNotifications->UnregisterNotificationObserver();
        // let request for canceling notications to complete
        CTestScheduler::StartAndStopL( 2000 ); 
        CleanupStack::PopAndDestroy( &eventList2 );
        CleanupStack::PopAndDestroy( &eventList );
        }
    return KErrNone;
    }
// -----------------------------------------------------------------------------
// CMediatorTest::SubscribeNotificationReceiverUnregisterEventsSoThatCatIsEmpty
// SubscribeNotificationReceiver and unegister Events
// KESKEN
// -----------------------------------------------------------------------------
//
TInt CMediatorTest::SubscribeNotificationReceiverUnregisterEventsSoThatCatIsEmptyL( TTestResult& aResult )
    {
    TInt status( KErrNone );
    TUid myUid = TUid::Uid( KTestUid );
    TCapabilitySet caps;
    caps.SetEmpty();
    
    REventList eventList;
    CleanupClosePushL( eventList );
    REventList eventList2;
    CleanupClosePushL( eventList2 );
    
    // Clean events if any exist
    TInt preStatus = iNotifications->GetEvents( myUid, myUid, eventList );
    if( KErrNone == preStatus )
        {
        preStatus = CleanRegisterMultipleEvents(myUid, myUid, eventList);
        CTestScheduler::StartAndStopL(2000);
        }
    
    iTestHelper->ClearAllNotifyData();
    status = iNotifications->RegisterNotificationObserver(iTestHelper);
    CTestScheduler::StartAndStopL( 2000 );
    if( KErrNone == status  )
        {
        for( TInt i=85; i<90; i++ ) // Create list of events (i = event ID)
            {
            TEvent newEvent;
            newEvent.iEventId = i;
            newEvent.iVersion = iDefaultVersion;
            newEvent.iCaps = caps;    
            eventList2.AppendL( newEvent );
            }
        
            preStatus = PreconditionRegisterEvents(myUid, myUid, eventList2);
            _LIT( KDescription, "PreconditionRegisterEvents returns with" );
            aResult.SetResult( preStatus, KDescription );
        
            if( KErrNone == preStatus )
                {
                status = iEventProvider->UnregisterEvent( myUid, 
                                                          myUid, // this category should be empty when these events are unregistered
                                                          eventList2 );
    
                CTestScheduler::StartAndStopL(2000);
                _LIT( KDescription, "UnregisterEvent returns with" );
                aResult.SetResult( status, KDescription );
                
                if( iTestHelper->iNotificationReceived == 0 )
                    {
                    status = KErrNotFound;
                    _LIT( KDescription, "iTestHelper check failed" );
                    aResult.SetResult( status, KDescription );
                    }
                }
        status = iNotifications->UnregisterNotificationObserver();
        CTestScheduler::StartAndStopL( 2000 );
        }
    CleanupStack::PopAndDestroy( &eventList2 );
    CleanupStack::PopAndDestroy( &eventList );
    return KErrNone;
    }
// -----------------------------------------------------------------------------
// CMediatorTest::SubscribeNotificationReceiverRegisterCommandsSoThatErrorInRegistration
// SubscribeNotificationReceiver and Register commands
// 
// KESKEN
// -----------------------------------------------------------------------------
//
TInt CMediatorTest::SubscribeNotificationReceiverRegisterCommandsSoThatErrorInRegistrationL( TTestResult& aResult )
    {
    TInt status( KErrNone );
    TUid myUid = TUid::Uid( KTestUid ); 
    TCapabilitySet caps;
    caps.SetEmpty();
    iTestHelper->ClearAllNotifyData();
    TInt preStatus = PreconditionRegisterCommand( myUid, 
                                                  myUid, 
                                                  KDefaultCommandId,
                                                  iDefaultVersion,
                                                  caps,
                                                  KDefaultTimeOut );
    
    _LIT( KErrorDescription, "PreconditionRegisterCommand returns with" );
    aResult.SetResult( preStatus, KErrorDescription );    
    if( KErrNone == preStatus )
        {
        status = iCommandResponder->RegisterCommand( myUid, 
                                                     myUid, 
                                                     KDefaultCommandId, 
                                                     iDefaultVersion,
                                                     caps, 
                                                     KDefaultTimeOut );
        CTestScheduler::StartAndStopL( 2000 );
        _LIT( KDescription, "RegisterCommand returns with" );
        aResult.SetResult( KErrNone, KDescription );
        if( iTestHelper->iNotificationReceived != 0 )
            {
            status = KErrNotFound;
            _LIT( KDescription, "iTestHelper check failed" );
            aResult.SetResult( KErrNone, KDescription );
            }
      
         status = CleanRegisterCommand( myUid, 
                                        myUid, 
                                        KDefaultCommandId );
      
         if( KErrNone != status )
             {
             _LIT( KDescription, "Test passed but cleaning failed" );
             aResult.SetResult( status, KDescription );        
             }     
        }
    return KErrNone;    
    }

// -----------------------------------------------------------------------------
// CMediatorTest::SubscribeNotificationReceiverUnegisterCommandsSoThatErrorInRegistration
// 
// KESKEN
// -----------------------------------------------------------------------------
//
TInt CMediatorTest::SubscribeNotificationReceiverUnegisterCommandsSoThatErrorInRegistrationL( TTestResult& aResult )
    {
    TInt status( KErrNone );
    TUid myUid = TUid::Uid( KTestUid );
    TCapabilitySet caps;
    caps.SetEmpty();
    TVersion version( 8,5,7 ); // version!
    
    REventList eventList;
    CleanupClosePushL(eventList);
                        
    for( TInt i=85; i<90; i++ ) // Create list of events (i = event ID)
        {
        TEvent newEvent;
        newEvent.iEventId = i;
        newEvent.iVersion = version;
        newEvent.iCaps = caps;
        eventList.AppendL(newEvent);
        }
    
    // This unregistration should fail!
    status = iEventProvider->UnregisterEvent( myUid, 
                                              myUid, 
                                              eventList );
    _LIT( KDescription, "UnregisterEvent returns with" );
    if( KErrNone == status  )
        {
        status = KErrNotFound;
        }
    else
        {
        status = KErrNone;
        }
    CleanupStack::PopAndDestroy(&eventList);
    aResult.SetResult( status, KDescription );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CMediatorTest::SubscribeNotificationReceiverRegisterEventsSoThatErrorInRegistration
// SubscribeNotificationReceiver and Register Events
// KESKEN
// -----------------------------------------------------------------------------
//
TInt CMediatorTest::SubscribeNotificationReceiverRegisterEventsSoThatErrorInRegistration( TTestResult& aResult )
    {
    TInt status( KErrNone );
    TUid myUid = TUid::Uid( KTestUid );
    TCapabilitySet caps;
    caps.SetEmpty();
    TVersion version( -1,5,8 );
    
    iTestHelper->ClearAllNotifyData();
    
    TInt preStatus = PreconditionRegisterOneEvent( myUid, myUid, KTestEventId, version, caps );
    _LIT( KErrorDescription, "PreconditionRegisterOneEvent returns with" );
    aResult.SetResult( preStatus, KErrorDescription );
    
    if( KErrNone == preStatus )
        {    
        iTestHelper->ClearAllNotifyData();
        // This registration fail
        status = iEventProvider->RegisterEvent( myUid, 
                                                myUid,
                                                KTestEventId,
                                                version,
                                                caps ) ;        
        if( KErrNone == status  )
            {
            status = KErrNotFound;
            }
        else
            {
            status = CleanRegisterOneEvent(myUid, myUid, KTestEventId);
            if ( KErrNone != status )
                {
                _LIT( KErrorDescription, "Test passed but cleaning failed" );
                aResult.SetResult( status, KErrorDescription );    
                }    
            } 
        }
    _LIT( KDescription, "RegisterEvent test returns with" );
    aResult.SetResult( status, KDescription ); // return status as test result
    return KErrNone;
    }
// -----------------------------------------------------------------------------
// CMediatorTest::SubscribeNotificationReceiverUnregisterEventsSoThatErrorInRegistration
// UnRegister Events with wrong domainId
// Expected Behaviour: KErrMediatorDomainNotFound
// KESKEN
// -----------------------------------------------------------------------------
//
TInt CMediatorTest::SubscribeNotificationReceiverUnregisterEventsSoThatErrorInRegistration( TTestResult& aResult )
    {
    TInt status( KErrNone );
    TUid myUid = TUid::Uid( KTestUid );
    TUid invalidUid = TUid::Uid( KNonExistingDomainId );
    
    status = iEventProvider->UnregisterEvent( invalidUid, 
                                              myUid,
                                              KTestEventId ) ;       
    //expected error is KErrMediatorDomainNotFound
    if( KErrMediatorDomainNotFound == status )
        {
        _LIT( KDescription, "UnregisterEvent returns with expected error KErrMediatorDomainNotFound" );
        aResult.SetResult( KErrNone, KDescription );        
        }
    else
        {
        _LIT( KErrorDescription, "Wrong error status" );
        aResult.SetResult( KErrGeneral, KErrorDescription );
        }
    return KErrNone;
    }
       
// -----------------------------------------------------------------------------
// CMediatorTest::IssueCommandCancelAndReissue
//
// Test steps:
// 1) Issues a command
// 2) Cancels the command
// 3) Reissues the command
// -----------------------------------------------------------------------------
//

TInt CMediatorTest::IssueCommandCancelAndReissueL( TTestResult& aResult )
    {
    TUid myUid = TUid::Uid( KTestUid );
    iTestHelper->ClearAllNotifyData();
    TCapabilitySet caps;
    caps.SetEmpty();
    
    TInt status = PreconditionRegisterCommand( myUid, myUid, KMinListId+2, iDefaultVersion, caps, 5000 );
    
    if( KErrNone == status   )
        {    
        _LIT8(KTest, "TestTestTest");
        HBufC8* data = KTest().AllocLC();
        
        status = iCommandInitiator->IssueCommand( myUid, myUid, KMinListId+2, iDefaultVersion, *data );
        
        if ( KErrNone == status  )
            {
            iCommandInitiator->CancelCommand( myUid, myUid, KMinListId+2 );
            status = iCommandInitiator->IssueCommand( myUid, myUid, KMinListId+2, iDefaultVersion, *data );
            CTestScheduler::StartAndStopL(4000);
            if ( KErrNone != status )
                {
                _LIT( KErrorDescription, "2nd issue command failed" );
                aResult.SetResult( status, KErrorDescription );    
                }
            }
        else
            {
            _LIT( KErrorDescription, "1st issue command failed" );
            aResult.SetResult( status, KErrorDescription );    
            }
        CleanupStack::PopAndDestroy( data ); 
        status = CleanRegisterCommand( myUid, myUid,  KMinListId+2 );
        if ( KErrNone != status )
            {
            _LIT( KDescription, "Test passed but cleaning failed" );
            aResult.SetResult( status, KDescription );      
            }
        }
    else
        {
        _LIT( KErrorDescription, "Preconditions failed" );
        aResult.SetResult( status, KErrorDescription );    
        }
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CMediatorTest::IssueCommandCancelAndIssueNewCommand
//
// Test steps:
// 1) Issue a command
// 2) Cancels the command
// 3) Issue a new command
// -----------------------------------------------------------------------------
//

TInt CMediatorTest::IssueCommandCancelAndIssueNewCommandL( TTestResult& aResult )
    {
    TUid myUid = TUid::Uid( KTestUid );
    iTestHelper->ClearAllNotifyData();
    TCapabilitySet caps;
    caps.SetEmpty();
    const TInt cmdId1 = KMinListId+2;
    const TInt cmdId2 = KMinListId+3;
    
    TInt status = PreconditionRegisterCommand( myUid, myUid, cmdId1, iDefaultVersion, caps, 5000 );
    
    if ( KErrNone == status  )
        {
        status = PreconditionRegisterCommand( myUid, myUid, cmdId2, iDefaultVersion, caps, 5000 );
        
        if( KErrNone == status  )
            {    
            _LIT8(KTest, "TestTestTest");
            HBufC8* data = KTest().AllocLC();
            status = iCommandInitiator->IssueCommand( myUid, myUid, cmdId1, iDefaultVersion, *data );
            CTestScheduler::StartAndStopL(2000);
            if ( KErrNone == status  )
                {
                iCommandInitiator->CancelCommand( myUid, myUid, cmdId1 );
                status = iCommandInitiator->IssueCommand( myUid, myUid, cmdId2, iDefaultVersion, *data );
                CTestScheduler::StartAndStopL(2000);
                if ( KErrNone != status )
                    {
                    _LIT( KErrorDescription, "2nd issue command failed" );
                    aResult.SetResult( status, KErrorDescription );    
                    }
                }
            else
                {
                _LIT( KErrorDescription, "1st issue command failed" );
                aResult.SetResult( status, KErrorDescription );    
                }
            //unregister 2nd command
            status = CleanRegisterCommand( myUid, myUid,  cmdId2 );
            if ( KErrNone != status )
                {
                _LIT( KDescription, "Cleaning failed: 2nd command" );
                aResult.SetResult( status, KDescription );      
                }
            CleanupStack::PopAndDestroy( data );            
            }
        else
            {
            _LIT( KErrorDescription, "Preconditions failed: 2nd command register" );
            aResult.SetResult( status, KErrorDescription );    
            }
        //unregister 1st command
        status = CleanRegisterCommand( myUid, myUid,  cmdId1 );
        if ( KErrNone != status )
            {
            _LIT( KDescription, "Cleaning failed: 1st command" );
            aResult.SetResult( status, KDescription );      
            }
        CTestScheduler::StartAndStopL(2000); 
        }
    else
        {
        _LIT( KErrorDescription, "Preconditions failed: 1st command register" );
        aResult.SetResult( status, KErrorDescription );    
        }
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CMediatorTest::InstantiateAndDeleteMediatorPlugin
//
// Test steps:
// 1) Instantiates the test plugin
// 2) Destroys the the test plugin
// -----------------------------------------------------------------------------
//
TInt CMediatorTest::InstantiateAndDeleteMediatorPluginL( TTestResult& aResult )
    {
    CMediatorPluginBase* testPlugin;
    TRAPD( status, testPlugin = CMediatorPluginBase::NewL( TUid::Uid(0x87654323 ) ) );
    if ( KErrNone != status )
        {
        _LIT( KDescription, "Failed to create plugin" );
        aResult.SetResult( status, KDescription );      
        }
    else
        {
        delete testPlugin;
        _LIT( KErrorDescription, "InstantiateAndDeleteMediatorPlugin test passed" );
        aResult.SetResult( status, KErrorDescription );  
        }
    REComSession::FinalClose();
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CMediatorTest::IssueCommandNoData
// A command is issued that does not contain parameter data. 
// Test steps:
// 1) Issues a command and check for error
// 2) Issue response and check for error
// 3) Cleanup
// -----------------------------------------------------------------------------
//
TInt CMediatorTest::IssueCommandNoDataL( TTestResult& aResult )
    {
    TUid myUid = TUid::Uid( KTestUid );
    TCapabilitySet caps;
    caps.SetEmpty();
    iTestHelper->ClearAllNotifyData();

    TInt status = PreconditionRegisterCommand( myUid, 
                                               myUid, 
                                               KDefaultCommandId,
                                               iDefaultVersion,
                                               caps,
                                               KDefaultTimeOut );
    
    _LIT( KErrorDescription, "PreconditionRegisterCommand returns with" );
    aResult.SetResult( status, KErrorDescription );    
    
    if( KErrNone == status  )
        {
        iCommandInitiator->CancelCommand( myUid, myUid, KDefaultCommandId );
        status = iCommandInitiator->IssueCommand( myUid, 
                                                  myUid, 
                                                  KDefaultCommandId,
                                                  iDefaultVersion, 
                                                  KNullDesC8 );
        CTestScheduler::StartAndStopL(2000); // wait 2s
        _LIT( KErrorDescription, "IssueCommand returns with");
        aResult.SetResult( status, KErrorDescription );
        
        if ( KErrNone == status  )
            {
            status = iCommandResponder->IssueResponse( iTestHelper->iDomain, 
                                                       iTestHelper->iCategory, 
                                                       iTestHelper->iCommandId, 
                                                       KErrNone,
                                                       KNullDesC8 );                                      
            
            _LIT( KErrorDescription, "IssueResponse returns with" );
            aResult.SetResult( status, KErrorDescription );
            
            if ( KErrNone == status  ) // execute cleanup as part of the test
                {
                status = CleanRegisterCommand( myUid, myUid, KDefaultCommandId );
                if ( KErrNone != status )
                    {
                    _LIT( KDescription, "Test passed but cleaning failed" );
                    aResult.SetResult( status, KDescription );    
                    }
                }
            }
        else // some step has failed, attempt cleanup but don't care about the result
            {
            CleanRegisterCommand( myUid, myUid, KDefaultCommandId );
            }
        CTestScheduler::StartAndStopL(1000);
       }
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CMediatorTest::InstantiateAndDeleteMediatorPlugin
// An event is raised that does not contain parameter data. 
// Test steps:
// 1) Raise an event and check return value
// 2) cleanup
// -----------------------------------------------------------------------------
//
TInt CMediatorTest::RaiseEventNoDataL( TTestResult& aResult )
    {
    TUid myUid = TUid::Uid( KTestUid );
    TCapabilitySet caps;
    caps.SetEmpty();
    TInt status( KErrNone );
    iTestHelper->ClearAllNotifyData();
    
    CMediatorEventProvider* eventProvider = CMediatorEventProvider::NewL();
    CleanupStack::PushL( eventProvider );
    status = eventProvider->RegisterEvent( myUid, myUid, KTestEventId, iDefaultVersion, caps);
    CTestScheduler::StartAndStopL(1000);
    
    _LIT( KErrorDescription, "PreconditionRegisterOneEvent returns with" );
    aResult.SetResult( status, KErrorDescription );
      
    if( KErrNone == status  )
        { 
        status = eventProvider->RaiseEvent( myUid,
                                            myUid, 
                                            KTestEventId,
                                            iDefaultVersion, 
                                            KNullDesC8 );        
        CTestScheduler::StartAndStopL(2000);
        _LIT( KDescription, "RaiseEvent returns with" );
        aResult.SetResult( status, KDescription );      

        if ( KErrNone == status  ) // execute cleanup as part of the test
            {
            status = CleanRegisterOneEvent(myUid, myUid, KTestEventId);
            
            if ( KErrNone != status )
                {
                _LIT( KErrorDescription, "Test passed but cleaning failed" );
                aResult.SetResult( status, KErrorDescription );    
                }                
            }
        else // some test step has failed, try to cleanup
            {
            CleanRegisterOneEvent( myUid, myUid, KTestEventId );
            }
        }
    CleanupStack::PopAndDestroy( eventProvider );
    return KErrNone;
    }

void ResetTestKeys()
    {
    RProperty::Set( KMediatorStifTestPsKeys, KCurrentTestListVerificationVerdict, KErrGeneral );
    
    RProperty::Set( KMediatorStifTestPsKeys, KCurrentTestList, KErrNotSupported ) ;
    
    TTestItemListSetPckgBuf itemsPckgBuf;
    itemsPckgBuf().iCount = 0;
    RProperty::Set( KMediatorStifTestPsKeys, KReceivedTestLists, itemsPckgBuf );
    }

TInt VerifyTestItemList()
    {
    TTestItemListSetPckgBuf itemsPckgBuf;
    itemsPckgBuf().iCount = 0;
        
    TInt err = RProperty::Get( KMediatorStifTestPsKeys, KReceivedTestLists, itemsPckgBuf );
    
    if ( err != KErrNone ) return err;

    TInt listSize( sizeof(TestItemLists) / sizeof(TTestItemList) );
        
    if ( listSize !=  itemsPckgBuf().iCount ) return KErrCorrupt;
    
    TInt verdict( KErrNone );
    
    for ( TInt i = 0; i < listSize; i++ )
        {
        const TTestItemList* registeredDataList = &TestItemLists[i];
        const TTestItemList* receivedDataList = &( itemsPckgBuf().iTestLists[i]);
        
        if ( receivedDataList->iDomain != registeredDataList->iDomain ) verdict = KErrCorrupt;
        if ( receivedDataList->iCategory != registeredDataList->iCategory ) verdict = KErrCorrupt;
        if ( receivedDataList->iFirst !=  registeredDataList->iFirst ) verdict = KErrCorrupt;
        if ( receivedDataList->iLast !=  registeredDataList->iLast ) verdict = KErrCorrupt;
        }

    return verdict;
    }

// -----------------------------------------------------------------------------
// CMediatorTest::RegisterMultipleEventsWithMultipleUIDs
//
// Register 50 events in Mediator
// Expected precondition: No events created previously
// Expected behaviour: Registration succeed
// Cleaning: Unregister events
//
// Domain: KTestUid
// Category: KTestUid
// Event IDs: 50 - 99
// -----------------------------------------------------------------------------
//
void RegisterTestEventListL( TInt aIndex, CMediatorEventProvider& aEventProvider, MediatorService::REventList& aEventList )
    {
    if ( aIndex < 0 )User::Leave( KErrCorrupt );
    if ( aIndex > (sizeof(TestItemLists) / sizeof(TTestItemList) - 1) ) User::Leave( KErrCorrupt );
    
    User::LeaveIfError( RProperty::Set( KMediatorStifTestPsKeys, KCurrentTestList, aIndex ) ); 
    
    TCapabilitySet caps;
    caps.SetEmpty();    
    aEventList.Reset();
    
    const TTestItemList* currentList = &TestItemLists[aIndex];
    
    for( TInt i = currentList->iFirst; i <= currentList->iLast ; i++ )
        {
        TEvent newEvent;
        newEvent.iEventId = i;
        newEvent.iVersion = TVersion();
        newEvent.iCaps = caps;  
        aEventList.AppendL( newEvent );
        }
    
    User::LeaveIfError( aEventProvider.RegisterEvent( currentList->iDomain, 
                                                      currentList->iCategory,
                                                      aEventList ) );
    
    }

void CleanTestEventListL( TInt aIndex, CMediatorEventProvider& aEventProvider, REventList& aEventList )
    {
    if ( aIndex < 0 )User::Leave( KErrCorrupt );
    if ( aIndex > (sizeof(TestItemLists) / sizeof(TTestItemList) - 1) ) User::Leave( KErrCorrupt );
    
    const TTestItemList* currentList = &TestItemLists[aIndex];
    
    User::LeaveIfError( aEventProvider.UnregisterEvent( currentList->iDomain, currentList->iCategory, aEventList ) );
    
    }

TInt CMediatorTest::RegisterMultipleEventsWithMultipleUIDsL( TTestResult& aResult )
    {
    TInt KTestRounds( sizeof(TestItemLists) / sizeof(TTestItemList) );
    RArray<REventList*> eventLists;
    CleanupClosePushL( eventLists );
    User::LeaveIfError( eventLists.Reserve(KTestRounds) );
    
    TInt preStatus( KErrNone );
    
    // reserve space for events 
    for ( TInt i = 0; i < KTestRounds; i++ )
        {
        REventList* list = new REventList();
        if ( list )
            {
            User::LeaveIfError( preStatus = eventLists.Append(list) );
            }
        else
            {
            preStatus = KErrNoMemory;
            }
        
        if ( KErrNone != preStatus )
            {
            _LIT( KDescription, "Test list initialization failed" );
            aResult.SetResult( preStatus, KDescription );
            return KErrNone;
            }
        }
    
    TInt status( KErrNone );
    for ( TInt i = 0; i < KTestRounds; i++ )
        {
        TRAP( status, RegisterTestEventListL( i, *iEventProvider, *eventLists[i] ) );
        if ( KErrNone != status  )
            {
            _LIT( KDescription, "Registering events failed" );
            aResult.SetResult( status, KDescription );
            return KErrNone;
            }
        }
    CTestScheduler::StartAndStopL( 2000 ); // wait 2s
    // check results
    status = VerifyTestItemList();
    
    if ( KErrNone != status )
        {
        _LIT( KDescription, "Data registered to client corrupted" );
        aResult.SetResult( status, KDescription );
        }
    
    for ( TInt i = 0; i < KTestRounds; i++ )
        {
        TRAPD( cleanStatus, CleanTestEventListL( i, *iEventProvider, *eventLists[i] ) );
        eventLists[i]->Reset();
        if ( KErrNone != cleanStatus )
             {
             _LIT( KDescription, "Cleaning event list failed" );
             aResult.SetResult( status, KDescription );
             }
        }
    
    for ( TInt i = 0; i < eventLists.Count(); i++ )
        {
        delete eventLists[i];
        }
    CleanupStack::PopAndDestroy( &eventLists );
    ResetTestKeys();
    return KErrNone;
    }


// -----------------------------------------------------------------------------
// CMediatorTest::RegisterMultipleCommandsWithMultipleUIDs
// 
// Expected preconditions: Command does not exist
// Expected behaviour: Register command succeed
// Cleaning: Command unregistered
//  
// Register multiple commands
// Command ID(s): 1 -  KMaxListId
// -----------------------------------------------------------------------------
//

void RegisterTestCommandListL( TInt aIndex, CMediatorCommandResponder& aCommandResponder, RCommandList& aCommandList )
    {
    if ( aIndex < 0 )User::Leave( KErrCorrupt );
    if ( aIndex > (sizeof(TestItemLists) / sizeof(TTestItemList) - 1) ) User::Leave( KErrCorrupt );
    
    User::LeaveIfError( RProperty::Set( KMediatorStifTestPsKeys, KCurrentTestList, aIndex ) ); 
    
    TCapabilitySet caps;
    caps.SetEmpty();    
    aCommandList.Reset();
    
    const TTestItemList* currentList = &TestItemLists[aIndex];
    
    for( TInt i = currentList->iFirst; i <= currentList->iLast ; i++ )
        {
        MediatorService::TCommand newCommand;
        newCommand.iCommandId = i;
        newCommand.iVersion = TVersion();
        newCommand.iCaps = caps;
        newCommand.iTimeout = KMediatorResponseTimeout;
        aCommandList.AppendL( newCommand );
        }
    
    User::LeaveIfError( aCommandResponder.RegisterCommand( currentList->iDomain,
                                                           currentList->iCategory, 
                                                           aCommandList ) );
    }

void CleanTestCommandListL( TInt aIndex, CMediatorCommandResponder& aCommandResponder, RCommandList& aCommandList  )
    {
    if ( aIndex < 0 )User::Leave( KErrCorrupt );
    if ( aIndex > (sizeof(TestItemLists) / sizeof(TTestItemList) - 1) ) User::Leave( KErrCorrupt );
    
    const TTestItemList* currentList = &TestItemLists[aIndex];
    
    User::LeaveIfError( aCommandResponder.UnregisterCommand( currentList->iDomain, currentList->iCategory, aCommandList ) );
    
    }

TInt CMediatorTest::RegisterMultipleCommandsWithMultipleUIDsL( TTestResult& aResult )
    {
    TInt KTestRounds( sizeof(TestItemLists) / sizeof(TTestItemList) );
    
    RArray<RCommandList*> commandLists;
    CleanupClosePushL( commandLists );
    User::LeaveIfError( commandLists.Reserve(KTestRounds) );
    
    TInt preStatus( KErrNone );
    // reserve space for events 
    for( TInt i = 0; i < KTestRounds; i++ )
        {
        RCommandList* list = new RCommandList();
        if ( list )
            {
            User::LeaveIfError( preStatus = commandLists.Append(list) );
            }
        else
            {
            preStatus = KErrNoMemory;
            }
        if ( KErrNone != preStatus )
            {
            _LIT( KDescription, "Test list initialization failed" );
            aResult.SetResult( preStatus, KDescription );
            return KErrNone;
            }
        }
    TInt status( KErrNone );
    for( TInt i = 0; i < KTestRounds; i++ )
        {
        TRAP( status, RegisterTestCommandListL( i, *iCommandResponder, *commandLists[i] ) );
        if ( KErrNone == status  )
            {
            if ( status != KErrNone )
                {
                _LIT( KDescription, "Receiving end verification failed" );
                aResult.SetResult( status, KDescription );
                return KErrNone;
                }
            }
        else
            {
            _LIT( KDescription, "Registering commands failed" );
            aResult.SetResult( status, KDescription );
            return KErrNone;
            }
        }
    CTestScheduler::StartAndStopL( 2000 ); // wait 2s
    // check results
    status = VerifyTestItemList();
    if( status != KErrNone )
        {
        _LIT( KDescription, "Data registered to client corrupted" );
        aResult.SetResult( status, KDescription );
        }
    for( TInt i = 0; i < KTestRounds; i++ )
        {
        TRAPD( cleanStatus, CleanTestCommandListL( i, *iCommandResponder, *commandLists[i] ) );
        commandLists[i]->Reset();
        if( cleanStatus != KErrNone )
            {
            _LIT( KDescription, "Cleaning command list failed" );
            aResult.SetResult( status, KDescription );
            }
        }
    for( TInt i = 0; i < commandLists.Count(); i++ )
        {
        delete commandLists[i];
        }
    CleanupStack::PopAndDestroy( &commandLists );
    ResetTestKeys();
    return KErrNone;
    }

// -----------------------------------------------------------------------------
//Client side OOM test 
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// OOM test for Event Consumer
// -----------------------------------------------------------------------------


TInt CMediatorTest::OOMForEventConsumerTestL(TTestResult& aResult)
    {
    TInt ret = KErrNoMemory;
    
    for (TInt allocFailRate = 1;; allocFailRate++)
        {
        __UHEAP_RESET;
        __UHEAP_SETFAIL(RHeap::EDeterministic,allocFailRate);
        __UHEAP_MARK;
        TRAP(ret, EventConsumerTestL());
        __UHEAP_MARKEND;
        if (ret == KErrNone)
            {
            break;
            }
        }
    __UHEAP_RESET;
    aResult.SetResult(ret, _L("OOMForEventConsumerTestL"));
    return KErrNone;
    }

TInt CMediatorTest::EventConsumerTestL()
    {
    TInt status(KErrNone);
    TUid myUid = TUid::Uid(KTestUid);
    TCapabilitySet caps;
    caps.SetEmpty(); 
    CMediatorEventConsumer* eventConsumer;    
    eventConsumer = CMediatorEventConsumer::NewL(iTestHelper);
    CleanupStack::PushL(eventConsumer);
    CMediatorEventProvider* eventProvider;    
    eventProvider = CMediatorEventProvider::NewL();
    CleanupStack::PushL(eventProvider);
     
    status = eventProvider->RegisterEvent(myUid, myUid, KTestEventId,iDefaultVersion, caps);
       
    if( KErrNone == status || KErrMediatorEventAlreadyExists == status )
        {
        status = eventConsumer->SubscribeEvent(myUid, myUid, KTestEventId, iDefaultVersion);
        User::LeaveIfError(status);   
        if( KErrNone == status )   
            {
            status = eventConsumer->UnsubscribeEvent(myUid, myUid, KTestEventId);
            User::LeaveIfError(status);
            }
        status = eventProvider->UnregisterEvent(myUid,myUid,KTestEventId); 
        User::LeaveIfError(status);
        }
    else
        {
        User::LeaveIfError(status);
        }
    
    CleanupStack::PopAndDestroy(eventProvider);
    CleanupStack::PopAndDestroy(eventConsumer);
    return status;    
    }

// -----------------------------------------------------------------------------
// OOM test for Event Provider
// -----------------------------------------------------------------------------

TInt CMediatorTest::OOMForEventProviderTestL(TTestResult& aResult)
    {
    TInt ret = KErrNoMemory;
    for (TInt allocFailRate = 1;; allocFailRate++)
        {
        __UHEAP_RESET;
        __UHEAP_SETFAIL(RHeap::EDeterministic,allocFailRate);
        __UHEAP_MARK;
        TRAP(ret, EventProviderTestL());
        __UHEAP_MARKEND;
        if (ret == KErrNone)
            {
            break;
            }
        }
    __UHEAP_RESET;
    aResult.SetResult(ret, _L("OOMForEventProviderTestL"));
    return KErrNone;
    }

TInt CMediatorTest::EventProviderTestL()
    {
    TInt status(KErrNone);
    TUid myUid = TUid::Uid(KTestUid);
    TCapabilitySet caps;
    caps.SetEmpty();
    _LIT8(KTest, "TestTestTest");
    HBufC8* data = KTest().AllocLC();
       
    CMediatorEventProvider* eventProvider;    
    eventProvider = CMediatorEventProvider::NewL();
    CleanupStack::PushL(eventProvider);
  
    status = eventProvider->RegisterEvent(myUid, myUid, KTestEventId,iDefaultVersion, caps); 
    //RaiseEvent and Unregister only if event is registerd.
    if( KErrNone == status || KErrMediatorEventAlreadyExists == status )
        {
        status = eventProvider->RaiseEvent(myUid,myUid,KTestEventId,iDefaultVersion,*data);
        User::LeaveIfError(status);   
        if( KErrNone == status )   
            {
            status = eventProvider->UnregisterEvent(myUid,myUid,KTestEventId); 
            User::LeaveIfError(status);
            }
        }
    else
        {
        User::LeaveIfError(status);
        }
    CleanupStack::PopAndDestroy(eventProvider);
    CleanupStack::PopAndDestroy(data);
    return status;
    }

// -----------------------------------------------------------------------------
// OOM test for Event provider with list of event
// -----------------------------------------------------------------------------

TInt CMediatorTest::OOMForEventProviderwithListTestL(TTestResult& aResult)
    {
    TInt ret = KErrNoMemory;
    for (TInt allocFailRate = 1;; allocFailRate++)
        {
        __UHEAP_RESET;
        __UHEAP_SETFAIL(RHeap::EDeterministic,allocFailRate);
        __UHEAP_MARK;
        TRAP(ret, EventProviderListEventTestL());
        __UHEAP_MARKEND;
        if (ret == KErrNone)
            {
            break;
            }
        }
    __UHEAP_RESET;
    aResult.SetResult(ret, _L("OOMForEventProviderwithListTestL"));
    return KErrNone;
    }

TInt CMediatorTest::EventProviderListEventTestL()
    {
    TInt status(KErrNone);
    TUid myUid = TUid::Uid(KTestUid);
    TCapabilitySet caps;
    caps.SetEmpty();  
    
    REventList eventList;
    CleanupClosePushL(eventList);
    for (TInt i = 1; i < 10; i++) // i = Event ID
        {
        TEvent newEvent;
        newEvent.iEventId = i;
        newEvent.iVersion = iDefaultVersion;
        newEvent.iCaps = caps;
        eventList.AppendL( newEvent );
        }
    
    CMediatorEventProvider* eventProvider;    
    eventProvider = CMediatorEventProvider::NewL();
    CleanupStack::PushL(eventProvider);
  
    status = eventProvider->RegisterEvent(myUid, myUid, eventList); 
    //Unregister only if event is registerd.
    if( KErrNone == status || KErrMediatorEventAlreadyExists == status )
        {
        status = eventProvider->UnregisterEvent( myUid, myUid, eventList ); 
        User::LeaveIfError(status);
        }
    else
        {
        User::LeaveIfError(status);
        }
    CleanupStack::PopAndDestroy(eventProvider);
    CleanupStack::PopAndDestroy(&eventList);
    
    return status;
    }

// -----------------------------------------------------------------------------
// OOM test for Event consumer with list of event
// -----------------------------------------------------------------------------

TInt CMediatorTest::OOMForEventConsumerWithEventTestL(TTestResult& aResult)
    {
    TInt ret = KErrNoMemory;
    
    for (TInt allocFailRate = 1;; allocFailRate++)
        {
        __UHEAP_RESET;
        __UHEAP_SETFAIL(RHeap::EDeterministic,allocFailRate);
        __UHEAP_MARK;
        TRAP(ret, EventConsumerWithEventTestL());
        __UHEAP_MARKEND;
        if (ret == KErrNone)
            {
            break;
            }
        }
    __UHEAP_RESET;
    aResult.SetResult(ret, _L("OOMForEventConsumerWithEventTestL"));
    return KErrNone;
    }

TInt CMediatorTest::EventConsumerWithEventTestL()
    {
    TInt status(KErrNone);
    TUid myUid = TUid::Uid(KTestUid);
    TCapabilitySet caps;
    caps.SetEmpty();
    REventList eventList;
    CleanupClosePushL(eventList);
    for (TInt i = 1; i < 10; i++) // i = Event ID
        {
        TEvent newEvent;
        newEvent.iEventId = i;
        newEvent.iVersion = iDefaultVersion;
        newEvent.iCaps = caps;
        eventList.AppendL(newEvent);
        }
    CMediatorEventConsumer* eventConsumer;    
    eventConsumer = CMediatorEventConsumer::NewL(iTestHelper);
    CleanupStack::PushL(eventConsumer);
    CMediatorEventProvider* eventProvider;    
    eventProvider = CMediatorEventProvider::NewL();
    CleanupStack::PushL( eventProvider );
     
    status = eventProvider->RegisterEvent(myUid, myUid, eventList);
    if( KErrNone == status || KErrMediatorEventAlreadyExists == status )
        {
        status = eventConsumer->SubscribeEvent(myUid, myUid, eventList);
        User::LeaveIfError( status );   
        if( KErrNone == status )   
            {
            status = eventConsumer->UnsubscribeEvent( myUid, myUid, eventList );
            User::LeaveIfError( status );
            }
        if( KErrNone == status )  
            {
            status = eventProvider->UnregisterEvent( myUid, myUid, eventList ); 
            User::LeaveIfError( status );
            }
        }
    else
        {
        User::LeaveIfError(status);
        }
    
    CleanupStack::PopAndDestroy(eventProvider);
    CleanupStack::PopAndDestroy(eventConsumer);
    CleanupStack::PopAndDestroy(&eventList);
    return status;    
    }

// -----------------------------------------------------------------------------
// OOM test for CommandInitiator 
// -----------------------------------------------------------------------------

TInt CMediatorTest::OOMForCommandInitiatorTestL(TTestResult& aResult)
    {
    TInt ret = KErrNoMemory;
    for (TInt allocFailRate = 1;; allocFailRate++)
        {
        __UHEAP_RESET;
        __UHEAP_SETFAIL(RHeap::EDeterministic,allocFailRate);
        __UHEAP_MARK;
        TRAP(ret, CommandInitiatorTestL());
        __UHEAP_MARKEND;
        if (ret == KErrNone)
            {
            break;
            }
        }
    __UHEAP_RESET;
    aResult.SetResult(ret, _L("OOMForCommandInitiatorTestL"));
    return KErrNone;
    }

TInt CMediatorTest::CommandInitiatorTestL()
    {
    TInt status(KErrNone);
    TUid myUid = TUid::Uid(KTestUid);
    TCapabilitySet caps;
    caps.SetEmpty();
    _LIT8(KTest, "TestTestTest");
    HBufC8* data = KTest().AllocLC();
    CMediatorCommandResponder* commandResponder;    
    commandResponder = CMediatorCommandResponder::NewL( iTestHelper );
    CleanupStack::PushL(commandResponder);
    
    CMediatorCommandInitiator* commandInitiator;
    commandInitiator = CMediatorCommandInitiator::NewL(iTestHelper);
    CleanupStack::PushL(commandInitiator);
    
    status = commandResponder->RegisterCommand(myUid, myUid, KDefaultCommandId, iDefaultVersion, caps, KDefaultTimeOut);
    if( KErrNone == status || KErrMediatorCommandAlreadyExists == status )
        {
        status = commandInitiator->IssueCommand(myUid, myUid, KDefaultCommandId, iDefaultVersion, *data);
        User::LeaveIfError( status );   
        if( KErrNone == status )   
            {
            _LIT8(KTestResponse, "Command response");
            HBufC8* replyData = KTestResponse().AllocLC();
            status = commandResponder->IssueResponse( myUid, myUid, KDefaultCommandId,KErrNone,*replyData );
            User::LeaveIfError( status );
            CleanupStack::PopAndDestroy(replyData);
            }
        if( KErrNone == status )  
            {
            status = commandResponder->UnregisterCommand( myUid, myUid, KDefaultCommandId );
            User::LeaveIfError( status );
            }
        }
    else
        {
        User::LeaveIfError(status);
        }

    CleanupStack::PopAndDestroy(commandInitiator);
    CleanupStack::PopAndDestroy(commandResponder);
    CleanupStack::PopAndDestroy(data);
    return status;
    }

// -----------------------------------------------------------------------------
// OOM test for CommandInitiator with list of commands
// -----------------------------------------------------------------------------

TInt CMediatorTest::OOMForCommandInitiatorCommandListTestL(TTestResult& aResult)
    {
    TInt ret = KErrNoMemory;
    for (TInt allocFailRate = 1;; allocFailRate++)
        {
        __UHEAP_RESET;
        __UHEAP_SETFAIL(RHeap::EDeterministic,allocFailRate);
        __UHEAP_MARK;
        TRAP(ret, CommandInitiatorCommandListTestL());
        __UHEAP_MARKEND;
        if (ret == KErrNone)
            {
            break;
            }
        }
    __UHEAP_RESET;
    aResult.SetResult(ret, _L("OOMForCommandInitiatorCommandListTestL"));
    return KErrNone;
    }

TInt CMediatorTest::CommandInitiatorCommandListTestL()
    {
    TInt status(KErrNone);
    TUid myUid = TUid::Uid(KTestUid);
    TCapabilitySet caps;
    caps.SetEmpty();
    //command list
    RCommandList  commandList;
    CleanupClosePushL(commandList);    
    for ( TInt index = 1; index < 10; index++ )
        {
         MediatorService::TCommand newCommand;
         newCommand.iCommandId = index;
         newCommand.iVersion = iDefaultVersion;
         newCommand.iCaps = caps;
         newCommand.iTimeout = KDefaultTimeOut;
         commandList.AppendL( newCommand );
        }
    CMediatorCommandResponder* commandResponder;    
    commandResponder = CMediatorCommandResponder::NewL( iTestHelper );
    CleanupStack::PushL(commandResponder);
    
    status = commandResponder->RegisterCommand(myUid, myUid, commandList);
    if( KErrNone == status || KErrMediatorCommandAlreadyExists == status )
        {
        status = commandResponder->UnregisterCommand( myUid, myUid, commandList );
        User::LeaveIfError(status);
        }
    else
        {
        User::LeaveIfError( status );
        }
    
    CleanupStack::PopAndDestroy(commandResponder);
    CleanupStack::PopAndDestroy(&commandList);
   
    return status;
    }

// -----------------------------------------------------------------------------
// OOM test for MediatorNotifications
// -----------------------------------------------------------------------------

TInt CMediatorTest::OOMForMediatorNotificationsTestL( TTestResult& aResult )
    {
    TInt ret = KErrNoMemory;
    for (TInt allocFailRate = 1;; allocFailRate++)
        {
        __UHEAP_RESET;
        __UHEAP_SETFAIL( RHeap::EDeterministic,allocFailRate );
        __UHEAP_MARK;
        TRAP(ret, CMediatorNotificationsTestL());
        __UHEAP_MARKEND;
        if ( ret == KErrNone )
            {
            break;
            }
        }
    __UHEAP_RESET;
    aResult.SetResult(ret, _L("OOMForMediatorNotificationsTestL"));
    return KErrNone;
    }

TInt CMediatorTest::CMediatorNotificationsTestL()
    {
    TInt status(KErrNone);
    TUid myUid = TUid::Uid(KTestUid);
    TCapabilitySet caps;
    caps.SetEmpty();
    
    iTestHelper->ClearAllNotifyData();
    CMediatorNotifications* notifyMe = CMediatorNotifications::NewL();
    CleanupStack::PushL( notifyMe );
    status = notifyMe->RegisterNotificationObserver( iTestHelper );
    if( KErrNone == status  )
        {
        REventList eventList;
        CleanupClosePushL( eventList );
        
        CMediatorEventProvider* eventProvider;    
        eventProvider = CMediatorEventProvider::NewL();
        CleanupStack::PushL( eventProvider );
             
        for( TInt i=0; i<5; i++ ) // Create list of events (i = event ID)
            {
            TEvent newEvent;
            newEvent.iEventId = i;
            newEvent.iVersion = iDefaultVersion;
            newEvent.iCaps = caps;    
            eventList.AppendL( newEvent );
            }
        
        status = eventProvider->RegisterEvent( myUid, myUid, eventList );
        if( KErrNoMemory == status )
             {
             User::LeaveIfError( status );
             }
        CTestScheduler::StartAndStopL( 2000 );
        status = eventProvider->UnregisterEvent( myUid, myUid, eventList );
        if( KErrNoMemory == status )
             {
             User::LeaveIfError( status );
             }
        CleanupStack::PopAndDestroy( eventProvider );
        CleanupStack::PopAndDestroy( &eventList );
        }
    else
        {
        User::LeaveIfError( status );
        }
    status = notifyMe->UnregisterNotificationObserver();
    if( KErrNoMemory == status )
        {
        User::LeaveIfError( status );
        }
    iTestHelper->ClearAllNotifyData();
    CleanupStack::PopAndDestroy( notifyMe );
    return status;
    }

//  End of File

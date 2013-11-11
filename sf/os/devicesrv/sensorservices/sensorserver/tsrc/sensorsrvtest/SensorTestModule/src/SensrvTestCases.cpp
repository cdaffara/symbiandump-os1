/*
* Copyright (c) 2002-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
#include <e32svr.h>
#include <StifParser.h>
#include <Stiftestinterface.h>
#include <centralrepository.h>
#include <sensrvaccelerometersensor.h>
#include "SensrvTestCases.h"
#include "sensrvchannel.h"
#include "sensrvchannelFinder.h"
#include "SensrvInternalPSKeys.h"
#include "sensrvchanneldatatypes.h"
#include "sensrvprivatecrkeys.h"
#include "sensrvdefines.h"
#include "sensrvgeneralproperties.h"
#include "sensrvtestchanneldatatypes.h"
#include "sensrvsensorchannels.h"


// EXTERNAL DATA STRUCTURES
//extern  ?external_data;

// EXTERNAL FUNCTION PROTOTYPES
//extern ?external_function( ?arg_type,?arg_type );

// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def

// LOCAL CONSTANTS AND MACROS
_LIT( KDateString, "%-B%:0%J%:1%T%:2%S%.%*C4%:3%+B" );
_LIT( KSensrvTestProcessName, "SenSrvTestProcess%d" );
_LIT( KSensrvTestProcessNamePrefix, "SenSrvTestProcess" );

const TInt KSensrvUncertainExpectedValue(99999999);


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
// CSensrvTestCases::CSensrvTestCases
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSensrvTestCases::CSensrvTestCases( /*CTestModuleIf& aTestModuleIf*/ ):
        CActive( EPriorityHigh ), iChannelInfoList(10)
    {
    iPreviousAxis_xData = KErrNotFound;
    iPreviousAxis_yData = KErrNotFound;
    iPreviousAxis_zData = KErrNotFound;

    iPropDataRate.SetPropertyId( KSensrvPropIdDataRate );
    iPropMeasureRange.SetPropertyId( KSensrvPropIdMeasureRange );
    iPropSensorDescription.SetPropertyId( KSensrvSensorDescription );
    iPropChannelScale.SetPropertyId( KSensrvPropIdChannelScale );
    iPropAvailability.SetPropertyId( KSensrvPropIdAvailability );
    }

// -----------------------------------------------------------------------------
// CSensrvTestCases::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSensrvTestCases::ConstructL()
    {

    User::LeaveIfError(iThread.Open(iThread.Id()));

    // Init mutex
    User::LeaveIfError( iMutex.CreateLocal() );

    CActiveScheduler::Add(this);
    iStatus = KRequestPending;
    SetActive();
    }


// -----------------------------------------------------------------------------
// CSensrvTestCases::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSensrvTestCases* CSensrvTestCases::NewL()
    {
    CSensrvTestCases* self = new (ELeave) CSensrvTestCases( /*aTestModuleIf*/ );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;

    }

// Destructor
CSensrvTestCases::~CSensrvTestCases()
    {
    RDebug::Print( _L( "CSensrvTestCases::~CSensrvTestCases" ) );
    if( !iIsPanicked )
        {
        if( iChannelFinder )
            {
            RDebug::Print( _L( "CSensrvTestCases::~CSensrvTestCases - delete iChannelFinder" ) );
            delete iChannelFinder;
            iChannelFinder = NULL;
            }
        if( iSensorChannel )
            {
            RDebug::Print( _L( "CSensrvTestCases::~CSensrvTestCases - delete iSensorChannel" ) );
            delete iSensorChannel;
            iSensorChannel = NULL;
            }

        }

    iConditionList.ResetAndDestroy();
    iConditionSetList.ResetAndDestroy();

    delete iConditionEvaluator;

    iConditionListeningExpectedValuesArray.Close();

    RDebug::Print( _L( "CSensrvTestCases::~CSensrvTestCases - iChannelInfoList.Close()" ) );
    iChannelInfoList.Close();
    iStoredInfoList.Close();

    RDebug::Print( _L( "CSensrvTestCases::~CSensrvTestCases - iThread.Close()" ) );
    iThread.Close();

    RDebug::Print( _L( "CSensrvTestCases::~CSensrvTestCases - iMutex.Close()" ) );
    iMutex.Close();

    }

// -----------------------------------------------------------------------------
// CSensrvTestCases::StartTestCase
//
// -----------------------------------------------------------------------------
//
TInt CSensrvTestCases::StartTestCase( TSensrvTestCase aTestCase,
                                        CStifItemParser& aItem,
                                        TInt aCompletedKey )
    {

    RDebug::Print( _L( "CSensrvTestCases::StartTestCase - aTestCase: %d" ), aTestCase );

    TInt err( KErrNone );

    if ( !IsActive() )
        {
        RDebug::Print( ( _L( "CSensrvTestCases::StartTestCase - ERROR: Not active when attempted completion." ) ) );
        err = KErrNotReady;
        }
    else
        {
        err = ParseParameters( aItem, aTestCase );
        if (err)
            return err;
        iTestCase = aTestCase;
        iCompletedKey = aCompletedKey;
        iTestCaseCompleted = EFalse;

        // Cause RunL to be executed in the owner thread.
        TRequestStatus* status = &iStatus;
        iThread.RequestComplete( status, KErrNone );
        RDebug::Print( ( _L( "CSensrvTestCases::StartTestCase - Completion done." ) ) );
        }

    RDebug::Print( _L( "CSensrvTestCases::StartTestCase - return %d" ), err );

    return err;

    }

// -----------------------------------------------------------------------------
// CSensrvTestCases::StopTest
//
// -----------------------------------------------------------------------------
//
void CSensrvTestCases::StopTest()
    {

    if( !iIsPanicked )
        {
        iMutex.Wait();
        RDebug::Print( _L( "CSensrvTestCases::StopTest" ) );
        // Cause RunL to be executed in the owner thread.
        TRequestStatus* status = &iStatus;
        iMutex.Signal();
        iThread.RequestComplete( status, KErrCancel );
        }

    }

// -----------------------------------------------------------------------------
// CSensrvTestCases::ParseParameters
//
// -----------------------------------------------------------------------------
//
TInt CSensrvTestCases::ParseParameters( CStifItemParser& aItem, TSensrvTestCase aTestCase )
    {
    TInt err( KErrNone );
    switch( aTestCase )
        {
        case EStartChannelChangeListening:
        case EStopChannelChangeListening:
        case EFindChannels:
        case EOpenChannel:
            {
            err = ParseChannelInfo( iChannelInfo, aItem );
            break;
            }
        case EOpenClosePerformance:
            {
            err = aItem.GetNextInt( iOpenCloseCount );
            break;
            }
        case EStartDataListening:
        case EStartListeningWhenChannelIsNotOpen:
            {
            err = aItem.GetNextInt( iDataListeningCount );
            if( !err )
                {
                err = aItem.GetNextInt( iDataListeningTimeout );
                }
            if( !err )
                {
                err = aItem.GetNextInt( iDataListeningRepeatCount );
                }
            break;
            }
        case ESetProperty:
            {
			RDebug::Print( _L( "CSensrvTestCases::ESetProperty" ) );
            err = aItem.GetNextInt( iSetPropertyIdTInt );
            TInt index(0);
            if( !err )
                {
                err = aItem.GetNextInt( index );
                RDebug::Print( _L( "CSensrvTestCases::ESetProperty - get index" ) );
                }
            else
            	{
				RDebug::Print( _L( "CSensrvTestCases::ESetProperty - error getting iSetPropertyIdTInt" ) );
				}
            if( !err )
                {
                if( iSetPropertyIdTInt == KSensrvPropIdDataRate )
                    {
                    TInt value(0);
                    aItem.GetNextInt( value );
                    iPropDataRate.SetValue( value );
                    iPropDataRate.SetItemIndex( index );
                    }
                else if( iSetPropertyIdTInt == KSensrvPropIdMeasureRange )
                    {
                    TInt intValue(0);
                    aItem.GetNextInt( intValue );
                    TReal value( intValue );
                    iPropMeasureRange.SetValue( value );
                    iPropMeasureRange.SetItemIndex( index );
                    }
                else if( iSetPropertyIdTInt == KSensrvSensorDescription )
                    {
                    TBuf8<KSensrvPropertyTextBufferSize> value;
                    TPtrC16 ptr;
                    aItem.GetNextString( ptr );
                    value.Copy( ptr );
                    iPropSensorDescription.SetValue( value );
                    iPropSensorDescription.SetItemIndex( index );
                    }
                else if( iSetPropertyIdTInt == KSensrvPropIdChannelScale )
                    {
                    TInt value(0);
                    aItem.GetNextInt( value );
                    iPropChannelScale.SetValue( value );
                    iPropChannelScale.SetItemIndex( index );
                    }
                else if( iSetPropertyIdTInt == KSensrvPropIdAvailability )
                    {
                    TInt value(0);
                    aItem.GetNextInt( value );
                    iPropAvailability.SetValue( value );
                    iPropAvailability.SetItemIndex( KSensrvItemIndexNone );
                    }
                }
            break;
            }

        case EGetProperty:
            {
            // Get parameters
            err = aItem.GetNextInt( iGetPropertyIdTInt );
            if( !err )
                {
                err = aItem.GetNextInt( iGetItemIndex );
                }
            if( !err )
                {
                if( iGetPropertyIdTInt == KSensrvPropIdDataRate )
                    {
                    TInt value(0);
                    aItem.GetNextInt( value );
                    iPropDataRate.SetValue( value );
                    iPropDataRate.SetItemIndex( iGetItemIndex );
                    }
                else if( iGetPropertyIdTInt == KSensrvPropIdMeasureRange )
                    {
                    TInt intValue(0);
                    aItem.GetNextInt( intValue );
                    TReal value( intValue );
                    iPropMeasureRange.SetValue( value );
                    iPropMeasureRange.SetItemIndex( iGetItemIndex );
                    }
                else if( iGetPropertyIdTInt == KSensrvSensorDescription )
                    {
                    TBuf8<KSensrvPropertyTextBufferSize> value;
                    TPtrC16 ptr;
                    aItem.GetNextString( ptr );
                    value.Copy( ptr );
                    iPropSensorDescription.SetValue( value );
                    iPropSensorDescription.SetItemIndex( iGetItemIndex );
                    }
                else if( iGetPropertyIdTInt == KSensrvPropIdAvailability )
                    {
                    TInt value(0);
                    aItem.GetNextInt( value );
                    iPropAvailability.SetValue( value );
                    iPropAvailability.SetItemIndex( iGetItemIndex );
                    }
                }
            }
            break;

        case ECreateNewProcess:
            {
            err = aItem.GetNextInt( iProcessCount );
            }
            break;
        case EAddCondition:
            {
            err = aItem.GetNextInt( iConditionType );

            if( !err )
                {
                err = aItem.GetNextInt( iConditionOperator );
                }
            if( !err )
                {
                err = aItem.GetNextInt( iConditionIndex );
                }
            if( !err )
                {
                err = aItem.GetNextInt( iConditionValue );
                }
            if( !err )
                {
                err = aItem.GetNextInt( iConditionChannelType );
                }
            break;
            }

        case EAddConditionSet:
            {
            err = aItem.GetNextInt( iConditionSetType );

            // Optional params
            if( !err )
                {
                TInt optErr = aItem.GetNextInt( iConditionSetAddTwice );

                if (optErr != KErrNone)
                    {
                    iConditionSetAddTwice = 0;
                    }

                optErr = aItem.GetNextInt(iConditionSetReadd);

                if (optErr != KErrNone)
                    {
                    iConditionSetReadd = 0;
                    }
                }


            break;
            }
        case ERemoveConditions:
            {
            // Optional parameter
            TInt optErr = aItem.GetNextInt( iConditionRemoveNonexistent );

            if (optErr != KErrNone)
                {
                iConditionRemoveNonexistent = 0;
                }
            break;
            }

        case EStartConditionListening:
            {
            err = aItem.GetNextInt( iConditionListeningCount );
            if( !err )
                {
                err = aItem.GetNextInt( iConditionListeningTimeout );
                }
            if( !err )
                {
                err = aItem.GetNextInt( iConditionListeningRepeatCount );
                }
            // Optional: Get expected values if they are defined
            TInt optionalErr(KErrNone);
            iConditionListeningExpectedValuesArray.Reset();
            while (!optionalErr)
                {
                TInt value;
                optionalErr = aItem.GetNextInt( value );
                if (!optionalErr)
                    {
                    iConditionListeningExpectedValuesArray.Append(value);
                    }
                }
            break;
            }

        case EWaitSSYUnloading:
            {
            err = aItem.GetNextInt( iExpectedUnloads );
            break;
            }

        default:
            {
            break;
            }
        }

    if( err )
        {
        RDebug::Print( _L("CSensrvTestCases::ParseParameters: parameter error = %d"), err );
        }
    return err;
    }


// -----------------------------------------------------------------------------
// CSensrvTestCases::PropertyChangedCount
//
// -----------------------------------------------------------------------------
//
TInt CSensrvTestCases::PropertyChangedCount()
    {
    iMutex.Wait();
    TInt count = iPropertyChangedCount;
    iMutex.Signal();
    return count;
    }

// -----------------------------------------------------------------------------
// CSensrvTestCases::ClearPropertyChangedCount
//
// -----------------------------------------------------------------------------
//
void CSensrvTestCases::ClearPropertyChangedCount()
    {
    iMutex.Wait();
    iPropertyChangedCount = 0;
    iMutex.Signal();
    }

// -----------------------------------------------------------------------------
// CSensrvTestCases::ConditionMetCount
//
// -----------------------------------------------------------------------------
//
TInt CSensrvTestCases::ConditionMetCount()
    {
    iMutex.Wait();
    TInt count = iConditionListeningRepeatCounter;
    iMutex.Signal();
    return count;
    }


// -----------------------------------------------------------------------------
// CSensrvTestCases::FoundChannelCount
//
// -----------------------------------------------------------------------------
//
TInt CSensrvTestCases::FoundChannelCount()
    {
    iMutex.Wait();
    TInt count = iChannelInfoList.Count();
    iMutex.Signal();
    return count;
    }

// -----------------------------------------------------------------------------
// CSensrvTestCases::GetSetPropertySuccessIndicatorCount
//
// -----------------------------------------------------------------------------
//
TInt CSensrvTestCases::GetSetPropertySuccessIndicatorCount()
    {
    iMutex.Wait();
    TInt count = iSetPropertySuccessIndicatorCount;
    iMutex.Signal();
    return count;
    }

// -----------------------------------------------------------------------------
// CSensrvTestCases::GetSetPropertySuccessIndicator
//
// -----------------------------------------------------------------------------
//
TInt CSensrvTestCases::GetSetPropertySuccessIndicator()
    {
    iMutex.Wait();
    TInt ind = iSetPropertySuccessIndicator;
    iMutex.Signal();
    return ind;
    }

// -----------------------------------------------------------------------------
// CSensrvTestCases::ClearSetPropertySuccessIndicator
//
// -----------------------------------------------------------------------------
//
void CSensrvTestCases::ClearSetPropertySuccessIndicator()
    {
    iMutex.Wait();
    iSetPropertySuccessIndicatorCount = 0;
    iSetPropertySuccessIndicator = ESetPropertyIndicationUnknown;
    iMutex.Signal();
    }

// -----------------------------------------------------------------------------
// CSensrvTestCases::DoCancel
//
// -----------------------------------------------------------------------------
//
void CSensrvTestCases::DoCancel()
    {

    }

// -----------------------------------------------------------------------------
// CSensrvTestCases::RunL
//
// -----------------------------------------------------------------------------
//
void CSensrvTestCases::RunL()
    {
    RDebug::Print( _L("CSensrvTestCases::RunL" ) );
    if( iStatus.Int() == KErrCancel )
        {
        RDebug::Print( _L("CSensrvTestCases::RunL - iStatus = KErrCancel" ) );
        if( iSensorChannel )
            {
            if( iChannelOpen )
                {
                RDebug::Print( _L("CSensrvTestCases::RunL - Close channel" ) );
                iSensorChannel->CloseChannel();
                }
            RDebug::Print( _L("CSensrvTestCases::RunL - Delete iSensorChannel" ) );
            delete iSensorChannel;
            iSensorChannel = NULL;
            }
        if( iChannelFinder )
            {
            RDebug::Print( _L("CSensrvTestCases::RunL - Delete iChannelFinder" ) );
            delete iChannelFinder;
            iChannelFinder = NULL;
            }
        iTestCaseCompleted = ETrue;
        RDebug::Print( _L("CSensrvTestCases::RunL - Stop CActiveScheduler" ) );
        CActiveScheduler::Stop();
        return;
        }

    if( iTestCase != EPanicClient )
        {
        iStatus = KRequestPending;
        SetActive();
        }

    TInt err( KErrNone );
    switch( iTestCase )
        {
        case ECreateAndDestroyChannelFinder:
            {
            RDebug::Print( _L("CSensrvTestCases::RunL - ECreateAndDestroyChannelFinder" ) );
            err = CreateAndDestroyChannelFinderL();
            iTestCaseCompleted = ETrue;
            RDebug::Print( _L("CSensrvTestCases::RunL - ECreateAndDestroyChannelFinder %d" ), err );
            SetResult( err );
            break;
            }
        case EFindChannels:
            {
            RDebug::Print( _L("CSensrvTestCases::RunL - EFindChannels" ) );
            err = FindChannelsL();
            iTestCaseCompleted = ETrue;
            RDebug::Print( _L("CSensrvTestCases::RunL - EFindChannels completed %d" ), err );
            SetResult( err );
            break;
            }
        case EOpenChannel:
            {
            RDebug::Print( _L("CSensrvTestCases::RunL - EOpenChannel" ) );
            err = OpenChannelL();
            iTestCaseCompleted = ETrue;
            RDebug::Print( _L("CSensrvTestCases::RunL - EOpenChannel completed %d" ), err );
            SetResult( err );
            break;
            }
        case ECloseChannel:
            {
            RDebug::Print( _L("CSensrvTestCases::RunL - ECloseChannel" ) );
            err = CloseChannelL();
            iTestCaseCompleted = ETrue;
            RDebug::Print( _L("CSensrvTestCases::RunL - ECloseChannel completed %d" ), err );
            SetResult( err );
            break;
            }
        case EOpenClosePerformance:
            {
            RDebug::Print( _L("CSensrvTestCases::RunL - EOpenClosePerformance" ) );
            err = OpenClosePerformanceL();
            iTestCaseCompleted = ETrue;
            RDebug::Print( _L("CSensrvTestCases::RunL - EOpenClosePerformance completed %d" ), err );
            SetResult( err );
            break;
            }
        case EStartDataListening:
            {
            RDebug::Print( _L("CSensrvTestCases::RunL - EStartDataListening" ) );
            err = StartDataListeningL();
            RDebug::Print( _L("CSensrvTestCases::RunL - EStartDataListening completed %d" ), err );
            SetResult( err );
            break;
            }
        case EStopDataListening:
            {
            RDebug::Print( _L("CSensrvTestCases::RunL - EStopDataListening" ) );
            err = StopDataListeningL();
            iTestCaseCompleted = ETrue;
            RDebug::Print( _L("CSensrvTestCases::RunL - EStopDataListening completed %d" ), err );
            SetResult( err );
            break;
            }
        case EStartListeningWhenChannelIsNotOpen:
            {
            RDebug::Print( _L("CSensrvTestCases::RunL - EStartListeningWhenChannelIsNotOpen" ) );
            err = StartListeningWhenChannelIsNotOpenL();
            RDebug::Print( _L("CSensrvTestCases::RunL - EStartListeningWhenChannelIsNotOpen completed %d" ), err );
            SetResult( err );
            break;
            }
        case EPanicClient:
            {
            RDebug::Print( _L("CSensrvTestCases::RunL - EPanicClient" ) );
            err = PanicClient();
            break;
            }
        case EGetProperty:
            {
            RDebug::Print( _L("CSensrvTestCases::RunL - EGetProperty" ) );
            err = GetPropertyL();
            SetResult( err );
            break;
            }
        case ESetProperty:
            {
            RDebug::Print( _L("CSensrvTestCases::RunL - ESetProperty" ) );
            err = SetPropertyL();
            SetResult( err );
            break;
            }
        case EStartPropertyListening:
            {
            RDebug::Print( _L("CSensrvTestCases::RunL - EStartPropertyListening" ) );
            err = StartPropertyListeningL();
            SetResult( err );
            }
            break;
        case EStopPropertyListening:
            {
            RDebug::Print( _L("CSensrvTestCases::RunL - EStopPropertyListening" ) );
            err = StopPropertyListeningL();
            SetResult( err );
            }
            break;
        case EGetAllProperties:
            {
            RDebug::Print( _L("CSensrvTestCases::RunL - EGetAllProperties" ) );
            err = GetAllPropertiesL();
            SetResult( err );
            break;
            }
        case EGetAllPropertiesWithPropertyIdL:
            {
            RDebug::Print( _L("CSensrvTestCases::RunL - EGetAllPropertiesWithPropertyIdL" ) );
            err = GetAllPropertiesWithPropertyIdL();
            SetResult( err );
            break;
            }

        case EGetAllPropertiesWithArrayIndexL:
            {
            RDebug::Print( _L("CSensrvTestCases::RunL - EGetAllPropertiesWithArrayIndexL" ) );
            err = GetAllPropertiesWithArrayIndexL();
            SetResult( err );
            break;
            }

        case EWaitSSYUnloading:
            {
            RDebug::Print( _L("CSensrvTestCases::RunL - EWaitSSYUnloading" ) );
            err = WaitSSYUnloadingL();
            RDebug::Print( _L("CSensrvTestCases::RunL - EWaitSSYUnloading completed %d" ), err );
            SetResult( err );
            break;
            }
        case ECreateNewProcess:
            {
            RDebug::Print( _L("CSensrvTestCases::RunL - ECreateNewProcess" ) );
            err = CreateNewProcessL();
            RDebug::Print( _L("CSensrvTestCases::RunL - ECreateNewProcess completed %d" ), err );
            SetResult( err );
            break;
            }
        case EKillProcess:
            {
            RDebug::Print( _L("CSensrvTestCases::RunL - EKillProcess" ) );
            err = KillProcess();
            RDebug::Print( _L("CSensrvTestCases::RunL - EKillProcess completed %d" ), err );
            SetResult( err );
            break;
            }
        case EAddCondition:
            {
            RDebug::Print( _L("CSensrvTestCases::RunL - EAddCondition" ) );
            err = AddConditionL();
            RProperty::Set( KPSUidSensrvTest, iCompletedKey, err );
            break;
            }
        case EAddConditionSet:
            {
            RDebug::Print( _L("CSensrvTestCases::RunL - EAddConditionSet" ) );
            err = AddConditionSetL();
            RProperty::Set( KPSUidSensrvTest, iCompletedKey, err );
            break;
            }
        case ERemoveConditions:
            {
            RDebug::Print( _L("CSensrvTestCases::RunL - ERemoveConditions" ) );
            err = RemoveConditionsL();
            RProperty::Set( KPSUidSensrvTest, iCompletedKey, err );
            break;
            }
        case EStartConditionListening:
            {
            RDebug::Print( _L("CSensrvTestCases::RunL - EStartConditionListening" ) );
            err = StartConditionListeningL();
            RDebug::Print( _L("CSensrvTestCases::RunL - EStartConditionListening completed %d" ), err );
            SetResult( err );
            break;
            }
        case EStopConditionListening:
            {
            RDebug::Print( _L("CSensrvTestCases::RunL - EStopConditionListening" ) );
            err = StopConditionListeningL();
            iTestCaseCompleted = ETrue;
            RDebug::Print( _L("CSensrvTestCases::RunL - EStopConditionListening completed %d" ), err );
            SetResult( err );
            break;
            }
        case EStartChannelChangeListening:
            {
            RDebug::Print( _L("CSensrvTestCases::RunL - EStartChannelChangeListening" ) );
            err = StartChannelChangeListeningL();
            SetResult( err );
            }
            break;
        case EStopChannelChangeListening:
            {
            RDebug::Print( _L("CSensrvTestCases::RunL - EStopChannelChangeListening" ) );
            err = StopChannelChangeListeningL();
            SetResult( err );
            }
            break;
        }

    // do nothing here

    }

// -----------------------------------------------------------------------------
// CSensrvTestCases::RunError
//
// -----------------------------------------------------------------------------
//
TInt CSensrvTestCases::RunError( TInt aError )
    {
    RDebug::Print( _L("CSensrvTestCases::RunError iTestCase: %d, aError: %d" ), iTestCase, aError );
    RProperty::Set( KPSUidSensrvTest, iCompletedKey, aError );
    iStatus = KRequestPending;
    SetActive();
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CSensrvTestCases::SetResult
//
// -----------------------------------------------------------------------------
//
void CSensrvTestCases::SetResult( TInt aResult )
    {
    iMutex.Wait();
    RDebug::Print( _L("CSensrvTestCases::SetResult - aResult %d" ), aResult );
    TInt err = RProperty::Set( KPSUidSensrvTest, iCompletedKey, aResult );
    RDebug::Print( _L("CSensrvTestCases::SetResult - err = %d" ), err );
    iMutex.Signal();
    }


// -----------------------------------------------------------------------------
// CSensrvTestCases::CreateAndDestroyChannelFinderL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CSensrvTestCases::CreateAndDestroyChannelFinderL()
    {

    RDebug::Print( _L("CSensrvTestCases::CreateAndDestroyChannelFinderL" ) );

    // Create channel finder
    RDebug::Print( _L("CSensrvTestCases::CreateAndDestroyChannelFinderL: Create iChannelFinder") );
    iChannelFinder = CSensrvChannelFinder::NewL();
    RDebug::Print( _L("CSensrvTestCases::CreateAndDestroyChannelFinderL: iChannelFinder created") );

    RDebug::Print( _L("CSensrvTestCases::CreateAndDestroyChannelFinderL: Delete iChannelFinder") );
    delete iChannelFinder;
    iChannelFinder = NULL;
    RDebug::Print( _L("CSensrvTestCases::CreateAndDestroyChannelFinderL: iChannelFinder deleted") );

    return KErrNone;

    }

// -----------------------------------------------------------------------------
// CSensrvTestCases::FindChannelsL
// Parameters: ChannelId(TUint32), ContextType(TInt), Quantity(TInt),
//              ChannelType(TUint32), dataSize(TInt), Location(TDesC8), VendorId(TDesC8)(optional)
//
// -----------------------------------------------------------------------------
//
TInt CSensrvTestCases::FindChannelsL()
    {
    RDebug::Print( _L("CSensrvTestCases::FindChannelsL") );


    TInt err;

    iChannelInfoList.Reset();

    err = FindChannels( iChannelInfoList, iChannelInfo );
    if( err )
        {
        RDebug::Print( _L("CSensrvTestCases::FindChannelsL: FindChanels error: %d"), err );
        return err;
        }
    err = CheckFindResult( iChannelInfo );
    if( err )
        {
        RDebug::Print( _L("CSensrvTestCases::FindChannelsL: CheckFindResult error: %d"), err );
        return err;
        }

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CSensrvTestCases::FindChannels
//
// -----------------------------------------------------------------------------
//
TInt CSensrvTestCases::FindChannels( RSensrvChannelInfoList& aChannelInfoList, TSensrvChannelInfo& aSearchConditions )
    {
    RDebug::Print( _L("CSensrvTestCases::FindChanels: Find channels...") );
    if( !iChannelFinder )
        {
        // Create channel finder
        RDebug::Print( _L("CSensrvTestCases::FindChanels: Create iChannelFinder") );
        iChannelFinder = CSensrvChannelFinder::NewL();
        }

    TRAPD( err, iChannelFinder->FindChannelsL( aChannelInfoList, aSearchConditions ) );
    if( err )
        {
        RDebug::Print( _L("CSensrvTestCases::FindChanels: Leave: %d"), err );
        return err;
        }
    RDebug::Print( _L("CSensrvTestCases::FindChanels: Channel count: %d"), aChannelInfoList.Count() );

    return KErrNone;
    }


// -----------------------------------------------------------------------------
// CSensrvTestCases::CheckFindResult
//
// -----------------------------------------------------------------------------
//
TInt CSensrvTestCases::CheckFindResult( TSensrvChannelInfo& aSearchConditions )
    {
    RDebug::Print( _L("CSensrvTestCases::CheckFindResult") );
    RDebug::Print( _L("### conditions: ChannelId=%d, ContextType=%d, Quantity=%d, ChannelType=%d, Location=%S, VendorId=%S, DataItemSize=%d"),
                   aSearchConditions.iChannelId, aSearchConditions.iContextType, aSearchConditions.iQuantity, aSearchConditions.iChannelType,
                   &aSearchConditions.iLocation, &aSearchConditions.iVendorId, aSearchConditions.iDataItemSize );

    // Find all channels
    TBufC8<1> nullBuf( KNullDesC8 );
    TSensrvChannelInfo channelInfo( 0, ESensrvContextTypeNotDefined, ESensrvQuantityNotdefined, 0, nullBuf, nullBuf, 0, 0 );
    RSensrvChannelInfoList allChannelList(10);
    TInt err = FindChannels( allChannelList, channelInfo );
    if( err )
        {
        RDebug::Print( _L("CSensrvTestCases::CheckFindResult: Find all channels Leave: %d"), err );
        return err;
        }

    //
    RSensrvChannelInfoList acceptedChannels(10);
    for( TInt i = 0; i < allChannelList.Count(); i++ )
        {
        if( allChannelList[i].IsMatch( aSearchConditions ) )
            {
            acceptedChannels.Append( allChannelList[i] );
            }
        }

    if( acceptedChannels.Count() != iChannelInfoList.Count() )
        {
        // Test fails.
        RDebug::Print( _L("CSensrvTestCases::CheckFindResult: Test fails") );
        RDebug::Print( _L("   required count %d"), acceptedChannels.Count() );
        RDebug::Print( _L("   found channels count %d"), iChannelInfoList.Count() );
        return KErrGeneral;
        }

    TBool testFail( EFalse );
    // Check all channels infos
    for( TInt i = 0; i < iChannelInfoList.Count(); i++ )
        {
        RDebug::Print( _L("     ###channel info [%d]:"), i );
        RDebug::Print( _L("     ### ChannelId=%d, ContextType=%d, Quantity=%d, ChannelType=%d, Location=%S, VendorId=%S, DataItemSize=%d"),
                   iChannelInfoList[i].iChannelId, iChannelInfoList[i].iContextType, iChannelInfoList[i].iQuantity, iChannelInfoList[i].iChannelType,
                   &iChannelInfoList[i].iLocation, &iChannelInfoList[i].iVendorId, iChannelInfoList[i].iDataItemSize );

        TBool found( EFalse );
        for( TInt x = 0; x < acceptedChannels.Count(); x++ )
            {
            if( acceptedChannels[x].iChannelId == iChannelInfoList[i].iChannelId )
                {
                found = ETrue;
                break;
                }

            }
        if( found )
            {
            RDebug::Print( _L("     -OK-") );
            }
        else
            {
            RDebug::Print( _L("     -FAIL-") );
            testFail = ETrue;
            }

        }

    if( testFail )
        {
        return KErrGeneral;
        }
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CSensrvTestCases::OpenChannelL
// Parameters: ChannelId(TUint32), ContextType(TInt), Quantity(TInt),
//              ChannelType(TUint32), dataSize(TInt), Location(TDesC8), VendorId(TDesC8)(optional)
//
// -----------------------------------------------------------------------------
//
TInt CSensrvTestCases::OpenChannelL()
    {
    RDebug::Print(RThread().Name());
    RDebug::Print( _L("CSensrvTestCases::OpenChannelL") );

    iChannelInfoList.Reset();

    TInt err = FindChannels( iChannelInfoList, iChannelInfo );
    if( err )
        {
        RDebug::Print( _L("CSensrvTestCases::OpenChannelL: FindChanels error: %d"), err );
        return err;
        }
    if( iChannelInfoList.Count() == 0 )
        {
        RDebug::Print( _L("CSensrvTestCases::FindChannelsL: Channel not found" ) );
        return KErrNotFound;
        }
    if( !iSensorChannel )
        {
        RDebug::Print( _L("CSensrvTestCases::OpenChannelL - Create iSensorChannel...") );
        RDebug::Print( _L("CSensrvTestCases::OpenChannelL - ChannelId = %d "), iChannelInfoList[ 0 ].iChannelId );
        iSensorChannel = CSensrvChannel::NewL( iChannelInfoList[ 0 ] );
        RDebug::Print( _L("CSensrvTestCases::OpenChannelL - iSensorChannel Created") );
        }

    TBuf<16> location;
    location.Copy( iSensorChannel->GetChannelInfo().iLocation );
    RDebug::Print( _L("CSensrvTestCases::OpenChannelL - Channel %S"), &location );
    TRAP( err, iSensorChannel->OpenChannelL() );
    TInt openedChannel( 0 );
    RProperty::Get( KPSUidSensrvTest, KSensrvLatestOpenedChannel, openedChannel );
    if( err )
        {
        RDebug::Print( _L("CSensrvTestCases::OpenChannelL - Open channel error %d"), err );
        }
    else if( openedChannel != iChannelInfoList[ 0 ].iChannelId )
        {
        RDebug::Print( _L("CSensrvTestCases::OpenChannelL - Incorrect channel opened."), err );
        RDebug::Print( _L("CSensrvTestCases::OpenChannelL - opened hannel: %d"), openedChannel );
        //err = KErrGeneral;
        }
    else
        {
        RDebug::Print( _L("CSensrvTestCases::OpenChannelL - Channel open") );
        iChannelOpen = ETrue;
        }

    return err;
    }


// -----------------------------------------------------------------------------
// CSensrvTestCases::CloseChannelL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CSensrvTestCases::CloseChannelL()
    {
    RDebug::Print(RThread().Name());
    RDebug::Print( _L("CSensrvTestCases::CloseChannelL") );
    if( !iSensorChannel )
        {
        RDebug::Print( _L("CSensrvTestCases::CloseChannelL - Channel not found. Return KErrNotFound") );
        return KErrNotFound;
        }

    TBuf<16> location;
    location.Copy( iSensorChannel->GetChannelInfo().iLocation );
    RDebug::Print( _L("CSensrvTestCases::CloseChannelL - Channel %S"), &location );
    iSensorChannel->CloseChannel();

    TInt closedChannel( 0 );
    RProperty::Get( KPSUidSensrvTest, KSensrvLatestClosedChannel, closedChannel );

    if( closedChannel != iChannelInfoList[ 0 ].iChannelId )
        {
        RDebug::Print( _L("CSensrvTestCases::CloseChannelL - Incorrect channel closed.") );
        RDebug::Print( _L("CSensrvTestCases::OpenChannelL - closed hannel: %d"), iChannelInfoList[ 0 ].iChannelId );
        //err = KErrGeneral;
        }
    else
        {
        RDebug::Print( _L("CSensrvTestCases::CloseChannelL - Channel closed") );
        }

    delete iChannelFinder;
    iChannelFinder = NULL;

    iChannelOpen = EFalse;

    // Delete the channel or next open will use old parameters.
    delete iSensorChannel;
    iSensorChannel = NULL;

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CSensrvTestCases::OpenChannelL
// Parameters: Open-Close Count(TInt)
//
// -----------------------------------------------------------------------------
//
TInt CSensrvTestCases::OpenClosePerformanceL()
    {
    RDebug::Print( _L("CSensrvTestCases::OpenClosePerformanceL") );

    TInt err;

    TBufC8<1> nullBuf( KNullDesC8 );
    TSensrvChannelInfo channelInfo( 0, ESensrvContextTypeNotDefined, ESensrvQuantityNotdefined, 0, nullBuf, nullBuf, 0, 0 );
    iChannelInfoList.Reset();
    err = FindChannels( iChannelInfoList, channelInfo );
    if( err )
        {
        RDebug::Print( _L("CSensrvTestCases::OpenClosePerformanceL: FindChanels error: %d"), err );
        return err;
        }
    if( iChannelInfoList.Count() == 0 )
        {
        RDebug::Print( _L("CSensrvTestCases::OpenClosePerformanceL: Channel not found" ) );
        return KErrNotFound;
        }
    if( !iSensorChannel )
        {
        RDebug::Print( _L("CSensrvTestCases::OpenClosePerformanceL - Create iSensorChannel...") );
        RDebug::Print( _L("CSensrvTestCases::OpenClosePerformanceL - ChannelId = %d "), iChannelInfoList[ 0 ].iChannelId );
        iSensorChannel = CSensrvChannel::NewL( iChannelInfoList[ 0 ] );
        RDebug::Print( _L("CSensrvTestCases::OpenClosePerformanceL - iSensorChannel Created") );
        }

    TTime startTime;
    startTime.HomeTime();

    for( TInt i = 1; i <= iOpenCloseCount; ++i )
        {
        TRAP( err, iSensorChannel->OpenChannelL() );
        if( err )
            {
            //RDebug::Print( _L("CSensrvTestCases::OpenClosePerformanceL - Open channel error %d"), err );
            }

        iSensorChannel->CloseChannel();

        //RDebug::Print( _L("CSensrvTestCases::OpenClosePerformanceL - Open-Close [%d]"), i );

        }

    TTime endTime;
    endTime.HomeTime();

    TTimeIntervalMicroSeconds overallTime =  endTime.MicroSecondsFrom( startTime );

    RDebug::Print( _L("CSensrvTestCases::OpenClosePerformanceL - Overall time %d micro seconds"), overallTime.Int64() );


    return KErrNone;

    }

// -----------------------------------------------------------------------------
// CSensrvTestCases::StartDataListening
// Parameters: dataBlockCount, timeout, repeatCount
//
// -----------------------------------------------------------------------------
//
TInt CSensrvTestCases::StartDataListeningL()
    {
    TInt err;

    iDataListeningRepeatCounter = 0;

    if( !iSensorChannel )
        {
        RDebug::Print( _L("CSensrvTestCases::StartDataListeningL- Channel not found ") );
        return KErrNotFound;
        }

    TBuf<16> location;
    location.Copy( iSensorChannel->GetChannelInfo().iLocation );
    RDebug::Print( _L("CSensrvTestCases::StartDataListeningL: Channel: %S"), &location );

    RDebug::Print( _L("CSensrvTestCases::StartDataListeningL - count=%d, timeout=%d"), iDataListeningCount, iDataListeningTimeout );
    TRAP( err, iSensorChannel->StartDataListeningL( this, iDataListeningCount, iDataListeningCount*3, iDataListeningTimeout ) );
    if( err )
        {
        RDebug::Print( _L("CSensrvTestCases::StartDataListeningL-  StartDataListening error %d"), err );
        }
    return err;
    }

// -----------------------------------------------------------------------------
// CSensrvTestCases::StopDataListening
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CSensrvTestCases::StopDataListeningL()
    {
    if( !iSensorChannel )
        {
        RDebug::Print( _L("CSensrvTestCases::StopDataListeningL- Channel not found ") );
        return KErrNotFound;
        }

    TBuf<16> location;
    location.Copy( iSensorChannel->GetChannelInfo().iLocation );
    RDebug::Print( _L("CSensrvTestCasesCases::StopDataListeningL: Channel: %S"), &location );
    iSensorChannel->StopDataListening();

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CSensrvTestCases::StartListeningWhenChannelIsNotOpenL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CSensrvTestCases::StartListeningWhenChannelIsNotOpenL()
    {
    RDebug::Print(RThread().Name());
    RDebug::Print( _L("CSensrvTestCases::StartListeningWhenChannelIsNotOpenL") );
    if( iSensorChannel )
        {
        delete iSensorChannel;
        iSensorChannel = NULL;
        }

    if( iChannelInfoList.Count() == 0 )
        {
        RDebug::Print( _L("CSensrvTestCases::StartListeningWhenChannelIsNotOpenL: Channel not found" ) );
        return KErrNotFound;
        }

    // Create channel client
    RDebug::Print( _L("CSensrvTestCases::StartListeningWhenChannelIsNotOpenL - Create iSensorChannel...") );
    RDebug::Print( _L("CSensrvTestCases::StartListeningWhenChannelIsNotOpenL - ChannelId = %d "), iChannelInfoList[ 0 ].iChannelId );
    iSensorChannel = CSensrvChannel::NewL( iChannelInfoList[ 0 ] );
    RDebug::Print( _L("CSensrvTestCases::StartListeningWhenChannelIsNotOpenL - iSensorChannel Created") );

    // Start listening
    TRAPD( err, iSensorChannel->StartDataListeningL( this, iDataListeningCount, iDataListeningCount*3, iDataListeningTimeout ) );

    RDebug::Print( _L("CSensrvTestCases::StartListeningWhenChannelIsNotOpenL-  StartDataListening: %d"), err );

    return err;

    }


// -----------------------------------------------------------------------------
// CSensrvTestCases::PanicClient
// Parameters:
//
// -----------------------------------------------------------------------------
//
TInt CSensrvTestCases::PanicClient()
    {
    RDebug::Print( _L("CSensrvTest::PanicClient") );

    SetResult( KErrCancel );

    iIsPanicked = ETrue;

    User::Panic( _L("TESTPANIC"), 9999 );

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CSensrvTestCases::WaitSSYUnloadingL
// Parameters:
//
// -----------------------------------------------------------------------------
//
TInt CSensrvTestCases::WaitSSYUnloadingL()
    {
    RDebug::Print( _L("CSensrvTest::WaitSSYUnloadingL") );

    // Open settings repository and get SSY unloading time out
    CRepository* repository = NULL;
    TInt timeout = KSensrvSsyInactivityPeriod;
    TInt err( KErrNone );
    TRAP( err, repository = CRepository::NewL( KCRUidSensorServerSettings ) );

    if ( err == KErrNone )
        {
        TInt value(0);
        err = repository->Get( KSensrvSsyInactivityPeriodKey, value );
        if ( err == KErrNone )
            {
            if ( value > 0 )
                {
                timeout = value * KSecondsToMicros;
                }
            }
        else
            {
            RDebug::Print( _L( "CSensrvTest::WaitSSYUnloadingL - KSensrvSsyInactivityPeriodKey value getting failed: %d, using default value." ), err );
            }

        }
    else
        {
        RDebug::Print( _L( "CSensrvTest::WaitSSYUnloadingL - Settings repository opening failed: %d" ), err );
        }

    timeout += 2 * KSecondsToMicros;
    RDebug::Print( _L("CSensrvTest::WaitSSYUnloadingL - Wait %d microsecond...."), timeout );
    User::After( timeout );
    RDebug::Print( _L("CSensrvTest::WaitSSYUnloadingL - Wait completed") );

    TInt unloadingCount( 0 );
    err = RProperty::Get( KPSUidSensrvTest, KSensrvSSYUnloadingCount, unloadingCount );
    if( err == KErrNone )
        {
        RDebug::Print( _L("CSensrvTest::WaitSSYUnloadingL - SSY unloading count %d, expected %d"), unloadingCount, iExpectedUnloads );
        if( unloadingCount != iExpectedUnloads )
            {
            RDebug::Print( _L("CSensrvTest::WaitSSYUnloadingL - SSY unloading error") );
            err = KErrGeneral;
            }
        }
    else
        {
        RDebug::Print( _L("CSensrvTest::WaitSSYUnloadingL - Get KSensrvSSYUnloadingCount error %d"), err );
            }

    return err;

    }

// -----------------------------------------------------------------------------
// CSensrvTestCases::ParseChannelInfo
// Parameters: ChannelId(TUint32), ContextType(TInt), Quantity(TInt),
//              ChannelType(TUint32), dataSize(TInt), Location(TDesC8), VendorId(TDesC8)(optional)
//
// -----------------------------------------------------------------------------
//
TInt CSensrvTestCases::ParseChannelInfo( TSensrvChannelInfo& aChannelInfo, CStifItemParser& aItem )
    {
    RDebug::Print( _L("CSensrvTest::ParseChannelInfo") );
    // Get parameters: channelId, contextType, quantity, channelType
    TInt err;
    TUint param;
    RArray<TUint> array(5);

    for( TInt i = 0; i < 5; i++ )
        {
        err = aItem.GetNextInt( param );
        if( err )
            {
            RDebug::Print( _L("CSensrvTest::ParseChannelInfo: parameter[%d] error: %d"), i, err );
            return err;
            }
        RDebug::Print( _L("CSensrvTest::ParseChannelInfo: parameter[%d] = %d"), i, param );
        err = array.Append( param );
        if ( err )
            {
            RDebug::Print( _L("Append failed at position no. %d, and error code is %d."), i, err );
            return err;
            }
        }

    // location
    TPtrC16 location;
    err = aItem.GetNextString( location );
    if( err || !location.Compare(KSensrvNullString))
        {
        location.Set( KNullDesC() );
        }
    RDebug::Print( _L("CSensrvTest::ParseChannelInfo: location = %S"), &location );

    // vendorId
    TPtrC16 vendorId;
    err = aItem.GetNextString( vendorId );
    if( err || !vendorId.Compare(KSensrvNullString) )
        {
        vendorId.Set( KNullDesC() );
        }
    RDebug::Print( _L("CSensrvTest::ParseChannelInfo: vendorId = %S"), &vendorId );

    aChannelInfo.iChannelId = (TUint32)array[0];
    aChannelInfo.iContextType = (TSensrvContextType)array[1];
    aChannelInfo.iQuantity = (TSensrvQuantity)array[2];
    aChannelInfo.iChannelType = (TUint32)array[3];
    aChannelInfo.iLocation.Copy( location );
    aChannelInfo.iVendorId.Copy( vendorId );
    aChannelInfo.iDataItemSize = array[4];

	TInt value(1);
	err = aItem.GetNextInt( value );
	if( err )
		{
		value = 1;
		RDebug::Print( _L( "CSensrvTestCases::ParseChannelInfo KSensrvPropIdAvailability ERR: value = %d )" ), value );
		}

	iPropAvailability.SetValue( value );
	iPropAvailability.SetItemIndex( KSensrvItemIndexNone );
	RDebug::Print( _L( "CSensrvTestCases::ParseChannelInfo KSensrvPropIdAvailability: value = %d )" ), value );

    array.Close();

    return KErrNone;
    }



// -----------------------------------------------------------------------------
// CSensrvTestCases::DataReceived
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSensrvTestCases::DataReceived( CSensrvChannel& aChannel,
                                   TInt aCount,
                                   TInt aDataLost)
    {
    iMutex.Wait();

    if( &aChannel != iSensorChannel )
        {
        RDebug::Print( _L("CSensrvTestCasesCases::DataReceived - Error: incorrect channel" ) );
        RProperty::Set( KPSUidSensrvTest, iCompletedKey, KErrArgument );
        }

    TBuf<16> location;
    location.Copy( aChannel.GetChannelInfo().iLocation );
    RDebug::Print( _L("CSensrvTestCasesCases::DataReceived: Channel: %S"), &location );
    RDebug::Print( _L("CSensrvTestCasesCases::DataReceived[%d]: aCount = %d, aDataLost = %d"), iDataListeningRepeatCounter, aCount, aDataLost );

    if( aChannel.GetChannelInfo().iChannelType == KSensrvChannelTypeIdAccelerometerXYZAxisData )
        {
        if( iDataListeningRepeatCounter == 0 )
            {
            iPreviousAxis_xData = KErrNotFound;
            iPreviousAxis_yData = KErrNotFound;
            iPreviousAxis_zData = KErrNotFound;
            }

        TInt overflowTest( 0 );
        RProperty::Get( KPSUidSensrvTest, KSensrvGetDataOverflowTest, overflowTest );
        if( overflowTest )
            {
            TInt size = sizeof( TSensrvAccelerometerAxisData ) - 2;
            HBufC8* buf = HBufC8::NewL( size );
            TPtr8 ptr( const_cast<TUint8*>( buf->Des().Ptr() ), size );
            TInt err = iSensorChannel->GetData( ptr );
            delete buf;
            buf = NULL;
            if( err )
                {
                RDebug::Print( _L("CSensrvTestCasesCases::DataReceived - GetData Error %d" ), err );
                iSensorChannel->StopDataListening();
                RProperty::Set( KPSUidSensrvTest, iCompletedKey, err );
                iTestCaseCompleted = ETrue;
                iMutex.Signal();
                return;
                }
            }

        TBuf<50> timeBuf;

        for( TInt i = 0; i < aCount; i++ )
            {
            TPckgBuf<TSensrvAccelerometerAxisData> dataBuf;
            TInt err = iSensorChannel->GetData( dataBuf );
            if( err )
                {
                RDebug::Print( _L("CSensrvTestCasesCases::DataReceived - GetData Error %d" ), err );
                iSensorChannel->StopDataListening();
                RProperty::Set( KPSUidSensrvTest, iCompletedKey, err );
                iTestCaseCompleted = ETrue;
                iMutex.Signal();
                return;
                }

            TSensrvAccelerometerAxisData data = dataBuf();
            data.iTimeStamp.FormatL( timeBuf, KDateString );

            TTime now;
            now.HomeTime();

            RDebug::Print( _L(" ### data[%d]: iTimeStamp = %S, iAxisX = %d, iAxisY = %d, iAxisZ = %d,  delay: %ld"), i, &timeBuf, data.iAxisX, data.iAxisY, data.iAxisZ, now.MicroSecondsFrom( data.iTimeStamp.Int64()).Int64() );

            if( iPreviousAxis_xData != KErrNotFound )
                {
                if( i == 0 )
                    {
                    if( iPreviousAxis_xData + aDataLost + 1 != data.iAxisX ||
                        iPreviousAxis_yData + aDataLost + 1 != data.iAxisY ||
                        iPreviousAxis_zData + aDataLost + 1 != data.iAxisZ ||
                        iPreviousTimeStamp > data.iTimeStamp )
                        {
                        RDebug::Print( _L("CSensrvTestCasesCases::DataReceived - Error: Data is invalid" ) );
                        RDebug::Print( _L(" ### expected result: iAxisX = %d, iAxisY = %d, iAxisZ = %d" ),
                                   iPreviousAxis_xData + aDataLost + 1, iPreviousAxis_yData + aDataLost + 1, iPreviousAxis_zData + aDataLost + 1 );

                        iSensorChannel->StopDataListening();
                        RProperty::Set( KPSUidSensrvTest, iCompletedKey, KErrCorrupt );
                        iTestCaseCompleted = ETrue;
                        iMutex.Signal();
                        return;
                        }
                    }
                else
                    {
                    if( iPreviousAxis_xData + 1 != data.iAxisX  ||
                        iPreviousAxis_yData + 1 != data.iAxisY  ||
                        iPreviousAxis_zData + 1 != data.iAxisZ  ||
                        iPreviousTimeStamp > data.iTimeStamp )
                        {
                        RDebug::Print( _L("CSensrvTestCasesCases::DataReceived - Error: Data is invalid" ) );
                        iSensorChannel->StopDataListening();
                        RProperty::Set( KPSUidSensrvTest, iCompletedKey, KErrCorrupt );
                        iTestCaseCompleted = ETrue;
                        iMutex.Signal();
                        return;
                        }
                    }
                }

            iPreviousAxis_xData = data.iAxisX;
            iPreviousAxis_yData = data.iAxisY;
            iPreviousAxis_zData = data.iAxisZ;
            iPreviousTimeStamp = data.iTimeStamp;

            if( iGetDataDelay > 0 )
                {
                RDebug::Print( _L("CSensrvTestCasesCases::DataReceived - read delay %d" ), iGetDataDelay * 1000 );
                User::After( iGetDataDelay * 1000 );
                }
            }

        TInt notFoundTest( 0 );
        RProperty::Get( KPSUidSensrvTest, KSensrvGetDataNotFoundTest, notFoundTest );
        if( notFoundTest )
            {
            TPckgBuf<TSensrvAccelerometerAxisData> dataBuf;
            TInt err = iSensorChannel->GetData( dataBuf );
            if( err )
                {
                RDebug::Print( _L("CSensrvTestCasesCases::DataReceived - GetData Error %d" ), err );
                iSensorChannel->StopDataListening();
                RProperty::Set( KPSUidSensrvTest, iCompletedKey, err );
                iTestCaseCompleted = ETrue;
                iMutex.Signal();
                return;
                }
            }

        }

    else if( aChannel.GetChannelInfo().iChannelType == KSensrvTestEventChannelType )
        {
        TBuf<50> timeBuf1;
        TBuf<50> timeBuf2;

        for( TInt i = 0; i < aCount; i++ )
            {
            TPckgBuf<TSensrvTestEventData> dataBuf;
            iSensorChannel->GetData( dataBuf );
            TSensrvTestEventData data = dataBuf();
            data.iTimeStampFromEventPlugin.FormatL( timeBuf1, KDateString );
            data.iTimeStampFromAccelerometerPlugin.FormatL( timeBuf2, KDateString );

            TTime now;
            now.HomeTime();

            RDebug::Print( _L(" ### data[%d]: iTimeStampFromEventPlugin = %S, iTimeStampFromAccelerometerPlugin = %S"), i, &timeBuf1, &timeBuf2 );
            RDebug::Print( _L(" ###           delay from event plugin = %ld, delay from accelerometer plugin = %ld"), now.MicroSecondsFrom( data.iTimeStampFromEventPlugin.Int64() ).Int64(),
                                                                                                                      now.MicroSecondsFrom( data.iTimeStampFromAccelerometerPlugin.Int64() ).Int64() );

            }

        }
    else if (aChannel.GetChannelInfo().iChannelType == KSensrvChannelTypeIdAccelerometerDoubleTappingData)
        {
        TBuf<50> timeBuf1;

        for( TInt i = 0; i < aCount; i++ )
            {
            TPckgBuf<TSensrvTappingData> dataBuf;
            iSensorChannel->GetData( dataBuf );
            TSensrvTappingData data = dataBuf();
            data.iTimeStamp.FormatL( timeBuf1, KDateString );

            TTime now;
            now.HomeTime();

            RDebug::Print( _L(" ### data[%d]: iTimeStamp = %S, iDirection: 0x%x"), i, &timeBuf1, data.iDirection );
            RDebug::Print( _L(" ###           delay from event plugin = %ld"), now.MicroSecondsFrom( data.iTimeStamp.Int64() ).Int64() );
            }
        }


    iDataListeningRepeatCounter++;

    if( iDataListeningRepeatCounter >= iDataListeningRepeatCount )
        {
        iSensorChannel->StopDataListening();
        RProperty::Set( KPSUidSensrvTest, iCompletedKey, KErrNone );
        iTestCaseCompleted = ETrue;
        }

    iMutex.Signal();

    }

// -----------------------------------------------------------------------------
// CSensrvTestCases::DataError
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSensrvTestCases::DataError( CSensrvChannel& aChannel,
                                TSensrvErrorSeverity aError)
    {
    iMutex.Wait();
    TBuf<16> location;
    location.Copy( iSensorChannel->GetChannelInfo().iLocation );
    RDebug::Print( _L("CSensrvTestCases::DataError: Channel: %S, aError = %d"), &location, aError );
    if( &aChannel != iSensorChannel )
        {
        RDebug::Print( _L("CSensrvTestCases::DataError - Error: incorrect channel" ) );
        RProperty::Set( KPSUidSensrvTest, iCompletedKey, KErrArgument );
        }

    if( aError == ESensrvErrorSeverityFatal )
        {
        iChannelOpen = EFalse;
        }
    RProperty::Set( KPSUidSensrvTest, iCompletedKey, aError );
    iTestCaseCompleted = ETrue;
    iMutex.Signal();
    }

// -----------------------------------------------------------------------------
// CSensrvTestCases::GetDataListenerInterfaceL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSensrvTestCases::GetDataListenerInterfaceL( TUid /*aInterfaceUid*/,
	                                        TAny*& aInterface )
    {    
		aInterface = reinterpret_cast<TAny*>(
			static_cast<MSensrvDataListener*>( this ) );
    }

// -----------------------------------------------------------------------------
// CSensrvTestCases::StartPropertyListeningL
// Parameters:
//
// -----------------------------------------------------------------------------
//
TInt CSensrvTestCases::StartPropertyListeningL()
    {
    RDebug::Print( _L("CSensrvTestCases::StartPropertyListeningL") );

    TInt err( KErrNone );

    if( !iSensorChannel )
        {
        RDebug::Print( _L("CSensrvTestCases::StartPropertyListeningL - Channel not found ") );
        return KErrNotFound;
        }


    TRAP( err, iSensorChannel->SetPropertyListenerL( this ) );

    if( err )
        {
        RDebug::Print( _L("CSensrvTestCases::StartPropertyListeningL-  StartPropertyListening error %d"), err );
        }
    return err;
    }



// -----------------------------------------------------------------------------
// CSensrvTestCases::StopPropertyListeningL
// Parameters:
//
// -----------------------------------------------------------------------------
//
TInt CSensrvTestCases::StopPropertyListeningL()
    {
    RDebug::Print( _L("CSensrvTestCases::StopPropertyListeningL") );

    TInt err( KErrNone );

    if( !iSensorChannel )
        {
        RDebug::Print( _L("CSensrvTestCases::StopPropertyListeningL - Channel not found ") );
        return KErrNotFound;
        }


    TRAP( err, iSensorChannel->SetPropertyListenerL( NULL ) );

    if( err )
        {
        RDebug::Print( _L("CSensrvTestCases::StopPropertyListeningL -  StopPropertyListening error %d"), err );
        }
    return err;
    }

// -----------------------------------------------------------------------------
// CSensrvTestCases::GetPropertyL
// Parameters: PropertyId(TUint32), ItemIndex(TInt)
// -----------------------------------------------------------------------------
//
TInt CSensrvTestCases::GetPropertyL()
    {
    if( !iSensorChannel )
        {
        RDebug::Print( _L("CSensrvTestCases::GetPropertyL - Channel not found ") );
        return KErrNotFound;
        }

    TInt err( KErrNone );
    TSensrvProperty property;
    TRAP( err, iSensorChannel->GetPropertyL( iGetPropertyIdTInt, iGetItemIndex, property ) );

    if( !err )
        {
        if( iSetPropertyIdTInt == KSensrvPropIdDataRate )
            {
            TInt expectedValue;
            TInt value;
            iPropDataRate.GetValue( expectedValue );
            property.GetValue( value );
            if( expectedValue != value || iPropDataRate.PropertyItemIndex() != property.PropertyItemIndex() )
                {
                err = KErrGeneral;
                }
            }
        else if( iSetPropertyIdTInt == KSensrvPropIdMeasureRange )
            {
            TReal expectedValue;
            TReal value;
            iPropMeasureRange.GetValue( expectedValue );
            property.GetValue( value );
            if( expectedValue != value || iPropMeasureRange.PropertyItemIndex() != property.PropertyItemIndex() )
                {
                err = KErrGeneral;
                }
            }
        else if( iSetPropertyIdTInt == KSensrvSensorDescription )
            {
            TBuf8<KSensrvPropertyTextBufferSize> expectedValue;
            TBuf8<KSensrvPropertyTextBufferSize> value;
            iPropSensorDescription.GetValue( expectedValue );
            property.GetValue( value );
            if( expectedValue.Compare( value ) || iPropSensorDescription.PropertyItemIndex() != property.PropertyItemIndex() )
                {
                err = KErrGeneral;
                }
            }
        else if( iSetPropertyIdTInt == KSensrvPropIdAvailability )
            {
            TInt expectedValue;
            TInt value;
            iPropAvailability.GetValue( expectedValue );
            property.GetValue( value );
            if( expectedValue != value )
                {
                err = KErrGeneral;
                }
            }
        }

    if( err )
        {
        RDebug::Print( _L("CSensrvTestCases::GetPropertyL-  GetPropertyL error %d"), err );
        }

    RProperty::Set( KPSUidSensrvTest, KSensrvGetPropertyLeave, 0 );
    RProperty::Set( KPSUidSensrvTest, KSensrvGetPropertyJam, 0 );
    RProperty::Set( KPSUidSensrvTest, KSensrvGetPropertyPanic, 0 );

    return err;
    }

// -----------------------------------------------------------------------------
// CSensrvTestCases::SetPropertyL
// Parameters:
// -----------------------------------------------------------------------------
//
TInt CSensrvTestCases::SetPropertyL()
    {
    if( !iSensorChannel )
        {
        RDebug::Print( _L("CSensrvTestCases::SetPropertyL - Channel not found ") );
        return KErrNotFound;
        }

    TInt err( KErrNone );

    if( iSetPropertyIdTInt == KSensrvPropIdDataRate )
        {
        err = iSensorChannel->SetProperty( iPropDataRate );
        }
    else if( iSetPropertyIdTInt == KSensrvPropIdMeasureRange )
        {
        err = iSensorChannel->SetProperty( iPropMeasureRange );
        }
    else if( iSetPropertyIdTInt == KSensrvSensorDescription )
        {
        err = iSensorChannel->SetProperty( iPropSensorDescription );
        }
    else if( iSetPropertyIdTInt == KSensrvPropIdChannelScale )
        {
        err = iSensorChannel->SetProperty( iPropChannelScale );
        }
    else if( iSetPropertyIdTInt == KSensrvPropIdAvailability )
        {
        err = iSensorChannel->SetProperty( iPropAvailability );
        }

    if( err )
        {
        RDebug::Print( _L("CSensrvTestCases::SetPropertyL -  error %d"), err );
        }

    RProperty::Set( KPSUidSensrvTest, KSensrvSetPropertyLeave, 0 );
    RProperty::Set( KPSUidSensrvTest, KSensrvSetPropertyJam, 0 );
    RProperty::Set( KPSUidSensrvTest, KSensrvSetPropertyPanic, 0 );

    return err;
    }

// -----------------------------------------------------------------------------
// CSensrvTestCases::GetAllPropertiesL
// Parameters:
// -----------------------------------------------------------------------------
//
TInt CSensrvTestCases::GetAllPropertiesL()
    {
    TInt err( KErrNone );

    TInt propertyCount( 0 );
    RProperty::Get( KPSUidSensrvTest, KSensrvGetAllPropertyCount, propertyCount );

    RSensrvPropertyList channelPropertyList;
    CleanupClosePushL( channelPropertyList );

    TRAP( err, iSensorChannel->GetAllPropertiesL( channelPropertyList ) );

    if( err )
        {
        RDebug::Print( _L("CSensrvTestCases::GetAllPropertiesL -   error %d"), err );
        }

    else
        {
        RDebug::Print( _L("CSensrvTestCases::GetAllPropertiesL -   Property count = %d"), channelPropertyList.Count() );
        if( propertyCount != channelPropertyList.Count() && propertyCount > 0 )
            {
            err = KErrGeneral;
            }
        else if ( propertyCount > 0 )
            {
            for( TInt i = 0; i < propertyCount; i++ )
                {
                TSensrvProperty property = channelPropertyList[i];
                TInt value(0);
                property.GetValue( value );
                if( property.PropertyItemIndex() != i || value != i )
                    {
                    RDebug::Print( _L("CSensrvTestCases::GetAllPropertiesL - Incorrect property %d"), i );
                    err = KErrGeneral;
                    break;
                    }
                }
            }
        }


    CleanupStack::PopAndDestroy( &channelPropertyList ); //Close() being called on "channelPropertyList"

    RProperty::Set( KPSUidSensrvTest, KSensrvGetAllPropertiesLeave, 0 );
    RProperty::Set( KPSUidSensrvTest, KSensrvGetAllPropertiesJam, 0 );
    RProperty::Set( KPSUidSensrvTest, KSensrvGetAllPropertiesPanic, 0 );

    return err;
    }

// -----------------------------------------------------------------------------
// CSensrvTestCases::EGetAllPropertiesWithPropertyIdL()
// -----------------------------------------------------------------------------
//
TInt CSensrvTestCases::GetAllPropertiesWithPropertyIdL()
    {
    RSensrvPropertyList channelPropertyList;
    CleanupClosePushL( channelPropertyList );

    TRAPD( err, iSensorChannel->GetAllPropertiesL( KSensrvPropIdDataRate, channelPropertyList ) );

    if( err )
        {
        RDebug::Print( _L("CSensrvTestCases::GetAllPropertiesL - error %d"), err );
        }
    else
        {

        }

    CleanupStack::PopAndDestroy( &channelPropertyList ); //Close() being called on "channelPropertyList"
    return err;
    }

// -----------------------------------------------------------------------------
// CSensrvTestCases::GetAllPropertiesWithArrayIndexL()
// -----------------------------------------------------------------------------
//
TInt CSensrvTestCases::GetAllPropertiesWithArrayIndexL()
    {
     TInt err( KErrNone );
    TSensrvProperty property;


    TRAP( err, iSensorChannel->GetPropertyL( KSensrvPropIdDataRate,
                                             0, //aItemIndex
                                             1, //aArrayIndex
                                             property ) );

    if( err )
        {
        RDebug::Print( _L("CSensrvTestCases::GetAllPropertiesL - error %d"), err );
        }

    return err;
    }

// -----------------------------------------------------------------------------
// CSensrvTestCases::PropertyChanged
// -----------------------------------------------------------------------------
//
void CSensrvTestCases::PropertyChanged( CSensrvChannel& aChannel, const TSensrvProperty& aChangedProperty )
    {
    iMutex.Wait();

    if( &aChannel != iSensorChannel )
        {
        RDebug::Print( _L("CSensrvTestCasesCases::DataReceived - Error: incorrect channel" ) );
        RProperty::Set( KPSUidSensrvTest, iCompletedKey, KErrArgument );
        }

    TBuf<16> location;
    location.Copy( aChannel.GetChannelInfo().iLocation );
    RDebug::Print( _L("CSensrvTestCasesCases::PropertyChanged: Channel: %S"), &location );
    switch( aChangedProperty.PropertyType() )
        {
        case ESensrvIntProperty:
            {
            TInt value;
            aChangedProperty.GetValue( value );
            RDebug::Print( _L("CSensrvTestCasesCases::PropertyChanged: PropertyType = ESensrvIntProperty, value = %d"), value );
            break;
            }
        case ESensrvRealProperty:
            {
            TReal value;
            aChangedProperty.GetValue( value );
            RDebug::Print( _L("CSensrvTestCasesCases::PropertyChanged: PropertyType = ESensrvRealProperty, value = %f"), value );
            break;
            }
        case ESensrvBufferProperty:
            {
            TBuf8<KSensrvPropertyTextBufferSize> value;
            aChangedProperty.GetValue( value );
            RDebug::Print( _L("CSensrvTestCasesCases::PropertyChanged: PropertyType = ESensrvRealProperty, value = %S"), &value );
            break;
            }
        default:
            {
            RDebug::Print( _L("CSensrvTestCasesCases::PropertyChanged: Unknown property type") );
            break;
            }
        }


    iPropertyChangedCount++;
    iMutex.Signal();
    }

// -----------------------------------------------------------------------------
// CSensrvTestCases::PropertyError
// -----------------------------------------------------------------------------
//
void CSensrvTestCases::PropertyError( CSensrvChannel& aChannel, TSensrvErrorSeverity aError )
    {
    iMutex.Wait();
    TBuf<16> location;
    location.Copy( iSensorChannel->GetChannelInfo().iLocation );
    RDebug::Print( _L("CSensrvTestCasesCases::PropertyError: Channel: %S, aError = %d"), &location, aError );
    if( &aChannel != iSensorChannel )
        {
        RDebug::Print( _L("CSensrvTestCasesCases::PropertyError - Error: incorrect channel" ) );
        RProperty::Set( KPSUidSensrvTest, iCompletedKey, KErrArgument );
        }

    if( aError == ESensrvErrorSeverityFatal )
        {
        iChannelOpen = EFalse;
        }
    RProperty::Set( KPSUidSensrvTest, iCompletedKey, aError );
    iTestCaseCompleted = ETrue;
    iMutex.Signal();
    }

// -----------------------------------------------------------------------------
// CSensrvTestCases::SetPropertySuccessIndicationChanged
// -----------------------------------------------------------------------------
//
void CSensrvTestCases::SetPropertySuccessIndicationChanged( TSetPropertySuccessIndicator aIndication )
    {
    iMutex.Wait();
    RDebug::Print( _L("CSensrvTestCasesCases::SetPropertySuccessIndicationChanged: aIndication = %d"), aIndication );
    iSetPropertySuccessIndicator = aIndication;
    iSetPropertySuccessIndicatorCount++;
    iMutex.Signal();
    }

// -----------------------------------------------------------------------------
// CSensrvTestCases::GetPropertyListenerInterfaceL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSensrvTestCases::GetPropertyListenerInterfaceL( TUid /*aInterfaceUid*/,
	                                                    TAny*& aInterface )
    {    
		aInterface = reinterpret_cast<TAny*>(
		  static_cast<MSensrvPropertyListener*>( this ) );
    }


// -----------------------------------------------------------------------------
// CSensrvTestCases::CreateNewProcessL()
// -----------------------------------------------------------------------------
//
TInt CSensrvTestCases::CreateNewProcessL()
    {
    /*if( !iSensorChannel )
        {
        RDebug::Print( _L("CSensrvTestCases::CreateNewProcessL() - Channel not found ") );
        return KErrNotFound;
        }*/

    TInt err( KErrNone );

    for( TInt i=0 ; i<iProcessCount ; i++ )
        {
        TBuf<25> processName;
        TBuf<3> parameter;
        processName.Format( KSensrvTestProcessName, i+1 );
        parameter.Num( i+1 );
        RDebug::Print( _L("CSensrvTest::CreateNewProcessL: processName=%S parameter=%S"), &processName, &parameter );

		TRequestStatus status;
		RProcess process;
		err = process.Create( processName, parameter );

		if( err != KErrNone ) // thread created ok - now start it going
			{
			return err;
			}

		// Process created successfully
		process.Rendezvous( status );
		if( status != KRequestPending )
			{
			process.Kill( KErrNone ); // abort startup
			process.Close();
			return KErrGeneral;  // status.Int() can be KErrNone!
			}
		else
			{
			process.Resume();	// logon OK - start the server
			}

		// Wait until the completion of the server creation
		User::WaitForRequest( status );

		if( status != KErrNone )
			{
			process.Close();
			return status.Int();
			}

		process.Close(); //we're no longer interested in the other process
        }

    if( err )
        {
        RDebug::Print( _L("CSensrvTestCases::CreateNewProcessL -  error %d"), err );
        }

    return err;
    }

// -----------------------------------------------------------------------------
// CSensrvTestCases::KillProcess
// -----------------------------------------------------------------------------
//
TInt CSensrvTestCases::KillProcess()
    {
    TInt err( KErrNone );
    TRequestStatus status;

    TBuf<25> processName;
    processName.Append( KSensrvTestProcessNamePrefix );
    processName.Append( KMatchAny );
    RDebug::Print( _L("CSensrvTestCases::KillProcess: processName = %S"), &processName );

    TFindProcess findProcess(processName);
	TFullName name;

	while( err == KErrNone )
        {
	    err = findProcess.Next( name );

    	if( err == KErrNone )
    	    {
    	    RProcess process;
	    	err = process.Open( name );
	    	if( KErrNone != err )
	    	    {
	    	    return err;
	    	    }
            process.Kill( KErrNone );
            process.Close();
            }
        }

    if( KErrNotFound == err )
        {
        return KErrNone;
        }
    else
        {
        return err;
        }
    }

// -----------------------------------------------------------------------------
// CSensrvTestCases::AddConditionL
// Parameters:
// -----------------------------------------------------------------------------
//
TInt CSensrvTestCases::AddConditionL()
    {
    TInt err( KErrNone );

    CSensrvChannelCondition* condition = NULL;

    // Create condition based on parameters
    if (iConditionChannelType == KSensrvChannelTypeIdAccelerometerXYZAxisData)
        {
        TSensrvAccelerometerAxisData data;
        if (iConditionIndex == TSensrvAccelerometerAxisData::EAxisX)
            {
            data.iAxisX = iConditionValue;
            data.iAxisY = 0;
            data.iAxisZ = 0;
            }
        else if (iConditionIndex == TSensrvAccelerometerAxisData::EAxisY)
            {
            data.iAxisX = 0;
            data.iAxisY = iConditionValue;
            data.iAxisZ = 0;
            }
        else if (iConditionIndex == TSensrvAccelerometerAxisData::EAxisZ)
            {
            data.iAxisX = 0;
            data.iAxisY = 0;
            data.iAxisZ = iConditionValue;
            }
        else
            {
            // This will fail at server check with KErrArgument because of index...
            data.iAxisX = 0;
            data.iAxisY = 0;
            data.iAxisZ = 0;
            }

        TPckgBuf<TSensrvAccelerometerAxisData> pckg(data);

        condition = CSensrvChannelCondition::NewLC(iConditionType,
                                                   iConditionOperator,
                                                   iConditionIndex,
                                                   pckg);

        }
    else if (iConditionChannelType == KSensrvTestEventChannelType)
        {
        // Value indicates milliseconds from now for condition value
        TTime data;
        data.HomeTime();
        TTimeIntervalMicroSeconds add = (1000*iConditionValue);
        data += add;

        TPckgBuf<TTime> pckg(data);

        condition = CSensrvChannelCondition::NewLC(iConditionType,
                                                   iConditionOperator,
                                                   iConditionIndex,
                                                   pckg);
        }
    else if (iConditionChannelType == KSensrvChannelTypeIdAccelerometerDoubleTappingData)
        {
        // Value indicates direction
        TSensrvTappingData data;
        data.iDirection = iConditionValue;
        TPckgBuf<TSensrvTappingData> pckg(data);
        condition = CSensrvChannelCondition::NewLC(iConditionType,
                                                   iConditionOperator,
                                                   iConditionIndex,
                                                   pckg);
        }
    else
        {
        return KErrArgument;
        }

    // Add condition to local holding array
    err = iConditionList.Append(condition);

    // Cleanup
    iConditionType = 0;
    iConditionOperator = 0;
    iConditionIndex = 0;
    iConditionValue = 0;
    iConditionChannelType = 0;

    CleanupStack::Pop(condition);

    if( err )
        {
        RDebug::Print( _L("CSensrvTestCases::AddConditionL -  error %d"), err );
        }

    return err;
    }

// -----------------------------------------------------------------------------
// CSensrvTestCases::AddConditionSetL
// Parameters:
// -----------------------------------------------------------------------------
//
TInt CSensrvTestCases::AddConditionSetL()
    {
    if( !iSensorChannel )
        {
        RDebug::Print( _L("CSensrvTestCases::AddConditionSetL - Channel not found ") );
        return KErrNotFound;
        }

    TInt err( KErrNone );

    // Create condition set from local holding array
    CSensrvChannelConditionSet* set =
        CSensrvChannelConditionSet::NewLC(static_cast<TSensrvConditionSetType>(iConditionSetType));

    for (TInt i=0; err == KErrNone && i<iConditionList.Count();i++)
        {
        TRAP(err, set->AddChannelConditionL(iConditionList[i]));
        }

    if (err == KErrNone)
        {
        // Add set to server
        TRAP(err, iSensorChannel->AddConditionL(*set));

        if ( err == KErrNone && iConditionSetAddTwice )
            {
            TRAP(err, iSensorChannel->AddConditionL(*set));
            }

        if (err == KErrNone)
            {
            // Add set to local set holding array
            err = iConditionSetList.Append(set);
            }
        }

    // Cleanup
    iConditionList.Reset(); // Ownership of conditions passed to set
    CleanupStack::Pop(set);

    if( err )
        {
        RDebug::Print( _L("CSensrvTestCases::AddConditionSetL -  error %d"), err );
        }

    return err;
    }

// -----------------------------------------------------------------------------
// CSensrvTestCases::RemoveConditionsL
// Parameters:
// -----------------------------------------------------------------------------
//
TInt CSensrvTestCases::RemoveConditionsL()
    {
    if( !iSensorChannel )
        {
        RDebug::Print( _L("CSensrvTestCases::RemoveConditionsL - Channel not found ") );
        return KErrNotFound;
        }

    TInt err( KErrNone );

    if ( iConditionRemoveNonexistent )
        {
        TSensrvAccelerometerAxisData data;
        data.iAxisX = 0;
        data.iAxisY = 0;
        data.iAxisZ = 0;
        TPckgBuf<TSensrvAccelerometerAxisData> pckg(data);

        CSensrvChannelCondition* condition = CSensrvChannelCondition::NewLC(ESensrvSingleLimitCondition,
                                                   ESensrvOperatorEquals,
                                                   0,
                                                   pckg);

        CSensrvChannelConditionSet* set =
            CSensrvChannelConditionSet::NewLC(ESensrvAndConditionSet);

        set->AddChannelConditionL(condition);

        CleanupStack::Pop(set);
        CleanupStack::Pop(condition);

        TRAP(err,iSensorChannel->RemoveConditionL(*set));

        delete set;
        }
    else
        {
        // Remove all added condition sets
        for (TInt i=0; err == KErrNone && i<iConditionSetList.Count();i++)
            {
            TRAP(err, iSensorChannel->RemoveConditionL(*(iConditionSetList[i])));
            }

        // Cleanup
        iConditionSetList.ResetAndDestroy();
        }

    if( err )
        {
        RDebug::Print( _L("CSensrvTestCases::RemoveConditionsL -  error %d"), err );
        }

    return err;
    }

// -----------------------------------------------------------------------------
// CSensrvTestCases::StartConditionListeningL
// Parameters: dataBlockCount, timeout, repeatCount
//
// -----------------------------------------------------------------------------
//
TInt CSensrvTestCases::StartConditionListeningL()
    {
    TInt err;

    iConditionListeningRepeatCounter = 0;

    if( !iSensorChannel )
        {
        RDebug::Print( _L("CSensrvTestCases::StartConditionListeningL - Channel not found ") );
        return KErrNotFound;
        }

    TBuf<16> location;
    location.Copy( iSensorChannel->GetChannelInfo().iLocation );

    RDebug::Print( _L("CSensrvTestCases::StartConditionListeningL: Channel: %S"), &location );
    RDebug::Print( _L("CSensrvTestCases::StartConditionListeningL - count=%d, timeout=%d"), iConditionListeningCount, iConditionListeningTimeout );

    TRAP( err, iSensorChannel->StartConditionListeningL( this, iConditionListeningCount, iConditionListeningTimeout ) );

    if( err )
        {
        RDebug::Print( _L("CSensrvTestCases::StartConditionListeningL - error %d"), err );
        }
    else if (!iConditionEvaluator)
        {
        iConditionEvaluator = CSensrvConditionEvaluator::NewL(iChannelInfo);
        }

    return err;
    }

// -----------------------------------------------------------------------------
// CSensrvTestCases::StopConditionListeningL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CSensrvTestCases::StopConditionListeningL()
    {
    if( !iSensorChannel )
        {
        RDebug::Print( _L("CSensrvTestCases::StopConditionListeningL - Channel not found ") );
        return KErrNotFound;
        }

    TBuf<16> location;
    location.Copy( iSensorChannel->GetChannelInfo().iLocation );
    RDebug::Print( _L("CSensrvTestCasesCases::StopConditionListeningL: Channel: %S"), &location );
    iSensorChannel->StopConditionListening();

    return KErrNone;
    }


// -----------------------------------------------------------------------------
// CSensrvTestCases::ConditionMet
// -----------------------------------------------------------------------------
//
void CSensrvTestCases::ConditionMet( CSensrvChannel& aChannel,
                                     CSensrvChannelConditionSet& aChannelConditionSet,
                                     TDesC8& aValue)
    {
    iMutex.Wait();

    if( &aChannel != iSensorChannel )
        {
        RDebug::Print( _L("CSensrvTestCasesCases::ConditionMet - Error: incorrect channel" ) );
        RProperty::Set( KPSUidSensrvTest, iCompletedKey, KErrArgument );
        }

    TBuf<16> location;
    location.Copy( aChannel.GetChannelInfo().iLocation );
    RDebug::Print( _L("CSensrvTestCasesCases::ConditionMet: Channel: %S"), &location );
    RDebug::Print( _L("CSensrvTestCasesCases::ConditionMet[%d]"), iConditionListeningRepeatCounter );

    TBuf<50> timeBuf;
    TBuf<50> timeBuf2;
    TBool failedSizeCheck(EFalse);

    if( aChannel.GetChannelInfo().iChannelType == KSensrvChannelTypeIdAccelerometerXYZAxisData )
        {
        if ( sizeof(TSensrvAccelerometerAxisData) != aValue.Size() )
            {
            failedSizeCheck = ETrue;
            RDebug::Print( _L("CSensrvTestCasesCases::ConditionMet - sizeof(TSensrvAccelerometerAxisData): %d != aValue.Size(): %d"), sizeof(TSensrvAccelerometerAxisData), aValue.Size() );
            }
        }
    else if( aChannel.GetChannelInfo().iChannelType == KSensrvTestEventChannelType )
        {
        if ( sizeof(TSensrvTestEventData) != aValue.Size() )
            {
            RDebug::Print( _L("CSensrvTestCasesCases::ConditionMet - sizeof(TSensrvTestEventData): %d != aValue.Size(): %d"), sizeof(TSensrvTestEventData), aValue.Size() );
            failedSizeCheck = ETrue;
            }
        }
    else if( aChannel.GetChannelInfo().iChannelType == KSensrvChannelTypeIdAccelerometerDoubleTappingData )
        {
        if ( sizeof(TSensrvTappingData) != aValue.Size() )
            {
            RDebug::Print( _L("CSensrvTestCasesCases::ConditionMet - sizeof(TSensrvTappingData): %d != aValue.Size(): %d"), sizeof(TSensrvTappingData), aValue.Size() );
            failedSizeCheck = ETrue;
            }
        }

    if ( failedSizeCheck )
        {
        iSensorChannel->StopConditionListening();
        RProperty::Set( KPSUidSensrvTest, iCompletedKey, KErrArgument );
        iTestCaseCompleted = ETrue;
        iMutex.Signal();
        return;
        }

    TTime now;
    now.HomeTime();

    if( aChannel.GetChannelInfo().iChannelType == KSensrvChannelTypeIdAccelerometerXYZAxisData )
        {
        TPckgBuf<TSensrvAccelerometerAxisData> dataBuf;
        dataBuf.Copy(aValue);
        TSensrvAccelerometerAxisData data = dataBuf();

        data.iTimeStamp.FormatL( timeBuf, KDateString );

        RDebug::Print( _L(" ### Condition met data: iTimeStamp = %S, iAxisX = %d, iAxisY = %d, iAxisZ = %d,  delay: %ld"), &timeBuf, data.iAxisX, data.iAxisY, data.iAxisZ, now.MicroSecondsFrom( data.iTimeStamp.Int64() ).Int64() );
        }
    else if( aChannel.GetChannelInfo().iChannelType == KSensrvTestEventChannelType )
        {
        TPckgBuf<TSensrvTestEventData> dataBuf;
        dataBuf.Copy(aValue);
        TSensrvTestEventData data = dataBuf();

        data.iTimeStampFromAccelerometerPlugin.FormatL( timeBuf, KDateString );
        data.iTimeStampFromEventPlugin.FormatL( timeBuf2, KDateString );

        RDebug::Print( _L(" ### Condition met data: iTimeStampFromAccelerometerPlugin = %S, iTimeStampFromEventPlugin = %S,  delay: %ld"), &timeBuf, &timeBuf2, now.MicroSecondsFrom( data.iTimeStampFromAccelerometerPlugin.Int64() ).Int64() );
        }
    else if( aChannel.GetChannelInfo().iChannelType == KSensrvChannelTypeIdAccelerometerDoubleTappingData )
        {
        TPckgBuf<TSensrvTappingData> dataBuf;
        dataBuf.Copy(aValue);
        TSensrvTappingData data = dataBuf();

        data.iTimeStamp.FormatL( timeBuf, KDateString );

        RDebug::Print( _L(" ### Condition met data: iTimeStamp = %S, iDirection = 0x%x,  delay: %ld"), &timeBuf, data.iDirection, now.MicroSecondsFrom( data.iTimeStamp.Int64() ).Int64() );

        // Check that received value was the expected one
        if (iConditionListeningExpectedValuesArray.Count() > iConditionListeningRepeatCounter)
            {
            if (   iConditionListeningExpectedValuesArray[iConditionListeningRepeatCounter] == data.iDirection
                || iConditionListeningExpectedValuesArray[iConditionListeningRepeatCounter] == KSensrvUncertainExpectedValue)
                {
                RDebug::Print( _L(" ### Condition met data was expected") );
                }
            else
                {
                RDebug::Print( _L(" ### Condition met data was UNEXPECTED") );
                iSensorChannel->StopConditionListening();
                RProperty::Set( KPSUidSensrvTest, iCompletedKey, -999 );
                iTestCaseCompleted = ETrue;
                iMutex.Signal();
                return;
                }
            }

        }

    // Check that condition set indeed is met with this data item
    const RSensrvChannelConditionList* conditionList = &(aChannelConditionSet.AllConditions());
    TBool singleMatch(aChannelConditionSet.ConditionSetType() == ESensrvOrConditionSet);
    TInt matchCount(0);
    TInt checkCount(0);

    if (singleMatch)
        {
        RDebug::Print(_L(" ### OR-set"));
        }
    else
        {
        RDebug::Print(_L(" ### AND-set"));
        }

    // For OR-sets, finding single match is reason to exit loop.
    // For AND-sets, finding single mismatch is reason to exit loop.
    for ( TInt j = 0;
          j < conditionList->Count()
          && ( (   singleMatch && !matchCount)
                || !singleMatch && matchCount == checkCount );
          j++)
        {
        CSensrvChannelCondition* condition1 = (*conditionList)[j];
        CSensrvChannelCondition* condition2 = NULL;

        if( aChannel.GetChannelInfo().iChannelType == KSensrvChannelTypeIdAccelerometerXYZAxisData )
            {
            const TSensrvAccelerometerAxisData* value1 =
                reinterpret_cast<const TSensrvAccelerometerAxisData*>(condition1->ConditionValue().Ptr());

            RDebug::Print( _L(" ### Condition1 value: iAxisX = %d, iAxisY = %d, iAxisZ = %d"), value1->iAxisX, value1->iAxisY, value1->iAxisZ);

            if ( (*conditionList)[j]->ConditionType() ==
                        ESensrvRangeConditionLowerLimit )
                {
                // Since set is complete, there must exist upper
                // range limit after the lower range limit
                condition2 = (*conditionList)[++j];

                const TSensrvAccelerometerAxisData* value2 =
                    reinterpret_cast<const TSensrvAccelerometerAxisData*>(condition2->ConditionValue().Ptr());

                RDebug::Print( _L(" ### Condition2 value: iAxisX = %d, iAxisY = %d, iAxisZ = %d"), value2->iAxisX, value2->iAxisY, value2->iAxisZ);
                }
            }
        else if( aChannel.GetChannelInfo().iChannelType == KSensrvTestEventChannelType )
            {
            const TTime* value1 =
                reinterpret_cast<const TTime*>(condition1->ConditionValue().Ptr());

            value1->FormatL( timeBuf, KDateString );

            RDebug::Print( _L(" ### Condition1 value: %S"), &timeBuf);

            if ( (*conditionList)[j]->ConditionType() ==
                        ESensrvRangeConditionLowerLimit )
                {
                // Since set is complete, there must exist upper
                // range limit after the lower range limit
                condition2 = (*conditionList)[++j];

                const TTime* value2 =
                    reinterpret_cast<const TTime*>(condition2->ConditionValue().Ptr());

                value2->FormatL( timeBuf2, KDateString );

                RDebug::Print( _L(" ### Condition2 value: %S"), &timeBuf2);
                }
            }
        else if( aChannel.GetChannelInfo().iChannelType == KSensrvChannelTypeIdAccelerometerDoubleTappingData )
            {
            const TSensrvTappingData* value1 =
                reinterpret_cast<const TSensrvTappingData*>(condition1->ConditionValue().Ptr());

            RDebug::Print( _L(" ### Condition value: iDirection = 0x%x"), value1->iDirection);
            }

        checkCount++;

        TBool isMatch(
            iConditionEvaluator->EvaluateConditionL(
                condition1,
                condition2,
                aValue) );

        if (isMatch)
            {
            RDebug::Print(_L("  ### Condition is a match"));
            matchCount++;
            }
        else
            {
            // Ok, no match, do nothing
            }
        }

    // Check if required amount of matches was found
    if (!((  singleMatch && matchCount)
          || (!singleMatch && matchCount == checkCount))   )
        {
        RDebug::Print( _L("CSensrvTestCasesCases::ConditionMet - Error: Set does not match the data." ) );
        }

    iConditionListeningRepeatCounter++;

    if( iConditionListeningRepeatCounter >= iConditionListeningRepeatCount )
        {
        iSensorChannel->StopConditionListening();
        RProperty::Set( KPSUidSensrvTest, iCompletedKey, KErrNone );
        iTestCaseCompleted = ETrue;
        }
    else if (iConditionSetReadd)
        {
        // Add set just to server (no need to add local list anymore)
        TRAP_IGNORE( iSensorChannel->AddConditionL( aChannelConditionSet ) );
        }

    iMutex.Signal();

    }

// -----------------------------------------------------------------------------
// CSensrvTestCases::ConditionError
// -----------------------------------------------------------------------------
//
void CSensrvTestCases::ConditionError( CSensrvChannel& aChannel,
                                       TSensrvErrorSeverity aError)
    {
    iMutex.Wait();
    TBuf<16> location;
    location.Copy( iSensorChannel->GetChannelInfo().iLocation );
    RDebug::Print( _L("CSensrvTestCasesCases::ConditionError: Channel: %S, aError = %d"), &location, aError );
    if( &aChannel != iSensorChannel )
        {
        RDebug::Print( _L("CSensrvTestCasesCases::ConditionError - Error: incorrect channel" ) );
        RProperty::Set( KPSUidSensrvTest, iCompletedKey, KErrArgument );
        }

    if( aError == ESensrvErrorSeverityFatal )
        {
        iChannelOpen = EFalse;
        }
    RProperty::Set( KPSUidSensrvTest, iCompletedKey, aError );
    iTestCaseCompleted = ETrue;
    iMutex.Signal();
    }

// -----------------------------------------------------------------------------
// CSensrvTestCases::GetChannelConditionListenerInterfaceL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSensrvTestCases::GetChannelConditionListenerInterfaceL( TUid /*aInterfaceUid*/,
	                                        TAny*& aInterface )
    {    
    aInterface = reinterpret_cast<TAny*>(
			static_cast<MSensrvChannelConditionListener*>( this ) );
    }

// -----------------------------------------------------------------------------
// CSensrvTestCases::StartChannelChangeListeningL
// Parameters:
//
// -----------------------------------------------------------------------------
//
TInt CSensrvTestCases::StartChannelChangeListeningL()
    {
    RDebug::Print( _L("CSensrvTestCases::StartChannelChangeListeningL") );

    TInt err( KErrNone );

    if( !iChannelFinder )
        {
        RDebug::Print( _L("CSensrvTestCases::StartChannelChangeListeningL - iChannelFinder not found, creating new ") );
        iChannelFinder = CSensrvChannelFinder::NewL();
        }

    TRAP( err, iChannelFinder->SetChannelListenerL( this, iChannelInfo ) );

    if( err )
        {
        RDebug::Print( _L("CSensrvTestCases::StartChannelChangeListeningL-  StartChannelChangeListening error %d"), err );
        }

    return err;
    }



// -----------------------------------------------------------------------------
// CSensrvTestCases::StopChannelChangeListeningL
// Parameters:
//
// -----------------------------------------------------------------------------
//
TInt CSensrvTestCases::StopChannelChangeListeningL()
    {
    RDebug::Print( _L("CSensrvTestCases::StopChannelChangeListeningL") );

    TInt err( KErrNone );

    if( !iChannelFinder )
        {
        RDebug::Print( _L("CSensrvTestCases::StopChannelChangeListeningL - iChannelFinder not found, creating new ") );
        iChannelFinder = CSensrvChannelFinder::NewL();
        }

    TRAP( err, iChannelFinder->SetChannelListenerL( NULL, iChannelInfo ) );

    if( err )
        {
        RDebug::Print( _L("CSensrvTestCases::StopChannelChangeListeningL -  StopChannelChangeListening error %d"), err );
        }
    return err;
    }

// -----------------------------------------------------------------------------
// CSensrvTestCases::AdjustChannels
//
// -----------------------------------------------------------------------------
//
TInt CSensrvTestCases::AdjustChannels(CStifItemParser& aItem)
    {
    Mutex().Wait();

    // Get parameters
    TInt err = aItem.GetNextInt( iChannelChangeListeningRepeatCount );
    if( !err )
        {
        TPtrC16 vendorId;
        err = aItem.GetNextString( vendorId );
        if( err )
            {
            vendorId.Set( KNullDesC() );
            }
        RDebug::Print( _L("CSensrvTest::AdjustChannels: vendorId = %S"), &vendorId );

        iExpectedVendor.Copy(vendorId);
        }

    TInt type(KErrNotFound);

    if( !err )
        {
        err = aItem.GetNextInt( type );
        }

    TInt addCount(iChannelChangeListeningRepeatCount);

    if( !err )
        {
        // Ignore error, this parameter is optional and defaults to repeat count
        // It is only useful if this method is called multiple times before waiting for results
        aItem.GetNextInt( addCount );
        RDebug::Print( _L("CSensrvTestCases::AdjustChannels - addCount: %d"), addCount );
        }

    if (!err)
        {
        iChannelChangeListeningRepeatCounter = 0;

        if (type == ESensrvChannelChangeTypeRemoved)
            {
            RDebug::Print( _L("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@") );
            RDebug::Print( _L("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@") );
            RDebug::Print( _L("@@@ ") );
            RDebug::Print( _L("@@@ Please UNINSTALL SSYStubSis.sis now.") );
            RDebug::Print( _L("@@@ ") );
            RDebug::Print( _L("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@") );
            RDebug::Print( _L("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@") );
            iExpectedChannelChangeType = ESensrvChannelChangeTypeRemoved;
            }
        else if (type == ESensrvChannelChangeTypeAdded)
            {
            RDebug::Print( _L("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@") );
            RDebug::Print( _L("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@") );
            RDebug::Print( _L("@@@ ") );
            RDebug::Print( _L("@@@ Please INSTALL SSYStubSis.sis now.") );
            RDebug::Print( _L("@@@ ") );
            RDebug::Print( _L("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@") );
            RDebug::Print( _L("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@") );
            iExpectedChannelChangeType = ESensrvChannelChangeTypeAdded;
            }
        else if (   type == KSensrvCreateNewChannelsFirst
                 || type == KSensrvCreateNewChannelsSecond)
            {
            iExpectedChannelChangeType = ESensrvChannelChangeTypeAdded;
            RProperty::Set( KPSUidSensrvTest, type, addCount );
            }
        else if (   type == KSensrvRemoveNewChannelsFirst
                 || type == KSensrvRemoveNewChannelsSecond)
            {
            iExpectedChannelChangeType = ESensrvChannelChangeTypeRemoved;
            RProperty::Set( KPSUidSensrvTest, type, addCount );
            }
        }

    Mutex().Signal();

    return err;
    }

// -----------------------------------------------------------------------------
// CSensrvTestCases::StoreFoundChannelsL
//
// -----------------------------------------------------------------------------
//
TInt CSensrvTestCases::StoreFoundChannelsL(CStifItemParser& aItem)
    {
    Mutex().Wait();

    // Get parameters
    TInt storeOrCheck(0);
    TInt err(KErrNone);

    // Ignore error and default for zero if not found
    // zero = Check
    // nonzero = Store
    aItem.GetNextInt( storeOrCheck );

    if (storeOrCheck)
        {
        // store
        for (TInt i=0; i<iChannelInfoList.Count(); i++)
            {
            TRAP(err, iStoredInfoList.Append(iChannelInfoList[i]));
            }
        }
    else
        {
        // check
        if (iChannelInfoList.Count() != iStoredInfoList.Count())
            {
            err = KErrNotFound;
            }
        else
            {
            for (TInt i=0; !err && i<iChannelInfoList.Count(); i++)
                {
                if (!iChannelInfoList[i].IsMatch(iStoredInfoList[i]))
                    {
                    err = KErrNotFound;
                    }
                }
            }
        }

    Mutex().Signal();

    return err;
    }

// -----------------------------------------------------------------------------
// CSensrvTestCases::StoreFoundChannelsL
//
// -----------------------------------------------------------------------------
//
TInt CSensrvTestCases::SetDefaultValueL(CStifItemParser& aItem)
    {    
    TInt err = KErrNone;
    _LIT(KVendorId, "VendorNew");
    
    iExpectedVendor.Copy(KVendorId);
    iExpectedChannelChangeType = ESensrvChannelChangeTypeAdded;
    iChannelChangeListeningRepeatCount = 3;

    return err;
    }


// -----------------------------------------------------------------------------
// CSensrvTestCases::ChannelChangeDetected
// -----------------------------------------------------------------------------
//
void CSensrvTestCases::ChannelChangeDetected( const TSensrvChannelInfo& aDetectedChannel,
                                              TSensrvChannelChangeType aChangeType)
    {
    iMutex.Wait();

    // Convert location and vendorID from 8bit to 16 bit for proper printing
    TInt len = aDetectedChannel.iVendorId.Length();
    HBufC16* vidBuf16 = HBufC16::New(len);
    vidBuf16->Des().Copy(aDetectedChannel.iVendorId);

    len = aDetectedChannel.iLocation.Length();
    HBufC16* locBuf16 = HBufC16::New(len);
    locBuf16->Des().Copy(aDetectedChannel.iLocation);

    RDebug::Print(  _L( "CSensrvTestCasesCases::ChannelChangeDetected: aChangeType: %d iChannelId: %d, iContextType: 0x%x, iQuantity: 0x%x, iChannelType: 0x%x, iLocation: \"%S\", iVendorId: \"%S\", iDataItemSize: %d " ),
                       aChangeType,
                       aDetectedChannel.iChannelId,
                       aDetectedChannel.iContextType,
                       aDetectedChannel.iQuantity,
                       aDetectedChannel.iChannelType,
                       locBuf16 ? locBuf16 : &KSensrvNullString,
                       vidBuf16 ? vidBuf16 : &KSensrvNullString,
                       aDetectedChannel.iDataItemSize   );
    delete locBuf16;
    delete vidBuf16;

    if (iExpectedVendor.Compare(aDetectedChannel.iVendorId))
        {
        // Unexpected vendor
        RDebug::Print( _L("CSensrvTestCasesCases::ChannelChangeDetected: ERROR: Unexpected vendor") );
        RProperty::Set( KPSUidSensrvTest, iCompletedKey, KErrArgument );
        iTestCaseCompleted = ETrue;
        }
    else
        {
        if (iExpectedChannelChangeType == aChangeType)
            {
            iChannelChangeListeningRepeatCounter++;

            if( iChannelChangeListeningRepeatCounter == iChannelChangeListeningRepeatCount )
                {
                RProperty::Set( KPSUidSensrvTest, iCompletedKey, KErrNone );
                iTestCaseCompleted = ETrue;
                }
            else if( iChannelChangeListeningRepeatCounter >= iChannelChangeListeningRepeatCount )
                {
                // Unexpected notification
                RDebug::Print( _L("CSensrvTestCasesCases::ChannelChangeDetected: ERROR: Unexpected notification") );
                RProperty::Set( KPSUidSensrvTest, iCompletedKey, KErrOverflow );
                iTestCaseCompleted = ETrue;
                }
            }
        else
            {
            // Unexpected change type
            RDebug::Print( _L("CSensrvTestCasesCases::ChannelChangeDetected: ERROR: Unexpected change type: %d"), aChangeType );
            RProperty::Set( KPSUidSensrvTest, iCompletedKey, KErrArgument );
            iTestCaseCompleted = ETrue;
            }
        }

    iMutex.Signal();
    }

// -----------------------------------------------------------------------------
// CSensrvTestCases::ChannelDetectionError
// -----------------------------------------------------------------------------
//
void CSensrvTestCases::ChannelDetectionError( CSensrvChannelFinder& aChannelFinder,
                                              TSensrvErrorSeverity aError)
    {
    iMutex.Wait();
    RDebug::Print( _L("CSensrvTestCasesCases::ChannelDetectionError: aError = %d"), aError );
    if( &aChannelFinder != iChannelFinder )
        {
        RDebug::Print( _L("CSensrvTestCasesCases::ChannelDetectionError - Error: incorrect channelfinder reference" ) );
        RProperty::Set( KPSUidSensrvTest, iCompletedKey, KErrArgument );
        }

    if( aError == ESensrvErrorSeverityFatal )
        {
        // Check that iChannelFinder is indeed unusable
        TSensrvChannelInfo checkInfo;
        RSensrvChannelInfoList checkList;
        TInt err(KErrNone);

        TRAP( err, iChannelFinder->FindChannelsL(checkList, checkInfo) );

        if ( err != KErrBadHandle )
            {
            RDebug::Print( _L("CSensrvTestCasesCases::ChannelDetectionError - Error: Incorrect error at FindChannelsL check: %d" ), err );
            RProperty::Set( KPSUidSensrvTest, iCompletedKey, err );
            iTestCaseCompleted = ETrue;
            iMutex.Signal();
            return;
            }
        }

    RProperty::Set( KPSUidSensrvTest, iCompletedKey, aError );
    iTestCaseCompleted = ETrue;
    iMutex.Signal();
    }

// -----------------------------------------------------------------------------
// CSensrvTestCases::GetChannelListenerInterfaceL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSensrvTestCases::GetChannelListenerInterfaceL( TUid /*aInterfaceUid*/,
	                                        TAny*& aInterface )
    {
    aInterface = reinterpret_cast<TAny*>(
			static_cast<MSensrvChannelListener*>( this ) );
    }

// -----------------------------------------------------------------------------
// CSensrvTestCases::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
/*
TInt CSensrvTestCases::?member_function( CStifItemParser& aItem )
   {
    // Print to UI
    _LIT( KSensrvTest, "?" );
    _LIT( KTestcase, "?" );
    TestModuleIf().Printf( 0, KSensrvTest, KTestcase );
    // Print to log file
    RDebug::Print( KTestcase );


    return KErrNone;
   }
*/

// ========================== OTHER EXPORTED FUNCTIONS =========================
// None

//  End of File

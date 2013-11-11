/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Module test for CSensorDataCompensator API.
*
*/


#include <e32debug.h>
#include <e32property.h>
#include <stifparser.h>
#include <stiftestinterface.h>
#include <sensordatacompensatorplugintypes.h>

#include "sensordatacompensatortest.h"

// LOCAL CONSTANTS AND MACROS
_LIT_SECURITY_POLICY_C1( KReadDeviceDataPolicy, ECapabilityReadDeviceData );
_LIT_SECURITY_POLICY_C1( KWriteDeviceDataPolicy, ECapabilityWriteDeviceData );

// ---------------------------------------------------------------------------
// DefineTestPubSubL
// ---------------------------------------------------------------------------
//
static void DefineTestPubSubL( TInt aKey, TInt aType )
    {
    RDebug::Printf("[SensDataCompTest] DefineTestPubSubL key %d, type %d", aKey, aType );
    TInt err( RProperty::Define(
        KTestPSUid, aKey, aType, KReadDeviceDataPolicy, KWriteDeviceDataPolicy ) );
    if ( err != KErrNone && err != KErrAlreadyExists )
        {
        User::Leave( err );
        }
    RDebug::Printf("[SensDataCompTest] DefineTestPubSubL return");
    }

// ---------------------------------------------------------------------------
// SetTestPubSubL
// ---------------------------------------------------------------------------
//
static void SetTestPubSubL( TInt aKey, TInt aValue )
    {
    RDebug::Printf("[SensDataCompTest] SetTestPubSubL key %d value %d", aKey, aValue );
    User::LeaveIfError( RProperty::Set( KTestPSUid, aKey, aValue ) );
    RDebug::Printf("[SensDataCompTest] SetTestPubSubL return");
    }

// ---------------------------------------------------------------------------
// SetTestPubSubL
// ---------------------------------------------------------------------------
//
static void SetTestPubSubL( TInt aKey, const TDesC8& aValue )
    {
    RDebug::Printf("[SensDataCompTest] SetTestPubSubL key %d", aKey );
    User::LeaveIfError( RProperty::Set( KTestPSUid, aKey, aValue ) );
    RDebug::Printf("[SensDataCompTest] SetTestPubSubL return");
    }

// ---------------------------------------------------------------------------
// CheckValue64
// ---------------------------------------------------------------------------
//
static TBool CheckValue64( TInt64 aValue, TInt64 aValueExpected, TInt64 aThreshold )
    {
    // Needed for max signed 32bit value checking
    return ( aValue >= ( aValueExpected - aThreshold ) &&
             aValue <= ( aValueExpected + aThreshold ) );

    }

// ---------------------------------------------------------------------------
// CompareAccelerometerAxisDataL
// ---------------------------------------------------------------------------
//
static void CompareAccelerometerAxisDataL (
    const TSensrvAccelerometerAxisData& aActual,
    const TSensrvAccelerometerAxisData& aExpected,
    const TInt aThreshold )
    {
    RDebug::Print( _L( "[SensDataCompTest] expected: [%d,%d,%d], actual: [%d,%d,%d] [+-%d]" ),
        aExpected.iAxisX, aExpected.iAxisY, aExpected.iAxisZ,
        aActual.iAxisX, aActual.iAxisY, aActual.iAxisZ, aThreshold );

    if ( !CheckValue64( aActual.iAxisX, aExpected.iAxisX, aThreshold ) ||
         !CheckValue64( aActual.iAxisY, aExpected.iAxisY, aThreshold ) ||
         !CheckValue64( aActual.iAxisZ, aExpected.iAxisZ, aThreshold ) )
        {
        User::Leave( KErrAbort );
        }

    }

// ---------------------------------------------------------------------------
// CompareMagnetometerAxisDataL
// ---------------------------------------------------------------------------
//
static void CompareMagnetometerAxisDataL (
    const TSensrvMagnetometerAxisData& aActual,
    const TSensrvMagnetometerAxisData& aExpected,
    const TInt aThreshold )
    {
    RDebug::Print( _L( "[SensDataCompTest] expected: [%d,%d,%d,%d,%d,%d], actual: [%d,%d,%d,%d,%d,%d] [+-%d]" ),
        aExpected.iAxisXRaw, aExpected.iAxisYRaw, aExpected.iAxisZRaw,
        aExpected.iAxisXCalibrated, aExpected.iAxisYCalibrated, aExpected.iAxisZCalibrated,
        aActual.iAxisXRaw, aActual.iAxisYRaw, aActual.iAxisZRaw,
        aActual.iAxisXCalibrated, aActual.iAxisYCalibrated, aActual.iAxisZCalibrated,
        aThreshold );

    if ( aActual.iAxisXRaw > aExpected.iAxisXRaw + aThreshold ||
         aActual.iAxisXRaw < aExpected.iAxisXRaw - aThreshold ||
    
         aActual.iAxisYRaw > aExpected.iAxisYRaw + aThreshold ||
         aActual.iAxisYRaw < aExpected.iAxisYRaw - aThreshold ||

         aActual.iAxisZRaw > aExpected.iAxisZRaw + aThreshold ||
         aActual.iAxisZRaw < aExpected.iAxisZRaw - aThreshold ||

         aActual.iAxisXCalibrated > aExpected.iAxisXCalibrated + aThreshold ||
         aActual.iAxisXCalibrated < aExpected.iAxisXCalibrated - aThreshold ||
    
         aActual.iAxisYCalibrated > aExpected.iAxisYCalibrated + aThreshold ||
         aActual.iAxisYCalibrated < aExpected.iAxisYCalibrated - aThreshold ||

         aActual.iAxisZCalibrated > aExpected.iAxisZCalibrated + aThreshold ||
         aActual.iAxisZCalibrated < aExpected.iAxisZCalibrated - aThreshold )
        {
        User::Leave( KErrAbort );
        }
        
    }

// ---------------------------------------------------------------------------
// CompareMagneticNorthDataL
// ---------------------------------------------------------------------------
//
static void CompareMagneticNorthDataL (
    const TSensrvMagneticNorthData& aActual,
    const TSensrvMagneticNorthData& aExpected )
    {
    RDebug::Print( _L( "[SensDataCompTest] expected: [%d], actual: [%d]" ),
        aExpected.iAngleFromMagneticNorth, aActual.iAngleFromMagneticNorth );

    if ( aActual.iAngleFromMagneticNorth != aExpected.iAngleFromMagneticNorth )
        {
        User::Leave( KErrAbort );
        }
    }

// ---------------------------------------------------------------------------
// CompareTappingDataL
// ---------------------------------------------------------------------------
//
static void CompareTappingDataL (
    const TSensrvTappingData& aActual,
    const TSensrvTappingData& aExpected )
    {
    RDebug::Print( _L( "[SensDataCompTest] expected: [%d], actual: [%d]" ),
        aExpected.iDirection, aActual.iDirection );

    if ( aActual.iDirection != aExpected.iDirection )
        {
        User::Leave( KErrAbort );
        }
    }

// ---------------------------------------------------------
// CSensorDataCompensatorTest::CSensorDataCompensatorTest
// Returns: KErrNone
// ---------------------------------------------------------
CSensorDataCompensatorTest::CSensorDataCompensatorTest( CTestModuleIf& aTestModuleIf ) :
    CScriptBase( aTestModuleIf )
    {
    RDebug::Printf("[SensDataCompTest] CSensorDataCompensatorTest::CSensorDataCompensatorTest()");
    RDebug::Printf("[SensDataCompTest] CSensorDataCompensatorTest::CSensorDataCompensatorTest return");
    }

// ---------------------------------------------------------
// CSensorDataCompensatorTest::ConstructL
// ---------------------------------------------------------
void CSensorDataCompensatorTest::ConstructL()
    {
    RDebug::Printf("[SensDataCompTest] CSensorDataCompensatorTest::ConstructL()");
    TInt i( 0 );
    for( ; i < ETestIntKeyCount; ++i )
        {
        DefineTestPubSubL( i, RProperty::EInt );
        SetTestPubSubL( i, 0 ); // Initialize to 0
        }
    for( ; i < ETestDes8KeyCount; ++i )
        {
        DefineTestPubSubL( i, RProperty::EByteArray );
        SetTestPubSubL( i, KNullDesC8 ); // Initialize to empty
        }
    RDebug::Printf("[SensDataCompTest] CSensorDataCompensatorTest::ConstructL return");
    }

// ---------------------------------------------------------
// CSensorDataCompensatorTest::NewL
// ---------------------------------------------------------
CSensorDataCompensatorTest* CSensorDataCompensatorTest::NewL( CTestModuleIf& aTestModuleIf )
    {
    RDebug::Printf("[SensDataCompTest] CSensorDataCompensatorTest::NewL()");
    CSensorDataCompensatorTest* self = new (ELeave) CSensorDataCompensatorTest(aTestModuleIf);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    RDebug::Printf("[SensDataCompTest] CSensorDataCompensatorTest::NewL self %p", self );
    return self;
    }

// ---------------------------------------------------------
// CSensorDataCompensatorTest::~CSensorDataCompensatorTest
// ---------------------------------------------------------
CSensorDataCompensatorTest::~CSensorDataCompensatorTest()
    {
    delete iInputCrap;
    RDebug::Printf("[SensDataCompTest] CSensorDataCompensatorTest::~CSensorDataCompensatorTest()");
    RDebug::Printf("[SensDataCompTest] CSensorDataCompensatorTest::~CSensorDataCompensatorTest return");
    }

// ---------------------------------------------------------
// CSensorDataCompensatorTest::LibEntryL
// Returns: Poiter to CSensorDataCompensatorTest class
// ---------------------------------------------------------
EXPORT_C CSensorDataCompensatorTest* LibEntryL( CTestModuleIf& aTestModuleIf )
    {
    RDebug::Printf("[SensDataCompTest] CSensorDataCompensatorTest::LibEntryL()");
    CSensorDataCompensatorTest* libEntry( CSensorDataCompensatorTest::NewL(aTestModuleIf) );
    RDebug::Printf("[SensDataCompTest] CSensorDataCompensatorTest::LibEntryL libEntry %p", libEntry );
    return libEntry;
    }

// ---------------------------------------------------------
// CSensorDataCompensatorTest::RunMethodL
// ---------------------------------------------------------
TInt CSensorDataCompensatorTest::RunMethodL( CStifItemParser& aItem )
    {
    RDebug::Printf("[SensDataCompTest] CSensorDataCompensatorTest::RunMethodL()");
    const TStifFunctionInfo KFunctions[] =
        {
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function.
        ENTRY( "CreateCompensator", CSensorDataCompensatorTest::CreateCompensatorL ),
        ENTRY( "DestroyCompensator", CSensorDataCompensatorTest::DestroyCompensatorL ),
        ENTRY( "SetState", CSensorDataCompensatorTest::SetStateL ),
        ENTRY( "AppenDataItem", CSensorDataCompensatorTest::AppendDataItemL ),
        ENTRY( "ResetDataItems", CSensorDataCompensatorTest::ResetDataItemsL ),
        ENTRY( "CompensateAndVerifyDataItems", CSensorDataCompensatorTest::CompensateAndVerifyDataItemsL ),
        ENTRY( "ResetAllDataItems", CSensorDataCompensatorTest::ResetAllDataItems )
        };
    const TInt count( sizeof( KFunctions ) / sizeof( TStifFunctionInfo ) );
    TInt ret( RunInternalL( KFunctions, count, aItem ) );
    RDebug::Printf("[SensDataCompTest] CSensorDataCompensatorTest::RunMethodL return - ret %d", ret );
    return ret;
    }

// ---------------------------------------------------------
// CSensorDataCompensatorTest::CreateCompensatorL
// ---------------------------------------------------------

TInt CSensorDataCompensatorTest::CreateCompensatorL( CStifItemParser& aItem )
    {
    RDebug::Printf("[SensDataCompTest] CSensorDataCompensatorTest::CreateCompensatorL()");
    TInt dataType( 0 );
    User::LeaveIfError( aItem.GetNextInt( dataType ) );
    iDataType = (TSensrvChannelDataTypeId)dataType;
    TInt compensationType(0);
    User::LeaveIfError(aItem.GetNextInt(compensationType));

    iCompensatorApi = CSensorDataCompensator::NewL( iDataType, (TSensorCompensationType)compensationType );
    if ( iDataType != iCompensatorApi->GetChannelDataType() ||
         compensationType != iCompensatorApi->GetCompensationType() )
        {
        User::Leave( KErrArgument );
        }
    RDebug::Printf("[SensDataCompTest] CSensorDataCompensatorTest::CreateCompensatorL return");
    return KErrNone;
    }

// ---------------------------------------------------------
// CSensorDataCompensatorTest::DestroyCompensatorL
// ---------------------------------------------------------
TInt CSensorDataCompensatorTest::DestroyCompensatorL( CStifItemParser& /*aItem*/ )
    {
    RDebug::Printf("[SensDataCompTest] CSensorDataCompensatorTest::DestroyCompensatorL()");
    delete iCompensatorApi;
    iCompensatorApi = NULL;
    ResetAllDataItems();
    RDebug::Printf("[SensDataCompTest] CSensorDataCompensatorTest::DestroyCompensatorL return");
    return KErrNone;
    }

// ---------------------------------------------------------
// CSensorDataCompensatorTest::SetStateL
// ---------------------------------------------------------
TInt CSensorDataCompensatorTest::SetStateL( CStifItemParser& aItem )
    {
    RDebug::Printf("[SensDataCompTest] CSensorDataCompensatorTest::SetStateL()");
    TInt key(0);
    User::LeaveIfError(aItem.GetNextInt(key));
    TInt value(0);
    User::LeaveIfError(aItem.GetNextInt(value));
    switch( key )
        {
        case ETestOutputDataThreshold:
            {
            iOutputDataThreshold = value;
            break;
            }
        case ETestInputDataExpectedError:
            {
            iExpectedError = value;
            break;
            }
        default:
            {
            SetTestPubSubL( key, value );
            break;
            }
        }
    RDebug::Printf("[SensDataCompTest] CSensorDataCompensatorTest::SetStateL return" );
    return KErrNone;
    }

// ---------------------------------------------------------
// CSensorDataCompensatorTest::AppendDataItemL
// ---------------------------------------------------------
TInt CSensorDataCompensatorTest::AppendDataItemL( CStifItemParser& aItem )
    {
    RDebug::Printf("[SensDataCompTest] CSensorDataCompensatorTest::AppendDataItemL()");

    TInt arrayId( 0 );
    User::LeaveIfError( aItem.GetNextInt( arrayId ) );

    if ( arrayId == ETestPluginDataItemCrap )
        {
        // Fetch data for plugin input
        TInt size( 0 );
        User::LeaveIfError(aItem.GetNextInt( size ));
        HBufC8* crap = HBufC8::NewLC( size );
        SetTestPubSubL(ETestKeyPluginDataAppend, *crap);
        CleanupStack::PopAndDestroy( crap );
        }
    else if ( arrayId == ETestInputDataItemCrap )
        {
        // Fetch data for API input
        TInt size( 0 );
        User::LeaveIfError(aItem.GetNextInt( size ));
        delete iInputCrap;
        iInputCrap = NULL;
        iInputCrap = HBufC8::NewL( size );
        }
    else
        {
        switch( iDataType )
            {
            case TSensrvAccelerometerAxisData::KDataTypeId:
                {
                TInt axisX(0), axisY(0), axisZ(0);
                User::LeaveIfError(aItem.GetNextInt( axisX ));
                User::LeaveIfError(aItem.GetNextInt( axisY ));
                User::LeaveIfError(aItem.GetNextInt( axisZ ));
                if ( ETestInputDataItem == arrayId )
                    {
                    iInputAccelerometerAxisData.iAxisX = axisX;
                    iInputAccelerometerAxisData.iAxisY = axisY;
                    iInputAccelerometerAxisData.iAxisZ = axisZ;
                    }
                else if ( ETestOutputDataItem == arrayId )
                    {
                    iExpectedAccelerometerAxisData.iAxisX = axisX;
                    iExpectedAccelerometerAxisData.iAxisY = axisY;
                    iExpectedAccelerometerAxisData.iAxisZ = axisZ;
                    }
                else if ( ETestPluginDataItem == arrayId )
                    {
                    TCompensationTypeAxisData axisData;
                    axisData.iAxisX = axisX;
                    axisData.iAxisY = axisY;
                    axisData.iAxisZ = axisZ;
                    TCompensationTypeAxisDataPckg pckg(axisData);
                    SetTestPubSubL(ETestKeyPluginDataAppend, pckg);
                    }
                else
                    {
                    User::Leave( KErrArgument );
                    }
                break;
                }
            case TSensrvMagnetometerAxisData::KDataTypeId:
                {
                TInt axisXRaw(0), axisYRaw(0), axisZRaw(0), axisXCalibrated(0), axisYCalibrated(0), axisZCalibrated(0);
                User::LeaveIfError(aItem.GetNextInt( axisXRaw ));
                User::LeaveIfError(aItem.GetNextInt( axisYRaw ));
                User::LeaveIfError(aItem.GetNextInt( axisZRaw ));
                if ( ETestPluginDataItem != arrayId )
                    {
                    User::LeaveIfError(aItem.GetNextInt( axisXCalibrated ));
                    User::LeaveIfError(aItem.GetNextInt( axisYCalibrated ));
                    User::LeaveIfError(aItem.GetNextInt( axisZCalibrated ));
                    }
                if ( ETestInputDataItem == arrayId )
                    {
                    iInputMagnetometerAxisData.iAxisXRaw = axisXRaw;
                    iInputMagnetometerAxisData.iAxisYRaw = axisYRaw;
                    iInputMagnetometerAxisData.iAxisZRaw = axisZRaw;
                    iInputMagnetometerAxisData.iAxisXCalibrated = axisXCalibrated;
                    iInputMagnetometerAxisData.iAxisYCalibrated = axisYCalibrated;
                    iInputMagnetometerAxisData.iAxisZCalibrated = axisZCalibrated;
                    }
                else if ( ETestOutputDataItem == arrayId )
                    {
                    iExpectedMagnetometerAxisData.iAxisXRaw = axisXRaw;
                    iExpectedMagnetometerAxisData.iAxisYRaw = axisYRaw;
                    iExpectedMagnetometerAxisData.iAxisZRaw = axisZRaw;
                    iExpectedMagnetometerAxisData.iAxisXCalibrated = axisXCalibrated;
                    iExpectedMagnetometerAxisData.iAxisYCalibrated = axisYCalibrated;
                    iExpectedMagnetometerAxisData.iAxisZCalibrated = axisZCalibrated;
                    }
                else if ( ETestPluginDataItem == arrayId )
                    {
                    TCompensationTypeAxisData axisData;
                    axisData.iAxisX = axisXRaw;
                    axisData.iAxisY = axisYRaw;
                    axisData.iAxisZ = axisZRaw;
                    TCompensationTypeAxisDataPckg pckg(axisData);
                    SetTestPubSubL(ETestKeyPluginDataAppend, pckg);
                    }
                else
                    {
                    User::Leave( KErrArgument );
                    }
                break;
                }
            case TSensrvMagneticNorthData::KDataTypeId:
                {
                TInt angleFromMagneticNorth(0);
                User::LeaveIfError(aItem.GetNextInt( angleFromMagneticNorth ));
                if (ETestInputDataItem == arrayId )
                    {
                    iInputMagneticNorthData.iAngleFromMagneticNorth = angleFromMagneticNorth;
                    }
                else if ( ETestOutputDataItem == arrayId )
                    {
                    iExpectedMagneticNorthData.iAngleFromMagneticNorth = angleFromMagneticNorth;
                    }
                else if ( ETestPluginDataItem == arrayId )
                    {
                    TCompensationTypeAngleData angleData;
                    angleData.iAngle = angleFromMagneticNorth;
                    TCompensationTypeAngleDataPckg pckg(angleData);
                    SetTestPubSubL(ETestKeyPluginDataAppend, pckg);
                    }
                else
                    {
                    User::Leave( KErrArgument );
                    }
                break;
                }
            case TSensrvTappingData::KDataTypeId:
                {
                if ( ETestInputDataItem == arrayId )
                    {
                    TInt direction(0);
                    User::LeaveIfError(aItem.GetNextInt( direction ));
                    iInputTappingData.iDirection = direction;
                    }
                else if ( ETestOutputDataItem == arrayId )
                    {
                    TInt direction(0);
                    User::LeaveIfError(aItem.GetNextInt( direction ));
                    iExpectedTappingData.iDirection = direction;
                    }
                else if ( ETestPluginDataItem == arrayId )
                    {
                    TCompensationTypeAxisData axisData;
                    User::LeaveIfError(aItem.GetNextInt( axisData.iAxisX ));
                    User::LeaveIfError(aItem.GetNextInt( axisData.iAxisY ));
                    User::LeaveIfError(aItem.GetNextInt( axisData.iAxisZ ));
                    TCompensationTypeAxisDataPckg pckg(axisData);
                    SetTestPubSubL(ETestKeyPluginDataAppend, pckg);
                    }
                else
                    {
                    User::Leave( KErrArgument );
                    }
                break;
                }
            default:
                {
                User::Leave( KErrGeneral );
                break;
                }
            }
        }
    RDebug::Printf("[SensDataCompTest] CSensorDataCompensatorTest::AppendDataItemL return" );
    return KErrNone;
    }

// ---------------------------------------------------------
// CSensorDataCompensatorTest::ResetDataItemsL
// ---------------------------------------------------------
TInt CSensorDataCompensatorTest::ResetDataItemsL( CStifItemParser& aItem )
    {
    RDebug::Printf("[SensDataCompTest] CSensorDataCompensatorTest::ResetDataItemsL()");

    delete iInputCrap;
    iInputCrap = NULL;
    iExpectedError = KErrNone;
    iOutputDataThreshold = 0;

    TInt arrayId( 0 );
    User::LeaveIfError( aItem.GetNextInt( arrayId ) );
    switch( iDataType )
        {
        case TSensrvAccelerometerAxisData::KDataTypeId:
            {
            if ( ETestInputDataItem == arrayId )
                {
                iInputAccelerometerAxisData.iAxisX = 0;
                iInputAccelerometerAxisData.iAxisY = 0;
                iInputAccelerometerAxisData.iAxisZ = 0;
                }
            else if ( ETestOutputDataItem == arrayId )
                {
                iExpectedAccelerometerAxisData.iAxisX = 0;
                iExpectedAccelerometerAxisData.iAxisY = 0;
                iExpectedAccelerometerAxisData.iAxisZ = 0;
                }
            else
                {
                User::Leave( KErrArgument );
                }
            break;
        }
        case TSensrvMagnetometerAxisData::KDataTypeId:
            {
            if ( ETestInputDataItem == arrayId )
                {
                iInputMagnetometerAxisData.iAxisXRaw = 0;
                iInputMagnetometerAxisData.iAxisYRaw = 0;
                iInputMagnetometerAxisData.iAxisZRaw = 0;
                iInputMagnetometerAxisData.iAxisXCalibrated = 0;
                iInputMagnetometerAxisData.iAxisYCalibrated = 0;
                iInputMagnetometerAxisData.iAxisZCalibrated = 0;
                }
            else if ( ETestOutputDataItem == arrayId )
                {
                iExpectedMagnetometerAxisData.iAxisXRaw = 0;
                iExpectedMagnetometerAxisData.iAxisYRaw = 0;
                iExpectedMagnetometerAxisData.iAxisZRaw = 0;
                iExpectedMagnetometerAxisData.iAxisXCalibrated = 0;
                iExpectedMagnetometerAxisData.iAxisYCalibrated = 0;
                iExpectedMagnetometerAxisData.iAxisZCalibrated = 0;
                }
            else
                {
                User::Leave( KErrArgument );
                }
            break;
            }
        case TSensrvMagneticNorthData::KDataTypeId:
            {
            if ( ETestInputDataItem == arrayId )
                {
                iInputMagneticNorthData.iAngleFromMagneticNorth = 0;
                }
            else if ( ETestOutputDataItem == arrayId )
                {
                iExpectedMagneticNorthData.iAngleFromMagneticNorth = 0;
                }
            else
                {
                User::Leave( KErrArgument );
                }
            break;
            }
        case TSensrvTappingData::KDataTypeId:
            {
            if ( ETestInputDataItem == arrayId )
                {
                iInputTappingData.iDirection = 0;
                }
            else if ( ETestOutputDataItem == arrayId )
                {
                iExpectedTappingData.iDirection = 0;
                }
            else
                {
                User::Leave( KErrArgument );
                }
            break;
            }
        default:
            {
            User::Leave( KErrGeneral );
            break;
            }
        }
    RDebug::Printf("[SensDataCompTest] CSensorDataCompensatorTest::ResetDataItemsL return" );
    return KErrNone;
    }

// ---------------------------------------------------------
// CSensorDataCompensatorTest::CompensateAndVerifyDataItemsL
// ---------------------------------------------------------
TInt CSensorDataCompensatorTest::CompensateAndVerifyDataItemsL( CStifItemParser& aItem )
    {
    RDebug::Printf("[SensDataCompTest] CSensorDataCompensatorTest::CompensateAndVerifyDataItemsL()");

    TInt compensateCount( 0 );
    aItem.GetNextInt( compensateCount );

    if ( iInputCrap )
        {
        TPtr8 ptr( iInputCrap->Des() );
        CompensateDataAndCheckErrorL( ptr, compensateCount );
        }
    else
        {
        switch ( iDataType )
            {
            case 0: // To test unknown handler
                {
                TBuf8<1> buf; // Empty buffer
                CompensateDataAndCheckErrorL( buf, compensateCount );
                if ( buf.Length() > 0 ) // Must be unchanged
                    {
                    User::Leave(KErrGeneral);
                    }
                break;
                }
            case TSensrvAccelerometerAxisData::KDataTypeId:
                {
                TPckgBuf<TSensrvAccelerometerAxisData> dataPckg( iInputAccelerometerAxisData );
                CompensateDataAndCheckErrorL( dataPckg, compensateCount );
                CompareAccelerometerAxisDataL( dataPckg(), iExpectedAccelerometerAxisData, iOutputDataThreshold );
                break;
                }
            case TSensrvMagnetometerAxisData::KDataTypeId:
                {
                TPckgBuf<TSensrvMagnetometerAxisData> dataPckg( iInputMagnetometerAxisData );
                CompensateDataAndCheckErrorL( dataPckg, compensateCount );
                CompareMagnetometerAxisDataL( dataPckg(), iExpectedMagnetometerAxisData, iOutputDataThreshold );
                break;
                }
            case TSensrvMagneticNorthData::KDataTypeId:
                {
                TPckgBuf<TSensrvMagneticNorthData> dataPckg( iInputMagneticNorthData );
                CompensateDataAndCheckErrorL( dataPckg, compensateCount );
                CompareMagneticNorthDataL( dataPckg(), iExpectedMagneticNorthData );
                break;
                }
            case TSensrvTappingData::KDataTypeId:
                {
                TPckgBuf<TSensrvTappingData> dataPckg( iInputTappingData );
                CompensateDataAndCheckErrorL( dataPckg, compensateCount );
                CompareTappingDataL( dataPckg(), iExpectedTappingData );
                break;
                }
            default:
                {
                break;
                }
            }
        }
    RDebug::Printf("[SensDataCompTest] CSensorDataCompensatorTest::CompensateAndVerifyDataItemsL return - return" );
    return KErrNone;
    }

// ---------------------------------------------------------
// CSensorDataCompensatorTest::ResetAllDataItems
// ---------------------------------------------------------
void CSensorDataCompensatorTest::ResetAllDataItems()
    {
    RDebug::Printf("[SensDataCompTest] CSensorDataCompensatorTest::ResetAllDataItems()");

    delete iInputCrap;
    iInputCrap = NULL;
    iExpectedError = KErrNone;
    iOutputDataThreshold = 0;

    iInputAccelerometerAxisData.iAxisX = 0;
    iInputAccelerometerAxisData.iAxisY = 0;
    iInputAccelerometerAxisData.iAxisZ = 0;
    iExpectedAccelerometerAxisData.iAxisX = 0;
    iExpectedAccelerometerAxisData.iAxisY = 0;
    iExpectedAccelerometerAxisData.iAxisZ = 0;

    iInputMagnetometerAxisData.iAxisXRaw = 0;
    iInputMagnetometerAxisData.iAxisYRaw = 0;
    iInputMagnetometerAxisData.iAxisZRaw = 0;
    iInputMagnetometerAxisData.iAxisXCalibrated = 0;
    iInputMagnetometerAxisData.iAxisYCalibrated = 0;
    iInputMagnetometerAxisData.iAxisZCalibrated = 0;
    iExpectedMagnetometerAxisData.iAxisXRaw = 0;
    iExpectedMagnetometerAxisData.iAxisYRaw = 0;
    iExpectedMagnetometerAxisData.iAxisZRaw = 0;
    iExpectedMagnetometerAxisData.iAxisXCalibrated = 0;
    iExpectedMagnetometerAxisData.iAxisYCalibrated = 0;
    iExpectedMagnetometerAxisData.iAxisZCalibrated = 0;

    iInputMagneticNorthData.iAngleFromMagneticNorth = 0;
    iExpectedMagneticNorthData.iAngleFromMagneticNorth = 0;

    iInputTappingData.iDirection = 0;
    iExpectedTappingData.iDirection = 0;

    RDebug::Printf("[SensDataCompTest] CSensorDataCompensatorTest::ResetAllDataItems return" );
    }

// ---------------------------------------------------------
// CSensorDataCompensatorTest::CompensateDataAndCheckErrorL
// ---------------------------------------------------------
void CSensorDataCompensatorTest::CompensateDataAndCheckErrorL( TDes8& aDes, TInt aCount )
    {
    RDebug::Printf("[SensDataCompTest] CSensorDataCompensatorTest::CompensateDataAndCheckErrorL() %d", aCount);

    // Compensate first once for verification
    TInt err( iCompensatorApi->Compensate( aDes ) );
    if ( err != KErrNone && err != iExpectedError )
        {
        RDebug::Printf("[SensDataCompTest] CSensorDataCompensatorTest::CompensateDataAndCheckErrorL err %d", err );
        User::Leave( err );
        }
    if ( aCount > 0 )
        {
        // Loop compensation for perf tests
        HBufC8* data = HBufC8::NewLC( aDes.MaxSize() );
        TPtr8 ptr( data->Des() );
        ptr.Copy( aDes );
        for( TInt i( 0 ); i < aCount; ++i )
            {
            err = iCompensatorApi->Compensate( aDes );
            if ( err != KErrNone && err != iExpectedError )
                {
                RDebug::Printf("[SensDataCompTest] CSensorDataCompensatorTest::CompensateDataAndCheckErrorL err %d", err );
                User::Leave( err );
                }
            }
        aDes.Copy( ptr );
        CleanupStack::PopAndDestroy( data );
        }
    RDebug::Printf("[SensDataCompTest] CSensorDataCompensatorTest::CompensateDataAndCheckErrorL return - return" );
    }

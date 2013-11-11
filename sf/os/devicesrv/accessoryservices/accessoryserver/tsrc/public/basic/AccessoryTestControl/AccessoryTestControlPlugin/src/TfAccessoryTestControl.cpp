/*
 * Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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

// INCLUDE FILES

#include <ctffactory.h>
#include <tftypes.h>
#include <AccPolSubblockNameArray.h>
#include "siftrace.h"
#include "CtfAccessoryTestcontrol.h"
#include "CtfAccessoryTestCaseControl.h"
#include <AccPolProprietaryNameValuePairs.h>
#include <AccConGenericID.h>
#include <AccPolPropGenericid.h>
#include <AccessoryConnectionBase.h>
#include <s32mem.h>

#include <accpolobjectcon.h>
#include <accpolaudiostreamformat.h>
#include <accpolterminalbase.h>
#include <accpoloutputterminal.h>
#include <accpolinputterminal.h>
#include <accpolmixerunit.h>
#include <accpolfeatureunit.h>
#include <accpolvolumecontrol.h>
#include <accpolmutecontrol.h>
#include <accpolselectorunit.h>

#include <StartupDomainPSKeys.h>

#include <E32Math.h>
#include "AccClientServerMessages.h"
#include <e32property.h>

#include <accpolhdmiobjectcon.h>
#include <tvoutconfigdef.h>

//
// ----------------------------------------------------------------------------------
// MTFAccessoryTestControl* GetAccessoryTestControl
// ----------------------------------------------------------------------------------	  
MTFAccessoryTestControl* GetAccessoryTestControl( void )
    {
    CTFStub* stub = CTFFactory::Stub( KTFStubTypeAccessoryControl );
    return STATIC_CAST( CTFAccessoryTestControl*, stub );
    }
//
// ----------------------------------------------------------------------------------
// MTFAccessoryTestControl::MTFAccessoryTestControl
// ----------------------------------------------------------------------------------	  
MTFAccessoryTestControl::MTFAccessoryTestControl( void )
    {
    }
//
// ----------------------------------------------------------------------------------
// MTFAccessoryTestControl::~MTFAccessoryTestControl
// ----------------------------------------------------------------------------------	  
MTFAccessoryTestControl::~MTFAccessoryTestControl( void )
    {
    }

//
// ----------------------------------------------------------------------------------
// CTFAccessoryTestControl::CTFAccessoryTestControl
// ----------------------------------------------------------------------------------	
CTFAccessoryTestControl::CTFAccessoryTestControl( void ) :
    CTFRemoteStub( KTFStubTypeAccessoryControl ),
        iStack( CTFTestControlObserver::iOffset ), iStackIter( iStack )
    {
    COMPONENT_TRACE( ( _L( "ACCESSORYTESTCONTROL - CTFAccessoryTestControl::CTFAccessoryTestControl()" ) ) );

    COMPONENT_TRACE( ( _L( "ACCESSORYTESTCONTROL - CTFAccessoryTestControl::CTFAccessoryTestControl() - return" ) ) );
    }

//lint -e1740 Pointer not directly deleted by destructor
//
// ----------------------------------------------------------------------------------
// CTFAccessoryTestControl::~CTFAccessoryTestControl
// ----------------------------------------------------------------------------------	
CTFAccessoryTestControl::~CTFAccessoryTestControl( void )
    {

    COMPONENT_TRACE( ( _L( "ACCESSORYTESTCONTROL - CTFAccessoryTestControl::~CTFAccessoryTestControl()" ) ) );
    RProperty::Delete( KTFAccessoryTestProperty, KTFAccessoryMethod );

    ResetAccessoryServices();

    iAccessoryServer.Disconnect();

    COMPONENT_TRACE( ( _L( "ACCESSORYTESTCONTROL - CTFAccessoryTestControl::~CTFAccessoryTestControl() - Accessory Server connection closed" ) ) );

    }
//lint +e1740    
//
// ----------------------------------------------------------------------------------
// CTFAccessoryTestControl::InitializeL
// ----------------------------------------------------------------------------------	   
void CTFAccessoryTestControl::InitializeL( void )
    {

    TEST_CASE_TRACE( ( _L( "CTFAccessoryTestControl::InitializeL( void ) - enter" ) ) );
    TInt retval( KErrNone );

    iAccessoryConnection = NULL;
    iAccessorySingleConnection = NULL;
    iAccessoryControl = NULL;
    iAccessorySettings = NULL;
    iAccessoryMode = NULL;
    iAccessoryAudioControl = NULL;
    iAccessoryBTControl = NULL;

    iStreamContainer = NULL;
    iTopologyContainer = NULL;
    iSpeakerRightVolume = NULL;
    iSpeakerLeftVolume = NULL;
    iSpeakerMute = NULL;
    iMicRightVolume = NULL;
    iMicLeftVolume = NULL;

    TAccPolGenericID emptyInstance;
    for( TInt i( 0 ); i < 10; i++ )
        {
        iGenericId[i] = emptyInstance;
        }

    COMPONENT_TRACE( ( _L( "ACCESSORYTESTCONTROL - CTFAccessoryTestControl::InitializeL()" ) ) );

    User::LeaveIfError( iAccessoryServer.Connect() );
    COMPONENT_TRACE( ( _L( "ACCESSORYTESTCONTROL - CTFAccessoryTestControl::InitializeL() - Connected to accessory server" ) ) );

    retval = RProperty::Define( KTFAccessoryTestProperty,
        KTFAccessoryMethod,
        RProperty::EByteArray );
    if( retval != KErrAlreadyExists )
        User::LeaveIfError( retval );
    //attach to property


    RProperty systemState;
    User::LeaveIfError( systemState.Attach( KPSUidStartup, KPSGlobalSystemState ) );
    TInt value( KErrNone );
    //check value
    TInt err = systemState.Get( value );

    if( err != KErrNone )
        {
        err = systemState.Define( KPSUidStartup,
            KPSGlobalSystemState,
            RProperty::EInt );
        TInt err = systemState.Get( value );
        }

    if( value != ESwStateNormalRfOn || value != ESwStateNormalRfOff || value
        != ESwStateNormalBTSap )
        {
        TInt err = systemState.Set( KPSUidStartup,
            KPSGlobalSystemState,
            ESwStateNormalRfOn );
        }

    iInitialized = ETrue;

    COMPONENT_TRACE( ( _L( "ACCESSORYTESTCONTROL - CTFAccessoryTestControl::InitializeL - return" ) ) );
    TEST_CASE_TRACE( ( _L( "CTFAccessoryTestControl::InitializeL( void ) - return" ) ) );
    }
//
// ----------------------------------------------------------------------------------
// CTFAccessoryTestControl::ResetAccessoryServices
// ----------------------------------------------------------------------------------	
void CTFAccessoryTestControl::ResetAccessoryServices( void )
    {
    ResetAccessoryServer();

    iInitialized = EFalse;

    iAccessoryServer.Close();
    }

//
// ----------------------------------------------------------------------------------
// CTFAccessoryTestControl::ResetAccessoryServer
// ----------------------------------------------------------------------------------	
void CTFAccessoryTestControl::ResetAccessoryServer( void )
    {

    COMPONENT_TRACE( ( _L( "CTFAccessoryTestControl::ResetAccessoryServer()" ) ) );

    CTFTestControlObserver* observerItem;
    if( !iStack.IsEmpty() )
        {
        iStackIter.SetToFirst();

        while( ( observerItem = iStackIter++ ) != NULL )
            {
            observerItem->Cancel();
            User::After( 2000000 );
            iStack.Remove( *observerItem );
            COMPONENT_TRACE( ( _L( "ACCESSORYTESTCONTROL - CTFAccessoryTestControl::ResetAccessoryServer -- Remove Observer" ) ) );
            delete observerItem;
            COMPONENT_TRACE( ( _L( "ACCESSORYTESTCONTROL - CTFAccessoryTestControl::ResetAccessoryServer -- Delete Observer" ) ) );
            };
        }

    iStack.Reset();

    TAccPolGenericID emptyInstance;

    for( TInt i( 0 ); i < 10; i++ )
        {
        COMPONENT_TRACE( ( _L( "ACCESSORYTESTCONTROL - CTFAccessoryTestControl::ResetAccessoryServer -- Clear instances" ) ) );
        if( KErrNotFound != iGenericId[i].UniqueID() )
            {
            if( iGenericId[i].PhysicalConnectionCaps( KPCBluetooth ) )
                {
                COMPONENT_TRACE( ( _L( "ACCESSORYTESTCONTROL - CTFAccessoryTestControl::ResetAccessoryServer -- BT Found" ) ) );
                TBTDevAddr BTDevAddr( iGenericId[i].DeviceAddress() );
                if( iAccessoryBTControl != NULL )
                    {
                    TRequestStatus status;
                    status = KRequestPending;
                    iAccessoryBTControl->DisconnectAccessory( status, BTDevAddr );
                    User::WaitForRequest( status );
                    COMPONENT_TRACE( ( _L( "ACCESSORYTESTCONTROL - CTFAccessoryTestControl::ResetAccessoryServer -- BT Disconnected" ) ) );
                    }
                }
            else
                {
                COMPONENT_TRACE( ( _L( "ACCESSORYTESTCONTROL - CTFAccessoryTestControl::ResetAccessoryServer -- Wired Found" ) ) );
                TPckgBuf<TTFAccessoryPublishAndSubscribe> buf;
                buf().iMethod = ETFAsyDisc;
                User::LeaveIfError( iSubscribe.Set( KTFAccessoryTestProperty,
                    KTFAccessoryMethod,
                    buf ) );
                COMPONENT_TRACE( ( _L( "ACCESSORYTESTCONTROL - CTFAccessoryTestControl::ResetAccessoryServer -- Wired Disconnected" ) ) );
                }
            }
        iGenericId[i] = emptyInstance;
        }

    if( iAccessoryConnection != NULL )
        {
        iAccessoryConnection->CloseSubSession();
        delete iAccessoryConnection;
        iAccessoryConnection = NULL;
        }

    if( iAccessorySingleConnection != NULL )
        {
        iAccessorySingleConnection->CloseSubSession();
        delete iAccessorySingleConnection;
        iAccessorySingleConnection = NULL;
        }
    if( iAccessoryControl != NULL )
        {
        iAccessoryControl->CloseSubSession();
        delete iAccessoryControl;
        iAccessoryControl = NULL;
        }
    if( iAccessorySettings != NULL )
        {
        iAccessorySettings->CloseSubSession();
        delete iAccessorySettings;
        iAccessorySettings = NULL;
        }
    if( iAccessoryMode != NULL )
        {
        iAccessoryMode->CloseSubSession();
        delete iAccessoryMode;
        iAccessoryMode = NULL;
        }

    if( iAccessoryAudioControl != NULL )
        {
        iAccessoryAudioControl->CloseSubSession();
        delete iAccessoryAudioControl;
        iAccessoryAudioControl = NULL;
        }

    if( iAccessoryBTControl != NULL )
        {
        iAccessoryBTControl->CloseSubSession();
        delete iAccessoryBTControl;
        iAccessoryBTControl = NULL;
        }

    if( iSpeakerRightVolume != NULL )
        {
        delete iSpeakerRightVolume;
        iSpeakerRightVolume = NULL;
        }

    if( iSpeakerLeftVolume != NULL )
        {
        delete iSpeakerLeftVolume;
        iSpeakerLeftVolume = NULL;
        }

    if( iSpeakerMute != NULL )
        {
        delete iSpeakerMute;
        iSpeakerMute = NULL;
        }

    if( iMicRightVolume != NULL )
        {
        delete iMicRightVolume;
        iMicRightVolume = NULL;
        }

    if( iMicLeftVolume != NULL )
        {
        delete iMicLeftVolume;
        iMicLeftVolume = NULL;
        }

    if( iStreamContainer != NULL )
        {
        delete iStreamContainer;
        iStreamContainer = NULL;
        }

    if( iTopologyContainer != NULL )
        {
        delete iTopologyContainer;
        iTopologyContainer = NULL;
        }

    COMPONENT_TRACE( ( _L( "CTFAccessoryTestControl::ResetAccessoryServer() - Success" ) ) );
    }
//
// ----------------------------------------------------------------------------------
// CTFAccessoryTestControl::CancelOutStandingRequest
// ----------------------------------------------------------------------------------	    
void CTFAccessoryTestControl::CancelOutStandingRequest()
    {
    iObserverItem->Cancel();
    }

//
// ----------------------------------------------------------------------------------
// CTFAccessoryTestControl::FillObjectL
// ----------------------------------------------------------------------------------
void CTFAccessoryTestControl::FillObjectL( CAccPolAudioStreamFormatCon* aContainer,
    TStreamConObjectType aobjectType,
    TInt64 aNumberOfObjects )
    {

    for( TInt i = 0; i < aNumberOfObjects; i++ )
        {
        switch( aobjectType )
            {
            case EAccPolAudioInputStreamObject:
                {
                CAccPolAudioStreamFormat* stream =
                    CAccPolAudioStreamFormat::NewLC( aobjectType,
                        i,
                        KUidFormatPCM16,
                        CAccPolAudioStreamFormat::EStereo,
                        CAccPolAudioStreamFormat::EBitsPerSample16,
                        i + 200 );
                aContainer->AddL( *stream );
                CleanupStack::PopAndDestroy( stream );
                }
                break;

            case EAccPolAudioOutputStreamObject:
                {

                CAccPolAudioStreamFormat
                    * stream =
                        CAccPolAudioStreamFormat::NewLC( EAccPolAudioOutputStreamObject );

                stream->SetUnitId( i );
                stream->SetAudioFormat( KUidFormatPCM16 );
                stream->SetStreamType( CAccPolAudioStreamFormat::EMono );
                stream->SetBitResolution( CAccPolAudioStreamFormat::EBitsPerSample32 );
                stream->SetSamFreq( i + 200 );

                aContainer->AddL( *stream );
                CleanupStack::PopAndDestroy( stream );
                }
                break;

            default:
                {
                TRACE_ASSERT_ALWAYS;
                }
                break;
            }
        }

    }

//
// ----------------------------------------------------------------------------------
// CTFAccessoryTestControl::CreateObjectPubSub
// ----------------------------------------------------------------------------------
void CTFAccessoryTestControl::CreateObjectPubSubL( TStreamConObjectType aobjectType,
    TInt64 aNumberOfObjects,
    TStreamConObjectType aobjectType2,
    TInt64 aNumberOfObjects2 )
    {
    RProperty prop;
    prop.Define( KTFAccessoryTestProperty,
        KTFAccessoryObjectMethod,
        RProperty::EByteArray );

    CAccPolAudioStreamFormatCon* container =
        CAccPolAudioStreamFormatCon::NewLC();

    FillObjectL( container, aobjectType, aNumberOfObjects );
    FillObjectL( container, aobjectType2, aNumberOfObjects2 );

    CBufFlat* buf = CBufFlat::NewL( KAccSrvObjectBaseStreamBufGranularity );
    CleanupStack::PushL( buf );
    buf->ResizeL( KAccSrvObjectBaseStreamBufMaxSize );

    //Externalize container to buf
    RBufWriteStream wStrm;
    ( void )wStrm.Open( *buf );
    wStrm.PushL();
    container->ExternalizeL( wStrm );
    wStrm.CommitL();
    wStrm.Close();
    wStrm.Pop();

    TPtr8 ptr( buf->Ptr( 0 ) );

    TInt retval = prop.Set( KTFAccessoryTestProperty,
        KTFAccessoryObjectMethod,
        ptr );

    CleanupStack::PopAndDestroy( buf );
    CleanupStack::PopAndDestroy( container );
    }

//
// ----------------------------------------------------------------------------------
// CTFAccessoryTestControl::CreateObjectPubSub
// ----------------------------------------------------------------------------------
void CTFAccessoryTestControl::CreateTopologyObjectsToPubSubL(
    TTFAccessoryTestCaseStateControl& aParameter )
    {
    RProperty prop;
    prop.Define( KTFAccessoryTestProperty,
        KTFAccessoryObjectMethod,
        RProperty::EByteArray );

    CAccPolAudioTopologyObjectCon* aObject =
        CAccPolAudioTopologyObjectCon::NewLC();

    //Speaker
    //OT-3
    CAccPolOutputTerminal* OT3 = CAccPolOutputTerminal::NewLC( 3,
        CAccPolOutputTerminal::EAPTHeadphone,
        2 );
    //Connected to FU-2                                                                  
    aObject->AddL( *OT3 );

    //FU2
    CAccPolFeatureUnit* FU2 = CAccPolFeatureUnit::NewLC( 2, 8 );
    // Volume control object.
    CAccPolVolumeControl* speakerRightVolume =
        CAccPolVolumeControl::NewLC( KAccRightFrontChannel, 7, 5, 20, 5 );
    CAccPolVolumeControl* speakerLeftVolume =
        CAccPolVolumeControl::NewLC( KAccLeftFrontChannel, 7, 5, 20, 5 );

    CAccPolMuteControl* speakermute =
        CAccPolMuteControl::NewLC( KAccMasterChannel, ETrue );

    FU2->AddControlL( *speakerRightVolume );
    FU2->AddControlL( *speakerLeftVolume );
    FU2->AddControlL( *speakermute );

    //Connected to MU-8
    aObject->AddL( *FU2 );

    CAccPolMixerUnit* MU8 = CAccPolMixerUnit::NewLC( 8 );
    MU8->AddSourceIdL( 1 );
    MU8->AddSourceIdL( 7 );

    //Connected to IT1 and FU7
    aObject->AddL( *MU8 );

    //Choice one
    //IT-1
    TUint8 aChannelCount = 2;
    TUint16 aChannelConfig = 0;
    aChannelConfig = aChannelConfig | KAccRightFrontChannel;
    aChannelConfig = aChannelConfig | KAccLeftFrontChannel;

    CAccPolInputTerminal* IT1 = CAccPolInputTerminal::NewLC( 1,
        CAccPolInputTerminal::EAPTStream,
        aChannelCount,
        aChannelConfig );
    aObject->AddL( *IT1 );

    //choice two
    //FU7
    CAccPolFeatureUnit* FU7 = CAccPolFeatureUnit::NewLC( 7, 5 ); //Connected to FU5
    // Volume control object.
    CAccPolVolumeControl* sidetoneRightVolume =
        CAccPolVolumeControl::NewLC( KAccMasterChannel, 7, 5, 20, 5 );
    CAccPolVolumeControl* sidetoneLeftVolume =
        CAccPolVolumeControl::NewLC( KAccMasterChannel, 7, 5, 20, 5 );

    FU7->AddControlL( *sidetoneRightVolume );
    FU7->AddControlL( *sidetoneLeftVolume );

    aObject->AddL( *FU7 );

    //Microphone
    //IT-4

    //OT-6
    CAccPolOutputTerminal* OT6 = CAccPolOutputTerminal::NewLC( 6,
        CAccPolOutputTerminal::EAPTStream,
        10 );
    //Connected to SU-10                                                           
    aObject->AddL( *OT6 );

    //SU-10
    CAccPolSelectorUnit* SU10 = CAccPolSelectorUnit::NewLC( 10, 1 );

    // Connected to FU 5                                                       
    SU10->AddSourceIdL( 5 );
    aObject->AddL( *SU10 );

    //FU5
    CAccPolFeatureUnit* FU5 = CAccPolFeatureUnit::NewLC( 5, 9 );

    // Volume control object.
    CAccPolVolumeControl* micRightVolume =
        CAccPolVolumeControl::NewLC( KAccRightFrontChannel, 7, 5, 20, 5 );
    CAccPolVolumeControl* micLeftVolume =
        CAccPolVolumeControl::NewLC( KAccLeftFrontChannel, 7, 5, 20, 5 );
    CAccPolMuteControl* micmute = CAccPolMuteControl::NewLC( KAccMasterChannel,
        ETrue );

    FU5->AddControlL( *micRightVolume );
    FU5->AddControlL( *micLeftVolume );
    FU5->AddControlL( *micmute );

    //Connected to FU-9                                                               
    aObject->AddL( *FU5 );

    ///FU9
    CAccPolFeatureUnit* FU9 = CAccPolFeatureUnit::NewLC( 9, 4 );
    //Connected IT-4                                                               
    aObject->AddL( *FU9 );

    CAccPolInputTerminal* IT4 = CAccPolInputTerminal::NewLC( 4,
        CAccPolInputTerminal::EAPTMicrophone,
        aChannelCount,
        aChannelConfig );
    aObject->AddL( *IT4 );

    CBufFlat* buf = CBufFlat::NewL( KAccSrvObjectBaseStreamBufGranularity );
    CleanupStack::PushL( buf );
    buf->ResizeL( KAccSrvObjectBaseStreamBufMaxSize );

    //Externalize container to buf
    RBufWriteStream wStrm;
    ( void )wStrm.Open( *buf );
    wStrm.PushL();
    aObject->ExternalizeL( wStrm );
    wStrm.CommitL();
    wStrm.Close();
    wStrm.Pop();
    TPtr8 ptr( buf->Ptr( 0 ) );
    TInt retval = prop.Set( KTFAccessoryTestProperty,
        KTFAccessoryObjectMethod,
        ptr );
    
    // Notify ASY stub
    if( aParameter.iArg2 )
        {
        PublishAndSubscribeL( aParameter );
        }

    CleanupStack::PopAndDestroy( buf );
    CleanupStack::PopAndDestroy( IT4 );
    CleanupStack::PopAndDestroy( FU9 );
    CleanupStack::PopAndDestroy( micmute );
    CleanupStack::PopAndDestroy( micLeftVolume );
    CleanupStack::PopAndDestroy( micRightVolume );
    CleanupStack::PopAndDestroy( FU5 );
    CleanupStack::PopAndDestroy( SU10 );
    CleanupStack::PopAndDestroy( OT6 );
    CleanupStack::PopAndDestroy( sidetoneLeftVolume );
    CleanupStack::PopAndDestroy( sidetoneRightVolume );
    CleanupStack::PopAndDestroy( FU7 );
    CleanupStack::PopAndDestroy( IT1 );
    CleanupStack::PopAndDestroy( MU8 );
    CleanupStack::PopAndDestroy( speakermute );
    CleanupStack::PopAndDestroy( speakerLeftVolume );
    CleanupStack::PopAndDestroy( speakerRightVolume );
    CleanupStack::PopAndDestroy( FU2 );
    CleanupStack::PopAndDestroy( OT3 );
    CleanupStack::PopAndDestroy( aObject );
    }

//
// ----------------------------------------------------------------------------------
// CTFAccessoryTestControl::CallAccessoryFunctionL
// ----------------------------------------------------------------------------------	
void CTFAccessoryTestControl::CallAccessoryFunctionL( TTFAccessoryTestCaseStateControl& aParameter )
    {

    //    COMPONENT_TRACE( ( _L( "ACCESSORYTESTCONTROL - CTFAccessoryTestControl::CallAccessoryFunctionL(%d, %d, %d)" ), aParameter.iAccessoryFunction, aParameter.iArg1, aParameter.iArg2 ) );
    if( !iInitialized )
        {
        InitializeL();
        }

    TInt retval = KTFErrAccessoryServiceMissing;

    switch( aParameter.iAccessoryFunction )
        {
        case ETFAccessorySet_ObjectPubSub:
            {
            CreateObjectPubSubL( ( TStreamConObjectType )aParameter.iArg1,
                aParameter.iArg2,
                ( TStreamConObjectType )aParameter.iArg3,
                aParameter.iArg4 );
            retval = KErrNone;
            }
            break;

        case ETFAccessorySet_TopologyObjectsPubSub:
            {
            CreateTopologyObjectsToPubSubL( aParameter );
            retval = KErrNone;
            }
            break;

        case ETFAccessorySet_PubSub:
            {
            RProperty prop;
            prop.Define( KTFAccessoryTestProperty,
                KTFAccessoryNotifierMethod,
                RProperty::EInt );
            retval = prop.Set( KTFAccessoryTestProperty,
                KTFAccessoryNotifierMethod,
                aParameter.iArg1 );
            }
            break;

        case ETFConnectWiredAccessory:
            {
            PublishAndSubscribeL( aParameter );
            retval = KErrNone;
            }
            break;

        case ETFClearStack:
            {
            CTFTestControlObserver* observerItem;
            TInt count = 0;
            if( !iStack.IsEmpty() )
                {
                iStackIter.SetToFirst();

                while( ( observerItem = iStackIter++ ) != NULL )
                    {
                    observerItem->Cancel();
                    iStack.Remove( *observerItem );
                    delete observerItem;
                    count++;
                    };
                }

            TEST_CASE_TRACE( ( _L( "CTFAccessoryTestCaseControl::ETF_Pappa - count=%d " ), count ) );
            retval = KErrNone;
            }
            break;

        case ETFDisconnectWiredAccessory:
            {
            retval = KErrNone;
            PublishAndSubscribeL( aParameter );
            TAccPolGenericID emptyInstance;
            iGenericId[aParameter.iGid] = emptyInstance;
            }
            break;

        case ETFCheckCapabilityGroup:
            {
            if( aParameter.iArg2
                == iGenericId[aParameter.iGid].SubblockCaps( aParameter.iArg3 ) )
                {
                retval = KErrNone;
                }
            else
                {
                retval = KErrNotFound;
                }
            }
            break;

        case ETFCheckCapability:
            {
            RAccessoryConnection accessoryConnectionn;
            accessoryConnectionn.CreateSubSession( iAccessoryServer );

            CAccPolSubblockNameArray* nameArrayy =
                CAccPolSubblockNameArray::NewL();
            CleanupStack::PushL( nameArrayy );
            accessoryConnectionn.GetSubblockNameArrayL( iGenericId[aParameter.iGid],
                *nameArrayy );

            if( aParameter.iArg2 == nameArrayy->HasName( aParameter.iArg3 ) )
                {
                retval = KErrNone;
                }
            else
                {
                retval = KErrNotFound;
                }

            CleanupStack::PopAndDestroy( nameArrayy );
            accessoryConnectionn.CloseSubSession();
            }
            break;

        case ETFFindAndCheckRequest:

            {
            FindAndCheckRequestL(&retval, aParameter);
		}
            break;

        case ETFFindAndCheckObjectRequest:
            {
            CTFAccessoryTestCaseControl* testCase = STATIC_CAST( CTFAccessoryTestCaseControl*, CurrentTestCase());
            TRACE_ASSERT( testCase != NULL );
            if( testCase != NULL )
                {
                if( !iStack.IsEmpty() )
                    {
                    iStackIter.SetToFirst();

                    while( ( iObserverItem = iStackIter++ ) != NULL )
                        {
                        if( iObserverItem->FindRequest( aParameter.iRequestType ) )
                            {
                            testCase->CheckObjectRequest( iObserverItem->iStatus,
                                iObserverItem->iObjectValue,
                                iObserverItem->iCapabilityName );

                            if( !iStack.IsEmpty() ) // Check if observes are already deleted by CheckRequest()
                                {
                                if( KTFDontDeleteObserver != aParameter.iArg1 )
                                    {
                                    iStack.Remove( *iObserverItem );
                                    delete iObserverItem;
                                    return;
                                    }
                                else
                                    {
                                    User::Leave( KTFDontDeleteObserver );
                                    }
                                }
                            else
                                {
                                retval = KErrGeneral;
                                }
                            }
                        }
                    }
                else
                    {
                    retval = KErrGeneral;
                    }
                }

            retval = KErrNone;
            }
            break;

            //
            // Accessory Connection
            //                 
        case ETFAccessoryConnection_Open:
            {
                AccessoryConnectionOpenClose(ETFAccessoryConnection_Open, &retval);            
            }
            break;

        case ETFAccessoryConnection_Close:

            {
                AccessoryConnectionOpenClose(ETFAccessoryConnection_Close, &retval);
        	}        	
            break;

            //
            // Accessory Settings
            //                                           		
        case ETFAccessorySettings_Open:
            {
                AccessorySettingsOpenClose(ETFAccessorySettings_Open, &retval);        	
        	}
            break;

        case ETFAccessorySettings_Close:
            {
        	AccessorySettingsOpenClose(ETFAccessorySettings_Close, &retval);
        	}
            break;

        case ETFSetHWDeviceSettings:
            {
            retval
                = iAccessorySettings->SetHWDeviceSettingsL( aParameter.iArg1,
                    ( TBool )aParameter.iArg2 );
            }
            break;

        case ETFGetHWDeviceSettings:
            {
            TUint32 aSettingsValue( 0 );
            retval = iAccessorySettings->GetHWDeviceSettings( aSettingsValue );

            if( aSettingsValue != aParameter.iArg1 )
                {
                retval = KErrArgument;
                }
            }
            break;

        case ETFGetSupportedHWDeviceSettings:
            {
            TUint32 aSettingsSupportedValue( 0 );

            retval
                = iAccessorySettings->GetSupportedHWDeviceSettings( aSettingsSupportedValue );

            if( aSettingsSupportedValue != ( aParameter.iArg1
                | aParameter.iArg2 ) )
                {
                retval = KErrArgument;
                }
            }
            break;

        case ETFSetIntAccessoryModeSetting:
            {
            TAccMode aAccMode = ( TAccMode )aParameter.iArg1;
            TAccModeSetting aSetting;

            //Set Lights
            aSetting.SetId( aParameter.iArg2 );
            aSetting.SetTInt( aParameter.iArg4 );

            retval = iAccessorySettings->SetAccessoryModeSettingL( aAccMode,
                aSetting );
            }
            break;

        case ETFSetBoolAccessoryModeSetting:
            {
            TAccMode aAccMode = ( TAccMode )aParameter.iArg1;
            TAccModeSetting aSetting;

            //Set Lights 
            aSetting.SetId( aParameter.iArg2 );
            aSetting.SetTBool( aParameter.iArg4 );

            retval = iAccessorySettings->SetAccessoryModeSettingL( aAccMode,
                aSetting );
            }
            break;

        case ETFSetDesAccessoryModeSetting:
            {
            TAccMode aAccMode = ( TAccMode )aParameter.iArg1;
            TAccModeSetting aSetting;

            TBuf<KMaxAccModeSetting> buf;
            buf.Num( aParameter.iArg3 );
            //	buf.Copy( *aParameter.iArg3 );

            aSetting.SetId( aParameter.iArg2 );
            aSetting.SetTDes( buf );
            retval = iAccessorySettings->SetAccessoryModeSettingL( aAccMode,
                aSetting );
            }
            break;

        case ETFGetIntAccessoryModeSetting:
            {
            TInt aSettingsValueInt;

            TAccMode aAccMode = ( TAccMode )aParameter.iArg1;
            TAccModeSetting aSetting;
            aSetting.SetId( aParameter.iArg2 );

            retval = iAccessorySettings->GetAccessoryModeSetting( aAccMode,
                aSetting );

            if( aSetting.Type() == EAccModeSettingInt )
                {
                retval = aSetting.GetTInt( aSettingsValueInt );

                if( retval != KErrNotFound )
                    {
                    if( aSettingsValueInt == aParameter.iArg4 )
                        {
                        retval = KErrNone;
                        }
                    else
                        {
                        retval = KErrArgument;
                        }
                    }
                }
            else
                {
                retval = KErrNotFound;
                }
            }
            break;

        case ETFGetBoolAccessoryModeSetting:
            {
            TBool aSettingsValueBool;

            TAccMode aAccMode = ( TAccMode )aParameter.iArg1;
            TAccModeSetting aSetting;
            aSetting.SetId( aParameter.iArg2 );

            retval = iAccessorySettings->GetAccessoryModeSetting( aAccMode,
                aSetting );

            if( aSetting.Type() == EAccModeSettingBool )
                {
                retval = aSetting.GetTBool( aSettingsValueBool );

                if( retval != KErrNotFound )
                    {
                    if( aSettingsValueBool == ( TBool )aParameter.iArg4 )
                        {
                        retval = KErrNone;
                        }
                    else
                        {
                        retval = KErrArgument;
                        }
                    }
                }
            else
                {
                retval = KErrNotFound;
                }
            }
            break;

        case ETFGetDesAccessoryModeSetting:
            {
            TBuf<KMaxAccModeSetting> aSettingsValueDes;

            TAccMode aAccMode = ( TAccMode )aParameter.iArg1;
            TAccModeSetting aSetting;
            aSetting.SetId( aParameter.iArg2 );

            retval = iAccessorySettings->GetAccessoryModeSetting( aAccMode,
                aSetting );

            if( aSetting.Type() == EAccModeSettingDes )
                {
                retval = aSetting.GetTDes( aSettingsValueDes );

                if( retval != KErrNotFound )
                    {
                    TBuf<KMaxAccModeSetting> buffer;
                    buffer.Num( aParameter.iArg3 );
                    if( KErrNone == aSettingsValueDes.Compare( buffer ) )
                        {
                        retval = KErrNone;
                        }
                    else
                        {
                        retval = KErrArgument;
                        }
                    }
                }
            else
                {
                retval = KErrNotFound;
                }
            }
            break;

        case ETFSetSeveralAccessoryModeSetting:
            {

            TAccModeSetting accModeSettingIn;
            TAccMode tAccMode = EAccModeWirelessHeadset;
            TBuf<KMaxAccModeSetting> aDesIn;
            // Set
            // setting 1
            accModeSettingIn.SetId( 11 ); // unknown TInt
            accModeSettingIn.SetTInt( 1 );
            TAccModeSettingArray arrayIn;
            arrayIn.AddSetting( accModeSettingIn );

            // setting 2
            accModeSettingIn.SetId( 12 ); // unknown Des
            _LIT( KAccSetting2, "012345678901234567890" );
            aDesIn.Copy( KAccSetting2 );
            accModeSettingIn.SetTDes( aDesIn );
            arrayIn.AddSetting( accModeSettingIn );

            // setting 3
            accModeSettingIn.SetId( 13 ); // unknown TBool
            accModeSettingIn.SetTBool( ETrue );
            arrayIn.AddSetting( accModeSettingIn );

            retval = iAccessorySettings->SetAccessoryModeSettingsL( tAccMode,
                arrayIn );

            }
            break;

        case ETFGetSeveralAccessoryModeSetting:
            {

            TAccModeSetting accModeSettingOut;
            TAccMode tAccMode = EAccModeWirelessHeadset;
            TBuf<KMaxAccModeSetting> aDesOut;
            TAccModeSettingArray arrayOut;
            TInt intOut( 0 );
            // set input parameters for GetAccessoryModeSettings()
            // setting 11
            accModeSettingOut.SetId( 11 );
            arrayOut.AddSetting( accModeSettingOut );

            // setting 12
            accModeSettingOut.SetId( 12 );
            arrayOut.AddSetting( accModeSettingOut );

            // setting 13
            accModeSettingOut.SetId( 13 );
            arrayOut.AddSetting( accModeSettingOut );

            retval = iAccessorySettings->GetAccessoryModeSettings( tAccMode,
                arrayOut );

            // Check that got settings are right

            TInt count = arrayOut.GetArraySize();
            TInt foundCount( 0 );

            for( TInt index = 0; index < count; index++ )
                {
                arrayOut.GetSetting( index, accModeSettingOut );

                if( 11 == accModeSettingOut.Id() )
                    {

                    accModeSettingOut.GetTInt( intOut );

                    if( 1 == intOut )
                        {
                        foundCount++;
                        }
                    }

                if( 12 == accModeSettingOut.Id() )
                    {
                    accModeSettingOut.GetTDes( aDesOut );

                    if( KErrNone == aDesOut.Compare( _L("012345678901234567890") ) )
                        {
                        foundCount++;
                        }

                    }

                if( 13 == accModeSettingOut.Id() )
                    {

                    accModeSettingOut.GetTBool( intOut );

                    if( intOut )
                        {
                        foundCount++;
                        }
                    }
                }

            if( foundCount != 3 )
                {
                retval = KErrNotFound;
                }

            //
            // Reset Settings
            //
            TAccModeSetting accModeSettingIn;
            TBuf<KMaxAccModeSetting> aDesIn;
            // Set
            // setting 1
            accModeSettingIn.SetId( 11 ); // unknown TInt
            accModeSettingIn.SetTInt( 0 );
            TAccModeSettingArray arrayIn;
            arrayIn.AddSetting( accModeSettingIn );

            // setting 2
            accModeSettingIn.SetId( 12 ); // unknown Des
            _LIT( KAccSetting2, "" );
            aDesIn.Copy( KAccSetting2 );
            accModeSettingIn.SetTDes( aDesIn );
            arrayIn.AddSetting( accModeSettingIn );

            // setting 3
            accModeSettingIn.SetId( 13 ); // unknown TBool
            accModeSettingIn.SetTBool( EFalse );
            arrayIn.AddSetting( accModeSettingIn );

            iAccessorySettings->SetAccessoryModeSettingsL( tAccMode, arrayIn );
            }
            break;

            //
            // Accessory Single Connection
            //                    		
        case ETFAccessorySingleConnection_Open:
            {
            if( iAccessorySingleConnection == NULL )
                {
                iAccessorySingleConnection
                    = new ( ELeave ) RAccessorySingleConnection;
                retval
                    = iAccessorySingleConnection->CreateSubSession( iAccessoryServer );
                }
            else
                {
                retval = KErrNone;
                }
            }
            break;

        case ETFAccessorySingleConnection_Close:
            {
            TRACE_ASSERT( iAccessorySingleConnection != NULL );

            if( iAccessorySingleConnection != NULL )
                {
                iAccessorySingleConnection->CloseSubSession();
                delete iAccessorySingleConnection;
                iAccessorySingleConnection = NULL;
                }
            retval = KErrNone;
            }
            break;

        case ETFAccessoryNotifyNewAccessoryConnected:
            {
            CTFTestControlObserver* aConnectionObserver = CreateObserver();
            TRACE_ASSERT( aConnectionObserver != NULL );
            aConnectionObserver->NotifyNewAccessoryConnected( iGenericId[aParameter.iGid] );
            retval = KErrNone;
            }
            break;

        case ETFAccessoryNotifyAccessoryDisconnected:
            {
            CTFTestControlObserver* aConnectionObserver = CreateObserver();
            TRACE_ASSERT( aConnectionObserver != NULL );
            aConnectionObserver->NotifyAccessoryDisconnected( iGenericId[aParameter.iGid] );
            retval = KErrNone;
            }
            break;

        case ETFNotifyIntAccessoryValueChanged:
            {
            CTFTestControlObserver* aConnectionObserver = CreateObserver();
            TRACE_ASSERT( aConnectionObserver != NULL );
            aConnectionObserver->NotifyIntegerAccessoryValueChanged( iGenericId[aParameter.iGid],
                aParameter.iArg3 );
            retval = KErrNone;
            }
            break;

        case ETFNotifyBoolAccessoryValueChanged:
            {
            CTFTestControlObserver* aConnectionObserver = CreateObserver();
            TRACE_ASSERT( aConnectionObserver != NULL );
            aConnectionObserver->NotifyBooleanAccessoryValueChanged( iGenericId[aParameter.iGid],
                aParameter.iArg3 );
            retval = KErrNone;
            }
            break;

        case ETFNotifyObjectAccessoryValueChanged:
            {
            CTFTestControlObserver* aConnectionObserver = CreateObserver();
            TRACE_ASSERT( aConnectionObserver != NULL );
            aConnectionObserver->NotifyObjectAccessoryValueChanged( iGenericId[aParameter.iGid],
                aParameter.iArg3 );
            retval = KErrNone;
            }
            break;

        case ETFAccessoryIntGetValue:
            {
            TAccPolNameRecord aNameRecord;
            aNameRecord.SetNameL( aParameter.iArg3 );
            TAccValueTypeTInt aIntValue;

            iAccessorySingleConnection->GetValueL( iGenericId[aParameter.iGid],
                aNameRecord,
                aIntValue );

            if( aIntValue.iValue == aParameter.iArg2 )
                {
                retval = KErrNone;
                }
            else
                {
                retval = KErrArgument;
                }
            }
            break;

        case ETFAccessoryBoolGetValue:
            {
            RAccessoryConnection accessoryConnection;
            accessoryConnection.CreateSubSession( iAccessoryServer );

            TAccPolNameRecord nameRecord;
            nameRecord.SetNameL( aParameter.iArg3 );
            TAccValueTypeTBool boolValue;
            retval
                = accessoryConnection.GetValueL( iGenericId[aParameter.iGid],
                    nameRecord,
                    boolValue );

            if( boolValue.iValue != aParameter.iArg1 )
                {
                retval = KErrArgument;
                }

            accessoryConnection.CloseSubSession();
            }
            break;

        case ETFAccessoryDes8GetValue:
            {
            TBuf8<80>
                worm( _L8("Malicious Worm Attach with extra long data with extra long content" ));
            TBuf8<85> valueBuf;
            TAccPolNameRecord aNameRecord;
            aNameRecord.SetNameL( aParameter.iArg1 );
            TRAPD( err, iAccessorySingleConnection->GetValueL( iGenericId[aParameter.iGid], aNameRecord, valueBuf ) );

            if( err == aParameter.iExpectedResult )
                {

                if( aParameter.iExpectedResult == KErrArgument )
                    {
                    retval = KErrNone;
                    }
                else
                    {
                    if( KErrNone == valueBuf.Compare( worm ) )
                        {
                        RDebug::RawPrint( valueBuf );
                        retval = KErrNone;
                        }
                    else
                        {
                        retval = KErrArgument;
                        }
                    }
                }
            else
                {
                retval = KErrArgument;
                }
            }
            break;

        case ETFAccessoryObjectParseTopology:
            {

            TInt rspeaker( KErrGeneral );
            TInt lspeaker( KErrGeneral );
            TInt rmic( KErrGeneral );
            TInt lmic( KErrGeneral );
            TInt mute( KErrGeneral );
            TInt sidetone( KErrGeneral );
            //
            // Find speaker volume object in USB headset case.
            //                                      
            CAccPolAudioUnitBase* audioUnit = NULL;
            CAccPolAudioControlBase* audioControl = NULL;
            //CAccPolOutputTerminal* ouputTerminal = NULL;	          	          

            if( iTopologyContainer->OutputTerminalL( CAccPolOutputTerminal::EAPTHeadphone,
                audioUnit ) )
                {
                if( audioUnit->GetNextAudioUnitL( audioUnit ) )
                    {
                    if( audioUnit->ObjectType()
                        == EAccPolAudioFeatureUnitObject )
                        {
                        if( audioUnit->GetAudioControlL( EAccPolAudioVolumeControlObject,
                            KAccRightFrontChannel,
                            audioControl ) )
                            {
                            iSpeakerRightVolume = CAccPolVolumeControl::NewL();
                            *iSpeakerRightVolume
                                = *reinterpret_cast<CAccPolVolumeControl*> ( audioControl );
                            rspeaker = KErrNone;
                            }
                        if( audioUnit->GetAudioControlL( EAccPolAudioVolumeControlObject,
                            KAccLeftFrontChannel,
                            audioControl ) )
                            {
                            iSpeakerLeftVolume = CAccPolVolumeControl::NewL();
                            *iSpeakerLeftVolume
                                = *reinterpret_cast<CAccPolVolumeControl*> ( audioControl );
                            lspeaker = KErrNone;
                            }
                        if( audioUnit->GetAudioControlL( EAccPolAudioMuteControlObject,
                            KAccMasterChannel,
                            audioControl ) )
                            {
                            iSpeakerMute = CAccPolMuteControl::NewL();
                            *iSpeakerMute
                                = *reinterpret_cast<CAccPolMuteControl*> ( audioControl );
                            mute = KErrNone;
                            }

                        }
                    }
                }

            //
            // Find mic volume object in USB headset case.
            //                                                                                           
            if( iTopologyContainer->OutputTerminalL( CAccPolOutputTerminal::EAPTStream,
                audioUnit ) )
                {
                do
                    {
                    }
                while( audioUnit->GetNextAudioUnitL( audioUnit )
                    && audioUnit->ObjectType() != EAccPolAudioFeatureUnitObject );

                RPointerArray<CAccPolAudioControlBase> aAudioControls;
                audioUnit->GetAudioControlsL( KAccRightFrontChannel,
                    aAudioControls );

                if( aAudioControls.Count() > 0 )
                    {
                    iMicRightVolume = CAccPolVolumeControl::NewL();
                    *iMicRightVolume
                        = *reinterpret_cast<CAccPolVolumeControl*> ( aAudioControls.operator[]( 0 ) );
                    rmic = KErrNone;
                    }
                aAudioControls.Reset();

                audioUnit->GetAudioControlsL( KAccLeftFrontChannel,
                    aAudioControls );

                if( aAudioControls.Count() > 0 )
                    {
                    iMicLeftVolume = CAccPolVolumeControl::NewL();
                    *iMicLeftVolume
                        = *reinterpret_cast<CAccPolVolumeControl*> ( aAudioControls.operator[]( 0 ) );
                    lmic = KErrNone;
                    }

                //
                // Find sidetone volume object in USB headset case.
                //                                                               
                iTopologyContainer->OutputTerminalL( CAccPolOutputTerminal::EAPTHeadphone,
                    audioUnit );

                do
                    {
                    audioUnit->GetNextAudioUnitL( audioUnit );
                    }
                while( audioUnit->ObjectType() != EAccPolAudioMixerUnitObject );

                if( audioUnit->SourceIdCount() > 1 )
                    {
                    RPointerArray<CAccPolAudioUnitBase> aAudioUnits;
                    audioUnit->GetAudioUnitListL( aAudioUnits );

                    for( TInt i = 0; i < aAudioUnits.Count(); i++ )
                        {
                        if( aAudioUnits.operator[]( i )->ObjectType()
                            == EAccPolAudioFeatureUnitObject )
                            {
                            audioUnit = aAudioUnits.operator[]( i );
                            break;
                            }
                        }
                    }

                aAudioControls.Reset();
                audioUnit->GetAudioControlsL( aAudioControls );
                if( aAudioControls.Count() == 2 )
                    {
                    sidetone = KErrNone;
                    }
                }

            retval = ( rspeaker | lspeaker | rmic | lmic | mute | sidetone );

            }
            break;

        case ETFAccessoryGetStreamObjects:
            {
            if( iStreamContainer != NULL )
                {
                delete iStreamContainer;
                iStreamContainer = NULL;
                }
            iStreamContainer = CAccPolAudioStreamFormatCon::NewL();

            TAccPolNameRecord aNameRecord;
            aNameRecord.SetNameL( aParameter.iArg3 );
            TRAPD( err, iAccessorySingleConnection->GetValueL( iGenericId[aParameter.iGid], aNameRecord, *iStreamContainer ) );

            if( err == aParameter.iExpectedResult )
                {
                //externalize container to bufFromASY        
                CBufFlat* bufFromASY =
                    CBufFlat::NewL( KAccSrvObjectBaseStreamBufGranularity );
                CleanupStack::PushL( bufFromASY );
                bufFromASY->ResizeL( KAccSrvObjectBaseStreamBufMaxSize );
                RBufWriteStream wStrm;
                ( void )wStrm.Open( *bufFromASY );
                wStrm.PushL();
                iStreamContainer->ExternalizeL( wStrm );
                wStrm.CommitL();
                wStrm.Close();
                wStrm.Pop();
                TPtr8 ptrbufFromASY( bufFromASY->Ptr( 0 ) );

                //Read container from PubSub
                RProperty prop;
                RBufReadStream rStrm;
                CBufFlat* bufFromPubSub =
                    CBufFlat::NewL( KAccSrvObjectBaseStreamBufGranularity );
                CleanupStack::PushL( bufFromPubSub );
                bufFromPubSub->ResizeL( KAccSrvObjectBaseStreamBufMaxSize );
                TPtr8 ptrBufFromPubSub( bufFromPubSub->Ptr( 0 ) );
                TInt ret = prop.Get( KTFAccessoryTestProperty,
                    KTFAccessoryObjectMethod,
                    ptrBufFromPubSub );

                TInt k = sizeof( bufFromASY );
                TInt kk = sizeof( bufFromPubSub );

                bufFromASY->Compress();
                bufFromPubSub->Compress();

                k = sizeof( bufFromASY );
                kk = sizeof( bufFromPubSub );

                retval
                    = ( 0
                        == memcompare( reinterpret_cast<TUint8*> ( &ptrbufFromASY ),
                            sizeof( bufFromASY ),
                            reinterpret_cast<TUint8*> ( &ptrBufFromPubSub ),
                            sizeof( bufFromASY ) ) ) ? KErrNone : KErrGeneral;

                CleanupStack::PopAndDestroy( bufFromPubSub );
                CleanupStack::PopAndDestroy( bufFromASY );
                }
            }
            break;

        case ETFAccessoryGetStreamObjectsFromPolicy:
            {
            if( iStreamContainer != NULL )
                {
                delete iStreamContainer;
                iStreamContainer = NULL;
                }
            iStreamContainer = CAccPolAudioStreamFormatCon::NewL();

            TAccPolNameRecord aNameRecord;
            aNameRecord.SetNameL( aParameter.iArg3 );
            TRAPD( err, iAccessorySingleConnection->GetValueL( iGenericId[aParameter.iGid], aNameRecord, *iStreamContainer ) );

            RPointerArray<CAccPolAudioStreamFormat> inputStreamObjects;
            RPointerArray<CAccPolAudioStreamFormat> outputStreamObjects;

            iStreamContainer->GetInputStreamFormatsL( inputStreamObjects );
            iStreamContainer->GetOutputStreamFormatsL( outputStreamObjects );

            CAccPolAudioStreamFormat* inputStream =
                inputStreamObjects.operator[]( 0 );
            CAccPolAudioStreamFormat* ouputStream =
                outputStreamObjects.operator[]( 0 );

            retval = err;
            }
            break;

        case ETFAccessorySetStreamObjects:
            {
            CAccPolAudioStreamFormatCon* setContainer = CAccPolAudioStreamFormatCon::NewLC();

            RPointerArray<CAccPolAudioStreamFormat> inputStreamObjects;
            RPointerArray<CAccPolAudioStreamFormat> outputStreamObjects;

            iStreamContainer->GetInputStreamFormatsL( inputStreamObjects );
            CAccPolAudioStreamFormat* inputStream = NULL;
            for(TInt i=0; i<inputStreamObjects.Count(); i++)
                {
                inputStream = inputStreamObjects.operator[]( i );
                if( inputStream->AudioFormat() == KUidFormatPCM16 &&
                    inputStream->UnitId() == 0 &&
                    inputStream->StreamType() == CAccPolAudioStreamFormat::EStereo &&
                    inputStream->BitResolution() == CAccPolAudioStreamFormat::EBitsPerSample16 &&
                    inputStream->SamFreq() == 200 )
                    {
                    setContainer->AddL( *inputStream );
                    break;
                    }
                }

            iStreamContainer->GetOutputStreamFormatsL( outputStreamObjects );
            CAccPolAudioStreamFormat* outputStream = NULL;
            for(TInt i=0; i<outputStreamObjects.Count(); i++)
                {
                outputStream = outputStreamObjects.operator[]( i );
                if( outputStream->AudioFormat() == KUidFormatPCM16 &&
                    outputStream->UnitId() == 0 &&
                    outputStream->StreamType() == CAccPolAudioStreamFormat::EMono &&
                    outputStream->BitResolution() == CAccPolAudioStreamFormat::EBitsPerSample32 &&
                    outputStream->SamFreq() == 200 )
                    {
                    setContainer->AddL( *outputStream );
                    break;
                    }
                }

            RBufWriteStream wStrm;
            CBufFlat* buf = CBufFlat::NewL( KAccSrvObjectBaseStreamBufGranularity );
            CleanupStack::PushL( buf );
            buf->ResizeL( KAccSrvObjectBaseStreamBufMaxSize );
            (void)wStrm.Open( *buf );
            wStrm.PushL();
            setContainer->ExternalizeL( wStrm );
            wStrm.Close();
            wStrm.Pop();
            TPtr8 ptr( buf->Ptr( 0 ) );

            retval = RProperty::Set( KTFAccessoryTestProperty, KTFAccessoryObjectMethod, ptr );

            TAccPolNameRecord aNameRecord;
            aNameRecord.SetNameL( aParameter.iArg3 );

            TRAPD( setErr, iAccessoryControl->SetValueL( iGenericId[aParameter.iGid], aNameRecord, *setContainer ) );

            retval = setErr;

            CleanupStack::PopAndDestroy( buf );
            CleanupStack::PopAndDestroy( setContainer );
            inputStreamObjects.Close();
            outputStreamObjects.Close();
            }
        break;

        case ETFAccessoryGetTopologyObjects:
            {
            if( iTopologyContainer != NULL )
                {
                delete iTopologyContainer;
                iTopologyContainer = NULL;
                }

            iTopologyContainer = CAccPolAudioTopologyObjectCon::NewL();

            TAccPolNameRecord aNameRecord;
            aNameRecord.SetNameL( aParameter.iArg3 );
            TRAPD( err, iAccessorySingleConnection->GetValueL( iGenericId[aParameter.iGid], aNameRecord, *iTopologyContainer ) );

            if( err == aParameter.iExpectedResult )
                {
                //externalize container to bufFromASY        
                CBufFlat* bufFromASY =
                    CBufFlat::NewL( KAccSrvObjectBaseStreamBufGranularity );
                CleanupStack::PushL( bufFromASY );
                bufFromASY->ResizeL( KAccSrvObjectBaseStreamBufMaxSize );
                RBufWriteStream wStrm;
                ( void )wStrm.Open( *bufFromASY );
                wStrm.PushL();
                iTopologyContainer->ExternalizeL( wStrm );
                wStrm.CommitL();
                wStrm.Close();
                wStrm.Pop();
                TPtr8 ptrbufFromASY( bufFromASY->Ptr( 0 ) );

                //Read container from PubSub
                RProperty prop;
                RBufReadStream rStrm;
                CBufFlat* bufFromPubSub =
                    CBufFlat::NewL( KAccSrvObjectBaseStreamBufGranularity );
                CleanupStack::PushL( bufFromPubSub );
                bufFromPubSub->ResizeL( KAccSrvObjectBaseStreamBufMaxSize );
                TPtr8 ptrBufFromPubSub( bufFromPubSub->Ptr( 0 ) );
                TInt ret = prop.Get( KTFAccessoryTestProperty,
                    KTFAccessoryObjectMethod,
                    ptrBufFromPubSub );

                TInt k = sizeof( bufFromASY );
                TInt kk = sizeof( bufFromPubSub );

                bufFromASY->Compress();
                bufFromPubSub->Compress();

                k = sizeof( bufFromASY );
                kk = sizeof( bufFromPubSub );

                retval
                    = ( 0
                        == memcompare( reinterpret_cast<TUint8*> ( &ptrbufFromASY ),
                            sizeof( bufFromASY ),
                            reinterpret_cast<TUint8*> ( &ptrBufFromPubSub ),
                            sizeof( bufFromASY ) ) ) ? KErrNone : KErrGeneral;

                CleanupStack::PopAndDestroy( bufFromPubSub );
                CleanupStack::PopAndDestroy( bufFromASY );
                }
            }
            break;

        case ETFAccessoryGetVolumeControlObjects:
            {

            if( iTopologyContainer != NULL )
                {
                delete iTopologyContainer;
                iTopologyContainer = NULL;
                }

            iTopologyContainer = CAccPolAudioTopologyObjectCon::NewL();

            iTopologyContainer->AddL( *iSpeakerRightVolume );
            iTopologyContainer->AddL( *iSpeakerLeftVolume );

            TAccPolNameRecord aNameRecord;
            aNameRecord.SetNameL( aParameter.iArg3 );
            TRAPD( err, iAccessorySingleConnection->GetValueL( iGenericId[aParameter.iGid], aNameRecord, *iTopologyContainer ) );

            CAccPolVolumeControl* rightVolume;
            rightVolume = ( CAccPolVolumeControl* )iTopologyContainer->AtL( 0 );

            CAccPolVolumeControl* leftVolume;
            leftVolume = ( CAccPolVolumeControl* )iTopologyContainer->AtL( 1 );

            if( err == aParameter.iExpectedResult )
                {
                retval = KErrNone;
                }
            }
            break;

        case ETFAccessorySetVolumeControlObjects:
            {
            RBufWriteStream wStrm;
            CAccPolAudioTopologyObjectCon* setContainer =
                CAccPolAudioTopologyObjectCon::NewLC();

            iSpeakerRightVolume->SetVolumeDb( aParameter.iArg1 );
            iSpeakerRightVolume->SetMinVolumeDb( aParameter.iArg2 );
            iSpeakerRightVolume->SetMaxVolumeDb( aParameter.iArg4 );
            iSpeakerRightVolume->SetVolumeResDb( 0x05 );
            setContainer->AddL( *iSpeakerRightVolume );
            iSpeakerLeftVolume->SetVolumeDb( aParameter.iArg1 );
            iSpeakerLeftVolume->SetMinVolumeDb( aParameter.iArg2 );
            iSpeakerLeftVolume->SetMaxVolumeDb( aParameter.iArg4 );
            iSpeakerLeftVolume->SetVolumeResDb( 0x05 );
            setContainer->AddL( *iSpeakerLeftVolume );

            CBufFlat* buf =
                CBufFlat::NewL( KAccSrvObjectBaseStreamBufGranularity );
            CleanupStack::PushL( buf );
            buf->ResizeL( KAccSrvObjectBaseStreamBufMaxSize );
            ( void )wStrm.Open( *buf );
            wStrm.PushL();
            setContainer->ExternalizeL( wStrm );
            wStrm.Close();
            wStrm.Pop();
            TPtr8 ptr( buf->Ptr( 0 ) );

            retval = RProperty::Set( KTFAccessoryTestProperty,
                KTFAccessoryObjectMethod,
                ptr );

            TAccPolNameRecord aNameRecord;
            aNameRecord.SetNameL( aParameter.iArg3 );
            TRAPD( setErr, iAccessoryControl->SetValueL( iGenericId[aParameter.iGid], aNameRecord, *setContainer ) );

            retval = setErr;

            CleanupStack::PopAndDestroy( buf );
            CleanupStack::PopAndDestroy( setContainer );
            }
            break;

        case ETFAccessoryGetMuteControlObjects:
            {

            if( iTopologyContainer != NULL )
                {
                delete iTopologyContainer;
                iTopologyContainer = NULL;
                }

            iTopologyContainer = CAccPolAudioTopologyObjectCon::NewL();

            iTopologyContainer->AddL( *iSpeakerMute );

            TAccPolNameRecord aNameRecord;
            aNameRecord.SetNameL( aParameter.iArg3 );
            TRAPD( err, iAccessorySingleConnection->GetValueL( iGenericId[aParameter.iGid], aNameRecord, *iTopologyContainer ) );

            CAccPolMuteControl* mute;
            mute = ( CAccPolMuteControl* )iTopologyContainer->AtL( 0 );

            if( err == aParameter.iExpectedResult )
                {
                retval = KErrNone;
                }
            }
            break;

        case ETFAccessorySetMuteControlObjects:
            {
            RBufWriteStream wStrm;
            CAccPolAudioTopologyObjectCon* setContainer =
                CAccPolAudioTopologyObjectCon::NewLC();

            iSpeakerMute->SetMute( aParameter.iArg1 );
            setContainer->AddL( *iSpeakerMute );

            CBufFlat* buf =
                CBufFlat::NewL( KAccSrvObjectBaseStreamBufGranularity );
            CleanupStack::PushL( buf );
            buf->ResizeL( KAccSrvObjectBaseStreamBufMaxSize );
            ( void )wStrm.Open( *buf );
            wStrm.PushL();
            setContainer->ExternalizeL( wStrm );
            wStrm.Close();
            wStrm.Pop();
            TPtr8 ptr( buf->Ptr( 0 ) );

            retval = RProperty::Set( KTFAccessoryTestProperty,
                KTFAccessoryObjectMethod,
                ptr );

            TAccPolNameRecord aNameRecord;
            aNameRecord.SetNameL( aParameter.iArg3 );
            TRAPD( setErr, iAccessoryControl->SetValueL( iGenericId[aParameter.iGid], aNameRecord, *setContainer ) );

            retval = setErr;

            CleanupStack::PopAndDestroy( buf );
            CleanupStack::PopAndDestroy( setContainer );
            }
            break;

            //
            // Accessory Control
            //           

        case ETFAccessoryControl_Open:
            {
            if( iAccessoryControl == NULL )
                {
                iAccessoryControl = new ( ELeave ) RAccessoryControl;
                retval = iAccessoryControl->CreateSubSession( iAccessoryServer );
                }
            else
                {
                retval = KErrNone;
                }
            }
            break;

        case ETFAccessoryControl_Close:
            {
            TRACE_ASSERT( iAccessoryControl != NULL );

            if( iAccessoryControl != NULL )
                {
                iAccessoryControl->CloseSubSession();
                delete iAccessoryControl;
                iAccessoryControl = NULL;
                }
            retval = KErrNone;
            }
            break;

        case ETFAccessoryNotifyAccessoryConnectionStatusChanged:
            {
            CTFTestControlObserver* aConnectionObserver = CreateObserver();
            TRACE_ASSERT( aConnectionObserver != NULL );
            aConnectionObserver->NotifyAccessoryConnectionStatusChanged( iGenericIdArray );
            retval = KErrNone;
            }
            break;

        case ETFSyncGetAccessoryConnectionStatus:
            {
            iAccessoryConnection->GetAccessoryConnectionStatus( iGenericIdArray );

            if( iGenericIdArray.Count() != aParameter.iArg1 )
                {
                retval = KErrArgument;
                }
            else
                {
                retval = KErrNone;
                }
            }
            break;

        case ETFASyncGetAccessoryConnectionStatus:
            {
            CTFTestControlObserver* aConnectionObserver = CreateObserver();
            TRACE_ASSERT( aConnectionObserver != NULL );
            aConnectionObserver->GetAccessoryConnectionStatus( iGenericIdArray );
            retval = KErrNone;
            }
            break;

        case ETFSetIntValue:
       	   {
       	   SetIntValue(aParameter, &retval);
       	   }
            break;

        case ETFSetBoolValue:
            {
            TAccPolNameRecord aNameRecord;
            aNameRecord.SetNameL( aParameter.iArg3 );
            TAccValueTypeTBool aBoolValue;
            aBoolValue.iValue = aParameter.iArg1;
            retval = iAccessoryControl->SetValueL( iGenericId[aParameter.iGid],
                aNameRecord,
                aBoolValue );
            }
            break;

            //
            //Tuupaa
            //
        case ETFAccessoryValueChangedNotifyInt:
            {
            TAccPolNameRecord aNameRecord;
            aNameRecord.SetNameL( KAccAudioOutputType );
            TAccValueTypeTInt aIntValue;
            aIntValue.iValue = EAccAudioOutPutTypePublic;
            iAccessoryControl->AccessoryValueChangedNotifyL( iGenericId[aParameter.iGid],
                aNameRecord,
                aIntValue );
            retval = KErrNone;
            }
            break;

        case ETFAccessoryValueChangedNotifyBool:
            {
            TAccPolNameRecord aNameRecord;
            aNameRecord.SetNameL( aParameter.iArg3 );
            TAccValueTypeTBool aBoolValue;
            aBoolValue.iValue = ETrue;
            iAccessoryControl->AccessoryValueChangedNotifyL( iGenericId[aParameter.iGid],
                aNameRecord,
                aBoolValue );
            retval = KErrNone;
            }
            break;

            //
            // BT Control    
            //
        case ETFBTAccessoryValueChangedNotifyBool:
            {
            TBTDevAddr btaddr( aParameter.iArg1 );
            TAccPolNameRecord nameRecord;
            nameRecord.SetNameL( aParameter.iArg3 );
            TAccValueTypeTBool boolValue;
            boolValue.iValue = aParameter.iArg4;
            iAccessoryBTControl->AccessoryValueChangedNotifyL( btaddr,
                nameRecord,
                boolValue );
            retval = KErrNone;
            }
            break;

        case ETFBTAccessoryValueChangedNotifyInt:
            {
            TBTDevAddr btaddr( aParameter.iArg1 );
            TAccPolNameRecord nameRecord;
            nameRecord.SetNameL( aParameter.iArg3 );
            TAccValueTypeTInt intValue;
            intValue.iValue = aParameter.iArg4;
            iAccessoryBTControl->AccessoryValueChangedNotifyL( btaddr,
                nameRecord,
                intValue );
            retval = KErrNone;
            }
            break;

        case ETFAccessoryBtControl_Open:
            {
            if( iAccessoryBTControl == NULL )
                {
                iAccessoryBTControl = new ( ELeave ) RAccessoryBTControl;
                retval
                    = iAccessoryBTControl->CreateSubSession( iAccessoryServer );
                }
            else
                {
                retval = KErrNone;
                }
            }
            break;

        case ETFAccessoryBtControl_Close:
            {
            TRACE_ASSERT( iAccessoryBTControl != NULL );

            if( iAccessoryBTControl != NULL )
                {
                iAccessoryBTControl->CloseSubSession();
                delete iAccessoryBTControl;
                iAccessoryBTControl = NULL;
                }
            retval = KErrNone;
            }
            break;

        case ETFAccessoryBtConnectAccessory:
            {
            TBTDevAddr btaddr( aParameter.iArg1 );
            CTFTestControlObserver* aConnectionObserver = CreateObserver();
            TRACE_ASSERT( aConnectionObserver != NULL );
            aConnectionObserver->ConnectBTAccessory( btaddr );
            retval = KErrNone;
            }
            break;
        case ETFAccessoryBtDisconnectAccessory:
            {
            TBTDevAddr btaddr( aParameter.iArg1 );
            TRequestStatus status;
            CTFTestControlObserver* aConnectionObserver = CreateObserver();
            TRACE_ASSERT( aConnectionObserver != NULL );
            aConnectionObserver->DisconnectBTAccessory( btaddr );
            retval = KErrNone;
            TAccPolGenericID emptyInstance;
            iGenericId[aParameter.iGid] = emptyInstance;
            }
            break;

        case ETFAccessoryNotifyBluetoothAudioLinkOpenReq:
            {
            //TRequestStatus status; 			   	
            CTFTestControlObserver* aConnectionObserver = CreateObserver();
            TRACE_ASSERT( aConnectionObserver != NULL );
            aConnectionObserver->NotifyBluetoothAudioLinkOpenReq();
            retval = KErrNone;
            }
            break;

        case ETFAccessoryBluetoothAudioLinkOpenedNotify:
            {
            TBTDevAddr btaddr( aParameter.iArg1 );
            TRAPD( err, iAccessoryBTControl->BluetoothAudioLinkOpenedNotifyL( btaddr, (TAccAudioType)aParameter.iArg3 ) );

            if( err == aParameter.iExpectedResult )
                {
                retval = KErrNone;
                }
            else
                {
                retval = KErrGeneral;
                }
            }
            break;

        case ETFAccessoryBluetoothAudioLinkOpenResp:
            {
            TBTDevAddr btaddr( aParameter.iArg1 );
            TInt response( aParameter.iArg2 );
            TRAPD( err, iAccessoryBTControl->BluetoothAudioLinkOpenRespL( btaddr, response ) );

            if( err == aParameter.iExpectedResult )
                {
                retval = KErrNone;
                }
            else
                {
                retval = KErrGeneral;
                }
            }
            break;

        case ETFAccessoryNotifyBluetoothAudioLinkCloseReq:
            {
            CTFTestControlObserver* aConnectionObserver = CreateObserver();
            TRACE_ASSERT( aConnectionObserver != NULL );
            aConnectionObserver->NotifyBluetoothAudioLinkCloseReq();
            retval = KErrNone;
            }
            break;

        case ETFAccessoryBluetoothAudioLinkClosedNotify:
            {
            TBTDevAddr btaddr( aParameter.iArg1 );
            TRAPD( err, iAccessoryBTControl->BluetoothAudioLinkClosedNotifyL( btaddr, ( TAccAudioType )aParameter.iArg3 ) );

            if( err == aParameter.iExpectedResult )
                {
                retval = KErrNone;
                }
            else
                {
                retval = KErrGeneral;
                }
            }
            break;

        case ETFAccessoryBluetoothAudioLinkCloseResp:
            {
            TBTDevAddr btaddr( aParameter.iArg1 );
            TInt response( aParameter.iArg2 );
            TRAPD( err, iAccessoryBTControl->BluetoothAudioLinkCloseRespL( btaddr, response ) );

            if( err == aParameter.iExpectedResult )
                {
                retval = KErrNone;
                }
            else
                {
                retval = KErrGeneral;
                }
            }
            break;

        case ETFAccessoryBtCancelConnectAccessory:
            {
            TRequestStatus status;
            TBTDevAddr btaddr( aParameter.iArg1 );
            iAccessoryBTControl->ConnectAccessory( status, btaddr );
            iAccessoryBTControl->CancelConnectAccessory();
            User::WaitForRequest( status );
            if( KErrCancel != status.Int() )
                {
                retval = status.Int();
                }
            else
                {
                retval = KErrNone;
                }
            }
            break;

        case ETFAccessoryBtCancelDisconnectAccessory:
            {
            TRequestStatus status;
            TBTDevAddr btaddr( aParameter.iArg1 );
            iAccessoryBTControl->DisconnectAccessory( status, btaddr );
            iAccessoryBTControl->CancelDisconnectAccessory();
            User::WaitForRequest( status );
            retval = status.Int();
            }
            break;

            //
            // Accessory Mode 
            //                                            	         		      		
        case ETFAccessoryMode_Open:
            {
            if( iAccessoryMode == NULL )
                {
                iAccessoryMode = new ( ELeave ) RAccessoryMode;
                retval = iAccessoryMode->CreateSubSession( iAccessoryServer );
                }
            else
                {
                retval = KErrNone;
                }
            }
            break;

        case ETFAccessoryMode_Close:
            {
            TRACE_ASSERT( iAccessoryMode != NULL );

            if( iAccessoryMode != NULL )
                {
                iAccessoryMode->CloseSubSession();
                delete iAccessoryMode;
                iAccessoryMode = NULL;
                }
            retval = KErrNone;
            }
            break;

        case ETFAccessoryNotifyAccessoryModeChanged:
            {
            CTFTestControlObserver* aConnectionObserver = CreateObserver();
            TRACE_ASSERT( aConnectionObserver != NULL );
            aConnectionObserver->NotifyAccessoryModeChanged();
            retval = KErrNone;
            }
            break;

        case ETFAccessoryAccessoryModeSync:
            {
            TAccPolAccessoryMode mode;
            retval = iAccessoryMode->GetAccessoryMode( mode );

            if( retval == KErrNone )
                {
                if( aParameter.iArg1 != mode.iAccessoryMode )
                    {
                    TEST_CASE_TRACE( ( _L( "CTFAccessoryTestControl::CallAccessoryFunctionL: Unexpected Accessory Mode Parameter1 %d %d" ), aParameter.iArg1, mode.iAccessoryMode ) );
                    retval = KTFErrAccessoryUnexpectedArg1;
                    }
#ifdef __WINS__ // Just check this in winscw
                if( aParameter.iArg2 != mode.iAudioOutputStatus )
                    {
                    TEST_CASE_TRACE( ( _L( "CTFAccessoryTestControl::CallAccessoryFunctionL: Unexpected Accessory Mode Parameter2 %d %d" ), aParameter.iArg2, mode.iAudioOutputStatus ) );
                    retval = KTFErrAccessoryUnexpectedArg2;
                    }
#endif
                }
            else
                {
                TEST_CASE_TRACE( ( _L( "CTFAccessoryTestControl::CallAccessoryFunctionL: Getting Accessory Mode Failed %d" ), retval ) );
                }

            }
            break;

        case ETFAccessoryAccessoryModeASync:
            {
            CTFTestControlObserver* aConnectionObserver = CreateObserver();
            TRACE_ASSERT( aConnectionObserver != NULL );
            aConnectionObserver->GetAccessoryMode();
            retval = KErrNone;
            }
            break;

            //
            // Audio Control
            //       
        case ETFAccessoryAudioControl_Open:
            {
            if( iAccessoryAudioControl == NULL )
                {
                iAccessoryAudioControl = new ( ELeave ) RAccessoryAudioControl;
                retval
                    = iAccessoryAudioControl->CreateSubSession( iAccessoryServer );
                }
            else
                {
                retval = KErrNone;
                }
            }
            break;

        case ETFAccessoryAudioControl_Close:
            {
            TRACE_ASSERT( iAccessoryAudioControl != NULL );

            if( iAccessoryAudioControl != NULL )
                {
                iAccessoryAudioControl->CloseSubSession();
                delete iAccessoryAudioControl;
                iAccessoryAudioControl = NULL;
                }
            retval = KErrNone;
            }
            break;

        case ETFAudioRoutingStatusNotify:
            {
            RAccessoryAudioControl audioControl;
            audioControl.CreateSubSession( iAccessoryServer );

            audioControl.AudioRoutingStatusNotify( aParameter.iArg1,
                iGenericId[aParameter.iGid] );
            retval = KErrNone;

            audioControl.CloseSubSession();
            }
            break;

        case ETFAccessoryAudioLinkOpen:
            {
            CTFTestControlObserver* aConnectionObserver = CreateObserver();
            TRACE_ASSERT( aConnectionObserver != NULL );
            aConnectionObserver->AccessoryAudioLinkOpen( iGenericId[aParameter.iGid],
                aParameter.iArg3 );
            retval = KErrNone;
            }
            break;

        case ETFAccessoryNotifyAccessoryAudioLinkOpened:
            {
            CTFTestControlObserver* aConnectionObserver = CreateObserver();
            TRACE_ASSERT( aConnectionObserver != NULL );
            aConnectionObserver->NotifyAccessoryAudioLinkOpened( iGenericId[aParameter.iGid] );
            retval = KErrNone;
            }
            break;

        case ETFAccessoryNotifyAccessoryAudioLinkClosed:
            {
            CTFTestControlObserver* aConnectionObserver = CreateObserver();
            TRACE_ASSERT( aConnectionObserver != NULL );
            aConnectionObserver->NotifyAccessoryAudioLinkClosed( iGenericId[aParameter.iGid] );
            retval = KErrNone;
            }
            break;

        case ETFAccessoryAccessoryAudioLinkClose:
            {
            CTFTestControlObserver* aConnectionObserver = CreateObserver();
            TRACE_ASSERT( aConnectionObserver != NULL );
            aConnectionObserver->AccessoryAudioLinkClose( iGenericId[aParameter.iGid],
                aParameter.iArg3 );
            retval = KErrNone;
            }
            break;

        case ETFAccessoryPublishAndSubscribe:
            {
            PublishAndSubscribeL( aParameter );
            retval = KErrNone;
            }
            break;

        case ETFAccessory_Cancel:
            {
            CTFTestControlObserver* item;
            if( !iStack.IsEmpty() )
                {
                item = iStack.First();
                item->Cancel();
                }
            retval = KErrNone;
            }
            break;

            //
            // Cancels
            //
        case ETFCancelAccessoryAudioLinkOpen:
            {
            iAccessoryAudioControl->CancelAccessoryAudioLinkOpen();
            retval = KErrNone;
            }
            break;

        case ETFCancelAccessoryAudioLinkClose:
            {
            iAccessoryAudioControl->CancelAccessoryAudioLinkClose();
            retval = KErrNone;
            }
            break;

        case ETFCancelNotifyAccessoryAudioLinkOpened:
            {
            iAccessoryAudioControl->CancelNotifyAccessoryAudioLinkOpened();
            retval = KErrNone;
            }
            break;

        case ETFCancelNotifyAccessoryAudioLinkClosed:
            {
            iAccessoryAudioControl->CancelNotifyAccessoryAudioLinkClosed();
            retval = KErrNone;
            }
            break;

        case ETFCancelNotifyProcessCommand:
            {
            TRACE_ASSERT_ALWAYS;
            }
            break;

        case ETFCancelConnectAccessory:
            {
            iAccessoryControl->CancelConnectAccessory();
            retval = KErrNone;
            }
            break;

        case ETFCancelDisconnectAccessory:
            {
            iAccessoryControl->CancelDisconnectAccessory();
            retval = KErrNone;
            }
            break;

        case ETFCancelNotifyBluetoothAudioLinkOpenReq:
            {
            iAccessoryBTControl->CancelNotifyBluetoothAudioLinkOpenReq();
            retval = KErrNone;
            }
            break;

        case ETFCancelNotifyBluetoothAudioLinkCloseReq:
            {
            iAccessoryBTControl->CancelNotifyBluetoothAudioLinkCloseReq();
            retval = KErrNone;
            }
            break;

        case ETFCancelNotifyAccessoryConnectionStatusChanged:
            {
            iAccessoryConnection->CancelNotifyAccessoryConnectionStatusChanged();
            retval = KErrNone;
            }
            break;

        case ETFCancelGetAccessoryConnectionStatus:
            {
            iAccessoryConnection->CancelGetAccessoryConnectionStatus();
            retval = KErrNone;
            }
            break;

        case ETFCancelConnectAccessoryBT:
            {
            iAccessoryBTControl->CancelConnectAccessory();
            retval = KErrNone;
            }
            break;

        case ETFCancelDisconnectAccessoryBT:
            {
            iAccessoryBTControl->CancelDisconnectAccessory();
            retval = KErrNone;
            }
            break;

        case ETFCancelNotifyAccessoryModeChanged:
            {
            iAccessoryMode->CancelNotifyAccessoryModeChanged();
            retval = KErrNone;
            }
            break;

        case ETFCancelGetAccessoryMode:
            {
            iAccessoryMode->CancelGetAccessoryMode();
            retval = KErrNone;
            }
            break;

        case ETFCancelNotifyNewAccessoryConnected:
            {
            iAccessorySingleConnection->CancelNotifyNewAccessoryConnected();
            retval = KErrNone;
            }
            break;

        case ETFCancelNotifyAccessoryDisconnected:
            {
            iAccessorySingleConnection->CancelNotifyAccessoryDisconnected();
            retval = KErrNone;
            }
            break;

        case ETFCancelNotifyAccessoryValueChanged:
            {
            TAccPolNameRecord nameRecord;
            nameRecord.SetNameL( aParameter.iArg3 );
            iAccessorySingleConnection->CancelNotifyAccessoryValueChanged( nameRecord );
            retval = KErrNone;
            }
            break;

        case ETFSelectionDialogText:
            {
                SelectionDialog(ETFSelectionDialogText, &retval);
            }
            break;

        case ETFSelectionDialogCancel:
            {
                SelectionDialog(ETFSelectionDialogCancel, &retval);
            }
            break;

        case ETFNotSupportedNote:
            {
            TUid KAccFwUiNoteNotifierUid =
                {
                0x10205061
                };
            RNotifier notifier;
            notifier.Connect();

            TInt err = notifier.StartNotifier( KAccFwUiNoteNotifierUid,
                KNullDesC8 );

            TInt value( 0 );
            TPckg<TInt> intPckg( value );
            notifier.Close();

            notifier.Connect();
            err = notifier.StartNotifier( KAccFwUiNoteNotifierUid, intPckg );
            notifier.Close();

            value = 1;
            notifier.Connect();
            err = notifier.StartNotifier( KAccFwUiNoteNotifierUid, intPckg );
            notifier.Close();

            value = value << 1;
            notifier.Connect();
            err = notifier.StartNotifier( KAccFwUiNoteNotifierUid, intPckg );
            notifier.Close();

            value = value << 1;
            notifier.Connect();
            err = notifier.StartNotifier( KAccFwUiNoteNotifierUid, intPckg );
            notifier.Close();

            value = value << 1;
            notifier.Connect();
            err = notifier.StartNotifier( KAccFwUiNoteNotifierUid, intPckg );
            notifier.Close();

            value = value << 1;
            notifier.Connect();
            err = notifier.StartNotifier( KAccFwUiNoteNotifierUid, intPckg );
            notifier.Close();

            value = value << 1;
            notifier.Connect();
            err = notifier.StartNotifier( KAccFwUiNoteNotifierUid, intPckg );
            notifier.Close();

            value = value << 1;
            notifier.Connect();
            err = notifier.StartNotifier( KAccFwUiNoteNotifierUid, intPckg );
            notifier.Close();

            retval = KErrNone;
            }
            break;

        case ETFValueRecordTests:
            {
            retval = TestValueRecordL();
            }
            break;

        case ETFObjectBaseSerializationTests:
            {
            retval = TestBaseSerializationL();
            }
            break;

        case ETFCheckConnectedAccessory:
            {
            CheckConnectedAccessory(&retval);
            }
            break;

        case ETFCallTClassMethods:
            {
						CallTClassMethods(&retval);  \
						}
            
        case ETFSetHdmiObject_PubSub:
            {
            CreateHdmiObjectsToPubSubL( aParameter );
            retval = KErrNone;
            break;
            }
            
        case ETFGetHdmiObject:
            {
            TInt err = KErrNone;
            TAccPolNameRecord nameRecord;
            nameRecord.SetNameL( aParameter.iArg3 );

            // Create HDMI container
            CAccPolHdmiObjectCon* conFromASY = CAccPolHdmiObjectCon::NewLC();
            CAccPolHdmiObjectCon* conFromPS = CAccPolHdmiObjectCon::NewLC();
            
            // Get HDMI container
            TRAP( err, iAccessorySingleConnection->GetValueL(
                iGenericId[aParameter.iGid], nameRecord, *conFromASY ) );
            
            // Get HDMI container from P&S
            CBufFlat* buf = CBufFlat::NewL( KAccSrvObjectBaseStreamBufGranularity );
            CleanupStack::PushL( buf );
            buf->ResizeL( KAccSrvObjectBaseStreamBufMaxSize );
            
            TPtr8 bufPtr( buf->Ptr( 0 ) );
            err = RProperty::Get( KTFAccessoryTestProperty,
                KTFAccessoryObjectMethod,
                bufPtr );
            if( err == KErrNone )
                {
                RBufReadStream readStream( *buf );
                readStream.PushL();
                conFromPS->InternalizeL( readStream );
                CleanupStack::PopAndDestroy( &readStream );
                }
            switch( aParameter.iArg3 )
                {
                case KAccVideoHdmiAttributes:
                    {
                    // Validate object count
                    if( conFromASY->Count() != conFromPS->Count() )
                        {
                        err = KErrArgument;
                        }
                    
                    // Validate speaker allocation
                    RAccPolHdmiSpeakerAllocationArray array;
                    CleanupClosePushL( array );
                    conFromASY->GetHdmiSpeakerAllocationObjectsL( array );
                    if( array.Count() )
                        {
                        CAccPolHdmiSpeakerAllocation* speakerAllocation = array[0];
                        TUint32 bits = speakerAllocation->SpeakerAllocation();
                        
                        // Speaker allocation should be EHdmiSpeakerUnknown
                        if( bits != CAccPolHdmiSpeakerAllocation::EHdmiSpeakerUnknown )
                            {
                            err = KErrArgument;
                            }
                        
                        // Speaker count should be zero
                        if( speakerAllocation->SpeakerCount() )
                            {
                            err = KErrArgument;
                            }
                        }
                    CleanupStack::PopAndDestroy( &array );
                    break;
                    }
                case KAccVideoFormat:
                    {
                    // Validate object count
                    if( conFromASY->Count() != conFromPS->Count() )
                        {
                        err = KErrArgument;
                        }
                    
                    // Validate video format
                    RAccPolHdmiVideoFormatArray array;
                    CleanupClosePushL( array );
                    conFromASY->GetHdmiVideoFormatObjectsL( array );
                    if( array.Count() )
                        {
                        CAccPolHdmiVideoFormat* videoFormat = array[0];
                        
                        // Interlaced video should be enabled
                        if( !videoFormat->Interlaced() )
                            {
                            err = KErrArgument;
                            }
                        }
                    else
                        {
                        err = KErrArgument;
                        }
                    CleanupStack::PopAndDestroy( &array );
                    break;
                    }
                default:
                    {
                    break;
                    }
                }            
            CleanupStack::PopAndDestroy( buf );
            CleanupStack::PopAndDestroy( conFromPS );
            CleanupStack::PopAndDestroy( conFromASY );
            retval = err;
            break;
            }
            
        case ETFCheckHdmiObject:
            {
            TInt err = KErrNone;
            TEST_CASE_TRACE( ( _L( "CTFAccessoryTestControl::CallAccessoryFunctionL: Check HDMI object %d" ),
                aParameter.iArg1 ) );
            switch( aParameter.iArg1 )
                {
                case EAccPolHdmiObjectCon:
                    {
                    err = CheckHdmiContainerObjectL();
                    break;
                    }
                case EAccPolHdmiSinkObject:
                    {
                    err = CheckHdmiSinkObjectL();
                    break;
                    }
                case EAccPolHdmiAudioFormatObject:
                    {
                    err = CheckHdmiAudioFormatObjectL();
                    break;
                    }
                case EAccPolHdmiLatencyObject:
                    {
                    err = CheckHdmiLatencyObjectL();
                    break;
                    }
                case EAccPolHdmiSpeakerAllocationObject:
                    {
                    err = CheckHdmiSpeakerAllocationObjectL();
                    break;
                    }
                case EAccPolHdmiVideoFormatObject:
                    {
                    err = CheckHdmiVideoFormatObjectL();
                    break;
                    }
                default:
                    {
                    TEST_CASE_TRACE( ( _L( "CTFAccessoryTestControl::CallAccessoryFunctionL: Invalid HDMI object=%d" ),
                        aParameter.iArg1 ) );
                    err = KErrArgument;
                    break;
                    }
                }
            TEST_CASE_TRACE( ( _L( "CTFAccessoryTestControl::CallAccessoryFunctionL: Check HDMI object %d returned with code %d" ),
                aParameter.iArg1, err ) );
            retval = err;
            break;
            }

        default:
            TRACE_ASSERT_ALWAYS;
            break;

        }
    User::LeaveIfError( retval );
    }

//
// ----------------------------------------------------------------------------------
// CTFAccessoryTestControl::TestValueRecordL()
// ----------------------------------------------------------------------------------	
//
TInt CTFAccessoryTestControl::TestValueRecordL()
    {
    TInt ret( KErrNone );

    TAccPolValueRecord valueRecord;
    TBuf8<KAccMaxECIBlockLength> eci;
    TBuf8<KAccMaxECIBlockLength> check;
    _LIT( Kissa, "Kissa" );
    eci.Copy( Kissa );

    valueRecord.SetValue( eci );
    check.Copy( valueRecord.ValueRef() );

    if( KErrNone != eci.Compare( check ) )
        {
        User::Leave( KErrGeneral );
        }

    TDesC8& test( valueRecord.ValueRef() );
    if( KErrNone != eci.Compare( test ) )
        {
        User::Leave( KErrGeneral );
        }

    return ret;
    }

//
// ----------------------------------------------------------------------------------
// CTFAccessoryTestControl::TestBaseSerializationL()
// ----------------------------------------------------------------------------------
//
TInt CTFAccessoryTestControl::TestBaseSerializationL()
    {
    TInt ret( KErrNone );
    TInt err( KErrNone );

    /*** One container *************************************************************/
    CAccPolAudioStreamFormatCon* container1 =
        CAccPolAudioStreamFormatCon::NewLC();
    CAccPolAudioStreamFormatCon* container2 =
        CAccPolAudioStreamFormatCon::NewLC();

    CBufFlat* buf = CBufFlat::NewL( KAccSrvObjectBaseStreamBufGranularity );
    CleanupStack::PushL( buf );
    buf->ResizeL( KAccSrvObjectBaseStreamBufMaxSize );

    //Externalize to buf
    RBufWriteStream wStrm;
    ( void )wStrm.Open( *buf );
    wStrm.PushL();
    container1->ExternalizeL( wStrm );
    wStrm.CommitL();
    wStrm.Close();
    wStrm.Pop();

    //Internalize from buf
    RBufReadStream rStrm;
    ( void )rStrm.Open( *buf );
    rStrm.PushL();
    container2->InternalizeL( rStrm );
    rStrm.Close();
    rStrm.Pop();

    //container1 content must equal to container2 content
    err = ( 0 == memcompare( reinterpret_cast<TUint8*> ( container1 ),
        sizeof(CAccPolObjectCon),
        reinterpret_cast<TUint8*> ( container2 ),
        sizeof(CAccPolObjectCon) ) ) ? KErrNone : KErrGeneral;
    User::LeaveIfError( err );

    CleanupStack::PopAndDestroy( buf );
    CleanupStack::PopAndDestroy( container2 );
    CleanupStack::PopAndDestroy( container1 );
    container2 = NULL;
    container1 = NULL;
    /* end *************************************************************************/

    //Make test again!

    CAccPolAudioStreamFormat* audioStream1 =
        CAccPolAudioStreamFormat::NewLC( EAccPolAudioInputStreamObject,
            15,
            KUidFormatMP3,
            CAccPolAudioStreamFormat::EStereo,
            CAccPolAudioStreamFormat::EBitsPerSample24,
            11 );

    buf = CBufFlat::NewL( KAccSrvObjectBaseStreamBufGranularity );
    CleanupStack::PushL( buf );
    buf->ResizeL( KAccSrvObjectBaseStreamBufMaxSize );

    //Externalize to buf        
    ( void )wStrm.Open( *buf );
    wStrm.PushL();
    audioStream1->ExternalizeL( wStrm );
    wStrm.CommitL();
    wStrm.Close();
    wStrm.Pop();

    //Internalize from buf
    ( void )rStrm.Open( *buf );
    rStrm.PushL();

    TInt objectid = rStrm.ReadUint8L();

    if( ( TStreamConObjectType )objectid == EAccPolAudioInputStreamObject )
        {
        CAccPolAudioStreamFormat* audioStream2 =
            CAccPolAudioStreamFormat::NewLC( EAccPolAudioInputStreamObject );

        audioStream2->InternalizeL( rStrm );

        //audioStream1 content must equal to audioStream2 content
        err = ( 0 == memcompare( reinterpret_cast<TUint8*> ( audioStream1 ),
            sizeof(CAccPolAudioStreamFormat),
            reinterpret_cast<TUint8*> ( audioStream2 ),
            sizeof(CAccPolAudioStreamFormat) ) ) ? KErrNone : KErrGeneral;

        User::LeaveIfError( err );

        CleanupStack::PopAndDestroy( audioStream2 );

        rStrm.Close();
        rStrm.Pop();

        }
    else
        {
        User::LeaveIfError( KErrGeneral );
        }

    CleanupStack::PopAndDestroy( buf );
    CleanupStack::PopAndDestroy( audioStream1 );
    audioStream1 = NULL;
    /* end *************************************************************************/

    return ret;
    }

//
// ----------------------------------------------------------------------------------
// CTFAccessoryTestControl::PublishAndSubscribeL()
// ----------------------------------------------------------------------------------	
//
void CTFAccessoryTestControl::PublishAndSubscribeL( TTFAccessoryTestCaseStateControl& aParameter )
    {
    TPckgBuf<TTFAccessoryPublishAndSubscribe> buf;

    buf().iTestCaseID = aParameter.iTestCaseID;
    buf().iMethod = aParameter.iMethod;
    buf().iParam1 = aParameter.iArg1;
    buf().iTimeMs = aParameter.iArg4;
    buf().iGenericID = iGenericId[aParameter.iGid];

    // Send capability to stub(s).

    // if( aParameter.iArg3 )
    // {
    buf().iParam2 = aParameter.iArg3;
    // }

    /*
     if( aParameter.iArg3->Compare( KNullDesC ) )
     {
     buf().iParam2.Copy( *aParameter.iArg3 );    
     }

     */
    User::LeaveIfError( iSubscribe.Set( KTFAccessoryTestProperty,
        KTFAccessoryMethod,
        buf ) );
    }

//
// ----------------------------------------------------------------------------------
// CTFAccessoryTestControl::CreateHdmiObjectsToPubSubL()
// ----------------------------------------------------------------------------------   
//
void CTFAccessoryTestControl::CreateHdmiObjectsToPubSubL(
    TTFAccessoryTestCaseStateControl& aParameter )
    {
    // Define property just in case
    RProperty::Define( KTFAccessoryTestProperty,
        KTFAccessoryObjectMethod,
        RProperty::EByteArray );
    
    // Create HDMI container
    CAccPolHdmiObjectCon* con = CAccPolHdmiObjectCon::NewLC();
    switch( aParameter.iArg3 )
        {
        case KAccVideoHdmiAttributes:
            {
            // Create audio format object
            CreateHdmiObjectL( *con, EAccPolHdmiAudioFormatObject );
            
            // Create latency object
            CreateHdmiObjectL( *con, EAccPolHdmiLatencyObject );
            
            // Create sink object
            CreateHdmiObjectL( *con, EAccPolHdmiSinkObject );
            
            // Create speaker allocation object
            CreateHdmiObjectL( *con, EAccPolHdmiSpeakerAllocationObject );
            
            // Create video format object
            CreateHdmiObjectL( *con, EAccPolHdmiVideoFormatObject );
            break;
            }
        case KAccVideoFormat:
            {
            // Create video format object
            CreateHdmiObjectL( *con, EAccPolHdmiVideoFormatObject );
            break;
            }
        default:
            {
            break;
            }
        }
    // Stream HDMI container to P&S
    CBufFlat* buf = CBufFlat::NewL( KAccSrvObjectBaseStreamBufGranularity );
    CleanupStack::PushL( buf );
    buf->ResizeL( KAccSrvObjectBaseStreamBufMaxSize );

    RBufWriteStream writeStream( *buf );
    writeStream.PushL();
    con->ExternalizeL( writeStream );
    writeStream.CommitL();
    CleanupStack::PopAndDestroy( &writeStream );
    
    TPtr8 bufPtr( buf->Ptr( 0 ) );
    TInt err = RProperty::Set( KTFAccessoryTestProperty,
        KTFAccessoryObjectMethod,
        bufPtr );
    User::LeaveIfError( err );
    
    // Check if ASY stub needs to be informed
    if( aParameter.iArg2 )
        {
        PublishAndSubscribeL( aParameter );
        }
    
    // Cleanup
    CleanupStack::PopAndDestroy( buf );
    CleanupStack::PopAndDestroy( con );
    }

//
// ----------------------------------------------------------------------------------
// CTFAccessoryTestControl::CheckHdmiContainerObjectL
// ----------------------------------------------------------------------------------   
TInt CTFAccessoryTestControl::CheckHdmiContainerObjectL()
    {
    TInt err = KErrNone;
    CAccPolHdmiObjectCon* con1 = NULL;
    CAccPolHdmiObjectCon* con2 = NULL;
    CBufFlat* buf = CBufFlat::NewL( KAccSrvObjectBaseStreamBufGranularity );
    CleanupStack::PushL( buf );
    buf->ResizeL( KAccSrvObjectBaseStreamBufMaxSize );
    
    // NewL
    con1 = CAccPolHdmiObjectCon::NewL();
    CleanupStack::PushL( con1 );
    CleanupStack::PopAndDestroy( con1 );
    
    // NewLC
    con1 = CAccPolHdmiObjectCon::NewLC();
    CleanupStack::PopAndDestroy( con1 );
    
    // Internalize & Externalize
    con1 = CAccPolHdmiObjectCon::NewLC();
    CreateHdmiObjectL( *con1, EAccPolHdmiAudioFormatObject );
    CreateHdmiObjectL( *con1, EAccPolHdmiLatencyObject );
    CreateHdmiObjectL( *con1, EAccPolHdmiSinkObject );
    CreateHdmiObjectL( *con1, EAccPolHdmiSpeakerAllocationObject );
    CreateHdmiObjectL( *con1, EAccPolHdmiVideoFormatObject );
    RBufWriteStream writeStream;
    writeStream.Open( *buf );
    writeStream.PushL();
    con1->ExternalizeL( writeStream );
    writeStream.CommitL();
    CleanupStack::PopAndDestroy( &writeStream );
    
    con2 = CAccPolHdmiObjectCon::NewLC();
    RBufReadStream readStream;
    readStream.Open( *buf );
    readStream.PushL();
    con2->InternalizeL( readStream );
    if( con1->Count() == con2->Count() )
        {
        TInt count = con1->Count();
        CAccPolObjectBase* objBase1 = NULL;
        CAccPolObjectBase* objBase2 = NULL;
        for( TInt i = 0; i < count; i++ )
            {
            objBase1 = con1->AtL( i );
            objBase2 = con2->AtL( i );
            if( objBase1->ObjectType() != objBase2->ObjectType() )
                {
                err = KErrArgument;
                }
            }
        }
    else
        {
        err = KErrArgument;
        }
    CleanupStack::PopAndDestroy( &readStream );
    CleanupStack::PopAndDestroy( con2 );
    CleanupStack::PopAndDestroy( con1 );
    
    // DuplicateLC
    con1 = CAccPolHdmiObjectCon::NewLC();
    CreateHdmiObjectL( *con1, EAccPolHdmiAudioFormatObject );
    CreateHdmiObjectL( *con1, EAccPolHdmiLatencyObject );
    CreateHdmiObjectL( *con1, EAccPolHdmiSinkObject );
    CreateHdmiObjectL( *con1, EAccPolHdmiSpeakerAllocationObject );
    CreateHdmiObjectL( *con1, EAccPolHdmiVideoFormatObject );
    con2 = static_cast<CAccPolHdmiObjectCon*>( con1->DuplicateLC() );
    if( con1->Count() == con2->Count() )
        {
        TInt count = con1->Count();
        CAccPolObjectBase* objBase1 = NULL;
        CAccPolObjectBase* objBase2 = NULL;
        for( TInt i = 0; i < count; i++ )
            {
            objBase1 = con1->AtL( i );
            objBase2 = con2->AtL( i );
            if( objBase1->ObjectType() != objBase2->ObjectType() )
                {
                err = KErrArgument;
                }
            }
        }
    else
        {
        err = KErrArgument;
        }
    CleanupStack::PopAndDestroy( con2 );
    CleanupStack::PopAndDestroy( con1 );
    
    // Rest of the API
    con1 = CAccPolHdmiObjectCon::NewLC();
    THdmiConObjectType objectType = EAccPolHdmiObjectCon;
    TBool exit = EFalse;
    TInt index = 0;
    TInt count = 0;
    do
        {
        index++;
        err = KErrArgument;
        objectType = ( THdmiConObjectType )( index );
        CreateHdmiObjectL( *con1, objectType );
        count = con1->Count();
        CAccPolObjectBase* obj = NULL;
        if( con1->Count() != index )
            {
            err = KErrArgument;
            break;
            }
        for( TInt ii = 0; ii < count; ii++ )
            {
            obj = con1->AtL( ii );
            if( obj->ObjectType() == objectType )
                {
                err = KErrNone;
                break;
                }
            }
        if( objectType == EAccPolHdmiVideoFormatObject )
            {
            // Last object
            exit = ETrue;
            }
        }
    while( !exit && ( err == KErrNone ) );
    CleanupStack::PopAndDestroy( con1 );
    CleanupStack::PopAndDestroy( buf );
    
    return err;
    }

//
// ----------------------------------------------------------------------------------
// CTFAccessoryTestControl::CheckHdmiSinkObjectL
// ----------------------------------------------------------------------------------   
TInt CTFAccessoryTestControl::CheckHdmiSinkObjectL()
    {
    TInt err = KErrNone;
    CAccPolHdmiSink* obj = NULL;
    CAccPolHdmiSink* objDuplicate = NULL;
    TUint8 objectType = 0;
    CBufFlat* buf = CBufFlat::NewL( KAccSrvObjectBaseStreamBufGranularity );
    CleanupStack::PushL( buf );
    buf->ResizeL( KAccSrvObjectBaseStreamBufMaxSize );
    
    // NewL
    obj = CAccPolHdmiSink::NewL();
    CleanupStack::PushL( obj );
    CleanupStack::PopAndDestroy( obj );

    // NewL - with params
    obj = CAccPolHdmiSink::NewL( ETrue, ETrue );
    CleanupStack::PushL( obj );
    if( !obj->BasicAudioSupport() )
        {
        err = KErrArgument;
        }
    if( !obj->HdcpSupport() )
        {
        err = KErrArgument;
        }
    CleanupStack::PopAndDestroy( obj );
    
    // NewLC
    obj = CAccPolHdmiSink::NewLC();
    CleanupStack::PopAndDestroy( obj );

    // NewLC - with params
    obj = CAccPolHdmiSink::NewLC( ETrue, ETrue );
    if( !obj->BasicAudioSupport() )
        {
        err = KErrArgument;
        }
    if( !obj->HdcpSupport() )
        {
        err = KErrArgument;
        }
    CleanupStack::PopAndDestroy( obj );
    
    // Internalize & Externalize
    obj = CAccPolHdmiSink::NewLC( ETrue, ETrue );
    
    RBufWriteStream writeStream;
    writeStream.Open( *buf );
    writeStream.PushL();
    obj->ExternalizeL( writeStream );
    writeStream.CommitL();
    CleanupStack::PopAndDestroy( &writeStream );
    CleanupStack::PopAndDestroy( obj );
    
    obj = CAccPolHdmiSink::NewLC();
    RBufReadStream readStream;
    readStream.Open( *buf );
    readStream.PushL();
    objectType = readStream.ReadUint8L();
    obj->InternalizeL( readStream );
    if( objectType != EAccPolHdmiSinkObject )
        {
        err = KErrArgument;
        }
    if( !obj->BasicAudioSupport() )
        {
        err = KErrArgument;
        }
    if( !obj->HdcpSupport() )
        {
        err = KErrArgument;
        }
    CleanupStack::PopAndDestroy( &readStream );
    CleanupStack::PopAndDestroy( obj );

    // DuplicateLC - mem compare can be used since the object does not contain
    // any ponters as memebers
    obj = CAccPolHdmiSink::NewLC( ETrue, ETrue );
    objDuplicate = static_cast<CAccPolHdmiSink*>( obj->DuplicateLC() );
    TUint8* left = reinterpret_cast<TUint8*>( obj );
    TUint8* right = reinterpret_cast<TUint8*>( objDuplicate );
    TInt size = sizeof( CAccPolHdmiSink );
    if( Mem::Compare( left, size, right, size ) != KErrNone )
        {
        err = KErrArgument;
        }
    CleanupStack::PopAndDestroy( objDuplicate );
    CleanupStack::PopAndDestroy( obj );

    // Rest of the API
    obj = CAccPolHdmiSink::NewLC();
    obj->SetBasicAudioSupport( ETrue );
    if( !obj->BasicAudioSupport() )
        {
        err = KErrArgument;
        }
    obj->SetHdcpSupport( ETrue );
    if( !obj->HdcpSupport() )
        {
        err = KErrArgument;
        }
    CleanupStack::PopAndDestroy( obj );
    CleanupStack::PopAndDestroy( buf );
    return err;
    }

// ----------------------------------------------------------------------------------
// CTFAccessoryTestControl::CheckHdmiAudioFormatObjectL
// ----------------------------------------------------------------------------------   
TInt CTFAccessoryTestControl::CheckHdmiAudioFormatObjectL()
    {
    TInt err = KErrNone;
    CAccPolHdmiAudioFormat* obj = NULL;
    CAccPolHdmiAudioFormat* objDuplicate = NULL;
    TUint8 objectType = 0;
    CBufFlat* buf = CBufFlat::NewL( KAccSrvObjectBaseStreamBufGranularity );
    CleanupStack::PushL( buf );
    buf->ResizeL( KAccSrvObjectBaseStreamBufMaxSize );
    
    // NewL
    obj = CAccPolHdmiAudioFormat::NewL();
    CleanupStack::PushL( obj );
    CleanupStack::PopAndDestroy( obj );
    
    // NewL - with params
    obj = CAccPolHdmiAudioFormat::NewL( HdmiAudioFormat::KUidFormatPCM16,
        1,
        1,
        1,
        1,
        1 );
    if( obj->AudioFormat() != HdmiAudioFormat::KUidFormatPCM16 )
        {
        err = KErrArgument;
        }
    if( obj->ChannelCount() != 1 )
        {
        err = KErrArgument;
        }
    if( obj->BitResolution() != 1 )
        {
        err = KErrArgument;
        }
    if( obj->MaxBitResolution() != 1 )
        {
        err = KErrArgument;
        }
    if( obj->SamFreq() != 1 )
        {
        err = KErrArgument;
        }
    if( obj->FormatDependentValue() != 1 )
        {
        err = KErrArgument;
        }
    CleanupStack::PushL( obj );
    CleanupStack::PopAndDestroy( obj );
    
    // NewLC
    obj = CAccPolHdmiAudioFormat::NewLC();
    CleanupStack::PopAndDestroy( obj );
    
    // NewLC - with params
    obj = CAccPolHdmiAudioFormat::NewLC( HdmiAudioFormat::KUidFormatPCM16,
        1,
        1,
        1,
        1,
        1 );
    if( obj->AudioFormat() != HdmiAudioFormat::KUidFormatPCM16 )
        {
        err = KErrArgument;
        }
    if( obj->ChannelCount() != 1 )
        {
        err = KErrArgument;
        }
    if( obj->BitResolution() != 1 )
        {
        err = KErrArgument;
        }
    if( obj->MaxBitResolution() != 1 )
        {
        err = KErrArgument;
        }
    if( obj->SamFreq() != 1 )
        {
        err = KErrArgument;
        }
    if( obj->FormatDependentValue() != 1 )
        {
        err = KErrArgument;
        }
    CleanupStack::PopAndDestroy( obj );
    
    // Internalize & Externalize
    obj = CAccPolHdmiAudioFormat::NewLC( HdmiAudioFormat::KUidFormatPCM16,
        1,
        1,
        1,
        1,
        1 );
    RBufWriteStream writeStream;
    writeStream.Open( *buf );
    writeStream.PushL();
    obj->ExternalizeL( writeStream );
    writeStream.CommitL();
    CleanupStack::PopAndDestroy( &writeStream );
    CleanupStack::PopAndDestroy( obj );
    
    obj = CAccPolHdmiAudioFormat::NewLC();
    RBufReadStream readStream;
    readStream.Open( *buf );
    readStream.PushL();
    objectType = readStream.ReadUint8L();
    obj->InternalizeL( readStream );
    if( objectType != EAccPolHdmiAudioFormatObject )
        {
        err = KErrArgument;
        }
    if( obj->AudioFormat() != HdmiAudioFormat::KUidFormatPCM16 )
        {
        err = KErrArgument;
        }
    if( obj->ChannelCount() != 1 )
        {
        err = KErrArgument;
        }
    if( obj->BitResolution() != 1 )
        {
        err = KErrArgument;
        }
    if( obj->MaxBitResolution() != 1 )
        {
        err = KErrArgument;
        }
    if( obj->SamFreq() != 1 )
        {
        err = KErrArgument;
        }
    if( obj->FormatDependentValue() != 1 )
        {
        err = KErrArgument;
        }
    CleanupStack::PopAndDestroy( &readStream );
    CleanupStack::PopAndDestroy( obj );
    
    // DuplicateLC - mem compare can be used since the object does not contain
    // any ponters as memebers
    obj = CAccPolHdmiAudioFormat::NewLC( HdmiAudioFormat::KUidFormatPCM16,
        1,
        1,
        1,
        1,
        1 );
    objDuplicate = static_cast<CAccPolHdmiAudioFormat*>( obj->DuplicateLC() );
    TUint8* left = reinterpret_cast<TUint8*>( obj );
    TUint8* right = reinterpret_cast<TUint8*>( objDuplicate );
    TInt size = sizeof( CAccPolHdmiSink );
    if( Mem::Compare( left, size, right, size ) != KErrNone )
        {
        err = KErrArgument;
        }
    CleanupStack::PopAndDestroy( objDuplicate );
    CleanupStack::PopAndDestroy( obj );

    // Rest of the API
    obj = CAccPolHdmiAudioFormat::NewLC();
    obj->SetAudioFormat( HdmiAudioFormat::KUidFormatEAC3 );
    if( obj->AudioFormat() != HdmiAudioFormat::KUidFormatEAC3 )
        {
        err = KErrArgument;
        }
    obj->SetChannelCount( 2 );
    if( obj->ChannelCount() != 2 )
        {
        err = KErrArgument;
        }
    obj->SetBitResolution( CAccPolHdmiAudioFormat::EBitsPerSample20 );
    if( !( obj->BitResolution() & CAccPolHdmiAudioFormat::EBitsPerSample20 ) )
        {
        err = KErrArgument;
        }
    obj->SetMaxBitResolution( CAccPolHdmiAudioFormat::EBitsPerSample24 );
    if( obj->MaxBitResolution() != CAccPolHdmiAudioFormat::EBitsPerSample24 )
        {
        err = KErrArgument;
        }
    obj->SetSamFreq( CAccPolHdmiAudioFormat::ESamplingFreq192KHz );
    if( !( obj->SamFreq() & CAccPolHdmiAudioFormat::ESamplingFreq192KHz ) )
        {
        err = KErrArgument;
        }
    obj->SetFormatDependentValue( CAccPolHdmiAudioFormat::EBitsPerSample16 );
    if( !( obj->FormatDependentValue() & CAccPolHdmiAudioFormat::EBitsPerSample16 ) )
        {
        err = KErrArgument;
        }
    CleanupStack::PopAndDestroy( obj );
    CleanupStack::PopAndDestroy( buf );
    
    return err;
    }

// ----------------------------------------------------------------------------------
// CTFAccessoryTestControl::CheckHdmiLatencyObjectL
// ----------------------------------------------------------------------------------   
TInt CTFAccessoryTestControl::CheckHdmiLatencyObjectL()
    {
    TInt err = KErrNone;
    CAccPolHdmiLatency* obj = NULL;
    CAccPolHdmiLatency* objDuplicate = NULL;
    TUint8 objectType = 0;
    CBufFlat* buf = CBufFlat::NewL( KAccSrvObjectBaseStreamBufGranularity );
    CleanupStack::PushL( buf );
    buf->ResizeL( KAccSrvObjectBaseStreamBufMaxSize );
    
    // NewL
    obj = CAccPolHdmiLatency::NewL();
    CleanupStack::PushL( obj );
    CleanupStack::PopAndDestroy( obj );
    
    // NewL - with params
    obj = CAccPolHdmiLatency::NewL( HdmiLatency::KUidLatency, 100, 200 );
    CleanupStack::PushL( obj );
    if( obj->LatencyType() != HdmiLatency::KUidLatency )
        {
        err = KErrArgument;
        }
    if( obj->AudioLatency() != 100 )
        {
        err = KErrArgument;
        }
    if( obj->VideoLatency() != 200 )
        {
        err = KErrArgument;
        }
    CleanupStack::PopAndDestroy( obj );
    
    // NewLC
    obj = CAccPolHdmiLatency::NewLC();
    CleanupStack::PopAndDestroy( obj );
    
    // NewLC - with params
    obj = CAccPolHdmiLatency::NewLC( HdmiLatency::KUidLatency, 100, 200 );
    if( obj->LatencyType() != HdmiLatency::KUidLatency )
        {
        err = KErrArgument;
        }
    if( obj->AudioLatency() != 100 )
        {
        err = KErrArgument;
        }
    if( obj->VideoLatency() != 200 )
        {
        err = KErrArgument;
        }
    CleanupStack::PopAndDestroy( obj );
    
    // Internalize & Externalize
    obj = CAccPolHdmiLatency::NewLC( HdmiLatency::KUidLatency, 100, 200 );
    RBufWriteStream writeStream;
    writeStream.Open( *buf );
    writeStream.PushL();
    obj->ExternalizeL( writeStream );
    writeStream.CommitL();
    CleanupStack::PopAndDestroy( &writeStream );
    CleanupStack::PopAndDestroy( obj );
    
    obj = CAccPolHdmiLatency::NewLC();
    RBufReadStream readStream;
    readStream.Open( *buf );
    readStream.PushL();
    objectType = readStream.ReadUint8L();
    obj->InternalizeL( readStream );
    if( objectType != EAccPolHdmiLatencyObject )
        {
        err = KErrArgument;
        }
    if( obj->LatencyType() != HdmiLatency::KUidLatency )
        {
        err = KErrArgument;
        }
    if( obj->AudioLatency() != 100 )
        {
        err = KErrArgument;
        }
    if( obj->VideoLatency() != 200 )
        {
        err = KErrArgument;
        }
    CleanupStack::PopAndDestroy( &readStream );
    CleanupStack::PopAndDestroy( obj );
    
    // DuplicateLC - mem compare can be used since the object does not contain
    // any ponters as memebers
    obj = CAccPolHdmiLatency::NewLC( HdmiLatency::KUidLatency, 100, 200 );
    objDuplicate = static_cast<CAccPolHdmiLatency*>( obj->DuplicateLC() );
    TUint8* left = reinterpret_cast<TUint8*>( obj );
    TUint8* right = reinterpret_cast<TUint8*>( objDuplicate );
    TInt size = sizeof( CAccPolHdmiLatency );
    if( Mem::Compare( left, size, right, size ) != KErrNone )
        {
        err = KErrArgument;
        }
    CleanupStack::PopAndDestroy( objDuplicate );
    CleanupStack::PopAndDestroy( obj );

    // Rest of the API
    obj = CAccPolHdmiLatency::NewLC();
    obj->SetLatencyType( HdmiLatency::KUidLatency );
    if( obj->LatencyType() != HdmiLatency::KUidLatency )
        {
        err = KErrArgument;
        }
    obj->SetAudioLatency( 100 );
    if( obj->AudioLatency() != 100 )
        {
        err = KErrArgument;
        }
    obj->SetVideoLatency( 200 );
    if( obj->VideoLatency() != 200 )
        {
        err = KErrArgument;
        }
    CleanupStack::PopAndDestroy( obj );    
    CleanupStack::PopAndDestroy( buf );
    
    return err;
    }

// ----------------------------------------------------------------------------------
// CTFAccessoryTestControl::CheckHdmiSpeakerAllocationObjectL
// ----------------------------------------------------------------------------------   
TInt CTFAccessoryTestControl::CheckHdmiSpeakerAllocationObjectL()
    {
    TInt err = KErrNone;
    CAccPolHdmiSpeakerAllocation* obj = NULL;
    CAccPolHdmiSpeakerAllocation* objDuplicate = NULL;
    TUint8 objectType = 0;
    CBufFlat* buf = CBufFlat::NewL( KAccSrvObjectBaseStreamBufGranularity );
    CleanupStack::PushL( buf );
    buf->ResizeL( KAccSrvObjectBaseStreamBufMaxSize );
    TUint32 bits = 0x0;
    
    // NewL
    obj = CAccPolHdmiSpeakerAllocation::NewL();
    CleanupStack::PushL( obj );
    CleanupStack::PopAndDestroy( obj );
    
    // NewLC
    obj = CAccPolHdmiSpeakerAllocation::NewLC();
    CleanupStack::PopAndDestroy( obj );
    
    // NewL - with params
    bits = CAccPolHdmiSpeakerAllocation::EHdmiSpeakerFrontCenter |
        CAccPolHdmiSpeakerAllocation::EHdmiSpeakerRearCenter;
    obj = CAccPolHdmiSpeakerAllocation::NewL( bits );
    CleanupStack::PushL( obj );
    if( !obj->FrontCenter() )
        {
        err = KErrArgument;
        }
    if( !obj->RearCenter() )
        {
        err = KErrArgument;
        }
    if( !( obj->SpeakerAllocation() & CAccPolHdmiSpeakerAllocation::EHdmiSpeakerFrontCenter ) )
        {
        err = KErrArgument;
        }
    if( !( obj->SpeakerAllocation() & CAccPolHdmiSpeakerAllocation::EHdmiSpeakerRearCenter ) )
        {
        err = KErrArgument;
        }
    CleanupStack::PopAndDestroy( obj );

    obj = CAccPolHdmiSpeakerAllocation::NewL( EFalse,
        EFalse,
        EFalse,
        ETrue,
        EFalse,
        EFalse,
        ETrue,
        EFalse,
        EFalse,
        EFalse,
        EFalse );
    CleanupStack::PushL( obj );
    if( !obj->FrontCenter() )
        {
        err = KErrArgument;
        }
    if( !obj->RearCenter() )
        {
        err = KErrArgument;
        }
    if( !( obj->SpeakerAllocation() & CAccPolHdmiSpeakerAllocation::EHdmiSpeakerFrontCenter ) )
        {
        err = KErrArgument;
        }
    if( !( obj->SpeakerAllocation() & CAccPolHdmiSpeakerAllocation::EHdmiSpeakerRearCenter ) )
        {
        err = KErrArgument;
        }
    CleanupStack::PopAndDestroy( obj );

    // NewLC - with params
    bits = CAccPolHdmiSpeakerAllocation::EHdmiSpeakerFrontCenter |
        CAccPolHdmiSpeakerAllocation::EHdmiSpeakerRearCenter;
    obj = CAccPolHdmiSpeakerAllocation::NewLC( bits );
    if( !obj->FrontCenter() )
        {
        err = KErrArgument;
        }
    if( !obj->RearCenter() )
        {
        err = KErrArgument;
        }
    if( !( obj->SpeakerAllocation() & CAccPolHdmiSpeakerAllocation::EHdmiSpeakerFrontCenter ) )
        {
        err = KErrArgument;
        }
    if( !( obj->SpeakerAllocation() & CAccPolHdmiSpeakerAllocation::EHdmiSpeakerRearCenter ) )
        {
        err = KErrArgument;
        }
    CleanupStack::PopAndDestroy( obj );

    obj = CAccPolHdmiSpeakerAllocation::NewLC( EFalse,
        EFalse,
        EFalse,
        ETrue,
        EFalse,
        EFalse,
        ETrue,
        EFalse,
        EFalse,
        EFalse,
        EFalse );
    if( !obj->FrontCenter() )
        {
        err = KErrArgument;
        }
    if( !obj->RearCenter() )
        {
        err = KErrArgument;
        }
    if( !( obj->SpeakerAllocation() & CAccPolHdmiSpeakerAllocation::EHdmiSpeakerFrontCenter ) )
        {
        err = KErrArgument;
        }
    if( !( obj->SpeakerAllocation() & CAccPolHdmiSpeakerAllocation::EHdmiSpeakerRearCenter ) )
        {
        err = KErrArgument;
        }
    CleanupStack::PopAndDestroy( obj );
    
    // Internalize & Externalize
    bits = CAccPolHdmiSpeakerAllocation::EHdmiSpeakerFrontCenter |
        CAccPolHdmiSpeakerAllocation::EHdmiSpeakerRearCenter;
    obj = CAccPolHdmiSpeakerAllocation::NewLC( bits );
    RBufWriteStream writeStream;
    writeStream.Open( *buf );
    writeStream.PushL();
    obj->ExternalizeL( writeStream );
    writeStream.CommitL();
    CleanupStack::PopAndDestroy( &writeStream );
    CleanupStack::PopAndDestroy( obj );
    
    obj = CAccPolHdmiSpeakerAllocation::NewLC();
    RBufReadStream readStream;
    readStream.Open( *buf );
    readStream.PushL();
    objectType = readStream.ReadUint8L();
    obj->InternalizeL( readStream );
    if( objectType != EAccPolHdmiSpeakerAllocationObject )
        {
        err = KErrArgument;
        }
    if( !obj->FrontCenter() )
        {
        err = KErrArgument;
        }
    if( !obj->RearCenter() )
        {
        err = KErrArgument;
        }
    if( !( obj->SpeakerAllocation() & CAccPolHdmiSpeakerAllocation::EHdmiSpeakerFrontCenter ) )
        {
        err = KErrArgument;
        }
    if( !( obj->SpeakerAllocation() & CAccPolHdmiSpeakerAllocation::EHdmiSpeakerRearCenter ) )
        {
        err = KErrArgument;
        }
    CleanupStack::PopAndDestroy( &readStream );
    CleanupStack::PopAndDestroy( obj );
    
    // DuplicateLC - mem compare can be used since the object does not contain
    // any ponters as memebers
    bits = CAccPolHdmiSpeakerAllocation::EHdmiSpeakerFrontCenter |
        CAccPolHdmiSpeakerAllocation::EHdmiSpeakerRearCenter;
    obj = CAccPolHdmiSpeakerAllocation::NewLC( bits );
    objDuplicate = static_cast<CAccPolHdmiSpeakerAllocation*>( obj->DuplicateLC() );
    TUint8* left = reinterpret_cast<TUint8*>( obj );
    TUint8* right = reinterpret_cast<TUint8*>( objDuplicate );
    TInt size = sizeof( CAccPolHdmiSpeakerAllocation );
    if( Mem::Compare( left, size, right, size ) != KErrNone )
        {
        err = KErrArgument;
        }
    CleanupStack::PopAndDestroy( objDuplicate );
    CleanupStack::PopAndDestroy( obj );

    // Rest of the API
    bits = CAccPolHdmiSpeakerAllocation::EHdmiSpeakerFrontCenter |
        CAccPolHdmiSpeakerAllocation::EHdmiSpeakerRearCenter |
        CAccPolHdmiSpeakerAllocation::EHdmiSpeakerTopCenter;
    obj = CAccPolHdmiSpeakerAllocation::NewLC();
    obj->SetSpeakerAllocation( bits );
    if( !( obj->SpeakerAllocation() & CAccPolHdmiSpeakerAllocation::EHdmiSpeakerFrontCenter ) )
        {
        err = KErrArgument;
        }
    if( !( obj->SpeakerAllocation() & CAccPolHdmiSpeakerAllocation::EHdmiSpeakerRearCenter ) )
        {
        err = KErrArgument;
        }
    if( !( obj->SpeakerAllocation() & CAccPolHdmiSpeakerAllocation::EHdmiSpeakerTopCenter ) )
        {
        err = KErrArgument;
        }
    CleanupStack::PopAndDestroy( obj );

    obj = CAccPolHdmiSpeakerAllocation::NewLC();
    obj->SetFrontSpeakers( ETrue );
    if( !obj->FrontSpeakers() || obj->SpeakerCount() != 2 )
        {
        err = KErrArgument;
        }
    obj->SetFrontSpeakers( EFalse );
    if( obj->FrontSpeakers() || obj->SpeakerCount() != 0 )
        {
        err = KErrArgument;
        }
    obj->SetRearSpeakers( ETrue );
    if( !obj->RearSpeakers() || obj->SpeakerCount() != 2 )
        {
        err = KErrArgument;
        }
    obj->SetRearSpeakers( EFalse );
    if( obj->RearSpeakers() || obj->SpeakerCount() != 0 )
        {
        err = KErrArgument;
        }
    obj->SetLowFrequencyEffect( ETrue );
    if( !obj->LowFrequencyEffect() || obj->SpeakerCount() != 1 )
        {
        err = KErrArgument;
        }
    obj->SetLowFrequencyEffect( EFalse );
    if( obj->LowFrequencyEffect() || obj->SpeakerCount() != 0 )
        {
        err = KErrArgument;
        }
    obj->SetFrontCenter( ETrue );
    if( !obj->FrontCenter() || obj->SpeakerCount() != 1 )
        {
        err = KErrArgument;
        }
    obj->SetFrontCenter( EFalse );
    if( obj->FrontCenter() || obj->SpeakerCount() != 0 )
        {
        err = KErrArgument;
        }
    obj->SetFrontCenterHigh( ETrue );
    if( !obj->FrontCenterHigh() || obj->SpeakerCount() != 1 )
        {
        err = KErrArgument;
        }
    obj->SetFrontCenterHigh( EFalse );
    if( obj->FrontCenterHigh() || obj->SpeakerCount() != 0 )
        {
        err = KErrArgument;
        }
    obj->SetTopCenter( ETrue );
    if( !obj->TopCenter() || obj->SpeakerCount() != 1 )
        {
        err = KErrArgument;
        }
    obj->SetTopCenter( EFalse );
    if( obj->TopCenter() || obj->SpeakerCount() != 0 )
        {
        err = KErrArgument;
        }
    obj->SetRearCenter( ETrue );
    if( !obj->RearCenter() || obj->SpeakerCount() != 1 )
        {
        err = KErrArgument;
        }
    obj->SetRearCenter( EFalse );
    if( obj->RearCenter() || obj->SpeakerCount() != 0 )
        {
        err = KErrArgument;
        }
    obj->SetFrontLeftRightCenter( ETrue );
    if( !obj->FrontLeftRightCenter() || obj->SpeakerCount() != 2 )
        {
        err = KErrArgument;
        }
    obj->SetFrontLeftRightCenter( EFalse );
    if( obj->FrontLeftRightCenter() || obj->SpeakerCount() != 0 )
        {
        err = KErrArgument;
        }
    obj->SetRearLeftRightCenter( ETrue );
    if( !obj->RearLeftRightCenter() || obj->SpeakerCount() != 2 )
        {
        err = KErrArgument;
        }
    obj->SetRearLeftRightCenter( EFalse );
    if( obj->RearLeftRightCenter() || obj->SpeakerCount() != 0 )
        {
        err = KErrArgument;
        }
    obj->SetFrontWideSpeakers( ETrue );
    if( !obj->FrontWideSpeakers() || obj->SpeakerCount() != 2 )
        {
        err = KErrArgument;
        }
    obj->SetFrontWideSpeakers( EFalse );
    if( obj->FrontWideSpeakers() || obj->SpeakerCount() != 0 )
        {
        err = KErrArgument;
        }
    obj->SetFrontHighSpeakers( ETrue );
    if( !obj->FrontHighSpeakers() || obj->SpeakerCount() != 2 )
        {
        err = KErrArgument;
        }
    obj->SetFrontHighSpeakers( EFalse );
    if( obj->FrontHighSpeakers() || obj->SpeakerCount() != 0 )
        {
        err = KErrArgument;
        }
    CleanupStack::PopAndDestroy( obj );
    CleanupStack::PopAndDestroy( buf );
    
    return err;
    }

// ----------------------------------------------------------------------------------
// CTFAccessoryTestControl::CheckHdmiVideoFormatObjectL
// ----------------------------------------------------------------------------------   
TInt CTFAccessoryTestControl::CheckHdmiVideoFormatObjectL()
    {
    TInt err = KErrNone;
    CAccPolHdmiVideoFormat* obj = NULL;
    CAccPolHdmiVideoFormat* objDuplicate = NULL;
    TUint8 objectType = 0;
    CBufFlat* buf = CBufFlat::NewL( KAccSrvObjectBaseStreamBufGranularity );
    CleanupStack::PushL( buf );
    buf->ResizeL( KAccSrvObjectBaseStreamBufMaxSize );
    
    // NewL
    obj = CAccPolHdmiVideoFormat::NewL();
    CleanupStack::PushL( obj );
    CleanupStack::PopAndDestroy( obj );
    
    // NewLC
    obj = CAccPolHdmiVideoFormat::NewLC();
    CleanupStack::PopAndDestroy( obj );
    
    // NewL - with params
    obj = CAccPolHdmiVideoFormat::NewL( E640x480p59_94d60Hz4d3,
        E640x350p85HzRB,
        1,
        ETrue );
    CleanupStack::PushL( obj );
    if( obj->CeaFixedMode() != E640x480p59_94d60Hz4d3 )
        {
        err = KErrArgument;
        }
    if( obj->DmtFixedMode() != E640x350p85HzRB )
        {
        err = KErrArgument;
        }
    if( obj->PixelRepeat() != 1 )
        {
        err = KErrArgument;
        }
    if( !obj->Interlaced() )
        {
        err = KErrArgument;
        }
    CleanupStack::PopAndDestroy( obj );
    
    // NewLC - with params
    obj = CAccPolHdmiVideoFormat::NewLC( E640x480p59_94d60Hz4d3,
        E640x350p85HzRB,
        1,
        ETrue );
    if( obj->CeaFixedMode() != E640x480p59_94d60Hz4d3 )
        {
        err = KErrArgument;
        }
    if( obj->DmtFixedMode() != E640x350p85HzRB )
        {
        err = KErrArgument;
        }
    if( obj->PixelRepeat() != 1 )
        {
        err = KErrArgument;
        }
    if( !obj->Interlaced() )
        {
        err = KErrArgument;
        }
    CleanupStack::PopAndDestroy( obj );
    
    // Internalize & Externalize
    obj = CAccPolHdmiVideoFormat::NewLC( E640x480p59_94d60Hz4d3,
        E640x350p85HzRB,
        1,
        ETrue );
    RBufWriteStream writeStream;
    writeStream.Open( *buf );
    writeStream.PushL();
    obj->ExternalizeL( writeStream );
    writeStream.CommitL();
    CleanupStack::PopAndDestroy( &writeStream );
    CleanupStack::PopAndDestroy( obj );
    
    obj = CAccPolHdmiVideoFormat::NewLC();
    RBufReadStream readStream;
    readStream.Open( *buf );
    readStream.PushL();
    objectType = readStream.ReadUint8L();
    obj->InternalizeL( readStream );
    if( objectType != EAccPolHdmiVideoFormatObject )
        {
        err = KErrArgument;
        }
    if( obj->CeaFixedMode() != E640x480p59_94d60Hz4d3 )
        {
        err = KErrArgument;
        }
    if( obj->DmtFixedMode() != E640x350p85HzRB )
        {
        err = KErrArgument;
        }
    if( obj->PixelRepeat() != 1 )
        {
        err = KErrArgument;
        }
    if( !obj->Interlaced() )
        {
        err = KErrArgument;
        }
    CleanupStack::PopAndDestroy( &readStream );
    CleanupStack::PopAndDestroy( obj );
    
    // DuplicateLC - mem compare can be used since the object does not contain
    // any ponters as memebers
    obj = CAccPolHdmiVideoFormat::NewLC( E640x480p59_94d60Hz4d3,
        E640x350p85HzRB,
        1,
        ETrue );
    objDuplicate = static_cast<CAccPolHdmiVideoFormat*>( obj->DuplicateLC() );
    TUint8* left = reinterpret_cast<TUint8*>( obj );
    TUint8* right = reinterpret_cast<TUint8*>( objDuplicate );
    TInt size = sizeof( CAccPolHdmiVideoFormat );
    if( Mem::Compare( left, size, right, size ) != KErrNone )
        {
        err = KErrArgument;
        }
    CleanupStack::PopAndDestroy( objDuplicate );
    CleanupStack::PopAndDestroy( obj );

    // Rest of the API
    obj = CAccPolHdmiVideoFormat::NewLC();
    obj->SetCeaFixedMode( E640x480p59_94d60Hz4d3 );
    if( obj->CeaFixedMode() != E640x480p59_94d60Hz4d3 )
        {
        err = KErrArgument;
        }
    obj->SetDmtFixedMode( E640x350p85HzRB );
    if( obj->DmtFixedMode() != E640x350p85HzRB )
        {
        err = KErrArgument;
        }
    obj->SetPixelRepeat( 4 );
    if( obj->PixelRepeat() != 4 )
        {
        err = KErrArgument;
        }
    obj->SetInterlaced( ETrue );
    if( !obj->Interlaced() )
        {
        err = KErrArgument;
        }
    CleanupStack::PopAndDestroy( obj );
    CleanupStack::PopAndDestroy( buf );
    
    return err;
    }

//
// ----------------------------------------------------------------------------------
// CTFTestControlObserver* CTFAccessoryTestControl::CreateObserver()
// ----------------------------------------------------------------------------------	
CTFTestControlObserver* CTFAccessoryTestControl::CreateObserver()
    {
    COMPONENT_TRACE( ( _L( "ACCESSORYTESTCONTROL - CTFAccessoryTestControl::CreateObserver()" ) ) );
    iStack.AddFirst/*AddLast*/( *CTFTestControlObserver::NewL( *this ) );
    COMPONENT_TRACE( ( _L( "ACCESSORYTESTCONTROL - CTFAccessoryTestControl::CreateObserver() return" ) ) );
    return iStack.First/*Last*/();
    }

// Not used at this point.
// ----------------------------------------------------------------------------------
// CTFAccessoryTestControl::ProcessMessageAndCompleteL
// ----------------------------------------------------------------------------------	
void CTFAccessoryTestControl::ProcessMessageAndCompleteL( const RMessage2& aMessage )
    {
    COMPONENT_TRACE( ( _L( "ACCESSORYTESTCONTROL - CTFAccessoryTestControl::ProcessMessageAndCompleteL enter" ) ) );
    aMessage.Complete( KErrNone );
    COMPONENT_TRACE( ( _L( "ACCESSORYTESTCONTROL - CTFAccessoryTestControl::ProcessMessageAndCompleteL return" ) ) );
    }
//
// ----------------------------------------------------------------------------------
// CTFAccessoryTestControl::MessageCompleteL
// ----------------------------------------------------------------------------------	
void CTFAccessoryTestControl::MessageCompleteL( TInt /*aResult*/)
    {

    }

//
// ----------------------------------------------------------------------------------
// CTFAccessoryTestControl::CreateHdmiObjectL
// ----------------------------------------------------------------------------------   
void CTFAccessoryTestControl::CreateHdmiObjectL( CAccPolObjectCon& aCon,
    THdmiConObjectType aType )
    {
    switch( aType )
        {
        case EAccPolHdmiSinkObject:
            {
            CAccPolHdmiSink* sink = CAccPolHdmiSink::NewLC();
            sink->SetBasicAudioSupport( ETrue );
            sink->SetHdcpSupport( ETrue );
            aCon.AddL( *sink );
            CleanupStack::PopAndDestroy( sink );
            break;
            }
        case EAccPolHdmiAudioFormatObject:
            {
            CAccPolHdmiAudioFormat* audioFormat = CAccPolHdmiAudioFormat::NewLC();
            audioFormat->SetAudioFormat( HdmiAudioFormat::KUidFormatPCM16 );
            audioFormat->SetChannelCount( 2 );
            audioFormat->SetBitResolution( CAccPolHdmiAudioFormat::EBitsPerSample16 );
            audioFormat->SetMaxBitResolution( CAccPolHdmiAudioFormat::EBitsPerSample16 );
            audioFormat->SetFormatDependentValue( CAccPolHdmiAudioFormat::EBitsPerSample16 );
            audioFormat->SamFreq( CAccPolHdmiAudioFormat::ESamplingFreq32KHz );
            aCon.AddL( *audioFormat );
            CleanupStack::PopAndDestroy( audioFormat );
            break;
            }
        case EAccPolHdmiLatencyObject:
            {
            CAccPolHdmiLatency* latency = CAccPolHdmiLatency::NewLC();
            latency->SetLatencyType( HdmiLatency::KUidLatency );
            latency->SetAudioLatency( 100 );
            latency->SetVideoLatency( 200 );
            aCon.AddL( *latency );
            CleanupStack::PopAndDestroy( latency );
            break;
            }
        case EAccPolHdmiSpeakerAllocationObject:
            {
            CAccPolHdmiSpeakerAllocation* speakerAllocation =
                CAccPolHdmiSpeakerAllocation::NewLC();
            speakerAllocation->SetFrontSpeakers( ETrue );
            speakerAllocation->SetRearSpeakers( ETrue );
            speakerAllocation->SetLowFrequencyEffect( ETrue );
            speakerAllocation->SetFrontCenter( ETrue );
            speakerAllocation->SetFrontCenterHigh( ETrue );
            speakerAllocation->SetTopCenter( ETrue );
            speakerAllocation->SetRearCenter( ETrue );
            speakerAllocation->SetFrontLeftRightCenter( ETrue );
            speakerAllocation->SetRearLeftRightCenter( ETrue );
            speakerAllocation->SetFrontWideSpeakers( ETrue );
            speakerAllocation->SetFrontHighSpeakers( ETrue );
            aCon.AddL( *speakerAllocation );
            CleanupStack::PopAndDestroy( speakerAllocation );
            break;
            }
        case EAccPolHdmiVideoFormatObject:
            {
            CAccPolHdmiVideoFormat* videoFormat = CAccPolHdmiVideoFormat::NewLC();
            videoFormat->SetCeaFixedMode( E640x480p59_94d60Hz4d3 );
            videoFormat->SetDmtFixedMode( E640x480p85HzRB );
            videoFormat->SetInterlaced( EFalse );
            aCon.AddL( *videoFormat );
            CleanupStack::PopAndDestroy( videoFormat );
            break;
            }
        default:
            {
            break;
            }
        }
    }
    

void CTFAccessoryTestControl::AccessoryConnectionOpenClose(TTFAccessoryFunction aCase, TInt* aRetval)
    {
    switch(aCase)
        {
        case ETFAccessoryConnection_Open:
            {
            if ( iAccessoryConnection == NULL )
                {
                    iAccessoryConnection = new ( ELeave ) RAccessoryConnection;
                    *aRetval = iAccessoryConnection->CreateSubSession( iAccessoryServer );
                    COMPONENT_TRACE( ( _L( "CTFAccessoryTestControl::CallAccessoryFunctionL(TTFAccessoryTestCaseStateControl& aParameter) - ETFAccessoryConnection_Open - Sub session created - (%d)" ), *aRetval ) );
                 }
           else
                 {
                    *aRetval = KErrNone;
                    COMPONENT_TRACE( ( _L( "CTFAccessoryTestControl::CallAccessoryFunctionL(TTFAccessoryTestCaseStateControl& aParameter) - ETFAccessoryConnection_Open - Sub session not created - (%d)" ), *aRetval ) );
                 }
            }
            break;
        
        case ETFAccessoryConnection_Close:
            {
            TRACE_ASSERT( iAccessoryConnection != NULL );
                                
            if ( iAccessoryConnection != NULL )
                {
                     iAccessoryConnection->CloseSubSession();
                     delete iAccessoryConnection;
                     iAccessoryConnection = NULL;
                     COMPONENT_TRACE( ( _L( "CTFAccessoryTestControl::CallAccessoryFunctionL(TTFAccessoryTestCaseStateControl& aParameter) - ETFAccessoryConnection_Close - Sub session closed" ) ) );
                 }                               
             *aRetval = KErrNone;
            }
            break;
            
        default:
            break;
        }
      
    }

void CTFAccessoryTestControl::FindAndCheckRequestL( TInt* aRetval, TTFAccessoryTestCaseStateControl& aParameter )
    {
    CTFAccessoryTestCaseControl* testCase = STATIC_CAST( CTFAccessoryTestCaseControl*, CurrentTestCase());          
    TInt errVal = 0;
                
    TRACE_ASSERT( testCase != NULL );
    if ( testCase != NULL )
        {
           TEST_CASE_TRACE( ( _L( "CTFAccessoryTestControl::FindAndCheckRequest() - inside if ( testCase != NULL )" ) ) );
           if(!iStack.IsEmpty())
              {
                 iStackIter.SetToFirst(); 
            
              while ( ( iObserverItem = iStackIter++ ) != NULL )
                 {
                     if(iObserverItem->FindRequest(aParameter.iRequestType))
                        {
                           testCase->CheckRequest( iObserverItem->iStatus, 
                                                   iObserverItem->iValue, 
                                                   iObserverItem->iCapabilityName,
                                                   &errVal );  
                                        
                            if(errVal == KTFDontDeleteObserver)
                                {
                                aParameter.iArg1 = KTFDontDeleteObserver;
                                }
                                
                                        
                            if ( !iStack.IsEmpty() ) // Check if observes are already deleted by CheckRequest()
                                {
                                   if( KTFDontDeleteObserver != aParameter.iArg1 )
                                      {
                                         if (iObserverItem->iStatus == KRequestPending )
                                             {
                                             User::WaitForRequest(iObserverItem->iStatus);
                                             }
                                         iStack.Remove( *iObserverItem );
                                         delete iObserverItem;
                                      }
                                   else
                                       {
                                          User::Leave( KTFDontDeleteObserver );
                                       }
                                 }
                            else
                                 {
                                     *aRetval = KErrGeneral;
                                 }
                          }                               
                  }
        }
    else
        {
            *aRetval = KErrGeneral;
        }
    }
          
    TEST_CASE_TRACE( ( _L( "CTFAccessoryTestControl::FindAndCheckRequest() returning and setting *aRetval = KErrNone )" ) ) );
   *aRetval = KErrNone;
    }


void CTFAccessoryTestControl::AccessorySettingsOpenClose( TTFAccessoryFunction aCase, TInt* aRetval )
    {
        switch(aCase)
            {
            case ETFAccessorySettings_Open:
                {
                if ( iAccessorySettings == NULL )
                    {
                         iAccessorySettings = new ( ELeave ) RAccessorySettings;
                         *aRetval = iAccessorySettings->CreateSubSession( iAccessoryServer );
                     }
                else
                         *aRetval = KErrNone;
                }
                break;
                
            case ETFAccessorySettings_Close:
                {
                    TRACE_ASSERT( iAccessorySettings != NULL );
                            
                    if ( iAccessorySettings != NULL )
                        {
                            iAccessorySettings->CloseSubSession();
                            delete iAccessorySettings;
                            iAccessorySettings = NULL;
                        }                                
                    *aRetval = KErrNone;
                }
                break;
                
            default:
                break;
            
            }
    }

void CTFAccessoryTestControl::SetSeveralAccessoryModeSetting( TInt* aRetval )
    {
        TAccModeSetting accModeSettingIn;
        TAccMode tAccMode = EAccModeWirelessHeadset;
        TBuf< KMaxAccModeSetting > aDesIn; 
        // Set
        // setting 1
        accModeSettingIn.SetId( 11 ); // unknown TInt
        accModeSettingIn.SetTInt( 1 );
        TAccModeSettingArray arrayIn;
        arrayIn.AddSetting( accModeSettingIn );
                
        // setting 2
        accModeSettingIn.SetId( 12 );   // unknown Des
        _LIT( KAccSetting2, "012345678901234567890" );
        aDesIn.Copy( KAccSetting2 );
        accModeSettingIn.SetTDes( aDesIn );
        arrayIn.AddSetting( accModeSettingIn );
                
        // setting 3
        accModeSettingIn.SetId( 13 ); // unknown TBool
        accModeSettingIn.SetTBool( ETrue );
        arrayIn.AddSetting( accModeSettingIn );
        
        *aRetval = iAccessorySettings->SetAccessoryModeSettingsL( tAccMode, arrayIn );
    }

void CTFAccessoryTestControl::GetSeveralAccessoryModeSetting( TInt* aRetval )
    {
    TAccModeSetting accModeSettingOut;
    TAccMode tAccMode = EAccModeWirelessHeadset;
    TBuf< KMaxAccModeSetting > aDesOut; 
    TAccModeSettingArray arrayOut;
    TInt intOut( 0 );
    // set input parameters for GetAccessoryModeSettings()
    // setting 11
    accModeSettingOut.SetId( 11 );
    arrayOut.AddSetting( accModeSettingOut );

    // setting 12
    accModeSettingOut.SetId( 12 );
    arrayOut.AddSetting( accModeSettingOut );

    // setting 13
    accModeSettingOut.SetId( 13 );
    arrayOut.AddSetting( accModeSettingOut );
                
    *aRetval = iAccessorySettings->GetAccessoryModeSettings( tAccMode, arrayOut );
                
    // Check that got settings are right
                
    TInt count = arrayOut.GetArraySize();
    TInt foundCount(0);
                
    for( TInt index = 0; index < count; index++ )
        {
            arrayOut.GetSetting( index, accModeSettingOut );
                        
            if( 11 == accModeSettingOut.Id() ) 
                {
                            
                accModeSettingOut.GetTInt( intOut);
                            
                if( 1 == intOut )
                    {
                        foundCount++;    
                    }
                 }
                            
                            
             if( 12 == accModeSettingOut.Id() ) 
                 {
                     accModeSettingOut.GetTDes( aDesOut );
                
                     if( KErrNone == aDesOut.Compare( _L("012345678901234567890") ) )
                     {
                         foundCount++;
                      }
                            
                  }
                            
              if( 13 == accModeSettingOut.Id() ) 
                  {
                            
                      accModeSettingOut.GetTBool( intOut );
                            
                      if( intOut )
                          {
                              foundCount++;    
                          }
                  } 
        }

        if(foundCount != 3)
            {
                *aRetval = KErrNotFound;
            }
                       
        //
        // Reset Settings
        //
        TAccModeSetting accModeSettingIn;
        TBuf< KMaxAccModeSetting > aDesIn; 
        // Set
        // setting 1
        accModeSettingIn.SetId( 11 ); // unknown TInt
        accModeSettingIn.SetTInt( 0 );
        TAccModeSettingArray arrayIn;
        arrayIn.AddSetting( accModeSettingIn );
                    
        // setting 2
        accModeSettingIn.SetId( 12 );   // unknown Des
        _LIT( KAccSetting2, "" );
        aDesIn.Copy( KAccSetting2 );
        accModeSettingIn.SetTDes( aDesIn );
        arrayIn.AddSetting( accModeSettingIn );
                    
        // setting 3
        accModeSettingIn.SetId( 13 ); // unknown TBool
        accModeSettingIn.SetTBool( EFalse );
        arrayIn.AddSetting( accModeSettingIn );
                    
        iAccessorySettings->SetAccessoryModeSettingsL( tAccMode, arrayIn );
    
    }

void CTFAccessoryTestControl::SetIntValue( TTFAccessoryTestCaseStateControl& aParameter, TInt *aRetval )
    {
        switch( aParameter.iArg3 )
               {
               case KAccSetVolumeLevel:
                   {
                   //AccPolCommonNameValuePairs.h contains following example code  
                   RAccessoryControl accessoryControl;
                   accessoryControl.CreateSubSession( iAccessoryServer );
                            
                   CAccPolSubblockNameArray* nameArray = CAccPolSubblockNameArray::NewL();  
                   CleanupStack::PushL( nameArray );      
                   accessoryControl.GetSubblockNameArrayL( iGenericId[aParameter.iGid], *nameArray );
            
                   TAccPolNameRecord nameRecord;
                   nameRecord.SetNameL( KAccSetVolumeLevel );                      
                   TAccValueTypeTInt value;
                        
                   if( nameArray->HasName( KAccSetVolumeLevel ) )
                       {
                       accessoryControl.GetValueL( iGenericId[aParameter.iGid], nameRecord, value );                                               
                       value.iValue = value.iValue | aParameter.iArg1;                
                       accessoryControl.AccessoryValueChangedNotifyL( iGenericId[aParameter.iGid], nameRecord, value );
                   }
                                
                   CleanupStack::PopAndDestroy( nameArray );
                   accessoryControl.CloseSubSession();
                   *aRetval = KErrNone;
                   }
                   break;
            
               default:
                   {
                   TAccPolNameRecord aNameRecord;
                   aNameRecord.SetNameL( aParameter.iArg3 );                       
                   TAccValueTypeTInt aIntValue;         
                   aIntValue.iValue = aParameter.iArg1;
                   *aRetval = iAccessoryControl->SetValueL( iGenericId[aParameter.iGid], aNameRecord, aIntValue );
                   }
                   break;
               }
    
    }

void CTFAccessoryTestControl::SelectionDialog( TTFAccessoryFunction aCase, TInt* aRetval )
    {
        switch(aCase)
            {
            case ETFSelectionDialogText:
                {
                    const TUid KAccFwUiDialogNotifierUid = { 0x10205062 };
                        
                    RNotifier notifier;
                    notifier.Connect();
                    
                    TInt selectionListBitmask = 255;
                    TInt reply = 0;
                    
                    TPckg<TInt> replyPck( reply);
                    TPckg<TUint32> selectionBitmaskPackage( selectionListBitmask);
        
                    //activate active object
                    TRequestStatus status;
                    status = KRequestPending;
                    
                    //open ui
                    notifier.StartNotifierAndGetResponse( status, KAccFwUiDialogNotifierUid, selectionBitmaskPackage, replyPck);
                    
                    User::WaitForRequest( status);
                    
                    
                    notifier.Close();
                    
                    *aRetval = KErrNone;
                }
 
            case ETFSelectionDialogCancel:
                {
                    const TUid KAccFwUiDialogNotifierUid = { 0x10205062 };
                            
                    RNotifier notifier;
                    notifier.Connect();
                    
                    TInt selectionListBitmask = 7;
                    TInt reply = 0;
                    
                    TPckg<TInt> replyPck( reply);
                    TPckg<TUint32> selectionBitmaskPackage( selectionListBitmask);
        
                    //activate active object
                    TRequestStatus status;
                    
                    //open ui
                    notifier.StartNotifierAndGetResponse( status, KAccFwUiDialogNotifierUid, selectionBitmaskPackage, replyPck);
                    User::After( 2500000);
                    notifier.CancelNotifier( KAccFwUiDialogNotifierUid);
                    
                    // Implementation of the dialog is changed. Request is completed every time
                    User::WaitForRequest( status );
                    
                    notifier.Close();
                    
                    *aRetval = KErrNone;
                }
            
            default:
                break;
                
            }
    
    }

void CTFAccessoryTestControl::CheckConnectedAccessory( TInt* aRetval )
    {
        *aRetval = KErrNone;
                    
        TAccPolGenericID            genID;                                  // TGeneric ID that should be connected
        TAccPolGenericID            genIDConnected;                         // TGeneric ID that is connected
        TAccPolGIDHeader            iGenericIDHeader;                       // Header of the GID that should be connected
        TAccPolGIDHeader            iGenericIDHeaderConnected;              // Header of the connected GID
        TAccPolGenericIDArray       genericIdArray;                         // Array of connected generic ID's
        CAccConGenericID*           aGenericID = CAccConGenericID::NewL();  // This might be needed for capabilities 
        CAccPolSubblockNameArray*   nameArrayConnected = CAccPolSubblockNameArray::NewL();
        
        CleanupStack::PushL( aGenericID );                        
        CleanupStack::PushL( nameArrayConnected );
        
        TAccPolGenericID genericID = aGenericID->GenericID();

        TBuf<KHWModelIDMaxLength>      hwModelID(_L("headset"));
        // Set GenericID header
        iGenericIDHeader.iAccessoryDeviceType   = KDTHeadset;
        iGenericIDHeader.iPhysicalConnection    = KPCWired;
        iGenericIDHeader.iApplicationProtocol   = 0x0;
    
        iGenericIDHeader.iCapabilitiesSubblocks = KSBAudioSubblock;   
    
        iGenericIDHeader.iHWModelID             = hwModelID;
        iGenericIDHeader.iHWDeviceID            = 0x0;
        iGenericIDHeader.iDBID                    = 0x0;
                                           
        RAccessoryConnection* accessoryConnection = new ( ELeave ) RAccessoryConnection();
        CleanupStack::PushL( accessoryConnection );
        accessoryConnection->CreateSubSession( iAccessoryServer );
        accessoryConnection->GetAccessoryConnectionStatus( genericIdArray );
        CleanupStack::PopAndDestroy( accessoryConnection );
        
        genIDConnected = genericIdArray.GetGenericIDL( 0 );
        
        RAccessorySingleConnection singleConnect;// = new ( ELeave ) RAccessoryConnectionBase();
        singleConnect.CreateSubSession( iAccessoryServer );
        singleConnect.GetSubblockNameArrayL( genIDConnected, *nameArrayConnected );
        
        singleConnect.CloseSubSession();
        
        iGenericIDHeaderConnected.iAccessoryDeviceType = genIDConnected.DeviceTypeCaps();
        iGenericIDHeaderConnected.iPhysicalConnection = genIDConnected.PhysicalConnectionCaps();
        iGenericIDHeaderConnected.iApplicationProtocol = genIDConnected.ApplicationProtocolCaps();
        iGenericIDHeaderConnected.iCapabilitiesSubblocks = genIDConnected.SubblockCaps();
        iGenericIDHeaderConnected.iHWModelID = genIDConnected.HWModelID();
        iGenericIDHeaderConnected.iHWDeviceID = genIDConnected.HWDeviceID();
        iGenericIDHeaderConnected.iDBID = 0x0;

        //Check GenericID header
        if( iGenericIDHeader.iAccessoryDeviceType != iGenericIDHeaderConnected.iAccessoryDeviceType )
        {
            *aRetval = EFalse;
        }
        if( iGenericIDHeader.iPhysicalConnection != iGenericIDHeaderConnected.iPhysicalConnection )
        {
            *aRetval = EFalse;
        }
        if( iGenericIDHeader.iApplicationProtocol != iGenericIDHeaderConnected.iApplicationProtocol )
        {
            *aRetval = EFalse;
        }
        if( iGenericIDHeader.iCapabilitiesSubblocks != iGenericIDHeaderConnected.iCapabilitiesSubblocks )
        {
            *aRetval = EFalse;
        }
        if( iGenericIDHeader.iHWModelID != iGenericIDHeaderConnected.iHWModelID )
        {
            *aRetval = EFalse;
        }
        if( iGenericIDHeader.iHWDeviceID != iGenericIDHeaderConnected.iHWDeviceID )
        {
            *aRetval = EFalse;
        }
        if( !( nameArrayConnected->HasName( KAccStereoAudio ) ) )
        {
            *aRetval = EFalse;
        }

        if( !( nameArrayConnected->HasName( KAccAudioOutConnector ) ) )
        {
            *aRetval = EFalse;
        }

        if( !( nameArrayConnected->HasName( KAccAudioOutputType ) ) )
        {
            *aRetval = EFalse;
        }
        //CleanupStack::PopAndDestroy( nameValueArray );
        CleanupStack::PopAndDestroy( nameArrayConnected );
        CleanupStack::PopAndDestroy( aGenericID );

    }


void CTFAccessoryTestControl::CallTClassMethods( TInt* aRetval )
    {
     //   Settings();
        Policy();
      *aRetval = KErrNone;
    }

void CTFAccessoryTestControl::Settings()
    {
    TAccModeSettingArray array;
    array.Reset();
    TInt ret = array.GetMaxArraySize();
    TAccModeSetting setting;
    TUint32 settingId( 1 );
    setting.SetId( settingId );
    array.AddSetting( setting );
    TAccModeSetting setting2;
    ret = array.GetSetting( 0, setting2 );
    ret = array.GetArraySize();
    }

void CTFAccessoryTestControl::Policy()
    {
    TInt valueInt( 1 );
    TUint32 valueUInt( 1 );
    TDesC8 des( _L8( "timo" ) );
    //TDes8 valueDesc = des;
    TAccPolNameRecord nameRecord;
    TAccPolValueRecord valRec( des, EAPVPolicy );
    TAccPolNameValueRecord nameValRec;
    TAccPolNameValueRecord nameValRec1( nameRecord, valRec );
    TAccPolNameValueRecord nameValRec2( valueUInt, valueInt, EAPVInt, EAPVPolicy );
    TAccPolNameValueRecord nameValRec3( valueUInt, des, EAPVPolicy );
    TAccPolNameValueRecord nameValRec4( valueUInt );
    //TDesC8 valueToGet;
    valRec.GetValueL( des );
    valRec.SetValue( des );
    //valRec.SetValue( valueToGet );
    TAccPolValueRecord valRec1( valueInt, EAPVInt, EAPVPolicy );
    valRec1.SetValue( valueInt );
    valRec1.SetLocation( EAPVPolicy );
    valRec1.SetL( valueInt, EAPVInt, EAPVPolicy );
    }

void CTFAccessoryTestControl::GetBoolAccessoryModeSetting( TTFAccessoryTestCaseStateControl& aParameter, TInt* aRetval )
    {
        TBool aSettingsValueBool;
                    
        TAccMode aAccMode = (TAccMode)aParameter.iArg1;
        TAccModeSetting aSetting;
        aSetting.SetId(aParameter.iArg2);
        
        *aRetval = iAccessorySettings->GetAccessoryModeSetting(aAccMode, aSetting);
        
        if( aSetting.Type() == EAccModeSettingBool) 
            {
                *aRetval = aSetting.GetTBool(aSettingsValueBool);
                
                if(*aRetval != KErrNotFound)
                {
                    if(aSettingsValueBool == (TBool)aParameter.iArg4)  
                    {
                        *aRetval = KErrNone;
                    }
                    else
                    {
                        *aRetval = KErrArgument;
                    }
                }
            }
            else
            {
                *aRetval = KErrNotFound;
            }
    }

void CTFAccessoryTestControl::GetIntAccessoryModeSetting( TTFAccessoryTestCaseStateControl& aParameter, TInt* aRetval )
    {
        TInt aSettingsValueInt;
                    
        TAccMode aAccMode = (TAccMode)aParameter.iArg1;
        TAccModeSetting aSetting;
        aSetting.SetId(aParameter.iArg2);
        
        *aRetval = iAccessorySettings->GetAccessoryModeSetting(aAccMode, aSetting);
        
        if(aSetting.Type() == EAccModeSettingInt)  
            {
                *aRetval = aSetting.GetTInt(aSettingsValueInt);
                
                if(*aRetval != KErrNotFound)
                {
                    if( aSettingsValueInt == aParameter.iArg4 )  
                    {
                        *aRetval = KErrNone;
                    }
                    else
                    {
                        *aRetval = KErrArgument;
                    }
                }
            }
            else
            {
                *aRetval = KErrNotFound;
            }
    }

void CTFAccessoryTestControl::GetDesAccessoryModeSetting( TTFAccessoryTestCaseStateControl& aParameter, TInt* aRetval )
    {
        TBuf<KMaxAccModeSetting> aSettingsValueDes;
                    
        TAccMode aAccMode = (TAccMode)aParameter.iArg1;
        TAccModeSetting aSetting;
        aSetting.SetId( aParameter.iArg2 );
        
        *aRetval = iAccessorySettings->GetAccessoryModeSetting( aAccMode, aSetting );
        
        if( aSetting.Type() == EAccModeSettingDes ) 
            {
                *aRetval = aSetting.GetTDes( aSettingsValueDes );
                
                if(*aRetval != KErrNotFound)
                {
                    TBuf<KMaxAccModeSetting> buffer;
                    buffer.Num( aParameter.iArg3 );
                    if( KErrNone == aSettingsValueDes.Compare( buffer ) )  
                    {
                        *aRetval = KErrNone;
                    }
                    else
                    {
                        *aRetval = KErrArgument;
                    }
                }
            }
            else
            {
                *aRetval = KErrNotFound;
            }
    }    

// End of file

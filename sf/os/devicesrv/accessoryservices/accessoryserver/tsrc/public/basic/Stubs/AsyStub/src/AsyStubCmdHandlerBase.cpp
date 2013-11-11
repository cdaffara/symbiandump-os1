/*
 * Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
#include "ASYStubCmdHandlerBase.h"
#include "AsyStubMainServiceBase.h"
#include "siftrace.h"
#include "bttypes.h"
#include "AsyStubCmdObserver.h"
#include "../../../../../../inc/common/AccPolSubblockNameArrayAccessor.h"

#include <AccessorySingleConnection.h>
#include <AccPolSubblockNameArray.h>
#include <e32property.h>
#include <accpolobjectcon.h>

#include "../../../../../../inc/common/AccClientServerMessages.h"
#include <accpolaudiotopologyobjectcon.h>
#include <accpolobjecttypes.h>
#include <accpolaudiostreamformatcon.h>
#include <accpolterminalbase.h>
#include <accpoloutputterminal.h>
#include <accpolinputterminal.h>
#include <accpolmixerunit.h>
#include <accpolfeatureunit.h>
#include <accpolvolumecontrol.h>
#include <accpolmutecontrol.h>
#include <accpolselectorunit.h>
#include <accpolhdmiobjectcon.h>

//
// ----------------------------------------------------------------------------------
// CASYStubCmdHandlerBase::CASYStubCmdHandlerBase()
// ----------------------------------------------------------------------------------
//  
CASYStubCmdHandlerBase::CASYStubCmdHandlerBase() :
    iTimer( NULL ), iResponse( 0 ), iResponseType( EAPVNone )
    {
    COMPONENT_TRACE(
        ( _L( "ASYStub - CASYStubCmdHandlerBase::CASYStubCmdHandlerBase() - Enter" ) ) );

    COMPONENT_TRACE(
        ( _L( "ASYStub - CASYStubCmdHandlerBase::CASYStubCmdHandlerBase() - Return" ) ) );
    }

// -----------------------------------------------------------------------------
// CASYStubCmdHandlerBase::ConstructL(
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CASYStubCmdHandlerBase::ConstructL()
    {
    COMPONENT_TRACE( ( _L( "ASYStub - CASYStubCmdHandlerBase::ConstructL()" ) ) );

    iCmdHandler = CAsyStubCmdObserver::NewL( this );

    COMPONENT_TRACE( ( _L( "ASYStub - CASYStubCmdHandlerBase::ConstructL - return void" ) ) );
    }

//
// ----------------------------------------------------------------------------------
// CASYStubCmdHandlerBase::NewL() 
// ----------------------------------------------------------------------------------
//  
// Two-phased constructor.
CASYStubCmdHandlerBase* CASYStubCmdHandlerBase::NewL()
    {
    CASYStubCmdHandlerBase* self = new ( ELeave ) CASYStubCmdHandlerBase();

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

//
// ----------------------------------------------------------------------------------
// CASYStubCmdHandlerBase::~CASYStubCmdHandlerBase()
// ----------------------------------------------------------------------------------
//  
// Destructor.
CASYStubCmdHandlerBase::~CASYStubCmdHandlerBase()
    {
    if( iCmdHandler )
        {
        delete iCmdHandler;
        }

    if( iTimer )
        {
        iTimer->Cancel();
        delete iTimer;
        }
    }
//
// ----------------------------------------------------------------------------------
// CASYStubCmdHandlerBase::ProcessCommandL()
// ----------------------------------------------------------------------------------
// 
void CASYStubCmdHandlerBase::ProcessCommandL( const TProcessCmdId aCommand,
    const TASYCmdParams& aCmdParams )
    {

    COMPONENT_TRACE( ( _L( "ASYStub - CASYStubCmdHandlerBase::ProcessCommandL() - Enter" ) ) );

    CASYMainServiceBase* mainServiceBase = ASYMainServiceBase();
    COMPONENT_TRACE(
        ( _L( "AsyStub - CASYStubCmdHandlerBase::ProcessCommandL - mainServiceBase (0x%x)" ), &mainServiceBase ) );

    TASYCommandParamRecord cmdParams( aCmdParams() );//For debugging

    if( aCommand == ECmdGetObjectValue || aCommand == ECmdSetObjectValue )
        {
        ProcessObjectL( aCommand, aCmdParams );
        }
    else
        {
        CASYStubMainServiceBase* AsyStub =
            reinterpret_cast<CASYStubMainServiceBase*> ( mainServiceBase );

        COMPONENT_TRACE(
            ( _L( "AsyStub - CASYStubCmdHandlerBase::ProcessCommandL - AsyStub (0x%x)" ), &AsyStub ) );
        AsyStub->AsyStubMainServicePointerReceivedL();
        COMPONENT_TRACE( ( _L( "ASYStub - CASYStubCmdHandlerBase::ProcessCommandL() - Return" ) ) );

        TInt aTestCaseID = AsyStub->GetTestCaseID(); // Do something with this
        TTFCapability aCapabilityS = AsyStub->GetAndRemoveCapability();
        switch( aCommand )
            {
            case ECmdGetValueBool:
                {

                TUint32 name;
                aCmdParams().iNameRecord.GetName( name );
                TAccValueTypeTBool aCmdResponse;
                aCmdResponse.iValue = EFalse;

                if( name == aCapabilityS.iCapability )
                    {
                    if( 0 != aCapabilityS.iTimeMs )
                        {
                        //Serve asynchronously       	            
                        Start( aCapabilityS.iTimeMs,
                            aCapabilityS.iValue,
                            EAPVBool );
                        }
                    else
                        {
                        aCmdResponse.iValue = aCapabilityS.iValue;
                        ProcessResponseL( aCmdResponse );
                        }
                    }
                else
                    {
                    TRACE_ASSERT_ALWAYS;
                    ProcessResponseL( aCmdResponse, KErrArgument );
                    }
                }
                break;

            case ECmdGetValueTDes8:
                {

                // Worm is fired through the Accessory Server.
                TBuf8<80>
                    worm( _L8("Malicious Worm Attach with extra long data with extra long content" ) );

                ProcessResponseL( worm, KErrNone );
                }
                break;
            case ECmdSetValueBool:
                {
                TUint32 name;
                aCmdParams().iNameRecord.GetName( name );
                TAccValueTypeTBool aCmdResponse;
                aCmdResponse.iValue = EFalse;

                if( name == aCapabilityS.iCapability && aCmdParams().iCmdValue
                    == aCapabilityS.iValue )
                    {
                    if( aCapabilityS.iTimeMs )
                        {
                        //Serve asynchronously       	            
                        Start( aCapabilityS.iTimeMs,
                            aCapabilityS.iValue,
                            EAPVBool );
                        }
                    else
                        {
                        aCmdResponse.iValue = aCmdParams().iCmdValue;
                        ProcessResponseL( aCmdResponse );
                        }
                    }
                else
                    {
                    TRACE_ASSERT_ALWAYS;
                    ProcessResponseL( aCmdResponse, KErrArgument );
                    }
                }
                break;

            case ECmdProcessCommandInit:
                {
                TAccValueTypeTBool boolInitResponse;
                TBool everyThing( ETrue );

                // If everything is ok
                if( everyThing )
                    {
                    COMPONENT_TRACE(
                        ( _L( "ASYStub - CASYStubCmdHandlerBase::ProcessCommandL() - everything is ok" ) ) );
                    boolInitResponse.iValue = ETrue;
                    ProcessResponseL( boolInitResponse );
                    }

                // If everything is not ok
                else
                    {
                    COMPONENT_TRACE(
                        ( _L( "ASYStub - CASYStubCmdHandlerBase::ProcessCommandL() - everything is not ok" ) ) );
                    boolInitResponse.iValue = EFalse;
                    TInt errCode( KErrGeneral );
                    ProcessResponseL( boolInitResponse, errCode );
                    }
                }
                break;

                //for user selection....
            case ECmdAccessoryUpdated:
                {
                TAccValueTypeTBool boolInitResponse;
                TBool everyThing( ETrue );

                //get capabilities
                CAccPolSubblockNameArray* iNameArray =
                    CAccPolSubblockNameArray::NewL();

                RAccessoryServer server;
                server.Connect();

                RAccessorySingleConnection connectionBase;
                connectionBase.CreateSubSession( server );
                connectionBase.GetSubblockNameArrayL( cmdParams.iGenericID,
                    *iNameArray );

                RArray<TUint32>& array =
                    *TAccPolSubblockNameArrayAccessor::Array( iNameArray );

                for( TInt i( 0 ); i < array.Count(); i++ )
                    {
                    TUint32 name = array[i];
                    TAccPolNameRecord nameRecord;
                    nameRecord.SetNameL( name );
                    TAccValueTypeTInt value;
                    TRAPD( err, connectionBase.GetValueL( cmdParams.iGenericID, nameRecord, value));

                    if( err != KErrNone )
                        {
                        COMPONENT_TRACE(
                            ( _L( "ASYStub - CASYStubCmdHandlerBase::ProcessCommandL() - Capability 0x%x has no value!"), name ) );
                        }

                    name = 0;
                    }

                connectionBase.CloseSubSession();
                server.Close();

                delete iNameArray;

                // If everything is ok
                if( everyThing )
                    {
                    COMPONENT_TRACE(
                        ( _L( "ASYStub - CASYStubCmdHandlerBase::ProcessCommandL() - everything is ok" ) ) );
                    boolInitResponse.iValue = ETrue;
                    ProcessResponseL( boolInitResponse );
                    }

                // If everything is not ok
                else
                    {
                    COMPONENT_TRACE(
                        ( _L( "ASYStub - CASYStubCmdHandlerBase::ProcessCommandL() - everything is not ok" ) ) );
                    boolInitResponse.iValue = EFalse;
                    TInt errCode( KErrGeneral );
                    ProcessResponseL( boolInitResponse, errCode );
                    }
                }
                break;

            default:
                {
                COMPONENT_TRACE(
                    ( _L( "BTASYStub - CBTBTASYStubCmdHandlerBase::ProcessCommandL() - ERROR: unhandled command" ) ) );
                TRACE_ASSERT_ALWAYS;
                User::Panic( _L("ASY Stub"), KErrGeneral );
                }
                break;
            }
        }
    }
void CASYStubCmdHandlerBase::ProcessObjectL( const TProcessCmdId aCommand,
    const TASYCmdParams& aCmdParams )
    {
    COMPONENT_TRACE(
        ( _L( "ASYStub - CASYStubCmdHandlerBase::ProcessCommandL() - CAccPolObjectBase" ) ) );

    RBufReadStream rStrm;

    TUint32 name;
    aCmdParams().iNameRecord.GetName( name );

    if( name == KAccAudioStreamsFormat )
        {
        switch( aCommand )
            {
            case ECmdGetObjectValue:
                {

                CAccPolAudioStreamFormatCon* container1 =
                    CAccPolAudioStreamFormatCon::NewLC();
                CAccPolAudioStreamFormatCon* container2 =
                    CAccPolAudioStreamFormatCon::NewLC();

                RProperty prop;
                CBufFlat* buf =
                    CBufFlat::NewL( KAccSrvObjectBaseStreamBufGranularity );
                CleanupStack::PushL( buf );
                buf->ResizeL( KAccSrvObjectBaseStreamBufMaxSize );
                TPtr8 ptr( buf->Ptr( 0 ) );
                TInt retval = prop.Get( KTFAccessoryTestProperty,
                    KTFAccessoryObjectMethod,
                    ptr );
                ( void )rStrm.Open( *buf );
                rStrm.PushL();
                container1->InternalizeL( rStrm );
                rStrm.Close();
                rStrm.Pop();

                for( TInt i = 0; i < container1->Count(); i++ )
                    {
                    container2->AddL( *container1->AtL( i ) );
                    }

                ProcessResponseL( *container2 );
                CleanupStack::PopAndDestroy( buf );
                CleanupStack::PopAndDestroy( container2 );
                CleanupStack::PopAndDestroy( container1 );

                }
                break;

            case ECmdSetObjectValue:
                {
                CAccPolAudioStreamFormatCon* container1 =
                    CAccPolAudioStreamFormatCon::NewLC();
                CAccPolAudioStreamFormatCon* container2 =
                    CAccPolAudioStreamFormatCon::NewLC();

                RProperty prop;
                CBufFlat* buf =
                    CBufFlat::NewL( KAccSrvObjectBaseStreamBufGranularity );
                CleanupStack::PushL( buf );
                buf->ResizeL( KAccSrvObjectBaseStreamBufMaxSize );
                TPtr8 ptr( buf->Ptr( 0 ) );
                TInt retval = prop.Get( KTFAccessoryTestProperty,
                    KTFAccessoryObjectMethod,
                    ptr );
                ( void )rStrm.Open( *buf );
                rStrm.PushL();
                container1->InternalizeL( rStrm );
                rStrm.Close();
                rStrm.Pop();

                // Read objects from proxy	     	
                ObjectConDataFromProxyL( *container2 );
                //externalize container to bufFromServer        
                CBufFlat* bufFromServer =
                    CBufFlat::NewL( KAccSrvObjectBaseStreamBufGranularity );
                CleanupStack::PushL( bufFromServer );
                bufFromServer->ResizeL( KAccSrvObjectBaseStreamBufMaxSize );
                RBufWriteStream wStrm;
                ( void )wStrm.Open( *bufFromServer );
                wStrm.PushL();
                container2->ExternalizeL( wStrm );
                wStrm.CommitL();
                wStrm.Close();
                wStrm.Pop();
                TPtr8 ptrbufFromServer( bufFromServer->Ptr( 0 ) );

                //bufFromServer content must equal to buf content
                TInt
                    err =
                        ( 0
                            == memcompare( reinterpret_cast<TUint8*> ( &ptrbufFromServer ),
                                sizeof( bufFromServer ),
                                reinterpret_cast<TUint8*> ( &ptr ),
                                sizeof( bufFromServer ) ) ) ? KErrNone
                            : KErrGeneral;

                ProcessResponseL( *container2, err ); // same container which was received from server


                CleanupStack::PopAndDestroy( bufFromServer );
                CleanupStack::PopAndDestroy( buf );
                CleanupStack::PopAndDestroy( container2 );
                CleanupStack::PopAndDestroy( container1 );

                }
                break;

            default:
                {
                COMPONENT_TRACE(
                    ( _L( "BTASYStub - CBTBTASYStubCmdHandlerBase::ProcessCommandL() - CAccPolObjectBase - ERROR: unhandled command" ) ) );
                TRACE_ASSERT_ALWAYS;
                User::Panic( _L("ASY Stub"), KErrGeneral );
                }
                break;
            }
        }
    else if( name == KAccAudioUnits )
        {

        CAccPolAudioTopologyObjectCon* container1 =
            CAccPolAudioTopologyObjectCon::NewLC();
        CAccPolAudioTopologyObjectCon* container2 =
            CAccPolAudioTopologyObjectCon::NewLC();

        RProperty prop;
        CBufFlat* buf = CBufFlat::NewL( KAccSrvObjectBaseStreamBufGranularity );
        CleanupStack::PushL( buf );
        buf->ResizeL( KAccSrvObjectBaseStreamBufMaxSize );
        TPtr8 ptr( buf->Ptr( 0 ) );
        TInt retval = prop.Get( KTFAccessoryTestProperty,
            KTFAccessoryObjectMethod,
            ptr );
        ( void )rStrm.Open( *buf );
        rStrm.PushL();
        container1->InternalizeL( rStrm );
        rStrm.Close();
        rStrm.Pop();

        for( TInt i = 0; i < container1->Count(); i++ )
            {
            container2->AddL( *container1->AtL( i ) );
            }

        ProcessResponseL( *container2 );

        CleanupStack::PopAndDestroy( buf );
        CleanupStack::PopAndDestroy( container2 );
        CleanupStack::PopAndDestroy( container1 );

        }
    else if( name == KAccAudioControl )
        {
        if( aCommand == ECmdSetObjectValue )
            {

            CAccPolAudioTopologyObjectCon* container1 =
                CAccPolAudioTopologyObjectCon::NewLC();

            // Get object from proxy	
            ObjectConDataFromProxyL( *container1 );
            for( TInt i = 0; i < container1->Count(); i++ )
                {
                if( container1->AtL( i )->ObjectType()
                    == EAccPolAudioVolumeControlObject )
                    {
                    CAccPolVolumeControl* mVolume =
                        ( CAccPolVolumeControl* )container1->AtL( i );
                    if( mVolume->UnitId() == 2 && mVolume->Channel()
                        == KAccRightFrontChannel )
                        {
                        COMPONENT_TRACE(
                            ( _L( "ASYStub - CASYStubCmdHandlerBase::ProcessCommandL - SET - right channel volume level=%f" ), mVolume->VolumeDb() ) );
                        COMPONENT_TRACE(
                            ( _L( "ASYStub - CASYStubCmdHandlerBase::ProcessCommandL - SET - right channel volume min level=%f" ), mVolume->MinVolumeDb() ) );
                        COMPONENT_TRACE(
                            ( _L( "ASYStub - CASYStubCmdHandlerBase::ProcessCommandL - SET - right channel volume max level=%f" ), mVolume->MaxVolumeDb() ) );
                        COMPONENT_TRACE(
                            ( _L( "ASYStub - CASYStubCmdHandlerBase::ProcessCommandL - SET - right channel volume res level=%f" ), mVolume->VolumeResDb() ) );
                        }
                    if( mVolume->UnitId() == 2 && mVolume->Channel()
                        == KAccLeftFrontChannel )
                        {
                        COMPONENT_TRACE(
                            ( _L( "ASYStub - CASYStubCmdHandlerBase::ProcessCommandL - SET - left channel volume level=%f" ), mVolume->VolumeDb() ) );
                        COMPONENT_TRACE(
                            ( _L( "ASYStub - CASYStubCmdHandlerBase::ProcessCommandL - SET - left channel volume min level=%f" ), mVolume->MinVolumeDb() ) );
                        COMPONENT_TRACE(
                            ( _L( "ASYStub - CASYStubCmdHandlerBase::ProcessCommandL - SET - left channel volume max level=%f" ), mVolume->MaxVolumeDb() ) );
                        COMPONENT_TRACE(
                            ( _L( "ASYStub - CASYStubCmdHandlerBase::ProcessCommandL - SET - left channel volume res level=%f" ), mVolume->VolumeResDb() ) );
                        }
                    }
                else
                    {
                    CAccPolMuteControl* mMute =
                        ( CAccPolMuteControl* )container1->AtL( i );
                    if( mMute->Channel() == KAccMasterChannel )
                        {
                        COMPONENT_TRACE(
                            ( _L( "ASYStub - CASYStubCmdHandlerBase::ProcessCommandL - KAccSetAudioControl - mute=%d" ), mMute->Mute() ) );
                        }
                    }

                }

            ProcessResponseL( *container1 );
            CleanupStack::PopAndDestroy( container1 );

            }
        else if( aCommand == ECmdGetObjectValue )
            {

            CAccPolAudioTopologyObjectCon* container1 =
                CAccPolAudioTopologyObjectCon::NewLC();
            CAccPolAudioTopologyObjectCon* container2 =
                CAccPolAudioTopologyObjectCon::NewLC();

            RProperty prop;
            CBufFlat* buf =
                CBufFlat::NewL( KAccSrvObjectBaseStreamBufGranularity );
            CleanupStack::PushL( buf );
            buf->ResizeL( KAccSrvObjectBaseStreamBufMaxSize );
            TPtr8 ptr( buf->Ptr( 0 ) );
            TInt retval = prop.Get( KTFAccessoryTestProperty,
                KTFAccessoryObjectMethod,
                ptr );
            ( void )rStrm.Open( *buf );
            rStrm.PushL();
            container2->InternalizeL( rStrm );
            rStrm.Close();
            rStrm.Pop();

            // Read object --> after that assign new value
            ObjectConDataFromProxyL( *container1 );
            if( container1->AtL( 0 )->ObjectType()
                == EAccPolAudioVolumeControlObject )
                {
                CAccPolVolumeControl* rightVolume =
                    ( CAccPolVolumeControl* )container2->AtL( 0 );
                CAccPolVolumeControl* leftVolume =
                    ( CAccPolVolumeControl* )container2->AtL( 1 );
                if( reinterpret_cast<CAccPolVolumeControl*> ( container1->AtL( 0 ) )->Channel()
                    == KAccRightFrontChannel )
                    {
                    reinterpret_cast<CAccPolVolumeControl*> ( container1->AtL( 0 ) )->SetVolumeDb( rightVolume->VolumeDb() );
                    reinterpret_cast<CAccPolVolumeControl*> ( container1->AtL( 0 ) )->SetMinVolumeDb( rightVolume->MinVolumeDb() );
                    reinterpret_cast<CAccPolVolumeControl*> ( container1->AtL( 0 ) )->SetMaxVolumeDb( rightVolume->MaxVolumeDb() );
                    reinterpret_cast<CAccPolVolumeControl*> ( container1->AtL( 0 ) )->SetVolumeResDb( rightVolume->VolumeResDb() );
                    }

                if( reinterpret_cast<CAccPolVolumeControl*> ( container1->AtL( 1 ) )->Channel()
                    == KAccLeftFrontChannel )
                    {
                    reinterpret_cast<CAccPolVolumeControl*> ( container1->AtL( 1 ) )->SetVolumeDb( rightVolume->VolumeDb() );
                    reinterpret_cast<CAccPolVolumeControl*> ( container1->AtL( 1 ) )->SetMinVolumeDb( rightVolume->MinVolumeDb() );
                    reinterpret_cast<CAccPolVolumeControl*> ( container1->AtL( 1 ) )->SetMaxVolumeDb( rightVolume->MaxVolumeDb() );
                    reinterpret_cast<CAccPolVolumeControl*> ( container1->AtL( 1 ) )->SetVolumeResDb( rightVolume->VolumeResDb() );
                    }
                }
            else
                {
                CAccPolMuteControl* psMute =
                    ( CAccPolMuteControl* )container2->AtL( 0 );
                if( reinterpret_cast<CAccPolMuteControl*> ( container1->AtL( 0 ) )->Channel()
                    == KAccMasterChannel )
                    {
                    reinterpret_cast<CAccPolMuteControl*> ( container1->AtL( 0 ) )->SetMute( psMute->Mute() );
                    }
                }

            ProcessResponseL( *container1 );

            CleanupStack::PopAndDestroy( buf );
            CleanupStack::PopAndDestroy( container2 );
            CleanupStack::PopAndDestroy( container1 );

            }
        else
            {
            COMPONENT_TRACE(
                ( _L( "ASYStub - CASYStubCmdHandlerBase::ProcessCommandL - No control object to found!" ) ) );
            }

        }
    else if( name == KAccVideoHdmiAttributes )
        {
        switch( aCommand )
            {
            case ECmdGetObjectValue:
                {
                // Create buffer
                CBufFlat* buf = CBufFlat::NewL( KAccSrvObjectBaseStreamBufGranularity );
                CleanupStack::PushL( buf );
                buf->ResizeL( KAccSrvObjectBaseStreamBufMaxSize );
                
                // Read container from P&S
                TPtr8 bufPtr( buf->Ptr( 0 ) );
                TInt err = RProperty::Get( KTFAccessoryTestProperty,
                    KTFAccessoryObjectMethod,
                    bufPtr );
                if( err == KErrNone )
                    {
                    // Stream HDMI container from data read from P&S
                    CAccPolHdmiObjectCon* con = CAccPolHdmiObjectCon::NewLC(); 
                    RBufReadStream readStream( *buf );
                    readStream.PushL();
                    con->InternalizeL( readStream );
                    
                    // Change all aatributes in speaker allocation to false
                    RAccPolHdmiSpeakerAllocationArray array;
                    CleanupClosePushL( array );
                    con->GetHdmiSpeakerAllocationObjectsL( array );
                    if( array.Count() )
                        {
                        CAccPolHdmiSpeakerAllocation* speakerAllocation = array[0];
                        CAccPolHdmiSpeakerAllocation::THdmiSpeakerAllocation bits =
                            CAccPolHdmiSpeakerAllocation::EHdmiSpeakerUnknown;
                        speakerAllocation->SetSpeakerAllocation( bits );
                        }
                    CleanupStack::PopAndDestroy( &array );
                    
                    // Process response
                    ProcessResponseL( *con, err );
                    
                    // Cleanup
                    CleanupStack::PopAndDestroy( &readStream );
                    CleanupStack::PopAndDestroy( con );
                    }
                
                // Cleanup
                CleanupStack::PopAndDestroy( buf );
                break;
                }
            case ECmdSetObjectValue:
            default:
                {
                COMPONENT_TRACE(
                    ( _L( "ASYStub - CASYStubCmdHandlerBase::ProcessCommandL - Nothing to do!" ) ) );
                break;
                }
            }
        }
    else if( name == KAccVideoFormat )
        {
        switch( aCommand )
            {
            case ECmdGetObjectValue:
                {
                // Create buffer
                CBufFlat* buf = CBufFlat::NewL( KAccSrvObjectBaseStreamBufGranularity );
                CleanupStack::PushL( buf );
                buf->ResizeL( KAccSrvObjectBaseStreamBufMaxSize );
                
                // Read container from P&S
                TPtr8 bufPtr( buf->Ptr( 0 ) );
                TInt err = RProperty::Get( KTFAccessoryTestProperty,
                    KTFAccessoryObjectMethod,
                    bufPtr );
                if( err == KErrNone )
                    {
                    // Stream HDMI object container format from data read from P&S
                    CAccPolHdmiObjectCon* con = CAccPolHdmiObjectCon::NewLC();
                    RBufReadStream readStream( *buf );
                    readStream.PushL();
                    con->InternalizeL( readStream );
                    CleanupStack::PopAndDestroy( &readStream );
                    
                    // Set video format to interlaced mode
                    RAccPolHdmiVideoFormatArray array;
                    CleanupClosePushL( array );
                    con->GetHdmiVideoFormatObjectsL( array );
                    if( array.Count() )
                        {
                        CAccPolHdmiVideoFormat* videoFormat = array[0];
                        videoFormat->SetInterlaced( ETrue );
                        }
                    CleanupStack::PopAndDestroy( &array );
                    
                    // Process response
                    ProcessResponseL( *con, err );
                    
                    // Cleanup
                    CleanupStack::PopAndDestroy( con );
                    }
                
                // Cleanup
                CleanupStack::PopAndDestroy( buf );
                break;
                }
            case ECmdSetObjectValue:
            default:
                {
                COMPONENT_TRACE(
                    ( _L( "ASYStub - CASYStubCmdHandlerBase::ProcessCommandL - Nothing to do!" ) ) );
                break;
                }
            }
        }
    else
        {
        COMPONENT_TRACE(
            ( _L( "ASYStub - CASYStubCmdHandlerBase::ProcessCommandL - Nothing to do!" ) ) );
        }

    /*CleanupStack::PopAndDestroy( buf );	        
     CleanupStack::PopAndDestroy( container );
     CleanupStack::PopAndDestroy( aObject );    
     */
    COMPONENT_TRACE(
        ( _L( "ASYStub - CASYStubCmdHandlerBase::ProcessCommandL - return void - CAccPolObjectBase" ) ) );
    }

void CASYStubCmdHandlerBase::Start( TInt aTimeMs,
    TInt64 aValue,
    TAccPolValueType aValueType )
    {
    iTimer = CPeriodic::NewL( CActive::EPriorityStandard );
    TCallBack callback( TimeOutL, this );
    TTimeIntervalMicroSeconds32 delay( 1000 * aTimeMs );
    iResponse = aValue;
    iResponseType = aValueType;
    iTimer->Start( delay, delay/*not used*/, callback );
    }

TInt CASYStubCmdHandlerBase::TimeOutL( TAny* aThis )
    {
    static_cast<CASYStubCmdHandlerBase*> ( aThis )->DoTimeOutL();
    return KErrNone;
    }

void CASYStubCmdHandlerBase::DoTimeOutL()
    {
    delete iTimer;//one timeout is all what is needed
    iTimer = NULL;

    switch( iResponseType )
        {
        case ( EAPVBool ):
            {
            TAccValueTypeTBool response =
                {
                EFalse
                };
            response.iValue = iResponse;
            ProcessResponseL( response );
            }
            break;

        default:
            {
            TRACE_ASSERT_ALWAYS;
            }
            break;
        }

    iResponse = 0;
    iResponseType = EAPVNone;

    }

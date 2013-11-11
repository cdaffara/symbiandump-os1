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
 * Description:
 *
 */

// INCLUDE FILES
#include "siftrace.h"
#include "ASYStubObserver.h"
#include <TfAccessoryTestControlTypes.h>
#include <AccConGenericID.h>
#include <AccPolProprietaryNameValuePairs.h>
#include <accpolaudiostreamformatcon.h>
#include <accpolaudiotopologyobjectcon.h>
#include <accpolaudiostreamformat.h>
#include <accpolobjecttypes.h>
#include <s32mem.h>
#include <accpolhdmiobjectcon.h>
#include <accessoryservicesinternalpskeys.h>
#ifdef FF_AUTOMOTIVESTACK
#include <autoaudiopskeys.h>
#endif

// ----------------------------------------------------------------------------------
// CASYStubService::CASYStubService() 
// ----------------------------------------------------------------------------------	
CASYStubService::CASYStubService()
    {
    iTestCaseID = 0;

    }

// ----------------------------------------------------------------------------------
// CASYStubService::ConstructL() 
// ----------------------------------------------------------------------------------	
void CASYStubService::ConstructL( void )
    {

    TInt err( KErrNone );

    err = iAccessoryServer.Connect();

    if( err != KErrNone )
        {
        COMPONENT_TRACE( _L("ASYSTUB - CASYStubService::ConstructL() - failed to connect AccServer") );
        User::Leave( err );
        }

    err = iAccessoryControl.CreateSubSession( iAccessoryServer ); //CHECK THIS !!!

    if( err != KErrNone )
        {
        COMPONENT_TRACE( _L("ASYSTUB - CASYStubService::ConstructL() - failed to connect RAccessoryControl") );
        User::Leave( err );
        }

    COMPONENT_TRACE( ( _L( "ASYSTUB - CASYParameterResolver::ConstructL() - return" ) ) );
    }
//
// ----------------------------------------------------------------------------------
// CASYStubService::~CASYStubService()
// ----------------------------------------------------------------------------------	
CASYStubService::~CASYStubService()
    {
    COMPONENT_TRACE( ( _L( "ASYSTUB - CTFTestControlObserver::~CTFTestControlObserver - Enter" ) ) );
    iAccessoryControl.CloseSubSession();
    iAccessoryServer.Close();
    COMPONENT_TRACE( ( _L( "ASYSTUB - CTFTestControlObserver::~CTFTestControlObserver - succesful" ) ) );
    }
//
// ----------------------------------------------------------------------------------
// CASYStubService::Service()
// ----------------------------------------------------------------------------------
void CASYStubService::Service( TInt aTestCaseID,
    TTFASYReference aMethod,
    TInt64 aParam1,
    TUint32& aParam2,
    TInt aTimeMs )
    {

    //Test case is changed!
    if( iTestCaseID != aTestCaseID )
        {
        iTestCaseID = aTestCaseID;
        iCapabilityArray.Reset();
        }

    switch( aMethod )
        {
        case ETFAsyConn:
            {
            __UHEAP_MARK;
            TRequestStatus status;
            CAccConfigFileParser* accConfigFileParser =
                CAccConfigFileParser::NewL( _L("Configuration file") );

            TBuf<12> HWModelID;
            HWModelID.Num( aParam1 );
            __UHEAP_MARK;
            CAccConGenericID* genericID = CAccConGenericID::NewL();
            CleanupStack::PushL( genericID );

            accConfigFileParser->FindL( genericID, aParam1, HWModelID );
            iAccessoryControl.ConnectAccessory( status, genericID, EFalse );

            User::WaitForRequest( status );
            TInt retval = status.Int();
            COMPONENT_TRACE( ( _L( "ASYSTUB - CASYStubService::Service - Connect status %d" ), retval) );

            iGenericId = genericID->GenericID();
            CleanupStack::PopAndDestroy( genericID );
            __UHEAP_MARKEND;
            delete accConfigFileParser;
            __UHEAP_MARKEND;
            }
            break;

        case ETFAsyConnOpenCable:
            {            
            }
            break;
//        case ETFAsyConnOpenCable:
        case ETFAsyConnectOpenCable3Poles:
            {
            TRequestStatus status;

            CAccConfigFileParser* accConfigFileParser =
                CAccConfigFileParser::NewL( _L("Configuration file") );
            CAccConGenericID* genericID = CAccConGenericID::NewL();

            RArray<TAccPolNameValueRecord> nameValueArray;
            CleanupClosePushL( nameValueArray );
            TBuf<KHWModelIDMaxLength> hwModelID( _L("opencable"));
            TAccPolNameRecord nameRecord;
            TAccPolValueRecord valueRecord;

            TBuf<KHWModelIDMaxLength> HWModelID( _L("opencable"));

            //Set GenericID header
            iGenericIDHeader.iAccessoryDeviceType = KDTHeadset;
            iGenericIDHeader.iPhysicalConnection = KPCWired;
            iGenericIDHeader.iApplicationProtocol = 0x0;

            if( aMethod == ETFAsyConnectOpenCable3Poles )
                {
                iGenericIDHeader.iCapabilitiesSubblocks = KSBAudioSubblock;
                }
            else
                {
                iGenericIDHeader.iCapabilitiesSubblocks = KSBVideoSubblock
                    | KSBAudioSubblock;
                }

            iGenericIDHeader.iHWModelID = hwModelID;
            iGenericIDHeader.iHWDeviceID = 0x0;
            iGenericIDHeader.iDBID = 0x0;

            /*       		//Set "Audio Output Type" capability
             TAccPolNameValueRecord  nameValueRecordAudioOutputType( KAccAudioOutputType,
             EAccAudioOutPutTypePrivate,
             EAPVInt,
             EAPVPolicy );
             */

            accConfigFileParser->ParseGenericIDL( genericID,
                iGenericIDHeader,
                nameValueArray );
            nameValueArray.Close();

            iAccessoryControl.ConnectAccessory( status, genericID, EFalse );
            User::WaitForRequest( status );
            TInt retval = status.Int();

            COMPONENT_TRACE( ( _L( "ASYSTUB - CASYStubService::Service - Connect status %d" ), retval) );
            delete accConfigFileParser;

            iGenericId = genericID->GenericID();

            delete genericID;
            CleanupStack::PopAndDestroy( &nameValueArray );//nameValueArray.Close() is called;
            }
            break;

        case ETFAsyUpdateTVOut:
            {
            TRequestStatus status;

            CAccConfigFileParser* accConfigFileParser =
                CAccConfigFileParser::NewL( _L("Configuration file") );
            CAccConGenericID* genericID = CAccConGenericID::NewL();

            RArray<TAccPolNameValueRecord> nameValueArray;
            CleanupClosePushL( nameValueArray );
            TBuf<KHWModelIDMaxLength> hwModelID( _L("opencable"));
            TAccPolNameRecord nameRecord;
            TAccPolValueRecord valueRecord;

            TBuf<KHWModelIDMaxLength> HWModelID( _L("opencable"));

            //Set GenericID header
            iGenericIDHeader.iAccessoryDeviceType = KDTAVDevice;
            iGenericIDHeader.iPhysicalConnection = KPCWired;
            iGenericIDHeader.iApplicationProtocol = 0x0;
            iGenericIDHeader.iCapabilitiesSubblocks = KSBAudioSubblock
                | KSBVideoSubblock;
            iGenericIDHeader.iHWModelID = hwModelID;
            iGenericIDHeader.iHWDeviceID = 0x0;

            iGenericIDHeader.iDBID = iGenericId.UniqueID();

            //Set "Audio Output Type" capability
            TAccPolNameValueRecord
                nameValueRecordAudioOutputType( KAccAudioOutputType,
                    EAccAudioOutPutTypePublic,
                    EAPVInt,
                    EAPVPolicy );

            nameValueArray.Append( nameValueRecordAudioOutputType );

            //Set "Audio stereo" capability
            TAccPolNameValueRecord nameValueRecordAudioStereo( KAccStereoAudio,
                EAccAudioOutPutTypePublic,
                EAPVInt,
                EAPVPolicy );

            nameValueArray.Append( nameValueRecordAudioStereo );

            //Set "Line out Type" capability
            TAccPolNameValueRecord nameValueRecordAudioOut( KAccAudioOut,
                TAccAudioOutLineout,
                EAPVInt,
                EAPVPolicy );

            nameValueArray.Append( nameValueRecordAudioOut );

            //Set "Video Output Type" capability
            TAccPolNameValueRecord nameValueRecordVideoOut( KAccVideoOut,
                EAccVideoOutCVideo,
                EAPVInt,
                EAPVPolicy );

            nameValueArray.Append( nameValueRecordVideoOut );

            accConfigFileParser->ParseGenericIDL( genericID,
                iGenericIDHeader,
                nameValueArray );
            nameValueArray.Close();

            iAccessoryControl.ConnectAccessory( status, genericID, EFalse );
            User::WaitForRequest( status );
            TInt retval = status.Int();

            COMPONENT_TRACE( ( _L( "ASYSTUB - CASYStubService::Service - Update status %d" ), retval) );
            delete accConfigFileParser;

            iGenericId = genericID->GenericID();

            delete genericID;
            CleanupStack::PopAndDestroy( &nameValueArray );//nameValueArray.Close() is called;
            }
            break;

        case ETFAsyUpdateHeadset:
            {
            TRequestStatus status;

            CAccConfigFileParser* accConfigFileParser =
                CAccConfigFileParser::NewL( _L("Configuration file") );
            CAccConGenericID* genericID = CAccConGenericID::NewL();

            RArray<TAccPolNameValueRecord> nameValueArray;
            CleanupClosePushL( nameValueArray );
            TBuf<KHWModelIDMaxLength> hwModelID( _L("headset"));
            TAccPolNameRecord nameRecord;
            TAccPolValueRecord valueRecord;

            TBuf<KHWModelIDMaxLength> HWModelID( _L("headset"));

            //Set GenericID header
            iGenericIDHeader.iAccessoryDeviceType = KDTHeadset;
            iGenericIDHeader.iPhysicalConnection = KPCWired;
            iGenericIDHeader.iApplicationProtocol = 0x0;
            iGenericIDHeader.iCapabilitiesSubblocks = KSBAudioSubblock;
            iGenericIDHeader.iHWModelID = hwModelID;
            iGenericIDHeader.iHWDeviceID = 0x0;

            iGenericIDHeader.iDBID = iGenericId.UniqueID();

            //Set "Audio Output Type" capability
            TAccPolNameValueRecord
                nameValueRecordAudioOutputType( KAccAudioOutputType,
                    EAccAudioOutPutTypePublic,
                    EAPVInt,
                    EAPVPolicy );

            nameValueArray.Append( nameValueRecordAudioOutputType );

            //Set "Audio stereo" capability
            TAccPolNameValueRecord nameValueRecordAudioStereo( KAccStereoAudio,
                EAccAudioOutPutTypePublic,
                EAPVInt,
                EAPVPolicy );

            nameValueArray.Append( nameValueRecordAudioStereo );

            accConfigFileParser->ParseGenericIDL( genericID,
                iGenericIDHeader,
                nameValueArray );
            nameValueArray.Close();

            iAccessoryControl.ConnectAccessory( status, genericID, EFalse );
            User::WaitForRequest( status );
            TInt retval = status.Int();

            COMPONENT_TRACE( ( _L( "ASYSTUB - CASYStubService::Service - Update status %d" ), retval) );
            delete accConfigFileParser;

            iGenericId = genericID->GenericID();

            delete genericID;
            CleanupStack::PopAndDestroy( &nameValueArray );//nameValueArray.Close() is called;
            }
            break;

        case ETFAsyConnectHeadset:
            {
            TRequestStatus status;

            CAccConfigFileParser* accConfigFileParser =
                CAccConfigFileParser::NewL( _L("Configuration file") );
            CAccConGenericID* genericID = CAccConGenericID::NewL();

            RArray<TAccPolNameValueRecord> nameValueArray;
            CleanupClosePushL( nameValueArray );
            TBuf<KHWModelIDMaxLength> hwModelID( _L("headset"));
            TAccPolNameRecord nameRecord;
            TAccPolValueRecord valueRecord;

            TBuf<KHWModelIDMaxLength> HWModelID( _L("headset"));

            //Set GenericID header
            iGenericIDHeader.iAccessoryDeviceType = KDTHeadset;
            iGenericIDHeader.iPhysicalConnection = KPCWired;
            iGenericIDHeader.iApplicationProtocol = 0x0;
            iGenericIDHeader.iCapabilitiesSubblocks = KSBAudioSubblock;
            iGenericIDHeader.iHWModelID = hwModelID;
            iGenericIDHeader.iHWDeviceID = 0x0;

            //Set "Audio Output Type" capability
            TAccPolNameValueRecord
                nameValueRecordAudioOutputType( KAccAudioOutputType,
                    EAccAudioOutPutTypePublic,
                    EAPVInt,
                    EAPVPolicy );

            nameValueArray.Append( nameValueRecordAudioOutputType );

            //Set "Audio stereo" capability
            TAccPolNameValueRecord nameValueRecordAudioStereo( KAccStereoAudio );
            nameValueArray.Append( nameValueRecordAudioStereo );

            //integrated audio output
            TAccPolNameValueRecord
                nameValueRecordIntAudioInput( KAccIntegratedAudioInput );
            nameValueArray.Append( nameValueRecordIntAudioInput );

            //integrated audio input
            TAccPolNameValueRecord
                nameValueRecordIntAudioOutput( KAccIntegratedAudioOutput );
            nameValueArray.Append( nameValueRecordIntAudioOutput );

            accConfigFileParser->ParseGenericIDL( genericID,
                iGenericIDHeader,
                nameValueArray );
            nameValueArray.Close();

            iAccessoryControl.ConnectAccessory( status, genericID, EFalse );
            User::WaitForRequest( status );
            TInt retval = status.Int();

            COMPONENT_TRACE( ( _L( "ASYSTUB - CASYStubService::Service - Update status %d" ), retval) );
            delete accConfigFileParser;

            iGenericId = genericID->GenericID();

            delete genericID;
            CleanupStack::PopAndDestroy( &nameValueArray );//nameValueArray.Close() is called;
            }
            break;

        case ETFAsyConnectECIAccessory:
            {
            TRequestStatus status;

            CAccConfigFileParser* accConfigFileParser =
                CAccConfigFileParser::NewL( _L("Configuration file") );
            CAccConGenericID* genericID = CAccConGenericID::NewL();

            RArray<TAccPolNameValueRecord> nameValueArray;
            CleanupClosePushL( nameValueArray );
            TBuf<KHWModelIDMaxLength> hwModelID( _L("headset"));
            TAccPolNameRecord nameRecord;
            TAccPolValueRecord valueRecord;

            TBuf<KHWModelIDMaxLength> HWModelID( _L("headset"));

            //Set GenericID header
            iGenericIDHeader.iAccessoryDeviceType = KDTHeadset;
            iGenericIDHeader.iPhysicalConnection = KPCWired;
            iGenericIDHeader.iApplicationProtocol = 0x0;
            iGenericIDHeader.iCapabilitiesSubblocks = KSBAudioSubblock;
            iGenericIDHeader.iHWModelID = hwModelID;
            iGenericIDHeader.iHWDeviceID = 0x0;

            // Audio Out Connector
            TAccPolNameValueRecord
                nameValueRecordAudioOutconnector( KAccAudioOutConnector,
                    1,
                    EAPVBool,
                    EAPVPolicy );
            TAccPolNameRecord name;
            name.SetNameL( KAccECIAudioBlock );

            TBuf8<1> test( _L8(""));

            TAccPolValueRecord value( test, EAPVAccessory );

            value.SetType( EAPVDes8 );

            TAccPolNameValueRecord eciAudioBlock( name, value );

            //Set "Audio stereo" capability
            TAccPolNameValueRecord nameValueRecordAudioStereo( KAccStereoAudio,
                0,
                EAPVNone,
                EAPVPolicy );

            //Set Integrated Audio Input
            TAccPolNameValueRecord
                nameValueRecordIntegrated( KAccIntegratedAudioInput,
                    0,
                    EAPVNone,
                    EAPVPolicy );

            //Set "Audio Output Type" capability
            TAccPolNameValueRecord
                nameValueRecordAudioOutputType( KAccAudioOutputType,
                    EAccAudioOutPutTypePrivate,
                    EAPVInt,
                    EAPVPolicy );

            nameValueArray.Append( nameValueRecordAudioOutconnector );
            nameValueArray.Append( eciAudioBlock );
            nameValueArray.Append( nameValueRecordAudioStereo );
            nameValueArray.Append( nameValueRecordIntegrated );
            nameValueArray.Append( nameValueRecordAudioOutputType );
            accConfigFileParser->ParseGenericIDL( genericID,
                iGenericIDHeader,
                nameValueArray );
            nameValueArray.Close();

            iAccessoryControl.ConnectAccessory( status, genericID, EFalse );
            User::WaitForRequest( status );
            TInt retval = status.Int();

            COMPONENT_TRACE( ( _L( "ASYSTUB - CASYStubService::Service - Update status %d" ), retval) );
            delete accConfigFileParser;

            iGenericId = genericID->GenericID();

            delete genericID;
            CleanupStack::PopAndDestroy( &nameValueArray );//nameValueArray.Close() is called;
            }
            break;

        case ETFAsyDisc:
            {
            TRequestStatus status;
            iAccessoryControl.DisconnectAccessory( status, iGenericId );
            User::WaitForRequest( status );
            TInt retval = status.Int();
            COMPONENT_TRACE( ( _L( "ASYSTUB - CASYStubService::Service - Diconnect status %d" ), retval) );
            }
            break;

        case ETFAsyIntNotify:
            {
            TAccPolNameRecord aNameRecord;
            TAccValueTypeTInt aValue;
            //M10210 KAccAudioOutputType
            aNameRecord.SetNameL( aParam2 );
            aValue.iValue = aParam1;

            iAccessoryControl.AccessoryValueChangedNotifyL( iGenericId,
                aNameRecord,
                aValue );

            COMPONENT_TRACE( ( _L( "ASYSTUB - CASYStubService::Service - AccessoryValueChangedNotifyL - TInt" ) ) );
            }
            break;

        case ETFAsyBoolNotify:
            {
            TAccPolNameRecord aNameRecord;
            TAccValueTypeTBool aValue;
            aNameRecord.SetNameL( aParam2 );
            aValue.iValue = ( TBool )aParam1;

            iAccessoryControl.AccessoryValueChangedNotifyL( iGenericId,
                aNameRecord,
                aValue );

            COMPONENT_TRACE( ( _L( "ASYSTUB - CASYStubService::Service - AccessoryValueChangedNotifyL - TBool" ) ) );
            }
            break;

        case ETFAsyObjectNotify:
            {
            // Set name record
            TAccPolNameRecord aNameRecord;
            aNameRecord.SetNameL( aParam2 );
            
            // Create buffer
            CBufFlat* valueBuf = CBufFlat::NewL( 50 );
            CleanupStack::PushL( valueBuf );
            valueBuf->ResizeL( 256 );
            TPtr8 valueBufPtr( valueBuf->Ptr( 0 ) );
            
            // Get buffer from P&S
            TInt err = RProperty::Get( KTFAccessoryTestProperty,
                KTFAccessoryObjectMethod,
                valueBufPtr );
            if( err == KErrNone )
                {
                CAccPolObjectCon* object = NULL;
                switch( aParam2 )
                    {
                    case KAccAudioUnits:
                        {
                        object = CAccPolAudioTopologyObjectCon::NewLC();
                        break;
                        }
                    case KAccVideoHdmiAttributes:
                    case KAccVideoFormat:
                        {
                        object = CAccPolHdmiObjectCon::NewLC(); 
                        break;
                        }
                    default:
                        {
                        break;
                        }
                    }
                if( object )
                    {
                    RBufReadStream readStream( *valueBuf );
                    readStream.PushL();
                    object->InternalizeL( readStream );
                    iAccessoryControl.AccessoryValueChangedNotifyL( iGenericId,
                        aNameRecord,
                        *object );
                    CleanupStack::PopAndDestroy( &readStream );
                    CleanupStack::PopAndDestroy( object );
                    }
                CleanupStack::PopAndDestroy( valueBuf );
                }
            COMPONENT_TRACE( ( _L( "ASYSTUB - CASYStubService::Service - AccessoryValueChangedNotifyL - Object" ) ) );
            }
            break;

        case ETFAsySetCapability:
            {
            TTFCapability aCapabilityS;

            aCapabilityS.iCapability = aParam2;
            aCapabilityS.iValue = aParam1;
            aCapabilityS.iTimeMs = aTimeMs;
            iCapabilityArray.Append( aCapabilityS );
            }
            break;

        case ETFAsyConnUSB:
            {
            TRequestStatus status;

            CAccConfigFileParser* accConfigFileParser =
                CAccConfigFileParser::NewL( _L("Configuration file") );
            CAccConGenericID* genericID = CAccConGenericID::NewL();

            RArray<TAccPolNameValueRecord> nameValueArray;
            CleanupClosePushL( nameValueArray );
            TBuf<KHWModelIDMaxLength> hwModelID( _L("headset"));
            TAccPolNameRecord nameRecord;
            TAccPolValueRecord valueRecord;

            TBuf<KHWModelIDMaxLength> HWModelID( _L("headset"));

            //Set GenericID header
            iGenericIDHeader.iAccessoryDeviceType = KDTHeadset;
            iGenericIDHeader.iPhysicalConnection = KPCWired | KPCUSB;
            iGenericIDHeader.iApplicationProtocol = 0x0;
            iGenericIDHeader.iCapabilitiesSubblocks = KSBAudioSubblock;
            iGenericIDHeader.iHWModelID = hwModelID;
            iGenericIDHeader.iHWDeviceID = 0x666;

            //Set "Audio Output Type" capability
            TAccPolNameValueRecord
                nameValueRecordAudioOutputType( KAccAudioOutputType,
                    EAccAudioOutPutTypePublic,
                    EAPVInt,
                    EAPVPolicy );
            nameValueArray.Append( nameValueRecordAudioOutputType );

            //Set "Audio stereo" capability
            TAccPolNameValueRecord nameValueRecordAudioStereo( KAccStereoAudio );
            nameValueArray.Append( nameValueRecordAudioStereo );

            //integrated audio output
            TAccPolNameValueRecord
                nameValueRecordIntAudioInput( KAccIntegratedAudioInput );
            nameValueArray.Append( nameValueRecordIntAudioInput );

            //integrated audio input
            TAccPolNameValueRecord
                nameValueRecordIntAudioOutput( KAccIntegratedAudioOutput );
            nameValueArray.Append( nameValueRecordIntAudioOutput );

            // Audio Streams cpability.
            TAccPolNameValueRecord
                audioStreamingFormats( KAccAudioStreamsFormat, EAPVAccessory );

            nameValueArray.Append( audioStreamingFormats );

            // Audio Controls capability.
            TAccPolNameValueRecord
                audioControls( KAccAudioUnits, EAPVAccessory );

            nameValueArray.Append( audioControls );

            // Audio Controls capability.
            TAccPolNameValueRecord audioControl( KAccAudioControl,
                EAPVAccessory );

            nameValueArray.Append( audioControl );

            accConfigFileParser->ParseGenericIDL( genericID,
                iGenericIDHeader,
                nameValueArray );
            nameValueArray.Close();

            iAccessoryControl.ConnectAccessory( status, genericID, EFalse );
            User::WaitForRequest( status );
            TInt retval = status.Int();

            COMPONENT_TRACE( ( _L( "ASYSTUB - CASYStubService::Service - Update status for USB %d" ), retval) );
            delete accConfigFileParser;

            iGenericId = genericID->GenericID();

            delete genericID;
            CleanupStack::PopAndDestroy( &nameValueArray );//nameValueArray.Close() is called;
            }
            ;
            break;

        case ETFAsyConnUSB2:
            {
            TRequestStatus status;

            CAccConfigFileParser* accConfigFileParser =
                CAccConfigFileParser::NewL( _L("Configuration file") );
            CAccConGenericID* genericID = CAccConGenericID::NewL();

            RArray<TAccPolNameValueRecord> nameValueArray;
            CleanupClosePushL( nameValueArray );
            TBuf<KHWModelIDMaxLength> hwModelID( _L("headset"));
            TAccPolNameRecord nameRecord;
            TAccPolValueRecord valueRecord;

            TBuf<KHWModelIDMaxLength> HWModelID( _L("headset"));

            //Set GenericID header
            iGenericIDHeader.iAccessoryDeviceType = KDTHeadset;
            iGenericIDHeader.iPhysicalConnection = KPCWired | KPCUSB;
            iGenericIDHeader.iApplicationProtocol = 0x0;
            iGenericIDHeader.iCapabilitiesSubblocks = KSBAudioSubblock;
            iGenericIDHeader.iHWModelID = hwModelID;
            iGenericIDHeader.iHWDeviceID = 0x666;

            //Set "Audio Output Type" capability
            TAccPolNameValueRecord
                nameValueRecordAudioOutputType( KAccAudioOutputType,
                    EAccAudioOutPutTypePublic,
                    EAPVInt,
                    EAPVPolicy );
            nameValueArray.Append( nameValueRecordAudioOutputType );

            //Set "Audio stereo" capability
            TAccPolNameValueRecord nameValueRecordAudioStereo( KAccStereoAudio );
            nameValueArray.Append( nameValueRecordAudioStereo );

            //integrated audio output
            TAccPolNameValueRecord
                nameValueRecordIntAudioInput( KAccIntegratedAudioInput );
            nameValueArray.Append( nameValueRecordIntAudioInput );

            //integrated audio input
            TAccPolNameValueRecord
                nameValueRecordIntAudioOutput( KAccIntegratedAudioOutput );
            nameValueArray.Append( nameValueRecordIntAudioOutput );

            CAccPolAudioStreamFormatCon* stream =
                CAccPolAudioStreamFormatCon::NewLC();

            CAccPolAudioStreamFormat* audioStream1 =
                CAccPolAudioStreamFormat::NewLC( EAccPolAudioInputStreamObject,
                    15,
                    KUidFormatMP3,
                    CAccPolAudioStreamFormat::EStereo,
                    CAccPolAudioStreamFormat::EBitsPerSample24,
                    11 );

            CAccPolAudioStreamFormat
                * audioStream2 =
                    CAccPolAudioStreamFormat::NewLC( EAccPolAudioOutputStreamObject,
                        15,
                        KUidFormatMP3,
                        CAccPolAudioStreamFormat::EStereo,
                        CAccPolAudioStreamFormat::EBitsPerSample24,
                        11 );

            stream->AddL( *audioStream1 );
            stream->AddL( *audioStream2 );

            // Audio Streams cpability.
            TAccPolNameValueRecord
                audioStreamingFormats( KAccAudioStreamsFormat,
                    EAPVPolicy,
                    stream );

            nameValueArray.Append( audioStreamingFormats );

            // Audio Controls capability.
            TAccPolNameValueRecord
                audioControls( KAccAudioUnits, EAPVAccessory );

            nameValueArray.Append( audioControls );

            // Audio Controls capability.
            TAccPolNameValueRecord audioControl( KAccAudioControl,
                EAPVAccessory );

            nameValueArray.Append( audioControl );

            accConfigFileParser->ParseGenericIDL( genericID,
                iGenericIDHeader,
                nameValueArray );
            nameValueArray.Close();

            iAccessoryControl.ConnectAccessory( status, genericID, EFalse );
            User::WaitForRequest( status );
            TInt retval = status.Int();

            COMPONENT_TRACE( ( _L( "ASYSTUB - CASYStubService::Service - Update status for USB %d" ), retval) );
            delete accConfigFileParser;

            iGenericId = genericID->GenericID();

            delete genericID;
            CleanupStack::PopAndDestroy( audioStream2 );//			
            CleanupStack::PopAndDestroy( audioStream1 );//			
            CleanupStack::PopAndDestroy( stream );//
            CleanupStack::PopAndDestroy( &nameValueArray );//nameValueArray.Close() is called;
            }
            ;
            break;
        case ETFAsyConnHDMI:
            {
            TRequestStatus status;

            CAccConfigFileParser* accConfigFileParser =
                CAccConfigFileParser::NewL( _L("Configuration file") );
            CAccConGenericID* genericID = CAccConGenericID::NewL();

            RArray<TAccPolNameValueRecord> nameValueArray;
            CleanupClosePushL( nameValueArray );
            TBuf<KHWModelIDMaxLength> hwModelID( _L("hdmi"));
            TAccPolNameRecord nameRecord;
            TAccPolValueRecord valueRecord;

            TBuf<KHWModelIDMaxLength> HWModelID( _L("hdmi"));

            //Set GenericID header
            iGenericIDHeader.iAccessoryDeviceType = KDTAVDevice;
            iGenericIDHeader.iPhysicalConnection = KPCHDMI;
            iGenericIDHeader.iApplicationProtocol = 0x0;    
            iGenericIDHeader.iCapabilitiesSubblocks = KSBAudioSubblock | KSBVideoSubblock;
            iGenericIDHeader.iHWModelID = hwModelID;
            iGenericIDHeader.iHWDeviceID = 0x0;

            iGenericIDHeader.iDBID = iGenericId.UniqueID();

            //Set "Video Output Type" capability
            TAccPolNameValueRecord nameValueRecordVideoOut( KAccVideoOut,
                EAccVideoOutEHDMI,
                EAPVInt,
                EAPVAccessory );
            nameValueArray.Append( nameValueRecordVideoOut );
            
            // Set "HDMI attributes" capability
            TAccPolNameValueRecord nameValueRecordHdmiAttributes( KAccVideoHdmiAttributes,
                EAPVAccessory );
            nameValueArray.Append( nameValueRecordHdmiAttributes );

            // Set "HDCP" capability
            TAccPolNameValueRecord nameValueRecordHdcpSupport( KAccVideoHdcpSupport,
                EFalse,// Assumption, this is fetched through TV Out Config API
                EAPVBool,
                EAPVAccessory );
            nameValueArray.Append( nameValueRecordHdcpSupport );
            
            // Set "Video active format" capability
            TAccPolNameValueRecord nameValueRecordVideoFormat( KAccVideoFormat,
                EAPVAccessory );
            nameValueArray.Append( nameValueRecordVideoFormat );

            accConfigFileParser->ParseGenericIDL( genericID,
                iGenericIDHeader,
                nameValueArray );
            nameValueArray.Close();

            iAccessoryControl.ConnectAccessory( status, genericID, EFalse );
            User::WaitForRequest( status );
            TInt retval = status.Int();

            COMPONENT_TRACE( ( _L( "ASYSTUB - CASYStubService::Service - Update status %d" ), retval) );
            delete accConfigFileParser;

            iGenericId = genericID->GenericID();

            delete genericID;
            CleanupStack::PopAndDestroy( &nameValueArray );//nameValueArray.Close() is called;

            break;
            }
#ifdef FF_AUTOMOTIVESTACK           
        case ETFAsyRTPStreamingConn:
            {
            RProperty property;          
            CleanupClosePushL ( property );
            COMPONENT_TRACE( ( _L( "ASYSTUB - CTFTestControlObserver::Service - Attaching to RTP streaming key" ) ) );
            User::LeaveIfError ( property.Attach ( KPSUidAccessoryServices, KPSAutoKeyRTPStreamingConnectionStatus ) );
            COMPONENT_TRACE( ( _L( "ASYSTUB - CTFTestControlObserver::Service - publishing to RTP streaming key" ) ) );                        
            User::LeaveIfError ( property.Set ( aParam1 ) );
            CleanupStack::PopAndDestroy (); // property
            }
            break;
#endif
            
        default:

            COMPONENT_TRACE( ( _L( "ASYSTUB - CASYStubService::Service - Subscribe destination is WIRELESS ASY" ) ) );
            break;
        }
    }
//
// ----------------------------------------------------------------------------------
// CASYStubService::GetTestCaseID()
// ----------------------------------------------------------------------------------
TInt CASYStubService::GetTestCaseID()
    {
    COMPONENT_TRACE( ( _L( "ASYSTUB - CTFTestControlObserver::GetTestCaseID" ) ) );
    return iTestCaseID;
    }

//
// ----------------------------------------------------------------------------------
// CASYStubService::GetTestCaseID()
// ----------------------------------------------------------------------------------
TTFCapability CASYStubService::GetAndRemoveCapability()
    {

    TTFCapability aCapabilityS; // Jihuu

    if( iCapabilityArray.Count() != 0 )
        {
        aCapabilityS.iCapability = iCapabilityArray[0].iCapability;
        aCapabilityS.iValue = iCapabilityArray[0].iValue;
        aCapabilityS.iTimeMs = iCapabilityArray[0].iTimeMs;
        iCapabilityArray.Remove( 0 );
        }
    else
        {
        aCapabilityS.iCapability = 0;
        aCapabilityS.iValue = 0;
        aCapabilityS.iTimeMs = 0;
        }

    return aCapabilityS;
    }


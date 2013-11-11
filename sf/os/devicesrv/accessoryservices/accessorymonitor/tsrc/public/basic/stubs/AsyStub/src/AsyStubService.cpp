/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Used to emulate accessory connections in emulator.
*
*/


// INCLUDE FILES
#include <AccConGenericID.h>
#include <AccPolProprietaryNameValuePairs.h>
#include "siftrace.h"
#include "ASYStubObserver.h"
#include "AccMonitorCapMapper.h"

// ----------------------------------------------------------------------------------
// CASYStubService::CASYStubService() 
// ---------------------------------------------------------------------------------- 
CASYStubService::CASYStubService() 
    {           
    }
  
// ----------------------------------------------------------------------------------
// CASYStubService::ConstructL() 
// ---------------------------------------------------------------------------------- 
void CASYStubService::ConstructL() 
    {           
    
    TInt err(KErrNone);
    
    err = iAccessoryServer.Connect();                
    if ( err != KErrNone )
        {
        COMPONENT_TRACE( _L("ASYSTUB - CASYStubService::ConstructL() - failed to connect AccServer") );
        User::Leave( err );
        }
    err = iAccessoryControl.CreateSubSession( iAccessoryServer );
    if ( err != KErrNone )
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
void CASYStubService::Service(
    TTFASYReference aMethod,
    TInt64 aParam1,
    RArray<TUint32> aCapabilityArray )
    {
    
    switch( aMethod )
        {
        case ETFAsyConn:
            {
            // Connection that is made is based on the database
            // Generic ID is mapped from HW Device ID
            TRequestStatus status;                
            CAccConfigFileParser* accConfigFileParser =
                CAccConfigFileParser::NewL( _L( "Configuration file" ) ); 
            CleanupStack::PushL( accConfigFileParser );
            TBuf< 12 > HWModelID;
            HWModelID.Num( aParam1 );
            
            CAccConGenericID* genericID = CAccConGenericID::NewL();   
            CleanupStack::PushL( genericID ); 
            
            accConfigFileParser->FindL( genericID, aParam1, HWModelID );
            iAccessoryControl.ConnectAccessory( status, genericID, EFalse );
            
            User::WaitForRequest( status );
            TInt retval = status.Int();         
            COMPONENT_TRACE( ( _L( "ASYSTUB - CASYStubService::Service - Connect status %d" ), retval) );
            
            iGenericId = genericID->GenericID();
            CleanupStack::PopAndDestroy( genericID );
            CleanupStack::PopAndDestroy( accConfigFileParser );
            }
            break;
        
        case ETFAsyConnOpenCable:
        case ETFAsyConnectOpenCable3Poles:
            {
            // Connection is made by giving capabilities from adaptation side
            TRequestStatus status;                
            CAccConfigFileParser* accConfigFileParser 
                = CAccConfigFileParser::NewL( _L( "Configuration file" ) );
            CleanupStack::PushL( accConfigFileParser );
            CAccConGenericID* genericID = CAccConGenericID::NewL();
            CleanupStack::PushL( genericID );
            
            RArray<TAccPolNameValueRecord> nameValueArray;  
            CleanupClosePushL( nameValueArray );      
            TBuf< KHWModelIDMaxLength > hwModelID( _L( "opencable" ) );
            TAccPolNameRecord nameRecord;
            TAccPolValueRecord valueRecord;
            
            // Set GenericID header
            iGenericIDHeader.iAccessoryDeviceType = KDTAVDevice;
            iGenericIDHeader.iPhysicalConnection = KPCWired;
            iGenericIDHeader.iApplicationProtocol = 0x0;
            
            if ( aMethod == ETFAsyConnectOpenCable3Poles )
                {
                iGenericIDHeader.iCapabilitiesSubblocks = KSBAudioSubblock;                 
                }
            else
                {
                iGenericIDHeader.iCapabilitiesSubblocks = KSBVideoSubblock | KSBAudioSubblock;  
                }
            
            iGenericIDHeader.iHWModelID = hwModelID;
            iGenericIDHeader.iHWDeviceID = 0x0;
            iGenericIDHeader.iDBID = 0x0;
            
            accConfigFileParser->ParseGenericIDL( genericID, iGenericIDHeader, nameValueArray );
            nameValueArray.Close();
                                                   
            iAccessoryControl.ConnectAccessory( status, genericID, ETrue );
            User::WaitForRequest( status ); 
            TInt retval = status.Int();           
            
            COMPONENT_TRACE( ( _L( "ASYSTUB - CASYStubService::Service - Connect status %d" ), retval ) );
            
            iGenericId = genericID->GenericID();                    
      
            CleanupStack::PopAndDestroy( &nameValueArray );//nameValueArray.Close() is called;
            CleanupStack::PopAndDestroy( genericID );
            CleanupStack::PopAndDestroy( accConfigFileParser );
            }
        break;        
        
        case ETFAsyUpdateTVOut:
            {
            // Connection is made by giving capabilities from adaptation side
            TRequestStatus status;
            CAccConfigFileParser* accConfigFileParser = CAccConfigFileParser::NewL( _L("Configuration file"));
            CleanupStack::PushL( accConfigFileParser );
            CAccConGenericID* genericID = CAccConGenericID::NewL();
            CleanupStack::PushL( genericID );
            
            RArray<TAccPolNameValueRecord> nameValueArray;  
            CleanupClosePushL( nameValueArray );      
            TBuf< KHWModelIDMaxLength > hwModelID( _L( "opencable" ) );
            TAccPolNameRecord nameRecord;
            TAccPolValueRecord valueRecord;

            //Set GenericID header
            iGenericIDHeader.iAccessoryDeviceType = KDTAVDevice;
            iGenericIDHeader.iPhysicalConnection = KPCWired;
            iGenericIDHeader.iApplicationProtocol = 0x0;
            iGenericIDHeader.iCapabilitiesSubblocks = KSBAudioSubblock | KSBVideoSubblock;
            iGenericIDHeader.iHWModelID = hwModelID;
            iGenericIDHeader.iHWDeviceID = 0x0;
            
            iGenericIDHeader.iDBID = iGenericId.UniqueID();
            
            //Set "Audio Output Type" capability
            TAccPolNameValueRecord  nameValueRecordAudioOutputType( KAccAudioOutputType,
                                                                  EAccAudioOutPutTypePublic,
                                                                  EAPVInt,
                                                                  EAPVPolicy );

            nameValueArray.Append( nameValueRecordAudioOutputType );
      

            //Set "Audio stereo" capability
            TAccPolNameValueRecord  nameValueRecordAudioStereo( KAccStereoAudio,
                                                                  EAccAudioOutPutTypePublic,
                                                                  EAPVInt,
                                                                  EAPVPolicy );
            
            nameValueArray.Append( nameValueRecordAudioStereo );
            //Set "Line out Type" capability
            TAccPolNameValueRecord  nameValueRecordAudioOut( KAccAudioOut,
                                                                  TAccAudioOutLineout,
                                                                  EAPVInt,
                                                                  EAPVPolicy );
            nameValueArray.Append( nameValueRecordAudioOut );
            //Set "Video Output Type" capability
            TAccPolNameValueRecord  nameValueRecordVideoOut( KAccVideoOut,
                                                                  EAccVideoOutCVideo,
                                                                  EAPVInt,
                                                                  EAPVPolicy );
            nameValueArray.Append( nameValueRecordVideoOut );
            accConfigFileParser->ParseGenericIDL( genericID, iGenericIDHeader, nameValueArray );
            nameValueArray.Close();
            
            iAccessoryControl.ConnectAccessory( status, genericID, EFalse );
            User::WaitForRequest( status ); 
            TInt retval = status.Int();           
            
            COMPONENT_TRACE( ( _L( "ASYSTUB - CASYStubService::Service - Update status %d" ), retval ) );    
            
            iGenericId = genericID->GenericID();                    
            
            CleanupStack::PopAndDestroy( &nameValueArray );//nameValueArray.Close() is called;
            CleanupStack::PopAndDestroy( genericID );
            CleanupStack::PopAndDestroy( accConfigFileParser );
            }
        break;     
        
        case ETFAsyUpdateHeadset:
            {
            // Connection is made by giving capabilities from adaptation side
            TRequestStatus status;                
            CAccConfigFileParser* accConfigFileParser = CAccConfigFileParser::NewL( _L( "Configuration file" ) );
            CleanupStack::PushL( accConfigFileParser );
            CAccConGenericID* genericID = CAccConGenericID::NewL();
            CleanupStack::PushL( genericID );
            
            RArray<TAccPolNameValueRecord> nameValueArray;    
            CleanupClosePushL( nameValueArray );    
            TBuf< KHWModelIDMaxLength > hwModelID( _L( "headset" ) );
            TAccPolNameRecord nameRecord;
            TAccPolValueRecord valueRecord;
            
            //Set GenericID header
            iGenericIDHeader.iAccessoryDeviceType = KDTHeadset;
            iGenericIDHeader.iPhysicalConnection = KPCWired;
            iGenericIDHeader.iApplicationProtocol = 0x0;
            iGenericIDHeader.iCapabilitiesSubblocks = KSBAudioSubblock;
            iGenericIDHeader.iHWModelID = hwModelID;
            iGenericIDHeader.iHWDeviceID = 0x0;
            iGenericIDHeader.iDBID = iGenericId.UniqueID();
            
            //Set "Audio Output Type" capability
            TAccPolNameValueRecord  nameValueRecordAudioOutputType( KAccAudioOutputType,
                                                                  EAccAudioOutPutTypePublic,
                                                                  EAPVInt,
                                                                  EAPVPolicy );
            nameValueArray.Append( nameValueRecordAudioOutputType );
      
            //Set "Audio stereo" capability
            TAccPolNameValueRecord  nameValueRecordAudioStereo( KAccStereoAudio,
                                                                  EAccAudioOutPutTypePublic,
                                                                  EAPVInt,
                                                                  EAPVPolicy );
            nameValueArray.Append( nameValueRecordAudioStereo );
            accConfigFileParser->ParseGenericIDL( genericID, iGenericIDHeader, nameValueArray );
            nameValueArray.Close();
                                                   
            iAccessoryControl.ConnectAccessory( status, genericID, ETrue );
            User::WaitForRequest( status ); 
            TInt retval = status.Int();           
        
            COMPONENT_TRACE( ( _L( "ASYSTUB - CASYStubService::Service - Update status %d" ), retval ) );    
      
            iGenericId = genericID->GenericID();                    
      
            CleanupStack::PopAndDestroy( &nameValueArray );//nameValueArray.Close() is called;
            CleanupStack::PopAndDestroy( genericID );
            CleanupStack::PopAndDestroy( accConfigFileParser );
            }
        break;           
  
        case ETFAsyConnectHeadset:
            {
            // Connection is made by giving capabilities from adaptation side
            TRequestStatus status;                
            CAccConfigFileParser* accConfigFileParser = CAccConfigFileParser::NewL( _L( "Configuration file" ) );
            CleanupStack::PushL( accConfigFileParser );
            CAccConGenericID* genericID = CAccConGenericID::NewL();
            CleanupStack::PushL( genericID );
            RArray<TAccPolNameValueRecord> nameValueArray;    
            CleanupClosePushL( nameValueArray );    
            TBuf<KHWModelIDMaxLength>      hwModelID( _L( "headset" ) );
            TAccPolNameRecord              nameRecord;
            TAccPolValueRecord             valueRecord;
          
            //Set GenericID header
            iGenericIDHeader.iAccessoryDeviceType   = KDTHeadset;
            iGenericIDHeader.iPhysicalConnection    = KPCWired;
            iGenericIDHeader.iApplicationProtocol   = 0x0;
            iGenericIDHeader.iCapabilitiesSubblocks = KSBAudioSubblock;
            iGenericIDHeader.iHWModelID             = hwModelID;
            iGenericIDHeader.iHWDeviceID            = 0x0;
            
            //Set "Audio Output Type" capability
            TAccPolNameValueRecord  nameValueRecordAudioOutputType( KAccAudioOutputType,
                                                                    EAccAudioOutPutTypePublic,
                                                                    EAPVInt,
                                                                    EAPVPolicy );
            nameValueArray.Append( nameValueRecordAudioOutputType );
            //Set "Audio stereo" capability
            TAccPolNameValueRecord  nameValueRecordAudioStereo( KAccStereoAudio );
            nameValueArray.Append( nameValueRecordAudioStereo );
            accConfigFileParser->ParseGenericIDL( genericID, iGenericIDHeader, nameValueArray );
            nameValueArray.Close();
            iAccessoryControl.ConnectAccessory( status, genericID, ETrue );
            User::WaitForRequest( status ); 
            TInt retval = status.Int();           
            COMPONENT_TRACE( ( _L( "ASYSTUB - CASYStubService::Service - Update status %d" ), retval ) );    
            
            iGenericId = genericID->GenericID();                    
            
            CleanupStack::PopAndDestroy( &nameValueArray );//nameValueArray.Close() is called;
            CleanupStack::PopAndDestroy( genericID );
            CleanupStack::PopAndDestroy( accConfigFileParser );
            }       
        break;
        
  
          
        case ETFAsyConnectECIAccessory:
            {
            // Connection is made by giving capabilities from adaptation side
            TRequestStatus status;                
            CAccConfigFileParser* accConfigFileParser = CAccConfigFileParser::NewL( _L( "Configuration file" ) );
            CleanupStack::PushL( accConfigFileParser );
            CAccConGenericID* genericID = CAccConGenericID::NewL();
            CleanupStack::PushL( genericID );
            
            RArray< TAccPolNameValueRecord > nameValueArray;    
            CleanupClosePushL( nameValueArray );    
            TBuf< KHWModelIDMaxLength >      hwModelID( _L( "headset" ) );
            TAccPolNameRecord              nameRecord;
            TAccPolValueRecord             valueRecord;
            
            //Set GenericID header
            iGenericIDHeader.iAccessoryDeviceType   = KDTHeadset;
            iGenericIDHeader.iPhysicalConnection    = KPCWired;
            iGenericIDHeader.iApplicationProtocol   = 0x0;
            iGenericIDHeader.iCapabilitiesSubblocks = KSBAudioSubblock;
            iGenericIDHeader.iHWModelID             = hwModelID;
            iGenericIDHeader.iHWDeviceID            = 0x0;

            //Set "ECI audio block" capability
            TBuf8<30> eciData(_L8("ECIAudioBlockData"));
            TAccPolNameValueRecord  eciAudioBlock(  KAccECIAudioBlock, eciData, EAPVPolicy );
            nameValueArray.Append( eciAudioBlock );
            //Set "Audio Output Type" capability
            TAccPolNameValueRecord  nameValueRecordAudioOutputType( KAccAudioOutputType,
                                                                  EAccAudioOutPutTypePublic,
                                                                  EAPVInt,
                                                                  EAPVPolicy );
            nameValueArray.Append( nameValueRecordAudioOutputType );
            //Set "Audio stereo" capability
            TAccPolNameValueRecord  nameValueRecordAudioStereo( KAccStereoAudio,
                                                                  EAccAudioOutPutTypePublic,
                                                                  EAPVInt,
                                                                  EAPVPolicy );
            nameValueArray.Append( nameValueRecordAudioStereo );
            //Set "Audio stereo" capability
            TAccPolNameValueRecord  nameValueRecordAudioOutconnector( KAccAudioOutConnector,
                                                                    1,
                                                                    EAPVBool,
                                                                    EAPVAccessory );
            nameValueArray.Append( nameValueRecordAudioOutconnector );
            accConfigFileParser->ParseGenericIDL( genericID, iGenericIDHeader, nameValueArray );
            nameValueArray.Close();
                                                   
            iAccessoryControl.ConnectAccessory( status, genericID, EFalse );
            User::WaitForRequest( status ); 
            TInt retval = status.Int();           
            
            COMPONENT_TRACE( ( _L( "ASYSTUB - CASYStubService::Service - Update status %d" ), retval ) );    
            iGenericId = genericID->GenericID();                    
            CleanupStack::PopAndDestroy( &nameValueArray );//nameValueArray.Close() is called;
            CleanupStack::PopAndDestroy( genericID );
            CleanupStack::PopAndDestroy( accConfigFileParser );
            }
        break;           
                      
        case ETFAsyConnectECITVOut:
            {
            // Connection is made by giving capabilities from adaptation side
            TRequestStatus status;                
            CAccConfigFileParser* accConfigFileParser = CAccConfigFileParser::NewL( _L( "Configuration file" ) );
            CleanupStack::PushL( accConfigFileParser );
            CAccConGenericID* genericID = CAccConGenericID::NewL();
            CleanupStack::PushL( genericID );
            RArray<TAccPolNameValueRecord> nameValueArray;    
            CleanupClosePushL( nameValueArray );    
            TBuf<KHWModelIDMaxLength>      hwModelID( _L( "TVOut" ) );
            TAccPolNameRecord              nameRecord;
            TAccPolValueRecord             valueRecord;
            
            //Set GenericID header
            iGenericIDHeader.iAccessoryDeviceType   = KDTAVDevice;
            iGenericIDHeader.iPhysicalConnection    = KPCWired;
            iGenericIDHeader.iApplicationProtocol   = 0x0;
            iGenericIDHeader.iCapabilitiesSubblocks = KSBAudioSubblock|KSBVideoSubblock;
            iGenericIDHeader.iHWModelID             = hwModelID;
            iGenericIDHeader.iHWDeviceID            = 0x0;
            
            //Set "ECI audio block" capability
            TBuf8<30> eciData(_L8("ECIAudioBlockData"));
            TAccPolNameValueRecord  eciAudioBlock(  KAccECIAudioBlock, eciData, EAPVPolicy );
            nameValueArray.Append( eciAudioBlock);
            //Set "Audio Output Type" capability
            TAccPolNameValueRecord  nameValueRecordAudioOutputType( KAccAudioOutputType,
                                                                    EAccAudioOutPutTypePublic,
                                                                    EAPVInt,
                                                                    EAPVPolicy );
            nameValueArray.Append( nameValueRecordAudioOutputType );
            //Set "Audio stereo" capability
            TAccPolNameValueRecord  nameValueRecordAudioStereo( KAccStereoAudio,
                                                                EAccAudioOutPutTypePublic,
                                                                EAPVInt,
                                                                EAPVPolicy );
            nameValueArray.Append( nameValueRecordAudioStereo );
            //Set "Audio stereo" capability
            TAccPolNameValueRecord  nameValueRecordAudioOutconnector( KAccAudioOutConnector,
                                                                      1,
                                                                      EAPVBool,
                                                                      EAPVAccessory );
            nameValueArray.Append( nameValueRecordAudioOutconnector );
            //Set "Audio stereo" capability
            TAccPolNameValueRecord  nameValueRecordVideoOut( KAccVideoOut,
                                                             EAccVideoOutCVideo,
                                                             EAPVInt,
                                                             EAPVPolicy );
            nameValueArray.Append( nameValueRecordVideoOut );
            accConfigFileParser->ParseGenericIDL( genericID, iGenericIDHeader, nameValueArray );
            nameValueArray.Close();
            iAccessoryControl.ConnectAccessory( status, genericID, EFalse );
            User::WaitForRequest( status ); 
            TInt retval = status.Int();           
            
            COMPONENT_TRACE( ( _L( "ASYSTUB - CASYStubService::Service - Update status %d" ), retval ) );    
            
            iGenericId = genericID->GenericID();                    
            CleanupStack::PopAndDestroy( &nameValueArray );//nameValueArray.Close() is called;
            CleanupStack::PopAndDestroy( genericID );
            CleanupStack::PopAndDestroy( accConfigFileParser );
            }
        break;
        
        case ETFAsyConnectECIMusicStand:
            {
            // Connection is made by giving capabilities from adaptation side
            TRequestStatus status;                
            CAccConfigFileParser* accConfigFileParser = CAccConfigFileParser::NewL( _L( "Configuration file" ) );
            CleanupStack::PushL( accConfigFileParser );
            CAccConGenericID* genericID = CAccConGenericID::NewL();
            CleanupStack::PushL( genericID );
            RArray< TAccPolNameValueRecord > nameValueArray;    
            CleanupClosePushL( nameValueArray );    
            TBuf< KHWModelIDMaxLength >      hwModelID( _L( "MusicStand" ) );
            TAccPolNameRecord              nameRecord;
            TAccPolValueRecord             valueRecord;
            
            //Set GenericID header
            iGenericIDHeader.iAccessoryDeviceType   = KDTOffice;
            iGenericIDHeader.iPhysicalConnection    = KPCWired;
            iGenericIDHeader.iApplicationProtocol   = 0x0;
            iGenericIDHeader.iCapabilitiesSubblocks = KSBAudioSubblock;
            iGenericIDHeader.iHWModelID             = hwModelID;
            iGenericIDHeader.iHWDeviceID            = 0x0;
            
            //Set "ECI audio block" capability
            TBuf8< 30 > eciData( _L8( "ECIAudioBlockData" ) );
            TAccPolNameValueRecord  eciAudioBlock(  KAccECIAudioBlock, eciData, EAPVPolicy );
            nameValueArray.Append( eciAudioBlock );
            //Set "Audio Output Type" capability
            TAccPolNameValueRecord  nameValueRecordAudioOutputType( KAccAudioOutputType,
                                                                    EAccAudioOutPutTypePublic,
                                                                    EAPVInt,
                                                                    EAPVPolicy );
            nameValueArray.Append( nameValueRecordAudioOutputType );
            //Set "Audio stereo" capability
            TAccPolNameValueRecord  nameValueRecordAudioStereo( KAccStereoAudio,
                                                                EAccAudioOutPutTypePublic,
                                                                EAPVInt,
                                                                EAPVPolicy );
            nameValueArray.Append( nameValueRecordAudioStereo );
            
            accConfigFileParser->ParseGenericIDL( genericID, iGenericIDHeader, nameValueArray );
            nameValueArray.Close();
                                                   
            iAccessoryControl.ConnectAccessory( status, genericID, EFalse );
            User::WaitForRequest( status ); 
            TInt retval = status.Int();           
            
            COMPONENT_TRACE( ( _L( "ASYSTUB - CASYStubService::Service - Update status %d" ), retval ) );    
            
            iGenericId = genericID->GenericID();                    
      
            CleanupStack::PopAndDestroy( &nameValueArray );//nameValueArray.Close() is called;
            CleanupStack::PopAndDestroy( genericID );
            CleanupStack::PopAndDestroy( accConfigFileParser );
            }
        break;
        
        case ETFAsyConnectECICarKit:
            {
            // Connection is made by giving capabilities from adaptation side
            TRequestStatus status;                
            CAccConfigFileParser* accConfigFileParser = CAccConfigFileParser::NewL( _L( "Configuration file" ) );
            CleanupStack::PushL( accConfigFileParser );
            CAccConGenericID* genericID = CAccConGenericID::NewL();
            CleanupStack::PushL( genericID );
            
            RArray< TAccPolNameValueRecord > nameValueArray;    
            CleanupClosePushL( nameValueArray );    
            TBuf< KHWModelIDMaxLength >      hwModelID( _L( "MusicStand" ) );
            TAccPolNameRecord              nameRecord;
            TAccPolValueRecord             valueRecord;
            
            //Set GenericID header
            iGenericIDHeader.iAccessoryDeviceType   = KDTCarKit;
            iGenericIDHeader.iPhysicalConnection    = KPCWired;
            iGenericIDHeader.iApplicationProtocol   = 0x0;
            iGenericIDHeader.iCapabilitiesSubblocks = KSBAudioSubblock|KDTCharger|KSBCarKitSubblock;
            iGenericIDHeader.iHWModelID             = hwModelID;
            iGenericIDHeader.iHWDeviceID            = 0x0;
            
            //Set "ECI audio block" capability
            TBuf8< 30 > eciData( _L8( "ECIAudioBlockData" ) );
            TAccPolNameValueRecord  eciAudioBlock(  KAccECIAudioBlock, eciData, EAPVPolicy );
            nameValueArray.Append( eciAudioBlock);
            //Set "Audio Output Type" capability
            TAccPolNameValueRecord  nameValueRecordAudioOutputType( KAccAudioOutputType,
                                                                    EAccAudioOutPutTypePublic,
                                                                    EAPVInt,
                                                                    EAPVPolicy );
            nameValueArray.Append( nameValueRecordAudioOutputType );
            //Set "Audio stereo" capability
            TAccPolNameValueRecord  nameValueRecordAudioStereo( KAccStereoAudio,
                                                                EAccAudioOutPutTypePublic,
                                                                EAPVInt,
                                                                EAPVPolicy );
            nameValueArray.Append( nameValueRecordAudioStereo );
            //Set "Car Radio Mute" capability
            TAccPolNameValueRecord nameValueRecordCarRadioMute( KAccCarRadioMute,
                                                                1,
                                                                EAPVBool,
                                                                EAPVAccessory );
            nameValueArray.Append( nameValueRecordCarRadioMute );
            //Set "External power supply" capability
            TAccPolNameValueRecord nameValueRecordPowerExtOut( KAccExtPowerSupply,
                                                               1,
                                                               EAPVBool,
                                                               EAPVAccessory );
            nameValueArray.Append( nameValueRecordPowerExtOut );
            accConfigFileParser->ParseGenericIDL( genericID, iGenericIDHeader, nameValueArray );
            nameValueArray.Close();
                                                   
            iAccessoryControl.ConnectAccessory( status, genericID, EFalse );
            User::WaitForRequest( status ); 
            TInt retval = status.Int();           
        
            COMPONENT_TRACE( ( _L( "ASYSTUB - CASYStubService::Service - Update status %d" ), retval ) );    
      
            iGenericId = genericID->GenericID();                    
      
            CleanupStack::PopAndDestroy( &nameValueArray );//nameValueArray.Close() is called;
            CleanupStack::PopAndDestroy( genericID );
            CleanupStack::PopAndDestroy( accConfigFileParser );
            }
        break;
        
        case ETFAsyConnectArrayOfCapabilities:
            {
            // Connection is made by giving capabilities from adaptation side
            TRequestStatus status;                
            CAccConfigFileParser* accConfigFileParser = CAccConfigFileParser::NewL( _L( "Configuration file" ) );
            CleanupStack::PushL( accConfigFileParser );
            CAccConGenericID* genericID = CAccConGenericID::NewL();
            CleanupStack::PushL( genericID );
            RArray< TAccPolNameValueRecord > nameValueArray;    
            CleanupClosePushL( nameValueArray );    
            TBuf< KHWModelIDMaxLength > hwModelID( _L( "Some Device" ) );
            TAccPolNameRecord nameRecord;
            TAccPolValueRecord valueRecord;
            CAccMonitorCapMapper* mapper = CAccMonitorCapMapper::NewL();
            CleanupStack::PushL( mapper );
            TUint32 subblocks( 0x0 );
            TUint32 capa( 0 );
            
            for( TInt i = 0; i != aCapabilityArray.Count(); i++ )
                {
                COMPONENT_TRACE( ( _L( "[ASYSTUB] CASYStubService::Service() - In array capability: %d" ), aCapabilityArray[ i ] ) );
                }
            
            // Go through capabilities and create name values
            for( TInt i = 0; i != aCapabilityArray.Count(); i++ )
                {
                TAccMonCapability capability = aCapabilityArray[ i ];
                if( capability < 17 )
                    {
                    TUint32 luku = mapper->GetHeaderCapabilityFromDefinitionL( capability );
                    COMPONENT_TRACE( ( _L( "[ASYSTUB] CASYStubService::Service() - Header Device type capability: %d" ), luku ) );
                    iGenericIDHeader.iAccessoryDeviceType = luku;
                    if( luku == 1 )
                        {
                        //iGenericIDHeader.iDeviceAddress = KBTAddrHeadsetHSPAndHFP;
                        }
                    }
                if( ( capability > 16 ) && ( capability < 31 ) )
                    {
                    TUint32 luku = mapper->GetHeaderCapabilityFromDefinitionL( capability );
                    COMPONENT_TRACE( ( _L( "[ASYSTUB] CASYStubService::Service() - Header Physical connection capability: %d" ), luku ) );
                    iGenericIDHeader.iPhysicalConnection = 0x01;
                    }
                if( ( capability > 80 ) && ( capability < 97 ) )
                    {
                    subblocks = subblocks|KSBAudioSubblock;
                    capa = mapper->GetCapabilityFromDefinitionL( capability );
                    COMPONENT_TRACE( ( _L( "[ASYSTUB] CASYStubService::Service() - Audio capability: %d" ), capa ) );
                    if( capa == KAccAudioOutputType )
                        {
                        TAccPolNameValueRecord  nameValue( 
                                           capa,
                                           EAccAudioOutPutTypePublic,
                                           EAPVInt,
                                           EAPVPolicy );
                        nameValueArray.Append( nameValue );
                        }
                    else
                        {
                        TAccPolNameValueRecord  nameValue( capa );
                        nameValueArray.Append( nameValue );
                        }
                    }
                if( ( capability > 96 ) && ( capability < 113 ) )
                    {
                    COMPONENT_TRACE( ( _L( "[ASYSTUB] CASYStubService::Service() - BT capa, not supported" ) ) );
                    
                    /*subblocks = subblocks|KSBBluetoothSubblock;
                    capa = mapper->GetCapabilityFromDefinitionL( capability );
                    TAccPolNameValueRecord  nameValue( capa );
                    nameValueArray.Append( nameValue );*/
                    }
                if( ( capability > 112 ) && ( capability < 129 ) )
                    {
                    subblocks = subblocks|KSBVideoSubblock;
                    capa = mapper->GetCapabilityFromDefinitionL( capability );
                    COMPONENT_TRACE( ( _L( "[CConnectionEmulator] CASYStubService::Service() - Video capability: %d" ), capa ) );
                    TAccPolNameValueRecord  nameValue( capa );
                    nameValueArray.Append( nameValue );
                    }
                }
            CleanupStack::Pop( mapper );
            //Set GenericID header
            iGenericIDHeader.iApplicationProtocol   = 0x0;
            COMPONENT_TRACE( ( _L( "[ASYSTUB] CASYStubService::Service() - Accessory to be connected" ) ) );
            COMPONENT_TRACE( ( _L( "[ASYSTUB] CASYStubService::Service() - Device type: %d" ), iGenericIDHeader.iAccessoryDeviceType ) );
            COMPONENT_TRACE( ( _L( "[ASYSTUB] CASYStubService::Service() - Physical connection: %d" ), iGenericIDHeader.iPhysicalConnection ) );
            COMPONENT_TRACE( ( _L( "[ASYSTUB] CASYStubService::Service() - Application protocol: %d" ), iGenericIDHeader.iApplicationProtocol ) );
            iGenericIDHeader.iCapabilitiesSubblocks = subblocks;
            COMPONENT_TRACE( ( _L( "[ASYSTUB] CASYStubService::Service() - Capability subblocks: %d" ), iGenericIDHeader.iCapabilitiesSubblocks ) );
            iGenericIDHeader.iHWModelID             = hwModelID;
            iGenericIDHeader.iHWDeviceID            = 0x0;
            COMPONENT_TRACE( ( _L( "[ASYSTUB] CASYStubService::Service() - HW device id: %d" ), iGenericIDHeader.iHWDeviceID ) );
            accConfigFileParser->ParseGenericIDL( genericID, iGenericIDHeader, nameValueArray );
            nameValueArray.Close();
            iAccessoryControl.ConnectAccessory( status, genericID, EFalse );
            COMPONENT_TRACE( ( _L( "[ASYSTUB] CASYStubService::Service() - Don't evaluate connection rules" ) ) );
            User::WaitForRequest( status ); 
            TInt retval = status.Int();           
            
            COMPONENT_TRACE( ( _L( "ASYSTUB - CASYStubService::Service - Update status %d" ), retval ) );    
            
            iGenericId = genericID->GenericID();                    
            
            CleanupStack::PopAndDestroy( &nameValueArray );
            CleanupStack::PopAndDestroy( genericID );
            CleanupStack::PopAndDestroy( accConfigFileParser );
            }
            break;
    
        case ETFAsyDisc:
            {
            // Just a disconnect
            TRequestStatus status;                
            iAccessoryControl.DisconnectAccessory( status, iGenericId );
            User::WaitForRequest( status ); 
            TInt retval = status.Int();           
            COMPONENT_TRACE( ( _L( "ASYSTUB - CASYStubService::Service - Diconnect status %d" ), retval ) );    
            }
        break;
        
        default:
            {
            COMPONENT_TRACE( ( _L( "ASYSTUB - CASYStubService::Service - Subscribe destination is WIRELESS ASY" ) ) );    
            break;
            }
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
    TTFCapability aCapabilityS;
    COMPONENT_TRACE( ( _L( "ASYSTUB - CASYStubService::GetAndRemoveCapability - This does nothing" ) ) ); 
    if( iCapabilityArray.Count() != 0 )
        {
        /*aCapabilityS.iCapability = iCapabilityArray[0].iCapability;
        aCapabilityS.iValue  = iCapabilityArray[0].iValue;
        aCapabilityS.iTimeMs = iCapabilityArray[0].iTimeMs;
        iCapabilityArray.Remove(0);    */
        }
    else
        {
        /*aCapabilityS.iCapability = 0;
        aCapabilityS.iValue = 0;
        aCapabilityS.iTimeMs = 0;*/
        }
    COMPONENT_TRACE( ( _L( "ASYSTUB - CASYStubService::GetAndRemoveCapability - return" ) ) );
    return aCapabilityS;     
    }

// End of File

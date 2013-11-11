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
* Description:  Command handler for Accessory Server commands.
*
*/


// INCLUDE FILES
#include "ASYStubCmdHandlerBase.h"
#include "AsyStubMainServiceBase.h"
#include "siftrace.h"
#include "bttypes.h"
#include "AsyStubCmdObserver.h"
#include "AccPolSubblockNameArrayAccessor.h"
#include <AccessorySingleConnection.h>
#include <AccPolSubblockNameArray.h>
#include <AsyCmdTypes.h>

// ----------------------------------------------------------------------------------
// CASYStubCmdHandlerBase::CASYStubCmdHandlerBase()
// ----------------------------------------------------------------------------------
CASYStubCmdHandlerBase::CASYStubCmdHandlerBase() :
    iTimer( NULL ),
    iResponse ( 0 ), 
    iResponseType ( EAPVNone )
    { 
    COMPONENT_TRACE( ( _L( "ASYStub - CASYStubCmdHandlerBase::CASYStubCmdHandlerBase() - Enter" ) ) );
      
    COMPONENT_TRACE( ( _L( "ASYStub - CASYStubCmdHandlerBase::CASYStubCmdHandlerBase() - Return" ) ) );
    }

// -----------------------------------------------------------------------------
// CASYStubCmdHandlerBase::ConstructL(
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
void CASYStubCmdHandlerBase::ConstructL()
    {
    COMPONENT_TRACE( ( _L( "ASYStub - CASYStubCmdHandlerBase::ConstructL()" ) ) );

    iCmdHandler = CAsyStubCmdObserver::NewL( this );
        
    COMPONENT_TRACE( ( _L( "ASYStub - CASYStubCmdHandlerBase::ConstructL - return void" ) ) );
    }
    
// ----------------------------------------------------------------------------------
// CASYStubCmdHandlerBase::NewL() 
// ----------------------------------------------------------------------------------
CASYStubCmdHandlerBase* CASYStubCmdHandlerBase::NewL()
    {
    CASYStubCmdHandlerBase* self = new ( ELeave ) CASYStubCmdHandlerBase();
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
    return self;
    }
  
// ----------------------------------------------------------------------------------
// CASYStubCmdHandlerBase::~CASYStubCmdHandlerBase()
// ----------------------------------------------------------------------------------
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

// ----------------------------------------------------------------------------------
// CASYStubCmdHandlerBase::ProcessCommandL()
// ----------------------------------------------------------------------------------
void CASYStubCmdHandlerBase::ProcessCommandL( const TProcessCmdId aCommand, const TASYCmdParams& aCmdParams )
    {
    COMPONENT_TRACE( ( _L( "ASYStub - CASYStubCmdHandlerBase::ProcessCommandL() - Enter" ) ) );
    
    CASYMainServiceBase* mainServiceBase = ASYMainServiceBase();
    COMPONENT_TRACE( ( _L( "AsyStub - CASYStubCmdHandlerBase::ProcessCommandL - mainServiceBase (0x%x)" ), &mainServiceBase ) );
    CASYStubMainServiceBase* AsyStub = reinterpret_cast<CASYStubMainServiceBase*>( mainServiceBase );
    
    COMPONENT_TRACE( ( _L( "AsyStub - CASYStubCmdHandlerBase::ProcessCommandL - AsyStub (0x%x)" ), &AsyStub ) );
    
    TTFCapability aCapabilityS = AsyStub->GetAndRemoveCapability();
    
    TASYCommandParamRecord cmdParams( aCmdParams() );//For debugging
    
    COMPONENT_TRACE( ( _L( "AsyStub - CASYStubCmdHandlerBase::ProcessCommandL - command: (0x%x)" ), aCommand ) );
    
    switch ( aCommand )
        {
        case ECmdGetValueBool: 
            {
            COMPONENT_TRACE( ( _L( "ASYStub - CASYStubCmdHandlerBase::ProcessCommandL() - Get value Bool" ) ) );
            TUint32 name;
            aCmdParams().iNameRecord.GetName( name );
            TAccValueTypeTBool aCmdResponse = { EFalse };
            
            if( name == aCapabilityS.iCapability )
                {
                if( 0 != aCapabilityS.iTimeMs )
                    {
                    //Serve asynchronously                    
                    Start( aCapabilityS.iTimeMs, aCapabilityS.iValue, EAPVBool );
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
          
        case ECmdSetValueBool: 
            {
            COMPONENT_TRACE( ( _L( "ASYStub - CASYStubCmdHandlerBase::ProcessCommandL() - Set value Bool" ) ) );
            TUint32 name;
            aCmdParams().iNameRecord.GetName( name );
            TAccValueTypeTBool aCmdResponse = { EFalse };
            
            if( name == aCapabilityS.iCapability 
             && aCmdParams().iCmdValue == aCapabilityS.iValue )
                {
                if( aCapabilityS.iTimeMs )
                    {
                    //Serve asynchronously                    
                    Start( aCapabilityS.iTimeMs, aCapabilityS.iValue, EAPVBool );
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
            COMPONENT_TRACE( ( _L( "ASYStub - CASYStubCmdHandlerBase::ProcessCommandL() - Process command init" ) ) );
            TAccValueTypeTBool boolInitResponse;
            TBool everyThing( ETrue );
        
            // If everything is ok
            if ( everyThing )
                {            
                COMPONENT_TRACE( ( _L( "ASYStub - CASYStubCmdHandlerBase::ProcessCommandL() - everything is ok" ) ) );
                boolInitResponse.iValue = ETrue;
                ProcessResponseL( boolInitResponse );            
                }
        
            // If everything is not ok
            else
                {
                COMPONENT_TRACE( ( _L( "ASYStub - CASYStubCmdHandlerBase::ProcessCommandL() - everything is not ok" ) ) );
                boolInitResponse.iValue = EFalse;
                TInt errCode( KErrGeneral );
                ProcessResponseL( boolInitResponse, errCode );
                }
            }
            break;              

        // For user selection...
        case ECmdAccessoryUpdated:
            {
            COMPONENT_TRACE( ( _L( "ASYStub - CASYStubCmdHandlerBase::ProcessCommandL() - Accessory updated" ) ) );
            TAccValueTypeTBool boolInitResponse;
            TBool everyThing( ETrue );
        
            //get capabilities
            CAccPolSubblockNameArray* nameArray = CAccPolSubblockNameArray::NewL();
            CleanupStack::PushL( nameArray );
            
            RAccessoryServer server;
            server.Connect();
          
            RAccessorySingleConnection connectionBase;
            connectionBase.CreateSubSession( server);
            connectionBase.GetSubblockNameArrayL( cmdParams.iGenericID, *nameArray );
            RArray<TUint32>& array = *TAccPolSubblockNameArrayAccessor::Array( nameArray );
            
            for ( TInt i( 0 ); i < array.Count(); i++ )
                {
                TUint32 name = array[ i ];
                TAccPolNameRecord nameRecord;
                nameRecord.SetNameL( name );
                TAccValueTypeTInt value;
                
                TRAPD( err, connectionBase.GetValueL( cmdParams.iGenericID, nameRecord, value ) );
                
                if ( err != KErrNone )       
                    {
                    COMPONENT_TRACE( ( _L( "ASYStub - CASYStubCmdHandlerBase::ProcessCommandL() - Capability 0x%x has no value!" ), name ) );
                    }
                name = 0;     
                }
            connectionBase.CloseSubSession();
            server.Close();
            
            CleanupStack::PopAndDestroy( nameArray );
            // If everything is ok
            if ( everyThing )
                 {            
                 COMPONENT_TRACE( ( _L( "ASYStub - CASYStubCmdHandlerBase::ProcessCommandL() - everything is ok" ) ) );
                 boolInitResponse.iValue = ETrue;
                 ProcessResponseL( boolInitResponse );            
                 }
             // If everything is not ok
            else
                 {
                 COMPONENT_TRACE( ( _L( "ASYStub - CASYStubCmdHandlerBase::ProcessCommandL() - everything is not ok" ) ) );
                 boolInitResponse.iValue = EFalse;
                 TInt errCode( KErrGeneral );
                 ProcessResponseL( boolInitResponse, errCode );
                 }
            }
            break;      
            
        case ECmdGetSupportedBTProfiles:
            {
            COMPONENT_TRACE( ( _L( "ASYStub - CASYStubCmdHandlerBase::ProcessCommandL() - Get BT profiles - Here is something wrong" ) ) );
            TASYBTAccInfo btInfo;
            btInfo.iReserved = 0;
            TBTDevAddr btAddr( aCmdParams().iCmdValue );
            switch( aCmdParams().iCmdValue )            
                {
                case KBTAddrCarkit:
                    {
                    btInfo.iDeviceInfo = 0x191;
                    btInfo.iDeviceType = KDTCarKit;
                    btInfo.iProfiles = KAccInfoHFP;
                    btInfo.iSupportedFeatures.At(0) = 1;   
                                
                    TASYBTAccInfoPckgC btInfoPckgC( btInfo );                                 
                    ProcessResponseL( btInfoPckgC, KErrNone );
                    }
                    break;
                
                case KBTAddrHeadsetHSPAndHFP:
                    {
                    btInfo.iDeviceInfo = 0x192;
                    btInfo.iDeviceType = KDTHeadset;
                    btInfo.iProfiles = KAccInfoHSP | KAccInfoHFP;
                    btInfo.iSupportedFeatures.At( 0 ) = 1;   
                    btInfo.iSupportedFeatures.At( 1 ) = 2;   
                  
                    TASYBTAccInfoPckgC btInfoPckgC( btInfo );                                 
                    ProcessResponseL( btInfoPckgC, KErrNone );
                    }
                    break;
                
                case KBTAddrHeadsetHSP:
                    {
                    btInfo.iDeviceInfo = 0x193;
                    btInfo.iDeviceType = KDTHeadset;
                    btInfo.iProfiles = KAccInfoHSP; 
                    btInfo.iSupportedFeatures.At(0) = 1;   
                  
                    TASYBTAccInfoPckgC btInfoPckgC( btInfo );                                 
                    ProcessResponseL( btInfoPckgC, KErrNone );
                    }
                    break;
                
                case KBTAddrHeadsetHFP:
                    {
                    btInfo.iDeviceInfo = 0x194;
                    btInfo.iDeviceType = KDTHeadset;
                    btInfo.iProfiles = KAccInfoHFP; 
                    btInfo.iSupportedFeatures.At(1) = 1;   
                  
                    TASYBTAccInfoPckgC btInfoPckgC( btInfo );                                 
                    ProcessResponseL( btInfoPckgC, KErrNone );
                    }
                    break;

                case KBTAddrHeadsetAVRCP:
                    {
                    btInfo.iDeviceInfo = 0x195;
                    btInfo.iDeviceType = KDTHeadset;
                    btInfo.iProfiles = KAccInfoHFP; 
                    btInfo.iSupportedFeatures.At(1) = 1;   
                  
                    TASYBTAccInfoPckgC btInfoPckgC( btInfo );                                 
                    ProcessResponseL( btInfoPckgC, KErrNone );
                    }
                    break;
                 
                case KBTAddrHeadseA2DP:
                    {
                    btInfo.iDeviceInfo = 0x196;
                    btInfo.iDeviceType = KDTHeadset;
                    btInfo.iProfiles = KAccInfoHFP; 
                    btInfo.iSupportedFeatures.At(1) = 1;   
                    btInfo.iReserved |= KAccInfoDRM;
                  
                    TASYBTAccInfoPckgC btInfoPckgC( btInfo );                                 
                    ProcessResponseL( btInfoPckgC, KErrNone );
                    }
                    break;                                        

                case KBTAddrHeadsetEmptyPR:
                    {
                    btInfo.iDeviceInfo = 0x197;
                    btInfo.iDeviceType = KDTHeadset;
                    btInfo.iProfiles = 0; 
                  
                    TASYBTAccInfoPckgC btInfoPckgC( btInfo );                                 
                    ProcessResponseL( btInfoPckgC, KErrNone );
                    }
                    break;

                case KBTAddrHeadsetNokiaSPId:
                    {
                    btInfo.iDeviceInfo = 0x181;
                    btInfo.iDeviceType = KDTHeadset;
                    btInfo.iProfiles = KAccInfoHSP; 
                    btInfo.iSupportedFeatures.At(0) = 1;   
                    
                    TASYBTAccInfoPckgC btInfoPckgC( btInfo );                                 
                    ProcessResponseL( btInfoPckgC, KErrNone );
                    }
                    break;
                    
                case KBTAddrNoResponse:
                    {
                    // No response
                    }
                    break;    

                case KBTAddrErrCode:
                    {
                    btInfo.iDeviceInfo = 0x198;
                    btInfo.iDeviceType = KDTHeadset;
                    btInfo.iProfiles = KAccInfoHSP; 
                    btInfo.iSupportedFeatures.At( 0 ) = 1;   
                    
                    TASYBTAccInfoPckgC btInfoPckgC( btInfo );                                 
                    ProcessResponseL( btInfoPckgC, KErrGeneral );
                    }
                    break;

                case KBTAllBitsOn: // All supported bits on
                    {
                    btInfo.iDeviceInfo = 0x198;
                    btInfo.iDeviceType = KDTHeadset;
                    btInfo.iProfiles = KAccInfoHSP | KAccInfoHFP | KAccInfoAVRCP | KAccInfoA2DP; 
                    btInfo.iSupportedFeatures.At( 0 ) = 0xFFFF;   
                    btInfo.iSupportedFeatures.At( 1 ) = 0xFFFF;   
                    btInfo.iSupportedFeatures.At( 2 ) = 0xFFFF;   
                    btInfo.iSupportedFeatures.At( 3 ) = 0xFFFF;   

                    TASYBTAccInfoPckgC btInfoPckgC( btInfo );                                 
                    ProcessResponseL( btInfoPckgC, KErrNone );
                    }
                    break;

                default:
                    {                           
                    btInfo.iDeviceInfo = 0x181;
                    btInfo.iDeviceType = KDTHeadset;
                    btInfo.iProfiles = KAccInfoHSP | KAccInfoHFP;
                    btInfo.iSupportedFeatures.At( 0 ) = 1;   
                    btInfo.iSupportedFeatures.At( 1 ) = 2;  
             
                    TASYBTAccInfoPckgC btInfoPckgC( btInfo );                                 
                    ProcessResponseL( btInfoPckgC, KErrNone );
                    }
                    break;
                }
            
          break;
          }

        default:
            {   
            COMPONENT_TRACE( ( _L( "BTASYStub - CBTBTASYStubCmdHandlerBase::ProcessCommandL() - ERROR: unhandled command" ) ) );  
            TRACE_ASSERT_ALWAYS;
            User::Panic(_L("ASY Stub"), KErrGeneral );
            }
            break;      
        }
    COMPONENT_TRACE( ( _L( "ASYStub - CASYStubCmdHandlerBase::ProcessCommandL() - Return" ) ) );
    }

// ----------------------------------------------------------------------------------
// CASYStubCmdHandlerBase::Start()
// ----------------------------------------------------------------------------------
void CASYStubCmdHandlerBase::Start(TInt aTimeMs, TInt64 aValue, TAccPolValueType aValueType )
    {
    iTimer = CPeriodic::NewL( CActive::EPriorityStandard );
    TCallBack callback( TimeOutL, this );
    TTimeIntervalMicroSeconds32 delay( 1000 * aTimeMs );
    iResponse = aValue;
    iResponseType = aValueType;
    iTimer->Start( delay, delay/*not used*/, callback );
    }

// ----------------------------------------------------------------------------------
// CASYStubCmdHandlerBase::TimeOutL()
// ----------------------------------------------------------------------------------
TInt CASYStubCmdHandlerBase::TimeOutL( TAny* aThis )
    {
    static_cast<CASYStubCmdHandlerBase*>( aThis )->DoTimeOutL();
    return KErrNone;
    }

// ----------------------------------------------------------------------------------
// CASYStubCmdHandlerBase::DoTimeOutL()
// ----------------------------------------------------------------------------------
void CASYStubCmdHandlerBase::DoTimeOutL()
    {
    if( iTimer )
        {
        delete iTimer; // One timeout is all what is needed
        iTimer = NULL;
        }
    switch ( iResponseType )
        {
        case ( EAPVBool ):
            {
            TAccValueTypeTBool response = { EFalse };
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

// End of File

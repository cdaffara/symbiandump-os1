/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Maps Generic ID capabilities to AccMonitor capabilities
*
*/


#include <AccPolGenericID.h>
#include <AccPolCommonNameValuePairs.h>
#include <e32std.h>
#include <AccessoryServer.h>
#include <e32svr.h>
#include "AccMonitorCapMapper.h"
#include "acc_debug.h"

// ======== MEMBER FUNCTIONS ========
   
// ---------------------------------------------------------------------------
// C++ constructor.
// ---------------------------------------------------------------------------
CAccMonitorCapMapper::CAccMonitorCapMapper()
    {
    COM_TRACE_( "[AccFW:AccMonitor] CAccMonitorCapMapper::CAccMonitorCapMapper()" );
    
    COM_TRACE_( "[AccFW:AccMonitor] CAccMonitorCapMapper::CAccMonitorCapMapper - return" );
    }

// ---------------------------------------------------------------------------
// CAccMonitor::NewL
// 
// Symbian OS two-phased constructor.
// ---------------------------------------------------------------------------
CAccMonitorCapMapper* CAccMonitorCapMapper::NewL()
    {
    COM_TRACE_( "[AccFW:AccMonitor] CAccMonitorCapMapper::ConstructL()" );
    
    CAccMonitorCapMapper* self = new ( ELeave ) CAccMonitorCapMapper();
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
    COM_TRACE_( "[AccFW:AccMonitor] CAccMonitorCapMapper::ConstructL - return self" );
    return self;
    }

// ---------------------------------------------------------------------------
// CAccMonitor::ConstructL
// 
// Symbian OS two-phased constructor.
// ---------------------------------------------------------------------------
void CAccMonitorCapMapper::ConstructL()
    {
    COM_TRACE_( "[AccFW:AccMonitor] CAccMonitorCapMapper::ConstructL()" );
    
    COM_TRACE_( "[AccFW:AccMonitor] CAccMonitorCapMapper::ConstructL - return" );
    }
    
// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
CAccMonitorCapMapper::~CAccMonitorCapMapper()
    {
    COM_TRACE_( "[AccFW:AccMonitor] CAccMonitorCapMapper::~CAccMonitorCapMapper()" );
    
    COM_TRACE_( "[AccFW:AccMonitor] CAccMonitorCapMapper::~CAccMonitorCapMapper - return" );
    }

// ---------------------------------------------------------------------------
// CAccMonitorCapMapper::GetCapabilitiesFromGIDHeader
// 
// Gets capabilities in GID header. Other items commented in header file.
// ---------------------------------------------------------------------------
TInt CAccMonitorCapMapper::GetCapabilitiesFromGIDHeader(
    const TAccPolGenericID& aGenericIDHeader,
    RAccMonCapabilityArray& aCapabilityArray )
    {
    COM_TRACE_( "[AccFW:AccMonitor] CAccMonitorCapMapper::GetCapabilitiesFromGIDHeader()" );  
    
    TInt err( KErrNone );
    switch( aGenericIDHeader.DeviceTypeCaps() )
        {
        case KDTHeadset:
          aCapabilityArray.Append( KAccMonHeadset );
          break;
        
        case KDTCarKit:
          aCapabilityArray.Append( KAccMonCarKit );
          break;
        
        case KDTOffice:
          aCapabilityArray.Append( KAccMonOffice );
          break;
        
        case KDTMessaging:
          break;
        
        case KDTImaging:
          break;
          
        case KDTLoopset:
          aCapabilityArray.Append( KAccMonLoopset );
          break;
          
        case KDTDataCable:
          break;
          
        case KDTCharger:
          break;
          
        case KDTTTY:
          aCapabilityArray.Append( KAccMonTTY );
          break;
          
        case KDTAVDevice:
          aCapabilityArray.Append( KAccMonAVDevice );
          break;
          
        default:
          break;  
        }
    
    if( KPCWired & aGenericIDHeader.PhysicalConnectionCaps() )
        {
        aCapabilityArray.Append( KAccMonWired );
        }
    if( KPCBluetooth & aGenericIDHeader.PhysicalConnectionCaps() )
        {
        aCapabilityArray.Append( KAccMonBluetooth );
        }
    
    COM_TRACE_1( "[AccFW:AccMonitor] CAccMonitorCapMapper::GetCapabilitiesFromGIDHeader() - return err %d", err );  
    return err;
    }
  
// ---------------------------------------------------------------------------
// CAccMonitorCapMapper::GetCapabilitiesFromSubblockNameArray
// 
// Gets capabilities from capgroups. Other items commented in header file.
// ---------------------------------------------------------------------------
TInt CAccMonitorCapMapper::GetCapabilitiesFromSubblockNameArray(
    const CAccPolSubblockNameArray* aNameArray,
    RAccMonCapabilityArray& aCapabilityArray )
    {
    COM_TRACE_( "[AccFW:AccMonitor] CAccMonitorCapMapper::GetCapabilitiesFromSubblockNameArray()" );
    TInt err( KErrNone );
    
    if( aNameArray->HasName( KAccStereoAudio ) )
        {
        aCapabilityArray.Append( KAccMonStereoAudio );
        }
    if( aNameArray->HasName( KAccMonoAudio ) )
        {
        aCapabilityArray.Append( KAccMonMonoAudio );
        }
    if( aNameArray->HasName( KAccIntegratedAudioOutput ) )
        {
        aCapabilityArray.Append( KAccMonSpeakerAudioOutput );
        }
    if( aNameArray->HasName( KAccIntegratedAudioInput ) )
        {
        aCapabilityArray.Append( KAccMonMicAudioInput );
        }
    if( aNameArray->HasName( KAccBTHSP ) )
        {
        aCapabilityArray.Append( KAccMonBTHSP );
        }
    if( aNameArray->HasName( KAccBTHFP ) )
        {
        aCapabilityArray.Append( KAccMonBTHFP );
        }
    if( aNameArray->HasName( KAccBTAVRCP ) )
        {
        aCapabilityArray.Append( KAccMonBTAVRCP );
        }
    if( aNameArray->HasName( KAccBTA2DP ) )
        {
        aCapabilityArray.Append( KAccMonBTA2DP );
        }
    if( aNameArray->HasName( KAccVideoOut ) )
        {
        aCapabilityArray.Append( KAccMonVideoOut );
        }
  
    COM_TRACE_1( "[AccFW:AccMonitor] CAccMonitorCapMapper::GetCapabilitiesFromSubblockNameArray() - return err %d", err );
    return err;
    }

// ---------------------------------------------------------------------------
// CAccMonitorCapMapper::GetCapabilityFromDefinition
// 
// Gets subblock capability from definition. Other items commented in header.
// ---------------------------------------------------------------------------
TUint32 CAccMonitorCapMapper::GetCapabilityFromDefinitionL( 
    const TAccMonCapability aCapabilityDefinition )
    {
    COM_TRACE_1( "[AccFW:AccMonitor] CAccMonitorCapMapper::GetDeviceAddress() - aCapabilityDefinition %d", aCapabilityDefinition );
    
    TUint32 returnValue( 0 );
    
    if( aCapabilityDefinition < 81 )
        {
        User::Leave( KErrArgument );
        }
    
    switch( aCapabilityDefinition )
        {
        case KAccMonStereoAudio:
          returnValue = KAccStereoAudio;
          break;
        case KAccMonMonoAudio:
          returnValue = KAccMonoAudio;
          break;
        case KAccMonSpeakerAudioOutput:
          returnValue = KAccIntegratedAudioOutput;
          break;
        case KAccMonMicAudioInput:
          returnValue = KAccIntegratedAudioInput;
          break;
        case KAccMonBTHSP:
          returnValue = KAccBTHSP;
          break;
        case KAccMonBTHFP:
          returnValue = KAccBTHFP;
          break;
        case KAccMonBTAVRCP:
          returnValue = KAccBTAVRCP;
          break;
        case KAccMonBTA2DP:
          returnValue = KAccBTA2DP;
          break;
        case KAccMonVideoOut:
          returnValue = KAccVideoOut;
          break;
        default:
          User::Leave( KErrGeneral );
          break;
        }
  
    COM_TRACE_1( "[AccFW:AccMonitor] CAccMonitorCapMapper::GetDeviceAddress() - return returnValue %d", returnValue );
    return returnValue;
    }
  
// ---------------------------------------------------------------------------
// CAccMonitorCapMapper::GetHeaderCapabilityFromDefinitionL
// 
// Gets header capability from capabilities. Other items commented in header file.
// ---------------------------------------------------------------------------
TUint32 CAccMonitorCapMapper::GetHeaderCapabilityFromDefinitionL( 
    const TAccMonCapability aCapabilityDefinition )
    {
    COM_TRACE_1( "[AccFW:AccMonitor] CAccMonitorCapMapper::GetHeaderCapabilityFromDefinitionL() - aCapabilityDefinition %d", aCapabilityDefinition );
    
    TUint32 returnValue( 0 );
    
    if( 
      ( aCapabilityDefinition >= 81 ) && 
      ( aCapabilityDefinition == KAccMonNoDevice ) 
      )
        {
        User::Leave( KErrArgument );
        }
    
    switch( aCapabilityDefinition )
        {
        case KAccMonHeadset:
          returnValue = KDTHeadset;
          break;
        case KAccMonCarKit:
          returnValue = KDTCarKit;
          break;
        case KAccMonOffice:
          returnValue = KDTOffice;
          break;
        case KAccMonLoopset:
          returnValue = KDTLoopset;
          break;
        case KAccMonTTY:
          returnValue = KDTTTY;
          break;
        case KAccMonAVDevice:
          returnValue = KDTAVDevice;
          break;
        case KAccMonWired:
          returnValue = KPCWired;
          break;
        case KAccMonBluetooth:
          returnValue = KPCBluetooth;
          break;  
        default:
          User::Leave( KErrGeneral );
          break;
        }
    COM_TRACE_1( "[AccFW:AccMonitor] CAccMonitorCapMapper::GetHeaderCapabilityFromDefinitionL - return returnValue %d", returnValue );
    
    return returnValue;
    }
// ---------------------------------------------------------------------------
// CAccMonitorCapMapper::GetDeviceAddress
// 
// Gets device address. Other items commented in header file.
// ---------------------------------------------------------------------------
TUint64 CAccMonitorCapMapper::GetDeviceAddress( 
    const TAccPolGenericID& aGenericIDHeader ) const
    {
    COM_TRACE_( "[AccFW:AccMonitor] CAccMonitorCapMapper::GetDeviceAddress()" );
    return aGenericIDHeader.DeviceAddress();
    }
  
// End of file

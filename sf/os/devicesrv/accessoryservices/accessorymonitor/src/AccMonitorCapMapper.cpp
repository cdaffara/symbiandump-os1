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
* Description:  Main functionality of AccMonitorCapMapper
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
EXPORT_C CAccMonitorCapMapper* CAccMonitorCapMapper::NewL()
    {
    COM_TRACE_( "[AccFW:AccMonitor] CAccMonitorCapMapper::NewL()" );
    
    CAccMonitorCapMapper* self = new ( ELeave ) CAccMonitorCapMapper();
    
    COM_TRACE_( "[AccFW:AccMonitor] CAccMonitorCapMapper::NewL - return self" );
    return self;
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
    RAccMonCapabilityArray& aCapabilityArray ) const
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
        
        case KDTLoopset:
            aCapabilityArray.Append( KAccMonLoopset );
            break;
          
        case KDTTTY:
            aCapabilityArray.Append( KAccMonTTY );
            break;
          
        case KDTAVDevice:
            aCapabilityArray.Append( KAccMonAVDevice );
            break;
          
        default:
            err = KErrGeneral;
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
    if( KPCUSB & aGenericIDHeader.PhysicalConnectionCaps() )
        {
        aCapabilityArray.Append( KAccMonUSB );
        }
    if( KPCHDMI & aGenericIDHeader.PhysicalConnectionCaps() )
        {
        aCapabilityArray.Append( KAccMonHDMI );
        }
    
    COM_TRACE_1( "[AccFW:AccMonitor] CAccMonitorCapMapper::GetCapabilitiesFromGIDHeader() - return err %d", err );  
    return err;
    }
  
// ---------------------------------------------------------------------------
// CAccMonitorCapMapper::GetCapabilitiesFromSubblockNameArray
// 
// Gets capabilities from capgroups. Other items commented in header file.
// ---------------------------------------------------------------------------
void CAccMonitorCapMapper::GetCapabilitiesFromSubblockNameArray(
    const CAccPolSubblockNameArray* aNameArray,
    RAccMonCapabilityArray& aCapabilityArray ) const
    {
    COM_TRACE_( "[AccFW:AccMonitor] CAccMonitorCapMapper::GetCapabilitiesFromSubblockNameArray()" );
    
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
    
    COM_TRACE_( "[AccFW:AccMonitor] CAccMonitorCapMapper::GetCapabilitiesFromSubblockNameArray() - return " );
    }
  
// ---------------------------------------------------------------------------
// CAccMonitorCapMapper::GetCapabilityFromDefinition
// 
// Gets subblock capability from definition. Other items commented in header.
// ---------------------------------------------------------------------------
TUint32 CAccMonitorCapMapper::GetCapabilityFromDefinitionL( 
    const TAccMonCapability aCapabilityDefinition ) const
    {
    COM_TRACE_1( "[AccFW:AccMonitor] CAccMonitorCapMapper::GetCapabilityFromDefinitionL() - aCapabilityDefinition %d", aCapabilityDefinition );
    
    TUint32 returnValue( 0 );
    
    if( aCapabilityDefinition < KAccMonStereoAudio )
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
            break;
        }
  
    COM_TRACE_1( "[AccFW:AccMonitor] CAccMonitorCapMapper::GetCapabilityFromDefinitionL() - return returnValue %d", returnValue );
    return returnValue;
    }
  
// End of file

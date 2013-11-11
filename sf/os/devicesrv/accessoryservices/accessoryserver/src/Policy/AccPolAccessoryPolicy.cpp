/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Private, Accessory Policy
*
*/


// INCLUDE FILES
#include <AccPolCommonNameValuePairs.h>
#include <AccPolSubblockNameArray.h>
#include <AccessoryTypes.h>
#include <featmgr.h>
#include "AccPolGenericIDAccessor.h"
#include "AccPolNameValueArraySerial.h"
#include "AccPolAccessoryPolicy.h"
#include "AccPolSubblockNameArrayAccessor.h"
#include "acc_debug.h"
#include "AccPolicyDB.h"
#include "AccPolGenericIDArrayAccessor.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

#ifdef FF_AUTOMOTIVESTACK
const TInt KAccMaxRules = 8;
#else
const TInt KAccMaxRules = 7;
#endif

const TInt  KAccConfigMaxCapabilityGroups = 64;
const TInt  KAccGidIntBuf                 = 100;
const TInt  KCapabilityGroupMaxLength     = 12;//32 bit number presented in decimal format is 10 character lenght

// bit mask
const TUint8 KDT  = 0x1;        // 00000001
const TUint8 KPC  = 0x2;        // 00000010
const TUint8 KAP  = 0x4;        // 00000100
const TUint8 KCG  = 0x8;        // 00001000
const TUint8 KDI  = 0x10;       // 00010000
    

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES
struct TAccPolMultibleConnectionRules
    {
    TUint32 iPhysicalConnection;
    TUint32  iCapabiltyName;
    };

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CAccPolAccessoryPolicy::CAccPolAccessoryPolicy
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CAccPolAccessoryPolicy::CAccPolAccessoryPolicy( CCapabilityStorage * aCapabilityStorage )
  : iCapabilityStorage( aCapabilityStorage ),
    iCriticalCaps( EFalse )
    {
    API_TRACE_( "[AccFW: ACCPOLICY] CAccPolAccessoryPolicy::CAccPolAccessoryPolicy()" );

    API_TRACE_( "[AccFW: ACCPOLICY] CAccPolAccessoryPolicy::CAccPolAccessoryPolicy() - return void" );
    }

// Destructor
EXPORT_C CAccPolAccessoryPolicy::~CAccPolAccessoryPolicy()
    {
    API_TRACE_( "[AccFW: ACCPOLICY] CAccPolAccessoryPolicy::~CAccPolAccessoryPolicy()" );
    
    API_TRACE_( "[AccFW: ACCPOLICY] CAccPolAccessoryPolicy::~CAccPolAccessoryPolicy() - return void" );
    }

// -----------------------------------------------------------------------------
// TAccPolGenericID::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CAccPolAccessoryPolicy::ConstructL()
    {
    API_TRACE_( "[AccFW: ACCPOLICY] CAccPolAccessoryPolicy::ConstructL()" );
    
    API_TRACE_( "[AccFW: ACCPOLICY] CAccPolAccessoryPolicy::ConstructL() - return void" );
    }

// -----------------------------------------------------------------------------
// TAccPolGenericID::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CAccPolAccessoryPolicy* CAccPolAccessoryPolicy::NewL( 
    CCapabilityStorage * aCapabilityStorage )
    {
    API_TRACE_( "[AccFW: ACCPOLICY] CAccPolAccessoryPolicy::NewL()" );

    CAccPolAccessoryPolicy* self = new( ELeave ) CAccPolAccessoryPolicy( aCapabilityStorage );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    API_TRACE_( "[AccFW: ACCPOLICY] CAccPolAccessoryPolicy::CAccPolAccessoryPolicy::NewL() - return" );
    return self;
    }

// -----------------------------------------------------------------------------
// CAccPolAccessoryPolicy::ResolveCriticalCapabilitiesL
// -----------------------------------------------------------------------------
//
EXPORT_C void CAccPolAccessoryPolicy::ResolveCriticalCapabilitiesL(
    const TAccPolGenericID& aGenericID,
    CAccPolSubblockNameArray& aCriticalCapabilities )
    {
    API_TRACE_1( "[AccFW: ACCPOLICY] CAccPolAccessoryPolicy::ResolveCriticalCapabilitiesL() UniqueId: %d", aGenericID.UniqueID() );
        
    if( !iCriticalCaps )
        {
        //At first time, read all critical capabilities from AccessoryPolicyDB and store them to capability storage.           
        GetAllCriticalCapabilitiesL();
        iCriticalCaps = ETrue;
        }

    iCapabilityStorage->ResolveCriticalCapabilitiesL( aGenericID, aCriticalCapabilities );    

    API_TRACE_( "[AccFW: ACCPOLICY] CAccPolAccessoryPolicy::ResolveCriticalCapabilitiesL - return void" );
    }

// -----------------------------------------------------------------------------
// CAccPolAccessoryPolicy::GetAllCriticalCapabilities
//
// Critical capabilities are read from Policy DB
// -----------------------------------------------------------------------------
//
EXPORT_C void CAccPolAccessoryPolicy::GetAllCriticalCapabilitiesL() const
    {
    API_TRACE_( "[AccFW: ACCPOLICY] CAccPolAccessoryPolicy::GetAllCriticalCapabilitiesL()" );
    TUint32 name( 0 ); 
	TInt32  value( 0 );
	TInt capabilityCount( 0 );    

    CAccPolicyDB* db = CAccPolicyDB::NewL();
    CleanupStack::PushL( db );
    TRAPD( err, db->OpenDBL() );
    if ( KErrNone == err )
        {
        // Check do we use type as extra search criteria
        capabilityCount = db->ReadAllCriticalCapabilitiesL();
        //Go throught all critical capabilities and store them
	    for( TInt i = 0; i < capabilityCount; i++ )
	        {        
	        db->GetNextCriticalCapabilityL( name, value );//Get next capability from filtered view...
	        iCapabilityStorage->SetCriticalCapabilityL( name, EAPVNotDefined, EAPVInt, value ); //...and store it 
	        }
        }
    else
        {
        // No need to leave. Result of this is that GenericID contains
        // unique ID with value KErrNotFound.
        capabilityCount = 0;
        }
    

    db->CloseDB();
    CleanupStack::PopAndDestroy( db );

    API_TRACE_( "[AccFW: ACCPOLICY] CAccPolAccessoryPolicy::GetAllCriticalCapabilitiesL - return void" );
    }

// -----------------------------------------------------------------------------
// CAccPolAccessoryPolicy::IsValidConnectionL
//
// Check if connection of the accessory is allowed.
// Connection is allowed if all critical capabilities have non-critical value.
// -----------------------------------------------------------------------------
//
EXPORT_C TAccSrvConnection CAccPolAccessoryPolicy::IsValidConnectionL(
    const TAccPolGenericID& aGenericID,
    TInt& aError )
    {
    API_TRACE_1( "[AccFW: ACCPOLICY] CAccPolAccessoryPolicy::IsValidConnectionL() 0x%x", aGenericID.UniqueID() );

    TAccSrvConnection         ret( EAccSrvConnect );
        
    //Chek if all critical capabilities have correct value
    if( !iCapabilityStorage->CompareCriticalValuesL( aGenericID ) )
        {
        ret = EAccSrvDetect;
        }
    
    //*************************************************************************
    // BT-specific connection checkings
    //
    // 1. It is not possible to connect more than one
    //    Bluetooth Handsfree Profile accessory at any time.
    // 2. It is not possible to connect a Bluetooth Handsfree Profile accessory
    //    while in Bluetooth Headset Profile mode.
    // 3  It is not possible to connect a Bluetooth Headset Profile accessory
    //    while in Bluetooth Handsfree Profile accessory mode.
    // 4. It is not possible to connect more than one
    //    Bluetooth Headset Profile accessory at any time.
    //*******************************************************************
    if( aGenericID.PhysicalConnectionCaps( KPCBluetooth ) )
        {
        API_TRACE_( "[AccFW: ACCPOLICY] CAccPolAccessoryPolicy::IsValidConnectionL() - BT checks" );
        CAccPolSubblockNameArray* nameArray = CAccPolSubblockNameArray::NewL();
        CleanupStack::PushL( nameArray );

        if( iCapabilityStorage->GetCapability( aGenericID, KAccBTHSP))
            {
            // Accessory supports Headset Profile (HSP).
            if( iCapabilityStorage->CapabilityInUse( aGenericID, KAccBTHSP ))
                {
                // Accessory with Headset Profile (HSP) is allready connected
                // #4
                aError = KErrInUse;
                }
            if( iCapabilityStorage->CapabilityInUse( aGenericID, KAccBTHFP ) )
                {
                // Accessory with Handsfree Profile (HFP) is allready connected
                // #3
                aError = KErrInUse;
                }
            }

        if( iCapabilityStorage->GetCapability( aGenericID, KAccBTHFP)  )
            {
            //Accessory supports Handsfree Profile (HFP).
            if( iCapabilityStorage->CapabilityInUse( aGenericID, KAccBTHSP ) )
                {
                // Accessory with Headset Profile (HSP) is allready connected
                // #2
                aError = KErrInUse;
                }
            if( iCapabilityStorage->CapabilityInUse( aGenericID, KAccBTHFP ) )
                {
                // Accessory with Handsfree Profile (HFP) is allready connected
                // #1
                aError = KErrInUse;
                }
            }

        CleanupStack::PopAndDestroy( nameArray );
        }

    API_TRACE_1( "[AccFW: ACCPOLICY] CAccPolAccessoryPolicy::IsValidConnectionL() - return %d", ret );
    return ret;
    }

// -----------------------------------------------------------------------------
// CAccPolAccessoryPolicy::ResolveAccessoryModeL
// Resolve the accessory mode based on the given set of accessories.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TAccPolAccessoryMode CAccPolAccessoryPolicy::ResolveAccessoryModeL(
    const TAccPolGenericIDArray& aGenericIDArray,
    const TInt aPresentUniqueID,
    const TBool aCurrentAudioOutputStatus,
    const TAccPolAccessoryMode aCurrentAccessoryMode,
    const TInt aCurrentAudioOutputStatusUniqueID )
    {

    COM_TRACE_3( "[AccFW: ACCPOLICY] CAccPolAccessoryPolicy::ResolveAccessoryModeL(mode=%d dbID=%d audio=%d)", aCurrentAccessoryMode.iAccessoryMode, aPresentUniqueID, aCurrentAudioOutputStatus );

    TAccPolAccessoryMode accMode( aCurrentAccessoryMode );

    switch ( aPresentUniqueID )
        {
        // KErrUnknown means that HandleAccessoryModeChanged() is
        // NOT called via AudioRoutingStatusNotify() method.
        case KErrUnknown:
            {
            // Check current mode, if audio is already routed.
            if( aCurrentAccessoryMode.iAudioOutputStatus && aGenericIDArray.Count() > 0 )
                {
                TInt index( TAccPolGenericIDArrayAccessor::FindWithUniqueIDL( 
                    aGenericIDArray, aCurrentAudioOutputStatusUniqueID ) );
                if ( KErrNotFound != index )
                    {
                    // Audio is routed to current GID, so resolve mode because this can be accessory settings case.
                    SetAccessoryModeL( aGenericIDArray.GetGenericIDL( index ), accMode );
                    if( accMode.iAccessoryMode == aCurrentAccessoryMode.iAccessoryMode )
                        {
                        accMode.iAudioOutputStatus = aCurrentAccessoryMode.iAudioOutputStatus;
                        }
                    COM_TRACE_2( "[AccFW: ACCPOLICY] CAccPolAccessoryPolicy::ResolveAccessoryModeL - set mode according to current audio output status UniqueID, (UniqueID=%d, mode=%d) ", aCurrentAudioOutputStatusUniqueID, accMode.iAccessoryMode );
                    }
                else
                    {
                    // Case when accessory is removed and GID count > 0.
                    GetAccessoryModeFromRulesTableL( aGenericIDArray, accMode );
                    accMode.iAudioOutputStatus = EFalse;
                    COM_TRACE_2( "[AccFW: ACCPOLICY] CAccPolAccessoryPolicy::ResolveAccessoryModeL - No need to resolve accessory mode, (UniqueID=%d, mode=%d) ", aCurrentAudioOutputStatusUniqueID, accMode.iAccessoryMode );
                    }
                }
            else
                {
                // Set accessory mode using rules table, audio is not routed.
                GetAccessoryModeFromRulesTableL( aGenericIDArray, accMode );
                COM_TRACE_1( "[AccFW: ACCPOLICY] CAccPolAccessoryPolicy::ResolveAccessoryModeL - mode is changed: %d", accMode.iAccessoryMode );
                }
            }
            break;

        // AudioRoutingStatusNotify() method is called with a Gereric ID
        default:
            {
            if ( aCurrentAudioOutputStatus )
                {								
                TInt index( TAccPolGenericIDArrayAccessor::FindWithUniqueIDL( 
                    aGenericIDArray, aPresentUniqueID ) );
                if ( KErrNotFound != index )
                    {
                    // Audio is routed to current GID
                    SetAccessoryModeL( aGenericIDArray.GetGenericIDL( index ), accMode );
                    accMode.iAudioOutputStatus = aCurrentAudioOutputStatus;
                    COM_TRACE_1( "[AccFW: ACCPOLICY] CAccPolAccessoryPolicy::ResolveAccessoryModeL - set mode according to audio routing notification : %d", accMode.iAccessoryMode );
                    }
                else if ( KErrNotFound == aPresentUniqueID )
                    {
                    // AudioRoutingStatusNotify() method is called with empty Generic ID
                    // which means that audio is routed to phone.
                    accMode.iAccessoryMode     = EAccModeHandPortable;
                    accMode.iAudioOutputStatus = aCurrentAudioOutputStatus;
                    COM_TRACE_( "[AccFW: ACCPOLICY] CAccPolAccessoryPolicy::ResolveAccessoryModeL - mode is handportable" );
                    }
                else
                    {
                    // AudioRoutingStatusNotify() is called with Generic ID that doesn't exist anymore
                    GetAccessoryModeFromRulesTableL( aGenericIDArray, accMode );
                    accMode.iAudioOutputStatus = EFalse;
                    }
                }
            else
                {
                // Set accessory mode using rules table.
                GetAccessoryModeFromRulesTableL( aGenericIDArray, accMode );
                accMode.iAudioOutputStatus = aCurrentAudioOutputStatus;
                COM_TRACE_1( "[AccFW: ACCPOLICY] CAccPolAccessoryPolicy::ResolveAccessoryModeL - mode is changed to %d", accMode.iAccessoryMode );
                }
            }
            break;
        }

    COM_TRACE_2( "[AccFW: ACCPOLICY] CAccPolAccessoryPolicy::ResolveAccessoryModeL - return %d (audio=%d)", accMode.iAccessoryMode, accMode.iAudioOutputStatus );

    return accMode;
    }

// -----------------------------------------------------------------------------
// CAccPolAccessoryPolicy::GetAccessoryModeFromRulesTableL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CAccPolAccessoryPolicy::GetAccessoryModeFromRulesTableL(
     const TAccPolGenericIDArray& aGenericIDArray,
     TAccPolAccessoryMode& aAccMode)
    {

    COM_TRACE_( "[AccFW: ACCPOLICY] CAccPolAccessoryPolicy::GetAccessoryModeFromRulesTableL()" );

    TInt rulesCount(0);
    TInt GIDIndex;
    TInt count( aGenericIDArray.Count() );

    // This should allocated from heap in future
    TAccPolMultibleConnectionRules aAccPolConnRules[KAccMaxRules] =
    {
     {KPCHDMI,      0}, 
     {KPCWired,     KAccStereoAudio},
     {KPCWired,     KAccMonoAudio},
     {KPCBluetooth, KAccStereoAudio},
     {KPCBluetooth, KAccMonoAudio},
#ifdef FF_AUTOMOTIVESTACK	 
	 {KPCUSB,   	KAccStereoAudio},
#endif
     {KPCWired,     0},
     {KPCBluetooth, 0}
    };


  rulesCount = sizeof ( aAccPolConnRules ) / sizeof ( TAccPolMultibleConnectionRules );

  for( TInt i = 0; i < rulesCount; i++ )
      {
        for( GIDIndex = 0; GIDIndex < count;  GIDIndex++ )
            {
          TAccPolGenericID gID( aGenericIDArray.GetGenericIDL( GIDIndex ) );

      if( aAccPolConnRules[i].iCapabiltyName != 0 )
          {
          if( gID.PhysicalConnectionCaps(aAccPolConnRules[i].iPhysicalConnection )
               && iCapabilityStorage->GetCapability( gID , aAccPolConnRules[i].iCapabiltyName ))
              {
            i = rulesCount; //break from rules count loop
            COM_TRACE_1( "[AccFW: ACCPOLICY] CAccPolAccessoryPolicy::GetAccessoryModeFromRulesTableL() - mode found (index=%d)",
                          GIDIndex );
            break;
            }
          }
      else
         {
        if( gID.PhysicalConnectionCaps( aAccPolConnRules[i].iPhysicalConnection ) )
            {
            i = rulesCount; //break from rules count loop
            COM_TRACE_1( "[AccFW: ACCPOLICY] CAccPolAccessoryPolicy::GetAccessoryModeFromRulesTableL() - mode found (index=%d)",
                          GIDIndex );
            break;
            }
         }
        }
      }


    if(count == GIDIndex)
        {
        GIDIndex = 0; // Just get first one in GID array to set mode handportable.
        COM_TRACE_( "[AccFW: ACCPOLICY] CAccPolAccessoryPolicy::GetAccessoryModeFromRulesTableL() - mode not found, set default" );
        }

    SetAccessoryModeL( aGenericIDArray.GetGenericIDL( GIDIndex ), aAccMode);

    COM_TRACE_( "[AccFW: ACCPOLICY] CAccPolAccessoryPolicy::GetAccessoryModeFromRulesTableL() - Return void" );
    }

// -----------------------------------------------------------------------------
// CAccPolAccessoryPolicy::SetAccessoryModeL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CAccPolAccessoryPolicy::SetAccessoryModeL(
    const TAccPolGenericID& gID,
    TAccPolAccessoryMode& aAccMode) const
    {

    COM_TRACE_( "[AccFW: ACCPOLICY] CAccPolAccessoryPolicy::SetAccessoryModeL() )" );

    TUint32 physicalCon( gID.PhysicalConnectionCaps() );

    if ( physicalCon == 0  )
        {
        aAccMode.iAccessoryMode = EAccModeHandPortable;
        }
    else if ( gID.PhysicalConnectionCaps( KPCBluetooth ) )
        {
        if( gID.DeviceTypeCaps( KDTHeadset ) )
            {
            aAccMode.iAccessoryMode = EAccModeWirelessHeadset;
            }
        else if( gID.DeviceTypeCaps( KDTCarKit ) )
            {
            aAccMode.iAccessoryMode = EAccModeWirelessCarKit;
            }
        }
    else if ( gID.PhysicalConnectionCaps( KPCWired ) ||
              gID.PhysicalConnectionCaps( KPCHDMI )
#ifdef FF_AUTOMOTIVESTACK
			  || gID.PhysicalConnectionCaps ( KPCUSB )
#endif
			)
        {
        if( gID.DeviceTypeCaps( KDTHeadset ) )
            {
            	
            if( iCapabilityStorage->GetCapability( gID  , KAccIntegratedAudioInput ) )
                {
                aAccMode.iAccessoryMode = EAccModeWiredHeadset;
                }
            else
                {
                aAccMode.iAccessoryMode = EAccModeHeadphones;
                }
            }
        else if( gID.DeviceTypeCaps( KDTCarKit ) )
            {
            aAccMode.iAccessoryMode = EAccModeWiredCarKit;
            }
        else if(  gID.DeviceTypeCaps( KDTLoopset ) )
            {
            aAccMode.iAccessoryMode = EAccModeLoopset;
            }
        else if( gID.DeviceTypeCaps( KDTTTY ) )
            {
            aAccMode.iAccessoryMode = EAccModeTextDevice;
            }
        else if( gID.DeviceTypeCaps( KDTOffice ) )
            {
            //Office device is interpreted as MusicStand if it has
            //mono or stereo audio capability
            if ( iCapabilityStorage->GetCapability( gID, KAccStereoAudio ) || 
                 iCapabilityStorage->GetCapability( gID, KAccMonoAudio ) )
                {
                aAccMode.iAccessoryMode = EAccModeMusicStand;
                }
            else
                {
                aAccMode.iAccessoryMode = EAccModeHandPortable;
                }
            }
        else if( gID.DeviceTypeCaps( KDTAVDevice ) )
            {
            if( gID.SubblockCaps( KSBVideoSubblock ) )
                {
                if( gID.PhysicalConnectionCaps( KPCHDMI ) )
                	{
                	aAccMode.iAccessoryMode = EAccModeHDMI;
                	}
                else
                	{
                	aAccMode.iAccessoryMode = EAccModeTVOut;
                	}
                }
            else    
                {
                aAccMode.iAccessoryMode = EAccModeHandPortable;
                }
            }
        }
    else
        {
        aAccMode.iAccessoryMode = EAccModeHandPortable;
        }

    COM_TRACE_( "[AccFW: ACCPOLICY] CAccPolAccessoryPolicy::SetAccessoryModeL() - return void" );
    }

// -----------------------------------------------------------------------------
// CAccPolAccessoryPolicy::EvaluateConnectionRulesL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CAccPolAccessoryPolicy::EvaluateConnectionRulesL(
    const TAccPolGenericID& aGenericID,
    TUint32& aSelectionListBitMask  )
    {
    TAccPolGenericID staticRule;
    TBuf8<KGIDIntMaxLength> ruleBuf;
    TUint64 ruleIndex(0);
    TBool bMatch = EFalse;

    //Create DB instance
    CAccPolicyDB* db = CAccPolicyDB::NewL();
    CleanupStack::PushL( db );

    //Open DB
    db->OpenDBL();

    //Filter GID by type
    TInt ruleCount = db->FilterGIDByTypeL( EAccRule );

  //Go throught the connection rules.
    for(TInt ii = 0; ii < ruleCount; ii++ )
      {

    //Get next GID from filtered view
    db->GetNextGIDL( ruleIndex, ruleBuf );


    if( ruleBuf.Length() > 0 )
        {

      //Generic ID instance,
        CAccConGenericID* decodedRule = CAccConGenericID::NewL();
        CleanupStack::PushL( decodedRule );

      //  Parse GID
        ParseGenericIDContentL( decodedRule,
                                ruleIndex,
                                KNullDesC,
                                ruleBuf,
                                EAccRule );

        // Return static part of rules
      staticRule = TAccPolGenericIDAccessor::GenericIDRef( decodedRule );


      if( EvaluateGidHeader( aGenericID, staticRule ) )
        {
        bMatch = ETrue;
        db->GetSelectionsL( ruleIndex, aSelectionListBitMask );
        }
      else
        {
        COM_TRACE_( "[AccFW: ACCPOLICY] CAccPolAccessoryPolicy::EvaluateConnectionRulesL() - rule not match" );
        }

      // Match found, stop loop.
      if( bMatch )
        {
        ii = ruleCount;
        }

      CleanupStack::PopAndDestroy( decodedRule );

      }
     
    }

   //Close DB
   db->CloseDB();
   CleanupStack::Pop( db );
   delete db;
}

// -----------------------------------------------------------------------------
// CAccPolAccessoryPolicy::UpdateGenericIDL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CAccPolAccessoryPolicy::UpdateGenericIDL( TAccPolGenericID& aGenericID,
                const TInt& aSelectionIndex )
{
  // Create new GID
    CAccConGenericID* genericId = CAccConGenericID::NewL();
  CleanupStack::PushL( genericId );

  //Parse GID
    TBuf8<KGIDIntMaxLength> gidInt;
  ParseGenericIDContentL( genericId,
                          aSelectionIndex,
                          KNullDesC,
                          gidInt,
                          EAccSelection );

  //Remove old one
  iCapabilityStorage->RemoveCapabilitiesL( aGenericID );

  //and create new gid header
  TAccPolGenericID staticGenericId( TAccPolGenericIDAccessor::GenericIDRef( genericId ));
  TAccPolGIDHeader header( *((TAccPolGIDHeader*)&staticGenericId));
  header.iDBID = aGenericID.UniqueID();

  // Update static header part
  TAccPolGenericIDAccessor::SetGenericIDStaticAttributes( aGenericID, header);
  iCapabilityStorage->SetGenericIDHeaderL( header);

  //Update caps
  RBufReadStream rStrm;
  (void) rStrm.Open( *TAccPolGenericIDAccessor::NameValueBuf( genericId ) );
  rStrm.PushL();
  iCapabilityStorage->SetCapabilitiesL( aGenericID, rStrm );
  rStrm.Close();
  rStrm.Pop();

  //destroy genericId
  CleanupStack::PopAndDestroy( genericId );
}

// -----------------------------------------------------------------------------
// CAccPolAccessoryPolicy::ParseGenericIDContentL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CAccPolAccessoryPolicy::ParseGenericIDContentL(
    CAccConGenericID* const aGenericID,
    TUint64 aHWDeviceID,
    const TDesC& aHWModelID,
    const TDes8& aGidInt,
    TAccGIDtype aType )
    {
    const TChar KDelimiter = ' ';              //space
    const TChar KLineEnd1 = '\n';              //line feed, new line
    const TChar KLineEnd2 = '\r' ;              //carriage return
    const TChar KCapabilitiesEndDelimiter = ',' ;  //after KCapabilitiesEndDelimiter are values

    API_TRACE_2( "[AccFW:SrvUtil] CAccPolAccessoryPolicy::ParseGenericIDContentL(Device HI:0x%x LO:0x%x)", I64HIGH( aHWDeviceID ), I64LOW( aHWDeviceID ) );

    const TChar KAccSpaceSeparator(' ');
    RArray<TAccPolNameValueRecord> nameValueArray;
    CleanupClosePushL( nameValueArray );
    RBufWriteStream wStrm;
    TAccPolGenericID genericID;
    TBuf8<KGIDIntMaxLength> gidInt;

    if ( aType != EAccRule )
        {
        //Read Gid integer from database
        CAccPolicyDB* db = CAccPolicyDB::NewL();
        CleanupStack::PushL( db );
        TRAPD( err, db->OpenDBL() );
        if ( KErrNone == err )
            {
            // Check do we use type as extra search criteria
            if ( EAccUnknown == aType )
                {
                // type is unknown, do not use it  
                db->FindGIDIntL( aHWDeviceID, gidInt );
                TAccPolGenericIDAccessor::SetHWDeviceID( genericID, aHWDeviceID );
                }
            else
                {
                // Type is known, use it to optimize search
                db->FindGIDIntL( aHWDeviceID, gidInt, aType );
                }
            }
        else
            {
            // No need to leave. Result of this is that GenericID contains
            // unique ID with value KErrNotFound.
            TAccPolGenericIDAccessor::SetHWDeviceID( genericID, aHWDeviceID );
            }
        db->CloseDB();
        CleanupStack::Pop( db );
        delete db;
        }
    else
        {
        gidInt.Copy( aGidInt );
        }

    if ( gidInt.Length() > 0 )
        {
        TAccPolGenericIDAccessor::SetDBID( genericID, KAccSrvGenerateGID );

        // Parse static GID Data from GIDInteger
        RArray<TUint64> foundCapabilityGroups;
        CleanupClosePushL( foundCapabilityGroups );
        RArray<TUint32> foundCapability;
        CleanupClosePushL( foundCapability );

        TBuf<KAccGidIntBuf> result;
        TLex8 buf( gidInt );
        TUint32 val(0);
        TInt64 deviceid(0);

        /*** Parse Accessory name, i.e. HW Model ID **************************/
        TBuf<KHWModelIDMaxLength> modelID;
        while ( buf.Peek() != KDelimiter )
            {
            modelID.Append( buf.Get () );
            }
        buf.Inc();// Skip over delimiter between Accessory name and device ID

        if( KErrNone == aHWModelID.Compare( KNullDesC ) )
            {
            TAccPolGenericIDAccessor::SetHWModelID( genericID, modelID );
            }
        else
            {
            TAccPolGenericIDAccessor::SetHWModelID( genericID, aHWModelID );
            }
        /*** Skip device ID **************************************************/
        result.Copy( KNullDesC );
        while ( buf.Peek() != KDelimiter )
            {
            result.Append( buf.Get () );
            }

        // If type is unknown, DB Query result is different and HWDeviceId is already set
        if ( EAccUnknown != aType  )
            {                
            // Device ID
            TLex resDV( result );
            resDV.Val( deviceid, EDecimal );

            TAccPolGenericIDAccessor::SetHWDeviceID( genericID, deviceid );
            }

        buf.Inc(); // Skip over delimiter between device ID and GID Int
        /*** Parse Device Type ***********************************************/
        result.Copy( KNullDesC );
        while ( buf.Peek() != KAccSpaceSeparator && !buf.Eos() )
            {
            result.Append( buf.Get() );
            }

        // DT-def block from GID-integer
        TLex resDT( result );
        resDT.Val( val, EDecimal );
        TAccPolGenericIDAccessor::SetFeatureAttributeL( genericID, KAccPolDTDeviceType, val );

        // Parse Physical Connection
        val = 0;
        result.Copy( KNullDesC );
        buf.Inc();
        while ( buf.Peek() != KAccSpaceSeparator && !buf.Eos() )
            {
            result.Append( buf.Get() );
            }
        TLex resPC( result );
        resPC.Val( val, EDecimal );
        TAccPolGenericIDAccessor::SetFeatureAttributeL( 
            genericID, KAccPolPCPhysicalConnection, val );

        // Parse Application Protocol
        val = 0;
        result.Copy( KNullDesC );
        buf.Inc();
        while ( buf.Peek() != KAccSpaceSeparator && !buf.Eos() )
            {
            result.Append( buf.Get() );
            }
        TLex resAP( result );
        resAP.Val( val, EDecimal );
        TAccPolGenericIDAccessor::SetFeatureAttributeL( 
            genericID, KAccPolAPApplicationProtocol, val );

        // Parse Capability Groups
        foundCapabilityGroups.Reset();
        result.Copy( KNullDesC );
        buf.Inc();
        while ( buf.Peek() != KAccSpaceSeparator && !buf.Eos() )
            {
            result.Append( buf.Get() );
            }
        TLex resSB( result );
        TInt64 val64( 0 ); // Capability Groups from GID-integer
        resSB.Val( val64, EDecimal );
        TUint64 valU64( ( TUint64 ) val64 );
        TAccPolGenericIDAccessor::SetFeatureAttributeL( 
            genericID, KAccPolSBCapabilities, valU64 );

        ParseSubblocksL( valU64, foundCapabilityGroups );

        //set "values" to point to firts value
        TLex8 values;
        values.Assign( buf );
        while ( ( values.Peek() != KLineEnd1 ) &&
                ( values.Peek() != KLineEnd2 ) &&
                ( !values.Eos() ) &&
                ( values.Peek() != KCapabilitiesEndDelimiter ) )
            {
            values.Inc();
            }

        if ( values.Peek() == KCapabilitiesEndDelimiter )
            {
            values.Inc();//Skip the delimiter i.e. ","
            }

        TInt capabilityGroupCount( foundCapabilityGroups.Count() );
        for ( TInt c(0); c < capabilityGroupCount; ++c )
            {
            TInt iterations( 0 ); // Capability Group has 4 namespaces (i.e. 4 times 32 bits)
            do  {
                TBool firstDigit( ETrue ); // If first digit is 0 then do not insert name (empty)
                foundCapability.Reset(); // Clear last iterations results
                buf.Inc();

                TBuf<KCapabilityGroupMaxLength> capabilityGroupVal;
                while ( buf.Peek() != KAccSpaceSeparator && !buf.Eos() )
                    {
                    if ( firstDigit )
                        {
                        firstDigit = EFalse;
                        TChar c = buf.Get();
                        if ( c == '0' )
                            {
                            break; // from while loop, capability is 0 (not used)
                            }
                        else
                            {
                            capabilityGroupVal.Append( c );
                            }
                        }
                    else
                        {
                        capabilityGroupVal.Append( buf.Get() );
                        }
                    }
                TLex lVal( capabilityGroupVal );
                TUint32 capabilityGroupValue(0);
                lVal.Val( capabilityGroupValue, EDecimal );

                ParseNamesL( capabilityGroupValue, foundCapability );

                TInt capabilityIter(0);
                TInt foundCapabilityCount( foundCapability.Count() );
                while ( capabilityIter < foundCapabilityCount )
                    {
                    //Read Location
                    TBuf<2> locationBuf;
                    while ( values.Peek() != KDelimiter &&
                          ( values.Peek() != KLineEnd1 ) &&
                          ( values.Peek() != KLineEnd2 ) &&
                          ( !values.Eos() ) )
                        {
                        locationBuf.Append( values.Get() );
                        }

                    if ( values.Peek() == KDelimiter )
                        {
                        values.Inc();//Skip the delimiter i.e. " "
                        }

                    //Read Value
                    TBuf<KSBMaxValueLength> valueBuff;
                    while ( values.Peek() != KDelimiter &&
                          ( values.Peek() != KLineEnd1 ) &&
                          ( values.Peek() != KLineEnd2 ) &&
                          ( !values.Eos() ) )
                        {
                        valueBuff.Append( values.Get() );
                        }

                    if ( values.Peek() == KDelimiter )
                        {
                        values.Inc();//Skip the delimiter i.e. " "
                        }

                    //Read Type
                    TBuf<2> typeBuf;
                    while ( values.Peek() != KDelimiter &&
                          ( values.Peek() != KLineEnd1 ) &&
                          ( values.Peek() != KLineEnd2 ) &&
                          ( !values.Eos() ) )
                        {
                        typeBuf.Append( values.Get() );
                        }

                    if ( values.Peek() == KDelimiter )
                        {
                        values.Inc();//Skip the delimiter i.e. " "
                        }

                    TUint capability( 0 );

                    capability = BitNumber( foundCapabilityGroups[c] ) << 5;
                    capability = capability + iterations << 5;
                    capability = capability + BitNumber( foundCapability[capabilityIter] );

                    TInt location( 0 );
                    TLex loc( locationBuf );
                    loc.Val( location );

                    TInt type( 0 );
                    TLex typ( typeBuf );
                    typ.Val( type );

                    COM_TRACE_4( "[AccFW:SrvUtil] CAccPolAccessoryPolicy::ParseGenericIDContentL - capability group=(HI:0x%x LO:0x%x) namespace=%d capability=%d", I64HIGH( foundCapabilityGroups[c] ),
                                                                                                                                                I64LOW( foundCapabilityGroups[c] ),
                                                                                                                                                iterations,
                                                                                                                                                foundCapability[capabilityIter] );
                    COM_TRACE_3( "[AccFW:SrvUtil] CAccPolAccessoryPolicy::ParseGenericIDContentL - location=%d type=%d value=%S", location, type, &valueBuff );

                    // Format of capability:
                    // =====================
                    //
                    // 0b 000001 00000 00010
                    //    ------ ----- -----
                    //    |      |     |
                    //    |      |     |_5 bits for capability. Tells which bit is one (*).
                    //    |      |_5 bits for namespace,
                    //    |_6 bits for capability group. Tells which bit is one (*).
                    //
                    // (*):
                    // 0b00 -> bit number 1 is one -> value is 0b0001=1
                    // 0b01 -> bit number 2 is one -> value is 0b0010=2
                    // 0b10 -> bit number 3 is one -> value is 0b0100=4
                    // 0b11 -> bit number 4 is one -> value is 0b1000=8
                    //
                    COM_TRACE_2( "[AccFW:SrvUtil] CAccPolAccessoryPolicy::ParseGenericIDContentL - capability=0x%x 0b%b", capability, capability );

                    //Store value to nameValueArray
                    if( EAPVDes8 != type )
                        {
                        TLex val( valueBuff );
                        TInt value( 0 );
                        val.Val( value );
                        TAccPolNameValueRecord nameValueRecord( capability,
                                                                value,
                                                              ( TAccPolValueType )type,
                                                              ( TAccPolValueLocation )location );
                        nameValueArray.AppendL( nameValueRecord );
                        }
                    ++capabilityIter;
                    }
                ++iterations;
                }
            while ( iterations <= 3 );
            }

        //Assign constructed static Generic ID header to
        //Generic ID given as a parameter
        TAccPolGenericIDAccessor::SetGenericIDStaticAttributes( aGenericID, genericID );
        //Externalize vaslues to Generic ID given as a parameter
        (void) wStrm.Open( *TAccPolGenericIDAccessor::NameValueBuf( aGenericID ) );
        wStrm.PushL();
        TAccPolNameValueArraySerial::ExternalizeL( nameValueArray, wStrm );
        wStrm.CommitL();
        wStrm.Close();
        wStrm.Pop();

        CleanupStack::PopAndDestroy( &foundCapability );//foundCapability.Close() is called;     
        CleanupStack::PopAndDestroy( &foundCapabilityGroups );//foundCapabilityGroups.Close() is called;
        }
    else
        {
        COM_TRACE_( "[AccFW:SrvUtil] Unsupported accessory detected" );
        }

    CleanupStack::PopAndDestroy( &nameValueArray );//nameValueArray.Close() is called;

    API_TRACE_( "[AccFW:SrvUtil] CAccPolAccessoryPolicy::ParseGenericIDContentL -  return void" );
    }

// -----------------------------------------------------------------------------
// CAccPolAccessoryPolicy::ParseSubblocksL
// Detecting subblocks and store them to destination array
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CAccPolAccessoryPolicy::ParseSubblocksL( const TUint64& aVal, RArray<TUint64>& aCaps )
    {
    COM_TRACE_( "[AccFW:SrvUtil] CAccConfigFileParser::ParseSubblocksL()" );

    TUint64 sum( 0 );
    TUint64 curVal( 1 );
    TInt curBit( 0 );

    do {
        COM_TRACE_4( "[AccFW:SrvUtil] CAccConfigFileParser::(Cap) Binary for this roundtrip is HI:0x%x LO:0x%x, total sum is HI:0x%x LO:0x%x", I64HIGH( curVal ), I64LOW( curVal ), I64HIGH( sum ), I64LOW( sum ) );
        COM_TRACE_2( "[AccFW:SrvUtil] CAccConfigFileParser::(Cap) & HI:0x%x LO:0x%x", I64HIGH( curVal & aVal ), I64LOW( curVal & aVal ) );

        // Check if this subblock is defined in SB-def block
        if ( ( curVal & aVal ) == curVal )
            {
            COM_TRACE_2( "[AccFW:SrvUtil] CAccConfigFileParser::(Cap) MATCH! HI:0x%x LO:0x%x", I64HIGH( curVal ), I64LOW( curVal ) );
            aCaps.AppendL( curVal ); // Append to found caps array
            sum += curVal;
            }
        curBit++;
        curVal = 2 * curVal;
        }
    while ( sum < aVal && curBit < KAccConfigMaxCapabilityGroups );

    COM_TRACE_( "[AccFW:SrvUtil] CAccConfigFileParser::ParseSubblocksL - return void" );
    }

// -----------------------------------------------------------------------------
// CAccPolAccessoryPolicy::ParseNamesL
// Detect all subblock names and append them to the gicen array.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CAccPolAccessoryPolicy::ParseNamesL( const TUint32& aVal, RArray<TUint32>& aNames )
    {
    COM_TRACE_( "[AccFW:SrvUtil] CAccConfigFileParser::ParseNamesL()" );
    TUint32 sum( 0 );
    TUint32 curVal( 1 );
    TInt curBit( 0 );

    do {
        COM_TRACE_2( "[AccFW:SrvUtil] CAccConfigFileParser::ParseNamesL Binary for this roundtrip is 0x%x, total sum is 0x%x", curVal, sum );
        COM_TRACE_1( "[AccFW:SrvUtil] CAccConfigFileParser::ParseNamesL & 0x%x", curVal & aVal );
        // Check if this subblock is defined in SB-def block

        if ( ( curVal & aVal ) == curVal )
            {
            COM_TRACE_1( "[AccFW:SrvUtil] (Name) MATCH! 0x%x",curVal );
            aNames.AppendL( curVal ); // Append to found caps array
            sum += curVal;
            }
        curBit++;
        curVal = 2 * curVal;
        }
    while ( sum < aVal && curBit < KAccConfigMaxCapabilityGroups );

    COM_TRACE_( "[AccFW:SrvUtil] CAccConfigFileParser::ParseNamesL - return void" );
    }

// -----------------------------------------------------------------------------
// CAccPolAccessoryPolicy::BitNumber
// Find a bit from aBitmask.
// -----------------------------------------------------------------------------
//
TInt CAccPolAccessoryPolicy::BitNumber( TUint64 aBitmask )
    {
    API_TRACE_2( "[AccFW:SrvUtil] CAccConfigFileParser::BitNumber(HI:0x%x LO:0x%x)", I64HIGH( aBitmask ), I64LOW( aBitmask ) );
    TUint64 bitIterator( 1 );
    TInt    currentBit( 0 );

    do  {
        if ( aBitmask & bitIterator )
            {
            break;//Found a hit
            }
            bitIterator = bitIterator << 1;
            currentBit++;
        }
    while ( currentBit < KAccConfigMaxCapabilityGroups );

    API_TRACE_1( "[AccFW:SrvUtil] CAccConfigFileParser::BitNumber - return %d", currentBit );
    return currentBit;
    }

// -----------------------------------------------------------------------------
// CAccPolAccessoryPolicy::EvaluateGidHeader
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CAccPolAccessoryPolicy::EvaluateGidHeader(
    const TAccPolGenericID& aGenericID,
    const TAccPolGenericID& aGIDRule )
    {

    TUint32 KDoNotEvaluate(0);
    TUint8 ruleMatch(0);
    TUint8 ruleEvaluated(0);

    //Device types
    if ( aGIDRule.DeviceTypeCaps() != KDoNotEvaluate )
        {            
        ruleEvaluated = ( ruleEvaluated | KDT ); // Rule evaluated
        if ( aGIDRule.DeviceTypeCaps() == aGenericID.DeviceTypeCaps() )
            {
            ruleMatch = ( ruleMatch | KDT ); //Rule match
            }
        else
            {
            COM_TRACE_( "[AccFW: ACCPOLICY] CAccPolAccessoryPolicy::EvaluateGidHeader - DeviceType rule not evaluated" );    
            }         
        }
    else
        {
        ruleEvaluated = ( 0xFF & ruleEvaluated );
        }

    //Physical Connection
    if ( aGIDRule.PhysicalConnectionCaps() != KDoNotEvaluate )
        {        
        ruleEvaluated = ( ruleEvaluated | KPC );
        if ( !aGIDRule.PhysicalConnectionCaps() == aGenericID.PhysicalConnectionCaps() )
            {
            ruleMatch = ( ruleMatch | KPC );
            }
        else
            {
            COM_TRACE_( "[AccFW: ACCPOLICY] CAccPolAccessoryPolicy::EvaluateGidHeader - PhysicalConnection rule not evaluated" );        
            }            
        }
    else
        {
        ruleEvaluated = ( 0xFF & ruleEvaluated );
        }

    //Application protocol
    if ( aGIDRule.ApplicationProtocolCaps() != KDoNotEvaluate )
        {
        ruleEvaluated = ( ruleEvaluated | KAP );
        if ( aGIDRule.ApplicationProtocolCaps() == aGenericID.ApplicationProtocolCaps() )
            {
            ruleMatch = ( ruleMatch | KAP );
            }
        else
            {
            COM_TRACE_( "[AccFW: ACCPOLICY] CAccPolAccessoryPolicy::EvaluateGidHeader - ApplicationProtocol rule not evaluated" );          
            }
        }
    else
        {
        ruleEvaluated = ( 0xFF & ruleEvaluated );
        }
    
    //Capability groups
    if ( aGIDRule.SubblockCaps() != KDoNotEvaluate )
        {
        ruleEvaluated = ( ruleEvaluated | KCG );
        if ( aGIDRule.SubblockCaps() == aGenericID.SubblockCaps() )
            {
            ruleMatch = ( ruleMatch | KCG );
            }
        else
            {
            COM_TRACE_( "[AccFW: ACCPOLICY] CAccPolAccessoryPolicy::EvaluateGidHeader - SubblockCaps rule not evaluated" );            
            }
        }
    else
        {
        ruleEvaluated = ( 0xFF & ruleEvaluated );
        }

    //DeviceID
    if( aGIDRule.HWDeviceID() != KDoNotEvaluate )
        {
        ruleEvaluated = ( ruleEvaluated | KDI );
        if ( aGIDRule.HWDeviceID() == aGenericID.HWDeviceID() )
            {
            ruleMatch = ( ruleMatch | KDI );
            }
        else
            {
            COM_TRACE_( "[AccFW: ACCPOLICY] CAccPolAccessoryPolicy::EvaluateGidHeader - HWDeviceID rule not evaluated" );            
            }
        }
    else
        {
        ruleEvaluated = ( 0xFF & ruleEvaluated );
        }

    return ( ( ruleEvaluated != 0x0 ) && ( ruleMatch !=0x0 ) && ( ruleEvaluated == ruleMatch  ) );
    }

// -----------------------------------------------------------------------------
// CAccPolAccessoryPolicy::SupportedHWDevicesL
// -----------------------------------------------------------------------------
//
EXPORT_C TUint32 CAccPolAccessoryPolicy::SupportedHWDevicesL()
    {
    COM_TRACE_( "[AccFW:ACCPOLICY] CAccPolAccessoryPolicy::SupportedHWDevicesL()" );

    TUint32 dummyRuleIndex( 0 );
    TUint32 supportedDevices( 0 );

    //Read All supported Device Types from Accessory Policy DB.
    CAccPolicyDB* db = CAccPolicyDB::NewL();
    CleanupStack::PushL( db );
    db->OpenDBL();
    db->GetSelectionsL( dummyRuleIndex, supportedDevices, ETrue );
    db->CloseDB();
    CleanupStack::Pop( db );
    delete db;

    //AccFW doesnt support Loopsets which can not be identified properly.
    supportedDevices = supportedDevices & ( ~KASLoopset );

    //Support for following features are queried from FeatureManager.
    FeatureManager::InitializeLibL();// Initialize FeatureManager for this thread
    if ( !FeatureManager::FeatureSupported( KFeatureIdPhoneTty ) )
        {
        supportedDevices = supportedDevices & ( ~KASTTY );//Device doesn't support TTY
        }
    if ( !FeatureManager::FeatureSupported( KFeatureIdTvOut ) )
        {
        supportedDevices = supportedDevices & ( ~KASTVOut );//Device doesn't support TV-Out
        }
    FeatureManager::UnInitializeLib();// Uninitialize FeatureManager for this thread

    COM_TRACE_1( "[AccFW:ACCPOLICY] CAccPolAccessoryPolicy::SupportedHWDevicesL - return 0b%b", supportedDevices );
    return supportedDevices;
    }

EXPORT_C TBool CAccPolAccessoryPolicy::IsCapabilityDefinedL(
        const TAccPolGenericID& aGenericId,
        const TUint32& aName )
    {
    if(iCapabilityStorage->GetCapability(aGenericId,aName) != NULL)
        return ETrue;
    else
        return EFalse;
    }
//  End of File

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
* Description:  Singleton Data Storage of Accessory Server
*
*/



// INCLUDE FILES
#include "AccSrvServerModel.h"
#include "AccPolGenericIDAccessor.h"
#include "AccPolGenericIDArrayAccessor.h"
#include "AccPolAccessoryPolicy.h"
#include "acc_debug.h"
#include <e32std.h>

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CAccSrvServerModel::CAccSrvServerModel
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CAccSrvServerModel::CAccSrvServerModel() :
    iASYsLoadCompleted( EFalse ),
    iAudioType( EAccMonoAudio ),
    iValueDataObject( NULL, 0, 0 ),
    iAudioLinkOpenCancelled ( EFalse ),
    iAudioLinkCloseCancelled ( EFalse ),
    iSupportedDevices( 0 ),
    iIdleDetected( EFalse )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvServerModel::CAccSrvServerModel()" );

    iAccMode.iAccessoryMode     = EAccModeHandPortable;
    iAccMode.iAudioOutputStatus = EFalse;
    iUniqueID = KErrUnknown;

    COM_TRACE_( "[AccFW:AccServer] CAccSrvServerModel::CAccSrvServerModel - return" );
    }

// -----------------------------------------------------------------------------
// CAccSrvServerModel::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CAccSrvServerModel::ConstructL()
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvServerModel::ConstructL()" );

    iConnectionArray.Reset();
    iPhysicalConnectionArray.Reset();
    iRegisteredASYs.Reset();
    iASYThreadID.Reset();
    iASYUniqueID.Reset();
    iThreadIDFilterArray.Reset();

    iCapabilityStorage = CCapabilityStorage::NewL();

    COM_TRACE_( "[AccFW:AccServer] CAccSrvServerModel::ConstructL - return void" );
    }

// -----------------------------------------------------------------------------
// CAccSrvServerModel::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CAccSrvServerModel* CAccSrvServerModel::NewL()
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvServerModel::NewL()" );
    CAccSrvServerModel* self = new( ELeave ) CAccSrvServerModel;

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// Destructor
CAccSrvServerModel::~CAccSrvServerModel()
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvServerModel::~CAccSrvServerModel()" );

    if ( KErrNotFound != iLastDisconnectedAccessory.UniqueID() )
        {
        //If iLastDisconnectedAccessory is not "empty",
        //then names/values are never removed from AccPolicyDB for that gid.
        TRACE_ASSERT_ALWAYS;
        }

    if ( iConnectionArray.Count() )
        {
        // Not all connections cleared, assert.
        TRACE_ASSERT_ALWAYS;
        }

    if ( iPhysicalConnectionArray.Count() )
        {
        //all possible gids must be handled properly,
        //      i.e. names/values must be removed from AccPolicyDB
        //      for every gid in iPhysicalConnectionArray
        TRACE_ASSERT_ALWAYS;
        }

    iConnectionArray.Close();
    iPhysicalConnectionArray.Close();
    iRegisteredASYs.Close();
    iASYThreadID.Close();
    iASYUniqueID.Close();
    iThreadIDFilterArray.Close();

    delete iCapabilityStorage;

    COM_TRACE_( "[AccFW:AccServer] CAccSrvServerModel::~CAccSrvServerModel - return" );
    }

// -----------------------------------------------------------------------------
// CAccSrvServerModel::IsConnected
//
// Accessory is connected if it can be found from connection array.
// -----------------------------------------------------------------------------
//
TBool CAccSrvServerModel::IsConnected( const TAccPolGenericID& aGenericID ) const
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvServerModel::IsConnected()" );
    TBool ret( EFalse );

    if ( KErrNotFound != FindGID( aGenericID, iConnectionArray ) )
        {
        ret = ETrue;
        }

    COM_TRACE_1( "[AccFW:AccServer] CAccSrvServerModel::IsConnected - return %d", ret );
    return ret;
    }

// -----------------------------------------------------------------------------
// CAccSrvServerModel::IsConnectedL
//
// Accessory is connected if it can be found from connection array.
// -----------------------------------------------------------------------------
//
void CAccSrvServerModel::IsConnectedL( const TAccPolGenericID& aGenericID ) const
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvServerModel::IsConnectedL()" );

    if ( !IsConnected( aGenericID ) )
        {
        COM_TRACE_( "[AccFW:AccServer] CAccSrvServerModel::IsConnectedL - accessory is not connected" );
        User::Leave( KErrArgument );
        }

    COM_TRACE_( "[AccFW:AccServer] CAccSrvServerModel::IsConnectedL - return void" );
    }

// -----------------------------------------------------------------------------
// CAccSrvServerModel::IsConnectedOrDetectedL
//
// Accessory is connected if it can be found from connection array.
// Accessory is detected if it can be found from physical connections array.
// -----------------------------------------------------------------------------
//
TBool CAccSrvServerModel::IsConnectedOrDetected( const TAccPolGenericID& aGenericID ) const
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvServerModel::IsConnectedOrDetected()" );

    TBool retval( EFalse );

    if ( IsConnected( aGenericID ) || FindGID( aGenericID, iPhysicalConnectionArray ) >= 0 )
        {
        retval = ETrue;
        }

    COM_TRACE_( "[AccFW:AccServer] CAccSrvServerModel::IsConnectedOrDetected - return void" );	
    return retval;
    }            

// -----------------------------------------------------------------------------
// CAccSrvServerModel::IsConnectedOrDetectedL
//
// Accessory is connected if it can be found from connection array.
// Accessory is detected if it can be found from physical connections array.
// -----------------------------------------------------------------------------
//
void CAccSrvServerModel::IsConnectedOrDetectedL( const TAccPolGenericID& aGenericID ) const
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvServerModel::IsConnectedOrDetectedL()" );

    if ( !IsConnected( aGenericID ) )
        {
        if ( KErrNotFound == FindGID( aGenericID, iPhysicalConnectionArray ) )
            {
            User::Leave( KErrArgument );
            }
        }

    COM_TRACE_( "[AccFW:AccServer] CAccSrvServerModel::IsConnectedOrDetectedL - return void" );
    } 

// -----------------------------------------------------------------------------
// CAccSrvServerModel::FindGID
//
// Returns index to GID given as a parameter.
// Comparison is done according HWDeviceID.
// (Comparison can be made also according to the unique DB id)
// -----------------------------------------------------------------------------
//
TInt CAccSrvServerModel::FindGID( const TAccPolGenericID& aGenericID,
                                  const RArray<TAccPolGenericID>& aArray ) const
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvServerModel::FindGID()" );
    TInt index( KErrNotFound );

    for ( TInt i( 0 ) ; i < aArray.Count() ; ++i )
        {
        if ( aGenericID.UniqueID() == aArray.operator[](i).UniqueID() )
            {
            index = i;
            break;// found matching GID (according to HWDeviceID)
                  // ->no need to continue
            }
        }

    COM_TRACE_1( "[AccFW:AccServer] CAccSrvServerModel::FindGID - return %d", index );
    return index;
    }

// -----------------------------------------------------------------------------
// CAccSrvServerModel::AddConnectionL
// -----------------------------------------------------------------------------
//
void CAccSrvServerModel::AddConnectionL( const TAccPolGenericID& aGenericID )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvServerModel::AddConnectionL()" );

    if ( iConnectionArray.Count() >= KMaxGenericIDInstances )
        {
        User::Leave( KErrOverflow );
        }
    else
        {
        iConnectionArray.InsertL( aGenericID, 0 );//i.e. last connected sccessory in index 0
        TAccPolGenericIDAccessor::DebugPrint( aGenericID );
        }

    COM_TRACE_1( "[AccFW:AccServer] CAccSrvServerModel::AddConnectionL - return void - current count = %d", iConnectionArray.Count() );
    }

// -----------------------------------------------------------------------------
// CAccSrvServerModel::UpdateConnectionL
// -----------------------------------------------------------------------------
//
void CAccSrvServerModel::UpdateConnectionL( const TAccPolGenericID& aGenericID )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvServerModel::UpdateConnectionL()" );

	RemoveConnectionL( aGenericID, EFalse );
	AddConnectionL( aGenericID );

    COM_TRACE_1( "[AccFW:AccServer] CAccSrvServerModel::UpdateConnectionL - return void - current count = %d", iConnectionArray.Count() );  	
    }

// -----------------------------------------------------------------------------
// CAccSrvServerModel::AddPhysicalConnectionL
// -----------------------------------------------------------------------------
//
void CAccSrvServerModel::AddPhysicalConnectionL( const TAccPolGenericID& aGenericID )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvServerModel::AddPhysicalConnectionL()" );

    if ( iPhysicalConnectionArray.Count() >= KMaxGenericIDInstances )
        {
        User::Leave( KErrOverflow );
        }
    else
        {
        iPhysicalConnectionArray.InsertL( aGenericID, 0 );//i.e. last connected sccessory in index 0
        TAccPolGenericIDAccessor::DebugPrint( aGenericID );
        }

    COM_TRACE_1( "[AccFW:AccServer] CAccSrvServerModel::AddPhysicalConnectionL - return void - current count = %d", iPhysicalConnectionArray.Count() );
    }

// -----------------------------------------------------------------------------
// CAccSrvServerModel::RemovePhysicalConnectionL
// -----------------------------------------------------------------------------
//
TInt CAccSrvServerModel::RemovePhysicalConnection( const TAccPolGenericID& aGenericID )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvServerModel::RemovePhysicalConnection()" );

    TInt index( FindGID( aGenericID, iPhysicalConnectionArray ) );

    // KErrNotFound is not an error case since accessory can be detected 
    // (stored in PhysicalConnection array) or connected (stored in Connection array)
    if ( KErrNotFound != index )
        {
        iPhysicalConnectionArray.Remove( index );
        }

    COM_TRACE_1( "[AccFW:AccServer] CAccSrvServerModel::RemovePhysicalConnection - return %d", index );

	return index;
    }

// -----------------------------------------------------------------------------
// CAccSrvServerModel::RemoveConnectionL
//
// Move disconnected gid to "iLastDisconnectedAccessory" for later
// names/values removal from AccPolicyDB.
// This is done while disconnecting accessory(aDisconnect=ETrue).
// -----------------------------------------------------------------------------
//
void CAccSrvServerModel::RemoveConnectionL( const TAccPolGenericID& aGenericID,
                                            TBool aDisconnect )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvServerModel::RemoveConnectionL()" );

    TInt index( FindGID( aGenericID, iConnectionArray ) );
    COM_TRACE_1( "[AccFW:AccServer] CAccSrvServerModel::RemoveConnectionL - handling index %x", index );

    if ( aDisconnect )
        {
        if ( KErrNotFound != iLastDisconnectedAccessory.UniqueID() )
            {
            //If iLastDisconnectedAccessory is not "empty" and its overwritten,
            //then names/values are never removed from from AccPolicyDB for that gid.
            //This is an error.
            TRACE_ASSERT_ALWAYS;
            }

        if ( KErrNotFound != index )
            {
            //This is not an error case since accessory can be
            //detected (stored in PhysicalConnection array) or
            //connected (stored in Connection array)
            COM_TRACE_( "[AccFW:AccServer] CAccSrvServerModel::RemoveConnectionL - GID stored" );
            iLastDisconnectedAccessory = iConnectionArray[ index ];
            }
        else
            {
            //Try to search accessory from Physical connection array
            TInt ind( FindGID( aGenericID, iPhysicalConnectionArray ) );
            if ( KErrNotFound != ind )
                {
                iLastDisconnectedAccessory = iPhysicalConnectionArray[ ind ];
                }
            }
        }

    //Remove GID from connection array
    //KErrNotFound is not an error case since accessory can be detected 
    //(stored in PhysicalConnection array) or connected (stored in Connection array)
    if ( KErrNotFound != index )
        {
        iConnectionArray.Remove( index );
        }

    COM_TRACE_( "[AccFW:AccServer] CAccSrvServerModel::RemoveConnectionL - return void" );
    }

// -----------------------------------------------------------------------------
// CAccSrvServerModel::CurrentConnectionStatusL
// -----------------------------------------------------------------------------
//
void CAccSrvServerModel::CurrentConnectionStatusL(
    TAccPolGenericIDArray& aGenericIDArray )
    {
    COM_TRACE_1( "[AccFW:AccServer] CAccSrvServerModel::CurrentConnectionStatusL(0x%x)", &aGenericIDArray );

    if (iConnectionArray.Count() != 0 )
        {
        TAccPolGenericIDArrayAccessor::ReplaceArrayL( aGenericIDArray, iConnectionArray );
        }
    else
        {
        COM_TRACE_( "[AccFW:AccServer] CAccSrvServerModel::CurrentConnectionStatusL - No connected accessories" );
        }

    COM_TRACE_( "[AccFW:AccServer] CAccSrvServerModel::CurrentConnectionStatusL - return void" );
    }

// -----------------------------------------------------------------------------
// CAccSrvServerModel::FindWithUniqueIDL
// -----------------------------------------------------------------------------
//
void CAccSrvServerModel::FindWithUniqueIDL( TInt aUniqueID,
                                            TAccPolGenericID& aGenericID ) const
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvServerModel::FindWithUniqueIDL()" );

    TInt index( 0 );
    TAccPolGenericIDArray genericIDArray;

    TAccPolGenericIDArrayAccessor::ReplaceArrayL( genericIDArray, iConnectionArray );

    index = TAccPolGenericIDArrayAccessor::FindWithUniqueIDL( genericIDArray,
                                                              aUniqueID );

    if ( index < 0 )
        {
        COM_TRACE_( "[AccFW:AccServer] CAccSrvServerModel::FindWithUniqueIDL - Unique ID not found" );
        }
    else
        {
        aGenericID = iConnectionArray.operator[]( index );
        }

    COM_TRACE_( "[AccFW:AccServer] CAccSrvServerModel::FindWithUniqueIDL - return void" );
    }

// -----------------------------------------------------------------------------
// CAccSrvServerModel::FindWithDeviceAddressL
// -----------------------------------------------------------------------------
//
TInt CAccSrvServerModel::FindWithDeviceAddressL( TUint64 aDeviceAddress,
                                                 TInt& aUniqueID ) const
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvServerModel::FindWithDeviceAddressL()" );

    TInt index( 0 );
    TAccPolGenericIDArray genericIDArray;

    TAccPolGenericIDArrayAccessor::ReplaceArrayL( genericIDArray, iConnectionArray );

    index = TAccPolGenericIDArrayAccessor::FindWithDeviceAddressL( genericIDArray,
                                                                   aDeviceAddress );

    if ( index < 0 )
        {
        COM_TRACE_( "[AccFW:AccServer] CAccSrvServerModel::FindWithDeviceAddressL - Device Address not found" );
        }
    else
        {
        aUniqueID = iConnectionArray.operator[]( index ).UniqueID();
        }

    COM_TRACE_1( "[AccFW:AccServer] CAccSrvServerModel::FindWithDeviceAddressL return %d", index );
    return index;
    }

// -----------------------------------------------------------------------------
// CAccSrvServerModel::AccessoryMode
// -----------------------------------------------------------------------------
//
TAccPolAccessoryMode CAccSrvServerModel::AccessoryMode() const
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvServerModel::AccessoryMode()" );
    COM_TRACE_1( "[AccFW:AccServer] CAccSrvServerModel::AccessoryMode - return 0x%x", iAccMode );
    return iAccMode;
    }

// -----------------------------------------------------------------------------
// CAccSrvServerModel::GetLastConnectedAccessoryL
//
// Last connected accessory is allways in index 0.
// -----------------------------------------------------------------------------
//
void CAccSrvServerModel::GetLastConnectedAccessoryL( TAccPolGenericID& aGenericID ) const
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvServerModel::GetLastConnectedAccessoryL()" );

    if ( iConnectionArray.Count() != 0 )
        {
        aGenericID = iConnectionArray[ 0 ];
        }
    else
        {
        TRACE_ASSERT_ALWAYS;
        User::Leave( KErrGeneral );
        }

    COM_TRACE_( "[AccFW:AccServer] CAccSrvServerModel::GetLastConnectedAccessoryL - return" );
    }
	
// -----------------------------------------------------------------------------
// CAccSrvServerModel::GetLastConnectedWiredAccessory
//
// Last connected wired accessory is to be searched in the iConnectionArray 
// -----------------------------------------------------------------------------
//
TBool CAccSrvServerModel::GetLastConnectedWiredAccessory( TAccPolGenericID& aGenericID ) const
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvServerModel::GetLastConnectedWiredAccessoryL()" );
    
    TInt arrCount = 0;
    
    if ( (arrCount = iConnectionArray.Count()) != 0 )
        {
        TUint arrIndex = 0;
        TAccPolGenericID iGenericID;
        
        while(arrIndex < arrCount)
            {
            iGenericID = iConnectionArray[arrIndex];
            
            if( iGenericID.PhysicalConnectionCaps(KPCWired) )
                {
                aGenericID = iGenericID;
                return ETrue;
                }
            arrIndex++;
            }        
        }
        
    COM_TRACE_( "[AccFW:AccServer] CAccSrvServerModel::GetLastConnectedWiredAccessory - return" );
    
    return EFalse;
    
    }


// -----------------------------------------------------------------------------
// CAccSrvServerModel::SetAccessoryMode
// -----------------------------------------------------------------------------
//
TBool CAccSrvServerModel::SetAccessoryMode( TAccPolAccessoryMode aAccMode, TInt aUniqueID )
    {
    COM_TRACE_3( "[AccFW:AccServer] CAccSrvServerModel::SetAccessoryMode(mode=%d audio=%d uniqueid=%d)", aAccMode.iAccessoryMode, aAccMode.iAudioOutputStatus, aUniqueID );
    TBool changed( EFalse );

    if ( aAccMode.iAccessoryMode == iAccMode.iAccessoryMode 
     && aAccMode.iAudioOutputStatus == iAccMode.iAudioOutputStatus 
     && iUniqueID == aUniqueID )
        {
        COM_TRACE_( "[AccFW:AccServer] CAccSrvServerModel::SetAccessoryMode - Mode not changed" );
        }
    else
        {               	
        if ( KErrUnknown == aUniqueID )
	        {
	        COM_TRACE_( "[AccFW:AccServer] CAccSrvServerModel::SetAccessoryMode - UniqueID not changed" );
	        iAccMode  = aAccMode;
        	changed   = ETrue;
	      	}
	    else
	      	{
        	iUniqueID = aUniqueID;	
            iAccMode  = aAccMode;
        	changed   = ETrue;
        	}
        }
    COM_TRACE_1( "[AccFW:AccServer] CAccSrvServerModel::SetAccessoryMode - return %d", changed );
    return changed;
    }

// -----------------------------------------------------------------------------
// CAccSrvServerModel::SetASYsLoadCompleted
// -----------------------------------------------------------------------------
//
void CAccSrvServerModel::SetASYsLoadCompleted()
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvServerModel::SetASYsLoadCompleted()" );
    iASYsLoadCompleted = ETrue;
    COM_TRACE_( "[AccFW:AccServer] CAccSrvServerModel::SetASYsLoadCompleted" );
    }

// -----------------------------------------------------------------------------
// CAccSrvServerModel::ASYsLoadCompleted
// -----------------------------------------------------------------------------
//
TBool CAccSrvServerModel::ASYsLoadCompleted() const
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvServerModel::ASYsLoadCompleted()" );
    COM_TRACE_1( "[AccFW:AccServer] CAccSrvServerModel::ASYsLoadCompleted return 0x%x", iASYsLoadCompleted );
    return iASYsLoadCompleted;
    }

// -----------------------------------------------------------------------------
// CAccSrvServerModel::AudioType()
// -----------------------------------------------------------------------------
//
TAccAudioType CAccSrvServerModel::AudioType() const
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvServerModel::AudioType()" );
    COM_TRACE_1( "[AccFW:AccServer] CAccSrvServerModel::AudioType - return 0x%x", iAudioType );
    return iAudioType;
    }

// -----------------------------------------------------------------------------
// CAccSrvServerModel::SetAudioType
// -----------------------------------------------------------------------------
//
void CAccSrvServerModel::SetAudioType( TAccAudioType aAudioType )
    {
    COM_TRACE_1( "[AccFW:AccServer] CAccSrvServerModel::SetAudioType(0x%x)", aAudioType );
    iAudioType = aAudioType;
    COM_TRACE_( "[AccFW:AccServer] CAccSrvServerModel::SetAudioType - return void" );
    }

// -----------------------------------------------------------------------------
// CAccSrvServerModel::DeviceType
// -----------------------------------------------------------------------------
//
TUint32 CAccSrvServerModel::DeviceType() const
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvServerModel::DeviceType()" );
    COM_TRACE_1( "[AccFW:AccServer] CAccSrvServerModel::DeviceType() - return 0x%x", iDeviceType );
    return iDeviceType;
    }

// -----------------------------------------------------------------------------
// CAccSrvServerModel::SetDeviceType
// -----------------------------------------------------------------------------
//
void CAccSrvServerModel::SetDeviceType( TUint32 aDeviceType )
    {
    COM_TRACE_1( "[AccFW:AccServer] CAccSrvServerModel::SetDeviceType(0x%x)", aDeviceType );
    iDeviceType = aDeviceType;
    COM_TRACE_( "[AccFW:AccServer] CAccSrvServerModel::SetDeviceType - return void" );
    }

// -----------------------------------------------------------------------------
// CAccSrvServerModel::SupportedDevicesL
// -----------------------------------------------------------------------------
//
TUint32 CAccSrvServerModel::SupportedDevicesL()
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvServerModel::SupportedDevicesL()" );
    //get supported devices from policy (only at first time)
	if ( !iSupportedDevices )
	    {
		CAccPolAccessoryPolicy * policy = CAccPolAccessoryPolicy::NewL( 0 );
		CleanupStack::PushL( policy );

		iSupportedDevices = policy->SupportedHWDevicesL();    

		CleanupStack::PopAndDestroy( policy );
	    }
    COM_TRACE_( "[AccFW:AccServer] CAccSrvServerModel::SupportedDevicesL() - return supported devices" );
	return iSupportedDevices;
    }

// -----------------------------------------------------------------------------
// CAccSrvServerModel::LastDisconnectedAccessory
// -----------------------------------------------------------------------------
//
const TAccPolGenericID& CAccSrvServerModel::LastDisconnectedAccessory()
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvServerModel::LastDisconnectedAccessory()" );
    COM_TRACE_( "[AccFW:AccServer] CAccSrvServerModel::LastDisconnectedAccessory - return gid" );
    return iLastDisconnectedAccessory;
    }

// -----------------------------------------------------------------------------
// CAccSrvServerModel::ClearLastDisconnectedAccessory
// -----------------------------------------------------------------------------
//
void CAccSrvServerModel::ClearLastDisconnectedAccessory()
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvServerModel::ClearLastDisconnectedAccessory()" );
    TAccPolGenericID emptyGid;
    iLastDisconnectedAccessory = emptyGid;
    COM_TRACE_( "[AccFW:AccServer] CAccSrvServerModel::ClearLastDisconnectedAccessory - return void" );
    }

// -----------------------------------------------------------------------------
// CAccSrvServerModel::SetValueData
// -----------------------------------------------------------------------------
//
void CAccSrvServerModel::SetValueData( TAccPolGenericID& aGenericId,
                                       TAccPolNameRecord& aNameRecord,
                                       TAccValueTypeTInt aValueTint,
                                       TAccValueTypeTBool aValueTBool,
                                       TAccValueTypeObject aValueObject )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvServerModel::SetValueData()" );
    iValueDataGenericId  = aGenericId;
    iValueDataNameRecord = aNameRecord;
    iValueDataTint       = aValueTint;
    iValueDataTBool      = aValueTBool;
    iValueDataObject.Set( aValueObject.iValue );
    COM_TRACE_( "[AccFW:AccServer] CAccSrvServerModel::SetValueData - return void" );
    }

// -----------------------------------------------------------------------------
// CAccSrvServerModel::ValueDataGid
// -----------------------------------------------------------------------------
//
TAccPolGenericID CAccSrvServerModel::ValueDataGid() const
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvServerModel::ValueDataGid()" );
    return iValueDataGenericId;
    }

// -----------------------------------------------------------------------------
// CAccSrvServerModel::ValueDataNameRecord
// -----------------------------------------------------------------------------
//
TAccPolNameRecord CAccSrvServerModel::ValueDataNameRecord() const
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvServerModel::ValueDataNameRecord()" );
    return iValueDataNameRecord;
    }

// -----------------------------------------------------------------------------
// CAccSrvServerModel::ValueDataTInt
// -----------------------------------------------------------------------------
//
TAccValueTypeTInt CAccSrvServerModel::ValueDataTInt() const
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvServerModel::ValueDataTInt()" );
    return iValueDataTint;
    }

// -----------------------------------------------------------------------------
// CAccSrvServerModel::ValueDataTBool
// -----------------------------------------------------------------------------
//
TAccValueTypeTBool CAccSrvServerModel::ValueDataTBool() const
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvServerModel::ValueDataTBool()" );
    return iValueDataTBool;
    }

// -----------------------------------------------------------------------------
// CAccSrvServerModel::ValueDataObject
// -----------------------------------------------------------------------------
//
TAccValueTypeObject CAccSrvServerModel::ValueDataObject() const
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvServerModel::ValueDataObject()" );
    TAccValueTypeObject valueTypeObject( iValueDataObject );
    return valueTypeObject;
    }

// -----------------------------------------------------------------------------
// CAccSrvServerModel::AddASYThreadIdL
// -----------------------------------------------------------------------------
//
void CAccSrvServerModel::AddASYThreadIdL( TUint aThreadID )
    {
    COM_TRACE_1( "[AccFW:AccServer] CAccSrvServerModel::AddASYThreadIdL(%d)", aThreadID );
    iRegisteredASYs.AppendL( aThreadID );
    }

// -----------------------------------------------------------------------------
// CAccSrvServerModel::StoreASYThreadIDL
//
// If aThreadID can be found from connected ASYs table (iRegisteredASYs),
// mapping between aUniqueID and aThreadID can be made.
// This means that ASY has made accessory connection and that ASY will serve
// all request for that gineric ID.
// -----------------------------------------------------------------------------
//
void CAccSrvServerModel::StoreASYThreadIDL( TUint aThreadID, TInt aUniqueID )
    {
    COM_TRACE_2( "[AccFW:AccServer] CAccSrvServerModel::StoreASYThreadIDL(%d %d)", aThreadID, aUniqueID );
    if ( KErrNotFound == iRegisteredASYs.Find( aThreadID ) )
        {
        COM_TRACE_( "[AccFW:AccServer] CAccSrvServerModel::StoreASYThreadIDL - Thread ID is not ASYs" );
        }
    else
        {
        iASYThreadID.AppendL( aThreadID );
        iASYUniqueID.AppendL( aUniqueID );
        }
    }

// -----------------------------------------------------------------------------
// CAccSrvServerModel::RemoveASYThreadID
// -----------------------------------------------------------------------------
//
void CAccSrvServerModel::RemoveASYThreadID( TInt aUniqueID )
    {
    COM_TRACE_1( "[AccFW:AccServer] CAccSrvServerModel::RemoveASYThreadID(%d)", aUniqueID );
    TInt index( iASYUniqueID.Find( aUniqueID ) );

    //If index is KErrNotFound, threadIDs are never stored in CAccSrvServerModel::StoreASYThreadIDL()
    if ( KErrNotFound != index )
        {
        iASYThreadID.Remove( index );
        iASYUniqueID.Remove( index );
        }
    COM_TRACE_( "[AccFW:AccServer] CAccSrvServerModel::RemoveASYThreadID - return" );
    }

// -----------------------------------------------------------------------------
// CAccSrvServerModel::IsMyRequest
// -----------------------------------------------------------------------------
//
TBool CAccSrvServerModel::IsMyRequest( TUint aThreadID, TInt aUniqueID )
    {
    COM_TRACE_2( "[AccFW:AccServer] CAccSrvServerModel::IsMyRequest(%d %d)", aThreadID, aUniqueID );

    TBool ret( EFalse );
    TInt index( iASYUniqueID.Find( aUniqueID ) );
    COM_TRACE_1( "[AccFW:AccServer] CAccSrvServerModel::IsMyRequest - handling index %d", index );

    // If KErrNotFound, Generic ID / ASY thread ID mapping is not done
    // (because accessory connection is not made by ASY).
    if ( KErrNotFound != index )
        {
        if ( iASYThreadID.operator[]( index ) == aThreadID )
            {
            //match
            COM_TRACE_2( "[AccFW:AccServer] CAccSrvServerModel::IsMyRequest - found %d %d", iASYThreadID.operator[]( index ), iASYUniqueID.operator[]( index ) );
            ret = ETrue;
            }
        }

    COM_TRACE_1( "[AccFW:AccServer] CAccSrvServerModel::IsMyRequest - return %d", ret );
    return ret;
    }

// -----------------------------------------------------------------------------
// CAccSrvServerModel::SetMsgThreadIDFilterL
// -----------------------------------------------------------------------------
//
void CAccSrvServerModel::SetMsgThreadIDFilterL( TUint aThreadID, TMsgID aMsgID )
    {
    COM_TRACE_2( "[AccFW:AccServer] CAccSrvServerModel::SetMsgThreadIDFilterL(%d %d)", aThreadID, aMsgID );

    TAccSrvThreadIDFilter threadIDFilter;
    threadIDFilter.iThreadID = aThreadID;
    threadIDFilter.iMsgID = aMsgID;

    iThreadIDFilterArray.AppendL( threadIDFilter );

    COM_TRACE_1( "[AccFW:AccServer] CAccSrvServerModel::SetMsgThreadIDFilterL - count=%d - return void", iThreadIDFilterArray.Count() );
    }

// -----------------------------------------------------------------------------
// CAccSrvServerModel::FindAndDeleteMsgThreadIDFilter
// -----------------------------------------------------------------------------
//
TBool CAccSrvServerModel::FindAndDeleteMsgThreadIDFilter( TUint aThreadID, TMsgID aMsgID )
    {
    COM_TRACE_2( "[AccFW:AccServer] CAccSrvServerModel::FindAndDeleteMsgThreadIDFilter(%d %d)", aThreadID, aMsgID );

    TBool ret( EFalse );
    TInt count( iThreadIDFilterArray.Count() );

    for ( TInt i( 0 ) ; i < count ; ++i )
        {
        if ( iThreadIDFilterArray.operator[](i).iThreadID == aThreadID && 
             iThreadIDFilterArray.operator[](i).iMsgID == aMsgID )
            {
            COM_TRACE_1( "[AccFW:AccServer] CAccSrvServerModel::FindAndDeleteMsgThreadIDFilter - match found in index %d", i );
            iThreadIDFilterArray.Remove( i );
            ret = ETrue;
            break;
            }
        }

    COM_TRACE_1( "[AccFW:AccServer] CAccSrvServerModel::FindAndDeleteMsgThreadIDFilter - return %d", ret );
    return ret;
    }

// -----------------------------------------------------------------------------
// CAccSrvServerModel::SetCancelledConnectAccessoryL (UniqueID)
// -----------------------------------------------------------------------------
//
void CAccSrvServerModel::SetCancelledConnectAccessoryL( TInt aUniqueID )
    {
    COM_TRACE_1( "[AccFW:AccServer] CAccSrvServerModel::SetCancelledConnectAccessoryL(%d)", aUniqueID );
    TAccPolGenericIDArray genericIDArray;
    TInt index( KErrNotFound );

    //Find the GenericID in question.
    //ConnectAccessory() can be cancelled only for accessories in detected state
    TAccPolGenericIDArrayAccessor::ReplaceArrayL( genericIDArray, iPhysicalConnectionArray );
    index = TAccPolGenericIDArrayAccessor::FindWithUniqueIDL( genericIDArray, aUniqueID );

    if ( index < 0 )
        {
        //Connection of this accessory can not be cancelled
        }
    else
        {
        COM_TRACE_1( "[AccFW:AccServer] CAccSrvServerModel::SetCancelledConnectAccessoryL - handling index %d", index );
        iCancelledConnectAccessory.AppendL( genericIDArray.GetGenericIDL( index ).UniqueID() );
        }

    COM_TRACE_( "[AccFW:AccServer] CAccSrvServerModel::SetCancelledConnectAccessoryL - return void" );
    }

// -----------------------------------------------------------------------------
// CAccSrvServerModel::SetCancelledConnectAccessoryL (DeviceAddress)
// -----------------------------------------------------------------------------
//
void CAccSrvServerModel::SetCancelledConnectAccessoryL( TUint64 aDeviceAddress )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvServerModel::SetCancelledConnectAccessoryL(DeviceAddress)" );
    TAccPolGenericIDArray genericIDArray;
    TInt index( KErrNotFound );

    //Find the GenericID in question.
    //ConnectAccessory() can be cancelled only for accessories in detected state
    TAccPolGenericIDArrayAccessor::ReplaceArrayL( genericIDArray, 
                                                  iPhysicalConnectionArray );

    index = TAccPolGenericIDArrayAccessor::FindWithDeviceAddressL( genericIDArray, 
                                                                   aDeviceAddress );

    if ( index < 0 )
        {
        //Connection of this accessory can not be cancelled
        }
    else
        {
        COM_TRACE_1( "[AccFW:AccServer] CAccSrvServerModel::SetCancelledConnectAccessoryL - handling index %d", index );
        iCancelledConnectAccessory.AppendL( genericIDArray.GetGenericIDL( index ).UniqueID() );
        }

    COM_TRACE_( "[AccFW:AccServer] CAccSrvServerModel::SetCancelledConnectAccessoryL - DeviceAddress - return void" );
    }

// -----------------------------------------------------------------------------
// CAccSrvServerModel::FindAndRemoveCancelledConnectAccessory
// -----------------------------------------------------------------------------
//
TBool CAccSrvServerModel::FindAndRemoveCancelledConnectAccessory(
    TInt aUniqueID )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvServerModel::FindAndRemoveCancelledConnectAccessory()" );
    TAccPolGenericIDArray genericIDArray;
    TInt index( KErrNotFound );
    TBool ret( EFalse );

    index = iCancelledConnectAccessory.Find( aUniqueID );

    if ( index < 0 )
        {
        //Connection is not cancelled
        }
    else
        {
        COM_TRACE_1( "[AccFW:AccServer] CAccSrvServerModel::FindAndRemoveCancelledConnectAccessory - cancelled index %d", index );
        iCancelledConnectAccessory.Remove( index );
        ret = ETrue;
        }

    COM_TRACE_1( "[AccFW:AccServer] CAccSrvServerModel::FindAndRemoveCancelledConnectAccessory - return %d", ret );
    return ret;
    }

// -----------------------------------------------------------------------------
// CAccSrvServerModel::FindAndRemoveCancelledConnectAccessory
// -----------------------------------------------------------------------------
//
CCapabilityStorage& CAccSrvServerModel::CapabilityStorage()
    {
	return *iCapabilityStorage;
    }

// -----------------------------------------------------------------------------
// CAccSrvServerModel::SetAudioLinkOpenCancelled
// -----------------------------------------------------------------------------
//
void CAccSrvServerModel::SetAudioLinkOpenCancelled( TBool aCancelled )
    {
    COM_TRACE_1( "[AccFW:AccServer] CAccSrvServerModel::SetAudioLinkOpenCancelled(%d)", aCancelled );
    iAudioLinkOpenCancelled = aCancelled;
    COM_TRACE_( "[AccFW:AccServer] CAccSrvServerModel::SetAudioLinkOpenCancelled - return void" );
    }

// -----------------------------------------------------------------------------
// CAccSrvServerModel::AudioLinkOpenCancelled
// -----------------------------------------------------------------------------
//
TBool CAccSrvServerModel::AudioLinkOpenCancelled()
    {
    COM_TRACE_1( "[AccFW:AccServer] CAccSrvServerModel::AudioLinkOpenCancelled()- return %d", iAudioLinkOpenCancelled );
    return iAudioLinkOpenCancelled;
    }

// -----------------------------------------------------------------------------
// CAccSrvServerModel::SetAudioLinkCloseCancelled
// -----------------------------------------------------------------------------
//    
void CAccSrvServerModel::SetAudioLinkCloseCancelled( TBool aCancelled )
    {
    COM_TRACE_1( "[AccFW:AccServer] CAccSrvServerModel::SetAudioLinkCloseCancelled(%d)", aCancelled );
    iAudioLinkCloseCancelled = aCancelled;
    COM_TRACE_( "[AccFW:AccServer] CAccSrvServerModel::SetAudioLinkCloseCancelled - return void" );
    }

// -----------------------------------------------------------------------------
// CAccSrvServerModel::AudioLinkCloseCancelled
// -----------------------------------------------------------------------------
//   
TBool CAccSrvServerModel::AudioLinkCloseCancelled()
    {
    COM_TRACE_1( "[AccFW:AccServer] CAccSrvServerModel::AudioLinkCloseCancelled() - return %d", iAudioLinkCloseCancelled );
    return iAudioLinkCloseCancelled;
    }

// -----------------------------------------------------------------------------
// CAccSrvServerModel::SetIdleDetected
// -----------------------------------------------------------------------------
//   
void CAccSrvServerModel::SetIdleDetected()
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvServerModel::SetIdleDetected()" );

	//idle detected   	
  	iIdleDetected = ETrue;

 	if ( iIdleStatus )
 	    {
 		//complete outstanding requests
 		User::RequestComplete( iIdleStatus, KErrNone );
 	    }
    COM_TRACE_( "[AccFW:AccServer] CAccSrvServerModel::SetIdleDetected() - return void" );
    }

// -----------------------------------------------------------------------------
// CAccSrvServerModel::IdleDetected
// -----------------------------------------------------------------------------
//   
TBool CAccSrvServerModel::IdleDetected()
    {
    COM_TRACE_1( "[AccFW:AccServer] CAccSrvServerModel::IdleDetected() - return %d", iIdleDetected );
  	return iIdleDetected;
    }

// -----------------------------------------------------------------------------
// CAccSrvServerModel::WaitForIdle
// -----------------------------------------------------------------------------
//   
TInt CAccSrvServerModel::WaitForIdle( TRequestStatus* iStatus)
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvServerModel::WaitForIdle()" );

  	//idle already detected
  	if ( iIdleDetected )
  	    {
  		return KErrGeneral;
  	    }

  	iIdleStatus = iStatus;
    COM_TRACE_( "[AccFW:AccServer] CAccSrvServerModel::WaitForIdle() - return" );
  	return KErrNone;
    }

// -----------------------------------------------------------------------------
// CAccSrvServerModel::CancelWaitForIdle
// -----------------------------------------------------------------------------
//
void CAccSrvServerModel::CancelWaitForIdle()
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvServerModel::CancelWaitForIdle()");
    if ( iIdleStatus )
 	    {
 		//complete outstanding requests
 		User::RequestComplete( iIdleStatus, KErrCancel );
        iIdleStatus = NULL;
 	    }
    }

// -----------------------------------------------------------------------------
// CAccSrvServerModel::BluetoothAudioLinkOpenReqPostponed
// -----------------------------------------------------------------------------
//    
TBool CAccSrvServerModel::BluetoothAudioLinkOpenReqPostponed()
    {
    COM_TRACE_1( "[AccFW:AccServer] CAccSrvServerModel::BluetoothAudioLinkOpenReqPostponed() return %d", iBluetoothAudioLinkOpenReqPostponed );
    return iBluetoothAudioLinkOpenReqPostponed;
    }

// -----------------------------------------------------------------------------
// CAccSrvServerModel::SetBluetoothAudioLinkOpenReqPostponed
// -----------------------------------------------------------------------------
//   
void CAccSrvServerModel::SetBluetoothAudioLinkOpenReqPostponed( TBool aValue )
    {
    COM_TRACE_1( "[AccFW:AccServer] CAccSrvServerModel::SetBluetoothAudioLinkOpenReqPostponed(%d)", aValue );
    iBluetoothAudioLinkOpenReqPostponed = aValue;
    COM_TRACE_( "[AccFW:AccServer] CAccSrvServerModel::SetBluetoothAudioLinkOpenReqPostponed() - return" );
    }

//  End of File

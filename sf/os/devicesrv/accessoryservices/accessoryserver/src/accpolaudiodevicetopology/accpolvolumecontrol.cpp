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
* Description:  Implementation for volume control object.
*
*/

#include <accpolvolumecontrol.h>
#include "acc_debug.h"

const TUint16 KAccVolumeIncreaseMax = 0x7FFF;
const TUint16 KAccVolumeIncreaseMin = 0;
const TUint16 KAccVolumeDecreaseMax = 0xFFFF;
const TUint16 KAccVolumeDecreaseMin = 0x8000;
const TUint16 KAccVolumeStep = 256;
// -----------------------------------------------------------------------------
// CAccPolVolumeControl::CAccPolVolumeControl()
// -----------------------------------------------------------------------------
//
CAccPolVolumeControl::CAccPolVolumeControl( CAccPolObjectCon* aObject ) :
    CAccPolAudioControlBase( EAccPolAudioVolumeControlObject, aObject )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolVolumeControl::CAccPolVolumeControl() - Enter" );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolVolumeControl::CAccPolVolumeControl() - Return" );
    }

// -----------------------------------------------------------------------------
// CAccPolMuteControl::CAccPolMuteControl()
// -----------------------------------------------------------------------------
//
CAccPolVolumeControl::CAccPolVolumeControl( TUint32 aChannel,
    const TReal aVolumeDb,
    const TReal aMinVolumeDb,
    const TReal aMaxVolumeDb,
    const TReal aVolumeResDb ) :
    CAccPolAudioControlBase( EAccPolAudioVolumeControlObject, aChannel )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolVolumeControl::CAccPolVolumeControl() - Enter" );
    iVolumeDb = aVolumeDb;
    iMinVolumeDb = aMinVolumeDb;
    iMaxVolumeDb = aMaxVolumeDb;
    iVolumeResDb = aVolumeResDb;
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolVolumeControl::CAccPolVolumeControl() - Return" );
    }

// ---------------------------------------------------------------------------
// CAccPolVolumeControl::NewL()
// ---------------------------------------------------------------------------
//
EXPORT_C CAccPolVolumeControl* CAccPolVolumeControl::NewL(
    CAccPolObjectCon* aObject )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolVolumeControl::NewL() - Enter" );
    CAccPolVolumeControl* self = CAccPolVolumeControl::NewLC( aObject );
    CleanupStack::Pop( self );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolVolumeControl::NewL() - Return" );
    return self;
    }

// ---------------------------------------------------------------------------
// CAccPolVolumeControl::NewLC()
// ---------------------------------------------------------------------------
//
EXPORT_C CAccPolVolumeControl* CAccPolVolumeControl::NewLC(
    CAccPolObjectCon* aObject )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolVolumeControl::NewLC() - Enter" );
    CAccPolVolumeControl* self = new ( ELeave ) CAccPolVolumeControl( aObject );
    CleanupStack::PushL( self );
    self->ConstructL();
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolVolumeControl::NewLC() - Return" );
    return self;
    }

// ---------------------------------------------------------------------------
// CAccPolVolumeControl::NewL()
// ---------------------------------------------------------------------------
//
EXPORT_C CAccPolVolumeControl* CAccPolVolumeControl::NewL( TUint32 aChannel,
    const TReal aVolumeDb,
    const TReal aMinVolumeDb,
    const TReal aMaxVolumeDb,
    const TReal aVolumeResDb )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolVolumeControl::NewL() - Enter" );
    CAccPolVolumeControl* self = CAccPolVolumeControl::NewLC( aChannel,
        aVolumeDb, aMinVolumeDb, aMaxVolumeDb, aVolumeResDb );

    CleanupStack::Pop( self );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolVolumeControl::NewL() - Return" );
    return self;
    }

// ---------------------------------------------------------------------------
// CAccPolVolumeControl::NewLC()
// ---------------------------------------------------------------------------
//
EXPORT_C CAccPolVolumeControl* CAccPolVolumeControl::NewLC( TUint32 aChannel,
    const TReal aVolumeDb,
    const TReal aMinVolumeDb,
    const TReal aMaxVolumeDb,
    const TReal aVolumeResDb )

    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolVolumeControl::NewLC() - Enter" );
    CAccPolVolumeControl* self = new ( ELeave ) CAccPolVolumeControl( aChannel,
        aVolumeDb, aMinVolumeDb, aMaxVolumeDb, aVolumeResDb );

    CleanupStack::PushL( self );
    self->ConstructL();
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolVolumeControl::NewLC() - Return" );
    return self;
    }

// ---------------------------------------------------------------------------
// CAccPolVolumeControl::ConstructL()
// ---------------------------------------------------------------------------
//
void CAccPolVolumeControl::ConstructL()
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolVolumeControl::ConstructL() - Enter" );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolVolumeControl::ConstructL() - Return" );
    }

// Destructor
EXPORT_C CAccPolVolumeControl::~CAccPolVolumeControl()
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolVolumeControl::~CAccPolVolumeControl() - Enter" );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolVolumeControl::~CAccPolVolumeControl() - Return" );
    }

// -----------------------------------------------------------------------------
// CAccPolVolumeControl::MapToDb()
// Maps volume level from USB specific integer value to real decibel value
// -----------------------------------------------------------------------------
// 
TReal CAccPolVolumeControl::MapToDb( TUint16 aValue ) const
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolVolumeControl::MapToDb() - Enter" );
    TReal value;
    TReal integer = aValue;
    TReal step = KAccVolumeStep;

    if( ( aValue <= KAccVolumeIncreaseMax )
        && ( aValue > KAccVolumeIncreaseMin ) )
        {
        value = integer / step;
        }
    else
        if( ( aValue <= KAccVolumeDecreaseMax ) && ( aValue
            >= KAccVolumeDecreaseMin ) )
            {
            value = -( ( ( static_cast<TReal> ( KAccVolumeDecreaseMax )
                - integer ) + 1 ) / step );
            }
        else
            {
            value = aValue;
            }
    API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolVolumeControl::MapToDb() - Return=%f", value );
    return value;
    }

// -----------------------------------------------------------------------------
// CAccPolVolumeControl::CAccPolVolumeControl()
// Maps volume level from decibel value to USB specific integer value
// -----------------------------------------------------------------------------
// 
TUint16 CAccPolVolumeControl::MapFromDb( TReal aValue ) const
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolVolumeControl::MapFromDb() - Enter" );
    TUint16 value;
    TReal step = KAccVolumeStep;

    if( aValue > 0 )
        {
        value = ( aValue * step );
        }
    else
        if( aValue < 0 )
            {
            value = ( ( static_cast<TReal> ( KAccVolumeDecreaseMax ) + ( aValue
                * step ) ) + 1 );
            }
        else
            {
            value = 0;
            }

    API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolVolumeControl::MapFromDb() - Return=%d", value );
    return value;
    }

// ---------------------------------------------------------------------------
// CAccPolVolumeControl::operator()
// ---------------------------------------------------------------------------
//
EXPORT_C CAccPolVolumeControl& CAccPolVolumeControl::operator=(
    const CAccPolVolumeControl& aVolumeControlObject )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolVolumeControl::operator() - Enter" );
    CAccPolAudioControlBase::operator=(
        ( CAccPolAudioControlBase& )aVolumeControlObject );
    iVolumeDb = aVolumeControlObject.VolumeDb();
    iMinVolumeDb = aVolumeControlObject.MinVolumeDb();
    iMaxVolumeDb = aVolumeControlObject.MaxVolumeDb();
    iVolumeResDb = aVolumeControlObject.VolumeResDb();
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolVolumeControl::operator() - Return" );
    return ( *this );
    }

// ---------------------------------------------------------------------------
// CAccPolVolumeControl::SetVolumeLevel()
// ---------------------------------------------------------------------------
//   
EXPORT_C void CAccPolVolumeControl::SetVolumeDb( TReal aVolumeDb )
    {
    API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolVolumeControl::SetVolumeDb() - Volume level=%f", iVolumeDb );
    iVolumeDb = aVolumeDb;
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolVolumeControl::SetVolumeDb() - Return" );
    }

// ---------------------------------------------------------------------------
// CAccPolVolumeControl::SetVolumeLevel()
// ---------------------------------------------------------------------------
//   
EXPORT_C void CAccPolVolumeControl::SetMinVolumeDb( TReal aMinVolumeDb )
    {
    API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolVolumeControl::SetMinVolumeDb() - Min volume level=%f", iMinVolumeDb );
    iMinVolumeDb = aMinVolumeDb;
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolVolumeControl::SetMinVolumeDb() - Return" );
    }

// ---------------------------------------------------------------------------
// CAccPolVolumeControl::SetVolumeLevel()
// ---------------------------------------------------------------------------
//   
EXPORT_C void CAccPolVolumeControl::SetMaxVolumeDb( TReal aMaxVolumeDb )
    {
    API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolVolumeControl::SetMaxVolumeDb() - Max volume level=%f", iMaxVolumeDb );
    iMaxVolumeDb = aMaxVolumeDb;
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolVolumeControl::SetMaxVolumeDb() - Return" );
    }

// ---------------------------------------------------------------------------
// CAccPolVolumeControl::SetVolumeResDb()
// ---------------------------------------------------------------------------
//   
EXPORT_C void CAccPolVolumeControl::SetVolumeResDb( TReal aVolumeResDb )
    {
    API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolVolumeControl::SetVolumeResDb() -  Volume level res=%f", iVolumeResDb );
    iVolumeResDb = aVolumeResDb;
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolVolumeControl::SetVolumeResDb() - Return" );
    }

// ---------------------------------------------------------------------------
// CAccPolVolumeControl::VolumeLevel()
// ---------------------------------------------------------------------------
//       
EXPORT_C TReal CAccPolVolumeControl::VolumeDb() const
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolVolumeControl::VolumeDb() - Enter" );
    API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolVolumeControl::VolumeDb() - Return=%f", iVolumeDb );
    return iVolumeDb;
    }

// ---------------------------------------------------------------------------
// CAccPolVolumeControl::VolumeLevel()
// ---------------------------------------------------------------------------
//       
EXPORT_C TReal CAccPolVolumeControl::MinVolumeDb() const
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolVolumeControl::MinVolumeDb() - Enter" );
    API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolVolumeControl::MinVolumeDb() - Return=%f", iMinVolumeDb );
    return iMinVolumeDb;
    }

// ---------------------------------------------------------------------------
// CAccPolVolumeControl::VolumeLevel()
// ---------------------------------------------------------------------------
//       
EXPORT_C TReal CAccPolVolumeControl::MaxVolumeDb() const
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolVolumeControl::MaxVolumeDb() - Enter" );
    API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolVolumeControl::MaxVolumeDb() - Return=%f", iMaxVolumeDb );
    return iMaxVolumeDb;
    }

// ---------------------------------------------------------------------------
// CAccPolVolumeControl::VolumeResDb()
// ---------------------------------------------------------------------------
//       
EXPORT_C TReal CAccPolVolumeControl::VolumeResDb() const
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolVolumeControl::VolumeResDb() - Enter" );
    API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolVolumeControl::VolumeResDb() - Return=%f", iVolumeResDb );
    return iVolumeResDb;
    }

// ---------------------------------------------------------------------------
// CAccPolVolumeControl::ExternalizeL()
// ---------------------------------------------------------------------------
//   
void CAccPolVolumeControl::ExternalizeL( RWriteStream& aStream ) const
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolMuteControl::ExternalizeL() - Enter" );

    //Add object type at the beginning
    aStream.WriteUint8L( ObjectType() );

    // Externalize base.
    ExternalizeAudioControlbaseL( aStream );

    //Volume level
    aStream.WriteUint16L( MapFromDb( iVolumeDb ) );
    aStream.WriteUint16L( MapFromDb( iMinVolumeDb ) );
    aStream.WriteUint16L( MapFromDb( iMaxVolumeDb ) );
    aStream.WriteUint16L( MapFromDb( iVolumeResDb ) );
    API_TRACE_( "[AccFW:A CCPOLAUDIODEVICETOPOLOGY] CAccPolMuteControl::ExternalizeL - Return" );
    }

// ---------------------------------------------------------------------------
// CAccPolVolumeControl::InternalizeL()
// ---------------------------------------------------------------------------
//  
void CAccPolVolumeControl::InternalizeL( RReadStream& aStream )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolMuteControl::InternalizeL() - Enter" );
    InternalizeAudioControlbaseL( aStream );
    iVolumeDb = MapToDb( aStream.ReadUint16L() );
    iMinVolumeDb = MapToDb( aStream.ReadUint16L() );
    iMaxVolumeDb = MapToDb( aStream.ReadUint16L() );
    iVolumeResDb = MapToDb( aStream.ReadUint16L() );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolMuteControl::InternalizeL - Return" );
    }

// ---------------------------------------------------------------------------
// CAccPolVolumeControl::DuplicateL()
// ---------------------------------------------------------------------------
//
CAccPolObjectBase* CAccPolVolumeControl::DuplicateLC( CAccPolObjectCon* aObject )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolVolumeControl::DuplicateLC() - Enter" );
    CAccPolVolumeControl* aVolume = CAccPolVolumeControl::NewLC( aObject );
    *aVolume = ( CAccPolVolumeControl& )*this;
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolVolumeControl::DuplicateLC() - Return" );
    return aVolume;
    }

//  End of File 

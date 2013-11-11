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
* Description:  Implementation for audio feature unit object.
*
*/

#include <accpolaudiounitbase.h>
#include <accpolaudiotopologyobjectcon.h>
#include <accpolvolumecontrol.h>
#include <accpolmutecontrol.h>
#include <accpolinputcontrol.h>
#include "acc_debug.h"

// -----------------------------------------------------------------------------
// CAccPolAudioUnitBase::CAccPolAudioUnitBase
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CAccPolAudioUnitBase::CAccPolAudioUnitBase(
    const TTopologyConObjectType aObjecType,
    CAccPolObjectCon* aObject ) :
    CAccPolObjectBase( aObjecType, aObject )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioUnitBase::CAccPolAudioUnitBase() -Enter" );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioUnitBase::CAccPolAudioUnitBase() - Return" );
    }

// -----------------------------------------------------------------------------
// CAccPolAudioUnitBase::CAccPolAudioUnitBase
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CAccPolAudioUnitBase::CAccPolAudioUnitBase( const TUint8 aUnitId,
    const TTopologyConObjectType aObjecType ) :
    CAccPolObjectBase( aObjecType )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioUnitBase::CAccPolAudioUnitBase() - Enter" );
    iUnitId = aUnitId;
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioUnitBase::CAccPolAudioUnitBase() - Return" );
    }

// -----------------------------------------------------------------------------
// CAccPolAudioUnitBase::CAccPolAudioUnitBase
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CAccPolAudioUnitBase::CAccPolAudioUnitBase( const TUint8 aUnitId,
    const TUint8 aSourceId,
    const TTopologyConObjectType aObjecType ) :
    CAccPolObjectBase( aObjecType )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioUnitBase::CAccPolAudioUnitBase() -Enter" );
    iUnitId = aUnitId;
    iSourceId.Append( aSourceId );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioUnitBase::CAccPolAudioUnitBase() - Return" );
    }

// Destructor
EXPORT_C CAccPolAudioUnitBase::~CAccPolAudioUnitBase()
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioUnitBase::~CAccPolAudioUnitBase() - Enter" );
    iSourceId.Close();
    TInt count = iAudioControls.Count();
    if( count > 0 )
        {
        for( TInt i = 0; i < count; i++ )
            {
            delete iAudioControls.operator[]( i );
            }
        }
    iAudioControls.Close();
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioUnitBase::~CAccPolAudioUnitBase() - Return" );
    }

// ---------------------------------------------------------------------------
// CAccPolAudioUnitBase::operator()
// ---------------------------------------------------------------------------
//
EXPORT_C CAccPolAudioUnitBase& CAccPolAudioUnitBase::operator=(
    const CAccPolAudioUnitBase& aAudioUnitBaseObject )
    {
    TRAP_IGNORE ( CopyL ( aAudioUnitBaseObject ) );
    
    return ( *this );
    }

void CAccPolAudioUnitBase::CopyL ( const CAccPolAudioUnitBase& aAudioUnitBaseObject )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioUnitBase::operator() - Enter" );
    // Copy base	
    CAccPolObjectBase::operator=( ( CAccPolObjectBase& )aAudioUnitBaseObject );

    //Copy Unit id
    iUnitId = aAudioUnitBaseObject.UnitId();
    RArray<TInt> sourceIds;
    CleanupClosePushL( sourceIds );
    
    //Copy source Ids
    aAudioUnitBaseObject.GetSourceIds( sourceIds );

    for( TInt i = 0; i < sourceIds.Count(); i++ )
        {
        iSourceId.AppendL( sourceIds.operator[]( i ) );
        }

    //Copy previous unit id
    iPreUnitId = aAudioUnitBaseObject.PreviousUnitId();

    //Copy audio controls.
    RPointerArray<CAccPolAudioControlBase> audioControls;
    CleanupClosePushL( audioControls );
    
    aAudioUnitBaseObject.GetAudioControlsL( audioControls );
    
    for( TInt j = 0; j < audioControls.Count(); j++ )
        {
        TUint8 objectType;
        objectType = audioControls.operator[]( j )->ObjectType();

        switch( objectType )
            {
            case EAccPolAudioVolumeControlObject:
                {
                CAccPolVolumeControl *volumeControl =
                    CAccPolVolumeControl::NewL();
                CleanupStack::PushL( volumeControl );
                *volumeControl
                    = *( CAccPolVolumeControl* )audioControls.operator[]( j );
                iAudioControls.AppendL( volumeControl );
                CleanupStack::Pop();
                }
                break;

            case EAccPolAudioMuteControlObject:
                {
                CAccPolMuteControl *muteControl = CAccPolMuteControl::NewL();
                CleanupStack::PushL( muteControl );
                *muteControl
                    = *( CAccPolMuteControl* )audioControls.operator[]( j );
                iAudioControls.AppendL( muteControl );
                CleanupStack::Pop();
                }
                break;

            case EAccPolAudioInputControlObject:
                {
                CAccPolInputControl *inputControl = CAccPolInputControl::NewL();
                CleanupStack::PushL( inputControl );
                *inputControl
                    = *( CAccPolInputControl* )audioControls.operator[]( j );
                iAudioControls.AppendL( inputControl );
                CleanupStack::Pop();
                }
                break;

            default:
                {
                User::Leave( KErrNotSupported );
                }
                break;
            }
        }

    CleanupStack::PopAndDestroy (); //audioControls...dont use ResetAndDestroy on this!
    CleanupStack::PopAndDestroy (); //sourceIds
        
    
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioUnitBase::operator() - Return" );
    }

// ---------------------------------------------------------------------------
// CAccPolAudioUnitBase::PreviousUnitId
// ---------------------------------------------------------------------------
//
TUint8 CAccPolAudioUnitBase::PreviousUnitId() const
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioUnitBase::PreviousUnitId() - Enter" );
    API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioUnitBase::PreviousUnitId() - Return %d", iPreUnitId );
    return iPreUnitId;
    }

// ---------------------------------------------------------------------------
// CAccPolAudioUnitBase::SetPreviousUnitId
// ---------------------------------------------------------------------------
//
void CAccPolAudioUnitBase::SetPreviousUnitId( TUint8 aPreUnitId )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioUnitBase::SetPreviousUnitId() - Enter" );
    API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioUnitBase::SetPreviousUnitId() - PreUnit Id %d", aPreUnitId );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioUnitBase::SetPreviousUnitId() - Return" );
    iPreUnitId = aPreUnitId;
    }

// ---------------------------------------------------------------------------
// CAccPolAudioUnitBase::UnitId
// ---------------------------------------------------------------------------
//
EXPORT_C TUint8 CAccPolAudioUnitBase::UnitId() const
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioUnitBase::UnitId() - Enter" );
    API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioUnitBase::UnitId() - Return %d", iUnitId );
    return iUnitId;
    }

// ---------------------------------------------------------------------------
// CAccPolAudioUnitBase::SourceId
// ---------------------------------------------------------------------------
//
EXPORT_C TUint8 CAccPolAudioUnitBase::SourceId() const
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioUnitBase::SourceId() - Enter" );
    API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioUnitBase::SourceId() - Return %d", iSourceId.operator[]( 0 ) );
    return ( TUint8 )iSourceId.operator[]( 0 );
    }

// ---------------------------------------------------------------------------
// CAccPolAudioUnitBase::GetSourceIds
// ---------------------------------------------------------------------------
//
EXPORT_C void CAccPolAudioUnitBase::GetSourceIds( RArray<TInt>& aSourceIds ) const
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioUnitBase::SourceIds() - Enter" );
    TInt count = iSourceId.Count();
    for( TInt i = 0; i < count; i++ )
        {
        API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioUnitBase::SourceIds() Return SourceId=%d", iSourceId.operator[]( i ) );
        aSourceIds.Append( ( TInt )iSourceId.operator[]( i ) );
        }
    }

// ---------------------------------------------------------------------------
// CAccPolAudioUnitBase::SourceIdCount
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CAccPolAudioUnitBase::SourceIdCount() const
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioUnitBase::SourceIdCount() -Enter" );
    API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioUnitBase::SourceIdCount() return count=%d", iSourceId.Count() );
    return iSourceId.Count();
    }

// ---------------------------------------------------------------------------
// CAccPolAudioUnitBase::AddSourceIdL
// ---------------------------------------------------------------------------
//
EXPORT_C void CAccPolAudioUnitBase::AddSourceIdL( TUint8 aSourceId )
    {
    API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioUnitBase::AddSourceIdL() Enter - Source Id=%d", aSourceId );
    iSourceId.AppendL( aSourceId );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioUnitBase::AddSourceIdL() - Return" );
    }

// ---------------------------------------------------------------------------
// CAccPolAudioUnitBase::GetAudioControlL
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CAccPolAudioUnitBase::GetAudioControlL(
    TTopologyConObjectType aObjectType,
    TUint32 aChannel,
    CAccPolAudioControlBase*& aAudioControl ) const
    {
    API_TRACE_2( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioUnitBase::GetAudioControlL() - Object type=%d, Channel number=%d", aObjectType, aChannel );
    TBool bFound( EFalse );
    aAudioControl = NULL;
    if( aObjectType == EAccPolAudioVolumeControlObject || aObjectType
        == EAccPolAudioMuteControlObject )
        {
        TInt count = iAudioControls.Count();
        for( TInt i = 0; i < count; i++ )
            {
            if( iAudioControls.operator[]( i )->ObjectType() == aObjectType
                && iAudioControls.operator[]( i )->Channel() == aChannel )
                {
                aAudioControl = iAudioControls.operator[]( i );
                bFound = ETrue;
                break;
                }
            }
        }
    else
        {
        User::Leave( KErrGeneral );
        }
    API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioUnitBase::GetAudioControlL() - Return=%d", bFound );
    return bFound;
    }
// ---------------------------------------------------------------------------
// CAccPolAudioUnitBase::GetAudioControlsL
// ---------------------------------------------------------------------------
//
EXPORT_C void CAccPolAudioUnitBase::GetAudioControlsL( TUint32 aChannel,
    RPointerArray<CAccPolAudioControlBase>& aAudioControls ) const
    {
    API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioUnitBase::GetAudioControlsL() - Channel number=%d", aChannel );
    TInt count = iAudioControls.Count();
    API_TRACE_1( "[AccFW: ACCPOLICY] CAccPolAudioUnitBase::GetAudioControlsL() - count=%d", count );
    for( TInt i = 0; i < count; i++ )
        {
        if( iAudioControls.operator[]( i )->Channel() == aChannel )
            {
            aAudioControls.AppendL( iAudioControls.operator[]( i ) );
            }
        }
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioUnitBase::GetAudioControlL() Return" );
    }

// ---------------------------------------------------------------------------
// CAccPolAudioUnitBase::GetAudioControlsL
// ---------------------------------------------------------------------------
//
EXPORT_C void CAccPolAudioUnitBase::GetAudioControlsL(
    RPointerArray<CAccPolAudioControlBase>& aAudioControls ) const
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioUnitBase::GetAudioControlsL() -Enter" );
    TInt count = iAudioControls.Count();
    API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioUnitBase::GetAudioControlsL() - count=%d", count );
    for( TInt i = 0; i < count; i++ )
        {
        aAudioControls.AppendL( iAudioControls.operator[]( i ) );
        }
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioUnitBase::GetAudioControlsL() - Return" );
    }

// ---------------------------------------------------------------------------
// CAccPolAudioUnitBase::GetAudioControlsL
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CAccPolAudioUnitBase::GetNextAudioUnitL(
    CAccPolAudioUnitBase*& aAudioUnit ) const
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioUnitBase::GetNextAudioUnitL() - Enter" );
    RPointerArray<CAccPolAudioUnitBase> audioUnits;
    TBool bFound = EFalse;

    reinterpret_cast<CAccPolAudioTopologyObjectCon*> ( iObjectContainer )->GetAudioUnitsL(
        audioUnits );

    TInt count = audioUnits.Count();

    aAudioUnit = NULL;
    for( TInt i = 0; i < count; i++ )
        {

        if( iSourceId.Count() != 0 ) //JNI
            {
            API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] SourceId - count=%d", iSourceId.Count() );
            TInt sourceid = iSourceId.operator[]( 0 );
            if( iSourceId.operator[]( 0 )
                == audioUnits.operator[]( i )->UnitId() )
                {
                aAudioUnit = audioUnits.operator[]( i );
                aAudioUnit->SetPreviousUnitId( UnitId() );
                bFound = ETrue;
                break;
                }
            }
        }

    API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioUnitBase::GetNextAudioUnitL() - Return=%d", bFound );
    return bFound;
    }

// ---------------------------------------------------------------------------
// CAccPolAudioUnitBase::GetAudioControlsL
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CAccPolAudioUnitBase::GetPreviousAudioUnitL(
    CAccPolAudioUnitBase*& aAudioUnit ) const
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioUnitBase::GetPreviousAudioUnitL() - Enter" );
    TBool bFound = EFalse;
    RPointerArray<CAccPolAudioUnitBase> audioUnits;

    reinterpret_cast<CAccPolAudioTopologyObjectCon*> ( iObjectContainer )->GetAudioUnitsL(
        audioUnits );
    TInt count = audioUnits.Count();

    aAudioUnit = NULL;
    for( TInt i = 0; i < count; i++ )
        {
        if( PreviousUnitId() == audioUnits.operator[]( i )->UnitId() )
            {
            aAudioUnit = audioUnits.operator[]( i );
            bFound = ETrue;
            break;
            }
        }
    API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioUnitBase::GetPreviousAudioUnitL() - Return=%d", bFound );
    return bFound;
    }

// ---------------------------------------------------------------------------
// CAccPolAudioUnitBase::GetAudioControlsL
// ---------------------------------------------------------------------------
//
EXPORT_C void CAccPolAudioUnitBase::GetAudioUnitListL(
    RPointerArray<CAccPolAudioUnitBase>& aAudioUnits ) const
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioUnitBase::GetAudioUnitListL() - Enter" );
    RPointerArray<CAccPolAudioUnitBase> audioUnits;

    reinterpret_cast<CAccPolAudioTopologyObjectCon*> ( iObjectContainer )->GetAudioUnitsL(
        audioUnits );
    TInt audioUnitCount = audioUnits.Count();
    TInt sourceIdCount = iSourceId.Count();
    API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioUnitBase::GetAudioUnitListL() - Source id count=%d", sourceIdCount );
    for( TInt i = 0; i < sourceIdCount; i++ )
        {
        for( TInt j = 0; j < audioUnitCount; j++ )
            {
            if( iSourceId.operator[]( i )
                == audioUnits.operator[]( j )->UnitId() )
                {
                aAudioUnits.AppendL( audioUnits.operator[]( j ) );
                audioUnits.operator[]( j )->SetPreviousUnitId( UnitId() ); //JNI               
                }
            }
        }
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioUnitBase::GetAudioUnitListL() - Return" );
    }

// ---------------------------------------------------------------------------
// CAccPolAudioUnitBase::AddControlL
// ---------------------------------------------------------------------------
//
EXPORT_C void CAccPolAudioUnitBase::AddControlL(
    CAccPolAudioControlBase& aAudioControl )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioUnitBase::AddControlL() - Enter" );
    aAudioControl.SetUnitId( iUnitId );

    TUint8 objectType;
    objectType = aAudioControl.ObjectType();

    switch( objectType )
        {
        case EAccPolAudioVolumeControlObject:
            {
            CAccPolVolumeControl *volumeControl = CAccPolVolumeControl::NewL();
            CleanupStack::PushL( volumeControl );
            *volumeControl = ( CAccPolVolumeControl& )aAudioControl;
            iAudioControls.AppendL( volumeControl );
            CleanupStack::Pop();
            }
            break;

        case EAccPolAudioMuteControlObject:
            {
            CAccPolMuteControl *muteControl = CAccPolMuteControl::NewL();
            CleanupStack::PushL( muteControl );
            *muteControl = ( CAccPolMuteControl& )aAudioControl;
            iAudioControls.AppendL( muteControl );
            CleanupStack::Pop();
            }
            break;

        case EAccPolAudioInputControlObject:
            {
            CAccPolInputControl *inputControl = CAccPolInputControl::NewL();
            CleanupStack::PushL( inputControl );
            *inputControl = ( CAccPolInputControl& )aAudioControl;
            iAudioControls.AppendL( inputControl );
            CleanupStack::Pop();
            }
            break;

        default:
            {
            User::Leave( KErrNotSupported );
            }
            break;
        }

    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioUnitBase::AddControlL() - Return" );
    }
// ---------------------------------------------------------------------------
// CAccPolObjectBase::ExternalizeBaseLd
// ---------------------------------------------------------------------------
//
EXPORT_C void CAccPolAudioUnitBase::ExternalizeAudioUnitBaseL(
    RWriteStream& aStream ) const
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioUnitBase::ExternalizeAudioUnitBaseL() - Enter" );
    TInt count( KErrNone );
    //Add data
    aStream.WriteUint8L( iUnitId );
    count = iSourceId.Count();
    aStream.WriteInt8L( iSourceId.Count() );
    for( TInt i = 0; i < count; i++ )
        {
        aStream.WriteUint8L( ( TUint8 )iSourceId.operator[]( i ) );
        }
    count = iAudioControls.Count();
    aStream.WriteInt8L( count );

    API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioUnitBase::ExternalizeAudioUnitBaseL - Unit id         = %d", iUnitId );
    API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioUnitBase::ExternalizeAudioUnitBaseL - Source id count = %d", iSourceId.Count() );
    API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioUnitBase::ExternalizeAudioUnitBaseL - Audio controls count = %d", iAudioControls.Count() );

    for( TInt i = 0; i < iAudioControls.Count(); i++ )
        {
        iAudioControls[i]->ExternalizeL( aStream );
        }
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioUnitBase::ExternalizeBaseL - Return" );
    }

// ---------------------------------------------------------------------------
// CAccPolObjectBase::InternalizeBaseL
// ---------------------------------------------------------------------------
//
EXPORT_C void CAccPolAudioUnitBase::InternalizeAudioUnitBaseL(
    RReadStream& aStream )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioUnitBase::InternalizeAudioUnitBaseL() - Enter" );
    TInt count( KErrNone );
    iUnitId = aStream.ReadUint8L();
    count = aStream.ReadInt8L();
    for( TInt i = 0; i < count; i++ )
        {
        iSourceId.Append( ( TInt )aStream.ReadUint8L() );
        }

    API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioUnitBase::InternalizeAudioUnitBaseL - Unit id         = %d", iUnitId );
    API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioUnitBase::InternalizeAudioUnitBaseL - Source id count = %d", iSourceId.Count() );

    count = aStream.ReadInt8L();
    for( TInt i = 0; i < count; i++ )
        {
        //Object type
        TUint8 objectType = aStream.ReadUint8L();
        switch( objectType )
            {
            case EAccPolAudioVolumeControlObject:
                {
                CAccPolVolumeControl *volumeControl =
                    CAccPolVolumeControl::NewLC();
                volumeControl->InternalizeL( aStream );
                this->AddControlL( *volumeControl );
                CleanupStack::PopAndDestroy( volumeControl );
                }
                break;

            case EAccPolAudioMuteControlObject:
                {
                CAccPolMuteControl *muteControl = CAccPolMuteControl::NewLC();
                muteControl->InternalizeL( aStream );
                this->AddControlL( *muteControl );
                CleanupStack::PopAndDestroy( muteControl );
                }
                break;

            case EAccPolAudioInputControlObject:
                {
                CAccPolInputControl *inputControl =
                    CAccPolInputControl::NewLC();
                inputControl->InternalizeL( aStream );
                this->AddControlL( *inputControl );
                CleanupStack::PopAndDestroy( inputControl );
                }
                break;

            default:
                {
                User::Leave( KErrNotSupported );
                }
                break;
            }
        }
    API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioUnitBase::InternalizeAudioUnitBaseL - Audio controls count = %d", iAudioControls.Count() );

    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioUnitBase::InternalizeAudioUnitBaseL - Return" );
    }

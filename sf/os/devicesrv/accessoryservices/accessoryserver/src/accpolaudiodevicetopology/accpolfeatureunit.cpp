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

#include <accpolfeatureunit.h>
#include "acc_debug.h"

// -----------------------------------------------------------------------------
// CAccPolFeatureUnit::CAccPolFeatureUnit()
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CAccPolFeatureUnit::CAccPolFeatureUnit( CAccPolObjectCon* aObject ) :
    CAccPolAudioUnitBase( EAccPolAudioFeatureUnitObject, aObject )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolFeatureUnit::CAccPolFeatureUnit() - Enter" );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolFeatureUnit::CAccPolFeatureUnit() - Return" );
    }

// -----------------------------------------------------------------------------
// CAccPolFeatureUnit::CAccPolFeatureUnit()
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CAccPolFeatureUnit::CAccPolFeatureUnit( const TUint8 aUnitId,
    const TUint8 aSourceId ) :
    CAccPolAudioUnitBase( aUnitId, aSourceId, EAccPolAudioFeatureUnitObject )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolFeatureUnit::CAccPolFeatureUnit() - Enter" );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolFeatureUnit::CAccPolFeatureUnit() - Return" );
    }

// ---------------------------------------------------------------------------
// CAccPolFeatureUnit::NewL()
// ---------------------------------------------------------------------------
//
EXPORT_C CAccPolFeatureUnit* CAccPolFeatureUnit::NewL(
    CAccPolObjectCon* aObject )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolFeatureUnit::NewL() - Enter" );
    CAccPolFeatureUnit* self = CAccPolFeatureUnit::NewLC( aObject );
    CleanupStack::Pop( self );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolFeatureUnit::NewL() - Return" );
    return self;
    }

// ---------------------------------------------------------------------------
// CAccPolFeatureUnit::NewLC()
// ---------------------------------------------------------------------------
//
EXPORT_C CAccPolFeatureUnit* CAccPolFeatureUnit::NewLC(
    CAccPolObjectCon* aObject )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolFeatureUnit::NewLC() - Enter" );
    CAccPolFeatureUnit* self = new ( ELeave ) CAccPolFeatureUnit( aObject );
    CleanupStack::PushL( self );
    self->ConstructL();
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolFeatureUnit::NewLC() - Return" );
    return self;
    }

// ---------------------------------------------------------------------------
// CAccPolFeatureUnit::NewL()
// ---------------------------------------------------------------------------
//
EXPORT_C CAccPolFeatureUnit* CAccPolFeatureUnit::NewL( const TUint8 aUnitId,
    const TUint8 aSourceId )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolFeatureUnit::NewL() - Enter" );
    CAccPolFeatureUnit* self = CAccPolFeatureUnit::NewLC( aUnitId, aSourceId );

    CleanupStack::Pop( self );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY CAccPolFeatureUnit::NewL() - Return" );
    return self;
    }

// ---------------------------------------------------------------------------
// CAccPolFeatureUnit::NewLC()
// ---------------------------------------------------------------------------
//
EXPORT_C CAccPolFeatureUnit* CAccPolFeatureUnit::NewLC( const TUint8 aUnitId,
    const TUint8 aSourceId )

    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolFeatureUnit::NewLC() - Enter" );
    CAccPolFeatureUnit* self = new ( ELeave ) CAccPolFeatureUnit( aUnitId,
        aSourceId );
    CleanupStack::PushL( self );
    self->ConstructL();
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolFeatureUnit::NewLC() - Return" );
    return self;
    }

// ---------------------------------------------------------------------------
// CAccPolFeatureUnit::ConstructL()
// ---------------------------------------------------------------------------
//
void CAccPolFeatureUnit::ConstructL()
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolFeatureUnit::ConstructL() - Enter" );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolFeatureUnit::ConstructL() - Return" );
    }

// Destructor
EXPORT_C CAccPolFeatureUnit::~CAccPolFeatureUnit()
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolFeatureUnit::~CAccPolFeatureUnit() - Enter" );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolFeatureUnit::~CAccPolFeatureUnit() - Return" );
    }

// ---------------------------------------------------------------------------
// CAccPolFeatureUnit::operator()
// ---------------------------------------------------------------------------
//
EXPORT_C CAccPolFeatureUnit& CAccPolFeatureUnit::operator=(
    const CAccPolFeatureUnit& aFeatureUnitObject )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolFeatureUnit::operator() - Enter" );
    CAccPolAudioUnitBase::operator=(
        ( CAccPolAudioUnitBase& )aFeatureUnitObject );
    TInt index( KErrNone );

    for( TInt i = 0; i < KAccChannelCount; i++ )
        {
        iSupportedControls[index] = aFeatureUnitObject.SupportedControls( i );
        }
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolFeatureUnit::operator() - Return" );
    return ( *this );
    }

// ---------------------------------------------------------------------------
// CAccPolFeatureUnit::SetSupportedControls()
// ---------------------------------------------------------------------------
//
EXPORT_C void CAccPolFeatureUnit::SetSupportedControls( TUint32 aChannel,
    TUint32 aBitmap )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolFeatureUnit::SetSupportedControls() - Enter" );
    API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolFeatureUnit::SetSupportedControls() - Channel number:%d", aChannel );
    API_TRACE_1("[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolFeatureUnit::SetSupportedControlsL: Bitmap: 0x%bb", aBitmap );
    iSupportedControls[aChannel] = aBitmap;
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolFeatureUnit::SetSupportedControls() - Return" );
    }

// ---------------------------------------------------------------------------
// CAccPolFeatureUnit::SetSupportedControls()
// ---------------------------------------------------------------------------
//
EXPORT_C TUint32 CAccPolFeatureUnit::SupportedControls( TUint32 aChannel ) const
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolFeatureUnit::SupportedControls() - Enter" );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolFeatureUnit::SupportedControls() - Return" );
    return iSupportedControls[aChannel];
    }

// ---------------------------------------------------------------------------
// CAccPolFeatureUnit::SetSupportedControls()
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CAccPolFeatureUnit::SupportedControls( TUint32 aChannel,
    TUint32 aSupportedControlsBitMask ) const
    {
    API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolFeatureUnit::SupportedControls() 0x%x", aSupportedControlsBitMask );

    TUint32 result( iSupportedControls[aChannel] );
    API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolFeatureUnit::result() - 1 0x%x", result );
    result = result & aSupportedControlsBitMask;
    API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolFeatureUnit::result() - 2 0x%x", result );

    API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolFeatureUnit::SupportedControls() - return %d",
        ( result == aSupportedControlsBitMask ) );

    return ( result == aSupportedControlsBitMask );
    }

// ---------------------------------------------------------------------------
// CAccPolFeatureUnit::ExternalizeL()
// ---------------------------------------------------------------------------
//   
void CAccPolFeatureUnit::ExternalizeL( RWriteStream& aStream ) const
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolFeatureUnit::ExternalizeL() - Enter" );
    TUint8 count( KErrNone );
    //
    //Add object type at the beginning
    //
    aStream.WriteUint8L( ObjectType() );
    ExternalizeAudioUnitBaseL( aStream );

    count = iSupportedControls.Count();
    aStream.WriteUint8L( count );
    for( TInt i = 0; i < count; i++ )
        {
        aStream.WriteUint16L( iSupportedControls[i] );
        }

    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolFeatureUnit::ExternalizeL - Return" );
    }

// ---------------------------------------------------------------------------
// CAccPolFeatureUnit::InternalizeL()
// ---------------------------------------------------------------------------
//  
void CAccPolFeatureUnit::InternalizeL( RReadStream& aStream )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolFeatureUnit::InternalizeL() - Enter" );
    InternalizeAudioUnitBaseL( aStream );

    TUint8 count = aStream.ReadUint8L();
    for( TInt i = 0; i < count; i++ )
        {
        iSupportedControls[i] = aStream.ReadUint16L();
        }

    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolFeatureUnit::InternalizeL - Return" );
    }

// ---------------------------------------------------------------------------
// CAccPolFeatureUnit::DuplicateL()
// ---------------------------------------------------------------------------
//
CAccPolObjectBase* CAccPolFeatureUnit::DuplicateLC( CAccPolObjectCon* aObject )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolFeatureUnit::DublicateL() - Enter" );
    CAccPolFeatureUnit* aFeature = CAccPolFeatureUnit::NewLC( aObject );
    *aFeature = ( CAccPolFeatureUnit& )*this;
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolFeatureUnit::DublicateL() - Return" );

    return aFeature;
    }


/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation for audio mixer unit object.
*
*/

#include <accpolmixerunit.h>
#include "acc_debug.h"

// -----------------------------------------------------------------------------
// CAccPolMixerUnit::CAccPolMixerUnit()
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CAccPolMixerUnit::CAccPolMixerUnit( CAccPolObjectCon* aObject ) :
    CAccPolAudioUnitBase( EAccPolAudioMixerUnitObject, aObject )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolMixerUnit::CAccPolMixerUnit() -Enter" );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolMixerUnit::CAccPolMixerUnit() - Return void" );
    }

// -----------------------------------------------------------------------------
// CAccPolMixerUnit::CAccPolMixerUnit()
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CAccPolMixerUnit::CAccPolMixerUnit( const TUint8 aUnitId ) :
    CAccPolAudioUnitBase( aUnitId, EAccPolAudioMixerUnitObject )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolMixerUnit::CAccPolMixerUnit() -Enter" );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolMixerUnit::CAccPolMixerUnit() - Return" );
    }

// ---------------------------------------------------------------------------
// CAccPolMixerUnit::NewL()
// ---------------------------------------------------------------------------
//
EXPORT_C CAccPolMixerUnit* CAccPolMixerUnit::NewL( CAccPolObjectCon* aObject )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolMixerUnit::NewL() - Enter" );
    CAccPolMixerUnit* self = CAccPolMixerUnit::NewLC( aObject );
    CleanupStack::Pop( self );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolMixerUnit::NewL() - Return" );
    return self;
    }

// ---------------------------------------------------------------------------
// CAccPolMixerUnit::NewLC()
// ---------------------------------------------------------------------------
//
EXPORT_C CAccPolMixerUnit* CAccPolMixerUnit::NewLC( CAccPolObjectCon* aObject )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolMixerUnit::NewLC() - Enter" );
    CAccPolMixerUnit* self = new ( ELeave ) CAccPolMixerUnit( aObject );
    CleanupStack::PushL( self );
    self->ConstructL();
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolMixerUnit::NewLC() - Return" );
    return self;
    }

// ---------------------------------------------------------------------------
// CAccPolMixerUnit::NewL()
// ---------------------------------------------------------------------------
//
EXPORT_C CAccPolMixerUnit* CAccPolMixerUnit::NewL( const TUint8 aUnitId )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolMixerUnit::NewL() - Enter" );
    CAccPolMixerUnit* self = CAccPolMixerUnit::NewLC( aUnitId );

    CleanupStack::Pop( self );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolMixerUnit::NewL() - Return" );
    return self;
    }

// ---------------------------------------------------------------------------
// CAccPolMixerUnit::NewLC()
// ---------------------------------------------------------------------------
//
EXPORT_C CAccPolMixerUnit* CAccPolMixerUnit::NewLC( const TUint8 aUnitId )

    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolMixerUnit::NewLC() - Enter" );
    CAccPolMixerUnit* self = new ( ELeave ) CAccPolMixerUnit( aUnitId );

    CleanupStack::PushL( self );
    self->ConstructL();
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolMixerUnit::NewLC() - Return" );
    return self;
    }

// ---------------------------------------------------------------------------
// CAccPolMixerUnit::ConstructL()
// ---------------------------------------------------------------------------
//
void CAccPolMixerUnit::ConstructL()
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolMixerUnit::ConstructL() - Enter" );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolMixerUnit::ConstructL() - Return" );
    }

// Destructor
EXPORT_C CAccPolMixerUnit::~CAccPolMixerUnit()
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolMixerUnit::~CAccPolMixerUnit() - Enter" );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolMixerUnit::~CAccPolMixerUnit() - Return" );
    }

// ---------------------------------------------------------------------------
// CAccPolMixerUnit::operator()
// ---------------------------------------------------------------------------
//
EXPORT_C CAccPolMixerUnit& CAccPolMixerUnit::operator=(
    const CAccPolMixerUnit& aAudioMixerUnitObject )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolMixerUnit::operator() - Enter" );
    CAccPolAudioUnitBase::operator=(
        ( CAccPolAudioUnitBase& )aAudioMixerUnitObject );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolMixerUnit::operator() - Return" );
    return ( *this );
    }

// ---------------------------------------------------------------------------
// CAccPolMixerUnit::ExternalizeL()
// ---------------------------------------------------------------------------
//
void CAccPolMixerUnit::ExternalizeL( RWriteStream& aStream ) const
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolMixerUnit::ExternalizeL() -Enter" );
    //
    //Add object type at the beginning.
    //
    aStream.WriteUint8L( ObjectType() );
    ExternalizeAudioUnitBaseL( aStream );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolMixerUnit::ExternalizeL - Return" );
    }

// ---------------------------------------------------------------------------
// CAccPolMixerUnit::InternalizeL()
// ---------------------------------------------------------------------------
//
void CAccPolMixerUnit::InternalizeL( RReadStream& aStream )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolMixerUnit::InternalizeL() - Enter" );
    InternalizeAudioUnitBaseL( aStream );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolMixerUnit::InternalizeL - Return" );
    }

// ---------------------------------------------------------------------------
// CAccPolFeatureUnit::DuplicateL()
// ---------------------------------------------------------------------------
//
CAccPolObjectBase* CAccPolMixerUnit::DuplicateLC( CAccPolObjectCon* aObject )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolMixerUnit::DuplicateLC() - Enter" );
    CAccPolMixerUnit* aMixer = CAccPolMixerUnit::NewLC( aObject );
    *aMixer = ( CAccPolMixerUnit& )*this;
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolMixerUnit::DuplicateLC() - Return" );
    return aMixer;
    }

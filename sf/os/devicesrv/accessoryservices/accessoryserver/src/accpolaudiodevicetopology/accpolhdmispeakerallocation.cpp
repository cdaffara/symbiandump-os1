/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description:  HDMI sink object
 *
 */

#include <accpolhdmispeakerallocation.h>
#include <accpolobjecttypes.h>

#include "acc_debug.h"

// -----------------------------------------------------------------------------
// CAccPolHdmiSpeakerAllocation::CAccPolHdmiSpeakerAllocation()
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CAccPolHdmiSpeakerAllocation::CAccPolHdmiSpeakerAllocation( CAccPolObjectCon* aObject ) :
    CAccPolObjectBase( EAccPolHdmiSpeakerAllocationObject, aObject )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiSpeakerAllocation::CAccPolHdmiSpeakerAllocation() - Enter" );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiSpeakerAllocation::CAccPolHdmiSpeakerAllocation() - Return" );
    }

// -----------------------------------------------------------------------------
// CAccPolHdmiSpeakerAllocation::CAccPolHdmiSpeakerAllocation()
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CAccPolHdmiSpeakerAllocation::CAccPolHdmiSpeakerAllocation( const TBool aFrontSpeakers,
    const TBool aRearSpeakers,
    const TBool aLowFrequencyEffect,
    const TBool aFrontCenter,
    const TBool aFrontCenterHigh,
    const TBool aTopCenter,
    const TBool aRearCenter,
    const TBool aFrontLeftRightCenter,
    const TBool aRearLeftRightCenter,
    const TBool aFrontWideSpeakers,
    const TBool aFrontHighSpeakers ) :
    CAccPolObjectBase( EAccPolHdmiSpeakerAllocationObject ),
        iSpeakerAllocation( EHdmiSpeakerUnknown )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiSpeakerAllocation::CAccPolHdmiSpeakerAllocation() - Enter" );

    if( aFrontSpeakers )
        {
        iSpeakerAllocation |= EHdmiSpeakerFrontLeftRight;
        iSpeakerCount += 2;
        }
    if( aRearSpeakers )
        {
        iSpeakerAllocation |= EHdmiSpeakerRearLeftRight;
        iSpeakerCount += 2;
        }
    if( aLowFrequencyEffect )
        {
        iSpeakerAllocation |= EHdmiSpeakerLowFrequencyEffect;
        ++iSpeakerCount;
        }
    if( aFrontCenter )
        {
        iSpeakerAllocation |= EHdmiSpeakerFrontCenter;
        ++iSpeakerCount;
        }
    if( aFrontCenterHigh )
        {
        iSpeakerAllocation |= EHdmiSpeakerFrontCenterHigh;
        ++iSpeakerCount;
        }
    if( aTopCenter )
        {
        iSpeakerAllocation |= EHdmiSpeakerTopCenter;
        ++iSpeakerCount;
        }
    if( aRearCenter )
        {
        iSpeakerAllocation |= EHdmiSpeakerRearCenter;
        ++iSpeakerCount;
        }
    if( aFrontLeftRightCenter )
        {
        iSpeakerAllocation |= EHdmiSpeakerFrontLeftRightCenter;
        iSpeakerCount += 2;
        }
    if( aRearLeftRightCenter )
        {
        iSpeakerAllocation |= EHdmiSpeakerRearLeftRightCenter;
        iSpeakerCount += 2;
        }
    if( aFrontWideSpeakers )
        {
        iSpeakerAllocation |= EHdmiSpeakerFrontLeftRightWide;
        iSpeakerCount += 2;
        }
    if( aFrontHighSpeakers )
        {
        iSpeakerAllocation |= EHdmiSpeakerFrontLeftRightHigh;
        iSpeakerCount += 2;
        }

    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiSpeakerAllocation::CAccPolHdmiSpeakerAllocation() - Return" );
    }

// -----------------------------------------------------------------------------
// CAccPolHdmiSpeakerAllocation::CAccPolHdmiSpeakerAllocation()
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CAccPolHdmiSpeakerAllocation::CAccPolHdmiSpeakerAllocation( const TUint32 aSpeakerAllocation ) :
    CAccPolObjectBase( EAccPolHdmiSpeakerAllocationObject ),
        iSpeakerAllocation( aSpeakerAllocation ), iSpeakerCount( 0 )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiSpeakerAllocation::CAccPolHdmiSpeakerAllocation() - Enter" );

    if( aSpeakerAllocation & EHdmiSpeakerFrontLeftRight )
        {
        iSpeakerCount += 2;
        }
    if( aSpeakerAllocation & EHdmiSpeakerRearLeftRight )
        {
        iSpeakerCount += 2;
        }
    if( aSpeakerAllocation & EHdmiSpeakerLowFrequencyEffect )
        {
        ++iSpeakerCount;
        }
    if( aSpeakerAllocation & EHdmiSpeakerFrontCenter )
        {
        ++iSpeakerCount;
        }
    if( aSpeakerAllocation & EHdmiSpeakerFrontCenterHigh )
        {
        ++iSpeakerCount;
        }
    if( aSpeakerAllocation & EHdmiSpeakerTopCenter )
        {
        ++iSpeakerCount;
        }
    if( aSpeakerAllocation & EHdmiSpeakerRearCenter )
        {
        ++iSpeakerCount;
        }
    if( aSpeakerAllocation & EHdmiSpeakerFrontLeftRightCenter )
        {
        iSpeakerCount += 2;
        }
    if( aSpeakerAllocation & EHdmiSpeakerRearLeftRightCenter )
        {
        iSpeakerCount += 2;
        }
    if( aSpeakerAllocation & EHdmiSpeakerFrontLeftRightWide )
        {
        iSpeakerCount += 2;
        }
    if( aSpeakerAllocation & EHdmiSpeakerFrontLeftRightHigh )
        {
        iSpeakerCount += 2;
        }

    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiSpeakerAllocation::CAccPolHdmiSpeakerAllocation() - Return" );
    }

// ---------------------------------------------------------------------------
// CAccPolHdmiSpeakerAllocation::NewL()
// ---------------------------------------------------------------------------
//
EXPORT_C CAccPolHdmiSpeakerAllocation* CAccPolHdmiSpeakerAllocation::NewL( CAccPolObjectCon* aObject )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiSpeakerAllocation::NewL() - Enter" );
    CAccPolHdmiSpeakerAllocation* self =
        CAccPolHdmiSpeakerAllocation::NewLC( aObject );
    CleanupStack::Pop( self );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiSpeakerAllocation::NewL() - Return" );
    return self;
    }

// ---------------------------------------------------------------------------
// CAccPolHdmiSpeakerAllocation::NewLC()
// ---------------------------------------------------------------------------
//
EXPORT_C CAccPolHdmiSpeakerAllocation* CAccPolHdmiSpeakerAllocation::NewLC( CAccPolObjectCon* aObject )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiSpeakerAllocation::NewLC() - Enter" );
    CAccPolHdmiSpeakerAllocation* self =
        new ( ELeave ) CAccPolHdmiSpeakerAllocation( aObject );
    CleanupStack::PushL( self );
    self->ConstructL();
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiSpeakerAllocation::NewLC() - Return" );
    return self;
    }

// ---------------------------------------------------------------------------
// CAccPolHdmiSpeakerAllocation::NewL()
// ---------------------------------------------------------------------------
//
EXPORT_C CAccPolHdmiSpeakerAllocation* CAccPolHdmiSpeakerAllocation::NewL( const TUint32 aSpeakerAllocation )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiSpeakerAllocation::NewL() - Enter" );
    CAccPolHdmiSpeakerAllocation* self =
        CAccPolHdmiSpeakerAllocation::NewLC( aSpeakerAllocation );
    CleanupStack::Pop( self );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiSpeakerAllocation::NewL() - Return" );
    return self;
    }

// ---------------------------------------------------------------------------
// CAccPolHdmiSpeakerAllocation::NewLC()
// ---------------------------------------------------------------------------
//
EXPORT_C CAccPolHdmiSpeakerAllocation* CAccPolHdmiSpeakerAllocation::NewLC( const TUint32 aSpeakerAllocation )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiSpeakerAllocation::NewLC() - Enter" );
    CAccPolHdmiSpeakerAllocation* self =
        new ( ELeave ) CAccPolHdmiSpeakerAllocation( aSpeakerAllocation );
    CleanupStack::PushL( self );
    self->ConstructL();
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiSpeakerAllocation::NewLC() - Return" );
    return self;
    }

// ---------------------------------------------------------------------------
// CAccPolHdmiSpeakerAllocation::NewL()
// ---------------------------------------------------------------------------
//
EXPORT_C CAccPolHdmiSpeakerAllocation* CAccPolHdmiSpeakerAllocation::NewL( const TBool aFrontSpeakers,
    const TBool aRearSpeakers,
    const TBool aLowFrequencyEffect,
    const TBool aFrontCenter,
    const TBool aFrontCenterHigh,
    const TBool aTopCenter,
    const TBool aRearCenter,
    const TBool aFrontLeftRightCenter,
    const TBool aRearLeftRightCenter,
    const TBool aFrontWideSpeakers,
    const TBool aFrontHighSpeakers )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiSpeakerAllocation::NewL() - Enter" );
    CAccPolHdmiSpeakerAllocation* self =
        CAccPolHdmiSpeakerAllocation::NewLC( aFrontSpeakers,
            aRearSpeakers,
            aLowFrequencyEffect,
            aFrontCenter,
            aFrontCenterHigh,
            aTopCenter,
            aRearCenter,
            aFrontLeftRightCenter,
            aRearLeftRightCenter,
            aFrontWideSpeakers,
            aFrontHighSpeakers );

    CleanupStack::Pop( self );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiSpeakerAllocation::NewL()- Return" );
    return self;
    }

// ---------------------------------------------------------------------------
// CAccPolHdmiSpeakerAllocation::NewLC()
// ---------------------------------------------------------------------------
//
EXPORT_C CAccPolHdmiSpeakerAllocation* CAccPolHdmiSpeakerAllocation::NewLC( const TBool aFrontSpeakers,
    const TBool aRearSpeakers,
    const TBool aLowFrequencyEffect,
    const TBool aFrontCenter,
    const TBool aFrontCenterHigh,
    const TBool aTopCenter,
    const TBool aRearCenter,
    const TBool aFrontLeftRightCenter,
    const TBool aRearLeftRightCenter,
    const TBool aFrontWideSpeakers,
    const TBool aFrontHighSpeakers )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiSpeakerAllocation::NewLC() - Enter" );
    CAccPolHdmiSpeakerAllocation* self =
        new ( ELeave ) CAccPolHdmiSpeakerAllocation( aFrontSpeakers,
            aRearSpeakers,
            aLowFrequencyEffect,
            aFrontCenter,
            aFrontCenterHigh,
            aTopCenter,
            aRearCenter,
            aFrontLeftRightCenter,
            aRearLeftRightCenter,
            aFrontWideSpeakers,
            aFrontHighSpeakers );
    CleanupStack::PushL( self );
    self->ConstructL();
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiSpeakerAllocation::NewLC() - Return" );
    return self;
    }

// ---------------------------------------------------------------------------
// CAccPolHdmiSpeakerAllocation::ConstructL()
// ---------------------------------------------------------------------------
//
void CAccPolHdmiSpeakerAllocation::ConstructL()
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiSpeakerAllocation::ConstructL() - Enter" );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiSpeakerAllocation::ConstructL() - Return" );
    }

// Destructor
CAccPolHdmiSpeakerAllocation::~CAccPolHdmiSpeakerAllocation()
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiSpeakerAllocation::~CAccPolHdmiSpeakerAllocation() - Enter" );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiSpeakerAllocation::~CAccPolHdmiSpeakerAllocation() - Return" );
    }

// ---------------------------------------------------------------------------
// CAccPolHdmiSpeakerAllocation::operator()
// ---------------------------------------------------------------------------
//
EXPORT_C CAccPolHdmiSpeakerAllocation& CAccPolHdmiSpeakerAllocation::operator=( const CAccPolHdmiSpeakerAllocation& aOutputTerminalObject )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiSpeakerAllocation::operator() - Enter" );
    CAccPolObjectBase::operator=( ( CAccPolObjectBase& )aOutputTerminalObject );

    iSpeakerAllocation = aOutputTerminalObject.SpeakerAllocation();
    iSpeakerCount = aOutputTerminalObject.SpeakerCount();

    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiSpeakerAllocation::operator() - Return" );
    return ( *this );
    }

// ---------------------------------------------------------------------------
// CAccPolHdmiSpeakerAllocation::ExternalizeL()
// ---------------------------------------------------------------------------
//
void CAccPolHdmiSpeakerAllocation::ExternalizeL( RWriteStream& aStream ) const
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiSpeakerAllocation::ExternalizeL() - Enter" );

    // Add object type at the beginning
    aStream.WriteUint8L( ObjectType() );

    // Object data
    aStream.WriteUint32L( SpeakerAllocation() );
    aStream.WriteUint32L( SpeakerCount() );

    API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiSpeakerAllocation::ExternalizeL - Speaker Allocation  = %d", iSpeakerAllocation );
    API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiSpeakerAllocation::ExternalizeL - Speaker Count  = %d", iSpeakerCount );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiSpeakerAllocation::ExternalizeL - Return" );
    }

// ---------------------------------------------------------------------------
// CAccPolHdmiSpeakerAllocation::InternalizeL()
// ---------------------------------------------------------------------------
//
void CAccPolHdmiSpeakerAllocation::InternalizeL( RReadStream& aStream )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiSpeakerAllocation::InternalizeL() - Enter" );

    // Object data
    iSpeakerAllocation = aStream.ReadUint32L();
    iSpeakerCount = aStream.ReadUint32L();

    API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiSpeakerAllocation::InternalizeL - Speaker Allocation  = %d", iSpeakerAllocation );
    API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiSpeakerAllocation::InternalizeL - Speaker Count  = %d", iSpeakerCount );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiSpeakerAllocation::InternalizeL - Return" );
    }

// ---------------------------------------------------------------------------
// CAccPolHdmiSpeakerAllocation::DuplicateL()
// ---------------------------------------------------------------------------
//
CAccPolObjectBase* CAccPolHdmiSpeakerAllocation::DuplicateLC( CAccPolObjectCon* aObject )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiSpeakerAllocation::DuplicateLC() - Enter" );
    CAccPolHdmiSpeakerAllocation* aOutput =
        CAccPolHdmiSpeakerAllocation::NewLC( aObject );
    *aOutput = ( CAccPolHdmiSpeakerAllocation& )*this;
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiSpeakerAllocation::DuplicateLC() - Return" );
    return aOutput;
    }

// ---------------------------------------------------------------------------
// CAccPolHdmiSpeakerAllocation::SetSpeakerAllocation()
// ---------------------------------------------------------------------------
//
EXPORT_C void CAccPolHdmiSpeakerAllocation::SetSpeakerAllocation( const TUint32 aSpeakerAllocation )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiSpeakerAllocation::SpeakerAllocation() - Enter" );

    iSpeakerAllocation = aSpeakerAllocation;
    iSpeakerCount = 0;
    if( aSpeakerAllocation & EHdmiSpeakerFrontLeftRight )
        {
        iSpeakerCount += 2;
        }
    if( aSpeakerAllocation & EHdmiSpeakerRearLeftRight )
        {
        iSpeakerCount += 2;
        }
    if( aSpeakerAllocation & EHdmiSpeakerLowFrequencyEffect )
        {
        ++iSpeakerCount;
        }
    if( aSpeakerAllocation & EHdmiSpeakerFrontCenter )
        {
        ++iSpeakerCount;
        }
    if( aSpeakerAllocation & EHdmiSpeakerFrontCenterHigh )
        {
        ++iSpeakerCount;
        }
    if( aSpeakerAllocation & EHdmiSpeakerTopCenter )
        {
        ++iSpeakerCount;
        }
    if( aSpeakerAllocation & EHdmiSpeakerRearCenter )
        {
        ++iSpeakerCount;
        }
    if( aSpeakerAllocation & EHdmiSpeakerFrontLeftRightCenter )
        {
        iSpeakerCount += 2;
        }
    if( aSpeakerAllocation & EHdmiSpeakerRearLeftRightCenter )
        {
        iSpeakerCount += 2;
        }
    if( aSpeakerAllocation & EHdmiSpeakerFrontLeftRightWide )
        {
        iSpeakerCount += 2;
        }
    if( aSpeakerAllocation & EHdmiSpeakerFrontLeftRightHigh )
        {
        iSpeakerCount += 2;
        }

    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiSpeakerAllocation::SpeakerAllocation() - Return" );
    }

// ---------------------------------------------------------------------------
// CAccPolHdmiSpeakerAllocation::SetFrontSpeakers()
// ---------------------------------------------------------------------------
//
EXPORT_C void CAccPolHdmiSpeakerAllocation::SetFrontSpeakers( const TBool aValue )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiSpeakerAllocation::SetFrontSpeakers() - Enter" );
    if( !( iSpeakerAllocation & EHdmiSpeakerFrontLeftRight ) && aValue )
        {
        iSpeakerAllocation |= EHdmiSpeakerFrontLeftRight;
        iSpeakerCount += 2;
        }
    else if( ( iSpeakerAllocation & EHdmiSpeakerFrontLeftRight ) && !aValue )
        {
        iSpeakerAllocation &= ~EHdmiSpeakerFrontLeftRight;
        iSpeakerCount -= 2;
        }
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiSpeakerAllocation::SetFrontSpeakers() - Return" );
    }

// ---------------------------------------------------------------------------
// CAccPolHdmiSpeakerAllocation::SetRearSpeakers()
// ---------------------------------------------------------------------------
//
EXPORT_C void CAccPolHdmiSpeakerAllocation::SetRearSpeakers( const TBool aValue )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiSpeakerAllocation::SetRearSpeakers() - Enter" );
    if( !( iSpeakerAllocation & EHdmiSpeakerRearLeftRight ) && aValue )
        {
        iSpeakerAllocation |= EHdmiSpeakerRearLeftRight;
        iSpeakerCount += 2;
        }
    else if( ( iSpeakerAllocation & EHdmiSpeakerRearLeftRight ) && !aValue )
        {
        iSpeakerAllocation &= ~EHdmiSpeakerRearLeftRight;
        iSpeakerCount -= 2;
        }
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiSpeakerAllocation::SetRearSpeakers() - Return" );
    }

// ---------------------------------------------------------------------------
// CAccPolHdmiSpeakerAllocation::SetLowFrequencyEffect()
// ---------------------------------------------------------------------------
//
EXPORT_C void CAccPolHdmiSpeakerAllocation::SetLowFrequencyEffect( const TBool aValue )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiSpeakerAllocation::SetLowFrequencyEffect() - Enter" );
    if( !( iSpeakerAllocation & EHdmiSpeakerLowFrequencyEffect ) && aValue )
        {
        iSpeakerAllocation |= EHdmiSpeakerLowFrequencyEffect;
        ++iSpeakerCount;
        }
    else if( ( iSpeakerAllocation & EHdmiSpeakerLowFrequencyEffect ) && !aValue )
        {
        iSpeakerAllocation &= ~EHdmiSpeakerLowFrequencyEffect;
        --iSpeakerCount;
        }
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiSpeakerAllocation::SetLowFrequencyEffect() - Return" );
    }

// ---------------------------------------------------------------------------
// CAccPolHdmiSpeakerAllocation::SetFrontCenter()
// ---------------------------------------------------------------------------
//
EXPORT_C void CAccPolHdmiSpeakerAllocation::SetFrontCenter( const TBool aValue )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiSpeakerAllocation::SetFrontCenter() - Enter" );
    if( !( iSpeakerAllocation & EHdmiSpeakerFrontCenter ) && aValue )
        {
        iSpeakerAllocation |= EHdmiSpeakerFrontCenter;
        ++iSpeakerCount;
        }
    else if( ( iSpeakerAllocation & EHdmiSpeakerFrontCenter ) && !aValue )
        {
        iSpeakerAllocation &= ~EHdmiSpeakerFrontCenter;
        --iSpeakerCount;
        }
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiSpeakerAllocation::SetFrontCenter() - Return" );
    }

// ---------------------------------------------------------------------------
// CAccPolHdmiSpeakerAllocation::SetFrontCenterHigh()
// ---------------------------------------------------------------------------
//
EXPORT_C void CAccPolHdmiSpeakerAllocation::SetFrontCenterHigh( const TBool aValue )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiSpeakerAllocation::SetFrontCenterHigh() - Enter" );
    if( !( iSpeakerAllocation & EHdmiSpeakerFrontCenterHigh ) && aValue )
        {
        iSpeakerAllocation |= EHdmiSpeakerFrontCenterHigh;
        ++iSpeakerCount;
        }
    else if( ( iSpeakerAllocation & EHdmiSpeakerFrontCenterHigh ) && !aValue )
        {
        iSpeakerAllocation &= ~EHdmiSpeakerFrontCenterHigh;
        --iSpeakerCount;
        }
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiSpeakerAllocation::SetFrontCenterHigh() - Return" );
    }

// ---------------------------------------------------------------------------
// CAccPolHdmiSpeakerAllocation::SetTopCenter()
// ---------------------------------------------------------------------------
//
EXPORT_C void CAccPolHdmiSpeakerAllocation::SetTopCenter( const TBool aValue )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiSpeakerAllocation::SetTopCenter() - Enter" );
    if( !( iSpeakerAllocation & EHdmiSpeakerTopCenter ) && aValue )
        {
        iSpeakerAllocation |= EHdmiSpeakerTopCenter;
        ++iSpeakerCount;
        }
    else if( ( iSpeakerAllocation & EHdmiSpeakerTopCenter ) && !aValue )
        {
        iSpeakerAllocation &= ~EHdmiSpeakerTopCenter;
        --iSpeakerCount;
        }
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiSpeakerAllocation::SetTopCenter() - Return" );
    }

// ---------------------------------------------------------------------------
// CAccPolHdmiSpeakerAllocation::SetRearCenter()
// ---------------------------------------------------------------------------
//
EXPORT_C void CAccPolHdmiSpeakerAllocation::SetRearCenter( const TBool aValue )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiSpeakerAllocation::SetRearCenter() - Enter" );
    if( !( iSpeakerAllocation & EHdmiSpeakerRearCenter ) && aValue )
        {
        iSpeakerAllocation |= EHdmiSpeakerRearCenter;
        ++iSpeakerCount;
        }
    else if( ( iSpeakerAllocation & EHdmiSpeakerRearCenter ) && !aValue )
        {
        iSpeakerAllocation &= ~EHdmiSpeakerRearCenter;
        --iSpeakerCount;
        }
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiSpeakerAllocation::SetRearCenter() - Return" );
    }

// ---------------------------------------------------------------------------
// CAccPolHdmiSpeakerAllocation::SetFrontLeftRightCenter()
// ---------------------------------------------------------------------------
//
EXPORT_C void CAccPolHdmiSpeakerAllocation::SetFrontLeftRightCenter( const TBool aValue )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiSpeakerAllocation::SetFrontLeftRightCenter() - Enter" );
    if( !( iSpeakerAllocation & EHdmiSpeakerFrontLeftRightCenter ) && aValue )
        {
        iSpeakerAllocation |= EHdmiSpeakerFrontLeftRightCenter;
        iSpeakerCount += 2;
        }
    else if( ( iSpeakerAllocation & EHdmiSpeakerFrontLeftRightCenter )
        && !aValue )
        {
        iSpeakerAllocation &= ~EHdmiSpeakerFrontLeftRightCenter;
        iSpeakerCount -= 2;
        }
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiSpeakerAllocation::SetFrontLeftRightCenter() - Return" );
    }

// ---------------------------------------------------------------------------
// CAccPolHdmiSpeakerAllocation::SetRearLeftRightCenter()
// ---------------------------------------------------------------------------
//
EXPORT_C void CAccPolHdmiSpeakerAllocation::SetRearLeftRightCenter( const TBool aValue )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiSpeakerAllocation::SetRearLeftRightCenter() - Enter" );
    if( !( iSpeakerAllocation & EHdmiSpeakerRearLeftRightCenter ) && aValue )
        {
        iSpeakerAllocation |= EHdmiSpeakerRearLeftRightCenter;
        iSpeakerCount += 2;
        }
    else if( ( iSpeakerAllocation & EHdmiSpeakerRearLeftRightCenter )
        && !aValue )
        {
        iSpeakerAllocation &= ~EHdmiSpeakerRearLeftRightCenter;
        iSpeakerCount -= 2;
        }
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiSpeakerAllocation::SetRearLeftRightCenter() - Return" );
    }

// ---------------------------------------------------------------------------
// CAccPolHdmiSpeakerAllocation::SetFrontWideSpeakers()
// ---------------------------------------------------------------------------
//
EXPORT_C void CAccPolHdmiSpeakerAllocation::SetFrontWideSpeakers( const TBool aValue )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiSpeakerAllocation::SetFrontWideSpeakers() - Enter" );
    if( !( iSpeakerAllocation & EHdmiSpeakerFrontLeftRightWide ) && aValue )
        {
        iSpeakerAllocation |= EHdmiSpeakerFrontLeftRightWide;
        iSpeakerCount += 2;
        }
    else if( ( iSpeakerAllocation & EHdmiSpeakerFrontLeftRightWide ) && !aValue )
        {
        iSpeakerAllocation &= ~EHdmiSpeakerFrontLeftRightWide;
        iSpeakerCount -= 2;
        }
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiSpeakerAllocation::SetFrontWideSpeakers() - Return" );
    }

// ---------------------------------------------------------------------------
// CAccPolHdmiSpeakerAllocation::SetFrontHighSpeakers()
// ---------------------------------------------------------------------------
//
EXPORT_C void CAccPolHdmiSpeakerAllocation::SetFrontHighSpeakers( const TBool aValue )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiSpeakerAllocation::SetFrontHighSpeakers() - Enter" );
    if( !( iSpeakerAllocation & EHdmiSpeakerFrontLeftRightHigh ) && aValue )
        {
        iSpeakerAllocation |= EHdmiSpeakerFrontLeftRightHigh;
        iSpeakerCount += 2;
        }
    else if( ( iSpeakerAllocation & EHdmiSpeakerFrontLeftRightHigh ) && !aValue )
        {
        iSpeakerAllocation &= ~EHdmiSpeakerFrontLeftRightHigh;
        iSpeakerCount -= 2;
        }
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiSpeakerAllocation::SetFrontHighSpeakers() - Return" );
    }

// ---------------------------------------------------------------------------
// CAccPolHdmiSpeakerAllocation::SpeakerCount()
// ---------------------------------------------------------------------------
//
EXPORT_C TUint32 CAccPolHdmiSpeakerAllocation::SpeakerCount() const
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiSpeakerAllocation::SpeakerCount() - Enter" );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiSpeakerAllocation::SpeakerCount() - Return" );
    return iSpeakerCount;
    }

// ---------------------------------------------------------------------------
// CAccPolHdmiSpeakerAllocation::SpeakerAllocation()
// ---------------------------------------------------------------------------
//
EXPORT_C TUint32 CAccPolHdmiSpeakerAllocation::SpeakerAllocation() const
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiSpeakerAllocation::SpeakerAllocation() - Enter" );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiSpeakerAllocation::SpeakerAllocation() - Return" );
    return iSpeakerAllocation;
    }

// ---------------------------------------------------------------------------
// CAccPolHdmiSpeakerAllocation::FrontSpeakers()
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CAccPolHdmiSpeakerAllocation::FrontSpeakers() const
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiSpeakerAllocation::FrontSpeakers() - Enter" );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiSpeakerAllocation::FrontSpeakers() - Return" );
    return ( iSpeakerAllocation & EHdmiSpeakerFrontLeftRight );
    }

// ---------------------------------------------------------------------------
// CAccPolHdmiSpeakerAllocation::RearSpeakers()
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CAccPolHdmiSpeakerAllocation::RearSpeakers() const
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiSpeakerAllocation::RearSpeakers() - Enter" );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiSpeakerAllocation::RearSpeakers() - Return" );
    return ( iSpeakerAllocation & EHdmiSpeakerRearLeftRight );
    }

// ---------------------------------------------------------------------------
// CAccPolHdmiSpeakerAllocation::LowFrequencyEffect()
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CAccPolHdmiSpeakerAllocation::LowFrequencyEffect() const
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiSpeakerAllocation::LowFrequencyEffect() - Enter" );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiSpeakerAllocation::LowFrequencyEffect() - Return" );
    return ( iSpeakerAllocation & EHdmiSpeakerLowFrequencyEffect );
    }

// ---------------------------------------------------------------------------
// CAccPolHdmiSpeakerAllocation::FrontCenter()
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CAccPolHdmiSpeakerAllocation::FrontCenter() const
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiSpeakerAllocation::FrontCenter() - Enter" );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiSpeakerAllocation::FrontCenter() - Return" );
    return ( iSpeakerAllocation & EHdmiSpeakerFrontCenter );
    }

// ---------------------------------------------------------------------------
// CAccPolHdmiSpeakerAllocation::FrontCenterHigh()
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CAccPolHdmiSpeakerAllocation::FrontCenterHigh() const
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiSpeakerAllocation::FrontCenterHigh() - Enter" );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiSpeakerAllocation::FrontCenterHigh() - Return" );
    return ( iSpeakerAllocation & EHdmiSpeakerFrontCenterHigh );
    }

// ---------------------------------------------------------------------------
// CAccPolHdmiSpeakerAllocation::TopCenter()
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CAccPolHdmiSpeakerAllocation::TopCenter() const
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiSpeakerAllocation::TopCenter() - Enter" );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiSpeakerAllocation::TopCenter() - Return" );
    return ( iSpeakerAllocation & EHdmiSpeakerTopCenter );
    }

// ---------------------------------------------------------------------------
// CAccPolHdmiSpeakerAllocation::RearCenter()
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CAccPolHdmiSpeakerAllocation::RearCenter() const
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiSpeakerAllocation::RearCenter() - Enter" );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiSpeakerAllocation::RearCenter() - Return" );
    return ( iSpeakerAllocation & EHdmiSpeakerRearCenter );
    }

// ---------------------------------------------------------------------------
// CAccPolHdmiSpeakerAllocation::FrontLeftRightCenter()
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CAccPolHdmiSpeakerAllocation::FrontLeftRightCenter() const
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiSpeakerAllocation::FrontLeftRightCenter() - Enter" );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiSpeakerAllocation::FrontLeftRightCenter() - Return" );
    return ( iSpeakerAllocation & EHdmiSpeakerFrontLeftRightCenter );
    }

// ---------------------------------------------------------------------------
// CAccPolHdmiSpeakerAllocation::RearLeftRightCenter()
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CAccPolHdmiSpeakerAllocation::RearLeftRightCenter() const
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiSpeakerAllocation::RearLeftRightCenter() - Enter" );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiSpeakerAllocation::RearLeftRightCenter() - Return" );
    return ( iSpeakerAllocation & EHdmiSpeakerRearLeftRightCenter );
    }

// ---------------------------------------------------------------------------
// CAccPolHdmiSpeakerAllocation::FrontWideSpeakers()
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CAccPolHdmiSpeakerAllocation::FrontWideSpeakers() const
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiSpeakerAllocation::FrontWideSpeakers() - Enter" );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiSpeakerAllocation::FrontWideSpeakers() - Return" );
    return ( iSpeakerAllocation & EHdmiSpeakerFrontLeftRightWide );
    }

// ---------------------------------------------------------------------------
// CAccPolHdmiSpeakerAllocation::FrontHighSpeakers()
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CAccPolHdmiSpeakerAllocation::FrontHighSpeakers() const
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiSpeakerAllocation::FrontHighSpeakers() - Enter" );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiSpeakerAllocation::FrontHighSpeakers() - Return" );
    return ( iSpeakerAllocation & EHdmiSpeakerFrontLeftRightHigh );
    }

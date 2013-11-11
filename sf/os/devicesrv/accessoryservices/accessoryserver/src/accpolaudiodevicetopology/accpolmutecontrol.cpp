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
* Description:  Implementation for mute control object.
*
*/

#include <accpolmutecontrol.h>
#include "acc_debug.h"

// -----------------------------------------------------------------------------
// CAccPolMuteControl::CAccPolMuteControl()
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CAccPolMuteControl::CAccPolMuteControl( CAccPolObjectCon* aObject ) :
    CAccPolAudioControlBase( EAccPolAudioMuteControlObject, aObject )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolMuteControl::CAccPolMuteControl() - Enter" );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolMuteControl::CAccPolMuteControl() - Return" );
    }

// -----------------------------------------------------------------------------
// CAccPolMuteControl::CAccPolMuteControl()
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CAccPolMuteControl::CAccPolMuteControl( TUint32 aChannel, const TBool aMute ) :
    CAccPolAudioControlBase( EAccPolAudioMuteControlObject, aChannel )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolMuteControl::CAccPolMuteControl() - Enter" );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolMuteControl::CAccPolMuteControl() - Return void" );
    iMute = aMute;
    }

// ---------------------------------------------------------------------------
// CAccPolMuteControl::NewL()
// ---------------------------------------------------------------------------
//
EXPORT_C CAccPolMuteControl* CAccPolMuteControl::NewL(
    CAccPolObjectCon* aObject )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolMuteControl::NewL() - Enter" );
    CAccPolMuteControl* self = CAccPolMuteControl::NewLC( aObject );
    CleanupStack::Pop( self );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolMuteControl::NewL() - Return" );
    return self;
    }

// ---------------------------------------------------------------------------
// CAccPolMixerUnit::NewLC()
// ---------------------------------------------------------------------------
//
EXPORT_C CAccPolMuteControl* CAccPolMuteControl::NewLC(
    CAccPolObjectCon* aObject )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolMuteControl::NewLC() - Enter" );
    CAccPolMuteControl* self = new ( ELeave ) CAccPolMuteControl( aObject );
    CleanupStack::PushL( self );
    self->ConstructL();
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolMuteControl::NewLC() - Return" );
    return self;
    }

// ---------------------------------------------------------------------------
// CAccPolMuteControl::NewL()
// ---------------------------------------------------------------------------
//
EXPORT_C CAccPolMuteControl* CAccPolMuteControl::NewL( TUint32 aChannel,
    const TBool aMute )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolMuteControl::NewL() - Enter" );
    CAccPolMuteControl* self = CAccPolMuteControl::NewLC( aChannel, aMute );

    CleanupStack::Pop( self );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolMuteControl::NewL()- Return" );
    return self;
    }

// ---------------------------------------------------------------------------
// CAccPolMuteControl::NewLC()
// ---------------------------------------------------------------------------
//
EXPORT_C CAccPolMuteControl* CAccPolMuteControl::NewLC( TUint32 aChannel,
    const TBool aMute )

    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolMuteControl::NewLC() - Enter" );
    CAccPolMuteControl* self = new ( ELeave ) CAccPolMuteControl( aChannel,
        aMute );

    CleanupStack::PushL( self );
    self->ConstructL();
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolMuteControl::NewLC() - Return" );
    return self;
    }

// ---------------------------------------------------------------------------
// CAccPolMuteControl::ConstructL()
// ---------------------------------------------------------------------------
//
void CAccPolMuteControl::ConstructL()
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolMuteControl::ConstructL() - Enter" );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY CAccPolMuteControl::ConstructL()- Return void" );
    }

// Destructor
EXPORT_C CAccPolMuteControl::~CAccPolMuteControl()
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolMuteControl::~CAccPolMuteControl() - Enter" );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolMuteControl::~CAccPolMuteControl()- Return" );
    }

// ---------------------------------------------------------------------------
// CAccPolMuteControl::operator()
// ---------------------------------------------------------------------------
//
EXPORT_C CAccPolMuteControl& CAccPolMuteControl::operator=(
    const CAccPolMuteControl& aMuteControlObject )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolMuteControl::operator() - Enter" );

    CAccPolAudioControlBase::operator=(
        ( CAccPolAudioControlBase& )aMuteControlObject );
    iMute = aMuteControlObject.Mute();
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolMuteControl::operator()- Return" );
    return ( *this );
    }

// ---------------------------------------------------------------------------
// CAccPolMuteControl::SetMute()
// ---------------------------------------------------------------------------
//
EXPORT_C void CAccPolMuteControl::SetMute( const TBool aMute )
    {
    API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolMuteControl::SetMute() - mute=%d", aMute );
    iMute = aMute;
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolMuteControl::SetMute() - Return" );
    }
// ---------------------------------------------------------------------------
// CAccPolMuteControl::Mute()
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CAccPolMuteControl::Mute() const
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolMuteControl::Mute() - Enter" );
    API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolMuteControl::Mute() - mute=%d", iMute );
    return iMute;
    }

// ---------------------------------------------------------------------------
// CAccPolMuteControl::ExternalizeL()
// ---------------------------------------------------------------------------
//
void CAccPolMuteControl::ExternalizeL( RWriteStream& aStream ) const
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolMuteControl::ExternalizeL() - Enter" );
    aStream.WriteUint8L( ObjectType() );
    ExternalizeAudioControlbaseL( aStream );
    aStream.WriteInt8L( iMute );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolMuteControl::ExternalizeL - Return" );
    }

// ---------------------------------------------------------------------------
// CAccPolMuteControl::InternalizeL()
// ---------------------------------------------------------------------------
//
void CAccPolMuteControl::InternalizeL( RReadStream& aStream )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolMuteControl::InternalizeL() - Enter" );
    InternalizeAudioControlbaseL( aStream );
    iMute = aStream.ReadInt8L();
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolMuteControl::InternalizeL - Return" );
    }

// ---------------------------------------------------------------------------
// CAccPolMuteControl::DuplicateL()
// ---------------------------------------------------------------------------
//
CAccPolObjectBase* CAccPolMuteControl::DuplicateLC( CAccPolObjectCon* aObject )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolMuteControl::DuplicateLC() - Enter" );
    CAccPolMuteControl* aMute = CAccPolMuteControl::NewLC( aObject );
    *aMute = ( CAccPolMuteControl& )*this;
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolMuteControl::DuplicateLC() - Return" );
    return aMute;
    }

//  End of File

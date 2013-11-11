/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  CSensrvChannel implementation
*
*/



#include "sensrvchannel.h"
#include "sensrvchannelimpl.h"
#include "sensrvtrace.h"



// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Default constructor
// ---------------------------------------------------------------------------
//
CSensrvChannel::CSensrvChannel()
    {
    }


// ---------------------------------------------------------------------------
// 2-phase constructor
// ---------------------------------------------------------------------------
//
EXPORT_C CSensrvChannel* CSensrvChannel::NewL( const TSensrvChannelInfo& aChannelInfo )
    {
    CSensrvChannel* self = CSensrvChannel::NewLC( aChannelInfo );
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// 2-phase constructor
// ---------------------------------------------------------------------------
//
EXPORT_C CSensrvChannel* CSensrvChannel::NewLC( const TSensrvChannelInfo& aChannelInfo )
    {
    CSensrvChannelImpl* self = new( ELeave ) CSensrvChannelImpl( aChannelInfo );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// End of file

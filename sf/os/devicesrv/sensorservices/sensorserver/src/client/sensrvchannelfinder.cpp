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
* Description:  CSensrvChannelFinder implementation
*
*/


#include "sensrvchannelfinder.h"
#include "sensrvchannelfinderimpl.h"
#include "sensrvtrace.h"



// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Default constructor
// ---------------------------------------------------------------------------
//
CSensrvChannelFinder::CSensrvChannelFinder()
    {
    }


// ---------------------------------------------------------------------------
// 2-phase constructor
// ---------------------------------------------------------------------------
//
EXPORT_C CSensrvChannelFinder* CSensrvChannelFinder::NewL()
    {
    CSensrvChannelFinder* self = CSensrvChannelFinder::NewLC();
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// 2-phase constructor
// ---------------------------------------------------------------------------
//
EXPORT_C CSensrvChannelFinder* CSensrvChannelFinder::NewLC()
    {
    CSensrvChannelFinderImpl* self = new( ELeave ) CSensrvChannelFinderImpl;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


//  End of File

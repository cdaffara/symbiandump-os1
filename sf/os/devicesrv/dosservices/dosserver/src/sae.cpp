/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  
*
*/


// INCLUDE FILES
#include <e32svr.h>
#include "sae.h"

#include "saepubsubnotifier.h"
#include "sae_debug.h"

// LOCAL FUNCTION PROTOTYPES

// MODULE DATA STRUCTURES

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
CSae::CSae()
    {
    }

// EPOC default constructor can leave.
void CSae::ConstructL()
    {
    COM_TRACE_( "CSae::ConstructL()" );
    iPubSubNotifier = CSaePubSubNotifier::NewL();
    }

// Two-phased constructor.
CSae* CSae::NewL()
    {
    COM_TRACE_( "CSae::NewL()" );
    CSae* self = new (ELeave) CSae();
	CleanupStack::PushL( self );
    self->ConstructL();
	CleanupStack::Pop();
    return self;
    }

CSae::~CSae()
    {
    delete iPubSubNotifier;
    }

//  End of File  

// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//

// INCLUDE FILES
#include "SatMessaging.h"
#include "cmmmessagerouter.h"

// ======== MEMBER FUNCTIONS ========

CTsySatMessaging::CTsySatMessaging( CMmMessageRouter* aRouter ) 
    : iMessageRouter( aRouter )
    {
    }

void CTsySatMessaging::ConstructL()
    {
    }

EXPORT_C CTsySatMessaging* CTsySatMessaging::NewL( CMmMessageRouter* aRouter )
    {
    CTsySatMessaging* self = new ( ELeave ) CTsySatMessaging( aRouter );
    CleanupStack::PushL( self );
    self->ConstructL();   
    CleanupStack::Pop();    
    return self;
    }

CTsySatMessaging::~CTsySatMessaging()
    {
    }

// ---------------------------------------------------------------------------
// CTsySatMessaging::CompleteSendSmsMessage
// SMS message send complete
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CTsySatMessaging::CompleteSendSmsMessage( TInt /*aError*/ )
    {       
    return KErrNone;
    }
    
// ---------------------------------------------------------------------------
// CTsySatMessaging::IsMoSmControlBySimActivated
//
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TBool CTsySatMessaging::IsMoSmControlBySimActivated()
    {
    return EFalse; 
    }

// ---------------------------------------------------------------------------
// CTsySatMessaging::ExtFunc
// 
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CTsySatMessaging::ExtFunc( const TTsyReqHandle /*aTsyReqHandle*/, 
    const TInt /*aIpc*/, const TDataPackage& /*aPackage*/ )
    {       
    // handling of SIM ATK TSY related request
    return KErrNotSupported;
    }    

//  End of File

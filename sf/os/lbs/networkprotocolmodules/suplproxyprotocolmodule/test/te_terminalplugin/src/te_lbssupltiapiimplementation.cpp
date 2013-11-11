/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implementation class for supltiapi
*
*/

#include <ecom/ecom.h>
#include <badesca.h>

#include "te_lbssupltiapiimplementation.h"     




// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CLbsSuplTiApiImplementation::CLbsSuplTiApiImplementation
// Other items were commented in a header
// -----------------------------------------------------------------------------
//
CLbsSuplTiApiImplementation::CLbsSuplTiApiImplementation(MLbsSuplTiObserver& aObserver):CLbsSuplTiApi(aObserver)
        { 

        }

// -----------------------------------------------------------------------------
// CLbsSuplTiApiImplementation::~CLbsSuplTiApiImplementation
// Other items were commented in a header
// -----------------------------------------------------------------------------
//
EXPORT_C CLbsSuplTiApiImplementation::~CLbsSuplTiApiImplementation()
    {

    }

// -----------------------------------------------------------------------------
// CLbsSuplTiApiImplementation::NewL
// Other items were commented in a header
// -----------------------------------------------------------------------------
//

EXPORT_C  CLbsSuplTiApiImplementation* CLbsSuplTiApiImplementation::NewL(MLbsSuplTiObserver& aObserver)
    {
    CLbsSuplTiApiImplementation* self = new(ELeave) CLbsSuplTiApiImplementation(aObserver);
    CleanupStack::PushL(self);
    self->ConstructL();//remove observer
    CleanupStack::Pop(self);
    return self;
    }           

// -----------------------------------------------------------------------------
// CLbsSuplTiApiImplementation::ConstructL
// Other items were commented in a header
// -----------------------------------------------------------------------------
//
void CLbsSuplTiApiImplementation::ConstructL() 
    {
    }           

// -----------------------------------------------------------------------------
// CLbsSuplTiApiImplementation::RequestLocation
// Other items were commented in a header
// -----------------------------------------------------------------------------
//
void CLbsSuplTiApiImplementation::RequestLocation(const TLbsNetSessionId& /*aSessionId*/,
                                  const TLbsNetPosRequestOptionsBase& /*aOptions*/, const TLbsNetPosRequestMethod& /*aMethod*/)
    {
    
    }
// -----------------------------------------------------------------------------
// CLbsSuplTiApiImplementation::CancelRequest
// Other items were commented in a header
// -----------------------------------------------------------------------------
//
TInt CLbsSuplTiApiImplementation::CancelRequest(const TLbsNetSessionId& /*aSessionId*/)
    {
    return KErrNone;
    }
// -----------------------------------------------------------------------------
// CLbsSuplTiApiImplementation::CloseSession
// Other items were commented in a header
// -----------------------------------------------------------------------------
//
void CLbsSuplTiApiImplementation::CloseSession()
    {
    }
// -----------------------------------------------------------------------------
// CLbsSuplTiApiImplementation::GetPosition
// Other items were commented in a header
// -----------------------------------------------------------------------------
//
TInt CLbsSuplTiApiImplementation::GetPosition(const TLbsNetSessionId& /*aSessionId*/, TPositionInfoBase& /*aPositionInfo*/)
    {
    return KErrNone;
    }

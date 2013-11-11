// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Description:
//



// INCLUDE FILES
#include "epos_cpospsyresult.h"

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CPosPSYResult::CPosPSYResult()
    {
    }

// EPOC default constructor can leave.
void CPosPSYResult::ConstructL(
    const TDesC& aMessage, 
    const TInt aResultType)
    {
    iResultType = aResultType;    
    
    iMessage = HBufC::NewL(aMessage.Length());
    TPtr messageAppend(iMessage->Des());
    messageAppend = aMessage;
    }

// Two-phased constructor.
CPosPSYResult* CPosPSYResult::NewLC(
    const TDesC& aMessage,
    const TInt aResultType)
    {
    CPosPSYResult* self = new (ELeave) CPosPSYResult;
    CleanupStack::PushL(self);
    self->ConstructL(aMessage, aResultType);
    return self;
    }

// Two-phased constructor.
CPosPSYResult* CPosPSYResult::NewL(
    const TDesC& aMessage,
    const TInt aResultType)
    {
    CPosPSYResult* self = CPosPSYResult::NewLC(aMessage, aResultType);
    CleanupStack::Pop(self);
    return self;
    }

// Destructor
CPosPSYResult::~CPosPSYResult()
    {
    delete iMessage;
    }

// ---------------------------------------------------------
// CPosPSYResult::Message
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
HBufC* CPosPSYResult::Message() const
    {
    return iMessage;
    }

// ---------------------------------------------------------
// CPosPSYResult::ResultType
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CPosPSYResult::ResultType() const
    {
    return iResultType;
    }

//  End of File

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
#include "epos_cpospsyresults.h"

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CPosPSYResults::CPosPSYResults()
    {
    }

// EPOC default constructor can leave.
void CPosPSYResults::ConstructL()
    {
    }

// Two-phased constructor.
CPosPSYResults* CPosPSYResults::NewL()
    {
    CPosPSYResults* self = new (ELeave) CPosPSYResults;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// Destructor
CPosPSYResults::~CPosPSYResults()
    {
    iResultArray.ResetAndDestroy();
	iResultArray.Close();
    }

// ---------------------------------------------------------
// CPosPSYResults::AddResultL
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYResults::AddResultL(
    const CPosPSYResult& aResult)
    {
	User::LeaveIfError(iResultArray.Append(&aResult));
    }

// ---------------------------------------------------------
// CPosPSYResults::NumberOfResults
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CPosPSYResults::NumberOfResults() const
    {
    return iResultArray.Count();
    }

// ---------------------------------------------------------
// CPosPSYResults::GetResult
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
CPosPSYResult* CPosPSYResults::GetResult(
    const TInt aIndex)
    {
    return iResultArray[aIndex];
    }

// ---------------------------------------------------------
// CPosPSYResults::ClearResults
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYResults::ClearResults()
    {
    iResultArray.ResetAndDestroy();
    }
//  End of File

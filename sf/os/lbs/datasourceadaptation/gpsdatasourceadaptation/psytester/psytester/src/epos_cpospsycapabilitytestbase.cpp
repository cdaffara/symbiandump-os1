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
#include <lbspositioninfo.h>
#include "epos_cpospsycapabilitytestbase.h"

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CPosPSYCapabilityTestBase::CPosPSYCapabilityTestBase()
    {
    }

// ---------------------------------------------------------
// CPosPSYCapabilityTestBase::BaseConstructL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYCapabilityTestBase::BaseConstruct(
    CPosPSYResults& aResults,
    const RPointerArray<TPositionInfo>& aPositionArray,
    const TPosPSYInfo& aPSYInfo)
    {
    iTestResults = &aResults;
    iPositionArray = &aPositionArray;
    iPSYInfo = aPSYInfo;
    }

// Destructor
CPosPSYCapabilityTestBase::~CPosPSYCapabilityTestBase()
    {
    }

// ---------------------------------------------------------
// CPosPSYCapabilityTestBase::AddResultL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYCapabilityTestBase::AddResultL(
    const TDesC& aMessage, 
    const TInt aResultType)
    {
    CPosPSYResult* result = CPosPSYResult::NewLC(aMessage, aResultType);
    iTestResults->AddResultL(*result);
    CleanupStack::Pop(result);
    }

//  End of File

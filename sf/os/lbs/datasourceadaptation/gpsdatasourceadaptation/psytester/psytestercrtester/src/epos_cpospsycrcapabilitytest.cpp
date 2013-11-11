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
#include <EikDll.h>
#include <apacmdln.h>

#include "EPos_CPosPSYCRCapabilityTest.h"

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
EPos_CPosPSYCRCapabilityTest::EPos_CPosPSYCRCapabilityTest()
    {
    }

// EPOC default constructor can leave.
void EPos_CPosPSYCRCapabilityTest::ConstructL()
    {
    BaseConstructL();
    }

EPos_CPosPSYCRCapabilityTest* EPos_CPosPSYCRCapabilityTest::NewL()
    {
    EPos_CPosPSYCRCapabilityTest* self = new (ELeave) EPos_CPosPSYCRCapabilityTest;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// Destructor
EPos_CPosPSYCRCapabilityTest::~EPos_CPosPSYCRCapabilityTest()
    {
    }

// ---------------------------------------------------------
// CPosPSYParameterTest::GetName
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void EPos_CPosPSYCRCapabilityTest::GetName(
    TDes& aName) const
    {
    _LIT(KTestName, "PSY Central Repository Capability Test.");
    aName = KTestName;
    }

//  End of File

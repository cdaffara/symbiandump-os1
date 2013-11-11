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
* Description:  Factory class for vibra control
*
*/


// INCLUDE FILES
#include <e32std.h>
#include "vibractrl.h"
#include "vibractrlwrapperimpl.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// VibraFactory::VibraFactory()
// C++ default constructor.
// -----------------------------------------------------------------------------
//
VibraFactory::VibraFactory()
    {
    //...
    }

// -----------------------------------------------------------------------------
// VibraFactory::NewL
// Two-phased constructor without notify handling.
// -----------------------------------------------------------------------------
//
EXPORT_C CVibraControl* VibraFactory::NewL()
    {
    CVibraControl *p = NewLC(NULL);
    CleanupStack::Pop();
    return p;
    }

// -----------------------------------------------------------------------------
// VibraFactory::NewL
// Two-phased constructor with notify handling.
// -----------------------------------------------------------------------------
//
EXPORT_C CVibraControl* VibraFactory::NewL(MVibraControlObserver* aCallback)
    {
    CVibraControl *p = NewLC(aCallback);
    CleanupStack::Pop();
    return p;
    }

// -----------------------------------------------------------------------------
// VibraFactory::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CVibraControl* VibraFactory::NewLC(MVibraControlObserver* aCallback)
    {
    CVibraControlImpl *p = new (ELeave)CVibraControlImpl(aCallback);
    CleanupStack::PushL(p);
    p->ConstructL();
    return p;
    }

//  End of File

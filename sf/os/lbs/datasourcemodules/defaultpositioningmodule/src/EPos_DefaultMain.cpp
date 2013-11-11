// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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



// INCLUDES
#include <e32base.h>
#include <ecom/implementationproxy.h>
#include "EPos_CPosDefaultPositioner.h"
#include "eposdefaultpsy.hrh"

// CONSTANTS
const TImplementationProxy KFactoryPtr =
    {{KPosDefaultPsyImplUid}, (TProxyNewLPtr)(CPosDefaultPositioner::NewL)};

// ================= OTHER EXPORTED FUNCTIONS ==============

// ---------------------------------------------------------
// ImplementationGroupProxy
// Required ECom interface.
// ---------------------------------------------------------
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
    {
    aTableCount = 1;
    return &KFactoryPtr;
    }

//  End of File

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
#include "EPos_CPosSimulationPositioner.h"
#include "epossimulationpsy.hrh"

// ================= OTHER EXPORTED FUNCTIONS ==============

const TImplementationProxy KFactoryPtr =
    IMPLEMENTATION_PROXY_ENTRY(KPosSimulationPsyImplUid,
        CPosSimulationPositioner::NewL);

//
// ---------------------------------------------------------
// ImplementationGroupProxy
// Required PSY interface.
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(
    TInt& aTableCount)
    {
    aTableCount = 1;
    return &KFactoryPtr;
    }

//  End of File

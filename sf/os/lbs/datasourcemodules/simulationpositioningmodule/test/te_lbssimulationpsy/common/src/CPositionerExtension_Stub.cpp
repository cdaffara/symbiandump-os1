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



// INCLUDE FILES
#include "EPos_CPositionerExtension.h"
#include "EPos_MPosStatusObserver.h"
#include "EPos_CPositionerRegistry.h"
#include "EPos_CPosPsyExtension.h"

// ================= MEMBER FUNCTIONS =======================

// Static constructor.
CPositionerExtension* CPositionerExtension::NewL(
    TUid aImplementationUid,
    MPosStatusObserver* aStatusObserver,
    MPosParameterObserver* aParamObserver)
    {
    CPositionerExtension* self = new (ELeave) CPositionerExtension(
        aParamObserver);
    CleanupStack::PushL(self);
    self->ConstructL(aImplementationUid, aStatusObserver);
    CleanupStack::Pop(self);
    return self;
    }

// 2nd phase contruction
void CPositionerExtension::ConstructL(
    TUid aImplementationUid,
    MPosStatusObserver* aStatusObserver)
    {
    iRegistry = CPositionerRegistry::InstanceL(aStatusObserver);
    iPsyExtension = iRegistry->RegisterPositionerL(aImplementationUid);
    }

// Private constructor.
CPositionerExtension::CPositionerExtension(
    MPosParameterObserver* aParamObserver)
:   iParamObserver(aParamObserver)
    {
    }

// Destructor
CPositionerExtension::~CPositionerExtension()
    {
    if (iRegistry)
        {
        if (iPsyExtension)
            {
            TUid implementationUid = iPsyExtension->iImplementationUid;
            iRegistry->DeregisterPositioner(implementationUid);
            }
        iRegistry->Release();
        iRegistry = NULL;
        }
    iPsyExtension = NULL;
    }

//  End of File  

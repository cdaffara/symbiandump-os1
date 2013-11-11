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
#include "EPos_CPositionerRegistry.h"
#include "EPos_MPosStatusObserver.h"
#include "EPos_CPosPsyExtension.h"
#include <e32std.h>

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor
CPositionerRegistry::CPositionerRegistry(
    MPosStatusObserver* aStatusObserver)
:   iStatusObserver(aStatusObserver)
    {
    }
    
// Destructor
CPositionerRegistry::~CPositionerRegistry()
    {
    iPsyExtensions.ResetAndDestroy();
    }

TInt 					CPositionerRegistry::iInstanceCount = 0;
CPositionerRegistry* 	CPositionerRegistry::iInstance = 0;
// Static Singleton constructor.
CPositionerRegistry* CPositionerRegistry::InstanceL(
    MPosStatusObserver* aStatusObserver)
    {
    CPositionerRegistry* self;
    if(!iInstance)
    	{
    	self = new (ELeave) CPositionerRegistry(aStatusObserver);
    	}
    self->iInstanceCount++;
	self->iRefCount++;
	return self;
    }

// Singleton destructor
void CPositionerRegistry::Release()
    {
	if (--iRefCount == 0)
		{
		delete this;
		}

    }

// ---------------------------------------------------------
// CPositionerRegistry::RegisterPositionerL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
CPosPsyExtension* CPositionerRegistry::RegisterPositionerL(TUid aImplementationUid)
    {
    TInt pos = Find(aImplementationUid);
    CPosPsyExtension* psyExtension = NULL;
    if (pos == KErrNotFound)
        {
        psyExtension = new (ELeave) CPosPsyExtension(
            aImplementationUid, iStatusObserver);
        CleanupStack::PushL(psyExtension);
        User::LeaveIfError(iPsyExtensions.Append(psyExtension));
        CleanupStack::Pop();
        }
    else
        {
        psyExtension = iPsyExtensions[pos];
        }
    psyExtension->RegisterPositioner();
    return psyExtension;
    }

// ---------------------------------------------------------
// CPositionerRegistry::DeregisterPositioner
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPositionerRegistry::DeregisterPositioner(TUid aImplementationUid)
    {
    TInt pos = Find(aImplementationUid);
    if (pos != KErrNotFound)
        {
        CPosPsyExtension* psyExtension = iPsyExtensions[pos];
        if (psyExtension->DeregisterPositioner() == 0)
            {
            // There are no more positioners using this PSY
            delete psyExtension;
            iPsyExtensions.Remove(pos);
            }
        }
    }

// ---------------------------------------------------------
// CPositionerRegistry::Find
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CPositionerRegistry::Find(TUid aImplementationUid)
    {
    TInt count = iPsyExtensions.Count();
    for (TInt i = 0; i < count; i++)
        {
        if (iPsyExtensions[i]->iImplementationUid == aImplementationUid)
            {
            return i;
            }
        }
    return KErrNotFound;
    }

//  End of File  

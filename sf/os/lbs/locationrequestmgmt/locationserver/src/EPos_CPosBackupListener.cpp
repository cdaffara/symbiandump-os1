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
#include <e32base.h>
#include <connect/sbdefs.h>
#include <e32property.h>
#include "EPos_ServerPanic.h"
#include "EPos_CPosBackupListener.h"

using namespace conn;

// ================= MEMBER FUNCTIONS =======================

/**
 * C++ Constructor
 */
CPosBackupListener::CPosBackupListener()
    : CActive(EPriorityStandard)
    {
    CActiveScheduler::Add(this);
    }

/**
 * C++ Destructor
 */
CPosBackupListener::~CPosBackupListener()
    {
    Cancel();
    iBackupProperty.Close();
    iCallback = NULL;
    }

/**
 * Two-phase constructor
 */
CPosBackupListener* CPosBackupListener::NewL()
    {
    CPosBackupListener* self = new (ELeave) CPosBackupListener;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

/**
 * Symbian default constructor
 */
void CPosBackupListener::ConstructL()
    {
    User::LeaveIfError(
        iBackupProperty.Attach(KUidSystemCategory, KUidBackupRestoreKey));
    }

/**
 * Start waiting for event.
 * @param aCallback Entity to inform when request completes.
 */
void CPosBackupListener::Start(MPosBackupEventCallback& aCallback)
    {
    iCallback = &aCallback;
    __ASSERT_DEBUG(iCallback, DebugPanic(EPosServerPanicGeneralInconsistency));
    iBackupProperty.Subscribe(iStatus);
    SetActive();
    }

/**
 * From CActive
 */
void CPosBackupListener::RunL()
    {
    if (iCallback)
        {
        iCallback->HandleBackupOperationEventL();
        iCallback = NULL; // observer must restart this listener
        }
    }

/**
 * From CActive
 */
TInt CPosBackupListener::RunError(TInt /*aError*/)
    {
    return KErrNone;
    }

/**
 * From CActive
 */
void CPosBackupListener::DoCancel()
    {
    iBackupProperty.Cancel();
    }

/**
 * Checks if backup or restore operation is ongoing by
 * reading Backup & Restore P&S property
 * @return ETrue if backup or restore is running, EFalse otherwise.
 */
TBool CPosBackupListener::IsBackupOperationOngoing()
    {
    TBURPartType state;
    TBackupIncType type;
    if (GetCurrentValue(state, type) == KErrNone)
        {
        return ((state != EBURUnset) && (state != EBURNormal));
        }
    else
        {
        return EFalse;
        }
    }

/**
 * Reads current value of Backup & Restore P&S property
 * @param aState Backup state as defined in <sbdefs.h>
 * @param aBackupType Backup type as defined in <sbdefs.h>
 */
TInt CPosBackupListener::GetCurrentValue(TBURPartType& aState, TBackupIncType& aType)
    {
    TInt res, value;
    res = iBackupProperty.Get(value);
    if (res == KErrNone)
        {
        aState = (TBURPartType) (value & KBURPartTypeMask);
        aType = (TBackupIncType) (value & KBackupIncTypeMask);
        }

    return res;
    }

// End of File

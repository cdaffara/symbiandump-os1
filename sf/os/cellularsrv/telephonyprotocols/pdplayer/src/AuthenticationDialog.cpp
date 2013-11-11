// Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Interface between the PDP and the connection agent dialog server implementation
//
//

/**
 @file
 @internalComponent
*/

#include "AuthenticationDialog.h"

//
// CAuthenticationDialog definitions
//

CAuthenticationDialog* CAuthenticationDialog::NewL(TInt aPriority)
    {
    CAuthenticationDialog* self = new(ELeave) CAuthenticationDialog(aPriority);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(); // self
    return self;
    }

CAuthenticationDialog::CAuthenticationDialog(TInt aPriority) 
    : CActive(aPriority)
    {}

void CAuthenticationDialog::ConstructL()
    {
    CActiveScheduler::Add(this);
    TInt ret = iDlgServ.Connect();
    User::LeaveIfError(ret);
    }

CAuthenticationDialog::~CAuthenticationDialog()
    {
    Cancel();
    iDlgServ.Close();
    }

void CAuthenticationDialog::Authenticate(MAuthenticationDialogObserver& aObserver, TDes& aUsername,TDes& aPassword,TBool aIsReconnect)
/**
Request authentication information (pops up authentication dialog)
*/
    {
    __ASSERT_DEBUG(iObserver == NULL, User::Invariant());
    iDlgServ.Authenticate(aUsername,aPassword,aIsReconnect,iStatus);
    iObserver = &aObserver;
    CActive::SetActive();
    }

void CAuthenticationDialog::RunL()
    {
    MAuthenticationDialogObserver* observer = iObserver;
    iObserver = NULL;

    observer->AuthenticationCompleteL(iStatus.Int());
    }

void CAuthenticationDialog::DoCancel()
    {
    iDlgServ.CancelAuthenticate();
    iObserver = NULL;
    }

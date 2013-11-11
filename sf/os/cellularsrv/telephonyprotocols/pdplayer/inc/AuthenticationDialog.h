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
// Interface between the PDP and the connection agent dialog server.
//

/**
 @file
 @internalComponent
*/

#ifndef SYMBIAN_AUTHENTICATION_DIALOG_H
#define SYMBIAN_AUTHENTICATION_DIALOG_H

#include <e32base.h>
#include <agentdialog.h>

class MAuthenticationDialogObserver
/**
MAuthenticationDialogObserver

Mixin observer class to be derived from by classes wishing to use CAuthenticationDialog.
When the Authenticate function call completes the AuthenticationComplete function will
be called on the observer.
@internalTechnology
*/
    {
public:
    virtual void AuthenticationCompleteL(TInt aError) = 0;
    };

class CAuthenticationDialog : public CActive
/**
CAuthenticationDialog

Has an RGenConAgentDialogServer class which provides the interface
to the dialog server.

@internalTechnology
*/
    {
public:
    static CAuthenticationDialog* NewL(TInt aPriority = CActive::EPriorityStandard);
    ~CAuthenticationDialog();

    /**
     * The aIsReconnect parameter indicates to the dialog server if it is a reconnection attempt.
     * The reasoning behind providing this information is that the dialog server implementation may choose to 
     * suppress the dialog if this is a reconnection and just silently return the values entered by the user 
     * at the first connection attempt. Here the dafault value is False, so dialog server will prompt for
     * authentication information all the time.
     */
    void Authenticate(MAuthenticationDialogObserver& aObserver, TDes& aUsername, TDes& aPassword, TBool aIsReconnect = EFalse);

private:
    CAuthenticationDialog(TInt aPriority);
    void ConstructL();

    /** From CActive */
    virtual void DoCancel();
    virtual void RunL();

private:
    MAuthenticationDialogObserver* iObserver;
    RGenConAgentDialogServer iDlgServ;
    };

#endif

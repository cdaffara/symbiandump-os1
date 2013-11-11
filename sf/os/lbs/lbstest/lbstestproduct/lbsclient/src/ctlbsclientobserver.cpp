// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include    "ctlbsclientobserver.h"


// CONSTANTS
// Apparently the EposServer name is same on WINS and ARMI
_LIT(KEPosServerName, "!PosServer");

_LIT(KPrivacyServerName, "eposprivsrv"); // TBD: Is this name OK?

// ================= MEMBER FUNCTIONS =======================


// ---------------------------------------------------------
// CT_LbsClientObserver::WaitForEPosShutdown
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientObserver::WaitForEPosShutdown()
    {
    WaitForProcessDeath(KEPosServerName);
    }

void EPosShutdownWaitCleanupItem(TAny*)
    {
    CT_LbsClientObserver::WaitForEPosShutdown();
    }

// ---------------------------------------------------------
// CT_LbsClientObserver::EPosShutdownWaitPushL
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientObserver::EPosShutdownWaitPushL()
    {
    CleanupStack::PushL(TCleanupItem(EPosShutdownWaitCleanupItem, NULL)); 
    }

// ---------------------------------------------------------
// CT_LbsClientObserver::EPosAliveL
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool CT_LbsClientObserver::EPosAliveL()
    {
    return ProcessAliveL(KEPosServerName);
    }

// ---------------------------------------------------------
// CT_LbsClientObserver::WaitForPrivacyServerShutdown
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientObserver::WaitForPrivacyServerShutdown()
    {
    WaitForProcessDeath(KPrivacyServerName);
    }

void PrivacyServerShutdownWaitCleanupItem(TAny*)
    {
    CT_LbsClientObserver::WaitForPrivacyServerShutdown();
    }

// ---------------------------------------------------------
// CT_LbsClientObserver::PrivacyServerShutdownWaitPushL
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientObserver::PrivacyServerShutdownWaitPushL()
    {
    CleanupStack::PushL(TCleanupItem (PrivacyServerShutdownWaitCleanupItem, NULL));
    }

// ---------------------------------------------------------
// CT_LbsClientObserver::PrivacyServerAliveL
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool CT_LbsClientObserver::PrivacyServerAliveL()
    {
    return ProcessAliveL(KPrivacyServerName);
    }

// ---------------------------------------------------------
// CT_LbsClientObserver::WaitForProcessDeath
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientObserver::WaitForProcessDeath(const TDesC& aProcessName)
    {
#ifdef __WINS__
    RThread process;
#else
    RProcess process;
#endif

    if (process.Open(aProcessName))
        {
        return;
        }

    TRequestStatus status;
    process.Logon(status);
    User::WaitForRequest(status);

    process.Close();
    }

// ---------------------------------------------------------
// CT_LbsClientObserver::ProcessAliveL
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool CT_LbsClientObserver::ProcessAliveL(const TDesC& aProcessName)
    {
	TFindServer find(aProcessName);
    TFullName   name;
    if (find.Next(name) == KErrNone) // server found
		{
		return ETrue;
		}
	else 
		{
		return EFalse;
		}
    }

//  End of File  

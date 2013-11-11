// Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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



//  INCLUDES
#include "ctlbsposclient.h"
#include "ctlbsposclientholder.h"
#include <e32svr.h>
#include <lbsipc.h>

// CONSTANTS

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CT_LbsPosClient::CT_LbsPosClient(CT_LbsPosClientHolder* aParent) 
: CActive(EPriorityHigh), iParent(aParent)
    {
    CActiveScheduler::Add(this);
    }
     
// Destructor
CT_LbsPosClient::~CT_LbsPosClient()
    {
    Cancel();
    iPositioner.Close();
    if (!iSameClient)
        iPosServer.Close();
    }

// ---------------------------------------------------------
// CPosClient::ConstructL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsPosClient::ConstructL(TUid aPsy)
    {
    //connect to EPos
    User::LeaveIfError(iPosServer.Connect());
    iSameClient = EFalse;
    //Open subsession
    User::LeaveIfError(iPositioner.Open(iPosServer, aPsy));
    iModuleInfo = TPositionInfo();
    }

// ---------------------------------------------------------
// CPosClient::ConstructL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsPosClient::ConstructL(TUid aPsy, RPositionServer& aPositionServer)
    {
    //connect to EPos
    //User::LeaveIfError(iPosServer.Connect());
    iPosServer = aPositionServer;
    iSameClient = ETrue;
    //Open subsession
    User::LeaveIfError(iPositioner.Open(iPosServer, aPsy));
    iModuleInfo = TPositionInfo();
    }
// ---------------------------------------------------------
// CPosClient::NewL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
CT_LbsPosClient* CT_LbsPosClient::NewL(CT_LbsPosClientHolder* aParent, TUid aPsy)
    {
    CT_LbsPosClient* self = new (ELeave) CT_LbsPosClient(aParent);
    CleanupStack::PushL(self);
    self->ConstructL(aPsy);
    CleanupStack::Pop(self); // self
    return self;
    }

// ---------------------------------------------------------
// CPosClient::NewL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
CT_LbsPosClient* CT_LbsPosClient::NewL(CT_LbsPosClientHolder* aParent, TUid aPsy, RPositionServer& aPositionServer)
    {
    CT_LbsPosClient* self = new (ELeave) CT_LbsPosClient(aParent);
    CleanupStack::PushL(self);
    self->ConstructL(aPsy, aPositionServer);
    CleanupStack::Pop(self); // self
    return self;
    }


// ---------------------------------------------------------
// CPosClient::MakeRequest
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsPosClient::MakeRequest()
    {
    _LIT(KContact, "11111");
    _LIT(KService, "SAAA");

    //TPositionInfo moduleInfo = TPositionInfo();

    CRequestor* contactId = 
      CRequestor::New(CRequestor::ERequestorContact, 
      CRequestor::EFormatTelephone, KContact);
    
    CRequestor* servId = 
      CRequestor::New(CRequestor::ERequestorService, 
      CRequestor::EFormatApplication, KService);
    
    RRequestorStack stack = RRequestorStack();
    stack.Append(contactId);
    stack.Append(servId);
    
    CleanupStack::PushL(contactId);
    CleanupStack::PushL(servId);
    
    TInt err = iPositioner.SetRequestor(stack);
    // what to do with err ???
    
    if (err)
      {
      // Do something here?
      }
    stack.Close();
    
    CleanupStack::PopAndDestroy(servId);
    CleanupStack::PopAndDestroy(contactId);
    
    iStartTime.UniversalTime();

    iPositioner.NotifyPositionUpdate(iModuleInfo, iStatus);
    SetActive();
    }

// ---------------------------------------------------------
// CPosClient::RunL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsPosClient::RunL()
    {
    if (iStatus == KErrNone)
        {
        TTime now;
        now.UniversalTime();
        iRequestTime = now.MicroSecondsFrom(iStartTime);

        iParent->HandleRequestDone();
        }
    else
        {
        _LIT(KErr, "iStatus != KErrNone");
        RDebug::Print(KErr);
        iParent->HandleRequestDone();
        }
    }

// ---------------------------------------------------------
// CPosClient::DoCancel
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsPosClient::DoCancel()
    {
    iPositioner.CancelRequest(EPositionerNotifyPositionUpdate); // Specified in LbsIpc.h
    TTime now;
    now.UniversalTime();
    iRequestTime = now.MicroSecondsFrom(iStartTime);
    }

// ---------------------------------------------------------
// CPosClient::GetResult
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsPosClient::GetResult(
    TInt& aStatus, 
    TPositionInfo& aModuleInfo,
    TTimeIntervalMicroSeconds& aRequestTime)
    {
    aStatus = iStatus.Int();
    aModuleInfo = iModuleInfo;
    aRequestTime = iRequestTime;
    }

// End of File

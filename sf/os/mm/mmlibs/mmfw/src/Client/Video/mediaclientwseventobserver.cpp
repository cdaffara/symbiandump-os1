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
//

#include "mediaclientwseventobserver.h"
#include "mediaclientvideotrace.h"

CMediaClientWsEventObserver* CMediaClientWsEventObserver::NewL(MMediaClientWsEventObserverCallback& aCallback)
    {
    DEBUG_PRINTF("CMediaClientWsEventObserver::NewL +++");
    CMediaClientWsEventObserver* self = new (ELeave) CMediaClientWsEventObserver(aCallback);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    DEBUG_PRINTF("CMediaClientWsEventObserver::NewL ---");
    return self;
    }

CMediaClientWsEventObserver::CMediaClientWsEventObserver(MMediaClientWsEventObserverCallback& aCallback) :
CActive(EPriorityStandard),
iCallback(aCallback)
    {   
    DEBUG_PRINTF("CMediaClientWsEventObserver::CMediaClientWsEventObserver +++"); 
    DEBUG_PRINTF("CMediaClientWsEventObserver::CMediaClientWsEventObserver ---");
    }

void CMediaClientWsEventObserver::ConstructL()
    {
    DEBUG_PRINTF("CMediaClientWsEventObserver::ConstructL +++");
    
    DEBUG_PRINTF("CMediaClientWsEventObserver::ConstructL RWsSession Connect");
    TInt err = iWs.Connect();
    if (err != KErrNone)
        {
        DEBUG_PRINTF2("CMediaClientWsEventObserver::ConstructL Windows Server unavailable, err %d", err);
        User::Leave(err);
        }
    
    DEBUG_PRINTF("CMediaClientWsEventObserver::ConstructL RWindowGroup Create");
    iWindowGroup = RWindowGroup(iWs);
    User::LeaveIfError(iWindowGroup.Construct((TUint32)this, EFalse));

    // Send created window to the background and hide it from the
    // application switcher    
    iWindowGroup.SetOrdinalPosition( -1, -1000 ); // -1000 = ECoeWinPriorityNeverAtFront

    DEBUG_PRINTF("CMediaClientWsEventObserver::ConstructL RWindowGroup EnableFocusChangeEvents");
    User::LeaveIfError(iWindowGroup.EnableFocusChangeEvents());
    
    UpdateFocusWindowGroupId(ETrue);
    DEBUG_PRINTF3("CMediaClientWsEventObserver::ConstructL Initial WgId %d Error %d", iWgId, iWgIdError);

    CActiveScheduler::Add(this);

    // register with window server to receive focus change notifications
    DEBUG_PRINTF("CMediaClientWsEventObserver::ConstructL iWs.EventReady()");
    iWs.EventReady(&iStatus);

    DEBUG_PRINTF("CMediaClientWsEventObserver::ConstructL SetActive()");
    SetActive();
    
    DEBUG_PRINTF("CMediaClientWsEventObserver::ConstructL ---");
    }

CMediaClientWsEventObserver::~CMediaClientWsEventObserver()
    {
    DEBUG_PRINTF("CMediaClientWsEventObserver::~CMediaClientWsEventObserver +++");

    Cancel();

    iWindowGroup.Close();
    
    iWs.Close();
    
    DEBUG_PRINTF("CMediaClientWsEventObserver::~CMediaClientWsEventObserver ---");
    }

TInt CMediaClientWsEventObserver::FocusWindowGroupId(TInt& aFocusGroupId)
    {
    DEBUG_PRINTF("CMediaClientWsEventObserver::FocusWindowGroupId +++");
    aFocusGroupId = iWgId;
    DEBUG_PRINTF3("CMediaClientWsEventObserver::FocusWindowGroupId --- WgId %d, Error %d", iWgId, iWgIdError);
    return iWgIdError;
    }

void CMediaClientWsEventObserver::UpdateFocusWindowGroupId(TBool aConstruction)
    {
    DEBUG_PRINTF2("CMediaClientWsEventObserver::UpdateFocusWindowGroupId +++, construction %d", aConstruction);
    
    TInt wgId = iWs.GetFocusWindowGroup();
    DEBUG_PRINTF2("CMediaClientWsEventObserver::UpdateFocusWindowGroupId id %d ", wgId);

    // get the thread that owns windowgroup id
    TThreadId threadId;
    iWgIdError = iWs.GetWindowGroupClientThreadId(wgId, threadId);
    if(iWgIdError != KErrNone)
        {
        DEBUG_PRINTF2("CMediaClientWsEventObserver::UpdateFocusWindowGroupId --- Get Thread Id error %d", iWgIdError);
        return;
        }
    
    RThread thread;    
    iWgIdError = thread.Open(threadId);
    if(iWgIdError != KErrNone)
        {
        DEBUG_PRINTF2("CMediaClientWsEventObserver::UpdateFocusWindowGroupId --- Open thread error %d", iWgIdError);
        return;
        }
    
    TSecureId fgThreadId = thread.SecureId();
    thread.Close();
    
    if(iCallback.MmcweoIgnoreProcess(fgThreadId))
        {
        // If ignore returns ETrue during construction the previous value of iWgId does not contain a valid id. It is 0.
        // This is an error case so set error flag accordingly.
        if(aConstruction)
            {
            iWgIdError = KErrNotSupported;    
            }
        }
    else
        {
        iWgId = wgId;
        }

    DEBUG_PRINTF("CMediaClientWsEventObserver::UpdateFocusWindowGroupId --- ");
    }

void CMediaClientWsEventObserver::RunL()
    {
    DEBUG_PRINTF("CMediaClientWsEventObserver::RunL +++");

    iWs.GetEvent(iEvent);    

    DEBUG_PRINTF("CMediaClientWsEventObserver::RunL EventReady()");
    iWs.EventReady(&iStatus);
    DEBUG_PRINTF("CMediaClientWsEventObserver::RunL SetActive()");
    SetActive();

    DEBUG_PRINTF2("CMediaClientWsEventObserver::RunL() Ws event.Type %d", iEvent.Type());
    if(iEvent.Type() == EEventFocusGroupChanged)
        {
        DEBUG_PRINTF("CMediaClientWsEventObserver::RunL Received EEventFocusGroupChanged");
        
        TInt prevWgId = iWgId;
        UpdateFocusWindowGroupId(EFalse);
        if((iWgIdError != KErrNone) || (iWgId != prevWgId))
            {
            iCallback.MmcweoFocusWindowGroupChanged();
            }
        }
    else
        {
        DEBUG_PRINTF("CMediaClientWsEventObserver::RunL Event Ignored");
        }

    DEBUG_PRINTF("CMediaClientWsEventObserver::RunL ---");
    }

void CMediaClientWsEventObserver::DoCancel()
    {
    DEBUG_PRINTF("CMediaClientWsEventObserver::DoCancel +++");
    iWs.EventReadyCancel();
    DEBUG_PRINTF("CMediaClientWsEventObserver::DoCancel ---");
    }

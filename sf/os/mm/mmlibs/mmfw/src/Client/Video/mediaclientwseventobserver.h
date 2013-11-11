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

/**
 @file
 @internalComponent
*/

#ifndef MEDIACLIENTWSEVENTOBSERVER_H
#define MEDIACLIENTWSEVENTOBSERVER_H

#include <e32base.h> 
#include <w32std.h> 

class MMediaClientWsEventObserverCallback
    {
public:
    virtual void MmcweoFocusWindowGroupChanged() = 0;
    virtual TBool MmcweoIgnoreProcess(TSecureId aId) = 0;
    };

NONSHARABLE_CLASS(CMediaClientWsEventObserver) : public CActive
    {
public:
    static CMediaClientWsEventObserver* NewL(MMediaClientWsEventObserverCallback& aCallback);
    ~CMediaClientWsEventObserver();
    
    TInt FocusWindowGroupId(TInt& aFocusGroupId);
    
    // from CActive
    void RunL();
    void DoCancel();
    
private:
    CMediaClientWsEventObserver(MMediaClientWsEventObserverCallback& aCallback);
    void ConstructL();
    void UpdateFocusWindowGroupId(TBool aConstruction);
    
private:
    RWsSession iWs;
    RWindowGroup iWindowGroup;
    MMediaClientWsEventObserverCallback& iCallback;
    TWsEvent iEvent;
    TInt iWgId;
    TInt iWgIdError; 
    };

#endif // MEDIACLIENTWSEVENTOBSERVER_H


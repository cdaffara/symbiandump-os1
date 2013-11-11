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

#include "mediaclientextdisplayhandler.h"
#include "mediaclientvideotrace.h"

CMediaClientExtDisplayHandler* CMediaClientExtDisplayHandler::NewL(TInt aDisplayId)
    {
    DEBUG_PRINTF("CMediaClientExtDisplayHandler::NewL +++");
    CMediaClientExtDisplayHandler* self = new (ELeave) CMediaClientExtDisplayHandler(aDisplayId);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    DEBUG_PRINTF("CMediaClientExtDisplayHandler::NewL ---");
    return self;
    }

CMediaClientExtDisplayHandler::CMediaClientExtDisplayHandler(TInt aDisplayId)
: iDisplayId(aDisplayId)
    {   
    DEBUG_PRINTF("CMediaClientExtDisplayHandler::CMediaClientExtDisplayHandler +++"); 
    DEBUG_PRINTF("CMediaClientExtDisplayHandler::CMediaClientExtDisplayHandler ---");
    }

void CMediaClientExtDisplayHandler::ConstructL()
    {
    DEBUG_PRINTF("CMediaClientExtDisplayHandler::ConstructL +++");
    
    DEBUG_PRINTF("CMediaClientExtDisplayHandler::ConstructL RWsSession Connect");
    TInt err = iWs.Connect();
    if (err != KErrNone)
        {
        DEBUG_PRINTF("CMediaClientExtDisplayHandler::ConstructL Windows Server unavailable");
        User::Leave(err);
        }

    DEBUG_PRINTF("CMediaClientExtDisplayHandler::ConstructL CWsScreenDevice Create");
    iScreen = new (ELeave) CWsScreenDevice(iWs);
    User::LeaveIfError(iScreen->Construct(iDisplayId));

    DEBUG_PRINTF("CMediaClientExtDisplayHandler::ConstructL RWindowGroup Create");
    iRootWindow = RWindowGroup(iWs);
    User::LeaveIfError(iRootWindow.Construct((TUint32)this, ETrue));

    DEBUG_PRINTF2("CMediaClientExtDisplayHandler::ConstructL RWindowGroupId %d", iRootWindow.WindowGroupId());
    
    DEBUG_PRINTF("CMediaClientExtDisplayHandler::ConstructL RWindow Create");
    iExternalDisplayWindow = RWindow(iWs);
    User::LeaveIfError(iExternalDisplayWindow.Construct(iRootWindow,((TUint32)(this)) + 1));
    DEBUG_PRINTF2("CMediaClientExtDisplayHandler::ConstructL - iExternalDisplayWindow WsHandle 0x%X", iExternalDisplayWindow.WsHandle());

    DEBUG_PRINTF("CMediaClientExtDisplayHandler::ConstructL Setup external window");
    iExternalDisplayWindow.SetExtent(TPoint(0,0), iScreen->SizeInPixels());
    iExternalDisplayWindow.SetVisible(ETrue);
    iExternalDisplayWindow.Activate();
    iWs.Flush();
    
    DEBUG_PRINTF("CMediaClientExtDisplayHandler::ConstructL ---");
    }

CMediaClientExtDisplayHandler::~CMediaClientExtDisplayHandler()
    {
    DEBUG_PRINTF("CMediaClientExtDisplayHandler::~CMediaClientExtDisplayHandler +++");
    
    iExternalDisplayWindow.Close();
    iRootWindow.Close();
    delete iScreen;
    iScreen = NULL;
    iWs.Close();
    
    DEBUG_PRINTF("CMediaClientExtDisplayHandler::~CMediaClientExtDisplayHandler ---");
    }

RWindowBase* CMediaClientExtDisplayHandler::Window()
    {
    DEBUG_PRINTF("CMediaClientExtDisplayHandler::Window +++");
    DEBUG_PRINTF("CMediaClientExtDisplayHandler::Window ---");
    return &iExternalDisplayWindow;
    }

TSize CMediaClientExtDisplayHandler::DisplaySizeInPixels()
    {
    DEBUG_PRINTF("CMediaClientExtDisplayHandler::DisplaySizeInPixels +++");
    DEBUG_PRINTF("CMediaClientExtDisplayHandler::DisplaySizeInPixels ---");
    return iScreen->SizeInPixels();
    }

void CMediaClientExtDisplayHandler::UpdateWindow()
    {
    DEBUG_PRINTF("CMediaClientExtDisplayHandler::UpdateWindow +++");
    iExternalDisplayWindow.SetExtent(TPoint(0,0), iScreen->SizeInPixels());
    DEBUG_PRINTF("CMediaClientExtDisplayHandler::UpdateWindow ---");
    }

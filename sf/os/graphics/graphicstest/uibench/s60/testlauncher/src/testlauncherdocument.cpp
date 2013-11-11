 // Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "testlauncherappUi.h"
#include "testlauncherdocument.h"


CHelloWorldBasicDocument* CHelloWorldBasicDocument::NewL(CEikApplication& aApp)
    {
    CHelloWorldBasicDocument* self = NewLC(aApp);
    CleanupStack::Pop(self);
    return self;
    }

CHelloWorldBasicDocument* CHelloWorldBasicDocument::NewLC(CEikApplication& aApp)
    {
    CHelloWorldBasicDocument* self = new (ELeave) CHelloWorldBasicDocument(aApp);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

void CHelloWorldBasicDocument::ConstructL()
    {
    // empty
    }

CHelloWorldBasicDocument::CHelloWorldBasicDocument(CEikApplication& aApp) : CAknDocument(aApp)
    {
    // empty
    }

CHelloWorldBasicDocument::~CHelloWorldBasicDocument()
    {
    // empty
    }

CEikAppUi* CHelloWorldBasicDocument::CreateAppUiL()
    {
    // Create the application user interface, and return a pointer to it;
    // the framework takes ownership of this object
    return static_cast<CEikAppUi*>(new (ELeave) CHelloWorldBasicAppUi);
    }

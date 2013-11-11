// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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




#include "HWRMLightTestBappui.h"
#include "HWRMLightTestBdocument.h"

CHWRMLightTestBDocument* CHWRMLightTestBDocument::NewL(CEikApplication& aApp)
    {
    CHWRMLightTestBDocument* self = NewLC(aApp);
    CleanupStack::Pop(self);
    return self;
    }

CHWRMLightTestBDocument* CHWRMLightTestBDocument::NewLC(CEikApplication& aApp)
    {
    CHWRMLightTestBDocument* self = new (ELeave) CHWRMLightTestBDocument(aApp);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

void CHWRMLightTestBDocument::ConstructL()
    {
	// No implementation required
    }    

CHWRMLightTestBDocument::CHWRMLightTestBDocument(CEikApplication& aApp) : CEikDocument(aApp) 
    {
	// No implementation required
    }

CHWRMLightTestBDocument::~CHWRMLightTestBDocument()
    {
	// No implementation required
    }

CEikAppUi* CHWRMLightTestBDocument::CreateAppUiL()
    {
    // Create the application user interface, and return a pointer to it;
    // the framework takes ownership of this object
	return (static_cast <CEikAppUi*> (new (ELeave) CHWRMLightTestBAppUi)); 
    }


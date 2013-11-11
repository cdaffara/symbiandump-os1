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



#include "HWRMLightTestFappui.h"
#include "HWRMLightTestFdocument.h"

CHWRMLightTestFDocument* CHWRMLightTestFDocument::NewL(CEikApplication& aApp)
    {
    CHWRMLightTestFDocument* self = NewLC(aApp);
    CleanupStack::Pop(self);
    return self;
    }

CHWRMLightTestFDocument* CHWRMLightTestFDocument::NewLC(CEikApplication& aApp)
    {
    CHWRMLightTestFDocument* self = new (ELeave) CHWRMLightTestFDocument(aApp);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

void CHWRMLightTestFDocument::ConstructL()
    {
	// No implementation required
    }    

CHWRMLightTestFDocument::CHWRMLightTestFDocument(CEikApplication& aApp) : CEikDocument(aApp) 
    {
	// No implementation required
    }

CHWRMLightTestFDocument::~CHWRMLightTestFDocument()
    {
	// No implementation required
    }

CEikAppUi* CHWRMLightTestFDocument::CreateAppUiL()
    {
    // Create the application user interface, and return a pointer to it;
    // the framework takes ownership of this object
	return (static_cast <CEikAppUi*> (new (ELeave) CHWRMLightTestFAppUi)); 
    }


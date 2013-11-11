// GuiConsDocument.cpp
// 
// Copyright (c) 2009 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#include "GuiConsAppUi.h"
#include "GuiConsDocument.h"

CGuiConsDocument* CGuiConsDocument::NewL(CEikApplication& aApp)
	{
	CGuiConsDocument* self = NewLC(aApp);
	CleanupStack::Pop(self);
	return self;
	}

CGuiConsDocument* CGuiConsDocument::NewLC(CEikApplication& aApp)
	{
	CGuiConsDocument* self = new(ELeave)CGuiConsDocument(aApp);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

void CGuiConsDocument::ConstructL()
	{
	}

CGuiConsDocument::CGuiConsDocument(CEikApplication& aApp)
	: CAknDocument(aApp)
	{
	}

CGuiConsDocument::~CGuiConsDocument()
	{
	}

CEikAppUi* CGuiConsDocument::CreateAppUiL()
	{
	return static_cast<CEikAppUi*>(new(ELeave)CGuiConsAppUi);
	}

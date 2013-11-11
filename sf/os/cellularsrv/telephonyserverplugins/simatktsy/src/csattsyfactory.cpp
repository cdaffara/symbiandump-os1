// Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "csattsyfactory.h"
#include "csattsy.h"

//EXPORT_C

 CSatTsyFactory* CSatTsyFactory::NewL()
	{
	CSatTsyFactory* self = new (ELeave) CSatTsyFactory;
	return self;	
	}

 EXPORT_C CSatTsyFactory* CSatTsyFactory::NewLC()
	{
	CSatTsyFactory* self = new (ELeave) CSatTsyFactory;
	CleanupStack::PushL(self);
	return self;	
	}

CSatTsyFactory::CSatTsyFactory()
	{
	}
	
CSatTsyFactory::~CSatTsyFactory()
	{

	}

CSatTsyFactory::TSatTsyPtrContainer CSatTsyFactory::CreateSatTsyL(MMessageRouter& aMmMessageRouter, MCtsySatService& aSatService)
	{
	//create STK CTSY - non owned as reference counted and will be destroyed after last client closed
	TSatTsyPtrContainer ptrcontainer;
	CSatTsy* sattsy = CSatTsy::NewL( aMmMessageRouter, aSatService);
	ptrcontainer.iSatTsy = sattsy;	
	ptrcontainer.iSatTsyMessageManagerCallback = sattsy->GetMessageManagerCallback();	
	return ptrcontainer;
	}


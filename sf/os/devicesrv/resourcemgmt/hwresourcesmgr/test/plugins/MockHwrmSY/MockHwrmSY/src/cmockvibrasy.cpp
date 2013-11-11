// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "cmockvibrasy.h"
#include "mockvibradata.h"
#include "cmockvibraengine.h"

/**
Factory function
*/
CMockVibraSY* CMockVibraSY::NewL()
	{
	CMockVibraSY* self =  new (ELeave)CMockVibraSY();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/**
Return mock server name
*/
const TDesC& CMockVibraSY::ServerName()
	{
	return MockVibraData::ServerName();
	}

/**
Create the engine
*/
CMockHwrmPluginEngine* CMockVibraSY::CreateEngine()
	{
	return CMockVibraEngine::NewL(*this);
	}

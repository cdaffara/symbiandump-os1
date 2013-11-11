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

#include "cmocksyserver.h"
#include "cmockpowerstatesy.h"
#include "cmockpowerstateengine.h"
#include "mockpowerstatedata.h"


/**
Factory function
*/
CMockPowerStateSY* CMockPowerStateSY::NewL()
	{
	CMockPowerStateSY* self =  new (ELeave)CMockPowerStateSY();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
	
/**
Constructor
*/	
CMockPowerStateSY::CMockPowerStateSY()
	{
	
	}

/**
2nd phase constructor
*/	
void CMockPowerStateSY::ConstructL()
	{
	iEngine = CMockPowerStateEngine::NewL(*this);
	iServer = CMockSYServer::NewL(MockPowerStateData::ServerName(), CActive::EPriorityHigh, *iEngine);
	}


/**
Destructor
*/	
CMockPowerStateSY::~CMockPowerStateSY()
	{
	delete iServer;
	delete iEngine;
	}


/**
Pass new powerstate value to the callback function
*/	
void CMockPowerStateSY::DoProcessCallback(TUint aKey, TInt aValue)
	{
	iNotificationCallback->NotifyStateChange(aKey,aValue);
	}

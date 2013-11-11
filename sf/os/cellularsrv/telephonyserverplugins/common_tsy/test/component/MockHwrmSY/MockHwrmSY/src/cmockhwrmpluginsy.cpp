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

#include "cmockhwrmpluginsy.h"
#include <cmocksyserver.h>
#include "cmockhwrmpluginengine.h"


/**
Constructor
*/	
CMockHwrmPluginSY::CMockHwrmPluginSY()
	{
	}

/**
2nd phase constructor
*/	
void CMockHwrmPluginSY::ConstructL()
	{
	iEngine = CreateEngine();
	iServer = CMockSYServer::NewL(ServerName(), CActive::EPriorityHigh, *iEngine);
	}

/**
Destructor
*/	
CMockHwrmPluginSY::~CMockHwrmPluginSY()
	{
	delete iServer;
	delete iEngine;
	}
		
/**
Method to invoke a particular command in the plugin
@param aCommandId Command ID
@param aTransId   Transaction ID
@param aData      Data associated with command
*/
void CMockHwrmPluginSY::ProcessCommandL(const TInt aCommandId, const TUint8 aTransId, TDesC8& aData)
	{
	TInt err =  iEngine->ExecuteProcessCommandL(aCommandId, aTransId, aData);
	User::LeaveIfError(err);
	}
	
/**
Method to cancel a particular command
@param aTransId   Transaction ID
@param aCommandId Command ID to optionally double check with the 
                  transaction ID
*/
void CMockHwrmPluginSY::CancelCommandL(const TUint8 aTransId, const TInt aCommandId)
	{
	TInt err =  iEngine->ExecuteCancelCommandL(aCommandId, aTransId);
	User::LeaveIfError(err);
	}


/**
Method called by the engine to send a response to a command
@param aTransId   Transaction ID
@param aCommandId Command ID to optionally double check with the 
                  transaction ID
*/
void CMockHwrmPluginSY::DoProcessResponse(const TInt aCommandId,const TUint8 aTransId, const TDesC8& aData)
	{
	TRAP_IGNORE(iResponseCallback->ProcessResponseL(aCommandId,aTransId,const_cast<TDesC8&>(aData)));
	}
	

// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include "cmockpowersy.h"
#include "mockpowerdata.h"
#include "cmockpowerengine.h"
#include <hwrm/HWRMPowerCommands.h>

/**
Factory function
*/

CMockPowerSY* CMockPowerSY::NewL()
	{
	CMockPowerSY* self =  new (ELeave)CMockPowerSY();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}


void CMockPowerSY::ConstructL()
	{	
	iEngine = CMockPowerEngine::NewL(*this);
	iServer = CMockSYServer::NewL(MockPowerData::ServerName(), CActive::EPriorityHigh, *iEngine);
	}

CMockPowerSY::CMockPowerSY()
	{
	
	}

CMockPowerSY::~CMockPowerSY()
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
void CMockPowerSY::ProcessCommandL(const TInt aCommandId, const TUint8 aTransId, TDesC8& aData)
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
void CMockPowerSY::CancelCommandL(const TUint8 aTransId, const TInt aCommandId)
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
void CMockPowerSY::DoProcessResponse(const TInt aCommandId,const TUint8 aTransId, const TDesC8& aData)
	{
	if (aCommandId == HWRMPowerCommand::EAverageBatteryPowerIndicationId
#ifdef SYMBIAN_HWRM_EXTPOWERINFO
		|| aCommandId == HWRMPowerCommand::EAverageBatteryChargingCurrentIndicationId 
		|| aCommandId == HWRMPowerCommand::EBatteryFullChargingTimeIndicationId
#endif //SYMBIAN_HWRM_EXTPOWERINFO
		)
		{
		// respond to the Power Common Data. Used for the measurement indications
		//iEventIndicationCallback->EventIndicationL(aCommandId,
		//										   const_cast<TDesC8&>(aData));
		
		TRAP_IGNORE(iResponseCallback->EventL(aCommandId, const_cast<TDesC8&>(aData)));
		}
	else
		{
		// respond to the plugin handler
		TRAP_IGNORE(iResponseCallback->ProcessResponseL(aCommandId,aTransId,const_cast<TDesC8&>(aData)));
		}
	
	}
	


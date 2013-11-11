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
// LBS BT GPS Positioner Class, which is used by the tests to make location 
// requests and get called back with location updates
//

#include <EPos_CPosModules.h>
#include <EPos_CPosModuleUpdate.h>
#include <EPos_CPosModuleIdList.h>

#include "ctlbsbtgpspositioner.h"

//Constants
_LIT(KtdBT1,"tdBT1");


CBtGpsPositioner* CBtGpsPositioner::NewL(MBtGpsPositionerObserver& aObserver)
	{
	CBtGpsPositioner* self = new (ELeave) CBtGpsPositioner(aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}


CBtGpsPositioner::~CBtGpsPositioner()
	{
	Cancel();
	iPositioner.Close();
	iPositionServer.Close();
	}


void CBtGpsPositioner::StartLocationRequest()
	{
	//Make the location requests
	iPositioner.NotifyPositionUpdate(iPosition, iStatus);
	SetActive();
	}


void CBtGpsPositioner::OpenL()
	{
	TPositionModuleId btGPSPsy = { 0x101FE999 };
	
	//Set up the Positioner
	User::LeaveIfError(iPositionServer.Connect());	
	User::LeaveIfError(iPositioner.Open(iPositionServer, btGPSPsy));
	User::LeaveIfError(iPositioner.SetRequestor(CRequestor::ERequestorService, CRequestor::EFormatApplication, KtdBT1));
	}
	
	
void CBtGpsPositioner::EnableL()
	{
	//Ensure the BT GPS PSY is enabled
	TPositionModuleId btGPSPsy = { 0x101FE999 };
	
	CPosModules* db = CPosModules::OpenL();
	CleanupStack::PushL(db);
	CPosModuleUpdate* moduleUpdate = CPosModuleUpdate::NewLC();

	moduleUpdate->SetUpdateAvailability(ETrue);
	
	db->UpdateModuleL(btGPSPsy, *moduleUpdate);
	
	CleanupStack::PopAndDestroy(moduleUpdate);
	CleanupStack::PopAndDestroy(db);
	}


RPositioner& CBtGpsPositioner::Positioner()
	{
	//Allow the tests direct access to the position
	return iPositioner;
	}


void CBtGpsPositioner::RunL()
	{
	if (iStatus.Int() == KErrNone)
		{
		TPosition pos;
		iPosition.GetPosition(pos);
		//Call the test to inform it of the arriving position update
		iObserver.OnPositionUpdate(pos);
		}
	else
		{
		//Call the test to inform it of the positioning error
		iObserver.OnPositionError(iStatus.Int());
		}
	}


void CBtGpsPositioner::DoCancel()
	{
	//Cancel the location request
	iPositioner.CancelRequest(EPositionerNotifyPositionUpdate);
	}


TInt CBtGpsPositioner::RunError(TInt aError)
	{
	return aError;
	}

	
CBtGpsPositioner::CBtGpsPositioner(MBtGpsPositionerObserver& aObserver)
	: CActive(CActive::EPriorityStandard),
	  iObserver(aObserver)
	{
	CActiveScheduler::Add(this);
	}


void CBtGpsPositioner::ConstructL()
	{

	}

// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @internalComponent
 @released
*/

#ifndef __NETLOCMANLOGIC_H__
#define __NETLOCMANLOGIC_H__

//****************************************************************
// Includes
//****************************************************************
//System
#include <e32base.h>

//LBS
#include <lbs.h>
#include "LbsInternalInterface.h"
#include "lbsrootapi.h"
#include "lbsprocesssupervisor.h"
#include "cquietus.h"

//Component
#include "lbsnetlocmanrequesthandler.h"
#include "lbsnetlocmanresponsehandler.h"


//****************************************************************
// Classes
//****************************************************************

/** This is the class implementing the main logic of the Network Location Manager
This class is responsible for:
	- Subscribing to position update request from the LocServer
	- Starting the requesthandler, which is handles the request 
	  (LocServer->NetworkLocationManager->NetworkGateway) 
	  and the responsehandler, which handles the response
	  (NetworkLocationManager->NetworkGateway->LocServer)
	- Keeping track of session id's for requests passed through
	- Listening for close down requests

@see MLbsProcessCloseDown
*/

//************************************************************************************************************
//	CManagerMainLogic
//************************************************************************************************************
class CNetLocManLogic :	public CBase, public MLbsProcessCloseDown
	{
public:
	static CNetLocManLogic* NewL();
	~CNetLocManLogic();
	
public:
	TVersion Version();
	void UpdateLocation(TInt aStatus, TPositionInfoBase& aPosition, const TTime& aTargetTime);
	void GetSessionId(TLbsNetSessionIdInt& aSessionId);
	void IncrementSessionId();
	
	//from MLbsCloseDownObserver
	void OnProcessCloseDown();
	void UpdateDeviceStatus(TPositionModuleStatus::TDeviceStatus aDeviceStatus);
private:
	CNetLocManLogic();
	void ConstructL();

private:
	CNetLocManRequestHandler*		iLocSvrLocationRequestHandler;
	CNetLocManResponseHandler*		iNetLocationResponseHandler;
	CLbsCloseDownRequestDetector*	iCloseDownRequestDetector;
	RLbsPositionUpdates 			iPositionUpdates;
	TLbsNetSessionIdInt				iCurrentId;
	RLbsModuleStatus				iModuleStatus;
	TPositionModuleStatus			iPreviousReportedModuleStatus;
	};


#endif //__NETLOCMANLOGIC_H__

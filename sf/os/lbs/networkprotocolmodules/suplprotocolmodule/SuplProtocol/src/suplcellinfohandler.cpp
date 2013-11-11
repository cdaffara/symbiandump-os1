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
// Object to monitor the ETel cell information containing
// timing advance needed by SUPL to perform enhanced cell-ID.
// 
//

/**
 @file
 @internalTechnology
 @deprecated 
*/


#include "suplnetworkinfohandler.h"
#include <e32base.h>
#include <etelmm.h>
#include "supldevloggermacros.h"

/**
*/
CSuplCellInfoHandler::CSuplCellInfoHandler(MSuplNetworkInfoObserver& aObserver, const RMobilePhone& aCellPhone) :
	CActive(EPriorityStandard),
	iObserver(aObserver),
	iCellPhone(aCellPhone),
	iCellInfoPckg(iCellInfo),
	iMonitoring(EFalse)
	{
	SUPLLOG(ELogP1, "CSuplCellInfoHandler::CSuplCellInfoHandler() Begin\n");
	CActiveScheduler::Add(this);
	SUPLLOG(ELogP1, "CSuplCellInfoHandler::CSuplCellInfoHandler() End\n");
	}

/**
*/	
CSuplCellInfoHandler::~CSuplCellInfoHandler()
	{
	SUPLLOG(ELogP1, "CSuplCellInfoHandler::~CSuplCellInfoHandler() Begin\n");
	Cancel();
	SUPLLOG(ELogP1, "CSuplCellInfoHandler::~CSuplCellInfoHandler() End\n");
	}

/**
*/	
CSuplCellInfoHandler* CSuplCellInfoHandler::NewL(MSuplNetworkInfoObserver& aObserver, const RMobilePhone& aCellPhone)
	{
	SUPLLOG(ELogP1, "CSuplCellInfoHandler::NewL() Begin\n");
	CSuplCellInfoHandler* self = new (ELeave) CSuplCellInfoHandler(aObserver, aCellPhone);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	SUPLLOG(ELogP1, "CSuplCellInfoHandler::NewL() End\n");
	return self;
	}

/**
*/
void CSuplCellInfoHandler::ConstructL()
	{
	SUPLLOG(ELogP1, "CSuplCellInfoHandler::ConstructL() Begin\n");
	SUPLLOG(ELogP1, "CSuplCellInfoHandler::ConstructL() End\n");
	}

/**
*/
void CSuplCellInfoHandler::StartGettingCellInfo()
{
	SUPLLOG(ELogP1, "CSuplCellInfoHandler::StartGettingCellInfo() Begin\n");
	// Get the cell info
	iCellPhone.GetCellInfo(iStatus, iCellInfoPckg);
	SetActive();
	SUPLLOG(ELogP1, "CSuplCellInfoHandler::StartGettingCellInfo() End\n");
}

/**
*/
void CSuplCellInfoHandler::RunL()
	{
	SUPLLOG(ELogP1, "CSuplCellInfoHandler::RunL() Begin\n");
	User::LeaveIfError(iStatus.Int());
	
	if (!iMonitoring)
		{
		iObserver.CellInfoResults(iCellInfo);
		MonitorCellInfoChange();
		iMonitoring = ETrue;
		}
	else					
		{
		iObserver.CellInfoResults(iCellInfo);
		MonitorCellInfoChange(); //Keep monitoring
		}
	SUPLLOG(ELogP1, "CSuplCellInfoHandler::RunL() End\n");
	}

/**
*/	
void CSuplCellInfoHandler::DoCancel()
	{
	SUPLLOG(ELogP1, "CSuplCellInfoHandler::DoCancel() Begin\n");
	//Check to see what stage we in are to find out what to cancel
	if(!iMonitoring)
		{
		iCellPhone.CancelAsyncRequest(EMobilePhoneGetCellInfo);
		}
	else
		{
		iCellPhone.CancelAsyncRequest(EMobilePhoneNotifyCellInfoChange);
		}
	SUPLLOG(ELogP1, "CSuplCellInfoHandler::DoCancel() End\n");
	}

/**
*/	
TInt CSuplCellInfoHandler::RunError(TInt /*aError*/)
	{
	SUPLLOG(ELogP1, "CSuplCellInfoHandler::DoCancel() Begin\n");
	SUPLLOG(ELogP1, "CSuplCellInfoHandler::DoCancel() End\n");
	return KErrNone;
	}


/** Asychronous request to obtain notifications
of change in the cell info data
*/
void CSuplCellInfoHandler::MonitorCellInfoChange()
	{
	SUPLLOG(ELogP1, "CSuplCellInfoHandler::MonitorCellInfoChange() Begin\n");
	iCellPhone.NotifyCellInfoChange(iStatus, iCellInfoPckg);
	SetActive();
	SUPLLOG(ELogP1, "CSuplCellInfoHandler::MonitorCellInfoChange() End\n");
	}


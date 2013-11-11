// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <lbs/LbsLocDataSourceGpsBase.h>

#include "LbsLasCommon.h"
#include "LbsLasRqstHandler.h"

//************************************************************************************************************
//   CLasRequestHandler
//************************************************************************************************************

CLasRequestHandler* CLasRequestHandler::NewL
	(
	TInt aModuleNum,
	CLbsLocationSourceGpsBase* aLasModule
	)
	{
	CLasRequestHandler* self = new(ELeave) CLasRequestHandler(aModuleNum, aLasModule);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}


CLasRequestHandler::CLasRequestHandler
	(
	TInt aModuleNum,
	CLbsLocationSourceGpsBase* aLasModule
	)
	:
	CActive(CActive::EPriorityStandard),
	iModuleNum(aModuleNum),
	iLocModule(aLasModule)
	{
	}

void CLasRequestHandler::ConstructL()
	{
	CActiveScheduler::Add(this);

	// Attach and subscribe to listen for requests
	TInt err = iProperty.Attach(KLasPropertyCat,ELasLocationRequestBus + iModuleNum, EOwnerThread);
	
   User::LeaveIfError(err);
   
   iProperty.Subscribe(iStatus);
   SetActive();
   
   TLasRequest lasRequest;
   TPckg<TLasRequest> lasRequestPckg(lasRequest);
   
   err = iProperty.Get(lasRequestPckg);
   
   if (! err && lasRequest.iRequest == TLasRequest::ELasRequestLocation)
   	{
   	HandleRequest(lasRequest);
   	}
 	}

CLasRequestHandler::~CLasRequestHandler()
	{
	Cancel();
	iProperty.Close();
	}
   
void CLasRequestHandler::DoCancel()
	{
	iProperty.Cancel();
	}

void CLasRequestHandler::HandleRequest(const TLasRequest& aRequest)
	{
	LASLOG("CLasRequestHandler::HandleRequest - Enter");

	if (aRequest.iVersion != 1)
		{
		LASLOG("CLasRequestHandler::HandleRequest - Exit (Invalid Version Number)");
		return;
		}
	
	TLbsLocRequestQuality quality;

	switch(aRequest.iRequest)
		{
	case TLasRequest::ELasRequestLocation:
		iLocModule->RequestLocationUpdate(aRequest.iTargetTime, quality);
		break;
	case TLasRequest::ELasRequestCancel:
		iLocModule->CancelLocationRequest();
		break;
	case TLasRequest::ELasRequestMode:
		iLocModule->AdvisePowerMode(aRequest.iMode);
		break;
	default:
		LASLOG("Handler Las Request: Unknown Request");
		break;
		}
	
	LASLOG("CLasRequestHandler::HandleRequest - Exit");
	}

void CLasRequestHandler::RunL()
   {
	LASLOG("CLasRequestHandler::RunL - Enter");
   TInt err;
   TLasRequest lasRequest;
   TPckg<TLasRequest> lasRequestPckg(lasRequest);
   
	err = iStatus.Int();
	
   iProperty.Subscribe(iStatus);

   if (err == KErrNone)
      {          
      err = iProperty.Get(lasRequestPckg);
      User::LeaveIfError(err);

		HandleRequest(lasRequest);    
      }

   SetActive();
   
   LASLOG("CLasRequestHandler::RunL - Exit");
   }

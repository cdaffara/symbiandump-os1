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

#include "LbsLocSourceGps.h"
#include "LbsLocSourceObserver.h"
#include "LbsLocManagerGps.h"
#include "LbsLasRqstHandler.h"

CLocationManagerGps* CLocationManagerGps::NewL
	(
	TInt aModuleNum
	)
	{
	CLocationManagerGps* self = new(ELeave)CLocationManagerGps(aModuleNum);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CLocationManagerGps::CLocationManagerGps
	(
	TInt aModuleNum
	)
	:
	iModuleNum(aModuleNum)
	{
	}
	
void CLocationManagerGps::ConstructL()
	{
   LASLOG("CLocationManagerGps::ConstructL - Enter");
   
	TInt err;
      
	// Create Property containing Position and Request Buses (keys)

   LASLOG("Defining Location Request Bus");

   err = RProperty::Define
								(
								KLasPropertyCat,
								ELasLocationRequestBus + iModuleNum,
								RProperty::EByteArray,
                        sizeof(TLasRequest)
                        );

   if (err != KErrAlreadyExists)
   	{
   	User::LeaveIfError(err);
   	}
   
   
   LASLOG("Defining Location Data Bus");

   err = RProperty::Define
								(
								KLasPropertyCat,
								ELasLocationDataBus + iModuleNum,
								RProperty::EByteArray,
                        sizeof(TPositionCourseInfo)
                        );

   if (err != KErrAlreadyExists)
   	{
   	User::LeaveIfError(err);
   	}
   
   LASLOG("Defining Status Bus");

   err = RProperty::Define
								(
								KLasPropertyCat,
								ELasModuleStatusBus + iModuleNum,
								RProperty::EByteArray,
                        sizeof(TPositionModuleStatusEvent)
                        );

   if (err != KErrAlreadyExists)
   	{
   	User::LeaveIfError(err);
   	}
	
	LASLOG("Data and Status Bus defined");

	iMonitor        = CLbsLocationSourceGpsObserver::NewL(iModuleNum);
	iLasModule      = CLbsLocationSourceGps::NewL(*iMonitor);
	iRequestHandler = CLasRequestHandler::NewL(iModuleNum, iLasModule);
	
   LASLOG("CLocationManagerGps::ConstructL - Exit");
	}

CLocationManagerGps::~CLocationManagerGps()
	{
	delete iMonitor;
	delete iLasModule;
	delete iRequestHandler;
	}


void CLocationManagerGps::NotifyServiceToClose(TRequestStatus* aStatus)
	{
	iMonitor->NotifyServiceToClose(aStatus);
	}


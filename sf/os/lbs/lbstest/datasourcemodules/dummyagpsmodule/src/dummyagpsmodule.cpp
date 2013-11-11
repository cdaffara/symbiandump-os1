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
// This is the cpp file which contains the
//

//! @file dummyapgsmodule.cpp

// User includes
#include "dummyagpsmodule.h"
#include "lbsdevloggermacros.h"

CDummyAGpsModule::CDummyAGpsModule(MLbsLocationSourceGpsObserver& aObserver)
	: CLbsLocationSourceGpsBase(aObserver)
	{
	}


CLbsLocationSourceGpsBase* CDummyAGpsModule::NewL(MLbsLocationSourceGpsObserver& aObserver)
	{
	CDummyAGpsModule* self = new(ELeave) CDummyAGpsModule(aObserver);

	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);

	return self;
	}

void CDummyAGpsModule::ConstructL()
	{
	LBSLOG(ELogP1, "CDummyAGpsModule::ConstructL()\n");

	CLbsLocationSourceGpsBase::ConstructL();
	}


CDummyAGpsModule::~CDummyAGpsModule()
	{
	LBSLOG(ELogP1, "CDummyAGpsModule::~CDummyAGpsModule()\n");
	}	


void CDummyAGpsModule::SetGpsOptions(const TLbsGpsOptions& /*aGpsOptions*/)
	{
	LBSLOG(ELogP1, "CDummyAGpsModule::SetGpsOptions()\n");
	}


void CDummyAGpsModule::RequestLocationUpdate(const TTime& /*aTargetTime*/, const TLbsLocRequestQuality& /*aQuality*/)
	{
	LBSLOG(ELogP1, "CDummyAGpsModule::RequestLocationUpdate()\n");
	}


void CDummyAGpsModule::CancelLocationRequest()
	{
	LBSLOG(ELogP1, "CDummyAGpsModule::CancelLocationRequest()\n");	
	}


void CDummyAGpsModule::AdvisePowerMode(CLbsLocationSourceGpsBase::TPowerMode aMode)
	{
	LBSLOG(ELogP1, "CDummyAGpsModule::AdvisePowerMode()\n");
	if (aMode == CLbsLocationSourceGpsBase::EPowerModeClose)
		{
		iLocSourceGpsObserver.Shutdown();
		}
	}


void CDummyAGpsModule::AssistanceDataEvent(TInt /*aError*/, TLbsAsistanceDataGroup /*aDataMask*/)
	{
	LBSLOG(ELogP1, "CDummyAGpsModule::AssistanceDataEvent()\n");
	}


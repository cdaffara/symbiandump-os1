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
#include "LbsLasCommon.h"
#include "LbsLasDataSource.h"


CLbsLocationSourceGps::CLbsLocationSourceGps(MLbsLocationSourceGpsObserver& aObserver)
	:
	CLbsLocationSourceGpsBase(aObserver)
	{
	}


EXPORT_C CLbsLocationSourceGpsBase* CLbsLocationSourceGps::NewL
	(
	MLbsLocationSourceGpsObserver& aObserver
	)
	{
	CLbsLocationSourceGps* self = new(ELeave) CLbsLocationSourceGps(aObserver);

	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);

	return self;
	}

   	
void CLbsLocationSourceGps::ConstructL()
	{
	CLbsLocationSourceGpsBase::ConstructL();
	
	iUpdateSource = CLasUpdateSource::NewL(this);
	
	iLocSourceGpsObserver.UpdateDeviceStatus(TPositionModuleStatus::EDeviceUnknown);
	iLocSourceGpsObserver.UpdateDataQualityStatus(TPositionModuleStatus::EDataQualityUnknown);
	}

CLbsLocationSourceGps::~CLbsLocationSourceGps()
	{
	delete iUpdateSource;
	}	
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


void CLbsLocationSourceGps::SetGpsOptions(const TLbsGpsOptions& /*aGpsOptions*/)
	{
	
	}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CLbsLocationSourceGps::RequestLocationUpdate
	(
	const TTime& aTargetTime,
	const TLbsLocRequestQuality& /*aQuality*/)
	{
	LASLOG("CLbsLocationSourceGps::RequestLocationUpdate - Enter");

	iUpdateSource->RequestLocationUpdate(aTargetTime);

	LASLOG("CLbsLocationSourceGps::RequestLocationUpdate - Exit");
	}
   	
void CLbsLocationSourceGps::CancelLocationRequest()
	{
	LASLOG("CLbsLocationSourceGps::CancelLocationRequest - Enter");
	LASLOG("CLbsLocationSourceGps::CancelLocationRequest - Exit");
	}

void CLbsLocationSourceGps::AdvisePowerMode(CLbsLocationSourceGpsBase::TPowerMode aMode)
	{
	LASLOG("CLbsLocationSourceGps::AdviseOperationMode - Enter");
	
	iUpdateSource->SetPowerMode(aMode);

	LASLOG("CLbsLocationSourceGps::AdviseOperationMode - Exit");
	}


void CLbsLocationSourceGps::AssistanceDataEvent(TInt /*aError*/, TLbsAsistanceDataGroup /*aDataMask*/)
	{
	}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CLbsLocationSourceGps::HandleLocationUpdate(const TPositionInfoBase& aPosInfo)
	{
	const TPositionInfoBase* posInfo = &aPosInfo;
	iLocSourceGpsObserver.UpdateLocation(KErrNone, &posInfo, 1, 0);
	}
	
void CLbsLocationSourceGps::HandleDeviceStatusUpdate(TPositionModuleStatus::TDeviceStatus aDeviceStatus)
	{
	iLocSourceGpsObserver.UpdateDeviceStatus(aDeviceStatus);
	}

void CLbsLocationSourceGps::HandleQualityStatusUpdate(TPositionModuleStatus::TDataQualityStatus aDataQuality)
	{
	iLocSourceGpsObserver.UpdateDataQualityStatus(aDataQuality);
	}

void CLbsLocationSourceGps::Shutdown()
	{
	iLocSourceGpsObserver.Shutdown();
	}


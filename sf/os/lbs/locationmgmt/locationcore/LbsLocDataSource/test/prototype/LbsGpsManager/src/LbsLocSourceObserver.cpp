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

#include <e32property.h>
#include "LbsLasCommon.h"
#include "LbsLocSourceObserver.h"

//************************************************************************************************************
//   CLbsLocationSourceGpsObserver
//************************************************************************************************************
CLbsLocationSourceGpsObserver* CLbsLocationSourceGpsObserver::NewL
	(
	TInt aModuleNum
	)
	{
	CLbsLocationSourceGpsObserver* self = new(ELeave)CLbsLocationSourceGpsObserver(aModuleNum);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CLbsLocationSourceGpsObserver::CLbsLocationSourceGpsObserver
	(
	TInt aModuleNum
	)
	:
	iModuleNum(aModuleNum),
	iDeviceStatus(TPositionModuleStatus::EDeviceUnknown),
	iDataQuality(TPositionModuleStatus::EDataQualityUnknown)
	{
	}
	
void CLbsLocationSourceGpsObserver::ConstructL()
	{
	LASLOG("CLbsLocationSourceGpsObserver::ConstructL - Enter");
   LASLOG("CLbsLocationSourceGpsObserver::ConstructL - Exit");
	}

CLbsLocationSourceGpsObserver::~CLbsLocationSourceGpsObserver()
	{
	}
//-----------------------------------------------------------------------------

TVersion CLbsLocationSourceGpsObserver::Version()
	{
   return TVersion(1,0,0);
	}


//-----------------------------------------------------------------------------
void CLbsLocationSourceGpsObserver::PublishLocation
	(
	const TPositionInfoBase& aPosition
	)
	{
	LASLOG("CLbsLocationSourceGpsObserver::PublishLocation - Enter");
	
	TPositionCourseInfo posInfo(static_cast<const TPositionCourseInfo&>(aPosition));

   TPckgC<TPositionCourseInfo> posInfPckg(posInfo);

   TInt err = RProperty::Set(KLasPropertyCat,ELasLocationDataBus + iModuleNum,posInfPckg);
   
   LASLOG("CLbsLocationSourceGpsObserver::PublishLocation - Exit");
	}

void CLbsLocationSourceGpsObserver::UpdateLocation
	(
	TInt /*aStatus*/,
	const TPositionInfoBase* aPosInfoArray[],
	TInt  /*aNumItems*/,
	const TTime& /*aTargetTime*/
	)
	{
	__ASSERT_DEBUG(aPosInfoArray != NULL,User::Panic(_L("GPSMan"), KErrArgument));

	const TPositionInfoBase* position = aPosInfoArray[0];
	PublishLocation(*position);
   }

//-----------------------------------------------------------------------------
void CLbsLocationSourceGpsObserver::PublishModuleStatus
	(
	TPositionModuleStatus::TDeviceStatus      aDeviceStatus,
	TPositionModuleStatus::TDataQualityStatus aDataQuality
	)
	{
	LASLOG("CLbsLocationSourceGpsObserver::PublishModuleStatus - Enter");
	
	TPositionModuleStatus      modStatus;
	TPositionModuleStatusEvent modEvent;
	
	modStatus.SetDeviceStatus(aDeviceStatus);
	modStatus.SetDataQualityStatus(aDataQuality);
	
	TPositionModuleStatusEvent::TModuleEvent ocurredEvents;
	
	ocurredEvents  = aDeviceStatus == iDeviceStatus ? 0 : TPositionModuleStatusEvent::EEventDeviceStatus;
	ocurredEvents |= aDataQuality  != iDataQuality  ? 0 : TPositionModuleStatusEvent::EEventDataQualityStatus;
	
	iDeviceStatus = aDeviceStatus;
	iDataQuality  = aDataQuality;
	
	TPositionModuleId modId = {iModuleNum};
	
	modEvent.SetModuleId(modId);
	modEvent.SetModuleStatus(modStatus);
	modEvent.SetOccurredEvents(ocurredEvents);
	modEvent.SetRequestedEvents(TPositionModuleStatusEvent::EEventNone);
	modEvent.SetSystemModuleEvent(TPositionModuleStatusEvent::ESystemUnknown);
	
	TPckg<TPositionModuleStatusEvent> eventPckg(modEvent);
	
	RProperty::Set(KLasPropertyCat,ELasModuleStatusBus + iModuleNum, eventPckg);
	
	LASLOG("CLbsLocationSourceGpsObserver::PublishModuleStatus - Exit");
	}

void CLbsLocationSourceGpsObserver::UpdateDeviceStatus
	(
	TPositionModuleStatus::TDeviceStatus aDeviceStatus
	)
	{
	LASLOG("CLbsLocationSourceGpsObserver::UpdateDeviceStatus - Enter");
	
	if (aDeviceStatus != iDeviceStatus)
		{
		PublishModuleStatus(aDeviceStatus, iDataQuality);
		iDeviceStatus = aDeviceStatus;
		}

	LASLOG("CLbsLocationSourceGpsObserver::UpdateDeviceStatus - Exit");
	}
	   	
void CLbsLocationSourceGpsObserver::UpdateDataQualityStatus
	(
	TPositionModuleStatus::TDataQualityStatus aDataQuality
	)
	{
	LASLOG("CLbsLocationSourceGpsObserver::UpdateDataQualityStatus - Enter");

	if (aDataQuality != iDataQuality)
		{
		PublishModuleStatus(iDeviceStatus, aDataQuality);
		iDataQuality = aDataQuality;
		}
		
	LASLOG("CLbsLocationSourceGpsObserver::UpdateDataQualityStatus - Exit");
	}


void CLbsLocationSourceGpsObserver::Shutdown()
	{
	LASLOG("CLbsLocationSourceGpsObserver::Shutdown - Enter");
	
	__ASSERT_ALWAYS(iServiceCloseStatus,User::Panic(_L("GPSMan"), KErrArgument));
	
	User::RequestComplete(iServiceCloseStatus, KErrNone);
	
	LASLOG("CLbsLocationSourceGpsObserver::Shutdown - Exit");
	}

void CLbsLocationSourceGpsObserver::NotifyServiceToClose(TRequestStatus* aStatus)
	{
	iServiceCloseStatus = aStatus;
	*iServiceCloseStatus = KRequestPending;
	}


TInt CLbsLocationSourceGpsObserver::GetAssistanceDataItem
	(
	TLbsAssistanceDataItem /*aItem*/,
	RDataReaderRootBase& /*aDataRoot*/,
	TTime& /*aTimeStamp*/
	)
	{
	return KErrNotSupported;
	}

void CLbsLocationSourceGpsObserver::RequestAssistanceData
	(
	TLbsAsistanceDataGroup /*aDataItemMask*/
	)
	{
	}

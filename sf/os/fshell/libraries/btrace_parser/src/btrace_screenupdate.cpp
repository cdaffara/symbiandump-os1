// btrace_screenupdate.cpp
// 
// Copyright (c) 2008 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#include "btrace_parser.h"


EXPORT_C CBtraceScreenUpdate* CBtraceScreenUpdate::NewL(CBtraceReader& aReader)
	{
	CBtraceScreenUpdate* self = new(ELeave) CBtraceScreenUpdate(aReader);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CBtraceScreenUpdate::~CBtraceScreenUpdate()
	{
	iNotifs.Close();
	iReader.RemoveObserver(KAmTraceCategory, *this);
	}

EXPORT_C void CBtraceScreenUpdate::NotifyScreenUpdateL(MBtraceScreenUpdateObserver& aObserver)
	{
	NotifyScreenUpdateL(aObserver, ENotificationOneShot);
	}
	
EXPORT_C void CBtraceScreenUpdate::NotifyScreenUpdateL(MBtraceScreenUpdateObserver& aObserver, TBtraceNotificationPersistence aPersistence)
	{
	iNotifs.AppendL(TScreenUpdateNotif(aObserver, aPersistence));
	}

EXPORT_C void CBtraceScreenUpdate::NotifyScreenUpdateL(TInt aScreenNumber, const TRect& aRect, MBtraceScreenUpdateObserver& aObserver)
	{
	NotifyScreenUpdateL(aScreenNumber, aRect, aObserver, ENotificationOneShot);
	}
	
EXPORT_C void CBtraceScreenUpdate::NotifyScreenUpdateL(TInt aScreenNumber, const TRect& aRect, MBtraceScreenUpdateObserver& aObserver, TBtraceNotificationPersistence aPersistence)
	{
	iNotifs.AppendL(TScreenUpdateNotif(aScreenNumber, aRect, aObserver, aPersistence));
	}
	
EXPORT_C void CBtraceScreenUpdate::CancelNotifyScreenUpdate(MBtraceScreenUpdateObserver& aObserver)
	{
	for (TInt i = iNotifs.Count()-1; i>=0; --i)
		{
		if (iNotifs[i].iObserver == &aObserver)
			{
			iNotifs.Remove(i);
			}
		}
	}

CBtraceScreenUpdate::CBtraceScreenUpdate(CBtraceReader& aReader)
	: iReader(aReader)
	{
	}

void CBtraceScreenUpdate::ConstructL()
	{
	iReader.AddObserverL(KAmTraceCategory, *this);
	}

void CBtraceScreenUpdate::HandleScreenUpdateL(const TBtraceTickCount& aTickCount, TInt aScreenNumber, const TRect& aRect)
	{
	for (TInt i = (iNotifs.Count() - 1); i >= 0; --i)
		{
		TScreenUpdateNotif& notif = iNotifs[i];
		if (notif.iNotifyAll || ((notif.iScreenNumber == aScreenNumber) && (notif.iRect.Intersects(aRect))))
			{
			MBtraceScreenUpdateObserver* observer = notif.iObserver;
			if (notif.iPersistence == ENotificationOneShot)
				{
				iNotifs.Remove(i);
				}
			observer->HandleScreenUpdateL(aTickCount, aScreenNumber, aRect);
			}
		}
	}

void CBtraceScreenUpdate::HandleBtraceFrameL(const TBtraceFrame& aFrame)
	{
	if (aFrame.iCategory == KAmTraceCategory && aFrame.iSubCategory == EAmTraceSubCategoryScreenDriver)
		{
		const TUint8* data = aFrame.iData.Ptr();
		TAmTraceEventScreenDriver event = *((const TAmTraceEventScreenDriver*)data);
		data += sizeof (TAmTraceEventScreenDriver);
		if (event == EAmTraceEventScreenDriverRectangleUpdated)
			{
			TInt screenNumber = *((const TInt32*)data);
			data += sizeof (TInt);
			const TRect* rect = (const TRect*)data;
			HandleScreenUpdateL(aFrame.iTickCount, screenNumber, *rect);
			}
		}
	}

CBtraceScreenUpdate::TScreenUpdateNotif::TScreenUpdateNotif(MBtraceScreenUpdateObserver& aObserver, TBtraceNotificationPersistence aPersistence)
	: iNotifyAll(ETrue), iObserver(&aObserver), iPersistence(aPersistence)
	{
	}

CBtraceScreenUpdate::TScreenUpdateNotif::TScreenUpdateNotif(TInt aScreenNumber, const TRect& aRect, MBtraceScreenUpdateObserver& aObserver, TBtraceNotificationPersistence aPersistence)
	: iNotifyAll(EFalse), iScreenNumber(aScreenNumber), iRect(aRect), iObserver(&aObserver), iPersistence(aPersistence)
	{
	}


// btrace_domainevent.cpp
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

EXPORT_C CBtraceDomainEvent* CBtraceDomainEvent::NewL(CBtraceReader& aReader)
	{
	CBtraceDomainEvent* self = new (ELeave) CBtraceDomainEvent(aReader);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CBtraceDomainEvent::CBtraceDomainEvent(CBtraceReader& aReader)
	: iReader(aReader)
	{
	}

EXPORT_C CBtraceDomainEvent::~CBtraceDomainEvent()
	{
	iReader.RemoveObserver(KAmTraceCategory, *this);
	iNotifs.Close();
	}

void CBtraceDomainEvent::ConstructL()
	{
	iReader.AddObserverL(KAmTraceCategory, *this);
	}

EXPORT_C void CBtraceDomainEvent::NotifyDomainEventL(TUint aSubCategory, MBtraceDomainEventObserver& aObserver, TBtraceNotificationPersistence aPersistence)
	{
	TDomainEventNotif notif(aSubCategory, aObserver, aPersistence);
	iNotifs.AppendL(notif);
	}

EXPORT_C void CBtraceDomainEvent::NotifyDomainEventL(TUint aSubCategory, TUint aData1, MBtraceDomainEventObserver& aObserver, TBtraceNotificationPersistence aPersistence)
	{
	TDomainEventNotif notif(aSubCategory, aObserver, aPersistence);
	notif.AddDataItem(aData1);
	iNotifs.AppendL(notif);
	}

EXPORT_C void CBtraceDomainEvent::NotifyDomainEventL(TUint aSubCategory, TUint aData1, TUint aData2, MBtraceDomainEventObserver& aObserver, TBtraceNotificationPersistence aPersistence)
	{
	TDomainEventNotif notif(aSubCategory, aObserver, aPersistence);
	notif.AddDataItem(aData1);
	notif.AddDataItem(aData2);
	iNotifs.AppendL(notif);
	}

EXPORT_C void CBtraceDomainEvent::NotifyDomainEventL(TUint aSubCategory, TUint aData1, TUint aData2, TUint aData3, MBtraceDomainEventObserver& aObserver, TBtraceNotificationPersistence aPersistence)
	{
	TDomainEventNotif notif(aSubCategory, aObserver, aPersistence);
	notif.AddDataItem(aData1);
	notif.AddDataItem(aData2);
	notif.AddDataItem(aData3);
	iNotifs.AppendL(notif);
	}

EXPORT_C void CBtraceDomainEvent::CancelNotifyDomainEvent(MBtraceDomainEventObserver& aObserver)
	{
	for (TInt i = (iNotifs.Count() - 1); i >= 0; --i)
		{
		const TDomainEventNotif& thisNotif = iNotifs[i];
		if (thisNotif.iObserver == &aObserver)
			{
			iNotifs.Remove(i);
			}
		}
	}

void CBtraceDomainEvent::HandleBtraceFrameL(const TBtraceFrame& aFrame)
	{
	if (aFrame.iCategory == KAmTraceCategory && aFrame.iSubCategory > EAmTraceSubCategoryDomainEventBase)
		{
		const TInt numDataItems = (aFrame.iData.Length() / sizeof(TUint));
		if (numDataItems <= TDomainEventNotif::KMaxNumDataItems)
			{
			const TUint* data = reinterpret_cast<const TUint*>(aFrame.iData.Ptr());
			TDomainEventNotif notif(aFrame.iSubCategory);
			for (TInt i = 0; i < numDataItems; ++i)
				{
				notif.AddDataItem(*(data + i));
				}

			for (TInt i = (iNotifs.Count() - 1); i >= 0; --i)
				{
				const TDomainEventNotif& thisNotif = iNotifs[i];
				if (thisNotif == notif)
					{
					MBtraceDomainEventObserver* observer = thisNotif.iObserver;
					if (thisNotif.iPersistence == ENotificationOneShot)
						{
						iNotifs.Remove(i);
						}
					observer->HandleDomainEventL(aFrame.iTickCount, *observer);
					}
				}
			}
		}
	}

CBtraceDomainEvent::TDomainEventNotif::TDomainEventNotif(TUint aSubCategory)
	: iSubCategory(aSubCategory), iObserver(NULL), iPersistence(ENotificationOneShot), iDataCount(0)
	{
	}

CBtraceDomainEvent::TDomainEventNotif::TDomainEventNotif(TUint aSubCategory, MBtraceDomainEventObserver& aObserver, TBtraceNotificationPersistence aPersistence)
	: iSubCategory(aSubCategory), iObserver(&aObserver), iPersistence(aPersistence), iDataCount(0)
	{
	}

void CBtraceDomainEvent::TDomainEventNotif::AddDataItem(TUint aData)
	{
	__ASSERT_ALWAYS(iDataCount < KMaxNumDataItems, Panic(EBtpPanicTooManyDomainEventDataItems));
	iData[iDataCount++] = aData;
	}

TBool CBtraceDomainEvent::TDomainEventNotif::operator==(const TDomainEventNotif& aNotif) const
	{
	if ((aNotif.iSubCategory == iSubCategory) && (aNotif.iDataCount == iDataCount))
		{
		for (TInt i = 0; i < iDataCount; ++i)
			{
			if (aNotif.iData[i] != iData[i])
				{
				return EFalse;
				}
			}
		return ETrue;
		}
	else
		{
		return EFalse;
		}
	}


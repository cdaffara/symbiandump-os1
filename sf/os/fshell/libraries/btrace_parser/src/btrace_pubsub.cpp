// btrace_pubsub.cpp
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

EXPORT_C CBtracePubSub* CBtracePubSub::NewL(CBtraceReader& aReader)
	{
	CBtracePubSub* self = new(ELeave) CBtracePubSub(aReader);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CBtracePubSub::CBtracePubSub(CBtraceReader& aReader)
	: iReader(aReader)
	{
	}

EXPORT_C CBtracePubSub::~CBtracePubSub()
	{
	iNotifs.Close();
	iReader.RemoveObserver(ExtraBTrace::EPubSub, *this);
	}

void CBtracePubSub::ConstructL()
	{
	iReader.AddObserverL(ExtraBTrace::EPubSub, *this);
	}

EXPORT_C void CBtracePubSub::NotifyPropertyChangedL(MBtracePubSubObserver& aObserver)
	{
	iNotifs.AppendL(TPubSubNotif(NULL, NULL, NULL, NULL, aObserver, ENotificationOneShot));
	}

EXPORT_C void CBtracePubSub::NotifyPropertyChangedL(TUint* aCategory, TUint* aKey, MBtracePubSubObserver& aObserver, TBtraceNotificationPersistence aPersistence)
	{
	iNotifs.AppendL(TPubSubNotif(aCategory, aKey, NULL, NULL, aObserver, aPersistence));
	}

EXPORT_C void CBtracePubSub::NotifyIntegerPropertyChangedL(TUint* aCategory, TUint* aKey, TInt* aValue, MBtracePubSubObserver& aObserver, TBtraceNotificationPersistence aPersistence)
	{
	iNotifs.AppendL(TPubSubNotif(aCategory, aKey, aValue, NULL, aObserver, aPersistence));
	}

EXPORT_C void CBtracePubSub::NotifyDataPropertyChangedL(TUint* aCategory, TUint* aKey, const TDesC8* aValue, MBtracePubSubObserver& aObserver, TBtraceNotificationPersistence aPersistence)
	{
	iNotifs.AppendL(TPubSubNotif(aCategory, aKey, NULL, aValue, aObserver, aPersistence));
	}

EXPORT_C void CBtracePubSub::CancelNotifyPropertyChanged(MBtracePubSubObserver& aObserver)
	{
	for (TInt i = (iNotifs.Count() - 1); i >= 0; --i)
		{
		if (&iNotifs[i].iObserver == &aObserver)
			{
			iNotifs.Remove(i);
			}
		}
	}

void CBtracePubSub::HandleBtraceFrameL(const TBtraceFrame& aFrame)
	{
	if (aFrame.iCategory != ExtraBTrace::EPubSub) return;

	const TUint8* data = aFrame.iData.Ptr();
	TUint cat = *(TUint32*)data;
	TUint key = *((TUint32*)data + 1);

	switch (aFrame.iSubCategory)
		{
		case ExtraBTrace::EPubSubIntPropertyChanged:
			{
			TInt val = *((TUint32*)data + 2);
			for (TInt i = 0; i < iNotifs.Count(); i++)
				{
				if (iNotifs[i].Matches(cat, key, val))
					{
					iNotifs[i].iObserver.HandlePropertyChangedL(aFrame.iTickCount, cat, key, val);
					}
				}
			break;
			}
		case ExtraBTrace::EPubSubDataPropertyChanged:
			{
			TPtrC8 val = aFrame.iData.Mid(8);
			for (TInt i = 0; i < iNotifs.Count(); i++)
				{
				if (iNotifs[i].Matches(cat, key, val))
					{
					iNotifs[i].iObserver.HandlePropertyChangedL(aFrame.iTickCount, cat, key, val);
					}
				}
			break;
			}
		}
	}

CBtracePubSub::TPubSubNotif::TPubSubNotif(TUint* aCategory, TUint* aKey, TInt* aIntegerValue, const TDesC8* aDataValue, MBtracePubSubObserver& aObserver, TBtraceNotificationPersistence aPersistence)
	: iCategory(aCategory), iKey(aKey), iIntegerValue(aIntegerValue), iDataValue(aDataValue), iObserver(aObserver), iPersistence(aPersistence)
	{
	__ASSERT_ALWAYS(!(iIntegerValue && iDataValue), Panic(EBtpPanicIncompatiblePubSubArgs));
	}

TBool CBtracePubSub::TPubSubNotif::Matches(TUint aCategory, TUint aKey, TInt aIntegerValue) const
	{
	if ((iCategory == NULL) || (*iCategory == aCategory))
		{
		if ((iKey == NULL) || (*iKey == aKey))
			{
			if (((iIntegerValue == NULL) && (iDataValue == NULL)) || (*iIntegerValue == aIntegerValue))
				{
				return ETrue;
				}
			}
		}
	return EFalse;
	}

TBool CBtracePubSub::TPubSubNotif::Matches(TUint aCategory, TUint aKey, const TDesC8& aDataValue) const
	{
	if ((iCategory == NULL) || (*iCategory == aCategory))
		{
		if ((iKey == NULL) || (*iKey == aKey))
			{
			if (((iIntegerValue == NULL) && (iDataValue == NULL)) || (*iDataValue == aDataValue))
				{
				return ETrue;
				}
			}
		}
	return EFalse;
	}

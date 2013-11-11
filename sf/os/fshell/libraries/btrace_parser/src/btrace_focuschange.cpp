// btrace_focuschange.cpp
// 
// Copyright (c) 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#include "btrace_parser.h"

EXPORT_C CBtraceFocusChange* CBtraceFocusChange::NewL(CBtraceReader& aReader, CBtraceContext& aContext)
	{
	CBtraceFocusChange* self = new(ELeave) CBtraceFocusChange(aReader, aContext);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CBtraceFocusChange::CBtraceFocusChange(CBtraceReader& aReader, CBtraceContext& aContext)
	: iReader(aReader), iContext(aContext)
	{
	}

EXPORT_C CBtraceFocusChange::~CBtraceFocusChange()
	{
	iNotifs.Close();
	iReader.RemoveObserver(KAmTraceCategory, *this);
	}

void CBtraceFocusChange::ConstructL()
	{
	iReader.AddObserverL(KAmTraceCategory, *this);
	}

EXPORT_C void CBtraceFocusChange::NotifyFocusChangedL(const TDesC& aWindowGroupNamePattern, MBtraceFocusChangeObserver& aObserver)
	{
	NotifyFocusChangedL(aWindowGroupNamePattern, aObserver, ENotificationOneShot);
	}

EXPORT_C void CBtraceFocusChange::NotifyFocusChangedL(const TDesC& aWindowGroupNamePattern, MBtraceFocusChangeObserver& aObserver, TBtraceNotificationPersistence aPersistence)
	{
	iNotifs.AppendL(TFocusChangeNotif(aWindowGroupNamePattern, aObserver, aPersistence));
	}

EXPORT_C void CBtraceFocusChange::CancelNotifyFocusChanged(MBtraceFocusChangeObserver& aObserver)
	{
	for (TInt i = (iNotifs.Count() - 1); i >= 0; --i)
		{
		if (&iNotifs[i].iObserver == &aObserver)
			{
			iNotifs.Remove(i);
			}
		}
	}

void CBtraceFocusChange::HandleBtraceFrameL(const TBtraceFrame& aFrame)
	{
	if (aFrame.iSubCategory == EAmTraceSubCategoryWindowGroupFocus)
		{
		const TInt32* data = (const TInt32*)aFrame.iData.Ptr();
		TInt wservWindowGroupId = *data;
		const TBtraceWindowGroupId* windowGroupId = iContext.FindWindowGroup(wservWindowGroupId);
		__ASSERT_ALWAYS(windowGroupId, Panic(EBtpPanicWservWindowGroupIdNotFound));
		const TDesC& windowGroupName = iContext.WindowGroupName(*windowGroupId);
		
		for (TInt i = (iNotifs.Count() - 1); i >= 0; --i)
			{
			const TFocusChangeNotif& thisNotif = iNotifs[i];
			if (windowGroupName.MatchF(thisNotif.iWindowGroupNamePattern) != KErrNotFound)
				{
				MBtraceFocusChangeObserver& observer = thisNotif.iObserver;
				if (thisNotif.iPersistence == ENotificationOneShot)
					{
					iNotifs.Remove(i);
					}
				observer.HandleFocusChangedL(aFrame.iTickCount, *windowGroupId);
				}
			}
		}
	}

CBtraceFocusChange::TFocusChangeNotif::TFocusChangeNotif(const TDesC& aWindowGroupNamePattern, MBtraceFocusChangeObserver& aObserver, TBtraceNotificationPersistence aPersistence)
	: iWindowGroupNamePattern(aWindowGroupNamePattern), iObserver(aObserver), iPersistence(aPersistence)
	{
	}

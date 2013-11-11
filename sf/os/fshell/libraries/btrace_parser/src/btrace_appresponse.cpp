// btrace_appresponse.cpp
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
#include <fshell/common.mmh>


//
// CBtraceAppResponse
//

EXPORT_C CBtraceAppResponse* CBtraceAppResponse::NewL(CBtraceReader& aReader)
	{
	CBtraceAppResponse* self = new (ELeave) CBtraceAppResponse(aReader);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CBtraceAppResponse::CBtraceAppResponse(CBtraceReader& aReader)
	: iReader(aReader)
	{
	}

EXPORT_C CBtraceAppResponse::~CBtraceAppResponse()
	{
	iReader.RemoveObserver(KAmTraceCategory, *this);
	iNotifs.Close();
	}

void CBtraceAppResponse::ConstructL()
	{
	iReader.AddObserverL(KAmTraceCategory, *this);
	}

void CBtraceAppResponse::HandleBtraceFrameL(const TBtraceFrame& aFrame)
	{
	if (aFrame.iCategory != KAmTraceCategory) return;

	switch (aFrame.iSubCategory)
		{
		case EAmTraceSubCategoryEvCapture:
			{
			const TUint32* data = reinterpret_cast<const TUint32*>(aFrame.iData.Ptr());
			const TAmTraceEventEvCapture event = static_cast<TAmTraceEventEvCapture>(*data++);
			const TUint32 processIdLowWord = *data++;
			const TUint32 processIdHighWord = *data++;

			switch (event)
				{
				case EAmTraceEventEvCaptureSystemAppResponse:
					{
					SeenAppResponseL(aFrame.iTickCount, MAKE_TUINT64(processIdHighWord, processIdLowWord));
					}
				break;
				
				default:
					// ignore the event
				break;
				};
			}
		break;

		default:
			// ignore anything we don't know about.
		break;
		};
	}

void CBtraceAppResponse::SeenAppResponseL(const TBtraceTickCount& aTickCount, const TProcessId& aProcessId)
	{
	TInt ii = iNotifs.Count();
	while (--ii >= 0)
		{
		TAppResponseNotif& nt = iNotifs[ii];
		if (nt.iProcessId == aProcessId)
			{
			MBtraceAppResponseObserver& observer = nt.iObserver;
			if (nt.iPersistence == ENotificationOneShot)
				{
				iNotifs.Remove(ii);
				}
			observer.HandleAppResponseSeenL(aTickCount);
			}
		}	
	}

EXPORT_C void CBtraceAppResponse::NotifyAppResponseL(MBtraceAppResponseObserver& aObserver, const TProcessId& aProcessId)
	{
	NotifyAppResponseL(aObserver, aProcessId, ENotificationOneShot);
	}

EXPORT_C void CBtraceAppResponse::NotifyAppResponseL(MBtraceAppResponseObserver& aObserver, const TProcessId& aProcessId, TBtraceNotificationPersistence aPersistence)
	{
	TAppResponseNotif notify(aObserver, aProcessId, aPersistence);
	User::LeaveIfError(iNotifs.Append(notify));
	}

EXPORT_C void CBtraceAppResponse::CancelNotifyAppResponse(MBtraceAppResponseObserver& aObserver)
	{
	for (TInt i = (iNotifs.Count() - 1); i >= 0; --i)
		{
		if (&iNotifs[i].iObserver == &aObserver)
			{
			iNotifs.Remove(i);
			}
		}
	}


//
// CBtraceAppResponse::TAppResponseNotif
//

CBtraceAppResponse::TAppResponseNotif::TAppResponseNotif(MBtraceAppResponseObserver& aObserver, const TProcessId& aProcessId, TBtraceNotificationPersistence aPersistence)
	: iObserver(aObserver), iProcessId(aProcessId), iPersistence(aPersistence)
	{
	}

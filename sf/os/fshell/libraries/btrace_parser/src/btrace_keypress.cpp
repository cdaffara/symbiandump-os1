// btrace_keypress.cpp
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

#include <fshell/ltkutils.h>
#include "btrace_parser.h"

EXPORT_C CBtraceKeyPress* CBtraceKeyPress::NewL(CBtraceReader& aReader)
	{
	CBtraceKeyPress* self = new (ELeave) CBtraceKeyPress(aReader);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CBtraceKeyPress::CBtraceKeyPress(CBtraceReader& aReader): 
iReader(aReader)
	{
	}

EXPORT_C CBtraceKeyPress::~CBtraceKeyPress()
	{
	iReader.RemoveObserver(KAmTraceCategory, *this);
	iKeyNotifs.Close();
	iPointerNotifs.Close();
	iGenericNotifs.Close();
	iRawKeyNotifs.Close();
	}

void CBtraceKeyPress::ConstructL()
	{
	iReader.AddObserverL(KAmTraceCategory, *this);
	
	LtkUtils::W32CrackL(); // In case no-one's called it yet
	}

//
// CBtraceKeyPress::HandleBtraceFrameL
// process the btrace frame, stripping out the key event data
//
void CBtraceKeyPress::HandleBtraceFrameL(const TBtraceFrame& aFrame)
	{
	if(aFrame.iCategory == KAmTraceCategory && aFrame.iSubCategory == EAmTraceSubCategoryEvCapture)
		{
		const TUint32* data = reinterpret_cast<const TUint32*>(aFrame.iData.Ptr());
		const TAmTraceEventEvCapture event = static_cast<TAmTraceEventEvCapture>(*data++);
			
		switch(event)
			{
			case EAmTraceEventEvCaptureKeyPress:
				{
				data += 1; // step over superfluous bytes
				TKeyEvent* key = (TKeyEvent*) data;
				SeenKeyL(aFrame.iTickCount, *key);
				}
			break;
	
			case EAmTraceEventEvCapturePointer:
				{
				data += 1; // step over superfluous bytes
				TPointerEvent* pointer = (TPointerEvent*) data;
				SeenPointerL(aFrame.iTickCount, *pointer);			
				}
			break;
	
			case EAmTraceEventEvCaptureUnclassified:
				{
				TInt commandClass = *data++;
				TInt opcode = *data++;
				SeenUnclassifiedL(aFrame.iTickCount, commandClass, opcode, NULL);
				}
				break;
			
			case EAmTraceEventEvCaptureRawScan:
				{
				TBool isKeyUp = *data++;
				TInt scanCode = *data++;
				SeenRawKeyL(aFrame.iTickCount, isKeyUp, scanCode);
				}
			break;

			default:
				// Ignore anything we don't know about.
			break;
			}
		};
	}

//
// CBtraceKeyPress::SeenKeyL
// inform any observers that a key event has occured 
// & pass a reference back to the key event
//
void CBtraceKeyPress::SeenKeyL(const TBtraceTickCount& aTickCount, const TKeyEvent& aKey)
	{
	TInt ii = iKeyNotifs.Count();
	while (--ii >= 0)
		{
		MBtraceKeyPressObserver* observer = iKeyNotifs[ii].iObserver;
		if (iKeyNotifs[ii].iPersistence == ENotificationOneShot)
			{
			iKeyNotifs.Remove(ii);
			}
		observer->HandleKeyPressL(aTickCount, aKey);
		}	
	}

void CBtraceKeyPress::SeenRawKeyL(const TBtraceTickCount& aTickCount, TBool aKeyUp, TInt aScanCode)
	{
	TInt ii = iRawKeyNotifs.Count();
	while (--ii >= 0)
		{
		MBtraceKeyPressObserver* observer = iRawKeyNotifs[ii].iObserver;
		if (iRawKeyNotifs[ii].iPersistence == ENotificationOneShot)
			{
			iRawKeyNotifs.Remove(ii);
			}
		observer->HandleRawKeyL(aTickCount, aKeyUp, aScanCode);
		}
	}

//
// CBtraceKeyPress::SeenPointerL
// inform any observers that a key event has occured 
// & pass a reference back to the key event
//
void CBtraceKeyPress::SeenPointerL(const TBtraceTickCount& aTickCount, const TPointerEvent& aPointer)
	{
	TInt ii = iPointerNotifs.Count();
	while (--ii >= 0)
		{
		MBtraceKeyPressObserver* observer = iPointerNotifs[ii].iObserver;
		if (iPointerNotifs[ii].iPersistence == ENotificationOneShot)
			{
			iPointerNotifs.Remove(ii);
			}
		observer->HandlePointerPressL(aTickCount, aPointer);
		}	
	}

//
// CBtraceKeyPress::SeenUnclassifiedL
// inform any observers that unknown/unclassified data 
// has been received for a specific opcode
void CBtraceKeyPress::SeenUnclassifiedL(const TBtraceTickCount& aTickCount, const TInt aCommandClass, const TInt aOpcode, const TAny* aData)
	{
	TInt ii = iGenericNotifs.Count();
	while (--ii >= 0)
		{
		MBtraceKeyPressObserver* observer = iGenericNotifs[ii].iObserver;
		if (iGenericNotifs[ii].iPersistence == ENotificationOneShot)
			{
			iGenericNotifs.Remove(ii);
			}
		observer->HandleUnclassifiedDataL(aTickCount, aCommandClass, aOpcode, aData);
		}	
	}

//
// CBtraceKeyPress::NotifyKeyEvent
// called by the client to register an 
// interest in any key events captured 
//
EXPORT_C void CBtraceKeyPress::NotifyKeyEventL(MBtraceKeyPressObserver& aObserver)
	{
	NotifyKeyEventL(aObserver, ENotificationOneShot);
	}
	
EXPORT_C void CBtraceKeyPress::NotifyKeyEventL(MBtraceKeyPressObserver& aObserver, TBtraceNotificationPersistence aPersistence)
	{
	TKeyPressNotif notify(aObserver, aPersistence);
	User::LeaveIfError(iKeyNotifs.Append(notify));
	}
	
EXPORT_C void CBtraceKeyPress::CancelNotifyKeyEvent(MBtraceKeyPressObserver& aObserver)
	{
	for (TInt i=iKeyNotifs.Count()-1; i>=0; --i)
		{
		if (iKeyNotifs[i].iObserver == &aObserver)
			{
			iKeyNotifs.Remove(i);
			}
		}
	}

EXPORT_C void CBtraceKeyPress::NotifyRawKeyEventL(MBtraceKeyPressObserver& aObserver)
	{
	NotifyRawKeyEventL(aObserver, ENotificationOneShot);
	}
	
EXPORT_C void CBtraceKeyPress::NotifyRawKeyEventL(MBtraceKeyPressObserver& aObserver, TBtraceNotificationPersistence aPersistence)
	{
	TKeyPressNotif notify(aObserver, aPersistence);
	User::LeaveIfError(iRawKeyNotifs.Append(notify));
	}

EXPORT_C void CBtraceKeyPress::CancelNotifyRawKeyEvent(MBtraceKeyPressObserver& aObserver)
	{
	for (TInt i=iRawKeyNotifs.Count()-1; i>=0; --i)
		{
		if (iRawKeyNotifs[i].iObserver == &aObserver)
			{
			iRawKeyNotifs.Remove(i);
			}
		}
	}

//
// CBtraceKeyPress::NotifyPointerEvent
// called by the client to register an 
// interest in any pointer events captured 
//
EXPORT_C void CBtraceKeyPress::NotifyPointerEventL(MBtraceKeyPressObserver& aObserver)
	{
	NotifyPointerEventL(aObserver, ENotificationOneShot);
	}
	
EXPORT_C void CBtraceKeyPress::NotifyPointerEventL(MBtraceKeyPressObserver& aObserver, TBtraceNotificationPersistence aPersistence)
	{
	TKeyPressNotif notify(aObserver, aPersistence);
	User::LeaveIfError(iPointerNotifs.Append(notify));
	}

EXPORT_C void CBtraceKeyPress::CancelNotifyPointerEvent(MBtraceKeyPressObserver& aObserver)
	{
	for (TInt i=iPointerNotifs.Count()-1; i>=0; --i)
		{
		if (iPointerNotifs[i].iObserver == &aObserver)
			{
			iPointerNotifs.Remove(i);
			}
		}
	}

//
// CBtraceKeyPress::NotifyUnclassifiedDataL
// called by the client to register an 
// interest in any 'generic' events captured 
//
EXPORT_C void CBtraceKeyPress::NotifyUnclassifiedDataL(MBtraceKeyPressObserver& aObserver)
	{
	NotifyUnclassifiedDataL(aObserver, ENotificationOneShot);
	}

EXPORT_C void CBtraceKeyPress::NotifyUnclassifiedDataL(MBtraceKeyPressObserver& aObserver, TBtraceNotificationPersistence aPersistence)
	{
	TKeyPressNotif notify(aObserver, aPersistence);
	User::LeaveIfError(iGenericNotifs.Append(notify));	
	}

EXPORT_C void CBtraceKeyPress::CancelNotifyUnclassifiedData(MBtraceKeyPressObserver& aObserver)
	{
	for (TInt i=iGenericNotifs.Count()-1; i>=0; --i)
		{
		if (iGenericNotifs[i].iObserver == &aObserver)
			{
			iGenericNotifs.Remove(i);
			}
		}
	}

//
// TKeyPressNotif
// place holder for observers interested in key &/or pointer events
//
CBtraceKeyPress::TKeyPressNotif::TKeyPressNotif(MBtraceKeyPressObserver& aObserver, TBtraceNotificationPersistence aPersistence):
iObserver(&aObserver), iPersistence(aPersistence)
	{
	}

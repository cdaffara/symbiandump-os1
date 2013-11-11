// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Test Multi Pointer Click Plug-In DLL
// 
//

#include <e32std.h>
#include "W32CLICK.H"
#include "multiptrclick.h"

class CMultiPtrClickMaker : public CClickMaker
	{
	enum {EEventBufferSize=32};
public:
	~CMultiPtrClickMaker();
	void ConstructL();
	//Virtual function from CClickMaker
	void KeyEvent(TEventCode aType, const TKeyEvent& aEvent);
	void PointerEvent(const TPointerEvent& aEvent);
	void OtherEvent(TInt aType, TAny* aParam);
	TInt CommandReplyL(TInt aOpcode, TAny *aArgs);

private:
	TInt AddEvent(const TWsEvent* aEvent);
	TInt AddEvent(const TPointerEventData* aEvent);
	TBool ComparePointerEvents(const TPointerEvent* aExpectedEvent, const TPointerEvent* aActualEvent);
	TBool CompareOtherEvents(const TPointerEventData* aExpectedEvent, const TPointerEventData* aActualEvent);
private:
	CCirBuf<TWsEvent> iPtrEventBuffer;
	CCirBuf<TPointerEventData> iOtherEventBuffer;
	TBuf<255> iErrorDes;
	TInt iPtrEventCount;
	TInt iOtherEventCount;
	TInt iError;
	};


void CMultiPtrClickMaker::ConstructL()
	{
	iPtrEventBuffer.SetLengthL(EEventBufferSize);
	iOtherEventBuffer.SetLengthL(EEventBufferSize);
	}

CMultiPtrClickMaker::~CMultiPtrClickMaker()
	{
	}

void CMultiPtrClickMaker::KeyEvent(TEventCode /*aType*/,const TKeyEvent& /*aEvent*/)
	{
	}

TBool CMultiPtrClickMaker::ComparePointerEvents(const TPointerEvent* aExpectedEvent, const TPointerEvent* aActualEvent)
	{
	// Check pointer type
	if (aExpectedEvent->iType != aActualEvent->iType)
		{
		_LIT(KEventType, "Actual Wserv Event type = %d Expected Wserv Event Type = %d ");
		iErrorDes.Format(KEventType, aActualEvent->iType, aExpectedEvent->iType);
		return EFalse;
		}
	
	if (aExpectedEvent->iPosition != aActualEvent->iPosition)
		{
		_LIT(KPointerPosition, "Actual PointerPosition = (%d, %d) Expected PointerPosition = (%d, %d) ");
		iErrorDes.Format(KPointerPosition, aActualEvent->iPosition.iX, aActualEvent->iPosition.iY, aExpectedEvent->iPosition.iX, aExpectedEvent->iPosition.iY);
		return EFalse;
		}
	
	if (aExpectedEvent->iParentPosition != aActualEvent->iParentPosition)
		{
		_LIT(KPointerPosition, "Actual parent Position = (%d, %d) Expected parent Position = (%d, %d) ");
		iErrorDes.Format(KPointerPosition, aActualEvent->iParentPosition.iX, aActualEvent->iParentPosition.iY, aExpectedEvent->iParentPosition.iX, aExpectedEvent->iParentPosition.iY);
		return EFalse;
		}
	
	if (aExpectedEvent->IsAdvancedPointerEvent() != aActualEvent->IsAdvancedPointerEvent())
		{
		_LIT(KPointerPosition, "Actual event is TAdvancedPointerEvent = %d Expected event is TAdvancedPointerEvent = %d ");
		iErrorDes.Format(KPointerPosition, aActualEvent->IsAdvancedPointerEvent(), aExpectedEvent->IsAdvancedPointerEvent());
		return EFalse;
		}
	
	if (aExpectedEvent->IsAdvancedPointerEvent())
		{
		const TAdvancedPointerEvent& expectedAdvancedEvent = *aExpectedEvent->AdvancedPointerEvent();
		const TAdvancedPointerEvent& actualAdvancedEvent   = *aActualEvent->AdvancedPointerEvent();
		
		if (expectedAdvancedEvent.PointerNumber() != actualAdvancedEvent.PointerNumber())
			{
			_LIT(KPointerNumber, "Actual PointerNumber = %d Expected PointerNumber = %d ");
			iErrorDes.Format(KPointerNumber, actualAdvancedEvent.PointerNumber(), expectedAdvancedEvent.PointerNumber());
			return EFalse;
			}
		
		if (expectedAdvancedEvent.Pressure() != actualAdvancedEvent.Pressure())
			{
			_LIT(KPointerPressure, "Actual Pressure = %d Expected Pressure = %d ");
			iErrorDes.Format(KPointerPressure, actualAdvancedEvent.Pressure(), expectedAdvancedEvent.Pressure());
			return EFalse;
			}
		
		if (expectedAdvancedEvent.Proximity() != actualAdvancedEvent.Proximity())
			{
			_LIT(KPointerProximity, "Actual Proximity = %d Expected Proximity = %d ");
			iErrorDes.Format(KPointerProximity, actualAdvancedEvent.Proximity(), expectedAdvancedEvent.Proximity());
			return EFalse;
			}
		}

	return ETrue;
	}

//
void CMultiPtrClickMaker::PointerEvent(const TPointerEvent& aEvent)
	{
	// If it has already failed then do not test other events
	// becasue the error value and its descriptor will be over written
	if (iError)
		{
		return;
		}
		
	// Get pointer event from buffer
	TWsEvent expectedEvent;
	iPtrEventBuffer.Remove(&expectedEvent);
	
	// Increment event count
	iPtrEventCount++;
	
	// increment this counter in OtherEvent() becasue first pointer event iscalled and then OtherEvent() is called
	if (!ComparePointerEvents(expectedEvent.Pointer(), &aEvent))
		{
		iError = iPtrEventCount;
		}
	}

TBool CMultiPtrClickMaker::CompareOtherEvents(const TPointerEventData* aExpectedEvent, const TPointerEventData* aActualEvent)
	{
	if (aExpectedEvent->iCurrentPos != aActualEvent->iCurrentPos)
		{
		_LIT(KCurrentPosition, "Actual CurrentPosition  w.r.t screen = (%d,%d) Expected CurrentPosition  w.r.t screen = (%d,%d)");
		iErrorDes.Copy(KCurrentPosition);
		iErrorDes.Format(KCurrentPosition, aActualEvent->iCurrentPos.iX, aActualEvent->iCurrentPos.iX, aExpectedEvent->iCurrentPos.iX, aExpectedEvent->iCurrentPos.iY);
		return EFalse;
		}
	
	if (aExpectedEvent->iClientHandle != aActualEvent->iClientHandle)
		{
		_LIT(KWindowHandle, "Actual Window Handle = %d Expected Window Handle = %d ");
		iErrorDes.Copy(KWindowHandle);
		iErrorDes.Format(KWindowHandle, aActualEvent->iClientHandle, aExpectedEvent->iClientHandle);
		return EFalse;
		}
	
	return ComparePointerEvents(&aExpectedEvent->iPointerEvent, &aActualEvent->iPointerEvent);
	}

void CMultiPtrClickMaker::OtherEvent(TInt aType, TAny* aParam)
	{
	if (aType != EEventPointer)
		{
		return;
		}
			
	if (iError)
		{
		return;
		}

	TPointerEventData expectedEvent;
	iOtherEventBuffer.Remove(&expectedEvent);
	
	iOtherEventCount++;
	
	TPointerEventData* data=static_cast<TPointerEventData*>(aParam);
	
	if (!CompareOtherEvents(&expectedEvent, data))
		{
		iError = iPtrEventCount;
		}
	}

TInt CMultiPtrClickMaker::AddEvent(const TWsEvent* aEvent)
	{
	return iPtrEventBuffer.Add(aEvent);
	}

TInt CMultiPtrClickMaker::AddEvent(const TPointerEventData* aEvent)
	{
	return iOtherEventBuffer.Add(aEvent);
	}

TInt CMultiPtrClickMaker::CommandReplyL(TInt aOpcode, TAny *aArgs)
	{
	TMultiPtrClickArgUnion pData;
	pData.any=aArgs;
	switch (aOpcode)
		{
	case EMultiPtrClickEventAdd:
		return AddEvent(pData.WsEvent);
	case EMultiPtrOtherEventAdd:
		return AddEvent(pData.OtherEvent);
	case EMultiPtrClickEventError:
		return iError;
	case EMultiPtrClickEventErrorDesc:
		if (iError != KErrNone)
			{
			RChunk chunk;
			chunk.OpenGlobal(KMultiPtrClickChunk, ETrue);
			TUint8* desPtr = chunk.Base() + *(static_cast<TInt*>(aArgs));
			TPtr8 ptrDes(desPtr, iErrorDes.Length(), iErrorDes.Length());
			ptrDes.Copy(iErrorDes);
			chunk.Close();
			return iErrorDes.Length();
			// returns the error description which gets displayed in logs if test failed
			}
		break;
	case EMultiPtrClickEventReset:
		iPtrEventBuffer.Reset();
		iError = 0;
		iPtrEventCount = 0;
		break;
	default:;
		}
	return(KErrNone);
	}

EXPORT_C CClickMaker* CreateMultiPtrClickMakerL()
	{
	CMultiPtrClickMaker* plugIn=new(ELeave) CMultiPtrClickMaker;
	CleanupStack::PushL(plugIn);
	plugIn->ConstructL();
	CleanupStack::Pop(plugIn);
	return plugIn;
	}

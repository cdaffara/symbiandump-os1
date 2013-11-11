// Copyright (c) 1996-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Emulte key presses on pointer events
// 
//

#include <e32std.h>
#include <e32hal.h>
#include "server.h"
#include "EVENT.H"

TInt WsKeyboardEmulator::iCurrentKeyDown;
TBool WsKeyboardEmulator::iKeyIsDown=EFalse;

void WsKeyboardEmulator::RawKeyEvent(TRawEvent::TType aType)
	{
	if (aType==TRawEvent::EKeyUp)
		iKeyIsDown=EFalse;
	else
		iKeyIsDown=ETrue;
	TRawEvent rawEvent;
	rawEvent.Set(aType,iCurrentKeyDown);
	TWindowServerEvent::ProcessRawEvent(rawEvent);
	}

TBool WsKeyboardEmulator::PointerEvent(TPointerEvent::TType aPointerEventType,const TPoint &aPoint, TDblQue<TPointerKeyList> *aKeyList)
	{
	if (iKeyIsDown && (aPointerEventType==TPointerEvent::EButton1Up))
		RawKeyEvent(TRawEvent::EKeyUp);
	else if (!aKeyList)
		return(EFalse);
	else
		{
		TPointerKeyList *pkl=NULL;
		for(TDblQueIter<TPointerKeyList> iter(*aKeyList);(pkl=iter++)!=NULL;)
			if (pkl->iRect.Contains(aPoint))
				break;
		if (iKeyIsDown && 
			(aPointerEventType==TPointerEvent::EDrag || 
			 aPointerEventType==TPointerEvent::EEnterHighPressure ||
			 aPointerEventType==TPointerEvent::EExitHighPressure) && 
			(!pkl || iCurrentKeyDown!=pkl->iScanCode))
			{
			RawKeyEvent(TRawEvent::EKeyUp);
			}
		else if (pkl)
			{
			if (aPointerEventType==TPointerEvent::EButton1Down ||
			    (aPointerEventType==TPointerEvent::ESwitchOn && pkl->iActivatedByPointerSwitchOn))
				{
				if (iKeyIsDown)
					RawKeyEvent(TRawEvent::EKeyUp);
				iCurrentKeyDown=pkl->iScanCode;
				RawKeyEvent(TRawEvent::EKeyDown);
				if (aPointerEventType==TPointerEvent::ESwitchOn)
					RawKeyEvent(TRawEvent::EKeyUp);
				}
			}
		else
			return(EFalse);
		}
	return(ETrue);
	}


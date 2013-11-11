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
// Client interface to the pointer click plugin.
// 
//

#ifndef __MULTIPTRCLICK_H__
#define __MULTIPTRCLICK_H__

#ifndef __W32CLICK_H__
#include <w32click.h>
#endif

#include <graphics/pointereventdata.h>

#define MULTIPTR_CLICK_THIRD_UID 268455781

_LIT(KMultiPtrClickChunk, "PointerClickChunk");

enum TMultiPtrClickCommands
	{
	EMultiPtrClickEventAdd,
	EMultiPtrClickEventError,
	EMultiPtrClickEventErrorDesc,
	EMultiPtrClickEventReset,
	EMultiPtrOtherEventAdd,
	};

union TMultiPtrClickArgUnion
	{
	const TAny* any;
	const TWsEvent* WsEvent;
	const TPointerEventData* OtherEvent;
	};

#endif	//__MULTIPTRCLICK_H__

// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Client interface to the click plugin.
// 
//

#ifndef __CLICK_H__
#define __CLICK_H__

#define CLICK_THIRD_UID 268455780

enum TClickOutputModes
	{
	EClickNone,
	EClickCheck,
	EClickToWindow,		//For WINS only
	};

enum TClickCommands
	{
	// Control Between Plug-in Types
	EClickCommandToggleOutput=1,
	EClickCommandSetOutput,
	// Key and Pointer Testing
	EClickEventAdd=100,
	EClickFailed,
	EClickEvents,
	EClickReset,
	// Other Event Testing
	EClickCreateGroupWin=200,
	EClickCheckGpWinId,
	EClickCloseGroupWin,
	EClickCloseWin,
	EClickPointerEvent,
	};

class TPointerEventInfo
	{
public:
	TInt iClientHandle;
	TInt iWinGpId;
	TPoint iWinOrigin;		//Relative to parent
	TPoint iParentOrigin;	//Relative to screen
	};

#endif	//__CLICK_H__

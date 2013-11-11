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
//

#ifndef __TLIB_H__
#define __TLIB_H__

#include "testbase.h"

GLREF_C void TbPanic(TInt aPanic);

enum TTestBasePanic
	{
	ETestBasePanicResourceCount,
	ETestBasePanicResCountWinFail,
	ETestBasePanicResCountWinInit,
	ETestBasePanicEventStat,
	ETestBasePanicScheduler,
	ETestBasePanicKeyParams,
	ETestBasePanicInvalidEvent,
	ETestBasePanicReadCancelled,
	ETestBasePanicUnimplementedBaseFunction,
	ETestBasePanicCancelFunction,
	ETestBasePanicNullRedraw,
	ETestBasePanicNullOwnerWin,
	ETestBasePanicWinType,
	ETestBasePanicPassword,
	EDialogButtonCount,
	EDialogButtonIndex,
	EDialogDisplay,
	ETestBasePanicInvalidHeapAddress,
	};

#endif

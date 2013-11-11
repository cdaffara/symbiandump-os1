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
// hal\wins\hal.cpp
// 
//

#include <kernel/hal_int.h>
#include <u32std.h>

// ECPUSpeed
TInt ProcessCPUSpeed(TInt /*aDeviceNumber*/, TInt /*aAttrib*/, TBool aSet, TAny* aInOut)
	{
	return UserSvr::HalFunction(EHalGroupEmulator, EEmulatorHalCPUSpeed, (TAny*)aSet, aInOut);
	}

//
TInt GetDisplayNumberOfScreens(TInt /*aDeviceNumber*/, TInt /*aAttrib*/, TBool /*aSet*/, TAny* aInOut)
	{
	return UserSvr::HalFunction(EHalGroupEmulator, EEmulatorHalNumberOfScreens, NULL, aInOut);
	}

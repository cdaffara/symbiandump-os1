// Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __SURFACEUPDATE_H__
#define __SURFACEUPDATE_H__


#include <graphics/surface.h>


const TUint KSurfaceUpdateServMajorVersionNumber = 1;
const TUint KSurfaceUpdateServMinorVersionNumber = 1;
const TUint KSurfaceUpdateServBuildVersionNumber = 3;


enum TSurfaceUpdateServRqst
	{
	EUpdateServCreate = 1,
	EUpdateServClose,
	EUpdateServNotifyWhenAvailable,
	EUpdateServNotifyWhenDisplayed,
	EUpdateServNotifyWhenDisplayedXTimes,
	EUpdateServSubmitUpdate,
	EUpdateServCancelAllNotifications,
	EUpdateServLast = 1000,
	};

_LIT(KSurfaceUpdateServerName, "SURFACE_UPDATE_SERVER");
_LIT(KTestSurfaceUpdateServerName, "T_SURFACE_UPDATE_TEST_SERVER");


#endif

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
//

#include "gceavailable.h"

#include <e32svr.h>
#include <u32hal.h>

/**
Probes for GCE availability on emulator.

@return ETrue is GCE is available, EFalse otherwise.
*/

EXPORT_C TBool GCEAvailable()
	{
	TBool gce = EFalse;
	UserSvr::HalFunction(EHalGroupEmulator, EEmulatorHalBoolProperty,  (TAny*)"symbian_graphics_use_gce",  &gce);
	return gce;
	}


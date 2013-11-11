//audiodevicesource.cpp

// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#include "audiodevicesource.h"
#include <a3f/audioprocessingunittypeuids.h>

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CAudioDeviceSource::CAudioDeviceSource()
	{
	TRACE_CREATE();
	DP_CONTEXT(CAudioDeviceSource::CAudioDeviceSource, CtxDevSound, DPLOCAL);
	DP_IN();
	DP_OUT();
	}

// ---------------------------------------------------------------------------
// Factory method
// ---------------------------------------------------------------------------
//
EXPORT_C CAudioDeviceSource* CAudioDeviceSource::NewL()
	{
	DP_STATIC_CONTEXT(CAudioDeviceSource::NewL, CtxDevSound, DPLOCAL);
	DP_IN();
	CAudioDeviceSource* self = new(ELeave)CAudioDeviceSource();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	DP0_RET(self, "%x");
	}

// ---------------------------------------------------------------------------
// Second phase constructor
// ---------------------------------------------------------------------------
//
void CAudioDeviceSource::ConstructL()
	{
	DP_CONTEXT(CAudioDeviceSource::ConstructL, CtxDevSound, DPLOCAL);
	DP_IN();
	DP_OUT();
	}

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CAudioDeviceSource::~CAudioDeviceSource()
	{
	DP_CONTEXT(CAudioDeviceSource::~CAudioDeviceSource, CtxDevSound, DPLOCAL);
	DP_IN();
	DP_OUT();
	}

// end of file

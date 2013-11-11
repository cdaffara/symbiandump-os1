//audiodevicesink.cpp

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



#include "audiodevicesink.h"

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CAudioDeviceSink::CAudioDeviceSink()
	{
	TRACE_CREATE();
	DP_CONTEXT(CAudioDeviceSink::CAudioDeviceSink *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	DP_OUT();
	}

// ---------------------------------------------------------------------------
// Factory method
// ---------------------------------------------------------------------------
//
EXPORT_C CAudioDeviceSink* CAudioDeviceSink::NewL()
	{
	DP_STATIC_CONTEXT(CAudioDeviceSink::NewL *CD0*, CtxDevSound, DPLOCAL);
	DP_IN();
	CAudioDeviceSink* self = new(ELeave)CAudioDeviceSink();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	DP0_RET(self, "0x%x");
	}

// ---------------------------------------------------------------------------
// Second phase constructor
// ---------------------------------------------------------------------------
//
void CAudioDeviceSink::ConstructL()
	{
	DP_CONTEXT(CAudioDeviceSink::ConstructL *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	DP_OUT();
	}

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CAudioDeviceSink::~CAudioDeviceSink()
	{
	}

// end of file

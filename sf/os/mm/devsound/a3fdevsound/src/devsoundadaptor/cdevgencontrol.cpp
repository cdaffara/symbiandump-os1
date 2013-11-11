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



#include "cdevgencontrol.h"

#include <a3f/maudiocontext.h>
#include <a3f/audioprocessingunittypeuids.h>
#include <a3f/mbuffersource.h>
#include <a3f/maudiocodec.h>
#include <a3f/maudiogaincontrol.h>


// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Default constructor
// ---------------------------------------------------------------------------
//
CDevGenControl::CDevGenControl()
	{
	TRACE_CREATE();
	DP_CONTEXT(CDevGenControl::CDevGenControl *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();

	DP_OUT();
	}

// -----------------------------------------------------------------------------
// Symbian 2nd phase constructor
// -----------------------------------------------------------------------------
//
void CDevGenControl::ConstructL(CDevAudio* aDevAudio, MDevSoundAdaptationObserver& aDevSoundObserver)
	{
	DP_CONTEXT(CDevGenControl::ConstructL *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	CDevAudioControl::ConstructL(aDevAudio, aDevSoundObserver);
	DP_OUT();
	}

// -----------------------------------------------------------------------------
// Symbian constructor
// -----------------------------------------------------------------------------
//
CDevGenControl* CDevGenControl::NewL(CDevAudio* aDevAudio, MDevSoundAdaptationObserver& aDevSoundObserver)
	{
	DP_STATIC_CONTEXT(CDevGenControl::NewL *CD0*, CtxDevSound, DPLOCAL);
	DP_IN();
	CDevGenControl* self = new (ELeave) CDevGenControl();
	CleanupStack::PushL(self);
	self->ConstructL(aDevAudio, aDevSoundObserver);
	CleanupStack::Pop(self);
	DP0_RET(self, "0x%x");
	}

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CDevGenControl::~CDevGenControl()
	{
	DP_CONTEXT(CDevGenControl::~CDevGenControl *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	DP_OUT();
	}

// End of file

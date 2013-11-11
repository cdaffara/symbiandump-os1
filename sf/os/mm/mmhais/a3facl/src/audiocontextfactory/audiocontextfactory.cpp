//audiocontextfactory.cpp

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



#include <a3f/audiocontextfactory.h>

#include "audiocontext.h"


CAudioContextFactory::CAudioContextFactory()
	{
	}

EXPORT_C CAudioContextFactory* CAudioContextFactory::NewL()
	{
	DP_STATIC_CONTEXT(CAudioContextFactory::NewL *CD0*, CtxDevSound, DPLOCAL);
	DP_IN();
	CAudioContextFactory* self = new(ELeave) CAudioContextFactory();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	DP0_RET(self, "0x%x");
	}

void CAudioContextFactory::ConstructL()
	{
	DP_CONTEXT(CAudioContextFactory::ConstructL *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	DP_OUT();
	}

EXPORT_C CAudioContextFactory::~CAudioContextFactory()
	{
	DP_CONTEXT(CAudioContextFactory::~CAudioContextFactory *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	DP_OUT();
	}

EXPORT_C TInt CAudioContextFactory::CreateAudioContext(MAudioContext*& aContext)
	{
	DP_CONTEXT(CAudioContextFactory::CreateAudioContext *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	DP_OUT();
	TRAPD(err,aContext = CAudioContext::NewL());
	return err;
	}
	
EXPORT_C void CAudioContextFactory::DeleteAudioContext(MAudioContext*& aContext)
	{
	DP_CONTEXT(CAudioContextFactory::DeleteAudioContext *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	CAudioContext* contextPtr = static_cast<CAudioContext*>(aContext);
	delete contextPtr;
	DP_OUT();
	//DP0_RET(KErrNone, "%d");
	}

// End of file

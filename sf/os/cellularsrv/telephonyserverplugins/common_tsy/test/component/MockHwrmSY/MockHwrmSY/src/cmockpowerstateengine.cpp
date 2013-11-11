// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// MockSY engine for Hwrm PowerState plugin
// 
//

#include "cmockpowerstateengine.h"
#include "cmockpowerstatesy.h"


/**
Factory function
*/
CMockPowerStateEngine* CMockPowerStateEngine::NewL(CMockPowerStateSY& aMockPowerStateSY)
	{
	CMockPowerStateEngine* self =  new (ELeave)CMockPowerStateEngine(aMockPowerStateSY);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/**
Constructor
*/	
CMockPowerStateEngine::CMockPowerStateEngine(CMockPowerStateSY& aMockPowerStateSY)
	:iMockPowerStateSY(aMockPowerStateSY)
	{
	}


/**
Generate a completion event
*/
void CMockPowerStateEngine::DoCompleteEventL(const TMockSYEvent& aEvent)
	{
	TPtrC8 data(aEvent.iData);
	TPckgC<TInt>* valuePckg = static_cast<TPckgC<TInt>* >(&data);
 	iMockPowerStateSY.DoProcessCallback(aEvent.iCmdId, (*valuePckg)());
	}
	

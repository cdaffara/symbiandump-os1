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
//

#include "hwrmtestuiplugin_b.h"

CHwrmTestUiPlugin_B* CHwrmTestUiPlugin_B::NewL()
	{
	CHwrmTestUiPlugin_B* self = new(ELeave) CHwrmTestUiPlugin_B();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CHwrmTestUiPlugin_B::CHwrmTestUiPlugin_B()
	{
	}

CHwrmTestUiPlugin_B::~CHwrmTestUiPlugin_B()
	{
	}

void CHwrmTestUiPlugin_B::ConstructL()
	{
	}

TBool CHwrmTestUiPlugin_B::InForeground() const
	{
	return EFalse;
	}
	
void CHwrmTestUiPlugin_B::AddForegroundObserverL(MHwrmForegroundObserver* /*aObserver*/)
	{
	}

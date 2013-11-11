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

#include "hwrmtestuiplugin_f.h"

CHwrmTestUiPlugin_F* CHwrmTestUiPlugin_F::NewL()
	{
	CHwrmTestUiPlugin_F* self = new(ELeave) CHwrmTestUiPlugin_F();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CHwrmTestUiPlugin_F::CHwrmTestUiPlugin_F()
	{
	}

CHwrmTestUiPlugin_F::~CHwrmTestUiPlugin_F()
	{
	}

void CHwrmTestUiPlugin_F::ConstructL()
	{
	}

TBool CHwrmTestUiPlugin_F::InForeground() const
	{
	return ETrue;
	}
	
void CHwrmTestUiPlugin_F::AddForegroundObserverL(MHwrmForegroundObserver* /*aObserver*/)
	{
	}

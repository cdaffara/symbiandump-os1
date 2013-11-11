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
// This file defines the base class and observer to allow a SUPL protocol
// to send/receive messages to/from positioning protocol state machines.
// 
//

/**
 @file
 @internalTechnology
 
*/

#include "suplpositioningprotocolfsm.h"

// Constructors
CSuplPositioningProtocolFsm::CSuplPositioningProtocolFsm(MSuplPositioningProtocolFsmObserver& aObserver) :
	CActive(EPriorityStandard), iObserver(aObserver)
	{
	}

CSuplPositioningProtocolFsm::~CSuplPositioningProtocolFsm()
	{
	}

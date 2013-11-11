// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef BTPOWERCONTROLMESSAGES_H
#define BTPOWERCONTROLMESSAGES_H

/**
@file
@internalComponent
*/

#include <e32base.h>

enum THCIPowerOptionID
	{
	EHCISetPower					= 0,
	EHCIGetPower					= 1,
	KHCICancelPower					= 2
	};

enum TPowerControlMessageSlots
	{
	EPSCommandSlot					= 0,
	EPSInfoSlot						= 1,
	EPSStateReturnSlot				= 2,
	EPSStateSendSlot				= 3
	};

#endif // BTPOWERCONTROLMESSAGES_H

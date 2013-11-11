// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <bluetooth/logger.h>

#include <e32base.h>
#include "l2util.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_L2CAP);
#endif

void Panic(TL2CAPPanic aPanic)
// Panic L2CAP due to stack bug.
	{
	LOG_STATIC_FUNC
	User::Panic(KL2CAPPanicName, aPanic);
	}

void Panic(TL2CAPPanic aPanic, TInt aState)
// Panic L2CAP due to stack bug.
	{
	LOG_STATIC_FUNC
	User::Panic(KL2CAPPanicName, (aPanic*KL2StatePanicMultiplier) + KL2StatePanicDelimiter + aState);
	}

